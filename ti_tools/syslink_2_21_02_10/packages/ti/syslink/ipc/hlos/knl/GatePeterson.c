/*
 *  @file   GatePeterson.c
 *
 *  @brief      Gate based on Peterson Algorithm.
 *
 *              This module implements the Peterson Algorithm for mutual
 *              exclusion of shared memory. This implementation works for only 2
 *              processors.
 *              This module is instance based. Each instance requires a small
 *              piece of shared memory. This is specified via the #sharedAddr
 *              parameter to the create. The proper #sharedAddrSize parameter
 *              can be determined via the #sharedMemReq call. Note: the
 *              parameters to this function must be the same that will used to
 *              create or open the instance.
 *              The #create also initializes the shared memory as needed. The
 *              shared memory must be initialized to 0 or all ones
 *              (e.g. 0xFFFFFFFFF) before the GatePeterson instance is created.
 *              Once an instance is created, an open can be performed. The #open
 *              is used to gain access to the same GatePeterson instance.
 *              Generally an instance is created on one processor and opened
 *              on the other processor.
 *              The open returns a GatePeterson instance handle like the create,
 *              however the open does not modify the shared memory. Generally an
 *              instance is created on one processor and opened on the other
 *              processor.
 *              There are two options when opening the instance:
 *              @li Supply the same #sharedAddr value as specified in the create.
 *              If the open is called before the instance is created, open
 *              returns NULL.
 *              There is currently a list of restrictions for the module:
 *              @li Both processors must have the same endianness. Endianness
 *              conversion may supported in a future version of GatePeterson.
 *              @li The module will be made a gated module
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
#include <ti/syslink/inc/IObject.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/inc/IGateMPSupport.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/ipc/GateMP.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/List.h>

/* Module level headers */
#include <ti/syslink/utils/String.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/utils/GateSpinlock.h>
#include <ti/syslink/inc/GatePeterson.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */

/* Value to indicate GP is busy. */
#define GatePeterson_BUSY        1u

/* Value to indicate GP is free. */
#define GatePeterson_FREE        0u

/* Stamp to indicate GP was created here */
#define GatePeterson_CREATED     0x08201997

/* Cache line size */
#define GatePeterson_CACHESIZE   128u

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* structure for GatePeterson module state */
typedef struct GatePeterson_ModuleObject {
    Int32               refCount;
    /* Reference count */
    GatePeterson_Config cfg;
    /* Current config values */
    GatePeterson_Config defaultCfg;
    /* default config values */
    GatePeterson_Params defInstParams;
    /* default instance params */
} GatePeterson_ModuleObject;

/* Structure defining attribute parameters for the Gate Peterson module. */
typedef struct GatePeterson_Attrs {
    volatile Bits16 creatorProcId;
    /* Creator's processor Id */
    volatile Bits16 openerProcId;
    /* Opener's procId */
} GatePeterson_Attrs;

/* Structure defining internal object for the Gate Peterson.*/
struct GatePeterson_Object {
        IGateProvider_SuperObject; /* For inheritance from IGateProvider */
        IOBJECT_SuperObject;       /* For inheritance for IObject */
volatile GatePeterson_Attrs * attrs;
    /* Attributes assoicated with an instance */
volatile Bits16 *             flag[2u];
    /* Array holding flags for the processors */
volatile Bits16 *             turn;
    /* Indicates whoes turn it is now? */
    UInt16                    selfId;
    /* Self identifier */
    UInt16                    otherId;
    /* Other's identifier */
    UInt32                    nested;
    /* Counter to track nesting */
    IGateProvider_Handle      localGate;
    /* Pointer to the local lock handle */
    GateMP_LocalProtect       localProtect;
    /* Local Protect flag */
    GatePeterson_Params       params;
    /* the parameter structure */
    Int32                     refCount;
    /* Local reference count */
    SizeT                     cacheLineSize;
    /* Cache Line Size */
    Bool                      cacheEnabled;
    /* Is cache enabled? */
};


/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @var    GatePeterson_module
 *
 *  @brief  GatePeterson Module state object.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
GatePeterson_ModuleObject GatePeterson_module =
{
    .defaultCfg.defaultProtection  = GateMP_LocalProtect_INTERRUPT,
    .defaultCfg.numInstances       = 512,
    .defInstParams.sharedAddr      = 0x0,
    .defInstParams.resourceId      = 0x0,
    .defInstParams.regionId        = 0x0,
    .refCount                      = 0x0,
};


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/*
 *  Function to be called during
 *  1. module startup to complete the initialization of all static instances
 *  2. instance_init to complete the initialization of a dynamic instance
 *
 *  Main purpose is to set up shared memory
 */
Void GatePeterson_postInit(GatePeterson_Object *obj);


/* =============================================================================
 * APIS
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the GatePeterson module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to GatePeterson_setup filled in by
 *              the GatePeterson module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfgParams  Pointer to the GatePeterson module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         GatePeterson_setup
 */
Void
GatePeterson_getConfig (GatePeterson_Config * cfgParams)
{
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "GatePeterson_getConfig", cfgParams);

    GT_assert (curTrace, (cfgParams != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfgParams == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GatePeterson_getConfig",
                             GatePeterson_E_INVALIDARG,
                             "Argument of type (GatePeterson_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = Gate_enterSystem ();
        if (GatePeterson_module.refCount == 0) {
            Memory_copy ((Ptr) cfgParams,
                         (Ptr) &GatePeterson_module.defaultCfg,
                         sizeof (GatePeterson_Config));
        }
        else {
            Memory_copy ((Ptr) cfgParams,
                         (Ptr) &GatePeterson_module.cfg,
                         sizeof (GatePeterson_Config));
        }
        Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "GatePeterson_getConfig");
}


/*!
 *  @brief      Setup the GatePeterson module.
 *
 *              This function sets up the GatePeterson module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then GatePeterson_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call GatePeterson_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      cfg   Optional GatePeterson module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         GatePeterson_destroy, GatePeterson_getConfig
 */
Int32
GatePeterson_setup (const GatePeterson_Config * cfg)
{
    Int32               status = GatePeterson_S_SUCCESS;
    GatePeterson_Config tmpCfg;
    IArg                key;

    GT_1trace (curTrace, GT_ENTER, "GatePeterson_setup", cfg);

    if (cfg == NULL) {
        GatePeterson_getConfig (&tmpCfg);
        cfg = &tmpCfg;
    }

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    key = Gate_enterSystem ();
    if (GatePeterson_module.refCount >= 1) {
        status = GatePeterson_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "GatePeterson Module already initialized!");
        GatePeterson_module.refCount++;
        Gate_leaveSystem (key);
    }
    else {
        GatePeterson_module.refCount++;
        Gate_leaveSystem (key);

        /* Copy the cfg */
        Memory_copy ((Ptr) &GatePeterson_module.cfg,
                                 (Ptr) cfg,
                                 sizeof (GatePeterson_Config));
    }

    GT_1trace (curTrace, GT_LEAVE, "GatePeterson_setup", status);

    /*! @retval GatePeterson_S_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to destroy the GatePeterson module.
 *
 *  @sa         GatePeterson_setup
 */
Int32
GatePeterson_destroy (Void)
{
    Int32 status = GatePeterson_S_SUCCESS;
    IArg  key;

    GT_0trace (curTrace, GT_ENTER, "GatePeterson_destroy");

    key = Gate_enterSystem ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GatePeterson_module.refCount < 1) {
        Gate_leaveSystem (key);
        /*! @retval GatePeterson_E_INVALIDSTATE Module was not initialized */
        status = GatePeterson_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GatePeterson_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GatePeterson_module.refCount--;
        if (GatePeterson_module.refCount == 0) {
            Gate_leaveSystem (key);
            /* Clear cfg area */
            Memory_set ((Ptr) &GatePeterson_module.cfg,
                        0,
                        sizeof (GatePeterson_Config));
        }
        else {
            Gate_leaveSystem (key);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "GatePeterson_destroy", status);

    /*! @retval GatePeterson_S_SUCCESS Operation successful */
    return status;
}

/*!
 *  @brief      Function to return the number of instances configured in the
 *              module.
 *
 */
UInt32 GatePeterson_getNumInstances ()
{
    return (GatePeterson_module.defaultCfg.numInstances);
}

inline
Void GatePeterson_locksinit()
{
    /* Do nothing*/
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
 *  @sa         GatePeterson_create
 */
Void
GatePeterson_Params_init (GatePeterson_Params * params)
{
        IArg key;

    GT_1trace (curTrace, GT_ENTER, "GatePeterson_Params_init", params);

    GT_assert (curTrace, (params != NULL));

        key = Gate_enterSystem ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GatePeterson_module.refCount < 1) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GatePeterson_Params_init",
                             GatePeterson_E_INVALIDSTATE,
                             "Module was not initialized!");

                Gate_leaveSystem (key);
    }
    else if (params == NULL) {
        /* No retVal since this is a Void function. */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GatePeterson_Params_init",
                             GatePeterson_E_INVALIDARG,
                             "Argument of type (GatePeterson_Params *) is "
                             "NULL!");
                Gate_leaveSystem (key);
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                Gate_leaveSystem (key);
        Memory_copy (params,
                     &(GatePeterson_module.defInstParams),
                     sizeof (GatePeterson_Params));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "GatePeterson_Params_init");
}


/*
 *  ======== GatePeterson_Instance_init ========
 */
Int GatePeterson_Instance_init (      GatePeterson_Object *       obj,
                                      IGateProvider_Handle        localGate,
                                const GatePeterson_Params *       params)
{
    Int status = 0;

    GT_3trace (curTrace, GT_ENTER, "GatePeterson_Instance_init",
    obj, localGate, params);

    IGateProvider_ObjectInitializer (obj, GatePeterson);

    if (params->sharedAddr == NULL) {
                status = 1;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "GatePeterson_Instance_init",
                                     GatePeterson_E_FAIL,
                                     "params->sharedAddr passed is NULL!");
        }
        else {
            /* Create the local gate */
            obj->localGate = localGate;

            obj->cacheEnabled  = SharedRegion_isCacheEnabled(params->regionId);
            obj->cacheLineSize = SharedRegion_getCacheLineSize(params->regionId);

            /* Settings for both the creator and opener */
            if (obj->cacheLineSize > sizeof(GatePeterson_Attrs)) {
                obj->attrs   = params->sharedAddr;
                obj->flag[0] = (Bits16 *)((UInt32)(obj->attrs) + obj->cacheLineSize);
                obj->flag[1] =(Bits16 *)((UInt32)(obj->flag[0]) + obj->cacheLineSize);
                obj->turn = (Bits16 *)((UInt32)(obj->flag[1]) + obj->cacheLineSize);
            }
            else {
                obj->attrs   = params->sharedAddr;
                obj->flag[0] = (Bits16 *)((UInt32)(obj->attrs) +
                                                sizeof(GatePeterson_Attrs));
                obj->flag[1] = (Bits16 *)((UInt32)(obj->flag[0]) + sizeof(Bits16));
                obj->turn    = (Bits16 *)((UInt32)(obj->flag[1]) + sizeof(Bits16));
            }
            obj->nested  = 0;

            if (!params->openFlag) {
                /* Creating. */
                obj->selfId = 0;
                obj->otherId = 1;
                GatePeterson_postInit(obj);
            }
            else {
				if(obj->cacheEnabled == TRUE) {
                    Cache_inv((Ptr)obj->attrs, sizeof(GatePeterson_Attrs),
                                  Cache_Type_ALL, TRUE);
				}

                if (obj->attrs->creatorProcId == MultiProc_self()) {
                        /* Opening locally */
                        obj->selfId         = 0;
                        obj->otherId        = 1;
                }
                else {
                    /* Trying to open a gate remotely */
                    obj->selfId         = 1;
                    obj->otherId        = 0;
                    if (obj->attrs->openerProcId == MultiProc_INVALIDID) {
                            /* Opening remotely for the first time */
                            obj->attrs->openerProcId    = MultiProc_self();
                    }
                    else if (obj->attrs->openerProcId != MultiProc_self()) {
                            status = 3;
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "GatePeterson_Instance_init",
                                                 GatePeterson_E_FAIL,
                                                 "OpenerId is different!");
                    }

                    if (status >= 0) {
                        if (obj->cacheEnabled) {
                            Cache_wbInv ((Ptr)obj->attrs,
                                         sizeof(GatePeterson_Attrs),
                                                     Cache_Type_ALL, TRUE);
                        }
                    }
                }
            }
        }

    GT_1trace (curTrace, GT_LEAVE, "GatePeterson_Instance_init", status);

    return status;
}


/*
 *  ======== GatePeterson_Instance_finalize ========
 */
Void GatePeterson_Instance_finalize(GatePeterson_Object *obj, Int status)
{
    GT_2trace (curTrace, GT_ENTER, "GatePeterson_Instance_finalize",
    obj, status);

    switch (status) {
        /* No break here. Fall through to the next. */
        case 0:
        {
            /* Modify shared memory */
            obj->attrs->openerProcId = MultiProc_INVALIDID;
			if(obj->cacheEnabled == TRUE) {
                Cache_wbInv((Ptr)obj->attrs, sizeof(GatePeterson_Attrs), Cache_Type_ALL,
                                    TRUE);
			}
        }
        /* No break here. Fall through to the next. */

        case 1:
        {
            /* Nothing to be done. */
        }
    }

    GT_0trace (curTrace, GT_LEAVE, "GatePeterson_Instance_finalize");
}


/*
 *  ======== GatePeterson_enter ========
 */
IArg GatePeterson_enter(GatePeterson_Object *obj)
{
    IArg key;

    GT_1trace (curTrace, GT_ENTER, "GatePeterson_enter", obj);

    /* Enter local gate */
    key = IGateProvider_enter(obj->localGate);

    /* If the gate object has already been entered, return the key */
    obj->nested++;
    if (obj->nested > 1) {
        return (key);
    }

    /* Indicate that we need to use the resource. */
    *(obj->flag[obj->selfId]) = GatePeterson_BUSY ;
    if (obj->cacheEnabled) {
        Cache_wbInv((Ptr)obj->flag[obj->selfId], obj->cacheLineSize,
                    Cache_Type_ALL, TRUE);
    }

    /* Give away the turn. */
    *(obj->turn) = obj->otherId;

    if (obj->cacheEnabled) {
        Cache_wbInv((Ptr)obj->turn, obj->cacheLineSize, Cache_Type_ALL, TRUE);
        Cache_inv((Ptr)obj->flag[obj->otherId], obj->cacheLineSize,
            Cache_Type_ALL, TRUE);
    }

    /* Wait while other process is using the resource and has the turn. */
    while ((*(obj->flag[obj->otherId]) == GatePeterson_BUSY) &&
        (*(obj->turn) == obj->otherId)) {
        if (obj->cacheEnabled) {
            Cache_inv ((Ptr)obj->flag[obj->otherId], obj->cacheLineSize,
                                   Cache_Type_ALL, TRUE);
            Cache_inv((Ptr)obj->turn, obj->cacheLineSize, Cache_Type_ALL, TRUE);
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "GatePeterson_enter", key);

    return (key);
}

/*
 *  ======== GatePeterson_leave ========
 */
Void GatePeterson_leave(GatePeterson_Object *obj, IArg key)
{
    GT_2trace (curTrace, GT_ENTER, "GatePeterson_leave", obj, key);

    /* Release the resource and leave system gate. */
    obj->nested--;
    if (obj->nested == 0) {
        *(obj->flag[obj->selfId]) = GatePeterson_FREE;
        if (obj->cacheEnabled) {
            Cache_wbInv((Ptr)obj->flag[obj->selfId], obj->cacheLineSize,
                        Cache_Type_ALL, TRUE);
        }
    }

    /* Leave local gate */
    IGateProvider_leave(obj->localGate, key);

    GT_0trace (curTrace, GT_LEAVE, "GatePeterson_leave");
}


/*
 *  ======== GatePeterson_sharedMemReq ========
 */
SizeT GatePeterson_sharedMemReq(const IGateMPSupport_Params *params)
{
    SizeT memReq;

    GT_1trace (curTrace, GT_ENTER, "GatePeterson_sharedMemReq", params);

    if (SharedRegion_getCacheLineSize(params->regionId) >=
        sizeof(GatePeterson_Attrs)) {
        /*! 4 Because shared of shared memory usage (see GatePeterson.xdc) */
        memReq = 4 * SharedRegion_getCacheLineSize(params->regionId);
    }
    else {
        memReq = sizeof(GatePeterson_Attrs) + sizeof(Bits16) * 3;
    }

    GT_1trace (curTrace, GT_LEAVE, "GatePeterson_sharedMemReq", memReq);

    return(memReq);
}

/*
 *  ======== GatePeterson_query ========
 */
Bool GatePeterson_query(Int qual)
{
    Bool rc;

    GT_1trace (curTrace, GT_ENTER, "GatePeterson_query", qual);

    switch (qual) {
        case IGateProvider_Q_BLOCKING:
            /* GatePeterson is never blocking */
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

    GT_1trace (curTrace, GT_LEAVE, "GatePeterson_query", rc);

    return (rc);
}

Void GatePeterson_setReserved(UInt32 lockNum)
{
    Osal_printf ("WARN:GatePeterson_setReserved() not implemented!");
}

/*
 *************************************************************************
 *                       Internal functions
 *************************************************************************
 */

/*
 *  ======== GatePeterson_getReservedMask ========
 */
Bits32 *GatePeterson_getReservedMask(Void)
{
    /* This gate doesn't allow reserving resources */
    return (NULL);
}

/*
 *  ======== GatePeterson_postInit ========
 *  Function to be called during
 *  1. module startup to complete the initialization of all static instances
 *  2. instance_init to complete the initialization of a dynamic instance
 *
 *  Main purpose is to set up shared memory
 */
Void GatePeterson_postInit(GatePeterson_Object *obj)
{
    GT_1trace (curTrace, GT_ENTER, "GatePeterson_postInit", obj);

    /* Set up shared memory */
    *(obj->turn)       = 0;
    *(obj->flag[0])    = 0;
    *(obj->flag[1])    = 0;
    obj->attrs->creatorProcId  = MultiProc_self();
    obj->attrs->openerProcId   = MultiProc_INVALIDID;

    /*
     * Write everything back to memory. This assumes that obj->attrs is equal
     * to the shared memory base address
     */
    if (obj->cacheEnabled) {
        Cache_wbInv((Ptr)obj->attrs, sizeof(GatePeterson_Attrs), Cache_Type_ALL,
            FALSE);
        Cache_wbInv((Ptr)(obj->flag[0]), obj->cacheLineSize * 3, Cache_Type_ALL,
            TRUE);
    }

    GT_0trace (curTrace, GT_LEAVE, "GatePeterson_postInit");
}

/* Override the IObject interface to define craete and delete APIs */
IOBJECT_CREATE1 (GatePeterson, IGateProvider_Handle);
