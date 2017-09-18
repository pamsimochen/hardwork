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
 *  @file   ti/syslink/inc/knl/Linux/omap3530_phy_shmem.h
 *
 *  @brief      Physical Interface Abstraction Layer for OMAP3530.
 *
 *              This file contains the definitions for shared memory physical
 *              link being used with OMAP3530.
 *              The implementation is specific to OMAP3530.
 */

#ifndef omap3530_phy_shmem_H_0xbbec
#define omap3530_phy_shmem_H_0xbbec


#if defined (__cplusplus)
extern "C" {
#endif


/* =============================================================================
 *  Macros and types
 *  See _ProcDefs.h
 * =============================================================================
 */
/*!
 *  @brief  Base addresse of system control reg.
 */
#define GENERAL_CONTROL_BASE   0x48002270
/*!
 *  @brief  size to be ioremapped.
 */
#define GENERAL_CONTROL_SIZE   0x2FF
/*!
 *  @brief  IVA2_CM_BASE Base addresse.
 */
#define IVA2_CM_BASE           0x48004000
/*!
 *  @brief  size to be ioremapped.
 */
#define IVA2_CM_SIZE           0x2000
/*!
 *  @brief  CORE_CM_BASE Base addresse.
 */
#define CORE_CM_BASE           0x48004A00
/*!
 *  @brief  size to be ioremapped.
 */
#define CORE_CM_SIZE           0x2000
/*!
 *  @brief  PER_CM_BASE Base addresse.
 */
#define PER_CM_BASE            0x48005000
/*!
 *  @brief  size to be ioremapped.
 */
#define PER_CM_SIZE            0x2000
/*!
 *  @brief  IVA2_PRM_BASE Base addresse.
 */
#define IVA2_PRM_BASE          0x48306000
/*!
 *  @brief  size to be ioremapped.
 */
#define IVA2_PRM_SIZE          0x2000

#define MMU_BASE               0x5D000000
/*!
 *  @brief  size to be ioremapped.
 */
#define MMU_SIZE               0x1000


/* =============================================================================
 *  APIs
 * =============================================================================
 */
/* Initializes Shared Driver/device. */
Int OMAP3530_phyShmemInit (Ptr halObj);

/* Finalizes Shared Driver/device. */
Int OMAP3530_phyShmemExit (Ptr halObj);


#if defined (__cplusplus)
}
#endif /* defined (__cplusplus) */

#endif /* omap3530_phy_shmem_H_0xbbec */
