/** 
 *  @file   _TransportShm.h
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



#ifndef _TRANSPORTSHM_H_0x0a7a
#define _TRANSPORTSHM_H_0x0a7a


/* Standard headers */
#include <List.h>
#include <Gate.h>
#include <ti/ipc/GateMP.h>


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  All success and failure codes for the module
 * =============================================================================
 */
/*!
 *  @def    TRANSPORTSHM_MODULEID
 *  @brief  Unique module ID.
 */
#define TRANSPORTSHM_MODULEID               (0x0a7a)

/*!
 *  @brief  Module configuration structure.
 */
typedef struct TransportShm_Config {
    TransportShm_ErrFxn errFxn;
    /*!< Asynchronous error function for the transport module */
    UInt32       notifyEventId;
    /*!< Notify event number to be used for the module */
} TransportShm_Config;


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

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* TRANSPORTSHM_H_0x0a7a */

