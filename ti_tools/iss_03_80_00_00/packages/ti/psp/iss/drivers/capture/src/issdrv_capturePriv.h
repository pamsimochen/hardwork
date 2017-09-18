/** ==================================================================
 *  @file   issdrv_capturePriv.h
 *
 *  @path   /ti/psp/iss/drivers/capture/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _ISS_CAPT_PRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISS_CAPT_PRIV_H_

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <stdlib.h>
#include <ti/psp/vps/common/vpsutils_que.h>
#include <ti/psp/iss/iss.h>
#include <ti/psp/vps/common/vps_config.h>

#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/iss/drivers/capture/issdrv_capture.h>

#include <ti/psp/iss/drivers/alg/2A/inc/issdrv_algTIaewb.h>
#include <ti/psp/iss/core/inc/iss_comp.h>
#include <ti/psp/iss/hal/iss/bte/bte.h>

/* constant's */

/* Capture driver states */

// #define ISS_CAPTURE_RT_DEBUG

/*
 * State transitions are explained below
 *
 * IDLE - initial or not open state | CREATED - state after calling
 * FVID2_create | DO_START - temporary state when FVID2_start() is called |
 * this means driver has requested CLM to start the instance | RUNNING - this
 * is the state after FVID2_start() completes, this means | CLM started
 * capture and now it is running, it remains in this state | until
 * FVID2_stop() is called | DO_STOP - temporary state when FVID2_stop is
 * called() | this means driver has request CLM to stop the instance |
 * STOPPED - this is the state after FVID2_stop() completes, this means | CLM
 * has stopped this instance | IDLE - this is the state after FVID2_delete()
 * completes */
#define ISS_CAPT_STATE_IDLE     (0)
/** Driver is not open and is idle */
#define ISS_CAPT_STATE_CREATED  (1)
/** Driver is created */
#define ISS_CAPT_STATE_DO_START (2)
/** Driver is requesting a start */
#define ISS_CAPT_STATE_RUNNING  (3)
/** Driver is running */
#define ISS_CAPT_STATE_DO_STOP  (4)
/** Driver is requesting a stop */
#define ISS_CAPT_STATE_STOPPED  (5)
/** Driver is stopped */

/** Stack size for capture processing task */
#define ISS_CAPT_TSK_STACK_UPDATE     (10*1024)

/** Task priority for capture processing task */
#define ISS_CAPT_TSK_PRI_UPDATE       (15)

/** How list processing iteration to wait until VIP instance can be considered
 * to be stopped */
#define ISS_CAPT_STATE_DELETE_COUNT_MAX      (6)

/** Max channel per driver lofical channel */
#define ISS_CAPT_VCH_PER_LCH_MAX            (2)

/** MAX Data Desciptor per port */
#define ISS_CAPT_MAX_VPDMA_CHANNEL          (ISS_CAPT_CH_PER_PORT_MAX * 2u)
#define ISS_DATA_DESC_SIZE            (32u)
#define ISS_CAPT_NUM_DATA_DESC_SIZE         (ISS_CAPT_MAX_VPDMA_CHANNEL *      \
                                                ISS_DATA_DESC_SIZE)

#define ISS_CAPT_NUM_WRITE_DATA_DESC_SET    (8u)

#define ISS_CAPT_DATA_DESC_SIZE_PER_PORT    (ISS_CAPT_NUM_DATA_DESC_SIZE +     \
                                             ISS_CAPT_NUM_DATA_DESC_SIZE * ISS_CAPT_NUM_WRITE_DATA_DESC_SET )
/** Size of list maximum Atmost four descriptor can be programmed worst case
 * per channel */
#define ISS_CAPT_LIST_SIZE_MAX              (ISS_CAPT_DATA_DESC_SIZE_PER_PORT * \
                                                ISS_CAPT_INST_MAX)

/** Size of coeff overlay for VP scaler */
#define ISS_CAPT_VIP_SC_COEFF_SIZE          (ISSHAL_SC_COEFF_OVLY_SIZE)

#define ISS_CAPT_DO_CACHE_OPS

#define ISS_CAPT_DO_REPOST_LIST_FOR_YC_TEAR_ISSUE

// #define ISS_CAPT_DO_FRAME_TAGGING_FOR_FRAME_DROP_ISSUE

/** Size of error track object */
#define ISS_CAPT_ERR_TIME_TRACK_SIZE        (100u)
/** Size of FID repeatation time tracker */
#define ISS_CAPT_FID_RPT_TIME_TRACK_SIZE    (100u)

/* Enable discriptor / frame errors time line tracking */
// #define ISS_CPAT_ERR_TIME_TRACK_ENABLE

/* Enable FID repeation time tracker */
// #define ISS_CPAT_FID_RPT_TIME_TRACK_ENABLE

#define ISS_CAPT_DROP_FRAME_CH_ID           (0xABCD)


#define BTE_BWL 56

#define DIV_UP(n, alignment) ((n + alignment - 1)/alignment)

typedef enum
{
    ISS_CAPT_INT_VD_INT0 = 0,
    ISS_CAPT_INT_RSZ,
    ISS_CAPT_INT_H3A,
    ISS_CAPT_INT_BSC,
    ISS_CAPT_INT_RSZ_OVFL,
    ISS_CAPT_MAX_INT
} Iss_CaptIntId;

/* data structure's */

/**
 * Timeline tracker for descriptor / frame misses if any
 */
#ifdef ISS_CPAT_ERR_TIME_TRACK_ENABLE
typedef struct {
    UInt32 chanNo;
    UInt32 descNotWrittenTimeStamp;
    UInt32 frameNotWrittenTimeStamp;
    UInt32 secFrameNotWrittenTimeStamp;
} Iss_CaptStatsTimeObj;
#endif                                                     /* ISS_CPAT_ERR_TIME_TRACK_ENABLE*/

typedef struct {
    Bool bufIsNew;
    UInt16 mirrorMode;
    Bool flipH;
    Bool flipV;
    Int32 flipVOffsetY;
    Int32 flipVOffsetC;
    Int32 flipHOffsetYC;
    UInt32 memType;
    /* #Iss_MemoryType */
    FVID2_Frame *ptmpFrame;
} tStreamBuf;

/**
 * Object to track the tiem between FID repetations
 */
#ifdef ISS_CPAT_FID_RPT_TIME_TRACK_ENABLE
typedef struct {
    UInt32 chanNo;
    UInt32 fidRptTimeStamp;
} Iss_CaptFidRptObj;
#endif                                                     /* ISS_CPAT_FID_RPT_TIME_TRACK_ENABLE */

/**
 * Channel specific information */
typedef struct {

    /**
     * channel data format information passed from create params
     */
    UInt32 channelNum;
    UInt32 pitch[FVID2_MAX_PLANES];
    UInt32 dataFormat;

    /** invert FID in some specifc cases */
    Bool invertFid;

    /**
     * Pointer to current frame whose descriptor update is in progress
     */
    FVID2_Frame *pTmpFrame;

    /**
     * Current VPDMA descriptors received for this channel
     */
    UInt16 vChannelRecv;

    /**
     * Number of VPDMA descriptors that need to be received
     * to consider that the frame is captured completely
     */
    UInt16 vChannelNum;

    /**
     * VPDMA channel ID for this channel
     */
    // IssHal_VpdmaChannel vChannelId[ISS_CAPT_VCH_PER_LCH_MAX];

    /**
     * VPDMA data type associated with channel
     */
    // IssHal_VpdmaChanDT vChannelDataType[ISS_CAPT_VCH_PER_LCH_MAX];

    /**
     * driver logical number - is unique across all capture handles
     */
    UInt16 lChannelNum;

    /**
     * free or empty buffers available to this channel
     */
    VpsUtils_QueHandle emptyQue;

    /**
     * buffers that are queued to hardware but not yet fully captured
     */
    VpsUtils_QueHandle tmpQue;

    /**
     * que memory associated with emptyQue
     */
    FVID2_Frame *emptyQueMem[ISS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /**
     * que memory associated with tmpQue
     */
    FVID2_Frame *tmpQueMem[ISS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /**
     * frame skip mask for this channel - see iss_capture.h (Iss_CaptFrameSkip)
     * for meaning of frameskip mask
     */
    UInt32 frameSkipMask;
    UInt32 frameSkipMaskHigh;

    /**
     * Current frame number that is being captured, 0..29
     */
    UInt32 frameCount;

    /**
     * last frame/field captured width in pixels
     */
    UInt32 lastFrameWidth;

    /**
     * last frame/field captured height in lines
     */
    UInt32 lastFrameHeight;

    /**
     * last frame/field captured timestamp in OS ticks
     */
    UInt32 lastFrameTimestamp;

    UInt32 lastFrameFid;

    /**
     * tiled or non-tiled memory format
     */
    UInt32 memType;

    /** max output width limit */
    UInt16 maxOutWidth;

    /** max output height limit */
    UInt16 maxOutHeight;

    /**
     * dropped frame count
     */
    UInt32 dropFrameCount;
    UInt32 skipFrameCount;

    /**
     * capture frame count
     */
    UInt32 captureFrameCount;

    /**
     * Field cound for each field
     */
    UInt32 fieldCount[FVID2_MAX_FIELDS];

    UInt32 fidRepeatCount;
} Iss_CaptChObj;

/**
 * Driver instance specific handle */
typedef struct {
    Semaphore_Handle lock;                                 /** driver lock */

    /**
     * Number of channels in this instance
     */
    UInt16 numCh;

    /**
     * Number of streams in this instance
     */
    UInt16 numStream;

    /**
     * < TRUE/FALSE: Enable/disable VSTAB
     */
    UInt32 vsEnable;

    /**
     * < TRUE/FALSE: Enable/disable VSTAB
     */
    UInt32 vsDemoEnable;
    /**
     * < TRUE/FALSE: Enable/disable GLBC
     */
    UInt32 glbcEnable;

    /**
     * Instance ID
     */
    UInt16 instanceId;

    /**
     * VIP HW port ID
     */
    UInt16 vipPortId;

    /**
     * VIP HW instd ID
     */
    UInt16 vipInstId;

    /**
     * instance state
     */
    UInt16 state;

    /**
     * captured/full buffer que
     */
    VpsUtils_QueHandle fullQue[ISS_CAPT_STREAM_ID_MAX];

    /**
     * que memory associated with fullQue
     */
    FVID2_Frame *fullQueMem
        [ISS_CAPT_STREAM_ID_MAX]
        [ISS_CAPT_CH_PER_PORT_MAX * ISS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /**
     * channel object's for every channel and stream
     */
    Iss_CaptChObj chObj[ISS_CAPT_STREAM_ID_MAX][ISS_CAPT_CH_PER_PORT_MAX];

    /**
     * create arguments
     */
    Iss_CaptCreateParams createArgs;

    /**
     * user callback parameters
     */
    FVID2_DrvCbParams cbPrm;

    /**
     * semphore for waiting until instance is stopped
     */
    Semaphore_Handle semStopDone;

    /**
     * semphore for waiting until instance is started
     */
    Semaphore_Handle semStartDone;

    /**
     * number of iterations of list processing task that
     * instance is in delete state
     */
    UInt16 stateDeleteCount;

    /** add a dummy multi-ch desc to the list */
    Bool addMultiChDummyDesc;

    Bool resetStatistics;

    UInt8 *listAddr;
    /** Pointer to the start of the list for this capture object */
    UInt32 listSize;
    /**< Size of the list pointed by listAddr */

    volatile UInt32 descMask;
    /**< Descriptor Mask */

    UInt32 loopCount;
    UInt32 listPostCnt;
    UInt32 pidZeroCnt;

    UInt32 strVChCnt[4];
    UInt32 allDescMask;
    UInt32 firstDescMissMatch;
    UInt32 secDescMissMatch;
    UInt32 thirdDescMissMatch;
    /**< Stats counters */

    /** These variables would be initialized at initilization time and used
     * across */
    Iss_PlatformCpuRev cpuRev;
    /**< Version of the silicon being used */
    Iss_PlatformId platform;
    /**< Current pltform */

    UInt32 histData[256];
    /**< Histogram Buffer */

    UInt32 histSize;
    /**< Histogram Size */

    UInt32            updateResPrms;

    Iss_IspH3aCfg     h3aCfg;
    Iss_IspIsifCfg    isifCfg;

    FVID2_Format      inFmt;
	
	UInt32 dbgLineNo;

} Iss_CaptObj;

/**
 * Global/Common driver object */
typedef struct {
    MSP_BOOL bThisIsStarted;
    MSP_BOOL bWaitForCcpRx;
    MSP_IspIntHandleQueueT *pIspIntQueueFirst;
    iss_drv_config_t iss_drv_config;
} ispDriverInstanceT;

typedef struct {
    int     aewbNumWinH;
    int     aewbNumWinV;
    int     aewbNumPix;
    int     h3aUpdate;
    int     emptyFrame;
    Int32   *h3aBuffAddr[2];
    Int32   *h3aBuffAddrBkup[2];
}tH3AbuffParams;

typedef struct {
    int contextNumber;
    bte_config_t bte_config[BTE_CONTEXT_MAX];
    UInt32 bwLimiter;
    UInt32 baseAddress;
} bteHandle;

/**
 * Global/Common driver object */
typedef struct {
    /**
     * Alg processing task wakeup semaphore
     */
    Semaphore_Handle semUpdate;
    Semaphore_Handle ovflWait;

    /**
     * Alg processing task handle
     */
    Task_Handle tskUpdate;

    Task_Handle ovflTask;

    UInt32 exitTask;

    /**
     * clock ticks elasped since last semaphore post
     */
    UInt32 clockTicks;

    /**
     * FVID2 driver ops
     */
    FVID2_DrvOps fvidDrvOps;

    /**
     * capture instance objects
     */
    Iss_CaptObj captureObj[ISS_CAPT_INST_MAX];

    /**
     * user channel number to driver channel number map
     */
    UInt8 fvidChannelNum2lChannelNumMap
        [ISS_CAPT_STREAM_ID_MAX * ISS_CAPT_CH_PER_PORT_MAX * ISS_CAPT_INST_MAX];

    /**
     * exit Alg processing task
     */
    volatile UInt32 exitTsk;

    UInt32 descParseErrorCount;
    UInt32 resetCount;
    UInt32 descMissFound;

    Void *dropDataAddr;

    Iss_IttParams *gIttParams;

    VideoModuleInstanceT *pModuleInstance;

    MSP_IspConfigProcessingT *pIssConfig;

    ispDriverInstanceT *pIspDriverInstance;

    iss_drv_config_t *pIss_drv_config;

    MSP_COMPONENT_TYPE *hIspMSP;

    tH3AbuffParams gH3ABufParams;

    MSP_IspH3aCfgT tIspH3aCfg;

    Int32 *YUVcaptureAddr;

    Int32 *buffOffsetY;

    Int32 *buffOffsetYC;

    Int32 pitch[2];

    bteHandle handleBTE;


    Void *intHandle[ISS_CAPT_MAX_INT];
    UInt32 intCounter[ISS_CAPT_MAX_INT];

} Iss_CaptCommonObj;

typedef struct {
    UInt32 REG_ADDR;
    UInt32 NUM_REGS;
    UInt32 RSV0[2];
    UInt32 REG_VAL[4];
} Iss_captVipDataPathCfgOverlay;



/**
  \brief Capture descriptor parsing info for recevied descriptor
*/
typedef struct {
    Ptr descAddr;
  /**< Address from where descriptor needs to be parsed */

    UInt16 fid;
  /**< received field ID, 0 for even field, 1 for odd field  */

    UInt16 frameWidth;
  /**< received field/frame width, in pixels  */

    UInt16 frameHeight;
  /**< received field/frame height, in lines  */

    UInt16 lChannel;
  /**< Logical channel to which this descriptor belongs */

    UInt16 vChannelIdx;
  /**< VPDMA channel idx to which this descriptor belongs */

} Iss_CaptDataDescParseInfo;

#define ISS_CAPT_VIP_CFG_PAYLOAD_SIZE     (100u)

/* global's */
extern Iss_CaptCommonObj gIss_captCommonObj;

/* function's */

/* ===================================================================
 *  @func     Iss_captCreateQueues
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captCreateQueues(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captCreateFrames
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captCreateFrames(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captCreateChObj
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captCreateChObj(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captDeleteQueues
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captDeleteQueues(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captDeleteFrames
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captDeleteFrames(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captIsDataAvailable
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
UInt32 Iss_captIsDataAvailable(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captCreateVip
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captCreateVip(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captStartVip
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captStartVip(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captStopVip
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captStopVip(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captDeleteVip
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captDeleteVip(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captSetFrameStartEvent
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captSetFrameStartEvent(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captDeInitList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captDeInitList();

/* ===================================================================
 *  @func     Iss_captIsrListComplete
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Void Iss_captIsrListComplete();

/* ===================================================================
 *  @func     Iss_captStartList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captStartList(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captStopList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captStopList(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captAddToList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captAddToList(Iss_CaptObj * pObj, Iss_CaptChObj * pChObj);

/* ===================================================================
 *  @func     Iss_captAddAbortInList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captAddAbortInList(Iss_CaptObj * pObj, Iss_CaptChObj * pChObj);

/* Int32 Iss_captUpdateListDesc ( Iss_CaptObj * pObj,
 * Iss_CaptLmDataDescParseInfo * parseDescInfo, Iss_CaptLmDataDescUpdateInfo
 * * updateDescInfo, int timestamp ); */
/* ===================================================================
 *  @func     Iss_captSetFrameSkip
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captSetFrameSkip(Iss_CaptObj * pObj, Iss_CaptFrameSkip * frameSkip);

/* ===================================================================
 *  @func     Iss_captITTControl
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captITTControl(Iss_CaptObj * pObj, Iss_IttParams * frameSkip);

/* ===================================================================
 *  @func     IssCaptureRawFrame
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 IssCaptureRawFrame(Iss_CaptObj * pObj, Iss_IttParams * ittParams);

/* ===================================================================
 *  @func     IssCaptureYUVFrame
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 IssCaptureYUVFrame(Iss_CaptObj * pObj, Iss_IttParams * ittParams);

/* ===================================================================
 *  @func     Iss_captVipSetScParams
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captVipSetScParams(Iss_CaptObj * pObj, Iss_CaptScParams * scParams);

/* ===================================================================
 *  @func     Iss_captGetChStatus
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captGetChStatus(Iss_CaptObj * pObj, Iss_CaptChGetStatusArgs * args,
                          Iss_CaptChStatus * chStatus);

/* ===================================================================
 *  @func     Iss_captLock
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captLock(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captUnlock
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captUnlock(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captLockCreate
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captLockCreate(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captLockDelete
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captLockDelete(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captFreePath
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captFreePath(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captAllocPath
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captAllocPath(Iss_CaptObj * pObj);

// Int32 Iss_captVipInstReset(IssHal_VipInst vipInstId);

/* ===================================================================
 *  @func     Iss_captCheckOverflow
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captCheckOverflow(Iss_CaptOverFlowStatus * overFlowStatus);

/* ===================================================================
 *  @func     Iss_captResetAndRestart
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captResetAndRestart(Iss_CaptOverFlowStatus * overFlowStatus);

/**
  \brief Return pointer to scaler coeff memory for a given VIP instance

  \param vipInstId  [IN] VIP inst 0 or 1

  \return pointer to scaler coeff memory for a given VIP instance
*/
/* ===================================================================
 *  @func     Iss_captGetScCoeffMem
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Ptr Iss_captGetScCoeffMem(UInt32 vipInstId);

/* Static Function Declaration */
/* ===================================================================
 *  @func     Iss_captTskSubmitList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captTskSubmitList();

/* ===================================================================
 *  @func     Iss_captMakeList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captMakeList(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captTskListDoVipStart
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captTskListDoVipStart();

/* ===================================================================
 *  @func     Iss_captTskUpdateList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captTskUpdateList();

/* ===================================================================
 *  @func     Iss_captTskListDoCallback
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captTskListDoCallback();

/* ===================================================================
 *  @func     Iss_CaptTskPutCaptFrm
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_CaptTskPutCaptFrm(Iss_CaptObj * pObj,
                            Iss_CaptChObj * pChObj,
                            Iss_CaptDataDescParseInfo * parseDescInfo);
/* ===================================================================
 *  @func     Iss_CaptTskUpdateFrm
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_CaptTskUpdateFrm(Iss_CaptObj * pObj,
                           Iss_CaptChObj * pChObj,
                           Iss_CaptDataDescParseInfo * parseDescInfo);
/* ===================================================================
 *  @func     Iss_captTskListAddMagicNum
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Void Iss_captTskListAddMagicNum(UInt8 * addr, UInt32 pitch);

/* ===================================================================
 *  @func     Iss_captTskListCheckMagicNum
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captTskListCheckMagicNum(UInt8 * addr, UInt32 pitch,
                                   UInt32 channelNum);
/* ===================================================================
 *  @func     Iss_captIsDisSyncStartSequenceNeeded
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Bool Iss_captIsDisSyncStartSequenceNeeded(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captTskListDoStop
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captTskListDoStop();

/* ===================================================================
 *  @func     Iss_captTskMvDescTempToEmpty
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captTskMvDescTempToEmpty(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captUpdateStatitics
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Void Iss_captUpdateStatitics(Iss_CaptObj * pObj,
                             Iss_CaptChObj * pChObj,
                             Iss_CaptDataDescParseInfo * parseDescInfo);
/* ===================================================================
 *  @func     Iss_captResetStatitics
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Void Iss_captResetStatitics(Iss_CaptObj * pObj, Iss_CaptChObj * pChObj);

/* ===================================================================
 *  @func     Iss_captAddToList
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captAddToList(Iss_CaptObj * pObj, Iss_CaptChObj * pChObj);

/* ===================================================================
 *  @func     Iss_captDoDisSyncStartSequence
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captDoDisSyncStartSequence(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captGetListAddr
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
UInt8 *Iss_captGetListAddr(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captFlush
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captFlush(Fdrv_Handle handle, FVID2_FrameList * frameList);

/* ===================================================================
 *  @func     Iss_capt2AUpdate
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_capt2AUpdate(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captSetResolution
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captSetResolution(Iss_CaptObj * pObj,
                            Iss_CaptResolution * resolutionParams);
/* ===================================================================
 *  @func     Iss_captSetInResolution
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captSetInResolution(Iss_CaptObj * pObj,
                              Iss_CaptResParams *resolutionParams);
/* ===================================================================
 *  @func     Iss_captSetFrameRate
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captSetFrameRate(Iss_CaptObj * pObj);

/* ===================================================================
 *  @func     Iss_captITTControl
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captITTControl(Iss_CaptObj * pObj, Iss_IttParams * ittParams);

/* ===================================================================
 *  @func     Iss_captSetMirrorMode
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_captSetMirrorMode(Iss_CaptObj * pObj, Int32 * mirrorMode);

Int32 IssCdrv_registerInt();
Void IssCdrv_unRegisterInt();
Void IssCdrv_issH3aCallback();

Void *IssCdrv_init2AObj(UInt32 sensorId, Fdrv_Handle SensorHandle, UInt32 isIsifCapt);
Void IssCdrv_reInit2AObj(Void *p2AObj);
Void IssCdrv_deInit2AObj(Void *pObj);

Int32 IssCdrv_configureBteContext(bteHandle *handleBTE, int streamNumber, int width, int height, int format, int rotationMode);
Int32 IssCdrv_startBteContext(bte_config_t *contextHandle, uint32 tilerAddress);
Int32 IssCdrv_stopBteContext(bte_config_t *contextHandle);
UInt32 IssCdrv_getBteCtxNum();
Void IssCdrv_setBteBwLimiter(UInt32 bwLimit);
Void IssCdrv_setBteCtrlPosted(UInt32 enable);
Int32 isYUV422ILEFormat(Int32 format);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _ISS_CAPT_PRIV_H_
                                                            */
