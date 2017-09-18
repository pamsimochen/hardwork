/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_M2M_COMMON
 * \defgroup VPS_DRV_FVID2_NSF_API M2M Noise filter API
 *
 * @{
*/
/**
 * For Noise filter,
 *You can set input width and height as non-multiple of 32.
 *But input and output pitch MUST always be multiple of 32 "pixels".

 *Further the allocated buffer MUST also be as below

 *pitch x ALIGN(height,32)

 *i.e buffer allocation should be such that height is a multiple of 32.

 *Thus for D1 field of 720x240,

 *inPitch       = 736x2 (in number of bytes, YUV422I format)
 *outPitch[0] = 736 (in number of bytes, YUV420SP format, Y plane)
 *outPitch[1] = 736 (in number of bytes, YUV420SP format, C plane)

 *Buffer size would be,
 *input buffer size = 736x2x256 (next higher multiple of 32 of height)
 *output buffer size Y plane = 736x256 (next higher multiple of 32 of height)
 *output buffer size C plane = 736x256/2 (next higher multiple of 32 of height)

 *Note valid data in the input/output buffer can be of size 720x240.

 *This applies for both TI816x and TI814x.
 *This is HW constraint.
 */
/**
 * \file vps_m2mNsf.h
 *
 * \brief M2M Noise filter API
*/

#ifndef _VPS_NSF_H_
#define _VPS_NSF_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/vps.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief NSF Inst ID: 0  */
#define VPS_M2M_INST_NF0          (0u)

/** \brief NSF Inst ID: Max  */
#define VPS_NSF_INST_ID_MAX        (1u)

/** \brief Default value for NSF processing parameters */
#define VPS_NSF_PROCESSING_CFG_DEFAULT      (UInt32)(0xFFFFFFFFu)

/** \brief Total channels that can be processed across all handles  */
#define VPS_NSF_MAX_CH_IN_ALL_HANDLES (16)

/** \brief NSF max logical channels per handle. User should NOT modify this value */
#define VPS_NSF_MAX_CH_PER_HANDLE     (VPS_NSF_MAX_CH_IN_ALL_HANDLES)

/** \brief NSF max possible handles. User should NOT modify this value */
#define VPS_NSF_MAX_HANDLES           (4u)

/* NSF IOCTLs  */

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_M2M_NSF

  @{
*/

/**
  * \brief Set processing related config for a specific channel
  *
  * This IOCTL can be called while frames are being submitted for processing.
  * This IOCTL is effective from the next frame submission for that channel.
  *
  * \param cmdArgs       [IN]  Vps_NsfProcessingCfg *
  * \param cmdArgsStatus [OUT] NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_NSF_SET_PROCESSING_CFG   (VPS_M2M_NSF_IOCTL_BASE + 0x0000u)

/**
  * \brief Set data format for a specific channel
  *
  * This IOCTL can be called while frames are being submitted for processing.
  * This IOCTL is effective from the next frame submission for that channel.
  *
  * \param cmdArgs       [IN]  Vps_NsfDataFormat *
  * \param cmdArgsStatus [OUT] NULL
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_NSF_SET_DATA_FORMAT         (VPS_M2M_NSF_IOCTL_BASE + 0x0001u)

/**
  * \brief Get Advanced configuration parameters for a specific channel
  *
  * This IOCTL can be called to get Advanced configuration parameters for a specific channel.
  *
  *
  * \param cmdArgs       [OUT]  Vps_NsfAdvConfig *
  *
  *
  * \return FVID_SOK on success, else failure
  *
*/
#define IOCTL_VPS_READ_ADV_NSF_CFG         (VPS_M2M_NSF_IOCTL_BASE + 0x0002u)

/* @} */

/**
 * \brief NSF bypass mode
*/
typedef enum
{

    VPS_NSF_DISABLE_NONE = 0,
                               /**< Full NF, Spatial NF ON + Temporal NF ON */
    VPS_NSF_DISABLE_SNF,   /**< Only Temporal NF ON, Spatial NF bypassed */
    VPS_NSF_DISABLE_TNF,   /**< Only Spatial NF ON, Temporal NF bypassed */
    VPS_NSF_DISABLE_SNF_TNF,
                               /**< No NF, Only YUV422 to YUV420 downsample ON */
    VPS_NSF_BYPASS_MODE_MAX    /**< Max bypass mode value */
} Vps_NsfBypassMode;

/**
 *  \brief Reference frame configuration
 */
typedef enum
{
    VPS_NSF_REF_FRAME_ENABLED = 0,
    /**< Reference frame is enabled with DMA. Data Passed */
    VPS_NSF_REF_FRAME_DMA_ENABLED,
    /**< Only DMA enabled, Output Forced to Black */
    VPS_NSF_REF_FRAME_DISABLED
    /**< DMA disabled, Output Forced to Black */

} Vps_NsfRefFrameConfig;

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * \brief NSF processing parameters
*/
typedef struct
{

    UInt32 channelNum;
  /**< Logical channel for which these parameters belong */

    UInt32 bypassMode;
  /**< NSF bypass mode. For valid values see #Vps_NsfBypassMode */

    UInt32 frameNoiseAutoCalcEnable;
  /**< TRUE: "frameNoise" auto-calculated by HW,
   *  FALSE: auto-calculation disabled, staticFrameNoise[] used instead */

    UInt32 frameNoiseCalcReset;
  /**< TRUE: "frameNoise" calculation is reset,
   *  FALSE: "frameNoise" calculation is not reset
   *  Use this when changing input stream or input resolution or
   *  for the first frame of processing.
   */

    UInt32 subFrameModeEnable;
  /**< TRUE: SubFrame based NSF is ON,
   *   Vps_NsfPerFrameCfgSubFrameCfg must be set for FVID2_Frame.perFrameCfg,
   *  FALSE: Frame/field based NSF  */

    UInt32 numLinesPerSubFrame;
  /**< Num lines per subframe, except for last subframe. */

    UInt32 staticFrameNoise[FVID2_MAX_PLANES];
  /**< Static "frameNoise", valid only when frameNoiseAutoCalcEnable = FALSE */

    UInt32 spatialStrengthLow[FVID2_MAX_PLANES];
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

    UInt32 spatialStrengthHigh[FVID2_MAX_PLANES];
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

    UInt32 temporalStrength;
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

    UInt32 temporalTriggerNoise;
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

    UInt32 noiseIirCoeff;
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

    UInt32 maxNoise;
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

    UInt32 pureBlackThres;
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

    UInt32 pureWhiteThres;
  /**< Advanced NSF parameters, refer to NSF spec for details,
   *  set to VPS_NSF_PROCESSING_CFG_DEFAULT for default values  */

} Vps_NsfProcessingCfg;

/**
 * \brief NSF data format
 *
 * - Note, output width = inWidth, output height = inHeight
*/
typedef struct
{

    UInt32 channelNum;
  /**< Logical channel number  */

    UInt32 inMemType;
  /**< Tiled or non-tiled memory type selection.
    For valid values see #Vps_VpdmaMemoryType. */

    UInt32 outMemType;
  /**< Tiled or non-tiled memory type selection.
    For valid values see #Vps_VpdmaMemoryType. */

    UInt32 inDataFormat;
    /**< Input data format, must be FVID2_DF_YUV422I_YUYV.
         For valid values see #FVID2_DataFormat. */

    UInt32 inFrameWidth;
  /**< Total frame width, must be multiple of 2  */

    UInt32 inFrameHeight;
  /**< Total frame height, must be multiple of 2 */

    UInt32 inPitch;
  /**< Input Pitch in bytes, must be multiple of 32  */

    UInt32 outDataFormat;
    /**< Output data format, must be FVID2_DF_YUV420SP_UV.
         For valid values see #FVID2_DataFormat. */

    UInt32 outPitch[FVID2_MAX_PLANES];
  /**< Output Pitch for Y and C data in bytes, must be multiple of 32  */

} Vps_NsfDataFormat;

/**
 * \brief NSF Create arguments
*/
typedef struct
{

    UInt32 numCh;
  /**< Number of logical channels in this handle, must be < VPS_NSF_MAX_CH_PER_HANDLE */

    Vps_NsfDataFormat *dataFormat;
  /**< NSF input dataFormat for each channel.
    This is a pointer to an array having 'numCh' elements. */

    Vps_NsfProcessingCfg *processingCfg;
  /**< NSF processing related parameters for each channel.
    This is a pointer to an array having 'numCh' elements. */

} Vps_NsfCreateParams;

/**
 * \brief NSF Create status
*/
typedef struct
{
    Int32 retVal;
  /**< Driver create return status  */

    UInt32 maxFramesInProcessFrames;
  /**< Max number of frames that can be
   * processed per FVID2_processFrames() call */

    UInt32 maxFramesPerChInQueue;
  /**< Max number of frames from a CH that can be submitted
   * for processing without having to wait for completion */

    UInt32 maxReqInQueue;
  /**< Max number of requests that can be submitted for
   * processing without having to wait for completion */

} Vps_NsfCreateStatus;



/**
*  Struct Vps_NsfAdvConfig
*  \brief Advance Configuration parameters
*/
typedef struct
{

    UInt32                              channelNum;
    /**< Logical channel number  */

    Vps_NsfDataFormat                   dataFormat;
    /**< NSF input dataFormat for each channel */

    Vps_NsfProcessingCfg                processingCfg;
    /**< NSF processing related parameters for each channel */

    UInt16                              videoSrcIndex;
    /**< Video index to select stored noise level 0..
        VPSHAL_NF_MAX_VIDEO_SRC_IDX-1 */

    UInt32                              enable;
    /**< Flag to enable/disable noise filtering */

    Vps_NsfRefFrameConfig                refFrameConfig;
    /**< reference frame config  */


}Vps_NsfAdvConfig;

/**
* Struct Vps_M2mNsfRtParams
* \brief runtime parameters structure
*/
typedef struct
{

    Vps_NsfDataFormat                   *dataFormat;
    /**< NSF input dataFormat for each channel */

    Vps_NsfProcessingCfg                *processingCfg;
    /**< NSF processing related parameters for each channel */

}Vps_M2mNsfRtParams;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* @} */

#ifdef __cplusplus
}
#endif


#endif /* #ifndef _VPS_NSF_H */
