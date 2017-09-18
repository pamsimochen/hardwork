/** ==================================================================
 *  @file   iss_platformTI814x.c                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/ti814x/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file iss_platformTI814x.c
 *
 *  \brief Implements the TI814x platform specific functions.
 *
 */

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/iss/iss.h>
#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/cslr/soc_TI814x.h>
#include <ti/psp/cslr/cslr_TI814xprcm.h>
#include <ti/psp/cslr/cslr_TI814xpll.h>
#include <ti/psp/devices/iss_sensorDriver.h>
#include <ti/psp/platforms/ti814x/iss_platformTI814x.h>

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

#define ENABLE_HDVPSS_CLK

/* Enable I2C control to configure ecn/dec */
#define ENABLE_I2C_CLK

/* Set the PLLs */
#define CONFIG_PLL

#define ISS_VS_BOARD_IO_EXP_I2C_ADDR    (0x21u)

#define ISS_VC_BOARD_A1_IO_EXP_I2C_ADDR (0x27u)
#define ISS_VC_BOARD_A2_IO_EXP_I2C_ADDR (0x21u)

#define ISS_CA_BOARD_A1_IO_EXP_I2C_ADDR (0x21u)

#define ISS_PLATFORM_EVM_I2C_INST_ID    (ISS_DEVICE_I2C_INST_ID_2)

/** \brief PLL Control Module Base Address*/
#define ISS_CONTROL_MODULE_PLL_CTRL_BASE_ADDR   (CSL_TI814x_PLL_BASE)

/** \brief Control Module Device Configuration Base Address */
#define ISS_CTRL_MODULE_DEV_CFG_BASE_ADDR       (CSL_TI814x_CTRL_MODULE_BASE + \
                                                    0x0600u)

/* Default Values for DDR PLL configuration to get clock for I2C and CEC */
#define ISS_DDR_INT_FREQ2               (0x8u)
#define ISS_DDR_FRACT_FREQ2             (0xD99999u)
#define ISS_DDR_MDIV2                   (0x1Eu)
#define ISS_DDR_SYCCLK10_DIV            (0x0u)

/* gpio base addresses */
#define REG32                           *(volatile unsigned int*)

#define ISS_PRCM_CLKTRCTRL_NO_SLEEP     (0u)
#define ISS_PRCM_CLKTRCTRL_SW_SLEEP     (1u)
#define ISS_PRCM_CLKTRCTRL_SW_WKUP      (2u)
#define ISS_PRCM_CLKTRCTRL_HW_AUTO      (3u)

#define ISS_PRCM_MODULE_DISABLE         (0u)
#define ISS_PRCM_MODULE_ENABLE          (2u)

#define ISS_PRCM_MAX_REP_CNT            (100u)

/* ADPLLJ_CLKCRTL_Register Value Configurations ADPLLJ_CLKCRTL_Register SPEC
 * bug bit 19,bit29 -- CLKLDOEN,CLKDCOEN */
#define ADPLLJ_CLKCRTL_HS2              (0x00000801u)
/* HS2 Mode,TINTZ =1 --used by all PLL's except HDMI */
#define ADPLLJ_CLKCRTL_HS1              (0x00001001u)
/* HS1 Mode,TINTZ =1 --used only for HDMI */
#define ADPLLJ_CLKCRTL_CLKDCO           (0x200a0000u)
/* Enable CLKDCOEN,CLKLDOEN,CLKDCOPWDNZ -- used for HDMI,USB */
#define ISS_TI814X_KHz                  (1000u)
#define ISS_TI814X_MHz                  (ISS_TI814X_KHz * ISS_TI814X_KHz)
#define ISS_TI814X_EVM_OSC_FREQ         (20u * ISS_TI814X_MHz)


#ifndef BOARD_AP_IPNC
#define BOARD_AP_IPNC
#endif

#ifdef BOARD_TI_EVM
#undef BOARD_TI_EVM
#endif
/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/* \brief structure to keep track of pll configurations for a video mode */
typedef struct {
    UInt32 __n;
    /**< Divider N for the PLL.*/
    UInt32 __m;
    /**< Multiplier M for the PLL.*/
    UInt32 __m2;
    /**< Divider M2 for the PLL.*/
    UInt32 clkCtrlValue;
    /**< For comparison based on the clkOut used */
} Iss_VideoPllCtrl;

/* Structure to track the versions of boards */
typedef struct {
    Iss_PlatformBoardRev vcCard;
    UInt32 vcIoExpAddr;
    Semaphore_Handle ioExpLock;
    Iss_PlatformBoardRev vsCard_notused;                   /* Not used as of
                                                            * now */
    Iss_PlatformBoardRev baseBoard_notused;                /* Not used as of
                                                            * now */
    Iss_PlatformBoardRev caCard;
    UInt32 caIoExpAddr;
} Iss_BoardVersion;

/* ========================================================================== 
 */
/* Function Declarations */
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     Iss_platformTI814xSetPinMux                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 Iss_platformTI814xSetPinMux(void);

#ifdef POWER_OPT_DSS_OFF
/* ===================================================================
 *  @func     Iss_platformTI814xSetIntMux                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 Iss_platformTI814xSetIntMux(void);
#endif

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */

/**< Variable to track the version of daughter card, we have Aplha 1 and
     Alpha 2, designated as REV A and REV B */
static Iss_BoardVersion gTi814xBoardVer = { ISS_PLATFORM_BOARD_REV_UNKNOWN,
    ISS_VC_BOARD_A1_IO_EXP_I2C_ADDR,
    ISS_PLATFORM_BOARD_REV_UNKNOWN,
    ISS_PLATFORM_BOARD_REV_UNKNOWN
};

/* ========================================================================== 
 */
/* Function Definitions */
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     Iss_platformTI814xInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_platformTI814xInit(Iss_PlatformInitParams * initParams)
{
    Int32 status = FVID2_SOK;

    Iss_platformTI814xSetPinMux();

#ifdef POWER_OPT_DSS_OFF
    Iss_platformTI814xSetIntMux();
#endif

#ifdef ENABLE_HDVPSS_CLK
    /* Initialize Pixel Clock */
    // status |= Iss_platformTI814xConfigHdIsssPll();
    // status |= Iss_platformTI814xEnableHdIsssClk();
#endif

#ifdef ENABLE_I2C_CLK
    // status |= Iss_platformTI814xEnableI2c();
#endif

    return (status);
}

#ifdef POWER_OPT_DSS_OFF

/* ===================================================================
 *  @func     Iss_platformTI814xSetIntMux                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 Iss_platformTI814xSetIntMux(void)
{
    volatile unsigned int int_mux;

    /* I2C2 interrupt is routed through I2C1 interrupt through the crossbar.
     * For this, INT_MUX_[#int_number] register in the Chip Control Module
     * needs to be programmed. INT_MUX_[#int_number] registers start from
     * 0xF54 offset and one register is used to program 4 interrupt muxes (6
     * bits for each mux, 2 bits reserved). After reset INT_MUX_[#int_number] 
     * defaults to 000000, which maps the interrupt from default mapping to
     * interrupt_[#int_number]. I2C_INT1 is mapped to interrupt line 19 and
     * INTMUX 16 to 19 --> 0x0f64. So read it first, modify the respective
     * bit field and write is back. */
    int_mux = REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0f64);
    /* I2CINT2 value = 4, INT_MUX_19_SHIFT = 24 */
    int_mux |= (4 << 24);
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0f64) = int_mux;

    return (FVID2_SOK);
}
#endif

/* ===================================================================
 *  @func     Iss_platformTI814xDeInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_platformTI814xDeInit(void)
{
    Int32 status = FVID2_SOK;

    return (status);
}

/* Init EVM related sub-systems like I2C instance */
/* ===================================================================
 *  @func     Iss_platformTI814xDeviceInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_platformTI814xDeviceInit(Iss_PlatformDeviceInitParams * initPrms)
{
    Int32 status = FVID2_SOK;

    Vps_DeviceInitParams deviceInitPrm;

#ifdef POWER_OPT_DSS_OFF
    UInt8 i2cCnt = 0;

    /* TI814x has 4 I2C instances. */
    UInt32 i2cRegs[ISS_DEVICE_I2C_INST_ID_MAX] = { CSL_TI814x_I2C0_BASE,
        CSL_TI814x_I2C1_BASE,
        CSL_TI814x_I2C2_BASE,
        CSL_TI814x_I2C3_BASE
    };
    UInt32 i2cInt[ISS_DEVICE_I2C_INST_ID_MAX] = { CSL_INTC_EVENTID_I2CINT0,
        CSL_INTC_EVENTID_I2CINT1,
        CSL_INTC_EVENTID_I2CINT2,
        CSL_INTC_EVENTID_I2CINT3
    };
    /* 
     * External video device subsystem init
     */
    memset(&deviceInitPrm, 0, sizeof(deviceInitPrm));

    /* 
     * Initialize I2C instances
     */
    for (i2cCnt = 0; i2cCnt < ISS_DEVICE_I2C_INST_ID_MAX; i2cCnt++)
    {
        deviceInitPrm.i2cRegs[i2cCnt] = (Ptr) (i2cRegs[i2cCnt]);
        deviceInitPrm.i2cIntNum[i2cCnt] = i2cInt[i2cCnt];
        deviceInitPrm.i2cClkKHz[i2cCnt] = VPS_DEVICE_I2C_INST_NOT_USED;
    }
    deviceInitPrm.isI2cInitReq = initPrms->isI2cInitReq;

    /* TI814x uses only I2C[2], so modify the sampling frequency */
    deviceInitPrm.i2cRegs[ISS_PLATFORM_EVM_I2C_INST_ID]
        = (Ptr) CSL_TI814x_I2C2_BASE;
    deviceInitPrm.i2cIntNum[ISS_PLATFORM_EVM_I2C_INST_ID]
        = CSL_INTC_EVENTID_I2CINT2;
    deviceInitPrm.i2cClkKHz[ISS_PLATFORM_EVM_I2C_INST_ID] = 400;
#ifdef TI_8107_BUILD
#ifdef _IPNC_HW_PLATFORM_EVM_		
    deviceInitPrm.i2cClkKHz[ISS_PLATFORM_EVM_I2C_INST_ID]
        = 50;
#endif		
#endif		
#endif

    status = Iss_deviceInit(&deviceInitPrm);

    return (status);
}

/* De-Init EVM related sub-systems */
/* ===================================================================
 *  @func     Iss_platformTI814xDeviceDeInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_platformTI814xDeviceDeInit(void)
{
    /* 
     * Extern video device de-init
     */
    Iss_deviceDeInit();

    return (FVID2_SOK);
}

/* ===================================================================
 *  @func     Iss_platformTI814xGetI2cInstId                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt32 Iss_platformTI814xGetI2cInstId(void)
{
    return (ISS_PLATFORM_EVM_I2C_INST_ID);
}

/* ===================================================================
 *  @func     Iss_platformTI814xGetSensorI2cAddr                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
UInt8 Iss_platformTI814xGetSensorI2cAddr(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt8 devAddr = 0x0;

#ifdef BOARD_AP_IPNC
    UInt8 devAddrAr0331[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrMn34041[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrImx035[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrOv2715[ISS_CAPT_INST_MAX] = { 0x36 };
    UInt8 devAddrImx036[ISS_CAPT_INST_MAX] = { 0x30 };
    UInt8 devAddrOv9712[ISS_CAPT_INST_MAX] = { 0x30 };
    UInt8 devAddrOv10630[ISS_CAPT_INST_MAX] = { 0x30 };
    UInt8 devAddrMt9p031[ISS_CAPT_INST_MAX] = { 0x48 };
    UInt8 devAddrMt9d131[ISS_CAPT_INST_MAX] = { 0x5D };
    UInt8 devAddrMt9m034[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrTvp514x[ISS_CAPT_INST_MAX] = { 0x48 };
    UInt8 devAddrMt9j003[ISS_CAPT_INST_MAX] = { 0x36 };
    UInt8 devAddrAr0330[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrMt9m034dbl[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrImx136[ISS_CAPT_INST_MAX] = {0x2d};	//lvds324
	UInt8 devAddrImx122[ISS_CAPT_INST_MAX] = { 0xFF };	/* IMX-122 use SPI interface so it has no meaning */
    UInt8 devAddrOv7740[ISS_CAPT_INST_MAX] = { 0x21 };
    UInt8 devAddrOv2710[ISS_CAPT_INST_MAX] = { 0x36 };
    UInt8 devAddrImx104[ISS_CAPT_INST_MAX] = { 0x21 };
    UInt8 devAddrAl30210[ISS_CAPT_INST_MAX] = { 0x21 };
#endif
#ifdef BOARD_TI_EVM
    UInt8 devAddrAr0331[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrMn34041[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrImx035[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrOv2715[ISS_CAPT_INST_MAX] = { 0x36 };
    UInt8 devAddrImx036[ISS_CAPT_INST_MAX] = { 0x30 };
    UInt8 devAddrOv9712[ISS_CAPT_INST_MAX] = { 0x48 };
    UInt8 devAddrOv10630[ISS_CAPT_INST_MAX] = { 0x30 };
    UInt8 devAddrMt9p031[ISS_CAPT_INST_MAX] = { 0x5D };
    UInt8 devAddrMt9d131[ISS_CAPT_INST_MAX] = { 0x5D };
    UInt8 devAddrMt9m034[ISS_CAPT_INST_MAX] = { 0x5D };
    UInt8 devAddrTvp514x[ISS_CAPT_INST_MAX] = { 0x5D };
    UInt8 devAddrMt9j003[ISS_CAPT_INST_MAX] = { 0x36 };
    UInt8 devAddrMt9m034dbl[ISS_CAPT_INST_MAX] = { 0x10 };
    UInt8 devAddrImx136[ISS_CAPT_INST_MAX] = {0x2d};//lvds324
    UInt8 devAddrOv7740[ISS_CAPT_INST_MAX] = { 0x21 };
    UInt8 devAddrOv2710[ISS_CAPT_INST_MAX] = { 0x36 };
    UInt8 devAddrImx104[ISS_CAPT_INST_MAX] = { 0x21 };
    UInt8 devAddrAl30210[ISS_CAPT_INST_MAX] = { 0x21 };
#endif
    //UInt8 devAddrOv7740[ISS_CAPT_INST_MAX] = { 0x21 };
    GT_assert(GT_DEFAULT_MASK, vipInstId < ISS_CAPT_INST_MAX);

    switch (vidDecId)
    {
        case FVID2_ISS_SENSOR_AR0331_DRV:
            devAddr = devAddrAr0331[vipInstId];
            break;

        case FVID2_ISS_SENSOR_MN34041_DRV:
            devAddr = devAddrMn34041[vipInstId];
            break;

        case FVID2_ISS_SENSOR_IMX035_DRV:
            devAddr = devAddrImx035[vipInstId];
            break;

        case FVID2_ISS_SENSOR_OV2715_DRV:

            devAddr = devAddrOv2715[vipInstId];
            Vps_printf
                ("##&&Iss_platformTI814xGetSensorI2cAddr -- vipInstId=%d, devAddrOv2715[vipInstId]= %x !\n",
                 vipInstId, devAddrOv2715[vipInstId]);
            break;

        case FVID2_ISS_SENSOR_IMX036_DRV:
            devAddr = devAddrImx036[vipInstId];
            break;

        case FVID2_ISS_SENSOR_OV9712_DRV:
            devAddr = devAddrOv9712[vipInstId];

            break;

        case FVID2_ISS_SENSOR_OV10630_DRV:
            devAddr = devAddrOv10630[vipInstId];

            break;

        case FVID2_ISS_SENSOR_MT9P031_DRV:
            devAddr = devAddrMt9p031[vipInstId];

            break;

        case FVID2_ISS_SENSOR_MT9D131_DRV:

            devAddr = devAddrMt9d131[vipInstId];
            break;

        case FVID2_ISS_SENSOR_MT9M034_DRV:
            devAddr = devAddrMt9m034[vipInstId];
            break;
        case FVID2_ISS_SENSOR_TVP514X_DRV:
            devAddr = devAddrTvp514x[vipInstId];
            break;
        case FVID2_ISS_SENSOR_AR0330_DRV:
            devAddr = devAddrAr0330[vipInstId];
            break;
		case FVID2_ISS_SENSOR_MT9M034_DUAL_HEAD_BOARD_DRV:
			devAddr = devAddrMt9m034dbl[vipInstId];
			break;
        case FVID2_ISS_SENSOR_MT9J003_DRV:
            devAddr = devAddrMt9j003[vipInstId];
            break;
        case FVID2_ISS_SENSOR_OV7740_DRV:
            devAddr = devAddrOv7740[vipInstId];
            break;
        case FVID2_ISS_SENSOR_OV2710_DRV:
            devAddr = devAddrOv2710[vipInstId];
            break;
        case FVID2_ISS_SENSOR_IMX136_DRV:
            devAddr = devAddrImx136[vipInstId];
            break;
        case FVID2_ISS_SENSOR_IMX104_DRV:
	       devAddr = devAddrImx104[vipInstId];
            break;	
        case FVID2_ISS_SENSOR_AL30210_DRV:
            devAddr = devAddrAl30210[vipInstId];
            break;	
		case FVID2_ISS_SENSOR_IMX122_DRV:
			devAddr = devAddrImx122[vipInstId];
            break;	
        default:
            GT_0trace(GT_DEFAULT_MASK, GT_ERR, "Invalid decoder ID\n");
            break;
    }

    return (devAddr);
}

#define CSL_TI814x_ISS_BASE 0x0                            // to be done
#define ISSHAL_VIP_INST_MAX 0x1                            // to be done
#define ISSHAL_ISS_CLKC_VIP0 0x1                           // to be done
#define ISSHAL_ISS_CLKC_VIP1 0x1                           // to be done

/* ===================================================================
 *  @func     Iss_platformTI814xSetPinMux                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
static Int32 Iss_platformTI814xSetPinMux(void)
{
#ifdef CBB_PLATFORM
	//	
#else
    /* Vout 0 configuration DVO2 Function 1 */
    /* TODO There are two pins for the fid. Need to see whichone is used */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AB8) = 0x2;     /* vout0_fid_mux1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0ABC) = 0x1;     /* vout0_clk */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AC0) = 0x1;     /* vout0_hsync */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AC4) = 0x1;     /* vout0_vsync */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AC8) = 0x80;    /* vout0_avid */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0ACC) = 0x80;    /* vout0_b_cb_c[2] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AD0) = 0x80;    /* vout0_b_cb_c[3] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AD4) = 0x1;     /* vout0_b_cb_c[4] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AD8) = 0x1;     /* vout0_b_cb_c[5] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0ADC) = 0x1;     /* vout0_b_cb_c[6] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AE0) = 0x1;     /* vout0_b_cb_c[7] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AE4) = 0x1;     /* vout0_b_cb_c[8] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AE8) = 0x1;     /* vout0_b_cb_c[9] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AEC) = 0x80;    /* vout0_g_y_yc[2] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AF0) = 0x1;     /* vout0_g_y_yc[3] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AF4) = 0x1;     /* vout0_g_y_yc[4] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AF8) = 0x1;     /* vout0_g_y_yc[5] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AFC) = 0x1;     /* vout0_g_y_yc[6] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B00) = 0x1;     /* vout0_g_y_yc[7] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B04) = 0x1;     /* vout0_g_y_yc[8] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B08) = 0x1;     /* vout0_g_y_yc[9] 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B0C) = 0x80;    /* vout0_r_cr[2] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B10) = 0x1;     /* vout0_r_cr[3] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B14) = 0x1;     /* vout0_r_cr[4] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B18) = 0x1;     /* vout0_r_cr[5] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B1C) = 0x1;     /* vout0_r_cr[6] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B20) = 0x1;     /* vout0_r_cr[7] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B24) = 0x1;     /* vout0_r_cr[8] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B28) = 0x1;     /* vout0_r_cr[9] */

    /* HDMI I2C_scl and I2C_sda Function 2 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0934) = 0xE0002; /* hdmi_ddc_scl_mux0 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0938) = 0xE0002; /* hdmi_ddc_sda_mux0 
                                                            */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x09BC) = 0x40010; /* hdmi_hpd_mux0
                                                            * pinmmr112[4] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x09B8) = 0x60010; /* hdmi_cec_mux0
                                                            * pinmmr111[4] */
    /* TODO HDMI CEC and HPD to be added in pinmux */
    /* Currently its shared with GPMC. */

    /* VIN0 TODO Do we need to enable RXACTIVE Bit in pinmux for input pins? */
    /* Vin0 hsync1 and vin0 vsync1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A14) = 0x40001; /* vin0_clk1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A18) = 0xE0001; /* vin0_de0_mux0 - 
                                                            * DeSelect input */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A1C) = 0xE0001; /* vin0_fld0_mux0 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A20) = 0xC0001; /* vin0_clk0 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A24) = 0xE0001; /* vin0_hsync0 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A28) = 0xE0001; /* vin0_vsync0 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A2C) = 0xC0001; /* vin0_d0 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A30) = 0xC0001; /* vin0_d1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A34) = 0xC0001; /* vin0_d2 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A38) = 0xC0001; /* vin0_d3 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A3c) = 0xC0001; /* vin0_d4 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A40) = 0xC0001; /* vin0_d5 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A44) = 0xC0001; /* vin0_d6 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A48) = 0xC0001; /* vin0_d7 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A4c) = 0xC0001; /* vin0_d8 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A50) = 0xC0001; /* vin0_d9 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A54) = 0xC0080; /* vin0_d10 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A58) = 0xC0080; /* vin0_d11 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A5C) = 0x40001; /* vin0_d12 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A60) = 0xC0080; /* vin0_d13 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A64) = 0xC0080; /* vin0_d14 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A68) = 0xC0080; /* vin0_d15 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A6C) = 0xE0002; /* vin0_d16 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A70) = 0xC0002; /* vin0_d17 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A74) = 0xE0002; /* vin0_d18 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A78) = 0xE0002; /* vin0_d19 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A7C) = 0xC0002; /* vin0_d20 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A80) = 0x40002; /* vin0_d21 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A84) = 0x40002; /* vin0_d22 */
#ifdef 	IMGS_OMNIVISION_OV7740
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A88) = 0x50001;	 /* vin0_d23 */
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A8C) = 0x50001;	 /* vin0_de0_mux1 */
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A90) = 0x50001;	/* vin0_de1 */
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A94) = 0x50001;	 /* vin0_fld0_mux1 */
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A98) = 0x50001;	 /* vin0_fld1 */
#else
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A88) = 0x40002; /* vin0_d23 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A8C) = 0x60002; /* vin0_de0_mux1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A90) = 0x60002; /* vin0_de1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A94) = 0x60002; /* vin0_fld0_mux1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A98) = 0x60002; /* vin0_fld1 */
#endif
    /* VIN1 Configuration Function 3 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B2C) = 0x40001; /* vin1_hsync0 */
    /* this is function 2 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x09F0) = 0x60001; /* vin1_clk1 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B30) = 0x40001; /* vin1_vsync0 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B34) = 0x40001; /* vin1_de0 */

    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B38) = 0x40001; /* vin1_clk0 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B3C) = 0x40001; /* vin1a_d[0] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B40) = 0x40001; /* vin1a_d[1] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B44) = 0x40001; /* vin1a_d[2] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B48) = 0x40001; /* vin1a_d[3] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B4C) = 0x40001; /* vin1a_d[4] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B50) = 0x40001; /* vin1a_d[5] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B54) = 0x40001; /* vin1a_d[6] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B58) = 0x40080; /* vin1a_d[8] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B5C) = 0x40080; /* vin1a_d[9] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B60) = 0x40080; /* vin1a_d[10] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B64) = 0x40080; /* vin1a_d[11] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B68) = 0x40080; /* vin1a_d[12] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B6C) = 0x40080; /* vin1a_d[13] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B70) = 0x40080; /* vin1a_d[14] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B74) = 0x40001; /* vin1a_d[15] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B78) = 0x40001; /* vin1a_d[16] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B7C) = 0x40001; /* vin1a_d[17] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B80) = 0x40001; /* vin1a_d[18] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B84) = 0x40001; /* vin1a_d[19] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B88) = 0x40001; /* vin1a_d[20] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B8C) = 0x60080; /* vin1a_d[21] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B90) = 0x60001; /* vin1a_d[22] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B94) = 0x40001; /* vin1a_d[23] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0B98) = 0x60001; /* vin1a_d[7] */

    /* Function 2 */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BA8) = 0x40001; /* vin1a_d[0] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BAC) = 0x40001; /* vin1a_d[1] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BB0) = 0x40001; /* vin1a_d[2] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BB4) = 0x1;     /* vin1a_d[3] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BB8) = 0x1;     /* vin1a_d[4] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BBC) = 0x1;     /* vin1a_d[5] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BC0) = 0x40001; /* vin1a_d[6] */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0BC4) = 0x40001; /* vin1a_d[7] */

    /* I2c2 configuration Function 6 */
#if defined(TI_814X_BUILD)	
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0924) = 0xE0020; /* i2c2_scl_mux0 */
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0928) = 0xE0020; /* i2c2_sda_mux0 */
#endif

#if defined(TI_8107_BUILD)
	#if defined IMGS_MICRON_MT9M034
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0924) = 0xE0020; /* i2c2_scl_mux0 */
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0928) = 0xE0020; /* i2c2_sda_mux0 */
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A18) = 0xE0020;
	REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A1C) = 0xE0020;
	#endif
#endif

    /* TODO Find proper place for this Set the divider for the SYSCLK10 */
    *(UInt32 *) 0x48180324 = 3;
#ifndef IMGS_OMNIVISION_OV7740	
    /* Iss specific Input PIN MUX settings */
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A6C) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A70) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A74) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A78) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A7C) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A80) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A84) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A88) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A8C) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A90) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A94) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A98) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A9C) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AA0) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AA4) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AA8) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AAC) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AB0) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AB4) = 0x50002;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AB8) = 0x50002;

    /* setup I2C2 pin mux */
#ifdef IMGS_MICRON_MT9J003	
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0924) = 0x00020;
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0928) = 0x00020;
#endif
#ifdef IMGS_MICRON_MT9P031	
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AAC ) = 0x00060002;    // CAM_HSYNC        PINCNTL172[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AB0 ) = 0x00060002;    // CAM_VSYNC        PINCNTL173[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AB8 ) = 0x00060002;    // CAM_PCLK         PINCNTL175[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A58 ) = 0x00060020;    // CAM_WEn          PINCNTL151[5] cam_de_mux1
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A60 ) = 0x00060080;    // gpio2[18]        PINCNTL153[7] // 0x00060080   CAM_RST          PINCNTL153[5]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A64 ) = 0x00060020;    // CAM_STROBE       PINCNTL154[5]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A68 ) = 0x00060020;    // CAM_SHTR         PINCNTL155[5]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AA8 ) = 0x00060002;    // CAM_D0           PINCNTL171[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AA4 ) = 0x00060002;    // CAM_D1           PINCNTL170[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0AA0 ) = 0x00060002;    // CAM_D2           PINCNTL169[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A9C ) = 0x00060002;    // CAM_D3           PINCNTL168[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A98 ) = 0x00060002;    // CAM_D4           PINCNTL167[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A94 ) = 0x00060002;    // CAM_D5           PINCNTL166[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A90 ) = 0x00060002;    // CAM_D6           PINCNTL165[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A8C ) = 0x00060002;    // CAM_D7           PINCNTL164[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A6C ) = 0x00060002;    // CAM_D8           PINCNTL156[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A70 ) = 0x00060002;    // CAM_D9           PINCNTL157[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A74 ) = 0x00060002;    // CAM_D10          PINCNTL158[1]
    REG32(CSL_TI814x_CTRL_MODULE_BASE + 0x0A78 ) = 0x00060002;    // CAM_D11          PINCNTL159[1]
#endif	
#endif	
#endif
    return (FVID2_SOK);
}

/* ===================================================================
 *  @func     Iss_platformTI814xGetCpuRev                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Iss_PlatformCpuRev Iss_platformTI814xGetCpuRev(void)
{
    UInt32 cpuId, cpuRev;

    Iss_PlatformCpuRev cpuRevEnum;

    /* Read CPU ID */
    cpuId = REG32(ISS_CTRL_MODULE_DEV_CFG_BASE_ADDR + 0x0000u);

    cpuRev = ((cpuId >> 28u) & 0x0Fu);
    switch (cpuRev)
    {
        case 0x0u:
            cpuRevEnum = ISS_PLATFORM_CPU_REV_1_0;
            break;

        case 0xCu:                                        /* Certain intial
                                                            * sample of PG
                                                            * 2.1 has C but
                                                            * the production
                                                            * samples should
                                                            * read out 3 */
        case 0x3u:
            cpuRevEnum = ISS_PLATFORM_CPU_REV_2_1;
            break;

        default:
            // cpuRevEnum = ISS_PLATFORM_CPU_REV_UNKNOWN;
            Vps_printf
                (" PLATFORM: UNKNOWN CPU detected, defaulting to ISS_PLATFORM_CPU_REV_2_1\n");
            cpuRevEnum = ISS_PLATFORM_CPU_REV_2_1;
            break;
    }

    return (cpuRevEnum);
}

/* ===================================================================
 *  @func     Iss_platformTI814xGetBaseBoardRev                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Iss_PlatformBoardRev Iss_platformTI814xGetBaseBoardRev(void)
{
    /* Only one revision of board till date */
    return (ISS_PLATFORM_BOARD_REV_A);
}

/* ===================================================================
 *  @func     Iss_platformTI814xGetDcBoardRev                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Iss_PlatformBoardRev Iss_platformTI814xGetDcBoardRev(void)
{
    /* Only one revision of board till date */
    return (ISS_PLATFORM_BOARD_REV_A);
}

/* ===================================================================
 *  @func     Iss_platformTI814xSelectVideoDecoder                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
/* ===================================================================
 *  @func     Iss_platformTI814xSelectSensor                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 Iss_platformTI814xSelectSensor(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt32 i2cInstId = Iss_platformTI814xGetI2cInstId();

    UInt32 i2cDevAddr;

    UInt8 regValue[2];

    Iss_PlatformBoardId boardId;

    Int32 status = FVID2_SOK;

    boardId = Iss_platformGetBoardId();

    /* In TI814X, GMII1 and SiL9135A are muxed on the VC board. */
    if (boardId == ISS_PLATFORM_BOARD_VCAM)
    {
        i2cDevAddr = gTi814xBoardVer.vcIoExpAddr;
        regValue[0] = 0xEF;
        regValue[1] = 0xFF;
    }

    status = Iss_deviceRawWrite8(i2cInstId, i2cDevAddr, regValue, 2);
    return status;
}
