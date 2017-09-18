/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *
 *  \brief VPS M2M SubFrame level processing internal header file.
 *
 */

#ifndef _VPSDRV_M2MSLICE_H
#define _VPSDRV_M2MSLICE_H

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

#define VPSMDRV_SUBFRM_MAX_WIDTH    (1920)
#define VPSMDRV_SUBFRM_MAX_HEIGHT   (1080)

#define VPSMDRV_VERT_SUBFRM_OFFSET  (4 + 6)
#define VPSMDRV_HORZ_SUBFRM_OFFSET  (4 + 6)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    UInt32          srcW, fstSrcW;
    /**< Width of the source slice */
    UInt32          tarW, fstTarW;
    /**< Width of the target slice */
    UInt32          srcBufOffset;
    /**< Buffer offset in the input image */
    UInt32          tarBufOffset;
    /**< Buffer offset in the input image */
    UInt32          chrUsPixMemSlcLft;
    /**< Number of extra pixels requires for chroma upsampler on the
         left side of the image */
    UInt32          chrUsPixMemSlcRgt;
    /**< Number of extra pixels requires for chroma upsampler on the
         right side of the image */
    UInt32          scCropPixLft;
    /**< Number of extra pixels requires for cropping on the
         left side of the image */
    UInt32          scCropPixRgt;
    /**< Number of extra pixels requires for cropping on the
         left side of the image */
    unsigned long long          colAccOffset;
    /**<  Value of column accumulator offset required for this slice */

} VpsMDrv_HorzSubFrmCfg;

typedef struct
{
    UInt32          slcSz;
    /**< Slice Size */
    UInt32          noOfSlcs;
    /**< Number of valid entries in hSlcCfg array */
    UInt32          isEnable;
    /**< Flag to indicate whether horizontal slicing is enabled or not */
    unsigned long long          linAccIncr;
    /**<  Value of Linear Accumulator increment required for this slice */
    unsigned long long          colAcc;
    /**< Column accumulator value */
    VpsMDrv_HorzSubFrmCfg slcCfg[VPS_CFG_MAX_NUM_HORZ_SLICE];
} VpsMdrv_HorzSubFrmInfo;

/** struct VpsMdrv_SubFrameChParams
 *  \brief Strucutre containing subframe processing channel instance. This is
 *   initialised for first subframe and will be used and updated for subsequent subframes
 */
typedef struct
{
    /** subframe configuration : subframe mdoe enable flag and subframe size in No of Lines*/
    UInt32          subFrameMdEnable;
    UInt16          slcSz;
    UInt16          noOfSlcs;

    UInt32          isEnable;
    /**< Flag to indicate whether vertical slice is enabled or not */
    /** source and target frame height and Widh Info, used for sc phase calculation per subframe */
    UInt32          srcFrmH;
    UInt32          tarFrmH;
    UInt32          srcFrmW;
    UInt32          tarFrmW;

    /** subframe related information  */
    UInt16          subFrameNum;
    UInt16          slcLnsAvailable;
    UInt16          srcH;
    UInt16          tarH;

    /** buffer offsets of subframe from frame start address */
    UInt16          srcBufOffset;
    UInt16          tarBufOffset;
    UInt16          rowOffset;

    /** parameters which affects per subframe Sc phase info and buffer offset calculation*/
    UInt32          filtType;
    /**< vertical scaling filter type: RAV/PolyPhase
     *   For valid values see #Vps_ScVertScalarType
     */
    UInt32          dataFmt;
    /**< input buffer format 422/420 */
    UInt16          chrUsLnMemSlcTop;
    UInt16          chrUsLnMemSlcBtm; /* this value is used for Luma and 1/2 of this is used for Chroma*/
    UInt16          scCropLnsTop;
    UInt16          scCropLnsBtm;

    /** Scalar phase offset values based on filter type */
    /** polyphase */
    Int32           rowAccIncr;
    Int32           rowAccOffset;
    Int32           rowAccOffsetB;
    Int32           rowAcc;
    /** RAV */
    UInt32          ravScFactor;
    Int32           ravRowAccInit;
    Int32           ravRowAccInitB;
    UInt16          ravMaxSrcHtPerSlc;

    VpsMdrv_HorzSubFrmInfo   hSubFrmInfo;
}VpsMdrv_SubFrameChInst;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Void vpsMdrvScSubFrameInitInfo(VpsMdrv_SubFrameChInst *slcData);
Void vpsMdrvScSubFrameHorzInitInfo(VpsMdrv_SubFrameChInst *slcData);
Int32 vpsMdrvScSubFrameCalcHorzInfo(VpsMdrv_SubFrameChInst *slcData);
Void vpsMdrvScSubFrameCalcInfo(VpsMdrv_SubFrameChInst *slcData);
Void vpsMdrvScSubFrameCalcPostProcInfo(VpsMdrv_SubFrameChInst *slcData);

#ifdef __cplusplus
}
#endif

#endif /*  _VPSDRV_M2MSLICE_H */
