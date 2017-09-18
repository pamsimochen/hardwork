/*
 *  @file   MemoryOS.c
 *
 *  @brief      Qnx Memory interface implementation.
 *
 *              This abstracts the Memory management interface in the kernel
 *              code. Allocation, Freeing-up, copy and address translate are
 *              supported for the kernel memory management.
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



/* Standard headers */
#include <ti/syslink/Std.h>

/*OSAL and kernel utils */
#include <ti/syslink/utils/MemoryOS.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/List.h>
#include <ti/syslink/inc/Bitops.h>
#include <ti/syslink/utils/GateMutex.h>
#include <ti/syslink/utils/IGateProvider.h>

#include <ti/syslink/utils/hlos/knl/Qnx/resMgr/proto.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/mman.h>
#include <arm/mmu.h>
#include <process.h>


/* =============================================================================
 * Macros
 * =============================================================================
 */
/* Macro to make a correct module magic number with refCount */
#define MEMORYOS_MAKE_MAGICSTAMP(x) ((MEMORYOS_MODULEID << 12u) | (x))

/* Macros used when changing memory attributes */
#define ARM_PTE_V6_SP_TEX_MASK_CACHED    ARM_PTE_V6_SP_TEX(5)
#define ARM_PTE_V6_SP_TEX_MASK_NONCACHED ARM_PTE_V6_SP_TEX(1)

#define PTE_CACHED_SPECIAL_BITS \
	(ARM_PTE_V6_SP_TEX_MASK_CACHED | ARM_PTE_B | ARM_PTE_V6_SP_XN)

#define PTE_NONCACHED_SPECIAL_BITS \
    (ARM_PTE_V6_S | ARM_PTE_V6_SP_TEX_MASK_NONCACHED | ARM_PTE_V6_SP_XN)

#define NAME_LEN                         31

#define ALIGNED_ADDR(val)	((val) & ~(getpagesize()-1))
#define ROUNDUP(val, rounding) (((val) & ((rounding) - 1)) ? \
    (((val) & ~((rounding) - 1)) + (rounding)) : (val))

/* =============================================================================
 * Structs & Enums
 * =============================================================================
 */
/*!
 *  @brief  Structure for containing
 */
typedef struct MemoryOS_MapTableInfo {
    List_Elem next;
    /*!< Pointer to next entry */
    UInt32    actualAddress;
    /*!< Actual address */
    UInt32    mappedAddress;
    /*!< Mapped address */
    UInt32    size;
    /*!< Size of the region mapped */
    UInt16    refCount;
    /*!< Reference count of mapped entry. */
    Bool      isCached;
    /*!< Is cached? */
    Int       fd;
    /*!< File descriptor to shared memory object. */
    Int       pid;
    /*!< Process id associated with a peer mapping. */
} MemoryOS_MapTableInfo;

/*!
 *  @brief  Structure defining state object of system memory manager.
 */
typedef struct MemoryOS_ModuleObject {
    Atomic      refCount;
    /*!< Reference count */
    List_Object mapTable;
    /*!< Head of map table */
    List_Object peerMapTable;
    /*!< Head of peer map table */
    IGateProvider_Handle gateHandle;
    /*!< Pointer to lock */
} MemoryOS_ModuleObject;

/*!
 *  @brief  Structure describing a memory range to be mapped as Normal memory.
 */
typedef struct MemoryOS_NormalMemory {
    UInt32       addr;
    UInt32       size;
} MemoryOS_NormalMemory;

static Int _MemoryOS_mapNormal(Memory_MapInfo * mapInfo, Int pid, Bool peer);
static Int _MemoryOS_map(Memory_MapInfo * mapInfo, Int pid, Bool peer);
static Int _MemoryOS_unmap(Memory_UnmapInfo * unmapInfo, Int pid, Bool peer);
static Bool _MemoryOS_isNormal(UInt32 addr, UInt32 size);
/*
 * This table defines all memory ranges that need to be mapped as Normal memory
 */
#if defined(SYSLINK_VARIANT_TI814X) 
#define ADDR_TABLE_SIZE 6
static MemoryOS_NormalMemory addrTable[ADDR_TABLE_SIZE] =
    {
        { /* OCMC */
            .addr     = 0x40300000,
            .size     = 0x20000
        },
        { /* DSP L2 RAM */
            .addr     = 0x40800000,
            .size     = 0x40000
        },
        { /* DSP L1P RAM */
            .addr     = 0x40E00000,
            .size     = 0x8000
        },
        { /* DSP L1D RAM */
            .addr     = 0x40F00000,
            .size     = 0x8000
        },
        { /* Ducati L2 */
            .addr     = 0x55020000,
            .size     = 0x10000
        },
        { /* DDR */
            .addr     = 0x80000000,
            .size     = 0x80000000
        },
    };
#elif defined(SYSLINK_VARIANT_TI811X)
#define ADDR_TABLE_SIZE 6
static MemoryOS_NormalMemory addrTable[ADDR_TABLE_SIZE] =
    {
        { /* OCMC */
            .addr     = 0x40300000,
            .size     = 0x40000
        },
        { /* DSP L2 RAM */
            .addr     = 0x40800000,
            .size     = 0x40000
        },
        { /* DSP L1P RAM */
            .addr     = 0x40E00000,
            .size     = 0x8000
        },
        { /* DSP L1D RAM */
            .addr     = 0x40F00000,
            .size     = 0x8000
        },
        { /* Ducati L2 */
            .addr     = 0x55020000,
            .size     = 0x10000
        },
        { /* DDR */
            .addr     = 0x80000000,
            .size     = 0x80000000
        },
    };
#endif

/* =============================================================================
 * Globals
 * =============================================================================
 */
/*!
 *  @brief  Object containing state of the Memory OS module.
 */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
MemoryOS_ModuleObject MemoryOS_state;


/* =============================================================================
 * APIS
 * =============================================================================
 */
/*!
 *  @brief      Initialize the memory os module.
 */
Int32
MemoryOS_setup (void)
{
    Int32 status = MEMORYOS_S_SUCCESS;
    List_Params  listparams;
    Error_Block  eb;

    GT_0trace (curTrace, GT_ENTER, "MemoryOS_setup");
    Error_init (&eb);

    /* This sets the refCount variable is not initialized, upper 16 bits is
     * written with module Id to ensure correctness of refCount variable.
     */
    Atomic_cmpmask_and_set (&MemoryOS_state.refCount,
                            MEMORYOS_MAKE_MAGICSTAMP(0),
                            MEMORYOS_MAKE_MAGICSTAMP(0));

    if (   Atomic_inc_return (&MemoryOS_state.refCount)
        != MEMORYOS_MAKE_MAGICSTAMP(1u)) {
        status = MEMORYOS_S_ALREADYSETUP;
        GT_0trace (curTrace,
                   GT_2CLASS,
                   "MemoryOS Module already initialized!");
    }
    else {
        /* Create the Gate handle */
        MemoryOS_state.gateHandle = (IGateProvider_Handle)
                GateMutex_create((GateMutex_Params *)NULL, &eb);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (MemoryOS_state.gateHandle == NULL) {
            Atomic_set (&MemoryOS_state.refCount, MEMORYOS_MAKE_MAGICSTAMP(0));
            /*! @retval MEMORYOS_E_FAIL Failed to create the local gate */
            status = MEMORYOS_E_FAIL;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MemoryOS_alloc",
                                 status,
                                 "Failed to create the local gate");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            List_Params_init (&listparams);
            /* Construct the map table.Not passing gate as MemoryOs module
             *  takes care of protection over list operations
             */
            List_construct (&MemoryOS_state.mapTable, &listparams);

            List_Params_init (&listparams);
            /*
			 * Construct the map table for peer mappings.
             */
            List_construct (&MemoryOS_state.peerMapTable, &listparams);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif

    }

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_setup", status);

    /*! @retval MEMORYOS_S_SUCCESS Operation was successful */
    return status;
}


/*!
 *  @brief      Finalize the memory os module.
 */
Int32
MemoryOS_destroy (void)
{
    Int32 status     = MEMORYOS_S_SUCCESS;
    Int32 tmpStatus  = MEMORYOS_S_SUCCESS;
    List_Elem * info = NULL;

    GT_0trace (curTrace, GT_ENTER, "MemoryOS_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MEMORYOS_E_INVALIDSTATE Module was not initialized */
        status = MEMORYOS_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_destroy",
                             status,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   Atomic_dec_return (&MemoryOS_state.refCount)
            == MEMORYOS_MAKE_MAGICSTAMP(0)) {
            /* Free any remaining regions */
            List_traverse (info, (List_Handle) &MemoryOS_state.mapTable) {
                munmap ((unsigned int *)
                         ((MemoryOS_MapTableInfo *)info)->mappedAddress,
                         ((MemoryOS_MapTableInfo *)info)->size);
                List_remove ((List_Handle)&MemoryOS_state.mapTable,
                             info);
                MemoryOS_free (info, sizeof(MemoryOS_MapTableInfo), 0);
            }

            List_destruct(&MemoryOS_state.mapTable);

            List_traverse (info, (List_Handle) &MemoryOS_state.peerMapTable) {
                munmap_peer(((MemoryOS_MapTableInfo *)info)->pid,
                    (unsigned int *)
                    ((MemoryOS_MapTableInfo *)info)->mappedAddress,
                    ((MemoryOS_MapTableInfo *)info)->size);
                List_remove((List_Handle)&MemoryOS_state.peerMapTable,
                             info);
                MemoryOS_free(info, sizeof(MemoryOS_MapTableInfo), 0);
            }

            List_destruct(&MemoryOS_state.peerMapTable);

            /* Delete the gate handle */
            tmpStatus = GateMutex_delete ((GateMutex_Handle *)
                                          &MemoryOS_state.gateHandle);
            if ((status >= 0) && (tmpStatus < 0)) {
                status = tmpStatus;
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "MemoryOS_destroy",
                                     status,
                                     "GateMutex_delete failed!");
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_destroy", status);

    /*! @retval MEMORYOS_S_SUCCESS OPeration was successful */
    return status;
}


/*!
 *  @brief      Allocates the specified number of bytes.
 *
 *  @param      ptr pointer where the size memory is allocated.
 *  @param      size amount of memory to be allocated.
 *  @sa         Memory_calloc
 */
Ptr MemoryOS_alloc (UInt32 size, UInt32 align, UInt32 flags)
{
    Ptr ptr = NULL;

    GT_3trace (curTrace, GT_ENTER, "MemoryOS_alloc", size, align, flags);

    /* check whether the right paramaters are passed or not.*/
    GT_assert (curTrace, (size > 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_alloc",
                             MEMORYOS_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

        if (flags == MemoryOS_MemTypeFlags_Default) {
            /* Call the kernel API for memory allocation */
            ptr = malloc (size) ;
        }
#if 0 /* does any one actually allocate memory with this flag */
        else if (flags == MemoryOS_MemTypeFlags_Physical) {
            ptr = kmalloc (size, GFP_KERNEL);
        }
        else if (flags == MemoryOS_MemTypeFlags_Dma) {
            ptr = kmalloc (size, GFP_KERNEL|GFP_DMA);
        }
#endif
        else {
            /*! @retval NULL Memory allocation failed */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MemoryOS_alloc",
                                 MEMORYOS_E_MEMORY,
                                 "Memory allocation type is invalid");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NULL == ptr) {
            /*! @retval NULL Memory allocation failed */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MemoryOS_alloc",
                                 MEMORYOS_E_MEMORY,
                                 "Could not allocate memory");
        }
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_alloc", ptr);

    /*! @retval Pointer Memory allocation successful */
    return ptr;
}


/*!
 *  @brief      Allocates the specified number of bytes and memory is
 *              set to zero.
 *
 *  @param      ptr pointer where the size memory is allocated.
 *  @param      size amount of memory to be allocated.
 *  @sa         Memory_alloc
 */
Ptr
MemoryOS_calloc (UInt32 size, UInt32 align, UInt32 flags)
{
    Ptr ptr = NULL;
    GT_3trace (curTrace, GT_ENTER, "MemoryOS_calloc", size, align, flags);

    ptr = MemoryOS_alloc (size, align, flags);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NULL == ptr) {
        /*! @retval NULL Memory allocation failed */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_calloc",
                             MEMORYOS_E_MEMORY,
                             "Could not allocate memory");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        memset (ptr, 0, size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_calloc", ptr);

    /*! @retval Pointer Memory allocation successful */
    return ptr;
}


/*!
 *  @brief      Frees up the specified chunk of memory.
 *
 *  @param      ptr  Pointer to the previously allocated memory area.
 *  @sa         Memory_alloc
 */
Void
MemoryOS_free (Ptr ptr, UInt32 size, UInt32 flags)
{
    GT_3trace (curTrace, GT_ENTER, "MemoryOS_free", ptr, size, flags);

    GT_assert (GT_1CLASS, (ptr != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NULL == ptr) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_free",
                             MEMORYOS_E_INVALIDARG,
                             "Pointer NULL for free");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (flags == MemoryOS_MemTypeFlags_Default) {
            /*! free the memory pointed by ptr */
            free (ptr);
        }
#if 0
        else if (flags == MemoryOS_MemTypeFlags_Physical) {
            munmap (ptr, size);
        }
        else if (flags == MemoryOS_MemTypeFlags_Dma) {
            munmap (ptr, size);
        }
#endif
        else {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "MemoryOS_free",
                                 MEMORYOS_E_MEMORY,
                                 "Memory free type is invalid");
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "MemoryOS_free");
}

/*!
 *  @brief      Maps a memory area into virtual space.
 *
 *  @param      mapInfo  Pointer to the area which needs to be mapped.
 *  @sa         Memory_unmap
 */
Int
MemoryOS_map (Memory_MapInfo * mapInfo)
{
    Int        status = MEMORYOS_S_SUCCESS;

    GT_1trace(curTrace, GT_ENTER, "MemoryOS_map", mapInfo);

    if (_MemoryOS_isNormal(mapInfo->src, mapInfo->size)) {
        status = _MemoryOS_mapNormal(mapInfo, -1, FALSE);
    }
    else {
        status = _MemoryOS_map(mapInfo, -1, FALSE);
    }

    GT_1trace(curTrace, GT_LEAVE, "MemoryOS_map", status);

    /*! @retval MEMORYOS_SUCESS Operation completed successfully. */
    return status;
}


/*!
 *  @brief      UnMaps a memory area into virtual space.
 *
 *  @param      unmapInfo pointer to the area which needs to be unmapped.
 *  @sa         Memory_map
 */
Int
MemoryOS_unmap (Memory_UnmapInfo * unmapInfo)
{
    Int         status = MEMORYOS_S_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "MemoryOS_unmap", unmapInfo);

    status = _MemoryOS_unmap(unmapInfo, -1, FALSE);

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_unmap", status);

    /*! @retval MEMORYOS_SUCESS Operation completed successfully. */
    return status;
}

/*!
 *  @brief      Maps a memory area into virtual space on behalf of another
 *              process.
 *
 *  @param      mapInfo  Pointer to the area which needs to be mapped.
 *  #param      pid      Process id
 *  @sa         Memory_unmap
 */
Int
MemoryOS_peerMap (Memory_MapInfo * mapInfo, Int pid)
{
    Int                     status = MEMORYOS_S_SUCCESS;

    GT_2trace(curTrace, GT_ENTER, "MemoryOS_peermap", mapInfo, pid);

    if (_MemoryOS_isNormal(mapInfo->src, mapInfo->size)) {
        status = _MemoryOS_mapNormal(mapInfo, pid, TRUE);
    }
    else {
        status = _MemoryOS_map(mapInfo, pid, TRUE);
    }
    GT_1trace(curTrace, GT_LEAVE, "MemoryOS_peerMap", status);

    /*! @retval MEMORYOS_SUCESS Operation completed successfully. */
    return status;
}


/*!
 *  @brief      UnMaps a memory area into virtual space.
 *
 *  @param      unmapInfo poiinter to the area which needs to be unmapped.
 *  @sa         Memory_map
 */
Int
MemoryOS_peerUnmap (Memory_UnmapInfo * unmapInfo, Int pid)
{
    Int         status = MEMORYOS_S_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "MemoryOS_peerUnmap", unmapInfo, pid);

    status = _MemoryOS_unmap(unmapInfo, pid, TRUE);

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_peerUnmap", status);

    /*! @retval MEMORYOS_SUCESS Operation completed successfully. */
    return status;
}

/*!
 *  @brief      Maps a memory area into virtual space using normal
 *              attributes.
 *
 *  @param      mapInfo  Pointer to the area which needs to be mapped.
 *  @sa         Memory_unmap
 */
static Int
_MemoryOS_mapNormal(Memory_MapInfo * mapInfo, Int pid, Bool peer)
{
    Int                     status = MEMORYOS_S_SUCCESS;
    Bool                    exists = FALSE;
    IArg                    key;
    List_Elem *             listInfo;
    MemoryOS_MapTableInfo * info;
    Int                     rc;
    Char                    name[NAME_LEN];
    UInt                    specialBits;
    UInt                    adjustedSize;
    Int                     fd;
    List_Handle             listHandle;

    GT_3trace(curTrace, GT_ENTER, "_MemoryOS_mapNormal", mapInfo, pid, peer);

    GT_assert (curTrace, (NULL != mapInfo));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MEMORYOS_E_INVALIDSTATE Module was not initialized */
        status = MEMORYOS_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_mapNormal",
                             status,
                             "Module was not initialized!");
    }
    else  if (mapInfo == NULL) {
        /*! @retval MEMORYOS_E_INVALIDARG NULL provided for argument mapInfo. */
        status = MEMORYOS_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_mapNormal",
                             status,
                             "NULL provided for argument mapInfo");
    }
    else if (mapInfo->src == (UInt32) NULL) {
        /*! @retval MEMORYOS_E_INVALIDARG NULL provided for argument
                                          mapInfo->src. */
        status = MEMORYOS_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_mapNormal",
                             status,
                             "NULL provided for argument mapInfo->src");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter the critical section */
        key = IGateProvider_enter (MemoryOS_state.gateHandle);

        if (peer == TRUE) {
            listHandle = (List_Handle)&MemoryOS_state.peerMapTable;
        }
        else {
            listHandle = (List_Handle)&MemoryOS_state.mapTable;
        }

        /* First check if the mapping already exists in the map table */
        List_traverse(listInfo, listHandle) {
            if ((((MemoryOS_MapTableInfo *)listInfo)->actualAddress
                    == mapInfo->src)
                && (!peer || ((MemoryOS_MapTableInfo *)listInfo)->pid == pid)
                && (((MemoryOS_MapTableInfo *)listInfo)->size == mapInfo->size)
                &&  (((MemoryOS_MapTableInfo *)listInfo)->isCached
                     == mapInfo->isCached)) {
                exists = TRUE;
                mapInfo->dst =
                           ((MemoryOS_MapTableInfo *) listInfo)->mappedAddress;
                /* Increase the refcount. */
                ((MemoryOS_MapTableInfo *) listInfo)->refCount++;

                if (peer) {
                    GT_4trace(curTrace, GT_1CLASS,
                        "_MemoryOS_mapNormal: entry already exists\n"
                        "    pid           [0x%x]\n"
                        "    mapInfo->src  [0x%x]\n"
                        "    mapInfo->dst  [0x%x]\n"
                        "    mapInfo->size [0x%x]",
                        pid, mapInfo->src, mapInfo->dst, mapInfo->size);
                }
                else {
                    GT_3trace(curTrace, GT_1CLASS,
                        "_MemoryOS_mapNormal: entry already exists\n"
                        "    mapInfo->src  [0x%x]\n"
                        "    mapInfo->dst  [0x%x]\n"
                        "    mapInfo->size [0x%x]",
                        mapInfo->src, mapInfo->dst, mapInfo->size);
                }
                break;
            }
        }

        if (!exists) {
            mapInfo->dst = 0;
            /*
             * We need to map full pages, hence align size and address to page
             * boundary
             */
            adjustedSize = ROUNDUP(mapInfo->src - ALIGNED_ADDR(mapInfo->src) +
                mapInfo->size, getpagesize());
            if (peer) {
                snprintf(name, NAME_LEN, "map-%08x-%08x-%08x", pid,
                    mapInfo->src, mapInfo->size);
            }
            else {
                snprintf(name, NAME_LEN, "map-%08x-%08x", mapInfo->src,
                    mapInfo->size);
            }
            fd = shm_open(name, O_CREAT | O_TRUNC | O_EXCL | O_RDWR,
                0666);
            if (fd == -1) {
                status = MEMORYOS_E_FAIL;
                GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_mapNormal",
                             status,
                             "shm_open failed.");
            }
            else {
                shm_unlink(name);
                if (mapInfo->isCached == TRUE) {
                    specialBits = PTE_CACHED_SPECIAL_BITS;
                }
                else {
                    specialBits = PTE_NONCACHED_SPECIAL_BITS;
                }

                /*
                 * Use Normal Memory attributes. Need to use the same virtual
                 * address in all processes to allow syslink driver to
                 * maintain cache for user process' behalf.
                 */
                rc = shm_ctl_special(fd, SHMCTL_PHYS,
                    ALIGNED_ADDR(mapInfo->src), adjustedSize, specialBits);

                if (rc) {
                    status = MEMORYOS_E_FAIL;
                    GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_mapNormal",
                             status,
                             "shm_ctl_special failed.");
                }
                else {
                    if (peer) {
                        if (mapInfo->isCached == TRUE) {
                            mapInfo->dst = (UInt32)mmap_peer(pid, 0,
                                adjustedSize,
                                (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0) +
                                (mapInfo->src - ALIGNED_ADDR(mapInfo->src));
                        }
                        else {
                            mapInfo->dst = (UInt32)mmap_peer(pid, 0,
                                adjustedSize,
                                (PROT_READ | PROT_WRITE | PROT_NOCACHE),
                                MAP_SHARED, fd, (off_t)0) +
                                (mapInfo->src - ALIGNED_ADDR(mapInfo->src));
                        }
                    }
                    else {
                        if (mapInfo->isCached == TRUE) {
                            mapInfo->dst = (UInt32)mmap(NULL, adjustedSize,
                                (PROT_READ | PROT_WRITE), MAP_SHARED,
                                fd, (off_t)0) + (mapInfo->src -
                                ALIGNED_ADDR(mapInfo->src));
                        }
                        else {
                            mapInfo->dst = (UInt32)mmap(NULL, adjustedSize,
                                (PROT_READ | PROT_WRITE | PROT_NOCACHE),
                                MAP_SHARED, fd, (off_t)0) +
                                (mapInfo->src - ALIGNED_ADDR(mapInfo->src));
                        }
                    }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                    if (mapInfo->dst == (UInt32)MAP_FAILED) {
                        /*! @retval MEMORYOS_E_MAP Failed to map to host
                            address space. */
                        status = MEMORYOS_E_MAP;
                        GT_setFailureReason (curTrace,
                            GT_4CLASS,
                            "_MemoryOS_mapNormal",
                            status,
                            "Failed to map to host address space!");
                    }
                    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                        info = MemoryOS_alloc (sizeof (MemoryOS_MapTableInfo),
                            0, 0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        if (info == NULL) {
                            /*! @retval MEMORYOS_E_MEMORY Failed to allocate
                                memory. */
                            status = MEMORYOS_E_MEMORY;
                            GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "_MemoryOS_mapNormal",
                                             status,
                                             "Failed to allocate memory!");
                        }
                        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

                            /* Initialize the list element */
                            List_elemClear ((List_Elem *) info);
                            /* Populate the info */
                            info->actualAddress = mapInfo->src;
                            info->mappedAddress = mapInfo->dst;
                            info->size          = mapInfo->size;
                            info->refCount      = 1;
                            info->isCached      = mapInfo->isCached;
                            info->fd            = fd;
                            info->pid           = pid;
                            /* Put the info into the list */
                            List_putHead(listHandle,
                                      (List_Elem *)info);

                            GT_3trace(curTrace, GT_1CLASS,
                                "_MemoryOS_mapNormal: pa=0x%x, va=0x%x, sz=0x%x",
                                mapInfo->src, mapInfo->dst, mapInfo->size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                        }
                    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                }
            }
        }
        /* Leave the crtical section */
        IGateProvider_leave (MemoryOS_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace(curTrace, GT_LEAVE, "_MemoryOS_mapNormal", status);

    /*! @retval MEMORYOS_SUCESS Operation completed successfully. */
    return status;
}

/*!
 *  @brief      Maps a memory area into virtual space using strongly ordered
 *              attributes.
 *
 *  @param      mapInfo  Pointer to the area which needs to be mapped.
 *  @sa         Memory_unmap
 */
static Int
_MemoryOS_map(Memory_MapInfo * mapInfo, Int pid, Bool peer)
{
    Int                     status = MEMORYOS_S_SUCCESS;
    Bool                    exists = FALSE;
    IArg                    key;
    UInt                    adjustedSize;
    List_Elem *             listInfo;
    MemoryOS_MapTableInfo * info;
    List_Handle             listHandle;

    GT_3trace(curTrace, GT_ENTER, "_MemoryOS_map", mapInfo, pid, peer);

    GT_assert (curTrace, (NULL != mapInfo));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MEMORYOS_E_INVALIDSTATE Module was not initialized */
        status = MEMORYOS_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_map",
                             status,
                             "Module was not initialized!");
    }
    else  if (mapInfo == NULL) {
        /*! @retval MEMORYOS_E_INVALIDARG NULL provided for argument mapInfo. */
        status = MEMORYOS_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_map",
                             status,
                             "NULL provided for argument mapInfo");
    }
    else if (mapInfo->src == (UInt32) NULL) {
        /*! @retval MEMORYOS_E_INVALIDARG NULL provided for argument
                                          mapInfo->src. */
        status = MEMORYOS_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_map",
                             status,
                             "NULL provided for argument mapInfo->src");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Enter the critical section */
        key = IGateProvider_enter (MemoryOS_state.gateHandle);

        if (peer == TRUE) {
            listHandle = (List_Handle)&MemoryOS_state.peerMapTable;
        }
        else {
            listHandle = (List_Handle)&MemoryOS_state.mapTable;
        }

        /* First check if the mapping already exists in the map table */
        List_traverse(listInfo, listHandle) {
            if ((((MemoryOS_MapTableInfo *)listInfo)->actualAddress
                    == mapInfo->src)
                && (!peer || ((MemoryOS_MapTableInfo *)listInfo)->pid == pid)
                && (((MemoryOS_MapTableInfo *)listInfo)->size == mapInfo->size)
                &&  (((MemoryOS_MapTableInfo *)listInfo)->isCached
                     == mapInfo->isCached)) {
                exists = TRUE;
                mapInfo->dst =
                           ((MemoryOS_MapTableInfo *) listInfo)->mappedAddress;
                /* Increase the refcount. */
                ((MemoryOS_MapTableInfo *) listInfo)->refCount++;

                if (peer) {
                    GT_4trace(curTrace, GT_1CLASS,
                        "_MemoryOS_map: entry already exists\n"
                        "    pid           [0x%x]\n"
                        "    mapInfo->src  [0x%x]\n"
                        "    mapInfo->dst  [0x%x]\n"
                        "    mapInfo->size [0x%x]",
                        pid, mapInfo->src, mapInfo->dst, mapInfo->size);
                }
                else {
                    GT_3trace(curTrace, GT_1CLASS,
                        "_MemoryOS_map: entry already exists\n"
                        "    mapInfo->src  [0x%x]\n"
                        "    mapInfo->dst  [0x%x]\n"
                        "    mapInfo->size [0x%x]",
                        mapInfo->src, mapInfo->dst, mapInfo->size);
                }
                break;
            }
        }

        if (!exists) {
            mapInfo->dst = 0;
            /*
             * We need to map full pages, hence align size and address to page
             * boundary
             */
            adjustedSize = ROUNDUP(mapInfo->src - ALIGNED_ADDR(mapInfo->src) +
                mapInfo->size, getpagesize());

            if (peer) {
                if (mapInfo->isCached == TRUE) {
                    mapInfo->dst = (UInt32)mmap_peer(pid, 0, adjustedSize,
                            (PROT_READ | PROT_WRITE), (MAP_PHYS | MAP_SHARED),
                            NOFD, (off_t)ALIGNED_ADDR(mapInfo->src)) +
                            (mapInfo->src - ALIGNED_ADDR(mapInfo->src));
                }
                else {
                    mapInfo->dst = (UInt32)mmap_peer(pid, 0, adjustedSize,
                            (PROT_READ | PROT_WRITE | PROT_NOCACHE),
                            (MAP_PHYS|MAP_SHARED), NOFD,
                            (off_t)ALIGNED_ADDR(mapInfo->src)) + (mapInfo->src -
                            ALIGNED_ADDR(mapInfo->src));
                }
            }
            else {
                if (mapInfo->isCached == TRUE) {
                    mapInfo->dst = (UInt32)mmap(NULL, adjustedSize,
                            (PROT_READ | PROT_WRITE), (MAP_PHYS | MAP_SHARED),
                            NOFD, (off_t)ALIGNED_ADDR(mapInfo->src)) +
                            (mapInfo->src - ALIGNED_ADDR(mapInfo->src));
                }
                else {
                    mapInfo->dst = (UInt32)mmap(NULL, adjustedSize,
                            (PROT_READ | PROT_WRITE | PROT_NOCACHE),
                            (MAP_PHYS|MAP_SHARED), NOFD,
                            (off_t)ALIGNED_ADDR(mapInfo->src)) + (mapInfo->src -
                            ALIGNED_ADDR(mapInfo->src));
                }
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (mapInfo->dst == (UInt32)MAP_FAILED) {
                /*!
                 * @retval MEMORYOS_E_MAP Failed to map to host address space.
                 */
                status = MEMORYOS_E_MAP;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "_MemoryOS_map",
                                     status,
                                     "Failed to map to host address space!");
            }
            else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                info = MemoryOS_alloc (sizeof (MemoryOS_MapTableInfo), 0, 0);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (info == NULL) {
                    /*! @retval MEMORYOS_E_MEMORY Failed to allocate memory. */
                    status = MEMORYOS_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "_MemoryOS_map",
                                         status,
                                         "Failed to allocate memory!");
                }
                else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Initialize the list element */
                    List_elemClear ((List_Elem *) info);
                    /* Populate the info */
                    info->actualAddress = mapInfo->src;
                    info->mappedAddress = mapInfo->dst;
                    info->size          = mapInfo->size;
                    info->refCount      = 1;
                    info->isCached      = mapInfo->isCached;
                    info->fd            = -1;
                    info->pid           = pid;
                    /* Put the info into the list */
                    List_putHead(listHandle,
                                  (List_Elem *) info);

                    GT_3trace(curTrace, GT_1CLASS,
                        "_MemoryOS_map: pa=0x%x, va=0x%x, sz=0x%x",
                        mapInfo->src, mapInfo->dst, mapInfo->size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        }
        /* Leave the crtical section */
        IGateProvider_leave (MemoryOS_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace(curTrace, GT_LEAVE, "_MemoryOS_map", status);

    /*! @retval MEMORYOS_SUCESS Operation completed successfully. */
    return status;
}

/*!
 *  @brief      UnMaps a memory area into virtual space.
 *
 *  @param      unmapInfo poiinter to the area which needs to be unmapped.
 *  @sa         Memory_map
 */
static Int
_MemoryOS_unmap(Memory_UnmapInfo * unmapInfo, Int pid, Bool peer)
{
    Int         status = MEMORYOS_S_SUCCESS;
    Int         ret    = 0;
    Bool        found  = FALSE;
    IArg        key;
    List_Elem * info;
    UInt        adjustedSize;
    List_Handle listHandle;

    GT_3trace (curTrace, GT_ENTER, "_MemoryOS_unmap", unmapInfo, pid,
        peer);

    GT_assert (curTrace, (NULL != unmapInfo));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        /*! @retval MEMORYOS_E_INVALIDSTATE Module was not initialized */
        status = MEMORYOS_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_unmap",
                             status,
                             "Module was not initialized!");
    }
    else if (unmapInfo == NULL) {
        /*! @retval MEMORYOS_E_INVALIDARG NULL provided for argument
                                          unmapInfo. */
        status = MEMORYOS_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_unmap",
                             status,
                             "NULL provided for argument unmapInfo");
    }
    else if (unmapInfo->addr == (UInt32) NULL) {
        /*! @retval MEMORYOS_E_INVALIDARG NULL provided for argument
                                          unmapInfo->addr. */
        status = MEMORYOS_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "_MemoryOS_unmap",
                             status,
                             "NULL provided for argument unmapInfo->addr");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = IGateProvider_enter (MemoryOS_state.gateHandle);

        if (peer == TRUE) {
            listHandle = (List_Handle)&MemoryOS_state.peerMapTable;
        }
        else {
            listHandle = (List_Handle)&MemoryOS_state.mapTable;
        }

        /* Find the node in the map table */
        List_traverse (info, listHandle) {
                if (peer) {
                	GT_3trace (curTrace,
                               GT_1CLASS,
                               "_MemoryOS_unmap:\n"
                               "    info->pid            [0x%x]\n"
                               "    info->mappedAddress  [0x%x]\n"
                               "    unmapInfo->addr      [0x%x]\n",
                               ((MemoryOS_MapTableInfo *)info)->pid,
                               ((MemoryOS_MapTableInfo *)info)->mappedAddress,
                               unmapInfo->addr);
                }
                else {
                    GT_2trace (curTrace,
                               GT_1CLASS,
                               "_MemoryOS_unmap:\n"
                               "    info->mappedAddress  [0x%x]\n"
                               "    unmapInfo->addr      [0x%x]\n",
                               ((MemoryOS_MapTableInfo *) info)->mappedAddress,
                               unmapInfo->addr);
                }
            if ((((MemoryOS_MapTableInfo *)info)->mappedAddress
                    == unmapInfo->addr)
                && (!peer || ((MemoryOS_MapTableInfo *)info)->pid == pid)
                && (((MemoryOS_MapTableInfo *)info)->isCached
                    == unmapInfo->isCached)
                &&  (((MemoryOS_MapTableInfo *) info)->size
                    == unmapInfo->size)) {

                /* Found a match, decrease the refcount. */
                ((MemoryOS_MapTableInfo *) info)->refCount--;
                found = TRUE;
                break;
            }
        }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (!found) {
            status = MEMORYOS_E_UNMAP;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_MemoryOS_unmap",
                                 status,
                                 "Could not find specified entry to unmap!");
        }
        else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Unmap if refCount is 0. */
            if (((MemoryOS_MapTableInfo *) info)->refCount == 0) {
                List_remove (listHandle, (List_Elem *) info);

                /*
                 * We mapped full pages, so need to adjust size and address
                 * passed to munmap
                 */
                adjustedSize = ROUNDUP(unmapInfo->addr -
                    ALIGNED_ADDR(unmapInfo->addr) + unmapInfo->size,
                    getpagesize());
                if (peer) {
                    ret = munmap_peer(pid, (void *)ALIGNED_ADDR(
                        unmapInfo->addr), adjustedSize);
                    if (ret != 0) {
                        status = MEMORYOS_E_UNMAP;
                        GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_MemoryOS_unmap",
                                 status,
                                 "munmap_peer failed!");
                    }
                }
                else {
                    ret = munmap((void *)ALIGNED_ADDR(unmapInfo->addr),
                        adjustedSize);
                    if (ret != 0) {
                        status = MEMORYOS_E_UNMAP;
                        GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "_MemoryOS_unmap",
                                 status,
                                 "munmap failed!");
                    }
                }
                if (((MemoryOS_MapTableInfo *)info)->fd != -1) {
                    close(((MemoryOS_MapTableInfo *)info)->fd);
                }
                MemoryOS_free (info, sizeof (MemoryOS_MapTableInfo), 0);
            }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

        IGateProvider_leave (MemoryOS_state.gateHandle, key);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "_MemoryOS_unmap", status);

    /*! @retval MEMORYOS_SUCESS Operation completed successfully. */
    return status;
}

/*!
 *  @brief      Checks if memory range should be mapped as Normal memory
 *
 *  @param      mapInfo  Pointer to the area which needs to be mapped.
 *  @sa         Memory_unmap
 */
static Bool
_MemoryOS_isNormal(UInt32 addr, UInt32 size) {
    Bool   ret = FALSE;
    UInt32 upperBound;
    Int    i;

    for (i = 0; i < ADDR_TABLE_SIZE; i++) {
        upperBound = addrTable[i].addr + addrTable[i].size - 1;
        /*
         * If the range to be mapped is entirely within a range to be mapped as
         * Normal, then do it. Otherwise be conservative and mapped as strongly
         * ordered.
         */
        if ((addr >= addrTable[i].addr) &&
            (addr + size - 1 <= upperBound)) {
            ret = TRUE;
            break;
        }
    }

    return (ret);
}

/*!
 *  @brief      Copies the data between memory areas.
 *
 *  @param      dst  destination address.
 *  @param      src  source address.
 *  @param      len  length of byte to be copied.
 */
Ptr
MemoryOS_copy (Ptr dst, Ptr src, UInt32 len)
{
//    Int32 ret;

    GT_3trace (curTrace, GT_ENTER, "Memory_copy", dst, src, len);

    GT_assert (curTrace, ((NULL != dst) && (NULL != src)));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_copy",
                             MEMORYOS_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if ((dst == NULL) || (src == NULL)) {
        /*! @retval NULL Invalid argument */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_copy",
                             MEMORYOS_E_INVALIDARG,
                             "Invalid argument");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

            dst = memcpy (dst, src, len);
//        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_copy", dst);

    /*! @retval Pointer Success: Pointer to updated destination buffer */
    return dst;
}


/*!
 *  @brief      Set the specific value in the said memory area.
 *
 *  @param      buf  operating buffer.
 *  @param      value the value to be stored in each byte.
 *  @param      len  length of bytes to be set.
 */
Ptr
MemoryOS_set (Ptr buf, Int value, UInt32 len)
{
    GT_3trace (curTrace, GT_ENTER, "MemoryOS_set", buf, value, len);

    GT_assert (curTrace, (NULL != buf) && (len > 0));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_set",
                             MEMORYOS_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else if (buf == NULL) {
        /*! @retval NULL Invalid argument */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_set",
                             MEMORYOS_E_INVALIDARG,
                             "Invalid argument");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        buf = memset (buf, value, len);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_set", buf);

    /*! @retval Pointer Success: Pointer to updated destination buffer */
    return buf;
}


/*!
 *  @brief      Function to translate an address.
 *
 *  @param      srcAddr  source address.
 *  @param      flags    Tranlation flags.
 */
Ptr
MemoryOS_translate (Ptr srcAddr, Memory_XltFlags flags)
{
    Ptr                     buf    = NULL;
    MemoryOS_MapTableInfo * tinfo  = NULL;
    List_Elem *             info   = NULL;
    IArg                    key;
    UInt32                  frmAddr;
    UInt32                  toAddr;

    GT_2trace (curTrace, GT_ENTER, "MemoryOS_translate", srcAddr, flags);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (   Atomic_cmpmask_and_lt (&(MemoryOS_state.refCount),
                                  MEMORYOS_MAKE_MAGICSTAMP(0),
                                  MEMORYOS_MAKE_MAGICSTAMP(1))
        == TRUE) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "MemoryOS_translate",
                             MEMORYOS_E_INVALIDSTATE,
                             "Module was not initialized!");
    }
    else {
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */
        key = IGateProvider_enter (MemoryOS_state.gateHandle);

        /* Traverse to the node in the map table */
        List_traverse (info, (List_Handle) &MemoryOS_state.mapTable) {
            tinfo = (MemoryOS_MapTableInfo *) info;
            frmAddr = (flags == Memory_XltFlags_Virt2Phys) ?
                                    tinfo->mappedAddress : tinfo->actualAddress;
            toAddr = (flags == Memory_XltFlags_Virt2Phys) ?
                                    tinfo->actualAddress : tinfo->mappedAddress;
            if (   (((UInt32) srcAddr) >= frmAddr)
                && (((UInt32) srcAddr) < (frmAddr + tinfo->size))) {
                buf = (Ptr) (toAddr + ((UInt32)srcAddr - frmAddr));
                break;
            }
        }

        IGateProvider_leave (MemoryOS_state.gateHandle, key);

#if 0
        /* If the memory is allocated and not ioremap than below would work
         * probably */
        if (buf == NULL) {
            if (flags == Memory_XltFlags_Virt2Phys) {
                buf = (Ptr) virt_to_phys (srcAddr);
            }
            if (flags == Memory_XltFlags_Phys2Virt) {
                buf = (Ptr) phys_to_virt ((UInt32) srcAddr);
            }
        }
#endif
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "MemoryOS_translate", buf);

    /*! @retval Pointer Success: Pointer to updated destination buffer */
    return buf;
}
