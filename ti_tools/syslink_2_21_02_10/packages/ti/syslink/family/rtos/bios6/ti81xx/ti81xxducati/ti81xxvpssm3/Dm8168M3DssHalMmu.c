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
 *  @file   ti/syslink/family/rtos/bios6/ti81xx/ti81xxducati/ti81xxvpssm3/Dm8168M3DssHalMmu.c
 *
 *  @brief  Hardware abstraction for Memory Management Unit module.
 *
 *          This module is responsible for handling slave MMU-related
 *          hardware- specific operations.
 *          The implementation is specific to DM8168VPSSM3.
 */

/* Standard headers */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>

/* OSAL and utils headers */
#include <ti/syslink/utils/List.h>
#include <ti/syslink/utils/Trace.h>
#include <ti/syslink/utils/OsalPrint.h>
#include <ti/syslink/utils/_Memory.h>

/* Module level headers */
#include <ti/syslink/inc/knl/_ProcDefs.h>
#include <ti/syslink/inc/knl/Processor.h>

/* Hardware Abstraction Layer headers */
#include <ti/syslink/inc/knl/Dm8168M3DssHal.h>
#include <ti/syslink/inc/knl/Dm8168M3DssHalMmu.h>
#include <ti/syslink/inc/knl/Dm8168M3DssPhyShmem.h>


/* =============================================================================
 *  Macros and types
 * =============================================================================
 */


/* =============================================================================
 *  Forward declarations of internal functions
 * =============================================================================
 */
/* Enables the MMU for GEM Module. */
Int _DM8168VPSSM3_halMmuEnable (DM8168VPSSM3_HalObject * halObject,
                            UInt32               numMemEntries,
                            ProcMgr_AddrInfo *   memTable);

/* Disables the MMU for GEM Module. */
Int _DM8168VPSSM3_halMmuDisable (DM8168VPSSM3_HalObject * halObject);

/* Add entry in TWL. */
Int
_DM8168VPSSM3_halMmuAddEntry (DM8168VPSSM3_HalObject       * halObject,
                          DM8168VPSSM3_HalMmuEntryInfo * entry);

/* Add static entries in TWL. */
Int
_DM8168VPSSM3_halMmuAddStaticEntries (DM8168VPSSM3_HalObject * halObject,
                                  UInt32               numMemEntries,
                                  ProcMgr_AddrInfo *   memTable);

/* Delete entry from TLB. */
Int
_DM8168VPSSM3_halMmuDeleteEntry (DM8168VPSSM3_HalObject       * halObject,
                             DM8168VPSSM3_HalMmuEntryInfo * entry);
/* Set entry in to TLB. */
Int
_DM8168VPSSM3_halMmuPteSet (DM8168VPSSM3_HalObject       * halObject,
                        DM8168VPSSM3_HalMmuEntryInfo * setPteInfo);

/* =============================================================================
 * APIs called by DM8168VPSSM3PROC module
 * =============================================================================
 */
/*!
 *  @brief      Function to control MMU operations for this slave device.
 *
 *  @param      halObj  Pointer to the HAL object
 *  @param      cmd     MMU control command
 *  @param      arg     Arguments specific to the MMU control command
 *
 *  @sa
 */
Int
DM8168VPSSM3_halMmuCtrl (Ptr halObj, Processor_MmuCtrlCmd cmd, Ptr args)
{
    Int                  status     = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_LEAVE, "DM8168VPSSM3_halMmuCtrl",status);

    /*! @retval PROCESSOR_SUCCESS Operation successful */
    return status;
}


/* =============================================================================
 * Internal functions
 * =============================================================================
 */
/*!
 *  @brief      Enables and configures the MMU as per provided memory map.
 *
 *  @param      halObject       Pointer to the HAL object
 *  @param      numMemEntries   Number of memory entries in memTable
 *  @param      memTable        Table of memory entries to be configured
 *
 *  @sa
 */

Int
_DM8168VPSSM3_halMmuAddStaticEntries (DM8168VPSSM3_HalObject * halObject,
                                  UInt32               numMemEntries,
                                  ProcMgr_AddrInfo *   memTable)
{
    Int                           status    = PROCESSOR_SUCCESS;


    GT_1trace (curTrace, GT_LEAVE, "_DM8168VPSSM3_halMmuAddStaticEntries", status);

    /*! @retval PROCESSOR_SUCCESS Operation completed successfully. */
    return status ;
}

/*!
 *  @brief      Enables and configures the MMU as per provided memory map.
 *
 *  @param      halObject       Pointer to the HAL object
 *  @param      numMemEntries   Number of memory entries in memTable
 *  @param      memTable        Table of memory entries to be configured
 *
 *  @sa
 */

Int
_DM8168VPSSM3_halMmuEnable (DM8168VPSSM3_HalObject * halObject,
                        UInt32               numMemEntries,
                        ProcMgr_AddrInfo *   memTable)
{
    Int                           status    = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_LEAVE, "_DM8168VPSSM3_halMmuEnable", status);

    /*! @retval PROCESSOR_SUCCESS Operation completed successfully. */
    return status ;
}

/*!
 *  @brief      Disables the MMU.
 *
 *  @param      halObject       Pointer to the HAL object
 *
 *  @sa
 */
Int
_DM8168VPSSM3_halMmuDisable (DM8168VPSSM3_HalObject * halObject)
{
    Int                        status    = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_LEAVE, "_DM8168VPSSM3_halMmuDisable", status);

    /*! @retval PROCESSOR_SUCCESS Operation completed successfully. */
    return status;
}


/*!
 *  @brief      This function adds an MMU entry for the specified address and
 *              size.
 *
 *  @param      halObject   Pointer to the HAL object
 *  @param      entry       entry to be added
 *
 *  @sa
 */
Int
_DM8168VPSSM3_halMmuAddEntry (DM8168VPSSM3_HalObject       * halObject,
                          DM8168VPSSM3_HalMmuEntryInfo * entry)
{
    Int                         status = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_LEAVE, "_DM8168VPSSM3_halMmuAddEntry", status);

    /*! @retval PROCESSOR_SUCCESS Operation completed successfully. */
    return status;
}


/*!
 *  @brief      This function deletes an MMU entry for the specfied address and
 *              size.
 *
 *  @param      halObject   Pointer to the HAL object
 *  @param      slaveVirtAddr DSP virtual address of the memory region
 *  @param      size        Size of the memory region to be configured
 *  @param      isDynamic   Is the MMU entry being dynamically added?
 *
 *  @sa
 */
Int
_DM8168VPSSM3_halMmuDeleteEntry (DM8168VPSSM3_HalObject       * halObject,
                             DM8168VPSSM3_HalMmuEntryInfo * entry)
{
    Int                         status      = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_LEAVE, "_DM8168VPSSM3_halMmuDeleteEntry", status);

    /*! @retval PROCESSOR_SUCCESS Operation completed successfully. */
    return status;
}
/*!
 *  @brief      Updates entries in table.
 *
 *  @param      refData Argument provided to the ISR registration function
 *
 *  @sa
 */
Int
_DM8168VPSSM3_halMmuPteSet (DM8168VPSSM3_HalObject *      halObject,
                        DM8168VPSSM3_HalMmuEntryInfo* setPteInfo)
{
    Int    status = PROCESSOR_SUCCESS;

    GT_1trace (curTrace, GT_LEAVE, "_DM8168VPSSM3_halMmuPteSet", status);

    /*! @retval PROCESSOR_SUCCESS Operation completed successfully. */
    return status;
}
