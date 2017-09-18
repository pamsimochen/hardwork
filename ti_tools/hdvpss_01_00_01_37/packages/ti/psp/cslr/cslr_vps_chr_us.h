/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_chr_us.h
*
* Brief: This file contains the Register Description for vps_chr_us
*
*********************************************************************/
#ifndef _CSLR_VPS_CHR_US_H_
#define _CSLR_VPS_CHR_US_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 PID;
    volatile Uint32 REG[8];
} CSL_Vps_chr_usRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_chr_usRegs             *CSL_VpsChrusRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* pid */

#define CSL_VPS_CHR_US_PID_PID_MASK      (0xFFFFFFFFu)
#define CSL_VPS_CHR_US_PID_PID_SHIFT     (0x00000000u)

/* reg0 */

#define CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C0_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C0_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C1_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG0_ANCHOR_FID0_C1_SHIFT (0x00000002u)

#define CSL_VPS_CHR_US_REG0_CFG_MODE_MASK (0x00030000u)
#define CSL_VPS_CHR_US_REG0_CFG_MODE_SHIFT (0x00000010u)


/* reg1 */

#define CSL_VPS_CHR_US_REG1_ANCHOR_FID0_C2_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG1_ANCHOR_FID0_C2_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG1_ANCHOR_FID0_C3_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG1_ANCHOR_FID0_C3_SHIFT (0x00000002u)


/* reg2 */

#define CSL_VPS_CHR_US_REG2_INTERP_FID0_C0_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG2_INTERP_FID0_C0_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG2_INTERP_FID0_C1_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG2_INTERP_FID0_C1_SHIFT (0x00000002u)


/* reg3 */

#define CSL_VPS_CHR_US_REG3_INTERP_FID0_C2_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG3_INTERP_FID0_C2_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG3_INTERP_FID0_C3_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG3_INTERP_FID0_C3_SHIFT (0x00000002u)


/* reg4 */

#define CSL_VPS_CHR_US_REG4_ANCHOR_FID1_C0_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG4_ANCHOR_FID1_C0_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG4_ANCHOR_FID1_C1_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG4_ANCHOR_FID1_C1_SHIFT (0x00000002u)


/* reg5 */

#define CSL_VPS_CHR_US_REG5_ANCHOR_FID1_C2_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG5_ANCHOR_FID1_C2_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG5_ANCHOR_FID1_C3_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG5_ANCHOR_FID1_C3_SHIFT (0x00000002u)


/* reg6 */

#define CSL_VPS_CHR_US_REG6_INTERP_FID1_C0_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG6_INTERP_FID1_C0_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG6_INTERP_FID1_C1_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG6_INTERP_FID1_C1_SHIFT (0x00000002u)


/* reg7 */

#define CSL_VPS_CHR_US_REG7_INTERP_FID1_C2_MASK (0xFFFC0000u)
#define CSL_VPS_CHR_US_REG7_INTERP_FID1_C2_SHIFT (0x00000012u)

#define CSL_VPS_CHR_US_REG7_INTERP_FID1_C3_MASK (0x0000FFFCu)
#define CSL_VPS_CHR_US_REG7_INTERP_FID1_C3_SHIFT (0x00000002u)


#endif
