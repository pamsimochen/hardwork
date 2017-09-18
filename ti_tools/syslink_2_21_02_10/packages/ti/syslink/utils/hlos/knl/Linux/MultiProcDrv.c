/*
 *  @file   MultiProcDrv.c
 *
 *  @brief     Driver for MultiProc on HLOS side
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
#include <ti/ipc/MultiProc.h>
#include <ti/syslink/inc/knl/Linux/MultiProcDrv.h>
#include <ti/syslink/inc/MultiProcDrvDefs.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* MultiProc functions */

/* MultiProcDrv functions */
EXPORT_SYMBOL(MultiProcDrv_registerDriver);
EXPORT_SYMBOL(MultiProcDrv_unregisterDriver);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for MultiProc.
 */
#define MULTIPROC_DRV_MINOR_NUMBER 10


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int MultiProcDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int MultiProcDrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long MultiProcDrv_ioctl (struct file *  filp,
                                unsigned int   cmd,
                                unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init MultiProcDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit MultiProcDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations MultiProcDrv_driverOps = {
    open:    MultiProcDrv_open,
    release: MultiProcDrv_close,
    unlocked_ioctl:   MultiProcDrv_ioctl,


} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool MultiProcDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool MultiProcDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool MultiProcDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 MultiProcDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for MultiProc
 */
static Ptr MultiProcDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the MultiProc with OsalDriver
 */
Ptr
MultiProcDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "MultiProcDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("MultiProc",
                                            &MultiProcDrv_driverOps,
                                            MULTIPROC_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProcDrv_registerDriver",
                             MultiProc_E_FAIL,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MultiProcDrv_registerDriver", osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the MultiProc with OsalDriver
 */
Void
MultiProcDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "MultiProcDrv_unregisterDriver", drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "MultiProcDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
MultiProcDrv_open (struct inode * inode, struct file * filp)
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
MultiProcDrv_close (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long MultiProcDrv_ioctl (struct file *  filp,
                         unsigned int   cmd,
                         unsigned long  args)
{
    int                     osStatus = 0;
    Int32                   status   = MultiProc_S_SUCCESS;
    MultiProcDrv_CmdArgs *  cargs    = (MultiProcDrv_CmdArgs *) args;
    Int32                   ret;

    GT_3trace (curTrace, GT_ENTER, "MultiProcDrv_ioctl",
               filp, cmd, args);

    switch (cmd) {

        case CMD_MULTIPROC_SETUP:
        {
            MultiProc_Config config;

            ret = copy_from_user (&config,
                                  cargs->args.setup.config,
                                  sizeof (MultiProc_Config));
            GT_assert (curTrace, (ret == 0));

            status = MultiProc_setup (&config);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_MULTIPROC_DESTROY:
        {
            status = MultiProc_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;
        case CMD_MULTIPROC_GETCONFIG:
        {
            MultiProc_Config config;

            MultiProc_getConfig (&config);
            GT_assert (curTrace, (status >= 0));

            ret = copy_to_user (cargs->args.getConfig.config,
                                &config,
                                sizeof (MultiProc_Config));
            GT_assert (curTrace, (ret == 0));
        }
        break;
        case CMD_MULTIPROC_SETLOCALID:
        {
            status = MultiProc_setLocalId(cargs->args.setLocalId.id);
        }
        break;
        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = MultiProc_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MultiProcDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;

    }

    cargs->apiStatus = status;

    GT_1trace (curTrace, GT_LEAVE, "MultiProcDrv_ioctl", osStatus);

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
int __init MultiProcDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("MultiProc sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("MultiProcDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        MultiProcDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if (    (MultiProcDrv_enableTrace != 0)
            &&  (MultiProcDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (MultiProcDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (MultiProcDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        MultiProcDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (MultiProcDrv_enableTraceEnter != 0)
            &&  (MultiProcDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (MultiProcDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        MultiProcDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                          NULL,
                                                          16);
        if (    (MultiProcDrv_enableTraceFailure != 0)
            &&  (MultiProcDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (MultiProcDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        MultiProcDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (MultiProcDrv_enableTraceFailure != 1)
            &&  (MultiProcDrv_enableTraceFailure != 2)
            &&  (MultiProcDrv_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            MultiProcDrv_traceClass =
                         MultiProcDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= MultiProcDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    MultiProcDrv_osalDrvHandle = MultiProcDrv_registerDriver ();
    if (MultiProcDrv_osalDrvHandle == NULL) {
        /*! @retval NAMESERVER_E_OSFAILURE Failed to register MultiProc
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MultiProcDrv_initializeModule",
                             NAMESERVER_E_OSFAILURE,
                             "Failed to register MultiProc driver with OS!");
    }

    Osal_printf ("MultiProcDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit MultiProcDrv_finalizeModule (void)
{
    Osal_printf ("Entered MultiProcDrv_finalizeModule\n");

    MultiProcDrv_unregisterDriver (&(MultiProcDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving MultiProcDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (MultiProcDrv_initializeModule);
module_exit (MultiProcDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
