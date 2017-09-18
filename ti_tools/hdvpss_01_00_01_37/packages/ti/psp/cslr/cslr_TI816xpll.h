#ifndef _CSLR_PLL_H_
#define _CSLR_PLL_H_

typedef struct  {
    volatile Uint8 RSVD0[64];
    volatile Uint32 DDRPLL_CTRL;
    volatile Uint32 DDRPLL_PWD;
    volatile Uint8 RSVD1[4];
    volatile Uint32 DDR_PLL_DIV1;
    volatile Uint32 DDRLL_FREQ2;
    volatile Uint32 DDR_PLL_DIV2;
    volatile Uint32 DDRPLL_FREQ3;
    volatile Uint32 DDR_PLL_DIV3;
    volatile Uint32 DDRPLL_FREQ4;
    volatile Uint32 DDR_PLL_DIV4;
    volatile Uint32 DDRPLL_FREQ5;
    volatile Uint32 DDR_PLL_DIV5;
    volatile Uint32 VIDEOPLL_CTRL;
    volatile Uint32 VIDEOPLL_PWD;
    struct
    {
        volatile Uint32 VIDEOPLL_FREQ; //RF, VencD, VencA
        volatile Uint32 VIDEOPLL_DIV;
    } VideoPll_Div[3];
#if 0
    volatile Uint32 VIDEOPLL_FREQ2;
    volatile Uint32 VIDEOPLL_DIV2;
    volatile Uint32 VIDEOPLL_FREQ3;
    volatile Uint32 VIDEOPLL_DIV3;
#endif
} CSL_Pll_CtrlRegs;

typedef volatile CSL_Pll_CtrlRegs           *CSL_PllCtrlRegs;

/* VIDEOPLL_CTRL */
#define VPS_VIDEOPLL_CTRL_VIDEO_LOC_CTL_MASK        (0x1u)
#define VPS_VIDEOPLL_CTRL_VIDEO_LOC_CTL_SHIFT       (0u)

#define VPS_VIDEOPLL_CTRL_VIDEO_BP_MASK             (0x4u)
#define VPS_VIDEOPLL_CTRL_VIDEO_BP_SHIFT            (2u)

#define VPS_VIDEOPLL_CTRL_VIDEO_PLLEN_MASK          (0x8u)
#define VPS_VIDEOPLL_CTRL_VIDEO_PLLEN_SHIFT         (3u)

#define VPS_VIDEOPLL_CTRL_VIDEO_LOCK_MASK           (0x80u)
#define VPS_VIDEOPLL_CTRL_VIDEO_LOCK_SHIFT          (7u)

#define VPS_VIDEOPLL_CTRL_VIDEO_P_MASK              (0xFF00u)
#define VPS_VIDEOPLL_CTRL_VIDEO_P_SHIFT             (8u)

#define VPS_VIDEOPLL_CTRL_VIDEO_N_MASK              (0xFFFF0000u)
#define VPS_VIDEOPLL_CTRL_VIDEO_N_SHIFT             (16u)

/* VIDEOPLL_PWD */

#define VPS_VIDEOPLL_PWD_PWD_CLK1_MASK              (0x2u)
#define VPS_VIDEOPLL_PWD_PWD_CLK1_SHIFT             (1u)

#define VPS_VIDEOPLL_PWD_PWD_CLK2_MASK              (0x4u)
#define VPS_VIDEOPLL_PWD_PWD_CLK2_SHIFT             (2u)

#define VPS_VIDEOPLL_PWD_PWD_CLK3_MASK              (0x8u)
#define VPS_VIDEOPLL_PWD_PWD_CLK3_SHIFT             (3u)

/* VIDEOPLL_FREQ1 */

#define VPS_VIDEOPLL_FREQ_VID_FRACFREQ_MASK         (0x00FFFFFFu)
#define VPS_VIDEOPLL_FREQ_VID_FRACFREQ_SHIFT        (0u)

#define VPS_VIDEOPLL_FREQ_VID_INTFREQ_MASK          (0x0F000000u)
#define VPS_VIDEOPLL_FREQ_VID_INTFREQ_SHIFT         (24u)

#define VPS_VIDEOPLL_FREQ_VID_TRUNC_MASK           (0x10000000u)
#define VPS_VIDEOPLL_FREQ_VID_TRUNC_SHIFT          (28u)

#define VPS_VIDEOPLL_FREQ_VID_LDFREQ_MASK           (0x80000000u)
#define VPS_VIDEOPLL_FREQ_VID_LDFREQ_SHIFT          (31u)

/* VIDEOPLL_DIV1 */

#define VPS_VIDEOPLL_DIV_VID_MDIV_MASK              (0x000000FFu)
#define VPS_VIDEOPLL_DIV_VID_MDIV_SHIFT             (0u)

#define VPS_VIDEOPLL_DIV_VID_LDMDIV_MASK            (0x100u)
#define VPS_VIDEOPLL_DIV_VID_LDMDIV_SHIFT           (8u)

#endif
