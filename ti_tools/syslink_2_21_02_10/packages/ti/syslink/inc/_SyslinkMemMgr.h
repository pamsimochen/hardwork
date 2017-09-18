/** 
 *  @file   _SyslinkMemMgr.h
 *
 *  @brief   Implements internal defines and macros for SyslinkMemMgr.
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



#ifndef __MEMMGR_H_
#define __MEMMGR_H_
/* Internal header file  */
#include <ti/syslink/utils/SyslinkMemMgr.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*! @brief Type for function pointer to create function */
typedef Ptr
(*mmgr_create) (Ptr createParams);

/*! @brief Type for function pointer to delete function */
typedef Int32
(*mmgr_delete) (Ptr * handlePtr);

/*! @brief Type for function pointer to alloc function */
typedef Ptr
(*mmgr_alloc) (Ptr handle, SyslinkMemMgr_AllocParams *params);

/*! @brief Type for function pointer to free function */
typedef Int32
(*mmgr_free) (Ptr handle, Ptr ptr, UInt32 size);

/*! @brief Type for function pointer to map function */
typedef Ptr
(*mmgr_map) (Ptr handle, Ptr arg);

/*! @brief Type for function pointer to unmap function */
typedef Int32
(*mmgr_unmap) (Ptr handle, Ptr arg);

/*! @brief Type for function pointer to address translate function */
typedef Ptr
(*mmgr_translate) (Ptr handle,
                   Ptr buf,
                   SyslinkMemMgr_AddrType            srcAddrType,
                   SyslinkMemMgr_AddrType            desAddrType);

/* Structure defining Interface functions for SyslinkMemMgr */
typedef struct SyslinkMemMgr_Fxns_tag {
    mmgr_create     create;
    mmgr_delete     deleteInstance;
    mmgr_alloc      alloc;
    mmgr_free       free;
    mmgr_map        map;
    mmgr_unmap      unmap;
    mmgr_translate  translate;
}SyslinkMemMgr_Fxns;

#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */


#endif /*_MEMMGR_H*/
