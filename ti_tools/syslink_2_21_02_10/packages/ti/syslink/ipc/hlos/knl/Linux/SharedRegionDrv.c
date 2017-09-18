/*
 *  @file   SharedRegionDrv.c
 *
 *  @brief      OS-specific implementation of SharedRegion driver for Linux
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
#include <ti/ipc/MultiProc.h>

/* Linux specific header files */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>


/* Module specific header files */
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/knl/Linux/SharedRegionDrv.h>
#include <ti/syslink/inc/SharedRegionDrvDefs.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* SharedRegion functions */
EXPORT_SYMBOL(SharedRegion_getConfig);
EXPORT_SYMBOL(SharedRegion_setup);
EXPORT_SYMBOL(SharedRegion_destroy);
EXPORT_SYMBOL(SharedRegion_start);
EXPORT_SYMBOL(SharedRegion_stop);
EXPORT_SYMBOL(SharedRegion_setEntry);
EXPORT_SYMBOL(SharedRegion_getId);
EXPORT_SYMBOL(SharedRegion_getIdByName);
EXPORT_SYMBOL(SharedRegion_getNumRegions);
EXPORT_SYMBOL(SharedRegion_getCacheLineSize);
EXPORT_SYMBOL(SharedRegion_getPtr);
EXPORT_SYMBOL(SharedRegion_getSRPtr);
EXPORT_SYMBOL(SharedRegion_getHeap);
EXPORT_SYMBOL(SharedRegion_getEntry);
EXPORT_SYMBOL(SharedRegion_clearEntry);
EXPORT_SYMBOL(SharedRegion_isCacheEnabled);
EXPORT_SYMBOL(SharedRegion_translateEnabled);
EXPORT_SYMBOL(SharedRegion_getRegionInfo);

/* SharedRegionDrv functions */
EXPORT_SYMBOL(SharedRegionDrv_registerDriver);
EXPORT_SYMBOL(SharedRegionDrv_unregisterDriver);


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for SharedRegion.
 */
#define SHAREDREGION_DRV_MINOR_NUMBER 3


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int SharedRegionDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int SharedRegionDrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long SharedRegionDrv_ioctl (struct file *  filp,
                                   unsigned int   cmd,
                                   unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init SharedRegionDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit SharedRegionDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations SharedRegionDrv_driverOps = {
    open:    SharedRegionDrv_open,
    release: SharedRegionDrv_close,
    unlocked_ioctl:   SharedRegionDrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool SharedRegionDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool SharedRegionDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool SharedRegionDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 SharedRegionDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for SharedRegion
 */
static Ptr SharedRegionDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the SharedRegion with OsalDriver
 */
Ptr
SharedRegionDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "SharedRegionDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("SharedRegion",
                                            &SharedRegionDrv_driverOps,
                                            SHAREDREGION_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "SharedRegionDrv_registerDriver",
                             SharedRegion_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "SharedRegionDrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the SharedRegion with OsalDriver
 */
Void
SharedRegionDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "SharedRegionDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "SharedRegionDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
SharedRegionDrv_open (struct inode * inode, struct file * filp)
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
SharedRegionDrv_close (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long SharedRegionDrv_ioctl (struct file *  filp,
                            unsigned int   cmd,
                            unsigned long  args)
{
    int                       osStatus = 0;
    SharedRegionDrv_CmdArgs * cargs  = (SharedRegionDrv_CmdArgs *) args;
    Int32                     status = SharedRegion_S_SUCCESS;
    Int32                     ret;

    GT_3trace (curTrace, GT_ENTER, "SharedRegionDrv_ioctl",
               filp, cmd, args);

    switch (cmd) {
        case CMD_SHAREDREGION_GETCONFIG:
        {
            SharedRegion_Config config;

            SharedRegion_getConfig (&config);

            ret = copy_to_user (cargs->args.getConfig.config,
                                &config,
                                sizeof (SharedRegion_Config));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_SHAREDREGION_SETUP:
        {
            SharedRegion_Config   config;
            SharedRegion_Region   region;
            UInt16              i;

            ret = copy_from_user (&config,
                                  cargs->args.setup.config,
                                  sizeof (SharedRegion_Config));
            GT_assert (curTrace, (ret == 0));

            status = SharedRegion_setup (&config);
            GT_assert (curTrace, (status >= 0));

            for (i = 0; i < config.numEntries; i++) {

                SharedRegion_getRegionInfo (i, &region);

                if (region.entry.isValid == TRUE) {
                    /* MK: If createHeap==FALSE, pass the base unaltered
                     *     since it is the Physical address already.
                     */
                    if (region.entry.createHeap == TRUE) {
                        /* Otherwise convert the kernel virtual address to
                         * physical address */
                        region.entry.base = MemoryOS_translate (
                                               (Ptr)region.entry.base,
                                               Memory_XltFlags_Virt2Phys);
                    }
                    GT_assert (curTrace, (region.entry.base != NULL));

                    ret = copy_to_user (&(cargs->args.setup.regions [i]),
                                        &region,
                                        sizeof (SharedRegion_Region));
                    GT_assert (curTrace, (ret == 0));
                }
            }
        }
        break;

        /* Case to  return the shared region information to user space process
         */
        case CMD_SHAREDREGION_GETREGIONINFO:
        {
            SharedRegion_Config   config;
            SharedRegion_Region   region;
            UInt16              i;

            SharedRegion_getConfig (&config);

            for (i = 0; i < config.numEntries; i++) {
                SharedRegion_getRegionInfo (i, &region);
                if (region.entry.isValid == TRUE) {
                    /* Convert the kernel virtual address to physical
                     * addresses */
                    /* MK: If createHeap==FALSE, pass the base unaltered
                     *     since it is the Physical address already.
                     */
                    if (region.entry.createHeap == TRUE) {
                        region.entry.base = MemoryOS_translate (
                                               (Ptr)region.entry.base,
                                               Memory_XltFlags_Virt2Phys);
                    }
                    GT_assert (curTrace, (region.entry.base != NULL));

                    ret = copy_to_user (&(cargs->args.setup.regions [i]),
                                        &region,
                                        sizeof (SharedRegion_Region));
                    GT_assert (curTrace, (ret == 0));
                }
                else {
                    region.entry.base = NULL;
                    ret = copy_to_user (&(cargs->args.setup.regions [i]),
                                        &region,
                                        sizeof (SharedRegion_Region));
                    GT_assert (curTrace, (ret == 0));
                }
            }
        }
        break;

        case CMD_SHAREDREGION_DESTROY:
        {
            status = SharedRegion_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_SHAREDREGION_START:
        {
            status = SharedRegion_start ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_SHAREDREGION_STOP:
        {
            status = SharedRegion_stop ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_SHAREDREGION_ATTACH:
        {
            status = SharedRegion_attach (cargs->args.attach.remoteProcId);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_SHAREDREGION_DETACH:
        {
            status = SharedRegion_detach (cargs->args.attach.remoteProcId);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_SHAREDREGION_SETENTRY:
        {
            SharedRegion_Entry entry;
            ret = copy_from_user (&entry,
                                  &(cargs->args.setEntry.entry),
                                  sizeof (SharedRegion_Entry));

            /*
             * if there is no Heap in the SR, base gets set to Physical Addr,
             * otherwise assign base its Master Kernel virtual address.
             */
	    if (cargs->args.setEntry.entry.createHeap == TRUE) {
                entry.base = (Ptr)Memory_translate (
                        (Ptr)cargs->args.setEntry.entry.base,
                        Memory_XltFlags_Phys2Virt);
            } else {
                entry.base = (Ptr)cargs->args.setEntry.entry.base;
            }

            GT_assert (curTrace, (entry.base != (UInt32)NULL));
            status = SharedRegion_setEntry (cargs->args.setEntry.id,
                                            &entry);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_SHAREDREGION_CLEARENTRY:
        {
            status = SharedRegion_clearEntry (cargs->args.clearEntry.id);
        }
        break;

        case CMD_SHAREDREGION_GETHEAP:
        {
            IHeap_Handle  heapHandle = NULL;
            heapHandle = (IHeap_Handle) SharedRegion_getHeap (
                                                    cargs->args.getHeap.id);
            GT_assert (curTrace, (heapHandle != NULL));
            ret = copy_to_user (&(cargs->args.getHeap.heapHandle),
                                &heapHandle,
                                sizeof (IHeap_Handle));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_SHAREDREGION_RESERVEMEMORY:
        {
            /* Ignore the return value. */
            SharedRegion_reserveMemory (cargs->args.reserveMemory.id,
                                        cargs->args.reserveMemory.size);
        }
        break;

        case CMD_SHAREDREGION_CLEARRESERVEDMEMORY:
        {
            /* Ignore the return value. */
            SharedRegion_clearReservedMemory ();
        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = SharedRegion_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "SharedRegionDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs->apiStatus = status;

    GT_1trace (curTrace, GT_LEAVE, "SharedRegionDrv_ioctl",
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
int __init SharedRegionDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("SharedRegion sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("SharedRegionDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        SharedRegionDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if (    (SharedRegionDrv_enableTrace != 0)
            &&  (SharedRegionDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (SharedRegionDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (SharedRegionDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        SharedRegionDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (SharedRegionDrv_enableTraceEnter != 0)
            &&  (SharedRegionDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (SharedRegionDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        SharedRegionDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                            NULL,
                                                            16);
        if (    (SharedRegionDrv_enableTraceFailure != 0)
            &&  (SharedRegionDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (SharedRegionDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        SharedRegionDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (SharedRegionDrv_enableTraceFailure != 1)
            &&  (SharedRegionDrv_enableTraceFailure != 2)
            &&  (SharedRegionDrv_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            SharedRegionDrv_traceClass =
                       SharedRegionDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= SharedRegionDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    SharedRegionDrv_osalDrvHandle = SharedRegionDrv_registerDriver ();
    if (SharedRegionDrv_osalDrvHandle == NULL) {
        /*! @retval SharedRegion_E_OSFAILURE Failed to register SharedRegion
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "SharedRegionDrv_initializeModule",
                          SharedRegion_E_OSFAILURE,
                          "Failed to register SharedRegion driver with OS!");
    }

    Osal_printf ("SharedRegionDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit SharedRegionDrv_finalizeModule (void)
{
    Osal_printf ("Entered SharedRegionDrv_finalizeModule\n");

    SharedRegionDrv_unregisterDriver (&(SharedRegionDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving SharedRegionDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (SharedRegionDrv_initializeModule);
module_exit (SharedRegionDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
