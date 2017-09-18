/** 
 *  @file   FrameQBufMgr_ShMem.h
 *
 *  @brief  Header file for FrameQBufMgr ShMem.  (Deprecated)
 *
 *  @frameqDeprecated
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


#ifndef FRAMEQBUFMGR_SHMEM_H
#define FRAMEQBUFMGR_SHMEM_H

#include <ti/ipc/GateMP.h>
#include <ti/syslink/FrameQBufMgr.h>


#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 * Macros & defines
 * =============================================================================
 */
#define FrameQBufMgr_ShMem_BUFINTERFACE_SHAREDMEM  FrameQBufMgr_BUFINTERFACE_SHAREDMEM
#define FrameQBufMgr_ShMem_BUFINTERFACE_TILERMEM   FrameQBufMgr_BUFINTERFACE_TILERMEM
#define FrameQBufMgr_ShMem_HDRINTERFACE_SHAREDMEM  FrameQBufMgr_HDRINTERFACE_SHAREDMEM
#define FrameQBufMgr_ShMem_FrameBufParams          FrameQBufMgr_FrameBufParams


/* TODO - are different _internal_ vs _external types necessary? */
typedef FrameQBufMgr_Config           FrameQBufMgr_ShMem_Config ;
/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining parameters for the FrameQ module.
 */
typedef struct FrameQBufMgr_ShMem_Params_tag {
    FrameQBufMgr_CreateParams commonCreateParams;
    /*!< Common create parameters.  Same for all the implementations. */

    GateMP_Handle gate;
    /*!<
     *  Gate used for critical region management of the shared memory.
     *
     *  If it is NULL, FrameQBufMgr decides the gate based on the following:
     *      1.  If module's  usedefaultgate is set to FALSE, It creates  a gate
     *          internally based on  the localProtect and remoteProtect flags
     *          provided  in this params.
     *      2.  If module's usedefaultgate is set to TRUE, it uses the module's
     *          global gate for the instance.  If module's global gate does not
     *          exist, FrameQBufMgr uses the GateMP's default gate for the
     *          instance.
     */

    GateMP_LocalProtect localProtect;
    /*!< Local protection level for the module instance. */

    GateMP_RemoteProtect remoteProtect;
    /*!< Default multiprocessor protection for the module instances. */

    Ptr sharedAddr;
    /*!<
     *  Virtual Address of the shared memory. Must be mapped
     *  on other processors if instance needs to be accessible
     *  from other processors.
     *
     *  The creator must supply the shared memory that
     *  this will use for maintain shared state information.
     *  Required parameter.
     */

    UInt32 sharedAddrSize;
    /*!<
     *  Size of shareAddr
     *
     *  Can use FrameQBufMgr_ShMem_sharedMemReq() call to determine the
     *  required size.
     *
     *  Required parameter.
     */

    UInt32 regionId;
    /*!<
     *  Shared region ID
     *
     *  The ID corresponding to the shared region in which this shared instance
     *  is to be placed.
     */

    UInt32 headerInterfaceType;
    /*!<
     *  Type of the interface used for frame headers.
     *
     *  Required parameter.
     */
    UInt32 bufInterfaceType;
     /*!<
     *  Type of the interface used for frame buffers.
     *
     *  Required parameter.
     */

    Ptr sharedAddrHdrBuf;
    /*!<
     *  Virtual Address of the shared memory for frame header buffers. Must be
     *  mapped on other processors if the buffers managed by this instance needs
     *  to be accessible from other processors.
     *
     *  The creator can supply the shared memory that this will use for
     *  maintaining the data buffers. Applications can create instances without
     *  passing this address. In this case applications need to add buffers to
     *  the instance at runtime.
     */

    UInt32 sharedAddrHdrBufSize;
    /*!< Size of sharedAddrHdrBuf */

    UInt32 hdrBuf_regionId;
    /*!
     *  Shared region ID for Header buffers.
     *
     *  The ID corresponding to the shared region from which header buffers
     *  will be allocated if sharedAddrHdrBuf is not provided.
     */

    Ptr sharedAddrFrmBuf;
    /*!< Virtual shared Region Address of chunk for frame buffers. */

    UInt32 sharedAddrFrmBufSize;
    /*!< Size of the sharedAddrFrmBuf */

    UInt32 frmBuf_regionId;
    /*!<
     *  Shared region ID for Frame buffers.
     *
     *  The ID corresponding to the shared region from which frame buffers
     *  will be allocated if sharedAddrFrmBuf is not provided.
     */

    UInt32 numFreeFramePools;
    /*!< Number of buffer pools */

    UInt32 numFreeFramesInPool[FrameQBufMgr_MAX_POOLS];
    /*!< Number of free frames in each pool */

    UInt32 numFreeHeaderBufsInPool[FrameQBufMgr_MAX_POOLS];
    /*!< Number of free header buffers in each pool*/

    UInt32 frameHeaderBufSize[FrameQBufMgr_MAX_POOLS];
    /*!< Array of frame header buffer sizes for all the pools*/

    UInt32 numFrameBufsInFrame[FrameQBufMgr_MAX_POOLS];
    /*!< Number of frame buffers in a frame. For each pool, Apps can specify
     * the number of buffers in frames
     */

    FrameQBufMgr_ShMem_FrameBufParams *frameBufParams[FrameQBufMgr_MAX_POOLS];
    /*!< frame buffer size of framebuffers in pool */

    UInt32 cpuAccessFlags;
    /*!<
     *  cpuAccessFlags flags denoting whether frame buffers are accessed through
     *  CPU or not. All the conrol structure  and header buffers cache flags will
     *  be derived internally based on shared region settings.
     */

    UInt32 numNotifyEntries;
    /*!<
     * Number of clients that have registered callback functions with the
     * instance.
     */
} FrameQBufMgr_ShMem_Params;

/*!
 *  @brief  Structure defining open parameters for the FrameQ instance on ShMem
 *          (shared memory interface).
 */
typedef struct FrameQBufMgr_ShMem_OpenParams_tag {
    FrameQBufMgr_CommonOpenParams commonOpenParams;
    /*!< open params common to all the implementations*/
} FrameQBufMgr_ShMem_OpenParams;

/* =============================================================================
 *  APIs
 * =============================================================================
 */

/* Initialize the FrameQBufMgr module, there would be some default values for
 * the parameters, but if  any change is required, change the cfg before calling
 * this API.
 */
Int32 FrameQBufMgr_ShMem_setup(FrameQBufMgr_ShMem_Config * cfg);


/* Function to destroy(finalize) the FrameQBufMgr module.*/
Int32 FrameQBufMgr_ShMem_destroy(Void);

/*!
 *  @brief      Function to initiallze the params with the default params.
 *
 *  @param      params   pointer to FrameQBufMgr_ShMem_Params.
 *
 *  @sa         FrameQBufMgr_create
 */
void FrameQBufMgr_ShMem_Params_init(FrameQBufMgr_ShMem_Params * params);

/*!
 *  @brief      Function to find out the shared memory requirements.
 *
 *              This function returns size of the shared memory required for the
 *              instance based on the params provided.
 *
 *  @param      params          pointer to FrameQ_ShMem_Params.
 *  @param[out] hdrBufChunkSize Memory required for frame headers
 *  @param[out] frmBufChunkSize Memory required for frame buffers
 *
 *  @retval     size    shared memory required for the instance
 *
 *  @sa         FrameQBufMgr_ShMem_Params_init(), FrameQBufMgr_create()
 */
UInt32
FrameQBufMgr_ShMem_sharedMemReq (const FrameQBufMgr_ShMem_Params * params,
                                 UInt32                    * hdrBufChunkSize,
                                 UInt32                    * frmBufChunkSize);

Int32 FrameQBufMgr_ShMem_getConfig(FrameQBufMgr_ShMem_Config *cfg);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* HEAPBUF_H_0x4CD5 */
