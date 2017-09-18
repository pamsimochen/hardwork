/** 
 *  @file   INotifyDriver.h
 *
 *  @brief      Defines types and functions that interface the specific
 *              implementations of Notify drivers to the Notify module.
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




#if !defined (INotifyDriver_H)
#define INotifyDriver_H


/* Utils & OSAL */
#include <ti/syslink/utils/Trace.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* =============================================================================
 *  Forward declarations
 * =============================================================================
 */
/*! @brief Forward declaration of structure defining object for the
 *         NotifyDriver module
 */
typedef struct INotifyDriver_Object_tag INotifyDriver_Object;

/*!
 *  @brief  Handle for the NotifyDriver.
 */
typedef INotifyDriver_Object * INotifyDriver_Handle;

/* =============================================================================
 *  Function pointer types
 * =============================================================================
 */
/*!
 *  @brief  This type defines the function to register a callback for an event
 *          with the Notify driver.
 *          This function gets called internally from the Notify_registerEvent
 *          API. The Notify_registerEvent () function passes on the
 *          request into the Notify driver identified by the Notify Handle.
 *
 */
typedef Int (*INotifyDriver_RegisterEventFxn) (INotifyDriver_Handle handle,
                                               UInt32               eventId);
/*!
 *  @brief  This type defines the function to unregister a callback for an event
 *          with the Notify driver.
 *          This function gets called internally from the Notify_unregisterEvent
 *          API. The Notify_unregisterEvent () function passes on the
 *          request into the Notify driver identified by the Notify Handle.
 *
 */
typedef Int (*INotifyDriver_UnregisterEventFxn) (INotifyDriver_Handle handle,
                                                 UInt32               eventId);

/*!
 *  @brief  This type defines the function to send a notification event to the
 *          registered users for this notification on this driver.
 *          This function gets called internally from the Notify_sendEvent ()
 *          API. The Notify_sendEvent () function passes on the initialization
 *          request into the Notify driver identified by the Notify Handle.
 */
typedef Int (*INotifyDriver_SendEventFxn) (INotifyDriver_Handle handle,
                                           UInt32               eventId,
                                           UInt32               payload,
                                           Bool                 waitClear);

/*!
 *  @brief  This type defines the function to disable all events for the driver.
 *          This function gets called internally from the Notify_disable ()
 *          API. The Notify_disable () function passes on the request into the
 *          Notify driver identified by the Notify Handle.
 */
typedef Int (*INotifyDriver_DisableFxn) (INotifyDriver_Handle handle);

/*!
 *  @brief  This type defines the function to restore all events for the driver.
 *          This function gets called internally from the Notify_restore ()
 *          API. The Notify_restore () function passes on the request into the
 *          Notify driver identified by the Notify Handle.
 */
typedef Void (*INotifyDriver_EnableFxn) (INotifyDriver_Handle handle);

/*!
 *  @brief  This type defines the function to disable specified event for the
 *          specified driver.
 *          This function gets called internally from the Notify_disableEvent ()
 *          API. The Notify_disableEvent () function passes on the request into
 *          the Notify driver identified by the Notify Handle.
 */
typedef Void (*INotifyDriver_DisableEventFxn) (INotifyDriver_Handle handle,
                                               UInt32               eventId);

/*!
 *  @brief  This type defines the function to enable specified event for the
 *          specified driver.
 *          This function gets called internally from the Notify_enableEvent ()
 *          API. The Notify_enableEvent () function passes on the request into
 *          the Notify driver identified by the Notify Handle.
 *
 */
typedef Void (*INotifyDriver_EnableEventFxn) (INotifyDriver_Handle handle,
                                              UInt32               eventId);

/*!
 *  @brief  This type defines the function to set Notify handle within the
 *          driver handle.
 */
typedef Void (*INotifyDriver_SetNotifyHandleFxn) (INotifyDriver_Handle handle,
                                                  Ptr             driverHandle);



/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief  Function table for the Notify drivers.
 */
typedef struct INotifyDriver_FxnTable_tag {
    INotifyDriver_RegisterEventFxn   registerEvent;
    /*!< interface function registerEvent  */
    INotifyDriver_UnregisterEventFxn unregisterEvent;
    /*!< interface function unregisterEvent  */
    INotifyDriver_SendEventFxn       sendEvent;
    /*!< interface function sendEvent  */
    INotifyDriver_DisableFxn         disable;
    /*!< interface function disable  */
    INotifyDriver_EnableFxn          enable;
    /*!< interface function enable  */
    INotifyDriver_DisableEventFxn    disableEvent;
    /*!< interface function disableEvent  */
    INotifyDriver_EnableEventFxn     enableEvent;
    /*!< interface function enableEvent */
    INotifyDriver_SetNotifyHandleFxn setNotifyHandle;
    /*!< interface function setNotifyHandle */
}INotifyDriver_FxnTable;

/*!
 *  @brief  Structure for the Handle for the transport.
 */
struct INotifyDriver_Object_tag {
    INotifyDriver_FxnTable fxnTable;
    /*!< interface function setNotifyHandle */
    Ptr                    obj;
    /*!<  Actual driver Handle */
};


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Register a callback for an event with the Notify driver. */
static inline Int INotifyDriver_registerEvent (INotifyDriver_Handle    handle,
                                               UInt32                  eventId)
{
    Int status = Notify_S_SUCCESS;
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_registerEvent",
                             status,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.registerEvent != NULL));
        status = obj->fxnTable.registerEvent (obj->obj, eventId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    return (status);
}


/* Unregister a callback for an event with the Notify driver. */
static inline Int
INotifyDriver_unregisterEvent (INotifyDriver_Handle handle,
                               UInt32               eventId)
{
    Int status = Notify_S_SUCCESS;
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_unregisterEvent",
                             status,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.unregisterEvent != NULL));
        status = obj->fxnTable.unregisterEvent(obj->obj, eventId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    return status;
}


/* Send a notification event to the registered users for this notification on
 * this driver.
 */
static inline Int INotifyDriver_sendEvent (INotifyDriver_Handle handle,
                                           UInt32               eventId,
                                           UInt32               payload,
                                           Bool                 waitClear)
{
    Int status = Notify_S_SUCCESS;
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_sendEvent",
                             status,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.sendEvent != NULL));
        status = obj->fxnTable.sendEvent (obj->obj, eventId, payload, waitClear);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    return status;
}


/* Disable all events for the driver. */
static inline Int INotifyDriver_disable (INotifyDriver_Handle handle)
{
    Int status = Notify_S_SUCCESS;
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        status = Notify_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_disable",
                             status,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.disable != NULL));
        status = obj->fxnTable.disable (obj->obj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    return status;
}


/* Enable all events for the driver. */
static inline Void INotifyDriver_enable (INotifyDriver_Handle handle)
{
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_enable",
                             Notify_E_INVALIDSTATE,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.enable != NULL));
        obj->fxnTable.enable (obj->obj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
}


/* Disable specified event for the specified driver. */
static inline Void INotifyDriver_disableEvent (INotifyDriver_Handle handle,
                                               UInt32              eventId)
{
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_disableEvent",
                             Notify_E_INVALIDSTATE,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.disableEvent != NULL));
        obj->fxnTable.disableEvent (obj->obj, eventId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
}


/* Enable specified event for the specified driver. */
static inline Void INotifyDriver_enableEvent (INotifyDriver_Handle handle,
                                              UInt32              eventId)
{
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_enableEvent",
                             Notify_E_INVALIDSTATE,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.enableEvent != NULL));
        obj->fxnTable.enableEvent (obj->obj, eventId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
}


/* Set Notify object handle within the driver. */
static inline Void INotifyDriver_setNotifyHandle (INotifyDriver_Handle handle,
                                                  Ptr              driverHandle)
{
    INotifyDriver_Object * obj = (INotifyDriver_Object *) handle;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (handle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "INotifyDriver_setNotifyHandle",
                             Notify_E_INVALIDSTATE,
                             "Invalid NULL INotifyDriver_Handle");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GT_assert (curTrace, (obj->fxnTable.setNotifyHandle != NULL));
        obj->fxnTable.setNotifyHandle (obj->obj, driverHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
}


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (INotifyDriver_H) */
