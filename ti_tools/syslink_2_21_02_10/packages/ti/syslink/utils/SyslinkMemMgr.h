/** 
 *  @file   SyslinkMemMgr.h
 *
 *  @brief  Header file implementing Memory manager. It is a wrapper
 *          implementation to support Shared memory allocation using heap
 *          address translations using shared region or Tiler buffer
 *          allocation using tiler buffer manager and address translations.
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



#ifndef MEMMGR_H
#define MEMMGR_H


#if defined (__cplusplus)
extern "C" {
#endif

#define MEMMGR_MAX_NAME_LENGTH 32u

#define MEMMGR_MAX_TILER_BLOCKS 10u

/* =============================================================================
 * Structures & Enums
 * =============================================================================
 */

/*!
 *  @brief  Enumerations to indicate address types used for translation
 */
typedef enum {
    SyslinkMemMgr_AddrType_Virtual = 0u,
    /*!< Virtual address on calling process on DSP where MMU is not configured
     *   it could be physical address.
     */
    SyslinkMemMgr_AddrType_Portable = 1u,
    /*!< This is the shared region address incase sharedmemory manager plugged
     *in to  frameQbufMgr. In case of tiler it could be the tiler specific
     *portable address*/
    SyslinkMemMgr_AddrType_EndValue = 2u
    /*!< End delimiter indicating start of invalid values for this enum */
} SyslinkMemMgr_AddrType;

/*!
 *  @brief  enum for denoting the different types of SyslinkMemMgr implementations.
 */
typedef enum SyslinkMemMgr_Type_Type_tag {
    SyslinkMemMgr_TYPE_SHAREDMEM = 0x0,
    SyslinkMemMgr_TYPE_TILERMEM  = 0x1
}SyslinkMemMgr_Type;


/*!
 *  @brief  Structure defining common  create parameters for the Memory manager
 *          module.Should be the first element in implementation specific create
 *          params.
 */
typedef struct SyslinkMemMgr_CreateParams_tag {
    UInt32          size;
     /*!< Size of the param structure  */
    SyslinkMemMgr_Type     memMgrType;
    /*!< type of implementation.*/
    UInt8           name[MEMMGR_MAX_NAME_LENGTH];
     /*!< Name of the instance */
} SyslinkMemMgr_CreateParams;

#define SyslinkMemMgr_Params SyslinkMemMgr_CreateParams

typedef struct SyslinkMemMgr_AllocBlock_tag {
    UInt16 pixelFormat;
    union {
        struct {
            UInt16 width;
            UInt16 height;
        } area;
        UInt16  len;
    } dim;
    UInt32 stride;
} SyslinkMemMgr_AllocBlock;


/*!
 *  @brief  Alloc params
 */
typedef struct SyslinkMemMgr_AllocParams_tag {
    UInt32 size;
    UInt32 align;
    SyslinkMemMgr_AllocBlock block[MEMMGR_MAX_TILER_BLOCKS];
    UInt32 tilerBlocks;
} SyslinkMemMgr_AllocParams;


/*!
 *  @brief  Forward declaration of structure defining object for the
 *          MemoryManger.
 */
typedef struct SyslinkMemMgr_Object SyslinkMemMgr_Object;

/*!
 *  @brief  Handle for the MemoryManager instance.
 */
typedef SyslinkMemMgr_Object * SyslinkMemMgr_Handle;

/* =============================================================================
 * APIs
 * =============================================================================
 */
/* Function to setup the SyslinkMemMgr module */
Int32 SyslinkMemMgr_setup(Void);

/* Function to setup the SyslinkMemMgr module */
Int32 SyslinkMemMgr_destroy(Void);

/* Function to create a SyslinkMemMgr instance */
SyslinkMemMgr_Handle SyslinkMemMgr_create(Ptr params);

/* Function to delete the created Memory Manager  instance*/
Int32 SyslinkMemMgr_delete(SyslinkMemMgr_Handle * pHandle);


/* Function to allocate memory from the SyslinkMemMgr */
Ptr SyslinkMemMgr_alloc(SyslinkMemMgr_Handle handle,
        SyslinkMemMgr_AllocParams *params);

/* Function to free memory from the SyslinkMemMgr */
Int32 SyslinkMemMgr_free(SyslinkMemMgr_Handle handle, Ptr ptr, UInt32 size);

/* Function to map. not used for shared memory*/
Ptr SyslinkMemMgr_map(SyslinkMemMgr_Handle handle, Ptr arg);

/* Function to do unmap */
Int32 SyslinkMemMgr_unmap(SyslinkMemMgr_Handle handle, Ptr arg);

/* Function to translate source address to destination address type */
Ptr SyslinkMemMgr_translate(SyslinkMemMgr_Handle handle, Ptr srcAddr,
        SyslinkMemMgr_AddrType srcAddrType,
        SyslinkMemMgr_AddrType desAddrType);

/* Function to return the kernel space instance handle pointer when user space
 * instance handle is passed
 */
Ptr SyslinkMemMgr_getKnlHandle(SyslinkMemMgr_Handle handle);


#if defined (__cplusplus)
}
#endif

#endif
