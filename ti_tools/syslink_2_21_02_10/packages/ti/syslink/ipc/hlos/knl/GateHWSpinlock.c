/*
 *  @file   GateHWSpinlock.c
 *
 *  @brief      Gate based on Hardware SpinLock.
 *
 *
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



/* Standard headers */
#include <ti/syslink/Std.h>

/* Utilities & OSAL headers */
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/inc/IGateMPSupport.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/IObject.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/List.h>

/* Module level headers */
#include <ti/syslink/utils/String.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/inc/GateHWSpinlock.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */

/* Macro to make a correct module magic number with refCount */
#define GATEHWSPINLOCK_MAKE_MAGICSTAMP(x) (  (GATEHWSPINLOCK_MODULEID << 12u)  \
                                           | (x))

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* structure for GateHWSpinlock module state */
typedef struct GateHWSpinlock_Module_State {
    Int32                 refCount;
    /* Reference count */
    GateHWSpinlock_Config cfg;
    /* Current config values */
    GateHWSpinlock_Config defaultCfg;
    /* default config values */
    GateHWSpinlock_Params defInstParams;
    /* default instance params */
    UInt32 *              baseAddr;
    /* Base address of lock registers */
    UInt32                numLocks;
    /*!< Maximum number of locks */
    Bits32                *reservedMaskArr; /* for 64 locks of netra */
} GateHWSpinlock_Module_State;

/* Structure defining internal object for the Gate Peterson.*/
struct GateHWSpinlock_Object {
        IGateProvider_SuperObject; /* For inheritance from IGateProvider */
        IOBJECT_SuperObject;       /* For inheritance for IObject */
    UInt                        lockNum;
    UInt                        nested;
    IGateProvider_Handle        localGate;
};


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    GateHWSpinlock_state
 *
 *  @brief  GateHWSpinlock Module state object.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
GateHWSpinlock_Module_State GateHWSpinlock_state =
{
    .refCount = 0,
    .defaultCfg.defaultProtection  = GateHWSpinlock_LocalProtect_INTERRUPT,
    .defaultCfg.numLocks           = 64,/* Max limit supported in netra platform*/
    .defInstParams.resourceId      = 0,
    .defInstParams.openFlag        = FALSE,
    .defInstParams.regionId        = 0,
    .defInstParams.sharedAddr      = NULL,
    .numLocks                      = 64u,/* Max limit supported in netra platform*/
};

/*!
 *  @var    GateHWSpinlock_state
 *
 *  @brief  GateHWSpinlock Module state object.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
GateHWSpinlock_Module_State * GateHWSpinlock_module = &GateHWSpinlock_state;


/* =============================================================================
 * APIS
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the GateHWSpinlock module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to GateHWSpinlock_setup filled in by
 *              the GateHWSpinlock module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfgParams  Pointer to the GateHWSpinlock module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         GateHWSpinlock_setup
 */
Void
GateHWSpinlock_getConfig (GateHWSpinlock_Config * cfgParams)
{
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "GateHWSpinlock_getConfig", cfgParams);

    GT_assert (curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSpinlock_getConfig",
                             GateHWSpinlock_E_INVALIDARG,
                             "Argument of type (GateHWSpinlock_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                key = Gate_enterSystem ();
        if (GateHWSpinlock_module->refCount == 0) {
            Memory_copy ((Ptr) cfgParams,
                         (Ptr) &GateHWSpinlock_module->defaultCfg,
                         sizeof (GateHWSpinlock_Config));
        }
        else {
            Memory_copy ((Ptr) cfgParams,
                         (Ptr) &GateHWSpinlock_module->cfg,
                         sizeof (GateHWSpinlock_Config));
        }
                Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_0trace (curTrace, GT_ENTER, "GateHWSpinlock_getConfig");
}


/*!
 *  @brief      Setup the GateHWSpinlock module.
 *
 *              This function sets up the GateHWSpinlock module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then GateHWSpinlock_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call GateHWSpinlock_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional GateHWSpinlock module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         GateHWSpinlock_destroy, GateHWSpinlock_getConfig
 */
Int32 GateHWSpinlock_setup(const GateHWSpinlock_Config *cfg)
{
    Int32                       status = GateHWSpinlock_S_SUCCESS;
    IArg                        key;
    UInt32                      reservedSize;

    GT_1trace(curTrace, GT_ENTER, "GateHWSpinlock_setup: cfg=0x%08x", cfg);

    key = Gate_enterSystem();
    GateHWSpinlock_module->refCount++;

    /* if already setup, then leave gate and return (success) */
    if (GateHWSpinlock_module->refCount > 1) {
        Gate_leaveSystem(key);
        status = GateHWSpinlock_S_ALREADYSETUP;
        GT_0trace(curTrace, GT_2CLASS,
            "GateHWSpinlock Module already initialized!");
        goto leave;
    }

    /* save given config or use default config */
    if (cfg != NULL) {
        Memory_copy((Ptr)&GateHWSpinlock_module->cfg, (Ptr)cfg,
            sizeof(GateHWSpinlock_Config));
    }
    else {
        Memory_copy((Ptr)&GateHWSpinlock_module->cfg,
            (Ptr)&GateHWSpinlock_module->defaultCfg,
            sizeof(GateHWSpinlock_Config));
    }

    /* initialize module state */
    cfg = &GateHWSpinlock_module->cfg;
    GateHWSpinlock_module->baseAddr = (UInt32 *)cfg->baseAddr;
    GateHWSpinlock_module->numLocks = cfg->numLocks;
    reservedSize = (cfg->numLocks/32) + (cfg->numLocks % 32 ? 1 : 0);
    GateHWSpinlock_module->reservedMaskArr = Memory_calloc(NULL,
        reservedSize * sizeof(UInt32), 0u, NULL);

    Gate_leaveSystem (key);

leave:
    GT_1trace(curTrace, GT_LEAVE, "GateHWSpinlock_setup: [0x%08x]", status);

    /*! @retval GateHWSpinlock_S_SUCCESS Operation successful */
    return(status);
}

/*!
 *  @brief      Function to destroy the GateHWSpinlock module.
 *
 *  @sa         GateHWSpinlock_setup
 */
Int32 GateHWSpinlock_destroy(Void)
{
    Int32       status = GateHWSpinlock_S_SUCCESS;
    IArg        key;
    UInt32      reservedSize;

    GT_0trace(curTrace, GT_ENTER, "GateHWSpinlock_destroy");

    key = Gate_enterSystem();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateHWSpinlock_module->refCount <= 0) {
        Gate_leaveSystem (key);
        /*! @retval GateHWSpinlock_E_INVALIDSTATE Module was not initialized */
        status = GateHWSpinlock_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_destroy",
            status, "Module was not initialized!");
        goto leave;
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GateHWSpinlock_module->refCount--;

    /* if not last user, leave gate and return (success) */
    if (GateHWSpinlock_module->refCount > 0) {
        Gate_leaveSystem (key);
        status = GateHWSpinlock_S_SUCCESS;
        goto leave;
    }

    /* release all resources */
    reservedSize = (GateHWSpinlock_module->numLocks/32) +
        (GateHWSpinlock_module->numLocks % 32 ? 1 : 0);

    Memory_free(NULL, GateHWSpinlock_module->reservedMaskArr,
        reservedSize * sizeof(UInt32));
    GateHWSpinlock_module->reservedMaskArr = NULL;

    /* clear cfg area */
    Memory_set((Ptr)&GateHWSpinlock_module->cfg, 0,
        sizeof(GateHWSpinlock_Config));

    Gate_leaveSystem(key);

leave:
    GT_1trace(curTrace, GT_LEAVE, "GateHWSpinlock_destroy: [0x%08x]", status);

    /*! @retval GateHWSpinlock_S_SUCCESS Operation successful */
    return(status);
}

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 */
UInt32 GateHWSpinlock_getNumInstances()
{
    return (GateHWSpinlock_module->numLocks);
}

/* Initialize the locks */
Void GateHWSpinlock_locksinit()
{
    UInt32  i;
    Bits32 *reservedMaskArr = (Bits32 *)GateHWSpinlock_module->reservedMaskArr;

    for (i = 0; i < GateHWSpinlock_module->numLocks; i++) {
        if (reservedMaskArr[i >> 5] & (1 << (i % 32))) {
            /* Don't reset reserved spinlocks */
            continue;
        }
        GateHWSpinlock_module->baseAddr[i] = 0;
    }
}

/*!
 *  @brief      Initialize this config-params structure with supplier-specified
 *              defaults before instance creation.
 *
 *  @param      handle  If specified as NULL, default parameters are returned.
 *                      If not NULL, the parameters as configured for this
 *                      instance are returned.
 *  @param      params  Instance config-params structure.
 *
 *  @sa         GateHWSpinlock_create
 */
/*
 *  ======== GateHWSpinlock_Params_init ========
 */
Void GateHWSpinlock_Params_init(GateHWSpinlock_Params *params)
{
    GT_1trace(curTrace, GT_ENTER, "GateHWSpinlock_Params_init", params);

    GT_assert(curTrace, (params != NULL));

    Memory_copy((Ptr)params, &GateHWSpinlock_state.defInstParams,
        sizeof(GateHWSpinlock_Params));

    GT_0trace(curTrace, GT_LEAVE, "GateHWSpinlock_Params_init");
}


/*
 *  ======== GateHWSpinlock_Instance_init ========
 */
Int GateHWSpinlock_Instance_init(GateHWSpinlock_Object *obj,
        IGateProvider_Handle localGate,
        const GateHWSpinlock_Params *params)
{
    Int32 status = 0;

    IGateProvider_ObjectInitializer(obj, GateHWSpinlock);

    /* assert that params->resourceId is valid */
    if (params->resourceId >= GateHWSpinlock_module->numLocks) {
        status = 1;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateHWSpinlock_Instance_init",
            GateHWSpinlock_E_FAIL, "resourceId >= GateHWSpinlock_numLocks!");
    }

    /* assert that localGate is non-null */
    if ((status == 0) && (localGate == NULL)) {
        status = 2;
        GT_setFailureReason(curTrace, GT_4CLASS,
            "GateHWSpinlock_Instance_init", GateHWSpinlock_E_FAIL,
            "localGate is null");
    }

    /* create the local gate */
    if (status == 0) {
        obj->localGate = localGate;
        obj->lockNum = params->resourceId;
        obj->nested = 0;
    }

    return(status);
}

/*
 *  ======== GateHWSpinlock_Instance_finalize ========
 */
Void GateHWSpinlock_Instance_finalize(GateHWSpinlock_Object *obj, Int status)
{
}

/*
 *  ======== GateHWSpinlock_enter ========
 */
IArg GateHWSpinlock_enter(GateHWSpinlock_Object *obj)
{
    volatile UInt32 *baseAddr = (volatile UInt32 *)
                                             GateHWSpinlock_module->baseAddr;
    IArg key;

    key = IGateProvider_enter(obj->localGate);

    /* If the gate object has already been entered, return the nested value */
    obj->nested++;
    if (obj->nested > 1) {
        return (key);
    }

    /* Enter the spinlock */
    while (1) {
        if (baseAddr[obj->lockNum] == 0) {
            break;
        }
        obj->nested--; /* Restore state of delegate object */
        IGateProvider_leave(obj->localGate, key);
        key = IGateProvider_enter(obj->localGate);
        obj->nested++; /* Re-nest the gate */
    }

    return (key);
}

Void GateHWSpinlock_setReserved(UInt32 lockNum)
{
    if (lockNum >= GateHWSpinlock_module->cfg.numLocks) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateHWSpinlock_setReserved",
                             GateHWSpinlock_E_FAIL,
                             "lockNum is greater than the configured locks!");
    }
    else {
        GateHWSpinlock_module->reservedMaskArr[lockNum >> 5] |=
                                                         (1 << (lockNum % 32));
    }
}

/*
 *  ======== GateHWSpinlock_leave ========
 */
Void GateHWSpinlock_leave(GateHWSpinlock_Object *obj, IArg key)
{
    volatile UInt32 *baseAddr = (volatile UInt32 *)
                                            GateHWSpinlock_module->baseAddr;

    obj->nested--;

    /* Leave the spinlock if the leave() is not nested */
    if (obj->nested == 0) {
        baseAddr[obj->lockNum] = 0;
    }

    IGateProvider_leave(obj->localGate, key);
}

/*
 *  ======== GateHWSpinlock_getResourceId ========
 */
Bits32 GateHWSpinlock_getResourceId(GateHWSpinlock_Object *obj)
{
    return (obj->lockNum);
}

/*
 *  ======== GateHWSpinlock_sharedMemReq ========
 */
SizeT GateHWSpinlock_sharedMemReq(const IGateMPSupport_Params *params)
{
    return (0);
}

/*
 *************************************************************************
 *                       Module functions
 *************************************************************************
 */

/*
 *  ======== GateHWSpinlock_getReservedMask ========
 */
Bits32 *GateHWSpinlock_getReservedMask()
{
    return ((Bits32 *)GateHWSpinlock_module->reservedMaskArr);
}

/*
 *  ======== GateHWSpinlock_query ========
 */
Bool GateHWSpinlock_query(Int qual)
{
    Bool rc;

    switch (qual) {
        case IGateProvider_Q_BLOCKING:
            /* Depends on gate proxy? */
            rc = FALSE;
            break;

        case IGateProvider_Q_PREEMPTING:
            /* Depends on gate proxy? */
            rc = TRUE;
            break;
        default:
            rc = FALSE;
            break;
    }
    return (rc);
}

/* Override the IObject interface to define craete and delete APIs */
IOBJECT_CREATE1 (GateHWSpinlock, IGateProvider_Handle);
