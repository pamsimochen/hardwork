/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_vps.c
 *
 *  \brief VPS HAL Source file.
 *  This file implements the HAL APIs of the VPS.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <xdc/runtime/System.h>

#include <ti/sysbios/knl/Task.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/cslr/cslr_hd_vps.h>
#include <ti/psp/platforms/vps_platform.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VPS_RANGE_MAP_OFFSET        (7u)

/**
 *  enum VpsHal_VpsConfigOvlyReg
 *  \brief Enums for the configuration overlay register offsets - used as array indices
 *  to register offset array in Comp_Obj object. These enums defines the
 *  registers that will be used to form the configuration register overlay for
 *  DEI module. This in turn is used by client drivers to configure registers
 *  using VPDMA during context-switching (software channels with different
 *  configurations).
 *
 */
typedef enum
{
    VPSHAL_VPS_CONFIG_OVLY_VPS_CLKC_DPS = 0,
    VPSHAL_VPS_CONFIG_OVLY_NUM_REG
    /**< This should be the last enum */
} VpsHal_VpsConfigOvlyReg;

typedef enum
{
    VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC = 0,
    VPSHAL_VPS_VIP_CONFIG_OVLY_NUM_REG
    /**< This should be the last enum */
} VpsHal_VpsVipConfigOvlyReg;

typedef enum
{
    VPSHAL_VPS_MAIN_DPS_All = 0,
    /**< Write All fields in the DPS register */
    VPSHAL_VPS_MAIN_DPS_VCOMP_PIP_SELECT,
    /**< Only VCOMP PIP Select is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_VCOMP_MAIN_DISABLE,
    /**< Only VCOMP Main Disable is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_HDCOMP_DVO2_SELECT,
    /**< Only HDCOMP DVO2 is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_SDVENC_SELECT,
    /**< Only SDVENC Select is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_SC_WB2_SELECT,
    /**< Only SC WB2 is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_SEC0_SELECT,
    /**< Only secondary 0 is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_SEC1_SELECT,
    /**< Only secondary 1 is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_COMP_DECOMP_PRI_BYPASS,
    /**< Only Primary COMP/DECOMP Bypass is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_COMP_DECOMP_AUX_BYPASS,
    /**< Only Auxiliary COMP/DECOMP Bypass is written in the DPS register */
    VPSHAL_VPS_MAIN_DPS_NF_BYPASS
    /**< Only COMP/DECOMP Bypass NF is written in the DPS register */
} VpsHal_VpsMainDps;

typedef enum
{
    VPSHAL_VPS_VIP_DPS_ALL = 0,
    VPSHAL_VPS_VIP_DPS_CSC_SRC_SELECT,
    VPSHAL_VPS_VIP_DPS_SC_SRC_SELECT,
    VPSHAL_VPS_VIP_DPS_RGB_SRC_SELECT,
    VPSHAL_VPS_VIP_DPS_RGB_OUT_LO_SELECT,
    VPSHAL_VPS_VIP_DPS_RGB_OUT_HI_SELECT,
    VPSHAL_VPS_VIP_DPS_CHR_DS_0_SRC_SELECT,
    VPSHAL_VPS_VIP_DPS_CHR_DS_1_SRC_SELECT,
    VPSHAL_VPS_VIP_DPS_MULTI_CHANNEL_SELECT,
    VPSHAL_VPS_VIP_DPS_CHR_DS_0_BYPASS,
    VPSHAL_VPS_VIP_DPS_CHR_DS_1_BYPASS
} VpsHal_VpsVipDps;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    UInt32              *muxRegOffset[VPSHAL_VPS_CONFIG_OVLY_NUM_REG];
    UInt32               muxVirtRegOffset[VPSHAL_VPS_CONFIG_OVLY_NUM_REG];
    UInt32               muxConfigOvlySize;
    UInt32              *vc1RegOffset[VPSHAL_VPS_CONFIG_OVLY_NUM_REG];
    UInt32               vc1VirtRegOffset[VPSHAL_VPS_CONFIG_OVLY_NUM_REG];
    UInt32               vc1ConfigOvlySize;
    UInt32              *vipMuxRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_NUM_REG];
    UInt32               vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_NUM_REG];
    UInt32               vipMuxConfigOvlySize;
    UInt32               valClkcVencEna;
} VpsHal_RegOvly;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  Pointer to register overlay structure for the VPS registers
 */
CSL_VpsRegsOvly VpsRegOvly;
VpsHal_RegOvly    regOvly;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsHal_vpsInit
 *  \brief Function to initialize VPS HAL. Currently It does not
 *   do anything.
 *
 *  \param initParams     Init Params containing Base Address
 *  \param arg            Currently unused. For the future reference
 *
 *  \return               Returns 0 on success else returns error value.
 */
Int VpsHal_vpsInit(VpsHal_VpsInstParams *initParams, Ptr arg)
{
    GT_assert( GT_DEFAULT_MASK, (NULL != initParams));

    VpsRegOvly = (CSL_VpsRegsOvly)initParams->baseAddress;

    regOvly.muxRegOffset[VPSHAL_VPS_CONFIG_OVLY_VPS_CLKC_DPS] =
                                        (UInt32 *) &(VpsRegOvly->CLKC_DPS);
    /* Get the size of the overlay for COMP registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    regOvly.muxConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                  regOvly.muxRegOffset,
                                  VPSHAL_VPS_CONFIG_OVLY_NUM_REG,
                                  regOvly.muxVirtRegOffset);

    regOvly.vc1RegOffset[VPSHAL_VPS_CONFIG_OVLY_VPS_CLKC_DPS] =
                                    (UInt32 *) &(VpsRegOvly->CLKC_RANGE_MAP);
    /* Get the size of the overlay for VC1 registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    regOvly.vc1ConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                  regOvly.vc1RegOffset,
                                  VPSHAL_VPS_CONFIG_OVLY_NUM_REG,
                                  regOvly.vc1VirtRegOffset);

    regOvly.vipMuxRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC] =
                                        (UInt32 *) &(VpsRegOvly->CLKC_VIP0DPS);
    /* Get the size of the overlay for VIP Mux registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    regOvly.vipMuxConfigOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                  regOvly.vipMuxRegOffset,
                                  VPSHAL_VPS_VIP_CONFIG_OVLY_NUM_REG,
                                  regOvly.vipMuxVirtRegOffset);

    /* Disable All the Muxes */
    VpsHal_vpsVencMuxSrcSelect(
        VPSHAL_VPS_VENC_MUX_SD,
        VPSHAL_VPS_VENC_MUX_SRC_DISABLED,
        NULL);
    VpsHal_vpsVencMuxSrcSelect(
        VPSHAL_VPS_VENC_MUX_HDCOMP,
        VPSHAL_VPS_VENC_MUX_SRC_DISABLED,
        NULL);
    VpsHal_vpsVencMuxSrcSelect(
        VPSHAL_VPS_VENC_MUX_VCOMP,
        VPSHAL_VPS_VENC_MUX_SRC_DISABLED,
        NULL);
    VpsHal_vpsVencMuxSrcSelect(
        VPSHAL_VPS_VENC_MUX_PRI,
        VPSHAL_VPS_VENC_MUX_SRC_DISABLED,
        NULL);

    /* Disable all vencs at the beginning */
    regOvly.valClkcVencEna = 0u;
    VpsHal_vpsClkcVencEnable(CSL_HD_VPS_CLKC_VENC_ENA_HDCOMP_ENABLE_MASK |
                             CSL_HD_VPS_CLKC_VENC_ENA_HDMI_ENABLE_MASK |
                             CSL_HD_VPS_CLKC_VENC_ENA_DVO2_ENABLE_MASK |
                             CSL_HD_VPS_CLKC_VENC_ENA_SD_ENABLE_MASK,
                             FALSE);

    return (0);
}

/**
 *  VpsHal_vpsDeInit
 *  \brief Function to de-Initialize VPS HAL. Currently It
 *  does not do anything.
 *
 *  \param arg    Currently unused. For the future reference
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsDeInit(Ptr arg)
{
    return (0);
}



/**
 *  VpsHal_vpsGetOvlySize
 *  \brief Function to get the register overlay size for the mux register
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param  ovlyType NONE.
 *
 *  \return          Register overlay size
 */
UInt32 VpsHal_vpsGetOvlySize(VpsHal_OvlyType ovlyType)
{
    UInt32 ovlySize;
    if (VPSHAL_OVLY_TYPE_MUX == ovlyType)
    {
        ovlySize = regOvly.muxConfigOvlySize;
    }
    else
    {
        ovlySize = regOvly.vc1ConfigOvlySize;
    }
    return (ovlySize);
}



/**
 *  VpsHal_vpsCreateConfigOvly
 *  \brief Function to create config overlay in the given memory.
 *
 *  \param  configOvlyPtr Pointer to the memory where configuration
 *                        overlay is to be created.
 *  \param  ovlyType      NONE
 *
 *  \return               Register overlay size
 */
Int32 VpsHal_vpsCreateConfigOvly(VpsHal_OvlyType ovlyType, Ptr configOvlyPtr)
{
    Int ret;

    /* Check for NULL pointer */
    GT_assert( GT_DEFAULT_MASK, NULL != configOvlyPtr);

    if (VPSHAL_OVLY_TYPE_MUX == ovlyType)
    {
        /* Create the register overlay */
        ret = VpsHal_vpdmaCreateRegOverlay(
              regOvly.muxRegOffset,
              VPSHAL_VPS_CONFIG_OVLY_NUM_REG,
              configOvlyPtr);
    }
    else
    {
        /* Create the register overlay */
        ret = VpsHal_vpdmaCreateRegOverlay(
              regOvly.vc1RegOffset,
              VPSHAL_VPS_CONFIG_OVLY_NUM_REG,
              configOvlyPtr);
    }

    return (ret);
}



/**
 *  \brief Reset and enable all module CLKs in VPS
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsClkcModuleEnableAll()
{
    volatile UInt32 value = 0x01031FFF;
    volatile UInt32 delay;

    VpsRegOvly->CLKC_CLKEN = value;

#if defined (PLATFORM_SIM) || defined(PLATFORM_ZEBU)
    Task_sleep(1);
#else
    Task_sleep(10);
#endif

#ifdef PLATFORM_SIM
    VpsRegOvly->CLKC_RST = value;

    for(delay=0; delay < 1000; delay++);
#endif

    if ((VPS_PLATFORM_ID_EVM_TI816x == Vps_platformGetId()) &&
        (VPS_PLATFORM_CPU_REV_2_0 <= Vps_platformGetCpuRev()))
    {
        VpsRegOvly->CLKC_RST = (0x1u << VPSHAL_VPS_CLKC_MAIN);
        for(delay=0; delay < 1000; delay++);
    }

    VpsRegOvly->CLKC_RST = 0;

#if defined (PLATFORM_SIM) || defined(PLATFORM_ZEBU)
    Task_sleep(1);
#else
    Task_sleep(10);
#endif

    return (0);
}

/**
 *  VpsHal_vpsClkcModuleEnable
 *  \brief Function to enable/disable specific module
 *  in CLKC.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param module     Module for which clock is to be enabled.
 *  \param isEnabled  Flag to indicate whether to enable clock or disable.
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsClkcModuleEnable(VpsHal_VpsClkcModule module, UInt32 isEnabled)
{
    /* There is no bit to set the clock for entire main block so check
     * value of module should not be entire main block */
    GT_assert( GT_DEFAULT_MASK, (VPSHAL_VPS_CLKC_MAIN != module));

    if (VPSHAL_VPS_CLKC_MAIN > module)
    {
        #ifdef PLATFORM_SIM
        /* Enable Module clock in the VPS CLKC register. Since value of
         * module variable itself indicates the bit position, there is not
         * need to use mask and shift */
        if (TRUE == isEnabled)
        {
            VpsRegOvly->CLKC_CLKEN |= (0x1u << ((UInt32)module));
        }
        else
        {
            /* Disable Clock for the module */
            VpsRegOvly->CLKC_CLKEN &= (~(0x1u << ((UInt32)module)));
        }
        #endif
    }
    else /* Enable clock for the VENC */
    {
        if (TRUE == isEnabled)
        {
            /* Enable clock for the VENC in CLKC_VENC_ENA register. Since bit
             * position is selected from the value of the variable, there is
             * no need to use masks and shift */
            VpsRegOvly->CLKC_VENC_ENA |= (0x1u << (((UInt32)module) -
                                            VPSHAL_VPS_CLKC_HDMI_VENC));
        }
        else
        {
            /* Disable VENC Clock */
            VpsRegOvly->CLKC_VENC_ENA &= (~(0x1u << (((UInt32)module) -
                                            VPSHAL_VPS_CLKC_HDMI_VENC)));
        }
    }

    return (0);
}



/**
 *  VpsHal_vpsClkcVencEnable
 *  \brief Function to enable/disable Vencs
 *  in CLKC.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param isEnable   Flag to indicate whether to enable clock or disable.
 *  \param vencs      NONE
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsClkcVencEnable(UInt32 vencs, UInt32 isEnable)
{
    vencs = (vencs & (CSL_HD_VPS_CLKC_VENC_ENA_HDCOMP_ENABLE_MASK |
                      CSL_HD_VPS_CLKC_VENC_ENA_HDMI_ENABLE_MASK |
                      CSL_HD_VPS_CLKC_VENC_ENA_DVO2_ENABLE_MASK |
                      CSL_HD_VPS_CLKC_VENC_ENA_SD_ENABLE_MASK));
    if (TRUE == isEnable)
    {
        regOvly.valClkcVencEna |= vencs;
        /* Enable clock for the VENC in CLKC_VENC_ENA register. Since bit
         * position is selected from the value of the variable, there is
         * no need to use masks and shift */
        VpsRegOvly->CLKC_VENC_ENA = regOvly.valClkcVencEna;
    }
    else
    {
        regOvly.valClkcVencEna &= (~vencs);
        /* Disable VENC Clock */
        VpsRegOvly->CLKC_VENC_ENA = regOvly.valClkcVencEna;
    }
    return (0);
}


Int VpsHal_vpsClkcModuleReset(VpsHal_VpsClkcModule module, Bool assertReset)
{
    GT_assert( GT_DEFAULT_MASK, (module < VPSHAL_VPS_CLKC_MAIN));

    /* Reset the module. ince value of module variable itself indicates the
     * bit position, there is not need to use mask and shift*/
    if(assertReset)
    {
        VpsRegOvly->CLKC_RST |=  (0x1u << ((UInt32)module));
    }
    else
    {
        VpsRegOvly->CLKC_RST &= ~(0x1u << ((UInt32)module));
    }

    return (0);
}

Int VpsHal_vpsClkcResetModules(VpsHal_VpsClkcModule *module,
                               UInt32 num,
                               Bool assertReset)
{
    UInt32 cnt;
    UInt32 value = 0;

    GT_assert(GT_DEFAULT_MASK, (NULL != module));

    for (cnt = 0u; cnt < num; cnt ++)
    {
        GT_assert(GT_DEFAULT_MASK, (module[cnt] < VPSHAL_VPS_CLKC_MAIN));

        switch (module[cnt])
        {
            case VPSHAL_VPS_CLKC_VIP0_VIP:
                value |= CSL_HD_VPS_CLKC_RST_VIP0_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP1_VIP:
                value |= CSL_HD_VPS_CLKC_RST_VIP1_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP0_CSC:
                value |= CSL_HD_VPS_CLKC_RST_VIP0_CSC_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP1_CSC:
                value |= CSL_HD_VPS_CLKC_RST_VIP1_CSC_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP0_SC:
                value |= CSL_HD_VPS_CLKC_RST_VIP0_SC_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP1_SC:
                value |= CSL_HD_VPS_CLKC_RST_VIP1_SC_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP0_CHR_DS0:
                value |= CSL_HD_VPS_CLKC_RST_VIP0_CHRDS_0_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP1_CHR_DS0:
                value |= CSL_HD_VPS_CLKC_RST_VIP1_CHRDS_0_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP0_CHR_DS1:
                value |= CSL_HD_VPS_CLKC_RST_VIP0_CHRDS_1_DP_RST_MASK;
                break;
            case VPSHAL_VPS_CLKC_VIP1_CHR_DS1:
                value |= CSL_HD_VPS_CLKC_RST_VIP1_CHRDS_1_DP_RST_MASK;
                break;
            default:
                break;
        }
    }

    /* Reset the module. ince value of module variable itself indicates the
     * bit position, there is not need to use mask and shift*/
    if (assertReset)
    {
        VpsRegOvly->CLKC_RST |= value;
    }
    else
    {
        VpsRegOvly->CLKC_RST &= ~value;
    }

    return (0);
}


/**
 *  VpsHal_vpsModuleBypass
 *  \brief Few VPS modules can be bypassed or selected
 *  using VPS. This function is used to select/bypass those modules.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param module   Module which is to be selected/bypassed.
 *  \param bypass   Indicates whether to select or to bypass the module
 *
 *  \return         Returns 0 on success else returns error value.
 */
Int VpsHal_vpsModuleBypass(VpsHal_VpsModule module,
                           VpsHal_VpsBypassSelect bypass)
{
    UInt32 tempReg = 0u;

    switch (module)
    {
        case VPSHAL_VPS_MODULE_COMPR_AUX:
            if (VPSHAL_VPS_BYPASS_MODULE == bypass)
            {
                tempReg = (((UInt32)VPSHAL_VPS_MAIN_DPS_COMP_DECOMP_AUX_BYPASS <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                      (CSL_HD_VPS_CLKC_DPS_COMP_DECOMP_AUX_BYPASS_MASK);
            }
            else
            {
                tempReg = (((UInt32)VPSHAL_VPS_MAIN_DPS_COMP_DECOMP_AUX_BYPASS <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK);
            }
            VpsRegOvly->CLKC_DPS = tempReg;
            break;
        case VPSHAL_VPS_MODULE_COMPR_PRI:
            if (VPSHAL_VPS_BYPASS_MODULE == bypass)
            {
                tempReg = (((UInt32)VPSHAL_VPS_MAIN_DPS_COMP_DECOMP_PRI_BYPASS <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                      (CSL_HD_VPS_CLKC_DPS_COMP_DECOMP_PRI_BYPASS_MASK);
            }
            else
            {
                tempReg = (((UInt32)VPSHAL_VPS_MAIN_DPS_COMP_DECOMP_PRI_BYPASS <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_COMP_DECOMP_PRI_BYPASS_MASK);
            }
            VpsRegOvly->CLKC_DPS = tempReg;
            break;
        case VPSHAL_VPS_MODULE_NF:
            if (VPSHAL_VPS_BYPASS_MODULE == bypass)
            {
                tempReg = (((UInt32)VPSHAL_VPS_MAIN_DPS_NF_BYPASS <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                      (CSL_HD_VPS_CLKC_DPS_NF_BYPASS_SELECT_MASK);
            }
            else
            {
                tempReg = (((UInt32)VPSHAL_VPS_MAIN_DPS_NF_BYPASS <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK);
            }
            VpsRegOvly->CLKC_DPS = tempReg;
            break;
        case VPSHAL_VPS_MODULE_VIP0_CHR_DS0:
            if (VPSHAL_VPS_BYPASS_MODULE == bypass)
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_0_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK);
            }
            else
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_0_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                    (CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_0_BYPASS_MASK);
            }
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
            break;
        case VPSHAL_VPS_MODULE_VIP0_CHR_DS1:
            if (VPSHAL_VPS_BYPASS_MODULE == bypass)
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_1_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK);
            }
            else
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_1_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                    (CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_1_BYPASS_MASK);
            }
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
            break;
        case VPSHAL_VPS_MODULE_VIP1_CHR_DS0:
            if (VPSHAL_VPS_BYPASS_MODULE == bypass)
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_0_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK);
            }
            else
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_0_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                    (CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CHR_DS_0_BYPASS_MASK);
            }
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
            break;
        case VPSHAL_VPS_MODULE_VIP1_CHR_DS1:
            if (VPSHAL_VPS_BYPASS_MODULE == bypass)
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_1_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK);
            }
            else
            {
                tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_1_BYPASS <<
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                    CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                    (CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CHR_DS_1_BYPASS_MASK);
            }
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
            break;
    }
    return (0);
}

/**
 *  VpsHal_vpsSetIntcEoi
 *  \brief Function to set the End of Interrupt in INC_EOI
 *  register
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param ipgenericirq   IP Generic IRQ number
 *
 *  \return               Returns 0 on success else returns error value.
 */
Int VpsHal_vpsSetIntcEoi(VpsHal_VpsIpGenericIrq ipgenericirq)
{
    /* Set the value of the enum directly into register */
    /* TODO: This register may need to be protected by hardware interrupt */
    VpsRegOvly->INTC_EOI = (ipgenericirq <<
                                CSL_HD_VPS_INTC_EOI_EOI_VECTOR_SHIFT) &
                                CSL_HD_VPS_INTC_EOI_EOI_VECTOR_MASK;
    return (0);
}

/**
 *  VpsHal_vpsSecMuxSelect
 *  \brief Function to select source of the
 *  multiplexer on the Secondary path. It takes multiplexer instance and
 *  selects the source of the multiplexer
 *
 *  Clock must be enabled for the secondary path and VpsHal_vpsInit
 *  function must be called prior to this.
 *
 *  \param secInst       Secondary path
 *  \param muxSrc        Source of the multiplexer
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vpsSecMuxSelect(VpsHal_VpsSecInst secInst,
                           VpsHal_VpsSecMuxSrc muxSrc)
{
    UInt32 tempReg = 0u;

    if (VPSHAL_VPS_SEC_INST_0 == secInst)
    {
        if (VPSHAL_VPS_SEC_MUX_SRC_SEC == muxSrc)
        {
            /* Select mux source for secondary path 1 */
            tempReg = ((VPSHAL_VPS_MAIN_DPS_SEC0_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                      (CSL_HD_VPS_CLKC_DPS_SEC0_SELECT_MASK);
        }
        else
        {
            /* Select mux source for DEI */
            tempReg = ((VPSHAL_VPS_MAIN_DPS_SEC0_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK);
        }
        VpsRegOvly->CLKC_DPS = tempReg;
    }
    else
    {
        if (VPSHAL_VPS_SEC_MUX_SRC_SEC == muxSrc)
        {
            /* Select mux source for secondary path 1 */
            tempReg = ((VPSHAL_VPS_MAIN_DPS_SEC1_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                      (CSL_HD_VPS_CLKC_DPS_SEC1_SELECT_MASK);
        }
        else
        {
            /* Select mux source for DEI */
            tempReg = ((VPSHAL_VPS_MAIN_DPS_SEC1_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK);
        }
        VpsRegOvly->CLKC_DPS = tempReg;
    }
    return (0);
}

/**
 *  VpsHal_vpsWb2MuxSrcSelect
 *  \brief Function to select source of the
 *  multiplexer present on the scalar write back path
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param muxSrc Source of the multiplexer
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsWb2MuxSrcSelect(VpsHal_VpsWb2MuxSrc muxSrc)
{
    UInt32 tempReg = 0u;
    tempReg = ((VPSHAL_VPS_MAIN_DPS_SC_WB2_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
               ((muxSrc <<
                        CSL_HD_VPS_CLKC_DPS_SC_WB2_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_SC_WB2_SELECT_MASK);
    /* Select the input source */
    VpsRegOvly->CLKC_DPS = tempReg;
    return (0);
}

/**
 *  VpsHal_vpsVencMuxSrcSelect
 *  \brief Function to select source of the
 *  multiplexer, which provides input to the VENCs or to the VCOMP
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param vencMuxInst   Instance of the Multiplexer
 *  \param muxSrc        Source of the multiplexer
 *  \param configOvlyPtr NONE
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVencMuxSrcSelect(VpsHal_VpsVencMux vencMuxInst,
                               VpsHal_VpsVencMuxSrc muxSrc,
                               Ptr configOvlyPtr)
{
    UInt32 tempReg, *ovlyPtr = NULL;

    /* TODO: It is not possible to connect same input to multiple venc so check
     * the error condition */
    /* Secondary is supported only for sdvenc multiplexer */
    GT_assert( GT_DEFAULT_MASK, !((VPSHAL_VPS_VENC_MUX_SD != vencMuxInst) &&
                    (VPSHAL_VPS_VENC_MUX_SRC_SEC1 == muxSrc)));
    tempReg = 0u;
    if (VPSHAL_VPS_VENC_MUX_SD == vencMuxInst)
    {
        /* Select the input source */
        tempReg = ((VPSHAL_VPS_MAIN_DPS_SDVENC_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                  ((muxSrc <<
                        CSL_HD_VPS_CLKC_DPS_SD_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_SD_SELECT_MASK);
    }
    else if (VPSHAL_VPS_VENC_MUX_HDCOMP == vencMuxInst)
    {
        /* Select the input source */
        tempReg = ((VPSHAL_VPS_MAIN_DPS_HDCOMP_DVO2_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                  ((muxSrc <<
                        CSL_HD_VPS_CLKC_DPS_HDCOMP_DVO2_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_HDCOMP_DVO2_SELECT_MASK);
    }
    else if (VPSHAL_VPS_VENC_MUX_PRI == vencMuxInst)
    {
        /* Since there is only one input to this mux, If mux
           source is disabled, disable it, otherwise enable it */
        if (VPSHAL_VPS_VENC_MUX_SRC_DISABLED == muxSrc)
        {
            tempReg = ((VPSHAL_VPS_MAIN_DPS_VCOMP_MAIN_DISABLE <<
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                (CSL_HD_VPS_CLKC_DPS_VCOMP_MAIN_DISABLE_MASK);
        }
        else
        {
            tempReg = ((VPSHAL_VPS_MAIN_DPS_VCOMP_MAIN_DISABLE <<
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK);
        }
    }
    else /* VCOMP PIP multiplexer */
    {
        /* Select the input source */
        tempReg = ((VPSHAL_VPS_MAIN_DPS_VCOMP_PIP_SELECT <<
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                  ((muxSrc <<
                        CSL_HD_VPS_CLKC_DPS_VCOMP_PIP_SELECT_SHIFT) &
                        CSL_HD_VPS_CLKC_DPS_VCOMP_PIP_SELECT_MASK);
    }
    if (NULL == configOvlyPtr)
    {
        VpsRegOvly->CLKC_DPS = tempReg;
    }
    else
    {
        ovlyPtr = (UInt32 *)configOvlyPtr +
            regOvly.muxVirtRegOffset[VPSHAL_VPS_CONFIG_OVLY_VPS_CLKC_DPS];
        *ovlyPtr = tempReg;
    }
    return (0);
}


/**
 *  VpsHal_vpsVipChrdsMuxSrcSelect
 *  \brief Function to select source of the
 *  multiplexer, which provides input to the CHR_DS in VIP
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param vipInst     VIP Instance
 *  \param chrdsInst   CHR_DS instance within VIP
 *  \param muxSrc      Source of the multiplexer
 *
 *  \return            Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVipChrdsMuxSrcSelect(VpsHal_VpsVipInst vipInst,
                                   VpsHal_VpsChrdsInst chrdsInst,
                                   VpsHal_VpsVipChrdsMuxSrc muxSrc,
                                   Ptr configOvlyPtr)
{
    UInt32 tempReg = 0u;

    if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
    {
        if (VPSHAL_VPS_CHRDS_INST_0 == chrdsInst)
        {
            tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_0_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                      (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_0_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_0_SRC_SELECT_MASK);
        }
        else /* Chroma Down Sampler 2 */
        {
            tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_1_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                      (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_1_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_1_SRC_SELECT_MASK);
        }
        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    else /* VIP 2 */
    {
        if (VPSHAL_VPS_CHRDS_INST_0 == chrdsInst)
        {
            tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_0_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                      (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CHR_DS_0_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CHR_DS_0_SRC_SELECT_MASK);
        }
        else /* Chroma Down Sampler 2 */
        {
            tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CHR_DS_1_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                      (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CHR_DS_1_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CHR_DS_1_SRC_SELECT_MASK);
        }
        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    return (0);
}

/**
 *  VpsHal_vpsVipScMuxSrcSelect
 *  \brief Function to select source of the
 *  multiplexer, which provides input to the Scalar in VIP
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param vipInst VIP Instance
 *  \param muxSrc  Source of the multiplexer
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVipScMuxSrcSelect(VpsHal_VpsVipInst vipInst,
                                VpsHal_VpsVipScMuxSrc muxSrc,
                                Ptr configOvlyPtr)
{
    UInt32 tempReg = 0u;

    if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
    {
        tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_SC_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                  (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_SC_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_SC_SRC_SELECT_MASK);
        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    else /* VIP 2 */
    {
        tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_SC_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                  (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_SC_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_SC_SRC_SELECT_MASK);
        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    return (0);
}

/**
 *  VpsHal_vpsVipCscMuxSrcSelect
 *  \brief Function to select source of the
 *  multiplexer, which provides input to the CSC in VIP
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param vipInst  VIP Instance
 *  \param muxSrc   Source of the multiplexer
 *
 *  \return         Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVipCscMuxSrcSelect(VpsHal_VpsVipInst vipInst,
                                 VpsHal_VpsVipCscMuxSrc muxSrc,
                                 Ptr configOvlyPtr)
{
    UInt32 tempReg = 0u;

    if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
    {
        tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CSC_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                (((UInt32)muxSrc <<
            CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CSC_SRC_SELECT_SHIFT) &
            CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CSC_SRC_SELECT_MASK);
        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    else /* VIP 2 */
    {
        tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_CSC_SRC_SELECT  <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                  (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CSC_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP1DPS_VIP1_CSC_SRC_SELECT_MASK);
        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    return (0);
}

/**
 *  VpsHal_vpsVipRgbMuxSrcSelect
 *  \brief Function to select RGB source of the
 *  multiplexer in VIP
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param vipInst VIP Instance
 *  \param muxSrc  Source of the multiplexer
 *
 *  \return        Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVipRgbMuxSrcSelect(VpsHal_VpsVipInst vipInst,
                                 VpsHal_VpsVipRgbMuxSrc muxSrc,
                                 Ptr configOvlyPtr)
{
    UInt32 tempReg = 0u;

    if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
    {
        tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_RGB_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                  (((UInt32)muxSrc <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_RGB_SRC_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_RGB_SRC_SELECT_MASK);
        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    else /* VIP 2 */
    {
        tempReg = (((UInt32)VPSHAL_VPS_VIP_DPS_RGB_SRC_SELECT <<
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
                  (((UInt32)muxSrc <<
            CSL_HD_VPS_CLKC_VIP1DPS_VIP1_RGB_SRC_SELECT_SHIFT) &
            CSL_HD_VPS_CLKC_VIP1DPS_VIP1_RGB_SRC_SELECT_MASK);

        if (NULL == configOvlyPtr)
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
        else
        {
            /* Set the mux value in the vertual register */
            *((UInt32 *)configOvlyPtr +
                regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
                tempReg;
        }
    }
    return (0);
}


Int VpsHal_vpsVipChrDsBypass(VpsHal_VpsVipInst vipInst,
                             VpsHal_VpsChrdsInst chrDsInst,
                             UInt32 bypass,
                             Ptr configOvlyPtr)
{
    UInt32 tempReg, module, mask=0;

    if(chrDsInst==VPSHAL_VPS_CHRDS_INST_0)
    {
        module = VPSHAL_VPS_VIP_DPS_CHR_DS_0_BYPASS;

        if(bypass)
        {
            mask = CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_0_BYPASS_MASK;
        }
    }
    else
    {
        module = VPSHAL_VPS_VIP_DPS_CHR_DS_1_BYPASS;

        if(bypass)
        {
            mask = CSL_HD_VPS_CLKC_VIP0DPS_VIP0_CHR_DS_1_BYPASS_MASK;
        }
    }

    tempReg = (( module <<
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
        mask;

    if (NULL == configOvlyPtr)
    {
        if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
    }
    else
    {
        /* Set the mux value in the vertual register */
        *((UInt32 *)configOvlyPtr +
            regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
            tempReg;
    }

    return 0;
}

Int VpsHal_vpsVipMultiChEnable(VpsHal_VpsVipInst vipInst,
                               UInt32 enable,
                               Ptr configOvlyPtr)
{
    UInt32 tempReg, module, mask=0;

    module = VPSHAL_VPS_VIP_DPS_MULTI_CHANNEL_SELECT;

    if(enable)
    {
        mask = CSL_HD_VPS_CLKC_VIP0DPS_VIP0_MULTI_CHANNEL_SELECT_MASK;
    }

    tempReg = ((module <<
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
        mask;

    if (NULL == configOvlyPtr)
    {
        if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
    }
    else
    {
        /* Set the mux value in the vertual register */
        *((UInt32 *)configOvlyPtr +
            regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
            tempReg;
    }

    return 0;
}

Int VpsHal_vpsVipRgbLowEnable(VpsHal_VpsVipInst vipInst,
                              UInt32 enable,
                              Ptr configOvlyPtr)
{
    UInt32 tempReg, module, mask=0;

    module = VPSHAL_VPS_VIP_DPS_RGB_OUT_LO_SELECT;

    if(enable)
    {
        mask = CSL_HD_VPS_CLKC_VIP0DPS_VIP0_RGB_OUT_LO_SELECT_MASK;
    }

    tempReg = ((module <<
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
        mask;

    if (NULL == configOvlyPtr)
    {
        if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
    }
    else
    {
        /* Set the mux value in the vertual register */
        *((UInt32 *)configOvlyPtr +
            regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
            tempReg;
    }

    return 0;
}

Int VpsHal_vpsVipRgbHighEnable(VpsHal_VpsVipInst vipInst,
                               UInt32 enable,
                               Ptr configOvlyPtr)
{
    UInt32 tempReg, module, mask=0;

    module = VPSHAL_VPS_VIP_DPS_RGB_OUT_HI_SELECT;

    if(enable)
    {
        mask = CSL_HD_VPS_CLKC_VIP0DPS_VIP0_RGB_OUT_HI_SELECT_MASK;
    }

    tempReg = ((module <<
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_SHIFT) &
      CSL_HD_VPS_CLKC_VIP0DPS_VIP0_DATAPATH_SELECT_MASK) |
        mask;

    if (NULL == configOvlyPtr)
    {
        if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
        {
            VpsRegOvly->CLKC_VIP0DPS = tempReg;
        }
        else
        {
            VpsRegOvly->CLKC_VIP1DPS = tempReg;
        }
    }
    else
    {
        /* Set the mux value in the vertual register */
        *((UInt32 *)configOvlyPtr +
            regOvly.vipMuxVirtRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC]) =
            tempReg;
    }

    return 0;
}

/**
 *  VpsHal_vpsVencOutputPixClkEnable
 *  \brief Function to enable/disable Venc
 *  output pixel clock
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param outPixClk  VENC output Pixel clock
 *  \param isEnabled  Flag to indicate whether to enable or to disable
 *                    clock
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVencOutputPixClkEnable(VpsHal_VpsVencOutPixClk outPixClk,
                                  UInt32 isEnabled)
{
    if (VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO1 == outPixClk)
    {
        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO1_CLK_ON_MASK);
        if (isEnabled)
        {
            /* Enable DVO1 output pixel clock */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO1_CLK_ON_MASK;
        }
    }
    else if (VPSHAL_VPS_VENC_OUT_PIX_CLK_HDMI == outPixClk)
    {
        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_HDMI_CLK_ON_MASK);
        if (isEnabled)
        {
            /* Enable DVO1 output pixel clock */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_HDMI_CLK_ON_MASK;
        }
    }
    else if (VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO2 == outPixClk )
    {
        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK_ON_MASK);
        if (isEnabled)
        {
            /* Enable DVO2 output pixel clock */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK_ON_MASK;
        }
    }
    else /* SDVENC output pixel clock */
    {
        /* There is not bit to enable output clock for SDVENC */
    }
    return (0);
}

/**
 *  VpsHal_vpsVencClkDivide
 *  \brief Function to enable/disable clock division by
 *  2 for some of the VENC input clock
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param clkSrc   VENC Clock Source
 *  \param isDivide Flag to indicate whether to divice clock
 *
 *  \return         Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVencClkDivide(VpsHal_VpsVencClkDiv clkSrc, UInt32 isDivide)
{
    UInt32 clkenRegValue;

    clkenRegValue = VpsRegOvly->CLKC_CLKEN;
    if (VPSHAL_VPS_VENC_CLK_DIV_VENC_D_CLK1X == clkSrc)
    {
        /* Disable module clock first */
        VpsRegOvly->CLKC_CLKEN &=
            ~CSL_HD_VPS_CLKC_CLKEN_HDMI_DVO1_EN_MASK;

        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_HDMI_CLK1X_SELECT_MASK);
        if (FALSE == isDivide)
        {
            /* Set the Bit */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_HDMI_CLK1X_SELECT_MASK;
        }
    }
    else if (VPSHAL_VPS_VENC_CLK_DIV_DVO1 == clkSrc)
    {
        /* Disable module clock first */
        VpsRegOvly->CLKC_CLKEN &=
            ~CSL_HD_VPS_CLKC_CLKEN_HDMI_DVO1_EN_MASK;

        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO1_CLK_SELECT_MASK);
        if (TRUE == isDivide)
        {
            /* Set the Bit */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO1_CLK_SELECT_MASK;
        }
    }
    else if (VPSHAL_VPS_VENC_CLK_DIV_VENC_A_CLK1X == clkSrc)
    {
        /* Disable module clock first */
        VpsRegOvly->CLKC_CLKEN &=
            ~CSL_HD_VPS_CLKC_CLKEN_HDCOMP_EN_MASK;

        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_HDCOMP_CLK1X_SELECT_MASK);
        if (FALSE == isDivide)
        {
            /* Set the Bit */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_HDCOMP_CLK1X_SELECT_MASK;
        }
    }
    else if (VPSHAL_VPS_VENC_CLK_DIV_VENC_A_VBI == clkSrc)
    {
        /* Disable module clock first */
        VpsRegOvly->CLKC_CLKEN &=
            ~CSL_HD_VPS_CLKC_CLKEN_HDCOMP_EN_MASK;

        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_VBI_HD_CLK_SELECT_MASK);
        if (TRUE == isDivide)
        {
            /* Set the Bit */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_VBI_HD_CLK_SELECT_MASK;
        }
    }
    else if (VPSHAL_VPS_VENC_CLK_DIV_DVO2 == clkSrc)
    {
        /* Disable module clock first */
        VpsRegOvly->CLKC_CLKEN &=
            ~CSL_HD_VPS_CLKC_CLKEN_DVO2_EN_MASK;

        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK_SELECT_MASK);
        if (TRUE == isDivide)
        {
            /* Set the Bit */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK_SELECT_MASK;
        }
    }
    else
    {
        /* Disable module clock first */
        VpsRegOvly->CLKC_CLKEN &=
            ~CSL_HD_VPS_CLKC_CLKEN_DVO2_EN_MASK;

        /* Clear the Bit */
        VpsRegOvly->CLKC_VENC_CLKSEL &=
            (~CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK1X_SELECT_MASK);
        if (FALSE == isDivide)
        {
            /* Set the Bit */
            VpsRegOvly->CLKC_VENC_CLKSEL |=
                CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK1X_SELECT_MASK;
        }
    }

    /* Restore Original Clock Enable Value */
    VpsRegOvly->CLKC_CLKEN = clkenRegValue;

    return (0);
}

/**
 * VpsHal_vpsVencGClkSrcSelect
 * \brief Function to select the clock source for
 * VENC G (DVO2). VENCG either uses clock from VENC D or from VENC A. It does
 * not have separate clock.
 * VpsHal_vpsInit function must be called prior to this.
 *
 * \param clkSrc VENC G clock Source either VENC D or VENC A
 *
 * \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVencGClkSrcSelect(VpsHal_VpsVencGClkSrc clkSrc)
{
    UInt32 clkEnRegValue;

    clkEnRegValue = VpsRegOvly->CLKC_CLKEN;

    /* Disable module clock first */
    VpsRegOvly->CLKC_CLKEN &=
        ~CSL_HD_VPS_CLKC_CLKEN_DVO2_EN_MASK;

    /* Clear the Bit */
    VpsRegOvly->CLKC_VENC_CLKSEL &=
        (~CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK2X_SELECT_MASK);
    if (VPSHAL_VPS_VENC_G_CLK_SRC_VENC_A_CLK == clkSrc)
    {
        /* Select Venc_A as clock source */
        VpsRegOvly->CLKC_VENC_CLKSEL |=
            CSL_HD_VPS_CLKC_VENC_CLKSEL_DVO2_CLK2X_SELECT_MASK;
    }

    /* Restore Original Clock Enable Value */
    VpsRegOvly->CLKC_CLKEN = clkEnRegValue;

    return (0);
}

/**
 * VpsHal_vpsVencAClkSrcSelect
 * \brief Function to select the clock source for
 * VENC G (DVO2). VENCG either uses clock from VENC D or from VENC A. It does
 * not have separate clock.
 * VpsHal_vpsInit function must be called prior to this.
 *
 * \param clkSrc VENC G clock Source either VENC D or VENC A
 *
 * \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVencAClkSrcSelect(VpsHal_VpsVencAClkSrc clkSrc)
{
    UInt32 clkEnRegValue;

    clkEnRegValue = VpsRegOvly->CLKC_CLKEN;

    /* Disable module clock first */
    VpsRegOvly->CLKC_CLKEN &=
        ~CSL_HD_VPS_CLKC_CLKEN_HDCOMP_EN_MASK;

    /* Clear the Bit */
    VpsRegOvly->CLKC_VENC_CLKSEL &=
        (~CSL_HD_VPS_CLKC_VENC_CLKSEL_HDCOMP_CLK2X_SELECT_MASK);
    if (VPSHAL_VPS_VENC_A_CLK_SRC_VENC_D_CLK == clkSrc)
    {
        /* Select Venc_A as clock source */
        VpsRegOvly->CLKC_VENC_CLKSEL |=
            CSL_HD_VPS_CLKC_VENC_CLKSEL_HDCOMP_CLK2X_SELECT_MASK;
    }

    /* Restore Original Clock Enable Value */
    VpsRegOvly->CLKC_CLKEN = clkEnRegValue;

    return (0);
}

/**
 *  VpsHal_vpsSetRangeReductionConfig
 *  \brief This function enables/disables range
 *  reduction on the specific display path.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param src           Indicates the display path for which range
 *                       reduction is to be enabled/disabled
 *  \param isEnabled     Flag to indicate whether to enable or to disable
 *                       feature
 *  \param configOvlyPtr NONE
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vpsSetRangeReductionConfig(VpsHal_VpsRangeConvertSrc src,
                                      UInt32 isEnabled,
                                      Ptr configOvlyPtr)
{
    UInt32 bitPos;
    UInt32 *ovlyPtr = NULL;

    /* Calculate Bit position */
    bitPos =
        src + CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_REDUCTION_PRI_ON_SHIFT;
    if (NULL == configOvlyPtr)
    {
        /* Clear the Bit */
        VpsRegOvly->CLKC_RANGE_MAP &= (~(0x1u << bitPos));
        if (TRUE == isEnabled)
        {
            /* Set the bit position */
            VpsRegOvly->CLKC_RANGE_MAP |= (0x1u << bitPos);
        }
    }
    else
    {
        ovlyPtr = (UInt32 *)configOvlyPtr +
                regOvly.vc1VirtRegOffset[VPSHAL_VPS_CONFIG_OVLY_VPS_CLKC_DPS];
        (*ovlyPtr) &= (~(0x1u << bitPos));
        if (TRUE == isEnabled)
        {
            /* Set the bit position */
            (*ovlyPtr) |= (0x1u << bitPos);
        }
    }
    return (0);
}

/**
 *  VpsHal_vpsSetRangeMapConfig
 *  \brief Function to set the configuration
 *  parameters for Range Mapping. It also enables/disables range mapping for
 *  the given display path
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param config        Pointer to structure containing parameters.
 *  \param configOvlyPtr NONE
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vpsSetRangeMapConfig(VpsHal_VpsRangeMapConfig *config,
                                Ptr configOvlyPtr)
{
    UInt32 bitPos;
    UInt32 *ovlyPtr;

    GT_assert( GT_DEFAULT_MASK, (NULL != config));

    /* Calculate Bit position */
    bitPos = (config->src * VPS_RANGE_MAP_OFFSET) +
                CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAP_PRI_ON_SHIFT;
    if (NULL == configOvlyPtr)
    {
        /* Clear the Bit */
        VpsRegOvly->CLKC_RANGE_MAP &= (~(0x1u << bitPos));

        if (TRUE == config->isEnabled)
        {
            /* Enable Range Mapping */
            VpsRegOvly->CLKC_RANGE_MAP |= (0x1u << bitPos);

            /* Set the Range Mapping Constant for Luma */
            bitPos = (config->src * VPS_RANGE_MAP_OFFSET) +
                        CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPY_PRI_SHIFT;
            /* Clear Bits */
            VpsRegOvly->CLKC_RANGE_MAP &=
               (~(CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPY_PRI_SHIFT << bitPos));
            VpsRegOvly->CLKC_RANGE_MAP |= (config->lumaValue << bitPos);

            /* Set the Range Mapping Constant for Chroma */
            bitPos = (config->src * VPS_RANGE_MAP_OFFSET) +
                        CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPUV_PRI_SHIFT;
            /* Clear Bits */
            VpsRegOvly->CLKC_RANGE_MAP &=
                (~(CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPY_PRI_MASK << bitPos));
            VpsRegOvly->CLKC_RANGE_MAP |= (config->chromaValue << bitPos);
        }
    }
    else
    {
        ovlyPtr = (UInt32 *)configOvlyPtr +
                regOvly.vc1VirtRegOffset[VPSHAL_VPS_CONFIG_OVLY_VPS_CLKC_DPS];
        /* Clear the Bit */
        (*ovlyPtr) &= (~(0x1u << bitPos));

        if (TRUE == config->isEnabled)
        {
            /* Enable Range Mapping */
            (*ovlyPtr) |= (0x1u << bitPos);

            /* Set the Range Mapping Constant for Luma */
            bitPos = (config->src * VPS_RANGE_MAP_OFFSET) +
                        CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPY_PRI_SHIFT;
            /* Clear Bits */
            (*ovlyPtr) &=
               (~(CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPY_PRI_SHIFT << bitPos));
            (*ovlyPtr) |= (config->lumaValue << bitPos);

            /* Set the Range Mapping Constant for Chroma */
            bitPos = (config->src * VPS_RANGE_MAP_OFFSET) +
                        CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPUV_PRI_SHIFT;
            /* Clear Bits */
            (*ovlyPtr) &=
                (~(CSL_HD_VPS_CLKC_RANGE_MAP_RANGE_MAPY_PRI_MASK << bitPos));
            (*ovlyPtr) |= (config->chromaValue << bitPos);
        }
    }
    return (0);
}

/**
 *  VpsHal_vpsVcompMainEnable
 *  \brief Function to enable/disable main video pipeline in Vcomp.
 *
 *  \param  confiOvlyPtr Pointer to the memory where configuration
 *                        overlay is to be created.
 *  \param  isMainEnabled NONE
 *
 *  \return               Register overlay size
 */
Int32 VpsHal_vpsVcompMainEnable(UInt32 isMainEnabled, Ptr configOvlyPtr)
{
    UInt32 tempReg = 0u;
    Ptr ovlyPtr = NULL;

    if (TRUE == isMainEnabled)
    {
        tempReg = ((VPSHAL_VPS_MAIN_DPS_VCOMP_MAIN_DISABLE <<
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK);
    }
    else
    {
        tempReg = ((VPSHAL_VPS_MAIN_DPS_VCOMP_MAIN_DISABLE <<
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_SHIFT) &
                CSL_HD_VPS_CLKC_DPS_MAIN_DATAPATH_SELECT_MASK) |
                (CSL_HD_VPS_CLKC_DPS_VCOMP_MAIN_DISABLE_MASK);
    }
    if (NULL == configOvlyPtr)
    {
        VpsRegOvly->CLKC_DPS = tempReg;
    }
    else
    {
        ovlyPtr = (UInt32 *)configOvlyPtr +
            regOvly.muxVirtRegOffset[VPSHAL_VPS_CONFIG_OVLY_VPS_CLKC_DPS];
        (*(UInt32 *)ovlyPtr) = tempReg;
    }

    return (0);
}

/**
 *  Void VpsHal_vpsVipGetConfigOvlySize
 *  \brief Function to get the VPDMA register overlay size required to
 *  store VIP multiplexer register. This single function is used to get
 *  the configOvly size of all VIP muxes since single register is used
 *  to configure muxes.
 *
 *  \param vipInst          Vip Instace number
 *
 *  \return                 Register Overlay size
 */
UInt32 VpsHal_vpsVipGetConfigOvlySize(VpsHal_VpsVipInst vipInst)
{
    return (regOvly.vipMuxConfigOvlySize);
}

/**
 *  Void VpsHal_vpsVipCreateConfigOvly
 *  \brief This function is used to create the complete VPDMA register overlay
 *  for VIP multiplexers. It just create the overlay but does not
 *  initialize the virtual registers with the configuration.
 *  Configuration can be done by calling appropriate vip mux setting function.
 *
 *  \param handle           Scalar Handle
 *  \param configOvlyPtr    Pointer to memory where VPDMA overlay will be
 *                          stored
 *
 *  \return                 0 on success, -1 on error
 */
Int32 VpsHal_vpsVipCreateConfigOvly(VpsHal_VpsVipInst vipInst,
                                    Ptr configOvlyPtr)
{
    Int32 ret;

    if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
    {
        regOvly.vipMuxRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC] =
                                        (UInt32 *) &(VpsRegOvly->CLKC_VIP0DPS);
    }
    else
    {
        regOvly.vipMuxRegOffset[VPSHAL_VPS_VIP_CONFIG_OVLY_VIP_CLKC] =
                                        (UInt32 *) &(VpsRegOvly->CLKC_VIP1DPS);
    }
    /* Create the register overlay */
    ret = VpsHal_vpdmaCreateRegOverlay(
              regOvly.vipMuxRegOffset,
              VPSHAL_VPS_VIP_CONFIG_OVLY_NUM_REG,
              configOvlyPtr);
    return (ret);
}


UInt32 VpsHal_vpsGetVipMuxRegOffset(VpsHal_VpsVipInst vipInst)
{
    volatile UInt32 value32=0;

    if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
        value32 = (UInt32)&VpsRegOvly->CLKC_VIP0DPS;

    if (VPSHAL_VPS_VIP_INST_VIP1 == vipInst)
        value32 = (UInt32)&VpsRegOvly->CLKC_VIP1DPS;

    return (value32 & 0xFFFF);
}

UInt32 VpsHal_vpsGetVipMuxValue(VpsHal_VpsVipInst vipInst)
{
    volatile UInt32 value32=0;

    if (VPSHAL_VPS_VIP_INST_VIP0 == vipInst)
        value32 = VpsRegOvly->CLKC_VIP0DPS;

    if (VPSHAL_VPS_VIP_INST_VIP1 == vipInst)
        value32 = VpsRegOvly->CLKC_VIP1DPS;

    return value32 & 0x000FFFFF;
}

UInt32 VpsHal_vpsClkcModuleGetUnderFlowStatus(UInt32 venc)
{
    return (VpsRegOvly->CLKC_VENC_UNDERFLOW_STATUS & venc);
}

Void VpsHal_vpsClkcModuleClearUnderFlow(UInt32 venc)
{
    VpsRegOvly->CLKC_VENC_UNDERFLOW_STATUS |=
    (venc & (CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_HDMI_ENABLE_MASK |
            CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_HDCOMP_ENABLE_MASK |
            CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_DVO2_ENABLE_MASK |
            CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_SD_ENABLE_MASK));
}

Int32 VpsHal_vpsClkcModuleCountUnderFlow(Bool clearAll, UInt32 *vencUnderflowCounter)
{
    volatile UInt32 vencUnderflow, vencMask;

    vencMask =  CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_HDMI_ENABLE_MASK |
                CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_HDCOMP_ENABLE_MASK |
                CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_DVO2_ENABLE_MASK |
                CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_SD_ENABLE_MASK
                ;

    if(clearAll)
    {
        VpsHal_vpsClkcModuleClearUnderFlow(vencMask);
    }

    /* get underflow status for all vencs */
    vencUnderflow = VpsHal_vpsClkcModuleGetUnderFlowStatus(vencMask);

    if(CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_HDMI_ENABLE_MASK & vencUnderflow)
        vencUnderflowCounter[0]++;
    if(CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_HDCOMP_ENABLE_MASK & vencUnderflow)
        vencUnderflowCounter[1]++;
    if(CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_DVO2_ENABLE_MASK & vencUnderflow)
        vencUnderflowCounter[2]++;
    if(CSL_HD_VPS_CLKC_VENC_UNDERFLOW_STATUS_SD_ENABLE_MASK & vencUnderflow)
        vencUnderflowCounter[3]++;

    VpsHal_vpsClkcModuleClearUnderFlow(vencUnderflow);

    return 0;
}

Int VpsHal_vpsVipCscReset(VpsHal_VpsVipInst vipInst, UInt32 enable)
{
    Int32 ret = FVID2_SOK;
    volatile UInt32 tempReg;

    if (vipInst == VPSHAL_VPS_VIP_INST_VIP0)
    {
        tempReg = CSL_HD_VPS_CLKC_RST_VIP0_CSC_DP_RST_MASK;
    }
    else if (vipInst == VPSHAL_VPS_VIP_INST_VIP1)
    {
        tempReg = CSL_HD_VPS_CLKC_RST_VIP1_CSC_DP_RST_MASK;
    }
    else
    {
        ret = FVID2_EFAIL;
    }

    if (ret == FVID2_SOK)
    {
        if (enable == TRUE)
        {
            VpsRegOvly->CLKC_RST |= tempReg;
        }
        else
        {
            VpsRegOvly->CLKC_RST &= ~tempReg;
        }
    }
    return (ret);
}

Int VpsHal_vpsVipScReset(VpsHal_VpsVipInst vipInst, UInt32 enable)
{
    Int32 ret = FVID2_SOK;
    volatile UInt32 tempReg;

    if (vipInst == VPSHAL_VPS_VIP_INST_VIP0)
    {
        tempReg = CSL_HD_VPS_CLKC_RST_VIP0_SC_DP_RST_MASK;
    }
    else if (vipInst == VPSHAL_VPS_VIP_INST_VIP1)
    {
        tempReg = CSL_HD_VPS_CLKC_RST_VIP1_SC_DP_RST_MASK;
    }
    else
    {
        ret = FVID2_EFAIL;
    }

    if (ret == FVID2_SOK)
    {
        if (enable == TRUE)
        {
            VpsRegOvly->CLKC_RST |= tempReg;
        }
        else
        {
            VpsRegOvly->CLKC_RST &= ~tempReg;
        }
    }
    return (ret);
}

Int VpsHal_vpsVipChrdsReset(VpsHal_VpsVipInst vipInst,
                            VpsHal_VpsChrdsInst chrdsInst, UInt32 enable)
{
    Int32 ret = FVID2_SOK;
    volatile UInt32 tempReg;

    if (vipInst == VPSHAL_VPS_VIP_INST_VIP0)
    {
        if (chrdsInst == VPSHAL_VPS_CHRDS_INST_0)
        {
            tempReg = CSL_HD_VPS_CLKC_RST_VIP0_CHRDS_0_DP_RST_MASK;
        }
        else
        {
            tempReg = CSL_HD_VPS_CLKC_RST_VIP0_CHRDS_1_DP_RST_MASK;
        }
    }
    else if (vipInst == VPSHAL_VPS_VIP_INST_VIP1)
    {
        if (chrdsInst == VPSHAL_VPS_CHRDS_INST_0)
        {
            tempReg = CSL_HD_VPS_CLKC_RST_VIP1_CHRDS_0_DP_RST_MASK;
        }
        else
        {
            tempReg = CSL_HD_VPS_CLKC_RST_VIP1_CHRDS_1_DP_RST_MASK;
        }
    }
    else
    {
        ret = FVID2_EFAIL;
    }

    if (ret == FVID2_SOK)
    {
        if (enable == TRUE)
        {
            VpsRegOvly->CLKC_RST |= tempReg;
        }
        else
        {
            VpsRegOvly->CLKC_RST &= ~tempReg;
        }
    }
    return (ret);
}

