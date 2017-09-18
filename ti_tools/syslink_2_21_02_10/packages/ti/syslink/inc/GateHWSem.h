/** 
 *  @file   GateHWSem.h
 *
 *  @brief      Defines for Gate based on Hardware Semaphore.
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


#ifndef GATEHWSEM_H_0xF417
#define GATEHWSEM_H_0xF417

/* Utilities & Osal headers */
#include <ti/ipc/GateMP.h>
#include "IGateMPSupport.h"

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Module Success and Failure codes
 * =============================================================================
 */
/*!
 *  @def    GateHWSem_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define GateHWSem_E_INVALIDARG       -1

/*!
 *  @def    GateHWSem_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define GateHWSem_E_MEMORY           -2

/*!
 *  @def    GateHWSem_E_BUSY
 *  @brief  the name is already registered or not.
 */
#define GateHWSem_E_BUSY             -3

/*!
 *  @def    GateHWSem_E_FAIL
 *  @brief  Generic failure.
 */
#define GateHWSem_E_FAIL             -4

/*!
 *  @def    GateHWSem_E_NOTFOUND
 *  @brief  name not found in the nameserver.
 */
#define GateHWSem_E_NOTFOUND         -5

/*!
 *  @def    GateHWSem_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define GateHWSem_E_INVALIDSTATE     -6

/*!
 *  @def    GateHWSem_E_NOTONWER
 *  @brief  Instance is not created on this processor.
 */
#define GateHWSem_E_NOTONWER         -7

/*!
 *  @def    GateHWSem_E_REMOTEACTIVE
 *  @brief  Remote opener of the instance has not closed the instance.
 */
#define GateHWSem_E_REMOTEACTIVE     -8

/*!
 *  @def    GateHWSem_E_INUSE
 *  @brief  Indicates that the instance is in use..
 */
#define GateHWSem_E_INUSE            -9

/*!
 *  @def    GateHWSem_E_OSFAILURE
 *  @brief  Failure in OS call.
 */
#define GateHWSem_E_OSFAILURE        -10

/*!
 *  @def    GateHWSem_E_VERSION
 *  @brief  Version mismatch error.
 */
#define GateHWSem_E_VERSION          -11

/*!
 *  @def    GateHWSem_S_SUCCESS
 *  @brief  Operation successful.
 */
#define GateHWSem_S_SUCCESS            0

/*!
 *  @def    GateHWSem_S_ALREADYSETUP
 *  @brief  The GATEHWSPINLOCK module has already been setup in this process.
 */
#define GateHWSem_S_ALREADYSETUP     1


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*! @brief Forward declaration of structure defining object for the
 *                 GateHWSem. */
typedef struct GateHWSem_Object GateHWSem_Object;

/*!
 *  @brief  Handle for the GateHWSem.
 */
typedef GateHWSem_Object * GateHWSem_Handle;

/* Q_BLOCKING */
#define GateHWSem_Q_BLOCKING   (1)

/* Q_PREEMPTING */
#define GateHWSem_Q_PREEMPTING (2)


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining config parameters for the GateHWSem module.
 */
typedef struct GateHWSem_Config {
    GateMP_LocalProtect defaultProtection;
    UInt32               baseAddr;
    /* Device-specific base address for HW Semaphore subsystem in HOST OS
     * address space, this is updated in Ipc module */
    UInt                 numSems;
    /* Device-specific number of semphores in the HW Semaphore subsystem */
} GateHWSem_Config;

/*!
 *  @brief  Structure defining config parameters for the GateHWSem
 *          instances.
 */
typedef struct GateHWSem_Params {
    IGateMPSupport_SuperParams;
} GateHWSem_Params;


/* Inherit everything from IGateMPSupport */
IGateMPSupport_Inherit(GateHWSem);

/* =============================================================================
 * APIs
 * =============================================================================
 */
/* Function to get the default configuration for the GateHWSem module. */
Void
GateHWSem_getConfig (GateHWSem_Config * config);

/* Function to setup the GateHWSem module. */
Int
GateHWSem_setup (const GateHWSem_Config * config);

/* Function to destroy the GateHWSem module */
Int
GateHWSem_destroy (Void);

/* Get the default parameters for the GateHWSem instance. */
Void
GateHWSem_Params_init (GateHWSem_Params * params);

/* Function to create an instance of GateHWSem */
GateHWSem_Handle
GateHWSem_create (      IGateMPSupport_LocalProtect localProtect,
                       const GateHWSem_Params *     params);

/* Function to delete an instance of GateHWSem */
Int
GateHWSem_delete (GateHWSem_Handle * handlePtr);

/* Function to enter the GateHWSem instance */
IArg
GateHWSem_enter  (GateHWSem_Handle handle);

/* Function to leave the GateHWSem instance */
Void
GateHWSem_leave  (GateHWSem_Handle handle, IArg   key);

/* Function to return the shared memory requirement for a single instance */
UInt32
GateHWSem_sharedMemReq (const IGateMPSupport_Params * params);

Int GateHWSem_Module_startup (GateHWSem_Handle handle);

Void GateHWSem_postInit (GateHWSem_Handle handle);

UInt32 GateHWSem_getNumInstances(Void);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* GATEHWSEM_H_0xF417 */

