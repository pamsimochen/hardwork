/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_cig.h
 *
 *  \brief HAL layer for the VPS Constrained Image Generator (CIG).
 *  This file exposes the HAL APIs of the VPS CIG.
 *  This HAL is meant to be used by other VPS driver. The application don't
 *  have to access this HAL module.
 *
 */

#ifndef _VPSHAL_CIG_H
#define _VPSHAL_CIG_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief CIG Instance 0. */
#define VPSHAL_CIG_INST_0               (0u)
/** \brief CIG maximum number of instances */
#define VPSHAL_CIG_MAX_INST             (1u)

/**
 *  enum VpsHal_CigPipWinSz
 *  \brief Enumerations for PIP window size.
 */
typedef enum
{
    VPSHAL_CIG_PWS_SUB_WINDOW = 0,
    /**< Sub-Window size */
    VPSHAL_CIG_PWS_FULL_SIZE
    /**< Full size */
} VpsHal_CigPipWinSz;

/**
 *  enum VpsHal_CigTransMask
 *  \brief Enumerations for transparency color mask bit (Number of LSB bits to
 *  mask when checking for pixel transparency).
 */
typedef enum
{
    VPSHAL_CIG_TM_NO_MASK = 0,
    /**< Disable Masking */
    VPSHAL_CIG_TM_MASK_1_LSB,
    /**< Mask 1 LSB before checking */
    VPSHAL_CIG_TM_MASK_2_LSB,
    /**< Mask 2 LSB before checking */
    VPSHAL_CIG_TM_MASK_3_LSB
    /**< Mask 3 LSB before checking */
} VpsHal_CigTransMask;

/**
 *  enum VpsHal_CigOvlyType
 *  \brief Enumerations for different overlay types supported by CIG.
 */
typedef enum
{
    VPSHAL_COT_CONFIG = 0,
    VPSHAL_COT_PIP_ENABLE_CONFIG,
    VPSHAL_COT_RT_CONFIG,
    VPSHAL_COT_NUM_CONFIG
} VpsHal_CigOvlyType;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_CigInitParams
 *  \brief Instance specific parameters used at the init.
 */
typedef struct
{
    UInt32              instId;
    /**< CIG Instance */
    UInt32              baseAddress;
    /**< Base address of the instance */
} VpsHal_CigInitParams;

/**
 * struct VpsHal_CigTransConfig
 *  \brief Transparency and blending configuration parameters.
 */
typedef struct
{
    UInt32              transparency;
    /**< Enable Transparency */
    VpsHal_CigTransMask mask;
    /**< Transparency color mask bit */
    UInt32              alphaBlending;
    /**< Enable alpha blending */
    UInt8               alphaValue;
    /**< Alpha blending value */
    UInt8               rTransColor;
    /**< Transparency color - Red */
    UInt8               gTransColor;
    /**< Transparency color - Green */
    UInt8               bTransColor;
    /**< Transparency color - Blue */
} VpsHal_CigTransConfig;

/**
 *  struct VpsHal_CigPipConfig
 *  \brief CIG PIP path configuration parameters.
 */
typedef struct
{
    UInt16              dispWidth;
    /**< PIP width at output */
    UInt16              dispHeight;
    /**< PIP height at output */
    UInt16              xPos;
    /**< PIP window X Position */
    UInt16              yPos;
    /**< PIP window Y Position */
    UInt16              pipWidth;
    /**< PIP window width */
    UInt16              pipHeight;
    /**< PIP window height */
} VpsHal_CigPipConfig;

/**
 *  struct VpsHal_CigConfig
 *  \brief Configuration parameters of CIG module.
 */
typedef struct
{
    UInt32              enableCig;
    /**< CIG module enable */
    UInt32              nonCtrInterlace;
    /**< Interlaced for non-constrained out */
    UInt32              ctrInterlace;
    /**< Interlaced for constrained out */
    UInt32              pipInterlace;
    /**< Interlaced for pip out */
    UInt32              vertDecimation;
    /**< Vertical decimation */
    UInt32              fldRepeat;
    /**< Use field repeat */
    UInt32              enablePip;
    /**< CIG PIP Path enable */
    VpsHal_CigPipWinSz  pipWinSz;
    /**< PIP Window Size */
    UInt16              width;
    /**< Picture width at output */
    UInt16              height;
    /**< Picture height at output */
    VpsHal_CigPipConfig pipConfig;
    /**< PIP window configuration */
    VpsHal_CigTransConfig hdmi;
    /**< Transparency config for HDMI out */
    VpsHal_CigTransConfig hdcomp;
    /**< Transparency config for HDCOMP out */
    VpsHal_CigTransConfig pip;
    /**< Transparency config for PIP out */
} VpsHal_CigConfig;

/**
 *  struct VpsHal_CigRtConfig
 *  \brief Run time configuration parameters of this module.
 *  Note: Currently only PIP configuration is supported as a runtime change.
 *  Even in the PIP configuration, only the positioning and PIP size is
 *  configured in the overlay. The output window size is used only for error
 *  checking. Hence the upper layer has to provide proper value to PIP display
 *  dimensions as well.
 */
typedef struct
{
    UInt16              xPos;
    /**< PIP window X Position */
    UInt16              yPos;
    /**< PIP window Y Position */
    UInt16              pipWidth;
    /**< PIP window width */
    UInt16              pipHeight;
    /**< PIP window height */
} VpsHal_CigRtConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_cigInit
 *  \brief CIG HAL init function
 *  Initializes CIG objects, gets the register overlay offsets for CIG
 *  registers.
 *  This function should be called before calling any of CIG HAL API's
 *
 *  \param numInstances Number of instance objects to be initialized
 *  \param initParams   Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances
 *  \param arg          Not used currently.Meant for future purpose
 *
 *  \return             Returns 0 on success else returns error value
 */
Int VpsHal_cigInit(UInt32 numInstances,
                   const VpsHal_CigInitParams *initParams,
                   Ptr arg);

/**
*   VpsHal_cigDeInit
 *  \brief CIG HAL exit function.
 *  Currently this function does not do anything.
 *
 *  \param arg          Not used currently. Meant for future purpose
 *
 *  \return             Returns 0 on success else returns error value
 */
Int VpsHal_cigDeInit(Ptr arg);

/**
 *  VpsHal_cigOpen
 *  \brief Returns the handle to the requested CIG instance.
 *  This function should be called prior to calling any of the CIG HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param instId   Requested CIG instance.
 *
 *  \return         Returns CIG instance handle on success else
 *                  returns NULL
 */
VpsHal_Handle VpsHal_cigOpen(UInt32 instId);

/**
 *  VpsHal_cigClose
 *  \brief Closes the CIG HAL instance.
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param  handle  Valid handle returned by VpsHal_cigOpen function
 *
 *  \return         Returns 0 on success else returns error value
 */
Int VpsHal_cigClose(VpsHal_Handle handle);

/**
 *  VpsHal_cigGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the CIG registers using VPDMA
 *  config descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the CIG registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context
 *  (multiple instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay
 *  one-time and submit this to VPDMA everytime when context changes.
 *  Thus saving valuable CPU in programming the VPS registers
 *
 * \param  handle   Instance handle
 *
 * \return          Returns the size of the config overlay memory
 */
UInt32 VpsHal_cigGetConfigOvlySize(VpsHal_Handle handle,
                                   VpsHal_CigOvlyType ovlyType);

/**
 *  VpsHal_cigCreateConfigOvly
 *  \brief Creates the CIG register configuration overlay using VPDMA helper
 *  function
 *  This function does not initialize the overlay with CIG configuration.
 *  It is  the responsibility of the client driver to configure the overlay
 *  by calling VpsHal_cigSetConfig function before submitting the same
 *  to the VPDMA for register configuration at runtime
 *
 *  \param handle           Instance handle
 *  \param configOvlyPtr    Pointer to the overlay memory where the
 *                          overlay is formed. The memory for the overlay
 *                          should be allocated by client driver.
 *                          This parameter should be non-NULL
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_cigCreateConfigOvly(VpsHal_Handle handle,
                                 VpsHal_CigOvlyType ovlyType,
                                 Ptr configOvlyPtr);

/**
 *  VpsHal_cigSetConfig
 *  \brief Sets the entire CIG configuration to either the actual CIG registers
 *  or to the configuration overlay memory.
 *
 *  This function configures the entire CIG registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual CIG MMR or to configuration overlay memory
 *
 *  \param handle           Instance Handle
 *  \param config           Pointer to VpsHal_CigConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the configuration
 *                          is written to the actual CIG registers. Otherwise
 *                          the configuration is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsHal_cigSetConfig(VpsHal_Handle handle,
                          const VpsHal_CigConfig *config,
                          Ptr configOvlyPtr);

/**
 *  VpsHal_cigGetConfig
 *  \brief Gets the entire CIG configuration from the actual CIG registers.
 *  This function gets the entire CIG configuration.
 *
 *  \param handle   Instance handle
 *  \param config   Pointer to VpsHal_CigConfig structure to be filled with
 *                  register configurations.
 *                  This parameter should be non-NULL.
 *
 *  \return         Returns 0 on success else returns error value
 */
Int32 VpsHal_cigGetConfig(VpsHal_Handle handle, VpsHal_CigConfig *config);

/**
 *  VpsHal_cigSetRtConfig
 *  \brief Sets the runtime CIG configuration to the configuration
 *  overlay memory.
 *
 *  This function configures only the runtime CIG registers.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_CigConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          This parameter should be non-NULL.
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_cigSetRtConfig(VpsHal_Handle handle,
                            const VpsHal_CigRtConfig *config,
                            Ptr configOvlyPtr);

/**
 *  VpsHal_cigEnablePip
 *  \brief Function to enable/disable pip window in the register/overlay
 *
 *  \param handle           Instance handle
 *  \param isEnable         Flag to indicate whether to enable/disable pip
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_cigEnablePip(VpsHal_Handle handle,
                          UInt32 isEnable,
                          Ptr configOvlyPtr);

/**
 *  VpsHal_cigEnableMain
 *  \brief Function to enable/disable main window in the register/overlay.
 *         As such, there is no enable bit for the main input, This
 *         function is mainly used for storing CFG0 register in overlay
 *         memory when display path is from Vcomp.
 *
 *  \param handle           Instance handle
 *  \param isEnable         Flag to indicate whether to enable/disable main input
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_cigEnableMain(VpsHal_Handle handle,
                           UInt32 isEnable,
                           Ptr configOvlyPtr);

Int32 VpsHal_cigSetHdmiTransConfig(VpsHal_Handle handle,
                                   const VpsHal_CigTransConfig *config,
                                   Ptr configOvlyPtr);

Int32 VpsHal_cigSetHdcompTransConfig(VpsHal_Handle handle,
                                     const VpsHal_CigTransConfig *config,
                                     Ptr configOvlyPtr);

Int32 VpsHal_cigSetPipTransConfig(VpsHal_Handle handle,
                                  const VpsHal_CigTransConfig *config,
                                  Ptr configOvlyPtr);

Int32 VpsHal_cigGetHdmiTransConfig(VpsHal_Handle handle,
                                   VpsHal_CigTransConfig *config);

Int32 VpsHal_cigGetHdcompTransConfig(VpsHal_Handle handle,
                                     VpsHal_CigTransConfig *config);

Int32 VpsHal_cigGetPipTransConfig(VpsHal_Handle handle,
                                  VpsHal_CigTransConfig *config);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_CIG_H */
