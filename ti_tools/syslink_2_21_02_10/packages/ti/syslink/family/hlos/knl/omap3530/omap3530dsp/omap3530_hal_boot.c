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
 *  @file   ti/syslink/family/hlos/knl/omap3530/omap3530dsp/omap3530_hal_boot.c
 *
 *  @brief  Boot control module.
 *
 *          This module is responsible for handling boot-related hardware-
 *          specific operations.
 *          The implementation is specific to OMAP3530.
 */

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>

/* Module headers */
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>

/* Hardware Abstraction Layer headers */
#include <ti/syslink/inc/knl/Linux/omap3530_hal_mmu_defs.h>
#include <ti/syslink/inc/knl/omap3530_hal.h>
#include <ti/syslink/inc/knl/omap3530_hal_boot.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Offsets of the DSP BOOT ADDR register
 */
#define CONTROL_IVA2_BOOTADDR_OFFSET  0x190
#define CONTROL_IVA2_BOOTMOD_OFFSET   0x194


/* =============================================================================
 * APIs called by OMAP3530PROC module
 * =============================================================================
 */
/*!
 *  @brief      Function to control boot operations
 *
 *  @param      halObj  Pointer to the HAL object
 *  @param      cmd     Boot command
 *  @param      arg     Arguments specific to the boot command
 *
 *  @sa
 */
Int
OMAP3530_halBootCtrl (Ptr halObj, Processor_BootCtrlCmd cmd, Ptr args)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAP3530_HalObject * halObject = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAP3530_halBootCtrl", halObj, cmd, args);

    GT_assert (curTrace, (halObj != NULL));
    GT_assert (curTrace, (cmd < Processor_BootCtrlCmd_EndValue));

    halObject = (OMAP3530_HalObject *) halObj;

    switch (cmd) {
        case Processor_BootCtrlCmd_SetEntryPoint:
        {
            /* Set the boot address */
            REG (halObject->generalCtrlBase + CONTROL_IVA2_BOOTADDR_OFFSET) =
                ((UInt32) args & 0xFFFFFC00);


            /* Set the boot mode */
            REG (halObject->generalCtrlBase + CONTROL_IVA2_BOOTMOD_OFFSET) =
                (UInt32) 0;

        }
        break ;

        case Processor_BootCtrlCmd_SetBootComplete:
        {
            /* Do nothing here for OMAP3530 gem */
        }
        break ;

        case Processor_BootCtrlCmd_ResetBootComplete:
        {
            /* Do nothing here for OMAP3530 gem */
        }
        break ;

        default:
        {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530_halBootCtrl",
                                 status,
                                 "Unsupported boot ctrl cmd specified");
        }
        break ;
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530_halBootCtrl",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
