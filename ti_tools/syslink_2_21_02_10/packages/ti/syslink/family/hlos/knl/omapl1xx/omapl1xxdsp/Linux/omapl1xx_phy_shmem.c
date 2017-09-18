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
 *  @file   ti/syslink/family/hlos/knl/omapl1xx/omapl1xxdsp/Linux/omapl1xx_phy_shmem.c
 *
 *  @brief  Linux shared memory physical interface file for OMAPL1XX.
 *
 *          This module is responsible for handling boot-related hardware-
 *          specific operations.
 *          The implementation is specific to OMAPL1XX.
 */

/* OS headers */
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
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
#include <ti/syslink/inc/knl/omapl1xx_hal.h>
#include <ti/syslink/inc/knl/omapl1xx_hal_reset.h>
#include <ti/syslink/inc/knl/omapl1xx_hal_boot.h>
#include <ti/syslink/inc/knl/Linux/omapl1xx_phy_shmem.h>



/* =============================================================================
 *  Macros and types
 * =============================================================================
 */


/* =============================================================================
 * APIs called by OMAPL1XXPROC module
 * =============================================================================
 */
/*!
 *  @brief      Function to initialize Shared Driver/device.
 *
 *  @param      halObj  Pointer to the HAL object
 *
 *  @sa         OMAPL1XX_phyShmemExit
 *              Memory_map
 */
Int
OMAPL1XX_phyShmemInit (Ptr halObj)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAPL1XX_HalObject * halObject = NULL;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XX_phyShmemInit", halObj);

    GT_assert (curTrace, (halObj != NULL));

    halObject = (OMAPL1XX_HalObject *) halObj;

    halObject->baseCfgBus = (UInt32) IO_ADDRESS(IO_PHYS);

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XX_phyShmemInit", status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/*!
 *  @brief      Function to finalize Shared Driver/device.
 *
 *  @param      halObj  Pointer to the HAL object
 *
 *  @sa         OMAPL1XX_phyShmemInit
 *              Memory_Unmap
 */
Int
OMAPL1XX_phyShmemExit (Ptr halObj)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAPL1XX_HalObject * halObject = NULL;

    GT_1trace (curTrace, GT_ENTER, "OMAPL1XX_phyShmemExit", halObj);

    GT_assert (curTrace, (halObj != NULL));

    halObject = (OMAPL1XX_HalObject *) halObj;

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XX_phyShmemExit",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
