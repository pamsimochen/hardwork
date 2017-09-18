/*
 *  @file   HeapMemMPDrv.c
 *
 *  @brief      OS-specific implementation of HeapMemMP driver for Linux
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
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <linux/mman.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/io.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/knl/OsalDriver.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>

/* Module specific header files */
#include <ti/ipc/HeapMemMP.h>
#include <ti/syslink/inc/knl/Linux/HeapMemMPDrv.h>
#include <ti/syslink/inc/HeapMemMPDrvDefs.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* HeapMemMP functions */
EXPORT_SYMBOL(HeapMemMP_getConfig);
EXPORT_SYMBOL(HeapMemMP_setup);
EXPORT_SYMBOL(HeapMemMP_destroy);
EXPORT_SYMBOL(HeapMemMP_Params_init);
EXPORT_SYMBOL(HeapMemMP_create);
EXPORT_SYMBOL(HeapMemMP_delete);
EXPORT_SYMBOL(HeapMemMP_open);
EXPORT_SYMBOL(HeapMemMP_openByAddr);
EXPORT_SYMBOL(HeapMemMP_close);
EXPORT_SYMBOL(HeapMemMP_alloc);
EXPORT_SYMBOL(HeapMemMP_free);
EXPORT_SYMBOL(HeapMemMP_getStats);
EXPORT_SYMBOL(HeapMemMP_getExtendedStats);
EXPORT_SYMBOL(HeapMemMP_sharedMemReq);
EXPORT_SYMBOL(HeapMemMP_restore);

/* HeapMemMPDrv functions */
EXPORT_SYMBOL(HeapMemMPDrv_registerDriver);
EXPORT_SYMBOL(HeapMemMPDrv_unregisterDriver);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for HeapMemMP.
 */
#define HEAPMEMMP_DRV_MINOR_NUMBER     5

/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/* Linux driver function to open the driver object. */
static int HeapMemMPDrv_open (struct inode * inode, struct file * filp);

/* Linux driver function to close the driver object. */
static int HeapMemMPDrv_close (struct inode * inode, struct file * filp);

/* Linux driver function to ioctl of the driver object. */
static long HeapMemMPDrv_ioctl (struct file *  filp,
                                unsigned int   cmd,
                                unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/* Module initialization function for Linux driver. */
static int __init HeapMemMPDrv_initializeModule (void);

/* Module finalization  function for Linux driver. */
static void  __exit HeapMemMPDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations HeapMemMPDrv_driverOps = {
    open:    HeapMemMPDrv_open,
    release: HeapMemMPDrv_close,
    unlocked_ioctl:   HeapMemMPDrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool HeapMemMPDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool HeapMemMPDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool HeapMemMPDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 HeapMemMPDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for HeapMemMP
 */
static Ptr HeapMemMPDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the HeapMemMP with OsalDriver
 */
Ptr
HeapMemMPDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "HeapMemMPDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("HeapMemMP",
                                            &HeapMemMPDrv_driverOps,
                                            HEAPMEMMP_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        /*! @retval NULL OsalDriver_registerDriver failed */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapMemMPDrv_registerDriver",
                             HeapMemMP_E_FAIL,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMPDrv_registerDriver",
               osalHandle);

    /*! @retval Valid-Handle Operation successfully completed. */
    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the HeapMemMP with OsalDriver
 */
Void
HeapMemMPDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "HeapMemMPDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "HeapMemMPDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
HeapMemMPDrv_open (struct inode * inode, struct file * filp)
{
    OsalDriver_Handle handle = container_of (inode->i_cdev,
                                             OsalDriver_Object,
                                             cdev);
    filp->private_data = handle;

    /*! @retval 0 Operation successfully completed. */
    return 0;
}


/*!
 *  @brief  Linux specific function to close the driver.
 */
int
HeapMemMPDrv_close (struct inode * inode, struct file * filp)
{
    /*! @retval 0 Operation successfully completed. */
    return 0;
}


/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long
HeapMemMPDrv_ioctl (struct file *  filp,
                    unsigned int   cmd,
                    unsigned long  args)
{
    int                  osStatus = 0;
    HeapMemMPDrv_CmdArgs * dstArgs  = (HeapMemMPDrv_CmdArgs *) args;
    Int32                status = HeapMemMP_S_SUCCESS;
    Int32                ret;
    HeapMemMPDrv_CmdArgs   cargs;

    GT_3trace (curTrace, GT_ENTER, "HeapMemMPDrv_ioctl",
               filp, cmd, args);


    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (HeapMemMPDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    switch (cmd) {
        case CMD_HEAPMEMMP_ALLOC:
        {
            Char *             block;
            SharedRegion_SRPtr blockSrPtr = SharedRegion_INVALIDSRPTR;
            UInt16              index;
            block = HeapMemMP_alloc (cargs.args.alloc.handle,
                                     cargs.args.alloc.size,
                                     cargs.args.alloc.align);
            if (block != NULL) {
                index = SharedRegion_getId (block);
                GT_assert (curTrace, (index != SharedRegion_INVALIDREGIONID));
                blockSrPtr = SharedRegion_getSRPtr (block, index);
                GT_assert (curTrace, (blockSrPtr != SharedRegion_INVALIDSRPTR));
            }

            cargs.args.alloc.blockSrPtr = blockSrPtr;
        }
        break;

        case CMD_HEAPMEMMP_FREE:
        {
            Char * block;
            block = SharedRegion_getPtr (cargs.args.free.blockSrPtr);
            GT_assert (curTrace, (block != NULL));
            HeapMemMP_free (cargs.args.free.handle,
                            block,
                            cargs.args.free.size);
        }
        break;

        case CMD_HEAPMEMMP_PARAMS_INIT:
        {
            HeapMemMP_Params params;

            HeapMemMP_Params_init (&params);

            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (HeapMemMP_Params));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_HEAPMEMMP_CREATE:
        {
            HeapMemMP_Params params;

            ret = copy_from_user (&params,
                                  cargs.args.create.params,
                                  sizeof (HeapMemMP_Params));
            GT_assert (curTrace, (ret == 0));

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
            }

            params.sharedAddr = SharedRegion_getPtr (
                                             cargs.args.create.sharedAddrSrPtr);
            GT_1trace (curTrace,
                       GT_2CLASS,
                       "    HeapMemMP_create knl ioctl: Shared addr [0x%x]\n",
                       params.sharedAddr);


            /* Update gate in params. */
            params.gate = cargs.args.create.knlGate;

            cargs.args.create.handle = HeapMemMP_create (&params);
            GT_assert (curTrace, (cargs.args.create.handle != NULL));

            /* Set failure status if create has failed. */
            if (cargs.args.create.handle == NULL) {
                status = HeapMemMP_E_FAIL;
            }

            if (cargs.args.create.nameLen > 0) {
                Memory_free (NULL, params.name, cargs.args.create.nameLen);
            }
        }
        break;

        case CMD_HEAPMEMMP_DELETE:
        {
            status = HeapMemMP_delete ((HeapMemMP_Handle *)
                                            &cargs.args.deleteInstance.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_HEAPMEMMP_OPEN:
        {
            String           name = NULL;
            HeapMemMP_Handle handle;


            /* Allocate memory for the name */
            if (cargs.args.open.nameLen > 0) {
                name = Memory_alloc (NULL,
                                     cargs.args.open.nameLen,
                                     0,
                                     NULL);
                GT_assert (curTrace, (name != NULL));
                /* Copy the name */
                ret = copy_from_user (name,
                                      cargs.args.open.name,
                                      cargs.args.open.nameLen);
                GT_assert (curTrace, (ret == 0));
            }

            status = HeapMemMP_open (name, &handle);
            /* HeapMemMP_open can fail at run-time, so don't assert on its
             * failure.
             */
            cargs.args.open.handle = handle;

            if (cargs.args.open.nameLen > 0) {
                Memory_free (NULL, name, cargs.args.open.nameLen);
            }
        }
        break;

        case CMD_HEAPMEMMP_OPENBYADDR:
        {
            HeapMemMP_Handle handle;
            Ptr              sharedAddr;

            sharedAddr = SharedRegion_getPtr (cargs.args.openByAddr.sharedAddrSrPtr);

            status = HeapMemMP_openByAddr (sharedAddr, &handle);
            /* HeapMemMP_open can fail at run-time, so don't assert on its
             * failure.
             */
            cargs.args.openByAddr.handle = handle;

        }
        break;

        case CMD_HEAPMEMMP_CLOSE:
        {
            status = HeapMemMP_close ((HeapMemMP_Handle *)
                                                &(cargs.args.close.handle));
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_HEAPMEMMP_SHAREDMEMREQ:
        {
            HeapMemMP_Params params;

            ret = copy_from_user (&params,
                                  cargs.args.sharedMemReq.params,
                                  sizeof (HeapMemMP_Params));
            GT_assert (curTrace, (ret == 0));

            if (params.sharedAddr != NULL) {
                params.sharedAddr = SharedRegion_getPtr (
                                             cargs.args.create.sharedAddrSrPtr);
            }

            cargs.args.sharedMemReq.bytes =
                       HeapMemMP_sharedMemReq (&params);
        }
        break;

        case CMD_HEAPMEMMP_GETCONFIG:
        {
            HeapMemMP_Config config;

            HeapMemMP_getConfig (&config);

            ret = copy_to_user (cargs.args.getConfig.config,
                                &config,
                                sizeof (HeapMemMP_Config));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_HEAPMEMMP_SETUP:
        {
            HeapMemMP_Config config;

            ret = copy_from_user (&config,
                                  cargs.args.getConfig.config,
                                  sizeof (HeapMemMP_Config));
            GT_assert (curTrace, (ret == 0));

            status = HeapMemMP_setup (&config);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_HEAPMEMMP_DESTROY:
        {
            status = HeapMemMP_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_HEAPMEMMP_GETSTATS:
        {
            Memory_Stats  stats;

            HeapMemMP_getStats (cargs.args.getStats.handle,
                                (Ptr)&stats);

            ret = copy_to_user (cargs.args.getStats.stats,
                                &stats,
                                sizeof (Memory_Stats));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_HEAPMEMMP_GETEXTENDEDSTATS:
        {
            HeapMemMP_ExtendedStats stats;

            HeapMemMP_getExtendedStats (
                                       cargs.args.getExtendedStats.handle,
                                       (Ptr)&stats);

            ret = copy_to_user (cargs.args.getExtendedStats.stats,
                                &stats,
                                sizeof (HeapMemMP_ExtendedStats));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_HEAPMEMMP_RESTORE:
        {

            HeapMemMP_restore (cargs.args.restore.handle);
        }
        break;

        default :
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = HeapMemMP_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapMemMPDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs.apiStatus = status;

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (HeapMemMPDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "HeapMemMPDrv_ioctl",
               osStatus);

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
int __init HeapMemMPDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("HeapMemMP module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("HeapMemMPDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        HeapMemMPDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if ((HeapMemMPDrv_enableTrace != 0) && (HeapMemMPDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (HeapMemMPDrv_enableTrace == TRUE) {
            Osal_printf ("Trace enabled\n");
            curTrace = GT_TraceState_Enable;
        }
        else if (HeapMemMPDrv_enableTrace == FALSE) {
            Osal_printf ("Trace disabled\n");
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        HeapMemMPDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (HeapMemMPDrv_enableTraceEnter != 0)
            &&  (HeapMemMPDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (HeapMemMPDrv_enableTraceEnter == TRUE) {
            Osal_printf ("Trace entry/leave prints enabled\n");
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        HeapMemMPDrv_enableTraceFailure = simple_strtol (TRACEFAILURE, NULL, 16);
        if (    (HeapMemMPDrv_enableTraceFailure != 0)
            &&  (HeapMemMPDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n");
        }
        else if (HeapMemMPDrv_enableTraceFailure == TRUE) {
            Osal_printf ("Trace SetFailureReason enabled\n");
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        HeapMemMPDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (HeapMemMPDrv_traceClass != 1)
            &&  (HeapMemMPDrv_traceClass != 2)
            &&  (HeapMemMPDrv_traceClass != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n");
        }
        else {
            Osal_printf ("Trace class %s\n", TRACECLASS);
            HeapMemMPDrv_traceClass =
                            HeapMemMPDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= HeapMemMPDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    HeapMemMPDrv_osalDrvHandle = HeapMemMPDrv_registerDriver ();
    if (HeapMemMPDrv_osalDrvHandle == NULL) {
        /*! @retval HeapMemMP_E_OSFAILURE Failed to register HeapMemMP
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "HeapMemMPDrv_initializeModule",
                          HeapMemMP_E_OSFAILURE,
                          "Failed to register HeapMemMP driver with OS!");
    }

    Osal_printf ("HeapMemMPDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit HeapMemMPDrv_finalizeModule (void)
{
    Osal_printf ("HeapMemMPDrv_finalizeModule\n");
    Osal_printf ("Entered HeapMemMPDrv_finalizeModule\n");

    HeapMemMPDrv_unregisterDriver (&(HeapMemMPDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving HeapMemMPDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (HeapMemMPDrv_initializeModule);
module_exit (HeapMemMPDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
