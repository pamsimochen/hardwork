/** ==================================================================
 *  @file   cslr_TI814xprcm.h                                                  
 *                                                                    
 *  @path   /ti/psp/cslr/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
#ifndef _CSLR_TI814XPRCM_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_TI814XPRCM_H_

typedef struct {
    volatile Uint32 RSVD0[64];
    /**< PRM_DEVICE */
    volatile Uint32 RSVD1[64];
    /**< CM_DEVICE */
    volatile Uint32 RSVD2[64];
    /**< Revision PRM */
    volatile Uint32 RSVD3[43];
    /**< CM_DPLL registers */
    volatile Uint32 CM_HDMI_CLKSEL;
    /**< HDMI CLK Sel */
    volatile Uint32 RSVD4[20];
    /**< CM_DPLL registers */
    volatile Uint32 RSVD5[64];
    /**< CM_DSP */
    volatile Uint32 RSVD6[64];
    /**< CM_ALWON2 */
    volatile Uint32 RSVD7[64];
    /**< CM_HDVICP */
    volatile Uint32 RSVD8[64];
    /**< CM_ISS */
    volatile Uint32 CM_HDVPSS_CLKSTCTRL;
    /**< CM_HDVPSS_CLKSTCTRL */
    volatile Uint32 RSVD9[7];
    volatile Uint32 CM_HDVPSS_HDVPSS_CLK_CTRL;
    /**< CM_HDVPSS_HDVPSS_CLK_CTRL */
    volatile Uint32 CM_HDVPSS_HDMI_CLKCTRL;
    /**< CM_HDVPSS_HDMI_CLKCTRL */
    volatile Uint32 RSVD10[54];
    /**< CM_HDVPSS */
    volatile Uint32 RSVD11[64];
    /**< PRM_DSP  */
    volatile Uint32 RSVD12[64];
    /**< PRM_DSP */
    volatile Uint32 RSVD13[64];
    /**< PRM_ALWON2 */
    volatile Uint32 RSVD14[64];
    /**< PRM_HDVICP */
    volatile Uint32 RSVD15[64];
    /**< PRM_ISP */
    volatile Uint32 PM_HDVPSS_PWRSTCTRL;
    /**< PM_HDVPSS_PWRSTCTRL */
    volatile Uint32 PM_HDVPSS_PWRSTST;
    /**< PM_HDVPSS_PWRSTST */
    volatile Uint32 RSVD[2];
    /**< Reserved */
    volatile Uint32 RM_HDVPSS_RSTCTRL;
    /**< RM_HDVPSS_RSTCTRL */
    volatile Uint32 RM_HDVPSS_RSTST;
    /**< RM_HDVPSS_RSTST */
    volatile Uint32 RSVD16[58];
    /**< HDVPSS remaining reserved registers */
    volatile Uint32 RSVD17[64];
    /** PRM_GFX */
    volatile Uint32 RSVD18[256];
    /**< RSVD No registers */
    volatile Uint32 RSVD19[89];
    /** CM_ALWON */
    volatile Uint32 CM_ALWON_I2C_02_CLKCTRL;
    /**< I2c02 clk control */
    volatile Uint32 CM_ALWON_I2C_13_CLKCTRL;
    /**< I2c02 interface clock control */
    volatile Uint32 RSVD20[165];
    /**< Remaining CM_ALWON registers */
    volatile Uint32 RSVD21[256];
    /**< PRM_ALWON  */
} CSL_Prcm_Regs;

typedef volatile CSL_Prcm_Regs *CSL_PrcmRegs;

#define HDVPSS_CLKSTCTRL_CLKACTIVITY_HDVPSS_GCLK_MASK       (0x00000100u)
#define HDVPSS_CLKSTCTRL_CLKACTIVITY_HDVPSS_GCLK_SHIFT      (0x8u)

#define HDVPSS_CLKSTCTRL_CLKTRCTRL_MASK                     (0x00000003u)
#define HDVPSS_CLKSTCTRL_CLKTRCTRL_SHIFT                    (0x00000000u)

#define HDVPSS_CLKCTRL_STBYST_MASK                          (0x00040000u)
#define HDVPSS_CLKCTRL_STBYST_SHIFT                         (0x00000013u)

#define HDVPSS_CLKCTRL_IDLEST_MASK                          (0x00030000u)
#define HDVPSS_CLKCTRL_IDLEST_SHIFT                         (0x00000010u)

#define HDVPSS_CLKCTRL_MODULEMODE_MASK                      (0x00000003u)
#define HDVPSS_CLKCTRL_MODULEMODE_SHIFT                     (0x0u)

#define HDMI_CLKCTRL_IDLEST_MASK                            (0x00030000u)
#define HDMI_CLKCTRL_IDLEST_SHIFT                           (0x00000010u)

#define HDMI_CLKCTRL_MODULEMODE_MASK                        (0x00000003u)
#define HDMI_CLKCTRL_MODULEMODE_SHIFT                       (0x0u)

#define HDVPSS_PM_PWRSTST_INTRANSITION_MASK			        (0x00100000u)
#define HDVPSS_PM_PWRSTST_INTRANSITION_SHIFT		        (0x00000014u)

#define HDVPSS_PM_PWRSTST_MEM_STATEST_MASK                  (0x00000030u)
#define HDVPSS_PM_PWRSTST_MEM_STATEST_SHIFT                 (0x00000004u)

#define HDVPSS_PM_PWRSTST_LOGICSTATEST_MASK                 (0x00000004u)
#define HDVPSS_PM_PWRSTST_LOGICSTATEST_SHIFT                (0x00000002u)

#define HDVPSS_PM_PWRSTST_POWERSTATEST_MASK                 (0x00000003u)
#define HDVPSS_PM_PWRSTST_POWERSTATEST_SHIFT                (0x00000000u)

#define HDVPSS_RM_RSTCTRL_HDVPSS_RST_MASK                   (0x00000004u)
#define HDVPSS_RM_RSTCTRL_HDVPSS_RST_SHIFT                  (0x00000002u)

#define HDVPSS_RM_RSTST_HDVPSS_RST_MASK                     (0x00000004u)
#define HDVPSS_RM_RSTST_HDVPSS_RST_SHIFT                    (0x00000002u)

#define CM_ALWON_I2C_02_CLKCTRL_IDLEST_MASK                 (0x00030000u)
#define CM_ALWON_I2C_02_CLKCTRL_IDLEST_SHIFT                (0x00000010u)

#define CM_ALWON_I2C_02_CLKCTRL_MODULEMODE_MASK             (0x00000003u)
#define CM_ALWON_I2C_02_CLKCTRL_MODULEMODE_SHIFT            (0x00000000u)

#define CM_ALWON_I2C_13_CLKCTRL_IDLEST_MASK                 (0x00030000u)
#define CM_ALWON_I2C_13_CLKCTRL_IDLEST_SHIFT                (0x00000010u)

#define CM_ALWON_I2C_13_CLKCTRL_MODULEMODE_MASK             (0x00000003u)
#define CM_ALWON_I2C_13_CLKCTRL_MODULEMODE_SHIFT            (0x00000000u)

/* Chip configuration registers offsets */
#define HDMI_PHY_CTRL_REG_OFF                               0x1300
#define HDMI_PHY_CTRL_PD_PULLUPDET_MASK                     0x00000002
#define HDMI_PHY_CTRL_PD_PULLUPDET_SHIFT                    0x1
#define HDMI_PHY_CTRL_ENBYPASSCLK_MASK                      0x00000001
#define HDMI_PHY_CTRL_ENBYPASSCLK_SHIFT                     0x0

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _CSLR_TI814XPRCM_H_ 
                                                            */
