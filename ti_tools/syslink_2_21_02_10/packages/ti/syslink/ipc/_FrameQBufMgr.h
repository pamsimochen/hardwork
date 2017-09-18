/** 
 *  @file   _FrameQBufMgr.h
 *
 *  @brief      Defines for internal structures and functions for
 *              FrameQBufMgr module.
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



#ifndef __FRAMEQBUFMGR_H_
#define __FRAMEQBUFMGR_H_

#include <ti/ipc/GateMP.h>
#include <ti/syslink/FrameQBufMgr.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * macros & defines
 * =============================================================================
 */
/* FrameQ module id */
#define FrameQBufMgr_MODULEID       (0x684e)

/*
 * @brief FrameQBufMgr module name
 */
#define FRAMEQBUFMGR_NAMESERVERNAME  "FrameQBufMgr"


/*!
 *  @brief  Constant value to get the unique instance id from instance creator
 * processor id and  instance sequence no on that processor.
 */
#define FRAMEQBUFMGR_INSTNO_BITS             8u
#define FRAMEQBUFMGR_INSTNO_MASK             0xFF
#define FRAMEQBUFMGR_INSTNO_BITOFFSET        0u
#define FRAMEQBUFMGR_CREATORPROCID_BITS      8u
#define FRAMEQBUFMGR_CREATORPROCID_MASK      0xFF
#define FRAMEQBUFMGR_CREATORPROCID_BITOFFSET 8u

/*!
 *  @brief  Constant value to indicate that FrameQBufMgr instance is created.
 */
#define FrameQBufMgr_CREATED               (0x05251995)

/*!
 *  @brief  Version of FrameQBufMgr Module.
 */
#define FrameQBufMgr_VERSION                (1u)

/*!
 *  @brief  One of the FrameQBufMgr instance create types.
 *          It indicates FrameQBufMgr instance is created statically.
 */
#define FrameQBufMgr_STATIC_CREATE          (0x1u)

/*!
 *  @brief  One of the FrameQBufMgr instance create types.
 *          It indicates FrameQBufMgr instance is created statically.
 */
#define FrameQBufMgr_STATIC_CREATE_USEDREGION	(0x2u)

/*!
 *  @brief  One of the FrameQBufMgr instance create types.
 *          It indicates FrameQBufMgr instance is created dynamically.
 */
#define FrameQBufMgr_DYNAMIC_CREATE         (0x4u)

/*!
 *  @brief  One of the FrameQBufMgr instance create types.
 *          It indicates FrameQBufMgr instance is created dynamically.
 */
#define FrameQBufMgr_DYNAMIC_CREATE_USEDREGION	(0x8u)

/*!
 *  @brief  One of the FrameQBufMgr instance open types.
 *          It indicates FrameQBufMgr instance is opened statically.
 */
#define FrameQBufMgr_STATIC_OPEN            (0x10u)

/*!
 *  @brief  One of the FrameQBufMgr instance open types.
 *          It indicates FrameQBufMgr instance is opened statically.
 */
#define FrameQBufMgr_STATIC_OPEN            (0x10u)

/*!
 *  @brief  One of the FrameQBufMgr instance open types.
 *          It indicates FrameQBufMgr instance is opened dynamically.
 */
#define FrameQBufMgr_DYNAMIC_OPEN           (0x20u)

/*!
 *  @brief
 */
#define FrameQBufMgr_INVALID           (00xFFu)

/*!
 * @brief Denotes header buffer is not pointing to any  static frame or
 *        dynamicframe.
 */
#define  FrameQBufMgr_NONE_FRAME                         (0u)

/*!
 * @brief Denotes Frame belongs to static frames that are populated at
 *        create time of the instance.
 */
#define  FrameQBufMgr_STATIC_FRAME                       (1u)

/*!
 * @brief Denotes Frame belongs to dynamic frame grougp that are added
 *        dynamically at runtime.
 */
#define  FrameQBufMgr_DYNAMIC_FRAME                      (2u)

/*TBD: Need to remove this*/
#define FrameQBufMgr_CACHE_LINESIZE                      (128)

#define FrameQBufMgr_FRAMEBUF_CACHEFLAGS_BITOFFSET       (8)

#define FrameQBufMgr_FRAMEBUF_CPUACCESSFLAGS_BITOFFSET    (16)


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
 /*!
 *  @brief  Structure defining open params.
 */
typedef struct FrameQBufMgr_OpenParams_tag {
    FrameQBufMgr_CommonOpenParams     commonOpenParams;
}FrameQBufMgr_OpenParams;
/*!
 *  @brief  Shared  control strucrue for the instance attributes.
 */
typedef struct FrameQBufMgr_Attrs_Tag {
    volatile UInt32      status;
    /*!< Flag denotes whether instance is created or not.*/
    volatile UInt32      version;
    /*!< Version of the  instance.*/
    volatile UInt32      localProtect;
    /*!< Version of the  instance.*/
    volatile UInt32      remoteProtect;
    /*!< Version of the  instance.*/
    volatile Ptr         gateMPAddr;
    /* GateMP address (shm safe)     */
    volatile  char       name[FrameQBufMgr_MAXNAMELEN];
    /*!< Name of the instance */
    volatile Ptr      sharedDataBufAddr;
    /*!<  Address of the top of the big memory  that is split into datablocks
     *   (data buffers)
     */
    volatile  UInt32   sharedAddrSize;
    /*!< Size of the memory pointed to by sharedDataBufAddr */
    volatile UInt32   numFreeFramePools;
    /*!< Number of free frame pools maintained by this instance */
    volatile UInt32      numNotifyEntries;
    /*!
     * How many clients that do reister  callback function with the instance.
     */
    volatile UInt16      entryId;
    /*!< Unique id of the instance in the system. Combination of
	 *   creator procId and instance no on that processor.
     */
} FrameQBufMgr_Attrs;

/*!
 *  ======== FreeFramePoolObj ========
 *  @brief  Free Frame pool structure to keep track of the free frames in a
 *          frame pool.
 */
typedef struct FrameQBufMgr_FreeFramePoolObj_Tag {
    volatile UInt32 numFrms;
    volatile UInt32 freeFrms;
    volatile UInt32 numFrmHdrBufs;
    volatile UInt32 freeFrmHdrBufs;
    volatile UInt32 frmHdrBufSize;
    volatile UInt32 numBufsInFrame;
    volatile UInt32 numDynamicHdrBufs;
    volatile UInt32 freeDynamicHdrBufs;
    volatile UInt32 numDynamicFrames;
    volatile UInt32 freeDynamicFrames;
    volatile UInt32 frmStartAddrp;
    volatile UInt32 frmEndAddrp;
    volatile UInt32 hdrBufStartAddrp;
    volatile UInt32 hdrBufEndAddrp;
    volatile UInt32 frmBufsStartAddrp;
    volatile UInt32 frmBufsEndAddrp;
    volatile UInt32 frmBufSize[FrameQBufMgr_MAX_FRAMEBUFS];
    volatile UInt32 frmRefCnt[FrameQBufMgr_POOL_MAXFRAMES];
    volatile UInt32 dynFrame[FrameQBufMgr_POOL_DYNAMIC_MAXFRAMES];
    volatile UInt32 dynFrmRefCnt[FrameQBufMgr_POOL_DYNAMIC_MAXFRAMES];
    volatile UInt32 hdrBuf_refCntIndex[FrameQBufMgr_POOL_MAXHDRS];
    volatile UInt32 dynHdrBuf_refCntIndex[FrameQBufMgr_POOL_DYNAMIC_MAXHDRS];
    volatile UInt32 dynHdrBufs[FrameQBufMgr_POOL_DYNAMIC_MAXHDRS];
    volatile Bool   hdrBufIsUsed[FrameQBufMgr_POOL_MAXHDRS];
    volatile Bool   dynHdrBufIsUsed[FrameQBufMgr_POOL_DYNAMIC_MAXHDRS];
    volatile UInt8  hdrBuf_refCntIndexType[FrameQBufMgr_POOL_MAXHDRS];
    volatile UInt8  dynHdrBuf_refCntIndexType[FrameQBufMgr_POOL_DYNAMIC_MAXHDRS];
} FrameQBufMgr_FreeFramePoolObj;


/*!
 *  @brief  Structure defining Additional specific parameters that name server maintains
	 * along with the interface  type for ShMem implementation.
 */
typedef  struct FrameQBufMgr_ShMem_InstNameServerEntryParams_tag  {
        Ptr        sharedAddr;
        UInt32     sharedAddrSize;
        Ptr        instGateMPAddr;
        UInt8      frmBuf_regionId;
        UInt8      hdrBuf_regionId;
		Ptr        cliGateMPAddr;
}FrameQBufMgr_ShMem_InstNameServerEntryParams;

/*!
 *  @brief  Format of the  FrameQ instance entry in Nameserver.
 */
typedef struct FrameQBufMgr_NameServerEntry_tag {
   UInt32     ctrlInterfaceType;
   UInt32     headerInterfaceType;
   UInt32     bufInterfaceType;

   /*!< FrameQBufMgr type */
	union {
        FrameQBufMgr_ShMem_InstNameServerEntryParams  shMemParams;
	} instParams;

}FrameQBufMgr_NameServerEntry;


Int32
FrameQBufMgr_getNumFreeFrames (FrameQBufMgr_Handle   handle,
                               UInt32              * numFreeFrames);
Int32
FrameQBufMgr_getvNumFreeFrames (FrameQBufMgr_Handle   handle,
                                UInt32                numFreeFrames[],
                                UInt8                 freeFramePoolNo[],
                                UInt8                 numFreeFramePools);

/* =============================================================================
 *  Function pointer declarations
 * =============================================================================
 */
/* Type for function pointer to FrameQBufMgr_create function.
 */
typedef
Ptr (*fqbm_create) (Ptr      createParams);

/*
 * Type for function pointer to FrameQBufMgr_delete function.
 */
typedef
Ptr (*fqbm_delete) (Ptr * pHandle);

/*
 * Type for function pointer to FrameQBufMgr_open function.
 */
typedef
Int32 (*fqbm_open) (Ptr    * pHandle,
                    Ptr      openParams);

/*
 * Type for function pointer to FrameQBufMgr_openByAddress function.
 */
typedef Int32 (*fqbm_openByAddress) (Ptr * pHandle,
                                     Ptr   sharedAddr,
                                     Ptr   openParams);

/*
 * Type for function pointer to FrameQBufMgr_close function.
 */
typedef Int32 (*fqbm_close) (Ptr *pHandle);

/*
 * Type for function pointer to FrameQBufMgr_alloc function.
 */
typedef
Int32 (*fqbm_alloc) (Ptr                   handle,
                     FrameQBufMgr_Frame  * frame);

/*
 * Type for function pointer to FrameQBufMgr_allocv function.
 */
typedef
Int32 (*fqbm_allocv) (Ptr                  handle,
                      FrameQBufMgr_Frame   framePtr[],
                      UInt32               freeQId[],
                      UInt8               *numFrames);

/*
 * Type for function pointer to FrameQBufMgr_free function.
 */
typedef
Int32 (*fqbm_free) (Ptr                   handle,
                    FrameQBufMgr_Frame    frame);

/*
 * Type for function pointer to FrameQBufMgr_freev function
 */
typedef
Int32 (*fqbm_freev) (Ptr                  handle,
                     FrameQBufMgr_Frame   framePtr[],
                     UInt32               numFrames);

/*
 * Type for function pointer to FrameQBufMgr_add function.
 */
typedef
Ptr (*fqbm_add) (Ptr                    handle,
                 UInt8                  freeQId);

/*
 * Type for function pointer to FrameQBufMgr_remove function.
 */
typedef
Int32 (*fqbm_remove) (Ptr                 handle,
                      UInt8               freeQId,
                      FrameQBufMgr_Frame  frame);

/*
 * Type for function pointer to FrameQBufMgr_dup function.
 */
typedef
Int32 (*fqbm_dup) (Ptr                    handle,
                   FrameQBufMgr_Frame     framePtr,
                   FrameQBufMgr_Frame     dupedFramePtr[],
                   UInt32                 numDupedFrames);
/*
 * Type for function pointer to FrameQBufMgr_dupv function.
 */
typedef
Int32 (*fqbm_dupv) (Ptr                   handle,
                    FrameQBufMgr_Frame    framePtr[],
                    FrameQBufMgr_Frame  **dupedFramePtr,
                    UInt32                numDupedFrames,
                    UInt32                numFrames);

/*
 * Type for function pointer to FrameQBufMgr_registerNotifier function.
 */
typedef
Int32 (*fqbm_registerNotifier) (Ptr  handle,
                                FrameQBufMgr_NotifyParams *notifyParams);

/*
 * Type for function pointer to FrameQBufMgr_unregisterNotifier function.
 */
typedef
Int32 (*fqbm_unregisterNotifier) (Ptr handle);


/*
 * Type for function pointer to FrameQBufMgr_writeBack function.
 */
typedef
Int32 (*fqbm_writeBack)(Ptr handle, FrameQBufMgr_Frame frame);

/* Invalidate the contents  of a buffer*/
typedef Int32 (*fqbm_invalidate)(Ptr handle, FrameQBufMgr_Frame frame);

/*
 * Type for function pointer to FrameQBufMgr_writeBackHeaderBuf function.
 */
typedef
Int32
(*fqbm_writeBackHeaderBuf) (Ptr         handle,
                            Ptr         headerBuf);
/*
 * Type for function pointer to FrameQBufMgr_invalidateHeaderBuf function.
 */
typedef
Int32
(*fqbm_invalidateHeaderBuf)(Ptr        handle,
                            Ptr        headerBuf);
/*
 * Type for function pointer to FrameQBufMgr_writeBackFrameBuf function.
 */
typedef
Int32
(*fqbm_writeBackFrameBuf)   (Ptr         handle,
                             Ptr         frameBuf,
                             UInt32      size,
                             UInt8       bufIndexInFrame);
/*
 * Type for function pointer to FrameQBufMgr_invalidateFrameBuf function.
 */
typedef
Int32
(*fqbm_invalidateFrameBuf)  (Ptr          handle,
                             Ptr          frameBuf,
                             UInt32       size,
                             UInt8        bufIndexInFrame);
/*
 * Type for function pointer to FrameQBufMgr_translateAddr function.
 */
typedef
Int32 (*fqbm_translateAddr) (Ptr                    handle,
                             Ptr *                  dstAddr,
                             FrameQBufMgr_AddrType  dstAddrType,
                             Ptr                    srcAddr,
                             FrameQBufMgr_AddrType  srcAddrType,
                             FrameQBufMgr_BufType   bufType);

/* Function to get FrameQbufMgr Id. */
typedef UInt32 (*fqbm_getId) (Ptr handle);

/* Type for function pointer to FrameQBufMgr_control function.. */
typedef UInt32 (*fqbm_control) (Ptr handle, Int32 cmd, Ptr arg);

/* getCliNotifyMgrShAddr */
typedef
Ptr (*fqbm_getCliNotifyMgrShAddr)(Ptr handle);


/* getCliNotifyMgrGateShAddr */
typedef
Ptr (*fqbm_getCliNotifyMgrGateShAddr)(Ptr handle);

/* setNotifyId */
typedef
Int32 (*fqbm_setNotifyId)(Ptr handle,UInt32 notifyId);

/* resetNotifyId */
typedef
Int32 (*fqbm_resetNotifyId)(Ptr handle,UInt32 notifyId);


/* isCacheEnabledForHeaderBuf */
typedef
Bool (*fqbm_isCacheEnabledForHeaderBuf)(Ptr handle);

/* isCacheEnabledForHeaderBuf */
typedef
Bool (*fqbm_isCacheEnabledForFrameBuf)(Ptr handle, UInt8 bufIndex);

typedef
Int32
(*fqbm_getNumFreeFrames)(Ptr handle, UInt32 * numFreeFrames);

typedef
Int32
(*fqbm_getvNumFreeFrames)(Ptr       handle,
                          UInt32    numFreeFrames[],
                          UInt8     freeFramePoolNo[],
                          UInt8     numFreeFramePools);

/* Structure defining Interface functions for FrameQBufMgr */
typedef struct FrameQBufMgr_fxns_tag {
    fqbm_create             create;
    fqbm_delete             deleteInstance;
    fqbm_open               open;
    fqbm_openByAddress      openByAddress;
    fqbm_close              close;
    fqbm_alloc              alloc;
    fqbm_allocv             allocv;
    fqbm_free               free;
    fqbm_freev              freev;
    fqbm_add                add;
    fqbm_remove             remove;
    fqbm_dup                dup;
    fqbm_dupv               dupv;
    fqbm_registerNotifier   registerNotifier;
    fqbm_unregisterNotifier unregisterNotifier;
    fqbm_writeBack          writeBack;
    fqbm_invalidate         invalidate;
    fqbm_writeBackHeaderBuf  writeBackHeaderBuf;
    fqbm_invalidateHeaderBuf invalidateHeaderBuf;
    fqbm_writeBackFrameBuf   writeBackFrameBuf;
    fqbm_invalidateFrameBuf  invalidateFrameBuf;
    fqbm_translateAddr      translateAddr;
    fqbm_getId              getId;
    fqbm_control            control;
    fqbm_getCliNotifyMgrShAddr getCliNotifyMgrShAddr;
    fqbm_getCliNotifyMgrGateShAddr getCliNotifyMgrGateShAddr;
    fqbm_setNotifyId        setNotifyId;
    fqbm_resetNotifyId      resetNotifyId;
    fqbm_isCacheEnabledForHeaderBuf isCacheEnabledForHeaderBuf;
    fqbm_isCacheEnabledForFrameBuf  isCacheEnabledForFrameBuf;
    fqbm_getNumFreeFrames           getNumFreeFrames;
    fqbm_getvNumFreeFrames          getvNumFreeFrames;
}FrameQBufMgr_Fxns;


/* =============================================================================
 *  Internal APIs
 * =============================================================================
 */
/* Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for instance.
 */
Ptr _FrameQBufMgr_getCliNotifyMgrShAddr (FrameQBufMgr_Handle handle);

/* Function to get the sharedMemBaseAddress of the clientNotifyMgr
 *        instance used for this instance.
 */
Ptr _FrameQBufMgr_getCliNotifyMgrGateShAddr (FrameQBufMgr_Handle handle);

/* Function to set the notifyid received in userspace during call to
 *  ClientNotifyMgr_register client.
 */
Int32 _FrameQBufMgr_setNotifyId (FrameQBufMgr_Handle handle, UInt32 notifyId);

/* Function to set the notifyid received in userspace during call to
 * ClientNotifyMgr_unregister client.
 */
Int32 _FrameQBufMgr_resetNotifyId (FrameQBufMgr_Handle handle, UInt32 notifyId);

Int32 FrameQBufMgr_getInstNo(UInt8 *instNo);
Int32 FrameQBufMgr_freeInstNo(UInt8 instNo);

/*!
 * @brief API to to find out the specified interface type is supported or not.
 * @param type  interface type.
 */
inline
Int32 FrameQBufMgr_isSupportedInterface(UInt32 type);

/*!
 * @brief API to to find out the specified buf interface type is supported or not
 *        for the given control interface type.
 * @param ctrlInterfaceType  control interface type.
 * @param bufInterfaceType  frame buf interface type.
 *                                (see FrameQBufMgr_FrameBufferInterface enum).
 */
inline
Int32 FrameQBufMgr_isSupportedBufInterface(UInt32 ctrlInterfaceType,
                                           UInt32 bufInterfaceType);

GateMP_Handle FrameQBufMgr_getGate(FrameQBufMgr_Handle handle);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /*FRAMEQBUFMGR_H*/
