/*
 *  @file   MemoryOS.c
 *
 *  @brief      Bios Memory interface implementation.
 *
 *              This abstracts the Memory mapping interface
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



/*-------------------------   Standard headers   ---------------------------  */
#include <xdc/std.h>

/*-------------------------   OSAL and utils   -----------------------------  */
#include <ti/syslink/utils/Trace.h>
/*-------------------------   Module level includes   ----------------------  */
#include <ti/syslink/utils/_Memory.h>
#include <ti/syslink/utils/_MemoryOS.h>


#if defined (__cplusplus)
extern "C" {
#endif

/*!
 *  @brief      Maps a memory area into virtual space.
 *  @sa         MemoryOS_unmap
 */
Int MemoryOS_map (MemoryOS_MapInfo * mapInfo)
{
    Int status = MEMORYOS_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "MemoryOS_map", mapInfo);

    GT_assert (curTrace, (NULL != mapInfo));

    mapInfo->dst = mapInfo->src;

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_map", status);
    /*! @retval MemoryOS_S_SUCCESS Operation completed successfully. */
    return (status);
}

/*!
 *  @brief      Unmaps a memory area into virtual space.
 *  @sa         MemoryOS_unmap
 */
Int MemoryOS_unmap (MemoryOS_UnmapInfo * unmapInfo)
{
    Int status = MEMORYOS_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "MemoryOS_unmap", unmapInfo);

    GT_assert (curTrace, (NULL != unmapInfo));

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_unmap", status);

    /*! @retval MemoryOS_S_SUCCESS Operation completed successfully. */
    return status;

}

/* Function to translate an address. */
Ptr
Memory_translate (Ptr srcAddr, Memory_XltFlags flags)
{
    Ptr buf = NULL;

    GT_2trace (curTrace, GT_ENTER, "Memory_tranlate", srcAddr, flags);

    buf = srcAddr;

    GT_1trace (curTrace, GT_LEAVE, "Memory_tranlate", buf);

    return buf;
}

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */
