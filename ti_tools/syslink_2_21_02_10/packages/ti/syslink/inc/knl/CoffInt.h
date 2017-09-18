/** 
 *  @file   CoffInt.h
 *
 *  @brief      Defines internal generic functions and macros of COFF parser.
 *
 *
 */
/* 
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



#ifndef CoffInt_H_0xcb32
#define CoffInt_H_0xcb32


/* Module headers */
#include "LoaderDefs.h"


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @brief  Record size to be specified when reading file through file read
 */
#define READ_REC_SIZE   1u

/*!
 *  @brief  Macro to swap bytes within a 16 bit word.
 */
#define BYTESWAP_WORD(x) (UInt16) (( (UInt16) ((((UInt16)(x)) << 8u) & 0xFF00u) \
                                   | (UInt16)((((UInt16)(x)) >> 8u) & 0x00FFu)))

/*!
 *  @brief  Macro to swap bytes within a 32 bit dword.
 */
#define BYTESWAP_LONG(x) (   (((x) << 24u) & 0xFF000000u)    \
                          |  (((x) <<  8u) & 0x00FF0000u)   \
                          |  (((x) >>  8u) & 0x0000FF00u)   \
                          |  (((x) >> 24u) & 0x000000FFu))

/*!
 *  @brief  Macro to swap two 16-bit values within a 32 bit dword.
 */
#define WORDSWAP_LONG(x) (   (((x) << 16u) & 0xFFFF0000u)    \
                          |  (((x) >> 16u) & 0x0000FFFFu))

/*!
 *  @brief  Returns a word-swapped or non-swapped dword based on the parameter
 *          passed.
 */
#define SWAP_LONG(value, swap) (((swap) == FALSE) ?          \
                                (value) : WORDSWAP_LONG (value))

/* =============================================================================
 *  Functions
 * =============================================================================
 */
/* Reads an Int8 from file. */
Int8 Coff_read8 (Loader_Object * obj, Ptr fileDesc);

/* Reads an Int16 from file. */
Int16 Coff_read16 (Loader_Object * obj, Ptr fileDesc, Bool swap);

/* Reads an Int16 from file. */
Int32 Coff_read32 (Loader_Object * obj, Ptr fileDesc, Bool swap);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* CoffInt_H_0xcb32 */
