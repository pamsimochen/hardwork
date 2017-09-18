/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_vcomp.h
 *
 *  \brief VPS Vcomp HAL header file
 *  This file exposes the HAL APIs of the VPS Vcomp.
 *
 */

#ifndef VPS_VCOMPHAL_H
#define VPS_VCOMPHAL_H

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

/** \brief  VCOMP instance */
#define VPSHAL_VCOMP_INST_0             (0u)
/** \brief VCOMP maximum number of instances */
#define VPSHAL_VCOMP_MAX_INST           (1u)

/**
 *  enum Vcomp_DwnStrmSrcFid
 *  \brief Enum for controlling downstream source FID control
 */
typedef enum
{
    VPSHAL_VCOMP_DWNSTRM_SRC_FID_OUT_SAME_UPSTRM =0,
    /**< Source FID (bit 0) out of the VCOMP is the same as the respective bit
         from the Upstream Module */
    VPSHAL_VCOMP_DWNSTRM_SRC_FID_OUT_INVERSE_UPSTRM,
    /**< Source FID (bit 0) out of the VCOMP is the inverse of the respective bit
         from the Upstream Module*/
    VPSHAL_VCOMP_DWNSTRM_SRC_FID_OUT_ZERO,
    /**< Source FID (bit 0) out of the VCOMP is '0'*/
    VPSHAL_VCOMP_DWNSTRM_SRC_FID_OUT_ONE
    /**< Source FID (bit 0) out of the VCOMP is '1' */
     } VpsHal_VcompDwnStrmSrcFid;


/**
 *  enum Vcomp_DwnStrmSrcFid
 *  \brief Enum for controlling downstream Encoder FID control
 */
typedef enum
{
    VPSHAL_VCOMP_DWNSTRM_ENC_FID_OUT_SAME_UPSTRM =0,
    /**< Encoder FID (bit 0) out of the VCOMP is the same as the respective bit
         from the Upstream Module */
    VPSHAL_VCOMP_DWNSTRM_ENC_FID_OUT_INVERSE_UPSTRM,
    /**< Encoder FID (bit 0) out of the VCOMP is the inverse of the respective
         bit from the Upstream Module*/
    VPSHAL_VCOMP_DWNSTRM_ENC_FID_OUT_ZERO,
    /**< Encoder FID (bit 0) out of the VCOMP is '0'*/
    VPSHAL_VCOMP_DWNSTRM_ENC_FID_OUT_ONE
    /**< Encoder FID (bit 0) out of the VCOMP is '1' */
} VpsHal_VcompDwnStrmEncFid;


/**
 *  enum Vcomp_nfHandling
 *  \brief Enum for controlling new frame handling. This is used for deciding FID of
 *  which frame out of AUX and MAIN should be used.
 */
typedef enum
{
    VPSHAL_VCOMP_NF_USE_FIRST_OF_MAIN_AUX =0,
    /**< When both Main and Aux sources are enabled, the first NF to enter the
         VCOMP is used */
    VPSHAL_VCOMP_NF_UNDEFINED,
    /**< Undefined */
    VPSHAL_VCOMP_NF_USE_MAIN_SRC,
    /**< When both Main and Aux sources are enabled, the Main source's
         NF is used */
    VPSHAL_VCOMP_NF_USE_AUX_SRC
    /**< When both Main and Aux sources are enabled, the Aux source's NF
         is used */
} VpsHal_VcompNfHandling;


/**
 *  enum Vcomp_StartUpHandling
 *  \brief Enum for controlling vcomp startup behviour
 */
typedef enum
{
    VPSHAL_VCOMP_START_UP_VIDEO_IN_OUT =0,
    /**< Default condition for video in/video out operation */
    VPSHAL_VCOMP_START_UP_VIDEO_IN_NO_OUT
    /**< Set for video in/NO video out */
     } VpsHal_VcompStartUpHandling;


/**
 *  enum Vcomp_StartUpHandling
 *  \brief Enum for selecting one out of two input source
 */
typedef enum
{
    VPSHAL_VCOMP_SOURCE_MAIN = 0,
    /**< Main Input src */
    VPSHAL_VCOMP_SOURCE_AUX
    /**< Aux Input src */
}VpsHal_VcompInputVideo;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcomp_VcompInstParams
 *  \brief Structure containing Instance Specific Parameters
 *
 *  instId        Instance Identifier
 *  baseAddress   Physical register start address for this instance
 */
typedef struct
{
    UInt32             instId;
    /**< Vcomp Instance */
    UInt32             baseAddress;
    /**< Base address of the instance */
} VpsHal_VcompInstParams;

/**
 *  struct Vcomp_ClipWindowConfig
 *  \brief Structure containing paramters for cliping window size
 *
 */
typedef struct
{
   UInt16            skipNumPix;
   /**< Starting from the first pixel of each line, this field
        determines the number of incoming pixels to discard as part of the
        Compositor's picture clipping feature */
   UInt16            useNumPix;
   /**< skipNumPix number of pixels, this field determines the number of pixels
        to use from each incoming line */
   UInt16            skipNumLines;
   /**< Starting from the first line of each field or frame, this field
        determines the number of incoming lines to discard as part of the
        Compositor's picture clipping feature */
   UInt16            useNumLines;
   /**< After skipNumLines number of lines, this field determines the number of
        lines to use from each incoming field or frame */
}VpsHal_VcompClipWindowConfig;


/**
 *  struct Vcomp_DisplayPosition
 *  \brief Structure containing co ordinates for cliping window position
 *
 */
typedef struct
{
   UInt16            yOrigin;
   /**< Row origin index for the source picture */
   UInt16            xOrigin;
   /**< Column origin index for the source picture */
}VpsHal_VcompDisplayPosition;


/**
 *  struct Vcomp_Color
 *  \brief Structure containing color information in YCrCb could be used for
 *  background and alternate color.
 *
 */
typedef struct
{
    UInt16 yLuma;
    /**< Luma/Y value */
    UInt16 crChroma;
    /**< chroma/Cr value */
    UInt16 cbChroma;
    /**< chroma/Cb value */
}VpsHal_VcompColor;


/**
 *  struct VpsHal_VcompInputPathConfig
 *  \brief Structure containing all configuration paramters for input video to VCOMP.
 *
 */
typedef struct
{
    UInt32                       enabled;
    /**< decides respective ( aux or main) input video is enabled or not */
    UInt32                       enableFixedData;
    /**< decides whether video data or alternate color has to be displayed.*/
    UInt16                       numLines;
    /**< Number of lines in a field or frame from the incoming i/p source */
    UInt16                       numPix;
    /**< Number of pixels per line from the incoming i/p source */
    VpsHal_VcompClipWindowConfig clipWindow;
    /**< decides size of clip window for i/p video source */
    VpsHal_VcompDisplayPosition  dsplyPos;
    /**< decides display co ordinates for i/p source video */
    VpsHal_VcompColor            altColor;
    /**< alternate colcor to be displayed inb YCrCb in the case enableFixedData
         is enabled */
    VpsHal_VcompStartUpHandling  startUpHandling;
    /**< decides behviour vcomp for input source at start up. */
} VpsHal_VcompInputPathConfig;



/**
 *  struct VpsHal_VcompOutputDsplyConfig
 *  \brief Structure containing all configuration paramters for output from vcomp.
 *
 */
typedef struct
{
    UInt16                   dsplyNumlines;
    /**< Number of pixels per line for the output picture. */
    UInt16                   dsplyNumPix;
    /**< decides size of clip window for i/p video source */
    VpsHal_VcompColor        bckGrndColor;
    /**< background colcor to be displayed in YCrCb */
    VpsHal_VcompInputVideo   selectVideoOntop;
    /**< decides which video has to be on top. */
} VpsHal_VcompOutputDsplyConfig;


/**
 *  struct VpsHal_VcompConfig
 *  \brief Structure containing all configuration paramters for VCOMP.
 *
 */
typedef struct
{
    VpsHal_VcompInputPathConfig   mainVideo;
    /**< configuration paramters for main input video */
    VpsHal_VcompInputPathConfig   auxVideo;
    /**< configuration paramters for aux input video. */
    VpsHal_VcompOutputDsplyConfig dsplyConfig;
    /**< configuration paramters for output from vcomp */
    UInt32                        dsplyTimeout;
    /**< Timeout counter in VPS system clock cycles */
    VpsHal_VcompDwnStrmSrcFid     dwnStrmSrcFid;
    /**< configuration for downstream Source FID control */
    VpsHal_VcompDwnStrmEncFid     dwnStrmEncoderFid;
    /**< configuration for downstream Encoder FID control */
    VpsHal_VcompNfHandling        nfHandling;
    /**< controlling new frame handling. This is used for deciding FID of which
         frame out of AUX and MAIN should be used.*/
} VpsHal_VcompConfig;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/**
 *  VpsHal_vcompInit
 *  \brief VCOMP HAL init function.
 *
 *  Initializes VCOMP objects, gets the register overlay offsets for VCOMP
 *  registers.
 *  This function should be called before calling any of VCOMP HAL API's.
 *
 *  VPDMA HAL should be initialized prior to calling any of VCOMP HAL
 *  functions.
 *
 *  All global variables are initialized.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompInit(UInt32 numInstances,
                     VpsHal_VcompInstParams *vcompInstParams,
                     Ptr arg);



/**
 *  VpsHal_vcompDeInit
 *  \brief VCOMP HAL exit function.
 *
 *  Currently this function does not do anything.
 *
 *  \param arg    Not used currently. Meant for future purpose.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompDeInit(Ptr arg);


/**
 *  VpsHal_vcompOpen
 *  \brief Returns the handle to the requested VCOMP instance.
 *
 *  This function should be called prior to calling any of the VCOMP HAL
 *  configuration APIs to get the instance handle.
 *
 *  Vcomp_init should be called prior to this.
 *
 *  \param vcompInst Requested VCOMP instance.
 *
 *  \return          Returns VCOMP instance handle on success else returns NULL.
 */
VpsHal_Handle VpsHal_vcompOpen(UInt32 vcompInst);



/**
 *  VpsHal_vcompClose
 *  \brief Closes the VCOMP HAL instance
 *
 *  Vcomp_init and Comp_open should be called prior to this.
 *
 *  \param handle Valid handle returned by Vcomp_open function.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompClose(VpsHal_Handle handle);


/**
 *  VpsHal_vcompGetConfigOvlySize
 *  \brief Returns the size (in bytes) of the VPDMA
 *  register configuration overlay required to program the VCOMP registers using
 *  VPDMA config descriptors.
 *
 *  The client drivers can use this value to allocate memory for register overlay
 *  used to program the COMP registers using VPDMA.
 *  This feature will be useful for memory to memory operation of clients in
 *  which the client drivers have to switch between different context (multiple
 *  instance of mem-mem drivers).
 *  With this feature, the clients can form the configuration overlay one-time
 *  and submit this to VPDMA everytime when context changes. Thus saving valuable
 *  CPU in programming the VPS registers.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  \param handle     Valid handle returned by Vcomp_open function.
 *  \param inputVideo NONE
 *
 *  \return           Returns 0 on success else returns error value.
 */
UInt32 VpsHal_vcompGetConfigOvlySize(VpsHal_Handle handle,
                                     VpsHal_VcompInputVideo inputVideo);


/**
 *  VpsHal_vcompCreateConfigOvly
 *  \brief Creates the VCOMP register configuration
 *  overlay using VPDMA helper function.
 *
 *  This function does not initialize the overlay with VCOMP configuration. It is
 *  the responsibility of the client driver to configure the overlay by calling
 *  Comp_setConfig function before submitting the same to VPDMA for register
 *  configuration at runtime.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param inputVideo    NONE
 *  \param configOvlyPtr Pointer to the overlay memory where the overlay is
 *                       formed. The memory for the overlay should be allocated
 *                       by client driver. This parameter should be non-NULL.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompCreateConfigOvly(VpsHal_Handle handle,
                                 VpsHal_VcompInputVideo inputVideo,
                                 Ptr configOvlyPtr);


/**
 *  VpsHal_vcompSetConfig
 *  \brief Sets the entire VCOMP configuration to either the
 *  actual VCOMP registers or to the configuration overlay memory.
 *
 *  This function configures the entire VCOMP registers. Depending on the value
 *  of configOvlyPtr parameter, the updating will happen to actual VCOMP MMR or
 *  to configuration overlay memory.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  VCOMP registers or configuration overlay memory are programmed
 *  according to the parameters passed.
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param config        Pointer to VpsHal_VcompConfig structure containing the
 *                       register configurations. This parameter should be
 *                       non-NULL.
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual VCOMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompSetConfig(VpsHal_Handle handle,
                   const VpsHal_VcompConfig *config,
                   Ptr configOvlyPtr);

/**
 *  VpsHal_vcompGetConfig
 *  \brief Gets the entire VCOMP configuration from the actual
 *  VCOMP registers.
 *
 *  This function gets the entire VCOMP configuration.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  \param handle Valid handle returned by Comp_open function.
 *
 *  \param config Pointer to VpsHal_VcompConfig structure to be filled with
 *                register configurations. This parameter should be non-NULL.
 *
 *  \return       Returns 0 on success else returns error value.
 */
Int VpsHal_vcompGetConfig(VpsHal_Handle handle,
                   VpsHal_VcompConfig *config);

/**
 *  VpsHal_vcompSetInputVideoConfig
 *  \brief Set the input configuration for either
 *  aux or main video of VCOMP to either the actual VCOMP registers or to the
 *  configuration overlay memory.
 *
 *  This function configures only aux or main input channel.Depending on the
 *  value of configOvlyPtr parameter, the updating will happen to actual VCOMP
 *  MMR or to configuration overlay memory.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  Aux oe Main input configuration registers are programmed
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param inputVideo    NONE
 *
 *  \param inVideoconfig Pointer to VpsHal_VcompInputPathConfig structure
 *                       containing the register configurations for either Aux
 *                       or Main input video paramters. This parameter should be
 *                       non-NULL.
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual VCOMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompSetInputVideoConfig(VpsHal_Handle handle,
                              VpsHal_VcompInputVideo inputVideo,
                              const VpsHal_VcompInputPathConfig *inVideoconfig,
                              Ptr configOvlyPtr);

/**
 *  VpsHal_vcompGetInputVideoConfig
 *  \brief Get the input configuration for either
 *  aux or main video of VCOMP.
 *
 *  This function get configures for only aux or main input channel.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  Aux or Main input configuration registers are returned
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param inputVideo    NONE
 *  \param inVideoconfig Pointer to VpsHal_VcompInputPathConfig structure
 *                       returns configurations for either Aux or Main input
 *                       video paramters. This parameter should be non-NULL.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompGetInputVideoConfig(VpsHal_Handle handle,
                              VpsHal_VcompInputVideo inputVideo,
                              VpsHal_VcompInputPathConfig *inVideoconfig);


/**
 *  VpsHal_vcompSetOutputDisplayConfig
 *  \brief Set display output configuration
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  display output configuration registers are programmed
 *
 *  \param handle        Valid handle returned by Vcomp_open function.
 *
 *  \param displyconfig  Pointer to VpsHal_VcompOutputDsplyConfig structure
 *                       containing the register configurations for Vcomp output
 *
 *  \param configOvlyPtr Pointer to the configuration overlay memory. If this
 *                       parameter is NULL, then the configuration is written to
 *                       the actual VCOMP registers. Otherwise the configuration
 *                       is updated in the memory pointed by the same at proper
 *                       virtual offsets. This parameter can be NULL depending
 *                       on the intended usage.
 *
 *  \return              Returns 0 on success else returns error value.
 */
Int VpsHal_vcompSetOutputDisplayConfig(VpsHal_Handle handle,
                              const VpsHal_VcompOutputDsplyConfig *displyconfig,
                              Ptr configOvlyPtr);


/**
 *  VpsHal_vcompGetOutputDisplayConfig
 *  \brief Get the input configuration for either aux or main video of VCOMP.
 *
 *  Vcomp_init and Vcomp_open should be called prior to this.
 *
 *  Aux or Main input configuration registers are returned
 *
 *  \param handle       Valid handle returned by Vcomp_open function.
 *
 *  \param displyconfig Pointer to VpsHal_VcompOutputDsplyConfig structure
 *                      returns configurations for  for Vcomp output.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int VpsHal_vcompGetOutputDisplayConfig(VpsHal_Handle handle,
                              VpsHal_VcompOutputDsplyConfig *displyconfig);

Int32 VpsHal_vcompGetEnableConfigOvlySize(VpsHal_Handle handle,
                                          VpsHal_VcompInputVideo inputVideo);
Int VpsHal_vcompCreateEnableConfigOvly(VpsHal_Handle handle,
                                       VpsHal_VcompInputVideo inputVideo,
                                       Ptr configOvlyPtr);

Int VpsHal_vcompEnableVideoInputPath(VpsHal_Handle handle,
                                     VpsHal_VcompInputVideo inputVideo,
                                     UInt32 enable,
                                     Ptr configOvlyPtr);

Int VpsHal_vcompSetInputSize(VpsHal_Handle handle,
                             VpsHal_VcompInputVideo inputVideo,
                             UInt16 width,
                             UInt16 height,
                             Ptr configOvlyPtr);

Void VpsHal_vcompGetInputSize(VpsHal_Handle handle,
                              VpsHal_VcompInputVideo inputVideo,
                              UInt32 *width,
                              UInt32 *height);
#ifdef __cplusplus
}
#endif

#endif /* #ifndef VPS_VCOMPHAL_H */

