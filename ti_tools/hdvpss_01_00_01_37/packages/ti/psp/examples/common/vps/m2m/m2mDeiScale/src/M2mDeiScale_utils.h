/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file M2mDeiScale_utils.h
 *
 *  \brief VPS DEI memory to memory driver example header file containing
 *  appplication object and utility function desclaration.
 *
 */

#ifndef _M2MDEISCALE_UTILS_H
#define _M2MDEISCALE_UTILS_H

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

/* Number of input buffers to allocate. */
#define DEI_MAX_IN_BUFFERS              (20u)

/* Number of output buffers to allocate. */
#define DEI_MAX_OUT_BUFFERS             (VPS_M2M_DEI_MAX_CH_PER_INST *         \
                                         DEI_MAX_IN_BUFFERS)

/* Number of FVID2 frame list to allocate per process list. */
#define DEI_NUM_FRAMELIST_PER_PLIST     (3u)

/* Number of FVID2 frame object to allocate per framelist. */
#define DEI_NUM_FRAMES_PER_LIST         (VPS_M2M_DEI_MAX_CH_PER_INST)

/* Application name string used in print statements. */
#define APP_NAME                        "AppDeiScale"

/* Maximum number of buffer that would required to be submitted before the
   first buffer is returned back */
#define DEI_NO_OF_BUF_REQ_BEFORE_RES    (3u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief M2M DEI application object.
 */
typedef struct
{
    UInt32                  drvInst;
    /**< M2M DEI driver instance. */
    UInt32                  mode;
    /**< Mode of operation - per channel or per handle. */
    UInt32                  numCh;
    /**< Number of channels per handle. */

    UInt32                  inMemType;
    /**< VPDMA Memory type for the DEI input frame. */
    UInt32                  outMemTypeVip;
    /**< VPDMA Memory type for the DEI-VIP output frame. */

    UInt32                  numInBuffers;
    /**< Number of input buffers. */
    UInt32                  numOutBuffers;
    /**< Number of output buffers. */

    UInt32                  inWidth;
    /**< Input width in pixels. */
    UInt32                  inHeight;
    /**< Input height. */
    UInt32                  inDataFmt;
    /**< DEI input data format. */

    UInt32                  outWidthDei;
    /**< DEI output width in pixels. */
    UInt32                  outHeightDei;
    /**< DEI output height. */

    UInt32                  outWidthVip;
    /**< VIP output width in pixels. */
    UInt32                  outHeightVip;
    /**< VIP output height. */
    UInt32                  outDataFmtVip;
    /**< VIP output data format. */

    UInt32                  inScanFmt;
    /**< DEI input scan format. */
    UInt32                  outScanFmt;
    /**< DEI output scan format - applies to both DEI and DEI-VIP outputs. */

    UInt32                  drnEnable;
    /**< Enables/disables the DRN module in the DEI path. */
    UInt32                  comprEnable;
    /**< Enable compression/decompression for writing back the DEI context
         to and from DDR. */

    UInt32                  inPitchY;
    /**< Luma input buffer pitch in bytes. */
    UInt32                  inPitchCbCr;
    /**< Chroma input buffer pitch in bytes - used in semiplanar format. */
    UInt32                  outPitchDei;
    /**< DEI output buffer pitch in bytes. */
    UInt32                  outPitchYVip;
    /**< VIP luma output buffer pitch in bytes. */
    UInt32                  outPitchCbCrVip;
    /**< VIP chroma output buffer pitch in bytes - used in semiplanar format. */

    /* Below variables are defined so that the buffers are contiguous in memory
       and so the user could load the buffer using CCS using one command.
       This buffer will then be spilt in to multiple single buffers and assigned
       to buffer pointers below. In case of tiler memory, the buffer pointers
       are allocated separately one buffer at a time and then data is copied
       from the load/save buffers using CPU so that tiler conversion is taken
       care automatically. */
    UInt8                  *inLoadBuf;
    /**< Input buffer used to load the input video frames. */
    UInt8                  *outSaveBufDei;
    /**< DEI writeback out buffer used to store the output video frames. */
    UInt8                  *outSaveBufVip;
    /**< VIP out buffer used to store the output video frames. */

    UInt8                  *inBufY[DEI_MAX_IN_BUFFERS];
    /**< Input luma buffer. */
    UInt8                  *inBufCbCr[DEI_MAX_IN_BUFFERS];
    /**< Input chroma buffer. */
    UInt8                  *outBufDei[DEI_MAX_OUT_BUFFERS];
    /**< DEI out buffer. */
    UInt8                  *outBufYVip[DEI_MAX_OUT_BUFFERS];
    /**< VIP out luma buffer. */
    UInt8                  *outBufCbCrVip[DEI_MAX_OUT_BUFFERS];
    /**< VIP out chroma buffer. */

    FVID2_Handle            fvidHandle;
    /**< M2M DEI driver handle. */
    UInt32                  inBufIdx;
    /**< Current input buffer index. Used to wrap around input buffer. */
    UInt32                  outBufIdx;
    /**< Current output buffer index. Used to wrap around output buffer. */
    UInt32                  fid;
    /**< Software FID. */
    UInt32                  completedLoopCnt;
    /**< Total number of completed frames. */
    Semaphore_Handle        reqCompleteSem;
    /**< Request complete semaphore used to sync between callback and task. */
    UInt32                  deiBypassFlag;
    /**< DEI bypass flag. */

    FVID2_ProcessList       processList;
    /**< Process list used for queue/dequeue operations. */
    FVID2_FrameList         frameList[DEI_NUM_FRAMELIST_PER_PLIST];
    /**< Frame list used for queue/dequeue operations. */
    FVID2_Frame             frames[DEI_NUM_FRAMELIST_PER_PLIST]
                                  [DEI_NUM_FRAMES_PER_LIST];
    /**< Frames used for queue/dequeue operations. */

    FVID2_ProcessList       errProcessList;
    /**< Process list used for error operations. */

    Vps_M2mDeiCreateParams  createParams;
    /**< M2M DEI driver create parameters. */
    Vps_M2mDeiChParams      chPrms[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< M2M DEI driver channel parameters for each of the channels. */
    FVID2_Format            outFmtDei[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for DEI out format. */
    FVID2_Format            outFmtVip[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for VIP out format. */
    Vps_DeiHqConfig         deiHqCfg[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for DEI HQ config. */
    Vps_DeiConfig           deiCfg[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for DEI config. */
    Vps_ScConfig            scCfg[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for DEI scalar config. */
    Vps_ScConfig            vipScCfg[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for VIP scalar config. */
    Vps_CropConfig          vipScCropCfg[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for the VIP crop configuration */
    Vps_CropConfig          scCropCfg[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for the DEI crop configuration */
    FVID2_Frame             cntxFrames[VPS_M2M_DEI_MAX_CH_PER_INST]
                                      [DEI_NO_OF_BUF_REQ_BEFORE_RES];
    /**< Place holder for FVID2_frame containers */
    UInt32                  freeCntxFrameIndex[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Counter to indicate used FRAME containers */
    Bool                    isDeiPresent;
    /**< Flag to indicate whether DEI is present in an instance or not. */
    Vps_SubFrameParams      subFramePrms[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for subframe mode configuration parameters*/

    FVID2_SubFrameInfo      subFrameInfoIn[VPS_M2M_DEI_MAX_CH_PER_INST];
    FVID2_SubFrameInfo      subFrameInfoOutDei[VPS_M2M_DEI_MAX_CH_PER_INST];
    /**< Memory for SubFrame level processing information exchange between
         application and driver.*/
} App_M2mDeiObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

Int32 App_m2mDeiAllocBuf(App_M2mDeiObj *appObj);
Int32 App_m2mDeiFreeBuf(App_M2mDeiObj *appObj);
Void App_m2mDeiCopyToTilerMem(App_M2mDeiObj *appObj);
Void App_m2mDeiCopyFromTilerMem(App_M2mDeiObj *appObj);
Int32 App_m2mDeiProgramScCoeff(App_M2mDeiObj *appObj);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef _M2MDEISCALE_UTILS_H */
