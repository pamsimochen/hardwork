/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_ede.h
*
* Brief: This file contains the Register Description for vps_ede
*
*********************************************************************/
#ifndef _CSLR_VPS_EDE_H_
#define _CSLR_VPS_EDE_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 CFG_EDE0;
    volatile Uint32 CFG_EDE1;
    volatile Uint32 CFG_EDE2;
    volatile Uint32 CFG_EDE3;
    volatile Uint32 CFG_EDE4;
    volatile Uint32 CFG_EDE5;
    volatile Uint32 CFG_EDE6;
    volatile Uint32 CFG_EDE7;
    volatile Uint32 CFG_EDE8;
    volatile Uint32 CFG_EDE9;
    volatile Uint32 CFG_EDE10;
    volatile Uint32 CFG_EDE11;
    volatile Uint32 CFG_EDE12;
    volatile Uint32 CFG_EDE13;
    volatile Uint32 CFG_EDE14;
    volatile Uint32 CFG_EDE15;
    volatile Uint32 CFG_EDE16;
    volatile Uint32 CFG_EDE17;
    volatile Uint32 CFG_EDE18;
    volatile Uint32 CFG_EDE19;
    volatile Uint32 CFG_EDE20;
    volatile Uint32 CFG_EDE21;
} CSL_Vps_edeRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_edeRegs            *CSL_VpsEdeRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* cfg_ede0 */

#define CSL_VPS_EDE_CFG_EDE0_CFG_LTI_GAIN_MAX_X16_4_MASK (0x0F000000u)
#define CSL_VPS_EDE_CFG_EDE0_CFG_LTI_GAIN_MAX_X16_4_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE0_CFG_LTI_LENGTH_MAX_6_MASK (0x003F0000u)
#define CSL_VPS_EDE_CFG_EDE0_CFG_LTI_LENGTH_MAX_6_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE0_CFG_LTI_UNIFORM_Y_TH_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE0_CFG_LTI_UNIFORM_Y_TH_10_SHIFT (0x00000000u)


/* cfg_ede1 */

#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_GAIN_SLOPE_SHIFT_2_MASK (0x00000300u)
#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_GAIN_SLOPE_SHIFT_2_SHIFT (0x00000008u)

#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_GAIN_TH_6_MASK (0x0000003Fu)
#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_GAIN_TH_6_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_MODE_3_MASK (0x00070000u)
#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_MODE_3_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_PEAKING_EN_B_MASK (0x01000000u)
#define CSL_VPS_EDE_CFG_EDE1_CFG_LTI_PEAKING_EN_B_SHIFT (0x00000018u)


/* cfg_ede2 */

#define CSL_VPS_EDE_CFG_EDE2_CFG_LTI_PEAKING_GAIN_X8_5_MASK (0x1F000000u)
#define CSL_VPS_EDE_CFG_EDE2_CFG_LTI_PEAKING_GAIN_X8_5_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE2_CFG_LTI_PEAKING_MAX_NEG_S9_MASK (0x000001FFu)
#define CSL_VPS_EDE_CFG_EDE2_CFG_LTI_PEAKING_MAX_NEG_S9_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE2_CFG_LTI_PEAKING_MAX_POS_8_MASK (0x00FF0000u)
#define CSL_VPS_EDE_CFG_EDE2_CFG_LTI_PEAKING_MAX_POS_8_SHIFT (0x00000010u)


/* cfg_ede3 */

#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_GAIN_MAX_X16_4_MASK (0x0F000000u)
#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_GAIN_MAX_X16_4_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_LENGTH_MAX_6_MASK (0x003F0000u)
#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_LENGTH_MAX_6_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_MODE_3_MASK (0x70000000u)
#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_MODE_3_SHIFT (0x0000001Cu)

#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_UNIFORM_Y_TH_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE3_CFG_CTI_UNIFORM_Y_TH_10_SHIFT (0x00000000u)


/* cfg_ede4 */

#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_EN_TRANS_ADJUST_B_MASK (0x10000000u)
#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_EN_TRANS_ADJUST_B_SHIFT (0x0000001Cu)

#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_GAIN_SLOPE_SHIFT_2_MASK (0x03000000u)
#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_GAIN_SLOPE_SHIFT_2_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_GAIN_TH_6_MASK (0x003F0000u)
#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_GAIN_TH_6_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_UNIFORM_REF_TH_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE4_CFG_CTI_UNIFORM_REF_TH_10_SHIFT (0x00000000u)


/* cfg_ede5 */

#define CSL_VPS_EDE_CFG_EDE5_CFG_CTI_ADJ_DECAY_SLOPE_SHIFT_2_MASK (0x00000030u)
#define CSL_VPS_EDE_CFG_EDE5_CFG_CTI_ADJ_DECAY_SLOPE_SHIFT_2_SHIFT (0x00000004u)

#define CSL_VPS_EDE_CFG_EDE5_CFG_CTI_ADJ_DECAY_TH_4_MASK (0x0000000Fu)
#define CSL_VPS_EDE_CFG_EDE5_CFG_CTI_ADJ_DECAY_TH_4_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_ALG_2_MASK (0x00000300u)
#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_ALG_2_SHIFT (0x00000008u)

#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_CLIP_SUPPRESSION_EN_B_MASK (0x00040000u)
#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_CLIP_SUPPRESSION_EN_B_SHIFT (0x00000012u)

#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_C_EN_B_MASK (0x00020000u)
#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_C_EN_B_SHIFT (0x00000011u)

#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_Y_EN_B_MASK (0x00010000u)
#define CSL_VPS_EDE_CFG_EDE5_CFG_PEAKING_Y_EN_B_SHIFT (0x00000010u)


/* cfg_ede6 */

#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN0_S8_MASK (0x000000FFu)
#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN0_S8_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN1_S8_MASK (0x0000FF00u)
#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN1_S8_SHIFT (0x00000008u)

#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN2_S8_MASK (0x00FF0000u)
#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN2_S8_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN3_S8_MASK (0xFF000000u)
#define CSL_VPS_EDE_CFG_EDE6_CFG_PEAKING_HPF_GAIN3_S8_SHIFT (0x00000018u)


/* cfg_ede7 */

#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN4_S8_MASK (0x000000FFu)
#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN4_S8_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN5_S8_MASK (0x0000FF00u)
#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN5_S8_SHIFT (0x00000008u)

#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN6_S8_MASK (0x00FF0000u)
#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN6_S8_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN7_S8_MASK (0xFF000000u)
#define CSL_VPS_EDE_CFG_EDE7_CFG_PEAKING_HPF_GAIN7_S8_SHIFT (0x00000018u)


/* cfg_ede8 */

#define CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_GAIN8_S8_MASK (0x00FF0000u)
#define CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_GAIN8_S8_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_NORMARIZE_SHIFT_3_MASK (0x07000000u)
#define CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_HPF_NORMARIZE_SHIFT_3_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_Y_TABLE_TH_P_LOW_S10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE8_CFG_PEAKING_Y_TABLE_TH_P_LOW_S10_SHIFT (0x00000000u)


/* cfg_ede9 */

#define CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_N_LOW_S10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_N_LOW_S10_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_P_HIGH_S10_MASK (0x03FF0000u)
#define CSL_VPS_EDE_CFG_EDE9_CFG_PEAKING_Y_TABLE_TH_P_HIGH_S10_SHIFT (0x00000010u)


/* cfg_ede10 */

#define CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_N_LOW_X16_8_MASK (0xFF000000u)
#define CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_N_LOW_X16_8_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_P_LOW_X16_8_MASK (0x00FF0000u)
#define CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_SLOPE_P_LOW_X16_8_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_TH_N_HIGH_S10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE10_CFG_PEAKING_Y_TABLE_TH_N_HIGH_S10_SHIFT (0x00000000u)


/* cfg_ede11 */

#define CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_V_DIFF_Y_MAX_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_V_DIFF_Y_MAX_10_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_N_HIGH_SHIFT_3_MASK (0x07000000u)
#define CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_N_HIGH_SHIFT_3_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_P_HIGH_SHIFT_3_MASK (0x00070000u)
#define CSL_VPS_EDE_CFG_EDE11_CFG_PEAKING_Y_TABLE_SLOPE_P_HIGH_SHIFT_3_SHIFT (0x00000010u)


/* cfg_ede12 */

#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_MAX_Y_X16_4_MASK (0x0000000Fu)
#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_MAX_Y_X16_4_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_SLOPE_Y_X16_4_MASK (0x000000F0u)
#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_EDGE_SLOPE_Y_X16_4_SHIFT (0x00000004u)

#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_HORIZONTAL_SLOPE_X16_5_MASK (0x1F000000u)
#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_HORIZONTAL_SLOPE_X16_5_SHIFT (0x00000018u)

#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_C_X8_6_MASK (0x003F0000u)
#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_C_X8_6_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_Y_X8_6_MASK (0x00003F00u)
#define CSL_VPS_EDE_CFG_EDE12_CFG_PEAKING_GAIN_PEAK_Y_X8_6_SHIFT (0x00000008u)


/* cfg_ede13 */

#define CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MAX_9_MASK (0x000001FFu)
#define CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MAX_9_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MIN_S10_MASK (0x03FF0000u)
#define CSL_VPS_EDE_CFG_EDE13_CFG_PEAKING_Y_MIN_S10_SHIFT (0x00000010u)


/* cfg_ede14 */

#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_N_X512_8_MASK (0x0000FF00u)
#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_N_X512_8_SHIFT (0x00000008u)

#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_P_X512_8_MASK (0x000000FFu)
#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_C_SLOPE_P_X512_8_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_GAIN_8_MASK (0x00FF0000u)
#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_GAIN_8_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_LIMIT_8_MASK (0xFF000000u)
#define CSL_VPS_EDE_CFG_EDE14_CFG_PEAKING_Y_TABLE_NEG_CORING_LIMIT_8_SHIFT (0x00000018u)


/* cfg_ede15 */

#define CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MAX_X512_11_MASK (0x000007FFu)
#define CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MAX_X512_11_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MIN_X512_11_MASK (0x07FF0000u)
#define CSL_VPS_EDE_CFG_EDE15_CFG_PEAKING_C_GAIN_MIN_X512_11_SHIFT (0x00000010u)


/* cfg_ede16 */

#define CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MAX_Y_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MAX_Y_10_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MIN_Y_10_MASK (0x03FF0000u)
#define CSL_VPS_EDE_CFG_EDE16_CFG_CLIP_MIN_Y_10_SHIFT (0x00000010u)


/* cfg_ede17 */

#define CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MAX_C_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MAX_C_10_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MIN_C_10_MASK (0x03FF0000u)
#define CSL_VPS_EDE_CFG_EDE17_CFG_CLIP_MIN_C_10_SHIFT (0x00000010u)


/* cfg_ede18 */

#define CSL_VPS_EDE_CFG_EDE18_CFG_BYP_MASK (0x80000000u)
#define CSL_VPS_EDE_CFG_EDE18_CFG_BYP_SHIFT (0x0000001Fu)

#define CSL_VPS_EDE_CFG_EDE18_CFG_VEMO_MASK (0x30000000u)
#define CSL_VPS_EDE_CFG_EDE18_CFG_VEMO_SHIFT (0x0000001Cu)

#define CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_H_12_MASK (0x00000FFFu)
#define CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_H_12_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_V_12_MASK (0x0FFF0000u)
#define CSL_VPS_EDE_CFG_EDE18_CFG_SIZE_V_12_SHIFT (0x00000010u)


/* cfg_ede19 */

#define CSL_VPS_EDE_CFG_EDE19_CFG_LTI_MAX_CORING_THR_10_MASK (0x03FF0000u)
#define CSL_VPS_EDE_CFG_EDE19_CFG_LTI_MAX_CORING_THR_10_SHIFT (0x00000010u)

#define CSL_VPS_EDE_CFG_EDE19_CFG_LTI_MIN_CORING_THR_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE19_CFG_LTI_MIN_CORING_THR_10_SHIFT (0x00000000u)


/* cfg_ede20 */

#define CSL_VPS_EDE_CFG_EDE20_CFG_CTI_CORING_THR_10_MASK (0x000003FFu)
#define CSL_VPS_EDE_CFG_EDE20_CFG_CTI_CORING_THR_10_SHIFT (0x00000000u)


/* cfg_ede21 */

#define CSL_VPS_EDE_CFG_EDE21_CFG_PEAKING_CLIP_SUPPRESSION_GAIN_THR_10_MASK (0x00003FF0u)
#define CSL_VPS_EDE_CFG_EDE21_CFG_PEAKING_CLIP_SUPPRESSION_GAIN_THR_10_SHIFT (0x00000004u)

#define CSL_VPS_EDE_CFG_EDE21_CFG_PEAKING_CLIP_SUPPRESSION_SHIFT_4_MASK (0x0000000Fu)
#define CSL_VPS_EDE_CFG_EDE21_CFG_PEAKING_CLIP_SUPPRESSION_SHIFT_4_SHIFT (0x00000000u)

#define CSL_VPS_EDE_CFG_EDE21_CFG_PEAKING_MIN_SKIN_TONE_SUPPRESSION_GAIN_5_MASK (0x001F0000u)
#define CSL_VPS_EDE_CFG_EDE21_CFG_PEAKING_MIN_SKIN_TONE_SUPPRESSION_GAIN_5_SHIFT (0x00000010u)


#endif