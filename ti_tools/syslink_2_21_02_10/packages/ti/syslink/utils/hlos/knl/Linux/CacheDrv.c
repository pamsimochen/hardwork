/*
 *  @file   CacheDrv.c
 *
 *  @brief     Driver for Cache on HLOS side
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
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Trace.h>

/* Module specific header files */
#include <ti/syslink/inc/CacheDrv.h>
#include <ti/syslink/inc/CacheDrvDefs.h>

/* Linux specific header files */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>


/** ============================================================================
 *  Functions
 *  ============================================================================
 */
/*
 *	@brief	Function to invoke the APIs through ioctl.
 *
 *	@param	cmd    Command for driver ioctl
 *	@param	args	   Arguments for the ioctl command
 *
 *	@sa
 */
Int
CacheDrv_ioctl (UInt32 cmd, Ptr args) {
    CacheDrv_CmdArgs *  cargs     = (CacheDrv_CmdArgs *) args;
    CacheDrv_CmdArgs    cmdArgs;
    Int32               osStatus  = 0;
    Int32               ret;

    GT_2trace (curTrace, GT_ENTER, "CacheDrv_ioctl",
               cmd, args);

    /* set API return status as zero of void API calls*/
    cargs->apiStatus = 0;

    switch (cmd) {
        case CMD_CACHE_INV:
        {
            ret = copy_from_user (&cmdArgs.args.inv,
                                  &cargs->args.inv,
                                  sizeof (cmdArgs.args.inv));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "    CacheDrv_ioctl",
                           ret,
                           "copy_from_user call failed");
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                Cache_inv (cmdArgs.args.inv.blockPtr,
                           cmdArgs.args.inv.byteCnt,
                           cmdArgs.args.inv.type,
                           cmdArgs.args.inv.wait);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_CACHE_WB:
        {
             ret = copy_from_user (&cmdArgs.args.wb,
                                  &cargs->args.wb,
                                  sizeof (cmdArgs.args.wb));
             GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "    CacheDrv_ioctl",
                           ret,
                           "copy_from_user call failed");
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                Cache_wb (cmdArgs.args.wb.blockPtr,
                          cmdArgs.args.wb.byteCnt,
                          cmdArgs.args.wb.type,
                          cmdArgs.args.wb.wait);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;

        case CMD_CACHE_WBINV:
        {
            ret = copy_from_user (&cmdArgs.args.wbinv,
                                  &cargs->args.wbinv,
                                  sizeof (cmdArgs.args.wbinv));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "    CacheDrv_ioctl",
                           ret,
                           "copy_from_user call failed");
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                Cache_wbInv (cmdArgs.args.wbinv.blockPtr,
                             cmdArgs.args.wbinv.byteCnt,
                             cmdArgs.args.wbinv.type,
                             cmdArgs.args.wbinv.wait);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;
        case CMD_CACHE_WAIT:
        {
            Cache_wait ();
        }
        break;
        case CMD_CACHE_SETMODE:
        {
            ret = copy_from_user (&cmdArgs.args.setmode,
                                  &cargs->args.setmode,
                                  sizeof (cmdArgs.args.setmode));
            GT_assert (curTrace, (ret == 0));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ret != 0) {
                GT_setFailureReason (curTrace,
                           GT_4CLASS,
                           "    CacheDrv_ioctl",
                           ret,
                           "copy_from_user call failed");
                osStatus = -EFAULT;
            }
            else {
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
                cmdArgs.args.setmode.mode = Cache_setMode (
                                        cmdArgs.args.setmode.type,
                                        cmdArgs.args.setmode.mode);

                cmdArgs.apiStatus = Cache_S_SUCCESS;
                /* Copy the full args to the user-side. */
                ret = copy_to_user (cargs,
                                    &cmdArgs,
                                    sizeof (CacheDrv_CmdArgs));
                GT_assert (curTrace, (ret == 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
#endif /* !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        break;
        default:
        {
            /* This does not impact return status of this function, so retVal
             * comment is not used.
             */
            osStatus = Cache_E_FAIL;
            GT_setFailureReason (curTrace,
                       GT_4CLASS,
                       "CacheDrv_ioctl",
                       osStatus,
                       "Unsupported ioctl command specified");
        }
        break;

    }

    GT_1trace (curTrace, GT_LEAVE, "CacheDrv_ioctl", osStatus);

    /*! @return status Operation successfully completed. */
    return osStatus;
}

EXPORT_SYMBOL (Cache_inv);
EXPORT_SYMBOL (Cache_wb);
EXPORT_SYMBOL (Cache_wbInv);
EXPORT_SYMBOL (Cache_wait);
EXPORT_SYMBOL (Cache_setMode);
