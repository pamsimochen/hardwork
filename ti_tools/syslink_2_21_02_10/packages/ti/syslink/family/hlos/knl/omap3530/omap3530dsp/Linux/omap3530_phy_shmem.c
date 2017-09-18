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
 *  @file   ti/syslink/family/hlos/knl/omap3530/omap3530dsp/Linux/omap3530_phy_shmem.c
 *
 *  @brief  Linux shared memory physical interface file for OMAP3530.
 *
 *          This module is responsible for handling boot-related hardware-
 *          specific operations.
 *          The implementation is specific to OMAP3530.
 */

/* OS headers */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/dma-mapping.h>

#include <asm/uaccess.h>

/* Standard headers */
#include <ti/syslink/Std.h>

/* Module headers */
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>

/* OSAL and utils */
#include <ti/syslink/utils/Memory.h>
#include <ti/syslink/utils/Trace.h>

/* Hardware Abstraction Layer headers */
#include <ti/syslink/inc/knl/Linux/omap3530_hal_mmu_defs.h>
#include <ti/syslink/inc/knl/omap3530_hal.h>
#include <ti/syslink/inc/knl/omap3530_hal_reset.h>
#include <ti/syslink/inc/knl/omap3530_hal_boot.h>
#include <ti/syslink/inc/knl/Linux/omap3530_phy_shmem.h>
#include <ti/syslink/inc/knl/Linux/omap3530_hal_mmu.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */


/* =============================================================================
 * APIs called by OMAP3530PROC module
 * =============================================================================
 */
/*!
 *  @brief      Function to initialize Shared Driver/device.
 *
 *  @param      halObj  Pointer to the HAL object
 *
 *  @sa         OMAP3530_phyShmemExit
 *              Memory_map
 */
Int
OMAP3530_phyShmemInit (Ptr halObj)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAP3530_HalObject * halObject = NULL;
    Memory_MapInfo       mapInfo;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530_phyShmemInit", halObj);

    GT_assert (curTrace, (halObj != NULL));

    halObject = (OMAP3530_HalObject *) halObj;

    /* Map general control base */
    mapInfo.src      = GENERAL_CONTROL_BASE;
    mapInfo.size     = GENERAL_CONTROL_SIZE;
    mapInfo.isCached = FALSE;
    status = Memory_map (&mapInfo);
    if (status < 0) {
        GT_setFailureReason (curTrace,
                             GT_4CLASS,
                             "OMAP3530_phyShmemInit",
                             status,
                             "Failure in Memory_map for general ctrl base");
        halObject->generalCtrlBase = 0;
    }
    else {
        halObject->generalCtrlBase = mapInfo.dst;

        /* Map iva2CmBase */
        mapInfo.src      = IVA2_CM_BASE;
        mapInfo.size     = IVA2_CM_SIZE;
        mapInfo.isCached = FALSE;
        status = Memory_map (&mapInfo);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530_phyShmemInit",
                                 status,
                                 "Failure in Memory_map for general ctrl base");
            halObject->iva2CmBase = 0;
        }
        else {
            halObject->iva2CmBase = mapInfo.dst;

            /* Map coreCmBase */
            mapInfo.src      = CORE_CM_BASE;
            mapInfo.size     = CORE_CM_SIZE;
            mapInfo.isCached = FALSE;
            status = Memory_map (&mapInfo);
            if (status < 0) {
                GT_setFailureReason (curTrace,
                                     GT_4CLASS,
                                     "OMAP3530_phyShmemInit",
                                     status,
                                     "Failure in Memory_map for coreCmBase");
                halObject->coreCmBase = 0;
            }
            else {
                halObject->coreCmBase = mapInfo.dst;

                /* Map perCmBase */
                mapInfo.src      = PER_CM_BASE;
                mapInfo.size     = PER_CM_SIZE;
                mapInfo.isCached = FALSE;
                status = Memory_map (&mapInfo);
                if (status < 0) {
                    GT_setFailureReason (curTrace,
                                         GT_4CLASS,
                                         "OMAP3530_phyShmemInit",
                                         status,
                                         "Failure in Memory_map for perCmBase");
                    halObject->perCmBase = 0;
                }
                else {
                    halObject->perCmBase = mapInfo.dst;

                    /* Map iva2PrmBase */
                    mapInfo.src      = IVA2_PRM_BASE;
                    mapInfo.size     = IVA2_PRM_SIZE;
                    mapInfo.isCached = FALSE;
                    status = Memory_map (&mapInfo);
                    if (status < 0) {
                        GT_setFailureReason (curTrace,
                                             GT_4CLASS,
                                             "OMAP3530_phyShmemInit",
                                             status,
                                             "Failure in Memory_map for "
                                             "iva2PrmBase");
                        halObject->iva2PrmBase = 0;
                    }
                    else {
                        halObject->iva2PrmBase = mapInfo.dst;

                        /* Map MMU base */
                        mapInfo.src      = MMU_BASE;
                        mapInfo.size     = MMU_SIZE;
                        mapInfo.isCached = FALSE;
                        status = Memory_map (&mapInfo);
                        if (status < 0) {
                            GT_setFailureReason (curTrace,
                                                 GT_4CLASS,
                                                 "OMAP3530_phyShmemInit",
                                                 status,
                                                 "Failure in Memory_map for "
                                                 "MMU base registers");
                            halObject->mmuBase = 0;
                        }
                        else {
                            halObject->mmuBase = mapInfo.dst;
                        }
                    }
                }
            }
        }
    }



    GT_1trace (curTrace, GT_LEAVE, "OMAP3530_phyShmemInit", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to finalize Shared Driver/device.
 *
 *  @param      halObj  Pointer to the HAL object
 *
 *  @sa         OMAP3530_phyShmemInit
 *              Memory_Unmap
 */
Int
OMAP3530_phyShmemExit (Ptr halObj)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAP3530_HalObject * halObject = NULL;
    Memory_UnmapInfo     unmapInfo;

    GT_1trace (curTrace, GT_ENTER, "OMAP3530_phyShmemExit", halObj);

    GT_assert (curTrace, (halObj != NULL));

    halObject = (OMAP3530_HalObject *) halObj;

    iounmap((unsigned int *)halObject->generalCtrlBase);
    iounmap((unsigned int *)halObject->iva2CmBase);
    iounmap((unsigned int *)halObject->coreCmBase);
    iounmap((unsigned int *)halObject->perCmBase);
    iounmap((unsigned int *)halObject->iva2PrmBase);

    unmapInfo.addr = halObject->mmuBase;
    unmapInfo.size = MMU_SIZE;
    unmapInfo.isCached = FALSE;
    if (unmapInfo.addr != 0) {
        status = Memory_unmap (&unmapInfo);
        if (status < 0) {
            GT_setFailureReason (curTrace,
                              GT_4CLASS,
                              "OMAP3530_phyShmemExit",
                              status,
                              "Failure in Memory_Unmap for MMU base registers");
        }
        halObject->mmuBase = 0 ;
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530_phyShmemExit",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
