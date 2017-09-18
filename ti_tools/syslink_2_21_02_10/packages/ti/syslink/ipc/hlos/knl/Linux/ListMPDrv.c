/*
 *  @file   ListMPDrv.c
 *
 *  @brief      OS-specific implementation of ListMP driver for
 *              Linux
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

/* Module specific header files */
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/_ListMP.h>
#include <ti/ipc/ListMP.h>
#include <ti/syslink/inc/knl/Linux/ListMPDrv.h>
#include <ti/syslink/inc/ListMPDrvDefs.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* ListMP functions */
EXPORT_SYMBOL(ListMP_getConfig);
EXPORT_SYMBOL(ListMP_setup);
EXPORT_SYMBOL(ListMP_destroy);
EXPORT_SYMBOL(ListMP_Params_init);
EXPORT_SYMBOL(ListMP_create);
EXPORT_SYMBOL(ListMP_delete);
EXPORT_SYMBOL(ListMP_open);
EXPORT_SYMBOL(ListMP_openByAddr);
EXPORT_SYMBOL(ListMP_close);
EXPORT_SYMBOL(ListMP_empty);
//EXPORT_SYMBOL(ListMP_getGate);
EXPORT_SYMBOL(ListMP_getHead);
EXPORT_SYMBOL(ListMP_getTail);
EXPORT_SYMBOL(ListMP_putHead);
EXPORT_SYMBOL(ListMP_putTail);
EXPORT_SYMBOL(ListMP_insert);
EXPORT_SYMBOL(ListMP_remove);
EXPORT_SYMBOL(ListMP_next);
EXPORT_SYMBOL(ListMP_prev);
EXPORT_SYMBOL(ListMP_sharedMemReq);


/* ListMPDrv functions */
EXPORT_SYMBOL(ListMPDrv_registerDriver);
EXPORT_SYMBOL(ListMPDrv_unregisterDriver);


#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for ListMP.
 */
#define LISTMP_DRV_MINOR_NUMBER 7


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int ListMPDrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int ListMPDrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long ListMPDrv_ioctl (struct file *  filp,
                             unsigned int   cmd,
                             unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init ListMPDrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit ListMPDrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations ListMPDrv_driverOps = {
    open:    ListMPDrv_open,
    release: ListMPDrv_close,
    unlocked_ioctl:   ListMPDrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool ListMPDrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool ListMPDrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool ListMPDrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 ListMPDrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for ListMP
 */
static Ptr ListMPDrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the ListMP with OsalDriver
 */
Ptr
ListMPDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "ListMPDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("ListMP",
                                            &ListMPDrv_driverOps,
                                            LISTMP_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "ListMPDrv_registerDriver",
                             ListMP_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "ListMPDrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the ListMP with OsalDriver
 */
Void
ListMPDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "ListMPDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "ListMPDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
ListMPDrv_open (struct inode * inode, struct file * filp)
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
ListMPDrv_close (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long ListMPDrv_ioctl (struct file *  filp,
                      unsigned int   cmd,
                      unsigned long  args)
{
    int                 osStatus = 0;
    ListMPDrv_CmdArgs * dstArgs  = (ListMPDrv_CmdArgs *) args;
    Int32               status = ListMP_S_SUCCESS;
    Int32               ret;
    ListMPDrv_CmdArgs   cargs;

    GT_3trace (curTrace, GT_ENTER, "ListMPDrv_ioctl",
               filp, cmd, args);

    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (ListMPDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));


    switch (cmd) {
        case CMD_LISTMP_CREATE:
        {
            ListMP_Params params;

            ret = copy_from_user (&params,
                                  cargs.args.create.params,
                                  sizeof (ListMP_Params));
            GT_assert (curTrace, (ret == 0));

            if (    cargs.args.create.sharedAddrSrPtr
                !=  SharedRegion_INVALIDSRPTR) {
                params.sharedAddr = (Ptr) SharedRegion_getPtr (
                                            cargs.args.create.sharedAddrSrPtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (EXPECT_FALSE (params.sharedAddr == NULL)) {
                    status = ListMP_E_FAIL;
                    GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ListMPDrv_ioctl",
                                     status,
                                     "CMD_LISTMP_CREATE: "
                                     "Failed to translate params.sharedAddr!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
            else {
                params.sharedAddr = NULL;
            }

            if (EXPECT_TRUE (status >= 0)) {
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

                /* Update gate in params. */
                params.gate = cargs.args.create.knlGate;

                cargs.args.create.handle = ListMP_create (&params);
                GT_assert (curTrace, (cargs.args.create.handle != NULL));

                /* Set failure status if create has failed. */
                if (EXPECT_FALSE (cargs.args.create.handle == NULL)) {
                    status = ListMP_E_FAIL;
                }

                if (cargs.args.create.nameLen > 0) {
                    Memory_free (NULL, params.name, cargs.args.create.nameLen);
                }
            }
        }
        break;

        case CMD_LISTMP_DELETE:
        {
            status = ListMP_delete ((ListMP_Handle *)
                                            &cargs.args.deleteInstance.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_LISTMP_OPEN:
        {
            String        name = NULL;
            ListMP_Handle handle;

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

            status = ListMP_open (name, &handle);
            cargs.args.open.handle = handle;

            if (cargs.args.open.nameLen > 0) {
                Memory_free (NULL, name, cargs.args.open.nameLen);
            }
        }
        break;

        case CMD_LISTMP_OPENBYADDR:
        {
            Ptr           sharedAddr;
            ListMP_Handle handle;


            /* Initialize to NULL. name gets precedence. */
            sharedAddr = NULL;

            /* For openByAddr by name, the sharedAddr may be invalid. */
            sharedAddr = SharedRegion_getPtr (
                                       cargs.args.openByAddr.sharedAddrSrPtr);

            status = ListMP_openByAddr (sharedAddr, &handle);
            cargs.args.openByAddr.handle = handle;

        }
        break;

        case CMD_LISTMP_CLOSE:
        {
            status = ListMP_close ((ListMP_Handle *)
                                               &cargs.args.close.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_LISTMP_SHAREDMEMREQ:
        {
            ListMP_Params params;

            ret = copy_from_user (&params,
                                  cargs.args.sharedMemReq.params,
                                  sizeof (ListMP_Params));
            GT_assert (curTrace, (ret == 0));

            /* For ListMP_sharedMemReq, the sharedAddr may be invalid. */
            params.sharedAddr = (Ptr) SharedRegion_getPtr (
                                    cargs.args.sharedMemReq.sharedAddrSrPtr);

            cargs.args.sharedMemReq.bytes = ListMP_sharedMemReq (&params);
        }
        break;

        case CMD_LISTMP_ISEMPTY :
        {
            cargs.args.isEmpty.isEmpty = ListMP_empty (cargs.args.isEmpty.handle);
        }
        break;

        case CMD_LISTMP_PUTTAIL :
        {
            ListMP_Elem *  elem;
            elem = (ListMP_Elem *) SharedRegion_getPtr (
                                                 cargs.args.putTail.elemSrPtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (elem == NULL)) {
                /* This indicates that the elem was not in a SharedRegion. */
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ListMPDrv_ioctl",
                                     status,
                                     "CMD_LISTMP_PUTTAIL:"
                                     " Failed to translate elem!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = ListMP_putTail (cargs.args.putTail.handle, elem);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_LISTMP_GETHEAD:
        {
            ListMP_Elem *  elem;
            UInt16         index;

            elem = ListMP_getHead (cargs.args.getHead.handle);
            if (elem != NULL) {
                index = SharedRegion_getId (elem);
                cargs.args.getHead.elemSrPtr = SharedRegion_getSRPtr (elem,
                                                                       index);
            }
            else {
                /* Set to invalid in case of failure. */
                cargs.args.getHead.elemSrPtr = SharedRegion_INVALIDSRPTR;
            }
        }
        break;

        case CMD_LISTMP_NEXT:
        {
            ListMP_Elem * elem;
            ListMP_Elem * retElem;
            UInt16        index;

            /* elem can be NULL for ListMP_next. */
            elem = (ListMP_Elem *) SharedRegion_getPtr(
                                                    cargs.args.next.elemSrPtr);
            retElem  = ListMP_next (cargs.args.next.handle, elem);
            index = SharedRegion_getId (retElem);
            cargs.args.next.nextElemSrPtr = SharedRegion_getSRPtr (retElem,
                                                                    index);
        }
        break;

        case CMD_LISTMP_PREV:
        {
            ListMP_Elem * elem;
            ListMP_Elem * retElem;
            UInt16        index;

            /* elem can be NULL for ListMP_prev. */
            elem = (ListMP_Elem *) SharedRegion_getPtr(
                                                    cargs.args.prev.elemSrPtr);
            retElem  = ListMP_prev (cargs.args.prev.handle, elem);
            index = SharedRegion_getId (retElem);
            cargs.args.prev.prevElemSrPtr = SharedRegion_getSRPtr (retElem,
                                                                    index);
        }
        break;

        case CMD_LISTMP_INSERT:
        {
            ListMP_Elem *  newElem;
            ListMP_Elem *  curElem;
            newElem = (ListMP_Elem *) SharedRegion_getPtr (
                                               cargs.args.insert.newElemSrPtr);
            curElem = (ListMP_Elem *) SharedRegion_getPtr (
                                               cargs.args.insert.curElemSrPtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (newElem == NULL)) {
                /* This indicates that the newElem was not in a SharedRegion. */
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ListMPDrv_ioctl",
                                     status,
                                     "CMD_LISTMP_INSERT:"
                                     " Failed to translate newElem!");
            }
            else if (EXPECT_FALSE (curElem == NULL)) {
                /* This indicates that the curElem was not in a SharedRegion. */
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ListMPDrv_ioctl",
                                     status,
                                     "CMD_LISTMP_INSERT:"
                                     " Failed to translate curElem!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                status  = ListMP_insert (cargs.args.insert.handle,
                                         newElem,
                                         curElem);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_LISTMP_REMOVE:
        {
            ListMP_Elem * elem;

            elem = (ListMP_Elem *) SharedRegion_getPtr (
                                                cargs.args.remove.elemSrPtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (elem == NULL)) {
                /* This indicates that the elem was not in a SharedRegion. */
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ListMPDrv_ioctl",
                                     status,
                                     "CMD_LISTMP_REMOVE:"
                                     " Failed to translate elem!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                status  = ListMP_remove (cargs.args.remove.handle, elem);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_LISTMP_GETTAIL:
        {
            ListMP_Elem *  elem;
            UInt16         index;

            elem = ListMP_getTail (cargs.args.getTail.handle);
            index = SharedRegion_getId (elem);
            cargs.args.getTail.elemSrPtr = SharedRegion_getSRPtr (elem,
                                                                   index);
        }
        break;

        case CMD_LISTMP_PUTHEAD:
        {
            ListMP_Elem * elem;

            elem = (ListMP_Elem *) SharedRegion_getPtr (
                                                 cargs.args.putHead.elemSrPtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (EXPECT_FALSE (elem == NULL)) {
                /* This indicates that the elem was not in a SharedRegion. */
                status = ListMP_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "ListMPDrv_ioctl",
                                     status,
                                     "CMD_LISTMP_PUTHEAD:"
                                     " Failed to translate elem!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = ListMP_putHead (cargs.args.putHead.handle, elem);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_LISTMP_GETCONFIG:
        {
            ListMP_Config config;

            ListMP_getConfig (&config);
            GT_assert (curTrace, (status >= 0));

            ret = copy_to_user (cargs.args.getConfig.config,
                                &config,
                                sizeof (ListMP_Config));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_LISTMP_SETUP:
        {
            ListMP_Config config;

            ret = copy_from_user (&config,
                                  cargs.args.setup.config,
                                  sizeof (ListMP_Config));
            GT_assert (curTrace, (ret == 0));

            status = ListMP_setup (&config);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_LISTMP_DESTROY:
        {
            status = ListMP_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_LISTMP_PARAMS_INIT:
        {
            ListMP_Params params;

            ret = copy_from_user (&params,
                                  cargs.args.ParamsInit.params,
                                  sizeof (ListMP_Params));
            GT_assert (curTrace, (ret == 0));

            ListMP_Params_init (&params);

            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (ListMP_Params));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = ListMP_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "ListMPDrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs.apiStatus = status;

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (ListMPDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "ListMPDrv_ioctl", osStatus);

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
int __init ListMPDrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("ListMP sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("ListMPDrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        ListMPDrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if (    (ListMPDrv_enableTrace != 0)
            &&  (ListMPDrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (ListMPDrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (ListMPDrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        ListMPDrv_enableTraceEnter = simple_strtol (TRACEENTER,
                                                                NULL,
                                                                16);
        if (    (ListMPDrv_enableTraceEnter != 0)
            &&  (ListMPDrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (ListMPDrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        ListMPDrv_enableTraceFailure = simple_strtol (TRACEFAILURE,
                                                                  NULL,
                                                                  16);
        if (    (ListMPDrv_enableTraceFailure != 0)
            &&  (ListMPDrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (ListMPDrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        ListMPDrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (ListMPDrv_traceClass != 1)
            &&  (ListMPDrv_traceClass != 2)
            &&  (ListMPDrv_traceClass != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            ListMPDrv_traceClass =
                 ListMPDrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= ListMPDrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    ListMPDrv_osalDrvHandle =
                                        ListMPDrv_registerDriver ();
    if (ListMPDrv_osalDrvHandle == NULL) {
        /*! @retval ListMP_E_OSFAILURE Failed to register
                           ListMP driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                      GT_4CLASS,
                      "ListMPDrv_initializeModule",
                      ListMP_E_OSFAILURE,
                      "Failed to register ListMP driver with OS!");
    }

    Osal_printf ("ListMPDrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit ListMPDrv_finalizeModule (void)
{
    Osal_printf ("ListMPDrv_finalizeModule\n");
    Osal_printf ("Entered ListMPDrv_finalizeModule\n");

    ListMPDrv_unregisterDriver (
            &(ListMPDrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving ListMPDrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (ListMPDrv_initializeModule);
module_exit (ListMPDrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
