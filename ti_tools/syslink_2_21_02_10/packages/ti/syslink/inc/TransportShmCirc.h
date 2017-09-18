/** 
 *  @file   TransportShmCirc.h
 *
 *  @brief      MessageQ shared memory based circular buffer physical transport
 *              for communication with the remote processor.
 *
 *              This file contains the declarations of types and APIs as part
 *              of interface of the MessageQ shared memory circular buffer
 *              transport.
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



#ifndef TRANSPORTSHMCIRC_H_0x0a7a
#define TRANSPORTSHMCIRC_H_0x0a7a


/* Standard headers */
#include "IMessageQTransport.h"


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  All success and failure codes for the module: Use MessageQ codes.
 * =============================================================================
 */
/*!
 *  @def    TransportShmCirc_MODULEID
 *  @brief  Unique module ID.
 */
#define TransportShmCirc_MODULEID               (0x0a7c)


/* =============================================================================
 *  Forward declarations
 * =============================================================================
 */
/*
 *  @brief  Object for the TransportShmCirc Handle
 */
#define TransportShmCirc_Object IMessageQTransport_Object

/*!
 *  @brief  TransportShmCirc_Handle type
 */
typedef TransportShmCirc_Object * TransportShmCirc_Handle;

/*!
 *  @brief  TransportShmCirc error function
 */
#define TransportShmCirc_ErrFxn IMessageQTransport_ErrFxn


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/*!
 *  @brief  Module configuration structure.
 */
typedef struct TransportShmCirc_Config {
    TransportShmCirc_ErrFxn errFxn;
    /*!< Asynchronous error function for the transport module */
    UInt32                  notifyEventId;
    /*!< Notify event number to be used for the module */
    UInt32                  numMsgs;
    /*!< The number of messages or slots in the circular buffer
     *
     *  This is use to determine the size of the put and get buffers.
     *  Each eventEntry is two 32bits wide, therefore the total size
     *  of each circular buffer is [numMsgs * sizeof(eventEntry)].
     *  The total size of each buffer must be a multiple of the
     *  the cache line size. For example, if the cacheLineSize = 128
     *  then numMsgs could be 16, 32, etc...
     */
} TransportShmCirc_Config;

/*!
 *  @brief  Structure defining config parameters for the MessageQ transport
 *  instances.
 */
typedef struct TransportShmCirc_Params_tag {
    UInt            priority;
    /*!<  Priority of messages supported by this transport */
    Ptr             sharedAddr;
    /*!<  Address of the shared memory. The creator must supply the shared
     *    memory that this will use for maintain shared state information.
     */
} TransportShmCirc_Params;



/* =============================================================================
 *  APIs called by applications
 * =============================================================================
 */
/* Function to get the default configuration for the TransportShmCirc
 * module.
 */
Void TransportShmCirc_getConfig (TransportShmCirc_Config * cfg);

/* Function to setup the TransportShmCirc module. */
Int TransportShmCirc_setup (const TransportShmCirc_Config * cfg);

/* Function to destroy the TransportShmCirc module. */
Int TransportShmCirc_destroy (Void);

/* Get the default parameters for the NotifyShmDriver. */
Void TransportShmCirc_Params_init (TransportShmCirc_Params    * params);

/* Create an instance of the TransportShmCirc. */
TransportShmCirc_Handle
TransportShmCirc_create (      UInt16                 procId,
                     const TransportShmCirc_Params  * params);

/* Delete an instance of the TransportShmCirc. */
Int TransportShmCirc_delete (TransportShmCirc_Handle * handlePtr);

/* Get the shared memory requirements for the TransportShmCirc. */
SizeT
TransportShmCirc_sharedMemReq (const TransportShmCirc_Params * params);

/* Set the asynchronous error function for the transport module */
Void TransportShmCirc_setErrFxn (TransportShmCirc_ErrFxn errFxn);


/* =============================================================================
 *  APIs called internally by MessageQ module through function table.
 * =============================================================================
 */
/* Put msg to remote list */
Int TransportShmCirc_put (TransportShmCirc_Handle   handle,
                          Ptr                       msg);

/* Get current status of the TransportShmCirc */
Int
TransportShmCirc_getStatus (TransportShmCirc_Handle handle);

/* Control Function */
Int TransportShmCirc_control (TransportShmCirc_Handle handle,
                              UInt                    cmd,
                              UArg                    cmdArg);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* TRANSPORTSHMCIRC_H_0x0a7a */

