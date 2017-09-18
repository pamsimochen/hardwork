/*
 *  @file   ClientNotifyMgrDrv.c
 *
 *  @brief      OS-specific implementation of ClientNotifyMgr driver for Linux
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
#include <ti/syslink/utils/Gate.h>
#include <ti/syslink/inc/ClientNotifyMgr_errBase.h>
#include <ti/syslink/inc/ClientNotifyMgr_config.h>
#include <ti/syslink/inc/_ClientNotifyMgr.h>
#include <ti/syslink/inc/ClientNotifyMgr.h>
#include <ti/syslink/inc/ClientNotifyMgrDrvDefs.h>

/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* ClientNotifyMgr functions */
EXPORT_SYMBOL(ClientNotifyMgr_getConfig);
EXPORT_SYMBOL(ClientNotifyMgr_setup);
EXPORT_SYMBOL(ClientNotifyMgr_destroy);
EXPORT_SYMBOL(ClientNotifyMgr_sharedMemModReq);
EXPORT_SYMBOL(ClientNotifyMgr_sharedMemReq);
EXPORT_SYMBOL(ClientNotifyMgr_Params_init);
EXPORT_SYMBOL(ClientNotifyMgr_create);
EXPORT_SYMBOL(ClientNotifyMgr_delete);
EXPORT_SYMBOL(ClientNotifyMgr_close);
EXPORT_SYMBOL(ClientNotifyMgr_registerClient);
EXPORT_SYMBOL(ClientNotifyMgr_unregisterClient);
EXPORT_SYMBOL(ClientNotifyMgr_enableNotification);
EXPORT_SYMBOL(ClientNotifyMgr_disableNotification);
EXPORT_SYMBOL(ClientNotifyMgr_sendNotificationMulti);
EXPORT_SYMBOL(ClientNotifyMgr_sendNotification);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for ProcMgr.
 */
#define CLIENTNOTIFYMGR_DRV_MINOR_NUMBER 14


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int ClientNotifyMgr_drvopen (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int ClientNotifyMgr_drvclose (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long ClientNotifyMgr_drvioctl (struct file *  filp,
                                      unsigned int   cmd,
                                      unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init ClientNotifyMgr_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit ClientNotifyMgr_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations ClientNotifyMgr_driverOps = {
    open:    ClientNotifyMgr_drvopen,
    release: ClientNotifyMgr_drvclose,
    unlocked_ioctl:   ClientNotifyMgr_drvioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool ClientNotifyMgr_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool ClientNotifyMgr_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool ClientNotifyMgr_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 ClientNotifyMgr_traceClass = 0;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the ClientNotifyMgr with OsalDriver
 */
Ptr
ClientNotifyMgrDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "ClientNotifyMgrDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("ClientNotifyMgr",
                                            &ClientNotifyMgr_driverOps,
                                            CLIENTNOTIFYMGR_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ClientNotifyMgrDrv_registerDriver",
                             ClientNotifyMgr_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the ProcMgr with OsalDriver
 */
Void
ClientNotifyMgrDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "ClientNotifyMgrDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
ClientNotifyMgr_drvopen (struct inode * inode, struct file * filp)
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
ClientNotifyMgr_drvclose (struct inode * inode, struct file * filp)
{
    return 0;
}


/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long ClientNotifyMgr_drvioctl (struct file *  filp,
                               unsigned int   cmd,
                               unsigned long  args)
{
    int                          osStatus = 0;
    ClientNotifyMgrDrv_CmdArgs * dstArgs  = (ClientNotifyMgrDrv_CmdArgs *) args;
    Int32                        status = ClientNotifyMgr_S_SUCCESS;
    Int32                        ret;
    ClientNotifyMgrDrv_CmdArgs   cargs;

    GT_3trace (curTrace, GT_ENTER, "ClientNotifyMgrDrv_ioctl",
               filp, cmd, args);

    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (ClientNotifyMgrDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    switch (cmd) {

    case CMD_CLIENTNOTIFYMGR_CREATE      :
    {
        ClientNotifyMgr_Params params;

        ret = copy_from_user (&params,
                              cargs.args.create.params,
                              sizeof (ClientNotifyMgr_Params));
        GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ClientNotifyMgrDrv_ioctl: copy_from_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for the name */
                if (cargs.args.create.nameLen > 0) {
                    params.name = Memory_alloc (NULL,
                                                cargs.args.create.nameLen,
                                                0,
                                                NULL);
                    GT_assert (curTrace, (params.name != NULL));
                    /* Copy the name */
                    ret = copy_from_user (params.name,
                                          cargs.args.create.params->name,
                                          cargs.args.create.nameLen);
                    GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (ret != 0) {
                        GT_1trace (curTrace,
                                  GT_1CLASS,
                                  "    ClientNotifyMgrDrv_ioctl: copy_from_user"
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
                params.sharedAddr = SharedRegion_getPtr (
                                             cargs.args.create.sharedAddrSrPtr);
                params.gate = cargs.args.create.knlLockHandle;

                cargs.args.create.handle = ClientNotifyMgr_create (&params);
                if (cargs.args.create.handle != NULL) {
                    cargs.args.create.Id =
                             ClientNotifyMgr_getId (cargs.args.create.handle);
                }
                GT_assert (curTrace, (cargs.args.create.handle != NULL));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (cargs.args.create.nameLen > 0) {
                Memory_free (NULL, params.name, cargs.args.create.nameLen);
            }
    }

    break;



    case CMD_CLIENTNOTIFYMGR_GETCONFIG       :
    {
        ClientNotifyMgr_Config config;

        ClientNotifyMgr_getConfig (&config);

        ret = copy_to_user (cargs.args.getConfig.config,
                            &config,
                            sizeof (ClientNotifyMgr_Config));
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

    case CMD_CLIENTNOTIFYMGR_SETUP       :
    {
        ClientNotifyMgr_Config config;

        ret = copy_from_user (&config,
                              cargs.args.setup.config,
                              sizeof (ClientNotifyMgr_Config));
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
            status = ClientNotifyMgr_setup (&config);
            GT_assert (curTrace, (status >= 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    break;

    case CMD_CLIENTNOTIFYMGR_DESTROY     :
    {
        status = ClientNotifyMgr_destroy ();
        GT_assert (curTrace, (status >= 0));
    }
    break;

    case CMD_CLIENTNOTIFYMGR_PARAMS_INIT :
    {
        ClientNotifyMgr_Params params;

        ClientNotifyMgr_Params_init (&params);

        ret = copy_to_user (cargs.args.ParamsInit.params,
                            &params,
                            sizeof (ClientNotifyMgr_Params));
        GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    ClientNotifyMgrDrv_ioctl: copy_to_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
    }
    break;

    case CMD_CLIENTNOTIFYMGR_DELETE      :
    {
        status = ClientNotifyMgr_delete ((ClientNotifyMgr_Handle *)
                                             &cargs.args.deleteInstance.handle);
        GT_assert (curTrace, (status >= 0));
    }
    break;

    case CMD_CLIENTNOTIFYMGR_CLOSE   :
    {
        status = ClientNotifyMgr_close (&cargs.args.close.handle);
        GT_assert (curTrace, (status >= 0));
    }
    break;

    case CMD_CLIENTNOTIFYMGR_SHAREDMEMREQ :
    {
        ClientNotifyMgr_Params params;

        ret = copy_from_user (&params,
                              cargs.args.sharedMemReq.params,
                              sizeof (ClientNotifyMgr_Params));
        GT_assert (curTrace, (ret == 0));

        cargs.args.sharedMemReq.bytes =
                    ClientNotifyMgr_sharedMemReq (cargs.args.sharedMemReq.params);
    }
    break;

    default :
    {
        /* FIXME */
    }
    break;
    }

    cargs.apiStatus = status;

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_ioctl",
               osStatus);

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (ClientNotifyMgrDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "ClientNotifyMgrDrv_ioctl",
               osStatus);

    /*! @retval 0 Operation successfully completed. */
    return (osStatus);
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
int __init ClientNotifyMgr_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("ProcMgr sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("ClientNotifyMgr_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        ClientNotifyMgr_enableTrace = simple_strtol (TRACE, NULL, 16);
        if ((ClientNotifyMgr_enableTrace != 0) && (ClientNotifyMgr_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (ClientNotifyMgr_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (ClientNotifyMgr_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        ClientNotifyMgr_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (ClientNotifyMgr_enableTraceEnter != 0)
            &&  (ClientNotifyMgr_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (ClientNotifyMgr_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        ClientNotifyMgr_enableTraceFailure = simple_strtol (TRACEFAILURE, NULL, 16);
        if (    (ClientNotifyMgr_enableTraceFailure != 0)
            &&  (ClientNotifyMgr_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (ClientNotifyMgr_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        ClientNotifyMgr_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (ClientNotifyMgr_enableTraceFailure != 1)
            &&  (ClientNotifyMgr_enableTraceFailure != 2)
            &&  (ClientNotifyMgr_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            ClientNotifyMgr_traceClass =
                            ClientNotifyMgr_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= ClientNotifyMgr_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    Osal_printf ("ClientNotifyMgr_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit ClientNotifyMgr_finalizeModule (void)
{
    Osal_printf ("ClientNotifyMgr_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (ClientNotifyMgr_initializeModule);
module_exit (ClientNotifyMgr_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
