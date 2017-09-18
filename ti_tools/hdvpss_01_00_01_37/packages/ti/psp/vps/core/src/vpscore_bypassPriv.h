/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_bypassPriv.h
 *
 *  \brief VPS Core internal header file for bypass path.
 *
 */

#ifndef _VPSCORE_BYPASSPRIV_H
#define _VPSCORE_BYPASSPRIV_H

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
 *  \brief Maximum number of windows per row supported in mosaic mode.
 *  This is used to allocate memory and array size statically.
 */
#define VCORE_BP_MAX_MOSAIC_COL         (VPS_CFG_MAX_MULTI_WIN_COL)

/**
 *  \brief Maximum number of rows supported in mosaic mode.
 *  This is used to allocate memory and array size statically.
 */
#define VCORE_BP_MAX_MOSAIC_ROW         (VPS_CFG_MAX_MULTI_WIN_ROW)

/**
 *  \brief Maximum number of mosaic windows supported per frame.
 *  This is used to allocate memory and array size statically.
 */
#define VCORE_BP_MAX_MOSAIC_WINDOW      (VPS_CFG_MAX_MULTI_WIN)

/* \brief Maximum number of mosaic layout supported per channel. */
#define VCORE_BP_MAX_LAYOUT_PER_CH      (VCORE_BP_MAX_MULTIWIN_SETTINGS)

/**
 *  \brief Number of descriptors needed per window. This depends whether
 *  the input is 422P or 420T or 422T. Since bypass path supports only 422P,
 *  this is set as 1.
 */
#define VCORE_BP_NUM_DESC_PER_WINDOW    (1u)

/**
 *  \brief Maximum number of VPDMA free channel memory allocated - This is one
 *  less than the maximum number of windows supported in a row since according
 *  to VPDMA spec the first window needs to be the actual channel and rest all
 *  is the free channel.
 */
#define VCORE_BP_MAX_VPDMA_FREE_CH      ((VCORE_BP_MAX_MOSAIC_COL - 1u) *      \
                         VCORE_BP_MAX_MOSAIC_ROW * VCORE_BP_NUM_DESC_PER_WINDOW)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/** \brief Typedef for bypass path instance object. */
typedef struct Vcore_BpInstObj_t Vcore_BpInstObj;

/**
 *  struct Vcore_BpMosaicInfo
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
    Vps_WinFormat               appMulWinFmt[VCORE_BP_MAX_MOSAIC_WINDOW];
    /**< Application supplied multiple window format for each of the windows.
         This is used to initialize the pointer in appMulWinPrms. */
    VpsHal_VpdmaMosaicSplitWinParam mulWinPrms;
    /**< VPDMA utility multiple window parameter. */
    VpsHal_VpdmaMosaicWinFmt    mulWinFmt[VCORE_BP_MAX_MOSAIC_WINDOW];
    /**< VPDMA utility multiple window format for each split windows.
         This is used to initialize the pointer in mulWinPrms. */
} Vcore_BpMosaicInfo;

/**
 *  struct Vcore_BpChObj
 *  \brief Per channel information.
 */
typedef struct
{
    Vcore_States                state;
    /**< Current state. */
    Vcore_Format                coreFmt;
    /**< Buffer formats and other formats. */

    Vcore_DescInfo              descInfo;
    /**< Data descriptor information to be used by driver to allocate
         data and config descriptor memories required by this core for
         non-mosaic configuration. */
    UInt32                      numExtraDesc;
    /**< Number of extra descriptors needed at the end of the frame i.e. after
         the last mosaic data descriptor. This includes dummy data descriptor,
         SOCH descriptor on actual or last window free channel, abort
         descriptor on actual channel and abort descriptors on free channel of
         last window in all rows. Used in non-mosaic configuration. */
    UInt32                      isDummyNeeded;
    /**< Specifies if an dummy channel would be required. Used in non-mosaic
         configuration. */

    Vcore_BpMosaicInfo         *mInfo[VCORE_BP_MAX_LAYOUT_PER_CH];
    /**< List of created mosaic layout information for the channel.
         Array of pointers is used so that memory for each layout could be
         allocated from the pool on a need basis. */
} Vcore_BpChObj;

/**
 *  struct Vcore_BpHandleObj
 *  \brief Per handle information.
 */
typedef struct
{
    UInt32                      isUsed;
    /**< Flag to indicate whether the handle object is already used. */
    Vcore_BpInstObj            *instObj;
    /**< Pointer to the instance object. */
    UInt32                      perChCfg;
    /**< Flag indicating whether format is common for all the channels or
         it is different for each channel. */
    Vcore_BpChObj              *chObjs[VCORE_BP_MAX_CHANNELS];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool. */
    UInt32                      numCh;
    /**< Number of channels for this handle. This determines the number of
         valid pointers in channel array. */
} Vcore_BpHandleObj;

/**
 *  struct Vcore_BpInstObj_t
 *  \brief Per instance information.
 */
struct Vcore_BpInstObj_t
{
    UInt32                      instId;
    /**< Instance number/ID. */
    UInt32                      numHandle;
    /**< Number of handles for this instance. This determines the size of
         handle array. */
    VpsHal_VpdmaChannel         vpdmaCh[VCORE_BP_MAX_VPDMA_CH];
    /**< Array to store the VPDMA channel number required by core. */
    VpsHal_VpdmaPath            vpdmaPath;
    /**< VPDMA Path name. */

    UInt32                      openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    Vcore_OpMode                curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/capture or M2M operation of the core. */
    UInt32                      numVpdmaCh;
    /**< Number of vpdma channels supported by this path. */

    UInt32                      numFreeCh;
    /**< Number of VPDMA free channels allocated in freeChList array. */
    UInt32                      freeChList[VCORE_BP_MAX_VPDMA_FREE_CH];
    /**< Array containing the allocated VPDMA free channel numbers to link the
         mosaic windows in a row. Array size is one less as the first
         window in a row should have client's VPDMA channel number.
         Note: Same free channel is used for all channel objects of all
         handles. */

    Vcore_BpHandleObj           hObjs[VCORE_BP_MAX_HANDLES];
    /**< Handle object pool. */
};


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 vcoreBpPrivInit(UInt32 numInst, const Vcore_BpInitParams *initPrms);
Int32 vcoreBpPrivDeInit(void);

Int32 vcoreBpCreateLayout(Vcore_BpInstObj *instObj,
                          Vcore_BpChObj *chObj,
                          const Vps_MultiWinParams *multiWinPrms,
                          UInt32 *layoutId);
Int32 vcoreBpDeleteLayout(Vcore_BpInstObj *instObj,
                          Vcore_BpChObj *chObj,
                          UInt32 layoutId);

UInt32 vcoreBpProgramDesc(const Vcore_BpInstObj *instObj,
                          const Vcore_BpChObj *chObj,
                          const Vcore_DescMem *descMem);
UInt32 vcoreBpProgramMosaicDesc(const Vcore_BpInstObj *instObj,
                                Vcore_BpChObj *chObj,
                                UInt32 layoutId,
                                const Vcore_DescMem *descMem);

Int32 vcoreBpCheckFmt(const Vcore_BpInstObj *instObj,
                      const Vcore_Format *coreFmt);
Int32 vcoreBpCheckMosaicCfg(const Vps_MultiWinParams *multiWinPrms);

Vcore_BpMosaicInfo *vcoreBpGetLayoutObj(const Vcore_BpChObj *chObj,
                                        UInt32 layoutId);

Vcore_BpHandleObj *vcoreBpAllocHandleObj(UInt32 instId, Vcore_OpMode mode);
Int32 vcoreBpFreeHandleObj(Vcore_BpHandleObj *hObj);
Int32 vcoreBpAllocChObjs(Vcore_BpHandleObj *hObj, UInt32 numCh);
Int32 vcoreBpFreeChObjs(Vcore_BpHandleObj *hObj, UInt32 numCh);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_BYPASSPRIV_H */
