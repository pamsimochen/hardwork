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
 *  @file   ti/syslink/family/hlos/knl/omap3530/omap3530dsp/omap3530_hal_reset.c
 *
 *  @brief  Reset control module.
 *
 *          This module is responsible for handling reset-related hardware-
 *          specific operations.
 *          The implementation is specific to OMAP3530.
 */

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/inc/Bitops.h>

/* Module headers */
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>

/* Hardware Abstraction Layer headers */
#include <ti/syslink/inc/knl/Linux/omap3530_hal_mmu_defs.h>
#include <ti/syslink/inc/knl/omap3530_hal.h>
#include <ti/syslink/inc/knl/omap3530_hal_reset.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Offsets from the IVA2 PRM base address
 */
#define RM_RSTCTRL_DSP_OFFSET       0x50

/*!
 *  @brief  Offsets from the IVA2 PRM base address
 */
#define CM_FCLKEN_IVA2_OFFSET       0x00
#define CM_CLKSTCTRL_IVA2_OFFSET    0x48

/*!
 *  @brief  Offsets from the PER_CM base address
 */
#define CM_FCLKEN_PER_OFFSET        0x00
#define CM_ICLKEN_PER_OFFSET        0x10
#define CM_CLKSEL_PER_OFFSET        0x40


/* =============================================================================
 * APIs called by OMAP3530PROC module
 * =============================================================================
 */
/*!
 *  @brief      Function to control reset operations
 *
 *  @param      halObj  Pointer to the HAL object
 *  @param      cmd     Reset control command
 *  @param      arg     Arguments specific to the reset control command
 *
 *  @sa
 */
Int
OMAP3530_halResetCtrl (Ptr halObj, Processor_ResetCtrlCmd cmd, Ptr args)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAP3530_HalObject * halObject = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAP3530_halResetCtrl", halObj, cmd, args);

    GT_assert (curTrace, (halObj != NULL));
    GT_assert (curTrace, (cmd < Processor_ResetCtrlCmd_EndValue));

    halObject = (OMAP3530_HalObject *) halObj ;

    switch (cmd) {
        case Processor_ResetCtrlCmd_Reset:
        {
            /* Disable GPTimer5 FCLK.
             * This requires that the DSP configuration use Timer 5!
             */
            CLEAR_BIT ( REG (halObject->perCmBase + CM_FCLKEN_PER_OFFSET), 6);

            /* Disable GPTimer5 ICLK.
             * This requires that the DSP configuration use Timer 5!
             */
            CLEAR_BIT ( REG (halObject->perCmBase + CM_ICLKEN_PER_OFFSET), 6);

            /* Disable automatic transitions. */
            REG (halObject->iva2CmBase + CM_CLKSTCTRL_IVA2_OFFSET) = 0;

            /* Turn off IVA2 functional clock */
            REG (halObject->iva2CmBase + CM_FCLKEN_IVA2_OFFSET) = 0;

            /* Reset RST3_IVA2 and RST1_IVA2 */
            REG (halObject->iva2PrmBase + RM_RSTCTRL_DSP_OFFSET) =  5;

            /* Turn on IVA2 functional clock */
            REG (halObject->iva2CmBase + CM_FCLKEN_IVA2_OFFSET) = 1;

            /* Enable automatic transitions. */
            REG (halObject->iva2CmBase + CM_CLKSTCTRL_IVA2_OFFSET) = 3;

        }
        break ;

        case Processor_ResetCtrlCmd_Release:
        {
            /* Enable GPTimer5 FCLK.
             * This requires that the DSP configuration use Timer 5!
             */
            SET_BIT (REG (halObject->perCmBase + CM_FCLKEN_PER_OFFSET), 6);

            /* Enable GPTimer5 ICLK.
             * This requires that the DSP configuration use Timer 5!
             */
            SET_BIT (REG (halObject->perCmBase + CM_ICLKEN_PER_OFFSET), 6);

            /* Setting source to Sys_clk */
            SET_BIT (REG (halObject->perCmBase + CM_CLKSEL_PER_OFFSET), 3);

            /* Release DSP via RST1_IVA2 */
            CLEAR_BIT (REG (halObject->iva2PrmBase + RM_RSTCTRL_DSP_OFFSET), 0);

        }
        break;

        case Processor_ResetCtrlCmd_PeripheralUp:
        {
            /* Nothing to be done to bringup the peripherals for this device. */
        }
        break;

        default:
        {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAP3530_halResetCtrl",
                                 status,
                                 "Unsupported reset ctrl cmd specified");
        }
        break ;
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAP3530_halResetCtrl",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
