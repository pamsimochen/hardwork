/** 
 *  @file   _NotifyDriverShm.h
 *
 *  @brief      Internal types and structures for Notify shared memory driver.
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



#if !defined (_NOTIFY_SHMDRIVER_H_0xb9d4)
#define _NOTIFY_SHMDRIVER_H_0xb9d4


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Defines the structure of event entry within the event chart.
 *          Each entry contains occured event-specific information.
 *          Used to flag a remote event and determine if a local event has been
 *          flagged. This struct is placed in shared memory.
 */
typedef struct NotifyDriverShm_EventEntry_tag {
    volatile Bits32  flag;
    /*!< Flag indicating whether event is set */
    volatile Bits32  payload;
    /*!< Payload associated with event */
    volatile Bits32  reserved;
    /*!< Reserved field */
    /* Padding for cache line alignment */
} NotifyDriverShm_EventEntry;

/*!
 *  @brief   Defines the NotifyDriverShm control structure, which contains all
 *           information for one processor.
 *           This structure is shared between the two processors.
 */
typedef struct NotifyDriverShm_ProcCtrl_tag {
    volatile Bits32 recvInitStatus;
    /*!< Initialization status for receiving events */
    volatile Bits32 sendInitStatus;
    /*!< Initialization status for sending events */
    volatile Bits32 eventRegMask;
    /*!< Event Registered mask */
    volatile Bits32 eventEnableMask;
    /*!< Event Enabled mask */
} NotifyDriverShm_ProcCtrl ;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
Int NotifyDriverShm_registerEvent (NotifyDriverShm_Handle handle,
                                   UInt32              eventId);

/* Function to unregister an event */
Int NotifyDriverShm_unregisterEvent (NotifyDriverShm_Handle handle,
                                     UInt32              eventId);

/* Function to send an event to other processor */
Int NotifyDriverShm_sendEvent (NotifyDriverShm_Handle handle,
                               UInt32              eventId,
                               UInt32              payload,
                               Bool                waitClear);

/* Function to disable Notify module */
Int NotifyDriverShm_disable (NotifyDriverShm_Handle handle);

/* Function to enable Notify module state */
Void NotifyDriverShm_enable (NotifyDriverShm_Handle handle);

/* Function to disable particular event */
Void  NotifyDriverShm_disableEvent (NotifyDriverShm_Handle handle,
                                    UInt32              eventId);

/* Function to enable particular event */
Void  NotifyDriverShm_enableEvent (NotifyDriverShm_Handle handle,
                                   UInt32              eventId);

/* Function to set Notify object handle within the driver. */
Void  NotifyDriverShm_setNotifyHandle (NotifyDriverShm_Handle handle,
                                       Ptr                 driverHanlde);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (_NOTIFY_SHMDRIVER_H_0xb9d4) */

