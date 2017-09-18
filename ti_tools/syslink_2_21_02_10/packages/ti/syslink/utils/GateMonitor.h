/** 
 *  @file   GateMonitor.h
 *
 *  @brief      Header file for Gate based on Monitor.
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


#ifndef GATEMONITOR_H_0x189E
#define GATEMONITOR_H_0x189E


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Status codes
 * =============================================================================
 */
/*!
 *  @def    GateMonitor_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define GateMonitor_E_INVALIDARG       -1

/*!
 *  @def    GateMonitor_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define GateMonitor_E_MEMORY           -2

/*!
 *  @def    GateMonitor_E_BUSY
 *  @brief  The name is already registered or not.
 */
#define GateMonitor_E_BUSY             -3

/*!
 *  @def    GateMonitor_E_FAIL
 *  @brief  Generic failure.
 */
#define GateMonitor_E_FAIL             -4

/*!
 *  @def    GateMonitor_E_NOTFOUND
 *  @brief  Name not found in the nameserver.
 */
#define GateMonitor_E_NOTFOUND         -5

/*!
 *  @def    GateMonitor_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define GateMonitor_E_INVALIDSTATE     -6

/*!
 *  @def    GateMonitor_E_INUSE
 *  @brief  Indicates that the instance is in use.
 */
#define GateMonitor_E_INUSE            -7

/*!
 *  @def    GateMonitor_E_HANDLE
 *  @brief  An invalid handle was provided.
 */
#define GateMonitor_E_HANDLE           -8

/*!
 *  @def    GateMonitor_S_SUCCESS
 *  @brief  Operation successful.
 */
#define GateMonitor_S_SUCCESS          0


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*! @brief  Object for GateMonitor */
typedef struct GateMonitor_Object   GateMonitor_Object;

/*! @brief  Handle for GateMonitor */
typedef GateMonitor_Object * GateMonitor_Handle;

/*! No parameters for GateMonitor creation */
typedef Void GateMonitor_Params;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Function to create a GateMonitor */
GateMonitor_Handle GateMonitor_create (const GateMonitor_Params * params);

/* Function to delete a GateMonitor */
Int GateMonitor_delete (GateMonitor_Handle * handle);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* GATEMONITOR_H_0x188E */
