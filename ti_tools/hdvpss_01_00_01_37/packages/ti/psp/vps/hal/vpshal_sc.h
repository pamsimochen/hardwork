/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vpshal_sc.h
 *
 * \brief VPS Scalar HAL Header File
 * This file exposes the APIs of the Scalar HAL to the upper layers.
 *
 */

#ifndef _VPSHAL_SC_H
#define _VPSHAL_SC_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/common/vps_types.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Scalar on the primary path */
#define VPSHAL_SC_INST_1                (0u)
/** \brief Scalar on the auxiliary path */
#define VPSHAL_SC_INST_2                (1u)
/** \brief Scalar on the write back path */
#define VPSHAL_SC_INST_5                (2u)
/** \brief Scalar on the VIP0 path */
#define VPSHAL_SC_INST_3                (3u)
/** \brief Scalar on the VIP1 path */
#define VPSHAL_SC_INST_4                (4u)
/** \brief Scalar maximum number of instances */
#define VPSHAL_SC_MAX_INST              (5u)


#define VPSHAL_SC_NUM_PHASES            (32u)

/* Horizontal (luma, chroma), Vertical (luma, chroma), Bilinear (luma, chroma)*/
#define VPSHAL_SC_HQ_NUM_COEFF_SETS     (6u)

/* Horizontal (luma, chroma), Vertical (luma, chroma) */
#define VPSHAL_SC_NUM_COEFF_SETS        (4u)

#define VPSHAL_SC_HORZ_NUM_COEFF_SETS   (2u)
#define VPSHAL_SC_VERT_NUM_COEFF_SETS   (2u)
#define VPSHAL_SC_BILINEAR_NUM_COEFF_SETS   (2u)

#define VPSHAL_SC_COEFF_ALIGN           (16u)

#define VPSHAL_SC_HQ_COEFF_OVLY_SIZE      (VPSHAL_SC_NUM_PHASES *       \
                                           VPSHAL_SC_HQ_NUM_COEFF_SETS * \
                                           VPSHAL_SC_COEFF_ALIGN)

#define VPSHAL_SC_COEFF_OVLY_SIZE         (VPSHAL_SC_NUM_PHASES *       \
                                           VPSHAL_SC_NUM_COEFF_SETS *    \
                                           VPSHAL_SC_COEFF_ALIGN)

#define VPSHAL_SC_HORZ_COEFF_OVLY_SIZE    (VPSHAL_SC_NUM_PHASES *       \
                                           VPSHAL_SC_HORZ_NUM_COEFF_SETS * \
                                           VPSHAL_SC_COEFF_ALIGN)

#define VPSHAL_SC_VERT_COEFF_OVLY_SIZE    (VPSHAL_SC_NUM_PHASES *       \
                                           VPSHAL_SC_VERT_NUM_COEFF_SETS * \
                                           VPSHAL_SC_COEFF_ALIGN)

#define VPSHAL_SC_BILINEAR_COEFF_OVLY_SIZE (VPSHAL_SC_NUM_PHASES *       \
                                           VPSHAL_SC_BILINEAR_NUM_COEFF_SETS * \
                                           VPSHAL_SC_COEFF_ALIGN)

/**
 *  enum VpsHal_ScSetPhaseInfoMode
 *  \brief Enum for specifying whether Scalar phase offsets are used from
 *         application OR calculated locally from config
 *         used for setting following values
 *         PolyPh: row_acc_inc, row_acc_offset,row_acc_offset_b
 *         RAV   : Sc_factor_rav, Row_acc_init_rav, Row_acc_init_rav_b
 */
typedef enum
{
    VPS_SC_SET_PHASE_INFO_DEFAULT = 0,
    /**< Calculate phase info locally and use . */
    VPS_SC_SET_PHASE_INFO_FROM_APP
    /**< use the values passed from calling layer. */
} VpsHal_ScSetPhaseInfoMode;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief struct VpsHal_ScInstParams
 *  Structure containing instance specific parameters used at the init.
 *
 *  instId        Instance Identifier
 *  baseAddress   Physical register start address for this instance
 */
typedef struct
{
    UInt32                  instId;
    /**< Instance ID. */
    UInt32                  baseAddress;
    /**< Base address of the instance */
    VpsHal_VpdmaConfigDest  coeffConfigDest;
    /**< VPDMA configuration register destination for the Scalar instance. */
    UInt32                  isHqScalar;
    /**< Type of scalar - high quality or normal quality. */
} VpsHal_ScInstParams;

/**
 *  struct VpsHal_ScConfig
 *
 *  \brief This structure defines the parameters required to
 *  configure scalar. It can be used to set the parameters in the scalar
 *  registers as well as in the VPDMA register overlay. It also is used to
 *  configure runtime changes in the scale ratio. For memory to memory and
 *  display driver, all the parameters in the structure will be used and
 *  configured. For the runtime changes, only size and position parameters will
 *  be configured but still upper layer will have to pass all the parameters
 *  because scalar uses these parameters to calculate ratio specific parameters.
 *
 */
typedef struct
{
    UInt32 tarWidth, tarHeight;
    /**< Target image Height */
    /**< Target image Width */
    UInt32 srcWidth, srcHeight;
    /**< Source image height. This is the original non-cropped image height */
    /**< Source image width. This is the original non-cropped image width */
    UInt32 cropStartX, cropStartY;
    /**< Vertical Offset of the cropped image. From this vertical offset,
         original image will be cropped. If cropping is not required, this
         should be set to zero. */
    /**< Horizontal Offset of the cropped image. From this horizontal offset,
         original image will be cropped. If cropping is not required, this
         should be set to zero. */
    UInt32 cropWidth, cropHeight;
    /**< Cropped image Height. Original image will be cropped to this size. If
         cropping is not required, then this should same as the original image
         size */
    /**< Cropped image Width. Original image will be cropped to this size. If
         cropping is not required, then this should same as the original image
         size */
    Vps_ScanFormat inFrameMode, outFrameMode;
    /**< Input Frame Mode i.e. Interlaced or progressive. */
    /**< Output Frame Mode i.e. interlaced or progressive. Input and
         output frame mode can be different so that scalar can convert
         one format to another */
    UInt32 hsType;
    /**< This is used to specify horizontal scaling algorithm. Typically,
         Decimation is used for down scaling and polyphase filters are used
         for up scaling.
         For valid values see #Vps_ScHorzScalarType */
    UInt32 nonLinear;
    /**< Flag to indicate whether linear or non-linear scaling is used for
         horizontal scalar. Non-scaling is available for polyphase filter
         only */
    UInt32 stripSize;
    /**< Size of left and right strip for nonlinear horizontal scaling in terms
         of pixel. It must be set to zero for linear horz scaling. */
    UInt32 vsType;
    /**< This is used to specify vertical scaling algorithm. Typically, Running
         Average Filter is used for down scaling and polyphase filters are used
         for up scaling.
         For valid values see #Vps_ScVertScalarType */
    Vps_FidPol fidPol;
    /**< This is used to specify whether input FID will be inverted before use
         or not */
    Vps_ScSelfGenFid selfGenFid;
    /**< Flag to indicate whether to use self Generate FID feature of the
         scalar */
    UInt32 defConfFactor;
    /**< Default Confidence factor when edge detection is disabled. This is
        used only for the High Quality scalar where edge detection block is
        available. */
    UInt32 biLinIntpType;
    /**< Variable to specify Bilinear Interpolation Type
     *   For valid values see #Vps_ScBiLinearIntpType */
    UInt32 enableEdgeDetect;
    /**< Enables/Disables Edge detection. defConfFactor is only used when edge
         detection is disabled. Edge detection is only available in high
         quality scalar.*/
    UInt32 hPolyBypass;
    /**< Flag to bypass horizontal scalar. When enabled, it bypasses horizontal
         polyphase scalar when tar_w = src_w or tar_w = src_w/2 or
         tar_w = src_w/4. When disabled, polyphase scalar is always used
         regardless of the width configuration */
    UInt32 enablePeaking;
    /**< Flag to enable Peaking block in the scalar. All the
         coefficients required for the peaking block are already configured
         as part of initialization. These coefficient values are considered
         as expert values and taken from the specs. If required, it should
         be changed at the compile time in vpshaChrUsExpert.h file. */
    UInt32 bypass;
    /**< Flag to indicate whether to bypass scalar or not */
    VpsHal_ScSetPhaseInfoMode phInfoMode;
    /**< Flag to indicate whether to program Scalar phase information from
               below values Or calculate locally from config.  Used in subframe
               level scaling operation. Based on this flag following values are pregrammed:
               for PolyPh : row_acc_inc, row_acc_offset,row_acc_offset_b
               for RAV    : Sc_factor_rav, Row_acc_init_rav, Row_acc_init_rav_b*/
    UInt32 rowAccInc, rowAccOffset, rowAccOffsetB;
    /**< Scalar phase value for vertical polyphase filter case, for top field/
                progressive input */
    /**< Scalar phase value for vertical polyphase filter case, for bottom field */
    UInt32 ravScFactor, ravRowAccInit, ravRowAccInitB;
    /**< Scalar phase value for vertical Running average filter case, for top field/
                progressive input */
    /**< Scalar phase value for vertical Running average filter case, for bottom field*/
    UInt32 linAccIncr, colAccOffset;
    /**< Scalar phase value for horizontal polyphase filter case */
    Vps_ScPeakingConfig *peakingCfg;
    /**< Pointer to Peaking params. If this is NULL peaking configuation will
     *   not be applied.  Else peaking configuration for this will be applied
     *   Values applied will be applicable to all the channels of the driver.
     *   This is not per channel configuration. Only used by
     *   VpsHal_scSetAdvConfig function
     */
    Vps_ScEdgeDetectConfig *edgeDetectCfg;
    /**< Pointer to edege detect parameters.
     *   If this is NULL peaking configuation will
     *   not be applied.  Else configuration for this will be applied
     *   Values applied will be applicable to all the channels of the driver.
     *   This is not per channel configuration.  Only used by
     *   VpsHal_scSetAdvConfig function
     */
    Ptr arg;
    /**< Currently not used. It should be NULL. */
} VpsHal_ScConfig;

/**
 * VpsHal_ScCoeffConfig
 * \brief This structre is used to pass the coefficient information to the
 * scalar HAL.  User has to pass the appropriate set of scalar coefficients
 * to be used from the pre-defined scalar coefficient sets.  Else
 * user can specify the own scalar coefficients.
 */
typedef struct
{
    UInt32 hScalingSet;
    /**< Coefficient set to be used for the horizontal scaling.
     *   For valid values see #Vps_ScCoeffSet.
     */
    UInt32 vScalingSet;
    /**< Coefficient set to be used for the vertical scaling.
     *   For valid values see #Vps_ScCoeffSet.
     */
}VpsHal_ScCoeffConfig;

/**
 * VpsHal_ScFactorConfig
 * \brief This structure contains configuration information related to
 * horizontal and vertical scaling factor for the scaler HAL. This structure
 * contains the horizontal and vertical scaling configuration that is dependent
 * on the scaling factor.
 */
typedef struct
{
    UInt32 hsType;
    /**< This indicates the horizontal scaling algortihm used.
         For valid values see #Vps_ScHorzScalarType */
    UInt32 hScalingSet;
    /**< Coefficient set to be used for the horizontal scaling.
     *   For valid values see #Vps_ScCoeffSet.
     */
    UInt32 vsType;
    /**< This indicates the horizontal scaling algortihm.
         For valid values see #Vps_ScVertScalarType */
    UInt32 vScalingSet;
    /**< Coefficient set to be used for the vertical scaling.
     *   For valid values see #Vps_ScCoeffSet.
     */
}VpsHal_ScFactorConfig;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  VpsHal_scInit
 *  \brief Function to initialize scalar driver. It initializes global
 *  structure scInfo, sets the peaking and edge detect parameters
 *  in the registers and gets the VPDMA register overlay size and
 *  virtual register offsets in the overlay from the VPDMA. Since
 *  VPDMA register overlay size and virtual register offsets are
 *  non-changing and same for all scalars, these are set at initial
 *  time.
 *
 *  \param numInstances     Number of Noise Filter Instances and number of
 *                          array elements in the second argument
 *  \param initParams       Instance Specific Parameters
 *  \param arg              For the Future use, not used currently.
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int VpsHal_scInit(UInt32 numInstances,
                  const VpsHal_ScInstParams *initParams,
                  Ptr arg);

/**
 *  VpsHal_scDeInit
 *  \brief Function to de-initialize scalar driver.
 *
 *  \param arg             For the Future use, not used currently.
 *
 *  \return                Returns 0 on success else returns error value
 */
Int VpsHal_scDeInit(Ptr arg);

/**
 *  VpsHal_scOpen
 *  \brief Function to request the handle to the scalar. This function must
 *  be called before using any scalar. Handle returned from this
 *  function must be used in all other functions to set/get parameters
 *  correctly.
 *
 *  \param scalar           Instance of the scalar to be opened
 *
 *  \return                 Handle of scalar on success. NULL on error
 */
VpsHal_Handle VpsHal_scOpen(UInt32 scalar);

/**
 *  VpsHal_scClose
 *  \brief Function to close/release the scalar handle. Currently it does
 *  nothing other than decrementing reference count.
 *
 *  \param handle           Scalar Handle
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scClose(VpsHal_Handle handle);

/**
 *  VpsHal_scSetConfig
 *  \brief This function is used to set the scalar parameters in appropriate
 *  scalar register or in the virtual registers created in the register
 *  overlay whose address is passed as the last argument. If last
 *  argument is null, it will set the parameters in registers otherwise
 *  it will set the parameters in the VPDMA register overlay.
 *
 *  \param handle           Scalar Handle
 *  \param config           pointers to structure containing scalar parameters
 *  \param ovlyPtr          Pointer to memory containing VPDMA register
 *                          overlay for scalar. It can be set to memory
 *                          pointer to program register in overlay or set
 *                          to null to program directly in hardware register
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scSetConfig(VpsHal_Handle handle,
                       const VpsHal_ScConfig *config,
                       Ptr ovlyPtr);

/**
 *  VpsHal_scGetConfig
 *  \brief This function is used to get the scalar parameters from the
 *  appropriate scalar register.
 *
 *  \param handle           Scalar Handle
 *  \param config           pointers to structure in which scalar parameters
 *                          will be returned
 *  \param ovlyPtr          Overlay pointer from which configuration to be
 *                          read.  If this is NULL configuration will be read
 *                          from registers
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scGetConfig(VpsHal_Handle handle,
                       VpsHal_ScConfig *config,
                       Ptr ovlyPtr);

/**
 *  Void VpsHal_scSetRunTimeConfig
 *  \brief Function is used to set the scaling ratio and position specific
 *  parameters in the VPDMA register overlay. It will be called at the
 *  run time when scaling ratio or position is changed at the run
 *  time while display is on. This function will also be used for
 *  memory to memory driver to set the scaling parameters in register
 *  overlay. This function will only change scaling ratio specific
 *  parameters. It will not change scaling algorithm or parameters
 *  configurable in CFG_SC0 register.
 *
 *  \param handle           Scalar Handle
 *  \param memPtr           VPDMA register overlay pointer
 *  \param config           Pointer to structure containing scalar parameters.
 *
 *  \return                 None
 */
Void VpsHal_scSetRunTimeConfig(VpsHal_Handle handle,
                               const VpsHal_ScConfig *config,
                               Ptr ovlyPtr);

/**
 *  Void VpsHal_scGetRegOvlySize
 *  \brief Function to get the VPDMA register overlay size required to
 *  store scalar registers. This overlay is mainly used for stoing
 *  register for the runtime configuration as well as for the memory
 *  to memory driver where configuration changes for each channel.
 *
 *  \param handle           Scalar Handle
 *
 *  \return                 Register Overlay size
 */
UInt32 VpsHal_scGetConfigOvlySize(VpsHal_Handle handle);

/**
 *  Void VpsHal_scGetCoeffOvlySize
 *  \brief This function is used to get the memory size required to
 *  store coefficients. It returns size in terms of bytes. Upper
 *  layer can allocate the memory and pass this memory with the
 *  coefficients to fill the the memory with coefficients.
 *
 *  \param horzCoeffOvlySize Location to return size of horizontal scaling
 *                           coefficients
 *  \param vertCoeffOvlySize Location to return size of vertical scaling
 *                           coefficients
 *  \param vertBilinearCoeffOvlySize Location to return size of vertical
 *                                   bilinear scaling coefficients. If not HQ
 *                                   scalar, value returned shall be 0.
 */
Void VpsHal_scGetCoeffOvlySize(VpsHal_Handle handle,
                               UInt32 * horzCoeffOvlySize,
                               UInt32 * vertCoeffOvlySize,
                               UInt32 * vertBilinearCoeffOvlySize);

/**
 *  Void VpsHal_scCreateCoeffOvly
 *  \brief Function is used to fill up the coefficient overlay with the
 *  standard coefficients provided by the IP team. The coefficients
 *  are selected based on the scaling ratio. Upper layer should
 *  allocate coefficient memory required to store these coefficients.
 *  They can get the size of the coefficient memory by calling
 *  Sc_getCoeffOvlySize function.
 *
 *  \param handle           Scalar Handle
 *  \param config           Scalar Parameters using which it calculates the
 *                          scaling ratio.
 *  \param horzCoeffMemPtr  Location to return a pointer to horizontal scaling
 *                          coefficients
 *  \param vertCoeffMemPtr  Location to return a pointer to vertical scaling
 *                          coefficients
 *  \param vertBilinearCoeffMemPtr Location to return a pointer to vertical
 *                          bilinear scaling coefficients (only if applicable,
 *                          i.e. for HQ scalar. Can be NULL otherwise).
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scCreateCoeffOvly(VpsHal_Handle handle,
                             const VpsHal_ScCoeffConfig *config,
                             Ptr * horzCoeffMemPtr,
                             Ptr * vertCoeffMemPtr,
                             Ptr * vertBilinearCoeffMemPtr);

/**
 *  VpsHal_scCreateUserCoeffOvly
 *
 *  \brief  If the application has its own set of coefficients and does not
 *          want to use driver provided coefficient, this function is used. It
 *          copies the user provided coefficients locally so that they can be
 *          used instead of the defaults during scaling.
 *          Coefficient Memory must be allocated by using size provided
 *          by scalar.
 *  \param  handle          Scalar Handle
 *  \param  coeff           Structure containing pointers to various
 *                          coefficients. If provided as NULL, this function
 *                          has been called to get, rather than create the
 *                          coefficients.
 *  \param  horzCoeffMemPtr Location to return a pointer to horizontal scaling
 *                          coefficients
 *  \param  vertCoeffMemPtr Location to return a pointer to vertical scaling
 *                          coefficients
 *  \param  vertBilinearCoeffMemPtr Location to return a pointer to vertical
 *                          bilinear scaling coefficients (only if applicable,
 *                          i.e. for HQ scalar. Can be NULL otherwise).
 *
 *  \return 0               on Success
 *          -1              on Error
 */
Int VpsHal_scCreateUserCoeffOvly(VpsHal_Handle handle,
                                 const Vps_ScCoeff *coeff,
                                 Ptr * horzCoeffMemPtr,
                                 Ptr * vertCoeffMemPtr,
                                 Ptr * vertBilinearCoeffMemPtr);

/**
 *  Void VpsHal_scCreateConfigOvly
 *  \brief This function is used to create the complete VPDMA register overlay
 *  for scalar registers. It just create the overlay but does not
 *  initialize the virtual registers with the configuration.
 *  Configuration can be done by calling Sc_setParams function.
 *
 *  \param handle           Scalar Handle
 *  \param configOvlyPtr    Pointer to memory where VPDMA overlay will be
 *                          stored
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scCreateConfigOvly(VpsHal_Handle handle, Ptr configOvlyPtr);

/**
 *  VpsHal_scGetVpdmaConfigDest
 *  \brief Returns the VPDMA configuration destination to program coefficients
 *  for this scalar instance.
 *
 *  \param handle           Scalar Handle
 *
 *  \return                 VPDMA configuration destination
 */
VpsHal_VpdmaConfigDest VpsHal_scGetVpdmaConfigDest(VpsHal_Handle handle);

/**
 *  VpsHal_scSetAdvConfig
 *  \brief This function is used to set the advanced scalar parameters.
 *  This function should be called by core supporting advanced read
 *  and write ioctl.  It does not set all the parameters.
 *
 *  \param handle           Scalar Handle
 *  \param config           pointers to structure containing scalar parameters
 *  \param ovlyPtr          Pointer to memory containing VPDMA register
 *                          overlay for scalar. It can be set to memory
 *                          pointer to program register in overlay or set
 *                          to null to program directly in hardware register
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scSetAdvConfig(VpsHal_Handle handle,
                          const VpsHal_ScConfig *config,
                          Ptr ovlyPtr);

/**
 *  VpsHal_scGetScFactorConfig
 *  \brief This function is used to get the information that
 *  identifies the scaling factor for the provided configuration.
 *  It also sets the register related to vsType changes.
 *
 *  \param handle           Scalar Handle
 *  \param config           Pointer to scaling configuration structure
 *  \param scFactorConfig   Pointer to scaling factor configuration structure
 *                          in which the information is to be returned.
 *  \param ovlyPtr          Pointer to memory containing VPDMA register
 *                          overlay for scalar. It needs to be set to memory
 *                          pointer to program register in overlay.
 *
 *  \return                 0 on success, -1 on error
 */
Int VpsHal_scGetScFactorConfig(VpsHal_Handle handle,
                               VpsHal_ScConfig *config,
                               VpsHal_ScFactorConfig *scFactorConfig,
                               Ptr ovlyPtr);


#ifdef __cplusplus
}
#endif

#endif /* End of #ifndef _VPSHAL_SC_H */
