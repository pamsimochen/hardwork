/*
 *  @file   SharedMemoryMgr.c
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
#include <ti/syslink/inc/SharedMemoryMgr.h>

SyslinkMemMgr_Fxns SyslinkMemMgr_SharedMemory_fxns = {
    (mmgr_create)SyslinkMemMgr_SharedMemory_create,
    (mmgr_delete)SyslinkMemMgr_SharedMemory_delete,
    (mmgr_alloc)SyslinkMemMgr_SharedMemory_alloc,
    (mmgr_free)SyslinkMemMgr_SharedMemory_free,
    NULL,/*Not  supported  as heap returns virtual addreses */
    NULL,/*Not  supported. unmap is not  needed */
    (mmgr_translate)SyslinkMemMgr_SharedMemory_translate,
};

struct SyslinkMemMgr_SharedMemory_Object{
   IHeap_Handle   heapHandle;
};

/** ============================================================================
 *  External APIs
 *  ============================================================================
 */

/*
 *  ======== SyslinkMemMgr_SharedMemory_Params_init ========
 *  Function to get the default parameters of the SharedMemory SyslinkMemMgr.
 *
 *  This function can be called by the application to get their
 *  default parameters.
 *
 */
void SyslinkMemMgr_SharedMemory_Params_init(
        SyslinkMemMgr_SharedMemory_Params *params)
{
    if (params != NULL) {
        params->commonCreateParams.memMgrType = SyslinkMemMgr_TYPE_SHAREDMEM;
        params->commonCreateParams.size =
                sizeof(SyslinkMemMgr_SharedMemory_Params);
        strcpy((String)params->commonCreateParams.name, "");
        params->heapHandle = NULL;
    }
}

/* Function to create a SyslinkMemMgr instance */
SyslinkMemMgr_SharedMemory_Handle SyslinkMemMgr_SharedMemory_create(Ptr params)
{
    Error_Block eb;

    Error_init (&eb);

    SyslinkMemMgr_SharedMemory_Object * obj = NULL;
    SyslinkMemMgr_SharedMemory_Params *instParams =
            (SyslinkMemMgr_SharedMemory_Params *)params;

    /* Allocate Memory for the object */
    obj = (SyslinkMemMgr_SharedMemory_Object *)Memory_alloc(NULL,
            sizeof(SyslinkMemMgr_SharedMemory_Object), 0, &eb);
    if (obj != NULL) {
        obj->heapHandle = instParams->heapHandle;
    }

    return (obj);
}

/* Function to delete the created Memory Manager  instance*/
Int32 SyslinkMemMgr_SharedMemory_delete(
        SyslinkMemMgr_SharedMemory_Handle *pHandle)
{
    Int32 status = MEMMGR_SUCCESS;

    SyslinkMemMgr_SharedMemory_Object * obj = *pHandle;

    if (obj == NULL) {
        status = MEMMGR_E_FAIL;
    }
    else {
        Memory_free(NULL, (Ptr)obj, sizeof(SyslinkMemMgr_SharedMemory_Object));
        *pHandle = NULL;
    }
    return (status);
}

/* Function to allocate memory from the SyslinkMemMgr */
Ptr SyslinkMemMgr_SharedMemory_alloc(SyslinkMemMgr_SharedMemory_Object *obj,
        SyslinkMemMgr_AllocParams *allocParams)
{
    Ptr   buf = NULL;
    Error_Block eb;

    Error_init (&eb);

    if (obj != NULL) {
        buf = Memory_alloc(obj->heapHandle, allocParams->size,
                allocParams->align, &eb);
    }

    return (buf);
}

/* Function to allocate memory from the SyslinkMemMgr */
Int32 SyslinkMemMgr_SharedMemory_free (SyslinkMemMgr_SharedMemory_Object *obj,
        Ptr buf, UInt32 size)
{
    Int32 status = MEMMGR_SUCCESS;

    if (obj == NULL) {
        status = MEMMGR_E_FAIL;
    }
    else {
        Memory_free((IHeap_Handle)obj->heapHandle, buf, size);
    }

    return (status);
}

/* Function to allocate memory from the SyslinkMemMgr.Calls sharedregion API to do
 * address translations
 */
Ptr SyslinkMemMgr_SharedMemory_translate(SyslinkMemMgr_SharedMemory_Object *obj,
        Ptr srcAddr, SyslinkMemMgr_AddrType srcAddrType,
        SyslinkMemMgr_AddrType desAddrType)
{
    Ptr       dstAddr = NULL;
    Int32     index;

    if (obj != NULL) {
        if (srcAddrType == desAddrType) {
            /* Return NULL if both addr types are same */
            dstAddr = srcAddr;
        }
        else {
            switch (srcAddrType) {
                case SyslinkMemMgr_AddrType_Virtual:
                    if (desAddrType == SyslinkMemMgr_AddrType_Portable) {
                        index = SharedRegion_getId((Ptr)srcAddr);
                        dstAddr = (Ptr)SharedRegion_getSRPtr((Ptr)srcAddr,
                                index);
                        if (dstAddr == (Ptr)ti_sdo_ipc_SharedRegion_INVALIDSRPTR) {
                            dstAddr = NULL;
                        }
                    }
                    break;

                case SyslinkMemMgr_AddrType_Portable:
                    if (desAddrType == SyslinkMemMgr_AddrType_Virtual) {
                        dstAddr = SharedRegion_getPtr(
                                (SharedRegion_SRPtr)srcAddr);
                    }
                    break;

                default:
                    dstAddr = NULL;
                    break;
            }
        }
    }

    return (dstAddr);
}
