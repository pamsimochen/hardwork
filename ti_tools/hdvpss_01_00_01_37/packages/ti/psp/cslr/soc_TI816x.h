/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _SOC_TI816X_H_
#define _SOC_TI816X_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************\
* TI816x soc file
\**************************************************************************/

/******************************************************************************\
* Static inline definition
\******************************************************************************/
#ifndef CSL_IDEF_INLINE
#define CSL_IDEF_INLINE static inline
#endif

/******************************************************************************\
* Peripheral Instance count
\******************************************************************************/

/** @brief Number of VPS CHRUS instances                                      */
#define CSL_VPS_CHRUS_PER_CNT               (4u)

/** @brief Number of VPS CIG instances                                        */
#define CSL_VPS_CIG_PER_CNT                 (1u)

/** @brief Number of VPS COMP instances                                       */
#define CSL_VPS_COMP_PER_CNT                (1u)

/** @brief Number of VPS COMPR instances                                      */
#define CSL_VPS_COMPR_PER_CNT               (3u)

/** @brief Number of VPS DCOMPR instances                                     */
#define CSL_VPS_DCOMPR_PER_CNT              (5u)

/** @brief Number of VPS CSC instances                                        */
#define CSL_VPS_CSC_PER_CNT                 (5u)

/** @brief Number of VPS DEIH instances                                       */
#define CSL_VPS_DEI_HQ_PER_CNT              (1u)

/** @brief Number of VPS DEI instances                                        */
#define CSL_VPS_DEI_PER_CNT                 (1u)

/** @brief Number of VPS DRN instances                                        */
#define CSL_VPS_DRN_PER_CNT                 (2u)

/** @brief Number of VPS GRPX instances */
#define CSL_VPS_GRPX_PER_CNT                (3u)

/** @brief Number of VPS instances                                            */
#define CSL_VPS_PER_CNT                     (1u)

/** @brief Number of VPS EDE instances                                        */
#define CSL_VPS_EDE_PER_CNT                 (1u)

/** @brief Number of VPS HDVENC instances                                     */
#define CSL_VPS_HD_VENC_PER_CNT             (3u)

/** @brief Number of VPS NF instances                                         */
#define CSL_VPS_NF_PER_CNT                  (1u)

/** @brief Number of VPS SC instances                                         */
#define CSL_VPS_SC_PER_CNT                  (5u)

/** @brief Number of VPS SDVENC instances                                     */
#define CSL_VPS_SD_VENC_PER_CNT             (1u)

/** @brief Number of VPS VCOMP instances                                      */
#define CSL_VPS_VCOMP_PER_CNT               (1u)

/** @brief Number of VPS CPROC instances                                      */
#define CSL_VPS_CPROC_PER_CNT               (1u)

/** @brief Number of VPS VIP instances                                        */
#define CSL_VPS_VIP_PER_CNT                 (2u)

/** @brief Number of VPS VIP-PORT instances                                   */
#define CSL_VPS_VIP_PORT_PER_CNT            (4u)

/** @brief Number of VPS VPDMA instances                                      */
#define CSL_VPS_VPDMA_PER_CNT               (1u)

/**< @brief Number of VPS RF Modulator instances                              */
#define CSL_VPS_RF_PER_CNT                  (1u)

/*******************************************************************************
* Peripheral Base Address
*******************************************************************************/

/* This is MMPU mapped address.  Currently its one to one mapped */

#define CSL_TI816x_BASE                  (0x48000000u)

/** \brief HDVPSS Base Address */
#define CSL_TI816x_VPS_BASE              (CSL_TI816x_BASE + 0x00100000u)
/** \brief Control module base address */
#define CSL_TI816x_CTRL_MODULE_BASE      (CSL_TI816x_BASE + 0x00140000u)
/** \brief PRCM Base Address */
#define CSL_TI816x_PRCM_BASE             (CSL_TI816x_BASE + 0x00180000u)
/** \brief GPIO Base Address */
#define CSL_TI816x_GPIO0_BASE             (CSL_TI816x_BASE + 0x00032000u)
/** \brief I2C Base Address */
#define CSL_TI816x_I2C0_BASE             (CSL_TI816x_BASE + 0x00028000u)
#define CSL_TI816x_I2C1_BASE             (CSL_TI816x_BASE + 0x0002A000u)

#define CSL_VPS_INTC_0_REGS             (CSL_TI816x_VPS_BASE + 0x00000000u)
#define CSL_VPS_CLKC_0_REGS             (CSL_TI816x_VPS_BASE + 0x00000100u)
#define CSL_VPS_DFT_CONTROL_0_REGS      (CSL_TI816x_VPS_BASE + 0x00000200u)
#define CSL_VPS_CHR_US_PRI0_0_REGS      (CSL_TI816x_VPS_BASE + 0x00000300u)
#define CSL_VPS_DRN_PRI_0_REGS          (CSL_TI816x_VPS_BASE + 0x00000400u)
#define CSL_VPS_DEI_HQ_0_REGS           (CSL_TI816x_VPS_BASE + 0x00000500u)
#define CSL_VPS_SC_1_REGS               (CSL_TI816x_VPS_BASE + 0x00000600u)
#define CSL_VPS_CHR_US_AUX_0_REGS       (CSL_TI816x_VPS_BASE + 0x00000700u)
#define CSL_VPS_DRN_AUX_0_REGS          (CSL_TI816x_VPS_BASE + 0x00000800u)
#define CSL_VPS_DEI_0_REGS              (CSL_TI816x_VPS_BASE + 0x00000A00u)
#define CSL_VPS_SC_2_REGS               (CSL_TI816x_VPS_BASE + 0x00000B00u)
#define CSL_VPS_CSC_HD1_0_REGS          (CSL_TI816x_VPS_BASE + 0x00000C00u)
#define CSL_VPS_CSC_SD_0_REGS           (CSL_TI816x_VPS_BASE + 0x00000D00u)
#define CSL_VPS_VCOMP_0_REGS            (CSL_TI816x_VPS_BASE + 0x00000E00u)
#define CSL_VPS_EDE_0_REGS              (CSL_TI816x_VPS_BASE + 0x00000F00u)
#define CSL_VPS_CPROC_0_REGS            (CSL_TI816x_VPS_BASE + 0x00001000u)
#define CSL_VPS_SC_5_REGS               (CSL_TI816x_VPS_BASE + 0x00005000u)
#define CSL_VPS_CIG_0_REGS              (CSL_TI816x_VPS_BASE + 0x00005100u)
#define CSL_VPS_COMP_0_REGS             (CSL_TI816x_VPS_BASE + 0x00005200u)
#define CSL_VPS_CSC_WB2_0_REGS          (CSL_TI816x_VPS_BASE + 0x00005300u)
#define CSL_VPS_CHR_US_SEC0_0_REGS      (CSL_TI816x_VPS_BASE + 0x00005400u)
#define CSL_VPS_CHR_US_SEC1_0_REGS      (CSL_TI816x_VPS_BASE + 0x00005480u)
#define CSL_VPS_VIP0_PARSER_0_REGS      (CSL_TI816x_VPS_BASE + 0x00005500u)
#define CSL_VPS_CSC_VIP0_0_REGS         (CSL_TI816x_VPS_BASE + 0x00005700u)
#define CSL_VPS_SC_3_REGS               (CSL_TI816x_VPS_BASE + 0x00005800u)
#define CSL_VPS_VIP1_PARSER_0_REGS      (CSL_TI816x_VPS_BASE + 0x00005A00u)
#define CSL_VPS_CSC_VIP1_0_REGS         (CSL_TI816x_VPS_BASE + 0x00005C00u)
#define CSL_VPS_SC_4_REGS               (CSL_TI816x_VPS_BASE + 0x00005D00u)
#define CSL_VPS_SD_VENC_0_REGS          (CSL_TI816x_VPS_BASE + 0x00005E00u)
#define CSL_VPS_HDMI_VENC_0_REGS        (CSL_TI816x_VPS_BASE + 0x00006000u)
#define CSL_VPS_HDCOMP_VENC_0_REGS      (CSL_TI816x_VPS_BASE + 0x00008000u)
#define CSL_VPS_DVO2_VENC_0_REGS        (CSL_TI816x_VPS_BASE + 0x0000A000u)
#define CSL_VPS_NTSC_RF_0_REGS          (CSL_TI816x_VPS_BASE + 0x0000C000u)
#define CSL_VPS_NF_0_REGS               (CSL_TI816x_VPS_BASE + 0x0000C200u)
#define CSL_VPS_COMPR_PRI_TOP_0_REGS    (CSL_TI816x_VPS_BASE + 0x0000C500u)
#define CSL_VPS_COMPR_PRI_BOT_0_REGS    (CSL_TI816x_VPS_BASE + 0x0000C600u)
#define CSL_VPS_DECOMPR_PRI_FLD1_0_REGS (CSL_TI816x_VPS_BASE + 0x0000C700u)
#define CSL_VPS_DECOMPR_PRI_FLD2_0_REGS (CSL_TI816x_VPS_BASE + 0x0000C800u)
#define CSL_VPS_DECOMPR_PRI_FLD3_0_REGS (CSL_TI816x_VPS_BASE + 0x0000C900u)
#define CSL_VPS_COMPR_AUX_0_REGS        (CSL_TI816x_VPS_BASE + 0x0000CA00u)
#define CSL_VPS_DECOMPR_AUX_FLD1_0_REGS (CSL_TI816x_VPS_BASE + 0x0000CB00u)
#define CSL_VPS_DECOMPR_AUX_FLD2_0_REGS (CSL_TI816x_VPS_BASE + 0x0000CC00u)
#define CSL_VPS_VPDMA_0_REGS            (CSL_TI816x_VPS_BASE + 0x0000D000u)

/******************************************************************************\
* Interrupt Event IDs
\******************************************************************************/

/* I2C interrupt                                                              */
#define CSL_INTC_EVENTID_I2CINT0         (41u)
#define CSL_INTC_EVENTID_I2CINT1         (42u)

#ifdef __cplusplus
}
#endif

#endif  /* _SOC_TI816X_H_ */
