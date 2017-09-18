/*
 *  @file   RingIOShmDrv.c
 *
 *  @brief      TBD
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
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <asm/io.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
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
#include <ti/syslink/RingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/knl/Linux/RingIOShmDrv.h>
#include <ti/syslink/inc/RingIOShmDrvDefs.h>
#include <ti/ipc/SharedRegion.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* RingIOShm functions */
EXPORT_SYMBOL(RingIOShm_Params_init);
EXPORT_SYMBOL(RingIOShm_setup);
EXPORT_SYMBOL(RingIOShm_destroy);
EXPORT_SYMBOL(RingIOShm_getConfig);

/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for RingIO.
 */
#define RINGIOSHM_DRV_MINOR_NUMBER 35

/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int RingIOShmDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int RingIOShmDrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long RingIOShmDrv_ioctl ( struct file *  filp,
                              unsigned int   cmd,
                              unsigned long  args);

#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init RingIOShmDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit RingIOShmDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations RingIOShmDrv_driverOps = {
    open:    RingIOShmDrv_open,
    release: RingIOShmDrv_close,
    unlocked_ioctl:   RingIOShmDrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool RingIOShmDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool RingIOShmDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool RingIOShmDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 RingIOShmDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for RingIOShm
 */
static Handle RingIOShmDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the RingIOShm with OsalDriver
 */
Ptr
RingIOShmDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "RingIOShmDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("RingIOShm",
                                            &RingIOShmDrv_driverOps,
                                            RINGIOSHM_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIOShmDrv_registerDriver",
                             RingIOShm_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIOShmDrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the RingIOShm with OsalDriver
 */
Void
RingIOShmDrv_unregisterDriver (Ptr    * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "RingIOShmDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "RingIOShmDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
RingIOShmDrv_open (struct inode * inode, struct file * filp)
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
RingIOShmDrv_close (struct inode * inode, struct file * filp)
{
    return 0;
}
/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long RingIOShmDrv_ioctl ( struct file *  filp,
                       unsigned int   cmd,
                       unsigned long  args)
{
    int                 osStatus = 0;
    RingIOShmDrv_CmdArgs * dstArgs  = (RingIOShmDrv_CmdArgs *) args;
    Int32               status   = RingIOShm_S_SUCCESS;
    Int32               ret;
    RingIOShmDrv_CmdArgs   cargs;

    GT_3trace (curTrace, GT_ENTER, "RingIOShmDrv_ioctl",
               filp, cmd, args);

    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (RingIOShmDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    switch (cmd) {
        case CMD_RINGIOSHM_PARAMS_INIT :
        {
            RingIOShm_Params params;

            RingIOShm_Params_init (&params);
            GT_assert (curTrace, (status >= 0));

            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (RingIOShm_Params));
            GT_assert (curTrace, (ret == 0));
        }
        break;
        case CMD_RINGIOSHM_GETCONFIG:
        {
            RingIOShm_Config config;

            RingIOShm_getConfig (&config);

            ret = copy_to_user (cargs.args.getConfig.config,
                                &config,
                                sizeof (RingIOShm_Config));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_RINGIOSHM_SETUP:
        {
            RingIOShm_Config config;

            ret = copy_from_user (&config,
                                  cargs.args.setup.config,
                                  sizeof (RingIOShm_Config));
            GT_assert (curTrace, (ret == 0));

            status = RingIOShm_setup (&config);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_RINGIOSHM_DESTROY:
        {
            status = RingIOShm_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;
        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = RingIOShm_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIOShmDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs.apiStatus = status;

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (RingIOShmDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "RingIOShmDrv_ioctl",
               osStatus);

    /*! @retval 0 Operation successfully completed. */
    return osStatus;
}


