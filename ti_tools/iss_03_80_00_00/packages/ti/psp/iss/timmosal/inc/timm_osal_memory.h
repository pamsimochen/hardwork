/*
 *  Copyright (c) 2010-2011, Texas Instruments Incorporated
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
 *
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

/*
*  @file timm_osal_memory.h
*  The osal header file defines 
*  @path
*
*/
/* -------------------------------------------------------------------------- */
/* =========================================================================
 *!
 *! Revision History
 *! ===================================
 *! 1-June-2009 Aditya Monga: admonga@ti.com Added comments.
 *! 0.1: Created the first draft version, ksrini@ti.com
 * ========================================================================= */

#ifndef _TIMM_OSAL_MEMORY_H_
#define _TIMM_OSAL_MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*******************************************************************************
* Includes
*******************************************************************************/

#include "timm_osal_types.h"
#include "timm_osal_error.h"

#include <ti/psp/iss/alg/jpeg_enc/inc/msp.h>


/* Enumeration Flag for Memory Segmenation Id */
typedef enum TIMMOSAL_MEM_SEGMENTID {

   TIMMOSAL_MEM_SEGMENT_EXT = 0,
   TIMMOSAL_MEM_SEGMENT_INT,
   TIMMOSAL_MEM_SEGMENT_UNCACHED 

}TIMMOSAL_MEM_SEGMENTID;


/*******************************************************************************
* External interface
*******************************************************************************/

/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_CreateMemoryPool() - Create memory pool.
 *
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_CreateMemoryPool (void);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_DeleteMemoryPool() - Delete memory pool.
 *
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_DeleteMemoryPool (void);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_TIMM_OSAL_MallocExtn() - Allocate a chunk of memory.
 *
 *  @ param size             :Size of the memory to be allocated.
 *
 *  @ param bBlockContiguous :Whether a physically contiguous block is required.
 *
 *  @ param unBlockAlignment :Alignment of the memory chunk that will be 
 *                             allocated.
 *
 *  @ param tMemSegId        :Memory segment from wich memory will be allocated.
 *
 *  @ param hHeap            :Heap from which memory will be allocated.
 */
/* ===========================================================================*/
TIMM_OSAL_PTR TIMM_OSAL_MallocExtn(TIMM_OSAL_U32 size, 
                                   TIMM_OSAL_BOOL bBlockContiguous, 
                                   TIMM_OSAL_U32 unBlockAlignment, 
                                   TIMMOSAL_MEM_SEGMENTID tMemSegId,
							       TIMM_OSAL_PTR hHeap);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_Free() - Free a previously allocated chunk of memory. 
 *
 *  @ param pData        :Pointer to the memory chunk that has to be freed.
 */
/* ===========================================================================*/
void TIMM_OSAL_Free (TIMM_OSAL_PTR pData);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_Memset() - Sets each byte in the given chunk of memory to the
 *                          given value.
 *
 *  @ param pBuffer        :Pointer to the start of the memory chunk.
 *
 *  @ param uValue         :Value to be set.
 *
 *  @ param uSize          :Size from the start pointer till which the value 
 *                          will be set.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memset (TIMM_OSAL_PTR pBuffer, 
                                      TIMM_OSAL_U8 uValue, TIMM_OSAL_U32 uSize);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_Memcmp() - Compare two chunks of memory.
 *
 *  @ param pBuffer1        :Pointer to the start of the 1st memory chunk.
 *
 *  @ param pBuffer2        :Pointer to the start of the 2nd memory chunk.
 *
 *  @ param uSize           :Size from the start pointer till the comparison 
 *                           will be done.
 */
/* ===========================================================================*/
TIMM_OSAL_S32 TIMM_OSAL_Memcmp (TIMM_OSAL_PTR pBuffer1, TIMM_OSAL_PTR pBuffer2, 
                                TIMM_OSAL_U32 uSize);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_Memcpy() - Copy a memory chunk.
 *
 *  @ param pBufDst        :Destination memory address.
 *
 *  @ param pBufSrc        :Source memory address.
 *
 *  @ param uSize          :Size of memory to be copied.
 */
/* ===========================================================================*/
TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memcpy (TIMM_OSAL_PTR pBufDst, 
                                      TIMM_OSAL_PTR pBufSrc, 
                                      TIMM_OSAL_U32 uSize);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_GetMemCounter() - Returns the current number of allocted 
 *                                 memory chunks.
 *
 */
/* ===========================================================================*/
TIMM_OSAL_U32 TIMM_OSAL_GetMemCounter(void);



/* ===========================================================================*/
/**
 * @fn TIMM_OSAL_GetMemUsage() - Returns current value of the total memory size 
 *                               allocated.
 *
 */
/* ===========================================================================*/
TIMM_OSAL_U32 TIMM_OSAL_GetMemUsage(void);



/* Calling legacy TIMM_OSAL_Malloc will call TIMM_OSAL_MallocExtn with NULL as 
the heap handle */
#define TIMM_OSAL_Malloc(size, bBlockContiguous, unBlockAlignment, tMemSegId) \
 TIMM_OSAL_MallocExtn(size, bBlockContiguous, unBlockAlignment, tMemSegId, NULL)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TIMM_OSAL_DEFINES_H_ */
