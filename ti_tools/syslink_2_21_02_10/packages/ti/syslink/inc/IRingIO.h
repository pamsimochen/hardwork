/** 
 *  @file   IRingIO.h
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



#ifndef IRINGIO_H_0xd31a
#define IRINGIO_H_0xd31a


/* OSAL and utils */

#include <ti/syslink/utils/Trace.h>

/* Include generic RingIO defines. */
#include <ti/syslink/RingIO.h>


#if defined (__cplusplus)
extern "C" {
#endif



/* =============================================================================
 *  Forward declarations
 * =============================================================================
 */
/*! @brief Forward declaration of structure defining object for the
 *         RingIO module
 */
typedef struct IRingIO_Object_tag IRingIO_Object;

/*!
 *  @brief  Handle for the RingIO object.
 */
typedef IRingIO_Object * IRingIO_Handle;

/* =============================================================================
 *  Macros
 * =============================================================================
 */

/*! @brief Forward declaration of structure defining object for the
 *         RingIO module
 */
#define  RingIO_Object IRingIO_Object


/* =============================================================================
 *  Function pointer types for heap operations
 * =============================================================================
 */
/*! @brief Type for function pointer to delete a RingIO object for a specific
 *         slave processor.
 */
typedef Int (*IRingIO_deleteFxn) (IRingIO_Handle * handlePtr);

/*! @brief Type for function pointer to close previously opened instance of
 *         RingIO module.
 */
typedef Int (*IRingIO_closeFxn) (IRingIO_Handle * handlePtr);

/*
 * ! @brief Type for function pointer to register a call back function. */
typedef Int (*IRingIO_registerNotifierFxn) (IRingIO_Handle      handle,
                                            RingIO_NotifyType   notifyType,
                                            UInt32              watermark,
                                            RingIO_NotifyFxn    notifyFunc,
                                            Ptr                 cbContext);

/*! @brief Type for function pointer to unregister a call back function. */
typedef Int (*IRingIO_unregisterNotifierFxn) (IRingIO_Handle      handle);

/*! @brief Type for function pointer to set client's watermark */
typedef Int (*IRingIO_setWaterMarkFxn) (IRingIO_Handle handle,
                                        UInt32         watermark);

/*! @brief Type for function pointer to set the notify type */
typedef Int (*IRingIO_setNotifyTypeFxn) (IRingIO_Handle     handle,
                                         RingIO_NotifyType  notifyType);

/*! @brief Type for function pointer to acquire data from the RingIO instance */
typedef Int (*IRingIO_acquireFxn) (IRingIO_Handle   handle,
                                   RingIO_BufPtr *  pData,
                                   UInt32 *         pSize);

/*! @brief Type for function pointer to release data from the RingIO instance */
typedef Int (*IRingIO_releaseFxn) (IRingIO_Handle   handle,
                                   UInt32           size);

/*! @brief Type for function pointer to cancel the previous acquired buffer to
 *              the RingIO instance
 */
typedef Int (*IRingIO_cancelFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get attribute from the RingIO instance
 */
typedef Int (*IRingIO_getvAttributeFxn) (IRingIO_Handle handle,
                                         UInt16 *       type,
                                         UInt32 *       param,
                                         RingIO_BufPtr  vptr,
                                         UInt32 *       pSize);

/*! @brief Type for function pointer to set attribute in the RingIO instance */
typedef Int (*IRingIO_setvAttributeFxn) (IRingIO_Handle handle,
                                         UInt16         type,
                                         UInt32         param,
                                         RingIO_BufPtr  pData,
                                         UInt32         size,
                                         Bool           sendNotification);

/*! @brief Type for function pointer to get a fixed-size attribute from the
 *              attribute buffer.
 */
typedef Int (*IRingIO_getAttributeFxn) (IRingIO_Handle handle,
                                        UInt16 *       type,
                                        UInt32 *       param);

/*! @brief Type for function pointer to set a fixed-size attribute at the offset
 *              provided in the acquired data buffer.
 */
typedef Int (*IRingIO_setAttributeFxn)  (IRingIO_Handle handle,
                                         UInt16         type,
                                         UInt32         param,
                                         Bool           sendNotification);

/*! @brief Type for function pointer to flush the buffer in the RingIO instance
 */
typedef Int (*IRingIO_flushFxn) (IRingIO_Handle handle,
                                 Bool           hardFlush,
                                 UInt16 *       type,
                                 UInt32 *       param,
                                 UInt32 *       bytesFlushed);

/*! @brief Type for function pointer to send a force notification to the other
 *         client manually
 */
typedef Int (*IRingIO_sendNotifyFxn) (IRingIO_Handle   handle,
                                  RingIO_NotifyMsg msg);

/*! @brief Type for function pointer to get valid data buffer size. */
typedef UInt (*IRingIO_getValidSizeFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get empty data buffer size. */
typedef UInt (*IRingIO_getEmptySizeFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get valid attribute size. */
typedef UInt (*IRingIO_getValidAttrSizeFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get empty attribute size. */
typedef UInt (*IRingIO_getEmptyAttrSizeFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get client's acquired offset. */
typedef UInt (*IRingIO_getAcquiredOffsetFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get client's acquired size. */
typedef UInt (*IRingIO_getAcquiredSizeFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get client's watermark. */
typedef UInt (*IRingIO_getWaterMarkFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get client's watermark. */
typedef Ptr (*IRingIO_getCliNotifyMgrShAddrFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get client's watermark. */
typedef Ptr (*IRingIO_getCliNotifyMgrGateShAddrFxn) (IRingIO_Handle   handle);

/*! @brief Type for function pointer to get client's watermark. */
typedef Int (*IRingIO_setNotifyIdFxn) (IRingIO_Handle       handle,
                                       UInt32               notifyId,
                                       RingIO_NotifyType    notifyType,
                                       UInt32               watermark);



/*! @brief Type for function pointer to get client's watermark. */
typedef Int (*IRingIO_resetNotifyIdFxn) (IRingIO_Handle   handle, UInt32 notifyId);



/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */


/*!
 *  @brief  Structure for the Handle for the RingIO.
 */
struct IRingIO_Object_tag {
    List_Elem                         listElem;
    /*! List element for indexing in local list */
    UInt16                            ownerProcId;
    /*!< Processor Identifier */
    String                            name;
    /*!< Name of the instance */
    UInt32                            ctrlSharedAddr;
    /*!< Shared Region address for control structures */
    UInt16                            ctrlRegionId;
    /*!< Shared region ID for RingIOShm control structures */
    UInt16                            remoteProcId;
    /*!< Processor Identifier  */
    Ptr                               top;
    /*!< Pointer to the top object */
    Ptr                               nsKey;
    /*! NameServer key required for remove */
    Bool                              created;
    /*! Denotes whether this object was created through IRingIO_create */
    IRingIO_deleteFxn                 deleteFn;
    /*!<  delete function */
    IRingIO_closeFxn                  close;
    /*!<  Close function */
    IRingIO_registerNotifierFxn       registerNotifier;
    /*!<  RegisterNotifier function */
    IRingIO_unregisterNotifierFxn     unregisterNotifier;
    /*!<  UnregisterNotifier function */
    IRingIO_setWaterMarkFxn           setWaterMark;
    /*!<  SetWaterMark function */
    IRingIO_setNotifyTypeFxn          setNotifyType;
    /*!<  SetNotifierType function */
    IRingIO_acquireFxn                acquire;
    /*!<  Acquire function */
    IRingIO_releaseFxn                release;
    /*!<  Release function */
    IRingIO_cancelFxn                 cancel;
    /*!<  Cancel function */
    IRingIO_getvAttributeFxn          getvAttribute;
    /*!<  GetvAttribute function */
    IRingIO_setvAttributeFxn          setvAttribute;
    /*!<  SetvAttribute function */
    IRingIO_getAttributeFxn           getAttribute;
    /*!<  GetAttribute function */  
    IRingIO_setAttributeFxn           setAttribute;
    /*!<  SetvAttribute function */
    IRingIO_flushFxn                  flush;
    /*!<  Flush function */
    IRingIO_sendNotifyFxn                 notify;
    /*!<  Notify function */
    IRingIO_getValidSizeFxn           getValidSize;
    /*!<  GetValidSize function */
    IRingIO_getEmptySizeFxn           getEmptySize;
    /*!<  GetEmptySize function */
    IRingIO_getValidAttrSizeFxn       getValidAttrSize;
    /*!<  GetValidAttrSize function */
    IRingIO_getEmptyAttrSizeFxn       getEmptyAttrSize;
    /*!<  GetEmptyAttrSize function */
    IRingIO_getAcquiredOffsetFxn      getAcquiredOffset;
    /*!<  GetAcquiredOffset function */
    IRingIO_getAcquiredSizeFxn        getAcquiredSize;
    /*!<  GetAcquiredSize function */
    IRingIO_getWaterMarkFxn           getWaterMark;
    /*!<  GetWaterMark function */
    IRingIO_getCliNotifyMgrShAddrFxn  getCliNotifyMgrShAddr;
    /*!<  getCliNotifyMgrShAddr function */
    IRingIO_getCliNotifyMgrGateShAddrFxn getCliNotifyMgrGateShAddr;    
    /*!<  GetWaterMark function */
    IRingIO_setNotifyIdFxn             setNotifyId;    
    /*!<  GetWaterMark function */
    IRingIO_resetNotifyIdFxn           resetNotifyId;    
    /*!<  GetWaterMark function */
    Ptr                                obj;
    /*!<  Actual RingIO object handle specific to the implementation */
};


/* =============================================================================
 *  Extern declarations
 * =============================================================================
 */
/*! @brief Extern declaration of configured functions.
 */
extern RingIO_CfgFxns  SysLink_RingIO_cfgFxns[1];

/* =============================================================================
 *  APIs
 * =============================================================================
 */
/*!
 *  @brief      Function to delete a RingIO object for a specific slave
 *              processor.
 */
static inline Int IRingIO_delete (IRingIO_Handle * handlePtr)
{
    Int               status = RingIO_S_SUCCESS;
    IRingIO_Object *  handle = (IRingIO_Object * )(*handlePtr);

    GT_assert (curTrace, (((IRingIO_Object *) handle)->deleteFn != NULL));
    
    status = handle->deleteFn (handlePtr);
    return (status);
}

/*!
 *  @brief      Function to open a created instance of RingIO module.
 */
static inline Int IRingIO_openByAddr   (      Ptr                 ctrlSharedAddr,
                                        const RingIO_openParams * params,
                                              RingIO_Handle     * handlePtr,
                                              UInt16              type)
{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (SysLink_RingIO_cfgFxns [type].openByAddr != NULL));
    
    status = SysLink_RingIO_cfgFxns[type].openByAddr (ctrlSharedAddr,
                                                      params,
                                                      handlePtr);

    return (status);
}

/*!
 *  @brief      Function to close previously opened/created instance of RingIO
 *              module.
 */
static inline Int IRingIO_close (IRingIO_Handle * handlePtr)
{
    Int status = RingIO_S_SUCCESS;
    IRingIO_Object *  handle = (IRingIO_Object * )(*handlePtr);

    GT_assert (curTrace, (((IRingIO_Object *) handle)->close != NULL));
    
    status = handle->close (handlePtr);
    return (status);
}


/*!
 *  @brief      Function to register a call back function.
 */
static inline Int IRingIO_registerNotifier(IRingIO_Handle      handle,
                                           RingIO_NotifyType  notifyType,
                                           UInt32             watermark,
                                           RingIO_NotifyFxn   notifyFunc,
                                           Ptr                cbContext)


{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->registerNotifier != NULL));
    
    status =  ((IRingIO_Object *)handle)->registerNotifier (handle,
                                                            notifyType,
                                                            watermark,
                                                            notifyFunc,
                                                            cbContext);

    return (status);
}


/*!
 *  @brief      Function to un-register call back function.
 */
static inline Int IRingIO_unregisterNotifier (IRingIO_Handle     handle)
{
    Int status = RingIO_S_SUCCESS;

   GT_assert (curTrace,
                    (((IRingIO_Object *) handle)->unregisterNotifier != NULL));
                    
    status = ((IRingIO_Object *) handle)->unregisterNotifier (handle);
    return (status);
}


/*!
 *  @brief      Function to to set client's  watermark
 */
static inline Int IRingIO_setWaterMark(IRingIO_Handle      handle,
                                       UInt32             watermark)
{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->setWaterMark != NULL));
    
    status = ((IRingIO_Object *) handle)->setWaterMark ( handle,
                                                         watermark);

    return (status);
}


/*!
 *  @brief      Function to  to set the notify type
 */
static inline Int IRingIO_setNotifyType(IRingIO_Handle       handle,
                                        RingIO_NotifyType    notifyType)

{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->setNotifyType != NULL));
   
    status = ((IRingIO_Object *) handle)->setNotifyType (handle,
                                                         notifyType);

    return (status);
}


/*!
 *  @brief      Function to  Acquires data from the RingIO instance
 */
static inline Int IRingIO_acquire(IRingIO_Handle   handle,
                                  RingIO_BufPtr * pData,
                                  UInt32 *        pSize)
{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->acquire != NULL));
    
    status = ((IRingIO_Object *) handle)->acquire (handle,
                                                   pData,
                                                   pSize);

    return (status);
}


/*!
 *  @brief      Function to releases data from the RingIO instance
 */
static inline Int IRingIO_release(IRingIO_Handle handle,
                                  UInt32        size)
{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->release != NULL));
    
    status = ((IRingIO_Object *) handle)->release (handle,
                                                   size);

    return (status);
}


/*!
 *  @brief      Function to cancel the previous acquired buffer to
 *              the RingIO instance
 */
static inline Int IRingIO_cancel(IRingIO_Handle handle)
{
    Int status = RingIO_S_SUCCESS;


    GT_assert (curTrace, (((IRingIO_Object *) handle)->cancel != NULL));
    
    status = ((IRingIO_Object * )handle)->cancel (handle);
    return (status);
}


/*!
 *  @brief      Function to Get attribute from the RingIO instance
 */
static inline Int IRingIO_getvAttribute(IRingIO_Handle handle,
                                        UInt16 *      type,
                                        UInt32 *      param,
                                        RingIO_BufPtr vptr,
                                        UInt32 *      pSize)

{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->getvAttribute != NULL));
    
    status = ((IRingIO_Object *) handle)->getvAttribute (handle,
                                                         type,
                                                         param,
                                                         vptr,
                                                         pSize);

    return (status);
}


/*!
 *  @brief      Function to set attribute in the RingIO instance
 */
static inline Int IRingIO_setvAttribute(IRingIO_Handle handle,
                                        UInt16        type,
                                        UInt32        param,
                                        RingIO_BufPtr pData,
                                        UInt32        size,
                                        Bool          sendNotification)

{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->setvAttribute != NULL));
    
    status = ((IRingIO_Object *) handle)->setvAttribute (handle,
                                                         type,
                                                         param,
                                                         pData,
                                                         size,
                                                         sendNotification);

    return (status);
}
/*!
 *  @brief      Function to gets a fixed-size attribute from the attribute
 *              buffer.
 */
static inline Int IRingIO_getAttribute(IRingIO_Handle handle,
                                       UInt16 *       type,
                                       UInt32 *       param)

{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->getAttribute != NULL));
    
    status = ((IRingIO_Object *) handle)->getAttribute (handle,
                                                        type,
                                                        param);


    return (status);
}
/*!
 *  @brief      Function to set a fixed-size attribute at the offset provided
 *              in the acquired data buffer.
 */
static inline Int IRingIO_setAttribute(IRingIO_Handle handle,
                                       UInt16        type,
                                       UInt32        param,
                                       Bool          sendNotification)


{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->setAttribute != NULL));
    
    status = ((IRingIO_Object *) handle)->setAttribute (handle,
                                                        type,
                                                        param,
                                                        sendNotification);

    return (status);
}


/*!
 *  @brief      Function to flush the buffer in the RingIO instance
 */
static inline Int IRingIO_flush(IRingIO_Handle handle,
                                Bool           hardFlush,
                                UInt16 *       type,
                                UInt32 *       param,
                                UInt32 *       bytesFlushed)
{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->flush != NULL));
    
    status = ((IRingIO_Object *) handle)->flush (handle,
                                                 hardFlush,
                                                 type,
                                                 param,
                                                 bytesFlushed);

    return (status);
}


/*!
 *  @brief      Function to force notification to the other client manually
 */
static inline Int IRingIO_sendNotify(IRingIO_Handle    handle,
                                 RingIO_NotifyMsg  msg)


{
    Int status = RingIO_S_SUCCESS;

    GT_assert (curTrace, (((IRingIO_Object *) handle)->notify != NULL));
    
    status = ((IRingIO_Object *) handle)->notify ( handle,
                                                   msg);

    return (status);
}


/*!
 *  @brief      Function to get valid data buffer size
 */
static inline UInt IRingIO_getValidSize(IRingIO_Handle handle)
{

   GT_assert (curTrace, (((IRingIO_Object *) handle)->getValidSize != NULL));
   
   return (((IRingIO_Object * )handle)->getValidSize (handle));
}


/*!
 *  @brief      Function to get empty data buffer size.
 */
static inline UInt IRingIO_getEmptySize(IRingIO_Handle handle)
{
   GT_assert (curTrace, (((IRingIO_Object *) handle)->getEmptySize != NULL));
   
   return (((IRingIO_Object * )handle)->getEmptySize (handle));
}


/*!
 *  @brief      Function to to get valid attribute size.
 */
static inline UInt IRingIO_getValidAttrSize(IRingIO_Handle handle)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getValidAttrSize != NULL));
    
    return (((IRingIO_Object * )handle)->getValidAttrSize (handle));
}


/*!
 *  @brief      Function to get empty attribute size.
 */
static inline UInt IRingIO_getEmptyAttrSize(IRingIO_Handle handle)
{

    GT_assert (curTrace, (((IRingIO_Object *) handle)->getEmptyAttrSize != NULL));
    
    return (((IRingIO_Object * )handle)->getEmptyAttrSize (handle));
}


/*!
 *  @brief      Function to get client's acquired offset
 */
static inline UInt IRingIO_getAcquiredOffset(IRingIO_Handle handle)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getAcquiredOffset != NULL));    
    
    return (((IRingIO_Object * )handle)->getAcquiredOffset (handle));
}


/*!
 *  @brief      Function to get client's acquired size
 */
static inline UInt IRingIO_getAcquiredSize(IRingIO_Handle  handle)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getAcquiredSize != NULL));
    
    return (((IRingIO_Object * )handle)->getAcquiredSize (handle));
}
/*!
 *  @brief      Function to get client's  watermark
 */
static inline UInt IRingIO_getWaterMark (IRingIO_Handle  handle)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getWaterMark != NULL));
    
    return (((IRingIO_Object * )handle)->getWaterMark (handle));
}
/*!
 *  @brief      Function to getCliNotifyMgrShAddr
 */
static inline Ptr IRingIO_getCliNotifyMgrShAddr (IRingIO_Handle  handle)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getWaterMark != NULL));
    
    return (((IRingIO_Object * )handle)->getCliNotifyMgrShAddr (handle));
}
/*!
 *  @brief      Function to getCliNotifyMgrGateShAddr
 */
static inline Ptr IRingIO_getCliNotifyMgrGateShAddr (IRingIO_Handle  handle)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getWaterMark != NULL));
    
    return (((IRingIO_Object * )handle)->getCliNotifyMgrGateShAddr (handle));
}
/*!
 *  @brief      Function to getCliNotifyMgrGateShAddr
 */
static inline Int IRingIO_setNotifyId(IRingIO_Handle          handle,
                                      UInt32                 notifyId,
                                      RingIO_NotifyType      notifyType,
                                      UInt32                 watermark)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getWaterMark != NULL));
    
    return (((IRingIO_Object * )handle)->setNotifyId(handle,
                                                     notifyId,
                                                     notifyType,
                                                     watermark));
}
/*!
 *  @brief      Function to getCliNotifyMgrGateShAddr
 */
static inline Int IRingIO_resetNotifyId (IRingIO_Handle  handle,
                                         UInt32          notifyId)
{
    GT_assert (curTrace, (((IRingIO_Object *) handle)->getWaterMark != NULL));
    
    return (((IRingIO_Object * )handle)->resetNotifyId (handle, notifyId));
}


/*!
 * @brief     Get shared memory requirements
 */
static inline UInt32 IRingIO_sharedMemReq (
                          const Void *                   params,
                          RingIO_sharedMemReqDetails  * sharedMemReqDetails)
{
    RingIO_Params * rParams = (RingIO_Params *)params;
    UInt32 totalSize;

    GT_assert (curTrace, (rParams != NULL));

    /* Call the configured function to drop down into the appropriate
     * RingIO delegate implementation.
     */

    GT_assert (curTrace, (SysLink_RingIO_cfgFxns[rParams->interfaceType].sharedMemReq  != NULL));
    
    totalSize = SysLink_RingIO_cfgFxns[rParams->interfaceType].sharedMemReq (params, sharedMemReqDetails);

    return (totalSize);
}


/*!
 *  @brief      Function to create a RingIO object for a specific slave
 *              processor.
 */
static inline Int IRingIO_create (const Void * params, RingIO_Handle * handle)
{
    RingIO_Params * rParams = NULL;

    GT_assert (curTrace, (params != NULL));
    
    rParams = (RingIO_Params *)params;

    /* Call the configured function to drop down into the appropriate
     * RingIO delegate implementation.
     */
    GT_assert (curTrace, (SysLink_RingIO_cfgFxns[rParams->interfaceType].create != NULL)); 
    
    return (SysLink_RingIO_cfgFxns[rParams->interfaceType].create (params, handle));
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* IRINGIO_H */

