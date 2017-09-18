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
 *  @file   ti/syslink/inc/knl/Linux/omap3530_hal_mmu_defs.h
 *
 *  @brief      Hardware abstraction for Memory Management Unit module.
 *
 *              This module is responsible for handling slave MMU related
 *              hardware- specific operations.
 *              The implementation is specific to OMAP3530.
 */

#ifndef omap3530_hal_mmu_defs_H_0xbbed
#define omap3530_hal_mmu_defs_H_0xbbed


/* Module level headers */
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/omap3530proc.h>


/*Linux specific headers*/
#include <plat/iommu.h>
#include <plat/iovmm.h>
#include <asm/page.h>
#include <linux/pagemap.h>
#include <linux/platform_device.h>
#include <linux/clk.h>


/* =============================================================================
 *  Macros
 * =============================================================================
 */
/*!
 *  @brief  Mmu Sysconfig register offset
 */
#define MMU_SYSCONFIG_OFFSET      0x10
/*!
 *  @brief  Mmu Sysstatus register offset
 */

#define MMU_SYSSTATUS_OFFSET      0x14
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_IRQSTATUS_OFFSET      0x18
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_IRQENABLE_OFFSET      0x1C
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_CNTL_OFFSET           0x44
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_FAULT_AD_OFFSET       0x48
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_LOCK_OFFSET           0x50
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_LD_TBL_OFFSET         0x54
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_CAM_OFFSET            0x58
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_RAM_OFFSET            0x5C
/*!
 *  @brief  Offsets from the MMU base address
 */

#define MMU_FLUSH_ENTRY           0x64

/*!
 *  @brief  CAM register field values
 */
#define MMU_CAM_PRESERVE          (1 << 3)
/*!
 *  @brief  CAM register field values
 */

#define MMU_CAM_VALID             (1 << 2)

/*!
 *  @brief  Addresses lower than this do not go through the DSP's MMU
 */
#define MMU_GLOBAL_MEMORY         0x11000000

/*!
 *  @brief  Faults to enable (MULTIHITFAULT and TLBMISS)
 */
#define MMU_IRQENABLE_FAULTS      0x11

/*!
 *  @brief  Page size constants
 */
#define PAGE_SIZE_4KB             0x1000
/*!
 *  @brief  Page size constants
 */
#define PAGE_SIZE_64KB            0x10000
/*!
 *  @brief  Page size constants
 */
#define PAGE_SIZE_1MB             0x100000
/*!
 *  @brief  Page size constants
 */
#define PAGE_SIZE_16MB            0x1000000

/*
 * "L2 table" address mask and size definitions.
 */
#define IOPGD_SHIFT     20
#define IOPGD_SIZE      (1UL << IOPGD_SHIFT)
#define IOPGD_MASK      (~(IOPGD_SIZE - 1))

/*
 * "small page" address mask and size definitions.
 */
#define IOPTE_SHIFT     12
#define IOPTE_SIZE      (1UL << IOPTE_SHIFT)
#define IOPTE_MASK      (~(IOPTE_SIZE - 1))
#define IOPAGE_MASK     IOPTE_MASK

#define PTRS_PER_IOPGD  (1UL << (32 - IOPGD_SHIFT))

/* to find an entry in a page-table-directory */
#define iopgd_index(da)         (((da) >> IOPGD_SHIFT) & (PTRS_PER_IOPGD - 1))
#define iopgd_offset(obj, da)   ((obj)->iopgd + iopgd_index(da))

/* =============================================================================
 *  Enums
 * =============================================================================
 */
/*!
 *  @brief  enum for Element size.
 */
enum Omap3530_Hal_Mmu_Element_Size_tag {
    ELEM_SIZE_8BIT,
    ELEM_SIZE_16BIT,
    ELEM_SIZE_32BIT,
    ELEM_SIZE_64BIT
};

/*!
 *  @brief  enum for endianism.
 */
enum Omap3530_Hal_Mmu_Endianism_tag {
    LITTLE_ENDIAN,
    BIG_ENDIAN
};

/*!
 *  @brief  enum for endianism.
 */
enum Omap3530_Hal_Mmu_Mixed_Size_tag {
    MMU_TLBES,
    MMU_CPUES
};

/* =============================================================================
 *  structs
 * =============================================================================
 */

/*!
 *  @brief  Hardware Abstraction object for MMU module.
 */
typedef struct OMAP3530_HalMmuObject_tag {
    UInt32                 mmuFaultAddr;
    /*!< Pointer to IsrObject. */
    struct clk *           clkHandle;
    /*!< Enables and disables the iva2_ck clock. */
    struct platform_device iva2Device;
    /*!< Pltdorm device defined for iva2 sub system. */
    struct iommu *         dspMmuHandler;
    /*!< Mmu handle returned by iommu_get. */
} OMAP3530_HalMmuObject;

/*!
 *  @brief  Args type for Processor_MmuCtrlCmd_Enable
 */
typedef struct OMAP3530_HalMmuCtrlArgs_Enable_tag {
    UInt32             numMemEntries;
    /*!< Number of memory regions to be configured. */
    ProcMgr_AddrInfo * memEntries;
    /*!< Array of information structures for memory regions to be configured. */
} OMAP3530_HalMmuCtrlArgs_Enable;

/*!
 *  @brief  Args type for Processor_MmuCtrlCmd_AddEntry
 */
typedef struct OMAP3530_HalMmuEntryInfo_tag {
    UInt32   slaveVirtAddr;
    /*!< Slave address to be mapped */
    UInt32   size;
    /*!< Size (in bytes) of region to be mapped */
    UInt32   masterPhyAddr;
    /*!< Mapped address in host address space */
    UInt32   elementSize;
    /*!< element size */
    UInt32   endianism;
    /*!< Little / big endian */
    UInt32   mixedSize;
    /*!< Types of pages in on segment*/
}OMAP3530_HalMmuEntryInfo ;

/*!
 *  @brief  Args type for Processor_MmuCtrlCmd_DeleteEntry
 */
typedef OMAP3530_HalMmuEntryInfo OMAP3530_HalMmuCtrlArgs_AddEntry;
typedef OMAP3530_HalMmuEntryInfo OMAP3530_HalMmuCtrlArgs_DeleteEntry;

#endif /* omap3530_hal_mmu_defs_H_0xbbed */
