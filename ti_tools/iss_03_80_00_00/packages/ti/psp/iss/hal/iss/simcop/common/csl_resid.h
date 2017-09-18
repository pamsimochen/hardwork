/** ==================================================================
 *  @file   csl_resid.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ========================================================================== 
 * Copyright (c) Texas Instruments Inc , 2004 Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied provided
 * ========================================================================== */

/* ---- File: <csl_resid.h> ---- */
/* for the ARM-side of Davinci */
#ifndef _CSL_RESID_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSL_RESID_H_

/* ---- Total number of modues : CSL Abstraction ---- */
/* Module ID 0 is reserved. */
#define CSL_CHIP_NUM_MODULES   	  (9)

#define CSL_SIMCOP_ID                 (1)
#define CSL_ROT_ID                (2)
#define CSL_DCT_ID               (3)
#define CSL_NSF_ID                 (4)
#define CSL_LDC_ID                 (5)
#define CSL_VLCDJ_ID               (6)
#define CSL_IMXA_ID               (7)
#define CSL_IMXB_ID                 (8)
#define CSL_COPDMA_ID                (9)

/* ---- Total Number of Resource, one-to-one correspondence with H/W resource 
 */
/* Resource ID - 0 is resvd. */
#define CSL_CHIP_NUM_RESOURCES  (16)

#define CSL_SIMCOP_0_UID              (1)
#define CSL_ROT_0_UID                    (2)
#define CSL_DCT_0_UID                    (3)
#define CSL_NSF_0_UID                    (4)
#define CSL_LDC_0_UID                    (5)
#define CSL_VLCDJ_0_UID                 (6)
#define CSL_IMXA_0_UID                  (7)
#define CSL_IMXB_0_UID                  (8)

#define CSL_COPDMA_CHA0_UID       (9)
#define CSL_COPDMA_CHA1_UID       (10)
#define CSL_COPDMA_CHA2_UID       (11)
#define CSL_COPDMA_CHA3_UID       (12)
#define CSL_COPDMA_CHA4_UID       (13)
#define CSL_COPDMA_CHA5_UID       (14)
#define CSL_COPDMA_CHA6_UID       (15)
#define CSL_COPDMA_CHA7_UID       (16)

/*----  XIO masks for peripheral instances ---- */
#define CSL_ROT_0_XIO               (0x00000000)
#define CSL_DCT_0_XIO               (0x00000000)
#define CSL_NSF_0_XIO               (0x00000000)
#define CSL_LDC_0_XIO               (0x00000000)
#define CSL_VLCDJ_0_XIO              (0x00000000)
#define CSL_IMXA_0_XIO              (0x00000000)
#define CSL_IMXB_0_XIO              (0x00000000)
#define CSL_SIMCOP_0_XIO              (0x00000000)

#define CSL_COPDMA_CHA0_XIO          (0x00000000)
#define CSL_COPDMA_CHA1_XIO          (0x00000000)
#define CSL_COPDMA_CHA2_XIO          (0x00000000)
#define CSL_COPDMA_CHA3_XIO          (0x00000000)
#define CSL_COPDMA_CHA4_XIO          (0x00000000)
#define CSL_COPDMA_CHA5_XIO          (0x00000000)
#define CSL_COPDMA_CHA6_XIO          (0x00000000)
#define CSL_COPDMA_CHA7_XIO          (0x00000000)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _CSL_RESOURCE_H_ 
                                                            */

/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 7 Dec 14:17:09 7 4658 xkeshavm */
/* */
/* Uploaded the CSL Dec6 2004 Release */
/********************************************************************/
/* Rev.No.  Date/Time ECN No.  Modifier */
/* ------- --------- ------- -------- */

/* 1 13 Nov 2005 12:48:06 401 x0029935 */
/* */
/* Modified from davicni CSL files to Dm350 */
/* First CSL files build up fro Dm350 */
/********************************************************************/
