/*   ==========================================================================
 *   Copyright (c) Texas Instruments Inc , 2004
 *
 *   Use of this software is controlled by the terms and conditions found
 *   in the license agreement under which this software has been supplied
 *   provided
 *   ==========================================================================
 */
#ifndef _DAVINCI_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _DAVINCI_H_

/*****************************************************************************/
/** \file davinci.h
 * 
 * \brief This file contains the Chip Description for DAVINCI (DSP side)
 * 
 *****************************************************************************/
#include <cslr.h>
#include <tistdtypes.h>

#define CSL_IDEF_INLINE static

#include "cslr_edmacc_001.h"
#include "cslr_edmatc0_001.h"
#include "cslr_edmatc1_001.h"

/*****************************************************************************\
 * Peripheral Instance count
\*****************************************************************************/
#define CSL_EDMA_CC_CNT             1
#define CSL_EDMA_TC0_CNT            1
#define CSL_EDMA_TC1_CNT            1

/*****************************************************************************\
 * Overlay structure typedef definition
\*****************************************************************************/
typedef volatile CSL_EdmaccRegs         *CSL_EdmaccRegsOvly;
typedef volatile CSL_EdmaccShadowRegs   *CSL_EdmaccShadowRegsOvly;
typedef volatile CSL_Edmatc0Regs        *CSL_Edmatc0RegsOvly;
typedef volatile CSL_Edmatc1Regs        *CSL_Edmatc1RegsOvly;

/*****************************************************************************\
 * Peripheral Base Address
\*****************************************************************************/

#define CSL_EDMACC_0_REGS           ((CSL_EdmaccRegsOvly)  0x49000000u)
#define CSL_EDMATC_0_REGS           ((CSL_Edmatc0RegsOvly) 0x49800000u)
#define CSL_EDMATC_1_REGS           ((CSL_Edmatc1RegsOvly) 0x49900000u)
#define CSL_EDMATC_2_REGS           ((CSL_Edmatc1RegsOvly) 0x49A00000u)
#define CSL_EDMATC_3_REGS           ((CSL_Edmatc1RegsOvly) 0x49B00000u)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif


