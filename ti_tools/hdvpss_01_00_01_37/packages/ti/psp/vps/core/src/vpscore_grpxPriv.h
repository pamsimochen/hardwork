/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_grpxPriv.h
 *
 *  \brief VPS Core internal header file for GRPX path.
 *
 */

#ifndef _VPSCORE_GRPXPRIV_H
#define _VPSCORE_GRPXPRIV_H

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/psp/platforms/vps_platform.h>
/* None */
#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================================== */
/*                           Macros & Typedefs                                 */
/* =========================================================================== */

/**
 *  \brief Maximum number of handles/channels supported per graphics path
 *   instance.
 */
#define VCORE_GRPX_MAX_HANDLES              (4u)

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *   the instances. Each of the channels in an instance is assumed to be
 *   shared across all handles of that instance.
 *   All the handles will allocate channel objects from this pool only.
 */
#define VCORE_GRPX_MAX_CHANNEL_OBJECTS      (VCORE_GRPX_MAX_CHANNELS * \
                                            VCORE_GRPX_NUM_INST )

/**
 *  \brief Maximum region in one frame
 */
#define VCORE_GRPX_MAX_REGIONS              (VPS_GRPX_MAX_REGIONS_PER_FRAME)

/**
 *  \brief Time out to be used in sem pend
 */
#define VCORE_GRPX_SEM_TIMEOUT              (BIOS_WAIT_FOREVER)

/**
 *  \brief Default width used for initializing format structure.
 */
#define VCORE_GRPX_DEFAULT_WIDTH            (720u)

/**
 *  \brief Default height used for initializing format structure.
 */
#define VCORE_GRPX_DEFAULT_HEIGHT           (480u)

/**
 *  \brief Maximum number of Data descriptor required by this core
 *   one for stenciling and one for data.
 */
#define VCORE_GRPX_DESCS_PER_REGION        (2u)

/**
 *  \brief Maximum number of inBound Data descriptor required by this core
 *   one for clut, one for stenciling and one for data.
 */
#define VCORE_GRPX_INBOUND_DESC_PER_REGION  (1u + VCORE_GRPX_DESCS_PER_REGION)

/**
 *  \brief Index of stenciling pitch in the pitch[] structure
 */
#define VCORE_GRPX_STEN_PITCH_INDEX            (2u)

/**
 * \brief   MAX SUPPORT FORMAT BY GRPX
*/
#define VPSDDRV_GRPX_MAX_FORMAT            (sizeof(VpsDdrDataTable) /   \
                                                sizeof(vpsDdrvDataFormatTable))

/**
 *  structure vpsDdrvDataFormatTable
 *  \brief this structure to hold the FVID2_bitsPerPixel,
 *   vpdma channel and bits per pixel of each data format
 *   supported by GRPX
 */
typedef struct
{
    UInt32              dataFormat;
    /**< FVID2 data format*/
    UInt32              fvid2_bpp;
    /**< FVID2 BitsPerPixel*/
    VpsHal_VpdmaChanDT  vpmdaChanType;
    /**< VPDAM channel data type*/
    int              bitsPerPixel;
    /**< real bits per pixel*/
}vpsDdrvDataFormatTable;

/**
 *  \brief Typedef for graphics path instance object.
 */
typedef struct Vcore_GrpxInstObj_t Vcore_GrpxInstObj;


/**
 *  struct Vcore_GrpxMulRegionsInfo
 *  \brief Structure containing multiple regions information
 */
typedef struct
{
    UInt32                   numRegions;
    /**< number of regions in one frame*/
    Vps_WinFormat            regFmt[VCORE_GRPX_MAX_REGIONS];
    /**< regions format */
    Vps_GrpxScParams         scParams[VCORE_GRPX_MAX_REGIONS];
    /**< scaling configuration, current is no sued */
    Vps_GrpxRegionParams     regParam[VCORE_GRPX_MAX_REGIONS];
    /**< storet the region attributes */
    Ptr                      stenData[VCORE_GRPX_MAX_REGIONS];
    /**< stenciling data pointer */
} Vcore_GrpxMulRegionsInfo;

/**
 *  struct Vcore_GrpxChannelObj
 *  \brief containing per channel information.
 */
typedef struct
{
    Vcore_States             state;
    /**< Current state. */
    Vcore_Format             coreFmt;
    /**< Buffer formats and other formats. */
    Vcore_DescInfo           descInfo;
    /**< Data descriptor information to be used by driver to allocate
         data and config descriptor memories required by this core. */
    Vps_GrpxScParams         scParams;
    /**< scaling configuration */
    Vps_GrpxRegionParams     regParam;
    /**< storet he region attributes */
    Ptr                      clutData;
    /**<CLUT data pointer */
    Ptr                      stenData;
    /**< stenciling data pointer */
    Vcore_GrpxMulRegionsInfo mInfo;
    /**< region based graphics information*/
} Vcore_GrpxChObj;


/**
 *  struct Vcore_GrpxHandleObj
 *  \brief containing per handle information.
 */
typedef struct
{
    Vcore_GrpxInstObj        *instObj;
    /**< Pointer to the instance object. */
    UInt32                   perChCfg;
    /**< Flag indicating whether format is common for all the channels or
         it is different for each channel. */
    Vcore_GrpxChObj          handleContext;
    /**< Used when the handle is opened with perChCfg as false. */
    Vcore_GrpxChObj          *chObjs[VCORE_GRPX_MAX_CHANNELS];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool */
    UInt32                   numCh;
    /**< Number of channels for this handle. This determines the number of
         valid pointers in above channel array. */
} Vcore_GrpxHandleObj;

/**
 *  struct Vcore_GrpxInstObj_t
 *  \brief containing per instance information.
 */
struct Vcore_GrpxInstObj_t
{
    UInt32                   instId;
    /**< Instance number/ID. */
    UInt32                   openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    VpsHal_VpdmaChannel      chGrpx;
    /**< VPDMA Channel for Graphics data */
    VpsHal_VpdmaChannel      chSten;
    /**< VPDMA Channel for stenciling data. */
    VpsHal_VpdmaChannel      chClut;
    /**< VPDMA channel for CLUT data. */
    VpsHal_VpdmaConfigDest   configDest;
    /**< desctination of the config descriptor */
    Vcore_OpMode             curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/mem-mem operation of the core. */
    Vcore_GrpxHandleObj      hObjs[VCORE_GRPX_MAX_HANDLES];
    /**< Pointer to the handle information. This is a contiguous memory of
         numHandle Vcore_GrpxHandleObj structure */
    UInt32                   hObjsFlag[VCORE_GRPX_MAX_HANDLES];
    /**< Flag to indicate whether the handle object is already allocated or not. */
    VpsUtils_PoolParams      hObjsPoolParams;
    /**< pool parameter for the handle objects*/
    UInt32                   numHandle;
    /**< Number of channel for this instance. This determines the size of
         channel array. */
    UInt32                   numDescPerReg;
    /**< Number of descriptors required per region. */
    VpsHal_GrpxHandle        grpxHandle;
    /**< handle to the GRPX HAL*/
    void                     *demHandle;
    /**< handle to event registered to event manager*/
    UInt32                   grpxReset;
    /**< reset flag*/
    VpsHal_VpdmaPath          ptGrpx;
    /*VPDMA GRPX Data path name*/
    VpsHal_VpdmaPath          ptSten;
    /*VPDMA GRPX stenciling data path name*/

};
/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/*
 *  This table is store the FVID2_bitsPerPixel, VPDMA channel
 *  data type and real bits per pixel of each data format supported
 *  by GRPX
*/
static vpsDdrvDataFormatTable VpsDdrDataTable[] =
{
    {FVID2_DF_RGB16_565, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_RGB565,      16u},
    {FVID2_DF_ARGB16_1555, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_ARGB1555,     16u},
    {FVID2_DF_RGBA16_5551, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_RGBA5551,       16u},
    {FVID2_DF_ARGB16_4444, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_ARGB4444,       16u},
    {FVID2_DF_RGBA16_4444, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_RGBA4444,       16u},
    {FVID2_DF_ARGB24_6666, FVID2_BPP_BITS24,
        VPSHAL_VPDMA_CHANDT_ARGB6666,       24u},
    {FVID2_DF_RGBA24_6666, FVID2_BPP_BITS24,
        VPSHAL_VPDMA_CHANDT_RGBA6666,       24u},
    {FVID2_DF_RGB24_888, FVID2_BPP_BITS24,
        VPSHAL_VPDMA_CHANDT_RGB888,         24u},
    {FVID2_DF_ARGB32_8888, FVID2_BPP_BITS32,
        VPSHAL_VPDMA_CHANDT_ARGB8888,       32u},
    {FVID2_DF_RGBA32_8888, FVID2_BPP_BITS32,
        VPSHAL_VPDMA_CHANDT_RGBA8888,       32u},
    {FVID2_DF_BITMAP8, FVID2_BPP_BITS8,
        VPSHAL_VPDMA_CHANDT_BITMAP8,        8u},
    {FVID2_DF_BITMAP4_LOWER, FVID2_BPP_BITS4,
        VPSHAL_VPDMA_CHANDT_BITMAP4_LOWER,   4u},
    {FVID2_DF_BITMAP4_UPPER, FVID2_BPP_BITS4,
        VPSHAL_VPDMA_CHANDT_BITMAP4_UPPER,   4u},
    {FVID2_DF_BITMAP2_OFFSET0, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET0, 2u},
    {FVID2_DF_BITMAP2_OFFSET1, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET1, 2u},
    {FVID2_DF_BITMAP2_OFFSET2, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET2, 2u},
    {FVID2_DF_BITMAP2_OFFSET3, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET3, 2u},
    {FVID2_DF_BITMAP1_OFFSET0, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET0, 1u},
    {FVID2_DF_BITMAP1_OFFSET1, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET1, 1u},
    {FVID2_DF_BITMAP1_OFFSET2, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET2, 1u},
    {FVID2_DF_BITMAP1_OFFSET3, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET3, 1u},
    {FVID2_DF_BITMAP1_OFFSET4, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET4, 1u},
    {FVID2_DF_BITMAP1_OFFSET5, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET5, 1u},
    {FVID2_DF_BITMAP1_OFFSET6, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET6, 1u},
    {FVID2_DF_BITMAP1_OFFSET7, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET7, 1u},

    /*PG2 data format*/
    {FVID2_DF_BGR16_565, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_BGR565, 16u},
    {FVID2_DF_ABGR16_1555, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_ABGR1555, 16u},
    {FVID2_DF_ABGR16_4444, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_ABGR4444, 16u},
    {FVID2_DF_BGRA16_5551, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_BGRA5551, 16u},
    {FVID2_DF_BGRA16_4444, FVID2_BPP_BITS16,
        VPSHAL_VPDMA_CHANDT_BGRA4444, 16u},
    {FVID2_DF_ABGR24_6666, FVID2_BPP_BITS24,
        VPSHAL_VPDMA_CHANDT_ABGR6666, 24u},
    {FVID2_DF_BGR24_888, FVID2_BPP_BITS24,
        VPSHAL_VPDMA_CHANDT_BGR888, 24u},
    {FVID2_DF_ABGR32_8888, FVID2_BPP_BITS32,
        VPSHAL_VPDMA_CHANDT_ABGR8888, 32u},
    {FVID2_DF_BGRA24_6666, FVID2_BPP_BITS24,
        VPSHAL_VPDMA_CHANDT_BGRA6666, 24u},
    {FVID2_DF_BGRA32_8888, FVID2_BPP_BITS32,
        VPSHAL_VPDMA_CHANDT_BGRA8888, 32u},
    {FVID2_DF_BITMAP8_BGRA32, FVID2_BPP_BITS8,
        VPSHAL_VPDMA_CHANDT_BITMAP8_BGRA32, 8u},
    {FVID2_DF_BITMAP4_BGRA32_LOWER, FVID2_BPP_BITS4,
        VPSHAL_VPDMA_CHANDT_BITMAP4_LOWER_BGRA32, 4u},
    {FVID2_DF_BITMAP4_BGRA32_UPPER, FVID2_BPP_BITS4,
        VPSHAL_VPDMA_CHANDT_BITMAP4_UPPER_BGRA32, 4u},
    {FVID2_DF_BITMAP2_BGRA32_OFFSET0, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET0_BGRA32, 2u},
    {FVID2_DF_BITMAP2_BGRA32_OFFSET1, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET1_BGRA32, 2u},
    {FVID2_DF_BITMAP2_BGRA32_OFFSET2, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET2_BGRA32, 2u},
    {FVID2_DF_BITMAP2_BGRA32_OFFSET3, FVID2_BPP_BITS2,
        VPSHAL_VPDMA_CHANDT_BITMAP2_OFFSET3_BGRA32, 2u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET0, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET0_BGRA32, 1u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET1, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET1_BGRA32, 1u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET2, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET2_BGRA32, 1u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET3, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET3_BGRA32, 1u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET4, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET4_BGRA32, 1u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET5, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET5_BGRA32, 1u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET6, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET6_BGRA32, 1u},
    {FVID2_DF_BITMAP1_BGRA32_OFFSET7, FVID2_BPP_BITS1,
        VPSHAL_VPDMA_CHANDT_BITMAP1_OFFSET7_BGRA32, 1u},

};
/* ========================================================================== */
/*                          inLine Function                             */
/* ========================================================================== */
/**
 *  vcoreGrpxIsPG2DataFormat
 *  is this PG2 data format
 */

static inline Int32 vcoreGrpxIsPG2DataFormat(UInt32 dataFormat)
{
    Int32 retVal = FALSE;
    switch (dataFormat)
    {
        case    FVID2_DF_BGR16_565:
        case    FVID2_DF_ABGR16_1555:
        case    FVID2_DF_ABGR16_4444:
        case    FVID2_DF_BGRA16_5551:
        case    FVID2_DF_BGRA16_4444:
        case    FVID2_DF_ABGR24_6666:
        case    FVID2_DF_BGR24_888:
        case    FVID2_DF_ABGR32_8888:
        case    FVID2_DF_BGRA24_6666:
        case    FVID2_DF_BGRA32_8888:
        case    FVID2_DF_BITMAP8_BGRA32:
        case    FVID2_DF_BITMAP4_BGRA32_LOWER:
        case    FVID2_DF_BITMAP4_BGRA32_UPPER:
        case    FVID2_DF_BITMAP2_BGRA32_OFFSET0:
        case    FVID2_DF_BITMAP2_BGRA32_OFFSET1:
        case    FVID2_DF_BITMAP2_BGRA32_OFFSET2:
        case    FVID2_DF_BITMAP2_BGRA32_OFFSET3:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET0:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET1:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET2:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET3:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET4:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET5:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET6:
        case    FVID2_DF_BITMAP1_BGRA32_OFFSET7:
            retVal = TRUE;
            break;
    }
    return retVal;
}

/**
 *  vcoreGrpxCheckDataFormat
 *  is this a vlid grpx data format
 */

static inline Int32 vcoreGrpxCheckDataFormat(UInt32 dataFormat)
{
    int i;
    vpsDdrvDataFormatTable  *table;

    /* PG2.0 data format can not used on PG1.0 or PG1.1 */
    if ((Vps_platformGetId() != VPS_PLATFORM_ID_EVM_TI8107) &&
    	(Vps_platformGetCpuRev() <= VPS_PLATFORM_CPU_REV_1_1) &&
        (TRUE == vcoreGrpxIsPG2DataFormat(dataFormat)))
    {
            return VPS_EINVALID_PARAMS;
    }

    for (i = 0u; i < VPSDDRV_GRPX_MAX_FORMAT; i++)
    {
        table = &VpsDdrDataTable[i];
        if (dataFormat == table->dataFormat)
        {
            return (VPS_SOK);
        }
    }

    return (VPS_EINVALID_PARAMS);

}

/**
 *  vcoreGrpxGetVpdmaChanDataType
 *  get VPDMA data channel type from data format
 * vcoreGrpxCheckDataFormat should be called before this function
 */
static inline VpsHal_VpdmaChanDT vcoreGrpxGetVpdmaChanDataType(
                        UInt32 dataFormat)
{
    int i;
    vpsDdrvDataFormatTable  *table;

    for (i = 0u; i < VPSDDRV_GRPX_MAX_FORMAT; i++)
    {
        table = &VpsDdrDataTable[i];
        if (dataFormat == table->dataFormat)
        {
            return table->vpmdaChanType;
        }
    }

    return (VPSHAL_VPDMA_CHANDT_INVALID);
}

/**
 *  vcoreGrpxGetNumBitsPerPixel
 *  get number bits per pixels
 * vcoreGrpxCheckDataFormat should be called before this function
 */
static inline UInt32 vcoreGrpxGetNumBitsPerPixel(UInt32 dataFormat)
{
    int i;
    vpsDdrvDataFormatTable  *table;

    for (i = 0u; i < VPSDDRV_GRPX_MAX_FORMAT; i++)
    {
        table = &VpsDdrDataTable[i];
        if (dataFormat == table->dataFormat)
        {
            return table->bitsPerPixel;
        }
    }

    return 0;
}

/**
 *  vcoreGrpxGetFvid2Bpp
 *  get FVID2_BitsPerPixel from data format
 * vcoreGrpxCheckDataFormat should be called before this function
 */

static inline UInt32 vcoreGrpxGetFvid2Bpp(UInt32 dataFormat)
{
    int i;
    vpsDdrvDataFormatTable  *table;

    for (i = 0u; i < VPSDDRV_GRPX_MAX_FORMAT; i++)
    {
        table = &VpsDdrDataTable[i];
        if (dataFormat == table->dataFormat)
        {
            return table->fvid2_bpp;
        }
    }
    return (FVID2_BPP_MAX);
}

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int vcoreGrpxPrivInit(UInt32 numInst, const Vcore_GrpxInitParams *initParams);

Int vcoreGrpxPrivDeinit(void);
Vcore_GrpxHandleObj *vcoreGrpxAllocHandleObj(UInt32 instId,
                                             Vcore_OpMode mode);

Int32 vcoreGrpxFreeHandleObj(Vcore_GrpxHandleObj *hObj);

void vcoreGrpxSetDefaultCh(Vcore_GrpxChObj *chObj,
                           UInt32 chNum);
void vcoreGrpxSetDefaultMuliRegInfo(Vcore_GrpxMulRegionsInfo *mInfo);

Int32 voreGrpxAllocChObj(Vcore_GrpxChObj **chObjs,
                         UInt32 numCh);

Int32 vcoreGrpxFreeChObj(Vcore_GrpxChObj **chObjs,
                         UInt32 numCh);

Int32 vcoreGrpxCheckFormat(const Vcore_GrpxHandleObj *hObj,
                           const Vcore_Format * coreFmt);

Int32 vcoreGrpxCheckMultRegFormat(const Vcore_GrpxHandleObj *hObj,
                                  const Vps_MultiWinParams *mReg,
                                  Vcore_Format *coreFmt);

Int32 vcoreGrpxCheckPitchDataFormat(const Vcore_GrpxHandleObj *hObj,
                                    Vcore_GrpxChObj *chObj,
                                    Vps_GrpxRtParams *rtParams);

Vcore_GrpxChObj *vcoreGrpxGetChannelObj(Vcore_GrpxHandleObj *hObj,
                                        UInt32 chNum);

void vcoreGrpxErrCallback(const UInt32 *event,
                          UInt32 numEvents,
                          Ptr arg);

Int32 vcoreGrpxGetFrmAttr(Vcore_GrpxChObj *chObj,
                          VpsHal_GrpxFrmDispAttr *frmAttr);

Int32 vcoreGrpxCreateClutDataDesc(const Vcore_GrpxHandleObj *hObj,
                                  Ptr descPtr,
                                  Ptr clutData);

Int32 vcoreGrpxCreateStenDataDesc(const Vcore_GrpxHandleObj *hObj,
                                  Vcore_GrpxChObj *chObj,
                                  Vps_GrpxRegionParams *regParams,
                                  VpsHal_VpdmaInDescParams *stenDescParam,
                                  Ptr descPtr,
                                  UInt32 regId);

Int32 vcoreGrpxCreateGrpxDataDesc(const Vcore_GrpxHandleObj *hObj,
                                  Vcore_GrpxChObj *chObj,
                                  Vps_GrpxRegionParams *regParams,
                                  VpsHal_VpdmaRegionDataDescParams *regD,
                                  Ptr descPtr,
                                  UInt32 regId);



#ifdef __cplusplus
}
#endif

#endif /* #ifndef _VPSCORE_GRPXPRIV_H */
