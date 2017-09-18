/** 
 *  @file   SysMemMgr.h
 *
 *  @brief      Manager for the system memory. System level memory are allocated
 *              through this module.
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



#ifndef SYSTEMMEMORYMANAGER_H_0xb53d
#define SYSTEMMEMORYMANAGER_H_0xb53d


#if defined (__cplusplus)
extern "C" {
#endif


/*!
 *  @def    SYSMEMMGR_MODULEID
 *  @brief  Module identifier for System memory manager.
 */
#define SYSMEMMGR_MODULEID   (0xb53d)

/*!
 *  @def    SYSMEMMGR_STATUSCODEBASE
 *  @brief  Error code base for system memory manager module.
 */
#define SYSMEMMGR_STATUSCODEBASE         (SYSMEMMGR_MODULEID << 12u)

/*!
 *  @def    SYSMEMMGR_MAKE_ERROR
 *  @brief  Macro to make error code.
 */
#define SYSMEMMGR_MAKE_ERROR(x)         ((Int) (  0x80000000                   \
                                                + (SYSMEMMGR_STATUSCODEBASE    \
                                                + (x))))

/*!
 *  @def    SYSMEMMGR_MAKE_SUCCESS
 *  @brief  Macro to make success code.
 */
#define SYSMEMMGR_MAKE_SUCCESS(x)       (SYSMEMMGR_STATUSCODEBASE + (x))

/*!
 *  @def    SYSMEMMGR_E_CREATELOCK
 *  @brief  Mutex lock creation failed.
 */
#define SYSMEMMGR_E_CREATELOCK           SYSMEMMGR_MAKE_ERROR(0)

/*!
 *  @def    SYSMEMMGR_E_INVALIDSTATE
 *  @brief  Module is not initialized.
 */
#define SYSMEMMGR_E_INVALIDSTATE         SYSMEMMGR_MAKE_ERROR(1)

/*!
 *  @def    SYSMEMMGR_E_INVALIDARG
 *  @brief  Argument passed to a function is invalid.
 */
#define SYSMEMMGR_E_INVALIDARG           SYSMEMMGR_MAKE_ERROR(2)

/*!
 *  @def    SYSMEMMGR_E_MEMORY
 *  @brief  Memory allocation failed.
 */
#define SYSMEMMGR_E_MEMORY               SYSMEMMGR_MAKE_ERROR(3)

/*!
 *  @def    SYSMEMMGR_E_FAIL
 *  @brief  General failure.
 */
#define SYSMEMMGR_E_FAIL                 SYSMEMMGR_MAKE_ERROR(4)

/*!
 *  @def    SYSMEMMGR_E_ALREADYOPENED
 *  @brief  Internal OS Driver is already opened.
 */
#define SYSMEMMGR_E_ALREADYOPENED         SYSMEMMGR_MAKE_ERROR(5)

/*!
 *  @def    SYSMEMMGR_E_OSFAILURE
 *  @brief  OS Failure.
 */
#define SYSMEMMGR_E_OSFAILURE            SYSMEMMGR_MAKE_ERROR(6)

/*!
 *  @def    SYSMEMMGR_SUCCESS
 *  @brief  Operation successful.
 */
#define SYSMEMMGR_SUCCESS                SYSMEMMGR_MAKE_SUCCESS(0)

/*!
 *  @def    SYSMEMMGR_S_ALREADYSETUP
 *  @brief  Module already initialized.
 */
#define SYSMEMMGR_S_ALREADYSETUP         SYSMEMMGR_MAKE_SUCCESS(1)

/*!
 *  @def    SYSMEMMGR_S_DRVALREADYOPENED
 *  @brief  Internal OS Driver is already opened.
 */
#define SYSMEMMGR_S_DRVALREADYOPENED     SYSMEMMGR_MAKE_SUCCESS(2)


/*!
 *  @brief  Configuration data structure of system memory manager.
 */
typedef struct SysMemMgr_Config {
    UInt32      sizeOfValloc;
    /*!< Total size for virtual memory allocation */
    UInt32      sizeOfPalloc;
    /*!< Total size for physical memory allocation */
    UInt32      staticPhysBaseAddr;
    /*!< Physical address of static memory region */
    UInt32      staticVirtBaseAddr;
    /*!< Virtual  address of static memory region */
    UInt32      staticMemSize;
    /*!< size of static memory region */
    UInt32      pageSize;
    /*!< Page size */
    UInt32      eventNo;
    /*!< Event number to be used */
} SysMemMgr_Config;


/*!
 *  @brief  Flag used for allocating memory blocks.
 */
typedef enum SysMemMgr_AllocFlag {
    SysMemMgr_AllocFlag_Cached   = 0x0001u,
    /*!< Flag used for allocating cacheable block */
    SysMemMgr_AllocFlag_Uncached = 0x0002u,
    /*!< Flag used for allocating uncacheable block */
    SysMemMgr_AllocFlag_Physical = 0x0004u,
    /*!< Flag used for allocating physically contiguous block */
    SysMemMgr_AllocFlag_Virtual  = 0x0008u,
    /*!< Flag used for allocating virtually contiguous block */
    SysMemMgr_AllocFlag_Dma      = 0x0010u
    /*!< Flag used for allocating DMAable (physically contiguous) block */
} SysMemMgr_AllocFlag;

/*!
 *  @brief  Flag used for translating address.
 */
typedef enum SysMemMgr_XltFlag {
    SysMemMgr_XltFlag_Kvirt2Phys  = 0x0001u,
    /*!< Flag used for converting Kernel virtual address to physical address */
    SysMemMgr_XltFlag_Kvirt2Uvirt = 0x0002u,
    /*!< Flag used for converting Kernel virtual address to user virtual address */
    SysMemMgr_XltFlag_Uvirt2Phys  = 0x0004u,
    /*!< Flag used for converting user virtual address to physical address */
    SysMemMgr_XltFlag_Uvirt2Kvirt = 0x0008u,
    /*!< Flag used for converting user virtual address to Kernel virtual address */
    SysMemMgr_XltFlag_Phys2Kvirt  = 0x0010u,
    /*!< Flag used for converting physical address to user virtual address */
    SysMemMgr_XltFlag_Phys2Uvirt  = 0x0011u
    /*!< Flag used for converting physical address to Kernel virtual address */
} SysMemMgr_XltFlag;


/* Function prototypes */
Void SysMemMgr_getConfig (SysMemMgr_Config * params);
Int  SysMemMgr_setup     (SysMemMgr_Config * params);
Int  SysMemMgr_destroy   (Void);
Ptr  SysMemMgr_alloc     (UInt32 size, SysMemMgr_AllocFlag flag);
Int  SysMemMgr_free      (Ptr blk, UInt32 size, SysMemMgr_AllocFlag flag);
Ptr  SysMemMgr_translate (Ptr srcAddr, SysMemMgr_XltFlag flag);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* SYSTEMMEMORYMANAGER_H_0xb53d */
