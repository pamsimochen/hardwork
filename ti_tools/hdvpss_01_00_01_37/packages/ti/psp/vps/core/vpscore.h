/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore.h
 *
 *  \brief VPS Core common header file.
 *  This file defines the commom interface for the each of the core modules
 *  present in VPS.
 *
 */

#ifndef _VPSCORE_H
#define _VPSCORE_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_sc.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/drivers/display/vps_dctrlInt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of in data descriptors used to allocate the
 *  descriptor pointer array in desc memory structure.
 */
#define VCORE_MAX_IN_DESC               (VPS_CFG_MAX_IN_DESC)

/**
 *  \brief Maximum number of out data descriptors used to allocate the
 *  descriptor pointer array in desc memory structure.
 */
#define VCORE_MAX_OUT_DESC              (VPS_CFG_MAX_OUT_DESC)

/**
 *  \brief Maximum number of multi window data descriptors used to allocate the
 *  descriptor pointer array in desc memory structure.
 */
#define VCORE_MAX_MULTI_WIN_DESC        (VPS_CFG_MAX_MULTI_WIN_DESC)

/**
 *  \brief Maximum number of channels supported in display mode - only one is
 *  supported as multiple channel is valid only in M2M mode.
 */
#define VCORE_MAX_DISPLAY_CH            (1u)

/**
 *  \brief Maximum number of channels supported in capture mode - only one is
 *  supported as multiple channel is valid only in M2M mode.
 */
#define VCORE_MAX_CAPTURE_CH            (1u)

/**
 *  \brief Maximum number VPDMA channels returned in descriptor info structure.
 */
#define VCORE_MAX_VPDMA_CH              (20u)

/**
 *  \brief Default layout ID of the core used for normal
 *  non-multi window operation.
 */
#define VCORE_DEFAULT_LAYOUT_ID         (0u)
/** \brief Start index of valid layout ID. */
#define VCORE_START_VALID_LAYOUT_ID     (1u)
/** \brief Invalid layout ID returned when create fails. */
#define VCORE_INVALID_LAYOUT_ID         (0xFFFFFFFFu)

/**
 *  \brief Typedef for Core driver handle.
 */
typedef Ptr Vcore_Handle;

/** \brief IOCTL base address for the common IOCTLs listed below. */
#define VCORE_COMMON_IOCTL_BASE         (0x00000000u)
/** \brief IOCTL base address for the custom IOCTLs of each cores. */
#define VCORE_CUSTOM_IOCTL_BASE         (0x00000100u)

/** \brief Control command to get DEIHQ configuration. */
#define VCORE_IOCTL_GET_DEIHQ_CFG       (VCORE_COMMON_IOCTL_BASE + 0x0000u)
/** \brief Control command to set DEIHQ configuration. */
#define VCORE_IOCTL_SET_DEIHQ_CFG       (VCORE_COMMON_IOCTL_BASE + 0x0001u)
/** \brief Control command to get DEI configuration. */
#define VCORE_IOCTL_GET_DEI_CFG         (VCORE_COMMON_IOCTL_BASE + 0x0002u)
/** \brief Control command to set DEI configuration. */
#define VCORE_IOCTL_SET_DEI_CFG         (VCORE_COMMON_IOCTL_BASE + 0x0003u)
/** \brief Control command to get SC configuration. */
#define VCORE_IOCTL_GET_SC_CFG          (VCORE_COMMON_IOCTL_BASE + 0x0004u)
/** \brief Control command to set SC configuration. */
#define VCORE_IOCTL_SET_SC_CFG          (VCORE_COMMON_IOCTL_BASE + 0x0005u)

/**
 *  enum Vcore_OpMode
 *  \brief Enumerations for core operational mode.
 *  A specific core can operate either in a display, capture or M2M mode.
 *  The number of descriptors allocated, the behaviour of buffer update etc
 *  will depend on the mode of operation.
 *  For display and capture modes, the number of software channel per core
 *  handle is fixed to 1 - it does not makes sense to run multiple
 *  display/capture driver on the same hardware, isn't it?
 */
typedef enum
{
    VCORE_OPMODE_DISPLAY = 0u,
    /**< Display operation mode. */
    VCORE_OPMODE_CAPTURE,
    /**< Capture operation mode. */
    VCORE_OPMODE_MEMORY,
    /**< Memory to Memory operation mode. */
    VCORE_OPMODE_INVALID
    /**< Invalid mode. This should be the last enum. */
} Vcore_OpMode;

/**
 *  enum Vcore_Name
 *  \brief Enumerations for the core names.
 */
typedef enum
{
    VCORE_TYPE_DEI_HQ = 0u,
    /**< DEI high quality path core. */
    VCORE_TYPE_DEI,
    /**< DEI path core. */
    VCORE_TYPE_DEI_WB,
    /**< DEI HQ/ DEI writeback path core. */
    VCORE_TYPE_GRPX,
    /**< Graphics core. */
    VCORE_TYPE_SEC,
    /**< Secondary path core. */
    VCORE_TYPE_BP,
    /**< Bypass path core. */
    VCORE_TYPE_SC_WB2,
    /**< Scalar-5 writeback path-2 core. */
    VCORE_TYPE_VIP,
    /**< VIP path core. */
    VCORE_TYPE_CSC_CIG
    /**< CSC+CIG path core in display controller Area. */
} Vcore_Name;

/**
 *  enum Vcore_Type
 *  \brief Enumerations for the core types.
 */
typedef enum
{
    VCORE_TYPE_INPUT = 0u,
    /**< Input core. */
    VCORE_TYPE_OUTPUT,
    /**< Output core. */
    VCORE_TYPE_INTERMEDIATE
    /**< Intermediate core - doesn't consume input or output frames. */
} Vcore_Type;


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_Property
 *  \brief Properties of a particular core.
 */
typedef struct
{
    UInt32                  numInstance;
    /**< Number of instance supported by the core - This depends on the number
         of similar hardware paths available. Note: This is different from the
         software channel supported by each core. */
    Vcore_Name              name;
    /**< Core name. */
    Vcore_Type              type;
    /**< Core type. */
    UInt32                  internalContext;
    /**< Flag indicating whether the core has any internal context or not.
         This is used for core like DEI which has to update the internal
         context buffers. */
} Vcore_Property;

/**
 *  struct Vcore_Format
 *  \brief Core format information.
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
} Vcore_Format;

/**
 *  struct Vcore_DescInfo
 *  \brief Data and config descriptor information required by the core.
 *  The values for these fields depends on the parameters set for each core.
 */
typedef struct
{
    UInt32                  numInDataDesc;
    /**< Number of inbound data descriptors required. This also includes the
         first row descriptors required in multiple window mode. */
    UInt32                  numOutDataDesc;
    /**< Number of outbound data descriptors required. */
    UInt32                  numMultiWinDataDesc;
    /**< Number of inbound descriptors required from 2nd to last row in
         multiple window mode. */
    UInt32                  shadowOvlySize;
    /**< Size (in bytes) of the register overlay memory required to program
         shadow registers. */
    UInt32                  nonShadowOvlySize;
    /**< Size (in bytes) of the register overlay memory required to program
         non-shadow registers. */
    UInt32                  horzCoeffOvlySize;
    /**< Size (in bytes) of the coefficient overlay memory required to program
         horizontal scaling coefficients. This is required by cores which have
         scalar in their paths. */
    UInt32                  vertCoeffOvlySize;
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical scaling coefficients. This is required by cores which have
         scalar in their paths. */
    UInt32                  vertBilinearCoeffOvlySize;
    /**< Size (in bytes) of the coefficient overlay memory required to program
         vertical bilinear scaling coefficients. This is required by cores which
         have HQ scalar in their paths. */
    VpsHal_VpdmaConfigDest  coeffConfigDest;
    /**< VPDMA configuration register destination for the above coefficient.
         This will identify the scalar to which the coefficient needs to be
         applied. */
    VpsHal_VpdmaChannel     socChNum[VCORE_MAX_VPDMA_CH];
    /**< Client on which MLM should wait for the end of frame signal. */
    UInt32                  numChannels;
    /**< Number of valid vpdma channels in socChNum array */
} Vcore_DescInfo;

/**
 *  struct Vcore_DescMem
 *  \brief Data descriptor and config overlay memory information needed by the
 *  core to program/update the descriptors and register overlay memory.
 *  The memory for the descriptors and overlay should be allocated by the
 *  display/capture/mem2mem driver. The size of these memories should be
 *  allocated based on the descriptor information structure provided by
 *  the core.
 */
typedef struct
{
    Ptr                     inDataDesc[VCORE_MAX_IN_DESC];
    /**< Inbound data descriptor memory pointers. */
    Ptr                     outDataDesc[VCORE_MAX_OUT_DESC];
    /**< Outbound data descriptor memory pointers. */
    Ptr                     multiWinDataDesc[VCORE_MAX_MULTI_WIN_DESC];
    /**< Multiple window data descriptor memory pointers for 2nd to last row
         descriptors. */
    Ptr                     shadowOvlyMem;
    /**< Shadow register overlay memory pointer. */
    Ptr                     nonShadowOvlyMem;
    /**< Non-Shadow register overlay memory pointer. */
    Ptr                     coeffOvlyMem;
    /**< Coefficient overlay memory pointer. */
} Vcore_DescMem;

/**
 *  struct Vcore_States
 *  \brief Different states of each channel.
 */
typedef struct
{
    UInt32                  isFormatSet;
    /**< Flag to indicate that set format is called for this channel.
         Only after setting the format, the channel information could be
         used by the display/memory driver. */
    UInt32                  isParamsSet;
    /**< Flag to indicate that set params is called for this channel. */
    UInt32                  isMultiWinMode;
    /**< Flag to indicate whether the core is configured for multiple window
         mode. */
} Vcore_States;

/**
 *  struct Vcore_DeiCtxInfo
 *  \brief DEI information and state.
 */
typedef struct
{
    UInt32                  isDeinterlacing;
    /**< Flag to represent whether DEI is deinterlacing or not. */
    UInt32                  isTnrMode;
    /**< Flag to represent whether TNR is enabled or not. */
    UInt32                  numPrevFld;
    /**< Number of previous field input buffers needed. */
    UInt32                  numCurFldOut;
    /**< Number of current field output buffers needed. */
    UInt32                  numMvIn;
    /**< Number of previous MV input buffers needed. */
    UInt32                  numMvOut;
    /**< Number of current MV output buffers needed. */
    UInt32                  numMvstmIn;
    /**< Number of previous MVSTM input buffers needed. */
    UInt32                  numMvstmOut;
    /**< Number of current MVSTM output buffers needed. */
} Vcore_DeiCtxInfo;

/**
 *  Vcore_ScCoeffParams
 *  \brief Structure for programming the scalar coefficients.
 */
typedef struct
{
    UInt32                  hScalingSet;
    /**< Scaling coefficient set for horizontal scaling.
         For valid values see #Vps_ScCoeffSet. */
    UInt32                  vScalingSet;
    /**< Scaling coefficient set for the vertical scaling.
         For valid values see #Vps_ScCoeffSet. */
    Ptr                     horzCoeffMemPtr;
    /**< Pointer to overlay memory for the horizontal scalar coefficients */
    Ptr                     vertCoeffMemPtr;
    /**< Pointer to overlay memory for the vertical scalar coefficients */
    Ptr                     vertBilinearCoeffMemPtr;
    /**< Pointer to overlay memory for the vertical bilinear scalar
         coefficients */
    Ptr                     userCoeffPtr;
    /**< Pointer to the user provided scaling coefficients */
} Vcore_ScCoeffParams;

/**
 *  Vcore_ScCfgParams
 *  \brief Structure for programming the scalar advance configuration.
 */
typedef struct
{
    UInt32                  chNum;
    /**< Channel number for which the configuration is to be set/get.
         Used only in M2M mode, for display and capture mode set
         this to 0. */
    Vps_ScAdvConfig        *scAdvCfg;
    /**< Scalar HAL configuration.  This is for the advanced user. */
    Ptr                     ovlyPtr;
    /**< Overlay pointer for that channel number.
         TODO: Remove this for SWP core. */
    const Vcore_DescMem    *descMem;
    /**< Descriptor memory to update the configuration. */
} Vcore_ScCfgParams;

/** \brief Typedef for core open function pointer. */
typedef Vcore_Handle (*Vcore_Open) (UInt32 instId,
                                    Vcore_OpMode mode,
                                    UInt32 numChannel,
                                    UInt32 perChCfg);

/** \brief Typedef for core close function pointer. */
typedef Int32 (*Vcore_Close) (Vcore_Handle handle);

/** \brief Typedef for core get property function pointer. */
typedef Int32 (*Vcore_GetProperty) (Vcore_Handle handle,
                                    Vcore_Property *property);

/** \brief Typedef for core control functions. */
typedef Int32 (*Vcore_Control) (Vcore_Handle handle, UInt32 cmd, Ptr args);

/** \brief Typedef for core set format function pointer. */
typedef Int32 (*Vcore_SetFormat) (Vcore_Handle handle,
                                  UInt32 chNum,
                                  const Vcore_Format *coreFmt);

/** \brief Typedef for core get format function pointer. */
typedef Int32 (*Vcore_GetFormat) (Vcore_Handle handle,
                                  UInt32 chNum,
                                  Vcore_Format *coreFmt);

/**
 *  \brief Typedef for core set parameters function pointer.
 *  Each core will have its own set of params and will be defined in the
 *  respective core header file.
 */
typedef Int32 (*Vcore_SetParams) (Vcore_Handle handle,
                                  UInt32 chNum,
                                  const Void *params);

/**
 *  \brief Typedef for core get parameters function pointer.
 *  Each core will have its own set of params and will be defined in the
 *  respective core header file.
 */
typedef Int32 (*Vcore_GetParams) (Vcore_Handle handle,
                                  UInt32 chNum,
                                  Void *params);

/** \brief Typedef for core set multiple window config function pointer. */
typedef Int32 (*Vcore_SetMultipleWinCfg) (Vcore_Handle handle,
                                          UInt32 chNum,
                                          const Vps_MultiWinParams *multiWinPrms);

/** \brief Typedef for create layout core function pointer. */
typedef Int32 (*Vcore_CreateLayout) (Vcore_Handle handle,
                                     UInt32 chNum,
                                     const Vps_MultiWinParams *multiWinPrms,
                                     UInt32 *layoutId);

/** \brief Typedef for delete layout core function pointer. */
typedef Int32 (*Vcore_DeleteLayout) (Vcore_Handle handle,
                                     UInt32 chNum,
                                     UInt32 layoutId);

/** \brief Typedef for core get descriptor info function pointer. */
typedef Int32 (*Vcore_GetDescInfo) (Vcore_Handle handle,
                                    UInt32 chNum,
                                    UInt32 layoutId,
                                    Vcore_DescInfo *descInfo);

/** \brief Typedef for core set frame start event function pointer. */
typedef Int32 (*Vcore_SetFsEvent) (Vcore_Handle handle,
                                   UInt32 chNum,
                                   VpsHal_VpdmaFSEvent fsEvent,
                                   const Vcore_DescMem *descMem);

/** \brief Typedef for core program descriptor function pointer. */
typedef Int32 (*Vcore_ProgramDesc) (Vcore_Handle handle,
                                    UInt32 chNum,
                                    UInt32 layoutId,
                                    const Vcore_DescMem *descMem);


/** \brief Typedef for core program register function pointer. */
typedef Int32 (*Vcore_ProgramReg) (Vcore_Handle handle,
                                   UInt32 chNum,
                                   const Vcore_DescMem *descMem);

/** \brief Typedef for core program scalar coefficient function pointer. */
typedef Int32 (*Vcore_ProgramScCoeff) (Vcore_Handle handle,
                                       UInt32 chNum,
                                       Vcore_ScCoeffParams *coeffPrms);

/** \brief Typedef for core update descriptor function pointer. */
typedef Int32 (*Vcore_UpdateDesc) (Vcore_Handle handle,
                                   const Vcore_DescMem *descMem,
                                   const FVID2_Frame *frame,
                                   UInt32 fid);
/** \brief Typedef for core update multi window descriptor function pointer. */
typedef Int32 (*Vcore_UpdateMultiDesc) (Vcore_Handle handle,
                                        UInt32 layoutId,
                                        const Vcore_DescMem *descMem,
                                        const FVID2_FrameList *frameList,
                                        UInt32 fid,
                                        UInt32 startIdx,
                                        UInt32 *frmConsumed);
/** \brief Typedef for core update run time params to desc/overlay memory. */
typedef Int32 (*Vcore_UpdateRtMem) (Vcore_Handle handle,
                                    const Vcore_DescMem *descMem,
                                    UInt32 chNum,
                                    const Void *rtParams,
                                    UInt32  frmIdx);

/** \brief Typedef for core get DEI context info function pointer. */
typedef Int32 (*Vcore_GetDeiCtxInfo) (Vcore_Handle handle,
                                      UInt32 chNum,
                                      Vcore_DeiCtxInfo *ctxInfo);

/** \brief Typedef for core update internal context function pointer. */
typedef Int32 (*Vcore_UpdateContext) (Vcore_Handle handle,
                                      UInt32 chNum,
                                      const Vcore_DescMem *descMem,
                                      const Void *ctx);

/** \brief Typedef for core updating DEI Mode for handling start up
           and reset condtions. */
typedef Int32 (*Vcore_UpdateMode) (Vcore_Handle handle,
                                   UInt32 chNum,
                                   const Vcore_DescMem *cfgDescMem,
                                   UInt32 frmCnt);

/** \brief Typedef for core function to get the scaling factor config for
 *         the specified channel number, and to program registers appropriately.
 */
typedef Int32 (*Vcore_GetScFactorConfig) (Vcore_Handle handle,
                                          UInt32 chNum,
                                          VpsHal_ScFactorConfig *scFactorConfig,
                                          const Vcore_DescMem *descMem);

/**
 *  struct Vcore_Ops
 *  \brief Structure to store core function pointers.
 */
typedef struct
{
    Vcore_GetProperty       getProperty;
    /**< Get property function pointer. */
    Vcore_Open              open;
    /**< Open function pointer. */
    Vcore_Close             close;
    /**< Close function pointer. */
    Vcore_SetFormat         setFormat;
    /**< Set format function pointer. */
    Vcore_GetFormat         getFormat;
    /**< Get format function pointer. */
    Vcore_SetMultipleWinCfg setMultiWinCfg;
    /**< Set mosaic format function pointer. */
    Vcore_CreateLayout      createLayout;
    /**< Create layout function pointer. */
    Vcore_DeleteLayout      deleteLayout;
    /**< Delete layout function pointer. */
    Vcore_GetDescInfo       getDescInfo;
    /**< Get descriptor info function pointer. */
    Vcore_SetFsEvent        setFsEvent;
    /**< Set frame start event function pointer. */
    Vcore_SetParams         setParams;
    /**< Set parameters function pointer. */
    Vcore_GetParams         getParams;
    /**< Get parameters function pointer. */
    Vcore_Control           control;
    /**< Core control function pointer. */
    Vcore_ProgramDesc       programDesc;
    /**< Core program descriptor function pointer. */
    Vcore_ProgramReg        programReg;
    /**< Core program register function pointer. */
    Vcore_UpdateDesc        updateDesc;
    /**< Update descriptor function pointer. */
    Vcore_UpdateMultiDesc   updateMultiDesc;
    /**< Update multiple window descriptor function pointer. */
    Vcore_GetDeiCtxInfo     getDeiCtxInfo;
    /**< Get DEI context info function pointer. */
    Vcore_UpdateContext     updateContext;
    /**< Update context descriptor function pointer. */
    Vcore_UpdateRtMem       updateRtMem;
    /**< Update the descriptor or overlay memory with runtime params */
    Vcore_ProgramScCoeff    programScCoeff;
    /**< Program scalar coefficient funcion pointer. */
    Vcore_UpdateMode        updateMode;
    /**< Update DEI mode as per the frame number */
    Vcore_GetScFactorConfig getScFactorConfig;
    /**< Get the scaling factor config */
} Vcore_Ops;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_H */
