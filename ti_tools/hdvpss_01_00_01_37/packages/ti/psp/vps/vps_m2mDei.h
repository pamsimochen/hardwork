/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \ingroup VPS_DRV_FVID2_M2M_COMMON
 * \defgroup VPS_DRV_FVID2_DEI_API M2M DEI API
 *
 * @{
 */

/**
 *  \file vps_m2mDei.h
 *
 *  \brief Interface specific to DEI memory to memory driver.
 *         Note that interfaces names that includes H or HQ as part of its name
 *         are specific to DEI High Quality and is ignored by driver,
 *         for non-HQ mode of DEI.
 */

#ifndef _VPS_M2MDEI_H
#define _VPS_M2MDEI_H

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

/*
 *  DEI M2M driver.
 */
/* None */

/**
 *  \brief Scalar IDs in memory write back
 *
 *  Expected to be used to get/set scalar parameters
 *  In case of TI816X, it addresses SC1 and SC2 scalars and in case of
 *  TI814X/TI8107, it addresses SC1.
 */
#define VPS_M2M_DEI_SCALAR_ID_DEI_SC    (0u)
/**
 *  \brief Scalar IDs in memory writeback via VIP
 *
 *  In case of TI816X, it addressed SC3 and SC4 scalars and in case of
 *  TI814X/TI8107, it addresses SC3.
 */
#define VPS_M2M_DEI_SCALAR_ID_VIP_SC    (1u)

/**
 *  \brief Maximum number of M2M DEI scalar instances
 */
#define VPS_M2M_DEI_SCALAR_ID_MAX       (2u)

#ifdef TI_816X_BUILD
/*
 *  Macros used to identify different drivers, expected to be passed as the
 *  instance ID, during driver creation.
 *
 *  Note: These are read only macros. Don't modify the value of these macros.
 */
/** \brief DEI HQ WB memory driver instance number. */
#define VPS_M2M_INST_MAIN_DEIH_SC1_WB0          (0u)
/** \brief DEI WB memory driver instance number. */
#define VPS_M2M_INST_AUX_DEI_SC2_WB1            (1u)
/** \brief DEI HQ-VIP0 memory driver instance number. */
#define VPS_M2M_INST_MAIN_DEIH_SC3_VIP0         (2u)
/** \brief DEI VIP1 memory driver instance number. */
#define VPS_M2M_INST_AUX_DEI_SC4_VIP1           (3u)
/** \brief DEI HQ-WB-VIP0 dual output memory driver instance number. */
#define VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 (4u)
/** \brief DEI WB-VIP1 dual output memory driver instance number. */
#define VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1   (5u)

/**
 *  \brief Maximum number of M2M DEI driver instance - three HQ (DEI_HQ-SC,
 *  DEI_HQ-VIP_SC, DEI_HQ-SC_VIP_SC dual paths) and three DEI (DEI_SC,
 *  DEI_VIP_SC, DEI_SC_VIP_SC dual paths) drivers.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_DEI_INST_MAX            (6u)
#endif

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
/*
 *  Macros used to identify different drivers, expected to be passed as the
 *  instance ID, during driver creation.
 *
 *  Note: These are read only macros. Don't modify the value of these macros.
 */
/** \brief DEI WB memory driver instance number. */
#define VPS_M2M_INST_MAIN_DEI_SC1_WB0          (0u)
/** \brief DEI-VIP0 memory driver instance number. */
#define VPS_M2M_INST_MAIN_DEI_SC3_VIP0         (1u)
/** \brief DEI-WB-VIP0 dual output memory driver instance number. */
#define VPS_M2M_INST_MAIN_DEI_SC1_SC3_WB0_VIP0 (2u)
/** \brief SC WB memory driver instance number. */
#define VPS_M2M_INST_AUX_SC2_WB1               (3u)
/** \brief SC-VIP1 memory driver instance number. */
#define VPS_M2M_INST_AUX_SC4_VIP1              (4u)
/** \brief SC-WB1-VIP1 dual output memory driver instance number. */
#define VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1      (5u)

/**
 *  \brief Maximum number of M2M DEI driver instance - three (DEI-SC,
 *  DEI-VIP_SC, DEI-SC_VIP_SC dual paths) and three SC (SC-WB,  SC-VIP,
    SC_WB_VIP_SC dual paths ) drivers.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_DEI_INST_MAX            (6u)
#endif

/**
 *  \brief Maximum number of handles supported by M2M DEI driver per
 *  driver instance.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_DEI_MAX_HANDLE_PER_INST (4u)

/**
 *  \brief Maximum number of channels that could be allocated per instance.
 *  This macro is used in allocation of memory pool objects and
 *  is assumed that if more that one handle is opened per instance, then the
 *  channel memory will be shared across handles.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_DEI_MAX_CH_PER_INST     (40u)

/**
 *  \brief Maximum number of channels that could be allocated per handle.
 *  The total number of channels that could be allocated for all the handles
 *  for an instance should be less than VPS_M2M_DEI_MAX_CH_PER_INST.
 *  Note: This is a read only macro. Don't modify the value of this macro.
 */
#define VPS_M2M_DEI_MAX_CH_PER_HANDLE   (20u)

/**
  \addtogroup VPS_DRV_FVID2_IOCTL_M2M_DEI
  @{
*/

/**
 *  \brief M2M DEI IOCTL to replace the internal state of the previous
 *  field buffers.
 *
 *  This IOCTL could be used by the application to control the previous fields
 *  used to deinterlace the next request. This will be useful if the application
 *  wants to operate with lower FPS and still wants to perform proper
 *  deinterlacing operation.
 *
 *  The driver will return the FVID2 frames held by the driver in the same
 *  input structure.
 *
 *  When using this IOCTL the application should ensure that there is no
 *  input request pending with the driver. Otherwise the driver will return
 *  error.
 *
 *  This is valid only for TI814x/TI8107 platform where the input frame acts
 *  as a previous field buffers in the subsequent frames.
 *
 *  \param cmdArgs       [IN/OUT]   Vps_M2mDeiOverridePrevFldBuf *
 *  \param cmdStatusArgs [OUT]      NULL
 *
 *  \return FVID_SOK on success, else failure
 *
 */
#define IOCTL_VPS_DEI_OVERRIDE_PREV_FLD_BUF (VPS_M2M_DEI_IOCTL_BASE + 0x0000u)

/* @} */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vps_M2mDeiChParams
 *  \brief DEI M2M channel parameter structure to be passed to the
 *  driver at the time of mem to mem driver create call.
 */
typedef struct
{
    FVID2_Format            inFmt;
    /**< Frame format for the input frame to the DEI. */
    FVID2_Format           *outFmtDei;
    /**< Frame format for the output frame from DEI-Scalar output. */
    FVID2_Format           *outFmtVip;
    /**< Frame format for the output frame from DEI-VIP output. */
    FVID2_Format           *inFmtFldN_1;
    /**< Frame format for the N-1 field input frame to the DEI. This is
         used only in VPS_DEIHQ_CTXMODE_APP_N_1 mode. Otherwise this should
         be set to NULL.
         This member is not used for TI814X/TI8107 platforms. */

    UInt32                  inMemType;
    /**< VPDMA Memory type for the DEI input frame.
         For valid values see #Vps_VpdmaMemoryType. */
    UInt32                  outMemTypeDei;
    /**< VPDMA Memory type for the DEI-Scalar output frame.
         For valid values see #Vps_VpdmaMemoryType. */
    UInt32                  outMemTypeVip;
    /**< VPDMA Memory type for the DEI-VIP output frame.
         For valid values see #Vps_VpdmaMemoryType. */

    UInt32                  drnEnable;
    /**< Enables/disables the DRN module in the DEI path. DRN is not available
         in TI814X/TI8107 platforms, hence this is not used for TI814X/TI8107
         platforms. */
    UInt32                  comprEnable;
    /**< Enable compression/decompression for writing back the DEI context
         to and from DDR. COMPR is not available in TI814X/TI8107 platform,
         hence this is not used for above platforms. And for TI816X, this
         feature is not supported. Hence this flag should be always set to
         FALSE. */

    Vps_DeiHqConfig        *deiHqCfg;
    /**< Pointer to the high quality deinterlacer configuration used for
      *  DEI HQ drivers. This parameter should be set to NULL for others. */
    Vps_DeiConfig          *deiCfg;
    /**< Pointer to the deinterlacer configuration used for
      *  DEI drivers. This parameter should be set to NULL for
      *  DEI HQ drivers. */

    Vps_ScConfig           *scCfg;
    /**< Pointer to the scalar parameters like crop and scaler type for the
         scalar in DEI path. */
    Vps_ScConfig           *vipScCfg;
    /**< Pointer to the scalar parameters like crop and scaler type for the
         VIP scalar. */
    Vps_CropConfig         *deiCropCfg;
    /**< Pointer to the cropping configuration for the DEI scalar. */
    Vps_CropConfig         *vipCropCfg;
    /**< Pointer to the cropping configuration for the VIP scalar. */
    Vps_SubFrameParams     *subFrameParams;
    /**< SubFrame mode parameters. */
} Vps_M2mDeiChParams;

/**
 *  struct Vps_M2mDeiCreateParams
 *  \brief DEI M2M create parameter structure to be passed to the
 *  driver at the time of mem to mem driver create call.
 */
typedef struct
{
    UInt32                  mode;
    /**< Mode to indicate whether configuration is required per handle or
         per channel. For valid values see #Vps_M2mMode. */
    UInt32                  numCh;
    /**< Number of channels to be supported by handle. */

    UInt32                  deiHqCtxMode;
    /**< Determines the mode in which the DEI HQ context buffers are handled
         by driver and application - all context buffers maintaned by driver,
         or N-1 context buffer provided by application etc...
         For valid values see #Vps_DeiHqCtxMode.
         This parameter is ignored for others. */

    const Vps_M2mDeiChParams   *chParams;
    /**< Pointer to an array of channel params.  Array will contain only one
         element if the mode of the handle is configuration per handle
         else it will contain elements equal to the  number of channels
         that requires to be supported by a handle. */
    UInt32                  isVipScReq;
    /**< Flag to indicate whether VIP scalar is needed or not. Used only when
         VIP is used to get the output. Possible values are TRUE/FALSE. */
} Vps_M2mDeiCreateParams;

/**
 *  sturct Vps_M2mDeiCreateStatus
 *  \brief Create status for the M2M DEI create call. This should be
 *  passed to the create function as a create status argument while creating
 *  the driver.
 */
typedef struct
{
    Int32                   retVal;
    /**< Return value of create call. */
    UInt32                  maxHandles;
    /**< Maximum number of handles supported by driver per instance. */
    UInt32                  maxChPerHandle;
    /**< Maximum number of channels that can be supported/submitted per
         handle. Note that the the maximum number of channels will be
         shared between handles if more than one handle is opened.
         This is to avoid unnecessary wastage memory allocation. */
    UInt32                  maxSameChPerRequest;
    /**< Maximum number of same channels that can be submitted for processing
         per request. */
    UInt32                  maxReqInQueue;
    /**< Maximum number of request per driver handle that can be submitted
         for processing without having to dequeue the completed requests. */
} Vps_M2mDeiCreateStatus;

/**
 *  struct Vps_M2mDeiRtParams
 *  \brief Run time configuration structure for the DEI memory to memory
 *  driver. This needs to be passed along with process list to
 *  update any supported run time parameters.
 */
typedef struct
{
    Vps_FrameParams        *deiInFrmPrms;
    /**< Frame params for DEI input frame.
         Pass NULL if compressor is enabled or no change is required.

         When in deinterlacing mode or in progressive TNR mode, the application
         has to allocate bigger context buffer memory depending on the
         max input resolution it might change. Driver has no way of
         validating the actual allocated size and hence this might lead to
         buffer over flow in case max size is not allocated.
         Also at the time of input resolution change 2-3 frames after this
         will be invalid because of invalid context buffer content. And
         hence application should ignore these frames.
         Refer #Vps_DeiCtxInfo structure documentation on how to calculate
         the context buffer sizes for a given width and height.

         This runtime parameter can't be used when compressor is enabled.

         Note: When the application changes the input frame parameters and
         if the DEI/VIP scalar crop configuration is not provided, the
         driver changes the scalar crop configuration as per the new input
         resolution i.e. cropWidth/cropHeight will be the new input
         width/height and cropStartX/cropStartY will be 0. */

    Vps_FrameParams        *deiOutFrmPrms;
    /**< Frame params for DEI writeback output frame.
         Pass NULL if no change is required or this output is not used
         by an instance. */
    Vps_FrameParams        *vipOutFrmPrms;
    /**< Frame params for VIP output frame.
         Pass NULL if no change is required or this output is not used
         by an instance. */

    Vps_CropConfig         *deiScCropCfg;
    /**< Scalar crop configuration for DEI scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_CropConfig         *vipScCropCfg;
    /**< Scalar crop configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */

    Vps_ScRtConfig         *deiScRtCfg;
    /**< Scalar runtime configuration for DEI scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_ScRtConfig         *vipScRtCfg;
    /**< Scalar runtime configuration for VIP scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */

    Vps_DeiRtConfig        *deiRtCfg;
    /**< DEI runtime configuration.
         Pass NULL if no change is required. */
} Vps_M2mDeiRtParams;

/**
 *  struct Vps_M2mDeiOverridePrevFldBuf
 *  \brief Structure used to override the previous field context buffers.
 */
typedef struct
{
    FVID2_FrameList        *fldBufFrameList[VPS_DEI_MAX_CTX_FLD_BUF];
    /**< [IN] FVID2 frame list containing the previous field buffers for the
         requried channels.

         channelNum in FVID2_Frame will be used to update the corresponding
         channel field buffers in driver objects.

         numFrames in all the FVID2_FrameList should be same and less than
         numCh created during FVID2_create.

         N-1 field buffer is at index 0, N-2 at index 1 and so on. */
    UInt32                  numFldBufLists;
    /**< [IN] Number of previous field buffer frame lists. In case of
         TI814x/TI807 this should be set to 2 as the DEI IP in these
         platforms takes in only N-1 and N-2 inputs. */
} Vps_M2mDeiOverridePrevFldBuf;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPS_M2MDEI_H */

  /*  @}  */
