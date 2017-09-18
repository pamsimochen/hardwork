/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_ede.h
 *
 *  \brief VPS EDE HAL header file
 *  This file exposes the HAL APIs of the VPS Edge Enhancer.
 *
 */

#ifndef _VPSHAL_EDE_H
#define _VPSHAL_EDE_H

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

/** \brief EDE on the main path */
#define VPSHAL_EDE_INST_0               (0u)
/** \brief EDE maximum number of instances */
#define VPSHAL_EDE_MAX_INST             (1u)

/**
 *  enum VpsHal_EdeMode
 *  \brief Enum for Ede mode (Interlaced or Progressive video). This is used to
 *  select one of pre-calculated coefficients. There is no configuration in
 *  CHR_US for mode.
 */
typedef enum
{
    VPSHAL_EDE_MODE_PROGRESSIVE = 0,
    /**< Progressive video */
    VPSHAL_EDE_MODE_INTERLACED
    /**< Interlaced video */
} VpsHal_EdeMode;

/**
 *  enum VpsHal_EdePeaking
 *  \brief Enum for selecting peaking algorithm for peaking block.
 */
typedef enum
{
    VPSHAL_EDE_PEAK_2D = 0,
    /**< 2D Peaking */
    VPSHAL_EDE_PEAK_VERTICAL
    /**< Vertical Peaking */
} VpsHal_EdePeaking;

/**
 *  enum VpsHal_EdePeaking
 *  \brief Enum for selecting Vemo Mode.
 */
typedef enum
{
    VPSHAL_EDE_VEMO_MODE_NORMAL = 0,
    /**< Normal Operation */
    VPSHAL_EDE_VEMO_MODE_BYPASS_RIGHT_HALF,
    /**< Right Half is bypassed */
    VPSHAL_EDE_VEMO_MODE_BYPASS_LEFT_HALF,
    /**< Left Half is bypassed */
    VPSHAL_EDE_VEMO_MODE_BYPASS_ALL
    /**< All bypassed */
} VpsHal_EdeVemoMode;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    UInt32          instId;
    UInt32          baseAddress;
} VpsHal_EdeInitParams;

/**
 *  struct VpsHal_EdeConfig
 *
 *  \brief Configuration parameters for EDE.
 *
 */
typedef struct
{
    UInt32 ltiEnable;
    /**< Enables/Disables Luminance Transition Improvement Block */
    UInt32 horzPeaking;
    /**< Enables/Disables Horizontal Peaking */
    UInt32 ctiEnable;
    /**< Enables/Disables Chrominance Transition Improvement Block */
    UInt32 transAdjustEnable;
    /**< Enables/Disables Transition Adjustment for Chroma Block */
    UInt32 lumaPeaking;
    /**< Enables/Disables Luminance Peaking */
    UInt32 chromaPeaking;
    /**< Enables/Disables Chrominance Peaking */
    VpsHal_EdePeaking peakAlgo;
    /**< Used to select peaking algorithm */
    UInt32 peakingClipSuppressEnable;
    /**< Enables/Disables Luminance Transition Improvement Block */
    VpsHal_EdeVemoMode vemoMode;
    /**< Selects the vemo mode */
    UInt16 minClipLuma;
    /**< Minimum value of the C clipping in the clipping block */
    UInt16 maxClipLuma;
    /**< Maximum value of the Y clipping in the clipping block */
    UInt16 minClipChroma;
    /**< Minimum value of the C clipping in the clipping block */
    UInt16 maxClipChroma;
    /**< Maximum value of the C clipping in the clipping block */
    UInt16 width;
    /**< width of the image */
    UInt16 height;
    /**< Height of the image */
    UInt32 bypass;
    /**< used to bypass ede block */
} VpsHal_EdeConfig;

/**
 *  struct VpsHal_EdePeakingConfig
 *
 *  \brief Configuration parameters for EDE.
 *
 */
typedef struct
{
    UInt32 hpfGain[8];
    UInt32 hpfGain8;
    UInt32 yThrPLow;
    UInt32 yThrNLow;
    UInt32 yThrPHigh;
    UInt32 yThrNHigh;
    UInt32 ySlopePLowx16;
    UInt32 ySlopeNLowx16;
    UInt32 ySlopePHighShift3;
    UInt32 ySlopeNHighShift3;
    UInt32 yNegCoringGain8;
    UInt32 yNegCoringLimit8;
    UInt32 hpfNormShift;
    UInt32 yMax;
    UInt32 yMin;

    UInt32 gainEdgeMaxYx16; //CFG12
    UInt32 gainEdgeSlopeYx16;
    UInt32 gainPeakYx8;
    UInt32 gainPeakCx8;
    UInt32 gainHorizontalSlopex16;
    UInt32 cSlopePx512; //CFG14
    UInt32 cSlopeNx512;
    UInt32 cGainMaxx512; //CFG15
    UInt32 cGainMinx512;
    UInt32 VDiffYMax;
} VpsHal_EdePeakingConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_edeInit
 *  \brief EDE HAL init function.
 *
 *  Initializes EDE objects, gets the register overlay offsets for EDE
 *  registers.
 *  This function should be called before calling any of EDE HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of EDE HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *  \param numInstances NONE
 *  \param initParams   NONE
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_edeInit(UInt32 numInstances,
                   VpsHal_EdeInitParams *initParams,
                   Ptr arg);

/**
 *  VpsHal_edeDeInit
 *  \brief EDE HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_edeDeInit(Ptr arg);


/**
 *  VpsHal_edeOpen
 *  \brief Returns the handle to the requested EDE instance.
 *
 *  This function should be called prior to calling any of the EDE HAL
 *  configuration APIs to get the instance handle.
 *
 *  VpsHal_edeInit should be called prior to this.
 *
 *  \param edeInst Requested EDE instance.
 *
 *  \return        Returns EDE instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_edeOpen(UInt32 edeInst);

/**
 *  VpsHal_edeClose
 *  \brief Closes the EDE HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case in the
 *  future resource management is done by individual HAL - using counters.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle Valid handle returned by VpsHal_edeOpen function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_edeClose(VpsHal_Handle handle);


/**
 *  VpsHal_edeGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the EDE registers using VPDMA config
 *  descriptors.
 *
 *  The client drivers can use this value to allocate memory for register overlay
 *  used to program the EDE registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context (multiple
 *  instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay one-time
 *  and submit this to VPDMA everytime when context changes. Thus saving valuable
 *  CPU in programming the VPS registers.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle Valid handle returned by VpsHal_edeOpen function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
UInt32 VpsHal_edeGetConfigOvlySize(VpsHal_Handle handle);

/**
 *  VpsHal_edeCreateConfigOvly
 *  \brief Creates the EDE register configuration overlay using VPDMA helper
 *  function.
 *
 *  This function does not initialize the overlay with EDE configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  VpsHal_edeSetConfig function before submitting the same to VPDMA for register
 *  configuration at runtime.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle         Valid handle returned by VpsHal_edeOpen function.
 *
 *  \param configOvlyPtr  Pointer to the overlay memory where the overlay is
 *                        formed. The memory for the overlay should be
 *                        allocated by client driver.
 *                        This parameter should be non-NULL.
 *
 *  \return               Returns 0 on success else returns error value.
 */
Int VpsHal_edeCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr);


/**
 *  VpsHal_edeSetConfig
 *  \brief Sets the EDE configuration to either the
 *  actual EDE registers or to the configuration overlay memory.
 *
 *  This function configures the EDE registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual EDE MMR or
 *  to configuration overlay memory.
 *  This function configures all the sub-modules in EDE using other config
 *  function call.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  EDE registers or configuration overlay memory are programmed
 *  according to the parameters passed.
 *
 *  \param handle        Valid handle returned by VpsHal_edeOpen function.
 *
 *  \param config        Pointer to VpsHal_EdeConfig structure containing the
 *                       register configurations. This parameter should be
 *                       non-NULL.
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual EDE registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_edeSetConfig(VpsHal_Handle handle,
                  const VpsHal_EdeConfig *config,
                  Ptr configOvlyPtr);

/**
 *  VpsHal_edeGetConfig
 *  \brief Gets the EDE configuration from the actual EDE registers.
 *
 *  VpsHal_edeInit and VpsHal_edeOpen should be called prior to this.
 *
 *  \param handle Valid handle returned by VpsHal_edeOpen function.
 *
 *  \param config Pointer to VpsHal_EdeConfig structure to be filled with
 *                register configurations. This parameter should be non-NULL.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_edeGetConfig(VpsHal_Handle handle, VpsHal_EdeConfig *config);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_EDE_H */
