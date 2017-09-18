/*
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
 */

/**
 *  @file   ti/syslink/family/hlos/knl/omap3530/Linux/Cache.c
 *
 *  @brief  Cache API implementation for OMAP3530 platform
 */

/* Standard headers */
#include <linux/version.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>

/* Module level headers */
#include <ti/syslink/inc/CacheDrv.h>
#include <ti/syslink/inc/CacheDrvDefs.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*
 * The following macros control version-dependent code:
 * USE_CACHE_VOID_ARG - #define if dmac functions take "void *" parameters,
 *    otherwise unsigned long is used
 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
#define USE_CACHE_VOID_ARG
#else  /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */
#undef USE_CACHE_VOID_ARG
#endif /* LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18) */


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Function to invalidate the Cache module */
Void Cache_inv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    GT_4trace (curTrace, GT_ENTER, "Cache_inv", blockPtr, byteCnt, type, wait);

#ifdef USE_CACHE_VOID_ARG
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
    dmac_map_area(blockPtr, (size_t)byteCnt, DMA_FROM_DEVICE);
    outer_inv_range(__pa((UInt32)blockPtr),
                    __pa((UInt32)(blockPtr + byteCnt)) );
#else
    dmac_inv_range(blockPtr, (blockPtr + byteCnt) );
#endif
#else
    dmac_inv_range( (UInt32)blockPtr, (UInt32)(blockPtr + byteCnt) );
#endif

    GT_0trace (curTrace, GT_LEAVE, "Cache_inv");
}

/* Function to write back the Cache module */
Void Cache_wb(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    GT_4trace (curTrace, GT_ENTER, "Cache_wb", blockPtr, byteCnt, type, wait);

#ifdef USE_CACHE_VOID_ARG
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
    dmac_map_area(blockPtr, (size_t)byteCnt, DMA_TO_DEVICE);
    outer_clean_range(__pa((UInt32)blockPtr),
                      __pa((UInt32)(blockPtr+byteCnt)) );
#else
    dmac_clean_range(blockPtr, (blockPtr+byteCnt) );
#endif
#else
    dmac_clean_range( (UInt32)blockPtr, (UInt32)(blockPtr + byteCnt) );
#endif

    GT_0trace (curTrace, GT_LEAVE, "Cache_wb");
}

/* Function to write back invalidate the Cache module */
Void Cache_wbInv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    GT_4trace (curTrace, GT_ENTER, "Cache_wbInv", blockPtr, byteCnt, type, wait);

#ifdef USE_CACHE_VOID_ARG
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
                dmac_map_area(blockPtr, (size_t)byteCnt, DMA_BIDIRECTIONAL);
                outer_flush_range(__pa((UInt32)blockPtr),
                                  __pa((UInt32)(blockPtr+byteCnt)) );
#else
                dmac_flush_range(blockPtr, (blockPtr+byteCnt) );
#endif
#else
                dmac_flush_range( (UInt32)blockPtr, (UInt32)(blockPtr + byteCnt) );
#endif

    GT_0trace (curTrace, GT_LEAVE, "Cache_wbInv");
}

/* Function to write back invalidate the Cache module */
Void Cache_wait(Void) {
    GT_0trace (curTrace, GT_ENTER, "Cache_wait");

    GT_0trace (curTrace, GT_LEAVE, "Cache_wait");
}

/* Function to set the mode of Cache module */
enum Cache_Mode Cache_setMode(Bits16 type, enum Cache_Mode mode)
{
    enum Cache_Mode returnVal = Cache_Mode_FREEZE;
    GT_2trace (curTrace, GT_ENTER, "Cache_setMode", type, mode);

    GT_1trace (curTrace, GT_LEAVE, "Cache_setMode", returnVal);
    return returnVal;
}
