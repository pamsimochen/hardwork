/** 
 *  @file   TransportShm.h
 *
 *  @brief      MessageQ shared memory based physical transport for
 *              communication with the remote processor.
 *
 *              This file contains the declarations of types and APIs as part
 *              of interface of the MessageQ shared memory transport.
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



#ifndef TRANSPORTSHM_H_0x0a7a
#define TRANSPORTSHM_H_0x0a7a


/* Standard headers */
#include <ti/ipc/GateMP.h>
#include "IMessageQTransport.h"


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  All success and failure codes for the module: Use MessageQ codes.
 * =============================================================================
 */
/*!
 *  @def    TransportShm_MODULEID
 *  @brief  Unique module ID.
 */
#define TransportShm_MODULEID               (0x0a7a)


/* =============================================================================
 *  Forward declarations
 * =============================================================================
 */
/*
 *  @brief  Object for the TransportShm Handle
 */
#define TransportShm_Object IMessageQTransport_Object

/*!
 *  @brief  TransportShm_Handle type
 */
typedef TransportShm_Object * TransportShm_Handle;

/*!
 *  @brief  TransportShm error function
 */
#define TransportShm_ErrFxn IMessageQTransport_ErrFxn


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief  Module configuration structure.
 */
typedef struct TransportShm_Config {
    TransportShm_ErrFxn errFxn;
    /*!< Asynchronous error function for the transport module */
    UInt32       notifyEventId;
    /*!< Notify event number to be used for the module */
} TransportShm_Config;

/*!
 *  @brief  Structure defining config parameters for the MessageQ transport
 *  instances.
 */
typedef struct TransportShm_Params_tag {
    UInt            priority;
    /*!<  Priority of messages supported by this transport */
    GateMP_Handle   gate;
    /*!< Gate used for critical region management of the shared memory */
    Ptr             sharedAddr;
    /*!<  Address of the shared memory. The creator must supply the shared
     *    memory that this will use for maintain shared state information.
     */
} TransportShm_Params;



/* =============================================================================
 *  APIs called by applications
 * =============================================================================
 */
/* Function to get the default configuration for the TransportShm
 * module.
 */
Void TransportShm_getConfig (TransportShm_Config * cfg);

/* Function to setup the TransportShm module. */
Int TransportShm_setup (const TransportShm_Config * cfg);

/* Function to destroy the TransportShm module. */
Int TransportShm_destroy (Void);

/* Get the default parameters for the NotifyShmDriver. */
Void TransportShm_Params_init (TransportShm_Params    * params);

/* Create an instance of the TransportShm. */
TransportShm_Handle
TransportShm_create (      UInt16                 procId,
                     const TransportShm_Params  * params);

/* Delete an instance of the TransportShm. */
Int TransportShm_delete (TransportShm_Handle * handlePtr);

/* Open a created TransportShm instance by address */
Int TransportShm_openByAddr (Ptr sharedAddr, TransportShm_Handle * handlePtr);

/* Close an opened instance */
Int TransportShm_close (TransportShm_Handle * handlePtr);

/* Get the shared memory requirements for the TransportShm. */
SizeT
TransportShm_sharedMemReq (const TransportShm_Params * params);

/* Set the asynchronous error function for the transport module */
Void TransportShm_setErrFxn (TransportShm_ErrFxn errFxn);


/* =============================================================================
 *  APIs called internally by MessageQ module through function table.
 * =============================================================================
 */
/* Put msg to remote list */
Int TransportShm_put (TransportShm_Handle   handle,
                      Ptr                   msg);

/* Get current status of the TransportShm */
Int
TransportShm_getStatus (TransportShm_Handle handle);

/* Control Function */
Int TransportShm_control (TransportShm_Handle handle,
                          UInt                cmd,
                          UArg                cmdArg);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* TRANSPORTSHM_H_0x0a7a */

