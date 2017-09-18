/** 
 *  @file   FrameQ_ShMem.h
 *
 *  @brief  Header file for FrameQ on SharedMemory interface.  (Deprecated)
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


#ifndef FRAMEQ_SHMEM_H
#define FRAMEQ_SHMEM_H

#include <ti/ipc/GateMP.h>

#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 * Macros & defines
 * =============================================================================
 */

typedef FrameQ_Config           FrameQ_ShMem_Config ;

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining instance create parameters for the FrameQ on shared
 *          memory(inter core and intra core).
 */
typedef struct FrameQ_ShMem_Params_tag {
    FrameQ_CreateParams commonCreateParams;
    /*!< Common create parameters .Same for all the implementations of FrameQ*/

    GateMP_Handle         gate;
    /*!<
     *  gate used for critical region management of the shared memory.
     *
     *  If it is  null, FrameQ  decides the gate based on the following
     *      1.  If module's  usedefaultgate is set to FALSE, It creates  a gate
     *          internally based on  the localProtect and remoteProtect flags
     *          provided  in this params.
     *      2.  If module's  usedefaultgate is set to TRUE, it uses the  module'
     *          s gloabl gate for the instance. if module's global gate does not
     *          exist FrameQ uses the GateMP's default gate for the instance.
     */

    GateMP_LocalProtect localProtect;
    /*!<
     *  local protection level for the instance. Will be used if gate passed is
     *  NULL.
     */

    GateMP_RemoteProtect remoteProtect;
    /*!<
     *  Multiprocessor protection for the module instances.Will be used
     *  if gate is passed as NULL to create gate internally.
     */

     Ptr                sharedAddr;
    /*!<
     *  Virtual Address of the shared memory.Must be mapped
     *  on other processors if instance needs to be accessible
     *  from other processors.
     *
     *  The creator must supply the shared memory that
     *  this will use to maintain shared state information.
     *  Required parameter if regionId is not specified.
     */

    UInt32              sharedAddrSize;
    /*!<
     *  Size of sharedAddr
     *
     *  Can use the #FrameQ_ShMem_sharedMemReq call to
     *  determine the required size.
     *
     *  Required parameter if regionId is not specified.
     */

    UInt32 regionId;
    /*!<
     *  Shared region ID
     *
     *  The ID corresponding to the shared region in which this shared instance
     *  is to be placed.
     */

    UInt32              numReaders;
    /*!<
     *  Number of readers that can use this instance.
     */

    UInt32              numQueues;
    /*!< Number of queues( Reader can retrieve frames from these queue by
	 *  mentioning the queueId in getv calls.) supported for each reader.
     */
    UInt32              cpuAccessFlags;
    /*!<
     *  cpuAccessFlags flags denoting whether frame buffers are accessed through
     *  CPU or not. All the conrol structure  and header buffers cache flags will
     *  be derived internally based on shared region settings.
     */

    String frameQBufMgrName;
    /*!<
     *
     *  Name of FrameQ bufMgr that needs to be opened in FrameQ instance for use.
     *  Note: The FrameQBufMgr instance has to be created  prior to calling
     *  FrameQ_create API.
     */

    Ptr frameQBufMgrSharedAddr;
    /*!<
     *  Shared Address  of the FrameQBufMgr instance that needs to be used in
     *  FrameQ.Not required if FrameQ_ShMem_Params::frameQBufMgrName is provided.
     */
} FrameQ_ShMem_Params;


/*!
 *  @brief  Structure defining open parameters for the FrameQ instance on ShMem
 *          (shared memory interface).
 */
typedef struct FrameQ_ShMem_OpenParams_tag {
    FrameQ_CommonOpenParams commonOpenParams;
    /*!< Open params common to all the implementations*/
}FrameQ_ShMem_OpenParams;


/* =============================================================================
 *  APIs
 * =============================================================================
 */

/*!
 *  @brief      Function to initiallze the params with the default params.
 *
 *
 *  @param      params   pointer to FrameQ_ShMem_Params.
 *
 *
 *  @sa         FrameQ_create
 */
void
FrameQ_ShMem_Params_init(FrameQ_ShMem_Params * params);

/*!
 *  @brief      Function to find out the shared memory requirements.
 *
 *              This function returns size of the shared memory required for the
 *              instance based on the params provided.
 *
 *  @param      params   pointer to FrameQ_ShMem_Params.
 *
 *  @retval     size    shared memory reqired for the instance
 *
 *  @sa         FrameQ_ShMem_Params_init(), FrameQ_create()
 */
UInt32
FrameQ_ShMem_sharedMemReq (const FrameQ_ShMem_Params * params);

/*
 *  @brief      Get the default configuration for the FrameQ_ShMem module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to FrameQ_setup filled in by
 *              the FrameQ module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfg  Pointer to the FrameQ module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         FrameQ_setup
 */
Void FrameQ_ShMem_getConfig(FrameQ_ShMem_Config *cfg);

/*
 *  @brief      Setup the FrameQ module.
 *
 *              This function sets up the FrameQ module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then FrameQ_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call FrameQ_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional FrameQ module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         FrameQ_destroy, FrameQ_getConfig
 */
Int32
FrameQ_ShMem_setup (FrameQ_ShMem_Config * cfg);

/*
 *  ======== FrameQ_ShMem_destroy ========
 *  API to finalize the module.
 */
Int32
FrameQ_ShMem_destroy (void);

#if defined (__cplusplus)
}
#endif


#endif /* FRAMEQ_SHMEM_H */

