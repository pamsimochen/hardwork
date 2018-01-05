/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_platformTI8107.c
 *
 *  \brief Implements the TI8107 platform specific functions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/cslr/soc_TI8107.h>
#include <ti/psp/cslr/cslr_TI814xprcm.h>
#include <ti/psp/cslr/cslr_TI814xpll.h>
#include <ti/psp/vps/hal/vpshal_vip.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/devices/vps_videoDecoder.h>
#include <ti/psp/platforms/ti8107/vps_platformTI8107.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define ENABLE_HDVPSS_CLK

/* Enable I2C control to configure ecn/dec */
#define ENABLE_I2C_CLK

/* Set the pin mux */
#define CONFIG_PIN_MUX

/* Set the interrupt mux */
#define CONFIG_INT_MUX

/* Set the PLLs */
#define CONFIG_PLL

//#define ENABLE_I2C_PROBE_ON_INIT

#define VPS_VS_BOARD_IO_EXP_I2C_ADDR    (0x21u)

#define VPS_VC_BOARD_A1_IO_EXP_I2C_ADDR (0x27u)
#define VPS_VC_BOARD_A2_IO_EXP_I2C_ADDR (0x21u)

#define VPS_CA_BOARD_A1_IO_EXP_I2C_ADDR (0x21u)

/* THS7353 filter I2C address present in VC daughter card */
#define VPS_VC_BOARD_THS7353_I2C_ADDR   (0x2Cu)

#define VPS_PLATFORM_EVM_I2C_INST_ID    (VPS_DEVICE_I2C_INST_ID_2)

/** \brief PLL Control Module Base Address*/
#define VPS_CONTROL_MODULE_PLL_CTRL_BASE_ADDR   (CSL_TI8107_PLL_BASE)

#define VPS_HD_DAC_CLKSRC_REG_ADDR      (CSL_TI8107_PLL_BASE + 0x02F8u)

/** \brief Control Module Device Configuration Base Address */
#define VPS_CTRL_MODULE_DEV_CFG_BASE_ADDR       (CSL_TI8107_CTRL_MODULE_BASE + \
                                                    0x0600u)

/* Default Values for DDR PLL configuration to get clock for I2C and CEC */
#define VPS_DDR_INT_FREQ2               (0x8u)
#define VPS_DDR_FRACT_FREQ2             (0xD99999u)
#define VPS_DDR_MDIV2                   (0x1Eu)
#define VPS_DDR_SYCCLK10_DIV            (0x0u)

/* gpio base addresses  */
#define REG32                           *(volatile unsigned int*)

#define VPS_PRCM_CLKTRCTRL_NO_SLEEP     (0u)
#define VPS_PRCM_CLKTRCTRL_SW_SLEEP     (1u)
#define VPS_PRCM_CLKTRCTRL_SW_WKUP      (2u)
#define VPS_PRCM_CLKTRCTRL_HW_AUTO      (3u)

#define VPS_PRCM_MODULE_DISABLE         (0u)
#define VPS_PRCM_MODULE_ENABLE          (2u)

#define VPS_PRCM_MAX_REP_CNT            (100u)

/* ADPLLJ_CLKCRTL_Register Value Configurations
ADPLLJ_CLKCRTL_Register SPEC bug  bit 19,bit29 -- CLKLDOEN,CLKDCOEN */
#define ADPLLJ_CLKCRTL_HS2              (0x00000801u)
/*HS2 Mode,TINTZ =1  --used by all PLL's except HDMI */
#define ADPLLJ_CLKCRTL_HS1              (0x00001001u)
/* HS1 Mode,TINTZ =1  --used only for HDMI  */
#define ADPLLJ_CLKCRTL_CLKDCO           (0x200a0000u)
/* Enable CLKDCOEN,CLKLDOEN,CLKDCOPWDNZ -- used for HDMI,USB */
#define VPS_TI8107_KHz                  (1000u)
#define VPS_TI8107_MHz                  (VPS_TI8107_KHz * VPS_TI8107_KHz)
#define VPS_TI8107_EVM_OSC_FREQ         (20u * VPS_TI8107_MHz)

/* Defines specific to on-board peripherals */
#define VPS_PCF8575_P0_7_P0_MASK        (0x01u)
#define VPS_PCF8575_P0_7_P4_MASK        (0x10u)
#define VPS_PCF8575_P0_7_P5_MASK        (0x20u)
#define VPS_PCF8575_P0_7_P6_MASK        (0x40u)
#define VPS_PCF8575_P0_7_P7_MASK        (0x80u)

#define VPS_PCF8575_P10_17_P16_MASK     (0x40u)
#define VPS_PCF8575_P10_17_P17_MASK     (0x80u)

#define VPS_TI8107_CM_VPS_TI8107_TIMER7_CLKSEL     (CSL_TI8107_PRCM_BASE + 0x03A8)
#define VPS_TI8107_CM_ALWON_TIMER_7_CLKCTRL    (CSL_TI8107_PRCM_BASE + 0x1588)

#define VPS_TI8107_TIMER7_BASE_ADDR     (0x4804A000)

#define VPS_TI8107_TIMER7_TIDR           (VPS_TI8107_TIMER7_BASE_ADDR + 0x00)
#define VPS_TI8107_TIMER7_TIOCP_CFG      (VPS_TI8107_TIMER7_BASE_ADDR + 0x10)
#define VPS_TI8107_TIMER7_IRQ_EOI        (VPS_TI8107_TIMER7_BASE_ADDR + 0x20)
#define VPS_TI8107_TIMER7_IRQSTAT_RAW    (VPS_TI8107_TIMER7_BASE_ADDR + 0x24)
#define VPS_TI8107_TIMER7_IRQSTATUS      (VPS_TI8107_TIMER7_BASE_ADDR + 0x28)
#define VPS_TI8107_TIMER7_IRQSTAT_SET    (VPS_TI8107_TIMER7_BASE_ADDR + 0x2C)
#define VPS_TI8107_TIMER7_IRQSTAT_CLR    (VPS_TI8107_TIMER7_BASE_ADDR + 0x30)
#define VPS_TI8107_TIMER7_IRQWAKEEN      (VPS_TI8107_TIMER7_BASE_ADDR + 0x34)
#define VPS_TI8107_TIMER7_TCLR           (VPS_TI8107_TIMER7_BASE_ADDR + 0x38)
#define VPS_TI8107_TIMER7_TCRR           (VPS_TI8107_TIMER7_BASE_ADDR + 0x3C)
#define VPS_TI8107_TIMER7_TLDR           (VPS_TI8107_TIMER7_BASE_ADDR + 0x40)
#define VPS_TI8107_TIMER7_TTGR           (VPS_TI8107_TIMER7_BASE_ADDR + 0x44)
#define VPS_TI8107_TIMER7_TWPS           (VPS_TI8107_TIMER7_BASE_ADDR + 0x48)
#define VPS_TI8107_TIMER7_TMAR           (VPS_TI8107_TIMER7_BASE_ADDR + 0x4C)
#define VPS_TI8107_TIMER7_TCAR1          (VPS_TI8107_TIMER7_BASE_ADDR + 0x50)
#define VPS_TI8107_TIMER7_TSICR          (VPS_TI8107_TIMER7_BASE_ADDR + 0x54)
#define VPS_TI8107_TIMER7_TCAR2          (VPS_TI8107_TIMER7_BASE_ADDR + 0x58)

#define VPS_TI8107_GPIO2_BASE           (0x481AC000u)
#define VPS_TI8107_CM_ALWON_GPIO_0_CLKCTRL     (CSL_TI8107_PRCM_BASE + 0x155C)
#define VPS_TI8107_CM_ALWON_GPIO_1_CLKCTRL     (CSL_TI8107_PRCM_BASE + 0x1560)

#define VPS_TI8107_GPIO2_SYSCONFIG      (VPS_TI8107_GPIO2_BASE + 0x010)
#define VPS_TI8107_GPIO2_OE             (VPS_TI8107_GPIO2_BASE + 0x134)
#define VPS_TI8107_GPIO2_DATAOUT        (VPS_TI8107_GPIO2_BASE + 0x13C)

#define GP2_9                           (1 << 9)


/*  VC A2
 *  PCF8575 - mappings
 *  P7 - THS73861_FILTER2
 *  P6 - THS73861_FILTER1
 *  P5 - THS73861_BYPASS
 *  P4 - THS73861_DISABLE
 *  P0 - TVP7002_RSTN
 */
#define VPS_VC_A2_PCF8575_TVP_RESETn_MASK       (VPS_PCF8575_P0_7_P0_MASK)
#define VPS_VC_A2_PCF8575_THS73861_DISABLE_MASK (VPS_PCF8575_P0_7_P4_MASK)
#define VPS_VC_A2_PCF8575_THS73861_BYPASS_MASK  (VPS_PCF8575_P0_7_P5_MASK)
#define VPS_VC_A2_PCF8575_THS73861_FILTER1_MASK (VPS_PCF8575_P0_7_P6_MASK)
#define VPS_VC_A2_PCF8575_THS73861_FILTER2_MASK (VPS_PCF8575_P0_7_P7_MASK)

/*  Catalog A1
 *  PCF8575 - mappings
 *  P17 - THS7368_S1
 *  P16 - THS7368_S0
 *  P7 - THS7368_FILTER2
 *  P6 - THS7368_FILTER1
 *  P5 - THS7368_BYPASS
 *  P4 - THS7368_DISABLE
 *  P0 - TVP7002_RSTN
 */
#define VPS_CA_A1_PCF8575_TVP_RESETn_MASK       (VPS_PCF8575_P0_7_P0_MASK)
#define VPS_CA_A1_PCF8575_THS7368_DISABLE_MASK  (VPS_PCF8575_P0_7_P4_MASK)
#define VPS_CA_A1_PCF8575_THS7368_BYPASS_MASK   (VPS_PCF8575_P0_7_P5_MASK)
#define VPS_CA_A1_PCF8575_THS7368_FILTER1_MASK  (VPS_PCF8575_P0_7_P6_MASK)
#define VPS_CA_A1_PCF8575_THS7368_FILTER2_MASK  (VPS_PCF8575_P0_7_P7_MASK)

#define VPS_CA_A1_PCF8575_SEL_TVP_S0_MASK       (VPS_PCF8575_P10_17_P16_MASK)
#define VPS_CA_A1_PCF8575_SEL_TVP_S1_MASK       (VPS_PCF8575_P10_17_P17_MASK)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* \brief structure to keep track of pll configurations for a video mode */
typedef struct
{
    UInt32                  __n;
    /**< Divider N for the PLL.*/
    UInt32                  __m;
    /**< Multiplier M for the PLL.*/
    UInt32                  __m2;
    /**< Divider M2 for the PLL.*/
    UInt32                  clkCtrlValue;
    /**< For comparison based on the clkOut used */
} Vps_VideoPllCtrl;

/* Structure to track the versions of boards */
typedef struct
{
    Vps_PlatformBoardRev    vcCard;
    UInt32                  vcIoExpAddr;
    Semaphore_Handle        ioExpLock;
    Vps_PlatformBoardRev    vsCard_notused;         /* Not used as of now */
    Vps_PlatformBoardRev    baseBoard_notused;      /* Not used as of now */
    Vps_PlatformBoardRev    caCard;
    UInt32                  caIoExpAddr;
}Vps_BoardVersion;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
#ifdef CONFIG_PIN_MUX
static Int32 Vps_platformTI8107SetPinMux(void);
#endif

#ifdef CONFIG_INT_MUX
static Int32 Vps_platformTI8107SetIntMux(void);
#endif

#ifdef ENABLE_HDVPSS_CLK
static Int32 Vps_platformTI8107EnableHdVpssClk(void);
#endif

#ifdef ENABLE_I2C_CLK
static Int32 Vps_platformTI8107EnableI2c(void);
#endif

#if  defined(CONFIG_PLL) || defined(ENABLE_HDVPSS_CLK) || defined(ENABLE_I2C_CLK)
static void udelay(int delay_usec);
#endif

#if  defined(CONFIG_PLL) || defined(ENABLE_HDVPSS_CLK)
Int32 Vps_platformPllCfg(UInt32 baseAddr,
                  UInt32 N,
                  UInt32 M,
                  UInt32 M2,
                  UInt32 clkCtrlValue);
static Int32 Vps_platformTI8107ConfigHdVpssPll(void);
static Int32 Vps_getDividers(Vps_VideoPllCtrl *config, UInt32 reqOutClk,
                            UInt32 vencClk);
#endif

static Int32 Vps_getVcCardVersion(Vps_BoardVersion *boardVer);
static Int32 Vps_platformTi814xEnableThs73681(FVID2_Standard standard,
                                              UInt32 i2cInst,
                                              UInt32 ioExpAddr);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

#if defined (ENABLE_HDVPSS_CLK) || defined (ENABLE_I2C_CLK)
static CSL_PrcmRegs gVpsPrcmRegs = (CSL_PrcmRegs) CSL_TI8107_PRCM_BASE;
#endif

#if defined (CONFIG_PLL) || defined (ENABLE_HDVPSS_CLK)
static CSL_PllCtrlRegs gVpsPllCtrlRegs =
            (CSL_PllCtrlRegs) VPS_CONTROL_MODULE_PLL_CTRL_BASE_ADDR;
#endif

/**< Variable to track the version of daughter card, we have Aplha 1 and
     Alpha 2, designated as REV A and REV B */
static Vps_BoardVersion gTi814xBoardVer = {VPS_PLATFORM_BOARD_REV_UNKNOWN,  \
                                           VPS_VC_BOARD_A1_IO_EXP_I2C_ADDR, \
                                           VPS_PLATFORM_BOARD_REV_UNKNOWN,  \
                                           VPS_PLATFORM_BOARD_REV_UNKNOWN};
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Int32 Vps_platformTI8107Init(Vps_PlatformInitParams *initParams)
{
    Int32 status = FVID2_SOK;

#ifdef CONFIG_PIN_MUX
    if (TRUE == initParams->isPinMuxSettingReq)
    {
        Vps_platformTI8107SetPinMux();
    }
#endif

#ifdef CONFIG_INT_MUX
        Vps_platformTI8107SetIntMux();
#endif

#ifdef ENABLE_HDVPSS_CLK
    /* Initialize Pixel Clock */
    status |= Vps_platformTI8107ConfigHdVpssPll();
    status |= Vps_platformTI8107EnableHdVpssClk();
#endif

#ifdef ENABLE_I2C_CLK
#ifndef PLATFORM_ZEBU
    status |= Vps_platformTI8107EnableI2c();
#endif
#endif

    return (status);
}

Int32 Vps_platformTI8107DeInit(void)
{
    Int32 status = FVID2_SOK;

    return (status);
}

/* Init EVM related sub-systems like I2C instance */
Int32 Vps_platformTI8107DeviceInit(Vps_PlatformDeviceInitParams *initPrms)
{
    Int32 status = FVID2_SOK;
    Semaphore_Params semParams;
#ifdef PLATFORM_EVM_SI
    UInt8 i2cCnt = 0;
    /* TI8107 has 4 I2C instances. */
    UInt32 i2cRegs[VPS_DEVICE_I2C_INST_ID_MAX] = {CSL_TI8107_I2C0_BASE,
                                                  CSL_TI8107_I2C1_BASE,
                                                  CSL_TI8107_I2C2_BASE,
                                                  CSL_TI8107_I2C3_BASE};
    UInt32 i2cInt[VPS_DEVICE_I2C_INST_ID_MAX] = {CSL_INTC_EVENTID_I2CINT0,
                                                 CSL_INTC_EVENTID_I2CINT1,
                                                 CSL_INTC_EVENTID_I2CINT2,
                                                 CSL_INTC_EVENTID_I2CINT3};

    Vps_DeviceInitParams deviceInitPrm;

#ifdef PLATFORM_ZEBU
    return status;
#endif
    /*
     * External video device subsystem init
     */
    memset ( &deviceInitPrm, 0, sizeof ( deviceInitPrm ) );

    /* Initialize file locals */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    gTi814xBoardVer.ioExpLock = Semaphore_create(1u, &semParams, NULL);
    if (NULL == gTi814xBoardVer.ioExpLock)
    {
        status = FVID2_EALLOC;
        return (status);
    }
    Semaphore_pend(gTi814xBoardVer.ioExpLock, BIOS_WAIT_FOREVER);

    gTi814xBoardVer.vcCard              = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    gTi814xBoardVer.vcIoExpAddr         = VPS_VC_BOARD_A1_IO_EXP_I2C_ADDR;
    gTi814xBoardVer.vsCard_notused      = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    gTi814xBoardVer.baseBoard_notused   = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    gTi814xBoardVer.caCard              = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    gTi814xBoardVer.caIoExpAddr         = VPS_CA_BOARD_A1_IO_EXP_I2C_ADDR;


    Semaphore_post(gTi814xBoardVer.ioExpLock);

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

    /* TI8107 uses only I2C[2], so modify the sampling frequency */
    deviceInitPrm.i2cRegs[VPS_PLATFORM_EVM_I2C_INST_ID]
        = (Ptr)CSL_TI8107_I2C2_BASE;
    deviceInitPrm.i2cIntNum[VPS_PLATFORM_EVM_I2C_INST_ID]
        = CSL_INTC_EVENTID_I2CINT2;
    deviceInitPrm.i2cClkKHz[VPS_PLATFORM_EVM_I2C_INST_ID]
        = 400;

#ifdef TI_8107_BUILD
#ifdef _IPNC_HW_PLATFORM_EVM_		
    deviceInitPrm.i2cClkKHz[VPS_PLATFORM_EVM_I2C_INST_ID]
        = 50;
#endif	
#endif	

    status = Vps_deviceInit ( &deviceInitPrm );

#ifdef ENABLE_I2C_PROBE_ON_INIT
    if (TRUE == initPrms->isI2cInitReq &&
        TRUE == initPrms->isI2cProbingReq)
    {
        Vps_deviceI2cProbeAll(Vps_platformGetI2cInstId());
    }
#endif
#endif

    if (status == FVID2_SOK)
    {
        /* Right now, VC daughter card has different versions,
           Base board versions - does not require any special operations
           VS - we do not have multiple versions */
        if(Vps_platformGetBoardId() == VPS_PLATFORM_BOARD_VC)
        {
            status = Vps_getVcCardVersion(&gTi814xBoardVer);
            /* Two versions of VC cards */
            GT_assert( GT_DEFAULT_MASK, (gTi814xBoardVer.vcCard == VPS_PLATFORM_BOARD_REV_B) ||
                    (gTi814xBoardVer.vcCard == VPS_PLATFORM_BOARD_REV_A));
        }
    }
    return (status);
}

/* De-Init EVM related sub-systems */
Int32 Vps_platformTI8107DeviceDeInit(void)
{
#ifdef PLATFORM_ZEBU
    return 0;
#endif


#ifdef PLATFORM_EVM_SI
    /*
     * Extern video device de-init
     */
    Vps_deviceDeInit (  );

#endif

    return (FVID2_SOK);
}

UInt32 Vps_platformTI8107GetI2cInstId(void)
{
    return (VPS_PLATFORM_EVM_I2C_INST_ID);
}

UInt8 Vps_platformTI8107GetVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt8 devAddr;
    UInt8 devAddrTvp5158[VPS_CAPT_INST_MAX] = { 0x58, 0x5c, 0x5a, 0x5e };

    //TODO
    UInt8 devAddrSii9135[VPS_CAPT_INST_MAX] = { 0x00, 0x00, 0x30, 0x00 };
    UInt8 devAddrTvp7002[VPS_CAPT_INST_MAX] = { 0x5d, 0x00, 0x00, 0x00 };

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
            GT_0trace(GT_DEFAULT_MASK, GT_ERR, "Invalid decoder ID\n");
            break;
    }

    return (devAddr);
}

UInt8 Vps_platformTI8107GetVidEncI2cAddr(UInt32 vidEncId)
{
    UInt8 devAddr;
    Vps_PlatformBoardId boardId;
    /* There can be two Sii9022 instances, one on VS and other on VC board.
     * devAddrSii9022[0] -> VS Sii9022
     * devAddrSii9022[1] -> VC Sii9022
     */
    UInt8 devAddrSii9022[2u] = { 0x39, 0x39 };

    if (vidEncId == FVID2_VPS_VID_ENC_SII9022A_DRV)
    {
        boardId = Vps_platformGetBoardId();
        switch (boardId)
        {
            case VPS_PLATFORM_BOARD_VS:
                devAddr = devAddrSii9022[0];
                break;

            case VPS_PLATFORM_BOARD_VC:
                devAddr = devAddrSii9022[1];
                break;

            default:
                GT_0trace(GT_DEFAULT_MASK, GT_ERR, "Invalid Board ID\n");
                break;
        }
    }

    return (devAddr);
}

Int32 Vps_platformTI8107SimVideoInputSelect(UInt32 vipInstId,
                                            UInt32 fileId,
                                            UInt32 pixelClk)
{
    volatile UInt32 *pRegs[2];
    UInt32 instId, portId;

    pRegs[0] = (UInt32 *)(CSL_TI8107_VPS_BASE+0xE100);
    pRegs[1] = (UInt32 *)(CSL_TI8107_VPS_BASE+0xE200);

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

#if defined(CONFIG_PLL) || defined(ENABLE_HDVPSS_CLK)
static Int32 Vps_platformTI8107ConfigHdVpssPll(void)
{
    UInt32 baseAddr;
    baseAddr = (UInt32)&gVpsPllCtrlRegs->DSSPLL_PWRCTRL;

    /* set for 240Mhz HDVPSS Clock */
    Vps_platformPllCfg(baseAddr, 19, 960, 4, ADPLLJ_CLKCRTL_HS2);

    return (FVID2_SOK);
}
#endif

Int32 Vps_platformTI8107SetVencPixClk(Vps_SystemVPllClk *vpllCfg)
{
#ifdef ENABLE_HDVPSS_CLK
    Vps_VideoPllCtrl pllDividers;
    Int32 rtnValue = FVID2_SOK;
    Vps_VPllOutputClk  pllOutputClk;
    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPllCtrlRegs);
    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPrcmRegs);
    GT_assert( GT_DEFAULT_MASK, NULL != vpllCfg);

    if (VPS_SYSTEM_VPLL_OUTPUT_VENC_RF == vpllCfg->outputVenc)
    {
        GT_assert( GT_DEFAULT_MASK, 54000u  == vpllCfg->outputClk);
    }
    else if ((VPS_SYSTEM_VPLL_OUTPUT_VENC_A == vpllCfg->outputVenc) ||
            (VPS_SYSTEM_VPLL_OUTPUT_VENC_D == vpllCfg->outputVenc))
    {
        GT_assert( GT_DEFAULT_MASK, 54000u != vpllCfg->outputClk);
    }

    if (rtnValue == FVID2_SOK)
    {
        rtnValue = Vps_getDividers(&pllDividers, vpllCfg->outputClk, vpllCfg->outputVenc);

        if (rtnValue == FVID2_SOK)
        {
            /*HDMI shares the same PLL as VENC_A*/
            if (vpllCfg->outputVenc == VPS_SYSTEM_VPLL_OUTPUT_HDMI)
                pllOutputClk = VPS_SYSTEM_VPLL_OUTPUT_VENC_A;
            else
                pllOutputClk = (Vps_VPllOutputClk)vpllCfg->outputVenc;

                rtnValue = Vps_platformPllCfg((UInt32)
                &gVpsPllCtrlRegs->VideoPll_Factors[pllOutputClk].PWRCTRL,
                pllDividers.__n,
                pllDividers.__m,
                pllDividers.__m2,
                pllDividers.clkCtrlValue);
        }
    }
    return (rtnValue);
#else
    return (FVID2_SOK);
#endif /* ENABLE_HDVPSS_CLK */
}

#if  defined(CONFIG_PLL) || defined(ENABLE_HDVPSS_CLK)
/*******************************************************************************
****
****                                   ********* RANGE ************
****   REF_CLK       = (OSC_FREQ)/N+1  [  REF_CLK < 2.5MHz      ]
****   DCOCLK_HS2    = (REF_CLK)*M     [500  < DCOCLK < 1000MHz ]
****   DCOCLK_HS1    = (REF_CLK)*M     [1000 < DCOCLK < 2000MHz ]--used for HDMI CLKDCO
****   CLKOUT        =  DCOCLK/M2      [10   < CLKOUT < 2000MHz ]--used for DVO2, SD
****   N+1                             [1..256]
****   M                               [2..4095]
****   M2                              [1..127]
****
****
*******************************************************************************/
/* Get the divider value for video PLL for the specified frequency.
   N requires to start with 19 - TODO check with 0x0 */
static Int32 Vps_getDividers(Vps_VideoPllCtrl *config, UInt32 reqOutClk,
                            UInt32 vencClk)
{
    Int32   rtnValue = FVID2_EFAIL;
    Int32   n, m, m2;
    float   refClk, dcoClk, clkOut;
    UInt8   hsMode = 1; /* 1: HS2, 2:HS1*/
    UInt8   factor = 1;
    UInt32   padding;
    /* The input clock is specified in terms of KiloHertz, require MHz to work
       with */
    if (vencClk == VPS_SYSTEM_VPLL_OUTPUT_HDMI)
    {
        padding = reqOutClk % 500;
        if (padding)
        {
            if (padding >= 250)
                reqOutClk += 500 - padding;
            else
                reqOutClk -=padding;
        }
        /*if Freq is less than 50MHz, post-M2 clock is used to drive
        HDMI, therefore factor should b1. the other case factor is 10 since
        pre-M2 clock is to driver the HDMI
        check HDMI wrapper spec and TI8107 Clock Arch document to
        further understand this*/
        if (reqOutClk >= 50000)
                factor = 10;
        /*CLK must be 10x if it is for HDMI*/
        reqOutClk *= 1000u * 10;
    }
    else
    {
        factor = 1;
        reqOutClk *= 1000u;
    }
    if (reqOutClk > 1000 * VPS_TI8107_MHz)
        /*must use HS1 mode*/
        hsMode = 2;

    for (n = 19; ((n < 256u) && (rtnValue != FVID2_SOK)); n++)
    {
        refClk = VPS_TI8107_EVM_OSC_FREQ / (n + 1);
        if ( refClk < (2.5 * VPS_TI8107_MHz))
        {
            for (m = 2; ((m < 4095) && (rtnValue != FVID2_SOK)); m++)
            {
                dcoClk = (refClk) * m;
                if ((dcoClk > (500 * VPS_TI8107_MHz * hsMode)) &&
                    (dcoClk < (1000 * VPS_TI8107_MHz * hsMode)))
                {
                    for (m2 = 1; m2 <= 127; m2++)
                    {
                        clkOut = dcoClk / m2;
                        if (clkOut  == reqOutClk)
                        {
                            config->__n     = n;
                            config->__m     = m;
                            config->__m2    = m2 * factor;
                            if (hsMode == 1)
                                config->clkCtrlValue = ADPLLJ_CLKCRTL_HS2;
                            else
                                config->clkCtrlValue = ADPLLJ_CLKCRTL_HS1;
                            /*enable the CLKDCOLDO and CLKOUTLDO for HDMI*/
                            if (vencClk == VPS_SYSTEM_VPLL_OUTPUT_HDMI)
                                config->clkCtrlValue |= ADPLLJ_CLKCRTL_CLKDCO;
                            rtnValue = FVID2_SOK;
                            break;
                        }
                    }
                }
            }
            /* Work around for the hardware bug where it failed to set
               54k frequency with generated with the values generated by
               getdivider function - Essentially m = 540 and M2 == 10 will not
               work */
            if(reqOutClk == (54000u * 1000u))
            {
                config->__n     = 19;
                config->__m     = 1080;
                config->__m2    = 20;
                break;
            }
        }
    }
    return (rtnValue);
}
#endif

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

    I2C2
    - SDA               - ENABLED
    - SCL               - ENABLED

    HDMI I2C0
    - SDA               - ENABLED
    - SCL               - ENABLED
*/
#ifdef CONFIG_PIN_MUX
static Int32 Vps_platformTI8107SetPinMux(void)
{
    /* Vout 0 configuration DVO2 Function 1*/
    /* TODO There are two pins for the fid. Need to see whichone is used */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AB8) = 0x1;   /* vout0_fid_mux1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0ABC) = 0x1;   /* vout0_clk */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AC0) = 0x1;   /* vout0_hsync */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AC4) = 0x1;   /* vout0_vsync */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AC8) = 0x1;   /* vout0_avid */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0ACC) = 0x1;   /* vout0_b_cb_c[2] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AD0) = 0x1;   /* vout0_b_cb_c[3] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AD4) = 0x1;   /* vout0_b_cb_c[4] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AD8) = 0x1;   /* vout0_b_cb_c[5] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0ADC) = 0x1;   /* vout0_b_cb_c[6] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AE0) = 0x1;   /* vout0_b_cb_c[7] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AE4) = 0x1;   /* vout0_b_cb_c[8] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AE8) = 0x1;   /* vout0_b_cb_c[9] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AEC) = 0x1;   /* vout0_g_y_yc[2] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AF0) = 0x1;   /* vout0_g_y_yc[3] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AF4) = 0x1;   /* vout0_g_y_yc[4] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AF8) = 0x1;   /* vout0_g_y_yc[5] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0AFC) = 0x1;   /* vout0_g_y_yc[6] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B00) = 0x1;   /* vout0_g_y_yc[7] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B04) = 0x1;   /* vout0_g_y_yc[8] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B08) = 0x1;   /* vout0_g_y_yc[9] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B0C) = 0x1;   /* vout0_r_cr[2] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B10) = 0x1;   /* vout0_r_cr[3] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B14) = 0x1;   /* vout0_r_cr[4] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B18) = 0x1;   /* vout0_r_cr[5] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B1C) = 0x1;   /* vout0_r_cr[6] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B20) = 0x1;   /* vout0_r_cr[7] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B24) = 0x1;   /* vout0_r_cr[8] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B28) = 0x1;   /* vout0_r_cr[9] */

    /* HDMI I2C_scl and I2C_sda Function 2*/
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0934) = 0x60002;   /* hdmi_ddc_scl_mux0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0938) = 0x60002;   /* hdmi_ddc_sda_mux0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x09BC) = 0x40010;  /*hdmi_hpd_mux0 pinmmr112[4]*/
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x09B8) = 0x60010;  /*hdmi_cec_mux0 pinmmr111[4] */
    /* TODO HDMI CEC and HPD to be added in pinmux */
    /* Currently its shared with GPMC. */

    /* VIN0 TODO Do we need to enable RXACTIVE Bit in pinmux for input pins? */
    /* Vin0 hsync1 and vin0 vsync1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A14) = 0x50001;   /* vin0_clk1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A18) = 0x0;       /* vin0_de0_mux0 - DeSelect input */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A1C) = 0x50001;   /* vin0_fld0_mux0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A20) = 0x50001;   /* vin0_clk0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A24) = 0x50001;   /* vin0_hsync0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A28) = 0x50001;   /* vin0_vsync0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A2C) = 0x50001;   /* vin0_d0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A30) = 0x50001;   /* vin0_d1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A34) = 0x50001;   /* vin0_d2 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A38) = 0x50001;   /* vin0_d3 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A3c) = 0x50001;   /* vin0_d4 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A40) = 0x50001;   /* vin0_d5 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A44) = 0x50001;   /* vin0_d6 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A48) = 0x50001;   /* vin0_d7 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A4c) = 0x50001;   /* vin0_d8 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A50) = 0x50001;   /* vin0_d9 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A54) = 0x50001;   /* vin0_d10 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A58) = 0x50001;   /* vin0_d11 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A5C) = 0x50001;   /* vin0_d12 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A60) = 0x50001;   /* vin0_d13 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A64) = 0x50001;   /* vin0_d14 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A68) = 0x50001;   /* vin0_d15 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A6C) = 0x50001;   /* vin0_d16 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A70) = 0x50001;   /* vin0_d17 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A74) = 0x50001;   /* vin0_d18 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A78) = 0x50001;   /* vin0_d19 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A7C) = 0x50001;   /* vin0_d20 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A80) = 0x50001;   /* vin0_d21 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A84) = 0x50001;   /* vin0_d22 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A88) = 0x50001;   /* vin0_d23 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A8C) = 0x50001;   /* vin0_de0_mux1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A90) = 0x50001;  /* vin0_de1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A94) = 0x50001;   /* vin0_fld0_mux1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A98) = 0x50001;   /* vin0_fld1 */


    /* VIN1 Configuration Function 3*/
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B2C) = 0x50004;   /* vin1_hsync0 */
    /* this is function 2 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x09F0) = 0x50002;   /* vin1_clk1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B30) = 0x50004;   /* vin1_vsync0 */
#ifdef ORIGIAL_PORT
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B34) = 0x50004;   /* vin1_fid0 */
#endif /* ORIGIAL_PORT */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B34) = 0x50008;   /* vin1_de0 */

    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B38) = 0x50004;   /* vin1_clk0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B3C) = 0x50004;   /* vin1a_d[0] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B40) = 0x50004;   /* vin1a_d[1] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B44) = 0x50004;   /* vin1a_d[2] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B48) = 0x50004;   /* vin1a_d[3] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B4C) = 0x50004;   /* vin1a_d[4] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B50) = 0x50004;   /* vin1a_d[5] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B54) = 0x50004;   /* vin1a_d[6] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B58) = 0x50004;   /* vin1a_d[8] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B5C) = 0x50004;   /* vin1a_d[9] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B60) = 0x50004;   /* vin1a_d[10] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B64) = 0x50004;   /* vin1a_d[11] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B68) = 0x50004;   /* vin1a_d[12] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B6C) = 0x50004;   /* vin1a_d[13] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B70) = 0x50004;   /* vin1a_d[14] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B74) = 0x50004;   /* vin1a_d[15] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B78) = 0x50004;   /* vin1a_d[16] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B7C) = 0x50004;   /* vin1a_d[17] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B80) = 0x50004;   /* vin1a_d[18] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B84) = 0x50004;   /* vin1a_d[19] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B88) = 0x50004;   /* vin1a_d[20] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B8C) = 0x50004;   /* vin1a_d[21] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B90) = 0x50004;   /* vin1a_d[22] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B94) = 0x50004;   /* vin1a_d[23] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B98) = 0x50004;   /* vin1a_d[7] */

#ifdef ORIGIAL_PORT
    /* Function 2 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BA8) = 0x50002;   /* vin1a_d[0] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BAC) = 0x50002;   /* vin1a_d[1] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BB0) = 0x50002;   /* vin1a_d[2] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BB4) = 0x50002;   /* vin1a_d[3] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BB8) = 0x50002;   /* vin1a_d[4] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BBC) = 0x50002;   /* vin1a_d[5] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BC0) = 0x50002;   /* vin1a_d[6] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0BC4) = 0x50002;   /* vin1a_d[7] */
#endif /* ORIGIAL_PORT */

    /* I2c2  configuration Function 6*/
#if defined(TI_814X_BUILD)
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0924) = 0x60020;   /* i2c2_scl_mux0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0928) = 0x60020;   /* i2c2_sda_mux0 */
#endif

    /* TODO Find proper place for this Set the divider for the SYSCLK10 */
    *(UInt32 *)0x48180324 = 3;


#if 0 /* Idea was to make VIP 1 vSync hSync and clkc as input and watch GPIO */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B2C) = 0x50080;   /* vin1_hsync0 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x09F0) = 0x50080;   /* vin1_clk1 */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B30) = 0x50080;   /* vin1_vsync0 */
#endif
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B44) = 0x00080;   /* vin1a_d[2] */
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0B48) = 0x00080;   /* vin1a_d[2] */
    return (FVID2_SOK);
}
#endif

#ifdef CONFIG_INT_MUX
static Int32 Vps_platformTI8107SetIntMux(void)
{
    volatile unsigned int int_mux;

    /* I2C2 interrupt is routed through I2C1 interrupt through the
     * crossbar. For this, INT_MUX_[#int_number] register in the
     * Chip Control Module needs to be programmed.
     * INT_MUX_[#int_number] registers start from 0xF54
     * offset and one register is used to program 4 interrupt
     * muxes (6 bits for each mux, 2 bits reserved).
     * After reset INT_MUX_[#int_number] defaults to 000000, which
     * maps the interrupt from default mapping to interrupt_[#int_number].
     *
     * I2C_INT1 is mapped to interrupt line 19 and
     * INTMUX 16 to 19 --> 0x0f64. So read it first,
     * modify the respective bit field and write is back.
     */
    int_mux = REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0f64);
    /* I2CINT2 value = 4, INT_MUX_19_SHIFT = 24*/
    int_mux |= (4 << 24);
    REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0f64) = int_mux;

    return (FVID2_SOK);
}
#endif

Int32 Vps_platformTI8107ResetVideoDevices(void)
{
    UInt32 i2cInst = Vps_platformTI8107GetI2cInstId();
    UInt32 ioExpI2cAddr = 0xFF;
    UInt8 regValue[2];
    Int32 status;
    Vps_PlatformBoardId boardId;

    boardId = Vps_platformGetBoardId();
    switch (boardId)
    {
        case VPS_PLATFORM_BOARD_VC:
            ioExpI2cAddr = gTi814xBoardVer.vcIoExpAddr;
            break;

        case VPS_PLATFORM_BOARD_VS:
            ioExpI2cAddr = VPS_VS_BOARD_IO_EXP_I2C_ADDR;
            break;

        case VPS_PLATFORM_BOARD_CATALOG:
            ioExpI2cAddr = gTi814xBoardVer.caIoExpAddr;
            break;
        default:
            break;
    }

    regValue[0] = 0x00;
    regValue[1] = 0x00;
    /* 2 versions of VC card - IO expander and filters are different between
       them */
    if ((boardId == VPS_PLATFORM_BOARD_VC) &&
        (gTi814xBoardVer.vcCard == VPS_PLATFORM_BOARD_REV_B))
    {
        status = Vps_deviceRawRead8(i2cInst, ioExpI2cAddr, regValue, 2u);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
        /* Reset TVP and disable THS73681 filter */
        regValue[0] &= (UInt8)~(VPS_VC_A2_PCF8575_TVP_RESETn_MASK);
        regValue[0] |= (UInt8)(VPS_VC_A2_PCF8575_THS73861_DISABLE_MASK);
    }

    /* Catalog Card */
    if (boardId == VPS_PLATFORM_BOARD_CATALOG)
    {
        status = Vps_deviceRawRead8(i2cInst, ioExpI2cAddr, regValue, 2u);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
        /* Reset TVP and disable THS7368 filter */
        regValue[0] &= (UInt8)~(VPS_CA_A1_PCF8575_TVP_RESETn_MASK);
        regValue[0] |= (UInt8)(VPS_CA_A1_PCF8575_THS7368_DISABLE_MASK);
    }

    status = Vps_deviceRawWrite8(i2cInst, ioExpI2cAddr, regValue, 2u);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    Task_sleep(1000);

    if (boardId == VPS_PLATFORM_BOARD_VS)
    {
        regValue[0] = 0xFF;
        regValue[1] = 0xFE;
    }
    else
    {
        if (boardId == VPS_PLATFORM_BOARD_VC)
        {
            if (gTi814xBoardVer.vcCard == VPS_PLATFORM_BOARD_REV_A)
            {
                regValue[0] = 0xEF;
                regValue[1] = 0xFE;
            }
            else
            {
                /* Bring TVP outof reset and enable THS73681 filter */
                regValue[0] |= (UInt8)(VPS_VC_A2_PCF8575_TVP_RESETn_MASK);
                regValue[0] &= (UInt8)~(VPS_VC_A2_PCF8575_THS73861_DISABLE_MASK);
            }
        }

        if (boardId == VPS_PLATFORM_BOARD_CATALOG)
        {
                /* Bring TVP outof reset and enable THS73681 filter */
                regValue[0] |= (UInt8)(VPS_CA_A1_PCF8575_TVP_RESETn_MASK);
                regValue[0] &= (UInt8)~(VPS_CA_A1_PCF8575_THS7368_DISABLE_MASK);
        }

    }
    status = Vps_deviceRawWrite8(i2cInst, ioExpI2cAddr, regValue, 2);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    return (status);
}

Vps_PlatformCpuRev Vps_platformTI8107GetCpuRev(void)
{
    UInt32                  cpuId, cpuRev;
    Vps_PlatformCpuRev      cpuRevEnum;

    /* Read CPU ID */
    cpuId = REG32(VPS_CTRL_MODULE_DEV_CFG_BASE_ADDR + 0x0000u);

    cpuRev = ((cpuId >> 28u) & 0x0Fu);
    switch (cpuRev)
    {
        case 0x0u:
            cpuRevEnum = VPS_PLATFORM_CPU_REV_1_0;
            break;

       default:
            /* Default to last known version */
            Vps_printf("PLATFORM: UNKNOWN CPU detected, defaulting to VPS_PLATFORM_CPU_REV_1_0\n");
            cpuRevEnum = VPS_PLATFORM_CPU_REV_1_0;
            break;
    }

    return (cpuRevEnum);
}

Vps_PlatformBoardRev Vps_platformTI8107GetBaseBoardRev(void)
{
    /* Only one revision of board till date */
    return (VPS_PLATFORM_BOARD_REV_A);
}

Vps_PlatformBoardRev Vps_platformTI8107GetDcBoardRev(void)
{
    /* Only one revision of board till date */
    return (VPS_PLATFORM_BOARD_REV_A);
}

Int32 Vps_platformTI8107EnableTvp7002Filter(FVID2_Standard standard)
{
    Int32       status = FVID2_SOK;
    UInt32      i2cInstId, i2cDevAddr;
    Vps_PlatformBoardId boardId;

    boardId = Vps_platformGetBoardId();
    i2cInstId = Vps_platformTI8107GetI2cInstId();

    if (boardId == VPS_PLATFORM_BOARD_VC)
    {
        if (gTi814xBoardVer.vcCard == VPS_PLATFORM_BOARD_REV_A)
        {
            i2cDevAddr = VPS_VC_BOARD_THS7353_I2C_ADDR;
            status = Vps_platformEnableThs7353(standard, i2cInstId, i2cDevAddr);
        }
        else if(gTi814xBoardVer.vcCard == VPS_PLATFORM_BOARD_REV_B)
        {
            status = Vps_platformTi814xEnableThs73681(standard,
                                            i2cInstId,
                                            gTi814xBoardVer.vcIoExpAddr);
        }
        else
        {
            status = FVID2_EFAIL;
        }
    }

    if (boardId == VPS_PLATFORM_BOARD_CATALOG)
    {
        status = Vps_platformTi814xEnableThs73681(standard,
                                                i2cInstId,
                                                gTi814xBoardVer.caIoExpAddr);
    }

    return (status);
}

Int32 Vps_platformTI8107SetLcdBackLight(UInt32 value)
{
    volatile UInt32 cnt;

    /* Load Timer Match Register */
    REG32(VPS_TI8107_TIMER7_TMAR) = value;

    /* Stop Timer7 */
    REG32(VPS_TI8107_TIMER7_TCLR) = REG32(VPS_TI8107_TIMER7_TCLR) & ~0x1;

    /* Provide a n */
    for (cnt = 0; cnt < 10; cnt ++);

    /* Start Timer7 */
    REG32(VPS_TI8107_TIMER7_TCLR) = REG32(VPS_TI8107_TIMER7_TCLR) | 0x1;

    return (FVID2_SOK);
}

/* Enable LCD BackLight */
Int32 Vps_platformTI8107EnableLcdBackLight(UInt32 isEnable)
{
    if (isEnable)
    {
        /* Timer7 Clock: External 32 kHz clock */
        REG32(VPS_TI8107_CM_VPS_TI8107_TIMER7_CLKSEL) = 0x1u;

        /* Enable Timer7 Clock in PRCM Module */
        REG32(VPS_TI8107_CM_ALWON_TIMER_7_CLKCTRL) = 0x2u;
        while(REG32(VPS_TI8107_CM_ALWON_TIMER_7_CLKCTRL) != 0x2);

        /* Configure SAM_PWM_LCD (VPS_TI8107_TIMER7_IO) */
        REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x09CC) = 0x00010040;

        /* Perform a RESET on the Timer */
        REG32(VPS_TI8107_TIMER7_TIOCP_CFG) = REG32(VPS_TI8107_TIMER7_TIOCP_CFG) | 0x1;

        /* Wait till the RESET completes */
        while (REG32(VPS_TI8107_TIMER7_TIOCP_CFG) & 0x1);

        /* Configure Timer for Emulation-Free mode */
        REG32(VPS_TI8107_TIMER7_TIOCP_CFG)= REG32(VPS_TI8107_TIMER7_TIOCP_CFG) | 0x2;

        /* Stop the Timer */
        REG32(VPS_TI8107_TIMER7_TCLR) = REG32(VPS_TI8107_TIMER7_TCLR) & ~0x1;

        /* Configure the Timer */
        REG32(VPS_TI8107_TIMER7_TCLR) =
                        (1 << 1) |  // Enable Auto Reload
                        (1 << 6) |  // Enable Compare Mode
                        (2 << 10) | // PORTIMERPWM output trigger on overflow and match
                        (1 << 12); // Toggle mode on PORTIMERPWM output

        /* Configure Timer Load Register */
        REG32(VPS_TI8107_TIMER7_TLDR) = 0xFFFFFB3B;

        /* Configure Timer Trigger Register */
        REG32(VPS_TI8107_TIMER7_TTGR) = 0xFFFFFB3B;

        /* Start Timer7 */
        REG32(VPS_TI8107_TIMER7_TCLR) = REG32(VPS_TI8107_TIMER7_TCLR) | 0x1;
    }
    else
    {
        /* Configure SAM_PWM_LCD (VPS_TI8107_TIMER7_IO) */
        REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x09CC) = 0x00010080;
    }

    return (FVID2_SOK);
}

Void VpsPlatfformPowerOnGPIO()
{
    /* Enable GPIO2 Clock in PRCM Module */
    /* No register for GPIO2 clock in PRCM Module, It uses GPIO1 clock register */
    /* Enable GP1 Clock */
    REG32(VPS_TI8107_CM_ALWON_GPIO_1_CLKCTRL) = 0x2;
    /* Poll for GP1 Module Clock is functional */
    while(REG32(VPS_TI8107_CM_ALWON_GPIO_1_CLKCTRL) != 0x2);

    /* Do Software Reset */
    REG32(VPS_TI8107_GPIO2_SYSCONFIG) = 0x00000002;
    udelay(100);
    /* Put it in no-idle */
    REG32(VPS_TI8107_GPIO2_SYSCONFIG) = 0x00000008;   // no-idle

    /* Enable output High */
    REG32(VPS_TI8107_GPIO2_OE) &= ~GP2_9;
    REG32(VPS_TI8107_GPIO2_DATAOUT) |= GP2_9;
}

/* Enable LCD by setting GPIO output */
Int32 Vps_platformTI8107EnableLcd(UInt32 isEnable)
{
    if (isEnable)
    {
        VpsPlatfformPowerOnGPIO();

        REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A3c) = 0x10080;   /* vin0_d4 */
    }
    else
    {
        REG32(CSL_TI8107_CTRL_MODULE_BASE + 0x0A3c) = 0x50001;   /* vin0_d4 */
    }

    return (FVID2_SOK);
}

#ifdef ENABLE_HDVPSS_CLK
/** \brief Function to enable HDVPSS clock */
static Int32 Vps_platformTI8107EnableHdVpssClk(void)
{
    volatile UInt32 repeatCnt;
    volatile UInt32 regValue;

    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPrcmRegs);

    /* Bring the HDVPSS and HDMI out of reset */
    gVpsPrcmRegs->RM_HDVPSS_RSTCTRL = 0x0;

    /* Start a software forced  wakeup transition on the domain.*/
    gVpsPrcmRegs->CM_HDVPSS_CLKSTCTRL = 0x2;
    udelay(1000);
    /* Enable HDVPSS Clocks */
    gVpsPrcmRegs->CM_HDVPSS_HDVPSS_CLK_CTRL = 0x2;
    /* Enable HDMI Clocks */
    gVpsPrcmRegs->CM_HDVPSS_HDMI_CLKCTRL = 0x2;

    repeatCnt = 0;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        /* Check for
         * Current Power State Status
         * HDVPSS memory state status
         * Logic state status */
        regValue = gVpsPrcmRegs->CM_HDVPSS_CLKSTCTRL;
        if ((regValue & 0x100) == 0x100)
        {
            break;
        }
        udelay(1000);
        repeatCnt++;
    }
    if ((regValue & 0x100) != 0x100)
    {
        Vps_printf("HDVPSS Clocks not enabled\n");
    }
    repeatCnt = 0;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        /* Check for
         * Current Power State Status
         * HDVPSS memory state status
         * Logic state status */
        regValue = gVpsPrcmRegs->CM_HDVPSS_HDVPSS_CLK_CTRL;
        if ((regValue & 0x2) == 0x2)
        {
            break;
        }
        udelay(1000);
        repeatCnt++;
    }
    if ((regValue & 0x2) != 0x2)
    {
        Vps_printf("HDVPSS Clocks not enabled\n");
    }
    /* Bring the HDVPSS and HDMI out of reset */
    gVpsPrcmRegs->RM_HDVPSS_RSTCTRL = 0x0;
    return (FVID2_SOK);
}
#endif

#ifdef ENABLE_I2C_CLK
/** \brief Function sets the SYSCLK10 to 48MHz. SysClk10 is used for CEC
     and I2C */
static Int32 Vps_platformTI8107EnableI2c(void)
{
    volatile UInt32 repeatCnt;

    GT_assert( GT_DEFAULT_MASK, NULL != gVpsPrcmRegs);

    /* Enable Power Domain Transition */
    gVpsPrcmRegs->CM_ALWON_I2C_02_CLKCTRL = 0x2;

    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if (((gVpsPrcmRegs->CM_ALWON_I2C_02_CLKCTRL &
                    CM_ALWON_I2C_02_CLKCTRL_IDLEST_MASK) >>
                    CM_ALWON_I2C_02_CLKCTRL_IDLEST_SHIFT) == 0u)
        {
            break;
        }

        /* Wait for the 100 cycles */
        udelay(1000);

        repeatCnt++;
    }

    if (((gVpsPrcmRegs->CM_ALWON_I2C_02_CLKCTRL &
            CM_ALWON_I2C_02_CLKCTRL_IDLEST_MASK) >>
            CM_ALWON_I2C_02_CLKCTRL_IDLEST_SHIFT) != 0u)
    {
        Vps_printf("=== I2C0/2 Clk is Non active ===\n");
        return (FVID2_ETIMEOUT);
    }
    else
    {
        Vps_printf("=== I2C0/2 Clk is active ===\n");
    }
    /* Change it to #if 1 to enable I2C1 clk */
#if 0
    /* Enable Power Domain Transition */
    gVpsPrcmRegs->CM_ALWON_I2C_13_CLKCTRL = 0x2;

    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if (((gVpsPrcmRegs->CM_ALWON_I2C_13_CLKCTRL &
                    CM_ALWON_I2C_13_CLKCTRL_IDLEST_MASK) >>
                    CM_ALWON_I2C_13_CLKCTRL_IDLEST_SHIFT) == 0u)
        {
            break;
        }

        /* Wait for the 100 cycles */
        udelay(1000);

        repeatCnt++;
    }

    if (((gVpsPrcmRegs->CM_ALWON_I2C_13_CLKCTRL &
            CM_ALWON_I2C_13_CLKCTRL_IDLEST_MASK) >>
            CM_ALWON_I2C_13_CLKCTRL_IDLEST_SHIFT) != 0u)
    {
        Vps_printf("=== I2C1/3 Interface Clk is Non active ===\n");
        return (FVID2_ETIMEOUT);
    }
    else
    {
        Vps_printf("=== I2C1/3 Interface Clk is active ===\n");
    }
#endif
    return FVID2_SOK;

}
#endif

Int32 Vps_platformTI8107SelectVideoDecoder(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt32 i2cInstId = Vps_platformTI8107GetI2cInstId();
    UInt32 i2cDevAddr;
    UInt8  regValue[2];
    Vps_PlatformBoardId boardId;
    Int32  status = FVID2_SOK;

    boardId = Vps_platformGetBoardId();

    /* In TI8107, GMII1 and SiL9135A are muxed on the VC board. */
    if(boardId == VPS_PLATFORM_BOARD_VC)
    {
        i2cDevAddr = gTi814xBoardVer.vcIoExpAddr;
        regValue[0] = 0xEF;
        regValue[1] = 0xFF;
    }

    /* DVI Input and TVP input are muxed, select TVP input */
    if (boardId == VPS_PLATFORM_BOARD_CATALOG)
    {
        i2cDevAddr = gTi814xBoardVer.caIoExpAddr;

        status = Vps_deviceRawRead8(i2cInstId, i2cDevAddr, regValue, 2u);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
        /* Configure muxes to select TVP */
        regValue[1] |= (UInt8)(VPS_CA_A1_PCF8575_SEL_TVP_S0_MASK);
        regValue[1] &= (UInt8)~(VPS_PCF8575_P10_17_P17_MASK);
    }

    status = Vps_deviceRawWrite8(i2cInstId, i2cDevAddr, regValue, 2);
    return status;
}

#if  defined(CONFIG_PLL) || defined(ENABLE_HDVPSS_CLK) || defined (ENABLE_I2C_CLK)
static void udelay(int delay_usec)
{
    Int32 delay_msec;

    delay_msec = delay_usec/1000;
    if(delay_msec==0)
        delay_msec = 1;
    Task_sleep(delay_msec);
}
#endif


#if defined (CONFIG_PLL) || defined(ENABLE_HDVPSS_CLK)
static Int32 Vps_platformPllCfg( volatile UInt32 Base_Address,
                UInt32 N,UInt32 M,UInt32 M2,UInt32 CLKCTRL_VAL)
{
    Int32 rtnValue;
    UInt32 m2nval, mn2val, read_clkctrl;
    volatile UInt32 repeatCnt = 0;
    volatile UInt32 clkCtrlVal;

    /* Put the PLL in bypass mode */
    clkCtrlVal = RD_MEM_32(Base_Address+CLKCTRL);
    clkCtrlVal |= 0x1 << 23;
    WR_MEM_32((Base_Address+CLKCTRL), clkCtrlVal);

    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if (((RD_MEM_32(Base_Address+STATUS)) & 0x00000101) == 0x00000101)
        {
            break;
        }
        /* Wait for the 100 cycles */
        udelay(10000);
        repeatCnt++;
    }
    if (((RD_MEM_32(Base_Address+STATUS)) & 0x00000101) == 0x00000101)
    {
    ;
    }
    else
    {
    Vps_printf("Not able to put PLL in idle!!!\n");
    }

    /* we would require a soft reset before we program the dividers */
    WR_MEM_32(Base_Address+CLKCTRL, RD_MEM_32(Base_Address+CLKCTRL)& 0xfffffffe);
    udelay(3);

    /* Program the PLL for required frequency */
    m2nval = (M2 << 16) | N;
    mn2val =  M;
    /*ref_clk     = OSC_FREQ/(N+1);
    clkout_dco  = ref_clk*M;
    clk_out     = clkout_dco/M2;
    */
    WR_MEM_32((Base_Address+M2NDIV    ),m2nval);
    WR_MEM_32((Base_Address+MN2DIV    ),mn2val);
    udelay(3);
    WR_MEM_32((Base_Address+TENABLEDIV),0x1);
    udelay(3);
    WR_MEM_32((Base_Address+TENABLEDIV),0x0);
    udelay(3);
    WR_MEM_32((Base_Address+TENABLE   ),0x1);
    udelay(3);
    WR_MEM_32((Base_Address+TENABLE   ),0x0);
    udelay(3);
    read_clkctrl = RD_MEM_32(Base_Address+CLKCTRL);
    /*configure the TINITZ(bit0) and CLKDCO bits if required */
    WR_MEM_32(Base_Address+CLKCTRL,(read_clkctrl & 0xff7fe3ff) | CLKCTRL_VAL);
    read_clkctrl = RD_MEM_32(Base_Address+CLKCTRL);

    /* poll for the freq,phase lock to occur */
    repeatCnt = 0u;
    while (repeatCnt < VPS_PRCM_MAX_REP_CNT)
    {
        if (((RD_MEM_32(Base_Address+STATUS)) & 0x00000600) == 0x00000600)
        {
            break;
        }
        /* Wait for the 100 cycles */
        udelay(10000);
        repeatCnt++;
    }
    if (((RD_MEM_32(Base_Address+STATUS)) & 0x00000600) == 0x00000600)
    {
        //Vps_printf("PLL Locked\n");
        rtnValue = FVID2_SOK;
    }
    else
    {
        Vps_printf("PLL Not Getting Locked!!!\n");
        rtnValue = FVID2_EFAIL;
    }
    /*wait fot the clocks to get stabized */
    udelay(1000);
    return (rtnValue);
}
#endif

/* Relies on I2C Address of IO Expander.
   Alpha 1 IO Expander is at 0x27 and Alpha 2 IO Expander at 0x21 */

static Int32 Vps_getVcCardVersion (Vps_BoardVersion *boardVer)
{
    Int32  status;
    Vps_PlatformBoardRev version;
    UInt8  regValue[2];
    UInt32 expAddr;

    version = boardVer->vcCard;
    status = FVID2_SOK;

    if (version == VPS_PLATFORM_BOARD_REV_UNKNOWN)
    {
        Semaphore_pend(boardVer->ioExpLock, BIOS_WAIT_FOREVER);

        /* Check if its Alpha 1 - REV A */
        status = Vps_deviceRawRead8(Vps_platformTI8107GetI2cInstId(),
                        VPS_VC_BOARD_A1_IO_EXP_I2C_ADDR, regValue, 2u);
        if (status == FVID2_SOK)
        {
            version = VPS_PLATFORM_BOARD_REV_A;
            expAddr = VPS_VC_BOARD_A1_IO_EXP_I2C_ADDR;
        }
        else
        {   /* Otherwise shoule be Alpha 2 - check to ensure */
            status = Vps_deviceRawRead8(Vps_platformTI8107GetI2cInstId(),
                            VPS_VC_BOARD_A2_IO_EXP_I2C_ADDR, regValue, 2u);
            if (status == FVID2_SOK)
            {
                version = VPS_PLATFORM_BOARD_REV_B;
                expAddr = VPS_VC_BOARD_A2_IO_EXP_I2C_ADDR;
            }
        }
        boardVer->vcCard      = version;
        boardVer->vcIoExpAddr = expAddr;

        Semaphore_post(boardVer->ioExpLock);
    }
    return (status);
}

/* Used for VC - Aplha 2 card only */
static Int32 Vps_platformTi814xEnableThs73681(FVID2_Standard standard,
                                              UInt32 i2cInst,
                                              UInt32 ioExpAddr)
{
    Int32       status = FVID2_SOK;
    UInt8       regValue[2];

    status = Vps_deviceRawRead8(i2cInst, ioExpAddr, regValue, 2u);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    /*  PCF8575 - mappings
        P7 - THS73861_FILTER2
        P6 - THS73861_FILTER1
        P5 - THS73861_BYPASS
        P4 - THS73861_DISABLE
        P0 - TVP7002_RSTN */
            /* Enable filter, disable bypass, clear filter select bits */
    regValue[0] &= ~( VPS_VC_A2_PCF8575_THS73861_DISABLE_MASK
                    | VPS_VC_A2_PCF8575_THS73861_BYPASS_MASK
                    | VPS_VC_A2_PCF8575_THS73861_FILTER1_MASK
                    | VPS_VC_A2_PCF8575_THS73861_FILTER2_MASK);

    switch (standard)
    {
        case FVID2_STD_1080P_60:
        case FVID2_STD_1080P_50:
        case FVID2_STD_SXGA_60:
        case FVID2_STD_SXGA_75:
        case FVID2_STD_SXGAP_60:
        case FVID2_STD_SXGAP_75:
        case FVID2_STD_UXGA_60:
            /* Filter2: 1, Filter1: 1 */
            regValue[0] |=  (UInt8)
                            (VPS_VC_A2_PCF8575_THS73861_FILTER1_MASK |
                             VPS_VC_A2_PCF8575_THS73861_FILTER2_MASK);
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
            /* Filter2: 0, Filter1: 1 */
            regValue[0] &= (UInt8)
                           ~(VPS_VC_A2_PCF8575_THS73861_FILTER2_MASK);
            regValue[0] |= (UInt8)
                            (VPS_VC_A2_PCF8575_THS73861_FILTER1_MASK);
            break;

        case FVID2_STD_480P:
        case FVID2_STD_576P:
        case FVID2_STD_VGA_60:
        case FVID2_STD_VGA_72:
        case FVID2_STD_VGA_75:
        case FVID2_STD_VGA_85:
            /* Filter2: 1, Filter1: 0 */
            regValue[0] &= (UInt8)
                           ~(VPS_VC_A2_PCF8575_THS73861_FILTER1_MASK);
            regValue[0] |= (UInt8)
                            (VPS_VC_A2_PCF8575_THS73861_FILTER2_MASK);
            break;

        case FVID2_STD_NTSC:
        case FVID2_STD_PAL:
        case FVID2_STD_480I:
        case FVID2_STD_576I:
        case FVID2_STD_D1:
            /* Filter2: 0, Filter1: 0 */
            regValue[0] &=  (UInt8)
                            ~(VPS_VC_A2_PCF8575_THS73861_FILTER1_MASK |
                             VPS_VC_A2_PCF8575_THS73861_FILTER2_MASK);
            break;

        default:
            /* Filter2: 0, Filter1: 1 */
            regValue[0] &= (UInt8)
                           ~(VPS_VC_A2_PCF8575_THS73861_FILTER2_MASK);
            regValue[0] |= (UInt8)
                            (VPS_VC_A2_PCF8575_THS73861_FILTER1_MASK);
            break;
    }

    Vps_deviceRawWrite8(i2cInst, ioExpAddr, regValue, 2u);
    Task_sleep(500);

    return (status);
}

Int32 Vps_platformTI816xSelectHdCompClkSrc(Vps_VPllOutputClk clkSrc)
{
    Int32 status = FVID2_SOK;

    switch (clkSrc)
    {
        case VPS_SYSTEM_VPLL_OUTPUT_VENC_RF:
            REG32(VPS_HD_DAC_CLKSRC_REG_ADDR) = 0x2u;
            break;
        case VPS_SYSTEM_VPLL_OUTPUT_VENC_D:
            REG32(VPS_HD_DAC_CLKSRC_REG_ADDR) = 0x1u;
            break;
        case VPS_SYSTEM_VPLL_OUTPUT_VENC_A:
        case VPS_SYSTEM_VPLL_OUTPUT_HDMI:
            REG32(VPS_HD_DAC_CLKSRC_REG_ADDR) = 0x0u;
            break;
        default:
            REG32(VPS_HD_DAC_CLKSRC_REG_ADDR) = 0x0u;
            break;
    }

    return (status);
}

