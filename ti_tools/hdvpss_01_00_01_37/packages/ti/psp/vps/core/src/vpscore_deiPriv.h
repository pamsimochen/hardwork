/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_deiPriv.h
 *
 *  \brief VPS Core internal header file for DEI HQ and DEI path.
 *
 */

#ifndef _VPSCORE_DEIPRIV_H
#define _VPSCORE_DEIPRIV_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>

#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vpdmaMosaic.h>
#include <ti/psp/vps/hal/vpshal_chrus.h>
#ifdef VPS_HAL_INCLUDE_DRN
#include <ti/psp/vps/hal/vpshal_drn.h>
#endif
#include <ti/psp/vps/hal/vpshal_dei.h>
#ifdef VPS_HAL_INCLUDE_DEIH
#include <ti/psp/vps/hal/vpshal_deih.h>
#endif
#include <ti/psp/vps/hal/vpshal_sc.h>
#ifdef VPS_HAL_INCLUDE_COMPR
#include <ti/psp/vps/hal/vpshal_compr.h>
#endif
#ifdef VPS_HAL_INCLUDE_DCOMPR
#include <ti/psp/vps/hal/vpshal_dcompr.h>
#endif
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_deiPath.h>
#include <ti/psp/platforms/vps_platform.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of windows per row supported in mosaic mode.
 *  This is used to allocate memory and array size statically.
 */
#define VCORE_DEI_MAX_MOSAIC_COL        (VPS_CFG_MAX_MULTI_WIN_COL)

/**
 *  \brief Maximum number of rows supported in mosaic mode.
 *  This is used to allocate memory and array size statically.
 */
#define VCORE_DEI_MAX_MOSAIC_ROW        (VPS_CFG_MAX_MULTI_WIN_ROW)

/**
 *  \brief Maximum number of mosaic windows supported per frame.
 *  This is used to allocate memory and array size statically.
 */
#define VCORE_DEI_MAX_MOSAIC_WINDOW     (VPS_CFG_MAX_MULTI_WIN)

/* \brief Maximum number of mosaic layout supported per channel. */
#define VCORE_DEI_MAX_LAYOUT_PER_CH     (VCORE_DEI_MAX_MULTIWIN_SETTINGS)

/**
 *  \brief Number of descriptors needed per window. This depends whether
 *  the input is 422P or 420T or 422T. Since DEI path supports 420T and 422T,
 *  this is set as 2 as both these formats requires two descriptors.
 */
#define VCORE_DEI_NUM_DESC_PER_WINDOW   (2u)

/**
 *  \brief Maximum number of VPDMA free channel memory allocated - This is one
 *  less than the maximum number of windows supported in a row since according
 *  to VPDMA spec the first window needs to be the actual channel and rest all
 *  is the free channel.
 */
#define VCORE_DEI_MAX_VPDMA_FREE_CH     ((VCORE_DEI_MAX_MOSAIC_COL - 1u) *     \
                       VCORE_DEI_MAX_MOSAIC_ROW * VCORE_DEI_NUM_DESC_PER_WINDOW)

/** \brief Number of VPDMA frame start event regsiter. */
#define VCORE_DEI_MAX_VPDMA_FS_EVT_REG  (VCORE_DEI_NUM_DESC_PER_FIELD *        \
                                         VCORE_DEI_MAX_CHR_US)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief Typedef for DEI path instance object. */
typedef struct Vcore_DeiInstObj_t Vcore_DeiInstObj;

/**
 *  struct Vcore_DeiMosaicInfo
 *  \brief Mosaic layout information.
 */
typedef struct
{
    UInt32                      numExtraDesc;
    /**< Number of extra descriptors needed at the end of the frame i.e. after
         the last mosaic data descriptor. This includes dummy data descriptor,
         SOCH descriptor on actual or last window free channel, abort
         descriptor on actual channel and abort descriptors on free channel of
         last window in all rows. */
    Vcore_DescInfo              descInfo;
    /**< Data descriptor information to be used by driver to allocate
         data and config descriptor memories required for this layout. */
    UInt32                      numFreeCh;
    /**< Number of VPDMA free channels needed for this layout. */
    Vps_MultiWinParams          appMulWinPrms;
    /**< Application supplied multiple window parameters. */
    Vps_WinFormat               appMulWinFmt[VCORE_DEI_MAX_MOSAIC_WINDOW];
    /**< Application supplied multiple window format for each of the windows.
         This is used to initialize the pointer in appMulWinPrms. */
    VpsHal_VpdmaMosaicSplitWinParam mulWinPrms;
    /**< VPDMA utility multiple window parameter. */
    VpsHal_VpdmaMosaicWinFmt    mulWinFmt[VCORE_DEI_MAX_MOSAIC_WINDOW];
    /**< VPDMA utility multiple window format for each split windows.
         This is used to initialize the pointer in mulWinPrms. */
} Vcore_DeiMosaicInfo;

/**
 *  struct Vcore_DeiChObj
 *  \brief Per channel information.
 */
typedef struct
{
    Vcore_States                state;
    /**< Current state. */
    Vcore_DeiParams             corePrms;
    /**< DEI config parameters. */

    Vcore_DeiCtxInfo            ctxInfo;
    /**< DEI context information and state. */

    Vcore_DescInfo              descInfo;
    /**< Data descriptor information to be used by driver to allocate
         data and config descriptor memories required by this core. */
    UInt32                      numExtraDesc;
    /**< Number of extra descriptors needed at the end of the frame i.e. after
         the last mosaic data descriptor. This includes dummy data descriptor,
         SOCH descriptor on actual or last window free channel, abort
         descriptor on actual channel and abort descriptors on free channel of
         last window in all rows. Used in non-mosaic configuration. */
    UInt32                      isDummyNeeded;
    /**< Specifies if an dummy channel would be required. Used in non-mosaic
         configuration. */

    VpsHal_VpdmaFSEvent         fsEvent;
    /**< Frame start event. */

    Vcore_DeiMosaicInfo        *mInfo[VCORE_DEI_MAX_LAYOUT_PER_CH];
    /**< List of created mosaic layout information for the channel.
         Array of pointers is used so that memory for each layout could be
         allocated from the pool on a need basis. */

    UInt32                      isDescReq[VCORE_DEI_MAX_VPDMA_CH];
    /**< Flag to indicate whether a descriptor is required or not. */
    UInt32                      isDescDummy[VCORE_DEI_MAX_VPDMA_CH];
    /**< Flag to indicate whether a descriptor is dummy or not. */

    UInt32                      descOffset[VCORE_DEI_MAX_VPDMA_CH];
    /**< Byte offset to descriptor memory for each of the DEI descriptors. */
    UInt32                      bufIndex[VCORE_DEI_NUM_DESC_PER_WINDOW];
    /**< Buffer index to program the channel address of the VPDMA. */

    VpsHal_ChrusConfig          chrusHalCfg;
    /**< Chroma upsampler HAL configuration. */
#ifdef VPS_HAL_INCLUDE_DRN
    VpsHal_DrnConfig            drnHalCfg;
    /**< DRN HAL configuration. */
#endif
    VpsHal_DeiConfig            deiHalCfg;
    /**< Deinterlacer HAL configuration. */
#ifdef VPS_HAL_INCLUDE_DEIH
    VpsHal_DeihConfig           deihHalCfg;
    /**< Deinterlacer-HQ HAL configuration. */
#endif
    VpsHal_ScConfig             scHalCfg;
    /**< Scalar HAL configuration. */
#ifdef VPS_HAL_INCLUDE_COMPR
    VpsHal_ComprConfig          comprHalCfg[VCORE_DEI_MAX_COMPR];
    /**< Compressor HAL configuration. */
#endif
#ifdef VPS_HAL_INCLUDE_DCOMPR
    VpsHal_DcomprConfig         dcomprHalCfg[VCORE_DEI_MAX_DCOMPR];
    /**< Decompressor HAL configuration. */
#endif
} Vcore_DeiChObj;

/**
 *  struct Vcore_DeiHandleObj
 *  \brief Per handle information.
 */
typedef struct
{
    UInt32                      isUsed;
    /**< Flag to indicate whether the handle object is already used. */
    Vcore_DeiInstObj           *instObj;
    /**< Pointer to the instance object. */
    UInt32                      perChCfg;
    /**< Flag indicating whether format is common for all the channels or
         it is different for each channel. */
    Vcore_DeiChObj             *chObjs[VCORE_DEI_MAX_CHANNELS];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool. */
    UInt32                      numCh;
    /**< Number of channels for this handle. This determines the number of
         valid pointers in channel array. */
} Vcore_DeiHandleObj;

/**
 *  struct Vcore_DeiInstObj_t
 *  \brief Per instance information.
 */
struct Vcore_DeiInstObj_t
{
    UInt32                      instId;
    /**< Instance number/ID. */
    UInt32                      numHandle;
    /**< Number of handles for this instance. This determines the size of
         handle array. */
    UInt32                      isHqDei;
    /**< Flag to indicate whether the instance is DEI HQ or DEI deinterlacer. */
    VpsHal_VpdmaChannel         vpdmaCh[VCORE_DEI_MAX_VPDMA_CH];
    /**< Array to store the VPDMA channel number required by core. */
    VpsHal_Handle               halHandle[VCORE_DEI_MAX_HAL];
    /**< Array to store the HAL handles required by core. */
    VpsHal_VpdmaPath            vpdmaPath;
    /**< VPDMA Path name. */

    UInt32                      openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    Vcore_OpMode                curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/capture or M2M operation of the core. */
    UInt32                      numDescPerWindow;
    /**< Number of descriptors required per window. */

    UInt32                      numFreeCh;
    /**< Number of VPDMA free channels allocated in freeChList array. */
    UInt32                      freeChList[VCORE_DEI_MAX_VPDMA_FREE_CH];
    /**< Array containing the allocated VPDMA free channel numbers to link the
         mosaic windows in a row. Array size is one less as the first
         window in a row should have client's VPDMA channel number.
         Note: Same free channel is used for all channel objects of all
         handles. */

    Vcore_DeiHandleObj          hObjs[VCORE_DEI_MAX_HANDLES];
    /**< Pointer to the handle information. This is a contiguous memory of
         numHandle Vcore_DeiHandleObj structure. */

    UInt32                      halShadowOvlySize[VCORE_DEI_MAX_HAL];
    /**< Array to store the shadow overlay size for each module. */
    VpsHal_GetCfgOvlySize       getCfgOvlySize[VCORE_DEI_MAX_HAL];
    /**< Array to store the overlay size function pointer for each HAL. */
    VpsHal_CreateCfgOvly        createCfgOvly[VCORE_DEI_MAX_HAL];
    /**< Array to store the create overlay function pointer for each HAL. */
    UInt32                      ovlyOffset[VCORE_DEI_MAX_HAL];
    /**< Byte offset to overlay memory for each HAL. */

    UInt32                      numFsEvtReg;
    /**< Number of frame start event registers for this instance. */
    UInt32                      fsCfgOvlySize;
    /**< Size (in bytes) of VPDMA register configuration overlay memory
         required for programming the frame start event. */
    UInt32                     *fsRegOffset[VCORE_DEI_MAX_VPDMA_FS_EVT_REG];
    /**< Array of register offsets for all the frame start registers that
         needs to be programmed through VPDMA. */
    UInt32                      fsVirRegOffset[VCORE_DEI_MAX_VPDMA_FS_EVT_REG];
    /**< Array of virtual register offsets in the VPDMA register configuration
         overlay memory corresponding to fsRegOffset member. */
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/*
 * DEI core common APIs
 */
Int32 vcoreDeiInit(UInt32 numInst, const Vcore_DeiInitParams *initPrms);
Int32 vcoreDeiDeInit(void);

Int32 vcoreDeiCreateLayout(Vcore_DeiInstObj *instObj,
                           Vcore_DeiChObj *chObj,
                           const Vps_MultiWinParams *multiWinPrms,
                           UInt32 *layoutId);
Int32 vcoreDeiDeleteLayout(Vcore_DeiInstObj *instObj,
                           Vcore_DeiChObj *chObj,
                           UInt32 layoutId);
Vcore_DeiMosaicInfo *vcoreDeiGetLayoutObj(const Vcore_DeiChObj *chObj,
                                          UInt32 layoutId);

Vcore_DeiHandleObj *vcoreDeiAllocHandleObj(UInt32 instId, Vcore_OpMode mode);
Int32 vcoreDeiFreeHandleObj(Vcore_DeiHandleObj *hObj);
Int32 vcoreDeiAllocChObjs(Vcore_DeiHandleObj *hObj, UInt32 numCh);
Int32 vcoreDeiFreeChObjs(Vcore_DeiHandleObj *hObj, UInt32 numCh);

Int32 vcoreDeiCheckParams(const Vcore_DeiInstObj *instObj,
                          const Vcore_DeiParams *corePrms);
Int32 vcoreDeiCheckMosaicCfg(const Vps_MultiWinParams *multiWinPrms);


/*
 * DEI core deinterlacer related APIs
 */
UInt32 vcoreDeiProgramDesc(const Vcore_DeiInstObj *instObj,
                           const Vcore_DeiChObj *chObj,
                           const Vcore_DescMem *descMem);
UInt32 vcoreDeiProgramMosaicDesc(const Vcore_DeiInstObj *instObj,
                                 Vcore_DeiChObj *chObj,
                                 UInt32 layoutId,
                                 const Vcore_DescMem *descMem);
UInt32 vcoreDeiProgramPrevFldDesc(const Vcore_DeiInstObj *instObj,
                                  const Vcore_DeiChObj *chObj,
                                  const Vcore_DescMem *descMem);
UInt32 vcoreDeiProgramCurOutDesc(const Vcore_DeiInstObj *instObj,
                                 const Vcore_DeiChObj *chObj,
                                 const Vcore_DescMem *descMem);
UInt32 vcoreDeiProgramMvDesc(const Vcore_DeiInstObj *instObj,
                             const Vcore_DeiChObj *chObj,
                             const Vcore_DescMem *descMem);
UInt32 vcoreDeiProgramMvstmDesc(const Vcore_DeiInstObj *instObj,
                                const Vcore_DeiChObj *chObj,
                                const Vcore_DescMem *descMem);

Int32 vcoreDeiHqFigureOutState(const Vcore_DeiInstObj *instObj,
                               Vcore_DeiChObj *chObj,
                               const Vcore_DeiParams *corePrms);
Int32 vcoreDeiFigureOutState(const Vcore_DeiInstObj *instObj,
                             Vcore_DeiChObj *chObj,
                             const Vcore_DeiParams *corePrms);

Int32 vcoreDeiHqUpdateMode(Vcore_DeiInstObj *instObj,
                           Vcore_DeiChObj *chObj,
                           const Vcore_DescMem *descMem,
                           UInt32 prevFldState);
Int32 vcoreDeiUpdateMode(Vcore_DeiInstObj *instObj,
                         Vcore_DeiChObj *chObj,
                         const Vcore_DescMem *descMem,
                         UInt32 prevFldState);


/*
 * DEI core HAL APIs
 */
void vcoreDeiInitHalParams(Vcore_DeiInstObj *instObj,
                           const Vcore_DeiInitParams *initPrms);
Int32 vcoreDeiSetChParams(const Vcore_DeiInstObj *instObj,
                          Vcore_DeiChObj *chObj,
                          const Vcore_DeiParams *corePrms);
Int32 vcoreDeiSetDescInfo(const Vcore_DeiInstObj *instObj,
                          const Vcore_DeiChObj *chObj,
                          Vcore_DescInfo *descInfo,
                          UInt32 numFirstRowDesc,
                          UInt32 numMultiDesc);
Int32 vcoreDeiSetFsEvent(const Vcore_DeiInstObj *instObj,
                         const Vcore_DeiChObj *chObj,
                         const Vcore_DescMem *descMem);
Int32 vcoreDeiProgramReg(const Vcore_DeiInstObj *instObj,
                         const Vcore_DeiChObj *chObj,
                         const Vcore_DescMem *descMem);
Int32 vcoreDeiProgramFrmSizeReg(const Vcore_DeiInstObj *instObj,
                                const Vcore_DeiChObj *chObj,
                                const Vcore_DescMem *descMem);
Int32 vcoreDeiHqGetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               Vps_DeiHqRdWrAdvCfg *advCfg);
Int32 vcoreDeiHqSetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               const Vps_DeiHqRdWrAdvCfg *advCfg);
Int32 vcoreDeiGetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                             Vps_DeiRdWrAdvCfg *advCfg);
Int32 vcoreDeiSetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                             const Vps_DeiRdWrAdvCfg *advCfg);
Int32 vcoreDeiScGetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               Vcore_ScCfgParams *scCfgPrms);
Int32 vcoreDeiScSetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               const Vcore_ScCfgParams *scCfgPrms);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_DEIPRIV_H */
