/** 
 *  @file   GateHWSpinlock_qnx.h
 *
 *  @brief      Defines for Gate based on Hardware SpinLock.
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



#ifndef GATEHWSPINLOCK_H_0xF416
#define GATEHWSPINLOCK_H_0xF416

/* Utilities & Osal headers */
#include <ti/ipc/GateMP.h>
#include <ti/syslink/utils/IGateProvider.h>
#include "IGateMPSupport.h"

#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 * Module Success and Failure codes
 * =============================================================================
 */
/*!
 *  @def    GateHWSpinlock_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define GateHWSpinlock_E_INVALIDARG       -1

/*!
 *  @def    GateHWSpinlock_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define GateHWSpinlock_E_MEMORY           -2

/*!
 *  @def    GateHWSpinlock_E_BUSY
 *  @brief  the name is already registered or not.
 */
#define GateHWSpinlock_E_BUSY             -3

/*!
 *  @def    GateHWSpinlock_E_FAIL
 *  @brief  Generic failure.
 */
#define GateHWSpinlock_E_FAIL             -4

/*!
 *  @def    GateHWSpinlock_E_NOTFOUND
 *  @brief  name not found in the nameserver.
 */
#define GateHWSpinlock_E_NOTFOUND         -5

/*!
 *  @def    GateHWSpinlock_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define GateHWSpinlock_E_INVALIDSTATE     -6

/*!
 *  @def    GateHWSpinlock_E_NOTONWER
 *  @brief  Instance is not created on this processor.
 */
#define GateHWSpinlock_E_NOTONWER         -7

/*!
 *  @def    GateHWSpinlock_E_REMOTEACTIVE
 *  @brief  Remote opener of the instance has not closed the instance.
 */
#define GateHWSpinlock_E_REMOTEACTIVE     -8

/*!
 *  @def    GateHWSpinlock_E_INUSE
 *  @brief  Indicates that the instance is in use..
 */
#define GateHWSpinlock_E_INUSE            -9

/*!
 *  @def    GateHWSpinlock_E_OSFAILURE
 *  @brief  Failure in OS call.
 */
#define GateHWSpinlock_E_OSFAILURE        -10

/*!
 *  @def    GateHWSpinlock_E_VERSION
 *  @brief  Version mismatch error.
 */
#define GateHWSpinlock_E_VERSION          -11

/*!
 *  @def    GateHWSpinlock_S_SUCCESS
 *  @brief  Operation successful.
 */
#define GateHWSpinlock_S_SUCCESS            0

/*!
 *  @def    GateHWSpinlock_S_ALREADYSETUP
 *  @brief  The GATEHWSPINLOCK module has already been setup in this process.
 */
#define GateHWSpinlock_S_ALREADYSETUP     1

/*!
 *  @def    GateHWSpinlock_S_INUSE
 *  @brief  The GateHWSpinlock module is still in use by another client.
 */
#define GateHWSpinlock_S_INUSE            2


/* =============================================================================
 * Macros
 * =============================================================================
 */

/*! @brief Forward declaration of structure defining object for the
 *                 GateHWSpinlock. */
typedef struct GateHWSpinlock_Object GateHWSpinlock_Object;

/*!
 *  @brief  Handle for the GateHWSpinlock.
 */
typedef GateHWSpinlock_Object * GateHWSpinlock_Handle;

/* Q_BLOCKING */
#define GateHWSem_Q_BLOCKING   (1)

/* Q_PREEMPTING */
#define GateHWSem_Q_PREEMPTING (2)

#define GateHWSpinlock_GLOBAL   "GateHWSpinLock.global"
#define GateHWSpinlock_CONFIG   "GateHWSpinLock.cfg"


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Structure defining config parameters for the GateHWSpinlock module.
 */
typedef struct GateHWSpinlock_Config {
    GateMP_LocalProtect defaultProtection;
    /*!< Default module-wide local context protection level. The level of
     * protection specified here determines which local gate is created per
     * GateHWSpinlock instance for local protection during create. The instance
     * configuration parameter may be used to override this module setting per
     * instance.  The configuration used here should reflect both the context
     * in which enter and leave are to be called, as well as the maximum level
     * of protection needed locally.
     */
    UInt32              baseAddr;
    /* Device-specific base address for HW Semaphore subsystem in HOST OS
     * address space, this is updated in Ipc module */
    SizeT               size;
    /* Size needed to map hardware registers into virtual address space */
    UInt                numLocks;
    /* Device-specific number of semphores in the HW Semaphore subsystem */
} GateHWSpinlock_Config;

/*!
 *  @brief  Structure defining config parameters for the GateHWSpinlock
 *          instances.
 */
typedef struct GateHWSpinlock_Params {
    IGateMPSupport_SuperParams;
} GateHWSpinlock_Params;


/* Inherit everything from IGateMPSupport */
IGateMPSupport_Inherit(GateHWSpinlock);

/* =============================================================================
 * APIs
 * =============================================================================
 */

/*
 *  ======== GateHWSpinlock_setup ========
 *  Internal
 */
Int GateHWSpinlock_setup(Void);

/*
 *  ======== GateHWSpinlock_destroy ========
 *  Internal
 */
Int GateHWSpinlock_destroy(Void);

/*
 *  ======== GateHWSpinlock_Params_init ========
 */
Void GateHWSpinlock_Params_init(GateHWSpinlock_Params *params);

/*
 *  ======== GateHWSpinlock_create ========
 */
GateHWSpinlock_Handle GateHWSpinlock_create(IGateProvider_Handle localGate,
        const GateHWSpinlock_Params *params);

/*
 *  ======== GateHWSpinlock_delete ========
 */
Int GateHWSpinlock_delete(GateHWSpinlock_Handle *handlePtr);

/*
 *  ======== GateHWSpinlock_enter ========
 */
IArg GateHWSpinlock_enter(GateHWSpinlock_Handle handle);

/*
 *  ======== GateHWSpinlock_leave ========
 */
Void GateHWSpinlock_leave(GateHWSpinlock_Handle handle, IArg key);

/*
 *  ======== GateHWSpinlock_sharedMemReq ========
 *  Return the shared memory requirement for a single instance.
 */
UInt GateHWSpinlock_sharedMemReq(const IGateMPSupport_Params *params);

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 *  @return     Number of instances configured.
 */
UInt GateHWSpinlock_getNumInstances(Void);

/*!
 *  @brief      Function to initialize the locks
 *              module.
 */
Void GateHWSpinlock_locksinit(Void);

/*!
 *  ======== setReserved ========
 *  Reserve a HW spinlock for use outside of IPC.
 *
 *  GateMP will, by default, manage all HW spinlocks on the device unless
 *  this API is used to set aside specific spinlocks for use outside
 *  of IPC.
 *
 *  @param(lockNum)      HW spinlock number to reserve
 */
Void GateHWSpinlock_setReserved(UInt lockNum);

/*
 *  ======== GateHWSpinlock_getReservedMask ========
 */
Bits32 *GateHWSpinlock_getReservedMask(Void);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* GATEHWSPINLOCK_H_0xF416 */

