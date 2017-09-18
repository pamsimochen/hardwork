/*
 *  @file   NameServerDrv.c
 *
 *  @brief      OS-specific implementation of NameServer driver for Linux
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

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/inc/knl/OsalDriver.h>
#include <ti/syslink/osal/OsalTypes.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/ResTrack.h>
#include <ti/syslink/utils/Trace.h>

/* Linux specific header files */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/pid.h>

/* Module specific header files */
#include <ti/syslink/inc/_NameServer.h>
#include <ti/ipc/NameServer.h>
#include <ti/syslink/inc/knl/Linux/NameServerDrv.h>
#include <ti/syslink/inc/NameServerDrvDefs.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* NameServer functions */
EXPORT_SYMBOL(NameServer_setup);
EXPORT_SYMBOL(NameServer_destroy);
EXPORT_SYMBOL(NameServer_Params_init);
EXPORT_SYMBOL(NameServer_create);
EXPORT_SYMBOL(NameServer_delete);
EXPORT_SYMBOL(NameServer_add);
EXPORT_SYMBOL(NameServer_addUInt32);
EXPORT_SYMBOL(NameServer_get);
EXPORT_SYMBOL(NameServer_getLocal);
EXPORT_SYMBOL(NameServer_match);
EXPORT_SYMBOL(NameServer_remove);
EXPORT_SYMBOL(NameServer_removeEntry);
EXPORT_SYMBOL(NameServer_getHandle);
EXPORT_SYMBOL(NameServer_registerRemoteDriver);
EXPORT_SYMBOL(NameServer_unregisterRemoteDriver);

/* NameServerDrv functions */
EXPORT_SYMBOL(NameServerDrv_registerDriver);
EXPORT_SYMBOL(NameServerDrv_unregisterDriver);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for NameServer.
 */
#define NAMESERVER_DRV_MINOR_NUMBER 2

typedef struct {
    ResTrack_Handle     resTrack;
} NameServerDrv_ModuleObject;

typedef struct {
    Ptr         handle;
} NameServerDrv_CreateRes;

typedef struct {
    Ptr         handle;
    String      name;
    Int         len;
    Ptr         entry;
} NameServerDrv_AddRes;

typedef struct {
    List_Elem   elem;
    UInt        cmd;
    union {
        NameServerDrv_CreateRes create;
        NameServerDrv_AddRes    add;
    } args;
} NameServerDrv_Res;


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int NameServerDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int NameServerDrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long NameServerDrv_ioctl (struct file *  filp,
                                 unsigned int   cmd,
                                 unsigned long  args);

static Void NameServerDrv_setup(Void);
static Void NameServerDrv_destroy(Void);
static Void NameServerDrv_releaseResources(Osal_Pid pid);
static Bool NameServerDrv_resCmpFxn(Void *ptrA, Void *ptrB);
static Int  NameServerDrv_cmd_delete(NameServer_Handle *handlePtr);

#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init NameServerDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit NameServerDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

NameServerDrv_ModuleObject NameServerDrv_state = {
    .resTrack = NULL
};

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations NameServerDrv_driverOps = {
    open:    NameServerDrv_open,
    release: NameServerDrv_close,
    unlocked_ioctl:   NameServerDrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool NameServerDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool NameServerDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool NameServerDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 NameServerDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for NameServer
 */
static Ptr NameServerDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */

/*
 *  ======== NameServerDrv_registerDriver ========
 *
 *  Register the NameServer with OsalDriver
 */
Ptr NameServerDrv_registerDriver(Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace(curTrace, GT_ENTER, "NameServerDrv_registerDriver");

    /* setup the module */
    NameServerDrv_setup();

    osalHandle = OsalDriver_registerDriver("NameServer",
            &NameServerDrv_driverOps, NAMESERVER_DRV_MINOR_NUMBER);

    if (osalHandle == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS,
                "NameServerDrv_registerDriver", NameServer_E_INVALIDARG,
                "OsalDriver_registerDriver failed!");
    }

    GT_1trace(curTrace, GT_LEAVE, "NameServerDrv_registerDriver", osalHandle);

    return((Ptr)osalHandle);
}

/*
 *  ======== NameServerDrv_unregisterDriver ========
 *
 *  Register the NameServer with OsalDriver
 */
Void NameServerDrv_unregisterDriver(Ptr* drvHandle)
{
    GT_1trace(curTrace, GT_ENTER, "NameServerDrv_unregisterDriver", drvHandle);

    OsalDriver_unregisterDriver((OsalDriver_Handle *)drvHandle);

    /* destroy the module */
    NameServerDrv_destroy();

    GT_0trace(curTrace, GT_LEAVE, "NameServerDrv_unregisterDriver");
}

/*!
 *  @brief  Linux specific function to open the driver.
 */
int
NameServerDrv_open (struct inode * inode, struct file * filp)
{
    OsalDriver_Handle handle = container_of (inode->i_cdev,
                                             OsalDriver_Object,
                                             cdev);
    filp->private_data = handle;

    return 0;
}

/*
 *  ======== NameServerDrv_close ========
 *
 *  Linux specific function to close the driver.
 */
int NameServerDrv_close(struct inode *inode, struct file *filp)
{
    pid_t pid;

    /* release resources abandoned by the process */
    pid = pid_nr(filp->f_owner.pid);
    NameServerDrv_releaseResources(pid);

    return(0);
}

/*
 *  ======== NameServerDrv_ioctl ========
 *
 */
static long NameServerDrv_ioctl(struct file *filp, unsigned int cmd,
        unsigned long args)
{
    int                         osStatus = 0;
    Int32                       status = NameServer_S_SUCCESS;
    Int32                       ret;
    NameServerDrv_CmdArgs       cargs;
    Osal_Pid                    pid;

    GT_3trace(curTrace, GT_ENTER, "NameServerDrv_ioctl", filp, cmd, args);

    /* save the process id for resource tracking */
    pid = pid_nr(filp->f_owner.pid);

    /* copy the full args from user space */
    ret = copy_from_user(&cargs, (Ptr)args, sizeof(NameServerDrv_CmdArgs));
    GT_assert(curTrace, (ret == 0));

    switch (cmd) {
        case CMD_NAMESERVER_ADD: {
            NameServerDrv_Res * res = NULL;
            Ptr                 buf;

            /* allocate resource tracker object */
            res = Memory_alloc(NULL, sizeof(NameServerDrv_Res), 0, NULL);

            if (res == NULL) {
                status = NameServer_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "NameServerDrv_ioctl", status, "out of memory");
            }

            /* allocate memory for the name */
            if (status == NameServer_S_SUCCESS) {
                res->args.add.len = cargs.args.add.nameLen;
                res->args.add.name = Memory_alloc(NULL,
                        cargs.args.add.nameLen, 0, NULL);

                if (res->args.add.name == NULL) {
                    status = NameServer_E_MEMORY;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status, "out of memory");
                }
            }

            /* copy the name from user memory */
            if (status == NameServer_S_SUCCESS) {
                status = copy_from_user(res->args.add.name,
                        cargs.args.add.name, cargs.args.add.nameLen);

                if (status != 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "copy_from_user failed");
                    status = NameServer_E_OSFAILURE;
                    osStatus = -EFAULT;
                }
            }

            /* allocate memory for the buf */
            if (status == NameServer_S_SUCCESS) {
                buf = Memory_alloc(NULL, cargs.args.add.len, 0, NULL);

                if (buf == NULL) {
                    status = NameServer_E_MEMORY;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status, "out of memory");
                }
            }

            /* copy the value from user buf */
            if (status == NameServer_S_SUCCESS) {
                status = copy_from_user(buf, cargs.args.add.buf,
                        cargs.args.add.len);

                if (status != 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "copy_from_user failed");
                    status = NameServer_E_OSFAILURE;
                    osStatus = -EFAULT;
                }
            }

            /* invoke the module api */
            if (status == NameServer_S_SUCCESS) {
                cargs.args.add.entry = NameServer_add(cargs.args.add.handle,
                        res->args.add.name, buf, cargs.args.add.len);

                if (cargs.args.addUInt32.entry == NULL) {
                    status = NameServer_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "NameServer_addUInt32 failed");
                }
            }

            /* track the resource by process id */
            if (status == NameServer_S_SUCCESS) {
                Int rstat;

                res->cmd = CMD_NAMESERVER_ADD; /* use this command for both */
                res->args.add.handle = cargs.args.add.handle;
                res->args.add.entry = cargs.args.add.entry;

                rstat = ResTrack_push(NameServerDrv_state.resTrack, pid,
                        (List_Elem *)res);

                GT_assert(curTrace, (rstat >= 0));
            }

            /* don't need the buf memory anymore */
            if (buf != NULL) {
                Memory_free(NULL, buf, cargs.args.add.len);
            }

            /* failure cleanup */
            if (status < 0) {
                if ((res != NULL) && (res->args.add.name != NULL)) {
                    Memory_free(NULL, res->args.add.name,
                            cargs.args.add.nameLen);
                }
                if (res != NULL) {
                    Memory_free(NULL, res, sizeof(NameServerDrv_Res));
                }
            }
        }
        break;

        case CMD_NAMESERVER_ADDUINT32: {
            NameServerDrv_Res * res = NULL;

            /* allocate resource tracker object */
            res = Memory_alloc(NULL, sizeof(NameServerDrv_Res), 0, NULL);

            if (res == NULL) {
                status = NameServer_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "NameServerDrv_ioctl", status, "out of memory");
            }

            /* allocate memory for the name */
            if (status == NameServer_S_SUCCESS) {
                res->args.add.len = cargs.args.addUInt32.nameLen;
                res->args.add.name = Memory_alloc(NULL,
                        cargs.args.addUInt32.nameLen, 0, NULL);

                if (res->args.add.name == NULL) {
                    status = NameServer_E_MEMORY;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status, "out of memory");
                }
            }

            /* copy the name from user memory */
            if (status == NameServer_S_SUCCESS) {
                status = copy_from_user(res->args.add.name,
                        cargs.args.addUInt32.name,
                        cargs.args.addUInt32.nameLen);

                if (status != 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "copy_from_user failed");
                    status = NameServer_E_OSFAILURE;
                    osStatus = -EFAULT;
                }
            }

            /* invoke the module api */
            if (status == NameServer_S_SUCCESS) {
                cargs.args.addUInt32.entry = NameServer_addUInt32(
                        cargs.args.addUInt32.handle, res->args.add.name,
                        cargs.args.addUInt32.value);

                if (cargs.args.addUInt32.entry == NULL) {
                    status = NameServer_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "NameServer_addUInt32 failed");
                }
            }

            /* track the resource by process id */
            if (status == NameServer_S_SUCCESS) {
                Int rstat;

                res->cmd = CMD_NAMESERVER_ADD; /* use this command for both */
                res->args.add.handle = cargs.args.addUInt32.handle;
                res->args.add.entry = cargs.args.addUInt32.entry;

                rstat = ResTrack_push(NameServerDrv_state.resTrack, pid,
                        (List_Elem *)res);

                GT_assert(curTrace, (rstat >= 0));
            }

            /* failure cleanup */
            if (status < 0) {
                if ((res != NULL) && (res->args.add.name != NULL)) {
                    Memory_free(NULL, res->args.add.name,
                            cargs.args.addUInt32.nameLen);
                }
                if (res != NULL) {
                    Memory_free(NULL, res, sizeof(NameServerDrv_Res));
                }
            }
        }
        break;

        case CMD_NAMESERVER_GET:
        {
            String   name;
            Ptr      value;
            UInt16 * procId = NULL;

            /* Allocate memory for the name */
            name = Memory_alloc (NULL, cargs.args.get.nameLen, 0, NULL);
            GT_assert (curTrace, (name != NULL));
            /* Copy the name */
            ret = copy_from_user (name,
                                  cargs.args.get.name,
                                  cargs.args.get.nameLen);
            GT_assert (curTrace, (ret == 0));

            /* Allocate memory for the buf */
            value = Memory_alloc (NULL, cargs.args.get.len, 0, NULL);
            GT_assert (curTrace, (value != NULL));

            /* Allocate memory for the procId */
            if (cargs.args.get.procLen > 0) {
                procId = Memory_alloc (NULL,
                                       cargs.args.get.procLen * sizeof(UInt16),
                                       0,
                                       NULL);
                GT_assert (curTrace, (procId != NULL));
            }
            /* Copy the procIds */
            ret = copy_from_user (procId,
                                  cargs.args.get.procId,
                                  cargs.args.get.procLen);
            GT_assert (curTrace, (ret == 0));

            status = NameServer_get (cargs.args.get.handle,
                                     name,
                                     value,
                                     &cargs.args.get.len,
                                     procId);
            /* Do not assert. This can return NameServer_E_NOTFOUND
             * as a valid runtime failure.
             */

            /* Copy the value */
            ret = copy_to_user (cargs.args.get.value,
                                value,
                                cargs.args.get.len);
            GT_assert (curTrace, (ret == 0));

            /* free the allocated memory */
            Memory_free (NULL, name, cargs.args.get.nameLen);
            Memory_free (NULL, value, cargs.args.get.len);
            if (procId != NULL) {
                Memory_free (NULL,
                             procId,
                             cargs.args.get.procLen *sizeof(UInt16));
            }

        }
        break;

        case CMD_NAMESERVER_GETLOCAL:
        {
            String   name;
            Ptr      value;

            /* Allocate memory for the name */
            name = Memory_alloc (NULL, cargs.args.getLocal.nameLen, 0, NULL);
            GT_assert (curTrace, (name != NULL));
            /* Copy the name */
            ret = copy_from_user (name,
                                  cargs.args.getLocal.name,
                                  cargs.args.getLocal.nameLen);
            GT_assert (curTrace, (ret == 0));

            /* Allocate memory for the buf */
            value = Memory_alloc (NULL, cargs.args.getLocal.len, 0, NULL);
            GT_assert (curTrace, (value != NULL));

            status = NameServer_getLocal (cargs.args.getLocal.handle,
                                          name,
                                          value,
                                          &cargs.args.getLocal.len);
            GT_assert (curTrace, (status >= 0));

            /* Copy the value */
            ret = copy_to_user (cargs.args.getLocal.value,
                                value,
                                cargs.args.getLocal.len);
            GT_assert (curTrace, (ret == 0));

            /* free the allocated memory */
            Memory_free (NULL, name, cargs.args.getLocal.nameLen);
            Memory_free (NULL, value,  cargs.args.getLocal.len);
        }
        break;

        case CMD_NAMESERVER_MATCH:
        {
            String name;

            /* Allocate memory for the name */
            name = Memory_alloc (NULL, cargs.args.match.nameLen, 0, NULL);
            GT_assert (curTrace, (name != NULL));
            /* Copy the name */
            ret = copy_from_user (name,
                                  cargs.args.match.name,
                                  cargs.args.match.nameLen);
            GT_assert (curTrace, (ret == 0));

            cargs.args.match.count = NameServer_match (
                                                   cargs.args.match.handle,
                                                   name,
                                                   &cargs.args.match.value);
            GT_assert (curTrace, (cargs.args.match.count >= 0));

            /* free the allocated memory */
            Memory_free (NULL, name, cargs.args.match.nameLen);
        }
        break;

        case CMD_NAMESERVER_REMOVE: {
            NameServerDrv_Res   res;
            List_Elem *         elem;

            /* save for resource untracking */
            res.cmd = CMD_NAMESERVER_ADD;
            res.args.add.entry = NULL;

            /* allocate memory for the name */
            res.args.add.len = cargs.args.remove.nameLen;
            res.args.add.name = Memory_alloc(NULL,
                    cargs.args.remove.nameLen, 0, NULL);

            if (res.args.add.name == NULL) {
                status = NameServer_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "NameServerDrv_ioctl", status, "out of memory");
            }

            /* copy the name from user memory */
            if (status == NameServer_S_SUCCESS) {
                status = copy_from_user(res.args.add.name,
                        cargs.args.remove.name, cargs.args.remove.nameLen);

                if (status != 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "copy_from_user failed");
                    status = NameServer_E_OSFAILURE;
                    osStatus = -EFAULT;
                }
            }

            /* invoke the module api */
            if (status == NameServer_S_SUCCESS) {
                status = NameServer_remove(cargs.args.remove.handle,
                        res.args.add.name);

                if (status < 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "NameServer_remove failed");
                }
            }

            /* untrack the resource */
            if (status == NameServer_S_SUCCESS) {
                NameServerDrv_Res * resPtr;

                ResTrack_remove(NameServerDrv_state.resTrack, pid,
                        (List_Elem *)(&res), NameServerDrv_resCmpFxn, &elem);

                GT_assert(curTrace, (elem != NULL));

                resPtr = (NameServerDrv_Res *)elem;
                Memory_free(NULL, resPtr->args.add.name, resPtr->args.add.len);
                Memory_free(NULL, elem, sizeof(NameServerDrv_Res));
            }

            /* don't need the name memory anymore */
            if (res.args.add.name != NULL) {
                Memory_free(NULL, res.args.add.name,
                        cargs.args.remove.nameLen);
            }
        }
        break;

        case CMD_NAMESERVER_REMOVEENTRY: {
            NameServerDrv_Res   res;
            List_Elem *         elem;

            /* save for resource untracking */
            res.cmd = CMD_NAMESERVER_ADD;
            res.args.add.name = NULL;
            res.args.add.entry = cargs.args.removeEntry.entry;

            /* invoke the module api */
            status = NameServer_removeEntry(cargs.args.removeEntry.handle,
                    cargs.args.removeEntry.entry);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_ioctl",
                        status, "NameServer_remove failed");
            }

            /* untrack the resource */
            if (status == NameServer_S_SUCCESS) {
                NameServerDrv_Res * resPtr;

                ResTrack_remove(NameServerDrv_state.resTrack, pid,
                        (List_Elem *)(&res), NameServerDrv_resCmpFxn, &elem);

                GT_assert(curTrace, (elem != NULL));

                resPtr = (NameServerDrv_Res *)elem;
                Memory_free(NULL, resPtr->args.add.name, resPtr->args.add.len);
                Memory_free(NULL, elem, sizeof(NameServerDrv_Res));
            }
        }
        break;

        case CMD_NAMESERVER_PARAMS_INIT:
        {
            NameServer_Params params;

            NameServer_Params_init (&params);
            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (NameServer_Params));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_NAMESERVER_CREATE: {
            NameServer_Params   params;
            String              name = NULL;
            NameServerDrv_Res * res = NULL;

            /* allocate memory for the name */
            name = Memory_alloc(NULL, cargs.args.create.nameLen, 0, NULL);

            if (name == NULL) {
                status = NameServer_E_MEMORY;
                GT_setFailureReason(curTrace, GT_4CLASS,
                        "NameServerDrv_ioctl", status, "out of memory");
            }

            /* copy the name from user memory */
            if (status == NameServer_S_SUCCESS) {
                status = copy_from_user(name, cargs.args.create.name,
                        cargs.args.create.nameLen);

                if (status != 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "copy_from_user failed");
                    status = NameServer_E_OSFAILURE;
                    osStatus = -EFAULT;
                }
            }

            /* copy the params from user memory */
            if (status == NameServer_S_SUCCESS) {
                status = copy_from_user(&params, cargs.args.create.params,
                        sizeof(NameServer_Params));

                if (status != 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "copy_from_user failed");
                    status = NameServer_E_OSFAILURE;
                    osStatus = -EFAULT;
                }
            }

            /* allocate resource tracker object */
            if (status == NameServer_S_SUCCESS) {
                res = Memory_alloc(NULL, sizeof(NameServerDrv_Res), 0, NULL);

                if (res == NULL) {
                    status = NameServer_E_MEMORY;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status, "out of memory");
                }
            }

            /* invoke the module api */
            if (status == NameServer_S_SUCCESS) {
                cargs.args.create.handle = NameServer_create(name, &params);

                if (cargs.args.create.handle == NULL) {
                    status = NameServer_E_FAIL;
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "NameServer_create failed");
                }
            }

            /* track the resource by process id */
            if (status == NameServer_S_SUCCESS) {
                Int rstat;

                res->cmd = cmd;
                res->args.create.handle = cargs.args.create.handle;

                rstat = ResTrack_push(NameServerDrv_state.resTrack, pid,
                        (List_Elem *)res);

                GT_assert(curTrace, (rstat >= 0));
            }

            /* don't need name memory anymore */
            if (name != NULL) {
                Memory_free(NULL, name, cargs.args.create.nameLen);
            }

            /* failure cleanup */
            if (status < 0) {
                if (res != NULL) {
                    Memory_free(NULL, res, sizeof(NameServerDrv_Res));
                }
            }
        }
        break;

        case CMD_NAMESERVER_DELETE: {
            NameServerDrv_Res   res;
            List_Elem *         elem;

            /* save for resource untracking, handle set to null by delete */
            res.cmd = CMD_NAMESERVER_CREATE;
            res.args.create.handle = cargs.args.delete.handle;

            /* common code for delete command */
            status = NameServerDrv_cmd_delete(&cargs.args.delete.handle);

            /* untrack the resource */
            if (status == NameServer_S_SUCCESS) {
                ResTrack_remove(NameServerDrv_state.resTrack, pid,
                        (List_Elem *)(&res), NameServerDrv_resCmpFxn, &elem);

                GT_assert(curTrace, (elem != NULL));
                Memory_free(NULL, elem, sizeof(NameServerDrv_Res));
            }
        }
        break;

        case CMD_NAMESERVER_GETHANDLE:
        {
            String   name;

            /* Allocate memory for the name */
            name = Memory_alloc (NULL, cargs.args.getHandle.nameLen, 0, NULL);
            GT_assert (curTrace, (name != NULL));
            /* Copy the name */
            ret = copy_from_user (name,
                                  cargs.args.getHandle.name,
                                  cargs.args.getHandle.nameLen);
            GT_assert (curTrace, (ret == 0));

            cargs.args.getHandle.handle = NameServer_getHandle (name);

            /* free the allocated memory */
            Memory_free (NULL, name, cargs.args.getHandle.nameLen);
        }
        break;

        case CMD_NAMESERVER_SETUP: {
            /* register process with resource tracker */
            status = ResTrack_register(NameServerDrv_state.resTrack, pid);

            if (status < 0) {
                GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_ioctl",
                        status, "resource tracker register failed");
                status = NameServer_E_FAIL;
                pid = 0;
            }

            /* setup the module */
            if (status == NameServer_S_SUCCESS) {
                status = NameServer_setup();

                if (status < 0) {
                    GT_setFailureReason(curTrace, GT_4CLASS,
                            "NameServerDrv_ioctl", status,
                            "kernel-side MessageQ_setup failed");
                }
            }

            /* failure case */
            if (status < 0) {
                if (pid != 0) {
                    ResTrack_unregister(NameServerDrv_state.resTrack, pid);
                }
            }
        }
        break;

        case CMD_NAMESERVER_DESTROY: {
            /* unregister process from resource tracker */
            status = ResTrack_unregister(NameServerDrv_state.resTrack, pid);
            GT_assert(curTrace, (status >= 0));

            /* finalize the module */
            status = NameServer_destroy();
            GT_assert(curTrace, (status >= 0));
        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = NameServer_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "NameServerDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs.apiStatus = status;

    /* copy the full args back to user space */
    ret = copy_to_user((Ptr)args, &cargs, sizeof(NameServerDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "NameServerDrv_ioctl", osStatus);

    /*! @retval 0 Operation successfully completed. */
    return osStatus;
}


/** ============================================================================
 *  Internal functions
 *  ============================================================================
 */

/*
 *  ======== NameServerDrv_setup ========
 */
static Void NameServerDrv_setup(Void)
{
    /* create a resource tracker instance */
    NameServerDrv_state.resTrack = ResTrack_create(NULL);

    if (NameServerDrv_state.resTrack == NULL) {
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_setup", -1,
                "Failed to create resource tracker");
    }
}

/*
 *  ======== NameServerDrv_destroy ========
 */
static Void NameServerDrv_destroy(Void)
{
    /* delete the resource tracker instance */
    ResTrack_delete(&NameServerDrv_state.resTrack);
}

/*
 *  ======== NameServerDrv_releaseResources ========
 */
static Void NameServerDrv_releaseResources(Osal_Pid pid)
{
    Int                 status;
    ResTrack_Handle     resTrack;
    Bool                unreg;
    List_Elem *         elem;
    NameServerDrv_Res * res;

    status = NameServer_S_SUCCESS;
    resTrack = NameServerDrv_state.resTrack;
    unreg = FALSE;

    /* reclaim abandoned resources */
    do {
        status = ResTrack_pop(resTrack, pid, &elem);

        /* nothing to do if process no longer registered */
        if (status == ResTrack_E_PID) {
            break;
        }

        unreg = TRUE; /* need to unregister process */

        /* done if process has no more tracked resources */
        if (elem == NULL) {
            break;
        }

        /* decode resource */
        res = (NameServerDrv_Res*)elem;

        switch (res->cmd) {
            case CMD_NAMESERVER_CREATE:
                NameServerDrv_cmd_delete((NameServer_Handle *)
                        &(res->args.create.handle));
                break;

            case CMD_NAMESERVER_ADD:
                NameServer_removeEntry(res->args.add.handle,
                        res->args.add.entry);
                break;

            default:
                break;
        }

        Memory_free(NULL, elem, sizeof(NameServerDrv_Res));
    } while (elem != NULL);

    /* unregister the process object */
    if (unreg) {
        ResTrack_unregister(resTrack, pid);
    }
}

/*
 *  ======== NameServerDrv_resCmpFxn ========
 */
static Bool NameServerDrv_resCmpFxn(Void *ptrA, Void *ptrB)
{
    NameServerDrv_Res * resA;
    NameServerDrv_Res * resB;
    Bool                found;

    found = FALSE;
    resA = (NameServerDrv_Res *)ptrA;
    resB = (NameServerDrv_Res *)ptrB;

    if (resA->cmd != resB->cmd) {
        goto leave;
    }

    switch (resA->cmd) {
        case CMD_NAMESERVER_CREATE: {
            NameServerDrv_CreateRes *   argsA = &resA->args.create;
            NameServerDrv_CreateRes *   argsB = &resB->args.create;

            if (argsA->handle == argsB->handle) {
                found = TRUE;
            }
        }
        break;

        case CMD_NAMESERVER_ADD: {
            NameServerDrv_AddRes *      argsA = &resA->args.add;
            NameServerDrv_AddRes *      argsB = &resB->args.add;

            /* either the name will match, or... */
            if ((argsA->name != NULL) && (argsB->name != NULL) &&
                    (strcmp(argsA->name, argsB->name) == 0)) {
                found = TRUE;
            }

            /* the entry will match, but not both */
            else if (argsA->entry == argsB->entry) {
                found = TRUE;
            }
        }
        break;

        default:
            GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_resCmpFxn",
                    -1, "unknown command");
    }

leave:
    return(found);
}

/*
 *  ======== NameServerDrv_cmd_delete ========
 */
static Int NameServerDrv_cmd_delete(NameServer_Handle *handlePtr)
{
    Int status;

    /* invoke the module api */
    status = NameServer_delete(handlePtr);

    if (status < 0) {
        /* use NameServerDrv_ioctl in macro */
        GT_setFailureReason(curTrace, GT_4CLASS, "NameServerDrv_ioctl",
                status, "NameServer_delete failed");
    }

    return(status);
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
int __init NameServerDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("NameServer sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("NameServerDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        NameServerDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if (    (NameServerDrv_enableTrace != 0)
            &&  (NameServerDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (NameServerDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (NameServerDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        NameServerDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (NameServerDrv_enableTraceEnter != 0)
            &&  (NameServerDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (NameServerDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        NameServerDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                          NULL,
                                                          16);
        if (    (NameServerDrv_enableTraceFailure != 0)
            &&  (NameServerDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (NameServerDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        NameServerDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (NameServerDrv_enableTraceFailure != 1)
            &&  (NameServerDrv_enableTraceFailure != 2)
            &&  (NameServerDrv_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            NameServerDrv_traceClass =
                         NameServerDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= NameServerDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    NameServerDrv_osalDrvHandle = NameServerDrv_registerDriver ();
    if (NameServerDrv_osalDrvHandle == NULL) {
        /*! @retval NameServer_E_OSFAILURE Failed to register NameServer
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "NameServerDrv_initializeModule",
                             NameServer_E_OSFAILURE,
                             "Failed to register NameServer driver with OS!");
    }

    Osal_printf ("NameServerDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit NameServerDrv_finalizeModule (void)
{
    Osal_printf ("Entered NameServerDrv_finalizeModule\n");

    NameServerDrv_unregisterDriver (&(NameServerDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving NameServerDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (NameServerDrv_initializeModule);
module_exit (NameServerDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
