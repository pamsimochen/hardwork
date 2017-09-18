/** ==================================================================
 *  @file   _simcop.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ========================================================================== 
 * Copyright (c) Texas Instruments Inc , 2006 Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied provided
 * ========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include "../simcop.h"

void *_CSL_rotlookup[] = { (void *) CSL_ROT_0_REGS };
void *_CSL_dctlookup[] = { (void *) CSL_DCT_0_REGS };
void *_CSL_nsflookup[] = { (void *) CSL_NSF_0_REGS };
void *_CSL_ldclookup[] = { (void *) CSL_LDC_0_REGS };
void *_CSL_vlcdjlookup[] = { (void *) CSL_VLCDJ_0_REGS };
void *_CSL_imxlookup[] = { (void *) CSL_IMX_0_REGS };
void *_CSL_simcoplookup[] = { (void *) CSL_SIMCOP_0_REGS };
void *_CSL_copdmalookup[] = { (void *) CSL_COPDMA_0_REGS };
#ifdef __cplusplus
}
#endif /* __cplusplus */
