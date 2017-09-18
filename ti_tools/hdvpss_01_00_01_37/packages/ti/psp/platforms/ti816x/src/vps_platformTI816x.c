/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_platformTI816x.c
 *
 *  \brief Implements the TI816x platform specific functions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/cslr/soc_TI816x.h>
#include <ti/psp/cslr/cslr_TI816xpll.h>
#include <ti/psp/cslr/cslr_TI816xprcm.h>
#include <ti/psp/vps/hal/vpshal_vip.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/devices/vps_videoDecoder.h>
#include <ti/psp/platforms/ti816x/vps_platformTI816x.h>

//#define PLATFORM_ZEBU


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Required right now since GEL file is not doing this */
#define ENABLE_HDVPSS_CLK

#ifndef PLATFORM_ZEBU
#define ENABLE_I2C_PROBE_ON_INIT
#endif

/* IO expander present on the base board */
#define VPS_BASE_BOARD_IO_EXP0_I2C_ADDR (0x20u)

/* IO expander present on the daughter card */
#define VPS_VSVC_BOARD_IO_EXP1_I2C_ADDR (0x21u)

/* IO expander present on the daughter card */
#define VPS_VSVC_BOARD_IO_EXP2_I2C_ADDR (0x23u)

/* THS7353 filter I2C address present in VC daughter card */
#define VPS_VC_BOARD_THS7353_I2C_ADDR   (0x2Eu)

#define VPS_PLATFORM_EVM_I2C_INST_ID    (VPS_DEVICE_I2C_INST_ID_1)

/** \brief PLL Control Module Base Address*/
#define VPS_CONTROL_MODULE_PLL_CTRL_BASE_ADDR   (CSL_TI816x_CTRL_MODULE_BASE + \
                                                    0x0400u)
/**< CM_DPLL Base Address */
#define VPS_PRCM_CM_DPLL_BASE_ADDR              (CSL_TI816x_PRCM_BASE + 0x0300u)

/** \brief Control Module Device Configuration Base Address */
#define VPS_CTRL_MODULE_DEV_CFG_BASE_ADDR       (CSL_TI816x_CTRL_MODULE_BASE + \
                                                    0x0600u)

/* Default Values for DDR PLL configuration to get clock for I2C and CEC */
#define VPS_DDR_INT_FREQ2               (0x8u)
#define VPS_DDR_FRACT_FREQ2             (0xD99999u)
#define VPS_DDR_MDIV2                   (0x1Eu)
#define VPS_DDR_SYCCLK10_DIV            (0x0u)

/* gpio base addresses  */
#define REG32                           *(volatile unsigned int*)

#define VPS_PRCM_HDVPSS_CLKS   (VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_HDMI_GCLK_MASK | \
                         VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_HDCOMP_GCLK_MASK | \
                         VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_SD_GCLK_MASK |        \
                         VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_RFMOD_GCLK_MASK |     \
                         VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_HD_DSS_L3_GCLK_MASK | \
                         VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_HD_DSS_L4_GCLK_MASK | \
                         VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_PROC_GCLK_MASK |      \
                         VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKACTIVITY_HD_DSS_L3_EN_GCLK_MASK)

#define VPS_PRCM_HDMI_CLKS  (VPS_PRCM_CM_HDMI_CLKSTCTRL_CLKACTIVITY_HDMI_OCP_GCLK_MASK | \
                             VPS_PRCM_CM_HDMI_CLKSTCTRL_CLKACTIVITY_HDMI_CEC_GCLK_MASK)

#define VPS_PRCM_CLKTRCTRL_NO_SLEEP     (0u)
#define VPS_PRCM_CLKTRCTRL_SW_SLEEP     (1u)
#define VPS_PRCM_CLKTRCTRL_SW_WKUP      (2u)
#define VPS_PRCM_CLKTRCTRL_HW_AUTO      (3u)

#define VPS_PRCM_MODULE_DISABLE         (0u)
#define VPS_PRCM_MODULE_ENABLE          (2u)

#define VPS_PRCM_MAX_REP_CNT            (5u)

#define VPS_VS_IO_EXP_RESET_DEV_MASK        (0x03u)
#define VPS_VC_IO_EXP_RESET_DEV_MASK        (0x0Fu)
#define VPS_VC_IO_EXP_SEL_VIN0_S1_MASK      (0x04u)
#define VPS_VC_IO_EXP_SEL_VIN1_S1_MASK      (0x08u)
#define VPS_VC_IO_EXP_THS7368_DISABLE_MASK  (0x10u)
#define VPS_VC_IO_EXP_THS7368_BYPASS_MASK   (0x20u)
#define VPS_VC_IO_EXP_THS7368_FILTER1_MASK  (0x40u)
#define VPS_VC_IO_EXP_THS7368_FILTER2_MASK  (0x80u)
#define VPS_VC_IO_EXP_THS7368_FILTER_SHIFT  (0x06u)

#define VPS_PLATFORM_VPLL_INPUT_FREQ        (27)
#define VPS_PLATFORM_VPLL_VAL_P             (2)
#define VPS_PLATFORM_VPLL_VAL_N             (110)

#define VPS_PLATFORM_VPLL_MIN_M             (1)
#define VPS_PLATFORM_VPLL_MAX_M             (255)

#define VPS_PLATFORM_VPLL_MIN_FREQ          (8)
#define VPS_PLATFORM_VPLL_MAX_FREQ          (16)

#define VPS_PLATFORM_VPLL_MIN_VCO_FREQ      (800)
#define VPS_PLATFORM_VPLL_MAX_VCO_FREQ      (1850)



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* \brief structure to keep track of pll configurations for a video mode */
typedef struct
{
    struct
    {
        UInt32 intFreq;
        /**< 4 bit Integer part of FREQ used in Flying Adder PLL */
        UInt32 fractFreq;
        /**< 24 bit Fractional part of FREQ used in Flying Adder PLL */
        UInt32 M;
        /**< Frequncy Port divider value */
        UInt32 enableTrunct;
    } VidFlyAddParams;

    UInt32 sysClkDiv;
    /**< PRCM SysClk divisor value. The possible
         values for the SYSCLK15 and SYSCLK13 are 0 to 7, which divides
         clock by 1 to 8 respectively.
         Note: SYSCLK11 can divide RF pixel clock by 1 or 2 only, so if
               anyother frequency is needed, intFreq and/or fracFreq
               should be changed */
} Vps_VideoPllCtrl;

typedef struct
{
    Semaphore_Handle            glock;
    /**< global level lock - used for IO expander access. */
    UInt8                       ioExpVal[2];
    /**< Current value set in IO expander present in daughter card. */
} Vps_platformTI816xCommonObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 Vps_platformTI816xSetPinMux(void);

static Int32 Vps_platformTI816xEnableHdVpssClk(void);
static Int32 Vps_platformTI816xInitVencPixClk(void);
#ifndef PLATFORM_ZEBU
static Int32 Vps_platformTI816xEnableI2c(void);
#endif
static void udelay(int delay_usec);
static Int32 vpsPlatformIoExpLock(void);
static Int32 vpsPlatformIoExpUnLock(void);
static Int32 vpsPlatformCalcVpllClkParams(UInt32 outputFreq, Vps_VideoPllCtrl *vpllCtrl);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


static CSL_PllCtrlRegs gVpsPllCtrlRegs =
            (CSL_PllCtrlRegs) VPS_CONTROL_MODULE_PLL_CTRL_BASE_ADDR;
static CSL_PrcmRegs gVpsPrcmRegs = (CSL_PrcmRegs) CSL_TI816x_PRCM_BASE;

Vps_platformTI816xCommonObj gPlatformTI816xCommonObj;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Int32 Vps_platformTI816xInit(Vps_PlatformInitParams *initParams)
{
    Int32               status = FVID2_SOK;
    Semaphore_Params    semParams;

    memset(&gPlatformTI816xCommonObj, 0, sizeof(gPlatformTI816xCommonObj));
    gPlatformTI816xCommonObj.ioExpVal[0] = 0xFFu;
    gPlatformTI816xCommonObj.ioExpVal[1] = 0xFFu;

    /* Create global lock  */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    gPlatformTI816xCommonObj.glock = Semaphore_create(1u, &semParams, NULL);
    if (NULL == gPlatformTI816xCommonObj.glock)
    {
        status = FVID2_EALLOC;
    }
    else
    {
        if (TRUE == initParams->isPinMuxSettingReq)
        {
            Vps_platformTI816xSetPinMux();
        }

#ifdef ENABLE_HDVPSS_CLK
        /* Initialize Pixel Clock */
        status |= Vps_platformTI816xEnableHdVpssClk();
        status |= Vps_platformTI816xInitVencPixClk();
#ifndef PLATFORM_ZEBU
        status |= Vps_platformTI816xEnableI2c();
#endif  /* #ifndef PLATFORM_ZEBU */
#endif  /* #ifdef ENABLE_HDVPSS_CLK */
    }

    if ((FVID2_SOK != status) && (NULL != gPlatformTI816xCommonObj.glock))
    {
        /* Error - free acquired resources */
        Semaphore_delete(&gPlatformTI816xCommonObj.glock);
        gPlatformTI816xCommonObj.glock = NULL;
    }

    return (status);
}

Int32 Vps_platformTI816xDeInit(void)
{
    Int32 status = FVID2_SOK;

    if (NULL != gPlatformTI816xCommonObj.glock)
    {
        /* Error - free acquired resources */
        Semaphore_delete(&gPlatformTI816xCommonObj.glock);
        gPlatformTI816xCommonObj.glock = NULL;
    }

    return (status);
}

/* Init EVM related sub-systems like I2C instance */
Int32 Vps_platformTI816xDeviceInit(Vps_PlatformDeviceInitParams *initPrms)
{
#ifdef PLATFORM_ZEBU
    return FVID2_SOK;
#else

    Int32 status = FVID2_SOK;
#ifdef PLATFORM_EVM_SI
    UInt8 i2cCnt = 0;
    /* TI816x has 2 I2C instances. */
    UInt32 i2cRegs[VPS_DEVICE_I2C_INST_ID_MAX] = {CSL_TI816x_I2C0_BASE,
                                                  CSL_TI816x_I2C1_BASE,
                                                  NULL,
                                                  NULL};
    UInt32 i2cInt[VPS_DEVICE_I2C_INST_ID_MAX] = {CSL_INTC_EVENTID_I2CINT0,
                                                 CSL_INTC_EVENTID_I2CINT1,
                                                 NULL,
                                                 NULL};

    Vps_DeviceInitParams deviceInitPrm;

    /*
     * External video device subsystem init
     */
    memset ( &deviceInitPrm, 0, sizeof ( deviceInitPrm ) );

    /*
     * Initialize I2C instances
     */
    for (i2cCnt = 0; i2cCnt < VPS_DEVICE_I2C_INST_ID_MAX; i2cCnt++)
    {
        deviceInitPrm.i2cRegs[i2cCnt] = (Ptr)(i2cRegs[i2cCnt]);
        deviceInitPrm.i2cIntNum[i2cCnt] = i2cInt[i2cCnt];
        deviceInitPrm.i2cClkKHz[i2cCnt] = VPS_DEVICE_I2C_INST_NOT_USED;
    }
    deviceInitPrm.isI2cInitReq = initPrms->isI2cInitReq;

    /* TI816x uses only I2C[1], so modify the sampling frequency */
    deviceInitPrm.i2cRegs[VPS_PLATFORM_EVM_I2C_INST_ID]
        = (Ptr)CSL_TI816x_I2C1_BASE;
    deviceInitPrm.i2cIntNum[VPS_PLATFORM_EVM_I2C_INST_ID]
        = CSL_INTC_EVENTID_I2CINT1;
    deviceInitPrm.i2cClkKHz[VPS_PLATFORM_EVM_I2C_INST_ID]
        = 400;

    status = Vps_deviceInit ( &deviceInitPrm );

#ifdef ENABLE_I2C_PROBE_ON_INIT
    if (TRUE == initPrms->isI2cInitReq &&
            TRUE == initPrms->isI2cProbingReq)
    {
        Vps_deviceI2cProbeAll(Vps_platformGetI2cInstId());
    }
    #endif
#endif

    return (status);
#endif  /* ZEBU */
}

/* De-Init EVM related sub-systems */
Int32 Vps_platformTI816xDeviceDeInit(void)
{
#ifdef PLATFORM_ZEBU
    return 0;
#else

#ifdef PLATFORM_EVM_SI
    /*
     * Extern video device de-init
     */
    Vps_deviceDeInit (  );

#endif

    return (FVID2_SOK);
#endif  /* ZEBU */
}

UInt32 Vps_platformTI816xGetI2cInstId(void)
{
    return (VPS_PLATFORM_EVM_I2C_INST_ID);
}

UInt8 Vps_platformTI816xGetVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt8 devAddr;
    UInt8 devAddrTvp5158[VPS_CAPT_INST_MAX] = { 0x58, 0x5c, 0x5a, 0x5e };
    UInt8 devAddrSii9135[VPS_CAPT_INST_MAX] = { 0x31, 0x00, 0x30, 0x00 };
    UInt8 devAddrTvp7002[VPS_CAPT_INST_MAX] = { 0x5d, 0x00, 0x5c, 0x00 };

    GT_assert( GT_DEFAULT_MASK, vipInstId<VPS_CAPT_INST_MAX);

    switch (vidDecId)
    {
        case FVID2_VPS_VID_DEC_TVP5158_DRV:

            devAddr = devAddrTvp5158[vipInstId];
            break;

        case FVID2_VPS_VID_DEC_SII9135_DRV:
            devAddr = devAddrSii9135[vipInstId];
            break;

        case FVID2_VPS_VID_DEC_TVP7002_DRV:
            devAddr = devAddrTvp7002[vipInstId];
            break;

        default:
            GT_assert( GT_DEFAULT_MASK, 0);

    }

    return (devAddr);
}

UInt8 Vps_platformTI816xGetVidEncI2cAddr(UInt32 vidEncId)
{
    UInt8 devAddr = 0u;
    /* There can be two Sii9022 instances, one on VS and other on VC board.
     * devAddrSii9022[0] -> VS Sii9022
     * devAddrSii9022[1] -> VC Sii9022
     */
    UInt8 devAddrSii9022[2u] = { 0x39, 0x39 };

    if (vidEncId == FVID2_VPS_VID_ENC_SII9022A_DRV)
    {
        /* Since address is same for both VS and VC, no need to detect. */
        devAddr = devAddrSii9022[0];
    }

    return (devAddr);
}

Int32 Vps_platformTI816xSimVideoInputSelect(UInt32 vipInstId,
                                            UInt32 fileId,
                                            UInt32 pixelClk)
{
    volatile UInt32 *pRegs[2];
    UInt32 instId, portId;

    pRegs[0] = (UInt32 *)(CSL_TI816x_VPS_BASE+0xE100);
    pRegs[1] = (UInt32 *)(CSL_TI816x_VPS_BASE+0xE200);

    instId = vipInstId/VPSHAL_VIP_INST_MAX;
    portId = vipInstId%VPSHAL_VIP_INST_MAX;

    /*
     * VIP reset is done so that switch from single channel to
     * multi-channel case is proper in simulator
     */
    if (0 == instId)
    {
        VpsHal_vpsClkcModuleReset(VPSHAL_VPS_CLKC_VIP0, TRUE);
        VpsHal_vpsClkcModuleReset(VPSHAL_VPS_CLKC_VIP0, FALSE);
    }
    else
    {
        VpsHal_vpsClkcModuleReset(VPSHAL_VPS_CLKC_VIP1, TRUE);
        VpsHal_vpsClkcModuleReset(VPSHAL_VPS_CLKC_VIP1, FALSE);
    }

    pRegs[instId][0+portId] = fileId;
    pRegs[instId][2+portId] = pixelClk;

    return (FVID2_SOK);
}

static Int32 Vps_platformTI816xInitVencPixClk(void)
{
    UInt32 regValue;

    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPllCtrlRegs);
    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPrcmRegs);

    /* Put Video PLL in Bypass mode */
    regValue = gVpsPllCtrlRegs->VIDEOPLL_CTRL;
    regValue &= ~(VPS_VIDEOPLL_CTRL_VIDEO_BP_MASK);
    regValue |= VPS_VIDEOPLL_CTRL_VIDEO_BP_MASK;
    gVpsPllCtrlRegs->VIDEOPLL_CTRL = regValue;

#ifdef PLATFORM_ZEBU
    udelay(10);
#else
    udelay(1000);
#endif

    /*Bring Video PLL out of Power Down Mode*/
    regValue = gVpsPllCtrlRegs->VIDEOPLL_CTRL;
    regValue &= ~(VPS_VIDEOPLL_CTRL_VIDEO_PLLEN_MASK);
    regValue |= VPS_VIDEOPLL_CTRL_VIDEO_PLLEN_MASK;
    gVpsPllCtrlRegs->VIDEOPLL_CTRL = regValue;

#ifdef PLATFORM_ZEBU
    udelay(10);
#else
    udelay(1000);
#endif

    /*Program the Video PLL Multiplier and Pre-divider value*/
    regValue = gVpsPllCtrlRegs->VIDEOPLL_CTRL;
    regValue &= ~(VPS_VIDEOPLL_CTRL_VIDEO_P_MASK | VPS_VIDEOPLL_CTRL_VIDEO_N_MASK);
    regValue |=(((0x6Eu<<VPS_VIDEOPLL_CTRL_VIDEO_N_SHIFT)&
                    VPS_VIDEOPLL_CTRL_VIDEO_N_MASK) |
               ((2u<<VPS_VIDEOPLL_CTRL_VIDEO_P_SHIFT) &
                VPS_VIDEOPLL_CTRL_VIDEO_P_MASK));
    gVpsPllCtrlRegs->VIDEOPLL_CTRL = regValue;

    /*Bring the Video PLL Individual output clocks out of Power Down Mode
    1->Power Down
    0->Normal Mode i.e., out of Power Down Mode
    */
    gVpsPllCtrlRegs->VIDEOPLL_PWD &= ~(VPS_VIDEOPLL_PWD_PWD_CLK1_MASK |
                                    VPS_VIDEOPLL_PWD_PWD_CLK2_MASK |
                                    VPS_VIDEOPLL_PWD_PWD_CLK3_MASK);

    return FVID2_SOK;
}

Int32 Vps_platformTI816xSetVencPixClk(Vps_SystemVPllClk *vpllCfg)
{
    Int32 retVal;
    volatile UInt32 repeatCnt;
    UInt32 regValue;
    Vps_VideoPllCtrl *vppCtrl = NULL;
    Vps_VideoPllCtrl vpllCtrl;

    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPllCtrlRegs);
    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPrcmRegs);
    GT_assert( GT_DEFAULT_MASK, NULL != vpllCfg);

    if (VPS_SYSTEM_VPLL_OUTPUT_VENC_RF == vpllCfg->outputVenc)
    {
        GT_assert( GT_DEFAULT_MASK, 216000u  == vpllCfg->outputClk);
    }

    retVal = vpsPlatformCalcVpllClkParams(vpllCfg->outputClk, &vpllCtrl);

    if (FVID2_SOK != retVal)
    {
        return (retVal);
    }

    vppCtrl = &vpllCtrl;

    regValue = VPS_VIDEOPLL_FREQ_VID_LDFREQ_MASK; /* To Load FREQ Value */
    if (vppCtrl->VidFlyAddParams.enableTrunct)
    {
        regValue |= VPS_VIDEOPLL_FREQ_VID_TRUNC_MASK; /* To Enable Truncate Correction */
    }
    regValue |=
      (vppCtrl->VidFlyAddParams.intFreq <<
      VPS_VIDEOPLL_FREQ_VID_INTFREQ_SHIFT) &
      VPS_VIDEOPLL_FREQ_VID_INTFREQ_MASK;
    regValue |=
      (vppCtrl->VidFlyAddParams.fractFreq <<
      VPS_VIDEOPLL_FREQ_VID_FRACFREQ_SHIFT) &
      VPS_VIDEOPLL_FREQ_VID_FRACFREQ_MASK;
    gVpsPllCtrlRegs->VideoPll_Div[vpllCfg->outputVenc].VIDEOPLL_FREQ = regValue;

    regValue = VPS_VIDEOPLL_DIV_VID_LDMDIV_MASK;
    regValue |= vppCtrl->VidFlyAddParams.M;
    gVpsPllCtrlRegs->VideoPll_Div[vpllCfg->outputVenc].VIDEOPLL_DIV = regValue;

    if (VPS_SYSTEM_VPLL_OUTPUT_VENC_RF == vpllCfg->outputVenc)
    {
        gVpsPrcmRegs->CM_SYSCLK11_CLKSEL &= ~VPS_PRCM_CM_SYSCLK11_CLKSEL_CLKSEL_MASK;
        gVpsPrcmRegs->CM_SYSCLK11_CLKSEL |=
            (vppCtrl->sysClkDiv <<
            VPS_PRCM_CM_SYSCLK11_CLKSEL_CLKSEL_SHIFT) &
            VPS_PRCM_CM_SYSCLK11_CLKSEL_CLKSEL_MASK;
    }
    else if (VPS_SYSTEM_VPLL_OUTPUT_VENC_D == vpllCfg->outputVenc)
    {
        gVpsPrcmRegs->CM_SYSCLK13_CLKSEL &= ~VPS_PRCM_CM_SYSCLK13_CLKSEL_CLKSEL_MASK;
        gVpsPrcmRegs->CM_SYSCLK13_CLKSEL |=
            (vppCtrl->sysClkDiv <<
            VPS_PRCM_CM_SYSCLK13_CLKSEL_CLKSEL_SHIFT) &
            VPS_PRCM_CM_SYSCLK13_CLKSEL_CLKSEL_MASK;
    }
    else if (VPS_SYSTEM_VPLL_OUTPUT_VENC_A == vpllCfg->outputVenc)
    {
        gVpsPrcmRegs->CM_SYSCLK15_CLKSEL &= ~VPS_PRCM_CM_SYSCLK15_CLKSEL_CLKSEL_MASK;
        gVpsPrcmRegs->CM_SYSCLK15_CLKSEL |=
            (vppCtrl->sysClkDiv <<
            VPS_PRCM_CM_SYSCLK15_CLKSEL_CLKSEL_SHIFT) &
            VPS_PRCM_CM_SYSCLK15_CLKSEL_CLKSEL_MASK;
    }

    /*Wait for PLL to Lock*/
    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if ((gVpsPllCtrlRegs->VIDEOPLL_CTRL &
                VPS_VIDEOPLL_CTRL_VIDEO_LOCK_MASK) ==
                VPS_VIDEOPLL_CTRL_VIDEO_LOCK_MASK)
        {
            break;
        }

        /* Wait for the 100 cycles */
#ifdef PLATFORM_ZEBU
        udelay(10);
#else
        udelay(1000);
#endif

        repeatCnt++;
    }

    /*Put the Main PLL in Normal(PLL) Mode*/
    regValue = gVpsPllCtrlRegs->VIDEOPLL_CTRL;
    regValue &= ~(VPS_VIDEOPLL_CTRL_VIDEO_BP_MASK);
    gVpsPllCtrlRegs->VIDEOPLL_CTRL = regValue;

#ifdef PLATFORM_ZEBU
    udelay(10);
#else
    udelay(1000);
#endif

    return (FVID2_SOK);
}


/*
    Setup pinmux in for capture/display

    Pinmux setup is as shown below,

    VIP0
    - YCC 16-bit        - Always ENABLED
    - YCC 24-bit        - ENABLED
    - CLK0              - Always ENABLED
    - CLK1              - Always ENABLED
    - HD/VD/DE/FLD0     - ENABLED
    - HD/VD/DE/FLD1     - DISABLED

    VIP1
    - YCC 16-bit        - ENABLED
    - CLK0              - ENABLED
    - CLK1              - ENABLED
    - HD/VD/DE/FLD0     - DISABLED
    - HD/VD/DE/FLD1     - DISABLED

    VOUT0 (DVO2)
    - CLK               - Always ENABLED
    - HD/VD/DE/FLD      - ENABLED
    - YCC 16-bit        - Always ENABLED
    - YCC 20-bit        - ENABLED
    - RGB/YCC 24-bit    - ENABLED
    - RGB/YCC 30-bit    - ENABLED

    VOUT1 (DVO1)
    - CLK               - DISABLED
    - HD/VD/DE/FLD      - DISABLED
    - YCC 16-bit        - DISABLED
    - YCC 20-bit        - DISABLED
    - RGB/YCC 24-bit    - DISABLED
    - RGB/YCC 30-bit    - DISABLED
*/
static Int32 Vps_platformTI816xSetPinMux(void)
{
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0814) = 2;   //5   tsi1_dclk     vout1_b_cb_c3   VIN1_D9
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0818) = 2;   //6   tsi1_data     vout1_b_cb_c4   VIN1_D10
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x081c) = 2;   //7   tsi1_bytstrt  vout1_b_cb_c5   VIN1_D11
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0820) = 2;   //8   tsi1_pacval   vout1_b_cb_c6   VIN1_D12
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0824) = 2;   //9   tsi1_pacerr   vout1_b_cb_c7   VIN1_D13
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0828) = 1;   //10  tsi2_dclk     VIN1_D14
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x082c) = 1;   //11  tsi2_data     VIN0_D20        vin0_de1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0830) = 1;   //12  tsi2_bytstrt  VIN0_D21        vin0_fld1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0834) = 1;   //13  tsi2_pacval   VIN0_D22        vin0_vsync1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0838) = 1;   //14  tsi2_pacerr   VIN0_D23        vin0_hsync1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x083c) = 2;   //15  tsi3_dclk     vout1_g_y_yc6   VIN1_D4
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0840) = 2;   //16  tsi3_data     vout1_g_y_yc7   VIN1_D5
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0844) = 2;   //17  tsi3_bytstrt  vout1_g_y_yc8   VIN1_D6
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0848) = 2;   //18  tsi3_pacval   vout1_g_y_yc9   VIN1_D7
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x084c) = 2;   //19  tsi3_pacerr   vout1_b_cb_c2   VIN1_D8
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0850) = 2;   //20  tsi4_dclk     vout1_hsync     VIN1_D15
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0854) = 1;   //21  tsi4_data     VIN0_D16        vin1_hsync0     vout1_fld
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0858) = 1;   //22  tsi4_bytstrt  VIN0_D17        vin1_vsync0     vout1_vsync
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x085c) = 1;   //23  tsi4_pacval   VIN0_D18        vin1_fld0       vout1_b_cb_c8
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0860) = 1;   //24  tsi4_pacerr   VIN0_D19        vin1_de0        vout1_b_cb_c9
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0864) = 1;   //25  tsi5_dclk     vout0_r_cr0     vout1_b_cb_c8   vout1_clk
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0868) = 1;   //26  tsi5_data     vout0_b_cb_c0   vout1_b_cb_c9   vin1_hsync1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x086c) = 1;   //27  tsi5_bytstrt  vout0_b_cb_c1   vout1_hsync     vout1_avid
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0870) = 1;   //28  tsi5_pacval   vout0_g_y_yc0   vout1_vsync     vin1_vsync1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0874) = 1;   //29  tsi5_pacerr   vout0_g_y_yc1   vout1_fld       vin1_fld1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0878) = 2;   //30  tsi6_dclk     vout1_avid      VIN1_CLK1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x087c) = 1;   //31  tsi6_data     vin0_hsync0
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0880) = 1;   //32  tsi6_bytstrt  vin0_vsync0
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0884) = 1;   //33  tsi6_pacval   vin0_fld0
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0888) = 1;   //34  tsi6_pacerr   vin0_de0
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x088c) = 1;   //35  tsi7_dclk     vout0_hsync
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0890) = 1;   //36  tsi7_data     vout0_vsync
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0894) = 1;   //37  tsi7_bytstrt  vout0_fld
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0898) = 1;   //38  tsi7_pacval   vout0_avid
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x089c) = 1;   //39  tsi7_pacerr   vout0_r_cr1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x08b4) = 2;   //45  tso1_dclk     vout1_clk       VIN1_CLK0
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x08b8) = 2;   //46  tso1_data     vout1_g_y_yc2   VIN1_D0
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x08bc) = 2;   //47  tso1_bytstrt  vout1_g_y_yc3   VIN1_D1
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x08c0) = 2;   //48  tso1_pacval   vout1_g_y_yc4   VIN1_D2
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x08c4) = 2;   //49  tso1_pacerr   vout1_g_y_yc5   VIN1_D3
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09d4) = 0;   //117 vout0_r_cr2   vout0_hsync     vout1_g_y_yc2
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09d8) = 0;   //118 vout0_r_cr3   vout0_vsync     vout1_g_y_yc3
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09dc) = 0;   //119 vout0_r_cr4   vout0_fld       vout1_g_y_yc4
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09e0) = 0;   //120 vout0_r_cr5   vout0_avid      vout1_g_y_yc5
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09e4) = 0;   //121 vout0_r_cr6   vout0_g_y_yc0   vout1_g_y_yc6
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09e8) = 0;   //122 vout0_r_cr7   vout0_g_y_yc1   vout1_g_y_yc7
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09ec) = 0;   //123 vout0_r_cr8   vout0_b_cb_c0   vout1_g_y_yc8
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x09f0) = 0;   //124 vout0_r_cr9   vout0_b_cb_c1   vout1_g_y_yc9
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0C80) = 0x10;//288 iic1_scl (mode 0, pull-up selected, pull-up enabled)
    REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0C84) = 0x10;//289 iic1_sda (mode 0, pull-up selected, pull-up enabled)

    return (FVID2_SOK);
}

Int32 Vps_platformTI816xResetVideoDevices(void)
{
    Int32       status;
    UInt32      resetMask;
    UInt32      i2cInst = Vps_platformTI816xGetI2cInstId();
    UInt32      ioExpI2cAddr = VPS_VSVC_BOARD_IO_EXP1_I2C_ADDR;
    UInt32      boardId;

    boardId = Vps_platformGetBoardId();
    if (VPS_PLATFORM_BOARD_VS == boardId)
    {
        resetMask = VPS_VS_IO_EXP_RESET_DEV_MASK;
    }
    else
    {
        resetMask = VPS_VC_IO_EXP_RESET_DEV_MASK;
    }

    /* IO expander lock */
    vpsPlatformIoExpLock();

    gPlatformTI816xCommonObj.ioExpVal[0] &= ~resetMask;
    status = Vps_deviceRawWrite8(
                 i2cInst,
                 ioExpI2cAddr,
                 gPlatformTI816xCommonObj.ioExpVal,
                 2);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    Task_sleep(1000);

    if (VPS_PLATFORM_BOARD_CATALOG == boardId)
    {
        ioExpI2cAddr = VPS_VSVC_BOARD_IO_EXP2_I2C_ADDR;
        status = Vps_deviceRawWrite8(
                     i2cInst,
                     ioExpI2cAddr,
                     gPlatformTI816xCommonObj.ioExpVal,
                     2);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        Task_sleep(1000);
    }

     ioExpI2cAddr = VPS_VSVC_BOARD_IO_EXP1_I2C_ADDR;

    /* Applicable for both VS and VC boards */
    gPlatformTI816xCommonObj.ioExpVal[0] |= resetMask;
    status = Vps_deviceRawWrite8(
                 i2cInst,
                 ioExpI2cAddr,
                 gPlatformTI816xCommonObj.ioExpVal,
                 2);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    Task_sleep(500);

    if (VPS_PLATFORM_BOARD_CATALOG == boardId)
    {
     ioExpI2cAddr = VPS_VSVC_BOARD_IO_EXP2_I2C_ADDR;
        status = Vps_deviceRawWrite8(
                     i2cInst,
                     ioExpI2cAddr,
                     gPlatformTI816xCommonObj.ioExpVal,
                     2);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        Task_sleep(500);

    }
    /* IO expander unlock */
    vpsPlatformIoExpUnLock();

    return (status);
}

Vps_PlatformCpuRev Vps_platformTI816xGetCpuRev(void)
{
#ifdef PLATFORM_ZEBU
    return (VPS_PLATFORM_CPU_REV_2_0);
#else
    UInt32                  cpuId, cpuRev;
    Vps_PlatformCpuRev      cpuRevEnum;

    /* Read CPU ID */
    cpuId = REG32(VPS_CTRL_MODULE_DEV_CFG_BASE_ADDR + 0x0000u);

    cpuRev = ((cpuId >> 28u) & 0x0Fu);
    switch (cpuRev)
    {
        case 0u:
            cpuRevEnum = VPS_PLATFORM_CPU_REV_1_0;
            break;

       case 1u:
            cpuRevEnum = VPS_PLATFORM_CPU_REV_1_1;
            break;

       case 2u:
            cpuRevEnum = VPS_PLATFORM_CPU_REV_2_0;
            break;

       default:
            /* Default to last known version */
            Vps_printf("PLATFORM: UNKNOWN CPU detected, defaulting to VPS_PLATFORM_CPU_REV_2_0\n");
            cpuRevEnum = VPS_PLATFORM_CPU_REV_2_0;
            break;
    }

    return (cpuRevEnum);
#endif /* PLATFORM_ZEBU */
}

Vps_PlatformBoardRev Vps_platformTI816xGetBaseBoardRev(void)
{
    Int32                   status;
    UInt32                  i2cInst = Vps_platformTI816xGetI2cInstId();
    Vps_PlatformBoardRev    boardRev = VPS_PLATFORM_BOARD_REV_UNKNOWN;

    /* In REV A boards, IO expander was not accessible */
    status = Vps_deviceI2cProbeDevice(i2cInst, VPS_BASE_BOARD_IO_EXP0_I2C_ADDR);
    if (FVID2_SOK != status)
    {
        boardRev = VPS_PLATFORM_BOARD_REV_A;
    }
    else
    {
        boardRev = VPS_PLATFORM_BOARD_REV_B;
    }

    return (boardRev);
}

Vps_PlatformBoardRev Vps_platformTI816xGetDcBoardRev(void)
{
    Int32                   status;
    UInt32                  i2cInst = Vps_platformTI816xGetI2cInstId();
    Vps_PlatformBoardRev    boardRev = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    Vps_PlatformBoardId     boardId = Vps_platformGetBoardId();

    if (VPS_PLATFORM_BOARD_VS == boardId)
    {
        /* Revision A/B of VS board are same from drivers point of view */
        boardRev = VPS_PLATFORM_BOARD_REV_B;
    }
    else if (VPS_PLATFORM_BOARD_VC == boardId)
    {
        /* From REV C VC boards, THS7353 filter is removed */
        status = Vps_deviceI2cProbeDevice(
                     i2cInst,
                     VPS_VC_BOARD_THS7353_I2C_ADDR);
        if (FVID2_SOK == status)
        {
            /* Revision A/B of VC board are same from drivers point of view */
            boardRev = VPS_PLATFORM_BOARD_REV_B;
        }
        else
        {
            boardRev = VPS_PLATFORM_BOARD_REV_C;
        }
    }
    else if (VPS_PLATFORM_BOARD_CATALOG == boardId)
    {
        /*only one Revision*/
        boardRev = VPS_PLATFORM_BOARD_REV_A;
    }

    return (boardRev);
}

/** \brief Function to enable HDVPSS clock */
static Int32 Vps_platformTI816xEnableHdVpssClk(void)
{
    volatile UInt32 repeatCnt;
    UInt32 regValue;

    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPrcmRegs);

    /* Enable power domain transition for HDVPSS */
    regValue = gVpsPrcmRegs->CM_HDDSS_CLKSTCTRL;
    regValue &= ~VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKTRCTRL_MASK;
    regValue |= (VPS_PRCM_CLKTRCTRL_SW_WKUP <<
        VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKTRCTRL_SHIFT) &
        VPS_PRCM_CM_HDVPSS_CLKSTCTRL_CLKTRCTRL_MASK;
    gVpsPrcmRegs->CM_HDDSS_CLKSTCTRL = regValue;

    /* Wait for the 100 cycles */
#ifdef PLATFORM_ZEBU
    udelay(10);
#else
    udelay(1000);
#endif

    /* Disable HDVPSS Clocks */
    regValue = gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL;
    regValue &= ~VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_MODULEMODE_MASK;
    gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL = regValue;

#ifdef PLATFORM_ZEBU
    udelay(10);
#else
    udelay(1000);
#endif

    /* Enable HDVPSS Clocks */
    regValue = gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL;
    regValue &= ~VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_MODULEMODE_MASK;
    regValue |= (VPS_PRCM_MODULE_ENABLE <<
        VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_MODULEMODE_SHIFT) &
        VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_MODULEMODE_MASK;
    gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL = regValue;

    /* Wait for the 100 cycles */
#ifdef PLATFORM_ZEBU
    udelay(10);
#else
    udelay(1000);
#endif

    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if ((gVpsPrcmRegs->CM_HDDSS_CLKSTCTRL & VPS_PRCM_HDVPSS_CLKS) ==
                VPS_PRCM_HDVPSS_CLKS)
        {
            break;
        }

        /* Wait for the 100 cycles */
#ifdef PLATFORM_ZEBU
        udelay(10);
#else
        udelay(1000);
#endif


        repeatCnt++;
    }

    if ((repeatCnt == VPS_PRCM_MAX_REP_CNT) &&
        ((gVpsPrcmRegs->CM_HDDSS_CLKSTCTRL & VPS_PRCM_HDVPSS_CLKS) !=
            VPS_PRCM_HDVPSS_CLKS))
    {
        Vps_printf(
            "Clocks are gated CM_HDDSS_CLKSTCTRL = %x\n",
            gVpsPrcmRegs->CM_HDDSS_CLKSTCTRL);
        return (FVID2_ETIMEOUT);
    }
    else
    {
        Vps_printf("=== HDVPSS Clocks are enabled ===\n");
    }

    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if (((gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL &
                    VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_IDLEST_MASK) >>
                    VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_IDLEST_SHIFT) == 0u)
        {
            break;
        }

        /* Wait for the 100 cycles */
#ifdef PLATFORM_ZEBU
        udelay(10);
#else
        udelay(100);
#endif

        repeatCnt++;
    }

    if ((repeatCnt == VPS_PRCM_MAX_REP_CNT) &&
        (((gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL &
                    VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_IDLEST_MASK) >>
                    VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_IDLEST_SHIFT) != 0u))
    {
        Vps_printf(
            "HDVPSS Idle Status CM_ACTIVE_HDDSS_CLKCTRL = %x\n",
            gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL);
        return (FVID2_ETIMEOUT);
    }
    else
    {
        Vps_printf("=== HDVPSS is fully functional ===\n");
    }

    if (((gVpsPrcmRegs->CM_ACTIVE_HDDSS_CLKCTRL &
            VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_IDLEST_MASK) >>
            VPS_PRCM_CM_ACTIVE_HDDSS_CLKCTRL_IDLEST_SHIFT) == 0u)
    {
        Vps_printf("=== HDVPSS module is not in standby ===\n");
    }
    else
    {
        Vps_printf("=== HDVPSS module is in standby ===\n");
    }

    return (FVID2_SOK);
}


#ifndef PLATFORM_ZEBU
/** \brief Function sets the SYSCLK10 to 48MHz. SysClk10 is used for CEC
     and I2C */
static Int32 Vps_platformTI816xEnableI2c(void)
{
    volatile UInt32 repeatCnt;

    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPrcmRegs);

    /* Enable Power Domain Transition */
    gVpsPrcmRegs->CM_ALWON_I2C_1_CLKCTRL = 0x2;

    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if (((gVpsPrcmRegs->CM_ALWON_I2C_1_CLKCTRL &
                    VPS_PRCM_CM_ALWON_I2C_1_CLKCTRL_IDLEST_MASK) >>
                    VPS_PRCM_CM_ALWON_I2C_1_CLKCTRL_IDLEST_SHIFT) == 0u)
        {
            break;
        }

        /* Wait for the 100 cycles */
#ifdef PLATFORM_ZEBU
        udelay(10);
#else
        udelay(100);
#endif

        repeatCnt++;
    }

    if (((gVpsPrcmRegs->CM_ALWON_I2C_1_CLKCTRL &
            VPS_PRCM_CM_ALWON_I2C_1_CLKCTRL_IDLEST_MASK) >>
            VPS_PRCM_CM_ALWON_I2C_1_CLKCTRL_IDLEST_SHIFT) != 0u)
    {
        Vps_printf("=== I2C1 Clk is Non active ===\n");
        return (FVID2_ETIMEOUT);
    }
    else
    {
        Vps_printf("=== I2C1 Clk is active ===\n");
        return (FVID2_SOK);
    }
}
#endif

/* In TI816X, TVP7002 and SiL9135A are muxed on the VC board.
   TVP7002 and SIl1161A are muxed on the catalog board*/
Int32 Vps_platformTI816xSelectVideoDecoder(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt32 i2cInstId = Vps_platformTI816xGetI2cInstId();
    UInt32 i2cDevAddr = VPS_VSVC_BOARD_IO_EXP1_I2C_ADDR;
    Int32  status = FVID2_SOK;
    UInt32 instId;

    Vps_PlatformBoardId boardId = Vps_platformGetBoardId();
    if ((VPS_PLATFORM_BOARD_VC == boardId) ||
        (VPS_PLATFORM_BOARD_CATALOG == boardId))
    {
        /* IO expander lock */
        vpsPlatformIoExpLock();
        instId = vipInstId / VPSHAL_VIP_INST_MAX;
        if (FVID2_VPS_VID_DEC_TVP7002_DRV == vidDecId)
        {

               if (instId == 0)
               {
                   gPlatformTI816xCommonObj.ioExpVal[1] &=
                        ~VPS_VC_IO_EXP_SEL_VIN0_S1_MASK;

               }
               else
               {
                    if (VPS_PLATFORM_BOARD_CATALOG == boardId)
                    {
                       gPlatformTI816xCommonObj.ioExpVal[1] &=
                            ~VPS_VC_IO_EXP_SEL_VIN1_S1_MASK;
                    }

               }
        }
        else
        {
                if ((instId == 0) && (VPS_PLATFORM_BOARD_VC == boardId))
                {
                    gPlatformTI816xCommonObj.ioExpVal[1] |=
                        VPS_VC_IO_EXP_SEL_VIN0_S1_MASK;
                }
               /*add SIL1161A for Catalog board here */
        }
        status = Vps_deviceRawWrite8(
                     i2cInstId,
                     i2cDevAddr,
                     gPlatformTI816xCommonObj.ioExpVal,
                     2);

        Task_sleep(500);

        /* IO expander unlock */
        vpsPlatformIoExpUnLock();
    }

    return (status);
}

Int32 Vps_platformTI816xEnableTvp7002Filter(FVID2_Standard standard)
{
    Int32       status = FVID2_SOK;
    UInt32      i2cInstId, i2cDevAddr;
    UInt8       filterSel;
    Vps_PlatformBoardId boardId = Vps_platformGetBoardId();
    i2cInstId = Vps_platformTI816xGetI2cInstId();
    if ((VPS_PLATFORM_BOARD_VC == boardId) ||
        (VPS_PLATFORM_BOARD_CATALOG == boardId))
    {
        if ((Vps_platformGetDcBoardRev() <= VPS_PLATFORM_BOARD_REV_B) &&
            VPS_PLATFORM_BOARD_VC == boardId)
        {
            /* For REV A and REV B VC boards, THS7353 is used */
            i2cDevAddr = VPS_VC_BOARD_THS7353_I2C_ADDR;
            status = Vps_platformEnableThs7353(standard, i2cInstId, i2cDevAddr);
        }
        else
        {
            /* From REV C VC board or catalog board, THS7368 is used which is controlled
             * through the IO expander */
            i2cDevAddr = VPS_VSVC_BOARD_IO_EXP1_I2C_ADDR;

            /* IO expander lock */
            vpsPlatformIoExpLock();

            /* Enable filter, disable bypass, clear filter select bits */
            gPlatformTI816xCommonObj.ioExpVal[0] &=
                ~(VPS_VC_IO_EXP_THS7368_DISABLE_MASK
                    | VPS_VC_IO_EXP_THS7368_BYPASS_MASK
                    | VPS_VC_IO_EXP_THS7368_FILTER1_MASK
                    | VPS_VC_IO_EXP_THS7368_FILTER2_MASK);

            switch (standard)
            {
                case FVID2_STD_1080P_60:
                case FVID2_STD_1080P_50:
                case FVID2_STD_SXGA_60:
                case FVID2_STD_SXGA_75:
                case FVID2_STD_SXGAP_60:
                case FVID2_STD_SXGAP_75:
                case FVID2_STD_UXGA_60:
                    filterSel = 0x03u;  /* Filter2: 1, Filter1: 1 */
                    break;

                case FVID2_STD_1080I_60:
                case FVID2_STD_1080I_50:
                case FVID2_STD_1080P_24:
                case FVID2_STD_1080P_30:
                case FVID2_STD_720P_60:
                case FVID2_STD_720P_50:
                case FVID2_STD_SVGA_60:
                case FVID2_STD_SVGA_72:
                case FVID2_STD_SVGA_75:
                case FVID2_STD_SVGA_85:
                case FVID2_STD_XGA_60:
                case FVID2_STD_XGA_70:
                case FVID2_STD_XGA_75:
                case FVID2_STD_XGA_85:
                case FVID2_STD_WXGA_60:
                case FVID2_STD_WXGA_75:
                case FVID2_STD_WXGA_85:
                    filterSel = 0x01u;  /* Filter2: 0, Filter1: 1 */
                    break;

                case FVID2_STD_480P:
                case FVID2_STD_576P:
                case FVID2_STD_VGA_60:
                case FVID2_STD_VGA_72:
                case FVID2_STD_VGA_75:
                case FVID2_STD_VGA_85:
                    filterSel = 0x02u;  /* Filter2: 1, Filter1: 0 */
                    break;

                case FVID2_STD_NTSC:
                case FVID2_STD_PAL:
                case FVID2_STD_480I:
                case FVID2_STD_576I:
                case FVID2_STD_D1:
                    filterSel = 0x00u;  /* Filter2: 0, Filter1: 0 */
                    break;

                default:
                    filterSel = 0x01u;  /* Filter2: 0, Filter1: 1 */
                    break;
            }

            gPlatformTI816xCommonObj.ioExpVal[0] |=
                (filterSel << VPS_VC_IO_EXP_THS7368_FILTER_SHIFT);

            status = Vps_deviceRawWrite8(
                         i2cInstId,
                         i2cDevAddr,
                         gPlatformTI816xCommonObj.ioExpVal,
                         2);

            Task_sleep(500);
            if (VPS_PLATFORM_BOARD_CATALOG == boardId)
            {
                i2cDevAddr = VPS_VSVC_BOARD_IO_EXP2_I2C_ADDR;
                status |= Vps_deviceRawWrite8(
                             i2cInstId,
                             i2cDevAddr,
                             gPlatformTI816xCommonObj.ioExpVal,
                             2);

                Task_sleep(500);
            }
            /* IO expander unlock */
            vpsPlatformIoExpUnLock();
        }
    }

    return (status);
}

Int32 Vps_platformTI816xSelectHdCompSyncSource(
        Vps_PlatformHdCompSyncSource syncSrc,
        UInt32 enable)
{
    Int32       status = FVID2_SOK;

    if (Vps_platformGetCpuRev() >= VPS_PLATFORM_CPU_REV_2_0)
    {
        if (VPS_PLATFORM_HDCOMP_SYNC_SRC_DVO1 == syncSrc)
        {
            if (enable)
            {
                REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0724) |= 0x02u;
            }
            else
            {
                REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0724) &= ~0x02u;
            }
        }
        else if (VPS_PLATFORM_HDCOMP_SYNC_SRC_DVO2 == syncSrc)
        {
            if (enable)
            {
                REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0724) |= 0x04u;
            }
            else
            {
                REG32(CSL_TI816x_CTRL_MODULE_BASE + 0x0724) &= ~0x04u;
            }
        }
        else
        {
            status = FVID2_EFAIL;
        }
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}

static void udelay(int delay_usec)
{
    Int32 delay_msec;

    delay_msec = delay_usec/1000;
    if(delay_msec==0)
        delay_msec = 1;
    Task_sleep(delay_msec);
}

static Int32 vpsPlatformIoExpLock(void)
{
    Semaphore_pend(gPlatformTI816xCommonObj.glock, BIOS_WAIT_FOREVER);

    return (FVID2_SOK);
}

static Int32 vpsPlatformIoExpUnLock(void)
{
    Semaphore_post(gPlatformTI816xCommonObj.glock);

    return (FVID2_SOK);
}

static Int32 vpsPlatformCalcVpllClkParams(UInt32 outputFreq, Vps_VideoPllCtrl *vpllCtrl)
{
    UInt32 P = 0;
    UInt32 N = 0;
    UInt32 M = 0;
    UInt32 solFound = 0;
    UInt32 fractCnt = 0;
    UInt32 fractMatch = 0;
    UInt32 solWithFracJitter = 0;
    UInt32 fractHex = 0;
    double Fr = 0;
    double Fo = 0;
    double FREQ_Float = 0;
    double VCO_Freq_Float = 0;
    double Fraction = 0;
    double Fractional_Jitter = 0;
    double Fractional_Jitter_Max = 0;
    double a, b;

    solWithFracJitter = 1;
    Fractional_Jitter_Max = 200;

    solFound = 0;

    Fr = VPS_PLATFORM_VPLL_INPUT_FREQ;
    Fo = (double)outputFreq/1000.0;
    P = VPS_PLATFORM_VPLL_VAL_P;
    N = VPS_PLATFORM_VPLL_VAL_N;

    for (M = VPS_PLATFORM_VPLL_MIN_M; M < VPS_PLATFORM_VPLL_MAX_M; M ++)
    {
        /* Calculate double precision FREQ based on inputs */
        FREQ_Float = ((N * 8 * Fr) / (P * M * Fo));

        /* Check if double precision FREQ is within allowable range */
        if ((FREQ_Float >= VPS_PLATFORM_VPLL_MIN_FREQ) &&
            (FREQ_Float < VPS_PLATFORM_VPLL_MAX_FREQ))
        {
            /* Check if calculated VCO frequency is within allowable range */
            VCO_Freq_Float = ((double)N / P) * Fr;
            if ((VCO_Freq_Float >= VPS_PLATFORM_VPLL_MIN_VCO_FREQ) &&
                (VCO_Freq_Float <= VPS_PLATFORM_VPLL_MAX_VCO_FREQ))
            {
                /* Check that the double precision FREQ does not
                   produce fractional jitter */
                a = (UInt32)(FREQ_Float * M);
                b = (FREQ_Float * M);
                if (a == b)
                {
                    /* Check if calculated FREQ can be exactly represented
                       with 24-bit fraction */
                    Fraction = 0;
                    fractMatch = 0;

                    /* Check if FREQ is an integer */
                    if (FREQ_Float == (UInt32)(FREQ_Float))
                    {
                        fractMatch = 1;
                    }

                    /* Round FREQ to the nearest 24-bit fractional value */
                    for (fractCnt = 1; fractCnt <= 24; fractCnt++)
                    {
                        if (fractMatch == 1)
                        {
                            fractHex <<= 1u;
                            fractHex |= 0x0u;
                        }
                        else if ((Fraction + 1 / ((double)(1 << fractCnt))) ==
                                    (FREQ_Float - (UInt32)(FREQ_Float)))
                        {
                            Fraction = Fraction + 1 / ((double)(1 << fractCnt));
                            fractMatch = 1;
                            fractHex <<= 1u;
                            fractHex |= 0x1u;
                        }
                        else if ((Fraction + 1 / ((double)(1 << fractCnt))) <
                                    (FREQ_Float - (UInt32)(FREQ_Float)))
                        {
                            Fraction = Fraction + 1 / ((double)(1 << fractCnt));
                            fractHex <<= 1u;
                            fractHex |= 0x1u;
                        }
                        else
                        {
                            fractHex <<= 1u;
                            fractHex |= 0x0u;
                        }
                    }
                    /* Check if 24-bit FREQ produces no fractional
                       jitter when the truncation bit is not set */
                    if (fractMatch == 1)
                    {
                        /* Found a 24-bit FREQ that doesn't product
                           fractional jitter ( truncation bit not set) */
                        solFound = solFound + 1;

                        vpllCtrl->VidFlyAddParams.intFreq =
                            (UInt32)FREQ_Float;
                        vpllCtrl->VidFlyAddParams.fractFreq =
                            fractHex & 0xFFFFFFu;
                        vpllCtrl->VidFlyAddParams.M = (UInt32)M;
                        vpllCtrl->VidFlyAddParams.enableTrunct = 0;
                        vpllCtrl->sysClkDiv = 0;
                        break;
                    }
                    else if ((FREQ_Float * M) ==
                                (((UInt32)(FREQ_Float) + Fraction) * M) +
                                    (1 / (double)(1 << 24)))
                    {
                        solFound = solFound + 1;
                        vpllCtrl->VidFlyAddParams.intFreq =
                            (UInt32)FREQ_Float;
                        vpllCtrl->VidFlyAddParams.fractFreq =
                            fractHex & 0xFFFFFFu;
                        vpllCtrl->VidFlyAddParams.M = (UInt32)M;
                        vpllCtrl->VidFlyAddParams.enableTrunct = 1;
                        vpllCtrl->sysClkDiv = 0;

                        break;
                    }
                }
            }
        }
    }


    if (0 == solFound) {
        for (M = 1; M <= 255; M++)
        {
            /* Calculate double precision FREQ based on inputs */
            FREQ_Float = ((N * 8 * Fr) / (P * M * Fo));

            /* Check if double precision FREQ is within allowable range */
            if ((FREQ_Float >= VPS_PLATFORM_VPLL_MIN_FREQ) &&
                (FREQ_Float < VPS_PLATFORM_VPLL_MAX_FREQ))
            {

                /* Check if calculated VCO frequency is within
                   allowable range */
                VCO_Freq_Float = (N / P) * Fr;
                if ((VCO_Freq_Float >= VPS_PLATFORM_VPLL_MIN_VCO_FREQ) &&
                    (VCO_Freq_Float <= VPS_PLATFORM_VPLL_MAX_VCO_FREQ))
                {
                    if (solWithFracJitter)
                    {
                        /* Check if calculated FREQ can be exactly
                           represented with 24-bit fraction */
                        Fraction = 0;
                        fractMatch = 0;
                        fractHex = 0u;
                        if (FREQ_Float == (UInt32)(FREQ_Float))
                        {
                            fractMatch = 1;
                        }
                        else
                        {
                            for (fractCnt = 1; fractCnt <= 24; fractCnt++)
                            {
                                if ((Fraction + 1 / ((double)(1 << fractCnt))) <
                                    (FREQ_Float - (UInt32)(FREQ_Float)))
                                {
                                    Fraction = Fraction + 1 /
                                                    ((double)(1 << fractCnt));
                                    fractHex <<= 1u;
                                    fractHex |= 0x1u;
                                }
                                else
                                {
                                    fractHex <<= 1u;
                                    fractHex |= 0x0u;
                                }
                            }
                        }

                        Fractional_Jitter =
                            (1 / (Fr * N / P) / 1000000 / 8) * 1000000000000.0;

                        if (Fractional_Jitter < Fractional_Jitter_Max)
                        {
                            solWithFracJitter = 0;
                            solFound = 1;
                            vpllCtrl->VidFlyAddParams.intFreq =
                                (UInt32)FREQ_Float;
                            vpllCtrl->VidFlyAddParams.fractFreq =
                                fractHex & 0xFFFFFFu;
                            vpllCtrl->VidFlyAddParams.M = (UInt32)M;
                            vpllCtrl->VidFlyAddParams.enableTrunct = 0;
                            vpllCtrl->sysClkDiv = 0;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (solFound)
    {
        return (FVID2_SOK);
    }
    else
    {
        return (FVID2_EALLOC);
    }
}
