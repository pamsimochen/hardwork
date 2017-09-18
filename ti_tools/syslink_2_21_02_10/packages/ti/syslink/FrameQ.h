/** 
 *  @file   FrameQ.h
 *
 *  @brief      Defines for interfaces for FrameQ module.  (Deprecated)
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


#ifndef FRAMEQ_H_0x6e6f
#define FRAMEQ_H_0x6e6f

#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/SysLink.h>
#include <ti/ipc/Notify.h>

#if defined (__cplusplus)
extern "C" {
#endif

/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 *  @brief  Indicates module has been already destroyed.
 */
#define FrameQ_S_ALREADYDESTROYED           2

/*!
 *  @brief  Indicates module is already setup.
 */
#define FrameQ_S_ALREADYSETUP               1

/*!
 *  @brief  Operation successful.
 */
#define FrameQ_S_SUCCESS                    0

/*!
 *  @brief  General Failure
 */
#define FrameQ_E_FAIL                       -1

/*!
 *  @brief  Argument passed to function is invalid.
 */
#define FrameQ_E_INVALIDARG                 -2

/*!
 *  @brief  FrameQ instance  not found.
 */
#define FrameQ_E_NOTFOUND                   -3

/*!
 *  @brief  FrameQ instance  already exists.
 */
#define FrameQ_E_INST_EXISTS                -4

/*!
 *  @brief  Writer client  already exists.
 */
#define FrameQ_E_WRITER_EXISTS              -5

/*!
 *  @brief  Failed to allocate frame header.
 */
#define FrameQ_E_ALLOC_FRAME                -6

/*!
 *  @brief  Failed to allocate frame buffers.
 */
#define FrameQ_E_ALLOC_FRAMEBUF             -7

/*!
 *  @brief  Memory_alloc failed.
 */
#define FrameQ_E_MEMORY                     -8

/*!
 *  @brief  FrameQ is empty. Can not return frame.
 */
#define FrameQ_E_EMPTY                      -9

/*!
 *  @brief  Name server create failed for the module.
 */
#define FrameQ_E_CREATE_NAMESERVER          -10

/*!
 *  @brief  GateMP  instance create failed.
 */
#define FrameQ_E_CREATE_GATEMP              -11

/*!
 *  @brief  GateMP  instance open failed.
 */
#define FrameQ_E_OPEN_GATEMP                -12

/*!
 *  @brief  Failed to create Client NotifyMgr instance.
 */
#define FrameQ_E_FAIL_CLIENTN0TIFYMGR_CREATE -13

/*!
 *  @brief  Failed to open Client NotifyMgr instance.
 */
#define FrameQ_E_FAIL_CLIENTN0TIFYMGR_OPEN  -14

/*!
 *  @brief  Max limit of readers for a FrameQ is reached.
 */
#define FrameQ_E_MAX_READERS                -15

/*!
 *  @brief  Operation can not be permitted or not implemented.
 */
#define FrameQ_E_ACCESSDENIED               -16

/*!
 *  @brief  Module is not initialized.
 */
#define FrameQ_E_INVALIDSTATE               -17

/*!
 *  @brief  Failure in OS call.
 */
#define FrameQ_E_OSFAILURE                  -18

/*!
 *  @brief  Adding  FrameQ entry to name server instance failed.
 */
#define FrameQ_E_FAIL_NAMESERVERADD         -19

/*!
 *  @brief  Unsupported interface type.
 */
#define FrameQ_E_INVALID_INTERFACE          -20

/*!
 *  @brief  Called API is not implemented.
 */
#define FrameQ_E_NOTIMPLEMENTED             -21

/*!
 *  @brief  NameServer open failed.
 */
#define FrameQ_E_OPEN_NAMESERVER            -22

/*!
 *  @brief  Invalid FrameQbufMgr id.
 */
#define FrameQ_E_INVALID_FRAMEQBUFMGRID     -23

/*!
 *  @brief  Reader  client get this error in getv call if it is not able to get
 *          all the requested frames.  API is able to get only few frames.
 */
#define FrameQ_E_INSUFFICENT_FRAMES         -24

/*!
 *  @brief  Failed to allocate memory.
 */
#define FrameQ_E_ALLOC_MEMORY               -25

/*!
 *  @brief  Failed to open FrameQBufMgr.
 */
#define FrameQ_E_FRAMEQBUFMGROPEN            -26

/*!
 *  @brief  Failed to open ListMP instance.
 */
#define FrameQ_E_LISTMPOPEN                  -27

/*!
 *  @brief  Failed to register with the clientNotifyMgr.
 */
#define FrameQ_E_CLIENTNOTIFYMGRREGCLIENT       -28

/*!
 *  @brief  Failed to register with the FrameQBufMgr.
 */
#define FrameQ_E_FRAMEQBUFMGRREGCLIENT          -29

/*!
 *  @brief  Failed to unregister with the clientNotifyMgr.
 */
#define FrameQ_E_CLIENTNOTIFYMGRUNREGCLIENT     -30

/*!
 *  @brief  Failed to unregister with the FrameQBufMgr.
 */
#define FrameQ_E_FRAMEQBUFMGRUNREGCLIENT        -31

/*!
 *  @brief   Call back function is already registered.
 */
#define FrameQ_E_ALREADYREGISTERED             -32

/*!
 *  @brief  NameServer_add failed.
 */
#define FrameQ_E_NAMESERVERADD                 -33

/* =============================================================================
 * macros & defines
 * =============================================================================
 */
/*!
 *  @brief  Defines the frame buffer information
 */
#define FrameQ_FrameBufInfo       Frame_FrameBufInfo

/*!
 *  @brief  Defines the frame header structure
 */
#define FrameQ_FrameHeader        Frame_FrameHeader

/*!
 *  @brief  Defines the type for a frame pointer
 */
typedef Frame_FrameHeader *       FrameQ_Frame;

/*!
 *  @brief  Maximum length of the name string in bytes.
 */
#define FrameQ_MAX_NAMELEN                 (32u)

/*!
 *  @brief  Max filled queues for a reader client of the FrameQ instance.
 */
#define FrameQ_MAXFILLEDQUEUS_FOR_READER  (16)

/*!
 *  @brief  Maximum number of reader clients supported by the instance.
 */
#define FrameQ_MAX_INST_READERS           (2u)


/*!
 *  @brief  Maximum number of instances managed by FrameQ.
 */
#define FrameQ_MAX_INSTANCES               (64u)

/*!
 *  @brief  Notify Event Number to be used by FrameQ module.
 */
#define FrameQ_NOTIFY_RESERVED_EVENTNO    (1u)

/*!
 *  @brief   This type is used for the notification message. This is a 16 bit
 *           payload which can be sent to the remote processor in a notification
 *           call.
 */
typedef UInt16                    FrameQ_NotifyMsg;

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Handle for the FrameQ instance.
 */
typedef struct FrameQ_Object * FrameQ_Handle;

/*!
 *  @brief  Prototype of the FrameQ call back function.
 */
typedef void (* FrameQ_NotifyFunc) (FrameQ_Handle, Ptr arg, FrameQ_NotifyMsg);

/*!
 *  @brief   Notification types for FrameQ.
 */
typedef enum FrameQ_NotifyType_Tag {
    FrameQ_NOTIFICATION_NONE   = SysLink_NOTIFICATION_NONE,
    /*!< No notification is required. */
    FrameQ_NOTIFICATION_ALWAYS = SysLink_NOTIFICATION_ALWAYS,
    /*!< Notify whenever the other client sends data/frees up space.*/
    FrameQ_NOTIFICATION_ONCE   = SysLink_NOTIFICATION_ONCE,
    /*!< Notify when the other side sends data/frees up space. Once the
     *   notification is done, the notification is disabled until it is
     *   enabled again.
     */
    FrameQ_NOTIFICATION_HDWRFIFO_ALWAYS = SysLink_NOTIFICATION_HDWRFIFO_ALWAYS,
    /*!< Notify whenever the other side sends data/frees up space.
     *   This notification is never disabled.
     */
    FrameQ_NOTIFICATION_HDWRFIFO_ONCE = SysLink_NOTIFICATION_HDWRFIFO_ONCE
    /*!< Notify when the other side sends data/frees up space. Once the
     *   notification is done, the notification is disabled until it is
     *   enabled again. The notification is enabled once the watermark
     *   is crossed and does not require buffer to get full/empty.
     */
} FrameQ_NotifyType;

/*!
 *  @brief  FrameQ open modes.
 */
typedef enum FrameQ_OpenMode_Tag {
    FrameQ_MODE_NONE,
    /*!< Mode is not reader/writer.Apps should not use this  as open mode */
    FrameQ_MODE_WRITER,
    /*!< FrameQ mode is writer. */
    FrameQ_MODE_READER
    /*!< FrameQ mode is reader.*/
} FrameQ_OpenMode;

/*!
 *  @brief  CPU access flags for frame buffers. For frame headers
 *          CPUACCESS is assumed to be TRUE.
 */
typedef enum FrameQ_CpuAccessFlags_Tag {
        FrameQ_FRAMEBUF0_CPUACCESS    =  0x10000,
        /*!< If specified first frame buffer is accessed  through CPU
         *  (direct memory access).
         */
        FrameQ_FRAMEBUF1_CPUACCESS    =  0x20000,
        /*!< If specified second frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQ_FRAMEBUF2_CPUACCESS    =  0x40000,
        /*!< If specified third frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQ_FRAMEBUF3_CPUACCESS    =  0x80000,
        /*!< If specified fourth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQ_FRAMEBUF4_CPUACCESS    = 0x100000,
        /*!< If specified fifth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQ_FRAMEBUF5_CPUACCESS    = 0x200000,
        /*!< If specified sixth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQ_FRAMEBUF6_CPUACCESS    = 0x400000,
        /*!< If specified seventh frame buffer is accessed  through CPU
         *   (direct memory access).
         */
        FrameQ_FRAMEBUF7_CPUACCESS    = 0x800000
        /*!< If specified eighth frame buffer is accessed  through CPU
         *   (direct memory access).
         */
} FrameQ_CpuAccessFlags;

/*!
 *  @brief  FrameQ interface types.
 */
typedef enum FrameQ_Interface_Tag {
    FrameQ_INTERFACE_SHAREDMEM = 0x0,
    /*!< Denotes FrameQ implementation (FrameQ_ShMem
     * Implementation) on shared memory.
     */
    FrameQ_INTERFACE_NONE      = 0x1
} FrameQ_Interface;

/*!
 *  @brief  Structure defining config parameters for the FrameQ module.
 */
typedef struct FrameQ_Config_Tag {
    UInt32 eventNo;
    /*!< Notify event number used for client notifications */

    UInt8  maxInstances;
    /*!< Maximum instances */

    UInt32  usedefaultgate;
    /*!<  If set to TRUE, FrameQ instances will use dafault gate set in
     *  FrameQ.
     *  If default gate is not set in FrameQ, it uses GateMP's default
     *  gate. Set it to zero if FrameQ needs to create gate for a
     *  instance (remoteProtect and localProtect flags should be valid in
     *  params in this case).
     */
} FrameQ_Config;

/*!
 *  @brief  Common parameters required to create FrameQ instance of
 *          any implementation. It should be the first element of the actual
 *          implementaton params structure.
 */
typedef struct FrameQ_CreateParams_Tag {
    UInt32                  size;
    /*!< Size of the paramstructure */
    FrameQ_Interface        ctrlInterfaceType;
    /*!< Interface type */
    String                  name;
    /*!< Name of the instance */
    Bool                    openFlag;
    /* Open flag to indicate create/open. internal flag.  Caller should not
     * touch this flag during create
     */
} FrameQ_CreateParams;

/*!
 *  @brief  Common parameters required to open a FrameQ instance of
 *          any implementation.
 */
typedef struct FrameQ_CommonOpenParams_Tag {
    String       name;
    /*!< Name of the instance to open */
    Ptr         sharedAddr;
    /*!<  Virtual shared Region addr in case open by address is required.*/
    UInt32      openMode;
    /*!< Mode of the client. */
    UInt32      cpuAccessFlags;
    /*!< cpuAccessFlags flags to know if app is going to access frame buffers
     * through CPU.
     */
} FrameQ_CommonOpenParams;

/*!
 *  @brief  Common parameters required to create FrameQ instance of
 *          any implementation. It should be the first element of the actual
 *          implementaton params structure .
 */
typedef struct FrameQ_Params {
    FrameQ_CreateParams   commonCreateParams;
    Ptr impParams;
    /*!< Pointer to implementation specific parameters */

    UInt32  impParamsSize;
    /*!< Size of the implementation specific structure pointed by impParams. */
} FrameQ_Params;

/*!
 *  @brief  Notification parameter structure.
 */
typedef struct FrameQ_NotifyParams_Tag {
    FrameQ_NotifyType      notifyType;
    /*!< Notification type*/
    UInt32                  watermark;
    /*!< Minumum number of free frames required to generate notification
     *   Same water mark for all the individual freeFramePools in the set.
     */
    FrameQ_NotifyFunc       cbFxnPtr;
    /*!< Cacll back function*/
    Ptr                     cbContext;
    /*!< Context pointer that needs to be passed to call back function.*/
    Ptr                     cbClientHandle;
    /*!< Handle that needs to be passed as first arg to call back.
     *   Ideally this should be callers client handle.*/
} FrameQ_NotifyParams;

/* =============================================================================
 * APIs
 * =============================================================================
 */
/*!
 *  @brief      Determine whether the given interface type is supported
 *
 *              Returns TRUE if the interface provide is supported,
 *              otherwise returns FALSE.
 *
 *  @param[in]  type        Type of interface.  See FrameQ_Interface for details.
 *
 *  @retval     TRUE        Given interface is supported.
 *  @retval     FALSE       Given interface is no supported.
 *
 *  @sa         FrameQ_Interface
 */
Int32 FrameQ_isSupportedInterface(UInt32 type);

/*!
 *  @brief      Create a FrameQ instance
 *
 *              Create a new instance of FrameQ. It creates a
 *              frameQ based on the params specific to implementations.
 *
 *  @param[in]  params      pointer to implementation specific params.
 *
 *  @retval     Handle      Instance handle
 *  @retval     NULL        Create failed
 *
 *  @sa         FrameQ_ShMem_Params
 *  @sa         FrameQ_delete()
 *  @sa         FrameQ_open()
 *  @sa         FrameQ_close()
 */
FrameQ_Handle FrameQ_create(Ptr params);

/*!
 *  @brief      Delete a FrameQ instance
 *
 *              Deletes FrameQ instance that created.
 *
 *  @param      pHandle     Pointer to the created frameQ instance handle.
 *
 *  @retval     FrameQ_S_SUCCESS     Delete successful.
 *  @retval     FrameQ_E_FAIL        Delete failed.
 *  @retval     FrameQ_E_INVALIDARG  Pointer to Handle passed is null.
 *  @retval     FrameQ_E_INVALIDARG  Handle passed is null.
 *
 *  @sa         FrameQ_create()
 *  @sa         FrameQ_open()
 *  @sa         FrameQ_close()
 */
Int32 FrameQ_delete(FrameQ_Handle *pHandle);

/*!
 *  @brief      Open the created FrameQ instance.
 *
 *              Opens the FrameQ instance in reader or writer
 *              mode depending upon the open params passed to it.
 *
 *  @param      openParams  Pointer to implementation specific open params.
 *  @param      handlePtr   Return parameter.Instance opened in given mode.
 *
 *  @retval     FrameQ_S_SUCCESS      open successful.
 *  @retval     FrameQ_E_INVALIDARG   Invalid parameter specified.
 *  @retval     FrameQ_E_INVALIDSTATE Module is not initialized.
 *  @retval     FrameQ_E_MEMORY       Memory_alloc failed.
 *  @retval     FrameQ_E_ALLOC_MEMORY Memory_alloc failed.
 *  @retval     FrameQ_E_FRAMEQBUFMGROPEN FrameQBufMge instance open failed.
 *  @retval     FrameQ_E_OPEN_GATEMP  GateMP open failed.
 *  @retval     FrameQ_E_FAIL_CLIENTN0TIFYMGR_OPEN ClientNotifyMgr open failed.
 *  @retval     FrameQ_E_LISTMPOPEN   Internal ListMP instance open failed.
 *  @retval     FrameQ_E_NOTFOUND     Instance not found in FrameQ Nameserver.
 *
 *  @sa         FrameQ_ShMem_OpenParams
 *  @sa         FrameQ_create()
 *  @sa         FrameQ_delete()
 *  @sa         FrameQ_close()
 */
    Int32 FrameQ_open(Ptr openParams, FrameQ_Handle *handlePtr);

/*!
 *  @brief      Open the created FrameQ instance if shared address is known.
 *
 *              This function opens the FrameQ instance in reader or writer
 *              mode depending upon the open params passed to it. Apps should
 *              pass the valid shared addr of the created instance.
 *
 *  @param      openParams  Pointer to implementation specific open params.
 *  @param      handlePtr   Return parameter.Instance opened in given mode.
 *
 *  @retval     FrameQ_S_SUCCESS      open successful.
 *  @retval     FrameQ_E_INVALIDARG   Invalid parameter specified.
 *  @retval     FrameQ_E_INVALIDSTATE Module is not initialized.
 *  @retval     FrameQ_E_MEMORY       Memory_alloc failed.
 *  @retval     FrameQ_E_ALLOC_MEMORY Memory_alloc failed.
 *  @retval     FrameQ_E_FRAMEQBUFMGROPEN FrameQBufMge instance open failed.
 *  @retval     FrameQ_E_OPEN_GATEMP  GateMP open failed.
 *  @retval     FrameQ_E_FAIL_CLIENTN0TIFYMGR_OPEN ClientNotifyMgr open failed.
 *  @retval     FrameQ_E_LISTMPOPEN   Internal ListMP instance open failed.
 *  @retval     FrameQ_E_NOTFOUND     Instance not found in FrameQ Nameserver.
 *
 *  @sa         FrameQ_ShMem_OpenParams
 *  @sa         FrameQ_create()
 *  @sa         FrameQ_delete()
 *  @sa         FrameQ_close()
 */
Int32 FrameQ_openByAddr(FrameQ_Handle *handlePtr, Ptr openParams);

/*!
 *  @brief      Close the opened FrameQ instance.
 *
 *              Closes FrameQ instance that is opened.
 *
 *  @param      pHandle     Pointer to the opened frameQ instance handle.
 *
 *  @retval     FrameQ_S_SUCCESS     Close successful
 *  @retval     FrameQ_E_FAIL        Close failed
 *  @retval     FrameQ_E_INVALIDARG  @c pHandle is invalid
 *
 *  @sa         FrameQ_create()
 *  @sa         FrameQ_open()
 *  @sa         FrameQ_delete()
 */
Int32 FrameQ_close(FrameQ_Handle *pHandle);

/*!
 *  @brief      Allocate frame.
 *
 *              Allocates a frame when called by FrameQ writer.
 *
 *  @remark     This API internally makes a call to configured FrameQBufMgr to
 *              allocate a free frame.
 *
 *  @param      handle      FrameQ writer handle.
 *  @param      framePtr    Location to receive the allocated frame.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully allocated frame.
 *  @retval     FrameQ_E_FAIL         Internal FrameQBufMgr_alloc failed.
 *  @retval     FrameQ_E_INVALIDARG   framePtr paased is null.
 *  @retval     FrameQ_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not allocate frames.
 *                                    Only writer can allocate frames.
 *
 *  @sa         FrameQ_open()
 *  @sa         FrameQ_free()
 *  @sa         FrameQ_put()
 *  @sa         FrameQ_get()
 */
Int32 FrameQ_alloc(FrameQ_Handle handle, FrameQ_Frame *framePtr);

/*!
 *  @brief      Allocate multiple frames.
 *
 *              Allocates frames from multiple free pools of
 *              plugged in FrameQBufMgr. After API returns, numframes will
 *              denotes the number of successfully allocated frames.The max size
 *              of @c framePtr and @c freeQId array is defined by
 *              #FrameQ_MAX_FRAMESINVAPI.
 *
 *  @param      handle      FrameQ writer handle.
 *  @param      framePtr    Array to receive pointers to allocated frames.
 *  @param      freeQId     Array of free frame pool nos of the plugged in
 *                          FrameQBufMgrfrom from which this API needs to
 *                          allocate frames actual size of that is allocated.
 *  @param[in,out]      numFrames   Number of frames that needs to be allocated.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully allocated frames.
 *  @retval     FrameQ_E_FAIL         Internal FrameQBufMgr_allocv failed.
 *  @retval     FrameQ_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQ_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQ_E_INVALIDARG   freeQId passed is null.
 *  @retval     FrameQ_E_INVALIDARG   numFrames passed is null.
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not allocate frames.
 *                                    Only writer can allocate frames.
 *                                    Only writer can allocate frames.
 *
 *  @sa         FrameQ_open()
 *  @sa         FrameQ_freev()
 *  @sa         FrameQ_putv()
 *  @sa         FrameQ_getv()
 */
Int32 FrameQ_allocv(FrameQ_Handle handle, FrameQ_Frame framePtr[],
        UInt32 freeQId[], UInt8 *numFrames);

/*!
 *  @brief      Free frame.
 *
 *              Frees a frame when called by FrameQ writer or
 *              reader.  This API internally makes a call to configured
 *              FrameQBufMgr to free the frame.
 *
 *  @param      handle      Opened FrameQ instance handle
 *  @param      frame       Frame to be freed
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully freed frame
 *  @retval     FrameQ_E_FAIL         Internal FrameQBufMgr_free failed
 *  @retval     FrameQ_E_INVALIDARG   @c handle or @c frame is invalid
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not free frames.
 *                                    Only writer or reader can free frames.
 *
 *  @sa         FrameQ_open()
 *  @sa         FrameQ_alloc()
 *  @sa         FrameQ_put()
 *  @sa         FrameQ_get()
 */
Int32 FrameQ_free(FrameQ_Handle handle, FrameQ_Frame frame);

/*!
 *  @brief      Free multiple frames.
 *
 *              Frees multiple frames when called by
 *              FrameQ writer or writer.
 *
 *  @remark     This API internally makes a call to configured FrameQBufMgr to
 *              free frames.
 *
 *  @param      handle      Opened FrameQ instance handle.
 *  @param      framePtr    Array of frames to be freed.
 *  @param      numFrames   Number of Frames to be freed.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully freed frames.
 *  @retval     FrameQ_E_FAIL         Internal FrameQBufMgr_freev failed.
 *  @retval     FrameQ_E_INVALIDARG   @c handle or @c framePtr is invalid
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not free frames.
 *                                    Only writer or reader can free frames.
 *
 *  @sa         FrameQ_open()
 *  @sa         FrameQ_allocv()
 *  @sa         FrameQ_putv()
 *  @sa         FrameQ_getv()
 */
Int32 FrameQ_freev(FrameQ_Handle handle, FrameQ_Frame framePtr[],
        UInt32 numFrames);

/*!
 *  @brief      Insert frame in to FrameQ instance.
 *
 *              This function inserts frame in to FrameQ. It inserts frame in to
 *              filled frame queue 0 of FrameQ reader clients.
 *              If multiple readers exist,
 *              1.FrameQ inserts the original gien frame into first reader's
 *                 queue 0.
 *              2.FrameQ duplicates the original frame and inserts duplicated
 *                frames in to rest of the reader's queue 0.
 *
 *  @param      handle      Opened FrameQ instance handle.
 *  @param      frame       Frame to be inserted.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully freed frames.
 *  @retval     FrameQ_E_FAIL         Failed to put frame.
 *  @retval     FrameQ_E_INVALIDARG   frame passed is null.
 *  @retval     FrameQ_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not free frames.
 *                                    Only writer or reader can put frames.
 *
 *  @sa         FrameQ_get()
 */
Int32 FrameQ_put(FrameQ_Handle handle, FrameQ_Frame frame);

/*!
 *  @brief      Insert multiple frames into queues of a FrameQ reader client.
 *
 *              If multiple readers exist, It dups the given
 *              frames and inserts in to other readers' queues. The size of
 *              @c framePtr and @c filledQueueId must be at least numFrames.
 *              The max size of @c framePtr and @c filledQueueId is defined by
 *              #FrameQ_MAX_FRAMESINVAPI.
 *
 *  @param      handle       Instance handle.
 *  @param      framePtr     Array of frames to be inserted.
 *  @param      filledQueueId    Array of filled queues of reader.
 *  @param      numFrames    Number of frames.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully inserted frames.
 *  @retval     FrameQ_E_FAIL         Failed to put frames.
 *  @retval     FrameQ_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQ_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQ_E_INVALIDARG   filledQueueId passed is null.
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not put frames.
 *                                    Only writer or reader can put frames.
 *
 *  @sa         FrameQ_getv()
 */
Int32 FrameQ_putv(FrameQ_Handle handle, FrameQ_Frame framePtr[],
        UInt32 filledQueueId[], UInt8 numFrames);

/*!
 *  @brief      Retrieve frame from FrameQ instance by reader client.
 *
 *              Retrieves frame from the queue 0 of caller client.
 *              Only readers are allowed to call this API.
 *
 *  @param      handle       Instance handle.
 *  @param      framePtr     Location to receive frame.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully retrieved frame.
 *  @retval     FrameQ_E_FAIL         Failed to get frame.
 *  @retval     FrameQ_E_INVALIDARG   handle passed is null.
 *  @retval     FrameQ_E_INVALIDARG   framePtr passed is null.
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not retrieve frame.
 *                                    Only reader client can retrieve frame.
 *
 *  @sa         FrameQ_put()
 */
Int32 FrameQ_get(FrameQ_Handle handle, FrameQ_Frame *framePtr);

/*!
 *  @brief      Retrieve frames from the given queues of a reader client.
 *
 *              This Function retrieves frames from  the queues of caller client.
 *              Only readers are allowed to call this API. It returns frames if
 *              all the required frames are available in the FrameQ.
 *
 *  @remarks    The size of @c pframe and @c filledQueueId must be at least
 *              @c numFrames.
 *
 *  @remarks    The max size of @c framePtr and @c filledQueueId is defined by
 *              #FrameQ_MAX_FRAMESINVAPI.
 *
 *  @param      handle         Instance handle.
 *  @param      pframe         Location to receive frames the queues specified
 *                             in filledQueueId array argument.
 *  @param      filledQueueId  Array of queue ids  from which  frames.
 *  @param      numFrames      Denotes number of frames that needs to retrieved
 *                             from the filled Queues ids  specified by
 *                             filledQueueId. The Size of pframe and
 *                             filledQueueId should be atleast  equal to number
 *                             of frames specified in numFrames value can be max
 *                             FrameQ_MAX_FRAMESINVAPI.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully retrieved frames.
 *  @retval     FrameQ_E_FAIL         Failed to get frames.
 *  @retval     FrameQ_E_INVALIDARG   Invalid arguments
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not retrieve frames.
 *                                    Only reader client can retrieve frames.
 *
 *  @sa         FrameQ_putv()
 */
Int32 FrameQ_getv(FrameQ_Handle handle, FrameQ_Frame pframe[],
        UInt32 filledQueueId[], UInt8 *numFrames);

/*!
 *  @brief      Duplicate the given frame.
 *
 *              Duplicates the given frame.  It allocates one frame
 *              header buffer and copies the header information given in
 *              original frame and returns it.
 *
 *  @param      handle         Instance handle.
 *  @param      frame          frame to be duplicated.
 *  @param      dupedFrame     Location to receive the duplicated frame.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully retrieved frames
 *  @retval     FrameQ_E_FAIL         Failed to get frames
 *  @retval     FrameQ_E_INVALIDARG   Invalid argument
 *  @retval     FrameQ_E_ACCESSDENIED Provided handle can not dup frames.
 *                                    Only  writer/reader client can dup frame.
 *
 *  @sa         FrameQ_alloc()
 */
Int32 FrameQ_dup(FrameQ_Handle handle, FrameQ_Frame frame,
        FrameQ_Frame *dupedFrame);

/*!
 * @brief       Register a call back function.
 *
 *              For reader :
 *              It registers notification function to notify about available
 *              frames in the FrameQ.
 *              For writer :
 *              It registers notification function to notify about available
 *              free frames in the Plugged in FrameQBufMgr.
 *
 *  @param      handle          Instance handle.
 *  @param      notifyParams    Pointer to notifyParams.
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_INVALIDARG   Invalid Argument
 *  @retval     FrameQ_E_INVALIDSTATE Module is not initialized
 *  @retval     FrameQ_E_FAIL         Failed to register notification
 *
 *  @sa         FrameQ_unregisterNotifier()
 */
Int32 FrameQ_registerNotifier(FrameQ_Handle handle,
        FrameQ_NotifyParams *notifyParams);

/*!
 *  @brief      Unregister call back function.
 *
 *
 *  @param      handle      Reader/writer client Handle.
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully duped frame.
 *  @retval     FrameQ_E_INVALIDARG   Invalid argument
 *  @retval     FrameQ_E_INVALIDSTATE Module is not initialized.
 *  @retval     FrameQ_E_FAIL         Failed to register notification.
 *
 *  @sa         FrameQ_registerNotifier()
 */
Int32 FrameQ_unregisterNotifier(FrameQ_Handle handle);

/*!
 *  @brief      Send forced notification to the reader clients.
 *
 *
 *  @param      handle      Writer client Handle
 *  @param      msg         Payload that needs to be send to reader
 *
 *  @retval     FrameQ_S_SUCCESS      Successfully sent notification
 *  @retval     FrameQ_E_INVALIDARG   Invalid argument
 *  @retval     FrameQ_E_INVALIDSTATE Module is not initialized
 *  @retval     FrameQ_E_FAIL         Failed to send notification
 *  @retval     FrameQ_E_ACCESSDENIED  handle is not a writer handle.can not send
 *                                    notification.
 *
 *  @sa         FrameQ_registerNotifier()
 */
Int32 FrameQ_sendNotify(FrameQ_Handle handle, UInt16 msg);

/*!
 *  @brief       Function to find out the number of available frames in a FrameQ .
 *
 *              For Writer:
 *              It returns the number of frames in the primary reader's filled
 *              queue 0.
 *              For Reader:
 *              It returns the number of frames in the Reader's(caller)filledqueue 0.
 *
 *  @param handle      Reader client Handle.
 *  @param numFrames   Location to recieve the number of frames.
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_FAIL         API failed.
 *
 *  @sa         FrameQ_getvNumFrames()
 */
Int32 FrameQ_getNumFrames(FrameQ_Handle handle, UInt32 *numFrames);

/*!
 *  @brief      Find out the number of available frames in a FrameQ
 *
 *              For Reader this function is to get the available frames from
 *              the queues associated with it.
 *              For writer this function is to get the available frames from
 *              the queues associated with the primary Reader(first reader).
 *
 *  @param      handle          Writer/Reader client handle.
 *  @param      numFrames       Location to recieve the number of frames.
 *  @param      filledQId       Array of filled queues.
 *  @param      numFilledQids   Number of queues specified in filledQId array.
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_FAIL         API failed.
 *
 *  @sa         FrameQ_getNumFrames()
 */
Int32 FrameQ_getvNumFrames(FrameQ_Handle handle, UInt32 numFrames[],
        UInt8 filledQId[], UInt8 numFilledQids);

/*!
 *  @brief      Get the number of free frames.
 *
 *              Function to get the number of free frames available in the
 *              plugged in FrameQbufMgr's FreeFramePool 0. The returned number
 *              free frames may not exist in the frameQBufMgr after this call.
 *              This is because if allocs can be done by the other FrameQ
 *              writers on the same FrameQBufMgr. If frames are freed by other frameQ
 *              writers then the free Frames would be even more than the value that
 *              this API returns.
 *
 *  @param      handle          Instance handle.
 *  @param      numFreeFrames   out parameter that denotes the number of free
 *                              frames available
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_FAIL         API failed.
 *
 *  @sa         FrameQ_getvNumFreeFrames()
 */
Int32 FrameQ_getNumFreeFrames(FrameQ_Handle handle, UInt32 *numFreeFrames);

/*!
 *  @brief      Function to get the number of free frames.
 *
 *              Function to get the number of free frames available in the
 *              plugged in FrameQbufMgr's FreeFramePools. The returned number
 *              free frames may not exist in the frameQBufMgr after this call.
 *              This is because if  allocs can be done by the other FrameQ
 *              writers on the same FrameQBufMgr.If frames are freed by other frameQ
 *              writers then the free Frames would be even more than the value that
 *              this API returns.
 *
 *  @param      handle          Instance handle.
 *  @param      numFreeFrames   out parameter that denotes the number of free
 *                              frames available
 *  @param      freeQId         Array of free queues.
 *  @param      numFreeQids     Number of queues specified in freeQId array.
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_FAIL         API failed.
 *
 *  @sa         FrameQ_getNumFreeFrames()
 */
Int32 FrameQ_getvNumFreeFrames(FrameQ_Handle handle, UInt32 numFreeFrames[],
        UInt8 freeQId[], UInt8 numFreeQids);

/*!
 * @brief       Provides a hook to perform implementation dependent operation
 *
 * @param       handle    Instance handle.
 * @param       cmd       Command to perform.
 * @param       arg       void * argument.
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_FAIL         API failed.
 */
Int32 FrameQ_control(FrameQ_Handle handle, Int32 cmd, Ptr arg);

/*!
 *  @brief      Get the pointer to the extended header.
 *
 *  @param      frame   pointer to frame
 *
 *  @retval     pointer Pointer to extended header pointer in frame header.
 */
Ptr FrameQ_getExtendedHeaderPtr(FrameQ_Frame frame);


/*=============================================================================
 * Helper APIs
 *=============================================================================
 */
/*!
 *  @brief      Determine the number of frame buffers in a frame.
 *
 *  @param      frame   pointer to frame
 *
 *  @retval     number of frame buffers.
 */
UInt32 FrameQ_getNumFrameBuffers(FrameQ_Frame frame);

/*!
 *  @brief      Get the framebuffer.
 *
 *  @param      frame   pointer to frame
 *  @param      frameBufNum   frame buffer index in the given frame
 *
 *  @retval     Pointer to frame buffer.
 */
Ptr FrameQ_getFrameBuffer(FrameQ_Frame frame, UInt32 frameBufNum);

/*!
 *  @brief      Determine the size of frame buffer
 *
 *  @param      frame   pointer to frame
 *  @param      frameBufNum   frame buffer index in the given frame
 *
 *  @retval     size  frame buffer size.
 */
UInt32 FrameQ_getFrameBufSize(FrameQ_Frame frame, UInt32 frameBufNum);

/*!
 *  @brief      Get valid data size of a frame buffer.
 *
 *  @param      frame   pointer to frame
 *  @param      frameBufNum   frame buffer index in the given frame
 *
 *  @retval     size     valid size of the frame buffer.
 */
UInt32 FrameQ_getFrameBufValidSize(FrameQ_Frame frame, UInt32 frameBufNum);

/*!
 *  @brief      Get the valid data start offset in framebuffer
 *              identified by frameBufNum in a given frame
 *
 *  @param      frame           pointer to frame
 *  @param      frameBufNum     frame buffer index in the given frame
 *
 *  @retval     offset  valid data offset in the frame buffer.
 */
UInt32 FrameQ_getFrameBufDataStartOffset(FrameQ_Frame frame,
        UInt32 frameBufNum);

/*!
 *  @brief      Set the valid data size of a framebuffer identified by
 *              frameBufNum in given frame.
 *
 *  @param      frame           pointer to frame
 *  @param      frameBufNum     frame buffer index in the given frame
 *  @param      validDataSize   size of the valid data
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_FAIL         API failed.
 */
Int32 FrameQ_setFrameBufValidSize(FrameQ_Frame frame, UInt32 frameBufNum,
        UInt32 validDataSize);

/*!
 *  @brief      Set the valid data start offset of a framebuffer
 *              identified by frameBufNum in a given frame.
 *
 *  @param      frame           pointer to frame
 *  @param      frameBufNum     frame buffer index in the given frame
 *  @param      dataStartOffset valid data startoffset that needs to set for the
 *                              frame buffer.
 *
 *  @retval     FrameQ_S_SUCCESS      Operation successful.
 *  @retval     FrameQ_E_FAIL         API failed.
 */
Int32 FrameQ_setFrameBufDataStartOffset(FrameQ_Frame frame,
        UInt32 frameBufNum, UInt32 dataStartOffset);


/*!
 *  @brief      Get the default configuration for the FrameQ module.
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
 *  @sa         FrameQ_setup()
 */
Void FrameQ_getConfig(FrameQ_Config *cfg);


/*!
 *  @brief      Setup the FrameQ module
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
 *  @sa         FrameQ_destroy()
 *  @sa         FrameQ_getConfig()
 */
Int32 FrameQ_setup(FrameQ_Config *cfg);


/*!
 *  @brief      Finalize the FrameQ module
 *
 *  @sa         FrameQ_setup()
 */
Int32 FrameQ_destroy(Void);


#if defined (__cplusplus)
}
#endif


#endif /*FRAMEQ_H_0x6e6f*/
