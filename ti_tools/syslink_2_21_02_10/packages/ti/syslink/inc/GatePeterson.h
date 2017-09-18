/** 
 *  @file   GatePeterson.h
 *
 *  @brief      Defines for Gate based on Peterson Algorithm.
 *
 *              This module implements the Peterson Algorithm for mutual
 *              exclusion of shared memory. This implementation works for only 2
 *              processors.<br>
 *              This module is instance based. Each instance requires a small
 *              piece of shared memory. This is specified via the sharedAddr
 *              parameter to the create. The proper sharedAddrSize parameter
 *              can be determined via the #GatePeterson_sharedMemReq call. Note:
 *              the parameters to this function must be the same that will used
 *              to create or open the instance.<br>
 *              The GatePeterson module uses a NameServer instance
 *              to store instance information when an instance is created and
 *              the name parameter is non-NULL. If a name is supplied, it must
 *              be unique for all GatePeterson instances.
 *              The #GatePeterson_create also initializes the shared memory as
 *              needed. The shared memory must be initialized to 0 or all ones
 *              (e.g. 0xFFFFFFFFF) before the GatePeterson instance is created.
 *              Once an instance is created, an open can be performed. The
 *              #GatePeterson_open is used to gain access to the same
 *              GatePeterson instance. Generally an instance is created on one
 *              processor and opened on the other processor.<br>
 *              The open returns a GatePeterson instance handle like the create,
 *              however the open does not modify the shared memory. Generally an
 *              instance is created on one processor and opened on the other
 *              processor.<br>
 *              There are two options when opening the instance:<br>
 *              - Supply the same name as specified in the create. The
 *              GatePeterson module queries the NameServer to get the needed
 *              information.<br>
 *              - Supply the same sharedAddr value as specified in the create.
 *              If the open is called before the instance is created, open
 *              returns NULL.<br>
 *              There is currently a list of restrictions for the module:<br>
 *              - Both processors must have the same endianness. Endianness
 *             conversion may supported in a future version of GatePeterson.<br>
 *              - The module will be made a gated module
 *
 *  @p(code)
 *
 *              shmBaseAddr -> --------------------------- bytes
 *                             |  version                | 4
 *              (Attrs struct) |  creatorProcId          | 2
 *                             |  openerProcId           | 2
 *                             |  (PADDING if aligned)   |
 *                             |-------------------------|
 *                             |  flag[0]                | 2
 *                             |  (PADDING if aligned)   |
 *                             |-------------------------|
 *                             |  flag[1]                | 2
 *                             |  (PADDING if aligned)   |
 *                             |-------------------------|
 *                             |  turn                   | 2
 *                             |  (PADDING if aligned)   |
 *                             |-------------------------|
 *  @p
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



#ifndef GATEPETERSON_H_0xF415
#define GATEPETERSON_H_0xF415

/* Utilities & Osal headers */
#include "IGateMPSupport.h"
#include <ti/ipc/GateMP.h>
#include <ti/syslink/utils/IGateProvider.h>

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Module Success and Failure codes
 * =============================================================================
 */
/*!
 *  @def    GatePeterson_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define GatePeterson_E_INVALIDARG       -1

/*!
 *  @def    GatePeterson_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define GatePeterson_E_MEMORY           -2

/*!
 *  @def    GatePeterson_E_BUSY
 *  @brief  the name is already registered or not.
 */
#define GatePeterson_E_BUSY             -3

/*!
 *  @def    GatePeterson_E_FAIL
 *  @brief  Generic failure.
 */
#define GatePeterson_E_FAIL             -4

/*!
 *  @def    GatePeterson_E_NOTFOUND
 *  @brief  name not found in the nameserver.
 */
#define GatePeterson_E_NOTFOUND         -5

/*!
 *  @def    GatePeterson_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define GatePeterson_E_INVALIDSTATE     -6

/*!
 *  @def    GatePeterson_E_NOTONWER
 *  @brief  Instance is not created on this processor.
 */
#define GatePeterson_E_NOTONWER         -7

/*!
 *  @def    GatePeterson_E_REMOTEACTIVE
 *  @brief  Remote opener of the instance has not closed the instance.
 */
#define GatePeterson_E_REMOTEACTIVE     -8

/*!
 *  @def    GatePeterson_E_INUSE
 *  @brief  Indicates that the instance is in use..
 */
#define GatePeterson_E_INUSE            -9

/*!
 *  @def    GatePeterson_E_OSFAILURE
 *  @brief  Failure in OS call.
 */
#define GatePeterson_E_OSFAILURE        -10

/*!
 *  @def    GatePeterson_E_VERSION
 *  @brief  Version mismatch error.
 */
#define GatePeterson_E_VERSION          -12

/*!
 *  @def    GatePeterson_E_GATEREMOTELYOPENED
 *  @brief  Gate has already been opened remotely
 */
#define GatePeterson_E_GATEREMOTELYOPENED -13

/*!
 *  @def    GatePeterson_S_SUCCESS
 *  @brief  Operation successful.
 */
#define GatePeterson_S_SUCCESS            0

/*!
 *  @def    GatePeterson_S_ALREADYSETUP
 *  @brief  The GATEPETERSON module has already been setup in this process.
 */
#define GatePeterson_S_ALREADYSETUP       1


/* =============================================================================
 * Macros
 * =============================================================================
 */
/*! @brief  Object for Gate Peterson */
typedef struct GatePeterson_Object GatePeterson_Object;

/*! @brief  Handle for Gate Peterson */
typedef GatePeterson_Object * GatePeterson_Handle;

/* Inherit everything from IGateMPSupport */
IGateMPSupport_Inherit(GatePeterson);

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining config parameters for the Gate Peterson
 *          instances.
 */
typedef struct GatePeterson_Params {
        IGateMPSupport_SuperParams;
} GatePeterson_Params;


/*!
 *  @brief  Structure defining config parameters for the Gate Peterson module.
 */
typedef struct GatePeterson_Config {
    GateMP_LocalProtect defaultProtection;
    /*!< Default module-wide local context protection level. The level of
     * protection specified here determines which local gate is created per
     * GatePeterson instance for local protection during create. The instance
     * configuration parameter may be used to override this module setting per
     * instance.  The configuration used here should reflect both the context
     * in which enter and leave are to be called, as well as the maximum level
     * of protection needed locally.
     */
    UInt32                  numInstances;
    /*!<  Maximum number of instances supported by the GatePeterson module */
} GatePeterson_Config;


/* =============================================================================
 * APIs
 * =============================================================================
 */
/* Function to get the default configuration for the GatePeterson module. */
Void
GatePeterson_getConfig (GatePeterson_Config * config);

/* Function to setup the GatePeterson module. */
Int
GatePeterson_setup (const GatePeterson_Config * config);

/* Function to destroy the GatePeterson module */
Int
GatePeterson_destroy (Void);

/* Get the default parameters for the GatePeterson instance. */
Void
GatePeterson_Params_init (GatePeterson_Params * params);

/* Function to create an instance of GatePeterson */
GatePeterson_Handle
GatePeterson_create (IGateProvider_Handle localGate,
                     const GatePeterson_Params      * params);

/* Function to delete an instance of GatePeterson */
Int
GatePeterson_delete (GatePeterson_Handle * handlePtr);

/* Function to enter the GatePeterson instance */
IArg
GatePeterson_enter  (GatePeterson_Handle handle);

/* Function to leave the GatePeterson instance */
Void
GatePeterson_leave  (GatePeterson_Handle handle, IArg  key);

/* Function to return the shared memory requirement for a single instance */
UInt32
GatePeterson_sharedMemReq (const IGateMPSupport_Params * params);

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 *  @return     Number of instances configured.
 */
UInt32 GatePeterson_getNumInstances (Void);

/*!
 *  @brief  Function to initialize the locks
 *          module.
 *
 */
inline
Void GatePeterson_locksinit(Void);

Void GatePeterson_setReserved(UInt32 lockNum);
/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/* Returns the GatePeterson kernel object pointer. */
Void *
GatePeterson_getKnlHandle (GatePeterson_Handle handle);

Bits32 *GatePeterson_getReservedMask(Void);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* GATEPETERSON_H_0xF415 */

