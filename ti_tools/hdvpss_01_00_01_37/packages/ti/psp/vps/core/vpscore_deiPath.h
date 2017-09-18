/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_deiPath.h
 *
 *  \brief VPS Core header file for DEI HQ and DEI path.
 *  This core is used by both display as well as in M2M drivers.
 *  In display mode, only one handle could be opened per instance, while in
 *  M2M mode each instance could be opened multiple times.
 *
 */

#ifndef _VPSCORE_DEIPATH_H
#define _VPSCORE_DEIPATH_H

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

/** \brief Instance ID for DEI in the primary main path. */
#define VCORE_DEI_INST_PRI              (0u)
/** \brief Instance ID for DEI in the aux path. */
#define VCORE_DEI_INST_AUX              (1u)
/**
 *  \brief Maximum number of instance. Change this macro accordingly when
 *  number of instance is changed.
 */
#define VCORE_DEI_INST_MAX              (2u)

/** \brief Maximum number of handles supported per DEI path instance. */
#define VCORE_DEI_MAX_HANDLES           (4u)

/**
 *  \brief Maximum number of channels supported for all handles.
 *  When the core is configured in display mode, this is always 1.
 *  This is only used in M2M driver mode where software context makes sense.
 */
#define VCORE_DEI_MAX_CHANNELS          (40u)

/**
 *  \brief Maximum number of multiwindow settings that can be set for an
 *  instance. This is used in dynamic mosaic layout change.
 *  Note: When mosaic layout is supported in M2M mode, this should be
 *  changed to at least a minimum of VCORE_DEI_MAX_CHANNELS count, so that
 *  each channel could get one mosaic configuration memory.
 */
#define VCORE_DEI_MAX_MULTIWIN_SETTINGS (16u)

/**
 *  \brief Number of descriptors for DEI previous and current field output.
 *  One for Y buffer and other for CbCr buffer.
 */
#define VCORE_DEI_NUM_DESC_PER_FIELD    (2u)

/** \brief Luma index to the buffer address array in FVID frame structure. */
#define VCORE_DEI_Y_IDX                 (0u)
/** \brief Chroma index to the buffer address array in FVID frame structure. */
#define VCORE_DEI_CBCR_IDX              (1u)

/*
 *  Note:
 *  Below indices are provided in the same order in which the data
 *  descriptors will be programmed for each and every buffer.
 *  In bound and out bound index could be exchanged - only the order of
 *  the index within inbound or outbound should be maintained the same.
 *  Input luma and chroma indices are kept at the last of the inbound
 *  descriptors so that mosaic descriptors could continue after them.
 */
/*
 *  Caution:
 *  Always have VCORE_DEI_FLD1/2LUMA_IDX and VCORE_DEI_FLD1/2CHROMA_IDX value
 *  next to each other as some of the code involving loop depends on this fact.
 *  Also field 2 luma/chroma index should be next to field 1 luma/chroma.
 */
/** \brief DEI VPDMA channel index for previous N-1 field luma. */
#define VCORE_DEI_FLD1LUMA_IDX          (0u)
/** \brief DEI VPDMA channel index for previous N-1 field chroma. */
#define VCORE_DEI_FLD1CHROMA_IDX        (1u)
/** \brief DEI VPDMA channel index for previous N-2 field luma. */
#define VCORE_DEI_FLD2LUMA_IDX          (2u)
/** \brief DEI VPDMA channel index for previous N-2 field chroma. */
#define VCORE_DEI_FLD2CHROMA_IDX        (3u)
/** \brief DEI VPDMA channel index for previous N-3 field luma. */
#define VCORE_DEI_FLD3LUMA_IDX          (4u)
/** \brief DEI VPDMA channel index for previous N-3 field chroma. */
#define VCORE_DEI_FLD3CHROMA_IDX        (5u)

/** \brief DEI VPDMA channel index for MV N-1 input. */
#define VCORE_DEI_MV1_IDX               (6u)
/** \brief DEI VPDMA channel index for MV N-2 input. */
#define VCORE_DEI_MV2_IDX               (7u)
/** \brief DEI VPDMA channel index for MVSTM N-2 input. */
#define VCORE_DEI_MVSTM_IDX             (8u)

/** \brief DEI VPDMA channel index for current field luma. */
#define VCORE_DEI_FLD0LUMA_IDX          (9u)
/*
 *  Caution:
 *  Always have VCORE_DEI_FLD0LUMA_IDX and VCORE_DEI_FLD0CHROMA_IDX value
 *  next to each other as some of the code involving loop depends on this fact.
 */
/** \brief DEI VPDMA channel index for current field chroma. */
#define VCORE_DEI_FLD0CHROMA_IDX        (10u)

/** \brief DEI VPDMA channel index for top field output luma. */
#define VCORE_DEI_WRLUMA_IDX            (11u)
/** \brief DEI VPDMA channel index for top field output chroma. */
#define VCORE_DEI_WRCHROMA_IDX          (12u)
/*
 * Note: Below four indices are used in progressive TNR mode only.
 */
/** \brief DEI VPDMA channel index for top field output luma. */
#define VCORE_DEI_TOP_WRLUMA_IDX        (11u)
/** \brief DEI VPDMA channel index for top field output chroma. */
#define VCORE_DEI_TOP_WRCHROMA_IDX      (12u)
/** \brief DEI VPDMA channel index for Bottom field output luma. */
#define VCORE_DEI_BOT_WRLUMA_IDX        (13u)
/** \brief DEI VPDMA channel index for Bottom field output chroma. */
#define VCORE_DEI_BOT_WRCHROMA_IDX      (14u)

/** \brief DEI VPDMA channel index for MV output. */
#define VCORE_DEI_MVOUT_IDX             (15u)
/** \brief DEI VPDMA channel index for MVSTM output. */
#define VCORE_DEI_MVSTMOUT_IDX          (16u)

/** \brief Maximum number of VPDMA channels requried by DEI. */
#define VCORE_DEI_MAX_VPDMA_CH          (17u)

/** \brief Maximum number of previous field inputs requried by DEI. */
#define VCORE_DEI_MAX_PREV_FLD          (3u)
/** \brief Maximum number of Output field stored by DEI. */
#define VCORE_DEI_MAX_OUT_FLD           (2u)
/** \brief Maximum number of previous MV inputs requried by DEI. */
#define VCORE_DEI_MAX_MV_IN             (2u)
/** \brief Maximum number of previous MVSTM inputs requried by DEI. */
#define VCORE_DEI_MAX_MVSTM_IN          (1u)
/** \brief Number of compressor module required by DEI. */
#define VCORE_DEI_MAX_COMPR             (VCORE_DEI_MAX_OUT_FLD)
/** \brief Number of decompressor module required by DEI. */
#define VCORE_DEI_MAX_DCOMPR            (VCORE_DEI_MAX_PREV_FLD)
/** \brief Number of CHR_US module required by DEI. */
#define VCORE_DEI_MAX_CHR_US            (3u)

/** \brief Maximum number of in bound VPDMA channels. */
#define VCORE_DEI_MAX_IN_CH             (11u)
/** \brief Maximum number of out bound VPDMA channels. */
#define VCORE_DEI_MAX_OUT_CH            (6u)

/** \brief Start of in bound VPDMA channels. */
#define VCORE_DEI_START_IN_CH           (0u)
/** \brief Start of out bound VPDMA channels. */
#define VCORE_DEI_START_OUT_CH          (11u)

/** \brief DEI index for CHR_US0 HAL. */
#define VCORE_DEI_CHRUS0_IDX            (0u)
/** \brief DEI index for CHR_US1 HAL - used when write client is not there. */
#define VCORE_DEI_CHRUS1_IDX            (1u)
/** \brief DEI index for CHR_US2 HAL - used when write client is not there. */
#define VCORE_DEI_CHRUS2_IDX            (2u)
/** \brief DEI index for DRN HAL. */
#define VCORE_DEI_DRN_IDX               (3u)
/** \brief DEI index for DEI HAL. */
#define VCORE_DEI_DEI_IDX               (4u)
/** \brief DEI index for SC HAL. */
#define VCORE_DEI_SC_IDX                (5u)
/** \brief DEI index for COMPR0 HAL. */
#define VCORE_DEI_COMPR0_IDX            (6u)
/** \brief DEI index for COMPR1 HAL. */
#define VCORE_DEI_COMPR1_IDX            (7u)
/** \brief DEI index for DCOMPR0 HAL. */
#define VCORE_DEI_DCOMPR0_IDX           (8u)
/** \brief DEI index for DCOMPR1 HAL. */
#define VCORE_DEI_DCOMPR1_IDX           (9u)
/** \brief DEI index for DCOMPR2 HAL. */
#define VCORE_DEI_DCOMPR2_IDX           (10u)
/** \brief Maximum number of HAL required by DEI. */
#define VCORE_DEI_MAX_HAL               (11u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_DeiInitParams
 *  \brief Instance specific parameters used at init call.
 */
typedef struct
{
    UInt32                  instId;
    /**< DEI Path Instance. */
    UInt32                  maxHandle;
    /**< Maximum number of handles/channels to be supported per instance. */
    UInt32                  isHqDei;
    /**< Flag to indicate whether the instance is a DEI HQ/DEI deinterlacer. */
    VpsHal_VpdmaChannel     vpdmaCh[VCORE_DEI_MAX_VPDMA_CH];
    /**< Array to pass the VPDMA channel number required by core. */
    VpsHal_Handle           halHandle[VCORE_DEI_MAX_HAL];
    /**< Array to pass the HAL handles required by core. */
    VpsHal_VpdmaPath        vpdmaPath;
    /**< VPDMA Path name. */
} Vcore_DeiInitParams;

/**
 *  struct Vcore_DeiParams
 *  \brief DEI config parameters.
 */
typedef struct
{
    UInt32                  frameWidth;
    /**< Frame width. */
    UInt32                  frameHeight;
    /**< Frame height. */
    UInt32                  startX;
    /**< Horizontal start position in pixels. */
    UInt32                  startY;
    /**< Vertical start position in lines. */
    UInt32                  secScanFmt;
    /** Secondary scan format. For input cores, this represents the scan format
        that needs to be sent out of the core. For output cores, this
        represents the input scan format for that core.
        For valid values see #FVID2_ScanFormat. */
    UInt32                  memType;
    /**< VPDMA Memory type. For valid values see #Vps_VpdmaMemoryType. */

    FVID2_Format            fmt;
    /**< FVID frame format. */
    FVID2_Format            inFmtFldN_1;
    /**< Frame format for the N-1 field input frame to the DEI. This is
         used only in VPS_DEIHQ_CTXMODE_APP_N_1 mode. Otherwise this is
         ignored. */
    UInt32                  deiHqCtxMode;
    /**< Determines the mode in which the DEI HQ context buffers are handled
         by driver and application - all context buffers maintaned by driver,
         or N-1 context buffer provided by application etc...
         For valid values see #Vps_DeiHqCtxMode. */

    UInt32                  drnEnable;
    /**< Enables/disables the DRN module in the DEI path. */
    UInt32                  comprEnable[VCORE_DEI_MAX_COMPR];
    /**< Enable compression for writing back the DEI context to memory. */
    UInt32                  dcomprEnable[VCORE_DEI_MAX_DCOMPR];
    /**< Enable decompression for reading back the DEI context from memory. */
    Vps_DeiHqConfig         deiHqCfg;
    /**< High quality deinterlacer configuration. This parameter will be
         ignored for the DEI_M instance. */
    Vps_DeiConfig           deiCfg;
    /**< Deinterlacer configuration. This parameter will be
         ignored for the DEI_H instance. */
    Vps_ScConfig            scCfg;
    /**< Scalar parameters. */
    Vps_CropConfig          cropCfg;
    /**< Cropping parameters. */
    UInt32                  tarWidth;
    /**< Target image Width. */
    UInt32                  tarHeight;
    /**< Target image Height. */
} Vcore_DeiParams;

/**
 *  struct Vcore_DeiRtParams
 *  \brief DEI runtime configuration parameters.
 */
typedef struct
{
    Vps_FrameParams        *inFrmPrms;
    /**< Frame params for DEI input frame.
         Pass NULL if DEI is active or no change is required. */
    Vps_PosConfig          *posCfg;
    /**< Runtime position configuration containing startX and startY.
         Pass NULL if no change is required. */
    Vps_FrameParams        *outFrmPrms;
    /**< Frame params for DEI writeback output frame.
         Pass NULL if no change is required or this output is not used
         by an instance. */
    Vps_CropConfig         *scCropCfg;
    /**< Scalar crop configuration for DEI scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_ScRtConfig         *scRtCfg;
    /**< Scalar runtime configuration for DEI scalar.
         Pass NULL if no change is required or this scalar is not used
         by an instance. */
    Vps_DeiRtConfig        *deiRtCfg;
    /**< DEI runtime configuration.
         Pass NULL if no change is required. */
    UInt32                  isMosaicMode;
} Vcore_DeiRtParams;

/**
 *  struct Vcore_DeiUpdateContext
 *  \brief DEI context buffer information used to update the descriptors.
 */
typedef struct
{
    Void                   *prevInBuf[VCORE_DEI_MAX_PREV_FLD]
                                     [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< Pointer to the previous field input buffers. */
    UInt32                  prevInFid[VCORE_DEI_MAX_PREV_FLD];
    /**< Corresponding field ID of the previous field input buffers. */
    Void                   *curOutBuf[VCORE_DEI_MAX_OUT_FLD]
                                     [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< Pointer to the current field output buffers. */

    Void                   *mvInBuf[VCORE_DEI_MAX_MV_IN];
    /**< Pointer to the MV input buffers. */
    UInt32                  mvInFid[VCORE_DEI_MAX_MV_IN];
    /**< Corresponding field ID of the MV input buffers. */
    Void                   *mvOutBuf;
    /**< Pointer to the MV output buffer. */

    Void                   *mvstmInBuf[VCORE_DEI_MAX_MVSTM_IN + 1u];
    /**< Pointer to the MVSTM input buffers. */
    UInt32                  mvstmInFid[VCORE_DEI_MAX_MVSTM_IN + 1u];
    /**< Corresponding field ID of the MVSTM input buffers. */
    Void                   *mvstmOutBuf;
    /**< Pointer to the MVSTM output buffer. */

    UInt32                  prevFldState;
    /**< Represents the previous field state - used in DEI reset sequence
         to build up the DEI context and state. */
    UInt32                  dcomprEnable[VCORE_DEI_MAX_DCOMPR];
    /**< Flag to indicate whether each decompressor should be enabled or not. */

    Void                   *curOutWriteDesc[VCORE_DEI_MAX_OUT_FLD]
                                           [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< Pointer to the current output write descriptors, used to get the
         compressed out buffer size from VPDMA. */

    UInt32                  frameSize[VCORE_DEI_MAX_PREV_FLD]
                                     [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< When compressor is enabled, this is used to program the previous
         field compressed frame size. */
    UInt32                  transferSize[VCORE_DEI_MAX_PREV_FLD]
                                        [VCORE_DEI_NUM_DESC_PER_FIELD];
    /**< When compressor is enabled, this is used to program the previous
         field compressed transfer size. */
} Vcore_DeiUpdateContext;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  Vcore_deiInit
 *  \brief DEI core init function.
 *  Initializes DEI core objects, allocates memory etc.
 *  This function should be called before calling any of DEI core API's.
 *
 *  \param numInst      [IN] Number of instance objects to be initialized.
 *  \param initPrms     [IN] Pointer to the init parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInst.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_deiInit(UInt32 numInst,
                    const Vcore_DeiInitParams *initPrms,
                    Ptr arg);

/**
 *  Vcore_deiDeInit
 *  \brief DEI core exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 Vcore_deiDeInit(Ptr arg);

/**
 *  Vcore_deiGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_deiGetCoreOps(void);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_DEIPATH_H */
