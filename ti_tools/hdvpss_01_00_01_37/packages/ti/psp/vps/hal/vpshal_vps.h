/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
 *  \file vpshal_vps.h
 *
 *  \brief VPS HAL header file
 *  This file exposes the HAL APIs of the VPS INTC, Clock, multiplexers and
 *  other global modules.
 *
 */

#ifndef _VPSHAL_VPS_H
#define _VPSHAL_VPS_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  enum VpsHal_VpsStandByMode
 *  \brief Enum for Configuration of the local initiator state management mode.
 */
typedef enum
{
    HalVps_STBM_FORCE = 0,
    /**< Force-standby mode: local initiator is unconditionally placed in
         standby state. Backup mode, for debug only */
    VPSHAL_VPS_STBM_NO,
    /**< No-standby mode: local initiator is unconditionally placed out of
         standby state. Backup mode, for debug only */
    VPSHAL_VPS_STBM_SMART,
    /**< Smart-standby mode: local initiator standby status depends on local
         conditions, i.e. the module's functional requirement from the
         initiator. IP module shall not generate (initiator-related) wakeup
         events */
    VPSHAL_VPS_STBM_SMART_WAKEUP
    /**< Smart-Standby wakeup-capable mode: local initiator standby status
         depends on local conditions, i.e. the module's functional requirement
         from the initiator. IP module may generate (master-related) wakeup
         events when in standby state. Mode is only relevant if the appropriate
         IP module "mwakeup" output is implemented */
} VpsHal_VpsStandByMode;

/**
 *  enum VpsHal_VpsIdleMode
 *  \brief Enum for Configuration of the local target state management mode.
 */
typedef enum
{
    VPSHAL_VPS_IDLEM_FORCE = 0,
    /**< Force-idle mode: local target's idle state follows (acknowledges) the
         system's idle requests unconditionally, i.e. regardless of the IP
         module's internal requirements. Backup mode, for debug only */
    VPSHAL_VPS_IDLEM_NO,
    /**< No-idle mode: local target never enters idle state. Backup mode, for
         debug only */
    VPSHAL_VPS_IDLEM_SMART,
    /**< Smart-idle mode: local target's idle state eventually follows
         (acknowledges) the system's idle requests, depending on the IP module's
         internal requirements. IP module shall not generate (IRQ- or
         DMA-request-related) wakeup events */
    HALVPS_IDLEM_SMART_WAKEUP
    /**< Smart-idle wakeup-capable mode: local target's idle state eventually
         follows (acknowledges)the system's idle requests, depending on the IP
         module's internal requirements. IP module may generate (IRQ- or
         DMA-request-related) wakeup events when in idle state. Mode is only
         relevant if the appropriate IP module "swakeup" output(s) is (are)
         implemented */
} VpsHal_VpsIdleMode;

/**
 *  \brief enum VpsHal_VpsClkcModule
 *  Enum for selecting Clock Modules.
 */
typedef enum
{
    VPSHAL_VPS_CLKC_VPDMA = 0,
    /**< VPDMA */
    VPSHAL_VPS_CLKC_PRI = 1,
    /**< Primary Video Data Path */
    VPSHAL_VPS_CLKC_AUX = 2,
    /**< Auxiliary Video Data Path */
    VPSHAL_VPS_CLKC_GRPX0 = 3,
    /**< Graphics 0 Data Path */
    VPSHAL_VPS_CLKC_GRPX1 = 4,
    /**< Graphics 1 Data Path */
    VPSHAL_VPS_CLKC_GRPX2 = 5,
    /**< Graphics 2 Data Path */
    VPSHAL_VPS_CLKC_COMP = 6,
    /**< Compositor Data Path. This also enables all back end video encoders */
    VPSHAL_VPS_CLKC_SEC0 = 7,
    /**< Secondary 0 (to VIP0) Data Path */
    VPSHAL_VPS_CLKC_SEC1 = 8,
    /**< Secondary 1 (to VIP1) Data Path */
    VPSHAL_VPS_CLKC_HDMI_DVO1 = 9,
    /**< HDMI/DVO1 VENC */
#if defined(TI_816X_BUILD) || defined(TI_8107_BUILD)
    VPSHAL_VPS_CLKC_HDCOMP = 10,
#endif
    /**< HD COMP VENC */
    VPSHAL_VPS_CLKC_DVO2 = 11,
    /**< DVO2 VENC */
    VPSHAL_VPS_CLKC_SDVENC = 12,
    /**< SD VENC */
    VPSHAL_VPS_CLKC_VIP0 = 16,
    /**< VIP0 Data Path containing all modules of VIP */
    VPSHAL_VPS_CLKC_VIP1 = 17,
    /**< VIP1 Data Path containing all modules of VIP */
    VPSHAL_VPS_CLKC_VIP0_VIP = 18,
    /**< VIP parser of VIP0 Data Path */
    VPSHAL_VPS_CLKC_VIP1_VIP = 19,
    /**< VIP parser of VIP1 Data Path */
    VPSHAL_VPS_CLKC_VIP0_CSC = 20,
    /**< CSC of VIP0 Data Path */
    VPSHAL_VPS_CLKC_VIP1_CSC = 21,
    /**< CSC of VIP1 Data Path */
    VPSHAL_VPS_CLKC_VIP0_SC = 22,
    /**< SC of VIP0 Data Path */
    VPSHAL_VPS_CLKC_VIP1_SC = 23,
    /**< SC of VIP1 Data Path */
    VPSHAL_VPS_CLKC_NF = 24,
    /**< Noise Filter Data Path */
    VPSHAL_VPS_CLKC_VIP0_CHR_DS0 = 25,
    /**< CHR_DS0 of VIP0 Data Path */
    VPSHAL_VPS_CLKC_VIP1_CHR_DS0 = 26,
    /**< CHR_DS0 of VIP1 Data Path */
    VPSHAL_VPS_CLKC_VIP0_CHR_DS1 = 27,
    /**< CHR_DS1 of VIP0 Data Path */
    VPSHAL_VPS_CLKC_VIP1_CHR_DS1 = 28,
    /**< CHR_DS1 of VIP1 Data Path */
    VPSHAL_VPS_CLKC_MAIN = 31,
    /**< All modules in VPS Main Data Path. Used only for resetting not for
         enabling/disabling */
    VPSHAL_VPS_CLKC_HDMI_VENC = 33,
    /**< HDMI/Digital Video Output 1 VENC Enable */
#if defined(TI_816X_BUILD) || defined(TI_8107_BUILD)
    VPSHAL_VPS_CLKC_HDCOMP_VENC = 34,
    /**< HD Component VENC Enable */
#endif
    VPSHAL_VPS_CLKC_DVO2_VENC = 35,
    /**< Digital Video Output 2 VENC Enable */
    VPSHAL_VPS_CLKC_SD_VENC = 36
    /**< SD VENC Enable */
} VpsHal_VpsClkcModule;

/**
 *  enum VpsHal_VpsBypassSelect
 *  \brief Few modules can be selected/bypassed through VPS. Enum is used to
 *  select/bypass module
 */
typedef enum
{
    VPSHAL_VPS_SELECT_MODULE = 0,
    /**< Module is selected */
    VPSHAL_VPS_BYPASS_MODULE
    /**< Module is bypassed */
} VpsHal_VpsBypassSelect;

/**
 *  enum VpsHal_VpsModule
 *  \brief Few modules can be selected/bypassed through VPS. Enum for selecting
 *  those modules
 */
typedef enum
{
    VPSHAL_VPS_MODULE_COMPR_AUX = 0,
    /**< Auxiliary DEI Private Data Store (PDS) Data Compress/Decompress Bypass.
         This applies to both the output and inbound PDS data paths */
    VPSHAL_VPS_MODULE_COMPR_PRI,
    /**< Primary DEI TNR Data Compress/Decompress Bypass. This applies to both
         the output and inbound TNR data paths */
    VPSHAL_VPS_MODULE_NF,
    /**< Noise Filter Bypass. If Noise Filter is bypassed, 422 Private Data
         Store data is chroma downsampled and written back out as 420 Tiled
         Data.*/
    VPSHAL_VPS_MODULE_VIP0_CHR_DS0,
    /**< Video Input Port 0 Chroma Downsampler 0 Bypass */
    VPSHAL_VPS_MODULE_VIP0_CHR_DS1,
    /**< Video Input Port 0 Chroma Downsampler 1 Bypass */
    VPSHAL_VPS_MODULE_VIP1_CHR_DS0,
    /**< Video Input Port 1 Chroma Downsampler 0 Bypass */
    VPSHAL_VPS_MODULE_VIP1_CHR_DS1
    /**< Video Input Port 1 Chroma Downsampler 1 Bypass */
} VpsHal_VpsModule;

/**
 *  enum VpsHal_VpsWb2MuxSrc
 *  \brief Enum for selecting input stream for Write back scalar multiplexer
 */
typedef enum
{
    VPSHAL_VPS_WB2_MUX_SRC_DISABLE = 0,
    /**< Path Disabled */
    VPSHAL_VPS_WB2_MUX_SRC_HDMI,
    /**< HDMI Composited Path Input (from COMP) */
    VPSHAL_VPS_WB2_MUX_SRC_EDE,
    /**< EDE Path Input (from EDE) */
    VPSHAL_VPS_WB2_MUX_SRC_VCOMP,
    /**< VCOMP Path Input (from VCOMP) */
    VPSHAL_VPS_WB2_MUX_SRC_BP0,
    /**< BP0 Memory Input (from VPDMA) */
    VPSHAL_VPS_WB2_MUX_SRC_BP1,
    /**< BP1 Memory Input (from VPDMA) */
    VPSHAL_VPS_WB2_MUX_SRC_SEC1
    /**< Secondary 1 Input */
} VpsHal_VpsWb2MuxSrc;

/**
 *  enum VpsHal_VpsVencMuxSrc
 *  \brief Enum for selecting input stream for Multiplexers, which provides
 *  streams to VENCs or to VCOMP
 */
typedef enum
{
    VPSHAL_VPS_VENC_MUX_SRC_DISABLED = 0,
    /**< Path Disabled */
    VPSHAL_VPS_VENC_MUX_SRC_AUX,
    /**< Auxiliary Path Input */
    VPSHAL_VPS_VENC_MUX_SRC_BP0,
    /**< BP0 Memory Input (from VPDMA) */
    VPSHAL_VPS_VENC_MUX_SRC_BP1,
    /**< BP1 Memory Input (from VPDMA) */
    VPSHAL_VPS_VENC_MUX_SRC_SEC1
    /**< Secondary 1 Input. Used only for SDVENC mux */
} VpsHal_VpsVencMuxSrc;

/**
 *  enum VpsHal_VpsVencMux
 *  \brief Enum for selecting Multiplexers connected to Venc or to Vcomp
 */
typedef enum
{
    VPSHAL_VPS_VENC_MUX_SD = 0,
    /**< SD VENC Path Select */
    VPSHAL_VPS_VENC_MUX_HDCOMP,
    /**< HDCOMP Path Input Select */
    VPSHAL_VPS_VENC_MUX_VCOMP,
    /**< VCOMP PIP Input Select */
    VPSHAL_VPS_VENC_MUX_PRI
    /**< Multiplexer on the VCOMP main input */
} VpsHal_VpsVencMux;

/**
 *  enum VpsHal_VpsSecInst
 *  \brief Enum for selecting secondary path
 */
typedef enum
{
    VPSHAL_VPS_SEC_INST_0 = 0,
    /**< Secondary path 0 */
    VPSHAL_VPS_SEC_INST_1
    /**< Secondary path 1 */
} VpsHal_VpsSecInst;

/**
 *  enum VpsHal_VpsSecMuxSrc
 *  \brief Enum for selecting secondary Path
 */
typedef enum
{
    VPSHAL_VPS_SEC_MUX_SRC_PRI_AUX = 0,
    /**< Primary/Auxiliary output path is selected for VIP SC output */
    VPSHAL_VPS_SEC_MUX_SRC_SEC
    /**< Secondary path 0/1 is selected for VIP SC output*/
} VpsHal_VpsSecMuxSrc;

/**
 *  enum VpsHal_VpsVipInst
 *  \brief Enum for selecting VIP instance for the configuration of the VIP
 *  multiplexers
 */
typedef enum
{
    VPSHAL_VPS_VIP_INST_VIP0 = 0,
    /**< VIP 0 */
    VPSHAL_VPS_VIP_INST_VIP1,
    /**< VIP 1 */
    VPSHAL_VPS_VIP_INST_MAX
} VpsHal_VpsVipInst;

/**
 *  enum VpsHal_VpsChrdsInst
 *  \brief Enum for selecting Chroma Down Sampler instance within a VIP
 */
typedef enum
{
    VPSHAL_VPS_CHRDS_INST_0 = 0,
    /**< Chroma Down Sampler connected to Lower VPDMA clients */
    VPSHAL_VPS_CHRDS_INST_1
    /**< Chroma Down Sampler connected to Upper VPDMA clients */
} VpsHal_VpsChrdsInst;

/**
 *  enum VpsHal_VpsVipCscMuxSrc
 *  \brief Enum for selecting source stream for CSC in VIP
 */
typedef enum
{
    VPSHAL_VPS_VIP_CSC_MUX_SRC_DISABLED = 0,
    /**< Path Disabled */
    VPSHAL_VPS_VIP_CSC_MUX_SRC_VIP_PORTA_422,
    /**< Source from VIP_PARSER A (422) port */
    VPSHAL_VPS_VIP_CSC_MUX_SRC_VIP_PORTB_422,
    /**< Source from VIP_PARSER B port */
    VPSHAL_VPS_VIP_CSC_MUX_SRC_SEC,
    /**< Source from Secondary (422) */
    VPSHAL_VPS_VIP_CSC_MUX_SRC_VIP_PORTA_RGB,
    /**< Source from VIP_PARSER A (RGB) port */
    VPSHAL_VPS_VIP_CSC_MUX_SRC_COMP
    /**< Source from Compositor (RGB) */
} VpsHal_VpsVipCscMuxSrc;

/**
 *  enum VpsHal_VpsVipScMuxSrc
 *  \brief Enum for selecting source stream for scalar in VIP
 */
typedef enum
{
    VPSHAL_VPS_VIP_SC_MUX_SRC_DISABLED = 0,
    /**< Path Disabled */
    VPSHAL_VPS_VIP_SC_MUX_SRC_CSC,
    /**< Source from Color Space Converter (CSC) */
    VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTA,
    /**< Source from VIP_PARSER A port */
    VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTB,
    /**< Source from VIP_PARSER B port */
    VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_SEC
    /**< Source from Secondary (422) */
} VpsHal_VpsVipScMuxSrc;

/**
 *  enum VpsHal_VpsVipChrdsMuxSrc
 *  \brief Enum for selecting source stream for the Chroma Down Sampler. Since
 *  all chroma down sampler in VIP have same number and kind of stream, this
 *  enum is used for all down sampler
 */
typedef enum
{
    VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED = 0,
    /**< Path Disabled */
    VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SC,
    /**< Source from Scaler (SC_M) */
    VPSHAL_VPS_VIP_CHRDS_MUX_SRC_CSC,
    /**< Source from Color Space Converter (CSC) */
    VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTA,
    /**< Source from VIP_PARSER A port */
    VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTB,
    /**< Source from VIP_PARSER B port */
    VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SEC
    /**< Source from Secondary (422) */
} VpsHal_VpsVipChrdsMuxSrc;

/**
 *  enum VpsHal_VpsVipRgbMuxSrc
 *  \brief Enum for selecting RGB stream for VIP multiplexer, which is
 *  controlled by rgb_src_select bit.
 */
typedef enum
{
    VPSHAL_VPS_VIP_RGB_MUX_SRC_COMP = 0,
    /**< Source from Compositor RGB input */
    VPSHAL_VPS_VIP_RGB_MUX_SRC_CSC
    /**< Source from CSC */
} VpsHal_VpsVipRgbMuxSrc;

/**
 *  enum VpsHal_VpsVencOutPixClk
 *  \brief Enum for Enabling/Disabling output pixel clock for the given VENC.
 */
typedef enum
{
    VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO1 = 0,
    /**< Digital Video Output 1 output clock on */
    VPSHAL_VPS_VENC_OUT_PIX_CLK_HDMI,
    /**< HDMI output clock on */
    VPSHAL_VPS_VENC_OUT_PIX_CLK_DVO2,
    /**< Digital Video Output 2 output clock on */
    VPSHAL_VPS_VENC_OUT_PIX_CLK_INVALID
    /**< Does not do anything, Kept it just to be consistant */
} VpsHal_VpsVencOutPixClk;

/**
 *  enum VpsHal_VpsVencClkDiv
 *  \brief Enum for selecting input/output clock source for the specific VENC.
 *  This enum is used to decide whether clock is divided by 2 or not.
 */
typedef enum
{
    VPSHAL_VPS_VENC_CLK_DIV_VENC_D_CLK1X = 0,
    /**< Selects the HD_VENC_D_DVO1 clk1x source clock */
    VPSHAL_VPS_VENC_CLK_DIV_DVO1,
    /**< Digital Video Output 1 output clock */
    VPSHAL_VPS_VENC_CLK_DIV_VENC_A_CLK1X,
    /**< HD_VENC_A clk1x source clock */
    VPSHAL_VPS_VENC_CLK_DIV_VENC_A_VBI,
    /**< VBI HD Clock Select */
    VPSHAL_VPS_VENC_CLK_DIV_DVO2,
    /**< Digital Video Output 2 Clock 2x Select */
    VPSHAL_VPS_VENC_CLK_DIV_VENC_G_CLK1X
    /**< Digital Video Output 2 output clock */
} VpsHal_VpsVencClkDiv;

/**
 *  enum VpsHal_VpsVencGClkSrc
 *  \brief Enum for selecting the source of the VENC G (DVO2) pixclock.
 */
typedef enum
{
    VPSHAL_VPS_VENC_G_CLK_SRC_VENC_D_CLK = 0,
    /**< Selects clock of Venc D for the Venc G i.e. DVO2 encoder */
    VPSHAL_VPS_VENC_G_CLK_SRC_VENC_A_CLK
    /**< Selects clock of Venc A for the Venc G i.e. DVO2 encoder */
} VpsHal_VpsVencGClkSrc;

/**
 *  enum VpsHal_VpsVencAClkSrc
 *  \brief Enum for selecting the source of the VENC A pixclock.
 */
typedef enum
{
    VPSHAL_VPS_VENC_A_CLK_SRC_VENC_D_CLK = 0,
    /**< Selects clock of Venc D for the Venc A */
    VPSHAL_VPS_VENC_A_CLK_SRC_VENC_A_CLK
    /**< Selects clock of Venc A for the Venc A */
} VpsHal_VpsVencAClkSrc;

/* Range Mapping and Range Reduction are mutually exclusive */
/**
 *  enum VpsHal_VpsRangeConvertSrc
 *  \brief Range Mapping and Range Reduction is supported only the 4 display
 *  paths i.e. Primary Path, Auxiliary Path, both secondary Paths.
 *  This enum is used to select display path to enable range conversion on that
 *  path.
 */
typedef enum
{
    VPSHAL_VPS_RANGE_CONVERT_SRC_PRI = 0,
    /**< Primary Input Path */
    VPSHAL_VPS_RANGE_CONVERT_SRC_AUX,
    /**< Auxiliary Input Path */
    VPSHAL_VPS_RANGE_CONVERT_SRC_SEC0,
    /**< Secondary Path 0 */
    VPSHAL_VPS_RANGE_CONVERT_SRC_SEC1
    /**< Secondary Path 1 */
} VpsHal_VpsRangeConvertSrc;

/**
 *  enum VpsHal_VpsRangeMapValue
 *  \brief Range mapping uses following equation to map value from one range to another
 *  Y[n] = CLIP ((((Y[n] – 128)* (RANGE_MAP + 9) + 4) >> 3) + 128);
 *  Value of the RANGE_MAP variable is defined by the enum.
 */
typedef enum
{
    VPSHAL_VPS_RANGE_MAP_VALUE_0 = 0,
    /**< Range Mapping Value 0 */
    VPSHAL_VPS_RANGE_MAP_VALUE_1,
    /**< Range Mapping Value 1 */
    VPSHAL_VPS_RANGE_MAP_VALUE_2,
    /**< Range Mapping Value 2 */
    VPSHAL_VPS_RANGE_MAP_VALUE_3,
    /**< Range Mapping Value 3 */
    VPSHAL_VPS_RANGE_MAP_VALUE_4,
    /**< Range Mapping Value 4 */
    VPSHAL_VPS_RANGE_MAP_VALUE_5,
    /**< Range Mapping Value 5 */
    VPSHAL_VPS_RANGE_MAP_VALUE_6,
    /**< Range Mapping Value 6 */
    VPSHAL_VPS_RANGE_MAP_VALUE_7
    /**< Range Mapping Value 7 */
} VpsHal_VpsRangeMapValue;

/**
 *  \brief enum VpsHal_VpsIpGenericIrq
 *  Enum to clear irq in clearIRQ register
 */
typedef enum
{
    VPSHAL_VPS_IP_GENERIC_IRQ_0 = 0,
    /**< intr0 IP Generic */
    VPSHAL_VPS_IP_GENERIC_IRQ_1,
    /**< intr1 IP Generic */
    VPSHAL_VPS_IP_GENERIC_IRQ_2,
    /**< intr2 IP Generic */
    VPSHAL_VPS_IP_GENERIC_IRQ_3
    /**< intr3 IP Generic */
} VpsHal_VpsIpGenericIrq;

/**
 *  enum VpsHal_OvlyType
 *  \brief Enum to define type of overlays
 */
typedef enum
{
    VPSHAL_OVLY_TYPE_MUX = 0,
    /**< Overlay for Mux rgisters */
    VPSHAL_OVLY_TYPE_VC1
    /**< Overlay for VC1 range mapping and range reduction rgisters */
} VpsHal_OvlyType;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_VpsInstParams
 *  \brief Structure containing instance specific parameters used at the init.
 */
typedef struct
{
    UInt32             baseAddress;
    /**< Base address of the instance
         Physical register start address for this instance */
} VpsHal_VpsInstParams;

/**
 *  struct VpsHal_VpsRangeMapConfig
 *  \brief Structure containing paramters for Range Mapping Configuration
 */
typedef struct
{
    VpsHal_VpsRangeConvertSrc   src;
    /**< Selects the display path on which Range Mapping parameters should
         be configured */
    UInt32                      isEnabled;
    /**< Enable/Disable Range Mapping on given display path */
    VpsHal_VpsRangeMapValue     lumaValue;
    /**< Range Mapping Constant Value for Luma */
    VpsHal_VpsRangeMapValue     chromaValue;
    /**< Range Mapping Constant Value for Chroma */
} VpsHal_VpsRangeMapConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/*
 *  Note: All Vps Hal functions are not re-entrant so it is the
 *  responsibility of the upper layer to protect/serialize function calls.
 */

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
Int VpsHal_vpsInit(VpsHal_VpsInstParams *initParams, Ptr arg);

/**
 *  VpsHal_vpsDeInit
 *  \brief Function to de-Initialize VPS HAL. Currently It
 *  does not do anything.
 *
 *  \param arg    Currently unused. For the future reference
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsDeInit(Ptr arg);

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
Int VpsHal_vpsClkcModuleEnable(VpsHal_VpsClkcModule module, UInt32 isEnabled);

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
Int VpsHal_vpsClkcVencEnable(UInt32 vencs, UInt32 isEnable);

/**
 *  \brief Function to reset specific module in CLKC.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param module Module for which clock is to be reset.
 *  \param assertReset TRUE: reset module, FALSE: normal operation
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsClkcModuleReset(VpsHal_VpsClkcModule module, Bool assertReset);

/**
 *  \brief Function to reset set of modules in CLKC.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param module Module for which clock is to be reset.
 *  \param assertReset TRUE: reset module, FALSE: normal operation
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsClkcResetModules(VpsHal_VpsClkcModule *module,
                               UInt32 num,
                               Bool assertReset);

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
                           VpsHal_VpsBypassSelect bypass);

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
Int VpsHal_vpsSetIntcEoi(VpsHal_VpsIpGenericIrq ipgenericirq);

/**
 *  VpsHal_vpsSecMuxSelect
 *  \brief Function to select source of the
 *  multiplexer on the secondary path. It takes multiplexer instance and
 *  selects the source of the multiplexer
 *
 *  Clock must be enabled for the secondaryp ath and VpsHal_vpsInit
 *  function must be called prior to this.
 *
 *  \param secInst       Secondary path
 *  \param muxSrc        Source of the multiplexer
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vpsSecMuxSelect(VpsHal_VpsSecInst secInst,
                           VpsHal_VpsSecMuxSrc muxSrc);

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
Int VpsHal_vpsWb2MuxSrcSelect(VpsHal_VpsWb2MuxSrc muxSrc);

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
                               Ptr configOvlyPtr);

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
                                   Ptr configOvlyPtr);

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
                                VpsHal_VpsVipScMuxSrc scSrc,
                                Ptr configOvlyPtr);

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
                                 VpsHal_VpsVipCscMuxSrc cscSrc,
                                 Ptr configOvlyPtr);

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
                                 Ptr configOvlyPtr);





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
                                     UInt32 isEnabled);

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
Int VpsHal_vpsVencClkDivide(VpsHal_VpsVencClkDiv clkSrc, UInt32 isDivide);

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
Int VpsHal_vpsVencGClkSrcSelect(VpsHal_VpsVencGClkSrc clkSrc);

/**
 * VpsHal_vpsVencAClkSrcSelect
 * \brief Function to select the clock source for
 * VENC A. VENCA either uses clock from venc a or from VENC d.
 * VpsHal_vpsInit function must be called prior to this.
 *
 * \param clkSrc VENC A clock Source either VENC D or VENC A
 *
 * \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVencAClkSrcSelect(VpsHal_VpsVencAClkSrc clkSrc);

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
                                      Ptr ovlyPtr);

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
                                Ptr configOvlyPtr);

/**
 *  VpsHal_vpsGetOvlySize
 *  \brief Function to get the register overlay size for the mux register
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param  ovlyType NONE.
 *
 *  \return          Register overlay size
 */
UInt32 VpsHal_vpsGetOvlySize(VpsHal_OvlyType ovlyType);

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
Int32 VpsHal_vpsCreateConfigOvly(VpsHal_OvlyType ovlyType, Ptr configOvlyPtr);

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
Int32 VpsHal_vpsVcompMainEnable(UInt32 isMainEnabled, Ptr confiOvlyPtr);

/**
 *  \brief Reset and enable all module CLKs in VPS
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsClkcModuleEnableAll();


/**
 * \brief Enable Multi channel mode
 *
 * \param vipInst [IN] VIP Instance ID
 * \param enable  [IN] TRUE: enable, FALSE: disable
 *
 * \return 0 on success else error
*/
Int VpsHal_vpsVipMultiChEnable(VpsHal_VpsVipInst vipInst,
                               UInt32 enable,
                               Ptr configOvlyPtr);

/**
 * \brief Enable RGB high port
 *
 * \param vipInst [IN] VIP Instance ID
 * \param enable  [IN] TRUE: enable, FALSE: disable
 *
 * \return 0 on success else error
*/
Int VpsHal_vpsVipRgbHighEnable(VpsHal_VpsVipInst vipInst,
                               UInt32 enable,
                               Ptr configOvlyPtr);

/**
 * \brief Enable RGB low port
 *
 * \param vipInst [IN] VIP Instance ID
 * \param enable  [IN] TRUE: enable, FALSE: disable
 *
 * \return 0 on success else error
*/
Int VpsHal_vpsVipRgbLowEnable(VpsHal_VpsVipInst vipInst,
                              UInt32 enable,
                              Ptr configOvlyPtr);

/**
 * \brief Bypass Chorma downsample
 *
 * \param vipInst   [IN] VIP Instance ID
 * \param chrDsInst [IN] Chroma downsample ID
 * \param enable    [IN] TRUE: bypass, FALSE: do not bypass
 *
 * \return 0 on success else error
*/
Int VpsHal_vpsVipChrDsBypass(VpsHal_VpsVipInst vipInst,
                             VpsHal_VpsChrdsInst chrDsInst,
                             UInt32 bypass,
                             Ptr configOvlyPtr);

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
UInt32 VpsHal_vpsVipGetConfigOvlySize(VpsHal_VpsVipInst vipInst);

/**
 *  Void VpsHal_vpsVipCreateConfigOvly
 *  \brief This function is used to create the complete VPDMA register overlay
 *  for VIP multiplexers. It just create the overlay but does not
 *  initialize the virtual registers with the configuration.
 *  Configuration can be done by calling appropriate vip mux setting function.
 *
 *  \param vipInst          VIP Instance
 *  \param configOvlyPtr    Pointer to memory where VPDMA overlay will be
 *                          stored
 *
 *  \return                 0 on success, -1 on error
 */
Int32 VpsHal_vpsVipCreateConfigOvly(VpsHal_VpsVipInst vipInst,
                                    Ptr ConfigOvlyPtr);

/**
 * \brief Enable/Disable HDVPSS power and clocks in PRCM module
 *
 * This gets called inside VpsHal_vpsInit()
 *
 * \param enable TRUE: enable HDVPSS power and clocks, FALSE: disable HDVPSS
 *
 * \return 0 on success else error
*/
int VpsHal_prcmEnableHdvpss(int enable);


UInt32 VpsHal_vpsGetVipMuxValue(VpsHal_VpsVipInst vipInst);

UInt32 VpsHal_vpsGetVipMuxRegOffset(VpsHal_VpsVipInst vipInst);

/**
 *  VpsHal_vpsClkcModuleGetUnderFlowStatus
 *  \brief Function to get the underflow status of the given VENC.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param module     VENC Module.
 *
 *  \return           Returns 0 on success else returns error value.
 */
UInt32 VpsHal_vpsClkcModuleGetUnderFlowStatus(UInt32 venc);

/**
 *  VpsHal_vpsClkcModuleGetUnderFlowStatus
 *  \brief Function to get the underflow status of the given VENC.
 *  VpsHal_vpsInit function must be called prior to this.
 *
 *  \param module     VENC Module.
 *
 *  \return           Returns 0 on success else returns error value.
 */
Void VpsHal_vpsClkcModuleClearUnderFlow(UInt32 venc);

/**
 *  VpsHal_vpsVipCscReset
 *  \brief Function to reset CSC module of the VIP block.
 *
 *  \param vipInst    Instance of VIP (0/1)
 *  \param enable     TRUE to reset, bring out of reset otherwise
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVipCscReset(VpsHal_VpsVipInst vipInst, UInt32 enable);

/**
 *  VpsHal_vpsVipScReset
 *  \brief Function to reset SC module of the VIP block.
 *
 *  \param vipInst    Instance of VIP (0/1)
 *  \param enable     TRUE to reset, bring out of reset otherwise
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVipScReset(VpsHal_VpsVipInst vipInst, UInt32 enable);


/**
 *  VpsHal_vpsVipChrdsReset
 *  \brief Function to reset CHR DS module of the VIP block.
 *
 *  \param vipInst    Instance of VIP (0/1)
 *  \param chrdsInst  Instance of CHR DS (0/1)
 *  \param enable     TRUE to reset, bring out of reset otherwise
 *
 *  \return           Returns 0 on success else returns error value.
 */
Int VpsHal_vpsVipChrdsReset(VpsHal_VpsVipInst vipInst,
                            VpsHal_VpsChrdsInst chrdsInst, UInt32 enable);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_VPS_H */

