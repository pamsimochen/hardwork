/*
 *  @file   RingIODrv.c
 *
 *  @brief      Declarations of OS-specific functionality for
 *              RingIO
 *
 *              This file contains declarations of OS-specific functions for
 *              RingIO.
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
#include <ti/ipc/MultiProc.h>
#include <ti/ipc/GateMP.h> // temp
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
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/RingIO.h>
#include <ti/syslink/inc/_RingIO.h>
#include <ti/syslink/inc/IRingIO.h>
#include <ti/syslink/RingIOShm.h>
#include <ti/syslink/inc/knl/Linux/RingIODrv.h>
#include <ti/syslink/inc/RingIODrvDefs.h>
#include <ti/ipc/SharedRegion.h>


/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* RingIO functions */
EXPORT_SYMBOL(RingIO_getConfig);
EXPORT_SYMBOL(RingIO_setup);
EXPORT_SYMBOL(RingIO_destroy);
EXPORT_SYMBOL(RingIO_Params_init);
EXPORT_SYMBOL(RingIO_create);
EXPORT_SYMBOL(RingIO_delete);
EXPORT_SYMBOL(RingIO_open);
EXPORT_SYMBOL(RingIO_openByAddr);
EXPORT_SYMBOL(RingIO_close);
EXPORT_SYMBOL(RingIO_registerNotifier);
EXPORT_SYMBOL(RingIO_unregisterNotifier);
EXPORT_SYMBOL(RingIO_getValidSize);
EXPORT_SYMBOL(RingIO_getEmptySize);
EXPORT_SYMBOL(RingIO_getValidAttrSize);
EXPORT_SYMBOL(RingIO_getEmptyAttrSize);
EXPORT_SYMBOL(RingIO_getAcquiredOffset);
EXPORT_SYMBOL(RingIO_getAcquiredSize);
EXPORT_SYMBOL(RingIO_getWaterMark);
EXPORT_SYMBOL(RingIO_setWaterMark);
EXPORT_SYMBOL(RingIO_acquire);
EXPORT_SYMBOL(RingIO_release);
EXPORT_SYMBOL(RingIO_cancel);
EXPORT_SYMBOL(RingIO_flush);
EXPORT_SYMBOL(RingIO_sendNotify);
EXPORT_SYMBOL(RingIO_getvAttribute);
EXPORT_SYMBOL(RingIO_setvAttribute);
EXPORT_SYMBOL(RingIO_getAttribute);
EXPORT_SYMBOL(RingIO_setAttribute);
EXPORT_SYMBOL(RingIO_sharedMemReq);
EXPORT_SYMBOL(RingIO_setNotifyType);


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for RingIO.
 */
#define RINGIO_DRV_MINOR_NUMBER 17


/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int RingIODrv_open (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int RingIODrv_close (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long RingIODrv_ioctl ( struct file *  filp,
                              unsigned int   cmd,
                              unsigned long  args);


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Module initialization function for Linux driver.
 */
static int __init RingIODrv_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit RingIODrv_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations RingIODrv_driverOps = {
    open:    RingIODrv_open,
    release: RingIODrv_close,
    unlocked_ioctl:   RingIODrv_ioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool RingIODrv_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool RingIODrv_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool RingIODrv_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 RingIODrv_traceClass = 0;

/*!
 *  @brief  OsalDriver handle for RingIO
 */
static Handle RingIODrv_osalDrvHandle = NULL;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the RingIO with OsalDriver
 */
Ptr
RingIODrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "RingIODrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("RingIO",
                                            &RingIODrv_driverOps,
                                            RINGIO_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "RingIODrv_registerDriver",
                             RingIO_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "RingIODrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the RingIO with OsalDriver
 */
Void
RingIODrv_unregisterDriver (Ptr    * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "RingIODrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "RingIODrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
RingIODrv_open (struct inode * inode, struct file * filp)
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
RingIODrv_close (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long RingIODrv_ioctl ( struct file *  filp,
                       unsigned int   cmd,
                       unsigned long  args)
{
    int                 osStatus = 0;
    RingIODrv_CmdArgs * dstArgs  = (RingIODrv_CmdArgs *) args;
    Int32               status   = RingIO_S_SUCCESS;
    Int32               ret;
    RingIODrv_CmdArgs   cargs;

    GT_3trace (curTrace, GT_ENTER, "RingIODrv_ioctl",
               filp, cmd, args);

    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (RingIODrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    switch (cmd) {

        case CMD_RINGIO_PARAMS_INIT :
        {
            RingIO_Params params;

            RingIO_Params_init (&params);
            GT_assert (curTrace, (status >= 0));

            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (RingIO_Params));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_RINGIO_CREATE:
        {
            RingIOShm_Params params;
            Ptr              cliNotifyMgrshMem;
            Ptr              cliGateShMem;
            UInt16           index;

            ret = copy_from_user (&params,
                                  ((RingIOShm_Params *)cargs.args.create.params),
                                  sizeof (RingIOShm_Params));
            GT_assert (curTrace, (ret == 0));
            if (params.ctrlSharedAddr != NULL) {
                params.ctrlSharedAddr = (Ptr)SharedRegion_getPtr ((SharedRegion_SRPtr)
                                             cargs.args.create.ctrlSharedAddrSrPtr);
            }
            if (params.dataSharedAddr != NULL) {
                params.dataSharedAddr = (Ptr)SharedRegion_getPtr ((SharedRegion_SRPtr)
                                             cargs.args.create.dataSharedAddrSrPtr);
            }
            if (params.attrSharedAddr != NULL) {
                params.attrSharedAddr = (Ptr)SharedRegion_getPtr ((SharedRegion_SRPtr)
                                             cargs.args.create.attrSharedAddrSrPtr);
            }

            /* Allocate memory for the name */
            if (cargs.args.create.nameLen > 0) {
                params.commonParams.name = Memory_alloc (NULL,
                                            cargs.args.create.nameLen,
                                            0,
                                            NULL);
                GT_assert (curTrace, (params.commonParams.name != NULL));
                /* Copy the name */
                ret = copy_from_user (params.commonParams.name,
                                      ((RingIOShm_Params *)(cargs.args.create.params))->commonParams.name,
                                      cargs.args.create.nameLen);
                GT_assert (curTrace, (ret == 0));
            }

            params.gateHandle = cargs.args.create.knlLockHandle;

            cargs.args.create.handle = RingIO_create ((Ptr)&params);

            if (cargs.args.create.nameLen > 0) {
                Memory_free (NULL, params.commonParams.name, cargs.args.create.nameLen);
            }

            if (cargs.args.create.handle != NULL) {
                cliNotifyMgrshMem = RingIO_getCliNotifyMgrShAddr(cargs.args.create.handle);
                index = SharedRegion_getId(cliNotifyMgrshMem);

                cargs.args.create.cliNotifyMgrSharedMem = SharedRegion_getSRPtr(cliNotifyMgrshMem, index);
                GT_assert (curTrace, (cargs.args.create.cliNotifyMgrSharedMem != SharedRegion_INVALIDSRPTR));

                cliGateShMem = RingIO_getCliNotifyMgrGateShAddr( cargs.args.create.handle);
                index = SharedRegion_getId(cliGateShMem);
                cargs.args.create.cliGateSharedMem = SharedRegion_getSRPtr(cliGateShMem, index);
                GT_assert (curTrace, (cargs.args.create.cliGateSharedMem!= SharedRegion_INVALIDSRPTR));


            }
        }
        break;

        case CMD_RINGIO_DELETE:
        {
          status = RingIO_delete ((RingIO_Handle *)
                                       &cargs.args.deleteRingIO.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_RINGIO_SHAREDMEMREQ:
        {
            RingIOShm_Params           params;
            RingIO_sharedMemReqDetails sharedMemReqDetails;

            ret = copy_from_user (&params,
                                  (RingIOShm_Params *)cargs.args.sharedMemReq.params,
                                  sizeof (RingIOShm_Params));
            GT_assert (curTrace, (ret == 0));

            cargs.args.sharedMemReq.bytes = RingIO_sharedMemReq (
                                             (Ptr)cargs.args.sharedMemReq.params,
                                             &sharedMemReqDetails);

            cargs.args.sharedMemReq.ctrlSharedMemReq =
                sharedMemReqDetails.ctrlSharedMemReq;
            cargs.args.sharedMemReq.dataSharedMemReq =
                sharedMemReqDetails.dataSharedMemReq;
            cargs.args.sharedMemReq.attrSharedMemReq =
                sharedMemReqDetails.attrSharedMemReq;
        }
        break;

        case CMD_RINGIO_OPEN:
        {
            RingIO_openParams params;
            RingIO_Handle     handle;
            Ptr               cliNotifyMgrshMem;
            Ptr               cliGateShMem;
            UInt16            index;
            UInt16          * procIds = NULL;
            String            name    = NULL;
            UInt16            numProcs = 0;
            UInt16            i;
            RingIO_Object   * object = NULL;

            ret = copy_from_user (&params,
                                  cargs.args.open.params,
                                  sizeof (RingIO_openParams));
            GT_assert (curTrace, (ret == 0));


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

            if (cargs.args.open.procIds != NULL) {
            i = 0;
            numProcs = 0;
                while (cargs.args.open.procIds[i] != MultiProc_INVALIDID) {
                numProcs ++;
                i++;
                if (i > MultiProc_getNumProcessors() + 1) {
                    status = RingIO_E_INVALIDARG;
                    break;
                }
            }
            }

            /* Allocate memory for the procIds */
            if (numProcs > 0) {
                procIds = Memory_alloc (NULL,
                                        numProcs *
                                        sizeof(UInt16),
                                        0,
                                        NULL);
                GT_assert (curTrace, (procIds != NULL));
                /* Copy the procIds */
                ret = copy_from_user (procIds,
                                      cargs.args.open.procIds,
                                      numProcs);
                GT_assert (curTrace, (ret == 0));
            }
            status = RingIO_open (name,
                                  &params,
                                  procIds,
                                  &handle);
            cargs.args.open.handle = (Ptr)handle;

            if (cargs.args.open.nameLen > 0) {
                Memory_free (NULL, name, cargs.args.open.nameLen);
            }

            if (numProcs > 0) {
                Memory_free (NULL, procIds, numProcs * sizeof (UInt16));
            }

            if (cargs.args.open.handle != NULL) {
                object                       = (RingIO_Object *)handle;
//TBD                cargs.args.open.remoteProcId = object->params.remoteProcId;

                cliNotifyMgrshMem = RingIO_getCliNotifyMgrShAddr((RingIO_Handle)cargs.args.open.handle);
                index = SharedRegion_getId(cliNotifyMgrshMem);

                cargs.args.open.cliNotifyMgrSharedMem = SharedRegion_getSRPtr(cliNotifyMgrshMem, index);
                GT_assert (curTrace, (cargs.args.open.cliNotifyMgrSharedMem != SharedRegion_INVALIDSRPTR));

                cliGateShMem = RingIO_getCliNotifyMgrGateShAddr((RingIO_Handle)cargs.args.open.handle);
                index = SharedRegion_getId(cliGateShMem);
                cargs.args.open.cliGateSharedMem = SharedRegion_getSRPtr(cliGateShMem, index);
                GT_assert (curTrace, (cargs.args.open.cliGateSharedMem != SharedRegion_INVALIDSRPTR));
            }
        }
        break;

        case CMD_RINGIO_OPENBYADDR:
        {
            RingIO_openParams params;
            RingIO_Handle     handle;
            Ptr               cliNotifyMgrshMem;
            Ptr               cliGateShMem;
            UInt16            index;
            Ptr               ctrlSharedAddr = NULL;

            ret = copy_from_user (&params,
                                  cargs.args.openByAddr.params,
                                  sizeof (RingIO_openParams));
            GT_assert (curTrace, (ret == 0));

            if (cargs.args.openByAddr.ctrlSharedAddrSrPtr != SharedRegion_INVALIDSRPTR) {
                ctrlSharedAddr = (Ptr)SharedRegion_getPtr ((SharedRegion_SRPtr)
                        cargs.args.openByAddr.ctrlSharedAddrSrPtr);
            }

            status = RingIO_openByAddr (ctrlSharedAddr,
                    &params,
                    &handle);

            GT_assert (curTrace, (status >= 0));
            cargs.args.openByAddr.handle = handle;

            if (cargs.args.openByAddr.handle != NULL) {
                cliNotifyMgrshMem = RingIO_getCliNotifyMgrShAddr(cargs.args.openByAddr.handle);
                index = SharedRegion_getId(cliNotifyMgrshMem);

                cargs.args.openByAddr.cliNotifyMgrSharedMem = SharedRegion_getSRPtr(cliNotifyMgrshMem, index);
                GT_assert (curTrace, (cargs.args.openByAddr.cliNotifyMgrSharedMem != SharedRegion_INVALIDSRPTR));

                cliGateShMem = RingIO_getCliNotifyMgrGateShAddr( cargs.args.openByAddr.handle);
                index = SharedRegion_getId(cliGateShMem);
                cargs.args.openByAddr.cliGateSharedMem = SharedRegion_getSRPtr(cliGateShMem, index);
                GT_assert (curTrace, (cargs.args.openByAddr.cliGateSharedMem != SharedRegion_INVALIDSRPTR));
            }
        }
        break;

        case CMD_RINGIO_RESET_NOTIFYID :
        {
            status = RingIO_resetNotifyId (
                    cargs.args.resetNotifyId.handle,
                    cargs.args.resetNotifyId.notifyId);
            GT_assert (curTrace, (status >= 0));
        }
        break;


        case  CMD_RINGIO_SET_NOTIFYID:
        {
            status = RingIO_setNotifyId (
                    cargs.args.setNotifyId.handle,
                    cargs.args.setNotifyId.notifyId,
                    cargs.args.setNotifyId.notifyType,
                    cargs.args.setNotifyId.watermark);
        }
        break;

        case CMD_RINGIO_CLOSE:
        {
            status = RingIO_close ((RingIO_Handle *)
                                            &cargs.args.close.handle);
        }
        break;

        case CMD_RINGIO_GETCONFIG:
        {
            RingIO_Config config;

            RingIO_getConfig (&config);

            ret = copy_to_user (cargs.args.getConfig.config,
                                &config,
                                sizeof (RingIO_Config));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_RINGIO_SETUP:
        {
            RingIO_Config config;

            ret = copy_from_user (&config,
                                  cargs.args.setup.config,
                                  sizeof (RingIO_Config));
            GT_assert (curTrace, (ret == 0));

            status = RingIO_setup (&config);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_RINGIO_DESTROY:
        {
            status = RingIO_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_RINGIO_REGISTERNOTIFIER:
        {
            status = RingIO_registerNotifier
                (cargs.args.registerNotifier.handle,
                 cargs.args.registerNotifier.notifyType,
                 cargs.args.registerNotifier.watermark,
                 cargs.args.registerNotifier.notifyFunc,
                 cargs.args.registerNotifier.cbContext);
        }
        break;

        case CMD_RINGIO_UNREGISTERNOTIFIER:
        {
            status = RingIO_unregisterNotifier
                (cargs.args.registerNotifier.handle);
        }
        break;

        case CMD_RINGIO_GETVALIDSIZE:
        {
            cargs.args.getValidSize.size = RingIO_getValidSize (cargs.args.getValidSize.handle);
        }
        break;

        case CMD_RINGIO_GETEMPTYSIZE:
        {
            cargs.args.getEmptySize.size = RingIO_getEmptySize (cargs.args.getEmptySize.handle);
        }
        break;

        case CMD_RINGIO_GETVALIDATTRSIZE:
        {
            cargs.args.getValidAttrSize.attrSize = RingIO_getValidAttrSize (cargs.args.getValidAttrSize.handle);
        }
        break;

        case CMD_RINGIO_GETEMPTYATTRSIZE:
        {
            cargs.args.getEmptyAttrSize.attrSize = RingIO_getEmptyAttrSize (cargs.args.getEmptyAttrSize.handle);
        }
        break;

        case CMD_RINGIO_GETACQUIREDOFFSET:
        {
            cargs.args.getAcquiredOffset.offset = RingIO_getAcquiredOffset (cargs.args.getAcquiredOffset.handle);
        }
        break;

        case CMD_RINGIO_GETACQUIREDSIZE:
        {
            cargs.args.getAcquiredSize.size = RingIO_getAcquiredSize (cargs.args.getAcquiredSize.handle);
        }
        break;

        case CMD_RINGIO_GETWATERMARK:
        {
            cargs.args.getWaterMark.waterMark = RingIO_getWaterMark (cargs.args.getWaterMark.handle);
        }
        break;

        case CMD_RINGIO_SETWATERMARK:
        {
            status = RingIO_setWaterMark (cargs.args.setWaterMark.handle,
                                          cargs.args.setWaterMark.waterMark);
        }
        break;

        case CMD_RINGIO_ACQUIRE:
        {
            RingIO_BufPtr   pData = NULL;
            UInt32          pSize = 0;
            Ptr             phyPtr = NULL;

            ret = copy_from_user (&pData,
                                  cargs.args.acquire.pData,
                                  sizeof (RingIO_BufPtr));
            GT_assert (curTrace, (ret == 0));
            ret = copy_from_user (&pSize,
                                  cargs.args.acquire.pSize,
                                  sizeof (RingIO_BufPtr));
            GT_assert (curTrace, (ret == 0));

            status = RingIO_acquire (cargs.args.acquire.handle,
                                     &pData,
                                     &pSize);

            phyPtr = Memory_translate(pData, Memory_XltFlags_Virt2Phys);

//            ret = copy_to_user (cargs.args.acquire.pData,
//                                &phyPtr,
//                                sizeof (RingIO_BufPtr));
//            GT_assert (curTrace, (ret == 0));

            cargs.args.acquire.pData = phyPtr;
            ret = copy_to_user (cargs.args.acquire.pSize,
                                &pSize,
                                sizeof (UInt32));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_RINGIO_RELEASE:
        {

            GT_assert (curTrace, (ret == 0));

            status = RingIO_release (cargs.args.release.handle,
                                     cargs.args.release.size);


        }
        break;

        case CMD_RINGIO_CANCEL:
        {
            status = RingIO_cancel (cargs.args.cancel.handle);
        }
        break;

        case CMD_RINGIO_GETVATTRIBUTE:
        {
            UInt16              type;
            UInt32              param;
            UInt32           *  pSize  = NULL;
            RingIO_BufPtr       vptr   = NULL;

            if (cargs.args.getvAttribute.pSize != NULL) {
                pSize = (Ptr) kmalloc (sizeof (UInt32), GFP_ATOMIC);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (pSize == NULL) {
                    /*! @retval RingIO_E_MEMORY Failed to allocate memory for
                     *                          size of variable attribute */
                    status = RingIO_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "RingIODrv_acquire",
                                         status,
                                         "Failed to allocate memory for size of variable"
                                         " attribute!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

                if (status >= 0) {
                    ret = copy_from_user (pSize,
                                          cargs.args.getvAttribute.pSize,
                                          sizeof (UInt32));
                    GT_assert (curTrace, (ret == 0));
                    vptr = (Ptr) kmalloc (*pSize, GFP_ATOMIC);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (   (vptr == NULL)
                        && (pSize != NULL)) {
                        /*! @retval RingIO_E_MEMORY Failed to allocate memory for
                         *                          variable attribute */
                        status = RingIO_E_MEMORY;
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "RingIODrv_acquire",
                                             status,
                                             "Failed to allocate memory for variable"
                                             " attribute!");
                    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }

            if (status >= 0) {
                status = RingIO_getvAttribute (cargs.args.getvAttribute.handle,
                                               &type,
                                               &param,
                                               vptr,
                                               pSize);
                if (status >= 0) {
                    ret = copy_to_user (cargs.args.getvAttribute.type,
                                        &type,
                                        sizeof (UInt16));
                    GT_assert (curTrace, (ret == 0));

                    ret = copy_to_user (cargs.args.getvAttribute.param,
                                        &param,
                                        sizeof (UInt32));
                    GT_assert (curTrace, (ret == 0));

                    if (cargs.args.getvAttribute.pSize != NULL) {
                        ret = copy_to_user (cargs.args.getvAttribute.pSize,
                                            pSize,
                                            sizeof (UInt32));
                        GT_assert (curTrace, (ret == 0));
                    }

                    if (cargs.args.getvAttribute.pSize != NULL) {
                        ret = copy_to_user ((Ptr)cargs.args.getvAttribute.vptr,
                                            vptr,
                                            *pSize);
                        GT_assert (curTrace, (ret == 0));
                        kfree (pSize);
                        kfree (vptr);
                    }
                }
            }
        }
        break;

        case CMD_RINGIO_SETVATTRIBUTE:
        {
            Ptr         vptr   = NULL;

            if (cargs.args.setvAttribute.size != 0) {
                vptr = (Ptr)
                            kmalloc (cargs.args.setvAttribute.size, GFP_ATOMIC);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (  (vptr == NULL)
                   && (cargs.args.setvAttribute.size != 0)) {
                    /*! @retval RingIO_E_MEMORY Failed to allocate memory for
                     *                          variable attribute */
                    status = RingIO_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "RingIODrv_acquire",
                                         status,
                                         "Failed to allocate memory for variable"
                                         " attribute!");
                }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }

            if (status >= 0) {
                ret = copy_from_user (vptr,
                                      cargs.args.setvAttribute.pData,
                                      cargs.args.setvAttribute.size);

                status = RingIO_setvAttribute (cargs.args.setvAttribute.handle,
                                               cargs.args.setvAttribute.type,
                                               cargs.args.setvAttribute.param,
                                               vptr,
                                               cargs.args.setvAttribute.size,
                                               cargs.args.setvAttribute.sendNotification);

                if (vptr != NULL) {
                    kfree (vptr);
                }
            }
        }
        break;

        case CMD_RINGIO_FLUSH:
        {
            UInt16              type;
            UInt32              param;
            UInt32              bytesFlushed;

            status = RingIO_flush (cargs.args.flush.handle,
                                   cargs.args.flush.hardFlush,
                                   &type,
                                   &param,
                                   &bytesFlushed);

            ret = copy_to_user (cargs.args.flush.type,
                                &type,
                                sizeof (UInt16));
            GT_assert (curTrace, (ret == 0));

            ret = copy_to_user (cargs.args.flush.param,
                                &param,
                                sizeof (UInt32));
            GT_assert (curTrace, (ret == 0));

            ret = copy_to_user (cargs.args.flush.bytesFlushed,
                                &bytesFlushed,
                                sizeof (UInt32));
            GT_assert (curTrace, (ret == 0));
        }
        break;

        case CMD_RINGIO_SYNC:
        {
            status = RingIO_sendNotify (cargs.args.notify.handle,
                                    cargs.args.notify.msg);

        }
        break;

        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            status = RingIO_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "RingIODrv_ioctl",
                                 status,
                                 "Unsupported ioctl command specified");
        }
        break;
    }

    cargs.apiStatus = status;

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (RingIODrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "RingIODrv_ioctl",
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
int __init RingIODrv_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("RingIO sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("RingIODrv_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        RingIODrv_enableTrace = simple_strtol (TRACE, NULL, 16);
        if ((RingIODrv_enableTrace != 0) && (RingIODrv_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (RingIODrv_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (RingIODrv_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        RingIODrv_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (RingIODrv_enableTraceEnter != 0)
            &&  (RingIODrv_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (RingIODrv_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        RingIODrv_enableTraceFailure = simple_strtol (TRACEFAILURE, NULL, 16);
        if (    (RingIODrv_enableTraceFailure != 0)
            &&  (RingIODrv_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (RingIODrv_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        RingIODrv_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (RingIODrv_enableTraceFailure != 1)
            &&  (RingIODrv_enableTraceFailure != 2)
            &&  (RingIODrv_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            RingIODrv_traceClass =
                           RingIODrv_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= RingIODrv_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    /* Initialize the OsalDriver */
    OsalDriver_setup ();

    RingIODrv_osalDrvHandle = RingIODrv_registerDriver ();
    if (RingIODrv_osalDrvHandle == NULL) {
        /*! @retval RingIO_E_OSFAILURE Failed to register RingIO
                                        driver with OS! */
        result = -EFAULT;
        GT_setFailureReason (curTrace,
                          GT_4CLASS,
                          "RingIODrv_initializeModule",
                          RingIO_E_OSFAILURE,
                          "Failed to register RingIO driver with OS!");
    }

    Osal_printf ("RingIODrv_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit RingIODrv_finalizeModule (void)
{
    Osal_printf ("RingIODrv_finalizeModule\n");
    Osal_printf ("Entered RingIODrv_finalizeModule\n");

    RingIODrv_unregisterDriver (&(RingIODrv_osalDrvHandle));

    /* Finalize the OsalDriver */
    OsalDriver_destroy ();

    Osal_printf ("Leaving RingIODrv_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (RingIODrv_initializeModule);
module_exit (RingIODrv_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */
