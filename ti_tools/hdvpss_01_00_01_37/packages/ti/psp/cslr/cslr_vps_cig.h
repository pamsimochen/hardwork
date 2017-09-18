/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_cig.h
*
* Brief: This file contains the Register Description for vps_cig
*
*********************************************************************/
#ifndef _CSLR_VPS_CIG_H_
#define _CSLR_VPS_CIG_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 CIG_REG0;
    volatile Uint32 CIG_REG1;
    volatile Uint32 CIG_REG2;
    volatile Uint32 CIG_REG3;
    volatile Uint32 CIG_REG4;
    volatile Uint32 CIG_REG5;
    volatile Uint32 CIG_REG6;
    volatile Uint32 CIG_REG7;
    volatile Uint32 CIG_REG8;
    volatile Uint32 CIG_REG9;
    volatile Uint32 CIG_REG10;
    volatile Uint32 CIG_REG11;
} CSL_Vps_cigRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_cigRegs            *CSL_VpsCigRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* cig_reg0 */

#define CSL_VPS_CIG_CIG_REG0_CIG_EN_MASK (0x00000001u)
#define CSL_VPS_CIG_CIG_REG0_CIG_EN_SHIFT (0x00000000u)

#define CSL_VPS_CIG_CIG_REG0_CI_FIELD_RPT_EN_MASK (0x00000010u)
#define CSL_VPS_CIG_CIG_REG0_CI_FIELD_RPT_EN_SHIFT (0x00000004u)

#define CSL_VPS_CIG_CIG_REG0_CI_P2I_EN_MASK (0x00000004u)
#define CSL_VPS_CIG_CIG_REG0_CI_P2I_EN_SHIFT (0x00000002u)

#define CSL_VPS_CIG_CIG_REG0_CI_VDEC_EN_MASK (0x00000008u)
#define CSL_VPS_CIG_CIG_REG0_CI_VDEC_EN_SHIFT (0x00000003u)

#define CSL_VPS_CIG_CIG_REG0_P2I_EN_MASK (0x00000002u)
#define CSL_VPS_CIG_CIG_REG0_P2I_EN_SHIFT (0x00000001u)

#define CSL_VPS_CIG_CIG_REG0_PIP_EN_MASK (0x00000020u)
#define CSL_VPS_CIG_CIG_REG0_PIP_EN_SHIFT (0x00000005u)

#define CSL_VPS_CIG_CIG_REG0_PIP_FULLSIZE_MASK (0x00000040u)
#define CSL_VPS_CIG_CIG_REG0_PIP_FULLSIZE_SHIFT (0x00000006u)

#define CSL_VPS_CIG_CIG_REG0_PIP_P2I_EN_MASK (0x00000080u)
#define CSL_VPS_CIG_CIG_REG0_PIP_P2I_EN_SHIFT (0x00000007u)


/* cig_reg1 */

#define CSL_VPS_CIG_CIG_REG1_DISP_H_MASK (0x000007FFu)
#define CSL_VPS_CIG_CIG_REG1_DISP_H_SHIFT (0x00000000u)

#define CSL_VPS_CIG_CIG_REG1_DISP_W_MASK (0x07FF0000u)
#define CSL_VPS_CIG_CIG_REG1_DISP_W_SHIFT (0x00000010u)


/* cig_reg2 */

#define CSL_VPS_CIG_CIG_REG2_BL_ENABLE_MASK (0x00000008u)
#define CSL_VPS_CIG_CIG_REG2_BL_ENABLE_SHIFT (0x00000003u)

#define CSL_VPS_CIG_CIG_REG2_BL_LEVEL_MASK (0x00000FF0u)
#define CSL_VPS_CIG_CIG_REG2_BL_LEVEL_SHIFT (0x00000004u)

#define CSL_VPS_CIG_CIG_REG2_TR_ENABLE_MASK (0x00000001u)
#define CSL_VPS_CIG_CIG_REG2_TR_ENABLE_SHIFT (0x00000000u)

#define CSL_VPS_CIG_CIG_REG2_TR_MODE_MASK_MASK (0x00000006u)
#define CSL_VPS_CIG_CIG_REG2_TR_MODE_MASK_SHIFT (0x00000001u)


/* cig_reg3 */

#define CSL_VPS_CIG_CIG_REG3_TR_COLOR_MASK (0x00FFFFFFu)
#define CSL_VPS_CIG_CIG_REG3_TR_COLOR_SHIFT (0x00000000u)


/* cig_reg4 */

#define CSL_VPS_CIG_CIG_REG4_BL_ENABLE_MASK (0x00000008u)
#define CSL_VPS_CIG_CIG_REG4_BL_ENABLE_SHIFT (0x00000003u)

#define CSL_VPS_CIG_CIG_REG4_BL_LEVEL_MASK (0x00000FF0u)
#define CSL_VPS_CIG_CIG_REG4_BL_LEVEL_SHIFT (0x00000004u)

#define CSL_VPS_CIG_CIG_REG4_TR_ENABLE_MASK (0x00000001u)
#define CSL_VPS_CIG_CIG_REG4_TR_ENABLE_SHIFT (0x00000000u)

#define CSL_VPS_CIG_CIG_REG4_TR_MODE_MASK_MASK (0x00000006u)
#define CSL_VPS_CIG_CIG_REG4_TR_MODE_MASK_SHIFT (0x00000001u)


/* cig_reg5 */

#define CSL_VPS_CIG_CIG_REG5_TR_COLOR_MASK (0x00FFFFFFu)
#define CSL_VPS_CIG_CIG_REG5_TR_COLOR_SHIFT (0x00000000u)


/* cig_reg6 */

#define CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_MASK (0x000007FFu)
#define CSL_VPS_CIG_CIG_REG6_PIP_DISP_H_SHIFT (0x00000000u)

#define CSL_VPS_CIG_CIG_REG6_PIP_DISP_W_MASK (0x07FF0000u)
#define CSL_VPS_CIG_CIG_REG6_PIP_DISP_W_SHIFT (0x00000010u)


/* cig_reg7 */

#define CSL_VPS_CIG_CIG_REG7_PIP_X_MASK  (0x07FF0000u)
#define CSL_VPS_CIG_CIG_REG7_PIP_X_SHIFT (0x00000010u)

#define CSL_VPS_CIG_CIG_REG7_PIP_Y_MASK  (0x000007FFu)
#define CSL_VPS_CIG_CIG_REG7_PIP_Y_SHIFT (0x00000000u)


/* cig_reg8 */

#define CSL_VPS_CIG_CIG_REG8_PIP_H_MASK  (0x000007FFu)
#define CSL_VPS_CIG_CIG_REG8_PIP_H_SHIFT (0x00000000u)

#define CSL_VPS_CIG_CIG_REG8_PIP_W_MASK  (0x07FF0000u)
#define CSL_VPS_CIG_CIG_REG8_PIP_W_SHIFT (0x00000010u)


/* cig_reg9 */

#define CSL_VPS_CIG_CIG_REG9_BL_ENABLE_MASK (0x00000008u)
#define CSL_VPS_CIG_CIG_REG9_BL_ENABLE_SHIFT (0x00000003u)

#define CSL_VPS_CIG_CIG_REG9_BL_LEVEL_MASK (0x00000FF0u)
#define CSL_VPS_CIG_CIG_REG9_BL_LEVEL_SHIFT (0x00000004u)

#define CSL_VPS_CIG_CIG_REG9_TR_ENABLE_MASK (0x00000001u)
#define CSL_VPS_CIG_CIG_REG9_TR_ENABLE_SHIFT (0x00000000u)

#define CSL_VPS_CIG_CIG_REG9_TR_MODE_MASK_MASK (0x00000006u)
#define CSL_VPS_CIG_CIG_REG9_TR_MODE_MASK_SHIFT (0x00000001u)


/* cig_reg10 */

#define CSL_VPS_CIG_CIG_REG10_TR_COLOR_MASK (0x00FFFFFFu)
#define CSL_VPS_CIG_CIG_REG10_TR_COLOR_SHIFT (0x00000000u)


/* cig_reg11 */

#define CSL_VPS_CIG_CIG_REG11_CIG_STATUS_MASK (0x000000FFu)
#define CSL_VPS_CIG_CIG_REG11_CIG_STATUS_SHIFT (0x00000000u)


#endif
