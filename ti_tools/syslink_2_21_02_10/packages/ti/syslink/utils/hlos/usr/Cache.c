/*
 *  @file   Cache.c
 *
 *  @brief      Cache API implementation
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


/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Kernel Utils headers */
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/CacheDrv.h>
#include <ti/syslink/inc/CacheDrvDefs.h>


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/*!
 *  @brief         Function to invalidate the Cache module.
 *  @param      block pointer.
 *  @param      byte count
 *  @param      cache type.
 *  @param      wait flag.
 */
Void Cache_inv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {

    Int               status = Cache_S_SUCCESS;
    CacheDrv_CmdArgs  cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "Cache_inv", blockPtr, byteCnt, type, wait);

    /* TBD: Protect from multiple threads. */
    cmdArgs.args.inv.blockPtr = blockPtr;
    cmdArgs.args.inv.byteCnt  = byteCnt;
    cmdArgs.args.inv.type     = type;
    cmdArgs.args.inv.wait     = wait;

    status = CacheDrv_ioctl (CMD_CACHE_INV, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Cache_inv",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Cache_inv");
}


/*!
 *  @brief         Function to write back the Cache module.
 *  @param      block pointer.
 *  @param      byte count
 *  @param      cache type.
 *  @param      wait flag.
 */
Void Cache_wb(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    Int               status = Cache_S_SUCCESS;
    CacheDrv_CmdArgs  cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "Cache_wb", blockPtr, byteCnt, type, wait);

    /* TBD: Protect from multiple threads. */
    cmdArgs.args.wb.blockPtr = blockPtr;
    cmdArgs.args.wb.byteCnt  = byteCnt;
    cmdArgs.args.wb.type     = type;
    cmdArgs.args.wb.wait     = wait;

    status = CacheDrv_ioctl (CMD_CACHE_WB, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Cache_wb",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Cache_wb");
}


/*!
 *  @brief         Function to write back and invalidate the Cache module.
 *  @param      block pointer.
 *  @param      byte count
 *  @param      cache type.
 *  @param      wait flag.
 */
Void Cache_wbInv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    Int               status = Cache_S_SUCCESS;
    CacheDrv_CmdArgs  cmdArgs;

    GT_4trace (curTrace, GT_ENTER, "Cache_wbInv", blockPtr, byteCnt, type, wait);

    /* TBD: Protect from multiple threads. */
    cmdArgs.args.wbinv.blockPtr = blockPtr;
    cmdArgs.args.wbinv.byteCnt  = byteCnt;
    cmdArgs.args.wbinv.type     = type;
    cmdArgs.args.wbinv.wait     = wait;

    status = CacheDrv_ioctl (CMD_CACHE_WBINV, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Cache_wbInv",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Cache_wbInv");
}


/*!
 *  @brief         Function to wait for cache operation to get complete.
 */
Void Cache_wait(Void) {
    Int               status = Cache_S_SUCCESS;
    CacheDrv_CmdArgs  cmdArgs;

    GT_0trace (curTrace, GT_ENTER, "Cache_wait");

    status = CacheDrv_ioctl (CMD_CACHE_WAIT, &cmdArgs);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Cache_wait",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Cache_wait");
}


/*!
 *  @brief         Function to set the mode of Cache module.
 *  @param      cache type.
 *  @param      cache mode.
 */
enum Cache_Mode Cache_setMode(Bits16 type, enum Cache_Mode mode) {
    enum Cache_Mode   retMode = Cache_Mode_FREEZE;
    CacheDrv_CmdArgs  cmdArgs;
    Int32             status  = 0;

    GT_0trace (curTrace, GT_ENTER, "Cache_setMode");

    /* TBD: Protect from multiple threads. */
    cmdArgs.args.setmode.type = type;
    cmdArgs.args.setmode.mode = mode;

    status = CacheDrv_ioctl (CMD_CACHE_SETMODE, &cmdArgs);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Cache_setMode",
                             status,
                             "API (through IOCTL) failed on kernel-side!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    retMode = cmdArgs.args.setmode.mode;

    GT_1trace (curTrace, GT_LEAVE, "Cache_setMode", retMode);
    /*! @return new cache mode val after successful opperation */
    return (retMode);
}
