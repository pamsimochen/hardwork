/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_nf.h
*
* Brief: This file contains the Register Description for vps_nf
*
*********************************************************************/
#ifndef _CSLR_VPS_NF_H_
#define _CSLR_VPS_NF_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 NF_REG0;
    volatile Uint32 NF_REG1;
    volatile Uint32 NF_REG2;
    volatile Uint32 NF_REG3;
    volatile Uint32 NF_REG4;
    volatile Uint32 NF_REG5;
    volatile Uint32 NF_REG6;
    volatile Uint32 NF_REG7;
    volatile Uint32 NF_REG8;
    volatile Uint32 NF_REG9;
} CSL_Vps_nfRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_nfRegs           *CSL_VpsNfRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* nf_reg0 */

#define CSL_VPS_NF_NF_REG0_NF_BYPASS_CFG_MASK (0x00000600u)
#define CSL_VPS_NF_NF_REG0_NF_BYPASS_CFG_SHIFT (0x00000009u)

#define CSL_VPS_NF_NF_REG0_NF_EN_MASK    (0x00000001u)
#define CSL_VPS_NF_NF_REG0_NF_EN_SHIFT   (0x00000000u)

#define CSL_VPS_NF_NF_REG0_NF_LOAD_FRAME_NOISE_MASK (0x00000800u)
#define CSL_VPS_NF_NF_REG0_NF_LOAD_FRAME_NOISE_SHIFT (0x0000000Bu)

#define CSL_VPS_NF_NF_REG0_NF_REF_CFG_MASK (0x00000006u)
#define CSL_VPS_NF_NF_REG0_NF_REF_CFG_SHIFT (0x00000001u)

#define CSL_VPS_NF_NF_REG0_NF_UPDATE_FRAME_NOISE_MASK (0x00001000u)
#define CSL_VPS_NF_NF_REG0_NF_UPDATE_FRAME_NOISE_SHIFT (0x0000000Cu)

#define CSL_VPS_NF_NF_REG0_NF_VIDEO_INDEX_MASK (0x000001F0u)
#define CSL_VPS_NF_NF_REG0_NF_VIDEO_INDEX_SHIFT (0x00000004u)


/* nf_reg1 */

#define CSL_VPS_NF_NF_REG1_HEIGHT_MASK   (0x07FF0000u)
#define CSL_VPS_NF_NF_REG1_HEIGHT_SHIFT  (0x00000010u)

#define CSL_VPS_NF_NF_REG1_WIDTH_MASK    (0x000007FFu)
#define CSL_VPS_NF_NF_REG1_WIDTH_SHIFT   (0x00000000u)


/* nf_reg2 */

#define CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_HIGH_MASK (0x0000FF00u)
#define CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_HIGH_SHIFT (0x00000008u)

#define CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_LOW_MASK (0x000000FFu)
#define CSL_VPS_NF_NF_REG2_SPATIAL_STRENGTH_Y_LOW_SHIFT (0x00000000u)


/* nf_reg3 */

#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_HIGH_MASK (0x0000FF00u)
#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_HIGH_SHIFT (0x00000008u)

#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_LOW_MASK (0x000000FFu)
#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_U_LOW_SHIFT (0x00000000u)

#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_HIGH_MASK (0xFF000000u)
#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_HIGH_SHIFT (0x00000018u)

#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_LOW_MASK (0x00FF0000u)
#define CSL_VPS_NF_NF_REG3_SPATIAL_STRENGTH_V_LOW_SHIFT (0x00000010u)


/* nf_reg4 */

#define CSL_VPS_NF_NF_REG4_TEMPORAL_FILTER_TRIGGER_NOISE_MASK (0x00000F00u)
#define CSL_VPS_NF_NF_REG4_TEMPORAL_FILTER_TRIGGER_NOISE_SHIFT (0x00000008u)

#define CSL_VPS_NF_NF_REG4_TEMPORAL_STRENGTH_MASK (0x0000003Fu)
#define CSL_VPS_NF_NF_REG4_TEMPORAL_STRENGTH_SHIFT (0x00000000u)


/* nf_reg5 */

#define CSL_VPS_NF_NF_REG5_MAX_NOISE_MASK (0x000001F0u)
#define CSL_VPS_NF_NF_REG5_MAX_NOISE_SHIFT (0x00000004u)

#define CSL_VPS_NF_NF_REG5_NOISE_IIR_COEFFICIENT_MASK (0x0000000Fu)
#define CSL_VPS_NF_NF_REG5_NOISE_IIR_COEFFICIENT_SHIFT (0x00000000u)


/* nf_reg6 */

#define CSL_VPS_NF_NF_REG6_PURE_BLACK_THRESHOLD_MASK (0x0000003Fu)
#define CSL_VPS_NF_NF_REG6_PURE_BLACK_THRESHOLD_SHIFT (0x00000000u)

#define CSL_VPS_NF_NF_REG6_PURE_WHITE_THRESHOLD_MASK (0x00003F00u)
#define CSL_VPS_NF_NF_REG6_PURE_WHITE_THRESHOLD_SHIFT (0x00000008u)


/* nf_reg7 */

#define CSL_VPS_NF_NF_REG7_FRAME_NOISE_READ_INDEX_MASK (0x0000001Fu)
#define CSL_VPS_NF_NF_REG7_FRAME_NOISE_READ_INDEX_SHIFT (0x00000000u)


/* nf_reg8 */

#define CSL_VPS_NF_NF_REG8_FRAME_NOISE_Y_MASK (0x00001FFFu)
#define CSL_VPS_NF_NF_REG8_FRAME_NOISE_Y_SHIFT (0x00000000u)


/* nf_reg9 */

#define CSL_VPS_NF_NF_REG9_FRAME_NOISE_U_MASK (0x1FFF0000u)
#define CSL_VPS_NF_NF_REG9_FRAME_NOISE_U_SHIFT (0x00000010u)

#define CSL_VPS_NF_NF_REG9_FRAME_NOISE_V_MASK (0x00001FFFu)
#define CSL_VPS_NF_NF_REG9_FRAME_NOISE_V_SHIFT (0x00000000u)


#endif
