/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file cslr__nsf.h
 *
 * This File contains declarations of register overlay structure for NSF module. It also
 * defines tokens for register fields in NSF module. It is targeted at MONICA/OMAP4. 
 * 
 *
 * @path  $(DUCATIVOB)\drivers\csl\iss\simcop\nsf\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 20-Jan-2009 Sanish Mahadik: Initial Release
 *! 09-Dec -2008 Sherin Sasidharan:  Created the file.  
 *! 
 *!Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

#ifndef _CSLR__NF2_1_H_
#define _CSLR__NF2_1_H_

#ifdef __cplusplus

extern "C" {
#endif
    /* User code goes here */
    /* ------compilation control switches ------------------------- */
   /****************************************************************
    *  INCLUDE FILES                                                 
    ****************************************************************/
    /* ----- system and platform files ---------------------------- */
   /*-------program files ----------------------------------------*/
#include "../common/cslr.h"
#include "../common/csl_types.h"

    /* Minimum unit = 1 byte */

   /****************************************************************
    *  PUBLIC DECLARATIONS Defined here, used elsewhere
    ****************************************************************/
   /*--------data declarations -----------------------------------*/

   /**************************************************************************\
    * Register Overlay Structure
    \**************************************************************************/

    /* ========================================================================== 
     */
   /**
    * This is a structure description for CSL_NsfRegs. This structure is used for register overlay of the NSF module. 
    *
    * @param  CFG  Top-level configuration register
    *
    * @param  BLKW  block-width register
    *
    * @param  BLKH  block-height register
    *
    * @param  IOMST  IOMEM start-address register
    *
    * @param  IOMOFST  IOMEM offset register
    *
    * @param  WMST  WMEM start-address register
    *
    * @param  WMOFST  WMEM offset register
    *
    * @param  THRXX  Threshold parameter registers
    *
    * @param  SHD_X_Y  Shading Correction parameter registers
    *
    * @param  MAXG  Maximum Gain value register
    * 
    * @param  SFT_SLOPE  Soft-threshold slope register
    *
    * @param  EE_LX_YY  edge-enhancement parameter registers 
    *
    * @param  IOMSTC  Chroma start-address register
    *
    * @param  DS_XY  Desaturation parameter registers
    *
    *
    * @see 
    */
    /* ========================================================================== 
     */
    typedef struct {
        volatile Uint16 CFG;
        volatile Uint16 BLKW;
        volatile Uint16 BLKH;
        volatile Uint16 IOMST;
        volatile Uint16 IOMOFST;
        volatile Uint16 WMST;
        volatile Uint16 WMOFST;
        volatile Uint16 THR00;
        volatile Uint16 THR01;
        volatile Uint16 THR02;
        volatile Uint16 THR03;
        volatile Uint16 THR04;
        volatile Uint16 THR05;
        volatile Uint16 THR10;
        volatile Uint16 THR11;
        volatile Uint16 THR12;
        volatile Uint16 THR13;
        volatile Uint16 THR14;
        volatile Uint16 THR15;
        volatile Uint16 THR20;
        volatile Uint16 THR21;
        volatile Uint16 THR22;
        volatile Uint16 THR23;
        volatile Uint16 THR24;
        volatile Uint16 THR25;
        volatile Uint16 SHD_XST;
        volatile Uint16 SHD_YST;
        volatile Uint16 SHD_HA1;
        volatile Uint16 SHD_HA2;
        volatile Uint16 SHD_VA1;
        volatile Uint16 SHD_VA2;
        volatile Uint16 SHD_HS;
        volatile Uint16 SHD_VS;
        volatile Uint16 SHD_GADJ;
        volatile Uint16 SHD_OADJ;
        volatile Uint16 MAXG;
        volatile Uint16 SFT_SLOPE;
        volatile Uint16 EE_L1_SLOPE;
        volatile Uint16 EE_L1_THR1;
        volatile Uint16 EE_L1_THR2;
        volatile Uint16 EE_L1_OFST2;
        volatile Uint16 EE_L2_SLOPE;
        volatile Uint16 EE_L2_THR1;
        volatile Uint16 EE_L2_THR2;
        volatile Uint16 EE_L2_OFST2;
        volatile Uint16 EE_L3_SLOPE;
        volatile Uint16 EE_L3_THR1;
        volatile Uint16 EE_L3_THR2;
        volatile Uint16 EE_L3_OFST2;
        volatile Uint16 IOMSTC;
        volatile Uint16 DS_THR;
        volatile Uint16 DS_SLOPE1;
        volatile Uint16 DS_SLOPE2;
    } CSL_NsfRegs;

   /**************************************************************************\
    * Field Definition Macros
    \**************************************************************************/

    /* CFG */

#define CSL_NF2_CFG_BUSY_MASK (0x8000u)
#define CSL_NF2_CFG_BUSY_SHIFT (0x000Fu)
#define CSL_NF2_CFG_BUSY_RESETVAL (0x0000u)
   /*----BUSY Tokens----*/
#define CSL_NF2_CFG_BUSY_IDLE (0x0000u)
#define CSL_NF2_CFG_BUSY_BUSY (0x0001u)

#define CSL_NF2_CFG_DESAT_EN_MASK (0x4000u)
#define CSL_NF2_CFG_DESAT_EN_SHIFT (0x000Eu)
#define CSL_NF2_CFG_DESAT_EN_RESETVAL (0x0000u)
   /*----DESAT_EN Tokens----*/
#define CSL_NF2_CFG_DESAT_EN_DISABLE (0x0000u)
#define CSL_NF2_CFG_DESAT_EN_ENABLE (0x0001u)

#define CSL_NF2_CFG_SHD_EN_MASK (0x2000u)
#define CSL_NF2_CFG_SHD_EN_SHIFT (0x000Du)
#define CSL_NF2_CFG_SHD_EN_RESETVAL (0x0000u)
   /*----SHD_EN Tokens----*/
#define CSL_NF2_CFG_SHD_EN_DISABLE (0x0000u)
#define CSL_NF2_CFG_SHD_EN_ENABLE (0x0001u)

#define CSL_NF2_CFG_EE_EN_MASK (0x1000u)
#define CSL_NF2_CFG_EE_EN_SHIFT (0x000Cu)
#define CSL_NF2_CFG_EE_EN_RESETVAL (0x0000u)
   /*----EE_EN Tokens----*/
#define CSL_NF2_CFG_EE_EN_DISABLE (0x0000u)
#define CSL_NF2_CFG_EE_EN_ENABLE (0x0001u)

#define CSL_NF2_CFG_LBKEEP_MASK (0x0800u)
#define CSL_NF2_CFG_LBKEEP_SHIFT (0x000Bu)
#define CSL_NF2_CFG_LBKEEP_RESETVAL (0x0000u)
   /*----LBKEEP Tokens----*/
#define CSL_NF2_CFG_LBKEEP_NONE (0x0000u)
#define CSL_NF2_CFG_LBKEEP_ALL (0x0001u)

#define CSL_NF2_CFG_Y_EN_MASK (0x0400u)
#define CSL_NF2_CFG_Y_EN_SHIFT (0x000Au)
#define CSL_NF2_CFG_Y_EN_RESETVAL (0x0001u)
   /*----Y_EN Tokens----*/
#define CSL_NF2_CFG_Y_EN_ENABLE (0x0001u)
#define CSL_NF2_CFG_Y_EN_DISABLE (0x0000u)

#define CSL_NF2_CFG_CBCR_EN_MASK (0x0200u)
#define CSL_NF2_CFG_CBCR_EN_SHIFT (0x0009u)
#define CSL_NF2_CFG_CBCR_EN_RESETVAL (0x0001u)
   /*----CBCR_EN Tokens----*/
#define CSL_NF2_CFG_CBCR_EN_ENABLE (0x0001u)
#define CSL_NF2_CFG_CBCR_EN_DISABLE (0x0000u)

#define CSL_NF2_CFG_SOFT_THR_EN_CHROMA_MASK (0x0100u)
#define CSL_NF2_CFG_SOFT_THR_EN_CHROMA_SHIFT (0x0008u)
#define CSL_NF2_CFG_SOFT_THR_EN_CHROMA_RESETVAL (0x0000u)
   /*----SOFT_THR_EN_CHROMA Tokens----*/
#define CSL_NF2_CFG_SOFT_THR_EN_CHROMA_ENABLE (0x0001u)
#define CSL_NF2_CFG_SOFT_THR_EN_CHROMA_DISABLE (0x0000u)

#define CSL_NF2_CFG_SOFT_THR_EN_LUMA_MASK (0x0080u)
#define CSL_NF2_CFG_SOFT_THR_EN_LUMA_SHIFT (0x0007u)
#define CSL_NF2_CFG_SOFT_THR_EN_LUMA_RESETVAL (0x0000u)
   /*----SOFT_THR_EN_LUMA Tokens----*/
#define CSL_NF2_CFG_SOFT_THR_EN_LUMA_ENABLE (0x0001u)
#define CSL_NF2_CFG_SOFT_THR_EN_LUMA_DISABLE (0x0000u)

#define CSL_NF2_CFG_C_AS_INTRLV_Y_MASK (0x0020u)
#define CSL_NF2_CFG_C_AS_INTRLV_Y_SHIFT (0x0005u)
#define CSL_NF2_CFG_C_AS_INTRLV_Y_RESETVAL (0x0000u)
   /*----C_AS_INTRLV_Y Tokens----*/
#define CSL_NF2_CFG_C_AS_INTRLV_Y_ENABLE (0x0001u)
#define CSL_NF2_CFG_C_AS_INTRLV_Y_DISABLE (0x0000u)

#define CSL_NF2_CFG_TRIG_SRC_MASK (0x0010u)
#define CSL_NF2_CFG_TRIG_SRC_SHIFT (0x0004u)
#define CSL_NF2_CFG_TRIG_SRC_RESETVAL (0x0000u)
   /*----TRIG_SRC Tokens----*/
#define CSL_NF2_CFG_TRIG_SRC_ENABLE (0x0001u)
#define CSL_NF2_CFG_TRIG_SRC_DISABLE (0x0000u)

#define CSL_NF2_CFG_MODE_MASK (0x0008u)
#define CSL_NF2_CFG_MODE_SHIFT (0x0003u)
#define CSL_NF2_CFG_MODE_RESETVAL (0x0001u)
   /*----MODE Tokens----*/
#define CSL_NF2_CFG_MODE_ENABLE (0x0001u)
#define CSL_NF2_CFG_MODE_DISABLE (0x0000u)

#define CSL_NF2_CFG_OUT_EN_MASK (0x0004u)
#define CSL_NF2_CFG_OUT_EN_SHIFT (0x0002u)
#define CSL_NF2_CFG_OUT_EN_RESETVAL (0x0000u)
   /*----OUT_EN Tokens----*/
#define CSL_NF2_CFG_OUT_EN_ENABLE (0x0001u)
#define CSL_NF2_CFG_OUT_EN_DISABLE (0x0000u)

#define CSL_NF2_CFG_INT_EN_MASK (0x0002u)
#define CSL_NF2_CFG_INT_EN_SHIFT (0x0001u)
#define CSL_NF2_CFG_INT_EN_RESETVAL (0x0000u)
   /*----INT_EN Tokens----*/
#define CSL_NF2_CFG_INT_EN_ENABLE (0x0001u)
#define CSL_NF2_CFG_INT_EN_DISABLE (0x0000u)

#define CSL_NF2_CFG_START_MASK (0x0001u)
#define CSL_NF2_CFG_START_SHIFT (0x0000u)
#define CSL_NF2_CFG_START_RESETVAL (0x0000u)
   /*----START Tokens----*/
#define CSL_NF2_CFG_START_ENABLE (0x0001u)
#define CSL_NF2_CFG_START_DISABLE (0x0000u)

#define CSL_NF2_CFG_RESETVAL (0x0608u)

    /* BLKW */

#define CSL_NF2_BLKW_BLKW_MASK (0x3FFFu)
#define CSL_NF2_BLKW_BLKW_SHIFT (0x0000u)
#define CSL_NF2_BLKW_BLKW_RESETVAL (0x0200u)

#define CSL_NF2_BLKW_RESETVAL (0x0200u)

    /* BLKH */

#define CSL_NF2_BLKH_BLKH_MASK (0x3FFFu)
#define CSL_NF2_BLKH_BLKH_SHIFT (0x0000u)
#define CSL_NF2_BLKH_BLKH_RESETVAL (0x0004u)

#define CSL_NF2_BLKH_RESETVAL (0x0004u)

    /* IOMST */

#define CSL_NF2_IOMST_ADDR_MASK (0xFFFFu)
#define CSL_NF2_IOMST_ADDR_SHIFT (0x0000u)
#define CSL_NF2_IOMST_ADDR_RESETVAL (0x0000u)

#define CSL_NF2_IOMST_RESETVAL (0x0000u)

    /* IOMOFST */

#define CSL_NF2_IOMOFST_ADDR_MASK (0xFFFFu)
#define CSL_NF2_IOMOFST_ADDR_SHIFT (0x0000u)
#define CSL_NF2_IOMOFST_ADDR_RESETVAL (0x0400u)

#define CSL_NF2_IOMOFST_RESETVAL (0x0400u)

    /* WMST */

#define CSL_NF2_WMST_ADDR_MASK (0xFFFFu)
#define CSL_NF2_WMST_ADDR_SHIFT (0x0000u)
#define CSL_NF2_WMST_ADDR_RESETVAL (0x0000u)

#define CSL_NF2_WMST_RESETVAL (0x0000u)

    /* WMOFST */

#define CSL_NF2_WMOFST_ADDR_MASK (0xFFFFu)
#define CSL_NF2_WMOFST_ADDR_SHIFT (0x0000u)
#define CSL_NF2_WMOFST_ADDR_RESETVAL (0x0200u)

#define CSL_NF2_WMOFST_RESETVAL (0x0200u)

    /* THR00 */

#define CSL_NF2_THR00_OFSET_01_MASK (0x03FFu)
#define CSL_NF2_THR00_OFSET_01_SHIFT (0x0000u)
#define CSL_NF2_THR00_OFSET_01_RESETVAL (0x0000u)

#define CSL_NF2_THR00_RESETVAL (0x0000u)

    /* THR01 */

#define CSL_NF2_THR01_SLOPE_01_MASK (0x01FFu)
#define CSL_NF2_THR01_SLOPE_01_SHIFT (0x0000u)
#define CSL_NF2_THR01_SLOPE_01_RESETVAL (0x0000u)

#define CSL_NF2_THR01_RESETVAL (0x0000u)

    /* THR02 */

#define CSL_NF2_THR02_OFSET_02_MASK (0x03FFu)
#define CSL_NF2_THR02_OFSET_02_SHIFT (0x0000u)
#define CSL_NF2_THR02_OFSET_02_RESETVAL (0x0000u)

#define CSL_NF2_THR02_RESETVAL (0x0000u)

    /* THR03 */

#define CSL_NF2_THR03_SLOPE_02_MASK (0x01FFu)
#define CSL_NF2_THR03_SLOPE_02_SHIFT (0x0000u)
#define CSL_NF2_THR03_SLOPE_02_RESETVAL (0x0000u)

#define CSL_NF2_THR03_RESETVAL (0x0000u)

    /* THR04 */

#define CSL_NF2_THR04_OFSET_03_MASK (0x03FFu)
#define CSL_NF2_THR04_OFSET_03_SHIFT (0x0000u)
#define CSL_NF2_THR04_OFSET_03_RESETVAL (0x0000u)

#define CSL_NF2_THR04_RESETVAL (0x0000u)

    /* THR05 */

#define CSL_NF2_THR05_SLOPE_03_MASK (0x01FFu)
#define CSL_NF2_THR05_SLOPE_03_SHIFT (0x0000u)
#define CSL_NF2_THR05_SLOPE_03_RESETVAL (0x0000u)

#define CSL_NF2_THR05_RESETVAL (0x0000u)

    /* THR10 */

#define CSL_NF2_THR10_OFSET_11_MASK (0x03FFu)
#define CSL_NF2_THR10_OFSET_11_SHIFT (0x0000u)
#define CSL_NF2_THR10_OFSET_11_RESETVAL (0x0000u)

#define CSL_NF2_THR10_RESETVAL (0x0000u)

    /* THR11 */

#define CSL_NF2_THR11_SLOPE_11_MASK (0x01FFu)
#define CSL_NF2_THR11_SLOPE_11_SHIFT (0x0000u)
#define CSL_NF2_THR11_SLOPE_11_RESETVAL (0x0000u)

#define CSL_NF2_THR11_RESETVAL (0x0000u)

    /* THR12 */

#define CSL_NF2_THR12_OFSET_12_MASK (0x03FFu)
#define CSL_NF2_THR12_OFSET_12_SHIFT (0x0000u)
#define CSL_NF2_THR12_OFSET_12_RESETVAL (0x0000u)

#define CSL_NF2_THR12_RESETVAL (0x0000u)

    /* THR13 */

#define CSL_NF2_THR13_SLOPE_12_MASK (0x01FFu)
#define CSL_NF2_THR13_SLOPE_12_SHIFT (0x0000u)
#define CSL_NF2_THR13_SLOPE_12_RESETVAL (0x0000u)

#define CSL_NF2_THR13_RESETVAL (0x0000u)

    /* THR14 */

#define CSL_NF2_THR14_OFSET_13_MASK (0x03FFu)
#define CSL_NF2_THR14_OFSET_13_SHIFT (0x0000u)
#define CSL_NF2_THR14_OFSET_13_RESETVAL (0x0000u)

#define CSL_NF2_THR14_RESETVAL (0x0000u)

    /* THR15 */

#define CSL_NF2_THR15_SLOPE_13_MASK (0x01FFu)
#define CSL_NF2_THR15_SLOPE_13_SHIFT (0x0000u)
#define CSL_NF2_THR15_SLOPE_13_RESETVAL (0x0000u)

#define CSL_NF2_THR15_RESETVAL (0x0000u)

    /* THR20 */

#define CSL_NF2_THR20_OFSET_21_MASK (0x03FFu)
#define CSL_NF2_THR20_OFSET_21_SHIFT (0x0000u)
#define CSL_NF2_THR20_OFSET_21_RESETVAL (0x0000u)

#define CSL_NF2_THR20_RESETVAL (0x0000u)

    /* THR21 */

#define CSL_NF2_THR21_SLOPE_21_MASK (0x01FFu)
#define CSL_NF2_THR21_SLOPE_21_SHIFT (0x0000u)
#define CSL_NF2_THR21_SLOPE_21_RESETVAL (0x0000u)

#define CSL_NF2_THR21_RESETVAL (0x0000u)

    /* THR22 */

#define CSL_NF2_THR22_OFSET_22_MASK (0x03FFu)
#define CSL_NF2_THR22_OFSET_22_SHIFT (0x0000u)
#define CSL_NF2_THR22_OFSET_22_RESETVAL (0x0000u)

#define CSL_NF2_THR22_RESETVAL (0x0000u)

    /* THR23 */

#define CSL_NF2_THR23_SLOPE_22_MASK (0x01FFu)
#define CSL_NF2_THR23_SLOPE_22_SHIFT (0x0000u)
#define CSL_NF2_THR23_SLOPE_22_RESETVAL (0x0000u)

#define CSL_NF2_THR23_RESETVAL (0x0000u)

    /* THR24 */

#define CSL_NF2_THR24_OFSET_23_MASK (0x03FFu)
#define CSL_NF2_THR24_OFSET_23_SHIFT (0x0000u)
#define CSL_NF2_THR24_OFSET_23_RESETVAL (0x0000u)

#define CSL_NF2_THR24_RESETVAL (0x0000u)

    /* THR25 */

#define CSL_NF2_THR25_SLOPE_23_MASK (0x01FFu)
#define CSL_NF2_THR25_SLOPE_23_SHIFT (0x0000u)
#define CSL_NF2_THR25_SLOPE_23_RESETVAL (0x0000u)

#define CSL_NF2_THR25_RESETVAL (0x0000u)

    /* SHD_XST */

#define CSL_NF2_SHD_XST_HCOUNT_MASK (0x3FFFu)
#define CSL_NF2_SHD_XST_HCOUNT_SHIFT (0x0000u)
#define CSL_NF2_SHD_XST_HCOUNT_RESETVAL (0x0000u)

#define CSL_NF2_SHD_XST_RESETVAL (0x0000u)

    /* SHD_YST */

#define CSL_NF2_SHD_YST_VCOUNT_MASK (0x3FFFu)
#define CSL_NF2_SHD_YST_VCOUNT_SHIFT (0x0000u)
#define CSL_NF2_SHD_YST_VCOUNT_RESETVAL (0x0000u)

#define CSL_NF2_SHD_YST_RESETVAL (0x0000u)

    /* SHD_HA1 */

#define CSL_NF2_SHD_HA1_HA1_MASK (0x1FFFu)
#define CSL_NF2_SHD_HA1_HA1_SHIFT (0x0000u)
#define CSL_NF2_SHD_HA1_HA1_RESETVAL (0x0000u)

#define CSL_NF2_SHD_HA1_RESETVAL (0x0000u)

    /* SHD_HA2 */

#define CSL_NF2_SHD_HA2_HA2_MASK (0x1FFFu)
#define CSL_NF2_SHD_HA2_HA2_SHIFT (0x0000u)
#define CSL_NF2_SHD_HA2_HA2_RESETVAL (0x0000u)

#define CSL_NF2_SHD_HA2_RESETVAL (0x0000u)

    /* SHD_VA1 */

#define CSL_NF2_SHD_VA1_VA1_MASK (0x1FFFu)
#define CSL_NF2_SHD_VA1_VA1_SHIFT (0x0000u)
#define CSL_NF2_SHD_VA1_VA1_RESETVAL (0x0000u)

#define CSL_NF2_SHD_VA1_RESETVAL (0x0000u)

    /* SHD_VA2 */

#define CSL_NF2_SHD_VA2_VA2_MASK (0x1FFFu)
#define CSL_NF2_SHD_VA2_VA2_SHIFT (0x0000u)
#define CSL_NF2_SHD_VA2_VA2_RESETVAL (0x0000u)

#define CSL_NF2_SHD_VA2_RESETVAL (0x0000u)

    /* SHD_HS */

#define CSL_NF2_SHD_HS_HCS_MASK (0x3000u)
#define CSL_NF2_SHD_HS_HCS_SHIFT (0x000Cu)
#define CSL_NF2_SHD_HS_HCS_RESETVAL (0x0000u)

#define CSL_NF2_SHD_HS_HS2_MASK (0x0F00u)
#define CSL_NF2_SHD_HS_HS2_SHIFT (0x0008u)
#define CSL_NF2_SHD_HS_HS2_RESETVAL (0x0000u)

#define CSL_NF2_SHD_HS_HS1_MASK (0x00F0u)
#define CSL_NF2_SHD_HS_HS1_SHIFT (0x0004u)
#define CSL_NF2_SHD_HS_HS1_RESETVAL (0x0000u)

#define CSL_NF2_SHD_HS_S0_MASK (0x000Fu)
#define CSL_NF2_SHD_HS_S0_SHIFT (0x0000u)
#define CSL_NF2_SHD_HS_S0_RESETVAL (0x0000u)

#define CSL_NF2_SHD_HS_RESETVAL (0x0000u)

    /* SHD_VS */

#define CSL_NF2_SHD_VS_VCS_MASK (0x3000u)
#define CSL_NF2_SHD_VS_VCS_SHIFT (0x000Cu)
#define CSL_NF2_SHD_VS_VCS_RESETVAL (0x0000u)

#define CSL_NF2_SHD_VS_VS2_MASK (0x0F00u)
#define CSL_NF2_SHD_VS_VS2_SHIFT (0x0008u)
#define CSL_NF2_SHD_VS_VS2_RESETVAL (0x0000u)

#define CSL_NF2_SHD_VS_VS1_MASK (0x00F0u)
#define CSL_NF2_SHD_VS_VS1_SHIFT (0x0004u)
#define CSL_NF2_SHD_VS_VS1_RESETVAL (0x0000u)

#define CSL_NF2_SHD_VS_RESETVAL (0x0000u)

    /* SHD_GADJ */

#define CSL_NF2_SHD_GADJ_GADJ_MASK (0x00FFu)
#define CSL_NF2_SHD_GADJ_GADJ_SHIFT (0x0000u)
#define CSL_NF2_SHD_GADJ_GADJ_RESETVAL (0x0000u)

#define CSL_NF2_SHD_GADJ_RESETVAL (0x0000u)

    /* SHD_OADJ */

#define CSL_NF2_SHD_OADJ_OABJ_MASK (0x1FFFu)
#define CSL_NF2_SHD_OADJ_OABJ_SHIFT (0x0000u)
#define CSL_NF2_SHD_OADJ_OABJ_RESETVAL (0x0000u)

#define CSL_NF2_SHD_OADJ_RESETVAL (0x0000u)

    /* MAXG */

#define CSL_NF2_MAXG_MAXG_MASK (0x01FFu)
#define CSL_NF2_MAXG_MAXG_SHIFT (0x0000u)
#define CSL_NF2_MAXG_MAXG_RESETVAL (0x0000u)

#define CSL_NF2_MAXG_RESETVAL (0x0000u)

    /* SFT_SLOPE */

#define CSL_NF2_SFT_SLOPE_SLOPE_MASK (0x0003u)
#define CSL_NF2_SFT_SLOPE_SLOPE_SHIFT (0x0000u)
#define CSL_NF2_SFT_SLOPE_SLOPE_RESETVAL (0x0000u)

#define CSL_NF2_SFT_SLOPE_RESETVAL (0x0000u)

    /* EE_L1_SLOPE */

#define CSL_NF2_EE_L1_SLOPE_SLOPE_MASK (0x0007u)
#define CSL_NF2_EE_L1_SLOPE_SLOPE_SHIFT (0x0000u)
#define CSL_NF2_EE_L1_SLOPE_SLOPE_RESETVAL (0x0000u)

#define CSL_NF2_EE_L1_SLOPE_RESETVAL (0x0000u)

    /* EE_L1_THR1 */

#define CSL_NF2_EE_L1_THR1_THR_MASK (0x03FFu)
#define CSL_NF2_EE_L1_THR1_THR_SHIFT (0x0000u)
#define CSL_NF2_EE_L1_THR1_THR_RESETVAL (0x0000u)

#define CSL_NF2_EE_L1_THR1_RESETVAL (0x0000u)

    /* EE_L1_THR2 */

#define CSL_NF2_EE_L1_THR2_THR_MASK (0x03FFu)
#define CSL_NF2_EE_L1_THR2_THR_SHIFT (0x0000u)
#define CSL_NF2_EE_L1_THR2_THR_RESETVAL (0x0000u)

#define CSL_NF2_EE_L1_THR2_RESETVAL (0x0000u)

    /* EE_L1_OFST2 */

#define CSL_NF2_EE_L1_OFST2_OFST_MASK (0x03FFu)
#define CSL_NF2_EE_L1_OFST2_OFST_SHIFT (0x0000u)
#define CSL_NF2_EE_L1_OFST2_OFST_RESETVAL (0x0000u)

#define CSL_NF2_EE_L1_OFST2_RESETVAL (0x0000u)

    /* EE_L2_SLOPE */

#define CSL_NF2_EE_L2_SLOPE_SLOPE_MASK (0x0007u)
#define CSL_NF2_EE_L2_SLOPE_SLOPE_SHIFT (0x0000u)
#define CSL_NF2_EE_L2_SLOPE_SLOPE_RESETVAL (0x0000u)

#define CSL_NF2_EE_L2_SLOPE_RESETVAL (0x0000u)

    /* EE_L2_THR1 */

#define CSL_NF2_EE_L2_THR1_THR_MASK (0x03FFu)
#define CSL_NF2_EE_L2_THR1_THR_SHIFT (0x0000u)
#define CSL_NF2_EE_L2_THR1_THR_RESETVAL (0x0000u)

#define CSL_NF2_EE_L2_THR1_RESETVAL (0x0000u)

    /* EE_L2_THR2 */

#define CSL_NF2_EE_L2_THR2_THR_MASK (0x03FFu)
#define CSL_NF2_EE_L2_THR2_THR_SHIFT (0x0000u)
#define CSL_NF2_EE_L2_THR2_THR_RESETVAL (0x0000u)

#define CSL_NF2_EE_L2_THR2_RESETVAL (0x0000u)

    /* EE_L2_OFST2 */

#define CSL_NF2_EE_L2_OFST2_OFST_MASK (0x03FFu)
#define CSL_NF2_EE_L2_OFST2_OFST_SHIFT (0x0000u)
#define CSL_NF2_EE_L2_OFST2_OFST_RESETVAL (0x0000u)

#define CSL_NF2_EE_L2_OFST2_RESETVAL (0x0000u)

    /* EE_L3_SLOPE */

#define CSL_NF2_EE_L3_SLOPE_SLOPE_MASK (0x0007u)
#define CSL_NF2_EE_L3_SLOPE_SLOPE_SHIFT (0x0000u)
#define CSL_NF2_EE_L3_SLOPE_SLOPE_RESETVAL (0x0000u)

#define CSL_NF2_EE_L3_SLOPE_RESETVAL (0x0000u)

    /* EE_L3_THR1 */

#define CSL_NF2_EE_L3_THR1_THR_MASK (0x03FFu)
#define CSL_NF2_EE_L3_THR1_THR_SHIFT (0x0000u)
#define CSL_NF2_EE_L3_THR1_THR_RESETVAL (0x0000u)

#define CSL_NF2_EE_L3_THR1_RESETVAL (0x0000u)

    /* EE_L3_THR2 */

#define CSL_NF2_EE_L3_THR2_THR_MASK (0x03FFu)
#define CSL_NF2_EE_L3_THR2_THR_SHIFT (0x0000u)
#define CSL_NF2_EE_L3_THR2_THR_RESETVAL (0x0000u)

#define CSL_NF2_EE_L3_THR2_RESETVAL (0x0000u)

    /* EE_L3_OFST2 */

#define CSL_NF2_EE_L3_OFST2_OFST_MASK (0x03FFu)
#define CSL_NF2_EE_L3_OFST2_OFST_SHIFT (0x0000u)
#define CSL_NF2_EE_L3_OFST2_OFST_RESETVAL (0x0000u)

#define CSL_NF2_EE_L3_OFST2_RESETVAL (0x0000u)

    /* IOMSTC */

#define CSL_NF2_IOMSTC_ADDR_MASK (0xFFFFu)
#define CSL_NF2_IOMSTC_ADDR_SHIFT (0x0000u)
#define CSL_NF2_IOMSTC_ADDR_RESETVAL (0x0000u)

#define CSL_NF2_IOMSTC_RESETVAL (0x0000u)

    /* DS_THR */

#define CSL_NF2_DS_THR_THR2_MASK (0x3F00u)
#define CSL_NF2_DS_THR_THR2_SHIFT (0x0008u)
#define CSL_NF2_DS_THR_THR2_RESETVAL (0x0000u)

#define CSL_NF2_DS_THR_THR1_MASK (0x003Fu)
#define CSL_NF2_DS_THR_THR1_SHIFT (0x0000u)
#define CSL_NF2_DS_THR_THR1_RESETVAL (0x0000u)

#define CSL_NF2_DS_THR_RESETVAL (0x0000u)

    /* DS_SLOPE1 */

#define CSL_NF2_DS_SLOPE1_SLOPE_MASK (0x03FFu)
#define CSL_NF2_DS_SLOPE1_SLOPE_SHIFT (0x0000u)
#define CSL_NF2_DS_SLOPE1_SLOPE_RESETVAL (0x0000u)

#define CSL_NF2_DS_SLOPE1_RESETVAL (0x0000u)

    /* DS_SLOPE2 */

#define CSL_NF2_DS_SLOPE2_SLOPE_MASK (0x03FFu)
#define CSL_NF2_DS_SLOPE2_SLOPE_SHIFT (0x0000u)
#define CSL_NF2_DS_SLOPE2_SLOPE_RESETVAL (0x0000u)

#define CSL_NF2_DS_SLOPE2_RESETVAL (0x0000u)

#ifdef __cplusplus
}
#endif
#endif
