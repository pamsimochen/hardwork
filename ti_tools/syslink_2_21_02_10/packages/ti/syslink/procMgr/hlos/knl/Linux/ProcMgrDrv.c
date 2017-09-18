/*
 *  @file   ProcMgrDrv.c
 *
 *  @brief      OS-specific implementation of ProcMgr driver for Linux
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
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>

/* Linux specific header files */
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <asm/uaccess.h>

/* Module headers */
#include <ti/syslink/ProcMgr.h>
#include <ti/syslink/inc/_ProcMgr.h>
#include <ti/syslink/inc/_MultiProc.h>
#include <ti/syslink/inc/knl/ProcMgrDrv.h>
#include <ti/syslink/inc/ProcMgrDrvDefs.h>


/*  ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* ProcMgr functions */
EXPORT_SYMBOL(ProcMgr_getConfig);
EXPORT_SYMBOL(ProcMgr_setup);
EXPORT_SYMBOL(ProcMgr_destroy);
EXPORT_SYMBOL(ProcMgr_Params_init);
EXPORT_SYMBOL(ProcMgr_create);
EXPORT_SYMBOL(ProcMgr_delete);
EXPORT_SYMBOL(ProcMgr_open);
EXPORT_SYMBOL(ProcMgr_close);
EXPORT_SYMBOL(ProcMgr_getAttachParams);
EXPORT_SYMBOL(ProcMgr_attach);
EXPORT_SYMBOL(ProcMgr_detach);
EXPORT_SYMBOL(ProcMgr_load);
EXPORT_SYMBOL(ProcMgr_unload);
EXPORT_SYMBOL(ProcMgr_getStartParams);
EXPORT_SYMBOL(ProcMgr_start);
EXPORT_SYMBOL(ProcMgr_stop);
EXPORT_SYMBOL(ProcMgr_getState);
EXPORT_SYMBOL(ProcMgr_read);
EXPORT_SYMBOL(ProcMgr_write);
EXPORT_SYMBOL(ProcMgr_control);
EXPORT_SYMBOL(ProcMgr_translateAddr);
EXPORT_SYMBOL(ProcMgr_getSymbolAddress);
EXPORT_SYMBOL(ProcMgr_map);
EXPORT_SYMBOL(ProcMgr_unmap);
EXPORT_SYMBOL(ProcMgr_getProcInfo);
EXPORT_SYMBOL(ProcMgr_getSectionInfo);
EXPORT_SYMBOL(ProcMgr_getSectionData);
EXPORT_SYMBOL(ProcMgr_getLoadedFileId);
EXPORT_SYMBOL(_ProcMgr_configSysMap);

/* ProcMgrDrv functions */
EXPORT_SYMBOL(ProcMgrDrv_registerDriver);
EXPORT_SYMBOL(ProcMgrDrv_unregisterDriver);


/*  ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for ProcMgr.
 *  TBD: Needs to be removed when OsalDriver supports automatic minor number.
 */
#define ProcMgr_DRV_MINOR_NUMBER 0


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/* Linux driver function to open the driver object. */
static int ProcMgrDrv_open (struct inode * inode, struct file * filp);

/* Linux driver function to close the driver object. */
static int ProcMgrDrv_release (struct inode * inode, struct file * filp);

/* Linux driver function to invoke the APIs through ioctl. */
static long ProcMgrDrv_ioctl (struct file *     filp,
                              unsigned int      cmd,
                              unsigned long     args);

/* Linux driver function to map memory regions to user space. */
static int ProcMgrDrv_mmap (struct file * filp, struct vm_area_struct * vma);

#if defined (SYSLINK_MULTIPLE_MODULES)
/* Module initialization function for Linux driver. */
static int __init ProcMgrDrv_initializeModule (void);

/* Module finalization  function for Linux driver. */
static void  __exit ProcMgrDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  File operations table for ProcMgr.
 */
static struct file_operations ProcMgr_driverOps = {
    open:    ProcMgrDrv_open,
    release: ProcMgrDrv_release,
    unlocked_ioctl:   ProcMgrDrv_ioctl,
    mmap:    ProcMgrDrv_mmap,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool ProcMgr_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool ProcMgr_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool ProcMgr_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 ProcMgr_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for ProcMgr
 */
static Ptr ProcMgrDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the ProcMgr with OsalDriver
 *
 *  @sa     ProcMgrDrv_unregisterDriver
 */
Ptr
ProcMgrDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "ProcMgrDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("ProcMgr",
                                            &ProcMgr_driverOps,
                                            ProcMgr_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ProcMgrDrv_registerDriver",
                             ProcMgr_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ProcMgrDrv_registerDriver", osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Unregister the ProcMgr with OsalDriver
 *
 *  @param  drvHandle   Pointer to the driver handle to be unregistered.
 *
 *  @sa     ProcMgrDrv_registerDriver
 */
Void
ProcMgrDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "ProcMgrDrv_unregisterDriver", drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "ProcMgrDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 *
 *  @param  inode   Inode pointer.
 *  @param  filp    File structure pointer.
 *
 *  @sa     ProcMgrDrv_release
 */
static
int
ProcMgrDrv_open (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux driver function to close the driver object.
 *
 *  @param  inode   Inode pointer.
 *  @param  filp    File structure pointer.
 *
 *  @sa     ProcMgrDrv_open
 */
static
int
ProcMgrDrv_release (struct inode * inode, struct file * filp)

{
    return 0;
}


/*!
 *  @brief  Linux driver function to invoke the APIs through ioctl.
 *
 *  @param  inode   Inode pointer.
 *  @param  filp    File structure pointer.
 *  @param  cmd     IOCTL command id.
 *  @param  args    Arguments for the command.
 *
 *  @sa     ProcMgrDrv_open
 */
static
long
ProcMgrDrv_ioctl (struct file *     filp,
                  unsigned int      cmd,
                  unsigned long     args)
{
    Int                 status      = ProcMgr_S_SUCCESS;
    int                 osStatus    = 0;
    int                 retVal      = 0;
    ProcMgr_CmdArgs *   cmdArgs     = (ProcMgr_CmdArgs *) args;
    ProcMgr_CmdArgs     commonArgs;

    GT_3trace (curTrace, GT_ENTER, "ProcMgrDrv_ioctl", filp, cmd, args);

    switch (cmd) {
        case CMD_PROCMGR_GETCONFIG:
        {
            ProcMgr_CmdArgsGetConfig *  srcArgs =
                                              (ProcMgr_CmdArgsGetConfig *) args;
            ProcMgr_Config              cfg;

            /* copy_from_user is not needed for ProcMgr_getConfig, since the
             * user's config is not used.
             */
            ProcMgr_getConfig (&cfg);

            if (osStatus == 0) {
                retVal = copy_to_user ((Ptr) (srcArgs->cfg),
                                       (const Ptr) &cfg,
                                       sizeof (ProcMgr_Config));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_SETUP:
        {
            ProcMgr_CmdArgsSetup * srcArgs = (ProcMgr_CmdArgsSetup *) args;
            ProcMgr_Config        cfg;

            /* Shallow copy of config struct, setup doesn't actually use it. */
            retVal = copy_from_user ((Ptr) &cfg,
                                     (const Ptr) (srcArgs->cfg),
                                     sizeof (ProcMgr_Config));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_setup (&cfg);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_setup failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
         }
        break;

        case CMD_PROCMGR_CONFIGSYSMEMMAP:
        {
            ProcMgr_CmdArgsSetup *  srcArgs = (ProcMgr_CmdArgsSetup *)args;
            ProcMgr_Config          cfg;

            retVal = copy_from_user((Ptr)&cfg, (const Ptr)(srcArgs->cfg),
                sizeof(ProcMgr_Config));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace(curTrace, GT_1CLASS,
                    "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                    "        status [%d]", retVal);
                osStatus = -EFAULT;
            }
            else {
                /* TBD: Need to free this pointer while destroying */
                cfg.sysMemMap = Memory_calloc(NULL,
                    (sizeof(SysLink_MemoryMap)), 0u, NULL);

                GT_assert(curTrace, (cfg.sysMemMap != NULL));

                retVal = copy_from_user(cfg.sysMemMap,
                    (srcArgs->cfg->sysMemMap), (sizeof (SysLink_MemoryMap)));

                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace(curTrace, GT_1CLASS,
                        "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                        "        status [%d]", retVal);
                    osStatus = -EFAULT;
                }
                else {
                    if (cfg.sysMemMap->numBlocks == 0) {
                        cfg.sysMemMap->memBlocks = NULL;
                    }
                    else {
                        /* TBD: Need to free this pointer while destroying */
                        cfg.sysMemMap->memBlocks = Memory_calloc(NULL,
                            (sizeof(SysLink_MemEntry_Block) *
                            cfg.sysMemMap->numBlocks), 0u, NULL);

                        GT_assert(curTrace, (cfg.sysMemMap->memBlocks != NULL));

                        retVal = copy_from_user(cfg.sysMemMap->memBlocks,
                            (srcArgs->cfg->sysMemMap->memBlocks),
                            (sizeof(SysLink_MemEntry_Block) *
                            cfg.sysMemMap->numBlocks));

                        /* This check is needed at run-time also since it depends on
                         * run environment. It must not be optimized out.
                         */
                        if (retVal != 0) {
                            GT_1trace (curTrace, GT_1CLASS,
                                "    ProcMgrDrv_ioctl: copy_from_user call "
                                "    failed\n  status [%d]", retVal);
                            osStatus = -EFAULT;
                        }
                    }

                    if (retVal == 0) {
                        _ProcMgr_configSysMap(&cfg);

                        if (cfg.sysMemMap->memBlocks != NULL) {
                            Memory_free(NULL, cfg.sysMemMap->memBlocks,
                                (sizeof(SysLink_MemEntry_Block) *
                                (cfg.sysMemMap)->numBlocks));
                        }
                        Memory_free(NULL, cfg.sysMemMap,
                            sizeof(SysLink_MemoryMap));
                    }
                }
            }

            /* copy from user the SysLinkCfg_params string */
            if (retVal == 0) {

                if (cfg.paramsLen > 0) {
                    /* This memory is freed in ProcMgr when receiving next
                     * param string or in ProcMgr_destroy().
                     */
                    cfg.params = (String)Memory_calloc(NULL,
                        (cfg.paramsLen * sizeof(Char)), 0u, NULL);

                    GT_assert(curTrace, (cfg.params != NULL));

                    retVal = copy_from_user(cfg.params, srcArgs->cfg->params,
                        cfg.paramsLen);

                    GT_assert(curTrace,
                        (cfg.paramsLen == (strlen(cfg.params) + 1)));

                    if (retVal != 0) {
                        GT_1trace (curTrace, GT_1CLASS,
                                   "    ProcMgrDrv_ioctl: copy_from_user call "
                                   "    failed\n  status [%d]", retVal);
                        osStatus = -EFAULT;
                    }
                    else {
                        _ProcMgr_saveParams(cfg.params, cfg.paramsLen);
                    }
                }
                else {
                    /* make call to free previous params string */
                    _ProcMgr_saveParams(cfg.params, cfg.paramsLen);
                }
            }
        }
        break;

        case CMD_PROCMGR_DESTROY:
        {
            /* copy_from_user is not needed for ProcMgr_getConfig, since the
             * user's config is not used.
             */
            status = ProcMgr_destroy ();
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ProcMgrDrv_ioctl",
                                     status,
                                     "Kernel-side ProcMgr_destroy failed");
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

            /* copy_to_user is not needed for ProcMgr_destroy, since the config
             * is not to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_PARAMS_INIT:
        {
            ProcMgr_CmdArgsParamsInit   srcArgs;
            ProcMgr_Params              params;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsParamsInit));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                ProcMgr_Params_init (srcArgs.handle, &params);
            }

            if (osStatus == 0) {
                /* Copy only the params to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.params),
                                       (const Ptr) &params,
                                       sizeof (ProcMgr_Params));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_CREATE:
        {
            ProcMgr_CmdArgsCreate srcArgs;
            ProcMgr_Handle handle;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsCreate));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                handle = ProcMgr_create (srcArgs.procId, &(srcArgs.params));

                /* This check is needed at run-time also to propagate the
                 * status to user-side. This must not be optimized out.
                 */
                if (handle == NULL) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    status = ProcMgr_E_FAIL;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "ProcMgr_create failed");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
                else {
                    srcArgs.maxMemoryRegions =
                        ProcMgr_getMaxMemoryRegions(handle);
                    srcArgs.handle = handle;
                }
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsCreate));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_DELETE:
        {
            ProcMgr_CmdArgsDelete srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsDelete));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_delete (&(srcArgs.handle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_delete failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* copy_to_user is not needed for ProcMgr_delete, since nothing
             * is to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_OPEN:
        {
            ProcMgr_CmdArgsOpen srcArgs;
            ProcMgr_Handle handle;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsOpen));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_open (&handle, srcArgs.procId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_open failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                if (status >= 0 && handle != NULL) {
                    srcArgs.maxMemoryRegions =
                        ProcMgr_getMaxMemoryRegions(handle);
                    srcArgs.handle = handle;
                }
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsOpen));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_CLOSE:
        {
            ProcMgr_CmdArgsClose srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsClose));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_close (&(srcArgs.handle));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_close failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* copy_to_user is not needed for ProcMgr_close, since nothing
             * is to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_GETATTACHPARAMS:
        {
            ProcMgr_CmdArgsGetAttachParams   srcArgs;
            ProcMgr_AttachParams             params;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetAttachParams));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                ProcMgr_getAttachParams (srcArgs.handle, &params);
            }

            if (osStatus == 0) {
                /* Copy only the params to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.params),
                                       (const Ptr) &params,
                                       sizeof (ProcMgr_AttachParams));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_ATTACH:
        {
            ProcMgr_CmdArgsAttach srcArgs;
            ProcMgr_AttachParams  params;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsAttach));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                /* Copy params from user-side. */
                retVal = copy_from_user ((Ptr) &params,
                                         (const Ptr) (srcArgs.params),
                                         sizeof (ProcMgr_AttachParams));

                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                            GT_1CLASS,
                            "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                            "        status [%d]",
                            retVal);
                    osStatus = -EFAULT;
                }
                else {
                    status = ProcMgr_attach (srcArgs.handle, &params);
                    /* This check is needed at run-time also to propagate the
                     * status to user-side. This must not be optimized out.
                     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* This does not impact return status of this function,
                         * so retVal comment is not used.
                         */
                        GT_setFailureReason (curTrace,
                                          GT_4CLASS,
                                          "ProcMgrDrv_ioctl",
                                           status,
                                           "Kernel-side ProcMgr_attach failed");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsAttach));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_DETACH:
        {
            ProcMgr_CmdArgsDetach srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsDetach));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_detach (srcArgs.handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_detach failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* copy_to_user is not needed for ProcMgr_detach, since nothing
             * is to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_LOAD:
        {
            ProcMgr_CmdArgsLoad srcArgs;
            String              imagePath;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsLoad));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                imagePath = Memory_alloc (NULL, srcArgs.imagePathLen, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (imagePath == NULL) {
                    status = ProcMgr_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Memory_alloc failed");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    memset (imagePath, 0, srcArgs.imagePathLen);
                    retVal = copy_from_user ((Ptr) imagePath,
                                             (const Ptr) srcArgs.imagePath,
                                             srcArgs.imagePathLen);

                    /* This check is needed at run-time also since it depends on
                     * run environment. It must not be optimized out.
                     */
                    if (retVal != 0) {
                        GT_1trace (curTrace,
                                   GT_1CLASS,
                                   "    ProcMgrDrv_ioctl: copy_from_user call "
                                   "failed\n"
                           "        status [%d]",
                           retVal);
                        osStatus = -EFAULT;
                    }
                    else {
                        /* TBD: copy_from_user for imagePath, argv, params */
                        status = ProcMgr_load (srcArgs.handle,
                                imagePath,
                                srcArgs.argc,
                                srcArgs.argv,
                                srcArgs.params,
                                &(srcArgs.fileId));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (status < 0) {
                            /* This does not impact return status,
                             * so retVal comment is not used.
                             */
                            GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "ProcMgrDrv_ioctl",
                                    status,
                                    "Kernel-side ProcMgr_load failed");
                        }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }

                    Memory_free (NULL, imagePath, srcArgs.imagePathLen);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsLoad));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_UNLOAD:
        {
            ProcMgr_CmdArgsUnload srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsUnload));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_unload (srcArgs.handle, srcArgs.fileId);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_unload failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* copy_to_user is not needed for ProcMgr_unload, since nothing
             * is to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_GETSTARTPARAMS:
        {
            ProcMgr_CmdArgsGetStartParams   srcArgs;
            ProcMgr_StartParams             params;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetStartParams));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                ProcMgr_getStartParams (srcArgs.handle, &params);
            }

            if (osStatus == 0) {
                /* Copy only the params to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.params),
                                       (const Ptr) &params,
                                       sizeof (ProcMgr_StartParams));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_START:
        {
            ProcMgr_CmdArgsStart  srcArgs;
            ProcMgr_StartParams   params;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsStart));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                /* Copy params from user-side. */
                retVal = copy_from_user ((Ptr) &params,
                                         (const Ptr) (srcArgs.params),
                                         sizeof (ProcMgr_StartParams));

                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                            GT_1CLASS,
                            "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                            "        status [%d]",
                            retVal);
                    osStatus = -EFAULT;
                }
                else {
                    status = ProcMgr_start (srcArgs.handle, &params);
                    /* This check is needed at run-time also to propagate the
                     * status to user-side. This must not be optimized out.
                     */
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* This does not impact return status of this function,
                         * so retVal comment is not used.
                         */
                        GT_setFailureReason (curTrace,
                                          GT_4CLASS,
                                          "ProcMgrDrv_ioctl",
                                           status,
                                           "Kernel-side ProcMgr_start failed");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            /* copy_to_user is not needed for ProcMgr_start, since nothing
             * is to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_STOP:
        {
            ProcMgr_CmdArgsStop srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsStop));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_stop (srcArgs.handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_stop failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* copy_to_user is not needed for ProcMgr_stop, since nothing
             * is to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_GETSTATE:
        {
            ProcMgr_CmdArgsGetState srcArgs;
            ProcMgr_State           procMgrState;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetState));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                procMgrState = ProcMgr_getState (srcArgs.handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_getState failed");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    srcArgs.procMgrState = procMgrState;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsGetState));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_READ:
        {
            ProcMgr_CmdArgsRead srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsRead));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_read (srcArgs.handle,
                                       srcArgs.procAddr,
                                       &(srcArgs.numBytes),
                                       srcArgs.buffer);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_read failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsRead));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_WRITE:
        {
            ProcMgr_CmdArgsWrite srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsWrite));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_write (srcArgs.handle,
                                        srcArgs.procAddr,
                                        &(srcArgs.numBytes),
                                        srcArgs.buffer);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_write failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsWrite));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_CONTROL:
        {
            ProcMgr_CmdArgsControl srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsControl));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_control (srcArgs.handle,
                                          srcArgs.cmd,
                                          srcArgs.arg);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_control failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            /* copy_to_user is not needed for ProcMgr_control, since nothing
             * is to be returned back.
             */
        }
        break;

        case CMD_PROCMGR_TRANSLATEADDR:
        {
            ProcMgr_CmdArgsTranslateAddr srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsTranslateAddr));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_translateAddr (srcArgs.handle,
                                                &(srcArgs.dstAddr),
                                                srcArgs.dstAddrType,
                                                srcArgs.srcAddr,
                                                srcArgs.srcAddrType);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                    GT_4CLASS,
                                    "ProcMgrDrv_ioctl",
                                    status,
                                    "Kernel-side ProcMgr_translateAddr failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsTranslateAddr));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_GETSYMBOLADDRESS:
        {
            ProcMgr_CmdArgsGetSymbolAddress srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetSymbolAddress));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                /* TBD: copy_from_user for symbolName. */
                status = ProcMgr_getSymbolAddress (srcArgs.handle,
                                                   srcArgs.fileId,
                                                   srcArgs.symbolName,
                                                   &(srcArgs.symValue));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrv_ioctl",
                                 status,
                                 "Kernel-side ProcMgr_getSymbolAddress failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                      (const Ptr) &srcArgs,
                                      sizeof (ProcMgr_CmdArgsGetSymbolAddress));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_MAP:
        {
            ProcMgr_CmdArgsMap srcArgs;
            ProcMgr_AddrInfo   addrInfo;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsMap));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                /* Copy the full args from user-side. */
                retVal = copy_from_user ((Ptr) &addrInfo,
                                         (const Ptr) (srcArgs.addrInfo),
                                         sizeof (ProcMgr_AddrInfo));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                status = ProcMgr_map (srcArgs.handle,
                                          srcArgs.mapType,
                                          &addrInfo,
                                          srcArgs.srcAddrType);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Kernel-side ProcMgr_map failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.addrInfo),
                                       (const Ptr) &addrInfo,
                                       sizeof (ProcMgr_AddrInfo));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
                else {
                    /* Copy the full args to user-side */
                    retVal = copy_to_user ((Ptr) (args),
                                           (const Ptr) &srcArgs,
                                           sizeof (ProcMgr_CmdArgsMap));
                    /* This check is needed at run-time also since it depends on
                     * run environment. It must not be optimized out.
                     */
                    if (retVal != 0) {
                        GT_1trace (curTrace,
                                  GT_1CLASS,
                                  "ProcMgrDrv_ioctl: copy_to_user call failed\n"
                                  " status [%d]",
                                  retVal);
                        osStatus = -EFAULT;
                    }
                }
            }
        }
        break;

        case CMD_PROCMGR_UNMAP:
        {
            ProcMgr_CmdArgsUnmap srcArgs;
            ProcMgr_AddrInfo     addrInfo;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsUnmap));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                /* Copy the full args from user-side. */
                retVal = copy_from_user ((Ptr) &addrInfo,
                                         (const Ptr) (srcArgs.addrInfo),
                                         sizeof (ProcMgr_AddrInfo));

                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                               GT_1CLASS,
                               " ProcMgrDrv_ioctl: copy_from_user call failed\n"
                               "     status [%d]",
                               retVal);
                    osStatus = -EFAULT;
                }
                else {
                status = ProcMgr_unmap (srcArgs.handle,
                                        srcArgs.mapType,
                                        &addrInfo,
                                        srcArgs.srcAddrType);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* This does not impact return status of this function,
                         * so retVal comment is not used.
                         */
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "ProcMgrDrv_ioctl",
                                             status,
                                             "Kernel-side ProcMgr_unmap failed");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.addrInfo),
                                       (const Ptr) &addrInfo,
                                       sizeof (ProcMgr_AddrInfo));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
                else {
                    /* Copy the full args to user-side */
                    retVal = copy_to_user ((Ptr) (args),
                                           (const Ptr) &srcArgs,
                                           sizeof (ProcMgr_CmdArgsUnmap));
                    /* This check is needed at run-time also since it depends on
                     * run environment. It must not be optimized out.
                     */
                    if (retVal != 0) {
                        GT_1trace (curTrace,
                                  GT_1CLASS,
                                  "ProcMgrDrv_ioctl: copy_to_user call failed\n"
                                  " status [%d]",
                                  retVal);
                        osStatus = -EFAULT;
                    }
                }
            }
        }
        break;

        case CMD_PROCMGR_GETPROCINFO:
        {
            ProcMgr_Handle             procMgrHandle;
            ProcMgr_CmdArgsGetProcInfo srcArgs;
            ProcMgr_ProcInfo           *procInfo;
            Int                        procInfoSize;
            UInt32                     maxMemoryRegions;

            /* init for later check */
            procInfo = NULL;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetProcInfo));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                procMgrHandle = (ProcMgr_Handle)srcArgs.handle;
                maxMemoryRegions = ProcMgr_getMaxMemoryRegions(procMgrHandle);
                procInfoSize = sizeof(ProcMgr_ProcInfo) +
                               (maxMemoryRegions *
                                sizeof(ProcMgr_MappedMemEntry));
                procInfo = Memory_alloc(NULL, procInfoSize, 0, NULL);

                if (procInfo == NULL) {
                    status = ProcMgr_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "ProcMgrDrv_ioctl",
                                         status,
                                         "Memory_alloc failed");
                    goto getProcInfoRet;
                }

                status = ProcMgr_getProcInfo(procMgrHandle, procInfo);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "ProcMgrDrv_ioctl",
                                      status,
                                      "Kernel-side ProcMgr_getProcInfo failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy only the params to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.procInfo),
                                       (const Ptr) procInfo,
                                       procInfoSize);
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }

            if (procInfo != NULL) {
                Memory_free(NULL, procInfo, procInfoSize);
            }
        }
getProcInfoRet:
        break;

        case CMD_PROCMGR_GETSECTIONINFO:
        {
            ProcMgr_CmdArgsGetSectionInfo srcArgs;
            ProcMgr_SectionInfo           sInfo;
            String                        sName;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetSectionInfo));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                sName = getname (srcArgs.sectionName);
                status = ProcMgr_getSectionInfo (srcArgs.handle,
                                                 srcArgs.fileId,
                                                 sName,
                                                 &sInfo);
                putname (sName);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "ProcMgrDrv_ioctl",
                                      status,
                                      "Kernel-side ProcMgr_getSectionInfo "
                                      "failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy only the params to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.sectionInfo),
                                       (const Ptr) &sInfo,
                                       sizeof (ProcMgr_SectionInfo));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        case CMD_PROCMGR_GETSECTIONDATA:
        {
            ProcMgr_CmdArgsGetSectionData srcArgs;
            ProcMgr_SectionInfo           sInfo;
            UInt8 *                       buffer;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetSectionData));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                retVal = copy_from_user ((Ptr) &sInfo,
                                         (const Ptr) (srcArgs.sectionInfo),
                                         sizeof (ProcMgr_SectionInfo));
            }

            if (retVal !=0)  {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                buffer = Memory_alloc (NULL, sInfo.size, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (buffer == NULL) {
                    status = ProcMgr_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "ProcMgrDrv_ioctl",
                                      status,
                                      "Memory_alloc failed!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    status = ProcMgr_getSectionData (srcArgs.handle,
                                                     srcArgs.fileId,
                                                     &sInfo,
                                                     buffer);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (status < 0) {
                        /* This does not impact return status of this function,
                         * so retVal comment is not used.
                         */
                        GT_setFailureReason (curTrace,
                                          GT_4CLASS,
                                          "ProcMgrDrv_ioctl",
                                          status,
                                          "Kernel-side ProcMgr_getSectionData "
                                          "failed");
                    }
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy only the params to user-side */
                retVal = copy_to_user ((Ptr) (srcArgs.buffer),
                                       (const Ptr) buffer,
                                       sInfo.size);
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
                Memory_free (NULL, buffer, sInfo.size);
            }
        }
        break;

        case CMD_PROCMGR_GETLOADEDFILEID:
        {
            ProcMgr_CmdArgsGetLoadedFileId srcArgs;

            /* Copy the full args from user-side. */
            retVal = copy_from_user ((Ptr) &srcArgs,
                                     (const Ptr) (args),
                                     sizeof (ProcMgr_CmdArgsGetLoadedFileId));

            /* This check is needed at run-time also since it depends on
             * run environment. It must not be optimized out.
             */
            if (retVal != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                           "        status [%d]",
                           retVal);
                osStatus = -EFAULT;
            }
            else {
                srcArgs.fileId = ProcMgr_getLoadedFileId (srcArgs.handle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (status < 0) {
                    /* This does not impact return status of this function,
                     * so retVal comment is not used.
                     */
                    GT_setFailureReason (curTrace,
                                      GT_4CLASS,
                                      "ProcMgrDrv_ioctl",
                                      status,
                                      "Kernel-side ProcMgr_getLoadedFileId "
                                      "failed");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (osStatus == 0) {
                /* Copy the full args to user-side */
                retVal = copy_to_user ((Ptr) (args),
                                       (const Ptr) &srcArgs,
                                       sizeof (ProcMgr_CmdArgsGetLoadedFileId));
                /* This check is needed at run-time also since it depends on
                 * run environment. It must not be optimized out.
                 */
                if (retVal != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    ProcMgrDrv_ioctl: copy_to_user call failed\n"
                              "        status [%d]",
                              retVal);
                    osStatus = -EFAULT;
                }
            }
        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = ProcMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ProcMgrDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    /* Set the status and copy the common args to user-side. */
    commonArgs.apiStatus = status;
    retVal = copy_to_user ((Ptr) cmdArgs,
                           (const Ptr) &commonArgs,
                           sizeof (ProcMgr_CmdArgs));

    if (retVal != 0) {
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "    ProcMgrDrv_ioctl: copy_from_user call failed\n"
                   "        status [%d]",
                   retVal);
        osStatus = -EFAULT;
    }
    else {
        if (status == -ERESTARTSYS) {
            /*! @retval -ERESTARTSYS Call was interrupted */
            osStatus = -ERESTARTSYS;
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "ProcMgrDrv_ioctl", osStatus);

    /*! @retval 0 Operation successfully completed. */
    return osStatus;
}


/*!
 *  @brief  Linux driver function to map memory regions to user space.
 *
 *  @param  filp    File structure pointer.
 *  @param  vma     User virtual memory area structure pointer.
 *
 *  @sa     ProcMgrDrv_open
 */
static
int
ProcMgrDrv_mmap (struct file * filp, struct vm_area_struct * vma)
{
#ifdef CONFIG_MMU
    vma->vm_page_prot = pgprot_noncached (vma->vm_page_prot);
#endif

    if (remap_pfn_range (vma,
                         vma->vm_start,
                         vma->vm_pgoff,
                         vma->vm_end - vma->vm_start,
                         vma->vm_page_prot)) {
        return -EAGAIN;
    }
    return 0;
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
int __init ProcMgrDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("ProcMgr module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("ProcMgrDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        ProcMgr_enableTrace = simple_strtol (TRACE, NULL, 16);
        if ((ProcMgr_enableTrace != 0) && (ProcMgr_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (ProcMgr_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (ProcMgr_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        ProcMgr_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (ProcMgr_enableTraceEnter != 0)
            &&  (ProcMgr_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (ProcMgr_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        ProcMgr_enableTraceFailure = simple_strtol (TRACEFAILURE, NULL, 16);
        if (    (ProcMgr_enableTraceFailure != 0)
            &&  (ProcMgr_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (ProcMgr_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        ProcMgr_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (ProcMgr_enableTraceFailure != 1)
            &&  (ProcMgr_enableTraceFailure != 2)
            &&  (ProcMgr_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            ProcMgr_traceClass =
                            ProcMgr_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= ProcMgr_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    ProcMgrDrv_osalDrvHandle = ProcMgrDrv_registerDriver ();
    if (ProcMgrDrv_osalDrvHandle == NULL) {
        /*! @retval ProcMgr_E_OSFAILURE Failed to register ProcMgr
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "ProcMgrDrv_initializeModule",
                          ProcMgr_E_OSFAILURE,
                          "Failed to register ProcMgr driver with OS!");
    }

    Osal_printf ("ProcMgrDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit ProcMgrDrv_finalizeModule (void)
{
    Osal_printf ("Entered ProcMgrDrv_finalizeModule\n");

    ProcMgrDrv_unregisterDriver (&(ProcMgrDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving ProcMgrDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (ProcMgrDrv_initializeModule);
module_exit (ProcMgrDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
