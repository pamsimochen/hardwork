/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_M2M_COMMON
 * \defgroup VPS_DRV_FVID2_SC_API M2M SC API
 *
 * @{
 */

/**
 *  \file vps_m2mSc.h
 *
 *  \brief M2M Scalar API. This is the public header files for the
 *         applications using the scalar driver. Application requires to include
 *         vps.h, fvid2.h, vps_cfgCsc.h, vps_cfgSc.h and vps_m2m.h files.
 */

#ifndef _VPS_M2MSC_H
#define _VPS_M2MSC_H

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

/**
 *  \brief Scalar ID to be used in the set scalar coefficient and set/get
 *  advance scalar params IOCTLS.
 *  Since this driver supports only one scalar per driver instance, the scalar
 *  could be identified by the instance handle. The below macro is used for
 *  initializing the scalar ID to a default value.
 */
#define VPS_M2M_SC_SCALAR_ID_DEFAULT    (0u)

/** \brief Instance 0 for the scalar memory to memory driver
 *  involving the secondary path 0
 */
#define VPS_M2M_INST_SEC0_SC5_WB2       (0u)

/** \brief Instance 1,2 for the scalar memory to memory driver
 *  involving the bypass path 0
 */
#define VPS_M2M_INST_BP0_SC5_WB2        (1u)

/** \brief Instance 1,2 for the scalar memory to memory driver
 *  involving the bypass path 1
 */
#define VPS_M2M_INST_BP1_SC5_WB2        (2u)

/** \brief Instance 0 for the scalar memory to memory driver
 *  involving the secondary path 0 and VIP0 SC3
 */
#define VPS_M2M_INST_SEC0_SC3_VIP0     (3u)

/** \brief Instance 1 for the scalar memory to memory driver
 *  involving the secondary path 1 and VIP1 SC4
 */
#define VPS_M2M_INST_SEC1_SC4_VIP1     (4u)

/** \brief Total number of the instances for the scalar memory
 * to memory driver involving the secondary path
 */
#define VPS_M2M_SC_INST_MAX         (5u)

/** \brief Maximum number of handles supported by driver  per instance */
#define VPS_M2M_SC_MAX_HANDLE       (4u)

/**
 *  \brief Total number of channels that can be opened by application.
 *  This is total of all handles. If one handle open 5 channels remaining
 *  (VPS_M2M_SC_MAX_CH - 5) channels could be opened by rest of the handles.
 */
#define VPS_M2M_SC_MAX_CH           (64u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * struct Vps_M2mScChParams
 * \brief Parameter structure for each channel or handle
 * depending upon the mode of the memory driver. Each channel
 * will configure this structure if the driver is open in the
 * VPS_M2M_CONFIG_PER_CHANNEL mode else for all the channels
 * same configuration
 * will be applied */
typedef struct
{
    FVID2_Format        inFmt;
    /**< Format parameters for the input frame. This is a mandatory parameter
     *   to be passed by application.
     */
    FVID2_Format        outFmt;
    /**< Format parameters for the output/processed frame.
     *   This is a mandatory parameter to be passed by application.
     */
    Vps_ScConfig        *scCfg;
    /**< Scalar configuration. This parameter can be NULL.
     *   Driver will select the default configuration for the scalar
     */
    Vps_CropConfig      *srcCropCfg;
    /**< Cropping parameters. This parameter can be NULL
     *   Driver will set the startX and StartY field to 0.
     *   While the cropWidth and cropHeight will be set to
     *   input width and input height respectively
     */
    UInt32              inMemType;
    /**< Tiled or non tiled memory for the input buffer.
         For valid values see #Vps_VpdmaMemoryType. */
    UInt32              outMemType;
    /**< Tiled or non tiled memory for the output buffer.
         For valid values see #Vps_VpdmaMemoryType. */
    Vps_SubFrameParams *subFrameParams;
    /**< SubFrame mode configuration parameters. */
} Vps_M2mScChParams;

/**
 * struct Vps_M2mScCreateParams
 * \brief Application interface structure for scalar driver, needs
 * to be passed while opening of the driver.
 */
typedef struct
{
    UInt32 mode;
    /**< Mode to indicate whether configuration is required per handle or
     * per channel of handle. For valid values see #Vps_M2mMode.
     */
    UInt32 numChannels;
    /**< Maximum number of channels to be supported by handle in one
    request */
    Vps_M2mScChParams *chParams;
    /**< Pointer to an array of channel params.  Array will contain only one
     * element if the mode of the handle is configuration per handle
     * else it will contain elements equal to the  number of channels
     * that requires to be supported by handle.
     */
} Vps_M2mScCreateParams;

/**
 *  struct Vps_M2mScRtParams
 *  \brief Run time configuration structure for the Scalar memory
 * to memory driver.  This needs to be passed along with process list to
 * update any run time parameters supported.
 */
typedef struct
{
    Vps_FrameParams     *inFrmPrms;
    /**< Frame params for input frame
     *   Pass NULL if no change is required in input frame params.
     *   Note: When the application changes the input frame parameters and
     *   if the DEI/VIP scalar crop configuration is not provided, the
     *   driver changes the scalar crop configuration as per the new input
     *   resolution i.e. cropWidth/cropHeight will be the new input
     *   width/height and cropStartX/cropStartY will be 0.
     */
    Vps_FrameParams     *outFrmPrms;
    /**< Frame params for output frame
     *   Pass NULL if no change is required in output frame params
     */
    Vps_CropConfig      *srcCropCfg;
    /**< Crop Runtime Configuration.
     *   If inFrmPrms is not NULL this  is NULL then
     *   crop width and cropheight will be set to the inFrmPrms and
     *   cropStartX and cropStartY will be set to 0. Else crop params
     *   will be set according to the configuration provided.
     */
    Vps_ScRtConfig      *scCfg;
    /**< Scaling configuration.
     *   Pass NULL if no change is required in scaling configuration.
     */
} Vps_M2mScRtParams;

/**
 * sturct Vps_M2mScCreateStatus
 * \brief Create status for the M2M SC driver.
*/
typedef struct
{
    Int32   retVal;
    /**< Return value */

    UInt32  maxHandles;
    /**< Maximum number of handles supported by driver
     * instance
     */

    UInt32 maxChannelsPerHandle;
    /**< Maximum number of channels that can be
     * supported per handle
     */

    UInt32 maxSameChPerRequest;
    /**< Maximum number of same channels that can be submitted for processing
     * per request
     */
    UInt32 maxReqInQueue;
    /**< Max number of requests that can be submitted for
     * processing without having to wait for completion. This is shared between
     * all handles of instance.
     */
} Vps_M2mScCreateStatus;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_M2MSC_H */

  /*  @}  */

