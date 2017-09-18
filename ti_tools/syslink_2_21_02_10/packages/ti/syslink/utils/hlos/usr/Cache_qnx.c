/*
 *  @file   Cache_qnx.c
 *
 *  @brief      Cache API implementation for QNX user mode
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
#include <sys/mman.h>
#include <sys/neutrino.h>

#include <string.h>

#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Cache.h>
#include <ti/syslink/utils/_Cache_qnx.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>


/* 
 * Uncomment the following line if you want higher performance when performing
 * cache maintenance operations on SharedRegions. Note that it requires
 * _NTO_TCTL_IO privilege in the user application process that calls SysLink.
 * See QNX documentation for more details on how to enable _NTO_TCTL_IO 
 * privileges.
 */
/* #define SYSLINK_USE_QNX_CACHE_API */

#ifdef SYSLINK_USE_QNX_CACHE_API

/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
Cache_ModuleObject Cache_module;


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Initialize the cache module. */
Int32 Cache_setup (void) {
    Int32 status = Cache_S_SUCCESS;
    
    GT_0trace (curTrace, GT_ENTER, "Cache_setup");

    if (Atomic_inc_return((Atomic *)&(Cache_module.refCount)) != 1) {
        status = Cache_S_ALREADYSETUP;
        GT_1trace (curTrace,
                   GT_1CLASS,
                   "Cache module has been already setup in this process.\n"
                   "    RefCount: [%d]\n",
                   Cache_module.refCount);
    }
    else {
    
        /* 
         * Using QNX Cache API in user process requires I/O privileges and root 
         * access. Enable it here if not done elsewhere in user application.
         */
        /* ThreadCtl( _NTO_TCTL_IO, 0 ); */
        
        memset(&Cache_module.cinfo, 0, sizeof(Cache_module.cinfo));
        
        if (cache_init(0, &Cache_module.cinfo, NULL) != 0) {
            status = Cache_E_FAIL;
            GT_0trace (curTrace,
                       GT_2CLASS,
                       "cache_init failed!");
        }
    }
        
    return (status);
}

/* Finalize the cache module. */
Int32 Cache_destroy (void) {
    Int32 status = Cache_S_SUCCESS;
    
    GT_0trace (curTrace, GT_ENTER, "Cache_destroy");

    if (Atomic_dec_return((Atomic *)&(Cache_module.refCount)) == 0) {
        if (cache_fini(&Cache_module.cinfo) != 0) {
            status = Cache_E_FAIL;
            GT_0trace (curTrace,
                       GT_2CLASS,
                       "cache_fini failed!");
        }
    }
    
    return (status);
}

/* Function to invalidate the Cache module */
Void Cache_inv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    uint32_t offset;
   
    GT_4trace (curTrace, GT_ENTER, "Cache_inv", blockPtr, byteCnt, type, wait);

    offset = (uint32_t)Memory_translate(
                        blockPtr,
                        Memory_XltFlags_Virt2Phys);

    CACHE_INVAL(&Cache_module.cinfo, blockPtr, (uint64_t)offset, byteCnt);
  
    GT_0trace (curTrace, GT_LEAVE, "Cache_inv");
}

/* Function to write back the Cache module */
Void Cache_wb(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    uint32_t offset;
    
    GT_4trace (curTrace, GT_ENTER, "Cache_wb", blockPtr, byteCnt, type, wait);

    offset = (uint32_t)Memory_translate(
                        blockPtr,
                        Memory_XltFlags_Virt2Phys);

    CACHE_FLUSH(&Cache_module.cinfo, blockPtr, (uint64_t)offset, byteCnt);
    
    GT_0trace (curTrace, GT_LEAVE, "Cache_wb");
}

/* Function to write back invalidate the Cache module */
Void Cache_wbInv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {

    GT_4trace (curTrace, GT_ENTER, "Cache_wbInv", blockPtr, byteCnt, type, wait);

    Cache_wb(blockPtr, byteCnt, type, wait);
    Cache_inv(blockPtr, byteCnt, type, wait);
    
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
    return (returnVal);
}

#else /* default behavior is to use msync, which does not require IO priv. */

/*!
 *  @brief      Function to setup the Cache module.
 */
Int
Cache_setup (Void)
{
    Int              status = Cache_S_SUCCESS;

    return (status);
}

/*!
 *  @brief      Function to destroy the Cache module.
 */
Int
Cache_destroy (void)
{
    Int                status = Cache_S_SUCCESS;

    return (status);
}

/* Function to invalidate the Cache module */
Void Cache_inv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    GT_4trace (curTrace, GT_ENTER, "Cache_inv", blockPtr, byteCnt, type, wait);
     
    msync(blockPtr, byteCnt, MS_CACHE_ONLY | MS_INVALIDATE);
     
    GT_0trace (curTrace, GT_LEAVE, "Cache_inv");
}

/* Function to write back the Cache module */
Void Cache_wb(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    GT_4trace (curTrace, GT_ENTER, "Cache_wb", blockPtr, byteCnt, type, wait);
                      
    msync(blockPtr, byteCnt, MS_CACHE_ONLY | MS_SYNC);
        
    GT_0trace (curTrace, GT_LEAVE, "Cache_wb");
}

/* Function to write back invalidate the Cache module */
Void Cache_wbInv(Ptr blockPtr, UInt32 byteCnt, Bits16 type, Bool wait) {
    GT_4trace (curTrace, GT_ENTER, "Cache_wbInv", blockPtr, byteCnt, type, wait);

    msync(blockPtr, byteCnt, MS_CACHE_ONLY | MS_SYNC | MS_INVALIDATE);
        
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
    return (returnVal);
}

#endif  /* if !defined(SYSLINK_CACHE_IN_USER_PROCESS) */
