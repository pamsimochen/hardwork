/** 
 *  @file   _ProcDefs.h
 *
 *  @brief      Internal definitions for the Processor interface.
 *
 *
 */
/* 
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



#ifndef _ProcDefs_H_0x6a85
#define _ProcDefs_H_0x6a85


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */
/* -----------------------------------------------------------------------------
 *  HAL Boot
 * -----------------------------------------------------------------------------
 */
/*!
 *  @brief  Enumerations to indicate types of boot control commands supported
 *          for the slave processor.
 */
typedef enum {
    Processor_BootCtrlCmd_SetEntryPoint     = 0u,
    /*!< Set entry point */
    Processor_BootCtrlCmd_SetBootComplete   = 1u,
    /*!< Indicate complete of boot sequence */
    Processor_BootCtrlCmd_ResetBootComplete = 2u,
    /*!< Reset the boot complete boot flag. */
    Processor_BootCtrlCmd_EndValue          = 3u
    /*!< End delimiter indicating start of invalid values for this enum */
} Processor_BootCtrlCmd;

/* -----------------------------------------------------------------------------
 *  HAL Reset
 * -----------------------------------------------------------------------------
 */
/*!
 *  @brief  Enumerations to indicate types of reset control commands supported
 *          for the slave processor.
 */
typedef enum {
    Processor_ResetCtrlCmd_Reset        = 0u,
    /*!< Reset the slave processor. */
    Processor_ResetCtrlCmd_Release      = 1u,
    /*!< Release the slave processor from reset. */
    Processor_ResetCtrlCmd_PeripheralUp = 2u,
    /*!< Configure the required peripherals. */
    Processor_ResetCtrlCmd_EndValue     = 3u
    /*!< End delimiter indicating start of invalid values for this enum */
} Processor_ResetCtrlCmd ;

/* -----------------------------------------------------------------------------
 *  HAL MMU
 * -----------------------------------------------------------------------------
 */
/*!
 *  @brief  Enumerations to indicate types of MMU control commands supported
 *          for the slave processor.
 */
typedef enum {
    Processor_MmuCtrlCmd_Enable         = 0u,
    /*!< Enable slave MMU. */
    Processor_MmuCtrlCmd_Disable        = 1u,
    /*!< Disable slave MMU. */
    Processor_MmuCtrlCmd_AddEntry       = 2u,
    /*!< Add entry in the MMU table. */
    Processor_MmuCtrlCmd_DeleteEntry    = 3u,
    /*!< Delete entry from the MMU table. */
    Processor_MmuCtrlCmd_EndValue       = 4u
    /*!< End delimiter indicating start of invalid values for this enum */
} Processor_MmuCtrlCmd ;


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* _ProcDefs_H_0x6a85 */
