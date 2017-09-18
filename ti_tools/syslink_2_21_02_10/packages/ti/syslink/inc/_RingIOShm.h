/** 
 *  @file   _RingIOShm.h
 *
 *  @brief      RingIO shared memory based physical transport for
 *              communication with the remote processor.
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



#ifndef _RINGIOSHM_H
#define _RINGIOSHM_H


/* Module headers */
#include <ti/ipc/GateMP.h>
#include <ti/syslink/RingIO.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros
 * =============================================================================
 */
/*!
 *  @brief  Name of memory that contains the RingIOShm global structure
 */
#define RingIOShm_GLOBAL "RingIOShm.global"

/*!
 *  @brief  Name of memory that contains the RingIOShm configuration structure
 */
#define RingIOShm_CONFIG "RingIOShm.cfg"

/*!
 *  @brief  Created stamp for RingIOShm instance
 */
#define RingIOShm_CREATED 0x05251995

/*!
 *  @brief  Version information for RingIOShm instance
 */
#define RingIOShm_VERSION         1

/*!
 *  @brief  This constant an invalid offset in the attribute buffer.
 */
#define RingIOShm_invalidOffset -1

/*!
 *  @brief  This constant denotes an invalid attribute type.
 *          This should not be used for any valid attribute.
 */
#define RingIOShm_invalidAttr 0xFFFF

/*!
 *  @brief   Check if object is a writer.
 */
#define RingIOShm_IS_WRITER(object)             ((object)->openMode == (UInt32)RingIO_MODE_WRITER)

/*!
 *  @brief   Check if object is a reader.
 */
#define RingIOShm_IS_READER(object)             ((object)->openMode == (UInt32)RingIO_MODE_READER)

/*!
 *  @brief   Get Current attr buffer end.
 */
#define RingIOShm_GET_CUR_ATTR_END(object)      (object->control->curAttrBufEnd + 1)

/*!
 *  @brief   Get Current data buffer end.
 */
#define RingIOShm_GET_CUR_DATA_END(object)      (object->control->curBufEnd + 1)

/*!
 *  @brief   Modulo operation
 */
#define RingIOShm_ADJUST_MODULO(start, size, bufSize)   (((start) + (size)) % bufSize)

/*!
 *  @brief   Size of RingIO attributes
 */
#define RingIOShm_SIZEOF_ATTR(size)                     (sizeof (RingIOShm_Attr) + (size))

/* =============================================================================
 * Internal structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining config parameters for the RingIOShm module.
 */
typedef struct RingIOShm_Config_Tag {
    UInt32       notifyEventNo;
    /*!<  Notify event number to be used by the Client Notify Manager */
} RingIOShm_Config;

/*!
 * @brief  Private structure of RingIO Attribute buffer
 */
typedef struct RingIOShm_Attr_tag {
    UInt32    offset;
     /*!<  Offset into data buffer */
    Int32     prevoffset;
      /*!< Offset to previous attribute */
    UInt16    type;
      /*!< Attribute type */
    UInt16    size;
      /*!< Size of the attribute payload following this */
    UInt32    param;
      /*!< Optional Parameter */
} RingIOShm_Attr;

/*!
 *  @brief  Structure defining processor related information for the
 *          RingIO module.
 */
typedef struct RingIOShm_ProcAttrs_tag {
    Bool   creator;   /*!< Creator or opener */
    UInt16 procId;    /*!< Processor Identifier */
} RingIOShm_ProcAttrs;

/*!
 *  @brief  This structure defines the RingIO Shared memory client structure.
 *          The RingIO Reader or Writer state information is stored in this
 *          structure
 */
typedef struct RingIOShm_Client_tag {
    RingIO_BufPtr          pDataStart;
    /*!< Virtual start address of the data buffer */
    RingIO_BufPtr          pAttrStart;
    /*!< Virtual start address of the attr buffer */
    UInt32                 acqStart;
    /*!< Start offset of data buffer that has been acquired by the*
     *    application.
     */
    UInt32                 acqSize;
    /*!< Size of data that has been acquired */
    UInt32                 acqAttrStart;
    /*!< Start offset of the acquired attribute buffer */
    UInt32                 acqAttrSize;
    /*!< Size of attribute data that has been acquired */
    UInt32                 notifyType;
    /*!< Notification type */
    RingIO_NotifyParam     notifyParam;
    /*!< Parameter to the Notification function
     * TBD: To be removed once ClientNotifyMgr change is implemented
     */
    UInt32                 notifyWaterMark;
    /*!< Watermark that should be satisfied before notification is done
     * TBD: To be removed once ClientNotifyMgr change is implemented
     */
    UInt32                 flags;
    /*   Counting bits from MSB to LSB 0....31
     *   B31 - Denotes whether the client uses cache for accessing the
     *         data buffer (dataBufCacheUseFlag)
     *   B30 - Denotes whether requests for acquire buffer should always
     *         be satisfied. If a contiguous request could not be
     *         satisfied because of the buffer wrap, we need to mark and
     *         early end and provide only a contiguous buffer
     *         (contiguousBufFlag)
     */
    UInt32                 isValid;
    /*!< Indicates whether the Client is initialized */
    UInt32                 refCount;
    /*!< Denotes whether ringio has been re-opened or not */
    UInt32                 notifyId;
    /*!<  Id  returned by the register client notification */
    Bool                   isRegistered;
    /*!< If TRUE, denotes client has registered notification function */
    //UInt16          padding [CLIENT_PADDING];
     /*!< Padding for cache alignment */
    ClientNotifyMgr_Handle clientNotifyMgrHandle;
    /*!< Handle to the clientNotifyMgr handle used for this instance */
    GateMP_Handle            clientNotifyMgrGate;
    /*!< Handle to the gate to the clientNotifyMgr handle used for this instance
     *   This is used only for opening
     */
} RingIOShm_Client;

/*!
 *  @brief   This structure defines the RingIO Control Structure. This structure
 *           is stored in shared memory and is accessible by all clients. The
 *           control structure supports a single reader and a single writer for
 *           the ring buffer.
 */

/* DO NOT forget padding interfaceAttrs is newly added */
typedef struct RingIOShm_ControlStruct_tag {
    RingIO_InterfaceAttrs   interfaceAttrs;
    /*!< Identify type using address in openbyaddr. */
    SharedRegion_SRPtr     srPtrBufStart;
    /*!< Physical start address of the data buffer. */
    SharedRegion_SRPtr     srPtrBufEnd;
    /*!<        Total size of the Data buffer (offset from srPtrBufStart) */
    UInt32                curBufEnd;
    /*!< Current buffer size. This may be <= dataBufEnd (offset from
     *   srPtrBufStart)
     */
    UInt32                dataBufEnd;
    /*!< Size of the main data buffer (offset from srPtrBufStart) */
    UInt32                dataSharedAddrSize;
    /*!< Size of the data buffer */
    UInt32                footBufSize;
    /*!<        Footer area for providing contiguous buffer to a reader. */
    UInt32                validSize;
    /*!< Amount of valid data available in the data buffer. Valid Data is
     *   the total data that is readable by the reader using an acquire
     *   call. This  does not include the size of the data buffer already
     *   acquired by the reader
     */
    UInt32                emptySize;
    /*!< Amount of empty space in the data buffer. This does not include
     *   the empty space already acquired by the writer
     */
    SharedRegion_SRPtr    srPtrAttrBufStart;
    /*!< Physical start address of the attr buffer */
    SharedRegion_SRPtr    srPtrAttrBufEnd;
    /*!< Total Size of the attribute buffer (offset) */
    UInt32                curAttrBufEnd;
    /*!< Current Attr buffer size. This may be <= the srPtrAttrBufEnd
     *   (offset)
     */
    UInt32                validAttrSize;
    /*!< Amount of valid attribute bytes available in the attribute
     *   buffer. The valid attribute bytes does not include the attribute
     *   bytes already acquired by the reader
     */
    UInt32                emptyAttrSize;
    /*!< Amount of empty space in the attr buffer. This does not include
     *   the empty attr space already acquired by the writer
     */
    Int32                 prevAttrOffset;
    /*!< Offset of the most recent attribute */
    UInt16                procId;
    /*!< Processor Identifier. */
    SharedRegion_SRPtr    gateMPSharedAddr;
    /*!< Address to the gate MP for this RingIOShm instance */
} RingIOShm_ControlStruct;

/*!
 *  @brief   This type is used for the handle to the Shared Control structure.
 */
typedef RingIOShm_ControlStruct * RingIOShm_ControlHandle;

/*!
 *  @brief   This type is used for the handle to the Shared Client structure.
 */
typedef RingIOShm_Client       * RingIOShm_ClientHandle;

/*!
 *  @brief  Structure for the Handle for the RingIO.
 */
typedef struct RingIOShm_Obj_tag {
    String      name;
    /*!< Instance name */
    Ptr         ctrlSharedAddr;
     /*!<  Shared region address for control structure. The shared address
      *    details for control, data and attribute buffers.
      *    a RingIOShm instance.
      *    If this field is NULL, memory equal to that required to allocate that
      *    RingIOShm instance control structure is allocated from heap associated
      *    with ctrlRegionId
      */    
    GateMP_Handle               gateHandle;
    /*!< Handle to the gate to for this RingIOShm instance
     *   This is used only for creation */
    RingIO_Handle               transportHandle;
    /*! RingIO Transport handle */
    RingIOShm_ControlHandle     control;
    /*! RingIO control structure for this instance */
    RingIOShm_ClientHandle      writer;
    /*! RingIO writer client structure for this instance */
    RingIOShm_ClientHandle      reader;
    /*! RingIO reader client structure for this instance */
    UInt32                      openMode;
    /*! Open mode of client for this instance */
    RingIOShm_ProcAttrs         owner;
    /*!< Creator's attributes associated with an instance */
    ClientNotifyMgr_Handle      clientNotifyMgrHandle;
    /*!< Handle to the clientNotifyMgr handle used for this instance
     *   This is used only for creation
     */
    GateMP_Handle               clientNotifyMgrGate;
    /*!< Handle to the gate to the clientNotifyMgr handle used for this instance
     *   This is used only for creation
     */
    Bool                        controlCacheUse;
    /*! Whether control structure region is caches or not */
    Bool                        dataBufCacheUse;
    /*! Whether data buffer region is cached or not */
    Bool                        attrBufCacheUse;
    /*! Whether attr buffer region is cached or not */
    Bool                        controlIntAllocated;
    /*! Whether control structure region is internally allocated or not */
    Bool                        dataBufIntAllocated;
    /*! Whether data buffer region is internally allocated or not */
    Bool                        attrBufIntAllocated;
    /*! Whether attr buffer region is internally allocated or not */
    UInt32                      flags;
    /*! Whether attr buffer region is cached or not */
}RingIOShm_Obj;



/* =============================================================================
 *  Module functions exported through function table interface
 * =============================================================================
 */
/*!
 *  @brief      Function to create a RingIOShm object for a specific slave
 *              processor.
 *
 *              This function creates an instance of the RingIOShm module and
 *              returns an instance handle, which is used to access the
 *              specified RingIOShm.
 *              Instance-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then RingIOShm_Params_init can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @retval     Valid_handle_of_type_RingIO_Handle Operation successful
 *  @retval     NULL                               Operation unsuccessful
 *
 *  @sa         RingIO_delete, RingIO_open, RingIO_close
 */
Int32
RingIOShm_create (const Void * params, RingIO_Handle * handlePtr);


/*!
 * @brief     Get shared memory requirements
 *
 * @param     params           Instance creation parameters
 * @param     sharedMemDetails Control, Data and Attr shared memory details
 *
 * @retval    Shared-memory-requirements Operation successfully completed.
 *            The out parameter of sharedMemDetails contains the values for
 *            ctrlSharedMemReq : Control shared memory requirements
 *            dataSharedMemReq : Data Buffer shared memory requirements
 *            attrSharedMemReq : Attribute buffer shared memory requirements
 *
 * @sa        None
 */
UInt32
RingIOShm_sharedMemReq (const Void *                       params,
                              RingIO_sharedMemReqDetails * sharedMemReqDetails);
/*!
 *  @brief      Opens a created instance of RingIOShm module.
 */
Int
RingIOShm_openByAddr (   Ptr                 ctrlSharedAddr,
                   const RingIO_openParams * params,
                         RingIO_Handle     * handlePtr);
/*!
 *  @brief      Function to delete a RingIO object for a specific slave
 *              processor.
 */
Int RingIOShm_delete (RingIO_Handle * handlePtr);

/*!
 *  @brief      Closes previously opened/created instance of RingIO module.
 */
Int RingIOShm_close (RingIO_Handle * handlePtr);

/*!
 * @brief       Function to register a call back function.
 *
 * @sa                None
 */
Int   RingIOShm_registerNotifier (RingIO_Handle      handle,
                                  RingIO_NotifyType  notifyType,
                                  UInt32             watermark,
                                  RingIO_NotifyFxn   notifyFunc,
                                  Ptr                cbContext);

/*!
 * @brief              Function to un-register call back function.
 */
Int   RingIOShm_unregisterNotifier (RingIO_Handle      handle);

/*!
 * @brief Function to set client's  watermark
 */
Int   RingIOShm_setWaterMark (RingIO_Handle      handle,
                              UInt32             watermark);

/*!
 *  @brief     Function to set the notify type
 */
Int
RingIOShm_setNotifyType (RingIO_Handle        handle,
                      RingIO_NotifyType    notifyType);

/*!
 *  @brief      Acquires data from the RingIO instance
 *              This function acquires a data buffer from RingIO for reading or
 *              writing, depending on the mode in which the client (represented
 *              by the handle) has been opened.
 */
Int
RingIOShm_acquire (RingIO_Handle   handle,
                RingIO_BufPtr * pData,
                UInt32 *        pSize);

/*!
 *  @brief      Releases data from the RingIO instance
 */
Int
RingIOShm_release (RingIO_Handle handle,
                UInt32        size);


/*!
 *  @brief      Cancel the previous acquired buffer to
 *              the RingIO instance
 *              This function cancels any data buffers acquired by reader or
 *              writer. In the case of writer, all attributes that are set since
 *              the first acquire are removed. In the case of reader, all
 *              attributes that were obtained since the first acquired
 *              are re-instated in the RingIO instance.
 */
Int
RingIOShm_cancel (RingIO_Handle handle);

/*!
 *  @brief      Get attribute from the RingIO instance
 *              This function gets an attribute with a variable-sized payload
 *              from the attribute buffer.
 *              If an attribute is present, the attribute type, the optional
 *              parameter, a pointer to the optional payload and the payload
 *              size are returned.
 */
Int
RingIOShm_getvAttribute (RingIO_Handle handle,
                      UInt16 *      type,
                      UInt32 *      param,
                      RingIO_BufPtr vptr,
                      UInt32 *      pSize);


/*!
 *  @brief      Set attribute in the RingIO instance
 * This function sets an attribute with a variable sized payload at
 * the offset provided in the acquired data buffer.
 * If the offset is not in the range of the acquired data buffer,
 *              the attribute is not set, and an error is returned. One
 *              exception to this rule is when no data buffer has been acquired.
 *              In this case an attribute is set at the next data buffer offset
 *              that can be acquired
 */
Int
RingIOShm_setvAttribute (RingIO_Handle handle,
                      UInt16        type,
                      UInt32        param,
                      RingIO_BufPtr pData,
                      UInt32        size,
                      Bool          sendNotification);


/*!
 *  @brief      This function gets a fixed-size attribute from the attribute
 *              buffer.
 *              If an attribute is present, the attribute type and a related
 *              parameter are returned.
 */
Int
RingIOShm_getAttribute (RingIO_Handle handle,
                        UInt16 *      type,
                        UInt32 *      param);

/*!
 *  @brief      This function sets a fixed-size attribute at the offset provided
 *              in the acquired data buffer.
 *              This function sets a fixed-size attribute at the offset provided
 *              in the acquired data buffer.
 *              If the offset is not in the range of the acquired data buffer,
 *              the attribute is not set, and an error is returned. One
 *              exception to this rule is when no data buffer has been acquired.
 *              In this case an attribute is set at the next data buffer offset
 *              that can be acquired
 */
Int
RingIOShm_setAttribute (RingIO_Handle handle,
                        UInt16        type,
                        UInt32        param,
                        Bool          sendNotification);


/*!
 *  @brief      Flush the buffer in the RingIO instance
 */
Int
RingIOShm_flush (RingIO_Handle handle,
                 Bool          hardFlush,
                 UInt16 *      type,
                 UInt32 *      param,
                 UInt32 *      bytesFlushed);

/*!
 *  @brief      Send a force notification to the other client manually
 */
Int
RingIOShm_notify (RingIO_Handle    handle,
                  RingIO_NotifyMsg msg);

/*!
 * @brief Function to get valid data buffer size.
 */
UInt
RingIOShm_getValidSize (RingIO_Handle   handle);

/*!
 * @brief Function to get empty data buffer size.
 */
UInt
RingIOShm_getEmptySize (RingIO_Handle   handle);

/*!
 * @brief Function to get valid attribute size.
 */
UInt
RingIOShm_getValidAttrSize (RingIO_Handle  handle);

/*!
 * @brief Function to get empty attribute size.
 */
UInt
RingIOShm_getEmptyAttrSize (RingIO_Handle   handle);

/*!
 * @brief Function to get client's acquired offset
 */
UInt
RingIOShm_getAcquiredOffset (RingIO_Handle   handle);

/*!
 * @brief Function to get client's acquired size
 */
UInt
RingIOShm_getAcquiredSize (RingIO_Handle   handle);

/*!
 * @brief Function to get client's  watermark
 */
UInt
RingIOShm_getWaterMark (RingIO_Handle   handle);
/*!
 * @brief Function to get CliNotifyMgrShAddr
 */
Ptr 
RingIOShm_getCliNotifyMgrShAddr (RingIO_Handle handle);
/*!
 * @brief Function to get CliNotifyMgrGateShAddr
 */
Ptr 
RingIOShm_getCliNotifyMgrGateShAddr (RingIO_Handle handle);
/*!
 * @brief Function to setNotifyId
 */
Int
RingIOShm_setNotifyId (RingIO_Handle         handle,
                        UInt32               notifyId,
                        RingIO_NotifyType    notifyType,
                        UInt32               watermark);
/*!
 * @brief Function to resetNotifyId
 */
Int
RingIOShm_resetNotifyId (RingIO_Handle handle, UInt32 notifyId);

#ifdef SYSLINK_BUILDOS_QNX
/*!
 *  @brief      Function to setup the RingIOShm module.
 *
 *              This function sets up the RingIOShm module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then RingIOShm_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call RingIOShm with NULL parameters.
 *              The default parameters would get automatically used.
 *
 *
 *  @sa         RingIOShm_destroy
 *              NameServer_create
 *              GateMutex_create
 *              Memory_alloc
 *              Memory_alloc
 */
Int   RingIOShm_setup (Void);

/*!
 *  @brief      Function to destroy the RingIOShm module.
 *
 *              Once this function is called, other RingIOShm module APIs, except
 *              for the RingIOShm_getConfig API cannot be called anymore.
 *
 *  @sa         RingIOShm_setup, NameServer_delete, GateMutex_delete
 */
Int   RingIOShm_destroy (Void);

#else
/*!
 *  @brief      Function to get the default configuration for the RingIOShm
 *              module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to RingIOShm_setup filled in by the
 *              RingIOShm module with the default parameters. If the user does
 *              not wish to make any change in the default parameters, this API
 *              is not required to be called.
 *
 *  @param      cfgParams Pointer to the RingIOShm module configuration
 *                        structure in which the default config is to be
 *                        returned.
 *
 *  @sa         RingIOShm_setup
 */
Void  RingIOShm_getConfig (RingIOShm_Config * cfgParams);

/*!
 *  @brief      Function to setup the RingIOShm module.
 *
 *              This function sets up the RingIOShm module. This function must
 *              be called before any other instance-level APIs can be invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then RingIOShm_getConfig can be called to get the
 *              configuration filled with the default values. After this, only
 *              the required configuration values can be changed. If the user
 *              does not wish to make any change in the default parameters, the
 *              application can simply call RingIOShm with NULL parameters.
 *              The default parameters would get automatically used.
 *
 *  @param      cfgParams   Optional RingIOShm module configuration. If provided
 as
 *                    NULL, default configuration is used.
 *
 *  @sa         RingIOShm_destroy
 *              NameServer_create
 *              GateMutex_create
 *              Memory_alloc
 */
Int   RingIOShm_setup (const RingIOShm_Config * cfgParams);

/*!
 *  @brief      Function to destroy the RingIOShm module.
 *
 *              Once this function is called, other RingIOShm module APIs,
 *              except for the RingIOShm_getConfig API cannot be called anymore.
 *
 *  @sa         RingIOShm_setup, NameServer_delete, GateMutex_delete
 */
Int   RingIOShm_destroy (Void);

#endif

/* =============================================================================
 *  Internal functions
 * =============================================================================
 */

/* Acquire a buffer from the _RingIOShm instance for the writer client */
Int32
_RingIOShm_writerAcquire (RingIOShm_Obj * object,
                          RingIO_BufPtr * pData,
                          UInt32 *        pSize);


/* Acquire a buffer from the _RingIOShm instance for the reader client */
Int32
_RingIOShm_readerAcquire (RingIOShm_Obj * object,
                          RingIO_BufPtr * pData,
                          UInt32 *        pSize);


/* Release a buffer to the RingIO instance for writer client */
Int32
_RingIOShm_writerRelease (RingIOShm_Obj * object,
                          UInt32        size);


/* Release a buffer to the RingIO instance for reader client  */
Int32
_RingIOShm_readerRelease (RingIOShm_Obj * object,
                          UInt32        size);


/* Cancel the previous acquired buffer to the RingIO instance for writer client */
Int32
_RingIOShm_writerCancel (RingIOShm_Obj * object);


/* Cancel the previous acquired buffer to the RingIO instance for reader client */
Int32
_RingIOShm_readerCancel (RingIOShm_Obj * object);


/* Get a variable sized attribute */
Int32
_RingIOShm_readergetvAttribute (RingIOShm_Obj * object,
                                UInt16 *      type,
                                UInt32 *      param,
                                RingIO_BufPtr vptr,
                                UInt32 *      pSize);


/* This function sets an attribute with a variable sized payload at the
 * offset 0 provided in the acquired data buffer.
 */
Int32
_RingIOShm_writersetvAttribute (RingIO_Handle handle,
                                UInt16        type,
                                UInt32        param,
                                RingIO_BufPtr pdata,
                                UInt32        size,
                                Bool          sendNotification);


/* Flushes the data buffer for writer client */
Int32
_RingIOShm_writerFlush (RingIO_Handle handle,
                        Bool          hardFlush,
                        UInt16 *      type,
                        UInt32 *      param,
                        UInt32 *      bytesFlushed);


/* Flushes the data buffer for reader client */
Int32
_RingIOShm_readerFlush (RingIO_Handle handle,
                        Bool          hardFlush,
                        UInt16 *      type,
                        UInt32 *      param,
                        UInt32 *      bytesFlushed);

/*!
 *  This function returns the next attribute that can be acquired by
 *  the reader or the writer
 */
RingIOShm_Attr *
_RingIOShm_getFirstAttr (RingIOShm_Obj * object);


/*!
 *  This function returns the amount of contiguous data available for
 *  the reader to acquire. This is the minimum of
 *  1) the number of bytes available before encountering next attribute
 *  2) the number of bytes before encountering end of buffer
 *  3) total valid data available in the buffer
 *  This should be called only by the reader.
 *
 */
Int32
_RingIOShm_getContiguousDataSize (RingIOShm_Obj * object,
                                  UInt32 *        size);


/*
 *  This function checks whether offset lies between start and end
 *  offsets. Note that this includes the start and end offsets
 *  themselves. Note that no check is made to verify whether the
 *  offset lies in the ring buffer.
 */
Bool
_RingIOShm_isOffsetInBetween (UInt32 offset,
                              UInt32 start,
                              UInt32 end,
                              Bool   isFull);


/*!
 *  This function initializes the RingIO instance identified by the
 *  control handle specified.
 */
Void
_RingIOShm_initInstance (RingIOShm_Obj * object);


/*!
 *  This function updates the attribute's offset in early end scenarios.
 */
Void
_RingIOShm_updateAttributes (RingIOShm_Obj * object, UInt32 offset);


/*!
 *  This function returns the first attribute that is acquired by
 *  the reader or the writer
 */
RingIOShm_Attr *
_RingIOShm_getFirstAcquiredAttr (RingIOShm_Obj * object);


/*!  This function releases an attribute.  */
Int32
_RingIOShm_releaseAttribute (RingIOShm_Obj * object,
                             RingIOShm_Attr   * attrBuf);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* _RINGIOSHM_H */

