/*
 *  @file   GateMP.c
 *
 *  @brief      GateMP
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
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/String.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/Gate.h>

/* Module level headers */
#include <ti/ipc/GateMP.h>
#include <ti/ipc/NameServer.h>
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/_GateMP.h>
#include <ti/syslink/inc/usr/Linux/GateMPDrv.h>
#include <ti/syslink/inc/GateMPDrvDefs.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/IObject.h>
#include <ti/syslink/inc/_SharedRegion.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/* Helper macros */
#define GETREMOTE(mask) ((GateMP_RemoteProtect)(mask >> 8))
#define GETLOCAL(mask)  ((GateMP_LocalProtect)(mask & 0xFF))
#define SETMASK(remoteProtect, localProtect) \
                        ((Bits32)(remoteProtect << 8 | localProtect))


/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */
/* Structure defining object for the Gate Peterson */
typedef struct GateMP_Object_tag {
    IGateProvider_SuperObject; /* For inheritance from IGateProvider */
    IOBJECT_SuperObject;       /* For inheritance for IObject */
    Ptr knlHandle;             /* Handle to kernel object */
} GateMP_Object;

/*!
 *  @brief  GateMP Module state object
 */
typedef struct GateMP_Module_State {
    UInt32        refCount;
    GateMP_Config cfg;
    GateMP_Handle defaultGate; /* Default gate */
} GateMP_Module_State;

/*!
 *  @brief  Structure defining parameters for the GateMP module.
 */
typedef struct _GateMP_Params {
    String name;
    UInt16 regionId;
    Ptr sharedAddr;
    GateMP_LocalProtect localProtect;
    GateMP_RemoteProtect remoteProtect;
    UInt32 resourceId;
    Bool openFlag;
} _GateMP_Params;


/* =============================================================================
 *  Globals
 * =============================================================================
 */
static GateMP_Module_State   GateMP_state = { .refCount = 0, };
static GateMP_Module_State * GateMP_module = &GateMP_state;
static GateMP_Object *       GateMP_firstObject = NULL;


/* =============================================================================
 * APIS
 * =============================================================================
 */
/*!
 *  @brief      Get the default configuration for the GateMP module.
 *
 *              This function can be called by the application to get their
 *              configuration parameter to GateMP_setup filled in by
 *              the GateMP module with the default parameters. If the
 *              user does not wish to make any change in the default parameters,
 *              this API is not required to be called.
 *
 *  @param      cfgParams  Pointer to the GateMP module configuration
 *                         structure in which the default config is to be
 *                         returned.
 *
 *  @sa         GateMP_setup
 */
Void
GateMP_getConfig (GateMP_Config * cfg)
{
    Int               status = GateMP_S_SUCCESS;
    GateMPDrv_CmdArgs cmdArgs;
    IArg              key;

    GT_1trace (curTrace, GT_ENTER, "GateMP_getConfig", cfg);

    GT_assert (curTrace, (cfg != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (cfg == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_getConfig",
                             GateMP_E_INVALIDARG,
                             "Argument of type (GateMP_Config *) passed "
                             "is null!");
    }
    else {
#endif
        /* Temporarily open the handle to get the configuration. */
        status = GateMPDrv_open ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMP_getConfig",
                                 status,
                                 "Failed to open driver handle!");
        }
        else {
#endif
            key = Gate_enterSystem ();
            cmdArgs.args.getConfig.config = cfg;
            status = GateMPDrv_ioctl(CMD_GATEMP_GETCONFIG, &cmdArgs);
            Gate_leaveSystem (key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_getConfig",
                    status, "API (through IOCTL) failed on kernel-side!");
            }
        }
#endif
        /* Close the driver handle. */
        GateMPDrv_close ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_ENTER, "GateMP_getConfig");


}


/*!
 *  @brief      Setup the GateMP module.
 *
 *              This function sets up the GateMP module. This function
 *              must be called before any other instance-level APIs can be
 *              invoked.
 *              Module-level configuration needs to be provided to this
 *              function. If the user wishes to change some specific config
 *              parameters, then GateMP_getConfig can be called to get
 *              the configuration filled with the default values. After this,
 *              only the required configuration values can be changed. If the
 *              user does not wish to make any change in the default parameters,
 *              the application can simply call GateMP_setup with NULL
 *              parameters. The default parameters would get automatically used.
 *
 *  @param      config   Optional GateMP module configuration. If provided
 *                    as NULL, default configuration is used.
 *
 *  @sa         GateMP_destroy, GateMP_getConfig
 */
Int32 GateMP_setup(const GateMP_Config *config)
{
    Int               status = GateMP_S_SUCCESS;
    GateMPDrv_CmdArgs cmdArgs;
    IArg              key;

    GT_1trace(curTrace, GT_ENTER, "GateMP_setup: config=0x%08x", config);

    key = Gate_enterSystem();
    GateMP_module->refCount++;

    if (GateMP_module->refCount > 1) {
        status = GateMP_S_ALREADYSETUP;
        GT_0trace(curTrace, GT_2CLASS, "GateMP Module already initialized!");
    }
    else {
        /* open the driver handle */
        status = GateMPDrv_open();

        if (status < 0) {
            GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                status, "Failed to open driver handle!");
        }
        else {
            cmdArgs.args.setup.config = (GateMP_Config *)config;
            status = GateMPDrv_ioctl(CMD_GATEMP_SETUP, &cmdArgs);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup",
                    status, "API (through IOCTL) failed on kernel-side!");
            }
            else {
                if (config != NULL) {
                    Memory_copy((Ptr) &GateMP_state.cfg, (Ptr)config,
                        sizeof(GateMP_Config));
                }
            }
        }

        /* Allocate and initialize the default remote in user space */
        GateMP_module->defaultGate = (GateMP_Handle)Memory_alloc(NULL,
            sizeof(GateMP_Object), 0, NULL);
        if (GateMP_module->defaultGate != NULL) {
            /* Set pointer to kernel object into the user handle. */
            ((GateMP_Object *)(GateMP_module->defaultGate))->knlHandle = NULL;
            IGateProvider_ObjectInitializer(GateMP_module->defaultGate, GateMP);
        }
        else {
            status = GateMP_E_FAIL;
            GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_setup", status,
                "Memory_alloc failed!");
        }
    }

    Gate_leaveSystem(key);

    GT_1trace(curTrace, GT_LEAVE, "GateMP_setup:[0x%08x]", status);

    /*! @retval GateMP_S_SUCCESS Operation successful */
    return(status);
}


/*!
 *  @brief      Function to destroy the GateMP module.
 *
 *  @sa         GateMP_setup
 */
Int32
GateMP_destroy (Void)
{
    Int               status = GateMP_S_SUCCESS;
    GateMPDrv_CmdArgs cmdArgs;
    IArg              key;

    GT_0trace (curTrace, GT_ENTER, "GateMP_destroy");

    key = Gate_enterSystem ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateMP_module->refCount < 1) {
        /*! @retval GateMP_E_INVALIDSTATE Module was not initialized */
        status = GateMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (--GateMP_module->refCount == 0) {
            /* Delete the user space structure for the default gate */
            Memory_free (NULL, (GateMP_module->defaultGate), sizeof (GateMP_Object));
            GateMP_module->defaultGate = NULL;

            status = GateMPDrv_ioctl (CMD_GATEMP_DESTROY, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "GateMP_destroy",
                                     status,
                                     "API (through IOCTL) failed on kernel-side!");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Close the driver handle. */
            GateMPDrv_close ();
        }
        else {
            /*! @retval GateMP_S_ALREADYSETUP Success: ProcMgr module has been
                                               already setup in this process */
            status = GateMP_S_ALREADYSETUP;
            GT_1trace (curTrace,
                       GT_1CLASS,
                       "ProcMgr module has been already setup in this process.\n"
                       "    RefCount: [%d]\n",
                       GateMP_module->refCount);
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    Gate_leaveSystem (key);

    GT_1trace (curTrace, GT_LEAVE, "GateMP_destroy", status);

    return status;
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
 *  @sa         GateMP_create
 */
Void
GateMP_Params_init (GateMP_Params * params)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int               status = GateMP_S_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    GateMPDrv_CmdArgs cmdArgs;
    IArg              key;

    GT_1trace (curTrace, GT_ENTER, "GateMP_Params_init", params);

    GT_assert (curTrace, (params != NULL));

    key = Gate_enterSystem ();

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (GateMP_module->refCount == 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_Params_init",
                             GateMP_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (params == NULL) {
        /* No retVal comment since this is a Void function. */
        status = GateMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_Params_init",
                             status,
                             "Argument of type (GateMP_Params *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        cmdArgs.args.ParamsInit.params = params;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        GateMPDrv_ioctl (CMD_GATEMP_PARAMS_INIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMP_Params_init",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    Gate_leaveSystem (key);

    GT_0trace (curTrace, GT_LEAVE, "GateMP_Params_init");


}


/*
 *  ======== GateMP_Instance_init__F ========
 */
Int GateMP_Instance_init (      GateMP_Object *  obj,
                          const _GateMP_Params * params)
{
    Int               status = 0;
    GateMPDrv_CmdArgs cmdArgs;
    UInt32            index;

    GT_2trace (curTrace, GT_ENTER, "GateMP_Instance_init", obj, params);

    cmdArgs.args.create.params = (GateMP_Params *) params;

    /* Translate sharedAddr to SrPtr. */
    if (params->sharedAddr != NULL) {
        index = SharedRegion_getId (params->sharedAddr);
        cmdArgs.args.create.sharedAddrSrPtr = SharedRegion_getSRPtr (
                                                         params->sharedAddr,
                                                         index);
    }
    else {
        cmdArgs.args.create.sharedAddrSrPtr = SharedRegion_INVALIDSRPTR;
    }
    if (params->name != NULL) {
        cmdArgs.args.create.nameLen = String_len (params->name) + 1;
    }
    else {
        cmdArgs.args.create.nameLen = 0;
    }

    status = GateMPDrv_ioctl (CMD_GATEMP_CREATE, &cmdArgs);
    if (status < 0) {
        status = 1;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_Instance_init",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
    else {
        /* Set pointer to kernel object into the user handle. */
        obj->knlHandle = cmdArgs.args.create.handle;
        IGateProvider_ObjectInitializer (obj, GateMP);
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_Instance_init", status);

    return status;
}


/*
 *  ======== GateMP_Instance_finalize ========
 */
Void GateMP_Instance_finalize (GateMP_Object *obj, Int status)
{
    Int               osStatus = 0;
    GateMPDrv_CmdArgs cmdArgs;

    switch (status) {
    case 1:
    {
    }
    case 0:
    {
        cmdArgs.args.deleteInstance.handle = obj->knlHandle;
        osStatus = GateMPDrv_ioctl (CMD_GATEMP_DELETE, &cmdArgs);
    }
    }
}


/*
 *  ======== _GateMP_create ========
 */
GateMP_Handle _GateMP_create(const _GateMP_Params *params)
{
    GateMP_Object * obj;
    IArg            key;

    GT_1trace (curTrace, GT_ENTER, "_GateMP_create", params);

    obj = (GateMP_Object *)Memory_alloc(NULL, sizeof(GateMP_Object), 0, NULL);

    if (obj != NULL) {
        obj->status = GateMP_Instance_init(obj, params);

        if (obj->status == 0) {
            key = Gate_enterSystem ();
            if (GateMP_firstObject == NULL) {
                GateMP_firstObject = obj;
                obj->next = NULL;
            }
            else {
                obj->next = GateMP_firstObject;
                GateMP_firstObject = obj;
            }
            Gate_leaveSystem (key);
        }
        else {
            Memory_free(NULL, obj, sizeof(GateMP_Object));
            obj = NULL;
        }
    }

    GT_1trace(curTrace, GT_LEAVE, "_GateMP_create: obj=0x%x", obj);

    return((GateMP_Handle)obj);
}


/*!
 *  @brief      Creates a new instance of GateMP module.
 *
 *  @param      params  Instance config-params structure.
 *
 *  @sa         GateMP_delete, GateMP_open, GateMP_close
 */
GateMP_Handle GateMP_create(const GateMP_Params * params)
{
    GateMP_Handle       handle = NULL;
    _GateMP_Params      _params;
    IArg                key;
    UInt32              refCount;

    GT_1trace(curTrace, GT_ENTER, "GateMP_create: params=0x%x", params);

    GT_assert(curTrace, (params != NULL));

    /* acquire the module reference count */
    key = Gate_enterSystem();
    refCount = GateMP_module->refCount;
    Gate_leaveSystem(key);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (refCount == 0) {
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_create",
            GateMP_E_INVALIDSTATE, "Modules is invalidstate!");
    }
    else if (params == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_create",
            GateMP_E_INVALIDARG, "params passed is NULL!");
    }
    else if ((params->sharedAddr == (UInt32)NULL)
            && (params->regionId >= SharedRegion_getNumRegions())) {
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_create",
            GateMP_E_INVALIDARG,
            "Provide a valid shared address or region id.");
    }
    else {
#endif
        Memory_set(&_params, 0, sizeof(_GateMP_Params));
        Memory_copy(&_params, (Ptr)params, sizeof(GateMP_Params));

        handle = _GateMP_create(&_params);

        if (handle == NULL) {
            GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_create",
                GateMP_E_FAIL, "_GateMP_create failed!");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace(curTrace, GT_LEAVE, "GateMP_create: [0x%x]", handle);

    /*! @retval valid-handle Operation successful*/
    /*! @retval NULL Operation failed */
    return((GateMP_Handle)handle);
}


/*
 *  ======== GateMP_delete ========
 */
Int GateMP_delete(GateMP_Handle * handle)
{
    Int             status = GateMP_S_SUCCESS;
    IArg            key;
    GateMP_Object * temp;
    GateMP_Object * obj;
    UInt32          refCount;

    GT_1trace(curTrace, GT_ENTER, "GateMP_delete: handle=0x%x", handle);

    key = Gate_enterSystem();
    refCount = GateMP_module->refCount;
    Gate_leaveSystem(key);

    if (refCount == 0) {
        status = GateMP_E_INVALIDSTATE;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_create",
            status, "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        status = GateMP_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_create",
            status, "handle is null!");
    }
    else if (*handle == NULL) {
        status = GateMP_E_INVALIDARG;
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_create",
            status, "*handle is null!");
    }

    if (status >= 0) {
        obj = (GateMP_Object *)(*handle);
        key = Gate_enterSystem();
        if (GateMP_firstObject == obj) {
            GateMP_firstObject = obj->next;
        }
        else {
            temp = GateMP_firstObject;
            while (temp != NULL) {
                if (temp->next == obj) {
                    temp->next = obj->next;
                    break;
                }
                else {
                    temp = temp->next;
                }
            }
            if (temp == NULL) {
                status = GateMP_E_INVALIDARG;
            }
        }
        Gate_leaveSystem (key);
    }

    if (status >= 0) {
        GateMP_Instance_finalize(obj, obj->status);
        Memory_free(NULL, (Ptr)obj, sizeof(GateMP_Object));
        *handle = NULL;
    }

    GT_1trace(curTrace, GT_LEAVE, "GateMP_delete: [0x%x]", status);

    return(status);
}


/*
 *  ======== GateMP_open ========
 */
Int GateMP_open(String name, GateMP_Handle *handle)
{
    Int               status = GateMP_S_SUCCESS;
    GateMP_Object *   obj;
    IArg              key;
    GateMPDrv_CmdArgs cmdArgs;

    GT_2trace (curTrace, GT_ENTER, "GateMP_open", name, handle);

    /* Assert that a pointer has been supplied */
    key = Gate_enterSystem ();
    if (GateMP_module->refCount == 0) {
        status =  GateMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_open",
                             GateMP_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        status =  GateMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_open",
                             status,
                             "handle is null!");
    }
    Gate_leaveSystem (key);

    if (status >= 0) {
        cmdArgs.args.open.name    = name;
        cmdArgs.args.open.nameLen = String_len (name) + 1;
        key = Gate_enterSystem ();
        status = GateMPDrv_ioctl (CMD_GATEMP_OPEN, &cmdArgs);
        Gate_leaveSystem (key);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMP_open",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
            obj = (GateMP_Object *) Memory_alloc (NULL,
                                                  sizeof (GateMP_Object),
                                                  0,
                                                  NULL);
            if (obj) {
                obj->knlHandle = cmdArgs.args.open.handle;
                IGateProvider_ObjectInitializer (obj, GateMP);
                key = Gate_enterSystem ();
                if (GateMP_firstObject == NULL) {
                    GateMP_firstObject = obj;
                    obj->next = NULL;
                }
                else {
                    obj->next = GateMP_firstObject;
                    GateMP_firstObject = obj;
                }
                Gate_leaveSystem (key);
                *handle = (Ptr)obj;
            }
            else {
                key = Gate_enterSystem ();
                cmdArgs.args.close.handle = cmdArgs.args.openByAddr.handle;
                GateMPDrv_ioctl (CMD_GATEMP_CLOSE, &cmdArgs);
                Gate_leaveSystem (key);
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "GateMP_open",
                                     status,
                                     "Memory_alloc failed!");
                *handle =  NULL;
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_open", status);

    return (status);
}

/*
 *  ======== GateMP_openByAddr ========
 */
Int GateMP_openByAddr (Ptr sharedAddr, GateMP_Handle *handle)
{
    Int                status = GateMP_S_SUCCESS;
    IArg               key;
    GateMPDrv_CmdArgs  cmdArgs;
    GateMP_Object *    obj;
    UInt32             index;

    GT_2trace (curTrace, GT_ENTER, "GateMP_openByAddr", sharedAddr, handle);

    /* Assert that a pointer has been supplied */
    key = Gate_enterSystem ();
    if (GateMP_module->refCount == 0) {
        status =  GateMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_openByAddr",
                             GateMP_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        status =  GateMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_openByAddr",
                             status,
                             "handle is null!");
    }
    Gate_leaveSystem (key);

    if (status >= 0) {
        index = SharedRegion_getId (sharedAddr);
        cmdArgs.args.openByAddr.sharedAddrSrPtr = SharedRegion_getSRPtr (
                                                                 sharedAddr,
                                                                 index);
        key = Gate_enterSystem ();
        status = GateMPDrv_ioctl (CMD_GATEMP_OPENBYADDR, &cmdArgs);
        Gate_leaveSystem (key);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMP_openByAddr",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
        else {
            obj = (GateMP_Object *) Memory_alloc (NULL,
                                                  sizeof (GateMP_Object),
                                                  0,
                                                  NULL);
            if (obj) {
                obj->knlHandle = cmdArgs.args.openByAddr.handle;
                IGateProvider_ObjectInitializer (obj, GateMP);
                key = Gate_enterSystem ();
                if (GateMP_firstObject == NULL) {
                    GateMP_firstObject = obj;
                    obj->next = NULL;
                }
                else {
                    obj->next = GateMP_firstObject;
                    GateMP_firstObject = obj;
                }
                Gate_leaveSystem (key);
                *handle = (Ptr)obj;
            }
            else {
                key = Gate_enterSystem ();
                cmdArgs.args.close.handle = cmdArgs.args.openByAddr.handle;
                GateMPDrv_ioctl (CMD_GATEMP_CLOSE, &cmdArgs);
                Gate_leaveSystem (key);
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "GateMP_openByAddr",
                                     status,
                                     "Memory_alloc failed!");
                *handle =  NULL;
            }
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_openByAddr", status);

    return (status);
}


/*!
 *  @brief      Closes previously opened instance of GateMP module.
 *              Close may not be used to finalize a gate whose handle has been
 *              acquired using create. In this case, delete should be used
 *              instead.
 *
 *  @param      handlePtr  Pointer to GateMP handle whose instance has
 *                         been opened
 *
 *  @sa         GateMP_create, GateMP_delete, GateMP_open
 */
Int32
GateMP_close (GateMP_Handle * handle)
{
    Int32             status = GateMP_S_SUCCESS;
    GateMPDrv_CmdArgs cmdArgs;
    IArg              key;
    GateMP_Object *   obj;
    GateMP_Object *   temp;

    GT_1trace (curTrace, GT_ENTER, "GateMP_close", handle);

    GT_assert (curTrace, (handle != NULL));

    /* Assert that a pointer has been supplied */
    key = Gate_enterSystem ();
    if (GateMP_module->refCount == 0) {
        status =  GateMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_close",
                             GateMP_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        status =  GateMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_close",
                             status,
                             "handle is null!");
    }
    else if (*handle == NULL) {
        status =  GateMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_close",
                             status,
                             "*handle is null!");
    }
    Gate_leaveSystem (key);

    if (status >= 0) {
        obj = (GateMP_Object *) (*handle);
        key = Gate_enterSystem ();
        if ((GateMP_Object *)*handle == GateMP_firstObject) {
            GateMP_firstObject = obj->next;
        }
        else {
            temp = GateMP_firstObject;
            while (temp) {
                if (temp->next == obj) {
                    temp->next = obj->next;
                    break;
                }
                else {
                    temp = temp->next;
                }
            }
            if (temp == NULL) {
                Gate_leaveSystem (key);
                status = GateMP_E_INVALIDARG;
            }
        }

        if (status >= 0) {
            cmdArgs.args.close.handle = obj->knlHandle;
            status = GateMPDrv_ioctl (CMD_GATEMP_CLOSE, &cmdArgs);
            Gate_leaveSystem (key);
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "GateMP_openByAddr",
                                     status,
                                     "API (through IOCTL) failed on"
                                     "kernel-side!");
            }
            Memory_free (NULL, obj, sizeof (GateMP_Object));
            *handle = NULL;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_close", status);

    /*! @retval GateMP_S_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Enters the GateMP instance.
 *
 *  @param      handle  Handle to previously created/opened instance.
 *
 *  @sa         GateMP_leave
 */
IArg
GateMP_enter (GateMP_Handle handle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32             status = GateMP_S_SUCCESS;
#endif
    IArg              key;
    GateMPDrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "GateMP_enter", handle);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)

    /* Assert that a pointer has been supplied */
    key = Gate_enterSystem ();
    if (GateMP_module->refCount == 0) {
        status =  GateMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_enter",
                             GateMP_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        status =  GateMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_enter",
                             status,
                             "handle is null!");
    }
    Gate_leaveSystem (key);

    if (status >= 0) {
#endif
        cmdArgs.args.enter.handle = ((GateMP_Object *) handle)->knlHandle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
            GateMPDrv_ioctl (CMD_GATEMP_ENTER, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMP_enter",
                                 status,
                                 "API (through IOCTL) failed on"
                                 "kernel-side!");
        }
        else {
#endif
            key = cmdArgs.args.enter.flags;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
    }
#endif
    GT_1trace (curTrace, GT_LEAVE, "GateMP_enter", key);

    /*! @retval Key Key to be provided to GateMP_leave. */
    return (key);
}


/*!
 *  @brief      Leaves the GateMP instance.
 *
 *  @param      handle  Handle to previously created/opened instance.
 *  @param      key     Key received from GateMP_enter call.
 *
 *  @sa         GateMP_enter
 */
Void
GateMP_leave (GateMP_Handle handle, IArg   key)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int32             status = GateMP_S_SUCCESS;
    IArg              _key;
#endif
    GateMPDrv_CmdArgs cmdArgs;

    GateMP_Object *   obj;

    GT_2trace (curTrace, GT_ENTER, "GateMP_leave", handle, key);

    GT_assert (curTrace, (handle != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    /* Assert that a pointer has been supplied */
    _key = Gate_enterSystem ();
    if (GateMP_module->refCount == 0) {
        status =  GateMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_leave",
                             GateMP_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    else if (handle == NULL) {
        status =  GateMP_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_leave",
                             status,
                             "handle is null!");
    }
    Gate_leaveSystem (_key);

    if (status >= 0) {
#endif
        obj  = (GateMP_Object *) handle;
        cmdArgs.args.leave.handle = obj->knlHandle;
        cmdArgs.args.leave.flags  = key;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        status =
#endif
            GateMPDrv_ioctl (CMD_GATEMP_LEAVE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMP_leave",
                                 status,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }
#endif
    GT_0trace (curTrace, GT_LEAVE, "GateMP_leave");
}


/*!
 *  @brief      Returns the shared memory size requirement for a single
 *              instance.
 *
 *  @param      params  GateMP create parameters
 *
 *  @sa         GateMP_create
 */
UInt32
GateMP_sharedMemReq (const GateMP_Params * params)
{
    Int32             status = GateMP_S_SUCCESS;
    UInt32            retVal = 0;
    GateMPDrv_CmdArgs cmdArgs;

    GT_1trace (curTrace, GT_ENTER, "GateMP_sharedMemReq", params);

    GT_assert (curTrace, (params != NULL));

    cmdArgs.args.sharedMemReq.params = (GateMP_Params *) params;
    status = GateMPDrv_ioctl (CMD_GATEMP_SHAREDMEMREQ, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_sharedMemReq",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        retVal = cmdArgs.args.sharedMemReq.retVal;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "GateMP_sharedMemReq", retVal);

    /*! @retval Amount-of-shared-memory-required On success */
    return retVal;
}


/*!
 *  @brief      Returns the default GateMP remote instance.
 *
 *  @param      None
 *
 *  @sa         GateMP_create
 */
GateMP_Handle
GateMP_getDefaultRemote ()
{
    Int32             status = GateMP_S_SUCCESS;
    GateMP_Handle     handle = NULL;
    GateMPDrv_CmdArgs cmdArgs;

    GT_0trace (curTrace, GT_ENTER, "GateMP_getDefaultRemote");

    status = GateMPDrv_ioctl (CMD_GATEMP_GETDEFAULTREMOTE, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_getDefaultRemote",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
    else {
#endif

    ((GateMP_Object *)(GateMP_module->defaultGate))->knlHandle = cmdArgs.args.getDefaultRemote.handle;

    handle = GateMP_module->defaultGate;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace (curTrace, GT_LEAVE, "GateMP_getDefaultRemote", handle);

    /*! @retval Default-remote-GateMP-instance   On success */
    return handle;
}

/*!
 *  @brief      Returns the shared address of GateMP instance.
 *
 *  @param      None
 */
SharedRegion_SRPtr GateMP_getSharedAddr(GateMP_Handle handle)
{
    Int32             status = GateMP_S_SUCCESS;
    GateMPDrv_CmdArgs cmdArgs;
    Int32             retVal = 0;
    GateMP_Object * obj  = (GateMP_Object *) handle;

    GT_0trace(curTrace, GT_ENTER, "GateMP_getSharedAddr");

    cmdArgs.args.getSharedAddr.handle = obj->knlHandle;
    status = GateMPDrv_ioctl(CMD_GATEMP_GETSHAREDADDR, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason(curTrace, GT_4CLASS, "GateMP_getSharedAddr",
                status, "API (through IOCTL) failed on kernel-side!");
    }
    else {
#endif
        retVal = cmdArgs.args.getSharedAddr.retVal;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif

    GT_1trace(curTrace, GT_LEAVE, "GateMP_getSharedAddr", retVal);

    return (SharedRegion_SRPtr)retVal;
}

/*!
 *  @brief      Reserved gateHwspinlocks for Non SysLink/IPC use.
 *
 *  @param       None
 *
 *  @sa         GateMP_setReserved
 */
Void
GateMP_setReserved (UInt32 remoteProtectType,UInt32 lockNum)
{
    Int32             status = GateMP_S_SUCCESS;
    GateMPDrv_CmdArgs cmdArgs;

    GT_0trace (curTrace, GT_ENTER, "GateMP_setReserved");
    cmdArgs.args.setReserved.lockNum = lockNum;
    cmdArgs.args.setReserved.remoteProtectType = remoteProtectType;
    status = GateMPDrv_ioctl (CMD_GATEMP_SETRESERVED, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_setReserved",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif

}

/*
 *  ======== GateMP_getLocalProtect ========
 */
GateMP_LocalProtect GateMP_getLocalProtect(GateMP_Handle handle)
{
    GT_assert (curTrace, (handle != NULL));

    /* TBD: Implement. */
    return ((GateMP_LocalProtect) NULL);
}

/*
 *  ======== GateMP_getRemoteProtect ========
 */
GateMP_RemoteProtect GateMP_getRemoteProtect(GateMP_Handle handle)
{
    GT_assert (curTrace, (handle != NULL));

    /* TBD: Implement. */
    return ((GateMP_RemoteProtect) NULL);
}

/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/*!
 *  @brief      Returns the gatepeterson kernel object pointer.
 *
 *  @param      handle  Handle to previously created/opened instance.
 *
 *  @sa         GateMP_create
 */
Void *
GateMP_getKnlHandle (Void * handle)
{
    Void * kHandle = NULL;
    Int    status = GateMP_S_SUCCESS;
    IArg   key;

    GT_1trace (curTrace, GT_ENTER, "GateMP_getKnlHandle", handle);

    /* To allow NULL for default remote gate */
    /* GT_assert (curTrace, (handle != NULL)); */

    /* Assert that a pointer has been supplied */
    key = Gate_enterSystem ();
    if (GateMP_module->refCount == 0) {
        status =  GateMP_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMP_getKnlHandle",
                             GateMP_E_INVALIDSTATE,
                             "Modules is invalidstate!");
    }
    Gate_leaveSystem (key);

    if ((status >= 0) && (handle != NULL)) {
        kHandle = ((GateMP_Object *) (handle))->knlHandle;
        if (kHandle == NULL) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMP_getKnlHandle",
                                 GateMP_E_INVALIDSTATE,
                                 "API (through IOCTL) failed on kernel-side!");
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "GateMP_getKnlHandle", kHandle);

    /*! @retval Kernel-Object-handle Operation successfully completed. */
    return (kHandle);
}
