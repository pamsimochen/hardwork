/*
 *  @file   TilerMemoryMgr.c
 *
 *  @brief   Implements  wrapper functions over Heap  APIs and Shared region
 *           address translation API. Provides API to work on shared memory
 *           over which heap and shared region is created. instance  create takes
 *           heap handle created on shared memory for allocations.
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



/*-------------------------    XDC  specific includes ----------------------  */
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IHeap.h>

/*-------------------------    Generic includes  ---------------------------- */
#include <string.h>

/*-------------------------    IPC module specific includes ------------------*/
#include <ti/sdo/ipc/_SharedRegion.h>

#include <ti/syslink/inc/SyslinkMemMgr_errBase.h>

#include <ti/syslink/inc/_SyslinkMemMgr.h>
#include <ti/syslink/inc/TilerMemoryMgr.h>

#if defined (SYSLINK_USE_TILER)
#include <ti/sdo/tiler/MemMgr.h>


#define MEMSET(addr, val, count)        \
{                                       \
    UInt i;                             \
    UInt8 *ptr = (UInt8 *)(addr);       \
                                        \
    for (i = 0; i < count; i++) {       \
        *ptr++ = val;                   \
    }                                   \
}

#endif /* #if defined (SYSLINK_USE_TILER) */

SyslinkMemMgr_Fxns SyslinkMemMgr_TilerMemory_fxns = {
    (mmgr_create)SyslinkMemMgr_TilerMemory_create,
    (mmgr_delete)SyslinkMemMgr_TilerMemory_delete,
    (mmgr_alloc)SyslinkMemMgr_TilerMemory_alloc,
    (mmgr_free)SyslinkMemMgr_TilerMemory_free,
    NULL,/*Not  supported  as heap returns virtual addreses */
    NULL,/*Not  supported. unmap is not  needed */
    (mmgr_translate)SyslinkMemMgr_TilerMemory_translate,
};

struct SyslinkMemMgr_TilerMemory_Object {
   IHeap_Handle   heapHandle;
};

/** ============================================================================
 *  External APIs
 *  ============================================================================
 */

/*
 *  ======== SyslinkMemMgr_TilerMemory_Params_init ========
 *  Function to get the default parameters of the TilerMemory SyslinkMemMgr.
 *
 *  This function can be called by the application to get their
 *  default parameters.
 *
 */
void SyslinkMemMgr_TilerMemory_Params_init(
        SyslinkMemMgr_TilerMemory_Params *params)
{
    if (params != NULL) {
        params->commonCreateParams.memMgrType = SyslinkMemMgr_TYPE_TILERMEM;
        params->commonCreateParams.size =
                sizeof(SyslinkMemMgr_TilerMemory_Params);
        strcpy((String)params->commonCreateParams.name, "");
        params->heapHandle = NULL;
    }
}


/* Function to create a SyslinkMemMgr instance */
SyslinkMemMgr_TilerMemory_Handle SyslinkMemMgr_TilerMemory_create(Ptr params)
{
    Error_Block eb;

    Error_init (&eb);

    SyslinkMemMgr_TilerMemory_Object * obj = NULL;
    SyslinkMemMgr_TilerMemory_Params *instParams =
            (SyslinkMemMgr_TilerMemory_Params *)params;

    /* Allocate Memory for the object */
    obj = (SyslinkMemMgr_TilerMemory_Object *)Memory_alloc(NULL,
            sizeof(SyslinkMemMgr_TilerMemory_Object), 0, &eb);
    if (obj != NULL) {
        obj->heapHandle = instParams->heapHandle;
    }

    return (obj);
}

/* Function to delete the created Memory Manager  instance*/
Int32 SyslinkMemMgr_TilerMemory_delete(
        SyslinkMemMgr_TilerMemory_Handle *pHandle)
{
    Int32 status = MEMMGR_SUCCESS;

    SyslinkMemMgr_TilerMemory_Object * obj = *pHandle;

    if (obj == NULL) {
        status = MEMMGR_E_FAIL;
    }
    else {
        Memory_free(NULL, (Ptr)obj, sizeof(SyslinkMemMgr_TilerMemory_Object));
        *pHandle = NULL;
    }
    return (status);
}

/* Function to allocate memory from the SyslinkMemMgr */
Ptr SyslinkMemMgr_TilerMemory_alloc(SyslinkMemMgr_TilerMemory_Object *obj,
        SyslinkMemMgr_AllocParams *allocParams)
{
    Ptr   buf = NULL;
#if defined (SYSLINK_USE_TILER)
    SyslinkMemMgr_AllocParams *instAllocParams =
            (SyslinkMemMgr_AllocParams *)allocParams;

    /* TBD: This function should be able to allocate multiple blocks*/
    MemAllocBlock memAllocBlk;

    /* Memset is neccessary before MemMgr_Alloc RCM server raises assertions otherwise*/
    MEMSET(&memAllocBlk, 0, sizeof(memAllocBlk));

    if (instAllocParams->block[0].pixelFormat < ((1 << (sizeof(pixel_fmt_t) * 8)) -1)) {
        memAllocBlk.pixelFormat = (pixel_fmt_t)instAllocParams->block[0].pixelFormat;
    }
    else {
        obj= NULL;
        System_printf("\nThe pixelFormat value for SyslinkMemMgr_AllocParams is too big\n");
    }
    memAllocBlk.dim.area.width  = instAllocParams->block[0].dim.area.width;
    memAllocBlk.dim.area.height = instAllocParams->block[0].dim.area.height;

    if (obj != NULL) {
        buf = MemMgr_Alloc(&memAllocBlk,
                           instAllocParams->tilerBlocks);
        if(buf == NULL){
            System_printf("\nTiler MemMgr_Alloc failed\n");
        }
        else {
            instAllocParams->block[0].stride = memAllocBlk.stride;
        }
    }
#endif /* #if defined (SYSLINK_USE_TILER) */
    return (buf);
}

/* Function to allocate memory from the SyslinkMemMgr */
Int32 SyslinkMemMgr_TilerMemory_free(SyslinkMemMgr_TilerMemory_Object *obj,
        Ptr buf, UInt32 size)
{
    Int32 status = MEMMGR_SUCCESS;
#if defined (SYSLINK_USE_TILER)
    if (obj == NULL) {
        status = MEMMGR_E_FAIL;
    }
    else {
        MemMgr_Free(buf);
    }
#endif /* #if defined (SYSLINK_USE_TILER) */
    return (status);
}

/* Function to allocate memory from the SyslinkMemMgr.Calls sharedregion API to do
 * address translations
 */
Ptr SyslinkMemMgr_TilerMemory_translate(SyslinkMemMgr_TilerMemory_Object *obj,
        Ptr srcAddr, SyslinkMemMgr_AddrType srcAddrType,
        SyslinkMemMgr_AddrType desAddrType)
{
    Ptr       dstAddr = NULL;

    /* Currently translation APIs are not provided by MemMgr so return the same pointer*/

    dstAddr = srcAddr;

    return (dstAddr);

}
