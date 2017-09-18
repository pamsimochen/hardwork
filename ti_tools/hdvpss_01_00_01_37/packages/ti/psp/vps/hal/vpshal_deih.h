/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_deih.h
 *
 *  \brief VPS Deinterlacer High Quality HAL header file
 *  This file exposes the HAL APIs of the VPS High Quality Deinterlacer.
 *  This HAL is meant to be used by other VPS driver. The application don't
 *  have to access this HAL module.
 *
 */

#ifndef _VPSHAL_DEIH_H
#define _VPSHAL_DEIH_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps_advCfgDeiHq.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Instance 0 of DEI HQ */
#define VPSHAL_DEIH_INST_0              (0u)
/** \brief DEIH maximum number of instances */
#define VPSHAL_DEIH_MAX_INST            (1u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsHal_DeihInitParams
 *  \brief Instance specific parameters used at the init.
 */
typedef struct
{
    UInt32                  instId;
    /**< DEIH Instance */
    UInt32                  baseAddress;
    /**< Base address of the instance */
} VpsHal_DeihInitParams;

/**
 *  struct VpsHal_DeihMdtConfig
 *  \brief Configuration parameters of MDT sub-module.
 */
typedef struct
{
    UInt32                  fldMode;
    /**< Motion detection mode (4/5 field mode).
         For valid values see #Vps_DeiHqFldMode. */
    UInt32                  spatMaxBypass;
    /**< Bypass spatial maximum filtering */
    UInt32                  tempMaxBypass;
    /**< Bypass temporal maximum filtering */
    UInt32                  mvIIR;
    /**< Motion IIR filtering */
    UInt32                  lcModeEnable;
    /**< Low cost mode disables logics that
         reduces flicker and produces smooth
         motion in areas of detail */
} VpsHal_DeihMdtConfig;

/**
 *  struct VpsHal_DeihEdiConfig
 *  \brief Configuration parameters of EDI sub-module.
 */
typedef struct
{
    UInt32                  inpMode;
    /**< Interpolation mode. For valid values see #Vps_DeiHqEdiMode. */
    UInt32                  chromaEdiEnable;
    /**< Edge adaptive interpolation for chroma */
} VpsHal_DeihEdiConfig;

/**
 *  struct VpsHal_DeihTnrConfig
 *  \brief Configuration parameters of TNR sub-module.
 */
typedef struct
{
    UInt32                  chroma;
    /**< TNR for chroma */
    UInt32                  luma;
    /**< TNR for luma */
    UInt32                  adaptive;
    /**< TNR using adaptive threshold */
    UInt32                  advMode;
    /**< Advanced TNR mode. For valid values see #Vps_DeiHqTnrAdvMode. */
    UInt8                   advScaleFactor;
    /**< Scale factor in advanced TNR */
    UInt32                  skt;
    /**< Skin tone detection */
    UInt8                   maxSktGain;
    /**< Maximum gain for detected skintone */
    UInt8                   minChromaSktThrld;
    /**< Minimum diff of chroma value */
} VpsHal_DeihTnrConfig;

/**
 *  struct VpsHal_DeihFmdConfig
 *  \brief Configuration parameters of FMD sub-module.
 */
typedef struct
{
    UInt32                  filmMode;
    /**< Film Mode */
    UInt32                  bed;
    /**< Bad Edit Detection */
    UInt32                  window;
    /**< FMD operation window */
    UInt32                  lock;
    /**< Lock Deinterlacer to film mode */
    UInt32                  jamDir;
    /**< Field jamming direction. For valid values see #Vps_DeiFmdJamDir. */
    UInt16                  windowMinx;
    /**< Left boundary of FMD window */
    UInt16                  windowMiny;
    /**< Top boundary of FMD window */
    UInt16                  windowMaxx;
    /**< Right boundary of FMD window */
    UInt16                  windowMaxy;
    /**< Bottom boundary of FMD window */
} VpsHal_DeihFmdConfig;

/**
 *  struct VpsHal_DeihSnrConfig
 *  \brief Configuration parameters of SNR sub-module.
 */
typedef struct
{
    UInt32                  gnrChroma;
    /**< GNR for chroma */
    UInt32                  gnrLuma;
    /**< GNR for luma */
    UInt32                  gnrAdaptive;
    /**< Adaptive GNR */
    UInt32                  inrMode;
    /**< INR mode. For valid values see #Vps_DeiHqSnrInrMode. */
    UInt8                   gnmDevStableThrld;
    /**< GNR deviation stable thrld */
} VpsHal_DeihSnrConfig;

/**
 *  struct VpsHal_DeihMdtMiscConfig
 *  \brief Other configuration parameters of MDT sub-module.
 */
typedef struct
{
    UInt32                  adaptiveCoring;
    /**< Adaptive coring */
    UInt32                  fldComp;
    /**< Field comp */
    UInt32                  edgeKd;
    /**< Edge detect for mixing luma diffs */
    UInt32                  edgeVarThrld;
    /**< Edge detection for varying thrld */
    UInt32                  forceSlomoDiff;
    /**< Force luma differences to include immediate vicinity */
    UInt32                  forceF02Diff;
    /**< Force luma differences to frame difference */
    UInt32                  forceMv3D;
    /**< Force advanced mode to produce 3D */
    UInt32                  forceMv2D;
    /**< Force advanced mode to produce 2D */
    UInt32                  useDynGain;
    /**< Use dynamic gain */
    UInt32                  enable3PixelFilt;
    /**< Enable 3 pixel linear filter */
} VpsHal_DeihMdtMiscConfig;

/**
 *  struct VpsHal_DeihConfig
 *  \brief Configuration parameters of this module along with
 *  parameters of all other sub-modules.
 */
typedef struct
{
    UInt16                  width;
    /**< Picture width at output */
    UInt16                  height;
    /**< Picture height. It should be field size for the interlaced
         input and frame size for the progressive input */
    UInt32                  scanFormat;
    /**< Interlaced or Progressive video.
         For valid values see #FVID2_ScanFormat. */
    UInt32                  bypass;
    /**< Bypassing Deinterlacer */
    UInt32                  tempInpEnable;
    /**< 3D processing (temporal interpolation) */
    UInt32                  tempInpChromaEnable;
    /**< 3D processing for chroma */
    UInt32                  fidPol;
    /**< Invert FID polarity. For valid values see #Vps_FidPol. */
    VpsHal_DeihMdtConfig    mdt;
    /**< MDT configuration structure */
    VpsHal_DeihEdiConfig    edi;
    /**< EDI configuration structure */
    VpsHal_DeihTnrConfig    tnr;
    /**< TNR configuration structure */
    VpsHal_DeihFmdConfig    fmd;
    /**< FMD configuration structure */
    VpsHal_DeihSnrConfig    snr;
    /**< SNR configuration structure */
} VpsHal_DeihConfig;

/**
 *  struct VpsHal_DeihFmdStatus
 *  \brief FMD status information to be used by FMD ISR.
 */
typedef struct
{
    UInt32                  frameDiff;
    /**< Frame diff between 2 top/bottom field */
    UInt32                  fldDiff;
    /**< Field diff between 2 neighboring field */
    UInt32                  caf;
    /**< Detected combing artifacts */
    UInt32                  reset;
    /**< Reset status */
} VpsHal_DeihFmdStatus;

/**
 *  struct VpsHal_DeihGnmStatus
 *  \brief GNM status information to be used by FMD/GNM ISR.
 */
typedef struct
{
    UInt32                  yHistz;
    /**< Histogram (number of pix differences with value equal to 0)
         for luma */
    UInt32                  yHist0;
    /**< Histogram (number of pix differences with value less than yDelta0)
         for luma */
    UInt32                  yHist1;
    /**< Histogram (number of pix differences with value less than yDelta1)
         for luma */
    UInt32                  yHistm;
    /**< Histogram (number of pix differences with value less than yDeltam)
         for luma */
    UInt32                  cHistz;
    /**< Histogram (number of pix differences with value equal to 0)
         for chroma */
    UInt32                  cHist0;
    /**< Histogram (number of pix differences with value less than cDelta0)
         for luma */
    UInt32                  cHist1;
    /**< Histogram (number of pix differences with value less than cDelta1)
         for luma */
    UInt32                  cHistm;
    /**< Histogram (number of pix differences with value less than cDeltam)
         for luma */
} VpsHal_DeihGnmStatus;

/**
 *  struct VpsHal_DeihFmdUpdateConfig
 *  \brief FMD configuration to be updated by FMD ISR.
 */
typedef struct
{
    UInt32                  lock;
    /**< Film lock indicator */
    UInt32                  jamDir;
    /**< Field jamming direction. For valid values see #Vps_DeiFmdJamDir. */
} VpsHal_DeihFmdUpdateConfig;

/**
  * struct VpsHal_DeihGnmUpdateConfig
 *  \brief GNM configuration to be updated by FMD/GNM ISR.
 */
typedef struct
{
    UInt16                  yNoiseDev;
    /**< Measured std deviation of luma noise */
    UInt16                  cNoiseDev;
    /**< Measured std deviation of chroma noise */
    UInt16                  yDelta0;
    /**< Histogram samples hist_0 for luma */
    UInt16                  cDelta0;
    /**< Histogram samples hist_0 for chroma */
    UInt16                  yDelta1;
    /**< Histogram samples hist_1 for luma */
    UInt16                  cDelta1;
    /**< Histogram samples hist_1 for chroma */
    UInt16                  yDeltaM;
    /**< Histogram samples hist_m for luma */
    UInt16                  cDeltaM;
    /**< Histogram samples hist_m for chroma */
} VpsHal_DeihGnmUpdateConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_deihInit
 *  \brief DEIH HAL init function.
 *
 *  Initializes DEIH objects, gets the register overlay offsets for DEIH
 *  registers.
 *  This function should be called before calling any of DEIH HAL API's.
 *
 *  \param numInstances     Number of instance objects to be initialized
 *  \param initParams       Pointer to the instance parameter containing
 *                          instance specific information. If the number of
 *                          instance is greater than 1, then this pointer
 *                          should point to an array of init parameter
 *                          structure of size numInstances.
 *                          This parameter should not be NULL.
 *  \param arg              Not used currently. Meant for future purpose
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_deihInit(UInt32 numInstances,
                    const VpsHal_DeihInitParams *initParams,
                    Ptr arg);

/**
 *  VpsHal_deihDeInit
 *  \brief DEIH HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg              Not used currently. Meant for future purpose
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_deihDeInit(Ptr arg);

/**
 *  VpsHal_deihOpen
 *  \brief Returns the handle to the requested DEIH instance.
 *
 *  This function should be called prior to calling any of the DEIH HAL
 *  configuration APIs to get the instance handle.
 *
 *  \param instId           Requested DEIH instance.
 *
 *  \return                 Returns DEIH instance handle on success else
 *                          returns NULL.
 */
VpsHal_Handle VpsHal_deihOpen(UInt32 instId);

/**
 *  VpsHal_deihClose
 *  \brief Closes the DEIH HAL instance.
 *
 *  Currently this function does not do anything. It is provided in case
 *  in the future resource management is done by individual HAL - using
 *  counters.
 *
 *  \param handle           Valid handle returned by VpsHal_deihOpen function
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_deihClose(VpsHal_Handle handle);

/**
 *  VpsHal_deihGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA register configuration
 *  overlay required to program the DEIH registers using VPDMA config
 *  descriptors.
 *
 *  The client drivers can use this value to allocate memory for register
 *  overlay used to program the DEIH registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context
 *  (multiple instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay
 *  one-time and submit this to VPDMA everytime when context changes.
 *  Thus saving valuable CPU in programming the VPS registers.
 *
 *  \param handle           Instance handle
 *
 *  \return                 Returns the size of config overlay memory
 */
UInt32 VpsHal_deihGetConfigOvlySize(VpsHal_Handle handle);

/**
 *  VpsHal_deihCreateConfigOvly
 *  \brief Creates the DEIH register configuration overlay using VPDMA
 *  helper function.
 *
 *  This function does not initialize the overlay with DEIH configuration.
 *  It is the responsibility of the client driver to configure the overlay
 *  by calling VpsHal_deihSetConfig function before submitting the same
 *  to VPDMA for register configuration at runtime.
 *
 *  \param handle           Instance handle
 *  \param configOvlyPtr    Pointer to the overlay memory where the
 *                          overlay is formed. The memory for the overlay
 *                          should be allocated by client driver.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr);

/**
 *  VpsHal_deihSetConfig
 *  \brief Sets the entire DEIH configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetConfig(VpsHal_Handle handle,
                           const VpsHal_DeihConfig *config,
                           Ptr configOvlyPtr);

/**
 *  VpsHal_deihSetConfig
 *  \brief Sets Frame Size in DEIH register.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetFrameSize(VpsHal_Handle handle,
                              const VpsHal_DeihConfig *config,
                              Ptr configOvlyPtr);

/**
 *  VpsHal_deihSetMdtConfig
 *  \brief Sets the entire DEIH MDT configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH MDT registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetMdtConfig(VpsHal_Handle handle,
                              const VpsHal_DeihMdtConfig *config,
                              Ptr configOvlyPtr);

/**
 *  VpsHal_deihSetEdiConfig
 *  \brief Sets the entire DEIH EDI configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH EDI registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihEdiConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetEdiConfig(VpsHal_Handle handle,
                              const VpsHal_DeihEdiConfig *config,
                              Ptr configOvlyPtr);

/**
 *  VpsHal_deihSetTnrConfig
 *  \brief Sets the entire DEIH TNR configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH TNR registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihTnrConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetTnrConfig(VpsHal_Handle handle,
                              const VpsHal_DeihTnrConfig *config,
                              Ptr configOvlyPtr);

/**
 *  VpsHal_deihSetFmdConfig
 *  \brief Sets the entire DEIH FMD configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH FMD registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihFmdConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *  \param width            Picture width at output.
 *  \param height           Picture height at output.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetFmdConfig(VpsHal_Handle handle,
                              const VpsHal_DeihFmdConfig *config,
                              Ptr configOvlyPtr,
                              UInt32 width,
                              UInt32 height);

/**
 *  VpsHal_deihSetSnrConfig
 *  \brief Sets the entire DEIH SNR configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the entire DEIH SNR registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihSnrConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetSnrConfig(VpsHal_Handle handle,
                              const VpsHal_DeihSnrConfig *config,
                              Ptr configOvlyPtr);

/**
 *  VpsHal_deihSetMdtMiscConfig
 *  \brief Sets the other DEIH MDT configuration to either the actual DEIH
 *  registers or to the configuration overlay memory.
 *
 *  This function configures the other DEIH MDT registers. Depending
 *  on the value of configOvlyPtr parameter, the updating will happen
 *  to actual DEIH MMR or to configuration overlay memory.
 *  This function configures all the sub-modules in DEIH using other
 *  config function call.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtMiscConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihSetMdtMiscConfig(VpsHal_Handle handle,
                                  const VpsHal_DeihMdtMiscConfig *config,
                                  Ptr configOvlyPtr);

/**
 *  VpsHal_deihGetConfig
 *  \brief Gets the entire DEIH configuration from the actual DEIH registers.
 *
 *  This function gets the entire DEIH configuration. This function
 *  gets the configuration of all the sub-modules in DEIH using other
 *  get configuration function calls.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetConfig(VpsHal_Handle handle, VpsHal_DeihConfig *config);

/**
 *  VpsHal_deihGetMdtConfig
 *  \brief Gets DEIH MDT configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to MDT module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetMdtConfig(VpsHal_Handle handle,
                              VpsHal_DeihMdtConfig *config);

/**
 *  VpsHal_deihGetEdiConfig
 *  \brief Gets DEIH EDI configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to EDI module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihEdiConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetEdiConfig(VpsHal_Handle handle,
                              VpsHal_DeihEdiConfig *config);

/**
 *  VpsHal_deihGetTnrConfig
 *  \brief Gets DEIH TNR configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to TNR module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihTnrConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetTnrConfig(VpsHal_Handle handle,
                              VpsHal_DeihTnrConfig *config);

/**
 *  VpsHal_deihGetFmdConfig
 *  \brief Gets DEIH FMD configuration from the actual DEIH registers.
 *
 *  \brief This function gets the DEIH configuration specific to FMD module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihFmdConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetFmdConfig(VpsHal_Handle handle,
                              VpsHal_DeihFmdConfig *config);

/**
 *  VpsHal_deihGetSnrConfig
 *  \brief Gets DEIH SNR configuration from the actual DEIH registers.
 *
 *  This function gets the DEIH configuration specific to SNR module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihSnrConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetSnrConfig(VpsHal_Handle handle,
                              VpsHal_DeihSnrConfig *config);

/**
 *  VpsHal_deihGetMdtMiscConfig
 *  \brief Gets other DEIH MDT configuration from the actual DEIH registers.
 *
 *  This function gets the other DEIH configuration specific to MDT module.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihMdtMiscConfig structure to be
 *                          filled with register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetMdtMiscConfig(VpsHal_Handle handle,
                                  VpsHal_DeihMdtMiscConfig *config);

/**
 *  VpsHal_deihGetFmdStatus
 *  \brief Returns DEIH FMD status information that is needed by the FMD
 *  software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param status           Pointer to VpsHal_DeihFmdStatus structure
 *                          containing the register status.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetFmdStatus(VpsHal_Handle handle,
                              VpsHal_DeihFmdStatus *status);

/**
 *  VpsHal_deihGetGnmStatus
 *  \brief Returns DEIH GNM status information that is needed by the GNM
 *  software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param status           Pointer to VpsHal_DeihGnmStatus structure
 *                          containing the register status.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihGetGnmStatus(VpsHal_Handle handle,
                              VpsHal_DeihGnmStatus *status);

/**
 *  VpsHal_deihUpdateFmdConfig
 *  \brief Updates DEIH FMD configuration that needs to be updated by the
 *  FMD software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihFmdUpdateConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihUpdateFmdConfig(VpsHal_Handle handle,
                                 const VpsHal_DeihFmdUpdateConfig *config);

/**
 *  VpsHal_deihUpdateGnmConfig
 *  \brief Updates DEIH GNM configuration that needs to be updated by the
 *  GNM software (ISR context) at each frame ready interrupt.
 *
 *  \param handle           Instance handle
 *  \param config           Pointer to VpsHal_DeihGnmUpdateConfig structure
 *                          containing the register configurations.
 *                          This parameter should be non-NULL.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_deihUpdateGnmConfig(VpsHal_Handle handle,
                                 const VpsHal_DeihGnmUpdateConfig *config);

/**
 *  VpsHal_deiHqSetAdvConfig
 *  \brief This function is used to set the advanced DEI HQ parameters.
 *  This function should be called by core supporting advanced read
 *  and write ioctl. It does not set all the parameters.
 *
 *  \param handle           Instance handle
 *  \param advCfg           Pointers to structure containing DEI HQ advance
 *                          parameters
 *  \param configOvlyPtr    Pointer to the configuration overlay memory.
 *                          If this parameter is NULL, then the
 *                          configuration is written to the actual
 *                          DEIH registers. Otherwise the configuration
 *                          is updated in the memory pointed
 *                          by the same at proper virtual offsets.
 *                          This parameter can be NULL depending
 *                          on the intended usage.
 *                          Currently writing to overlay memory is not supported
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsHal_deiHqSetAdvConfig(VpsHal_Handle handle,
                               const Vps_DeiHqRdWrAdvCfg *advCfg,
                               Ptr configOvlyPtr);

/**
 *  VpsHal_deiHqGetAdvConfig
 *  \brief This function is used to get the advanced DEI HQ parameters.
 *  This function should be called by core supporting advanced read
 *  and write ioctl. It does not read all the parameters.
 *
 *  \param handle           Instance handle
 *  \param advCfg           Pointers to structure containing DEI HQ advance
 *                          parameters
 *
 *  \return                 Returns 0 on success else returns error value.
 */
Int32 VpsHal_deiHqGetAdvConfig(VpsHal_Handle handle,
                               Vps_DeiHqRdWrAdvCfg *advCfg);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_DEIH_H */
