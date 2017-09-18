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
 *  @file   ti/syslink/inc/knl/BuddyPageAllocator.h
 *
 *  @brief      Buddy Page block allocator defines.
 *
 *              The buddy memory allocation technique is a memory allocation
 *              technique that divides memory into partitions to try to satisfy
 *              a memory request as suitably as possible. This system makes use
 *              of splitting memory into halves to try to give a best-fit.
 *              The buddy memory allocation technique allocates memory in powers
 *              of 2^12(page) * x, i.e 2 * 2^12, where x is an integer. The
 *              given memory block must be page aligned. Number of block size
 *              possible is 1 page, 2page, ....., x Page. This sizes are called
 *              super blocks.
 *              So initially there is only one super block of size x page. If
 *              a request comes for 1 page, then x page block is divided into
 *              1 page and (x - 1)page blocks. 1 page block is given back to
 *              user and (x - 1) is attached to (x - 1) super block. hence on.
 *              When a request fails, buddies are joined back to form a big block.
 */

#ifndef BPA_H_
#define BPA_H_


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @brief  Unique module ID.
 */
#define BPA_MODULEID      (0xCD41)

/*!
 *  @brief  Error code base for buddy page allocator module.
 */
#define BPA_STATUSCODEBASE (BPA_MODULEID << 12u)

/*!
 *  @brief  Macro to make error code.
 */
#define BPA_MAKE_FAILURE(x) ((Int) (  0x80000000                               \
                                    + BPA_STATUSCODEBASE                       \
                                    + (x)))

/*!
 *  @brief  Macro to make success code.
 */
#define BPA_MAKE_SUCCESS(x) (BPA_STATUSCODEBASE + (x))

/*!
 *  @brief  Argument passed to a function is invalid.
 */
#define BPA_E_INVALIDARG      BPA_MAKE_FAILURE(1)

/*!
 *  @brief  Memory allocation failed.
 */
#define BPA_E_MEMORY          BPA_MAKE_FAILURE(2)

/*!
 *  @brief  Operation successful.
 */
#define BPA_SUCCESS           BPA_MAKE_SUCCESS(0)

/*!
 *  @brief  Structure defining config parameters for the allocator.
 */
struct BpaConfig {
    UInt32  virtBaseAddr;    /*!< Base address in virtual */
    UInt32  physBaseAddr;    /*!< Physical base address */
    UInt32  size;            /*!< Size of the region */
    UInt32  pageSize;        /*!< Page size */
};


Ptr  Bpa_create (struct BpaConfig * config);
void Bpa_delete (Ptr bpaHandle);
Ptr  Bpa_alloc (Ptr bpaHandle, UInt32 size);
Int  Bpa_free (Ptr bpaHandle, Ptr blk, UInt32 size);
Bool Bpa_checkBlockOwnership (Ptr bpaHandle, Ptr blk);

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /* !defined (BBA_H) */
