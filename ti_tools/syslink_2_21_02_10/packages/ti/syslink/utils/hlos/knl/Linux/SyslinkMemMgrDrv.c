/*
 *  @file   SyslinkMemMgrDrv.c
 *
 *  @brief      OS-specific implementation of SyslinkMemMgr driver for Linux
 *
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
#include <ti/syslink/inc/SyslinkMemMgr_errBase.h>
#include <ti/syslink/utils/SyslinkMemMgr.h>
#include <ti/syslink/inc/SharedMemoryMgr.h>
#include <ti/syslink/inc/knl/Linux/SyslinkMemMgrDrv.h>
#include <ti/syslink/inc/SyslinkMemMgrDrvDefs.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* SyslinkMemMgr functions */
EXPORT_SYMBOL(SyslinkMemMgr_create);
EXPORT_SYMBOL(SyslinkMemMgr_delete);
EXPORT_SYMBOL(SyslinkMemMgr_alloc);
EXPORT_SYMBOL(SyslinkMemMgr_free);
EXPORT_SYMBOL(SyslinkMemMgr_map);
EXPORT_SYMBOL(SyslinkMemMgr_unmap);
EXPORT_SYMBOL(SyslinkMemMgr_translate);


/* SyslinkMemMgrDrv functions */
EXPORT_SYMBOL(SyslinkMemMgrDrv_registerDriver);
EXPORT_SYMBOL(SyslinkMemMgrDrv_unregisterDriver);
/* Implementation specific exports */
EXPORT_SYMBOL(SyslinkMemMgr_SharedMemory_Params_init);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for SyslinkMemMgr.
 */
#define MEMMGR_DRV_MINOR_NUMBER                     (13u)


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int SyslinkMemMgrDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int SyslinkMemMgrDrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long SyslinkMemMgrDrv_ioctl (struct file *  filp,
                                    unsigned int   cmd,
                                    unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init SyslinkMemMgrDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit SyslinkMemMgrDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations SyslinkMemMgrDrv_driverOps = {
    open:    SyslinkMemMgrDrv_open,
    release: SyslinkMemMgrDrv_close,
    unlocked_ioctl:   SyslinkMemMgrDrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool SyslinkMemMgrDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool SyslinkMemMgrDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool SyslinkMemMgrDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 SyslinkMemMgrDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for SyslinkMemMgr
 */
static Ptr SyslinkMemMgrDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the SyslinkMemMgr with OsalDriver
 */
Ptr
SyslinkMemMgrDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "SyslinkMemMgrDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("SyslinkMemMgr",
                                            &SyslinkMemMgrDrv_driverOps,
                                            MEMMGR_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgrDrv_registerDriver",
                             MEMMGR_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "SyslinkMemMgrDrv_registerDriver", osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the SyslinkMemMgr with OsalDriver
 */
Void
SyslinkMemMgrDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "SyslinkMemMgrDrv_unregisterDriver", drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "SyslinkMemMgrDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
SyslinkMemMgrDrv_open (struct inode * inode, struct file * filp)
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
SyslinkMemMgrDrv_close (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long SyslinkMemMgrDrv_ioctl (struct file *  filp,
                             unsigned int   cmd,
                             unsigned long  args)
{
    int                     osStatus = 0;
    Int32                   status = MEMMGR_SUCCESS;
    SyslinkMemMgrDrv_CmdArgs       *dstArgs  = (SyslinkMemMgrDrv_CmdArgs *) args;
    Int32                   ret;
    UInt32                  size;
    SyslinkMemMgrDrv_CmdArgs       cargs;

    GT_3trace (curTrace, GT_ENTER, "SyslinkMemMgrDrv_ioctl",
               filp, cmd, args);

    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (SyslinkMemMgrDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    switch (cmd) {

        case CMD_MEMMGR_CREATE:
        {
            Ptr     params = NULL;
            if (cargs.args.create.type == SyslinkMemMgr_TYPE_SHAREDMEM) {
                size = sizeof(SyslinkMemMgr_SharedMemory_Params);
                params = Memory_alloc (NULL,
                                       size,
                                       0,
                                       NULL);
                Memory_copy(params,
                            cargs.args.create.params,
                            size);
            }
            if (params != NULL) {
                cargs.args.create.handle = SyslinkMemMgr_create (params);
                GT_assert (curTrace, (cargs.args.create.handle != NULL));

                Memory_free (NULL, params, size);
            }
        }
        break;

        case CMD_MEMMGR_DELETE:
        {
            status = SyslinkMemMgr_delete (&cargs.args.deleteInstance.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        /* params init for ShMem implementation */
        case CMD_MEMMGR_SHAREDMEM_PARAMS_INIT  :
        {
            SyslinkMemMgr_SharedMemory_Params params;

            SyslinkMemMgr_SharedMemory_Params_init(&params);
            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (SyslinkMemMgr_SharedMemory_Params));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    SyslinkMemMgrDrv_ioctl: copy_to_user call"
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
            status = MEMMGR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SyslinkMemMgrDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs.apiStatus = status;

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (SyslinkMemMgrDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "SyslinkMemMgrDrv_ioctl", osStatus);

    /*! @retval 0 Operation successfully completed. */
    return osStatus;
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
int __init SyslinkMemMgrDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("SyslinkMemMgr sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("SyslinkMemMgrDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        SyslinkMemMgrDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if (    (SyslinkMemMgrDrv_enableTrace != 0)
            &&  (SyslinkMemMgrDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (SyslinkMemMgrDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (SyslinkMemMgrDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        SyslinkMemMgrDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (SyslinkMemMgrDrv_enableTraceEnter != 0)
            &&  (SyslinkMemMgrDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (SyslinkMemMgrDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        SyslinkMemMgrDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                          NULL,
                                                          16);
        if (    (SyslinkMemMgrDrv_enableTraceFailure != 0)
            &&  (SyslinkMemMgrDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (SyslinkMemMgrDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        SyslinkMemMgrDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (SyslinkMemMgrDrv_enableTraceFailure != 1)
            &&  (SyslinkMemMgrDrv_enableTraceFailure != 2)
            &&  (SyslinkMemMgrDrv_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            SyslinkMemMgrDrv_traceClass =
                         SyslinkMemMgrDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= SyslinkMemMgrDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    SyslinkMemMgrDrv_osalDrvHandle = SyslinkMemMgrDrv_registerDriver ();
    if (SyslinkMemMgrDrv_osalDrvHandle == NULL) {
        /*! @retval MEMMGR_E_OSFAILURE Failed to register SyslinkMemMgr
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SyslinkMemMgrDrv_initializeModule",
                             MEMMGR_E_OSFAILURE,
                             "Failed to register SyslinkMemMgr driver with OS!");
    }

    Osal_printf ("SyslinkMemMgrDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit SyslinkMemMgrDrv_finalizeModule (void)
{
    Osal_printf ("Entered SyslinkMemMgrDrv_finalizeModule\n");

    SyslinkMemMgrDrv_unregisterDriver (&(SyslinkMemMgrDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving SyslinkMemMgrDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (SyslinkMemMgrDrv_initializeModule);
module_exit (SyslinkMemMgrDrv_finalizeModule);

#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
