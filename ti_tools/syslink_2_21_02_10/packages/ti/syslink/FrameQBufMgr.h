/** 
 *  @file   FrameQBufMgr.h
 *
 *  @brief      Defines for interfaces for FrameQBufMgr module.  (Deprecated)
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



#ifndef FRAMEQBUFMGR_H
#define FRAMEQBUFMGR_H


/* Module headers */
#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/SysLink.h>
#include <ti/ipc/Notify.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*!
 *  @brief  Indicates module has been already destroyed.
 */
#define FrameQBufMgr_S_ALREADYDESTROYED              2

/*!
 *  @brief  Indicates module is already setup.
 */
#define FrameQBufMgr_S_ALREADYSETUP                   1

/*!
 *  @brief  Operation successful.
 */
#define  FrameQBufMgr_S_SUCCESS                       0

/*!
 *  @brief  General Failure
 */
#define FrameQBufMgr_E_FAIL                          -1

/*!
 *  @brief  Invalid arguments are passed to  FrameQBufMgr API.
 */
#define FrameQBufMgr_E_INVALIDARG                    -2

/*!
 *  @brief  Instance not found.
 */
#define FrameQBufMgr_E_NOTFOUND                      -3

/*!
 *  @brief  FrameQBufMgr instance  already exists
 */
#define FrameQBufMgr_E_INST_EXISTS                   -4

/*!
 *  @brief  Failed to allocate buffer
 */
#define FrameQBufMgr_E_ALLOC_BUF                     -5

/*!
 *  @brief  Failed to allocate memory.
 */
#define FrameQBufMgr_E_ALLOC_MEMORY                  -6

/*!
 *  @brief  Name server create failed for the module.
 */
#define FrameQBufMgr_E_CREATE_NAMESERVER             -7

/*!
 *  @brief  ClientNotifyMgr create failed.
 */
#define FrameQBufMgr_E_FAIL_CLIENTN0TIFYMGR_CREATE   -8

/*!
 *  @brief  ClientNotifyMgr open failed.
 */
#define FrameQBufMgr_E_CLIENTN0TIFYMGR_OPEN         -9

/*!
 *  @brief  Creation of  GateMP failed.
 */
#define FrameQBufMgr_E_CREATE_GATEMP                 -10

/*!
 *  @brief  GateMP open failed.
 */
#define FrameQBufMgr_E_OPEN_GATEMP                   -11

/*!
 *  @brief  Maximum instances limit reached.
 */
#define FrameQBufMgr_E_MAX_CLIENTS                   -12

/*!
 *  @brief  Operation can not be permitted or not implemented.
 */
#define FrameQBufMgr_E_ACCESSDENIED                  -13

/*!
 *  @brief  API is not implemented.
 */
#define FrameQBufMgr_E_NOTIMPLEMENTED                -14

/*!
 *  @brief  Module is not initialized.
 */
#define FrameQBufMgr_E_INVALIDSTATE                  -15

/*!
 *  @brief  Failure in OS call.
 */
#define FrameQBufMgr_E_OSFAILURE                     -16

/*!
 *  @brief  Max instances limit reached. Can not create  instance.
 */
#define FrameQBufMgr_E_MAXINSTANCES                  -17

/*!
 *  @brief  Unsupported interface type.
 */
#define FrameQBufMgr_E_INVALID_INTERFACE             -18

/*!
 *  @brief  Failed to open NameServer.
 */
#define FrameQBufMgr_E_OPEN_NAMESERVER               -19

/*!
 *  @brief  Insufficient header buffers.
 */
#define FrameQBufMgr_E_INSUFFICIENT_HDRBUFS          -20

/*!
 *  @brief  Failed to allocate frame.
 */
#define FrameQBufMgr_E_ALLOC_FRAME                   -21

/*!
 *  @brief  Not able to allocate the requested frames.Able to allocate
 *          only few frames
 */
#define FrameQBufMgr_E_ALLOC_ALLFRAMES               -22

/*!
 *  @brief  Invalid MemMgr type specified.
 */
#define FrameQBufMgr_E_INVALID_MEMMGRTYPE            -23

/*!
 *  @brief  Invalid Header buffer interface type specified.see
 *          enum FrameQBufMgr_FrameHeaderInterface for the supported types.
 */
#define FrameQBufMgr_E_INVALID_HDRINTERFACETYPE      -24

/*!
 *  @brief  Invalid frame buffer interface type specified.see
 *          enum FrameQBufMgr_FrameBufferInterface for the supported types.
 */
#define FrameQBufMgr_E_INVALID_BUFINTERFACETYPE      -25

/*!
 *  @brief  NameServer_add failed.
 */
#define FrameQBufMgr_E_NAMESERVERADD                 -33

/*!
 *  @brief  Handle for the FrameQBufMgr instance.
 */
typedef struct FrameQBufMgr_Object* FrameQBufMgr_Handle;


/* =============================================================================
 * macros & defines
 * =============================================================================
 */
#define FrameQBufMgr_FrameBufInfo       Frame_FrameBufInfo

/*!
 *  @brief  Defines the frame header structure
 */
#define FrameQBufMgr_FrameHeader        Frame_FrameHeader

/*!
 *  @brief  Defines the type for a frame pointer
 */
typedef Frame_FrameHeader *             FrameQBufMgr_Frame;

/*!
 *  @brief  Maximum length of the  name string in bytes.
 */
#define FrameQBufMgr_MAXNAMELEN                 (32u)

/*!
 *  @brief  Maximum number of instances managed by FrameQBufMgr module.
 */
#define FrameQBufMgr_MAXINSTANCES               (64u)

/*!
 *  @brief  Maximum number of free frame pools to be maintained in a instance.
 */
#define FrameQBufMgr_MAX_POOLS                  (64u)

/*!
 *  @brief  Maximum number of frame buffers in a frame managed by in
 *          FrameQBufMgr instance.
 */
#define FrameQBufMgr_MAX_FRAMEBUFS              (8)

/*!
 *  @brief  Maximum number of static free Frames  managed by FrameQBufMgr
 *          instance Free Frame pool.
 */
#define FrameQBufMgr_POOL_MAXFRAMES             (32)

/*!
 *  @brief  Maximum number of additional static free headers  managed by
 *          FrameQBufMgr instance for each Free Frame pool.
 */
#define FrameQBufMgr_POOL_MAXHDRS               (64)

/*!
 *  @brief  Maximum number of dynbuffers managed by each buffer pool in
 *          FrameQBufMgr instance.
 */
#define FrameQBufMgr_POOL_DYNAMIC_MAXFRAMES     (1)

/*!
 *  @brief  Maximum number of additional dynamic free headers  managed by
 *          FrameQBufMgr instance for each Free Frame pool.
 */
#define FrameQBufMgr_POOL_DYNAMIC_MAXHDRS       (1)

/*!
 *  @brief  Notify Event Number to be used by  FrameQBufMgr module.
 */
#define FrameQBufMgr_NOTIFY_RESERVED_EVENTNO   (0u)


/*!
 *  @brief  Macro used in FrameQ for sorting of passed parameters in V API.
 *          In FrameQBufMgr_allocv case this denotes the number of frames that
 *          can be allocated in one call.
 */
#define FrameQBufMgr_MAX_FRAMESINVAPI           (FrameQ_MAX_FRAMESINVAPI)

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Enumeration of CPU access flags for frame buffers. For frame headers
 *          CPUACCESS is assumed to be TRUE..
 */
typedef enum FrameQBufMgr_CpuAccessFlags_Tag {
        FrameQBufMgr_FRAMEBUF0_CPUACCESS    =  0x10000,
        /*!< If specified first frame buffer is accessed  through CPU
         *  (direct memory access).
         */
        FrameQBufMgr_FRAMEBUF1_CPUACCESS    =  0x20000,
        /*!< If specified second frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQBufMgr_FRAMEBUF2_CPUACCESS    =  0x40000,
        /*!< If specified third frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQBufMgr_FRAMEBUF3_CPUACCESS    =  0x80000,
        /*!< If specified fourth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQBufMgr_FRAMEBUF4_CPUACCESS    = 0x100000,
        /*!< If specified fifth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQBufMgr_FRAMEBUF5_CPUACCESS    = 0x200000,
        /*!< If specified sixth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQBufMgr_FRAMEBUF6_CPUACCESS    = 0x400000,
        /*!< If specified seventh frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQBufMgr_FRAMEBUF7_CPUACCESS    = 0x800000
        /*!< If specified eighth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
} FrameQBufMgr_CpuAccessFlags;

/*!
 *  @brief   Denotes the type of buffer.
 */
typedef enum FrameQBufMgr_BufType_Tag {
    FrameQBufMgr_BUF_FRAMEHEADER = 0u,
    /*!< Denotes buffer is  of type used for frame headers. */
    FrameQBufMgr_BUF_FRAMEBUF    = 1u,
    /*!< Denotes buffer is  of type used for frame buffers */
    FrameQBufMgr_BUF_ENDVALUE    = 2u
    /*!< End delimiter indicating start of invalid values for this enum */
}FrameQBufMgr_BufType;

/*!
 *  @brief   Enumerations to indicate address types used for translation.
 */
typedef enum FrameQBufMgr_AddrType_Tag{
    FrameQBufMgr_AddrType_Virtual  = 0u,
    /*!< Virtual address on calling process on DSP where MMU is not configured
     *   it could be the physical address.
     */
    FrameQBufMgr_AddrType_Portable = 1u,
    /*!< This is the shared region address incase sharedmemory manager plugged
     *   in to frameQbufMgr. In case of tiler it could be the tiler specific
     *   portable address
     */
    FrameQBufMgr_AddrType_EndValue = 2u
    /*!< End delimiter indicating start of invalid values for this enum*/
}FrameQBufMgr_AddrType;

/*!
 *  @brief  Enumeration of FrameQBufMgr interface types.Denotes different
 *          implementation types.
 */
typedef enum FrameQBufMgr_Interface_Tag {
    FrameQBufMgr_INTERFACE_SHAREDMEM = 0x0,
    /*!< Denotes FrameQBufMgr implementation (FrameQBufMgr_ShMem
     * Implementation) on shared memory.
     */
    FrameQBufMgr_INTERFACE_NONE      = 0x1
} FrameQBufMgr_Interface;

/*!
 *  @brief  Enumeration of FrameQBufMgr interface types.Denotes different
 *          implementation types.
 */
typedef enum FrameQBufMgr_FrameHeaderInterface_Tag {
    FrameQBufMgr_HDRINTERFACE_SHAREDMEM = 0x0,
    /*!< Denotes FrameQBufMgr headers are from shared memory.Address translations
     *  will be done using shared region.
     */
    FrameQBufMgr_HDRINTERFACE_NONE      = 0x1
} FrameQBufMgr_FrameHeaderInterface;

/*!
 *  @brief  Enumeration of FrameQBufMgr interface types.Denotes different
 *          implementation types.
 */
typedef enum FrameQBufMgr_FrameBufferInterface_Tag {
    FrameQBufMgr_BUFINTERFACE_SHAREDMEM = 0x0,
    /*!< Denotes FrameQBufMgr implementation (FrameQBufMgr_ShMem
     * Implementation) on shared memory.
     */
    FrameQBufMgr_BUFINTERFACE_TILERMEM = 0x1,
    /*!< valid only on RTOS side for TI81XX platforms. Not valid on HLOS side.
     */
    FrameQBufMgr_BUFINTERFACE_NONE      = 0x2
} FrameQBufMgr_FrameBufferInterface;

/*!
 *  @brief  Structure defining module config parameters for the FrameQBufMgr
 *          module.
 */
typedef struct FrameQBufMgr_Config_Tag {
    UInt32 eventNo;
    /*!<  Event number used for notification*/
    Bool   maxInstances;
    /*!< Maximum instances of the module*/
    UInt32  usedefaultgate;
    /*  If set  to true FrameQBufMgr  instances will use dafault gate set in
     *  FrameQ.
     *  If default gate is not set in FrameQBufMgr, it uses GateMP's default
	 *  gate. Set it to zero if   FrameQ needs to  create  gate for a
     *  instance (remoteProtect and localProtect  flags should be valid in
     *  params in this case).
     */
} FrameQBufMgr_Config;

/*!
 *  @brief  Common parameters required to create FrameQBufMgr instance of
 *          any implementation.
 *
 *  This struct must be the first field of the implementation-specific params
 *  structure.
 */
typedef struct FrameQBufMgr_CreateParams_Tag {
    UInt32 size;                        /**< size of param struct */
    FrameQBufMgr_Interface ctrlInterfaceType; /**< Interface type */
    String name;                        /**< Instance name */
    UInt32 openFlag;                    /**< Open flag to indicate
                                         * create or open.
                                         *
                                         *  @remarks    This is an internal
                                         *              flag, the caller
                                         *              should not touch this
                                         *              flag during create
                                         */
} FrameQBufMgr_CreateParams;

/*!
 *  @brief  Common parameters required to open a FrameQBufMgr instance of
 *          any implementation.
 */
typedef struct FrameQBufMgr_CommonOpenParams_Tag {
    String      name;
    /*!< Name of the instance to open */

    UInt32      cpuAccessFlags;
    /*!< cpuAccessFlags flags to know if Frame buffers are accessed through CPU*/

    Ptr         sharedAddr;
    /*!<  Virtual shared Region addr in case open by address is required.*/
} FrameQBufMgr_CommonOpenParams;

/*!
 *  @brief  Common parameters required to create FrameQBufMgr instance of
 *          any implementation.
 *
 *  This struct must be the first element of the implementation-specific
 *  create params structure.
 */
typedef struct FrameQBufMgr_Params {
    FrameQBufMgr_CreateParams   commonCreateParams;
    Ptr impParams;              /**< Pointer to implementation-specific
                                 *   struct
                                 */
    UInt32 impParamsSize;       /**< Size of the implementation-specific
                                 *   @c impParams struct.
                                 */
} FrameQBufMgr_Params;

/*!
 *  @brief  Frame buffer param structure.
 */
typedef struct FrameQBufMgr_FrameBufParams_Tag {
    UInt32 size;
    /*!< size of the Frame buffer. valid if it is not a Tiler buffer*/
    UInt32 pixelFormat;
    /*!< Pixel format.Valid for Tiler buffers */
    UInt32 height;
    /*!< Height of buffer.Valid for Tiler buffers */
    UInt32 width;
    /*!< Width of the buffer.Valid for Tiler buffers */
    UInt32 align;
    /*!< Alignment of the buffer.Currently not used.*/
}FrameQBufMgr_FrameBufParams;

/*!
 *  @brief  Prototype of the FrameQBufMgr call back function.
 */
typedef void (*FrameQBufMgr_NotifyFunc) (FrameQBufMgr_Handle, Ptr arg, UInt32);

/*!
 *  @brief  Structure defining notification parameter structure.
 */
typedef struct FrameQBufMgr_NotifyParams_Tag {
    SysLink_NotifyType      notifyType;
    /*!< Notification type.See SysLink_NotifyType*/
    UInt32                  watermark;
    /*!< Minumum number of free frames required to generate notification
     *   Same water mark for all the individual freeFramePools in the set.
     */
    FrameQBufMgr_NotifyFunc cbFxnPtr;
    /*!< Callback function */
    Ptr                     cbContext;
    /*!< Context pointer that needs to be passed to call back function.*/
    Ptr                     cbClientHandle;
    /*!< Handle that needs to be passed as first arg to call back.
     *   Ideally this should be callers client handle.*/
} FrameQBufMgr_NotifyParams;

/* =============================================================================
 * APIs
 * =============================================================================
 */

/*!
 *  @brief  Function to setup the FrameQBufMgr module.
 *
 *          This function sets up the FrameQBufMgr module. This function must
 *          be called before any other instance-level APIs can be invoked.
 *          Module-level configuration needs to be provided to this
 *          function. If the user wishes to change some specific config
 *          parameters, then FrameQBufMgr_getConfig can be called to get the
 *          configuration filled with the default values. After this, only
 *          the required configuration values can be changed. If the user
 *          does not wish to make any change in the default parameters, the
 *          application can simply call setup with NULL parameters.
 *          The default parameters would get automatically used.
 *  @param  cfg
 *          Optional FrameQBufMgr module configuration. If provided as
 *          NULL, default configuration is used.
 *
 *  @sa    None.
 */
Int32 FrameQBufMgr_setup(FrameQBufMgr_Config *cfg);


/*!
 *  @brief  Function to destroy(finalize) the FrameQBufMgr module.
 */
Int32 FrameQBufMgr_destroy(Void);


/*!
 *  @brief      Function to create FrameQBufMgr instance.
 *
 *              This function create a new instance of FrameQBufMgr. It creates
 *              frameQBufMgr based on the params specific to implementation.
 *
 *  @param      params      pointer to implementation specific params.
 *
 *  @retval     Handle      Instance handle.
 *  @retval     NULL        Create failed.
 *
 *  @sa         FrameQBufMgr_Shmem_params, FrameQBufMgr_delete(),
 *              FrameQBufMgr_open() and FrameQBufMgr_close.
 */
FrameQBufMgr_Handle FrameQBufMgr_create(Ptr params);


/*!
 *  @brief      Function to delete FrameQBufMgr instance.
 *
 *              This function delets FrameQBufMgr instance that is created.
 *
 *  @param      pHandle     Pointer to the created frameQ instance handle.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS     Delete successful.
 *  @retval     FrameQBufMgr_E_FAIL        Delete failed.
 *  @retval     FrameQBufMgr_E_INVALIDARG  Pointer to Handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG        Handle passed is null.
 *
 *  @sa         FrameQBufMgr_create(), FrameQBufMgr_open() and
 *              FrameQBufMgr_close.
 */
Int32 FrameQBufMgr_delete(FrameQBufMgr_Handle *pHandle);


/*!
 *  @brief      Function to open the created FrameQBufMgr instance.
 *
 *              This function opens the  FrameQBufMgr instance  depending upon
 *              the open params passed to it.The instance must be created before
 *              opening it. Application is expected to pass the implementations
 *              specific open params for the openParams field.FrameQ_open
 *              internally calls this API  to use the FrameQBufMgr instance to
 *              allocate frames ftom it.
 *
 *  @param      handlePtr   Return parameter.Instance opened in given mode.
 *  @param      openParams  Pointer to implementation specific open params.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      open successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   Invalid parameter specified.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *  @retval     FrameQBufMgr_E_MEMORY       Memory_alloc failed.
 *  @retval     FrameQBufMgr_E_ALLOC_MEMORY Memory_alloc failed.
 *  @retval     FrameQBufMgr_E_FRAMEQBUFMGROPEN FrameQBufMge instance open
 *                                              failed.
 *  @retval     FrameQBufMgr_E_OPEN_GATEMP  GateMP open failed.
 *  @retval     FrameQBufMgr_E_CLIENTN0TIFYMGR_OPEN ClientNotifyMgr open
 *                                                       failed.
 *  @retval     FrameQBufMgr_E_NOTFOUND     Instance not found in FrameQBufMgr
 *                                          Nameserver or instance  is not crated
 *                                          at the shared address obtained
 *                                          internally.
 *  @retval     FrameQBufMgr_E_FAIL         Failed due to Both name is null and
 *                                          shared addr is null.
 *  @retval     FrameQBufMgr_E_INVALID_HDRINTERFACETYPE Failed to create the
 *                                          Syslink memory manager for header
 *                                          buffers.
 *  @retval     FrameQBufMgr_E_INVALID_BUFINTERFACETYPE Failed to create the
 *                                          Syslink memory manager for frame
 *                                          buffers.
 *
 *  @sa         FrameQBufMgr_ShMem_openParams, FrameQBufMgr_create(),
 *              FrameQBufMgr_delete() and FrameQBufMgr_close.
 */
Int32 FrameQBufMgr_open(FrameQBufMgr_Handle *handlePtr, Ptr openParams);


/*!
 *  @brief      Function to open the created FrameQBufMgr instance using the
 *              address provided in open params.
 *
 *              This function opens the  FrameQBufMgr instance  depending upon
 *              the open params passed to it.The instance must be created before
 *              opening it. Application is expected to pass the implementations
 *              specific open params for the openParams field.
 *
 *  @param      handlePtr   Return parameter.Instance opened in given mode.
 *  @param      openParams  Pointer to implementation specific open params.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      open successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   Invalid parameter specified.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *  @retval     FrameQBufMgr_E_MEMORY       Memory_alloc failed.
 *  @retval     FrameQBufMgr_E_ALLOC_MEMORY Memory_alloc failed.
 *  @retval     FrameQBufMgr_E_FRAMEQBUFMGROPEN FrameQBufMge instance open
 *                                              failed.
 *  @retval     FrameQBufMgr_E_OPEN_GATEMP  GateMP open failed.
 *  @retval     FrameQBufMgr_E_CLIENTN0TIFYMGR_OPEN ClientNotifyMgr open
 *                                                       failed.
 *  @retval     FrameQBufMgr_E_NOTFOUND     Instance not found in FrameQBufMgr
 *                                          Nameserver or instance  is not crated
 *                                          at the shared address obtained
 *                                          internally.
 *  @retval     FrameQBufMgr_E_FAIL         Failed due to shared addr is null.
 *  @retval     FrameQBufMgr_E_INVALID_HDRINTERFACETYPE Failed to create the
 *                                          Syslink memory manager for header
 *                                          buffers.
 *  @retval     FrameQBufMgr_E_INVALID_BUFINTERFACETYPE Failed to create the
 *                                          Syslink memory manager for frame
 *                                          buffers.
 *
 *  @sa         FrameQBufMgr_ShMem_openParams, FrameQBufMgr_create(),
 *              FrameQBufMgr_delete() and FrameQBufMgr_close.
 */
Int32
FrameQBufMgr_openByAddr (FrameQBufMgr_Handle     * handlePtr,
                         Ptr                       openParams);

/*!
 *  @brief      Function to close the opened FrameQBufMgr instance.
 *
 *              This function closes FrameQBufMgr instance that is opened.
 *
 *  @param      pHandle     Pointer to the opened FrameQBufMgr instance handle.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS     Close successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG  Pointer to Handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG  Handle passed is null.
 *  @retval     FrameQBufMgr_E_ACCESSDENIED Invalid handle passed.
 *
 *  @sa         FrameQBufMgr_create, FrameQBufMgr_open and
 *              FrameQBufMgr_delete.
 */
Int32
FrameQBufMgr_close (FrameQBufMgr_Handle *pHandle);

/*!
 *  @brief      Function to allocate frame from  free frame pool zero.
 *
 *              This function allocates a frame when called by FrameQBufMgr.
 *              from free Frame Pool zero.
 *
 *  @param      handle      FrameQBufMgr handle.
 *  @param      frame       Location to receive the allocated frame.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Successfully allocated frame.
 *  @retval     FrameQBufMgr_E_FAIL         Invalid buffer interface type.
 *  @retval     FrameQBufMgr_E_ALLOC_FRAME  Free frames are not available to
 *                                          allocate.
 *  @retval     FrameQBufMgr_E_INVALIDARG   framePtr paased is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_ACCESSDENIED Provided handle can not allocate frames.
 *                                    Only writer can allocate frames.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQ_alloc, FrameQ_free, FrameQBufMgr_free.
 */
Int32
FrameQBufMgr_alloc (FrameQBufMgr_Handle     handle,
                    FrameQBufMgr_Frame     *frame);

/*!
 *  @brief      Function to allocate multiple frames from the FrameQbufMgr
 *              instance..
 *
 *              This function allocates frames from multiple free pools.This API
 *              allocates all the frames requested if  available other wise
 *              returns failure.
 *
 *  @param      handle      FrameQBufMgr handle.
 *  @param      framePtr    Array to receive pointers to allocated frames..
 *  @param      freeQId     Array of free frame pool nos of the FrameQBufMgr
 *                          from  from which this API needs to allocate free
 *                          frames .
 *  @param      numFrames   Number of frames that needs to be allocated.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Successfully allocated frames.
 *  @retval     FrameQBufMgr_E_ALLOC_FRAME Failed due to non availabilty of free
 *                                    frames in free frame pool.
 *  @retval     FrameQBufMgr_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   freeQId passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   numFrames passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQ_allocv, FrameQ_freev, FrameQBufMgr_freev.
 */
Int32
FrameQBufMgr_allocv (FrameQBufMgr_Handle    handle,
                     FrameQBufMgr_Frame     framePtr[],
                     UInt32                  freeQId[],
                     UInt8                * numFrames);

/*!
 *  @brief      Function to free frame.
 *
 *              This function frees a frame.
 *
 *  @param      handle      FrameQBufMgr instance handle.
 *  @param      frame       Frame to be freed.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Successfully freed frame.
 *  @retval     FrameQBufMgr_E_FAIL         Free failed.
 *  @retval     FrameQBufMgr_E_INVALIDARG   frame passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQ_free, FrameQBufMgr_alloc
 */
Int32
FrameQBufMgr_free (FrameQBufMgr_Handle      handle,
                   FrameQBufMgr_Frame       frame);

/*!
 *  @brief      Function to free multiple frames  to the  FrameQBufMgr.
 *
 *              This function frees multiple frames to the FrameQBufMgr
 *              instance.
 *
 *  @param      handle      FrameQBufMgr instance handle.
 *  @param      framePtr    Array of frames to be freed.
 *  @param      numFrames   Number of frames to free. The size of the framePtr
 *                          should be at least numFrames.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Successfully freed frames.
 *  @retval     FrameQBufMgr_E_FAIL         Freev failed.
 *  @retval     FrameQBufMgr_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQ_freev, FrameQBufMgr_allocv
 */
Int32
FrameQBufMgr_freev (FrameQBufMgr_Handle     handle,
                    FrameQBufMgr_Frame      framePtr[],
                    UInt32                  numFrames);


/*!
 *  @brief      Function to add frame.
 *
 *              This function is to add free frames dynamically to the given pool
 *              id (freeQId).
 *              NOTE: API is not implemented.
 *
 */
Ptr
FrameQBufMgr_add (FrameQBufMgr_Handle       handle,
                  UInt8                     freeQId);

/*!
 *  @brief      Function to remove frame.
 *
 *              This function is to remove the frames  that is added dynamically.
 *              NOTE: API is not implemented.
 *
 */
Int32
FrameQBufMgr_remove (FrameQBufMgr_Handle    handle,
                     UInt8                  freeQId,
                     FrameQBufMgr_Frame     framePtr);

/*!
 *  @brief      Function to duplicate the frame.
 *
 *              This function duplicates the given frame and returns the
 *              duplicated frame. It internally allocates one Frame header buffer
 *              and copy the header info to the allocated header buffer and
 *              returns it. it also increments the reference count of the
 *              original frame by 1 as both original frame and duped frmae are
 *              pointing to the same frame buffers.
 *
 *  @param      handle      FrameQBufMgr instance handle.
 *  @param      framePtr    Original frame  that needs to be duplicated.
*   @param      dupedFramePtr Location to receive the duped frames.
 *  @param      numDupedFrames  Number of duplicated frames required.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Successfully duped frame.
 *  @retval     FrameQBufMgr_E_FAIL         dup failed.
 *  @retval     FrameQBufMgr_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   dupedFramePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQ_dup, FrameQBufMgr_alloc, FrameQBufMgr_allocv
 */
Int32
FrameQBufMgr_dup (FrameQBufMgr_Handle       handle,
                  FrameQBufMgr_Frame        framePtr,
                  FrameQBufMgr_Frame        dupedFramePtr[],
                  UInt32                    numDupedFrames);

/*!
 *  @brief      Function to duplicate the multiple frames
 *
 *              API to duplicate the given frames i.e it increments the
 *              corresponding frames's reference count.It internally allocates
 *              headers for the duped frames.API fails if it is not able to dup
 *              all the frames.
 *
 *  @param      handle          Instance handle.
 *  @param      framePtr        Array of frames to duplicated.
 *  @param      dupedFramePtr   Location to receive duplicated frames.
 *  @param      numDupedFrames  Number of duplicated frames required for each
 *                              given frame.
 *  @param      numFrames       Number of original frames that needs to be
 *                              duplicated.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Successfully duped frame.
 *  @retval     FrameQBufMgr_E_FAIL         dup failed.
 *  @retval     FrameQBufMgr_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   dupedFramePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQ_dup, FrameQBufMgr_alloc, FrameQBufMgr_allocv,
 *              FrameQBufMgr_dup
 */
Int32
FrameQBufMgr_dupv (FrameQBufMgr_Handle   handle,
                   FrameQBufMgr_Frame    framePtr[],
                   FrameQBufMgr_Frame  **dupedFramePtr,
                   UInt32                numDupedFrames,
                   UInt32                numFrames);

/*!
 *  @brief      Function to register call back function for notification
 *
 *              Function to Register notification with the instance. It
 *              internally registers notification with the individual free frame
 *              pools.If alloc on free frame pool 0 failed,Notification will be
 *              generated if free buffers  in that pool becomes more than
 *              watermark.
 *
 *  @param      handle          Instance handle.
 *  @param      notifyParams    notify params.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   notifyParams passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *  @retval     FrameQBufMgr_E_FAIL         Failed to register notification.
 *
 *  @sa         FrameQBufMgr_unregisterNotifier
 */
Int32
FrameQBufMgr_registerNotifier (FrameQBufMgr_Handle        handle,
                               FrameQBufMgr_NotifyParams *notifyParams);

/*!
 *  @brief      Function to unregister call back function.
 *
 *              Function to Register notification with the instance. It
 *              internally unregisters notification with the individual free frame
 *              pools.
 *
 *  @param      handle          Instance handle.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Successfully duped frame.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   notifyParams passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *  @retval     FrameQBufMgr_E_FAIL         Failed to register notification.

 *
 *  @sa         FrameQBufMgr_registerNotifier
 */
Int32
FrameQBufMgr_unregisterNotifier (FrameQBufMgr_Handle handle);

/*!
 *  @brief      Function to write back  the frame.
 *
 *              Function to write back the contents  of buffer .
 *
 *  @param      handle          Instance handle.
 *  @param      framePtr        frame to be written back to external memory.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQBufMgr_invlidate
 */
Int32
FrameQBufMgr_writeBack(FrameQBufMgr_Handle handle, FrameQBufMgr_Frame framePtr);

/*!
 *  @brief      Write back the contents  of buffer.
 *
 *              Function to write back the contents  of buffer . Flags denotes
 *              whether it is header buffer or frame  buffer..
 *
 *  @param      handle          Instance handle.
 *  @param      buf             Buffer to be written back.
 *  @param      flags           Flags denotes whether buf is a header buffer or
 *                              frame  buffer.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   notifyParams passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQBufMgr_invlidate
 */
Int32
FrameQBufMgr_writeBackBuf(FrameQBufMgr_Handle handle,
                          Ptr                 buf,
                          UInt32              flags);

/*!
 *  @brief      Function to invalidate frame.
 *
 *              Function to invalidate the contents  of frame .
 *
 *  @param      handle          Instance handle.
 *  @param      framePtr        frame to be invalidated from external memory.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQBufMgr_writeback
 */
Int32
FrameQBufMgr_invalidate(FrameQBufMgr_Handle handle,
                        FrameQBufMgr_Frame framePtr);

/*!
 *  @brief      Function to invalidate frame buffer.
 *
 *              Function to invalidate the contents  of frame buffer .
 *
 *  @param      handle          Instance handle.
 *  @param      frameBuf        framebuffer pointer.
 *  @param      size            Size to be invalidated.
 *  @param      bufIndexInFrame index of the frame buffer in  the frame to which
 *                              the buffer bellongs to.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   frameBuf passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQBufMgr_writeback
 */
Int32
FrameQBufMgr_invalidateFrameBuf(FrameQBufMgr_Handle  handle,
                                Ptr                  frameBuf,
                                UInt32               size,
                                UInt8                bufIndexInFrame);

/*!
 *  @brief      Function to writeback frame buffer.
 *
 *              Function to writeback the contents  of frame buffer .
 *
 *  @param      handle          Instance handle.
 *  @param      frameBuf        framebuffer pointer.
 *  @param      size            Size to be writtenback.
 *  @param      bufIndexInFrame index of the frame buffer in  the frame to which
 *                              the given buffer bellongs to.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   frameBuf passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 *
 *  @sa         FrameQBufMgr_writeback
 */
Int32
FrameQBufMgr_writeBackFrameBuf(FrameQBufMgr_Handle  handle,
                               Ptr                  frameBuf,
                               UInt32               size,
                               UInt8                bufIndexInFrame);
/*!
 *  @brief      Invalidate the contents  of  frame header.
 *
 *              API treates the passed buffer as frame Header.
 *
 *  @param      handle      Handle to the instance.
 *  @param      headerBuf   frame header to be invalidated.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   headerBuf passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 */
Int32
FrameQBufMgr_invalidateHeaderBuf(FrameQBufMgr_Handle  handle,
                                 Ptr                  headerBuf);

/*!
 *  @brief      Write back the contents  of  frame header.
 *
 *              API treates the passed buffer as frame Header.
 *
 *  @param      handle      Handle to the instance.
 *  @param      headerBuf   frame header to be written back.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   headerBuf passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 */
Int32
FrameQBufMgr_writeBackHeaderBuf(FrameQBufMgr_Handle  handle,
                                Ptr                  headerBuf);

/*!
 *  @brief      API to translate the given pointer to other address type.
 *
 *
 *  @param      handle      Handle to the instance.
 *  @param      dstAddr     Location to receive the destination address
 *                          (translated address).
 *  @param      dstAddrType address type to which the source address needs to be
 *                          translated to.
 *  @param      srcAddr     Source address.
 *  @param      srcAddrType Address type of the source address.
 *  @param      bufType     buffer type. Denotes source address specified is
 *                          frame header or frame buffer.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_FAIL         Invalid buffer type specified in
 *                                           bufType.
 *  @retval     FrameQBufMgr_E_FAIL         Translate failed in SysLinkMemMgr.
 *                                           bufType.
 *  @retval     FrameQBufMgr_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   dstAddr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDARG   srcAddr passed is null.
 *  @retval     FrameQBufMgr_E_INVALIDSTATE Module is not initialized.
 */
Int32
FrameQBufMgr_translateAddr (FrameQBufMgr_Handle            handle,
                            Ptr *                          dstAddr,
                            FrameQBufMgr_AddrType          dstAddrType,
                            Ptr                            srcAddr,
                            FrameQBufMgr_AddrType          srcAddrType,
                            FrameQBufMgr_BufType           bufType);

/*!
 *  @brief  Function to initialize the config parameter structure  with default
 *          values.
 *
 *  @param  cfg
 *          Parameter structure to return the config parameters.
 *
 *  @sa    None.
 */
Int32 FrameQBufMgr_getConfig(FrameQBufMgr_Config *cfg);

/*!
 *  @brief      API to get the ID of the instance.
 *
 *
 *  @param      handle      Handle to the instance.
 *
 *  @retval     id          Id of the instance.
 */
UInt32
FrameQBufMgr_getId (FrameQBufMgr_Handle handle);

/*!
 *  @brief      API to get the handle of the instance.
 *
 *
 *  @param      Id          id of the instance.
 *
 *  @retval     pointer     handle of the instance.
 *  @retval     NULL        getHandle failed.
 */
Ptr
FrameQBufMgr_getHandle (UInt32 Id);

/*!
 * @brief       Function to determine whether cache calls(writeback/invalidate)
 *              are enabled for frame headers.
 *
 * @param       handle    Instance handle.
 *
 *  @retval     TRUE        if cache writeback and invalidate calls are enabled
 *  @retval     FALSE       if cache writeback and invalidate calls are disabled.
 */
Bool
FrameQBufMgr_isCacheEnabledForHeaderBuf(FrameQBufMgr_Handle handle);

/*!
 * @brief       Function to determine whether cache calls(writeback/invalidate)
 *              are enabled for frame headers.
 *
 * @param       handle          Instance handle.
 * @param       framebufIndex   frame buffer index  in frame.
 *
 *  @retval     TRUE        if cache writeback and invalidate calls are enabled
 *  @retval     FALSE       if cache writeback and invalidate calls are disabled.
 */
Bool
FrameQBufMgr_isCacheEnabledForFrameBuf(FrameQBufMgr_Handle handle,
                                       UInt8 framebufIndex);

/*!
 * @brief       Function to get the number of free frames availabel in pool 0.
 *
 * @param       handle          Instance handle.
 * @param       numFreeFrames   Out parameter.Denotes number of free frames
 *                              available in queue 0.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS  Operation successful.
 */
Int32
FrameQBufMgr_getNumFreeFrames (FrameQBufMgr_Handle handle, UInt32* numFreeFrames);

/*!
 * @brief       Provides a hook to perform implementation dependent operation
 *
 * @param       handle    Instance handle.
 * @param       cmd       Command to perform.
 * @param       arg       void * argument.
 *
 *  @retval     FrameQBufMgr_S_SUCCESS      Operation successful.
 *  @retval     FrameQBufMgr_E_FAIL         API failed.
 */
Int32
FrameQBufMgr_control (FrameQBufMgr_Handle  handle,
                      Int32                cmd,
                      Ptr                  arg);

/*!
 *  @brief      Function to calcaulate the base frame header size when number of
 *              frames in a frame is given.
 *
 * @param       numFrameBufs  Number of Frame buffers.
 *
 *  @retval     positive value  size of the base header size.
 */
UInt32 FrameQBufMgr_getBaseHeaderSize(UInt8 numFrameBufs);

/*!
 *  @brief      Function to get the kernel space handle of the instance.
 *
 *              This API is valid only on HLOS side.
 * @param       handle  Handle of the instance.
 *
 *  @retval     pointer Kernel space handle.
 */
/* API that returns the kernel space handle of the instance */
Ptr FrameQBufMgr_getKnlHandle(FrameQBufMgr_Handle handle);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /*FRAMEQBUFMGR_H*/
