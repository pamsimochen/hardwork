/*
 *  @file   FrameQBufMgr_ShMem.c
 *
 *  @brief      Implements FrameQ  functions on user space.
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

/* Utilities & OSAL headers */
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/MemoryDefs.h>
#include <ti/syslink/utils/Memory.h>

#include <ti/ipc/GateMP.h>
#include <ti/ipc/MultiProc.h>
/* Module level headers */
#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr.h>

#include <ti/syslink/FrameQBufMgr_ShMem.h>
#include <ti/syslink/ipc/_FrameQBufMgr_ShMem.h>

#include <ti/syslink/inc/usr/Linux/FrameQBufMgrDrv.h>
#include <ti/syslink/inc/FrameQBufMgrDrvDefs.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>


/** ============================================================================
 *  @const  IPC_BUFFER_ALIGN
 *
 *  @desc   Macro to align a number.
 *          x: The number to be aligned
 *          y: The value that the number should be aligned to.
 *  ============================================================================
 */
#define IPC_BUFFER_ALIGN(x, y) (UInt32)((UInt32)((x + y - 1) / y) * y)

#define ROUND_UP(a, b) (SizeT)((((UInt32) a) + ((b) - 1)) & ~((b) - 1))

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

static Int32 FrameQBufMgr_ShMem_usedefaultgate = FALSE;

/* =============================================================================
 *  Globals
 * =============================================================================
 */

/* =============================================================================
 * APIS
 * =============================================================================
 */

/*!
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
FrameQBufMgr_ShMem_sharedMemReq (
                            const FrameQBufMgr_ShMem_Params * params,
                            UInt32 *                          frmHdrBufChunkSize,
                            UInt32 *                          frmBufChunkSize)
{
    UInt32                  totalSize        = 0;
    UInt32                  bufSize          = 0;
    UInt32                  frmBufSizesInFrm = 0;
    ClientNotifyMgr_Params  clientMgrParams;
    GateMP_Params           gateParams;
    UInt32                  i;
    UInt32                  j;
    SizeT                   minAlign;
    SizeT                   hdrBufMinAlign;
    SizeT                   frmBufMinAlign;
    UInt16                  regionId;
    UInt16                  hdrBuf_regionId;
    UInt16                  frmBuf_regionId;

    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgr_ShMem_sharedMemReq",params);

    GT_assert (curTrace, (params != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (params == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgr_ShMem_sharedMemReq",
                             FrameQBufMgr_E_FAIL,
                             "Argument of type (FrameQBufMgr_Params *) is "
                             "NULL!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (params->numFreeFramePools <= FrameQBufMgr_ShMem_MAX_POOLS));

        if (params->sharedAddr != NULL) {
            regionId = SharedRegion_getId(params->sharedAddr);
        }
        else {
            regionId = params->regionId;
        }
        GT_assert (curTrace, (regionId != SharedRegion_INVALIDREGIONID));

        minAlign = Memory_getMaxDefaultTypeAlign();
        if (SharedRegion_getCacheLineSize(regionId) > minAlign) {
            minAlign = SharedRegion_getCacheLineSize(regionId);
        }

        /* Header buffers */
        if (params->sharedAddrHdrBuf != NULL) {
            hdrBuf_regionId = SharedRegion_getId(params->sharedAddrHdrBuf);
        }
        else {
            hdrBuf_regionId = params->hdrBuf_regionId;
        }
        GT_assert (curTrace, (hdrBuf_regionId != SharedRegion_INVALIDREGIONID));

        hdrBufMinAlign = Memory_getMaxDefaultTypeAlign();
        if (SharedRegion_getCacheLineSize(hdrBuf_regionId) > hdrBufMinAlign) {
            hdrBufMinAlign = SharedRegion_getCacheLineSize(hdrBuf_regionId);
        }

        /* FrameQ buffers */
        if (params->sharedAddrFrmBuf) {
            frmBuf_regionId = SharedRegion_getId(params->sharedAddrFrmBuf);
        }
        else {
            frmBuf_regionId = params->frmBuf_regionId;
        }
        GT_assert (curTrace, (frmBuf_regionId != SharedRegion_INVALIDREGIONID));

        frmBufMinAlign = Memory_getMaxDefaultTypeAlign();
        if (SharedRegion_getCacheLineSize(frmBuf_regionId) > frmBufMinAlign) {
            frmBufMinAlign = SharedRegion_getCacheLineSize(frmBuf_regionId);
        }


        totalSize = ROUND_UP(sizeof (FrameQBufMgr_NameServerEntry), minAlign);

        /* Calculate the memory required for the instance
         * shared control structure
         */
        totalSize += ROUND_UP(sizeof (FrameQBufMgr_ShMem_Attrs), minAlign);

        totalSize += ROUND_UP(sizeof (FrameQBufMgr_ShMem_FreeFramePoolObj),
                               minAlign) * params->numFreeFramePools;
        if (FrameQBufMgr_ShMem_usedefaultgate == FALSE) {
            if(params->gate == NULL) {
                GateMP_Params_init(&gateParams);
                gateParams.remoteProtect =
                                  (GateMP_RemoteProtect) params->remoteProtect;

                gateParams.localProtect  =
                                      (GateMP_LocalProtect)params->localProtect;
                totalSize += ROUND_UP(GateMP_sharedMemReq(&gateParams),
                                       minAlign);/* for self */
                gateParams.localProtect  =
                             (GateMP_LocalProtect)GateMP_LocalProtect_INTERRUPT;
                totalSize += ROUND_UP(GateMP_sharedMemReq(&gateParams),
                                       minAlign);/* For ClientNotifyMgr */
            }
        }
        /* Calculate the memory required for the instance's
         * ClientNotifyMgr handle
         */
        ClientNotifyMgr_Params_init (&clientMgrParams);
        clientMgrParams.regionId = params->regionId;
        clientMgrParams.numNotifyEntries = params->numNotifyEntries;
        clientMgrParams.numSubNotifyEntries = params->numFreeFramePools;
        totalSize +=  ROUND_UP(ClientNotifyMgr_sharedMemReq (
                                    (ClientNotifyMgr_Params*)&clientMgrParams ),
                                   minAlign);

        /* Calculate buffer Chunk needed for frame header buffers */

        for (i = 0; i < params->numFreeFramePools; i++) {
            bufSize += (   (    params->numFreeFramesInPool[i]
                              + params->numFreeHeaderBufsInPool[i])
                        * (  ROUND_UP(params->frameHeaderBufSize[i],
                             hdrBufMinAlign) ));
        }

        if (frmHdrBufChunkSize != NULL) {
            *frmHdrBufChunkSize = bufSize;
        }

        /* Calculate buffer Chunk needed for frame buffers */
        /* Reset bufSize */
        bufSize = 0;
        for (i = 0; i < params->numFreeFramePools; i++) {
            for (j = 0; j < params->numFrameBufsInFrame[i]; j++ ) {
                if (params->bufInterfaceType ==
                    FrameQBufMgr_ShMem_BUFINTERFACE_SHAREDMEM) {
                    frmBufSizesInFrm +=
                           ROUND_UP((UInt32)params->frameBufParams[i][j].size,
                                 frmBufMinAlign);
                }
                if (params->bufInterfaceType ==
                    FrameQBufMgr_ShMem_BUFINTERFACE_TILERMEM) {
                        frmBufSizesInFrm = 0;
                      /* TBD */
                }
            }
            bufSize += (frmBufSizesInFrm * params->numFreeFramesInPool[i]);
            frmBufSizesInFrm = 0;
        }

        if (frmBufChunkSize != NULL) {
            *frmBufChunkSize = bufSize;
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_3trace (curTrace,
               GT_LEAVE,
               "FrameQBufMgr_ShMem_sharedMemReq",
               totalSize,
               *frmHdrBufChunkSize,
               *frmBufChunkSize);

    /*! @retval Shared memory required for  the instance */
    return (totalSize);
}
