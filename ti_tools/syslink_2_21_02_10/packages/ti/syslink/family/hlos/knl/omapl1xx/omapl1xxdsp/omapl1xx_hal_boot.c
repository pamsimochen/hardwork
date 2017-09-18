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
 *  @file   ti/syslink/family/hlos/knl/omapl1xx/omapl1xxdsp/omapl1xx_hal_boot.c
 *
 *  @brief  Boot control module.
 *
 *          This module is responsible for handling boot-related hardware-
 *          specific operations.
 *          The implementation is specific to OMAPL1XX.
 */

/* Standard headers */
#include <ti/syslink/Std.h>

/* OSAL & Utils headers */
#include <ti/syslink/utils/Trace.h>

/* Module headers */
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>

/* Hardware Abstraction Layer headers */
#include <ti/syslink/inc/knl/omapl1xx_hal.h>
#include <ti/syslink/inc/knl/omapl1xx_hal_boot.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/*!
 *  @brief  Offset of the HOST1CFG register from the base of the CFG
 *          memory.
 */
#define BOOT_BASE_ADDR      0x44


/* =============================================================================
 * APIs called by OMAPL1XXPROC module
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
OMAPL1XX_halBootCtrl (Ptr halObj, Processor_BootCtrlCmd cmd, Ptr args)
{
    Int                  status    = PROCESSOR_SUCCESS;
    OMAPL1XX_HalObject * halObject = NULL;

    GT_3trace (curTrace, GT_ENTER, "OMAPL1XX_halBootCtrl", halObj, cmd, args);

    GT_assert (curTrace, (halObj != NULL));
    GT_assert (curTrace, (cmd < Processor_BootCtrlCmd_EndValue));

    halObject = (OMAPL1XX_HalObject *) halObj;

    switch (cmd) {
        case Processor_BootCtrlCmd_SetEntryPoint:
        {
            /* Set the boot address */
            REG (   halObject->baseCfgBus
                 +  halObject->offsetSysModule
                 +  BOOT_BASE_ADDR) = ((UInt32) args | 0x1);
        }
        break ;

        case Processor_BootCtrlCmd_SetBootComplete:
        {
            /* Do nothing here for OMAPL1XX gem */
        }
        break ;

        case Processor_BootCtrlCmd_ResetBootComplete:
        {
            /* Do nothing here for OMAPL1XX gem */
        }
        break ;

        default:
        {
            /*! @retval PROCESSOR_E_INVALIDARG Invalid argument */
            status = PROCESSOR_E_INVALIDARG;
            GT_setFailureReason (curTrace,
                                 GT_4CLASS,
                                 "OMAPL1XX_halBootCtrl",
                                 status,
                                 "Unsupported boot ctrl cmd specified");
        }
        break ;
    }

    GT_1trace (curTrace, GT_LEAVE, "OMAPL1XX_halBootCtrl",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}
