/*
 *  @file   FrameQBufMgrDrv.c
 *
 *  @brief      OS-specific implementation of FrameQBufMgr driver for Linux
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

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/knl/OsalDriver.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>

/* Linux specific header files */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>

/* Module specific header files */
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/FrameQBufMgrDefs.h>
#include <ti/syslink/FrameQBufMgr.h>
#include <ti/syslink/ipc/_FrameQBufMgr.h>
#include <ti/syslink/FrameQBufMgr_ShMem.h>
#include <ti/syslink/ipc/_FrameQBufMgr_ShMem.h>
#include <ti/syslink/inc/FrameQBufMgrDrvDefs.h>

/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* FrameQBufMgr functions */
EXPORT_SYMBOL(FrameQBufMgr_getConfig);
EXPORT_SYMBOL(FrameQBufMgr_setup);
EXPORT_SYMBOL(FrameQBufMgr_destroy);
EXPORT_SYMBOL(FrameQBufMgr_create);
EXPORT_SYMBOL(FrameQBufMgr_delete);
EXPORT_SYMBOL(FrameQBufMgr_open);
EXPORT_SYMBOL(FrameQBufMgr_close);
EXPORT_SYMBOL(FrameQBufMgr_getHandle);
EXPORT_SYMBOL(FrameQBufMgr_getId);
EXPORT_SYMBOL(FrameQBufMgr_alloc);
EXPORT_SYMBOL(FrameQBufMgr_allocv);
EXPORT_SYMBOL(FrameQBufMgr_free);
EXPORT_SYMBOL(FrameQBufMgr_add);
EXPORT_SYMBOL(FrameQBufMgr_registerNotifier);
EXPORT_SYMBOL(FrameQBufMgr_unregisterNotifier);
EXPORT_SYMBOL(FrameQBufMgr_dup);
EXPORT_SYMBOL(FrameQBufMgr_getBaseHeaderSize);
EXPORT_SYMBOL(FrameQBufMgr_isCacheEnabledForFrameBuf);
EXPORT_SYMBOL(FrameQBufMgr_isCacheEnabledForHeaderBuf);
/* Implementation specific exports */
EXPORT_SYMBOL(FrameQBufMgr_ShMem_Params_init);
EXPORT_SYMBOL(FrameQBufMgr_ShMem_sharedMemReq);

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for FrameQBufMgr.
 */
#define FrameQBufMgr_MINOR_NUMBER 15


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int FrameQBufMgrDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int FrameQBufMgrDrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long FrameQBufMgrDrv_ioctl (struct file *  filp,
                                   unsigned int   cmd,
                                   unsigned long  args);

/*!
 *  @brief  Internal utility function to check if address is required
 */
static Bool _is_virt2phys_translationRequired(Ptr addr);

/*!
 *  @brief  Internal utility function to check if address is required
 */
static Bool _is_phys2virt_translationRequired(Ptr addr);

#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init FrameQBufMgrDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit FrameQBufMgrDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations FrameQBufMgrDrv_driverOps = {
    open:    FrameQBufMgrDrv_open,
    release: FrameQBufMgrDrv_close,
    unlocked_ioctl:   FrameQBufMgrDrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool FrameQBufMgrDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool FrameQBufMgrDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool FrameQBufMgrDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 FrameQBufMgrDrv_traceClass = 0;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the FrameQBufMgr with OsalDriver
 */
Ptr
FrameQBufMgrDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "FrameQBufMgrDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("FrameQBufMgr",
                                            &FrameQBufMgrDrv_driverOps,
                                            FrameQBufMgr_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQBufMgrDrv_registerDriver",
                             FrameQBufMgr_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgrDrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the ProcMgr with OsalDriver
 */
Void
FrameQBufMgrDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "FrameQBufMgrDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "FrameQBufMgrDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
FrameQBufMgrDrv_open (struct inode * inode, struct file * filp)
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
int
FrameQBufMgrDrv_close (struct inode * inode, struct file * filp)
{
    return 0;
}


/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long
FrameQBufMgrDrv_ioctl (struct file *  filp,
                       unsigned int   cmd,
                       unsigned long  args)
{
    int                       osStatus    = 0;
    FrameQBufMgrDrv_CmdArgs * dstArgs     = (FrameQBufMgrDrv_CmdArgs *) args;
    Int32                     status      = FrameQBufMgr_S_SUCCESS;
    Int32                     ret;
    UInt16                    index;
    FrameQBufMgrDrv_CmdArgs   cargs;

    GT_3trace (curTrace, GT_ENTER, "FrameQBufMgrDrv_ioctl",
               filp, cmd, args);

    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (FrameQBufMgrDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    switch (cmd) {
        case CMD_FRAMEQBUFMGR_GETCONFIG:
        {
            FrameQBufMgr_Config config;

            status = FrameQBufMgr_getConfig (&config);
            GT_assert (curTrace, (status >= 0));

            ret = copy_to_user (cargs.args.getConfig.config,
                                &config,
                                sizeof (FrameQBufMgr_Config));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    FrameQBufMgrDrv_ioctl: copy_to_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_FRAMEQBUFMGR_SETUP:
        {
            FrameQBufMgr_Config config;

            ret = copy_from_user (&config,
                                  cargs.args.setup.config,
                                  sizeof (FrameQBufMgr_Config));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    FrameQBufMgrDrv_ioctl: copy_from_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = FrameQBufMgr_setup (&config);
                GT_assert (curTrace, (status >= 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_FRAMEQBUFMGR_DESTROY:
        {
            status = FrameQBufMgr_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQBUFMGR_CREATE:
        {
            Ptr                       params = NULL;
            UInt32                    size = 0;
            FrameQBufMgr_ShMem_Params *shMemParams = NULL;
            Ptr                       cliNotifyMgrshMem;
            Ptr                       cliGateShMem;
            UInt32                    i;

            /* Allocate memory for the params pointer and copy the  params from
             * userspace pointer based on the type of interface
             */
            if (    cargs.args.create.interfaceType
                ==  FrameQBufMgr_INTERFACE_SHAREDMEM) {
                size = sizeof(FrameQBufMgr_ShMem_Params);
            }

            params = Memory_calloc(NULL, size, 0u, NULL);

            ret = copy_from_user (params,
                                  cargs.args.create.params,
                                  size);
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    FrameQBufMgrDrv_ioctl: copy_from_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for the name */
                if (cargs.args.create.nameLen > 0) {
                    ((FrameQBufMgr_Params*)params)->commonCreateParams.name =
                                   Memory_alloc (NULL,
                                                 cargs.args.create.nameLen,
                                                 0,
                                                 NULL);
                    GT_assert (curTrace,
                               (((FrameQBufMgr_Params*)params)->
                                             commonCreateParams.name != NULL));
                    /* Copy the name */
                    ret = copy_from_user (
                        ((FrameQBufMgr_Params*)params)->commonCreateParams.name,
                        cargs.args.create.name,
                        cargs.args.create.nameLen);
                    GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (ret != 0) {
                        GT_1trace (curTrace,
                                  GT_1CLASS,
                                  "    FrameQBufMgrDrv_ioctl: copy_from_user"
                                  " call failed\n"
                                  "        status [%d]",
                                  ret);
                        osStatus = -EFAULT;
                    }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }

            if (osStatus == 0) {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (     cargs.args.create.interfaceType
                     ==  FrameQBufMgr_INTERFACE_SHAREDMEM) {
                    shMemParams = (FrameQBufMgr_ShMem_Params*)params;
                    shMemParams->sharedAddr = SharedRegion_getPtr (
                                (SharedRegion_SRPtr)shMemParams->sharedAddr);
                    /*shMemParams->sharedAddr is null if user has not provided
                     * memory for it */
                    shMemParams->sharedAddrHdrBuf = SharedRegion_getPtr (
                        (SharedRegion_SRPtr)shMemParams->sharedAddrHdrBuf);
                    shMemParams->sharedAddrFrmBuf = SharedRegion_getPtr (
                        (SharedRegion_SRPtr)shMemParams->sharedAddrFrmBuf);


                    for (i = 0; i < shMemParams->numFreeFramePools;i++) {
                        shMemParams->frameBufParams[i] = Memory_alloc (NULL,
                                                                       sizeof(FrameQBufMgr_ShMem_FrameBufParams)* shMemParams->numFrameBufsInFrame[i],
                                                                       0,
                                                                       NULL);

                        ret = copy_from_user (
                        shMemParams->frameBufParams[i],
                        ((FrameQBufMgr_ShMem_Params*)cargs.args.create.params)->frameBufParams[i],
                        sizeof(FrameQBufMgr_ShMem_FrameBufParams)* shMemParams->numFrameBufsInFrame[i]);
                    GT_assert (curTrace, (ret == 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (ret != 0) {
                        GT_1trace (curTrace,
                                  GT_1CLASS,
                                  "    FrameQBufMgrDrv_ioctl: copy_from_user"
                                  " call failed\n"
                                  "        status [%d]",
                                  ret);
                        osStatus = -EFAULT;
                    }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (osStatus == 0) {
#endif
                    /* Call FrameQBufMgr_create  with the updated params */
                    cargs.args.create.handle = FrameQBufMgr_create (params);
                    GT_assert (curTrace, (cargs.args.create.handle != NULL));
                    if (cargs.args.create.handle == NULL) {
                        status =  FrameQBufMgr_E_FAIL;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "FrameQBufMgrDrv_ioctl",
                                             status,
                                             "FrameQBufMgr_create failed ");
                    }
                    else if (cargs.args.create.handle != NULL) {
                        cliNotifyMgrshMem = _FrameQBufMgr_getCliNotifyMgrShAddr(
                                                         cargs.args.create.handle);
                        index = SharedRegion_getId(cliNotifyMgrshMem);
                        cargs.args.create.cliNotifyMgrSharedMem =
                            SharedRegion_getSRPtr(cliNotifyMgrshMem, index);
                        cliGateShMem = _FrameQBufMgr_getCliNotifyMgrGateShAddr(
                                                         cargs.args.create.handle);
                        index = SharedRegion_getId(cliGateShMem);
                        cargs.args.create.cliGateSharedMem =
                            SharedRegion_getSRPtr(cliGateShMem, index);
                        cargs.args.create.instId = FrameQBufMgr_getId(
                                                         cargs.args.create.handle);
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (     cargs.args.create.interfaceType
                     ==  FrameQBufMgr_INTERFACE_SHAREDMEM) {
                    for (i = 0; i < shMemParams->numFreeFramePools;i++) {
                        Memory_free (NULL,
                                     shMemParams->frameBufParams[i],
                                     sizeof(FrameQBufMgr_ShMem_FrameBufParams)* shMemParams->numFrameBufsInFrame[i]);
                    }
                }

            if (   (cargs.args.create.nameLen > 0)
                && (  ((FrameQBufMgr_Params*)params)->commonCreateParams.name
                    != NULL)) {
                Memory_free (NULL,
                             ((FrameQBufMgr_Params*)params)->
                                                       commonCreateParams.name,
                             cargs.args.create.nameLen);
            }
            if (params != NULL) {
                Memory_free (NULL,
                             params,
                             size);
            }
        }
        break;

        case CMD_FRAMEQBUFMGR_DELETE:
        {
            status = FrameQBufMgr_delete ((FrameQBufMgr_Handle *)
                                            &cargs.args.deleteInstance.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQBUFMGR_OPEN:
        {
            Ptr                 openParams = NULL;
            UInt32              size       = 0;
            FrameQBufMgr_Handle handle;
            Ptr                 cliNotifyMgrshMem;
            Ptr                 cliGateShMem;

            /* Allocate memory for the params pointer and copy the  params from
             * userspace pointer based on the type of interface
             */
            if (     cargs.args.open.interfaceType
                 ==  FrameQBufMgr_INTERFACE_SHAREDMEM) {
                size = sizeof(FrameQBufMgr_ShMem_OpenParams);
            }
            openParams = Memory_alloc(NULL, size, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (openParams ==  NULL) {
                status = FrameQBufMgr_E_ALLOC_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQDrv_ioctl",
                                     status,
                                     "Memory_alloc failed for openParams");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                ret = copy_from_user (openParams,
                                      cargs.args.open.openParams,
                                      size);
                GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (ret != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              " FrameQBufMgrDrv_ioctl: copy_from_user"
                              " call failed\n"
                              "        status [%d]",
                              ret);
                    osStatus = -EFAULT;
                }
                else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Convert the shared control address to virtual format */
                    ((FrameQBufMgr_OpenParams *)openParams)->
                        commonOpenParams.sharedAddr =
                                      SharedRegion_getPtr ((SharedRegion_SRPtr)
                                           ((FrameQBufMgr_OpenParams *)openParams)->
                                            commonOpenParams.sharedAddr);
                    if (cargs.args.open.nameLen > 0) {
                        ((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.name =
                                       Memory_alloc (NULL,
                                                     cargs.args.open.nameLen,
                                                     0,
                                                     NULL);
                        GT_assert (curTrace, ((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.name);
                        /* Copy the name */
                        ret = copy_from_user (
                                              ((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.name,
                            ((FrameQBufMgr_OpenParams *)cargs.args.open.openParams)->commonOpenParams.name,
                            cargs.args.open.nameLen);
                        GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (ret != 0) {
                            GT_1trace (curTrace,
                                      GT_1CLASS,
                                      "    FrameQDrv_ioctl: copy_from_user"
                                      " call failed\n"
                                      "        status [%d]",
                                      ret);
                            osStatus = -EFAULT;
                        }
#endif
                    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (osStatus == 0) {
#endif
                        status = FrameQBufMgr_open (&handle, openParams);
                        GT_assert (curTrace, (status >= 0));
                        cargs.args.open.handle = handle;
                        if (cargs.args.open.handle != NULL) {
                            cliNotifyMgrshMem = _FrameQBufMgr_getCliNotifyMgrShAddr(
                                                             cargs.args.open.handle);
                            index = SharedRegion_getId(cliNotifyMgrshMem);
                            cargs.args.open.cliNotifyMgrSharedMem =
                                SharedRegion_getSRPtr(cliNotifyMgrshMem, index);
                            cliGateShMem = _FrameQBufMgr_getCliNotifyMgrGateShAddr(
                                                             cargs.args.open.handle);
                            index = SharedRegion_getId(cliGateShMem);
                            cargs.args.open.cliGateSharedMem =
                                SharedRegion_getSRPtr(cliGateShMem, index);
                            cargs.args.open.instId = FrameQBufMgr_getId(
                                                             cargs.args.open.handle);
                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.name != NULL) {
                    Memory_free(NULL,
                                ((FrameQBufMgr_OpenParams *)openParams)->commonOpenParams.name,
                                cargs.args.open.nameLen);
                }

                Memory_free (NULL, openParams, size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_FRAMEQBUFMGR_CLOSE:
        {
            status = FrameQBufMgr_close (&cargs.args.close.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQBUFMGR_GETHANDLE:
        {
            cargs.args.getHandle.handle =
                            FrameQBufMgr_getHandle (cargs.args.getHandle.instId);
        }
        break;

        case CMD_FRAMEQBUFMGR_GETID:
        {
            cargs.args.getId.instId =
                                FrameQBufMgr_getId (cargs.args.getId.handle);
        }
        break;

        case CMD_FRAMEQBUFMGR_ALLOC:
        {
            FrameQBufMgr_Frame  frame;
            FrameQBufMgr_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              j;

            status = FrameQBufMgr_alloc (cargs.args.alloc.handle,
                                         &frame);
            if (frame != NULL) {
                /* Convert Frame and frame buffer address in frame to physical
                 * address formatso that user space api convert this physical
                 * address in to user space virtual format.
                 */
                frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                    &(frame->frameBufInfo[0]);

                /* check to see if address translation is required (MK) */
                if (_is_virt2phys_translationRequired(
                        (Ptr)frameBufInfo[0].bufPtr)) {

                    /* translate frame buffer addresses */
                    for (j = 0; j < frame->numFrameBuffers; j++) {

                        addr = (Ptr)frameBufInfo[j].bufPtr;
                        phyPtr = Memory_translate(addr,
                                                  Memory_XltFlags_Virt2Phys);
                        GT_assert(curTrace,(phyPtr != NULL));
                        frameBufInfo[j].bufPtr = (UInt32)phyPtr;
                    }
                }

		/* frame ptr is in the header, so always translate */
                cargs.args.alloc.frame = Memory_translate(
                                                     frame,
                                                     Memory_XltFlags_Virt2Phys);
            }
            else {
                cargs.args.alloc.frame = NULL;
            }
        }
        break;

        case CMD_FRAMEQBUFMGR_ALLOCV:
        {
            FrameQBufMgr_Frame  framePtr[FrameQ_MAX_FRAMESINVAPI];
            FrameQBufMgr_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              pFreeQId[FrameQ_MAX_FRAMESINVAPI];
            UInt32              numFrames;
            UInt32              i;
            UInt32              j;
            Bool                virt2Phys_isRequired;

            numFrames =  cargs.args.allocv.numFrames;

            GT_assert(curTrace, (numFrames <= FrameQ_MAX_FRAMESINVAPI));

            ret = copy_from_user (pFreeQId,
                                  cargs.args.allocv.afreeQId,
                                  ( numFrames * sizeof (UInt32)));

            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    FrameQBufMgrDrv_ioctl: copy_from_user"
                              " call failed\n"
                              "        status [%d]",
                              ret);
                    osStatus = -EFAULT;
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */

            if ((status >= 0) && (osStatus == 0)) {

                status = FrameQBufMgr_allocv (cargs.args.allocv.handle,
                                         framePtr,
                                         (Ptr)pFreeQId,
                                         (UInt8*)&cargs.args.allocv.numFrames);

                if ((status >= 0) && (cargs.args.allocv.numFrames > 0)) {

                    /* check if address translation is required (MK) */
                    virt2Phys_isRequired =
                            _is_virt2phys_translationRequired(
                                (Ptr)framePtr[0]->frameBufInfo[0].bufPtr);

                    for (i = 0; i < cargs.args.allocv.numFrames; i++) {
                        /*
                         * Convert Frame and frame buffer address in frame
                         * to physical address so that user space api can
                         * convert physical addresses to user space virtual
                         */
                        frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                        &(framePtr[i]->frameBufInfo[0]);

                        if (virt2Phys_isRequired) {

                            for (j = 0; j < framePtr[i]->numFrameBuffers; j++){
                                addr = (Ptr)frameBufInfo[j].bufPtr;
                                phyPtr = Memory_translate(
                                                 addr,
                                                 Memory_XltFlags_Virt2Phys);
                                GT_assert(curTrace,(phyPtr != NULL));
                                frameBufInfo[j].bufPtr = (UInt32)phyPtr;
                            }
                        }

			/* always translate header pointers */
                        framePtr[i] = Memory_translate( framePtr[i],
                                                 Memory_XltFlags_Virt2Phys);
                    }

                    ret = copy_to_user (cargs.args.allocv.aframePtr, framePtr,
                                        (  sizeof (UInt32)
                                         * cargs.args.allocv.numFrames));
                    GT_assert (curTrace, (ret == 0));
                }
            }
        }
        break;

        case CMD_FRAMEQBUFMGR_FREE:
        {
            FrameQBufMgr_Frame          frame;
            FrameQBufMgr_FrameBufInfo   *frameBufInfo;
            Ptr                         virtPtr;
            Ptr                         addr;
            UInt32                      i;
            /* Convert frame address and frame buffer address in to knl virtual
             * format when necessary.
             */
            frame = Memory_translate (cargs.args.free.frame,
                                      Memory_XltFlags_Phys2Virt);
            GT_assert(curTrace,(frame != NULL));
            frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                                     &(frame->frameBufInfo[0]);

            /* Frame buffer addresses will only be converted when Heap alloc'd*/
            if (_is_phys2virt_translationRequired(
                               (Ptr)frameBufInfo[0].bufPtr)) {

                for (i = 0; i < frame->numFrameBuffers; i++) {
                    addr = (Ptr)frameBufInfo[i].bufPtr;
                    virtPtr = Memory_translate(addr, Memory_XltFlags_Phys2Virt);
                    GT_assert(curTrace,(virtPtr != NULL));
                    frameBufInfo[i].bufPtr = (UInt32)virtPtr;
                }
            }

            status = FrameQBufMgr_free (cargs.args.free.handle,
                                        frame);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQBUFMGR_FREEV:
        {
            FrameQBufMgr_Frame          framePtr[FrameQ_MAX_FRAMESINVAPI];
            FrameQBufMgr_FrameBufInfo   *frameBufInfo;
            Ptr                         virtPtr;
            Ptr                         addr;
            UInt32                      i;
            UInt32                      j;

            GT_assert(curTrace, (cargs.args.freev.numFrames
                                 <= FrameQ_MAX_FRAMESINVAPI));

            ret = copy_from_user (framePtr,
                              cargs.args.freev.aframePtr,
                              (  cargs.args.freev.numFrames * sizeof(UInt32)));
            GT_assert (curTrace, (ret == 0));

            for (i = 0; i < cargs.args.freev.numFrames; i++) {
                /* Convert frame address and frame buffer address in to knl
                 * virtual format.
                 */
                framePtr[i] = Memory_translate (framePtr[i],
                                          Memory_XltFlags_Phys2Virt);
                GT_assert(curTrace,(framePtr[i] != NULL));

                frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                            &(framePtr[i]->frameBufInfo[0]);

                /* Convert Frame buffer addresses only when Heap alloc'd*/
                if (_is_phys2virt_translationRequired(
                                   (Ptr)frameBufInfo[0].bufPtr)) {

                    for (j = 0; j < framePtr[i]->numFrameBuffers; j++) {
                        addr = (Ptr)frameBufInfo[j].bufPtr;
                        virtPtr = Memory_translate(addr,
                                               Memory_XltFlags_Phys2Virt);
                        GT_assert(curTrace,(virtPtr != NULL));
                        frameBufInfo[j].bufPtr = (UInt32)virtPtr;
                    }
                }
            }

            status = FrameQBufMgr_freev (cargs.args.freev.handle,
                                         framePtr,
                                         cargs.args.freev.numFrames);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQBUFMGR_DUP:
        {
            FrameQBufMgr_Frame          frame;
            FrameQBufMgr_Frame          dupedFramePtr[FrameQ_MAX_FRAMESINVAPI];
            UInt32                      i;

            GT_assert(curTrace,
                   (cargs.args.dup.numDupedFrames < FrameQ_MAX_FRAMESINVAPI));

            GT_assert(curTrace,(cargs.args.dup.frame != NULL));

            /* Convert original frame address and frame buffer address in to
             * knl virtual format.
             */
            frame = Memory_translate (cargs.args.dup.frame,
                                      Memory_XltFlags_Phys2Virt);

            GT_assert(curTrace,(frame != NULL));

            status = FrameQBufMgr_dup (cargs.args.dup.handle,
                                       frame,
                                       dupedFramePtr,
                                       cargs.args.dup.numDupedFrames);
            if (status >= 0) {

                for (i = 0; i < cargs.args.dup.numDupedFrames; i++) {
                    /* Convert Frame and frame buffer address in frame to
                     * physical address formatso that user space api convert
                     * this physical address in to user space virtual format.
                     */
                    dupedFramePtr[i] =  Memory_translate(
                                             dupedFramePtr[i],
                                             Memory_XltFlags_Virt2Phys);
                }
                ret = copy_to_user (cargs.args.dup.adupedFramePtr,
                                    dupedFramePtr,
                                    (  sizeof (UInt32)
                                     * cargs.args.dup.numDupedFrames));
                GT_assert (curTrace, (ret == 0));
            }
        }
        break;

        case CMD_FRAMEQBUFMGR_DUPV:
        {
            FrameQBufMgr_Frame          *framePtr = NULL;
            FrameQBufMgr_Frame          **dupedFramePtr = NULL;
            FrameQBufMgr_FrameBufInfo   *frameBufInfo ;
            Ptr                         virtPtr;
            Ptr                         phyPtr;
            Ptr                         addr;
            UInt32                      i;
            UInt32                      j;
            UInt32                      k;

            dupedFramePtr =  Memory_calloc(NULL,
                                          (  cargs.args.dupv.numFrames
                                           * sizeof(UInt32)),
                                          0,
                                          NULL);
            GT_assert(curTrace,(dupedFramePtr != NULL));

            if  (dupedFramePtr == NULL) {
                   status = FrameQBufMgr_E_ALLOC_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "FrameQBufMgrDrv_ioctl",
                                         status,
                                         "Memory_alloc failed for dupedFramePtr");
            }
            else {
                for (i = 0u; i < cargs.args.dupv.numFrames; i++) {
                    dupedFramePtr[i] = Memory_calloc (NULL,
                              cargs.args.dupv.numDupedFrames * sizeof(UInt32),
                              0u,
                              NULL);
                    GT_assert(curTrace,(dupedFramePtr[i] != NULL));
                    if  (dupedFramePtr[i] == NULL) {
                        status = FrameQBufMgr_E_ALLOC_MEMORY;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "FrameQBufMgrDrv_ioctl",
                                             status,
                                             "Memory_alloc failed for dupedFramePtr");
                    }
                }
            }

            if (status >= 0u) {
                framePtr =  Memory_alloc(NULL,
                                         (  cargs.args.dupv.numFrames
                                          * sizeof(UInt32)),
                                         0u,
                                         NULL);
                GT_assert(curTrace,(framePtr != NULL));

                ret = copy_from_user (framePtr,
                                      cargs.args.dupv.framePtr,
                                      (  cargs.args.dupv.numFrames
                                       * sizeof(UInt32)));
                GT_assert (curTrace, (ret == 0u));

                for (i = 0u; i < cargs.args.dupv.numFrames; i++) {
                    /* Convert original frame address and frame buffer address
                     * in to knl virtual format.
                     */
                    framePtr[i] = Memory_translate (framePtr[i],
                                              Memory_XltFlags_Phys2Virt);
                    GT_assert(curTrace,(framePtr[i] != NULL));
                    frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                               &(framePtr[i]->frameBufInfo[0u]);

                    for(i = 0u; i < framePtr[i]->numFrameBuffers; i++) {
                        addr = (Ptr)frameBufInfo[i].bufPtr;
                        virtPtr = Memory_translate(addr,
                                                   Memory_XltFlags_Phys2Virt);
                        GT_assert(curTrace,(virtPtr != NULL));
                        frameBufInfo[i].bufPtr = (UInt32)virtPtr;
                    }
                }
                status = FrameQBufMgr_dupv (cargs.args.dupv.handle,
                                            framePtr,
                                            dupedFramePtr,
                                            cargs.args.dupv.numDupedFrames,
                                            cargs.args.dupv.numFrames);
                Memory_free(NULL,
                            framePtr,
                            (  cargs.args.dupv.numFrames * sizeof(UInt32)));
                if (status >= 0u) {
                    for (i = 0u;i < cargs.args.dupv.numFrames; i++) {
                        for (j = 0u; j < cargs.args.dup.numDupedFrames; j++) {
                            /* Convert Frame and frame buffer address in frame
                             * to physical address formatso that user space api
                             * convert this physical address in to user space
                             * virtual format.
                             */
                            frameBufInfo = (FrameQBufMgr_FrameBufInfo *)
                                        &(dupedFramePtr[i][j]->frameBufInfo[0u]);

/* MK: TODO: this looks like an issue. shouldn't translate bufPtrs ??? */
                            for(k = 0u;
                                k < dupedFramePtr[i][j]->numFrameBuffers; k++) {
                                addr = (Ptr)frameBufInfo[j].bufPtr;
                                phyPtr = Memory_translate(
                                                     addr,
                                                     Memory_XltFlags_Virt2Phys);
                                GT_assert(curTrace,(phyPtr != NULL));
                                frameBufInfo[k].bufPtr = (UInt32)phyPtr;
                            }
                            dupedFramePtr[i][j] =  Memory_translate(
                                                     dupedFramePtr[i][j],
                                                     Memory_XltFlags_Virt2Phys);
                        }
                        ret = copy_to_user (cargs.args.dupv.dupedFramePtr[i],
                                            dupedFramePtr[i],
                                            (  sizeof (UInt32)
                                             * cargs.args.dupv.numDupedFrames));
                        GT_assert (curTrace, (ret == 0u));
                    }
                }
            }


            /* Free allocated memories */
            if (dupedFramePtr != NULL) {
                for (i = 0u; i < cargs.args.dupv.numFrames; i++) {
                    if  (dupedFramePtr[i] != NULL) {
                        Memory_free (NULL,
                                     dupedFramePtr[i],
                                     (  cargs.args.dupv.numDupedFrames
                                      * sizeof(UInt32)));
                    }
                }
                Memory_free (NULL,
                             dupedFramePtr,
                             (  cargs.args.dupv.numFrames
                              * sizeof(UInt32)));
            }
        }
        break;

        case CMD_FRAMEQBUFMGR_REG_NOTIFIER :
        {

            status = FrameQBufMgr_registerNotifier (
                                 cargs.args.regNotifier.handle,
                                 &cargs.args.regNotifier.notifyParams);
           GT_assert (curTrace, (status >= 0u));
        }
        break;

        case CMD_FRAMEQBUFMGR_UNREG_NOTIFIER :
        {
            status = FrameQBufMgr_unregisterNotifier (
                                              cargs.args.unregNotifier.handle);
        }
        break;

        case  CMD_FRAMEQBUFMGR_SET_NOTIFYID:
        {
            status = _FrameQBufMgr_setNotifyId (
                                     cargs.args.setNotifyId.handle,
                                     cargs.args.setNotifyId.notifyId);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQBUFMGR_RESET_NOTIFYID :
        {
            status = _FrameQBufMgr_resetNotifyId (
                                     cargs.args.resetNotifyId.handle,
                                     cargs.args.resetNotifyId.notifyId);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQBUFMGR_SHAREDMEMREQ:
        {
        }
        break;

        case CMD_FRAMEQBUFMGR_ADD:
        {
            status = FrameQBufMgr_E_NOTIMPLEMENTED;
            GT_assert (curTrace, (status >= 0));
        }
        break;
        case CMD_FRAMEQBUFMGR_REMOVE:
        {
            status = FrameQBufMgr_E_NOTIMPLEMENTED;
            GT_assert (curTrace, (status >= 0));
        }
        break;
        case CMD_FRAMEQBUFMGR_TRANSLATE:
        {
            Ptr     srcAddr;
            Ptr     dstAddr;

            srcAddr = (Ptr) Memory_translate ((Ptr)cargs.args.translate.srcAddr,
                                              Memory_XltFlags_Phys2Virt);
            GT_assert (curTrace, (srcAddr != NULL));
            status = FrameQBufMgr_translateAddr (
                             cargs.args.translate.handle,
                            &dstAddr,
                            cargs.args.translate.dstAddrType,
                            srcAddr,
                            cargs.args.translate.srcAddrType,
                            cargs.args.translate.bufType);
            if (status >= 0) {
                cargs.args.translate.dstAddr = Memory_translate(
                                                    (Ptr)dstAddr,
                                                     Memory_XltFlags_Virt2Phys);
            }
        }
        break;

        case CMD_FRAMEQBUFMGR_GET_NUMFREEFRAMES :
        {
            /* Get the available frames in fillef queue 0 of FrameQBufMgr */
            UInt32  numFreeFrames;

            status = FrameQBufMgr_getNumFreeFrames(cargs.args.getNumFreeFrames.handle,
                                            &numFreeFrames);
            cargs.args.getNumFreeFrames.numFreeFrames = numFreeFrames;

        }
        break;

        case CMD_FRAMEQBUFMGR_GET_VNUMFREEFRAMES :
        {
            /* Get the available frames in fillef queue 0 of FrameQBufMgr */
            UInt32  freeQueueId[FrameQ_MAX_FRAMESINVAPI];
            UInt32  numFreeFrames[FrameQ_MAX_FRAMESINVAPI];

            GT_assert(curTrace, (cargs.args.getvNumFreeFrames.numFreeQids <=
                                 FrameQ_MAX_FRAMESINVAPI));

            ret = copy_from_user (freeQueueId,
                                  cargs.args.getvNumFreeFrames.freeQId,
                                  ( cargs.args.getvNumFreeFrames.numFreeQids
                                   * sizeof (UInt32)));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    FrameQBufMgrDrv_ioctl: copy_from_user"
                              " call failed\n"
                              "        status [%d]",
                              ret);
                    osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = FrameQBufMgr_getvNumFreeFrames(
                                cargs.args.getvNumFreeFrames.handle,
                                numFreeFrames,
                                (Ptr)freeQueueId,
                                cargs.args.getvNumFreeFrames.numFreeQids);

                ret = copy_to_user (
                            cargs.args.getvNumFreeFrames.numFreeFrames,
                            numFreeFrames,
                            (  sizeof (UInt32)
                             * cargs.args.getvNumFreeFrames.numFreeQids));
                GT_assert (curTrace, (ret == 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_FRAMEQBUFMGR_CONTROL     :
        {
            Ptr arg;

            arg = Memory_alloc(NULL, cargs.args.control.size, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (arg == NULL) {
                status = FrameQBufMgr_E_ALLOC_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQBufMgrDrv_ioctl",
                                     status,
                                     "Memory_alloc failed for openParams");

            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                ret = copy_from_user (arg,
                                      cargs.args.control.arg,
                                      cargs.args.control.size);
                GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (ret != 0) {
                    GT_1trace (curTrace,
                               GT_1CLASS,
                               "    FrameQBufMgrDrv_ioctl: copy_to_user call"
                               " failed\n"
                               "        status [%d]",
                               ret);
                    osStatus = -EFAULT;
                }
                else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */

                    status = FrameQBufMgr_control(cargs.args.control.handle,
                                                  cargs.args.control.cmd,
                                                  arg);
                    if (status >= 0) {
                        /* TODO: copy  arg to user space if required based on cmd */
                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                Memory_free (NULL, arg, cargs.args.control.size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */

        }
        break;

        /* params init for ShMem implementation */
        case CMD_FRAMEQBUFMGR_SHMEM_PARAMS_INIT  :
        {
            FrameQBufMgr_ShMem_Params params;

            FrameQBufMgr_ShMem_Params_init(&params);

            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (FrameQBufMgr_ShMem_Params));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    FrameQDrv_ioctl: copy_to_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = FrameQBufMgr_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "FrameQBufMgrDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }
    cargs.apiStatus = status;

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgrDrv_ioctl",
               osStatus);

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (FrameQBufMgrDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "FrameQBufMgrDrv_ioctl",
               osStatus);

    /*! @retval 0 Operation successfully completed. */
    return (osStatus);
}


/*
 *  Internal utility function:
 *  NOTE:   Translate frame buffer pointers only when createHeap is TRUE.
 *          otherwise do no buffer pointer translation,  expect Physical
 *          addresses from the user-mode and leave them as-is, since they
 *          are no longer mapped to Master Kernel Virtual.
 *          This also means that Kernel APIs can not access the frame-buffer
 *          and will only serve to convert them to Portable pointers where
 *          necessary.
 */
static Bool _is_phys2virt_translationRequired(Ptr addr)
{
    UInt16              regionId;

    /*  this should always obtain a valid region id for both
     *  heap  or no-heap SR configuration.
     */
    regionId = _SharedRegion_getIdPhys(addr);

    /*
     * If frame buffer shared region has createHeap == FALSE
     * then we will not convert virtual ptrs to physical.
     * In this case virtual == physical.
     */
    if (SharedRegion_getHeap(regionId) == NULL) {
        /*
         * region has no heap => so the address is already Physical
         * No translation is required.
         */
        return FALSE;

    } else {
        /*
         * region has heap => so the address is virtual and it needs to
         * to be translated virtual to physical.
         */
        return TRUE;
    }
 }

/*
 *  Internal utility function
 */
static Bool _is_virt2phys_translationRequired(Ptr addr)
{
    UInt16              regionId;
    /*
     * SharedRegion_getId()  should always obtain a valid region id whether
     * Shared Region was created with or without a heap.
     */
    regionId = SharedRegion_getId(addr);

    /*
     * If frame buffer shared region has createHeap == FALSE
     * then we will not convert virtual ptrs to physical.
     * In this case virtual == physical.
     */
    if (SharedRegion_getHeap(regionId) == NULL) {
        /*
         * region has no heap => so the address is already Physical
         * No translation is required.
         */
        return FALSE;

    } else {
        /*
         * region has heap => so the address is virtual and it needs to
         * to be translated virtual to physical.
         */
        return TRUE;
    }
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
int __init FrameQBufMgrDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("ProcMgr sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("FrameQBufMgrDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        FrameQBufMgrDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if ((FrameQBufMgrDrv_enableTrace != 0) && (FrameQBufMgrDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (FrameQBufMgrDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (FrameQBufMgrDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        FrameQBufMgrDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (FrameQBufMgrDrv_enableTraceEnter != 0)
            &&  (FrameQBufMgrDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (FrameQBufMgrDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        FrameQBufMgrDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                         NULL,
                                                         16);
        if (    (FrameQBufMgrDrv_enableTraceFailure != 0)
            &&  (FrameQBufMgrDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (FrameQBufMgrDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        FrameQBufMgrDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (FrameQBufMgrDrv_traceClass != 1)
            &&  (FrameQBufMgrDrv_traceClass != 2)
            &&  (FrameQBufMgrDrv_traceClass != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            FrameQBufMgrDrv_traceClass =
                          FrameQBufMgrDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= FrameQBufMgrDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    Osal_printf ("FrameQBufMgrDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit FrameQBufMgrDrv_finalizeModule (void)
{
    Osal_printf ("FrameQBufMgrDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (FrameQBufMgrDrv_initializeModule);
module_exit (FrameQBufMgrDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
