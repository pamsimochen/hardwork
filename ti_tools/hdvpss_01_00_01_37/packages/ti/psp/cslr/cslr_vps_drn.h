/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_drn.h
*
* Brief: This file contains the Register Description for vps_drn
*
*********************************************************************/
#ifndef _CSLR_VPS_DRN_H_
#define _CSLR_VPS_DRN_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 DRN_CTRL;
    volatile Uint32 DRN_SIZE;
    volatile Uint32 THR_CFG0;
    volatile Uint32 THR_CFG1;
    volatile Uint32 THR_CFG2;
    volatile Uint32 THR_CFG3;
    volatile Uint32 THR_CFG4;
} CSL_Vps_drnRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_drnRegs            *CSL_VpsDrnRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* drn_ctrl */

#define CSL_VPS_DRN_DRN_CTRL_CFG_EN_DRN_MASK (0x00000001u)
#define CSL_VPS_DRN_DRN_CTRL_CFG_EN_DRN_SHIFT (0x00000000u)

#define CSL_VPS_DRN_DRN_CTRL_CFG_EN_EDGE_BLCK_HOR_DILATION_MASK (0x00000100u)
#define CSL_VPS_DRN_DRN_CTRL_CFG_EN_EDGE_BLCK_HOR_DILATION_SHIFT (0x00000008u)

#define CSL_VPS_DRN_DRN_CTRL_CFG_PROGRESSIVE_MASK (0x00000010u)
#define CSL_VPS_DRN_DRN_CTRL_CFG_PROGRESSIVE_SHIFT (0x00000004u)


/* drn_size */

#define CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_MASK (0x07FF0000u)
#define CSL_VPS_DRN_DRN_SIZE_CFG_HEIGHT_SHIFT (0x00000010u)

#define CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_MASK (0x000007FFu)
#define CSL_VPS_DRN_DRN_SIZE_CFG_WIDTH_SHIFT (0x00000000u)


/* thr_cfg0 */

#define CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_MASK (0x000000FFu)
#define CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_SHIFT (0x00000000u)

#define CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_STRONG_MASK (0x0000FF00u)
#define CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_EDGE_STRONG_SHIFT (0x00000008u)

#define CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_FLATPIX_BLCK_MASK (0x00FF0000u)
#define CSL_VPS_DRN_THR_CFG0_CFG_THR_NUM_FLATPIX_BLCK_SHIFT (0x00000010u)


/* thr_cfg1 */

#define CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_STRONGEDGE_MASK (0x00000FFFu)
#define CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_STRONGEDGE_SHIFT (0x00000000u)

#define CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_WEAKEDGE_MASK (0x0FFF0000u)
#define CSL_VPS_DRN_THR_CFG1_CFG_THR_GSUM_WEAKEDGE_SHIFT (0x00000010u)


/* thr_cfg2 */

#define CSL_VPS_DRN_THR_CFG2_CFG_THR_GSUM_FLAT_MASK (0x0FFF0000u)
#define CSL_VPS_DRN_THR_CFG2_CFG_THR_GSUM_FLAT_SHIFT (0x00000010u)

#define CSL_VPS_DRN_THR_CFG2_CFG_THR_SAD_VERYFLAT_MASK (0x00000FFFu)
#define CSL_VPS_DRN_THR_CFG2_CFG_THR_SAD_VERYFLAT_SHIFT (0x00000000u)


/* thr_cfg3 */

#define CSL_VPS_DRN_THR_CFG3_CFG_DRN_BLD_COEFF_QP_MASK (0x003F0000u)
#define CSL_VPS_DRN_THR_CFG3_CFG_DRN_BLD_COEFF_QP_SHIFT (0x00000010u)

#define CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_BLD_FACTOR_MASK (0x0000007Fu)
#define CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_BLD_FACTOR_SHIFT (0x00000000u)

#define CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_SIGMA_THR1_MASK (0x00007F00u)
#define CSL_VPS_DRN_THR_CFG3_CFG_DRN_MAX_SIGMA_THR1_SHIFT (0x00000008u)

#define CSL_VPS_DRN_THR_CFG3_CFG_MPEG_Q_MASK (0xFF000000u)
#define CSL_VPS_DRN_THR_CFG3_CFG_MPEG_Q_SHIFT (0x00000018u)


/* thr_cfg4 */

#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_BLOCK_EDGE_MASK (0x0000003Fu)
#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_BLOCK_EDGE_SHIFT (0x00000000u)

#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_LOCAL_EDGE_MASK (0x00003F00u)
#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_COEFF_LOCAL_EDGE_SHIFT (0x00000008u)

#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_STRGEDGE_MASK (0x3F000000u)
#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_STRGEDGE_SHIFT (0x00000018u)

#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_WEAKEDGE_MASK (0x003F0000u)
#define CSL_VPS_DRN_THR_CFG4_CFG_DRN_BLD_SCALE_WEAKEDGE_SHIFT (0x00000010u)


#endif
