/** ==================================================================
 *  @file   cslr__ldc.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/ldc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _CSLR__LDC_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR__LDC_H_

#include <ti/psp/iss/hal/iss/simcop/common/cslr.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>

/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct {
    volatile Uint32 PID;
    volatile Uint32 PCR;
    volatile Uint32 RD_BASE;
    volatile Uint32 RD_OFST;
    volatile Uint32 FRAME_SIZE;
    volatile Uint32 INITXY;
    volatile Uint32 WR_BASE;
    volatile Uint32 WR_OFST;
    volatile Uint32 RD_BASE_420C;
    volatile Uint32 WR_BASE_420C;
    volatile Uint32 CONFIG;
    volatile Uint32 CENTER;
    volatile Uint32 KHV;
    volatile Uint32 BLOCK;
    volatile Uint32 LUT_ADDR;
    volatile Uint32 LUT_WDATA;
    volatile Uint32 LUT_RDATA;
    volatile Uint32 AB;
    volatile Uint32 CD;
    volatile Uint32 EF;
} CSL_LdcRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* PID */

#define CSL_LDC_PID_TID_MASK (0x00FF0000u)
#define CSL_LDC_PID_TID_SHIFT (0x00000010u)
#define CSL_LDC_PID_TID_RESETVAL (0x00000024u)

#define CSL_LDC_PID_CID_MASK (0x0000FF00u)
#define CSL_LDC_PID_CID_SHIFT (0x00000008u)
#define CSL_LDC_PID_CID_RESETVAL (0x000000FEu)

#define CSL_LDC_PID_PREV_MASK (0x000000FFu)
#define CSL_LDC_PID_PREV_SHIFT (0x00000000u)
#define CSL_LDC_PID_PREV_RESETVAL (0x00000005u)

#define CSL_LDC_PID_RESETVAL (0x0024FE05u)

/* PCR */

#define CSL_LDC_PCR_BMODE_MASK (0x00000060u)
#define CSL_LDC_PCR_BMODE_SHIFT (0x00000005u)
#define CSL_LDC_PCR_BMODE_RESETVAL (0x00000000u)
/*----BMODE Tokens----*/
#define CSL_LDC_PCR_BMODE_ALAW (0x00000003u)
#define CSL_LDC_PCR_BMODE_PKD8BIT (0x00000002u)
#define CSL_LDC_PCR_BMODE_PKD12BIT (0x00000001u)
#define CSL_LDC_PCR_BMODE_UPKD12BIT (0x00000000u)

#define CSL_LDC_PCR_MODE_MASK (0x00000018u)
#define CSL_LDC_PCR_MODE_SHIFT (0x00000003u)
#define CSL_LDC_PCR_MODE_RESETVAL (0x00000000u)
/*----MODE Tokens----*/
#define CSL_LDC_PCR_MODE_YCBCR420LD (0x00000002u)
#define CSL_LDC_PCR_MODE_BAYERCA (0x00000001u)
#define CSL_LDC_PCR_MODE_YCBCR422LD (0x00000000u)

#define CSL_LDC_PCR_BUSY_MASK (0x00000004u)
#define CSL_LDC_PCR_BUSY_SHIFT (0x00000002u)
#define CSL_LDC_PCR_BUSY_RESETVAL (0x00000000u)
/*----BUSY Tokens----*/
#define CSL_LDC_PCR_BUSY_IDLE (0x00000000u)
#define CSL_LDC_PCR_BUSY_BUSY (0x00000001u)

#define CSL_LDC_PCR_LDMAPEN_MASK (0x00000002u)
#define CSL_LDC_PCR_LDMAPEN_SHIFT (0x00000001u)
#define CSL_LDC_PCR_LDMAPEN_RESETVAL (0x00000000u)
/*----LDMAPEN Tokens----*/
#define CSL_LDC_PCR_LDMAPEN_ENABLE (0x00000001u)
#define CSL_LDC_PCR_LDMAPEN_DISABLE (0x00000000u)

#define CSL_LDC_PCR_EN_MASK (0x00000001u)
#define CSL_LDC_PCR_EN_SHIFT (0x00000000u)
#define CSL_LDC_PCR_EN_RESETVAL (0x00000000u)

#define CSL_LDC_PCR_RESETVAL (0x00000000u)

/* RD_BASE */

#define CSL_LDC_RD_BASE_RBASE_MASK (0xFFFFFFFFu)
#define CSL_LDC_RD_BASE_RBASE_SHIFT (0x00000000u)
#define CSL_LDC_RD_BASE_RBASE_RESETVAL (0x00000000u)

#define CSL_LDC_RD_BASE_RESETVAL (0x00000000u)

/* RD_OFST */

#define CSL_LDC_RD_OFST_ROFST_MASK (0x0000FFFFu)
#define CSL_LDC_RD_OFST_ROFST_SHIFT (0x00000000u)
#define CSL_LDC_RD_OFST_ROFST_RESETVAL (0x00000000u)

#define CSL_LDC_RD_OFST_RESETVAL (0x00000000u)

/* FRAME_SIZE */

#define CSL_LDC_FRAME_SIZE_H_MASK (0x3FFF0000u)
#define CSL_LDC_FRAME_SIZE_H_SHIFT (0x00000010u)
#define CSL_LDC_FRAME_SIZE_H_RESETVAL (0x00000000u)

#define CSL_LDC_FRAME_SIZE_W_MASK (0x00003FFFu)
#define CSL_LDC_FRAME_SIZE_W_SHIFT (0x00000000u)
#define CSL_LDC_FRAME_SIZE_W_RESETVAL (0x00000000u)

#define CSL_LDC_FRAME_SIZE_RESETVAL (0x00000000u)

/* INITXY */

#define CSL_LDC_INITXY_INITY_MASK (0x3FFF0000u)
#define CSL_LDC_INITXY_INITY_SHIFT (0x00000010u)
#define CSL_LDC_INITXY_INITY_RESETVAL (0x00000000u)

#define CSL_LDC_INITXY_INITX_MASK (0x00003FFFu)
#define CSL_LDC_INITXY_INITX_SHIFT (0x00000000u)
#define CSL_LDC_INITXY_INITX_RESETVAL (0x00000000u)

#define CSL_LDC_INITXY_RESETVAL (0x00000000u)

/* WR_BASE */

#define CSL_LDC_WR_BASE_WBASE_MASK (0xFFFFFFFFu)
#define CSL_LDC_WR_BASE_WBASE_SHIFT (0x00000000u)
#define CSL_LDC_WR_BASE_WBASE_RESETVAL (0x00000000u)

#define CSL_LDC_WR_BASE_RESETVAL (0x00000000u)

/* WR_OFST */

#define CSL_LDC_WR_OFST_WOFST_MASK (0x0000FFFFu)
#define CSL_LDC_WR_OFST_WOFST_SHIFT (0x00000000u)
#define CSL_LDC_WR_OFST_WOFST_RESETVAL (0x00000000u)

#define CSL_LDC_WR_OFST_RESETVAL (0x00000000u)

/* RD_BASE_420C */

#define CSL_LDC_RD_BASE_420C_RBASE_MASK (0xFFFFFFFFu)
#define CSL_LDC_RD_BASE_420C_RBASE_SHIFT (0x00000000u)
#define CSL_LDC_RD_BASE_420C_RBASE_RESETVAL (0x00000000u)

#define CSL_LDC_RD_BASE_420C_RESETVAL (0x00000000u)

/* WR_BASE_420C */

#define CSL_LDC_WR_BASE_420C_WBASE_MASK (0xFFFFFFFFu)
#define CSL_LDC_WR_BASE_420C_WBASE_SHIFT (0x00000000u)
#define CSL_LDC_WR_BASE_420C_WBASE_RESETVAL (0x00000000u)

#define CSL_LDC_WR_BASE_420C_RESETVAL (0x00000000u)

/* CONFIG */

#define CSL_LDC_CONFIG_RTH_MASK (0x3FFF0000u)
#define CSL_LDC_CONFIG_RTH_SHIFT (0x00000010u)
#define CSL_LDC_CONFIG_RTH_RESETVAL (0x00000000u)

#define CSL_LDC_CONFIG_CNST_MD_MASK (0x00000080u)
#define CSL_LDC_CONFIG_CNST_MD_SHIFT (0x00000007u)
#define CSL_LDC_CONFIG_CNST_MD_RESETVAL (0x00000000u)

#define CSL_LDC_CONFIG_YINT_TYP_MASK (0x00000040u)
#define CSL_LDC_CONFIG_YINT_TYP_SHIFT (0x00000006u)
#define CSL_LDC_CONFIG_YINT_TYP_RESETVAL (0x00000000u)
/*----YINT_TYP Tokens----*/
#define CSL_LDC_CONFIG_YINT_TYP_BICUBIC (0x00000000u)
#define CSL_LDC_CONFIG_YINT_TYP_BILINEAR (0x00000001u)

#define CSL_LDC_CONFIG_INTC_MASK (0x00000030u)
#define CSL_LDC_CONFIG_INTC_SHIFT (0x00000004u)
#define CSL_LDC_CONFIG_INTC_RESETVAL (0x00000000u)
/*----INTC Tokens----*/
#define CSL_LDC_CONFIG_INTC_R (0x00000000u)
#define CSL_LDC_CONFIG_INTC_GR (0x00000001u)
#define CSL_LDC_CONFIG_INTC_GB (0x00000002u)
#define CSL_LDC_CONFIG_INTC_B (0x00000003u)

#define CSL_LDC_CONFIG_T_MASK (0x0000000Fu)
#define CSL_LDC_CONFIG_T_SHIFT (0x00000000u)
#define CSL_LDC_CONFIG_T_RESETVAL (0x00000000u)

#define CSL_LDC_CONFIG_RESETVAL (0x00000000u)

/* CENTER */

#define CSL_LDC_CENTER_V0_MASK (0x3FFF0000u)
#define CSL_LDC_CENTER_V0_SHIFT (0x00000010u)
#define CSL_LDC_CENTER_V0_RESETVAL (0x00000000u)

#define CSL_LDC_CENTER_H0_MASK (0x00003FFFu)
#define CSL_LDC_CENTER_H0_SHIFT (0x00000000u)
#define CSL_LDC_CENTER_H0_RESETVAL (0x00000000u)

#define CSL_LDC_CENTER_RESETVAL (0x00000000u)

/* KHV */

#define CSL_LDC_KHV_KVL_MASK (0xFF000000u)
#define CSL_LDC_KHV_KVL_SHIFT (0x00000018u)
#define CSL_LDC_KHV_KVL_RESETVAL (0x00000000u)

#define CSL_LDC_KHV_KVU_MASK (0x00FF0000u)
#define CSL_LDC_KHV_KVU_SHIFT (0x00000010u)
#define CSL_LDC_KHV_KVU_RESETVAL (0x00000000u)

#define CSL_LDC_KHV_KHR_MASK (0x0000FF00u)
#define CSL_LDC_KHV_KHR_SHIFT (0x00000008u)
#define CSL_LDC_KHV_KHR_RESETVAL (0x00000000u)

#define CSL_LDC_KHV_KHL_MASK (0x000000FFu)
#define CSL_LDC_KHV_KHL_SHIFT (0x00000000u)
#define CSL_LDC_KHV_KHL_RESETVAL (0x00000000u)

#define CSL_LDC_KHV_RESETVAL (0x00000000u)

/* BLOCK */

#define CSL_LDC_BLOCK_PIXPAD_MASK (0x000F0000u)
#define CSL_LDC_BLOCK_PIXPAD_SHIFT (0x00000010u)
#define CSL_LDC_BLOCK_PIXPAD_RESETVAL (0x00000000u)

#define CSL_LDC_BLOCK_OBH_MASK (0x0000FF00u)
#define CSL_LDC_BLOCK_OBH_SHIFT (0x00000008u)
#define CSL_LDC_BLOCK_OBH_RESETVAL (0x00000000u)

#define CSL_LDC_BLOCK_OBW_MASK (0x000000FFu)
#define CSL_LDC_BLOCK_OBW_SHIFT (0x00000000u)
#define CSL_LDC_BLOCK_OBW_RESETVAL (0x00000000u)

#define CSL_LDC_BLOCK_RESETVAL (0x00000000u)

/* LUT_ADDR */

#define CSL_LDC_LUT_ADDR_ADDR_MASK (0x000000FFu)
#define CSL_LDC_LUT_ADDR_ADDR_SHIFT (0x00000000u)
#define CSL_LDC_LUT_ADDR_ADDR_RESETVAL (0x00000000u)

#define CSL_LDC_LUT_ADDR_RESETVAL (0x00000000u)

/* LUT_WDATA */

#define CSL_LDC_LUT_WDATA_WDATA_MASK (0x00003FFFu)
#define CSL_LDC_LUT_WDATA_WDATA_SHIFT (0x00000000u)
#define CSL_LDC_LUT_WDATA_WDATA_RESETVAL (0x00000000u)

#define CSL_LDC_LUT_WDATA_RESETVAL (0x00000000u)

/* LUT_RDATA */

#define CSL_LDC_LUT_RDATA_RDATA_MASK (0x00003FFFu)
#define CSL_LDC_LUT_RDATA_RDATA_SHIFT (0x00000000u)
#define CSL_LDC_LUT_RDATA_RDATA_RESETVAL (0x00000000u)

#define CSL_LDC_LUT_RDATA_RESETVAL (0x00000000u)

/* AB */

#define CSL_LDC_AB_B_MASK (0x3FFF0000u)
#define CSL_LDC_AB_B_SHIFT (0x00000010u)
#define CSL_LDC_AB_B_RESETVAL (0x00000000u)

#define CSL_LDC_AB_A_MASK (0x00003FFFu)
#define CSL_LDC_AB_A_SHIFT (0x00000000u)
#define CSL_LDC_AB_A_RESETVAL (0x00001000u)

#define CSL_LDC_AB_RESETVAL (0x00001000u)

/* CD */

#define CSL_LDC_CD_D_MASK (0x3FFF0000u)
#define CSL_LDC_CD_D_SHIFT (0x00000010u)
#define CSL_LDC_CD_D_RESETVAL (0x00000000u)

#define CSL_LDC_CD_C_MASK (0x0000FFFFu)
#define CSL_LDC_CD_C_SHIFT (0x00000000u)
#define CSL_LDC_CD_C_RESETVAL (0x00000000u)

#define CSL_LDC_CD_RESETVAL (0x00000000u)

/* EF */

#define CSL_LDC_EF_F_MASK (0xFFFF0000u)
#define CSL_LDC_EF_F_SHIFT (0x00000010u)
#define CSL_LDC_EF_F_RESETVAL (0x00000000u)

#define CSL_LDC_EF_E_MASK (0x00003FFFu)
#define CSL_LDC_EF_E_SHIFT (0x00000000u)
#define CSL_LDC_EF_E_RESETVAL (0x00001000u)

#define CSL_LDC_EF_RESETVAL (0x00001000u)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
