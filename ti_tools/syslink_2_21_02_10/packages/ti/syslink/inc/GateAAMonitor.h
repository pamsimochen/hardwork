/** 
 *  @file   GateAAMonitor.h
 *
 *  @brief      Defines for Gate based on Atomic Access Monitor HW.
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


#ifndef GATEAAMONITOR_H_0xF416
#define GATEAAMONITOR_H_0xF416

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
 *  @def    GateAAMonitor_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define GateAAMonitor_E_INVALIDARG       -1

/*!
 *  @def    GateAAMonitor_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define GateAAMonitor_E_MEMORY           -2

/*!
 *  @def    GateAAMonitor_E_BUSY
 *  @brief  the name is already registered or not.
 */
#define GateAAMonitor_E_BUSY             -3

/*!
 *  @def    GateAAMonitor_E_FAIL
 *  @brief  Generic failure.
 */
#define GateAAMonitor_E_FAIL             -4

/*!
 *  @def    GateAAMonitor_E_NOTFOUND
 *  @brief  name not found in the nameserver.
 */
#define GateAAMonitor_E_NOTFOUND         -5

/*!
 *  @def    GateAAMonitor_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define GateAAMonitor_E_INVALIDSTATE     -6

/*!
 *  @def    GateAAMonitor_E_NOTONWER
 *  @brief  Instance is not created on this processor.
 */
#define GateAAMonitor_E_NOTONWER         -7

/*!
 *  @def    GateAAMonitor_E_REMOTEACTIVE
 *  @brief  Remote opener of the instance has not closed the instance.
 */
#define GateAAMonitor_E_REMOTEACTIVE     -8

/*!
 *  @def    GateAAMonitor_E_INUSE
 *  @brief  Indicates that the instance is in use..
 */
#define GateAAMonitor_E_INUSE            -9

/*!
 *  @def    GateAAMonitor_E_OSFAILURE
 *  @brief  Failure in OS call.
 */
#define GateAAMonitor_E_OSFAILURE        -10

/*!
 *  @def    GateAAMonitor_E_VERSION
 *  @brief  Version mismatch error.
 */
#define GateAAMonitor_E_VERSION          -11

/*!
 *  @def    GateAAMonitor_S_SUCCESS
 *  @brief  Operation successful.
 */
#define GateAAMonitor_S_SUCCESS            0

/*!
 *  @def    GateAAMonitor_S_ALREADYSETUP
 *  @brief  The GATEAAMONITOR module has already been setup in this process.
 */
#define GateAAMonitor_S_ALREADYSETUP     1


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*! @brief Forward declaration of structure defining object for the
 *                 GateAAMonitor. */
typedef struct GateAAMonitor_Object GateAAMonitor_Object;

/*!
 *  @brief  Handle for the GateAAMonitor.
 */
typedef GateAAMonitor_Object * GateAAMonitor_Handle;

/* Q_BLOCKING */
#define GateAAMon_Q_BLOCKING   (1)

/* Q_PREEMPTING */
#define GateAAMon_Q_PREEMPTING (2)


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining config parameters for the GateAAMonitor module.
 */
typedef struct GateAAMonitor_Config {
    GateMP_LocalProtect defaultProtection;
    /*!< Default module-wide local context protection level. The level of
     * protection specified here determines which local gate is created per
     * GateAAMonitor instance for local protection during create. The instance
     * configuration parameter may be used to override this module setting per
     * instance.  The configuration used here should reflect both the context
     * in which enter and leave are to be called, as well as the maximum level
     * of protection needed locally.
     */
    UInt32               sharedAddr;
    /* Device-specific base address for AA Monitor subsystem in HOST OS
     * address space, this is updated in Ipc module */
    UInt                 numMonitors;
    /* Device-specific number of semphores in the AA Monitor subsystem */
} GateAAMonitor_Config;

/*!
 *  @brief  Structure defining config parameters for the GateAAMonitor
 *          instances.
 */
typedef struct GateAAMonitor_Params {
    IGateMPSupport_SuperParams;
} GateAAMonitor_Params;


/* Inherit everything from IGateMPSupport */
IGateMPSupport_Inherit(GateAAMonitor);

/* =============================================================================
 * APIs
 * =============================================================================
 */
/* Function to get the default configuration for the GateAAMonitor module. */
Void
GateAAMonitor_getConfig (GateAAMonitor_Config * config);

/* Function to setup the GateAAMonitor module. */
Int
GateAAMonitor_setup (const GateAAMonitor_Config * config);

/* Function to destroy the GateAAMonitor module */
Int
GateAAMonitor_destroy (Void);

/* Get the default parameters for the GateAAMonitor instance. */
Void
GateAAMonitor_Params_init (GateAAMonitor_Params * params);

/* Function to create an instance of GateAAMonitor */
GateAAMonitor_Handle
GateAAMonitor_create (      IGateMPSupport_LocalProtect localProtect,
                       const GateAAMonitor_Params *     params);

/* Function to delete an instance of GateAAMonitor */
Int
GateAAMonitor_delete (GateAAMonitor_Handle * handlePtr);

/* Function to enter the GateAAMonitor instance */
IArg
GateAAMonitor_enter  (GateAAMonitor_Handle handle);

/* Function to leave the GateAAMonitor instance */
Void
GateAAMonitor_leave  (GateAAMonitor_Handle handle, IArg   key);

/* Function to return the shared memory requirement for a single instance */
UInt32
GateAAMonitor_sharedMemReq (const IGateMPSupport_Params * params);

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 *  @return     Number of instances configured.
 */
UInt32 GateAAMonitor_getNumInstances (Void);

/*!
 *  @brief      Function to initialize the locks
 *              module.
 *
 */
Void GateAAMonitor_locksinit(Void);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* GATEAAMONITOR_H_0xF416 */

