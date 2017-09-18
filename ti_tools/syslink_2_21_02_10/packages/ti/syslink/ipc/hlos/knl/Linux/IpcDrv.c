/*
 *  @file   IpcDrv.c
 *
 *  @brief      OS-specific implementation of Ipc driver for Linux
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



/*  Defined to include MACROS EXPORT_SYMBOL. This must be done before including
 *  module.h
 */
#if !defined (EXPORT_SYMTAB)
#define EXPORT_SYMTAB
#endif

/* OS-specific headers */
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/osal/OsalTypes.h>
#include <ti/syslink/utils/IGateProvider.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/knl/OsalDriver.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/GateMutex.h>

/* Module specific header files */
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>
#include <ti/syslink/IpcHost.h>
#include <ti/syslink/inc/_Ipc.h>
#include <ti/syslink/inc/knl/IpcKnl.h>
#include <ti/syslink/inc/knl/Linux/IpcDrv.h>
#include <ti/syslink/inc/IpcDrvDefs.h>
#include <ti/ipc/SharedRegion.h>

/* platform friend function */
extern Void Platform_terminateEventConfig(UInt16 procId, UInt32 *eventId,
        UInt16 *lineId);
extern Void Platform_terminateHandler(UInt16 procId);


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* Ipc functions */
EXPORT_SYMBOL(Ipc_getConfig);
EXPORT_SYMBOL(Ipc_setup);
EXPORT_SYMBOL(Ipc_destroy);
EXPORT_SYMBOL(Ipc_create);
EXPORT_SYMBOL(Ipc_control);
EXPORT_SYMBOL(Ipc_readConfig);
EXPORT_SYMBOL(Ipc_writeConfig);

/* IpcDrv functions */
EXPORT_SYMBOL(IpcDrv_registerDriver);
EXPORT_SYMBOL(IpcDrv_unregisterDriver);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for Ipc.
 */
#define IPC_DRV_MINOR_NUMBER  11

#define RTID_COUNT 256                  /* number of rtid's available */
#define MASK_SZ (RTID_COUNT / 32)       /* runtime id mask array size */

/*!
 *  @brief  IpcDrv Module state object
 */
typedef struct {
    GateMutex_Handle    gate;           /* gate for list traversal */
    List_Handle         events;         /* terminate events (IpcDrv_Event) */
    UInt8               rtid;           /* last used runtime id */
    UInt32              mask[MASK_SZ];  /* used runtime ids */
    List_Handle         procs;          /* process list (IpcDrv_ProcId) */
} IpcDrv_ModuleObject;

/*!
 *  @brief  IpcDrv Event list element
 */
typedef struct {
    List_Elem   elem;
    Osal_Pid    pid;
    UInt32      payload;
    UInt16      procId;
    Int         policy;
} IpcDrv_Event;

typedef struct {
    List_Elem   elem;
    Osal_Pid    pid;
    UInt8       rtid;
} IpcDrv_ProcId;


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */

/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int IpcDrv_drvopen (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int IpcDrv_drvclose (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long IpcDrv_drvioctl (struct file *  filp,
                             unsigned int   cmd,
                             unsigned long  args);

static Void IpcDrv_setup(Void);
static Void IpcDrv_destroy(Void);

#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init IpcDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit IpcDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @var    IpcDrv_state
 *
 *  @brief  IpcDrv state object variable
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif
IpcDrv_ModuleObject IpcDrv_state =
{
    .gate = NULL,
    .events = NULL,
    .rtid = 0,
    .procs = NULL
};


/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations IpcDrv_driverOps = {
    open:    IpcDrv_drvopen,
    release: IpcDrv_drvclose,
    unlocked_ioctl:   IpcDrv_drvioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool IpcDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool IpcDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool IpcDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 IpcDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for Ipc
 */
static Ptr IpcDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */

/*
 *  ======== IpcDrv_setup ========
 */
static Void IpcDrv_setup(Void)
{
    Int i;

    /* create the local gate instance */
    IpcDrv_state.gate = GateMutex_create(NULL, NULL);

    if (IpcDrv_state.gate == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcDrv_setup",
                Ipc_E_MEMORY, "GateMutex_create failed");
    }

    /* create the terminate event list */
    IpcDrv_state.events = List_create(NULL, NULL);

    if (IpcDrv_state.events == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcDrv_setup",
                Ipc_E_MEMORY, "List_create failed");
    }

    /* create the process list */
    IpcDrv_state.procs = List_create(NULL, NULL);

    if (IpcDrv_state.procs == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcDrv_setup",
                Ipc_E_MEMORY, "List_create failed");
    }

    /* clear the runtime id mask array */
    for (i = 0; i < MASK_SZ; i++) {
        IpcDrv_state.mask[i] = 0;
    }

    /* mask off zero, not a good runtime id */
    IpcDrv_state.mask[0] = 0x1;

}

/*
 *  ======== IpcDrv_destroy ========
 */
static Void IpcDrv_destroy(Void)
{
    if (IpcDrv_state.procs != NULL) {
        List_delete(&IpcDrv_state.procs);
    }

    IpcDrv_state.rtid = 0;

    if (IpcDrv_state.events != NULL) {
        List_delete(&IpcDrv_state.events);
    }

    if (IpcDrv_state.gate != NULL) {
        GateMutex_delete(&IpcDrv_state.gate);
    }
}

/*!
 *  @brief  Register the Ipc with OsalDriver
 */
Ptr IpcDrv_registerDriver(Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace(curTrace, GT_ENTER, "IpcDrv_registerDriver:");

    /* setup the module state */
    IpcDrv_setup();

    osalHandle = OsalDriver_registerDriver("Ipc", &IpcDrv_driverOps,
            IPC_DRV_MINOR_NUMBER);

    if (osalHandle == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcDrv_registerDriver",
                Ipc_E_INVALIDARG, "OsalDriver_registerDriver failed!");
    }

    GT_1trace(curTrace, GT_LEAVE, "IpcDrv_registerDriver", osalHandle);
    return((Ptr)osalHandle);
}

/*!
 *  @brief  Register the Ipc with OsalDriver
 */
Void IpcDrv_unregisterDriver(Ptr *drvHandle)
{
    GT_1trace(curTrace, GT_ENTER, "IpcDrv_unregisterDriver", drvHandle);

    OsalDriver_unregisterDriver((OsalDriver_Handle *)drvHandle);

    /* finalize the module state */
    IpcDrv_destroy();

    GT_0trace(curTrace, GT_LEAVE, "IpcDrv_unregisterDriver");
}

/*!
 *  @brief  Linux specific function to open the driver.
 */
int
IpcDrv_drvopen (struct inode * inode, struct file * filp)
{
    OsalDriver_Handle handle = container_of (inode->i_cdev,
                                             OsalDriver_Object,
                                             cdev);
    filp->private_data = handle;

    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
int IpcDrv_drvclose(struct inode *inode, struct file *filp)
{
    Int status;
    pid_t pid;
    IArg key;
    List_Elem *elem;
    IpcDrv_Event *event;
    IpcDrv_ProcId *proc;
    UInt8 rtid = 0;
    Int idx, bit;
    UInt32 mask;
    UInt32 eventId;
    UInt16 lineId;

    GT_0trace(curTrace, GT_ENTER, "IpcDrv_drvclose");

    /* get the pid associated with the file pointer */
    pid = pid_nr(filp->f_owner.pid);

    /* enter gate */
    key = IGateProvider_enter((IGateProvider_Handle)(IpcDrv_state.gate));

    /* search event list for all events with terminated pid */
    elem = NULL;
    while ((elem = List_next(IpcDrv_state.events, elem)) != NULL) {
        event = (IpcDrv_Event *)elem;

        if (event->pid == pid) {
            List_remove(IpcDrv_state.events, elem);

            GT_assert(curTrace, (Ipc_isAttached(event->procId)));

            switch (event->policy) {
                case Ipc_TERMINATEPOLICY_STOP:
                    Platform_terminateHandler(event->procId);
                    break;

                case Ipc_TERMINATEPOLICY_NOTIFY:
                    Platform_terminateEventConfig(event->procId, &eventId,
                            &lineId);
                    status = Notify_sendEvent(event->procId, lineId, eventId,
                            event->payload, FALSE);

                    if (status < 0) {
                        GT_setFailureReason(curTrace, GT_4CLASS,
                                "IpcDrv_drvclose", status,
                                "failed to send terminate event");
                    }
                    break;

                default:
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "IpcDrv_drvclose", -1, "unknown terminate policy");
                    break;
            }

            Memory_free(NULL, event, sizeof(IpcDrv_Event));

            /* must traverse list form begining because elem was removed */
            elem = NULL;
        }
    }

    /* remove process from procs list */
    elem = NULL;
    while ((elem = List_next(IpcDrv_state.procs, elem)) != NULL) {
        proc = (IpcDrv_ProcId *)elem;

        if (proc->pid == pid) {
            rtid = proc->rtid;
            List_remove(IpcDrv_state.procs, elem);
            Memory_free(NULL, proc, sizeof(IpcDrv_ProcId));
            break;
        }
    }

    /* clear rtid in bit mask */
    if (rtid != 0) {
        idx = rtid / 32;
        bit = rtid - (idx * 32);
        mask = 0x1 << bit;
        IpcDrv_state.mask[idx] &= ~(mask);
    }

    /* reset the rtid if process list is empty */
    if (List_empty(IpcDrv_state.procs)) {
        IpcDrv_state.rtid = 0;
    }

    /* leave gate */
    IGateProvider_leave((IGateProvider_Handle)(IpcDrv_state.gate), key);

    GT_0trace(curTrace, GT_LEAVE, "IpcDrv_drvclose");
    return(0);
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static long IpcDrv_drvioctl(struct file *filp, unsigned int cmd,
        unsigned long args)
{
    Int32               status = Ipc_S_SUCCESS;
    int                 osStatus = 0;
    IpcDrv_CmdArgs      cmdArgs;

    GT_3trace(curTrace, GT_ENTER, "IpcDrv_drvioctl", filp, cmd, args);

    /* copy args from user-side */
    status = copy_from_user((Ptr)&cmdArgs, (const Ptr)(args),
            sizeof(IpcDrv_CmdArgs));

    if (status != 0) {
        GT_setFailureReason(curTrace, GT_4CLASS, "IpcDrv_drvioctl", status,
            "copy_from_user failed");
        status = Ipc_E_FAIL;
        osStatus = -EFAULT;
        goto leave;
    }

    switch (cmd) {
        case CMD_IPC_CONTROL: {
            if (cmdArgs.args.control.arg != NULL) {
                status = Ipc_control(cmdArgs.args.control.procId,
                        cmdArgs.args.control.cmdId,
                        &(cmdArgs.args.control.arg));
            }
            else {
                status = Ipc_control(cmdArgs.args.control.procId,
                        cmdArgs.args.control.cmdId, NULL);
            }
        }
        break;

        case CMD_IPC_READCONFIG: {
            Ptr cfg;

            /* allocate memory for the cfg */
            cfg = Memory_alloc(NULL, cmdArgs.args.readConfig.size, 0, NULL);

            if (cfg == NULL) {
                status = Ipc_E_MEMORY;
            }

            /* invoke ipc api */
            if (status == Ipc_S_SUCCESS) {
                status = Ipc_readConfig(cmdArgs.args.readConfig.remoteProcId,
                        cmdArgs.args.readConfig.tag, cfg,
                        cmdArgs.args.readConfig.size);
            }

            /* copy data to user */
            if (status == Ipc_S_SUCCESS) {
                status = copy_to_user(cmdArgs.args.readConfig.cfg, cfg,
                        cmdArgs.args.readConfig.size);

                if (status != 0) {
                    status = Ipc_E_FAIL;
                    osStatus = -EFAULT;
                }
            }

            if (cfg != NULL) {
                Memory_free(NULL, cfg, cmdArgs.args.readConfig.size);
            }
        }
        break;

        case CMD_IPC_WRITECONFIG: {
            Ptr cfg = NULL;
            SizeT size = cmdArgs.args.writeConfig.size;

            /* cfg will be null when freeing the config object */
            if (cmdArgs.args.writeConfig.cfg != NULL) {

                /* allocate buffer to copy user data into */
                cfg = Memory_alloc(NULL, size, 0, NULL);

                if (cfg == NULL) {
                    status = Ipc_E_MEMORY;
                }

                /* copy data from user to kernel */
                if (status == Ipc_S_SUCCESS) {
                    status = copy_from_user(cfg, cmdArgs.args.writeConfig.cfg,
                            size);

                    if (status != 0) {
                        status = Ipc_E_FAIL;
                        osStatus = -EFAULT;
                    }
                }
            }

            /* invoke ipc api */
            if (status == Ipc_S_SUCCESS) {
                status = Ipc_writeConfig(cmdArgs.args.writeConfig.remoteProcId,
                        cmdArgs.args.writeConfig.tag, cfg, size);

                if (status != 0) {
                    status = Ipc_E_FAIL;
                }
            }

            if (cfg != NULL) {
                Memory_free(NULL, cfg, size);
            }
        }
        break;

        case CMD_IPC_ADDTERMINATEEVENT: {
            IpcDrv_Event *event;

            /* create new event object */
            event = Memory_alloc(NULL, sizeof(IpcDrv_Event), 0, NULL);

            if (event == NULL) {
                status = Ipc_E_MEMORY;
            }

            /* add event to list */
            if (status == Ipc_S_SUCCESS) {
                event->pid = cmdArgs.args.addTermEvent.pid;
                event->payload = cmdArgs.args.addTermEvent.payload;
                event->procId = cmdArgs.args.addTermEvent.procId;
                event->policy = cmdArgs.args.addTermEvent.policy;

                List_put(IpcDrv_state.events, (List_Elem *)(event));
            }
        }
        break;

        case CMD_IPC_REMOVETERMINATEEVENT: {
            List_Elem *elem = NULL;
            IpcDrv_Event *event;
            IGateProvider_Handle gate;
            IArg key;

            /* enter gate */
            gate = (IGateProvider_Handle)(IpcDrv_state.gate);
            key = IGateProvider_enter(gate);

            /* find and remove the requested event from the list */
            while ((elem = List_next(IpcDrv_state.events, elem)) != NULL) {
                event = (IpcDrv_Event *)elem;

                if ((event->pid == cmdArgs.args.removeTermEvent.pid) &&
                    (event->procId == cmdArgs.args.removeTermEvent.procId)) {

                    List_remove(IpcDrv_state.events, elem);
                    Memory_free(NULL, event, sizeof(IpcDrv_Event));

                    /* done, exit the loop */
                    break;
                }
            }

            /* leave gate */
            IGateProvider_leave(gate, key);
        }
        break;

        case CMD_IPC_RUNTIMEID: {
            IpcDrv_ProcId *proc = NULL;
            List_Elem *elem;
            IGateProvider_Handle gate;
            IArg key;
            Bool newRtid = TRUE;
            UInt8 rtid = 0;
            Int i, idx, bit;
            UInt32 mask;

            /* enter gate */
            gate = (IGateProvider_Handle)(IpcDrv_state.gate);
            key = IGateProvider_enter(gate);

            /* look for pid on process list */
            elem = NULL;
            while ((elem = List_next(IpcDrv_state.procs, elem)) != NULL) {
                proc = (IpcDrv_ProcId *)elem;

                if (proc->pid == cmdArgs.args.runtimeId.pid) {
                    cmdArgs.args.runtimeId.rtid = proc->rtid;
                    newRtid = FALSE;
                    break;
                }
            }

            /* if needed, find next available rtid */
            if (newRtid) {
                rtid = IpcDrv_state.rtid; /* start from last allocated value */
                for (i = 0; i < RTID_COUNT; i++) {
                    rtid = (++rtid == RTID_COUNT ? 1 : rtid);
                    idx = rtid / 32;            /* array index */
                    bit = rtid - (idx * 32);    /* bit offset */
                    mask = 0x1 << bit;

                    /* test if rtid is available */
                    if ((IpcDrv_state.mask[idx] & mask) == 0) {
                        IpcDrv_state.mask[idx] |= mask; /* reserve rtid */
                        IpcDrv_state.rtid = rtid;       /* cache last used */
                        break;
                    }
                }

                if (i == RTID_COUNT) {
                    status = Ipc_E_FAIL;
                }
            }

            /* allocate new proc object */
            if ((status == Ipc_S_SUCCESS) && newRtid) {
                proc = Memory_alloc(NULL, sizeof(IpcDrv_ProcId), 0, NULL);

                if (proc == NULL) {
                    status = Ipc_E_MEMORY;
                }
            }

            /* add proc object to list and return new runtimeId */
            if ((status == Ipc_S_SUCCESS) && newRtid) {
                proc->pid = cmdArgs.args.runtimeId.pid;
                proc->rtid = rtid;
                List_put(IpcDrv_state.procs, (List_Elem *)(proc));
                cmdArgs.args.runtimeId.rtid = proc->rtid;
            }

            /* leave gate */
            IGateProvider_leave(gate, key);
        }
        break;

        case CMD_IPC_ISATTACHED: {
            cmdArgs.args.isAttached.attached = Ipc_isAttached(
                    cmdArgs.args.isAttached.remoteProcId);
        }
        break;

        default: {
            status = Ipc_E_INVALIDARG;
            GT_setFailureReason(curTrace, GT_4CLASS, "IpcDrv_drvioctl",
                    status, "unsupported ioctl command specified");
        }
        break;
    }

    /* set status and copy common args to user-side */
    if (osStatus == 0) {
        cmdArgs.apiStatus = status;
        status = copy_to_user((Ptr)args, (const Ptr)&cmdArgs,
                sizeof(IpcDrv_CmdArgs));

        if (status != 0) {
            osStatus = -EFAULT;
        }
    }

leave:
    GT_1trace(curTrace, GT_LEAVE, "IpcDrv_drvioctl", osStatus);

    /*! @retval 0 Operation successfully completed. */
    return(osStatus);
}


/** ============================================================================
 *  Functions required for multiple .ko modules configuration
 *  ============================================================================
 */
#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization  function for Linux driver.
 */
static
int __init IpcDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("Ipc sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("IpcDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        IpcDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if (    (IpcDrv_enableTrace != 0)
            &&  (IpcDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (IpcDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (IpcDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        IpcDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (IpcDrv_enableTraceEnter != 0)
            &&  (IpcDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (IpcDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        IpcDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                            NULL,
                                                            16);
        if (    (IpcDrv_enableTraceFailure != 0)
            &&  (IpcDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (IpcDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        IpcDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (IpcDrv_enableTraceFailure != 1)
            &&  (IpcDrv_enableTraceFailure != 2)
            &&  (IpcDrv_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            IpcDrv_traceClass =
                       IpcDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= IpcDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    IpcDrv_osalDrvHandle = IpcDrv_registerDriver ();
    if (IpcDrv_osalDrvHandle == NULL) {
        /*! @retval Ipc_E_OSFAILURE Failed to register Ipc
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "IpcDrv_initializeModule",
                          Ipc_E_OSFAILURE,
                          "Failed to register Ipc driver with OS!");
    }

    Osal_printf ("IpcDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit IpcDrv_finalizeModule (void)
{
    Osal_printf ("Entered IpcDrv_finalizeModule\n");

    IpcDrv_unregisterDriver (&(IpcDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving IpcDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (IpcDrv_initializeModule);
module_exit (IpcDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
