/*
 *  @file   FrameQDrv.c
 *
 *  @brief      OS-specific implementation of FrameQ driver for Linux
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
#include <ti/syslink/FrameQDefs.h>
#include <ti/syslink/FrameQ.h>
#include <ti/syslink/inc/_FrameQ.h>
#include <ti/syslink/FrameQ_ShMem.h>
#include <ti/syslink/inc/_FrameQ_ShMem.h>
#include <ti/syslink/inc/FrameQDrvDefs.h>

/** ============================================================================
 *  Export kernel utils functions
 *  ============================================================================
 */
/* FrameQ functions */
EXPORT_SYMBOL(FrameQ_getConfig);
EXPORT_SYMBOL(FrameQ_setup);
EXPORT_SYMBOL(FrameQ_destroy);

EXPORT_SYMBOL(FrameQ_create);
EXPORT_SYMBOL(FrameQ_delete);
EXPORT_SYMBOL(FrameQ_open);
EXPORT_SYMBOL(FrameQ_close);
EXPORT_SYMBOL(FrameQ_alloc);
EXPORT_SYMBOL(FrameQ_allocv);
EXPORT_SYMBOL(FrameQ_free);
EXPORT_SYMBOL(FrameQ_freev);
EXPORT_SYMBOL(FrameQ_dup);
EXPORT_SYMBOL(FrameQ_put);
EXPORT_SYMBOL(FrameQ_putv);
EXPORT_SYMBOL(FrameQ_get);
EXPORT_SYMBOL(FrameQ_getv);
EXPORT_SYMBOL(FrameQ_registerNotifier);
EXPORT_SYMBOL(FrameQ_unregisterNotifier);
EXPORT_SYMBOL(FrameQ_sendNotify);
EXPORT_SYMBOL(FrameQ_getNumFrames);
EXPORT_SYMBOL(FrameQ_getvNumFrames);
EXPORT_SYMBOL(FrameQ_getNumFreeFrames);
EXPORT_SYMBOL(FrameQ_getvNumFreeFrames);

EXPORT_SYMBOL(FrameQ_control);

EXPORT_SYMBOL(FrameQ_setFrameBufValidSize);
EXPORT_SYMBOL(FrameQ_setFrameBufDataStartOffset);
EXPORT_SYMBOL(FrameQ_getNumFrameBuffers);
EXPORT_SYMBOL(FrameQ_getFrameBuffer);
EXPORT_SYMBOL(FrameQ_getFrameBufValidSize);
EXPORT_SYMBOL(FrameQ_getFrameBufDataStartOffset);
EXPORT_SYMBOL(FrameQ_getFrameBufSize);

/* Implementation specific exports */
EXPORT_SYMBOL(FrameQ_ShMem_Params_init);
EXPORT_SYMBOL(FrameQ_ShMem_sharedMemReq);

#if defined (__cplusplus)
extern "C" {
#endif /* defined (__cplusplus) */


/** ============================================================================
 *  Macros and types
 *  ============================================================================
 */
/*!
 *  @brief  Driver minor number for ProcMgr.
 */
#define FrameQ_DRV_MINOR_NUMBER             (16u)

/** ============================================================================
 *  Forward declarations of internal functions
 *  ============================================================================
 */
/*!
 *  @brief  Linux driver function to open the driver object.
 */
static int FrameQ_drvopen (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to close the driver object.
 */
static int FrameQ_drvclose (struct inode * inode, struct file * filp);

/*!
 *  @brief  Linux driver function to ioctl of the driver object.
 */
static long FrameQ_drvioctl (struct file *  filp,
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
static int __init FrameQ_initializeModule (void);

/*!
 *  @brief  Module finalization  function for Linux driver.
 */
static void  __exit FrameQ_finalizeModule (void);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Globals
 *  ============================================================================
 */

/*!
 *  @brief  Function to invoke the APIs through ioctl.
 */
static struct file_operations FrameQ_driverOps = {
    open:    FrameQ_drvopen,
    release: FrameQ_drvclose,
    unlocked_ioctl:   FrameQ_drvioctl,
} ;


#if defined (SYSLINK_MULTIPLE_MODULES)
/*!
 *  @brief  Indicates whether trace should be enabled.
 */
static Char * TRACE = FALSE;
module_param (TRACE, charp, S_IRUGO);
Bool FrameQ_enableTrace = FALSE;

/*!
 *  @brief  Indicates whether entry/leave trace should be enabled.
 */
static Char * TRACEENTER = FALSE;
module_param (TRACEENTER, charp, S_IRUGO);
Bool FrameQ_enableTraceEnter = FALSE;

/*!
 *  @brief  Indicates whether SetFailureReason trace should be enabled.
 */
static Char * TRACEFAILURE = FALSE;
module_param (TRACEFAILURE, charp, S_IRUGO);
Bool FrameQ_enableTraceFailure = FALSE;

/*!
 *  @brief  Indicates class of trace to be enabled
 */
static Char * TRACECLASS = NULL;
module_param (TRACECLASS, charp, S_IRUGO);
UInt32 FrameQ_traceClass = 0;
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*!
 *  @brief  Register the FrameQ with OsalDriver
 */
Ptr
FrameQDrv_registerDriver (Void)
{
    OsalDriver_Handle osalHandle;

    GT_0trace (curTrace, GT_ENTER, "FrameQDrv_registerDriver");

    osalHandle = OsalDriver_registerDriver ("FrameQ",
                                            &FrameQ_driverOps,
                                            FrameQ_DRV_MINOR_NUMBER);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (osalHandle == NULL) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "FrameQDrv_registerDriver",
                             FrameQ_E_INVALIDARG,
                             "OsalDriver_registerDriver failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "FrameQDrv_registerDriver",
               osalHandle);

    return (Ptr) osalHandle;
}


/*!
 *  @brief  Register the ProcMgr with OsalDriver
 */
Void
FrameQDrv_unregisterDriver (Ptr * drvHandle)
{
    GT_1trace (curTrace, GT_ENTER, "FrameQDrv_unregisterDriver",
               drvHandle);

    OsalDriver_unregisterDriver ((OsalDriver_Handle *) drvHandle);

    GT_0trace (curTrace, GT_LEAVE, "FrameQDrv_unregisterDriver");
}


/*!
 *  @brief  Linux specific function to open the driver.
 */
int
FrameQ_drvopen (struct inode * inode, struct file * filp)
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
FrameQ_drvclose (struct inode * inode, struct file * filp)
{
    return 0;
}

/*!
 *  @brief  Linux specific function to close the driver.
 */
static
long FrameQ_drvioctl (struct file *  filp,
                      unsigned int   cmd,
                      unsigned long  args)
{
    int                  osStatus = 0;
    FrameQDrv_CmdArgs * dstArgs  = (FrameQDrv_CmdArgs *) args;
    Int32                status = FrameQ_S_SUCCESS;
    Int32                ret;
    UInt32               i;
    UInt16               index;
    FrameQDrv_CmdArgs    cargs;

    GT_3trace (curTrace, GT_ENTER, "FrameQ_drvioctl",
               filp, cmd, args);

    /* Copy the full args from user-side */
    ret = copy_from_user (&cargs,
                          dstArgs,
                          sizeof (FrameQDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    switch (cmd) {

        case CMD_FRAMEQ_PUT:
        {
            FrameQ_Frame            frame;
            FrameQ_FrameBufInfo     *frameBufInfo;
            Ptr                     virtPtr;
            Ptr                     addr;

            frame = Memory_translate (cargs.args.put.frame,
                                      Memory_XltFlags_Phys2Virt);
            GT_assert(curTrace,(frame != NULL));

            frameBufInfo = (FrameQ_FrameBufInfo *)&(frame->frameBufInfo[0]);

            /* Convert frame address and frame buffer address in to knl virtual
             * format only when frame-buffer shared region's createHeap is TRUE.
             * Otherwise leave the addresses in the buffer-header as is,
             * which means FrameQ_put function will not expect virtual
             * frame-buffer memory addresses, and will
             * convert pointers to Portable pointers in place accordingly.
             */
            if (_is_phys2virt_translationRequired((Ptr)frameBufInfo[0].bufPtr)){

                for (i = 0; i < frame->numFrameBuffers; i++) {
                    addr = (Ptr)frameBufInfo[i].bufPtr;
                    virtPtr = Memory_translate(addr, Memory_XltFlags_Phys2Virt);
                    GT_assert(curTrace,(virtPtr != NULL));
                    frameBufInfo[i].bufPtr = (UInt32)virtPtr;
                }

            }

            status = FrameQ_put (cargs.args.put.handle, frame);

            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_PUTV:
        {
            UInt32  filledQueueId [FrameQ_MAX_FRAMESINVAPI];
            UInt32  framePtrArr   [FrameQ_MAX_FRAMESINVAPI];
            FrameQ_Frame            *framePtr      = NULL;
            FrameQ_FrameBufInfo     *frameBufInfo;
            Ptr                     virtPtr;
            Ptr                     addr;
            UInt32                  j;

            framePtr =  (FrameQ_Frame *) framePtrArr;

            ret = copy_from_user (framePtr,
                                  cargs.args.putv.aframePtr,
                                  (cargs.args.putv.numFrames * sizeof(UInt32)));
            GT_assert (curTrace, (ret == 0));
            GT_assert(curTrace, (cargs.args.putv.numFrames
                                 <= FrameQ_MAX_FRAMESINVAPI));


            if (ret != 0) {
                osStatus = -EFAULT;
            }
            else {
                ret = copy_from_user (filledQueueId,
                              cargs.args.putv.filledQueueId,
                              (cargs.args.putv.numFrames * sizeof(UInt32)));

                GT_assert (curTrace, (ret == 0));

                if (ret != 0) {
                    osStatus = -EFAULT;
                }
            }

            if (osStatus == 0) {

                for (i = 0; i < cargs.args.putv.numFrames; i++) {
                    /* Convert frame address and frame buffer address in to knl
                     * virtual format.
                     */
                    framePtr[i] = Memory_translate (framePtr[i],
                                              Memory_XltFlags_Phys2Virt);

                    GT_assert(curTrace,(framePtr[i] != NULL));

                    frameBufInfo = (FrameQ_FrameBufInfo *)
                                  &(framePtr[i]->frameBufInfo[0]);

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

                status = FrameQ_putv (cargs.args.putv.handle,
                                      (FrameQ_Frame*)framePtr,
                                      (Ptr)filledQueueId,
                                      cargs.args.putv.numFrames);

                GT_assert (curTrace, (status >= 0));
            }
        }
        break;

        case CMD_FRAMEQ_GET:
        {
            FrameQ_Frame        frame;
            FrameQ_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              j;

            status = FrameQ_get (cargs.args.get.handle, &frame);

            if (status >= 0) {
                /* Convert Frame and frame buffer address in frame to physical
                 * address formatso that user space api convert this physical
                 * address in to user space virtual format.
                 */
                frameBufInfo = (FrameQ_FrameBufInfo *)
                                                      &(frame->frameBufInfo[0]);

                /* check to see if address translation is required (MK) */
                if (_is_virt2phys_translationRequired(
		                        (Ptr)frameBufInfo[0].bufPtr)) {

                    /* translate frame buffer addresses when createHeap=true */
                    for(j = 0; j < frame->numFrameBuffers; j++) {
                        addr = (Ptr)frameBufInfo[j].bufPtr;
                        phyPtr = Memory_translate(addr,
                                                  Memory_XltFlags_Virt2Phys);
                        GT_assert(curTrace,(phyPtr != NULL));
                        frameBufInfo[j].bufPtr = (UInt32)phyPtr;
                    }
                }

		/* frame ptr is in the header, so always translate */
                cargs.args.get.frame = Memory_translate(
                                                    frame,
                                                    Memory_XltFlags_Virt2Phys);
            }
            else {
                cargs.args.get.frame = NULL;
            }
        }
        break;

        case CMD_FRAMEQ_GETV:
        {
            UInt32  filledQueueId [FrameQ_MAX_FRAMESINVAPI];
            UInt32  framePtrArr   [FrameQ_MAX_FRAMESINVAPI];
            FrameQ_Frame        *framePtr = NULL;
            FrameQ_FrameBufInfo *frameBufInfo;
            Ptr                 phyPtr;
            Ptr                 addr;
            UInt32              numFrames;
            UInt32              j;

            framePtr =  (FrameQ_Frame *) framePtrArr;
            numFrames =  cargs.args.getv.numFrames;
            ret = copy_from_user (filledQueueId,
                                  cargs.args.getv.filledQueueId,
                                  (numFrames * sizeof (UInt32)));

	    GT_assert (curTrace, (ret == 0));
            GT_assert (curTrace, (numFrames <= FrameQ_MAX_FRAMESINVAPI));

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
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */

            if ((status >= 0) && (osStatus == 0)) {

                status = FrameQ_getv (cargs.args.getv.handle,
                                      framePtr,
                                      (Ptr)filledQueueId,
                                      &cargs.args.getv.numFrames);

                if ((status >= 0) && (cargs.args.getv.numFrames > 0)) {

                    for (i = 0; i < cargs.args.getv.numFrames; i++) {
                        /* Convert Frame and frame buffer address in frame
                         * to physical address formatso that user space api
                         * convert this physical address in to user space
                         * virtual format.
                         */
                        frameBufInfo = (FrameQ_FrameBufInfo *)
                                        &(framePtr[i]->frameBufInfo[0]);

                        /* check to see if address translation is required*/
                        if (_is_virt2phys_translationRequired(
		                    (Ptr)frameBufInfo[0].bufPtr)) {

                            for (j = 0; j < framePtr[i]->numFrameBuffers;
                                    j++) {
                                addr = (Ptr)frameBufInfo[j].bufPtr;
                                phyPtr = Memory_translate(
                                                 addr,
                                                 Memory_XltFlags_Virt2Phys);
                                GT_assert(curTrace,(phyPtr != NULL));
                                frameBufInfo[j].bufPtr = (UInt32)phyPtr;
                            }
                        }

                        /* frame ptr is in the header, so always translate*/
                        framePtr[i] = Memory_translate(
                                                 framePtr[i],
                                                 Memory_XltFlags_Virt2Phys);
                    }

                    ret = copy_to_user (cargs.args.getv.aframePtr,
                                        framePtr,
                                        (  sizeof (UInt32)
                                         * cargs.args.getv.numFrames));
                    GT_assert (curTrace, (ret == 0));
                }
            }
        }
	break;

        case CMD_FRAMEQ_GET_NUMFRAMES :
        {
            /* Get the available frames in fillef queue 0 of FrameQ */
            UInt32  numFrames;

            status = FrameQ_getNumFrames(cargs.args.getNumFrames.handle,
                                         &numFrames);
            cargs.args.getNumFrames.numFrames = numFrames;

        }
        break;

        case CMD_FRAMEQ_GET_VNUMFRAMES :
        {
            /* Get the available frames in fillef queue 0 of FrameQ */
            UInt32  filledQueueId [FrameQ_MAX_FRAMESINVAPI];
            UInt32  numFrames[FrameQ_MAX_FRAMESINVAPI];

            GT_assert (curTrace, (cargs.args.getvNumFrames.numFilledQids
                                  <= FrameQ_MAX_FRAMESINVAPI));

            ret = copy_from_user (filledQueueId,
                                  cargs.args.getvNumFrames.filledQId,
                                  ( cargs.args.getvNumFrames.numFilledQids
                                    * sizeof (UInt32)));
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
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */

                status = FrameQ_getvNumFrames(
                                cargs.args.getvNumFrames.handle,
                                numFrames,
                                (Ptr)filledQueueId,
                                cargs.args.getvNumFrames.numFilledQids);

                ret = copy_to_user (
                            cargs.args.getvNumFrames.numFrames,
                            numFrames,
                            (  sizeof (UInt32)
                             * cargs.args.getvNumFrames.numFilledQids));
                GT_assert (curTrace, (ret == 0));


#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_FRAMEQ_GET_NUMFREEFRAMES :
        {
            /* Get the available frames in fillef queue 0 of FrameQ */
            UInt32  numFreeFrames;

            status = FrameQ_getNumFreeFrames(cargs.args.getNumFreeFrames.handle,
                                            &numFreeFrames);
            cargs.args.getNumFreeFrames.numFreeFrames = numFreeFrames;

        }
        break;

        case CMD_FRAMEQ_GET_VNUMFREEFRAMES :
        {
            /* Get the available frames in fillef queue 0 of FrameQ */
            UInt32  freeQueueId[FrameQ_MAX_FRAMESINVAPI];
            UInt32  numFreeFrames[FrameQ_MAX_FRAMESINVAPI];

            GT_assert (curTrace, (cargs.args.getvNumFreeFrames.numFreeQids
                                  <= FrameQ_MAX_FRAMESINVAPI));

            ret = copy_from_user (freeQueueId,
                                  cargs.args.getvNumFreeFrames.freeQId,
                                  ( cargs.args.getvNumFreeFrames.numFreeQids
                                   * sizeof (UInt32)));
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
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                status = FrameQ_getvNumFreeFrames(
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
#if 0
        case CMD_FRAMEQ_PARAMS_INIT :
        {
            FrameQ_Params params;

            FrameQ_Params_init (&params);

            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (FrameQ_Params));
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
#endif

        case CMD_FRAMEQ_CREATE      :
        {
            Ptr                     params = NULL;
            UInt32                  size = 0;
            FrameQ_ShMem_Params     *shMemParams;
            Ptr                     mgrshMem;
            Ptr                     gateShMem;
            UInt8                   fQNameLen =0;
            Ptr                     fQName= NULL;

            /* Allocate memory for the params pointer and copy the  parasm from
             * userspace pointer based on the type of interface
             */
            if (     cargs.args.create.interfaceType
                 ==  FrameQ_INTERFACE_SHAREDMEM) {
                size = sizeof(FrameQ_ShMem_Params);
            }

            params = Memory_calloc(NULL, size, 0, NULL);

            ret = copy_from_user (params,
                                  cargs.args.create.params,
                                  size);
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_1trace (curTrace,
                           GT_1CLASS,
                           "    FrameQDrv_ioctl: copy_from_user call"
                           " failed\n"
                           "        status [%d]",
                           ret);
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for the name */
                if (cargs.args.create.nameLen > 0) {
                    ((FrameQ_Params*)params)->commonCreateParams.name =
                                   Memory_alloc (NULL,
                                                 cargs.args.create.nameLen,
                                                 0,
                                                 NULL);
                    GT_assert (curTrace,
                               (((FrameQ_Params*)params)->
                                              commonCreateParams.name != NULL));
                    /* Copy the name */
                    ret = copy_from_user (
                        ((FrameQ_Params*)params)->commonCreateParams.name,
                        cargs.args.create.name,
                        cargs.args.create.nameLen);
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
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }

            if (osStatus == 0) {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */

                if (cargs.args.create.interfaceType
                        ==  FrameQ_INTERFACE_SHAREDMEM) {
                    shMemParams = (FrameQ_ShMem_Params*)params;
                    shMemParams->sharedAddr = SharedRegion_getPtr (
                                  (SharedRegion_SRPtr)shMemParams->sharedAddr);
                    if ( (SharedRegion_SRPtr) shMemParams->frameQBufMgrSharedAddr
                         == SharedRegion_INVALIDSRPTR) {
                        shMemParams->frameQBufMgrSharedAddr = NULL;
                    }
                    else {
                        shMemParams->frameQBufMgrSharedAddr =
                            SharedRegion_getPtr((SharedRegion_SRPtr)
                                           shMemParams->frameQBufMgrSharedAddr);
                    }
                    /* Copy FrameQ BufMgr Name */
                    if (cargs.args.create.fQBMnameLen > 0) {
                        fQNameLen = cargs.args.create.fQBMnameLen;
                        fQName =
                        shMemParams->frameQBufMgrName =
                                       Memory_alloc (NULL,
                                                     fQNameLen,
                                                     0,
                                                     NULL);
                        GT_assert (curTrace,
                                   (shMemParams->frameQBufMgrName != NULL));
                        /* Copy the name */
                        ret = copy_from_user (
                                              shMemParams->frameQBufMgrName,
                                              cargs.args.create.fQBMName,
                                              fQNameLen);
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
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                    }
                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (osStatus == 0) {
#endif
                    /* Call FrameQ_create  with the updated params */
                    cargs.args.create.handle = FrameQ_create (params);
                    GT_assert (curTrace, (cargs.args.create.handle != NULL));

                    if (cargs.args.create.handle != NULL) {
                        /* Get the required info to be able to create
                         * ClientNotifyMgr instance at user space
                         */
                        mgrshMem = _FrameQ_getCliNotifyMgrShAddr(
                                cargs.args.create.handle);
                        index = SharedRegion_getId(mgrshMem);
                        cargs.args.create.cliNotifyMgrSharedMem =
                                SharedRegion_getSRPtr(mgrshMem, index);

                        gateShMem = _FrameQ_getCliNotifyMgrGateShAddr(
                                cargs.args.create.handle);
                        index = SharedRegion_getId(gateShMem);
                        cargs.args.create.cliGateSharedMem =
                            SharedRegion_getSRPtr(gateShMem, index);

    /*
                        mgrshMem = _FrameQ_getFrameQBufMgrShAddr(
                                                     cargs.args.create.handle);
                        index = SharedRegion_getId(mgrshMem);
                        cargs.args.create.fQBufMgrSharedAddr =
                            SharedRegion_getSRPtr(mgrshMem, index);
                        gateShMem = _FrameQ_getFQBufMgrGateShAddr(
                                                    cargs.args.create.handle);
                        index = SharedRegion_getId(gateShMem);
                        cargs.args.create.fQGateSharedMem =
                                    SharedRegion_getSRPtr(gateShMem, index);
    */

                    }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
#endif

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
            if (((FrameQ_Params*)params)->commonCreateParams.name  != NULL) {
                Memory_free (NULL,
                            ((FrameQ_Params*)params)->commonCreateParams.name,
                             cargs.args.create.nameLen);
            }

            if (fQName != NULL) {
                Memory_free (NULL,
                             fQName,
                             fQNameLen);
            }
            if (params != NULL) {
                Memory_free (NULL, params, size);
            }
        }
        break;

        case CMD_FRAMEQ_DELETE      :
        {
            status = FrameQ_delete ((FrameQ_Handle *)
                                    &cargs.args.deleteInstance.handle);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_OPEN        :
        {
            Ptr                     openParams = NULL;
            UInt32                  size       = 0;
            FrameQ_Handle           handle;
            Ptr                     mgrshMem;
            Ptr                     gateShMem;

            /* Allocate memory for the params pointer and copy the  params from
             * userspace pointer based on the type of interface
             */
            if (     cargs.args.open.interfaceType
                 ==  FrameQ_INTERFACE_SHAREDMEM) {
                size = sizeof(FrameQ_ShMem_OpenParams);
            }
            openParams = Memory_alloc(NULL, size, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (openParams ==  NULL) {
                status = FrameQ_E_ALLOC_MEMORY;
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
                              "    FrameQDrv_ioctl: copy_from_user"
                              " call failed\n"
                              "        status [%d]",
                              ret);
                    osStatus = -EFAULT;
                }
                else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Convert the shared control address to virtual format */
                    ((FrameQ_OpenParams *)openParams)->
                        commonOpenParams.sharedAddr =
                            SharedRegion_getPtr ((SharedRegion_SRPtr)
                 ((FrameQ_OpenParams *)openParams)->commonOpenParams.sharedAddr);

                    if (cargs.args.open.nameLen > 0) {
                        ((FrameQ_OpenParams *)openParams)->
                        commonOpenParams.name =
                                       Memory_alloc (NULL,
                                                     cargs.args.open.nameLen,
                                                     0,
                                                     NULL);
                        GT_assert (curTrace,
                                   ((FrameQ_OpenParams *)openParams)->commonOpenParams.name);
                        /* Copy the name */
                        ret = copy_from_user (
                                              ((FrameQ_OpenParams *)openParams)->commonOpenParams.name,
                            ((FrameQ_OpenParams *)cargs.args.open.openParams)->commonOpenParams.name,
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
                        status = FrameQ_open (openParams,&handle);
                        GT_assert (curTrace, (status >= 0));
                        cargs.args.open.handle = handle;
                        if (cargs.args.open.handle != NULL) {
                            mgrshMem = _FrameQ_getCliNotifyMgrShAddr(
                                                             cargs.args.open.handle);
                            index = SharedRegion_getId(mgrshMem);
                            cargs.args.open.cliNotifyMgrSharedMem =
                                SharedRegion_getSRPtr(mgrshMem, index);
                            gateShMem = _FrameQ_getCliNotifyMgrGateShAddr(
                                                             cargs.args.open.handle);
                            index = SharedRegion_getId(gateShMem);
                            cargs.args.open.cliGateSharedMem =
                                SharedRegion_getSRPtr(gateShMem, index);

    /*
                            mgrshMem = _FrameQ_getFrameQBufMgrShAddr(
                                                             cargs.args.open.handle);
                            index = SharedRegion_getId(mgrshMem);
                            cargs.args.open.fQBufMgrSharedAddr =
                                SharedRegion_getSRPtr(mgrshMem, index);
                            gateShMem = _FrameQ_getFQBufMgrGateShAddr(
                                                             cargs.args.open.handle);
                            index = SharedRegion_getId(gateShMem);
                            cargs.args.open.fQGateSharedMem =
                                                SharedRegion_getSRPtr(gateShMem, index);
    */

                        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    }
                }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                if (((FrameQ_OpenParams *)openParams)->commonOpenParams.name != NULL) {
                    Memory_free(NULL,
                                ((FrameQ_OpenParams *)openParams)->commonOpenParams.name,
                                cargs.args.open.nameLen);
                }

                Memory_free (NULL, openParams, size);


#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_FRAMEQ_CLOSE   :
        {
            status = FrameQ_close ((FrameQ_Handle *)&(cargs.args.close.handle));
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_REG_NOTIFIER :
        {
            status = FrameQ_registerNotifier (
                                 cargs.args.regNotifier.handle,
                                 &cargs.args.regNotifier.notifyParams);
           GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_UNREG_NOTIFIER :
        {
            status = FrameQ_unregisterNotifier (
                                              cargs.args.unregNotifier.handle);
        }
        break;

        case  CMD_FRAMEQ_SET_NOTIFYID:
        {
            status = _FrameQ_setNotifyId (
                                         cargs.args.setNotifyId.handle,
                                         cargs.args.setNotifyId.notifyId);
           GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_RESET_NOTIFYID :
        {
            status = _FrameQ_resetNotifyId (
                                         cargs.args.resetNotifyId.handle,
                                         cargs.args.resetNotifyId.notifyId);
           GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_GETCONFIG       :
        {
            FrameQ_Config config;

            FrameQ_getConfig (&config);

            ret = copy_to_user (cargs.args.getConfig.config,
                                &config,
                                sizeof (FrameQ_Config));
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

        case CMD_FRAMEQ_SETUP       :
        {
            FrameQ_Config config;

            ret = copy_from_user (&config,
                                  cargs.args.getConfig.config,
                                  sizeof (FrameQ_Config));
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
            status = FrameQ_setup (&config);
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_DESTROY     :
        {
            status = FrameQ_destroy ();
            GT_assert (curTrace, (status >= 0));
        }
        break;

        case CMD_FRAMEQ_CONTROL     :
        {
            Ptr arg;

            arg = Memory_alloc(NULL, cargs.args.control.size, 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (arg == NULL) {
                status = FrameQ_E_ALLOC_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "FrameQDrv_ioctl",
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
                               "    FrameQDrv_ioctl: copy_to_user call"
                               " failed\n"
                               "        status [%d]",
                               ret);
                    osStatus = -EFAULT;
                }
                else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */

                    status = FrameQ_control( cargs.args.control.handle,
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
        case CMD_FRAMEQ_SHMEM_PARAMS_INIT  :
        {
            FrameQ_ShMem_Params params;

            FrameQ_ShMem_Params_init(&params);
            ret = copy_to_user (cargs.args.ParamsInit.params,
                                &params,
                                sizeof (FrameQ_ShMem_Params));
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

        case CMD_FRAMEQ_SHMEM_MEMREQ:
        {
            FrameQ_ShMem_Params params;
            UInt32      memreq;

            FrameQ_ShMem_Params_init(&params);
            ret = copy_from_user (&params,
                                  cargs.args.sharedMemReq.params,
                                  sizeof (FrameQ_ShMem_Params));
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
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                memreq = FrameQ_ShMem_sharedMemReq(&params);
                /* cargs gets copied to user space below */
                cargs.args.sharedMemReq.bytes = memreq;

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        default :
        {
            /* FIXME */
        }
    }

    cargs.apiStatus = status;

    /* Copy the full args to the user-side. */
    ret = copy_to_user (dstArgs,
                        &cargs,
                        sizeof (FrameQDrv_CmdArgs));
    GT_assert (curTrace, (ret == 0));

    GT_1trace (curTrace, GT_LEAVE, "FrameQDrv_ioctl",
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
int __init FrameQ_initializeModule (Void)
{
    int result = 0;

    /* Display the version info and created date/time */
    Osal_printf ("ProcMgr sample module created on Date:%s Time:%s\n",
                 __DATE__,
                 __TIME__);

    Osal_printf ("FrameQ_initializeModule\n");

    /* Enable/disable levels of tracing. */
    if (TRACE != NULL) {
        Osal_printf ("Trace enable %s\n", TRACE) ;
        FrameQ_enableTrace = simple_strtol (TRACE, NULL, 16);
        if ((FrameQ_enableTrace != 0) && (FrameQ_enableTrace != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACE\n") ;
        }
        else if (FrameQ_enableTrace == TRUE) {
            curTrace = GT_TraceState_Enable;
        }
        else if (FrameQ_enableTrace == FALSE) {
            curTrace = GT_TraceState_Disable;
        }
    }

    if (TRACEENTER != NULL) {
        Osal_printf ("Trace entry/leave prints enable %s\n", TRACEENTER) ;
        FrameQ_enableTraceEnter = simple_strtol (TRACEENTER, NULL, 16);
        if (    (FrameQ_enableTraceEnter != 0)
            &&  (FrameQ_enableTraceEnter != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (FrameQ_enableTraceEnter == TRUE) {
            curTrace |= GT_TraceEnter_Enable;
        }
    }

    if (TRACEFAILURE != NULL) {
        Osal_printf ("Trace SetFailureReason enable %s\n", TRACEFAILURE) ;
        FrameQ_enableTraceFailure = simple_strtol (TRACEFAILURE, NULL, 16);
        if (    (FrameQ_enableTraceFailure != 0)
            &&  (FrameQ_enableTraceFailure != 1)) {
            Osal_printf ("Error! Only 0/1 supported for TRACEENTER\n") ;
        }
        else if (FrameQ_enableTraceFailure == TRUE) {
            curTrace |= GT_TraceSetFailure_Enable;
        }
    }

    if (TRACECLASS != NULL) {
        Osal_printf ("Trace class %s\n", TRACECLASS) ;
        FrameQ_traceClass = simple_strtol (TRACECLASS, NULL, 16);
        if (    (FrameQ_enableTraceFailure != 1)
            &&  (FrameQ_enableTraceFailure != 2)
            &&  (FrameQ_enableTraceFailure != 3)) {
            Osal_printf ("Error! Only 1/2/3 supported for TRACECLASS\n") ;
        }
        else {
            FrameQ_traceClass =
                            FrameQ_traceClass << (32 - GT_TRACECLASS_SHIFT);
            curTrace |= FrameQ_traceClass;
        }
    }

    Osal_printf ("curTrace value: 0x%x\n", curTrace);

    Osal_printf ("FrameQ_initializeModule 0x%x\n", result);

    return result;
}


/*!
 *  @brief  Linux driver function to finalize the driver module.
 */
static
void __exit FrameQ_finalizeModule (void)
{
    Osal_printf ("FrameQ_finalizeModule\n");
}


/*!
 *  @brief  Macro calls that indicate initialization and finalization functions
 *          to the kernel.
 */
MODULE_LICENSE ("GPL v2");
module_init (FrameQ_initializeModule);
module_exit (FrameQ_finalizeModule);
#endif /* if defined (SYSLINK_MULTIPLE_MODULES) */


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
