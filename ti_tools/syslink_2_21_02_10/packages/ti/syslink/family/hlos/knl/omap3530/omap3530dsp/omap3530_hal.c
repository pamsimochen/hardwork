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
 *  @file   ti/syslink/family/hlos/knl/omap3530/omap3530dsp/omap3530_hal.c
 *
 *  @brief  Top-level Hardware Abstraction Module implementation
 *
 *          This module implements the top-level Hardware Abstraction Layer
 *          for OMAP3530.
 *          The implementation is specific to OMAP3530.
 */

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & utils headers */
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>

/* Hardware Abstraction Layer headers */
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>
#include <ti/syslink/inc/knl/Linux/omap3530_hal_mmu_defs.h>
#include <ti/syslink/inc/knl/omap3530_hal.h>
#include <ti/syslink/inc/knl/Linux/omap3530_phy_shmem.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */

/* =============================================================================
 * APIs called by OMAP3530PROC module
 * =============================================================================
 */
/*!
 *  @brief      Function to initialize the HAL object
 *
 *  @param      halObj      Return parameter: Pointer to the HAL object
 *  @param      initParams  Optional initialization parameters
 *
 *  @sa         OMAP3530_halExit
 *              OMAP3530_phyShmemInit
 */
Int
OMAP3530_halInit (Ptr * halObj, Ptr params)
{
    Int status = PROCESSOR_SUCCESS;

    GT_2trace (curTrace, GT_ENTER, "OMAP3530_halInit", halObj, params);

    GT_assert (curTrace, (halObj != NULL));

    (Void) params ; /* Not used. */

    *halObj = Memory_calloc (NULL, sizeof (OMAP3530_HalObject), 0, NULL);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (*halObj == NULL) {
        /*! @retval PROCESSOR_E_MEMORY Memory allocation failed */
        status = PROCESSOR_E_MEMORY;
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530_halInit",
                             status,
                             "Memory allocation failed for HAL object!");
    }
    else {
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */
        status = OMAP3530_phyShmemInit (*halObj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530_halInit",
                                 status,
                                 "OMAP3530_phyShmemInit failed!");
            Memory_free (NULL, *halObj, sizeof (OMAP3530_HalObject));
            *halObj = NULL;
        }
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530_halInit", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to finalize the HAL object
 *
 *  @param      halObj      Pointer to the HAL object
 *
 *  @sa         OMAP3530_halInit
 *              OMAP3530_phyShmemExit
 */
Int
OMAP3530_halExit (Ptr halObj)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAP3530_HalObject * halObject = NULL;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530_halExit", halObj);

    GT_assert (curTrace, (halObj != NULL));

    halObject = (OMAP3530_HalObject *) halObj ;
    status = OMAP3530_phyShmemExit (halObj);
#if !defined(SYSLINK_BUILD_OPTIMIZE)
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530_halExit",
                             status,
                             "OMAP3530_phyShmemExit failed!");
    }
#endif /* if !defined(SYSLINK_BUILD_OPTIMIZE) */

    if (halObj != NULL) {
        /* Free the memory for the HAL object. */
        Memory_free (NULL, halObj, sizeof (OMAP3530_HalObject));
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530_halExit", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
