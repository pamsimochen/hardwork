/** 
 *  @file   _NotifyDefs.h
 *
 *  @brief      Internal declarations and definitions of types and structures.
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




#if !defined (_NOTIFYDEFS_H_0x5f84)
#define _NOTIFYDEFS_H_0x5f84


/* Osal And Utils  headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/inc/Bitops.h>

/* Module headers */
#include <ti/ipc/Notify.h>
#include <ti/syslink/inc/_Notify.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/inc/INotifyDriver.h>


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*! @brief Macro to make a correct module magic number with refCount */
#define Notify_MAKE_MAGICSTAMP(x) ((Notify_MODULEID << 12u) | (x))

/*!
 *  @brief  Maximum number of Notify drivers supported.
 */
#define Notify_MAX_DRIVERS   16u

/*!
 *  @brief  Mask to check for system key.
 */
#define Notify_SYSTEMKEY_MASK  (UInt16) 0xFFFF0000

/*!
 *  @brief   Defines the Event callback information instance
 */
typedef struct Notify_EventCallback_tag {
    Notify_FnNotifyCbck fnNotifyCbck;
    /*!< Callback function pointer */
    UArg                cbckArg;
    /*!< Argument associated with callback function */
} Notify_EventCallback;

/*!
 *  @brief   Defines the Notify state object, which contains all the module
 *           specific information.
 */
typedef struct Notify_ModuleObject_tag {
    Atomic             refCount;
    /*!< Reference count */
    Notify_Config      cfg;
    /*!< Notify configuration structure */
    Notify_Config      defCfg;
    /*!< Default module configuration */
    IGateProvider_Handle gateHandle;
    /*!< Handle of gate to be used for local thread safety */
    Notify_Handle      notifyHandles [MultiProc_MAXPROCESSORS][Notify_MAX_INTLINES];
    /*!< Array of configured drivers. */
    Bits32             localEnableMask;
    /*!< This is used for local/loopback events. Default to enabled (-1) */
    Bool               startComplete;
    /*!< TRUE if start() was called */
    Bool               isSetup;
    /*!< Indicates whether the Notify module is setup. */
    Notify_Handle      localNotifyHandle;
    /*!< Handle to Notify object for local notifications. */
} Notify_ModuleObject;

/*!
 *  @brief   Defines the Notify instance object.
 */
struct Notify_Object_tag {
    UInt                 nesting;
    /*!< Disable/restore nesting */
    UInt16               remoteProcId;
    /*!< Remote MultiProc id        */
    UInt16               lineId;
    /*!< Interrupt line id          */
    Notify_EventCallback callbacks [Notify_MAXEVENTS];
    /*!< List of event callbacks registered */
    List_Object          eventList [Notify_MAXEVENTS];
    /*!< List of event listeners registered */
    INotifyDriver_Handle driverHandle;
    /*!< INotifyDriver object. */
};


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (_NOTIFYDEFS_H_0x5f84) */
