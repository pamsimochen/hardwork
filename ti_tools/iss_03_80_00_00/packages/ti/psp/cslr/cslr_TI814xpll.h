/** ==================================================================
 *  @file   cslr_TI814xpll.h
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
#ifndef _CSLR_TI814XPLL_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_TI814XPLL_H_

typedef struct {
    Uint32 CONTROL_REVISION;
    Uint32 CONTROL_HWINFO;
    Uint32 RSVD0[2];
    Uint8 CONTROL_SYSCONFIG;
    Uint32 RSVD1[87];
    Uint32 DSSPLL_PWRCTRL;
    Uint32 DSSPLL_CLKCTRL;
    Uint32 DSSPLL_TENABLE;
    Uint32 DSSPLL_TENABLEDIV;
    Uint32 DSSPLL_M2NDIV;
    Uint32 DSSPLL_MN2DIV;
    Uint32 DSSPLL_FRACDIV;
    Uint32 DSSPLL_BWCTRL;
    Uint32 DSSPLL_FRACCTRL;
    Uint32 DSSPLL_STATUS;
    Uint32 RSVD2[2];
    struct {
        Uint32 PWRCTRL;
        Uint32 CLKCTRL;
        Uint32 TENABLE;
        Uint32 TENABLEDIV;
        Uint32 M2NDIV;
        Uint32 MN2DIV;
        Uint32 FRACDIV;
        Uint32 BWCTRL;
        Uint32 FRACCTRL;
        Uint32 STATUS;
        Uint32 RSVD3[2];
    } VideoPll_Factors[3];
    Uint32 RSVD5[36];
    Uint32 OSC_SRC;
    Uint32 RSVD6[1];
    Uint32 VIDEO_PLL_CLKSRC;                               /* TODO Required */
    Uint32 RSVD7[4];
    Uint32 HDMI_I2S_CLKSRC;
} CSL_Pll_CtrlRegs;

typedef volatile CSL_Pll_CtrlRegs *CSL_PllCtrlRegs;

#define CLKCTRL                 0x4
#define TENABLE                 0x8
#define TENABLEDIV              0xC
#define M2NDIV                  0x10
#define MN2DIV                  0x14
#define STATUS                  0x24
#define OSC_FREQ                20

#define WR_MEM_32(addr, data)       *(volatile unsigned int*)(addr) =(unsigned int)(data)
#define RD_MEM_32(addr)             *(volatile unsigned int*)(addr)

/* Add the macros for the ADPLLLJx bitfields */

/* ADPLLLJx_PWRCTRL */
#define ADPLLLJx_PWRCTRL_PONIN_MASK                 (0x00000020u)
#define ADPLLLJx_PWRCTRL_PONIN_SHIFT                (0x5u)

#define ADPLLLJx_PWRCTRL_PGOODIN_MASK               (0x000000010u)
#define ADPLLLJx_PWRCTRL_PGOODIN_SHIFT              (0x4u)

#define ADPLLLJx_PWRCTRL_RET_MASK                   (0x00000008u)
#define ADPLLLJx_PWRCTRL_RET_SHIFT                  (0x3u)

#define ADPLLLJx_PWRCTRL_ISORET_MASK                (0x00000004u)
#define ADPLLLJx_PWRCTRL_ISORET_SHIFT               (0x2u)

#define ADPLLLJx_PWRCTRL_ISOSCAN_MASK               (0x00000002u)
#define ADPLLLJx_PWRCTRL_ISOSCAN_SHIFT              (0x1u)

#define ADPLLLJx_PWRCTRL_OFFMODE_MASK               (0x000000001u)
#define ADPLLLJx_PWRCTRL_OFFMODE_SHIFT              (0x0u)

#define ADPLLLJx_CLKCTRL_CYCLESLIPEN_MASK           (0x80000000u)
#define ADPLLLJx_CLKCTRL_CYCLESLIPEN_SHIFT          (31u)

#define ADPLLLJx_CLKCTRL_ENSSC_MASK                 (0x40000000u)
#define ADPLLLJx_CLKCTRL_ENSSC_SHIFT                (30u)

#define ADPLLLJx_CLKCTRL_NWELLTRIM_MASK             (0x1F000000u)
#define ADPLLLJx_CLKCTRL_NWELLTRIM_SHIFT            (24u)

#define ADPLLLJx_CLKCTRL_IDLE_MASK                  (0x00800000u)
#define ADPLLLJx_CLKCTRL_IDLE_SHIFT                 (23u)

#define ADPLLLJx_CLKCTRL_BYPASSACKZ_MASK            (0x00400000u)
#define ADPLLLJx_CLKCTRL_BYPASSACKZ_SHIFT           (22u)

#define ADPLLLJx_CLKCTRL_STBYRET_MASK               (0x00200000u)
#define ADPLLLJx_CLKCTRL_STBYRET_SHIFT              (21u)

#define ADPLLLJx_CLKCTRL_CLKOUTEN_MASK              (0x00100000u)
#define ADPLLLJx_CLKCTRL_CLKOUTEN_SHIFT             (20u)

#define ADPLLLJx_CLKCTRL_ULOWCLKEN_MASK             (0x00040000u)
#define ADPLLLJx_CLKCTRL_ULOWCLKEN_SHIFT            (18u)

#define ADPLLLJx_CLKCTRL_CLKDCOLDOPWDNZ_MASK        (0x00020000u)
#define ADPLLLJx_CLKCTRL_CLKDCOLDOPWDNZ_SHIFT       (17u)

#define ADPLLLJx_CLKCTRL_M2PWDNZ_MASK               (0x00010000u)
#define ADPLLLJx_CLKCTRL_M2PWDNZ_SHIFT              (16u)

#define ADPLLLJx_CLKCTRL_SELFREQDCO_MASK            (0x00001C00u)
#define ADPLLLJx_CLKCTRL_SELFREQDCO_SHIFT           (10u)

#define ADPLLLJx_CLKCTRL_RELAXED_LOCK_MASK          (0x00000100u)
#define ADPLLLJx_CLKCTRL_RELAXED_LOCK_SHIFT         (8u)

#define ADPLLLJx_CLKCTRL_TINITZ_MASK                (0x00000001u)
#define ADPLLLJx_CLKCTRL_TINITZ_SHIFT               (0u)

#define ADPLLLJx_TENABLE_MASK                       (0x00000001u)
#define ADPLLLJx_TENABLE_SHIFT                      (0u)

#define ADPLLLJx_TENABLEDIV_MASK                    (0x00000001u)
#define ADPLLLJx_TENABLEDIV_SHIFT                   (0u)

#define ADPLLLJx_M2NDIV_M_MASK                      (0x0x007F0000u)
#define ADPLLLJx_M2NDIV_M_SHIFT                     (16u)

#define ADPLLLJx_M2NDIV_N_MASK                      (0x000000FFu)
#define ADPLLLJx_M2NDIV_N_SHIFT                     (0u)

#define ADPLLLJx_FRACTONAL_DIVIDER_REGSD_MASK       (0xFF000000u)
#define ADPLLLJx_FRACTONAL_DIVIDER_REGSD_SHIFT      (24u)

#define ADPLLLJx_FRACTONAL_DIVIDER_FRACTIONALM_MASK (0x0003FFFFu)
#define ADPLLLJx_FRACTONAL_DIVIDER_FRACTIONALM_SHIFT (0x0)

#define ADPLLLJx_BWCTRL_BWCONTROL_MASK              (0x00000006u)
#define ADPLLLJx_BWCTRL_BWCONTROL_SHIFT             (1u)

#define ADPLLLJx_BWCTRL_BW_INCR_DECRZ_MASK          (0x00000001u)
#define ADPLLLJx_BWCTRL_BW_INCR_DECRZ_SHIFT         (0u)

#define ADPLLLJx_FRACT_CTRL_DOWNSPREAD_MASK         (0x80000000u)
#define ADPLLLJx_FRACT_CTRL_DOWNSPREAD_SHIFT        (31u)

#define ADPLLLJx_FRACT_CTRL_MODFREQDIVEXP_MASK      (0x70000000u)
#define ADPLLLJx_FRACT_CTRL_MODFREQDIVEXP_SHIFT     (28u)

#define ADPLLLJx_FRACT_CTRL_MODFREQDIVMAN_MASK      (0x0FE00000u)
#define ADPLLLJx_FRACT_CTRL_MODFREQDIVMAN_SHIFT     (21u)

#define ADPLLLJx_FRACT_CTRL_DELTAMSTEPINT_MASK      (0x001C0000u)
#define ADPLLLJx_FRACT_CTRL_DELTAMSTEPINT_SHIFT     (18u)

#define ADPLLLJx_FRACT_CTRL_DELTAMSTEPFRAC_MASK     (0x0003FFFFu)
#define ADPLLLJx_FRACT_CTRL_DELTAMSTEPFRAC_SHIFT     (0u)

#define ADPLLLJx_STATUS_PONOUT_MASK                 (0x80000000u)
#define ADPLLLJx_STATUS_PONOUT_SHIFT                (31u)

#define ADPLLLJx_STATUS_PGOODOUT_MASK               (0x40000000u)
#define ADPLLLJx_STATUS_PGOODOUT_SHIFT              (30u)

#define ADPLLLJx_STATUS_LDOPWDN_MASK                (0x20000000u)
#define ADPLLLJx_STATUS_LDOPWDN_SHIFT               (29u)

#define ADPLLLJx_STATUS_RECAL_BSTATUS3_MASK         (0x10000000u)
#define ADPLLLJx_STATUS_RECAL_BSTATUS3_SHIFT        (28u)

#define ADPLLLJx_STATUS_RECAL_OPPIN_MASK            (0x08000000u)
#define ADPLLLJx_STATUS_RECAL_OPPIN_SHIFT           (27u)

#define ADPLLLJx_STATUS_PHASELOCK_MASK              (0x00000400u)
#define ADPLLLJx_STATUS_PHASELOCK_SHIFT             (10u)

#define ADPLLLJx_STATUS_FREQLOCK_MASK               (0x00000200u)
#define ADPLLLJx_STATUS_FREQLOCK_SHIFT              (9u)

#define ADPLLLJx_STATUS_BYPASSACK_MASK              (0x00000100u)
#define ADPLLLJx_STATUS_BYPASSACK_SHIFT             (8u)

#define ADPLLLJx_STATUS_STBYRETACK_MASK             (0x00000080u)
#define ADPLLLJx_STATUS_STBYRETACK_SHIFT            (7u)

#define ADPLLLJx_STATUS_LOSSREF_MASK                (0x00000040u)
#define ADPLLLJx_STATUS_LOSSREF_SHIFT               (6u)

#define ADPLLLJx_STATUS_CLKOUTACK_MASK              (0x00000020u)
#define ADPLLLJx_STATUS_CLKOUTACK_SHIFT             (5u)

#define ADPLLLJx_STATUS_LOCK2_MASK                  (0x00000010u)
#define ADPLLLJx_STATUS_LOCK2_SHIFT                 (4u)

#define ADPLLLJx_STATUS_M2CHANGEACK_MASK            (0x00000008u)
#define ADPLLLJx_STATUS_M2CHANGEACK_SHIFT           (3u)

#define ADPLLLJx_STATUS_SSCACK_MASK                 (0x00000004u)
#define ADPLLLJx_STATUS_SSCACK_SHIFT                (2u)

#define ADPLLLJx_STATUS_HIGHJITTER_MASK             (0x00000002u)
#define ADPLLLJx_STATUS_HIGHJITTER_SHIFT            (1u)

#define ADPLLLJx_STATUS_BYPASS_MASK                 (0x00000001u)
#define ADPLLLJx_STATUS_BYPASS_SHIFT                (0u)

#define VID_PLL_CLK_SRC_DVO2_CLK_SRC_MASK      (0x01000000u)
#define VID_PLL_CLK_SRC_DVO2_CLK_SRC_SHIFT     (24u)

#define VID_PLL_CLK_SRC_VID_PLL_CLK2_SRC_MASK       (0x00000001u)
#define VID_PLL_CLK_SRC_VID_PLL_CLK2_SRC_SHIFT      (1u)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* #ifndef
                                                            * _CSLR_TI814XPLL_H_
                                                            */
