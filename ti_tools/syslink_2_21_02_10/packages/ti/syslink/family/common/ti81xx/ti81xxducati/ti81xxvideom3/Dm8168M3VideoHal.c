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
 *  @file   ti/syslink/family/common/ti81xx/ti81xxducati/ti81xxvideom3/Dm8168M3VideoHal.c
 *
 *  @brief  Top-level Hardware Abstraction Module implementation
 *
 *          This module implements the top-level Hardware Abstraction Layer
 *          for DM8168VIDEOM3.
 *          The implementation is specific to DM8168VIDEOM3.
 */

#if defined(SYSLINK_BUILD_RTOS)
#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>
#endif

#if defined(SYSLINK_BUILD_HLOS)
#include <ti/syslink/Std.h>
#include <ti/syslink/utils/Memory.h>
#endif

#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>
#include <ti/syslink/inc/knl/Dm8168M3VideoHal.h>
#include <ti/syslink/inc/knl/Dm8168M3VideoHalBoot.h>
#include <ti/syslink/inc/knl/Dm8168M3VideoPhyShmem.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

/* =============================================================================
 * APIs called by DM8168VIDEOM3PROC module
 * =============================================================================
 */
/*!
 *  @brief      Function to initialize the HAL object
 *
 *  @param      halObj      Return parameter: Pointer to the HAL object
 *  @param      initParams  Optional initialization parameters
 *
 *  @sa         DM8168VIDEOM3_halExit
 *              DM8168VIDEOM3_phyShmemInit
 */
Int
DM8168VIDEOM3_halInit (Ptr * halObj, Ptr params)
{
    Int                  status    = PROCESSOR_SUCCESS;
    DM8168VIDEOM3_HalObject * halObject = NULL;

    GT_2trace (curTrace, GT_ENTER, "DM8168VIDEOM3_halInit", halObj, params);

    GT_assert (curTrace, (halObj != NULL));

    halObject = (DM8168VIDEOM3_HalObject *) halObj ;

    (Void) params ; /* Not used. */

    *halObj = Memory_calloc (NULL, sizeof (DM8168VIDEOM3_HalObject), 0, NULL);
    if (halObject == NULL) {
        /*! @retval PROCESSOR_E_MEMORY Memory allocation failed */
        status = PROCESSOR_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "DM8168VIDEOM3_halInit",
                             status,
                             "Memory allocation failed for HAL object!");
    }
    else {
        status = DM8168VIDEOM3_phyShmemInit (*halObj);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "DM8168VIDEOM3_halInit",
                                 status,
                                 "DM8168VIDEOM3_phyShmemInit failed!");
            Memory_free (NULL, *halObj, sizeof (DM8168VIDEOM3_HalObject));
            *halObj = NULL;
        }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */
    }

    GT_1trace (curTrace, GT_LEAVE, "DM8168VIDEOM3_halInit", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to finalize the HAL object
 *
 *  @param      halObj      Pointer to the HAL object
 *
 *  @sa         DM8168VIDEOM3_halInit
 *              DM8168VIDEOM3_phyShmemExit
 */
Int
DM8168VIDEOM3_halExit (Ptr halObj)
{
    Int                  status    = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_ENTER, "DM8168VIDEOM3_halExit", halObj);

    GT_assert (curTrace, (halObj != NULL));

    status = DM8168VIDEOM3_phyShmemExit (halObj);
#if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "DM8168VIDEOM3_halExit",
                             status,
                             "DM8168VIDEOM3_phyShmemExit failed!");
    }
#endif /* #if !defined(SYSLINK_BUILD_OPTIMIZE) && defined (SYSLINK_BUILD_HLOS) */

    if (halObj != NULL) {
        /* Free the memory for the HAL object. */
        Memory_free (NULL, halObj, sizeof (DM8168VIDEOM3_HalObject));
    }

    GT_1trace (curTrace, GT_LEAVE, "DM8168VIDEOM3_halExit", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
