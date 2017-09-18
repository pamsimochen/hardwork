/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file isp_common_reg.h
*
*  This is the CSL register file for ISP5 configuration
*
* @path Centaurus\drivers\drv_isp\inc\csl
*
* @rev  1.0
*/
/*========================================================================
*!
*! Revision History
*! 
*========================================================================= */

#ifndef ISP_REG_COMMON
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define ISP_REG_COMMON

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "../../isp5_utils/isp5_sys_types.h"
#include "../../isp5_utils/isp5_csl_utils.h"

/* ================================================================ */
/* 
 *================================================================== */
typedef struct {

    volatile uint32 ISP5_REVISION;
    volatile uint32 ISP5_HWINFO1;
    volatile uint32 ISP5_HWINFO2;
    volatile uint32 rsrvd1;
    volatile uint32 ISP5_SYSCONFIG;
    volatile uint32 rsrvd2[3];
    volatile uint32 ISP5_IRQ_EOI;

    volatile uint32 ISP5_IRQSTATUS_RAW_0;                  /* Bank 0 register 
                                                            * set */
    volatile uint32 ISP5_IRQSTATUS_0;
    volatile uint32 ISP5_IRQENABLE_SET_0;
    volatile uint32 ISP5_IRQENABLE_CLR_0;

    volatile uint32 ISP5_IRQSTATUS_RAW_1;                  /* Bank 1 register 
                                                            * set */
    volatile uint32 ISP5_IRQSTATUS_1;
    volatile uint32 ISP5_IRQENABLE_SET_1;
    volatile uint32 ISP5_IRQENABLE_CLR_1;

    volatile uint32 ISP5_IRQSTATUS_RAW_2;                  /* Bank 2 register 
                                                            * set */
    volatile uint32 ISP5_IRQSTATUS_2;
    volatile uint32 ISP5_IRQENABLE_SET_2;
    volatile uint32 ISP5_IRQENABLE_CLR_2;

    volatile uint32 ISP5_IRQSTATUS_RAW_3;                  /* Bank 3 register 
                                                            * set */
    volatile uint32 ISP5_IRQSTATUS_3;
    volatile uint32 ISP5_IRQENABLE_SET_3;
    volatile uint32 ISP5_IRQENABLE_CLR_3;

    volatile uint32 ISP5_DMAENABLE_SET;
    volatile uint32 ISP5_DMAENABLE_CLR;
    volatile uint32 ISP5_CTRL;
    volatile uint32 ISP5_PG;
    volatile uint32 ISP5_PG_PULSE_CTRL;
    volatile uint32 ISP5_PG_FRAME_SIZE;
    volatile uint32 ISP5_MPSR;
    volatile uint32 ISP5_BL_MTC_1;
    volatile uint32 ISP5_BL_MTC_2;

} CSL_ISP5Regs;

/* ================================================================ */
/* 
 *================================================================== */

#define CSL_ISP5_CTRL_MSTANDBY_SHIFT 24
#define CSL_ISP5_CTRL_MSTANDBY_MASK ((0x00000001)<<CSL_ISP5_CTRL_MSTANDBY_SHIFT)

/* added for ES2.0 */
#define CSL_ISP5_CTRL_MSTANDBY_WAIT_SHIFT 20
#define CSL_ISP5_CTRL_MSTANDBY_WAIT_MASK ((0x00000001)<<CSL_ISP5_CTRL_MSTANDBY_WAIT_SHIFT)

#define CSL_ISP5_CTRL_VD_PULSE_EXT_SHIFT 23
#define CSL_ISP5_CTRL_VD_PULSE_EXT_MASK ((0x00000001)<<CSL_ISP5_CTRL_VD_PULSE_EXT_SHIFT)

#define CSL_ISP5_CTRL_PCLK_INV_SHIFT 22
#define CSL_ISP5_CTRL_PCLK_INV_MASK ((0x00000001)<<CSL_ISP5_CTRL_PCLK_INV_SHIFT)

#define CSL_ISP5_CTRL_SYNC_EN_SHIFT 9
#define CSL_ISP5_CTRL_SYNC_EN_MASK  ((0x00000001)<<CSL_ISP5_CTRL_SYNC_EN_SHIFT)

#define CSL_ISP5_CTRL_PSYNC_CLK_SEL_SHIFT 8
#define CSL_ISP5_CTRL_PSYNC_CLK_SEL_MASK  ((0x00000001)<<CSL_ISP5_CTRL_PSYNC_CLK_SEL_SHIFT)

#define CSL_ISP5_SYSCONFIG_RESET_SHIFT 1
#define CSL_ISP5_SYSCONFIG_RESET_MASK ((0x00000001)<< CSL_ISP5_SYSCONFIG_RESET_SHIFT)

/* Added for ES2.0 */
#define CSL_ISP5_SYSCONFIG_STANDBYMODE_SHIFT 4
#define CSL_ISP5_SYSCONFIG_STANDBYMODE_MASK (0x00000030)

#define CSL_ISP5_CTRL_OCP_WRNP_SHIFT 0
#define CSL_ISP5_CTRL_OCP_WRNP_MASK ((0x00000001)<< CSL_ISP5_CTRL_OCP_WRNP_SHIFT)

#define CSL_ISP5_PG_SRC_SEL_SHIFT 4
#define CSL_ISP5_PG_SRC_SEL_MASK 0x00000030

#define CSL_ISP5_MPSR_TABLE_CFG_SHIFT 1
#define CSL_ISP5_MPSR_TABLE_CFG_MASK 0x001FFFFE

#define CSL_ISP5_BL_MTC1_ISIF_R_SHIFT 16
#define CSL_ISP5_BL_MTC1_ISIF_R_MASK 0xFFFF0000

#define CSL_ISP5_BL_MTC1_IPIPEIF_R_SHIFT 0
#define CSL_ISP5_BL_MTC1_IPIPEIF_R_MASK 0x0000FFFF

#define CSL_ISP5_BL_MTC2_H3A_W_SHIFT 16
#define CSL_ISP5_BL_MTC2_H3A_W_MASK 0xFFFF0000

#define CSL_ISP5_CTRL_VBUSM_CIDS_SHIFT  4
#define CSL_ISP5_CTRL_VBUSM_CIDS_MASK 0x000000F0

#define CSL_ISP5_CTRL_VBUSM_CPRIORITY_SHIFT  1
#define CSL_ISP5_CTRL_VBUSM_CPRIORITY_MASK 0x0000000E

#define CSL_ISP5_MPSR_IPIPE_GAMMA_RGB_COPY_SHIFT 24
#define CSL_ISP5_MPSR_IPIPE_GAMMA_RGB_COPY_MASK 0x01000000

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
