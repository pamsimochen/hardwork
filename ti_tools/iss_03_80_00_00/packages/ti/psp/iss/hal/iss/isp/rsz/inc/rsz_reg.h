/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file rsz_reg.h
* This file contains the CSL register definition for the module
* 
*
* @path Centaurus\drivers\drv_isp\in\csl
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! 
*========================================================================= */
#ifndef RSZ_REG_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define RSZ_REG_H

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "../../isp5_utils/isp5_sys_types.h"

/* ================================================================ */
/* 
 *================================================================== */

    /* From here it is similar btween RSZA and RSZB */
typedef struct {
    volatile uint32 RZB_EN;
    volatile uint32 RZB_MODE;

    volatile uint32 RZB_420;                               /* YUV420 specific */

    /* Size params */
    volatile uint32 RZB_I_VPS;                             /* Resizer input
                                                            * starts after
                                                            * these many
                                                            * lines */
    volatile uint32 RZB_I_HPS;                             /* Resizer input
                                                            * starts after
                                                            * these many
                                                            * pixels */
    volatile uint32 RZB_O_VSZ;                             /* the vertical
                                                            * and horizontal
                                                            * sizes of the
                                                            * output frames
                                                            * after the
                                                            * resizing */
    volatile uint32 RZB_O_HSZ;

    volatile uint32 RZB_V_PHS_Y;                           /* YUV422 */
    volatile uint32 RZB_V_PHS_C;                           /* YUV422 */
    volatile uint32 RZB_V_DIF;                             /* The phase diff
                                                            * parameter for
                                                            * vertical
                                                            * resize. */
    volatile uint32 RZB_V_TYP;                             /* YUV422, cubic
                                                            * or bilinear
                                                            * interpolation */
    volatile uint32 RZB_V_LPF;                             /* YUV422 */

    volatile uint32 RZB_H_PHS;                             /* YUV422 */
    volatile uint32 RZB_H_PHS_ADJ;                         /* YUV422 */
    volatile uint32 RZB_H_DIF;                             /* The parameter
                                                            * for horizontal
                                                            * resizing
                                                            * process. */
    volatile uint32 RZB_H_TYP;                             /* YUV422,cubic or 
                                                            * bilinear
                                                            * interpolation */
    volatile uint32 RZB_H_LPF;                             /* YUV422 */

    volatile uint32 RZB_DWN_EN;                            /* Resizer
                                                            * downscale
                                                            * enable, if
                                                            * enabled only
                                                            * downscale is
                                                            * allowed */
    volatile uint32 RZB_DWN_AV;                            /* Vertical and
                                                            * Horizontal
                                                            * averaging
                                                            * ratios */

    /* RGB output related related */
    volatile uint32 RZB_RGB_EN;
    volatile uint32 RZB_RGB_TYP;
    volatile uint32 RZB_RGB_BLD;

    /* These registers give the address where the data is to be written
     * irrespective of the format of the data and is common for all formats */
    volatile uint32 RZB_SDR_Y_BAD_H;
    volatile uint32 RZB_SDR_Y_BAD_L;
    volatile uint32 RZB_SDR_Y_SAD_H;
    volatile uint32 RZB_SDR_Y_SAD_L;
    volatile uint32 RZB_SDR_Y_OFT;
    volatile uint32 RZB_SDR_Y_PTR_S;
    volatile uint32 RZB_SDR_Y_PTR_E;
    /* From here on it is YUV 420 specific configurations */
    volatile uint32 RZB_SDR_C_BAD_H;
    volatile uint32 RZB_SDR_C_BAD_L;
    volatile uint32 RZB_SDR_C_SAD_H;
    volatile uint32 RZB_SDR_C_SAD_L;
    volatile uint32 RZB_SDR_C_OFT;
    volatile uint32 RZB_SDR_C_PTR_S;
    volatile uint32 RZB_SDR_C_PTR_E;
} Rsz_B_Regs;

/* ================================================================ */
/* 
 *================================================================== */

typedef struct {

    volatile uint32 RZA_EN;
    volatile uint32 RZA_MODE;
    volatile uint32 RZA_420;
    volatile uint32 RZA_I_VPS;
    volatile uint32 RZA_I_HPS;
    volatile uint32 RZA_O_VSZ;
    volatile uint32 RZA_O_HSZ;
    volatile uint32 RZA_V_PHS_Y;
    volatile uint32 RZA_V_PHS_C;
    volatile uint32 RZA_V_DIF;
    volatile uint32 RZA_V_TYP;
    volatile uint32 RZA_V_LPF;
    volatile uint32 RZA_H_PHS;
    volatile uint32 RZA_H_PHS_ADJ;
    volatile uint32 RZA_H_DIF;
    volatile uint32 RZA_H_TYP;
    volatile uint32 RZA_H_LPF;
    volatile uint32 RZA_DWN_EN;
    volatile uint32 RZA_DWN_AV;
    volatile uint32 RZA_RGB_EN;
    volatile uint32 RZA_RGB_TYP;
    volatile uint32 RZA_RGB_BLD;
    volatile uint32 RZA_SDR_Y_BAD_H;
    volatile uint32 RZA_SDR_Y_BAD_L;
    volatile uint32 RZA_SDR_Y_SAD_H;
    volatile uint32 RZA_SDR_Y_SAD_L;
    volatile uint32 RZA_SDR_Y_OFT;
    volatile uint32 RZA_SDR_Y_PTR_S;
    volatile uint32 RZA_SDR_Y_PTR_E;
    volatile uint32 RZA_SDR_C_BAD_H;
    volatile uint32 RZA_SDR_C_BAD_L;
    volatile uint32 RZA_SDR_C_SAD_H;
    volatile uint32 RZA_SDR_C_SAD_L;
    volatile uint32 RZA_SDR_C_OFT;
    volatile uint32 RZA_SDR_C_PTR_S;
    volatile uint32 RZA_SDR_C_PTR_E;

} Rsz_A_Regs;

/* ================================================================ */
/* 
 *================================================================== */
typedef struct {

    /* Newly added */
    volatile uint32 REVISION;
    volatile uint32 SYSCONFIG;
    volatile uint32 SYSSTATUS;
    volatile uint32 IN_FIFO_CTRL;
    volatile uint32 GNC;
    volatile uint32 FRACDIV;
    volatile uint32 rsrvd0[2];

    volatile uint32 SRC_EN;
    volatile uint32 SRC_MODE;

    volatile uint32 SRC_FMT0;
    volatile uint32 SRC_FMT1;

    /* Dimensions of cropped image */
    volatile uint32 SRC_VPS;                               /* Global frame
                                                            * start */
    volatile uint32 SRC_VSZ;                               /* Global frame
                                                            * start */
    volatile uint32 SRC_HPS;                               /* Crop Horizontal 
                                                            * start */
    volatile uint32 SRC_HSZ;                               /* Crop vertical
                                                            * start */

    volatile uint32 DMA_RZA;
    volatile uint32 DMA_RZB;
    volatile uint32 DMA_STA;
    volatile uint32 GCK_MMR;
    volatile uint32 RESERVED0;
    volatile uint32 GCK_SDR;

    volatile uint32 IRQ_RZA;                               // Interrupt is
                                                           // generated after 
                                                           // so many lines
                                                           // are output
    volatile uint32 IRQ_RZB;

    /* YUV params */
    volatile uint32 YUV_Y_MIN;
    volatile uint32 YUV_Y_MAX;
    volatile uint32 YUV_C_MIN;
    volatile uint32 YUV_C_MAX;
    volatile uint32 YUV_PHS;
    volatile uint32 SEQ;                                   // ###

    Rsz_A_Regs RSZ_A_REG;
    Rsz_B_Regs RSZ_B_REG;
} Rsz_Regs;

/* ================================================================ */
/* 
 *================================================================== */

#define CSL_RSZ_SYSCONFIG_RSZB_CLK_EN_SHIFT (0x00000009u)
#define CSL_RSZ_SYSCONFIG_RSZB_CLK_EN_MASK (0x00000200u)

/* TO####################check this again */
#define CSL_RSZ_IN_FIFO_CTRL_THRLD_LOW_SHIFT (0x00000010u)
#define CSL_RSZ_IN_FIFO_CTRL_THRLD_LOW_MASK (0x1FFF0000u)

#define CSL_RSZ_IN_FIFO_CTRL_THRLD_HIGH_SHIFT (0x00000000u)
#define CSL_RSZ_IN_FIFO_CTRL_THRLD_HIGH_MASK (0x00001FFFu)
/**************************************/

#define CSL_RSZ_SYSCONFIG_RSZA_CLK_EN_SHIFT (0x00000008u)
#define CSL_RSZ_SYSCONFIG_RSZA_CLK_EN_MASK (0x00000100u)

#define CSL_RSZ_SYSCONFIG_SOFTRESET_SHIFT (0x00000001u)
#define CSL_RSZ_SYSCONFIG_SOFTRESET_MASK (0x00000002u)

#define CSL_RSZ_SYSCONFIG_AUTOGATING_SHIFT (0x00000000u)
#define CSL_RSZ_SYSCONFIG_AUTOGATING_MASK (0x00000001u)

#define CSL_RSZ_SRC_EN_EN_MASK (0x00000001u)
#define CSL_RSZ_SRC_EN_EN_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_EN_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_RSZ_SRC_EN_EN_DISABLE (0x00000000u)
#define CSL_RSZ_SRC_EN_EN_ENABLE (0x00000001u)

#define CSL_RSZ_SRC_EN_RESETVAL (0x00000000u)

/* SRC_MODE */

#define CSL_RSZ_SRC_MODE_WRT_MASK (0x00000002u)
#define CSL_RSZ_SRC_MODE_WRT_SHIFT (0x00000001u)
#define CSL_RSZ_SRC_MODE_WRT_RESETVAL (0x00000000u)
/*----WRT Tokens----*/
#define CSL_RSZ_SRC_MODE_WRT_DISABLE (0x00000000u)
#define CSL_RSZ_SRC_MODE_WRT_ENABLE (0x00000001u)

#define CSL_RSZ_SRC_MODE_OST_MASK (0x00000001u)
#define CSL_RSZ_SRC_MODE_OST_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_MODE_OST_RESETVAL (0x00000000u)
/*----OST Tokens----*/
#define CSL_RSZ_SRC_MODE_OST_DISABLE (0x00000000u)
#define CSL_RSZ_SRC_MODE_OST_ENABLE (0x00000001u)

#define CSL_RSZ_SRC_MODE_RESETVAL (0x00000000u)

/* SRC_FMT0 */

#define CSL_RSZ_SRC_FMT0_BYPASS_MASK (0x00000002u)
#define CSL_RSZ_SRC_FMT0_BYPASS_SHIFT (0x00000001u)
#define CSL_RSZ_SRC_FMT0_BYPASS_RESETVAL (0x00000000u)
/*----BYPASS Tokens----*/
#define CSL_RSZ_SRC_FMT0_BYPASS_OFF (0x00000000u)
#define CSL_RSZ_SRC_FMT0_BYPASS_ON (0x00000001u)

#define CSL_RSZ_SRC_FMT0_SEL_MASK (0x00000001u)
#define CSL_RSZ_SRC_FMT0_SEL_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_FMT0_SEL_RESETVAL (0x00000000u)
/*----SRC Tokens----*/
#define CSL_RSZ_SRC_FMT0_SEL_IPIPE (0x00000000u)
#define CSL_RSZ_SRC_FMT0_SEL_IPIPEIF (0x00000001u)

#define CSL_RSZ_SRC_FMT0_RESETVAL (0x00000000u)

/* SRC_FMT1 */

#define CSL_RSZ_SRC_FMT1_COL_MASK (0x00000004u)
#define CSL_RSZ_SRC_FMT1_COL_SHIFT (0x00000002u)
#define CSL_RSZ_SRC_FMT1_COL_RESETVAL (0x00000000u)
/*----COL Tokens----*/
#define CSL_RSZ_SRC_FMT1_COL_Y (0x00000000u)
#define CSL_RSZ_SRC_FMT1_COL_C (0x00000001u)

#define CSL_RSZ_SRC_FMT1_420_MASK (0x00000002u)
#define CSL_RSZ_SRC_FMT1_420_SHIFT (0x00000001u)
#define CSL_RSZ_SRC_FMT1_420_RESETVAL (0x00000000u)
/*----420 Tokens----*/
#define CSL_RSZ_SRC_FMT1_420__422 (0x00000000u)
#define CSL_RSZ_SRC_FMT1_420__420 (0x00000001u)

#define CSL_RSZ_SRC_FMT1_RAW_MASK (0x00000001u)
#define CSL_RSZ_SRC_FMT1_RAW_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_FMT1_RAW_RESETVAL (0x00000000u)
/*----RAW Tokens----*/
#define CSL_RSZ_SRC_FMT1_RAW__YCC (0x00000000u)
#define CSL_RSZ_SRC_FMT1_RAW__RAW (0x00000001u)

#define CSL_RSZ_SRC_FMT1_RESETVAL (0x00000000u)

/* SRC_VPS */

#define CSL_RSZ_SRC_VPS_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_SRC_VPS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_VPS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_SRC_VPS_RESETVAL (0x00000000u)

/* SRC_VSZ */

#define CSL_RSZ_SRC_VSZ_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_SRC_VSZ_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_VSZ_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_SRC_VSZ_RESETVAL (0x00000000u)

/* SRC_HPS */

#define CSL_RSZ_SRC_HPS_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_SRC_HPS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_HPS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_SRC_HPS_RESETVAL (0x00000000u)

/* SRC_HSZ */

#define CSL_RSZ_SRC_HSZ_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_SRC_HSZ_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_SRC_HSZ_VAL_RESETVAL (0x00000001u)

#define CSL_RSZ_SRC_HSZ_RESETVAL (0x00000001u)

/* DMA_RZA */

#define CSL_RSZ_DMA_RZA_VAL_MASK (0x000000FFu)
#define CSL_RSZ_DMA_RZA_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_DMA_RZA_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_DMA_RZA_RESETVAL (0x00000000u)

/* DMA_RZB */

#define CSL_RSZ_DMA_RZB_VAL_MASK (0x000000FFu)
#define CSL_RSZ_DMA_RZB_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_DMA_RZB_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_DMA_RZB_RESETVAL (0x00000000u)

/* DMA_STA */

#define CSL_RSZ_DMA_STA_STATUS_MASK (0x00000001u)
#define CSL_RSZ_DMA_STA_STATUS_SHIFT (0x00000000u)
#define CSL_RSZ_DMA_STA_STATUS_RESETVAL (0x00000000u)

#define CSL_RSZ_DMA_STA_RESETVAL (0x00000000u)

/* GCK_MMR */

#define CSL_RSZ_GCK_MMR_REG_MASK (0x00000001u)
#define CSL_RSZ_GCK_MMR_REG_SHIFT (0x00000000u)
#define CSL_RSZ_GCK_MMR_REG_RESETVAL (0x00000000u)
/*----REG Tokens----*/
#define CSL_RSZ_GCK_MMR_REG_OFF (0x00000000u)
#define CSL_RSZ_GCK_MMR_REG_ON (0x00000001u)

#define CSL_RSZ_GCK_MMR_RESETVAL (0x00000000u)

/* Reserved0 */

#define CSL_RSZ_RESERVED0_RESETVAL (0x00000000u)

/* GCK_SDR */

#define CSL_RSZ_GCK_SDR_CORE_MASK (0x00000001u)
#define CSL_RSZ_GCK_SDR_CORE_SHIFT (0x00000000u)
#define CSL_RSZ_GCK_SDR_CORE_RESETVAL (0x00000000u)
/*----CORE Tokens----*/
#define CSL_RSZ_GCK_SDR_CORE_OFF (0x00000000u)
#define CSL_RSZ_GCK_SDR_CORE_ON (0x00000001u)

#define CSL_RSZ_GCK_SDR_RESETVAL (0x00000000u)

/* IRQ_RZA */

#define CSL_RSZ_IRQ_RZA_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_IRQ_RZA_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_IRQ_RZA_VAL_RESETVAL (0x00001FFFu)

#define CSL_RSZ_IRQ_RZA_RESETVAL (0x00001FFFu)

/* IRQ_RZB */

#define CSL_RSZ_IRQ_RZB_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_IRQ_RZB_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_IRQ_RZB_VAL_RESETVAL (0x00001FFFu)

#define CSL_RSZ_IRQ_RZB_RESETVAL (0x00001FFFu)

/* YUV_Y_MIN */

#define CSL_RSZ_YUV_Y_MIN_VAL_MASK (0x000000FFu)
#define CSL_RSZ_YUV_Y_MIN_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_YUV_Y_MIN_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_YUV_Y_MIN_RESETVAL (0x00000000u)

/* YUV_Y_MAX */

#define CSL_RSZ_YUV_Y_MAX_VAL_MASK (0x000000FFu)
#define CSL_RSZ_YUV_Y_MAX_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_YUV_Y_MAX_VAL_RESETVAL (0x000000FFu)

#define CSL_RSZ_YUV_Y_MAX_RESETVAL (0x000000FFu)

/* YUV_C_MIN */

#define CSL_RSZ_YUV_C_MIN_VAL_MASK (0x000000FFu)
#define CSL_RSZ_YUV_C_MIN_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_YUV_C_MIN_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_YUV_C_MIN_RESETVAL (0x00000000u)

/* YUV_C_MAX */

#define CSL_RSZ_YUV_C_MAX_VAL_MASK (0x000000FFu)
#define CSL_RSZ_YUV_C_MAX_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_YUV_C_MAX_VAL_RESETVAL (0x000000FFu)

#define CSL_RSZ_YUV_C_MAX_RESETVAL (0x000000FFu)

/* YUV_PHS */

#define CSL_RSZ_YUV_PHS_POS_MASK (0x00000001u)
#define CSL_RSZ_YUV_PHS_POS_SHIFT (0x00000000u)
#define CSL_RSZ_YUV_PHS_POS_RESETVAL (0x00000000u)
/*----POS Tokens----*/
#define CSL_RSZ_YUV_PHS_POS_COSITING (0x00000000u)
#define CSL_RSZ_YUV_PHS_POS_CENTERING (0x00000001u)

#define CSL_RSZ_YUV_PHS_RESETVAL (0x00000000u)

/* SEQ */

#define CSL_RSZ_SEQ_CRV_MASK (0x00000010u)
#define CSL_RSZ_SEQ_CRV_SHIFT (0x00000004u)
#define CSL_RSZ_SEQ_CRV_RESETVAL (0x00000000u)
/*----CRV Tokens----*/
#define CSL_RSZ_SEQ_CRV_NOFLIP (0x00000000u)
#define CSL_RSZ_SEQ_CRV_FLIP (0x00000001u)

#define CSL_RSZ_SEQ_VRVB_MASK (0x00000008u)
#define CSL_RSZ_SEQ_VRVB_SHIFT (0x00000003u)
#define CSL_RSZ_SEQ_VRVB_RESETVAL (0x00000000u)
/*----VRVB Tokens----*/
#define CSL_RSZ_SEQ_VRVB_NOFLIP (0x00000000u)
#define CSL_RSZ_SEQ_VRVB_FLIP (0x00000001u)

#define CSL_RSZ_SEQ_HRVB_MASK (0x00000004u)
#define CSL_RSZ_SEQ_HRVB_SHIFT (0x00000002u)
#define CSL_RSZ_SEQ_HRVB_RESETVAL (0x00000000u)
/*----HRVB Tokens----*/
#define CSL_RSZ_SEQ_HRVB_NOFLIP (0x00000000u)
#define CSL_RSZ_SEQ_HRVB_FLIP (0x00000001u)

#define CSL_RSZ_SEQ_VRVA_MASK (0x00000002u)
#define CSL_RSZ_SEQ_VRVA_SHIFT (0x00000001u)
#define CSL_RSZ_SEQ_VRVA_RESETVAL (0x00000000u)
/*----VRVA Tokens----*/
#define CSL_RSZ_SEQ_VRVA_NOFLIP (0x00000000u)
#define CSL_RSZ_SEQ_VRVA_FLIP (0x00000001u)

#define CSL_RSZ_SEQ_HRVA_MASK (0x00000001u)
#define CSL_RSZ_SEQ_HRVA_SHIFT (0x00000000u)
#define CSL_RSZ_SEQ_HRVA_RESETVAL (0x00000000u)
/*----HRVA Tokens----*/
#define CSL_RSZ_SEQ_HRVA_NOFLIP (0x00000000u)
#define CSL_RSZ_SEQ_HRVA_FLIP (0x00000001u)

#define CSL_RSZ_SEQ_RESETVAL (0x00000000u)

/* RZA_EN */

#define CSL_RSZ_RZA_EN_EN_MASK (0x00000001u)
#define CSL_RSZ_RZA_EN_EN_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_EN_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_RSZ_RZA_EN_EN_DISABLE (0x00000000u)
#define CSL_RSZ_RZA_EN_EN_ENABLE (0x00000001u)

#define CSL_RSZ_RZA_EN_RESETVAL (0x00000000u)

/* RZA_MODE */

#define CSL_RSZ_RZA_MODE_OST_MASK (0x00000001u)
#define CSL_RSZ_RZA_MODE_OST_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_MODE_OST_RESETVAL (0x00000000u)
/*----OST Tokens----*/
#define CSL_RSZ_RZA_MODE_OST_CONTINUOUS (0x00000000u)
#define CSL_RSZ_RZA_MODE_OST_ONESHOT (0x00000001u)

#define CSL_RSZ_RZA_MODE_RESETVAL (0x00000000u)

/* RZA_420 */

#define CSL_RSZ_RZA_420_CEN_MASK (0x00000002u)
#define CSL_RSZ_RZA_420_CEN_SHIFT (0x00000001u)
#define CSL_RSZ_RZA_420_CEN_RESETVAL (0x00000000u)
/*----CEN Tokens----*/
#define CSL_RSZ_RZA_420_CEN_DISABLE (0x00000000u)
#define CSL_RSZ_RZA_420_CEN_ENABLE (0x00000001u)

#define CSL_RSZ_RZA_420_YEN_MASK (0x00000001u)
#define CSL_RSZ_RZA_420_YEN_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_420_YEN_RESETVAL (0x00000000u)
/*----YEN Tokens----*/
#define CSL_RSZ_RZA_420_YEN_DISABLE (0x00000000u)
#define CSL_RSZ_RZA_420_YEN_ENABLE (0x00000001u)

#define CSL_RSZ_RZA_420_RESETVAL (0x00000000u)

/* RZA_I_VPS */

#define CSL_RSZ_RZA_I_VPS_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZA_I_VPS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_I_VPS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_I_VPS_RESETVAL (0x00000000u)

/* RZA_I_HPS */

#define CSL_RSZ_RZA_I_HPS_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZA_I_HPS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_I_HPS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_I_HPS_RESETVAL (0x00000000u)

/* RZA_O_VSZ */

#define CSL_RSZ_RZA_O_VSZ_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZA_O_VSZ_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_O_VSZ_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_O_VSZ_RESETVAL (0x00000000u)

/* RZA_O_HSZ */

#define CSL_RSZ_RZA_O_HSZ_VAL_MASK (0x00001FFEu)
#define CSL_RSZ_RZA_O_HSZ_VAL_SHIFT (0x00000001u)
#define CSL_RSZ_RZA_O_HSZ_VAL_RESETVAL (0x00000001u)

#define CSL_RSZ_RZA_O_HSZ_RESETVAL (0x00000001u)

/* RZA_V_PHS_Y */

#define CSL_RSZ_RZA_V_PHS_Y_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZA_V_PHS_Y_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_V_PHS_Y_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_V_PHS_Y_RESETVAL (0x00000000u)

/* RZA_V_PHS_C */

#define CSL_RSZ_RZA_V_PHS_C_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZA_V_PHS_C_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_V_PHS_C_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_V_PHS_C_RESETVAL (0x00000000u)

/* RZA_V_DIF */

#define CSL_RSZ_RZA_V_DIF_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZA_V_DIF_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_V_DIF_VAL_RESETVAL (0x00000100u)

#define CSL_RSZ_RZA_V_DIF_RESETVAL (0x00000100u)

/* RZA_V_TYP */

#define CSL_RSZ_RZA_V_TYP_C_MASK (0x00000002u)
#define CSL_RSZ_RZA_V_TYP_C_SHIFT (0x00000001u)
#define CSL_RSZ_RZA_V_TYP_C_RESETVAL (0x00000000u)
/*----C Tokens----*/
#define CSL_RSZ_RZA_V_TYP_C_CUBIC (0x00000000u)
#define CSL_RSZ_RZA_V_TYP_C_LINEAR (0x00000001u)

#define CSL_RSZ_RZA_V_TYP_Y_MASK (0x00000001u)
#define CSL_RSZ_RZA_V_TYP_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_V_TYP_Y_RESETVAL (0x00000000u)
/*----Y Tokens----*/
#define CSL_RSZ_RZA_V_TYP_Y_CUBIC (0x00000000u)
#define CSL_RSZ_RZA_V_TYP_Y_LINEAR (0x00000001u)

#define CSL_RSZ_RZA_V_TYP_RESETVAL (0x00000000u)

/* RZA_V_LPF */

#define CSL_RSZ_RZA_V_LPF_C_MASK (0x00000FC0u)
#define CSL_RSZ_RZA_V_LPF_C_SHIFT (0x00000006u)
#define CSL_RSZ_RZA_V_LPF_C_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_V_LPF_Y_MASK (0x0000003Fu)
#define CSL_RSZ_RZA_V_LPF_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_V_LPF_Y_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_V_LPF_RESETVAL (0x00000000u)

/* RZA_H_PHS */

#define CSL_RSZ_RZA_H_PHS_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZA_H_PHS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_H_PHS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_H_PHS_RESETVAL (0x00000000u)

/* RZA_H_PHS_ADJ */

#define CSL_RSZ_RZA_H_PHS_ADJ_VAL_MASK (0x000001FFu)
#define CSL_RSZ_RZA_H_PHS_ADJ_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_H_PHS_ADJ_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_H_PHS_ADJ_RESETVAL (0x00000000u)

/* RZA_H_DIF */

#define CSL_RSZ_RZA_H_DIF_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZA_H_DIF_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_H_DIF_VAL_RESETVAL (0x00000100u)

#define CSL_RSZ_RZA_H_DIF_RESETVAL (0x00000100u)

/* RZA_H_TYP */

#define CSL_RSZ_RZA_H_TYP_C_MASK (0x00000002u)
#define CSL_RSZ_RZA_H_TYP_C_SHIFT (0x00000001u)
#define CSL_RSZ_RZA_H_TYP_C_RESETVAL (0x00000000u)
/*----C Tokens----*/
#define CSL_RSZ_RZA_H_TYP_C_CUBIC (0x00000000u)
#define CSL_RSZ_RZA_H_TYP_C_LINEAR (0x00000001u)

#define CSL_RSZ_RZA_H_TYP_Y_MASK (0x00000001u)
#define CSL_RSZ_RZA_H_TYP_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_H_TYP_Y_RESETVAL (0x00000000u)
/*----Y Tokens----*/
#define CSL_RSZ_RZA_H_TYP_Y_CUBIC (0x00000000u)
#define CSL_RSZ_RZA_H_TYP_Y_LINEAR (0x00000001u)

#define CSL_RSZ_RZA_H_TYP_RESETVAL (0x00000000u)

/* RZA_H_LPF */

#define CSL_RSZ_RZA_H_LPF_C_MASK (0x00000FC0u)
#define CSL_RSZ_RZA_H_LPF_C_SHIFT (0x00000006u)
#define CSL_RSZ_RZA_H_LPF_C_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_H_LPF_Y_MASK (0x0000003Fu)
#define CSL_RSZ_RZA_H_LPF_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_H_LPF_Y_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_H_LPF_RESETVAL (0x00000000u)

/* RZA_DWN_EN */

#define CSL_RSZ_RZA_DWN_EN_EN_MASK (0x00000001u)
#define CSL_RSZ_RZA_DWN_EN_EN_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_DWN_EN_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_RSZ_RZA_DWN_EN_EN_DISABLE (0x00000000u)
#define CSL_RSZ_RZA_DWN_EN_EN_ENABLE (0x00000001u)

#define CSL_RSZ_RZA_DWN_EN_RESETVAL (0x00000000u)

/* RZA_DWN_AV */

#define CSL_RSZ_RZA_DWN_AV_V_MASK (0x00000038u)
#define CSL_RSZ_RZA_DWN_AV_V_SHIFT (0x00000003u)
#define CSL_RSZ_RZA_DWN_AV_V_RESETVAL (0x00000000u)
/*----V Tokens----*/
#define CSL_RSZ_RZA_DWN_AV_V__DIV2 (0x00000000u)
#define CSL_RSZ_RZA_DWN_AV_V__DIV4 (0x00000001u)
#define CSL_RSZ_RZA_DWN_AV_V__DIV8 (0x00000002u)
#define CSL_RSZ_RZA_DWN_AV_V__DIV16 (0x00000003u)
#define CSL_RSZ_RZA_DWN_AV_V__DIV32 (0x00000004u)
#define CSL_RSZ_RZA_DWN_AV_V__DIV64 (0x00000005u)
#define CSL_RSZ_RZA_DWN_AV_V__DIV128 (0x00000006u)
#define CSL_RSZ_RZA_DWN_AV_V__DIV256 (0x00000007u)

#define CSL_RSZ_RZA_DWN_AV_H_MASK (0x00000007u)
#define CSL_RSZ_RZA_DWN_AV_H_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_DWN_AV_H_RESETVAL (0x00000000u)
/*----H Tokens----*/
#define CSL_RSZ_RZA_DWN_AV_H__DIV2 (0x00000000u)
#define CSL_RSZ_RZA_DWN_AV_H__DIV4 (0x00000001u)
#define CSL_RSZ_RZA_DWN_AV_H__DIV8 (0x00000002u)
#define CSL_RSZ_RZA_DWN_AV_H__DIV16 (0x00000003u)
#define CSL_RSZ_RZA_DWN_AV_H__DIV32 (0x00000004u)
#define CSL_RSZ_RZA_DWN_AV_H__DIV64 (0x00000005u)
#define CSL_RSZ_RZA_DWN_AV_H__DIV128 (0x00000006u)
#define CSL_RSZ_RZA_DWN_AV_H__DIV256 (0x00000007u)

#define CSL_RSZ_RZA_DWN_AV_RESETVAL (0x00000000u)

/* RZA_RGB_EN */

#define CSL_RSZ_RZA_RGB_EN_EN_MASK (0x00000001u)
#define CSL_RSZ_RZA_RGB_EN_EN_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_RGB_EN_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_RSZ_RZA_RGB_EN_EN_DISABLE (0x00000000u)
#define CSL_RSZ_RZA_RGB_EN_EN_ENABLE (0x00000001u)

#define CSL_RSZ_RZA_RGB_EN_RESETVAL (0x00000000u)

/* RZA_RGB_TYP */

#define CSL_RSZ_RZA_RGB_TYP_MSK1_MASK (0x00000004u)
#define CSL_RSZ_RZA_RGB_TYP_MSK1_SHIFT (0x00000002u)
#define CSL_RSZ_RZA_RGB_TYP_MSK1_RESETVAL (0x00000000u)
/*----MSK1 Tokens----*/
#define CSL_RSZ_RZA_RGB_TYP_MSK1_NOMASK (0x00000000u)
#define CSL_RSZ_RZA_RGB_TYP_MSK1_MASKLAST2 (0x00000001u)

#define CSL_RSZ_RZA_RGB_TYP_MSK0_MASK (0x00000002u)
#define CSL_RSZ_RZA_RGB_TYP_MSK0_SHIFT (0x00000001u)
#define CSL_RSZ_RZA_RGB_TYP_MSK0_RESETVAL (0x00000000u)
/*----MSK0 Tokens----*/
#define CSL_RSZ_RZA_RGB_TYP_MSK0_NOMASK (0x00000000u)
#define CSL_RSZ_RZA_RGB_TYP_MSK0_MASKFIRST2 (0x00000001u)

#define CSL_RSZ_RZA_RGB_TYP_TYP_MASK (0x00000001u)
#define CSL_RSZ_RZA_RGB_TYP_TYP_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_RGB_TYP_TYP_RESETVAL (0x00000000u)
/*----TYP Tokens----*/
#define CSL_RSZ_RZA_RGB_TYP_TYP_OUTPUT32BITS (0x00000000u)
#define CSL_RSZ_RZA_RGB_TYP_TYP_OUTPUT16BITS (0x00000001u)

#define CSL_RSZ_RZA_RGB_TYP_RESETVAL (0x00000000u)

/* RZA_RGB_BLD */

#define CSL_RSZ_RZA_RGB_BLD_VAL_MASK (0x000000FFu)
#define CSL_RSZ_RZA_RGB_BLD_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_RGB_BLD_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_RGB_BLD_RESETVAL (0x00000000u)

/* RZA_SDR_Y_BAD_H */

#define CSL_RSZ_RZA_SDR_Y_BAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_Y_BAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_Y_BAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_Y_BAD_H_RESETVAL (0x00000000u)

/* RZA_SDR_Y_BAD_L */

#define CSL_RSZ_RZA_SDR_Y_BAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_Y_BAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_Y_BAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_Y_BAD_L_RESETVAL (0x00000000u)

/* RZA_SDR_Y_SAD_H */

#define CSL_RSZ_RZA_SDR_Y_SAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_Y_SAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_Y_SAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_Y_SAD_H_RESETVAL (0x00000000u)

/* RZA_SDR_Y_SAD_L */

#define CSL_RSZ_RZA_SDR_Y_SAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_Y_SAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_Y_SAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_Y_SAD_L_RESETVAL (0x00000000u)

/* RZA_SDR_Y_OFT */

#define CSL_RSZ_RZA_SDR_Y_OFT_OFT_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_Y_OFT_OFT_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_Y_OFT_OFT_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_Y_OFT_RESETVAL (0x00000000u)

/* RZA_SDR_Y_PTR_S */

#define CSL_RSZ_RZA_SDR_Y_PTR_S_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZA_SDR_Y_PTR_S_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_Y_PTR_S_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_Y_PTR_S_RESETVAL (0x00000000u)

/* RZA_SDR_Y_PTR_E */

#define CSL_RSZ_RZA_SDR_Y_PTR_E_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZA_SDR_Y_PTR_E_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_Y_PTR_E_VAL_RESETVAL (0x00001FFFu)

#define CSL_RSZ_RZA_SDR_Y_PTR_E_RESETVAL (0x00001FFFu)

/* RZA_SDR_C_BAD_H */

#define CSL_RSZ_RZA_SDR_C_BAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_C_BAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_C_BAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_C_BAD_H_RESETVAL (0x00000000u)

/* RZA_SDR_C_BAD_L */

#define CSL_RSZ_RZA_SDR_C_BAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_C_BAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_C_BAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_C_BAD_L_RESETVAL (0x00000000u)

/* RZA_SDR_C_SAD_H */

#define CSL_RSZ_RZA_SDR_C_SAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_C_SAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_C_SAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_C_SAD_H_RESETVAL (0x00000000u)

/* RZA_SDR_C_SAD_L */

#define CSL_RSZ_RZA_SDR_C_SAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_C_SAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_C_SAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_C_SAD_L_RESETVAL (0x00000000u)

/* RZA_SDR_C_OFT */

#define CSL_RSZ_RZA_SDR_C_OFT_OFT_MASK (0x0000FFFFu)
#define CSL_RSZ_RZA_SDR_C_OFT_OFT_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_C_OFT_OFT_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_C_OFT_RESETVAL (0x00000000u)

/* RZA_SDR_C_PTR_S */

#define CSL_RSZ_RZA_SDR_C_PTR_S_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZA_SDR_C_PTR_S_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_C_PTR_S_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZA_SDR_C_PTR_S_RESETVAL (0x00000000u)

/* RZA_SDR_C_PTR_E */

#define CSL_RSZ_RZA_SDR_C_PTR_E_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZA_SDR_C_PTR_E_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZA_SDR_C_PTR_E_VAL_RESETVAL (0x00001FFFu)

#define CSL_RSZ_RZA_SDR_C_PTR_E_RESETVAL (0x00001FFFu)

/* RZB_EN */

#define CSL_RSZ_RZB_EN_EN_MASK (0x00000001u)
#define CSL_RSZ_RZB_EN_EN_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_EN_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_RSZ_RZB_EN_EN_DISABLE (0x00000000u)
#define CSL_RSZ_RZB_EN_EN_ENABLE (0x00000001u)

#define CSL_RSZ_RZB_EN_RESETVAL (0x00000000u)

/* RZB_MODE */

#define CSL_RSZ_RZB_MODE_OST_MASK (0x00000001u)
#define CSL_RSZ_RZB_MODE_OST_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_MODE_OST_RESETVAL (0x00000000u)
/*----OST Tokens----*/
#define CSL_RSZ_RZB_MODE_OST_CONTINUOUS (0x00000000u)
#define CSL_RSZ_RZB_MODE_OST_ONESHOT (0x00000001u)

#define CSL_RSZ_RZB_MODE_RESETVAL (0x00000000u)

/* RZB_420 */

#define CSL_RSZ_RZB_420_CEN_MASK (0x00000002u)
#define CSL_RSZ_RZB_420_CEN_SHIFT (0x00000001u)
#define CSL_RSZ_RZB_420_CEN_RESETVAL (0x00000000u)
/*----CEN Tokens----*/
#define CSL_RSZ_RZB_420_CEN_DISABLE (0x00000000u)
#define CSL_RSZ_RZB_420_CEN_ENABLE (0x00000001u)

#define CSL_RSZ_RZB_420_YEN_MASK (0x00000001u)
#define CSL_RSZ_RZB_420_YEN_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_420_YEN_RESETVAL (0x00000000u)
/*----YEN Tokens----*/
#define CSL_RSZ_RZB_420_YEN_DISABLE (0x00000000u)
#define CSL_RSZ_RZB_420_YEN_ENABLE (0x00000001u)

#define CSL_RSZ_RZB_420_RESETVAL (0x00000000u)

/* RZB_I_VPS */

#define CSL_RSZ_RZB_I_VPS_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_I_VPS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_I_VPS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_I_VPS_RESETVAL (0x00000000u)

/* RZB_I_HPS */

#define CSL_RSZ_RZB_I_HPS_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_I_HPS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_I_HPS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_I_HPS_RESETVAL (0x00000000u)

/* RZB_O_VSZ */

#define CSL_RSZ_RZB_O_VSZ_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_O_VSZ_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_O_VSZ_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_O_VSZ_RESETVAL (0x00000000u)

/* RZB_O_HSZ */

#define CSL_RSZ_RZB_O_HSZ_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_O_HSZ_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_O_HSZ_VAL_RESETVAL (0x00000001u)

#define CSL_RSZ_RZB_O_HSZ_RESETVAL (0x00000001u)

/* RZB_V_PHS_Y */

#define CSL_RSZ_RZB_V_PHS_Y_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZB_V_PHS_Y_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_V_PHS_Y_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_V_PHS_Y_RESETVAL (0x00000000u)

/* RZB_V_PHS_C */

#define CSL_RSZ_RZB_V_PHS_C_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZB_V_PHS_C_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_V_PHS_C_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_V_PHS_C_RESETVAL (0x00000000u)

/* RZB_V_DIF */

#define CSL_RSZ_RZB_V_DIF_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZB_V_DIF_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_V_DIF_VAL_RESETVAL (0x00000100u)

#define CSL_RSZ_RZB_V_DIF_RESETVAL (0x00000100u)

/* RZB_V_TYP */

#define CSL_RSZ_RZB_V_TYP_C_MASK (0x00000002u)
#define CSL_RSZ_RZB_V_TYP_C_SHIFT (0x00000001u)
#define CSL_RSZ_RZB_V_TYP_C_RESETVAL (0x00000000u)
/*----C Tokens----*/
#define CSL_RSZ_RZB_V_TYP_C_CUBIC (0x00000000u)
#define CSL_RSZ_RZB_V_TYP_C_LINEAR (0x00000001u)

#define CSL_RSZ_RZB_V_TYP_Y_MASK (0x00000001u)
#define CSL_RSZ_RZB_V_TYP_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_V_TYP_Y_RESETVAL (0x00000000u)
/*----Y Tokens----*/
#define CSL_RSZ_RZB_V_TYP_Y_CUBIC (0x00000000u)
#define CSL_RSZ_RZB_V_TYP_Y_LINEAR (0x00000001u)

#define CSL_RSZ_RZB_V_TYP_RESETVAL (0x00000000u)

/* RZB_V_LPF */

#define CSL_RSZ_RZB_V_LPF_C_MASK (0x00000FC0u)
#define CSL_RSZ_RZB_V_LPF_C_SHIFT (0x00000006u)
#define CSL_RSZ_RZB_V_LPF_C_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_V_LPF_Y_MASK (0x0000003Fu)
#define CSL_RSZ_RZB_V_LPF_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_V_LPF_Y_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_V_LPF_RESETVAL (0x00000000u)

/* RZB_H_PHS */

#define CSL_RSZ_RZB_H_PHS_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZB_H_PHS_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_H_PHS_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_H_PHS_RESETVAL (0x00000000u)

/* RZB_H_PHS_ADJ */

#define CSL_RSZ_RZB_H_PHS_ADJ_VAL_MASK (0x000001FFu)
#define CSL_RSZ_RZB_H_PHS_ADJ_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_H_PHS_ADJ_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_H_PHS_ADJ_RESETVAL (0x00000000u)

/* RZB_H_DIF */

#define CSL_RSZ_RZB_H_DIF_VAL_MASK (0x00003FFFu)
#define CSL_RSZ_RZB_H_DIF_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_H_DIF_VAL_RESETVAL (0x00000100u)

#define CSL_RSZ_RZB_H_DIF_RESETVAL (0x00000100u)

/* RZB_H_TYP */

#define CSL_RSZ_RZB_H_TYP_C_MASK (0x00000002u)
#define CSL_RSZ_RZB_H_TYP_C_SHIFT (0x00000001u)
#define CSL_RSZ_RZB_H_TYP_C_RESETVAL (0x00000000u)
/*----C Tokens----*/
#define CSL_RSZ_RZB_H_TYP_C_CUBIC (0x00000000u)
#define CSL_RSZ_RZB_H_TYP_C_LINEAR (0x00000001u)

#define CSL_RSZ_RZB_H_TYP_Y_MASK (0x00000001u)
#define CSL_RSZ_RZB_H_TYP_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_H_TYP_Y_RESETVAL (0x00000000u)
/*----Y Tokens----*/
#define CSL_RSZ_RZB_H_TYP_Y_CUBIC (0x00000000u)
#define CSL_RSZ_RZB_H_TYP_Y_LINEAR (0x00000001u)

#define CSL_RSZ_RZB_H_TYP_RESETVAL (0x00000000u)

/* RZB_H_LPF */

#define CSL_RSZ_RZB_H_LPF_C_MASK (0x00000FC0u)
#define CSL_RSZ_RZB_H_LPF_C_SHIFT (0x00000006u)
#define CSL_RSZ_RZB_H_LPF_C_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_H_LPF_Y_MASK (0x0000003Fu)
#define CSL_RSZ_RZB_H_LPF_Y_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_H_LPF_Y_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_H_LPF_RESETVAL (0x00000000u)

/* RZB_DWN_EN */

#define CSL_RSZ_RZB_DWN_EN_EN_MASK (0x00000001u)
#define CSL_RSZ_RZB_DWN_EN_EN_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_DWN_EN_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_RSZ_RZB_DWN_EN_EN_DISABLE (0x00000000u)
#define CSL_RSZ_RZB_DWN_EN_EN_ENABLE (0x00000001u)

#define CSL_RSZ_RZB_DWN_EN_RESETVAL (0x00000000u)

/* RZB_DWN_AV */

#define CSL_RSZ_RZB_DWN_AV_V_MASK (0x00000038u)
#define CSL_RSZ_RZB_DWN_AV_V_SHIFT (0x00000003u)
#define CSL_RSZ_RZB_DWN_AV_V_RESETVAL (0x00000000u)
/*----V Tokens----*/
#define CSL_RSZ_RZB_DWN_AV_V__DIV2 (0x00000000u)
#define CSL_RSZ_RZB_DWN_AV_V__DIV4 (0x00000001u)
#define CSL_RSZ_RZB_DWN_AV_V__DIV8 (0x00000002u)
#define CSL_RSZ_RZB_DWN_AV_V__DIV16 (0x00000003u)
#define CSL_RSZ_RZB_DWN_AV_V__DIV32 (0x00000004u)
#define CSL_RSZ_RZB_DWN_AV_V__DIV64 (0x00000005u)
#define CSL_RSZ_RZB_DWN_AV_V__DIV128 (0x00000006u)
#define CSL_RSZ_RZB_DWN_AV_V__DIV256 (0x00000007u)

#define CSL_RSZ_RZB_DWN_AV_H_MASK (0x00000007u)
#define CSL_RSZ_RZB_DWN_AV_H_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_DWN_AV_H_RESETVAL (0x00000000u)
/*----H Tokens----*/
#define CSL_RSZ_RZB_DWN_AV_H__DIV2 (0x00000000u)
#define CSL_RSZ_RZB_DWN_AV_H__DIV4 (0x00000001u)
#define CSL_RSZ_RZB_DWN_AV_H__DIV8 (0x00000002u)
#define CSL_RSZ_RZB_DWN_AV_H__DIV16 (0x00000003u)
#define CSL_RSZ_RZB_DWN_AV_H__DIV32 (0x00000004u)
#define CSL_RSZ_RZB_DWN_AV_H__DIV64 (0x00000005u)
#define CSL_RSZ_RZB_DWN_AV_H__DIV128 (0x00000006u)
#define CSL_RSZ_RZB_DWN_AV_H__DIV256 (0x00000007u)

#define CSL_RSZ_RZB_DWN_AV_RESETVAL (0x00000000u)

/* RZB_RGB_EN */

#define CSL_RSZ_RZB_RGB_EN_EN_MASK (0x00000001u)
#define CSL_RSZ_RZB_RGB_EN_EN_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_RGB_EN_EN_RESETVAL (0x00000000u)
/*----EN Tokens----*/
#define CSL_RSZ_RZB_RGB_EN_EN_DISABLE (0x00000000u)
#define CSL_RSZ_RZB_RGB_EN_EN_ENABLE (0x00000001u)

#define CSL_RSZ_RZB_RGB_EN_RESETVAL (0x00000000u)

/* RZB_RGB_TYP */

#define CSL_RSZ_RZB_RGB_TYP_MSK1_MASK (0x00000004u)
#define CSL_RSZ_RZB_RGB_TYP_MSK1_SHIFT (0x00000002u)
#define CSL_RSZ_RZB_RGB_TYP_MSK1_RESETVAL (0x00000000u)
/*----MSK1 Tokens----*/
#define CSL_RSZ_RZB_RGB_TYP_MSK1_NOMASK (0x00000000u)
#define CSL_RSZ_RZB_RGB_TYP_MSK1_MASKLAST2 (0x00000001u)

#define CSL_RSZ_RZB_RGB_TYP_MSK0_MASK (0x00000002u)
#define CSL_RSZ_RZB_RGB_TYP_MSK0_SHIFT (0x00000001u)
#define CSL_RSZ_RZB_RGB_TYP_MSK0_RESETVAL (0x00000000u)
/*----MSK0 Tokens----*/
#define CSL_RSZ_RZB_RGB_TYP_MSK0_NOMASK (0x00000000u)
#define CSL_RSZ_RZB_RGB_TYP_MSK0_MASKFIRST2 (0x00000001u)

#define CSL_RSZ_RZB_RGB_TYP_TYP_MASK (0x00000001u)
#define CSL_RSZ_RZB_RGB_TYP_TYP_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_RGB_TYP_TYP_RESETVAL (0x00000000u)
/*----TYP Tokens----*/
#define CSL_RSZ_RZB_RGB_TYP_TYP_OUTPUT32BITS (0x00000000u)
#define CSL_RSZ_RZB_RGB_TYP_TYP_OUTPUT16BITS (0x00000001u)

#define CSL_RSZ_RZB_RGB_TYP_RESETVAL (0x00000000u)

/* RZB_RGB_BLD */

#define CSL_RSZ_RZB_RGB_BLD_VAL_MASK (0x000000FFu)
#define CSL_RSZ_RZB_RGB_BLD_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_RGB_BLD_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_RGB_BLD_RESETVAL (0x00000000u)

/* RZB_SDR_Y_BAD_H */

#define CSL_RSZ_RZB_SDR_Y_BAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_Y_BAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_Y_BAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_Y_BAD_H_RESETVAL (0x00000000u)

/* RZB_SDR_Y_BAD_L */

#define CSL_RSZ_RZB_SDR_Y_BAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_Y_BAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_Y_BAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_Y_BAD_L_RESETVAL (0x00000000u)

/* RZB_SDR_Y_SAD_H */

#define CSL_RSZ_RZB_SDR_Y_SAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_Y_SAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_Y_SAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_Y_SAD_H_RESETVAL (0x00000000u)

/* RZB_SDR_Y_SAD_L */

#define CSL_RSZ_RZB_SDR_Y_SAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_Y_SAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_Y_SAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_Y_SAD_L_RESETVAL (0x00000000u)

/* RZB_SDR_Y_OFT */

#define CSL_RSZ_RZB_SDR_Y_OFT_OFT_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_Y_OFT_OFT_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_Y_OFT_OFT_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_Y_OFT_RESETVAL (0x00000000u)

/* RZB_SDR_Y_PTR_S */

#define CSL_RSZ_RZB_SDR_Y_PTR_S_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_SDR_Y_PTR_S_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_Y_PTR_S_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_Y_PTR_S_RESETVAL (0x00000000u)

/* RZB_SDR_Y_PTR_E */

#define CSL_RSZ_RZB_SDR_Y_PTR_E_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_SDR_Y_PTR_E_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_Y_PTR_E_VAL_RESETVAL (0x00001FFFu)

#define CSL_RSZ_RZB_SDR_Y_PTR_E_RESETVAL (0x00001FFFu)

/* RZB_SDR_C_BAD_H */

#define CSL_RSZ_RZB_SDR_C_BAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_C_BAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_C_BAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_C_BAD_H_RESETVAL (0x00000000u)

/* RZB_SDR_C_BAD_L */

#define CSL_RSZ_RZB_SDR_C_BAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_C_BAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_C_BAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_C_BAD_L_RESETVAL (0x00000000u)

/* RZB_SDR_C_SAD_H */

#define CSL_RSZ_RZB_SDR_C_SAD_H_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_C_SAD_H_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_C_SAD_H_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_C_SAD_H_RESETVAL (0x00000000u)

/* RZB_SDR_C_SAD_L */

#define CSL_RSZ_RZB_SDR_C_SAD_L_VAL_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_C_SAD_L_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_C_SAD_L_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_C_SAD_L_RESETVAL (0x00000000u)

/* RZB_SDR_C_OFT */

#define CSL_RSZ_RZB_SDR_C_OFT_OFT_MASK (0x0000FFFFu)
#define CSL_RSZ_RZB_SDR_C_OFT_OFT_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_C_OFT_OFT_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_C_OFT_RESETVAL (0x00000000u)

/* RZB_SDR_C_PTR_S */

#define CSL_RSZ_RZB_SDR_C_PTR_S_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_SDR_C_PTR_S_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_C_PTR_S_VAL_RESETVAL (0x00000000u)

#define CSL_RSZ_RZB_SDR_C_PTR_S_RESETVAL (0x00000000u)

/* RZB_SDR_C_PTR_E */

#define CSL_RSZ_RZB_SDR_C_PTR_E_VAL_MASK (0x00001FFFu)
#define CSL_RSZ_RZB_SDR_C_PTR_E_VAL_SHIFT (0x00000000u)
#define CSL_RSZ_RZB_SDR_C_PTR_E_VAL_RESETVAL (0x00001FFFu)

#define CSL_RSZ_RZB_SDR_C_PTR_E_RESETVAL (0x00001FFFu)
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
