/** 
 *  @file   RingIO.h
 *
 *  @brief      RingIO module
 *
 *              The RingIO component provides data streaming between cores
 *              using a ring buffer as the transport.
 *
 *              In a multiprocessor system having shared access to a memory
 *              region, an efficient mode of data transfer can be implemented,
 *              which uses a ring buffer created within the shared memory.
 *              The reader and writer of the ring buffer can be on different
 *              processors.
 *
 *              The RingIO component on each processor shall provide the ability
 *              to create RingIO buffers within the memory provided by the
 *              application. The memory provided may be within a shared
 *              memory region between two processors. The RingIO buffer is
 *              accessible to reader and writer present on the two processors
 *              between which the memory is shared. The application can obtain
 *              a handle to the RingIO through a call to open it.
 *
 *              The RingIO component shall provide the ability for the writer to
 *              acquire empty regions of memory within the data buffer. The
 *              contents of the acquired region are committed to memory when the
 *              data buffer is released by the writer.
 *
 *              The RingIO component shall provide the ability for the reader to
 *              acquire regions of memory within the data buffer with valid data
 *              within them. On releasing the acquired region, the contents of
 *              this region are marked as invalid.
 *
 *              The RingIO component shall also support APIs for enabling
 *              synchronous transfer of attributes with data. End of Stream
 *              (EOS), Time Stamps, Stream offset etc. are examples of such
 *              attributes and these shall be associated with offsets in the
 *              ring buffer.
 *
 *              A client using RingIO is a single unit of execution. It is
 *              typically a thread or task on a given processor.
 *
 *                - The RingIO instance can be created between a client on the
 *                  host CPU and a client on a slave CPU or between two clients
 *                  on slave CPUs.
 *
 *                - Either the reader or writer can create or delete the RingIO
 *                  instance.
 *
 *                - The RingIO instance should be created in a shared memory
 *                  region which can be accessed directly by both the reader
 *                  and the writer.
 *
 *                - Both the reader and the writer need to open the RingIO
 *                  instance and get a handle. Any data access on the RingIO
 *                  instance should be made using these handles.
 *
 *                - Each RingIO can have a single writer client and a single
 *                  reader client. A RingIO handle may not be shared between
 *                  multiple threads.
 *
 *              The RingIO client can be closed only if there is no currently
 *              acquired data or attributes. If there is any unreleased data or
 *              attributes, they must be released or cancelled before the RingIO
 *              client can be closed.
 *
 *              The RingIO can be deleted only when both reader and writer
 *              clients have successfully closed their RingIO clients.
 *
 *              Each RingIO instance may have an associated footer area, if
 *              configured. The foot buffer can be configured to be of zero size
 *              if not required. If configured to be non-zero size, the
 *              foot-buffer is physically contiguous with the data buffer.
 *
 *              The RingIO data and attribute buffer sizes must comply with
 *              any constraints imposed by the heap that they are specified to
 *              be allocated from. For example, for a Shared Memory Heap,
 *              the buffer sizes are automatically aligned to the
 *              SharedRegion's cache line size.
 *
 *  @par Example:
 *
 *  @code
 *  // Setup SysLink
 *  SysLink_setup();
 *
 *  // Create the RingIO
 *  RingIOShm_Params_init(&rioParams);
 *  rioParams.commonParams.name     = "My RingIO";
 *  rioParams.ctrlRegionId          = 1;
 *  rioParams.dataRegionId          = 1;
 *  rioParams.attrRegionId          = 1;
 *  rioParams.attrSharedAddrSize    = 16;
 *  rioParams.dataSharedAddrSize    = 40;
 *  rioParams.remoteProcId          = MultiProc_getId("DSP");
 *  rioParams.gateHandle            = NULL;
 *
 *  rio_Handle = RingIO_create(&rioParams);
 *
 *  // Open the RingIO
 *  rio_OpenParams.openMode         = RingIO_MODE_WRITER;
 *  rio_OpenParams.flags            = 0;
 *  status = RingIO_open(GPP_RINGIO_NAME, &rio_OpenParams, NULL,
 *          &rio_WriterHandle);
 *
 *  // Write to RingIO
 *  status = RingIO_acquire(rio_WriterHandle, (RingIO_BufPtr*)&buffer,
 *          &acquiredSize);
 *
 *  // (Can put some data into the buffer here)
 *
 *  // Release the RingIO
 *  status = RingIO_release(rio_WriterHandle, acquiredSize);
 *
 *  // Close the RingIO
 *  status = RingIO_close(&rio_WriterHandle);
 *
 *  // Delete the RingIO
 *  status = RingIO_delete(&rio_Handle);
 *
 *  @endcode
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



#ifndef RINGIO_H_0xd3f4
#define RINGIO_H_0xd3f4

/* Utilities headers */
#include <ti/syslink/SysLink.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 *  @brief  Indicates that either:
 *              -# The amount of data requested could not be
 *                 serviced due to the presence of an attribute
 *              -# During an attribute read if another is also present
 *                 at the same offset
 */
#define RingIO_S_PENDINGATTRIBUTE    4

/*!
 *  @brief  Indicates that data size requested is not available
 *          as a contiguous chunk. Returning partial buffer as NEED_EXACT_SIZE
 *          was not specified.
 */
#define RingIO_S_NOTCONTIGUOUSDATA   3

/*!
 *  @brief  The resource is still in use
 */
#define RingIO_S_BUSY                 2

/*!
 *  @brief  The module has been already setup
 */
#define RingIO_S_ALREADYSETUP         1

/*!
 *  @brief  Operation is successful.
 */
#define RingIO_S_SUCCESS              0

/*!
 *  @brief  Operation is not successful.
 */
#define RingIO_E_FAIL                 -1

/*!
 *  @brief  There is an invalid argument.
 */
#define RingIO_E_INVALIDARG           -2

/*!
 *  @brief  Operation resulted in memory failure.
 */
#define RingIO_E_MEMORY               -3

/*!
 *  @brief  The specified entity already exists.
 */
#define RingIO_E_ALREADYEXISTS        -4

/*!
 *  @brief  Unable to find the specified entity.
 */
#define RingIO_E_NOTFOUND             -5

/*!
 *  @brief  Operation timed out.
 */
#define RingIO_E_TIMEOUT              -6

/*!
 *  @brief  Module is not initialized.
 */
#define RingIO_E_INVALIDSTATE         -7

/*!
 *  @brief  A failure occurred in an OS-specific call
 */
#define RingIO_E_OSFAILURE            -8

/*!
 *  @brief  Specified resource is not available
 */
#define RingIO_E_RESOURCE             -9

/*!
 *  @brief  Operation was interrupted. Please restart the operation
 */
#define RingIO_E_RESTART              -10

/*!
 *  @brief  An invalid message was encountered
 */
#define RingIO_E_INVALIDMSG           -11

/*!
 *  @brief  Not the owner
 */
#define RingIO_E_NOTOWNER             -12

/*!
 *  @brief  Operation resulted in error
 */
#define RingIO_E_REMOTEACTIVE         -13

/*!
 *  @brief  An invalid heap id was encountered
 */
#define RingIO_E_INVALIDHEAPID        -14

/*!
 *  @brief  An invalid MultiProc id was encountered
 */
#define RingIO_E_INVALIDPROCID        -15

/*!
 *  @brief  The max has been reached.
 */
#define RingIO_E_MAXREACHED           -16

/*!
 *  @brief  Indicates that the instance is in use..
 */
#define RingIO_E_INUSE                -17

/*!
 *  @brief  Indicates that the api is called with wrong handle
 */
#define RingIO_E_INVALIDCONTEXT       -18

/*!
 *  @brief  Versions don't match
 */
#define RingIO_E_BADVERSION           -19

/*!
 *  @brief  Indicates that the amount of data requested could not be serviced
 *          due to the ring buffer getting wrapped
 */
#define RingIO_E_BUFWRAP              -20

/*!
 *  @brief  Indicates that there is no data in the buffer for reading
 */
#define RingIO_E_BUFEMPTY             -21

/*!
 *  @brief  Indicates that the buffer is full
 */
#define RingIO_E_BUFFULL              -22

/*!
 *  @brief  Indicates that there is no attribute at the current offset,
 *          but attributes are present at a future offset
 */
#define RingIO_E_PENDINGDATA          -23

/*!
 *  @brief  Indicates that getAttribute() failed, need to extract
 *          variable length message using getvAttribute()
 */
#define RingIO_E_VARIABLEATTRIBUTE   -24


/*!
 *  @brief  Indicates that the RingIO is in a wrong state
 */
#define RingIO_E_WRONGSTATE          -25
/*!
 *  @brief  Indicates that the underlying notification call failed
 */
#define RingIO_E_NOTIFYFAIL          -26

/*!
 *  @brief  Indicates that the the acquire failed as attribute is
 *          present at current read offset
 */
#define RingIO_E_PENDINGATTRIBUTE   -27


/* =============================================================================
 *  Typedefs
 * =============================================================================
 */
/*!
 *  @brief  Pointer to a data or attribute buffer.
 */
typedef Ptr                       RingIO_BufPtr;

/*!
 *  @brief  Notification message.
 *
 *          This is a 16 bit payload which can be sent to the remote
 *          processor in a notification call.
 */
typedef UInt16                    RingIO_NotifyMsg;

/* =============================================================================
 *  Forward declarations
 * =============================================================================
 */
/*!
 *  @brief  Handle for the RingIO instance.
 */
typedef struct RingIO_Object   *  RingIO_Handle;

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

typedef enum RingIO_Type_Tag {
    RingIO_Type_SHAREDMEM = 0x0
     /*!<  interface type for RingIO transport
      */
} RingIO_Type;

/* =============================================================================
 *  Structures for instance related configuration
 * =============================================================================
 */
/*!
 *  @brief  Common parameters required to create RingIO instance of
 *          any implementation. It should be the first element of the actual
 *          implementaton params structure .
 */
typedef struct RingIO_Params_Tag {
    String        name;
    /*!< Name of the instance */
    RingIO_Type interfaceType;
    /*!< interface type for ringio transport */
} RingIO_Params;

/*!
 *  @brief Enumeration of RingIO open modes.
 */
typedef enum RingIO_OpenMode_Tag {
    RingIO_MODE_READER = 0x0,
    /*!< If specifed in params, open call returns a reader handle.*/
    RingIO_MODE_WRITER = 0x1
    /*!< If specifed in params, open call returns a writer handle.*/
} RingIO_OpenMode;

/*!
 *  @brief  Structure defining config parameters for opening the RingIO
 *          instances.
 */
typedef struct RingIO_openParams_Tag {
    UInt32      flags;
    /*!<  Flags to set behavior of RingIO (e.g. cache maintenance, use exact
     *    size).
     *    See RingIO_Flags */
    RingIO_OpenMode openMode;
    /*!< Mode in which client will be opened */
} RingIO_openParams;


/*!
 *  @brief  Structure defining config parameters for opening the RingIO
 *          instances.
 */
typedef struct RingIO_sharedMemReqDetails_tag {
     UInt32        ctrlSharedMemReq;
     /*!<  Instance specific control shared memory requirements */
     UInt32        dataSharedMemReq;
     /*!<  Data buffer shared memory requirements */
     UInt32        attrSharedMemReq;
     /*!<  Attribute buffer shared memory requirements */
} RingIO_sharedMemReqDetails;


/*!
 *  @brief  Enumeration of flags for RingIO client. It includes information
 *          about access to buffers.
 */
typedef enum RingIO_Flags_tag  {
    RingIO_DATABUF_MAINTAINCACHE = 0x1,
     /*!<  Denotes whether cache maintenance needs to be performed on the
      *    data buffer
      */
    RingIO_NEED_EXACT_SIZE   = 0x2
    /*!<    Denotes whether requests for acquire buffer should always
     *      be satisfied. For a writer, if a contiguous request could not be
     *      satisfied because of the buffer wrap, we mark an
     *      early end and provide a contiguous buffer from the top of the
     *      buffer. For a reader, if a contiguous request could not be
     *      satisfied, we copy the top of the buffer into the footer.
     */
} RingIO_Flags;

/*!
 *  @brief   Enumerates the notification types for RingIO
 */
typedef enum RingIO_NotifyType_Tag {
    RingIO_NOTIFICATION_NONE   = SysLink_NOTIFICATION_NONE,
    /*!< No notification is required. */
    RingIO_NOTIFICATION_ALWAYS = SysLink_NOTIFICATION_ALWAYS,
    /*!< Notify whenever the other client sends data/frees up space.
     *   after a failure to acquire data/space. */
    RingIO_NOTIFICATION_ONCE   = SysLink_NOTIFICATION_ONCE,
    /*!< Notify when the other side sends data/frees up space, after a failure
     *   to acquire data/space. Once the notification is done, the notification
     *   is disabled until it is enabled again when acquire fails.
     */
    RingIO_NOTIFICATION_HDWRFIFO_ALWAYS =
                                  SysLink_NOTIFICATION_HDWRFIFO_ALWAYS,
    /*!< Notify whenever the other side sends data/frees up space if
     *   the size of data/space available is above the watermark.
     *   This notification is never disabled.
     */
    RingIO_NOTIFICATION_HDWRFIFO_ONCE   =
                                  SysLink_NOTIFICATION_HDWRFIFO_ONCE
    /*!< Notify when the other side sends data/frees up space, if the size is
     *   above the watermark level. Once the
     *   notification is done, the notification is disabled until it is
     *   enabled again. The notification is enabled once the watermark
     *   is crossed and does not require buffer to get full/empty.
     */
} RingIO_NotifyType;

/*!
 *  @brief      Prototype of the RingIO call back function.
 *
 *  @param      RingIO_Handle     Pointer to the RingIO instance structure
 *                                in which the default params is to be returned.
 *              Ptr               Argument to callback function e.g. handle to
 *                                semaphore to be posted from callback
 *              RingIO_NotifyMsg  message
 *
 *  @retval     None
 */
typedef void (* RingIO_NotifyFxn ) (RingIO_Handle, Ptr, RingIO_NotifyMsg);


/* =============================================================================
 * APIs to be used in Application
 * =============================================================================
 */
/* =============================================================================
 * Instance creation and deletion related API's
 * =============================================================================
 */
/*!
 *  @brief      Function to initialize the parameters for the RingIO instance.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to RingIO_create() filled in by the
 *              RingIO module with the default parameters.
 *
 *  @pre        @c params is a valid (non-NULL) pointer.
 *
 *  @param[out] params  Pointer to the RingIO interface type specific params.
 *                      Currently only #RingIOShm_Params are supported.
 *
 *  @retval     None
 *
 *  @sa         RingIO_create()
 */
Void RingIO_Params_init (Void * params);

/*!
 *  @brief      Create a RingIO object
 *
 *              This function creates an instance of the RingIO module and
 *              returns an instance handle, which is used to access the
 *              specified RingIO.
 *
 *              Instance-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then the parameter initialization function for the
 *              RingIO transport to be used (e.g. RingIOShm_Params_init()) can
 *              be called to get the configuration filled with the default
 *              values.  After this, only the required configuration values
 *              can be changed.
 *
 *  @param[in]  params  Pointer to the RingIO interface type specific params.
 *                      Currently only #RingIOShm_Params are supported.
 *
 *  @pre        @c params is a valid (non-NULL) pointer to a set of interface
 *              type specific params.
 *
 *  @retval     non-NULL        Operation successful
 *  @retval     NULL            Operation unsuccessful
 *
 *  @sa         RingIO_Params_init()
 *  @sa         RingIO_delete()
 *  @sa         RingIO_open()
 *  @sa         RingIO_close()
 */
RingIO_Handle RingIO_create(const Void *params);

/*!
 *  @brief      Delete a RingIO object
 *
 *              Once this function is called, other RingIO instance level APIs
 *              that require the instance handle cannot be called.
 *
 *  @param[in]  handlePtr  Pointer to Handle to the RingIO object. This MUST
 *                         be a handle that was returned from RingIO_create()
 *                         and not one that comes from RingIO_open().
 *
 *  @pre        @c handlePtr is a valid (non-NULL) pointer to a valid RingIO
 *              handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_INVALIDSTATE Module is in an invalid state
 *                                    Instance was not created on this processor
 *  @retval     RingIO_E_INVALIDARG   handlePtr pointer passed is NULL
 *  @retval     RingIO_E_INVALIDARG   *handlePtr passed is NULL
 *  @retval     RingIO_E_INVALIDCONTEXT  Api is not called with a handle from
 *                                       RingIO_create()
 *
 *  @sa         RingIO_create()
 */
Int RingIO_delete(RingIO_Handle *handlePtr);

/*!
 * @brief       Get shared memory requirements.
 *
 *              RingIO_Params_init() must be called to initialize the
 *              parameters before calling this function.
 *
 * @param[in]   params  RingIO interface type specific params.  Currently
 *                      only #RingIOShm_Params are supported.
 *
 * @param[out] sharedMemReqDetails Control, Data and Attr shared memory details
 *
 * @pre         @c params is a valid (non-NULL) pointer to interface specific
 *              params.
 * @pre         @c sharedMemReqDetails is a valid (non-NULL) pointer.
 *
 * @retval      Success         Total Shared memory requirements
 */
UInt32 RingIO_sharedMemReq(const Void *params,
        RingIO_sharedMemReqDetails *sharedMemReqDetails);

/* =============================================================================
 * Instance opening and closing related API's
 * =============================================================================
 */
/*!
 *  @brief      Opens a created instance of RingIO module.
 *
 *  @param[in]  name      Name of the RingIO instance to be opened
 *  @param[in]  params    Parameters for opening the RingIO instance.
 *  @param[in]  procIds   Array of processor IDs to search for the RingIO
 *                        instance. If NULL is specified, all processors are
 *                        checked.
 *  @param[out] handlePtr Holds the RingIO handle on success.
 *
 *  @pre        @c handlePtr is a valid (non-NULL) pointer.
 *  @pre        @c params is a valid (non-NULL) pointer to a set of parameters
 *              to open the RingIO instance.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_INVALIDSTATE Module is in an invalid state
 *  @retval     RingIO_E_NOTFOUND     RingIO not found
 *  @retval     RingIO_E_FAIL         Failed to open the gate associated with
 *                                    the RingIO instance.
 *                                    Failed to open the clientnotifymgr
 *                                    associated with the RingIO instance.
 *  @retval     RingIO_E_INVALIDARG   Parameters passed are invalid
 *
 *  @sa         RingIO_close()
 *  @sa         RingIO_openParams()
 */
Int RingIO_open(String name, const RingIO_openParams *params, UInt16 *procIds,
        RingIO_Handle *handlePtr);

/*!
 *  @brief      Opens a created instance of RingIO module by the shared
 *              memory address of the instance.
 *
 *  @param[in]  ctrlSharedAddr  Shared address at which the instance exists
 *  @param[in]  params          Parameters for opening the RingIO instance.
 *  @param[out] handlePtr       Holds the RingIO handle on success.
 *
 *  @pre        @c handlePtr is a valid (non-NULL) pointer.
 *  @pre        @c params is a valid (non-NULL) pointer to a set of parameters
 *              for opening the RingIO instance.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_INVALIDSTATE Module is in an invalid state
 *  @retval     RingIO_E_NOTFOUND     RingIO not found
 *  @retval     RingIO_E_FAIL         Failed to open the gate associated with
 *                                    the RingIO instance.
 *                                    Failed to open the clientnotifymgr
 *                                    associated with the RingIO instance.
 *  @retval     RingIO_E_INVALIDARG   Parameters passed are invalid
 *
 *  @sa         RingIO_create()
 *  @sa         RingIO_delete()
 *  @sa         RingIO_close()
 */
Int RingIO_openByAddr(Ptr ctrlSharedAddr, const RingIO_openParams *params,
        RingIO_Handle *handlePtr);

/*!
 *  @brief      Closes previously opened instance of RingIO module.
 *
 *  @param[in]  handlePtr  Pointer to handle for the opened RingIO instance.
 *                         MUST be a pointer returned by RingIO_open and not
 *                         RingIO_create.
 *                         Reset to invalid when the function successfully
 *                         completes.
 *
 *  @pre        @c handlePtr is a valid (non-NULL) pointer to a RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_INVALIDSTATE Module is in an invalid state
 *  @retval     RingIO_E_FAIL         Failed to open the gate associated with
 *                                    the RingIO instance.
 *                                    Failed to open the clientnotifymgr
 *                                    associated with the RingIO instance.
 *  @retval     RingIO_E_INVALIDARG   Parameters passed are invalid
 *  @retval     RingIO_E_INVALIDCONTEXT  Api is not called with a handle from
 *                                       RingIO_open
 *
 *  @sa         RingIO_create()
 *  @sa         RingIO_delete()
 *  @sa         RingIO_open()
 */
Int RingIO_close(RingIO_Handle *handlePtr);


/* =============================================================================
 * Instance Notification related API's
 * =============================================================================
 */
/*!
 * @brief       Function to register a call back function.
 *
 *              This function sets Notification parameters for the RingIO
 *              Client.
 *              Both the reader and writer can set their notification mechanism
 *              using this function
 *
 *              RingIO_open() for both reader and writer has been successful.
 *              No SysLink API should be called from a callback function
 *              registered through the RingIO module. On SYS/BIOS
 *              the callback functions are run from ISR context and must not
 *              perform any operations that
 *              may take a lock or block, which is done by most SysLink APIs.
 *              Minimum functionality must be used in the callback functions,
 *              most often limited to posting a semaphore on which application
 *              is waiting, posting SWI etc.
 *
 *  @param[in]  handle     RingIO client Handle.
 *  @param[in]  notifyType Type of notification.
 *  @param[in]  watermark  WaterMark
 *  @param[in]  notifyFunc Call back function
 *  @param[in]  cbContext  Context pointer that needs to be passed to call back.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *  @pre        @c notifyFunc is a valid (non-NULL) function pointer to a
 *              notification function.
 *  @pre        @c cbContext is a valid (non-NULL) pointer to data that
 *              would be passed to the notification function.
 *
 *  @retval RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval RingIO_E_INVALIDSTATE Module is in an invalid state
 *                                RingIO_open is not called yet
 *  @retval RingIO_E_FAIL         Failed to open the gate associated with
 *                                the RingIO instance.
 *                                Failed to open the clientnotifymgr associated
 *                                with the RingIO instance.
 *  @retval RingIO_E_INVALIDARG   Parameters passed are invalid
 *
 *  @sa         RingIO_unregisterNotifier()
 */
Int RingIO_registerNotifier(RingIO_Handle handle,
        RingIO_NotifyType notifyType, UInt32 watermark,
        RingIO_NotifyFxn notifyFunc, Ptr cbContext);

/*!
 *  @brief      Unregister a call back function
 *
 *  @param[in]  handle  RingIO client Handle.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_INVALIDSTATE Module is in an invalid state
 *                                    RingIO_open is not called yet
 *  @retval     RingIO_E_FAIL         Failed to open the gate associated with
 *                                    the RingIO instance.
 *                                    Failed to open the clientnotifymgr
 *                                    associated with the RingIO instance.
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *
 *  @sa         RingIO_registerNotifier()
 */
Int RingIO_unregisterNotifier(RingIO_Handle handle);

/*!
 *  @brief Function to set client's watermark
 *
 *  @param[in] handle       RingIO client Handle.
 *  @param[in] watermark    Watermark value
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         @copydoc RingIO_E_FAIL
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *
 *  @sa         RingIO_registerNotifier()
 */
Int RingIO_setWaterMark(RingIO_Handle handle, UInt32 watermark);


/*!
 *  @brief      Function to set the notify type
 *
 *  @param[in]  handle     Instance handle.
 *  @param[in]  notifyType Type of notification
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         @copydoc RingIO_E_FAIL
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *
 *  @sa         RingIO_NotifyType
 *  @sa         RingIO_registerNotifier()
 */
Int RingIO_setNotifyType(RingIO_Handle handle, RingIO_NotifyType notifyType);


/*=============================================================================
 * Main RingIO APIs called by application for RingIO functions
 *=============================================================================
 */
/*!
 *  @brief      Acquire data from the RingIO instance
 *
 *              This function acquires a data buffer from RingIO for reading or
 *              writing, depending on the mode in which the client (represented
 *              by the handle) has been opened.
 *
 *  @param[in]     handle   RingIO handle
 *  @param[out]    pData    Location to store the pointer to the acquired data
 *                          buffer.
 *  @param[in,out] pSize    Pointer to the size of data buffer. Holds
 *                          the size of buffer to be acquired. Returns the size
 *                          of buffer actually acquired.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *  @pre        @c pData is a valid (non-NULL) pointer to a RingIO_BufPtr.
 *  @pre        @c pSize is a valid (non-NULL) pointer to a size.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_PENDINGATTRIBUTE (For reader only) No data buffer
 *                                        could be acquired
 *                                        because an attribute was
 *                                        present at the current read offset.
 *  @retval     RingIO_S_PENDINGATTRIBUTE (For reader only) Flag NEED_EXACT_SIZE
 *                                        is not set.
 *                                        Partial Data buffer is retuned till
 *                                        the offset where an attribute was
 *                                        present.
 *  @retval     RingIO_E_BUFFULL          (For writer only) Requested size of
 *                                        data buffer could
 *                                        not be returned because the total
 *                                        available size is less than requested
 *                                        size. A smaller
 *                                        sized buffer may have been returned,
 *                                        if available.
 *  @retval     RingIO_E_BUFWRAP          (For writer only) Requested size of
 *                                        data buffer could
 *                                        not be returned because the
 *                                        available contiguous size till end
 *                                        of buffer is less than requested size.
 *                                        A smaller sized buffer may have been
 *                                        returned, if available.
 *  @retval     RingIO_E_FAIL             General failure.
 *                                        Contiguous buffer requirement cannot
 *                                        be satisfied even by setting early
 *                                        end.
 *  @retval     RingIO_S_NOTCONTIGUOUSDATA (For reader only) Data size
 *                                         requested is not available
 *                                         as a contiguous chunk. Returning
 *                                         partial buffer as NEED_EXACT_SIZE
 *                                         was not specified.
 *  @retval     RingIO_E_BUFEMPTY          (For reader only) No data available
 *                                         to acquire
 *  @retval     RingIO_E_INVALIDARG        @copydoc RingIO_E_INVALIDARG
 *
 *  @sa         RingIO_release()
 */
Int RingIO_acquire(RingIO_Handle handle, RingIO_BufPtr *pData, UInt32 *pSize);


/*!
 *  @brief      Releases data from the RingIO instance
 *
 *  @param[in]  handle      RingIO handle
 *  @param[in]  size        Size of the released buffer
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         General Failure
 *                                    Invalid size argument >
 *                                    client acquired size passed
 *  @retval     RingIO_E_INVALIDARG   Parameters passed are invalid
 *  @retval     RingIO_E_NOTIFYFAIL   Failure when sending notification
 *                                    to other client.
 *
 *  @sa         RingIO_acquire()
 */
Int RingIO_release(RingIO_Handle handle, UInt32 size);


/*!
 *  @brief      Cancel the previous acquired buffer to
 *              the RingIO instance
 *              This function cancels any data buffers acquired by reader or
 *              writer. In the case of writer, all attributes that are set since
 *              the first acquire are removed. In the case of reader, all
 *              attributes that were obtained since the first acquired
 *              are re-instated in the RingIO instance.
 *
 *  @param[in]  handle                RingIO handle
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         General Failure
 *                                    No data acquired
 *  @retval     RingIO_E_INVALIDARG   Parameters passed are invalid
 *
 *  @sa         RingIO_acquire()
 */
Int RingIO_cancel(RingIO_Handle handle);


/*!
 *  @brief      Get attribute from the RingIO instance
 *
 *              This function gets an attribute with a variable-sized payload
 *              from the attribute buffer.
 *              If an attribute is present, the attribute type, the optional
 *              parameter, a pointer to the optional payload and the payload
 *              size are returned.
 *
 *  @param[in]  handle                RingIO handle
 *  @param[out] type
 *              Location to receive the user-defined type of attribute.
 *  @param[out] param
 *              Location to receive an optional parameter which depends on the
 *              attribute type.
 *  @param[out] vptr
 *              Pointer to buffer to receive the optional payload.
 *  @param[out] pSize
 *              Location with the size of the variable attribute. On return,
 *              this stores the actual size of the payload.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *  @pre        @c type is a valid (non-NULL) pointer.
 *  @pre        @c param is a valid (non-NULL) pointer.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         General Failure
 *                                    No attribute present
 *  @retval     RingIO_E_INVALIDARG   Parameters passed are invalid
 *  @retval     RingIO_S_PENDINGATTRIBUTE  Additional attributes are present at
 *                                         the current read offset.
 *  @retval     RingIO_E_VARIABLEATTRIBUTE No buffer has been provided to
 *                                         receive the variable attribute
 *                                         payload.
 *  @retval     RingIO_E_PENDINGDATA       More data must be read before reading
 *                                         the attribute.
 *
 *  @sa         RingIO_getAttribute()
 */
Int RingIO_getvAttribute(RingIO_Handle handle, UInt16 *type, UInt32 *param,
        RingIO_BufPtr vptr, UInt32 *pSize);


/*!
 *  @brief      Set attribute in the RingIO instance
 *
 *              This function sets an attribute with a variable sized payload at
 *              the offset provided in the acquired data buffer.
 *
 *              If the offset is not in the range of the acquired data buffer,
 *              the attribute is not set, and an error is returned. One
 *              exception to this rule is when no data buffer has been acquired.
 *              In this case an attribute is set at the next data buffer offset
 *              that can be acquired
 *
 *  @param[in]  handle            Handle to the RingIO Client.
 *  @param[in]  type              User-defined type of attribute.
 *  @param[in]  param             Optional parameter which depends on the
 *                                attribute type.
 *  @param[in]  pData             Pointer to attribute payload buffer.
 *  @param[in]  size              Size of the attribute payload.
 *  @param[in]  sendNotification  Flag to indicate whether notification should
 *                                be sent or not on successful setting of an
 *                                attribute
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         General Failure
 *                                    No attribute present
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *  @retval     RingIO_E_WRONGSTATE
 *               Do not allow setting an attribute
 *               when it falls into reader
 *               region. The following scenarios cover this condition:
 *               - The buffer is completely full. In this case, attribute can
 *                 only be set at offset 0. But offset 0 falls into reader
 *                 region.
 *               - The buffer is completely acquired by the writer. Part or none
 *                 of this buffer may be released. Writer is attempting to set
 *                 an attribute at the end of its acquired range. In this case,
 *                 end of writer buffer is the same as beginning of reader
 *                 buffer.
 *               If the reader has acquired and released some data, resulting in
 *               its moving further such that its acquire start is not at the
 *               same location where writer may be able to set an attribute, the
 *               above conditions do not hold true, and the attribute is allowed
 *               to be set.
 *  @retval     RingIO_E_NOTIFYFAIL   @copydoc RingIO_E_NOTIFYFAIL
 *
 *  @sa         RingIO_acquire()
 */
Int RingIO_setvAttribute(RingIO_Handle handle, UInt16 type, UInt32 param,
        RingIO_BufPtr pData, UInt32 size, Bool sendNotification);


/*!
 *  @brief      Obtain a fixed-size attribute from the attribute buffer
 *
 *              If an attribute is present, the attribute type and a related
 *              parameter are returned.
 *
 *  @param[in]  handle      RingIO handle
 *  @param[out] type
 *              Location to receive the user-defined type of attribute.
 *  @param[out] param
 *              Location to receive an optional parameter which depends on the
 *              attribute type.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *  @pre        @c type is a valid (non-NULL) pointer.
 *  @pre        @c param is a valid (non-NULL) pointer.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         General Failure
 *                                    No attribute present
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *  @retval     RingIO_S_PENDINGATTRIBUTE  Additional attributes are present at
 *                                         the current read offset.
 *  @retval     RingIO_E_VARIABLEATTRIBUTE A variable attribute exists.The
 *                                         application
 *                                         must call RingIO_getvAttribute ()
 *                                         to get the variable attribute.
 *  @retval     RingIO_E_PENDINGDATA       More data must be read before reading
 *                                         the attribute.
 *
 *  @sa         RingIO_getvAttribute()
 */
Int RingIO_getAttribute(RingIO_Handle handle, UInt16 *type, UInt32 *param);

/*!
 *  @brief      This function sets a fixed-size attribute in the acquired data
 *              buffer.
 *
 *              When no data buffer has been acquired, the attribute
 *              is set at the next data buffer that can be acquired
 *
 *  @param[in]  handle                  Handle to the RingIO Client
 *  @param[in]  type                    User-defined type of attribute
 *  @param[in]  param                   Optional parameter which depends on the
 *                                      attribute type
 *  @param[in]  sendNotification        Flag indicating whether notification
 *                                      should be sent or not on successful
 *                                      setting of an attribute
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_E_FAIL           General Failure
 *  @retval     RingIO_E_INVALIDARG     Parameters passed are invalid
 *  @retval     RingIO_S_SUCCESS        Operation Successful
 *  @retval     RingIO_E_WRONGSTATE     Do not allow setting an attribute
 *                                      when it falls into reader region.
 *              The following scenarios cover this condition:
 *              - The buffer is completely full. In this case, attribute can
 *                only be set at offset 0, but offset 0 falls into the reader's
 *                region.
 *              - The buffer is completely acquired by the writer. Part or none
 *                of this buffer may be released. Writer is attempting to set an
 *                attribute at the end of its acquired range. In this case, end
 *                of writer buffer is the same as beginning of reader buffer.
 *              If the reader has acquired and released some data, resulting in
 *              its moving further such that its acquire start is not at the
 *              same location where writer may be able to set an attribute, the
 *              above conditions do not hold true, and the attribute is allowed
 *              to be set.
 *
 *  @retval     RingIO_E_NOTIFYFAIL   @copydoc RingIO_E_NOTIFYFAIL
 *
 *
 */
Int RingIO_setAttribute(RingIO_Handle handle, UInt16 type, UInt32 param,
        Bool sendNotification);


/*!
 *  @brief      Flush the buffer in the RingIO instance
 *
 *  This function is used to flush the data from the RingIO.  The
 *  behavior of this function depends on the value of hardFlush argument.
 * When hardFlush is false:
 *              If function is called for the writer, all the valid data in
 *              buffer after the first attribute location will be discarded.
 *              In case there are no attributes, no data will be cleared from
 *              the buffer. Note that this does not include the data that has
 *              been already acquired by the reader. Note that the attribute
 *              will also be cleared from the attribute buffer.
 *              For the reader, all the data till the next attribute location
 *              will be discarded. And if there is no attribute in the buffer,
 *              all valid data will get discarded. Note that the attribute will
 *              remain the attribute buffer. This is different from the
 *              behavior mentioned for the writer.
 * When hardFlush is true:
 *              If function is called from the writer, all committed data and
 *              attributes that is not acquired by reader are removed from
 *              the RingIO instance. The writer pointer is moved to point to
 *              reader's head pointer
 *              If function is called from the reader, all data and attributes
 *              that can be subsequently acquired from the reader are removed.
 *
 *  @param[in]  handle          Handle to the RingIO Client.
 *  @param[in]  hardFlush       Mode in which the flush operation discards
 *                               committed data and attributes
 *  @param[out] type            Location to receive the User-defined type
 *                              of attribute.
 *  @param[out] param           Location to receive an optional parameter
 *                              which depends on the attribute type.
 *  @param[out] bytesFlushed    Location to recieve the size of the buffer
 *                              flushed.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *  @pre        @c type is a valid (non-NULL) pointer.
 *  @pre        @c param is a valid (non-NULL) pointer.
 *  @pre        @c bytesFlushed is a valid (non-NULL) pointer.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         @copydoc RingIO_E_FAIL
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *  @retval     RingIO_E_NOTIFYFAIL   @copydoc RingIO_E_NOTIFYFAIL
 *
 *  @sa         RingIO_acquire()
 */
Int RingIO_flush(RingIO_Handle handle, Bool hardFlush, UInt16 *type,
        UInt32 *param, UInt32 *bytesFlushed);


/*!
 *  @brief      Send a force notification to the other client manually
 *
 *  @param[in]  handle      Handle to RingIO instance
 *  @param[in]  msg         Payload to notification
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval     RingIO_S_SUCCESS      @copydoc RingIO_S_SUCCESS
 *  @retval     RingIO_E_FAIL         @copydoc RingIO_E_FAIL
 *  @retval     RingIO_E_INVALIDARG   @copydoc RingIO_E_INVALIDARG
 *  @retval     RingIO_E_NOTIFYFAIL   @copydoc RingIO_E_NOTIFYFAIL
 *
 *  @sa         None
 */
Int RingIO_sendNotify(RingIO_Handle handle, RingIO_NotifyMsg msg);


/*=============================================================================
 * Helper APIs
 *=============================================================================
 */
/*!
 *  @brief Function to get valid data buffer size.
 *
 *  @param[in]  handle  RingIO client Handle.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval             Valid_data_buffer_size Operation successful
 *  @retval             -1                     Operation unsuccessful.
 */
UInt RingIO_getValidSize(RingIO_Handle handle);

/*!
 *  @brief Function to get empty data buffer size.
 *
 *  @param[in]  handle  RingIO client Handle.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval             Empty_data_buffer_size Operation successful
 *  @retval             -1                     Operation unsuccessful
 */
UInt RingIO_getEmptySize(RingIO_Handle handle);

/*!
 *  @brief Function to get valid attribute size.
 *
 *  @param[in]  handle  RingIO client Handle.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval             Valid_attribute_size Operation successful
 *  @retval             -1                   Operation unsuccessful
 */
UInt RingIO_getValidAttrSize(RingIO_Handle handle);

/*!
 *  @brief Function to get empty attribute size.
 *
 *  @param[in]  handle  RingIO client Handle.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval             Empty_attribute_size Operation successful
 *  @retval            -1                   Operation unsuccessful
 */
UInt RingIO_getEmptyAttrSize(RingIO_Handle handle);

/*!
 *  @brief Function to get client's acquire offset from the start of the buffer
 *
 *  @param[in]  handle  RingIO client Handle.
 *
 *  @pre        @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval             Acquired_offset Operation successful
 *  @retval             -1              Operation unsuccessful
 */
UInt RingIO_getAcquiredOffset(RingIO_Handle handle);

/*!
 * @brief Function to get client's acquired size
 *
 * @param[in]   handle       RingIO client Handle.
 *
 * @pre         @c handle is a valid (non-NULL) RingIO handle.
 *
 * @retval             Acquired_Size Operation successful
 * @retval             -1            Operation unsuccessful
 */
UInt RingIO_getAcquiredSize(RingIO_Handle handle);

/*!
 *  @brief Function to get client's  watermark
 *
 *  @param[in] handle   RingIO client Handle.
 *
 *  @pre       @c handle is a valid (non-NULL) RingIO handle.
 *
 *  @retval             Watermark_value Operation successful
 *  @retval            -1              Operation unsuccessful
 */
UInt RingIO_getWaterMark(RingIO_Handle handle);

#if defined (__cplusplus)
}
#endif


#endif /* RINGIO_H_0xd3f4 */
