/*
 *  @file   GateMPDrv.c
 *
 *  @brief      OS-specific implementation of GateMP driver for Linux
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
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/knl/OsalDriver.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Gate.h>

/* Module specific header files */
#include <ti/ipc/GateMP.h>
#include <ti/syslink/inc/knl/Linux/GateMPDrv.h>
#include <ti/syslink/inc/GateMPDrvDefs.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* GateMP functions */
EXPORT_SYMBOL(GateMP_getConfig);
EXPORT_SYMBOL(GateMP_setup);
EXPORT_SYMBOL(GateMP_destroy);
EXPORT_SYMBOL(GateMP_Params_init);
EXPORT_SYMBOL(GateMP_create);
EXPORT_SYMBOL(GateMP_delete);
EXPORT_SYMBOL(GateMP_open);
EXPORT_SYMBOL(GateMP_openByAddr);
EXPORT_SYMBOL(GateMP_close);
EXPORT_SYMBOL(GateMP_enter);
EXPORT_SYMBOL(GateMP_leave);
EXPORT_SYMBOL(GateMP_sharedMemReq);
EXPORT_SYMBOL(GateMP_getDefaultRemote);

/* GateMPDrv functions */
EXPORT_SYMBOL(GateMPDrv_registerDriver);
EXPORT_SYMBOL(GateMPDrv_unregisterDriver);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for GateMP.
 */
#define GATEMP_DRV_MINOR_NUMBER                       8


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int GateMPDrv_drvopen (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int GateMPDrv_drvclose (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long GateMPDrv_drvioctl (struct file *  filp,
                                unsigned int   cmd,
                                unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init GateMPDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit GateMPDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations GateMPDrv_driverOps = {
    open:    GateMPDrv_drvopen,
    release: GateMPDrv_drvclose,
    unlocked_ioctl:   GateMPDrv_drvioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool GateMPDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool GateMPDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool GateMPDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 GateMPDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for GateMP
 */
static Ptr GateMPDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the GateMP with OsalDriver
 */
Ptr
GateMPDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "GateMPDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("GateMP",
                                            &GateMPDrv_driverOps,
                                            GATEMP_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "GateMPDrv_registerDriver",
                             GateMP_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "GateMPDrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the GateMP with OsalDriver
 */
Void
GateMPDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "GateMPDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "GateMPDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
GateMPDrv_drvopen (struct inode * inode, struct file * filp)
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
GateMPDrv_drvclose (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long GateMPDrv_drvioctl (struct file *  filp,
                         unsigned int   cmd,
                         unsigned long  args)
{
    int                  osStatus = 0;
    GateMPDrv_CmdArgs *  cargs  = (GateMPDrv_CmdArgs *) args;
    Int32                status = GateMP_S_SUCCESS;
    Int32                ret;

    GT_3trace (curTrace, GT_ENTER, "GateMPDrv_drvioctl",
               filp, cmd, args);

    switch (cmd) {
        case CMD_GATEMP_ENTER:
        {
            cargs->args.enter.flags = GateMP_enter (cargs->args.enter.handle);
        }
        break;

        case CMD_GATEMP_LEAVE:
        {
            GateMP_leave (cargs->args.enter.handle,
                                cargs->args.enter.flags);
        }
        break;

        case CMD_GATEMP_GETCONFIG:
        {
            GateMP_Config config;

            GateMP_getConfig (&config);
            GT_assert (curTrace, (status >= 0));

            ret = copy_to_user (cargs->args.getConfig.config,
                                &config,
                                sizeof (GateMP_Config));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    GateMPDrv_drvioctl: copy_to_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_GATEMP_SETUP:
        {
            GateMP_Config config;

            ret = copy_from_user (&config,
                                  cargs->args.setup.config,
                                  sizeof (GateMP_Config));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    GateMPDrv_drvioctl: copy_from_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = GateMP_setup (&config);
                GT_assert (curTrace, (status >= 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_GATEMP_DESTROY:
        {
            status = GateMP_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_GATEMP_PARAMS_INIT:
        {
            GateMP_Params params;

            GateMP_Params_init (&params);
            GT_assert (curTrace, (status >= 0));

            ret = copy_to_user (cargs->args.ParamsInit.params,
                                &params,
                                sizeof (GateMP_Params));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    GateMPDrv_drvioctl: copy_to_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_GATEMP_CREATE:
        {
            GateMP_Params params;

            ret = copy_from_user (&params,
                                  cargs->args.create.params,
                                  sizeof (GateMP_Params));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    GateMPDrv_drvioctl: copy_from_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for the name */
                if (cargs->args.create.nameLen > 0) {
                    params.name = Memory_alloc (NULL,
                                                 cargs->args.create.nameLen,
                                                 0,
                                                 NULL);
                    GT_assert (curTrace, (params.name != NULL));
                    /* Copy the name */
                    ret = copy_from_user (params.name,
                                          cargs->args.create.params->name,
                                          cargs->args.create.nameLen);
                    GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (ret != 0) {
                        GT_1trace (curTrace,
                                  GT_1CLASS,
                                  "    GateMPDrv_drvioctl: copy_from_user"
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
				if (cargs->args.create.sharedAddrSrPtr ==
				    SharedRegion_INVALIDSRPTR) {
				    params.sharedAddr = NULL;
				}
				else  {
				    params.sharedAddr = SharedRegion_getPtr (
		                                        cargs->args.create.sharedAddrSrPtr);
				}
                cargs->args.create.handle = GateMP_create (&params);
                GT_assert (curTrace, (cargs->args.create.handle != NULL));

                /* Set failure status if create has failed. */
                if (cargs->args.create.handle == NULL) {
                    status = GateMP_E_FAIL;
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (cargs->args.create.nameLen > 0) {
                Memory_free (NULL, params.name, cargs->args.create.nameLen);
            }
        }
        break;

        case CMD_GATEMP_DELETE:
        {
            status = GateMP_delete ((GateMP_Handle *)
                                            &cargs->args.deleteInstance.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_GATEMP_OPEN:
        {
            GateMP_Handle handle;
            GateMP_Params params;
            params.name = NULL;

            /* Allocate memory for the name */
            if (cargs->args.open.nameLen > 0) {
                params.name = Memory_alloc (NULL,
                                            cargs->args.open.nameLen,
                                            0,
                                            NULL);
                GT_assert (curTrace, (params.name != NULL));
                /* Copy the name */
                ret = copy_from_user (params.name,
                                      cargs->args.open.name,
                                      cargs->args.open.nameLen);
                GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (ret != 0) {
                    GT_1trace (curTrace,
                              GT_1CLASS,
                              "    GateMPDrv_drvioctl: copy_from_user"
                              " call failed\n"
                              "        status [%d]",
                              ret);
                    osStatus = -EFAULT;
                }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)

            if (osStatus == 0) {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = GateMP_open (params.name, &handle);
                cargs->args.open.handle = handle;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (cargs->args.open.nameLen > 0) {
                Memory_free (NULL, params.name, cargs->args.open.nameLen);
            }
        }
        break;

        case CMD_GATEMP_OPENBYADDR:
        {
            GateMP_Handle handle;
            Ptr           sharedAddr = NULL;

            /* For openByAddr by name, the sharedAddrSrPtr may be invalid. */
            if (    cargs->args.openByAddr.sharedAddrSrPtr
                !=  SharedRegion_INVALIDSRPTR) {
                sharedAddr = SharedRegion_getPtr (
                                          cargs->args.openByAddr.sharedAddrSrPtr);
            }
            status = GateMP_openByAddr (sharedAddr, &handle);
            cargs->args.openByAddr.handle = handle;
        }
        break;

        case CMD_GATEMP_CLOSE:
        {
            status = GateMP_close ((GateMP_Handle *)
                                                &(cargs->args.close.handle));
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_GATEMP_SHAREDMEMREQ:
        {
            GateMP_Params params;

            ret = copy_from_user (&params,
                                  cargs->args.sharedMemReq.params,
                                  sizeof (GateMP_Params));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    GateMPDrv_drvioctl: copy_from_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                cargs->args.sharedMemReq.retVal =
                 GateMP_sharedMemReq (cargs->args.sharedMemReq.params);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_GATEMP_GETDEFAULTREMOTE:
        {
            GateMP_Handle handle;

            handle = GateMP_getDefaultRemote ();
            cargs->args.getDefaultRemote.handle = handle;
        }
        break;

        case CMD_GATEMP_SETRESERVED:
        {

            GateMP_setReserved (cargs->args.setReserved.remoteProtectType,
                                cargs->args.setReserved.lockNum);
        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = GateMP_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "GateMPDrv_drvioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs->apiStatus = status;

    GT_1trace (curTrace, GT_LEAVE, "GateMPDrv_drvioctl",
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
int __init GateMPDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("GateMP sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("GateMPDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        GateMPDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if (    (GateMPDrv_enableTrace != 0)
            &&  (GateMPDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (GateMPDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (GateMPDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        GateMPDrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (GateMPDrv_enableTraceEnter != 0)
            &&  (GateMPDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (GateMPDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        GateMPDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                            NULL,
                                                            16);
        if (    (GateMPDrv_enableTraceFailure != 0)
            &&  (GateMPDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (GateMPDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        GateMPDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (GateMPDrv_enableTraceFailure != 1)
            &&  (GateMPDrv_enableTraceFailure != 2)
            &&  (GateMPDrv_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            GateMPDrv_traceClass =
                       GateMPDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= GateMPDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    GateMPDrv_osalDrvHandle = GateMPDrv_registerDriver ();
    if (GateMPDrv_osalDrvHandle == NULL) {
        /*! @retval GateMP_E_OSFAILURE Failed to register GateMP
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "GateMPDrv_initializeModule",
                          GateMP_E_OSFAILURE,
                          "Failed to register GateMP driver with OS!");
    }

    Osal_printf ("GateMPDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit GateMPDrv_finalizeModule (void)
{
    Osal_printf ("Entered GateMPDrv_finalizeModule\n");

    GateMPDrv_unregisterDriver (&(GateMPDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving GateMPDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (GateMPDrv_initializeModule);
module_exit (GateMPDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
