/** 
 *  @file   NotifyDriverShm.h
 *
 *  @brief      Notify shared memory driver header
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



#if !defined (NOTIFYDRIVERSHM_H_0xb9d4)
#define NOTIFYDRIVERSHM_H_0xb9d4


/* Module headers. */
#include <ti/ipc/Notify.h>
#include "INotifyDriver.h"


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/*!
 *  @def    NOTIFYDRIVERSHM_MODULEID
 *  @brief  Module ID for NotifyDriverShm.
 */
#define NOTIFYDRIVERSHM_MODULEID           (UInt16) 0xb9d4


/* =============================================================================
 *  Drivers shall use Notify module success/failure codes.
 * =============================================================================
 */

/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Module configuration structure.
 */
typedef struct NotifyDriverShm_Config {
    UInt32 reserved;
    /*!< Reserved value */
} NotifyDriverShm_Config;

/*!
 *  @brief   This structure defines the configuration structure for
 *           initialization of the Notify driver.
 */
typedef struct NotifyDriverShm_Params_tag {
    Ptr       sharedAddr;
    /*!< Address in shared memory where this instance will be placed */
    Bool      cacheEnabled;
    /*!< Whether cache operations will be performed */
    Bool      cacheLineSize;
    /*!< The cache line size of the shared memory */
    UInt32    remoteProcId;
    /*!< Processor Id of remote processor required for communication */
    UInt32    lineId;
    /*!< Line ID for the interrupt */
    UInt32    localIntId;
    /*!< Local interrupt ID for interrupt line for incoming interrupts */
    UInt32    remoteIntId;
    /*!< Remote interrupt ID for interrupt line for outgoing interrupts */
} NotifyDriverShm_Params;


/*!
 *  @brief  Defines the type for the handle to the NotifyDriverShm
 */
typedef struct NotifyDriverShm_Object_tag NotifyDriverShm_Object;
typedef Ptr NotifyDriverShm_Handle;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Function to get the default configuration for the NotifyDriverShm module. */
Void NotifyDriverShm_getConfig (NotifyDriverShm_Config * cfg);

/* Function to setup the NotifyDriverShm module. */
Int NotifyDriverShm_setup (NotifyDriverShm_Config * cfg);

/* Function to destroy the NotifyDriverShm module. */
Int NotifyDriverShm_destroy (Void);

/* Get the default parameters for the NotifyShmDriver. */
Void NotifyDriverShm_Params_init (NotifyDriverShm_Params * params);

/* Create an instance of the NotifyShmDriver. */
INotifyDriver_Handle
NotifyDriverShm_create (const NotifyDriverShm_Params * params);

/* Function to exit from Notify driver */
Int NotifyDriverShm_delete (INotifyDriver_Handle * handle);

/* Get the shared memory requirements for the NotifyDriverShm. */
SizeT NotifyDriverShm_sharedMemReq (const NotifyDriverShm_Params * params);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif  /* !defined (NOTIFYDRIVERSHM_H_0xb9d4) */

/* Delete an instance of the NotifyShmDriver. */
