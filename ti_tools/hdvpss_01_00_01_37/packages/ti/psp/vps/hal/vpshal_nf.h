/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpshal_nf.h
 *
 *  \brief VPS NF HAL header file
 *
 */

#ifndef _VPSHAL_NF_H
#define _VPSHAL_NF_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <xdc/runtime/System.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/cslr/cslr_vps_nf.h>
#include <ti/psp/vps/vps_m2mNsf.h>


#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Noise Filter Instance */
#define VPSHAL_NF_INST_0                (0u)
/** \brief NF maximum number of instances */
#define VPSHAL_NF_MAX_INST              (1u)

/**
  \brief Maximum number of source video index's
*/
#define VPSHAL_NF_MAX_VIDEO_SRC_IDX     (32)



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Structure containing instance specific parameters used at the init.
 */
typedef struct
{
    UInt32             instId;
    /**< NF Instance */
    UInt32             baseAddress;
    /**< Base address of the instance */
} VpsHal_NfInstParams;

/**
 *  struct VpsHal_NfConfig
 *
 *  \brief Configuration parameters for NF.
 */
typedef struct
{
    UInt32                  enable;
    /**< Flag to enable/disable noise filtering */

    Vps_NsfRefFrameConfig   refFrameConfig;
    /** reference frame config  */

    UInt32                  bypassSpatial;
    /**< Spatial Data Bypass enable */

    UInt32                  bypassTemporal;
    /**< Temporal Data Bypass enable. Note only one of the bypass (spatial or
         temporal) modes can be selected. If both bits are selected, spatial
         data bypass is selected.*/

    UInt16                  videoSrcIndex;
    /** Video index to select stored noise level 0..VPSHAL_NF_MAX_VIDEO_SRC_IDX-1 */

    UInt32                  initFrameNoise;
    /**< Flag to indicate whether to initialize internal previous frame noise.
         When starting a new index for srcIdx, this bit needs to be set to
         initialize (clear or reload) internally stored "frameNoise_previous"
         registers. When enabled, noise values specified in frameNoise are used
         as new initial values. */

    UInt32                  updateFrameNoise;
    /**< Flag to indicate whether to update previous internal frame noise. When
         enabled, internal frameNoise_previous registers get updated at the end
         of current frame processing. When disabled, updates are not done
         allowing next frame to use the same initial frame noise value used by
         the  previous frame. */

    UInt16                  width;
    /**< Width of the frame. Minimum frame width is 8 pixels */

    UInt16                  height;
    /**< Height of the frame. Minimum frame width is 8 lines Note that VPDMA
         and CHR_US frame size should still be at least 32x32 because Noise
         filter works on tiles and The minimum source tile size is 1 */

    UInt16                  spatialStrengthLow[3];
    /** Spatial strength high for Y, U and V */

    UInt16                  spatialStrengthHigh[3];
    /** Spatial strength high for Y, U and V */


    UInt16                  temporalStrength;
    /** Temporal strength */

    UInt16                  temporalTriggerNoise;
    /** Temporal filter trigger noise */

    UInt16                  noiseIirCoeff;
    /** Noise IIR co-eff  */

    UInt16                  maxNoise;
    /** Max Noise */

    UInt16                  pureBlackThres;
    /** Pure Black Threshold */

    UInt16                  pureWhiteThres;
    /** Pure White Threshold */

    UInt16                  frameNoise[3];
    /**< frame noise for Y, U and V */

} VpsHal_NfConfig;

/**
  \brief NF Config overlay structure
*/
typedef struct {

  UInt32 NF_ADDR0;
  UInt32 NF_NUM_REGS0;
  UInt32 RSV0[2];
  UInt32 NF_REG1;
  UInt32 NF_REG2;
  UInt32 NF_REG3;
  UInt32 NF_REG4;
  UInt32 NF_REG5;
  UInt32 NF_REG6;
  UInt32 NF_REG7;
  UInt32 NF_REG8;
  UInt32 NF_REG9;
  UInt32 RSV1[3];
  UInt32 NF_ADDR1;
  UInt32 NF_NUM_REGS1;
  UInt32 RSV2[2];
  UInt32 NF_REG0;
  UInt32 RSV3[3];

} VpsHal_NfConfigOverlay;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Initializes NF objects
 *
 *  This function should be called before calling any of NF HAL API's.
 *
 *  \param numInstances     Number of Noise Filter Instances
 *  \param initParams       Instance Specific Parameters
 *  \param arg              Not used currently. Meant for future purpose.
 */
Int32 VpsHal_nfInit(UInt32 numInstances,
                  const VpsHal_NfInstParams *initParams,
                  Ptr arg);

/**
 *  \brief Releases all resources allocated inside VpsHal_nfInit()
 *
 *  \param arg       Not used currently. Meant for future purpose.
 */
Int32 VpsHal_nfDeInit(Ptr arg);


/**
 *  \brief Open handle to NF HAL
 *
 *  \param nfInst       Requested NF instance
 */
VpsHal_Handle VpsHal_nfOpen(UInt32 nfInst);

/**
 *  \brief Close NF HAL handle
 *
 *  \param handle       Requested NF instance
 */
Int32 VpsHal_nfClose(VpsHal_Handle handle);

/**
 *  \brief Sets the entire NF configuration NF HW registers or NF config overlay in memory
 *
 *  \param handle           [I ] Instance handle
 *  \param config           [I ] NF configuration
 *  \param configOverlay    [ O] NF congif overlay memory address, if NULL, config is written to NF HW register, else its written to overlay memory
 *
 *  \return                 Returns 0 on success else returns error value
 */
Int32 VpsHal_nfSetConfig(VpsHal_Handle handle,
                         VpsHal_NfConfig *config,
                         VpsHal_NfConfigOverlay *configOverlay
                          );

/**
 * \brief Read frame noise for a given srcIndex
 *
 * \param handle           [I ] Instance handle
 * \param srcIdx           [I ] Video source index, 0..VPSHAL_NF_MAX_VIDEO_SRC_IDX-1
 * \param frameNoise       [ O] Frame noise for Y, U, V for that index
 *
 * \return                 Returns 0 on success else returns error value
*/
Int32 Nf_readSavedFrameNoise(VpsHal_Handle handle, UInt16 srcIdx, UInt16 frameNoise[3]);

/**
 * \brief Write frame noise for a given srcIndex in NF HW regs or config overlay
 *
 * \param handle           [I ] Instance handle
 * \param srcIdx           [I ] Video source index, 0..VPSHAL_NF_MAX_VIDEO_SRC_IDX-1
 * \param frameNoise       [I ] Frame noise for Y, U, V for that index
 * \param configOverlay    [ O] Configuration overlay
 *
 * \return                 Returns 0 on success else returns error value
*/
Int32 Nf_writeSavedFrameNoise(VpsHal_Handle handle, UInt16 srcIdx, UInt16 frameNoise[3], VpsHal_NfConfigOverlay *configOverlay);


/**
  \brief Initialize VpsHal_NfConfig with default values

  \param config   [ O] Default configuration

  \return 0 on success else returns error value
*/
Int32 VpsHal_nfGetDefaultConfig(VpsHal_NfConfig *config);


/**
  \brief Set width and height in NF HW regs or config overlay

  \param handle         [I ] Instance handle
  \param width          [I ] Width in pixels
  \param height         [I ] Height in lines
  \param configOverlay  [ O] Configuration overlay

  \return 0 on success else returns error value
*/
Int32 VpsHal_nfSetWidthHeight(VpsHal_Handle handle, UInt16 width, UInt16 height, VpsHal_NfConfigOverlay *configOverlay);

/**
  \brief Reset are stored NF context to default state

  \param handle         [I ] Instance handle

  \return 0 on success else returns error value
*/
Int32 VpsHal_nfReset(VpsHal_Handle handle);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSHAL_NF_H */

