/*
 *  @file   HeapSmm.c
 *
 *  @brief      Cache line sized memory manager.
 *
 *              This module manages an shared region entry by providing
 *              application/modules to allocate/de-allocate memory. It requires
 *              one entry in the shared region to be reserved for it.
 *              CReator of this module should allocate required amount of memory
 *              either from SysMemMgr or from some other resource and passed to
 *              it.<br>
 *              Allocation and de-allocation is done through buddy page
 *              allocator with page size defined as cache line size. since the
 *              main purpose of this module is to allocate memory for sharing it
 *              with slave processors. It provides functionality such as
 *              @li allocate memory chunks
 *              @li de-allocate memory chunks
 *              @li getSRPtr for an address
 *              @li getPtr from a SRPtr.<br>
 *
 *              Application has to attach one callback with the processor
 *              manager. This callback would be called when a slave gets
 *              attached to the system. This callback needs to do the following:
 *              @Example
 *              @code
 *              void callback_function (UInt16 procId,...) {
 *                  HeapSmm_Config        smmConfig;
 *                  SharedRegion_Info info;
 *                  UInt32            vBaseAddress;
 *                  ...
 *                  HeapSmm_getConfig (&smmConfig);
 *                  # Call DMM API to map the physical address to a virtual
 *                  # address on slave.
 *
 *                  DMM_map (procId,
 *                           &vBaseAddress,
 *                           smmConfig->pBaseAddress,
 *                           smmConfig->size);
 *                  # Populate the HeapSmm Shared region entry into the
 *                  # procId's Shared region table
 *                  #
 *                  info.isValid = TRUE;
 *                  info.base    = vBaseAddress;
 *                  info.size    = smmConfig->size;
 *                  SharedRegion_setTableInfo (smmConfig->srIndex,
 *                                             procId,
 *                                             &info);
 *              }
 *              @endcode
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

/* Module level headers */
#include <ti/syslink/inc/knl/HeapSmm.h>
#include <ti/ipc/SharedRegion.h>
#include <ti/syslink/inc/_SharedRegion.h>
#include <ti/syslink/inc/knl/BuddyPageAllocator.h>


/*! @brief HeapSmm state object definition */
typedef struct HeapSmm_Object {
    Ptr            bpaHandle;  /*!< Handle for the buddy page allocator */
    HeapSmm_Config cfg;        /*!< Configuration values used for initiailzation */
    HeapSmm_Config defaultCfg; /*!< Default Configuration values used for initiailzation */
} HeapSmm_Object;


/*! @brief Variable to hold state of the HeapSmm */
#if !defined(SYSLINK_BUILD_DEBUG)
static
#endif /* if !defined(SYSLINK_BUILD_DEBUG) */
HeapSmm_Object HeapSmm_state = {
                                .defaultCfg.srIndex      = -1u,
                                .defaultCfg.vBaseAddress = 0u,
                                .defaultCfg.pBaseAddress = 0u,
                                .defaultCfg.size         = 0u,
                                .defaultCfg.cacheSize    = 128u,
                               };


/*! @brief  Function to get the current configuration values.
 *
 *  @params config Configuration values
 *
 *  @sa     HeapSmm_destroy
 */
Int
HeapSmm_getConfig (HeapSmm_Config * config)
{
    Int status = HEAPSMM_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "HeapSmm_getConfig", config);

    GT_assert (curTrace, (config != NULL));

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (config == NULL) {
        /*! @retval HEAPSMM_E_INVALIDARG Argument of type (HeapSmm_Config *)
         * passed is null */
        status = HEAPSMM_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapSmm_getConfig",
                             status,
                             "Argument of type (HeapSmm_Config *) passed "
                             "is null!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        if (HeapSmm_state.bpaHandle != NULL) {
            /* Not initialize pass the default */
            Memory_copy ((Ptr) config,
                         (Ptr) &HeapSmm_state.defaultCfg,
                         sizeof (HeapSmm_Config));
        }
        else {
            Memory_copy ((Ptr) config,
                         (Ptr) &HeapSmm_state.cfg,
                         sizeof (HeapSmm_Config));
        }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_ENTER, "HeapSmm_getConfig", status);

    /*! @retval HEAPSMM_SUCCESS operation was successful */
    return status;
}


/*! @brief  Function to setup the HeapSmm
 *
 *  @params config Configuration values
 *
 *  @sa     HeapSmm_destroy
 */
Int
HeapSmm_setup (HeapSmm_Config * config)
{
    Int              status = HEAPSMM_SUCCESS;
    struct BpaConfig bpaConfig;

    GT_1trace (curTrace, GT_ENTER, "HeapSmm_setup", config);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (config != NULL) {
        if (config->vBaseAddress == ((UInt32)NULL)) {
            /*! @retval HEAPSMM_E_INVALIDARG config->vBaseAddress is null */
            status = HEAPSMM_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_setup",
                                 status,
                                 "config->vBaseAddress is null!");
        }
        else if (config->pBaseAddress == ((UInt32)NULL)) {
            /*! @retval HEAPSMM_E_INVALIDARG config->pBaseAddress is null */
            status = HEAPSMM_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_setup",
                                 status,
                                 "config->pBaseAddress is null!");
        }
        else if (config->size == 0u) {
            /*! @retval HEAPSMM_E_INVALIDARG config->size is 0 */
            status = HEAPSMM_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_setup",
                                 status,
                                 "config->size is 0!");
        }
        else if (config->srIndex == -1u) {
            /*! @retval HEAPSMM_E_INVALIDARG config->srIndex is -1u */
            status = HEAPSMM_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_setup",
                                 status,
                                 "config->srIndex is -1u!");
        }
        else if (config->cacheSize == 0u) {
            /*! @retval HEAPSMM_E_INVALIDARG config->cacheSize is 0u */
            status = HEAPSMM_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_setup",
                                 status,
                                 "config->cacheSize is 0u!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            /* Copy the passed config values into the state object */
            Memory_copy ((Ptr) &HeapSmm_state.cfg,
                         (Ptr) config,
                         sizeof (HeapSmm_Config));

            /* Create buddy page allocator for cache size */
            bpaConfig.virtBaseAddr = config->vBaseAddress;
            bpaConfig.size         = config->size;
            bpaConfig.pageSize     = config->cacheSize;

            /* Create the static memory allocator */
            HeapSmm_state.bpaHandle = Bpa_create (&bpaConfig);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (HeapSmm_state.bpaHandle == NULL) {
                /*! @retval HEAPSMM_E_FAIL Failed to create buddy page
                 * allocator */
                status = HEAPSMM_E_FAIL;
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapSmm_setup",
                                     status,
                                     "Failed to create buddy page allocator!");
            }
            else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
                /* Add the entry into the shared region */
                //TBD
//                status = SharedRegion_add (config->srIndex,
//                                           (Ptr) config->vBaseAddress,
//                                           config->size);
//                if (status < 0) {
//                    Bpa_delete (HeapSmm_state.bpaHandle);
//                    GT_setFailureReason (curTrace,
//                                         GT_4CLASS,
//                                         "HeapSmm_setup",
//                                         status,
//                                         "Failed to add the entry into shared "
//                                         "region!");
//                }
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            }
        }
    }
    else {
        /*! @retval HEAPSMM_E_INVALIDARG Config parameter is null */
        status = HEAPSMM_E_INVALIDARG;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapSmm_setup",
                             status,
                             "Config parameter is null!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapSmm_setup", status);

    /*! @retval HEAPSMM_SUCCESS Operation was successful */
    return status;
}


/*! @brief  Function to destroy the HeapSmm
 *
 *  @sa     HeapSmm_setup
 */
Int
HeapSmm_destroy (void)
{
    Int status = HEAPSMM_SUCCESS;

    GT_0trace (curTrace, GT_ENTER, "HeapSmm_destroy");

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (HeapSmm_state.bpaHandle != NULL) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        Bpa_delete (HeapSmm_state.bpaHandle);
        //TBD
//        SharedRegion_remove (HeapSmm_state.cfg.srIndex);
        Memory_set (&HeapSmm_state.cfg, 0, sizeof (HeapSmm_Config));
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    }
    else {
        /*! @retval HEAPSMM_E_INVALIDSTATE HeapSmm module was not initialized */
        status = HEAPSMM_E_INVALIDSTATE;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapSmm_destroy",
                             status,
                             "HeapSmm module was not initialized!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapSmm_destroy", status);

    /*! @retval HEAPSMM_SUCCESS Operation was successful */
    return status;
}


/*! @brief  Function to allocate memory from HeapSmm.
 *
 *  @params size Size of the buffer
 *
 *  @sa     HeapSmm_free
 */
Ptr
HeapSmm_alloc (UInt32 size)
{
    Ptr retPtr = NULL;

    GT_1trace (curTrace, GT_ENTER, "HeapSmm_alloc", size);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (HeapSmm_state.bpaHandle != NULL) {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        retPtr = Bpa_alloc (HeapSmm_state.bpaHandle, size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (retPtr == NULL) {
            /*! @retval NULL HeapSmm module was not initialized */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_alloc",
                                 HEAPSMM_E_FAIL,
                                 "Failed to allocate memory from buddy page "
                                 "allocator!");
        }
    }
    else {
        /*! @retval NULL HeapSmm module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapSmm_alloc",
                             HEAPSMM_E_INVALIDSTATE,
                             "HeapSmm module was not initialized!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapSmm_alloc", retPtr);

    /*! @retval valid-pointer Operation was successful */
    return retPtr;
}


/*! @brief  Function to de-allocate memory to HeapSmm.
 *
 *  @params buf  Pointer of the buffer
 *  @params size Size of the buffer
 *
 *  @sa     HeapSmm_alloc
 */
Int
HeapSmm_free (Ptr buf, UInt32 size)
{
    Int status = HEAPSMM_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "HeapSmm_free", buf, size);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (HeapSmm_state.bpaHandle != NULL) {
        if (buf == NULL) {
            /*! @retval HEAPSMM_E_INVALIDARG Pointer to the buffer is null */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_free",
                                 HEAPSMM_E_INVALIDARG,
                                 "Pointer to the buffer is null!");
        }
        else if (size == 0) {
            /*! @retval HEAPSMM_E_INVALIDARG size is 0 */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_free",
                                 HEAPSMM_E_INVALIDARG,
                                 "size is 0!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            status = Bpa_free (HeapSmm_state.bpaHandle, buf, size);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapSmm_free",
                                     HEAPSMM_E_FAIL,
                                     "Failed to de-allocate memory from buddy page "
                                     "allocator!");
            }
        }
    }
    else {
        /*! @retval HEAPSMM_E_INVALIDSTATE HeapSmm module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapSmm_free",
                             HEAPSMM_E_INVALIDSTATE,
                             "HeapSmm module was not initialized!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapSmm_free", status);

    /*! @retval HEAPSMM_SUCCESS Operation was successful */
    return status;
}


/*! @brief  Function to get a SRPtr for the address.
 *
 *  @params addr Address of the buffer.
 *
 *  @sa     HeapSmm_getPtr
 */
SharedRegion_SRPtr
HeapSmm_getSRPtr (Ptr addr)
{
    SharedRegion_SRPtr  srPtr = SharedRegion_INVALIDSRPTR;

    GT_1trace (curTrace, GT_ENTER, "HeapSmm_getSRPtr", addr);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (HeapSmm_state.bpaHandle != NULL) {
        if (addr == NULL) {
            /*! @retval NULL Pointer to the buffer is null */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_getSRPtr",
                                 HEAPSMM_E_INVALIDARG,
                                 "addr is null!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            srPtr = SharedRegion_getSRPtr (addr, HeapSmm_state.cfg.srIndex);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (srPtr == SharedRegion_INVALIDSRPTR) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapSmm_getSRPtr",
                                     HEAPSMM_E_FAIL,
                                     "Failed to get SRPtr!");
            }
        }
    }
    else {
        /*! @retval NULL HeapSmm module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapSmm_getSRPtr",
                             HEAPSMM_E_INVALIDSTATE,
                             "HeapSmm module was not initialized!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "HeapSmm_getSRPPtr", srPtr);

    /*! @retval valid-SRPtr Operation was successful */
    return srPtr;
}


/*! @brief  Function to get a ptr from SRPtr.
 *
 *  @params srPtr Shared region SR pointer.
 *
 *  @sa     SharedRegion_SRPtr
 */
Ptr
HeapSmm_getPtr (SharedRegion_SRPtr srPtr)
{
    Ptr ptr = NULL;

    GT_1trace (curTrace, GT_ENTER, "HeapSmm_getPtr", srPtr);

#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (HeapSmm_state.bpaHandle != NULL) {
        if (srPtr == SharedRegion_INVALIDSRPTR) {
            /*! @retval NULL Pointer to the buffer is null */
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "HeapSmm_getPtr",
                                 HEAPSMM_E_INVALIDARG,
                                 "srPtr is null!");
        }
        else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
            ptr = SharedRegion_getPtr (srPtr);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
            if (ptr == 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "HeapSmm_getPtr",
                                     HEAPSMM_E_FAIL,
                                     "Failed to get ptr!");
            }
        }
    }
    else {
        /*! @retval NULL HeapSmm module was not initialized */
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "HeapSmm_getPtr",
                             HEAPSMM_E_INVALIDSTATE,
                             "HeapSmm module was not initialized!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "SMM_getPtr", ptr);

    /*! @retval valid-pointer Operation was successful */
    return ptr;
}
