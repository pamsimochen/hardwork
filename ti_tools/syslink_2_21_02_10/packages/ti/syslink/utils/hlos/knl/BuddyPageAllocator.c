/*
 *  @file   BuddyPageAllocator.c
 *
 *  @brief      Buddy page block allocator implmentation. Provides a way to
 *              manage the static memory region by dividing it into pages/blocks
 *              of 2^x size.
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

/* Utilities headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>

/* Module level headers */
#include <ti/syslink/inc/knl/BuddyPageAllocator.h>


/*!
 *  @brief  Structure defining super blocks.
 */
struct BpaSuperBlock {
    List_Object blkList;   /*!< block linked-list */
    UInt32      order;     /*!< Order of the super block */
};

/*!
 *  @brief  Structure defining normal blocks.
 */
struct BpaBlock {
    List_Elem elem;     /*!< Element of linked-list */
    UInt32    offset;   /*!< Offset of the block from start address */
    Ptr       sBlock;   /*!< Pointer to the superblock */
};

/*!
 *  @brief  Structure defining state object for the allocator.
 */
struct BpaObject {
    struct BpaSuperBlock * sbArray;     /*!< List_Object of the super blocks */
    struct BpaSuperBlock * higher;      /*!< Super block with highest order */
    UInt8            * map;             /*!< Bitmap for stating buddy's state */
    UInt32             virtBaseAddr;    /*!< Base address in virtual */
    UInt32             physBaseAddr;    /*!< Physical base address */
    UInt32             size;            /*!< Size of the region */
    UInt32             pageSize;        /*!< Page size */
    UInt32             nrFreePages;     /*!< Free Pages */
    UInt32             nrTotalPages;    /*!< Total Pages */
};

/*!
 *  @brief      Joins the free buddies.
 *
 *  @param      bpaHandle  Pointer to handle.
 *
 *  @sa         Bpa_create, Bpa_delete, Bpa_alloc, Bpa_free
 */
Void
Bpa_join (Ptr bpaHandle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                    status = BPA_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    struct BpaObject *     handle = (struct BpaObject *) bpaHandle;
    UInt32                 order  = 0;
    UInt32                 new_order  = 0;
    struct BpaSuperBlock * sBlk   = NULL;
    struct BpaSuperBlock * sTemp  = NULL;
    List_Elem *            blk    = NULL;
    struct BpaBlock *      bBlk   = NULL;
    UInt32                 bBitPos;

    GT_1trace (curTrace, GT_ENTER, "Bpa_join", bpaHandle);

    GT_assert (curTrace, (NULL != bpaHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NULL != bpaHandle) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Start with lower order and proceed upwards */
        do {
            sBlk = &handle->sbArray[order];

            List_traverse (blk, (List_Handle) &sBlk->blkList) {
                bBlk = (struct BpaBlock *) (  (UInt32) blk
                                            + (  (order + 1u)
                                               * handle->pageSize));
                if (  ((UInt32) bBlk) < (handle->virtBaseAddr + handle->size)) {
                    bBitPos = (  ((UInt32) bBlk  - handle->virtBaseAddr)
                               / handle->pageSize);
                    if (!TEST_BIT ((handle->map[bBitPos / 8u]),
                                   (bBitPos % 8u))) {
                        /* Buddy is free! wow join it with self*/
                        /* Remove self from the super block */
                        List_remove ((List_Handle) &sBlk->blkList,
                                     (List_Elem *) blk);

                        /* Remove buddy from super block */
                        sTemp = (struct BpaSuperBlock *) bBlk->sBlock;
                        List_remove ((List_Handle) &sTemp->blkList,
                                     (List_Elem *) bBlk);

                        /* Join and put them in correct super block */
                        new_order = order + sTemp->order + 1u;
                        ((struct BpaBlock *)blk)->offset = 0u;
                        ((struct BpaBlock *)blk)->sBlock =
                                                   &handle->sbArray [new_order];
                        List_putHead ((List_Handle)
                                        &handle->sbArray [new_order].blkList,
                                      blk);

                        /* Update the higher super block */
                        if (handle->higher == NULL) {
                            /* there was one block and that was allocated */
                            handle->higher = &handle->sbArray[new_order];
                        }
                        else {
                            if (handle->higher->order < new_order) {
                                handle->higher = &handle->sbArray[new_order];
                            }
                        }

                        /* List_Object has changed now, re-get the first element */
                        blk = List_next ((List_Handle) &sBlk->blkList, NULL);
                        if (blk == NULL) {
                            break;
                        }
                    }
                }
            }
            order++;
        } while (order < handle->nrTotalPages);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
    else {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_join",
                             status,
                             "Handle is null!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Bpa_join");
}


/*!
 *  @brief      Creates a new instance of Binary buddy allocator.
 *
 *  @param      config  Pointer to the config parameters.
 *
 *  @sa         Bpa_join, Bpa_delete, Bpa_alloc, Bpa_free
 */
Ptr
Bpa_create (struct BpaConfig * config)
{
    Int                status = BPA_SUCCESS;
    struct BpaObject * handle = NULL;
    UInt32             i      = 0;

    GT_1trace (curTrace, GT_ENTER, "Bpa_create", config);

    GT_assert (curTrace, (NULL != config));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NULL == config) {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_create",
                             status,
                             "Config parameter is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        /* Allocate the handle */
        handle = (struct BpaObject *) Memory_alloc (NULL,
                                                    sizeof (struct BpaObject),
                                                    0u,
                                                    NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (NULL == handle) {
            status = BPA_E_MEMORY;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "Bpa_create",
                                 status,
                                 "Unable to allocate memory for the handle!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Populate the handle */
            handle->virtBaseAddr = config->virtBaseAddr;
            handle->physBaseAddr = config->physBaseAddr;
            handle->pageSize     = config->pageSize;
            handle->size         = config->size;

            /* Calculate the number of pages */
            handle->nrFreePages = (handle->size / handle->pageSize) ;
            handle->nrTotalPages = handle->nrFreePages;

            /* Allocate bitmap */
            if (handle->nrTotalPages < 8u) {
                handle->map = (UInt8 *) Memory_calloc (NULL, 1u, 0, NULL);
            }
            else {
                handle->map = (UInt8 *) Memory_calloc (NULL,
                                       ((handle->nrTotalPages + 8u - 1u) / 8u),
                                       0u,
                                       NULL);
            }

#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (NULL == handle->map) {
                status = BPA_E_MEMORY;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "Bpa_create",
                                     status,
                                     "Unable to allocate memory for bitmap!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Allocate memory for superblocks */
                handle->sbArray = (struct BpaSuperBlock *) Memory_alloc (NULL,
                        (handle->nrTotalPages * sizeof (struct BpaSuperBlock)),
                        0u,
                        NULL);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
                if (NULL == handle->sbArray) {
                    status = BPA_E_MEMORY;
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "Bpa_create",
                                         status,
                                         "Unable to allocate super blocks!");
                }
                else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                    /* Initialize the super blocks */
                    for (i = 0; i < handle->nrTotalPages; i++) {
                        List_construct (&handle->sbArray[i].blkList, NULL);
                        handle->sbArray[i].order = i;
                    }

                    /* Initialize the whole region as a single block*/
                    List_elemClear ((List_Elem *) handle->virtBaseAddr);
                    ((struct BpaBlock *) handle->virtBaseAddr)->offset = 0;
                    ((struct BpaBlock *) handle->virtBaseAddr)->sBlock =
                                    &handle->sbArray[handle->nrTotalPages - 1u];

                    /* Put the block into the correct super block */
                    List_putHead ((List_Handle)
                            &handle->sbArray[handle->nrTotalPages - 1u].blkList,
                            (List_Elem *)handle->virtBaseAddr);

                    /* Update the higher super block */
                    handle->higher= &handle->sbArray[handle->nrTotalPages - 1u];
#if !defined(SYSLINK_BUILD_OPTIMIZE)
                }
            }
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (status < 0) {
        if (handle != NULL) {
            Memory_free (NULL,
                         handle->sbArray,
                         (handle->nrTotalPages * sizeof (struct BpaSuperBlock)));
            Memory_free (NULL,
                         handle->map,
                         ((handle->nrTotalPages + 8u - 1u) / 8u));
            Memory_free (NULL, handle, sizeof (struct BpaObject));
        }
    }

    GT_1trace (curTrace, GT_LEAVE, "Bpa_create", handle);

    /*! @retval Non-null Operation successful */
    /*! @retval Null Operation not successful */
    return handle;
}


/*!
 *  @brief      Deletes a instance of Binary buddy allocator.
 *
 *  @param      bpaHandle  Pointer to handle.
 *
 *  @sa         Bpa_join, Bpa_create, Bpa_alloc, Bpa_free
 */
Void
Bpa_delete (Ptr bpaHandle)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                status = BPA_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    struct BpaObject * handle = (struct BpaObject *) bpaHandle;
    UInt16             i;

    GT_1trace (curTrace, GT_ENTER, "Bpa_delete", bpaHandle);

    GT_assert (curTrace, (NULL != bpaHandle));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NULL != bpaHandle) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        for (i = 0; i < handle->nrTotalPages; i++) {
            List_destruct (&handle->sbArray[i].blkList);
        }

        /* Free all allocated memory */
        Memory_free (NULL,
                     handle->sbArray,
                     (handle->nrTotalPages * sizeof (struct BpaSuperBlock)));
        Memory_free (NULL,
                     handle->map,
                     ((handle->nrTotalPages + 8u - 1u) / 8u));
        Memory_free (NULL, handle, sizeof (struct BpaObject));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
    else {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_delete",
                             status,
                             "Handle is null!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_0trace (curTrace, GT_LEAVE, "Bpa_delete");
}


/*!
 *  @brief      Allocates a block of memory.
 *
 *  @param      bpaHandle  Pointer to handle.
 *  @param      size    Size of the block.
 *
 *  @sa         Bpa_join, Bpa_create, Bpa_delete, Bpa_free
 */
Ptr
Bpa_alloc (Ptr bpaHandle, UInt32 size)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                    status = BPA_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    struct BpaObject *     handle = (struct BpaObject *) bpaHandle;
    struct BpaSuperBlock * sBlk = NULL;
    UInt32                 order  = 0;
    UInt32                 new_order  = 0;
    Ptr                    retPtr     = NULL;
    List_Elem *            element    = NULL;
    UInt32                 bitPos;

    GT_2trace (curTrace, GT_ENTER, "Bpa_alloc", bpaHandle, size);

    GT_assert (curTrace, (NULL != bpaHandle));
    GT_assert (curTrace, (0 != size));


#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (NULL == bpaHandle) {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_alloc",
                             status,
                             "Handle is null!");
    }
    else if (size == 0u) {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_alloc",
                             status,
                             "size is 0u!");
    }
    else if ((size % handle->pageSize) != 0u) {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_alloc",
                             status,
                             "Size is not in multiple of pageSize!");
    }
    else if (((size / handle->pageSize) - 1u) >= handle->nrTotalPages) {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_alloc",
                             status,
                             "Size is more than total size of the full block!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        order = (size / handle->pageSize) - 1u;
        sBlk  = handle->sbArray + order;

        /* Check if exact size can be met */
        if (!List_empty ((List_Handle) &sBlk->blkList)) {
            retPtr = (Ptr) List_get ((List_Handle) &sBlk->blkList);
            /* Update the bitmap */
            /* Set 1 for the allocated block */
            bitPos = (  ((UInt32) retPtr - handle->virtBaseAddr)
                      / handle->pageSize);
            SET_BIT ((handle->map[bitPos / 8u]), (bitPos % 8u));
            if (   (handle->higher->order == order)
                && (List_empty ((List_Handle) &sBlk->blkList))) {
                handle->higher = NULL;
            }
        }
        else {
            if (handle->higher == NULL) {
                /* Join the buddies here */
                Bpa_join (bpaHandle);
            }

            if (handle->higher != NULL) {
                /* Try again, join buddies was sufficient for this call */
                if (handle->higher->order >= order) {
                    retPtr = (Ptr) List_get (
                                      (List_Handle) &handle->higher->blkList);
                    new_order = (handle->higher->order - order) - 1u;
                    if (new_order != -1u) {
                        /* Break the higher super block */
                        element = (List_Elem *) (  (UInt32)retPtr
                                                 + (  (order + 1u)
                                                    * handle->pageSize));
                        /* Initialize the remaining block */
                        List_elemClear (element);
                        ((struct BpaBlock *) element)->offset =
                                      ((UInt32) element - handle->virtBaseAddr);
                        ((struct BpaBlock *) element)->sBlock =
                                              (Ptr) &handle->sbArray[new_order];

                        /* Put the block in correct superblock */
                        List_putHead((List_Handle) &handle->sbArray[new_order],
                                     element);
                    }

                    /* Update the bitmap */
                    /* Set 1 for the allocated block */
                    bitPos = (  ((UInt32) retPtr - handle->virtBaseAddr)
                              / handle->pageSize);
                    SET_BIT ((handle->map[bitPos / 8u]), (bitPos % 8u));

                    if (new_order != -1u) {
                        /* Reset the remaining block */
                        bitPos = (  ((UInt32) element - handle->virtBaseAddr)
                                  / handle->pageSize);
                        CLEAR_BIT ((handle->map[bitPos / 8u]), (bitPos % 8u));
                    }

                    /* Update the higher super block */
                    if (new_order != -1u) {
                        /* if the higher super block is empty */
                        if (List_empty ((List_Handle)
                                            &handle->higher->blkList)) {
                            handle->higher = &handle->sbArray[new_order];
                        }
                    }
                    else {
                        handle->higher = NULL;
                    }
                }
            }
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (retPtr != NULL) {
        handle->nrFreePages -= (order + 1u);
    }

    GT_1trace (curTrace, GT_LEAVE, "Bpa_alloc", retPtr);

    /*! @retval Non-null Operation successful */
    /*! @retval null Operation not successful */
    return retPtr;
}


/*!
 *  @brief      Allocates a block of memory.
 *
 *  @param      bpaHandle  Pointer to handle.
 *  @param      blk     Pointer to the block.
 *  @param      size    Size of the block.
 *
 *  @sa         Bpa_join, Bpa_create, Bpa_delete, Bpa_alloc
 */
Int
Bpa_free (Ptr bpaHandle, Ptr blk, UInt32 size)
{
    Int                    status = BPA_SUCCESS;
    struct BpaObject *     handle = (struct BpaObject *) bpaHandle;
    UInt32                 order  = 0;
    struct BpaBlock *      element = NULL;
    UInt32                 bitPos;

    GT_3trace (curTrace, GT_ENTER, "Bpa_free", bpaHandle, blk, size);

    GT_assert (curTrace, (NULL != bpaHandle));
    GT_assert (curTrace, (NULL != blk));
    GT_assert (curTrace, (0 != size));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if ((NULL != bpaHandle) && (NULL != blk) && (0 != size)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        order = (size / handle->pageSize) - 1u;

        /* Initialize the  block */
        element = (struct BpaBlock *) blk;
        List_elemClear ((List_Elem *) element);
        element->offset = ((UInt32) element - handle->virtBaseAddr);
        element->sBlock = (Ptr) &handle->sbArray[order];

        /* put the block into the super block */
        List_putHead ((List_Handle) &handle->sbArray[order].blkList,
                      (List_Elem *) element);

        /* Reset the block's bitmap */
        bitPos = (((UInt32) element - handle->virtBaseAddr) / handle->pageSize);
        CLEAR_BIT ((handle->map[bitPos / 8u]), (bitPos % 8u));

        if (handle->higher == NULL) {
            handle->higher = &handle->sbArray[order];
        }
        else {
            if (handle->higher->order < order) {
                handle->higher = &handle->sbArray[order];
            }
        }

        handle->nrFreePages += (order + 1u);

        /* Fixme: this should be removed and code should adjust higher somehow
         * in the Bpa_alloc call
         */
        Bpa_join (bpaHandle);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
    else {
        /*! @retval BPA_E_INVALIDARG Handle is null */
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_free",
                             status,
                             "Handle is null!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Bpa_free", status);

    /*! @retval BPA_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Checks whether the block was allocated via buddy allocator or not
 *
 *  @param      bpaHandle  Pointer to handle.
 *  @param      blk        Pointer to block.
 */
Bool
Bpa_checkBlockOwnership (Ptr bpaHandle, Ptr blk)
{
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    Int                status = BPA_SUCCESS;
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
    struct BpaObject * handle = (struct BpaObject *) bpaHandle;
    /*! @retval FALSE Buffer is not allocated via buddy allocator */
    Bool               ret = FALSE;

    GT_2trace (curTrace, GT_ENTER, "Bpa_checkBlockOwnership", bpaHandle, blk);

    GT_assert (curTrace, (NULL != bpaHandle));
    GT_assert (curTrace, (NULL != blk));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if ((NULL != bpaHandle) && (blk != NULL)) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (   ((handle->virtBaseAddr + handle->size) > (UInt32) blk)
            && (handle->virtBaseAddr <= (UInt32) blk)) {
            /*! @retval TRUE Buffer is allocated via buddy allocator */
            ret = TRUE;
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
    else {
        status = BPA_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "Bpa_checkBlockOwnership",
                             status,
                             "Handle or block pointer is null!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "Bpa_checkBlockOwnership", ret);

    /*! @retval FALSE Buffer was not allocated via buddy allocator */
    return ret;
}
