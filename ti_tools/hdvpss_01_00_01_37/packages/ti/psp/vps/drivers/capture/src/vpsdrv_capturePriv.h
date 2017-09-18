/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _VPS_CAPT_PRIV_H_
#define _VPS_CAPT_PRIV_H_

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/drivers/capture/src/vpsdrv_captureLmPriv.h>
#include <ti/psp/vps/common/vpsutils_que.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_evtMgr.h>

#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_vip.h>
#include <ti/psp/vps/hal/vpshal_sc.h>

#include <ti/psp/vps/core/vpscore_vipRes.h>
#include <ti/psp/platforms/vps_platform.h>

/* constant's */
/**
 * \brief N lines added sub frame size specified by apps.
 *      Please note that driver will not configure the application supplied sub
 *      frame size. It would add another VPS_CAPT_SUBFRAME_ADDITIONAL_NO_OF_LINES
 *      to the specified size. This would required as VPDMA would trigger the
 *      interrupt much before all the lines are written into memory. Due to line
 *      buffer delays and others.
 */
#define VPS_CAPT_SUBFRAME_ADDITIONAL_NO_OF_LINES    (10u)

/**
 * \brief Macro that enables printing of TS stats.
 */
//#define VPS_CAPT_PRINT_TS_STATS

/* Capture driver debug options  */

/**
 * \brief Eables tracking of unique low / high value.
 */
//#define VPS_CAPTURE_TS_DEBUG
/**
 * \brief Number of instances of unique values that would be logged.
 */
#ifdef VPS_CAPTURE_TS_DEBUG
#define VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH (99u)
#endif

//#define VPS_CAPTURE_RT_DEBUG

/*
  State transitions are explained below

  IDLE - initial or not open state
   |
  CREATED - state after calling FVID2_create
   |
  DO_START - temporary state when FVID2_start() is called
   |         this means driver has requested CLM to start the instance
   |
  RUNNING  - this is the state after FVID2_start() completes, this means
   |         CLM started capture and now it is running, it remains in this state
   |         until FVID2_stop()  is called
   |
  DO_STOP  - temporary state when FVID2_stop is called()
   |         this means driver has request CLM to stop the instance
   |
  STOPPED  - this is the state after FVID2_stop() completes, this means
   |         CLM has stopped this instance
   |
  IDLE     - this is the state after FVID2_delete() completes
*/
#define VPS_CAPT_STATE_IDLE     (0) /* Driver is not open and is idle */
#define VPS_CAPT_STATE_CREATED  (1) /* Driver is created */
#define VPS_CAPT_STATE_DO_START (2) /* Driver is requesting a start  */
#define VPS_CAPT_STATE_RUNNING  (3) /* Driver is running  */
#define VPS_CAPT_STATE_DO_STOP  (4) /* Driver is requesting a stop  */
#define VPS_CAPT_STATE_STOPPED  (5) /* Driver is stopped */

/* Stack size for list processing task  */
#define VPS_CAPT_TSK_STACK_LIST_UPDATE     (10*1024)

/* Task priority for list processing task */
#define VPS_CAPT_TSK_PRI_LIST_UPDATE       (15)

/* How list processing iteration to wait until VIP instance can
  be considered to be stopped
*/
#define VPS_CAPT_STATE_DELETE_COUNT_MAX      (6)

/* Max VPDMA channel per driver logical channel */
#define VPS_CAPT_VCH_PER_LCH_MAX            (2)

/* MAX Data Desciptor per port */
#define VPS_CAPT_MAX_VPDMA_CHANNEL          (VPS_CAPT_CH_PER_PORT_MAX * 2u)

#define VPS_CAPT_NUM_DATA_DESC_SIZE         (VPS_CAPT_MAX_VPDMA_CHANNEL *      \
                                                VPSHAL_VPDMA_DATA_DESC_SIZE)

#define VPS_CAPT_NUM_WRITE_DATA_DESC_SET    (8u)

#define VPS_CAPT_DATA_DESC_SIZE_PER_PORT    (VPS_CAPT_NUM_DATA_DESC_SIZE +     \
                                             VPS_CAPT_NUM_DATA_DESC_SIZE * VPS_CAPT_NUM_WRITE_DATA_DESC_SET )
/* Size of list maximum
  Atmost four descriptor can be programmed worst case per channel
*/
#define VPS_CAPT_LIST_SIZE_MAX              (VPS_CAPT_DATA_DESC_SIZE_PER_PORT * \
                                                VPS_CAPT_INST_MAX)

/* Size of coeff overlay for VIP scaler */
#define VPS_CAPT_VIP_SC_COEFF_SIZE          (VPSHAL_SC_COEFF_OVLY_SIZE)

/* These states are used to initially program the Descriptors for the
 * frame capture or field merged capture mode.
 */
/* This is the state that no descriptors are programmed and list is also not
 * submitted
 */
#define VPS_CAPT_STATE_0                    (0u)

/* 1st set of descriptor is programmed. We always program 1st set of
 * descritptor is always programmed for field 0 reception. We may receive
 * field 0 or field1. And based on that we take appropriate steps in driver
 */
#define VPS_CAPT_STATE_1                    (1u)

/* 2nd set of descriptor is programmed. We always program 2nd set of
 * descritptor is always programmed for field 1 reception. We may receive
 * field 0 or field1. And based on that we take appropriate steps in driver.
 * This means 1st set of descriptor is already submitted to VPDMA for reception
 * of field
 */
#define VPS_CAPT_STATE_2                    (2u)

/* 3rd set of descriptor is programmed. We always program 3rd set of
 * descritptor for field 1 reception. We may receive
 * field 0 or field1. And based on that we take appropriate steps in driver.
 * This means 2nd set of descriptor is already submitted to VPDMA for reception
 * of field
 */
#define VPS_CAPT_STATE_3                    (3u)


/* In cases where application has not specified the incoming video height
 * we cannot calucalate expected sub frame count. Using a very large number
 * to indicate the same
 */
#define VPS_CAPT_SUBFRAME_HEIGHT_UNLIM_EXPECTED_SF_COUNT    (0xFFFFFFFFu)


#ifndef TI_8107_BUILD
#define VPS_CAPT_DO_CACHE_OPS
#endif

#ifdef TI_816X_BUILD
#define VPS_CAPT_DO_REPOST_LIST_FOR_YC_TEAR_ISSUE
#define VPS_CAPT_DO_ABORT_LIST_FOR_YC_TEAR_ISSUE
#endif /* #ifdef TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
#define VPS_CAPT_DO_REPOST_LIST_FOR_YC_TEAR_ISSUE
#endif /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

#ifdef PLATFORM_ZEBU
#undef VPS_CAPT_DO_ABORT_LIST_FOR_YC_TEAR_ISSUE
#endif

//#define VPS_CAPT_DO_FRAME_TAGGING_FOR_FRAME_DROP_ISSUE

/* Size of error track object */
#define VPS_CAPT_ERR_TIME_TRACK_SIZE        (100u)
/* Size of FID repeatation time tracker */
#define VPS_CAPT_FID_RPT_TIME_TRACK_SIZE    (100u)

/* Enable discriptor / frame errors time line tracking */
//#define VPS_CPAT_ERR_TIME_TRACK_ENABLE

/* Enable FID repeation time tracker */
//#define VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE

#define VPS_CAPT_DROP_FRAME_CH_ID           (0xABCD)

/* Maximum number of frames to be programmed with the
   dropped data descriptor at the time of restarting
   the capture when VIP overflows */
#define VPS_CAPT_MAX_DROP_FRAMES            (0x2u)

/* Forward data type declarations */
typedef struct Vps_CaptSubFrameQObj_t Vps_CaptSubFrameQObj;

/* data structure's */

/*
 * SubFrame channel object
 */
typedef struct
{

    /* Q that holds that frame that is being captured OR scheduled for capture.
       Note that the capture would in progress for the first frame in this Q.
       This Q is a FIFO Q */
    VpsUtils_QueHandle subFrameQ;

    /* Memory for the sub frame Q */
    Vps_CaptSubFrameQObj *subFrameQMem[VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /*
     * Sub Frame callback assigned by applications
     */
    FVID2_SubFrameCbFxn subFrameCb;

    /*
     * Required for book keeping of subFrame Q objects. when subframe mode is
     * enabled, one stream will have only one channel (non muxed mode of
     * capture) will require to store which stream a given channel belogs to.
     *
     * When allocating / deallocating Q objects streamId would be required
     * existing APIs signature would require changes, if streamId is to be
     * derived.
     */
    UInt32 streamId;

    /* Event Managers ISR register handle Client interrupt (N line CB) */
    Void * subFrameClientInt[VPS_CAPT_VCH_PER_LCH_MAX];

    /* Event Managers ISR register handle Channel interrupt (End of Frame CB) */
    Void * subFrameChannelInt;

    /*
     * Size of each sub frame as configured by the driver.
     * Please note that driver will not configure the application supplied sub
     * frame size. It would add another VPS_CAPT_SUBFRAME_ADDITIONAL_NO_OF_LINES
     * to the specified size. This would required as VPDMA would trigger the
     * interrupt much before all the lines are written into memory.
     *
     */
    UInt32 subFrameSize;

    /* Size of each sub frame as specified by applications */
    UInt32 nLineCount;

    /* Expected number of subFrames per frame */
    UInt32 expectSFCount;

    /*
     * Maximum height of the frame that is expected.
     * Default 0x0
     */
    UInt32 maxFrameHeight;

    /* Current field under reception, used as check by N line ISR to ensure
       correct field is being returned to applications */
    Vps_CaptSubFrameQObj *currCaptField;

    volatile UInt32 *fidReg;
    UInt32 maskFid;
    UInt32 shiftFid;

    volatile UInt32 *srcSizeHeightReg;
    UInt32  maskHeight;
    UInt32  shiftHeight;

    volatile UInt32 *srcSizeWidthReg;
    UInt32  maskWidth;
    UInt32  shiftWidth;

}Vps_CaptSubFrameChObj;


/*
 * Timeline tracker for descriptor / frame misses if any
 */
#ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
typedef struct
{
    UInt32 chanNo;
    UInt32 descNotWrittenTimeStamp;
    UInt32 frameNotWrittenTimeStamp;
    UInt32 secFrameNotWrittenTimeStamp;
}Vps_CaptStatsTimeObj;
#endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */

/*
 * Object to track the tiem between FID repetations
 */
#ifdef VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE
typedef struct
{
    UInt32  chanNo;
    UInt32  fidRptTimeStamp;
}Vps_CaptFidRptObj;
#endif /* VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE */

typedef struct
{
    UInt32 timeTicks;
    FVID2_Frame *prevFrmAddr;
    FVID2_Frame *nextFrmAddr;
    UInt32 descWrAddr;
    UInt32 programmedField;
    UInt32 expectedDescSet;
    UInt32 expectedFid;
}debugData;

/*
  Channel specific information
*/
typedef struct
{

    UInt32 channelNum;
    UInt32 pitch[FVID2_MAX_PLANES];
    UInt32 dataFormat;

    /* invert FID in some specifc cases */
    Bool invertFid;

    /*
     * Pointer to current frame whose descriptor update is in progress
     */
    FVID2_Frame *pTmpFrame;

    /*
     * Current VPDMA descriptors received for this channel
     */
    UInt16 vChannelRecv;

    /*
     * Number of VPDMA descriptors that need to be received
     * to consider that the frame is captured completely
     */
    UInt16 vChannelNum;

    /*
     * VPDMA channel ID for this channel
     */
    VpsHal_VpdmaChannel vChannelId[VPS_CAPT_VCH_PER_LCH_MAX];

    /*
     * VPDMA data type associated with channel
     */
    VpsHal_VpdmaChanDT vChannelDataType[VPS_CAPT_VCH_PER_LCH_MAX];

    /*
     * driver logical number - is unique across all capture handles
     */
    UInt16 lChannelNum;

    /*
     * free or empty buffers available to this channel
     */
    VpsUtils_QueHandle emptyQue;

    /*
     * buffers that are queued to hardware but not yet fully captured
     */
    VpsUtils_QueHandle tmpQue;

    /*
     * que memory associated with emptyQue
     */
    FVID2_Frame *emptyQueMem[VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /*
     * que memory associated with tmpQue
     */
    FVID2_Frame *tmpQueMem[VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /*
     * frame skip mask for this channel - see vps_capture.h (Vps_CaptFrameSkip)
     * for meaning of frameskip mask
     */
    UInt32 frameSkipMask;

    /*
     * Current frame number that is being captured, 0..29
     */
    UInt32 frameCount;

    /*
     * last frame/field captured width in pixels
     */
    UInt32 lastFrameWidth;

    /*
     * last frame/field captured height in lines
     */
    UInt32 lastFrameHeight;

    /*
     * last frame/field captured timestamp in OS ticks
     */
    UInt32 lastFrameTimestamp;

    UInt32 lastFrameFid;

    /*
     * tiled or non-tiled memory format
     */
    UInt32 memType;

    /* max output width limit */
    UInt16 maxOutWidth;

    /* max output height limit */
    UInt16 maxOutHeight;

    /*
     * dropped frame count
     */
    UInt32 droppedFrameCount;

    /*
     * capture frame count
     */
    UInt32 captureFrameCount;

    /*
     * Field cound for each field
     */
    UInt32 fieldCount[FVID2_MAX_FIELDS];

    UInt32 maxRecvFrameWidth;
    UInt32 minRecvFrameWidth;

    UInt32 maxRecvFrameHeight;
    UInt32 minRecvFrameHeight;

    /* Pointer to write descriptor address */
    UInt8 *writeDescAddr[VPS_CAPT_VCH_PER_LCH_MAX][VPS_CAPT_NUM_WRITE_DATA_DESC_SET];
    UInt32 writeDescIdx[VPS_CAPT_VCH_PER_LCH_MAX];

    UInt32 frameNotWrittenCount;
    UInt32 secFrameNotWrittenCount;
    UInt32 descNotWrittenCount;
    UInt32 secDescNotWrittenCount;

#ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
    Vps_CaptStatsTimeObj errTimeStamps[VPS_CAPT_ERR_TIME_TRACK_SIZE];
    UInt32 errTimeStampIndex;
#endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */

#ifdef VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE
    Vps_CaptFidRptObj fidRptTimeStamps[VPS_CAPT_FID_RPT_TIME_TRACK_SIZE];
    UInt32 fidRptIndex;
#endif /* VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE */

    FVID2_Frame dropFrame[VPS_CAPT_NUM_WRITE_DATA_DESC_SET];

    UInt32 fidRepeatCount;

    UInt32 printFid;

    /* Frame capture of field capture for interlaced scan format */
    FVID2_BufferFormat frameCapture;

    /* This variable is used to specify whether two fields are merged for
     * Frame capture or are in separate planes.
     */
    Bool fieldsMerged;

    /* This field is used only for frame based capture. We are programming
     * two sets of addresses to VPDMA one shadowed and other current. This
     * fields specifies which field address is programmed in each these
     * addresses.
     */
    UInt32  programmedField[2];
    /* This  field specifies out of which two address i am expecting the
     * field to be completed for. This field will toggle each time new
     * descriptor is received.
     */
    UInt32  expectedDescSet;
    /* This field holds the frame temporarily for which the address are
     * are programmed to VPDMA for buffer reception.
     */
    FVID2_Frame *pPrevFrame;
    /* This field holds the frame that is to be programmed for reception. */
    FVID2_Frame *pNxtFrame;

    /* Even field is captured or not If even field is captured and odd is
     * also captured frame capture is complete*/
    UInt32 evenFieldCaptured;
    /* What is the FID received for the currently captured field */
    UInt32 curFid;
    /* What is the expected FID for the captured field */
    UInt32 expectedFid;
    /* Flag for start sequence */
    UInt32  frameCaptureState;

    /* Data for debugging the frame based capture */
    debugData dbgData[200];

    /* Index for updating debug data */
    UInt32    dbgDataIndex;

    /* Programmed FID This is a temporary variable*/
    UInt32      programmedFid;
    /*
     * channel data format information passed from create params
     */
    /*
     * Flag to indicate if sub frame processing is enabled for this channel
     */
    UInt32 isSubFrameEnabled;

    /*
     * Sub Frame per Channel / Stream configurations and varibales
     */
    Vps_CaptSubFrameChObj subFrameObj;

    /* Maximum number of drop descriptor to be programmed */
    UInt32 numDropData;
    
    /*
     * Interrupts are used to detemine the time at which frame capture was
     * completed. In the 8msec timer tick, check this variable before updating
     * the frame timeStamp. If set, the ISR would have updated, do not update in
     * 8msec timer tick context.
     */
    UInt32 tsUseBestEffort;
    
    /*
     * Function pointer used to determine current time
     */
    Vps_CaptFrameTimeStamp getCurrTime;

    /* Event Managers ISR register handle Channel interrupt (End of Frame CB) */
    Void * tsEofIsrHndl;

    /* Queue that maintains the frames that are to be time stampped.
        This is FIFO queue, each element in this queue is a pointer to a frame
        / feild. The ISR would pick the top most element, time stamp it.
        No other operation is done on this frame. Regular processing of the
        frame continues.
    */
    VpsUtils_QueHandle tsFrameQue;
    
    /* Memory required for the queue above. */
    FVID2_Frame *tsFrameQueMem[VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /* Time Stamps Stats */
    /* Expected timer interval between frames */
    UInt32 tsExpectTime;

#ifdef VPS_CAPT_PRINT_TS_STATS
    /* Minimum time period between two consecutive frames / feilds */
    UInt32 tsMinDiff;
    /* Maximum time period between two consecutive frames / feilds */
    UInt32 tsMaxDiff;
    /* Cummulative difference between time stamps */
    UInt32 tsCummulative;
    /* Last frames / feilds time stamp */
    UInt32 tsLast;

    /* Number of frames that were not time stampped */
    UInt32 tsMissedFrameCnt;
    /* Error condition, Number of frames that was presented for time stamping, 
        more than once */
    UInt32 tsMultiStampFrameCnt;

    /* Number of times, frames were time stamped in-correctly. i.e. When
        N-1 frame time stamp is > Nth frame time stamp */
    UInt32 tsErr;
#endif /* VPS_CAPT_PRINT_TS_STATS */

#ifdef VPS_CAPTURE_TS_DEBUG
    /* Stats counters, for time stats measured in the driver. */
    /* Element 0 will have time difference between this frame and the last
       Element 1 will have the actual time ticks */
    UInt32 tsDrvDbg[VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH][2];
    UInt32 tsDrvDbgIndex;
    
    UInt32 tsDrvHDbg[VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH][2];
    UInt32 tsDrvHDbgIndex;

    UInt32 tsDbgLastTs;
    UInt32 tsIsrDbg[VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH][2];
    UInt32 tsIsrDbgIndex;

    UInt32 tsIsrHDbg[VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH][2];
    UInt32 tsIsrHDbgIndex;
#endif /* VPS_CAPTURE_TS_DEBUG */

} Vps_CaptChObj;


/*
  Driver instance specific handle
*/
typedef struct
{
    Semaphore_Handle lock;      /* driver lock */

    /*
     * Number of channels in this instance
     */
    UInt16 numCh;

    /*
     * Number of streams in this instance
     */
    UInt16 numStream;

    /*
     * Instance ID
     */
    UInt16 instanceId;

    /*
     * VIP HW port ID
     */
    UInt16 vipPortId;

    /*
     * VIP HW instd ID
     */
    UInt16 vipInstId;

    /*
     * instance state
     */
    UInt16 state;

    /*
     * captured/full buffer que
     */
    VpsUtils_QueHandle fullQue[VPS_CAPT_STREAM_ID_MAX];

    /*
     * que memory associated with fullQue
     */
    FVID2_Frame *fullQueMem
        [VPS_CAPT_STREAM_ID_MAX]
        [VPS_CAPT_CH_PER_PORT_MAX * VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

     /*
     * captured/full buffer que, We will copy one frame from full queue to this
     * queue one at time.  This is mainly required if you want to have
     * one call back for one completed frame and one frame returned in dequeued
     *
     */
    VpsUtils_QueHandle fullQue1[VPS_CAPT_STREAM_ID_MAX];

    /*
     * que memory associated with fullQue
     */
    FVID2_Frame *fullQueMem1
        [VPS_CAPT_STREAM_ID_MAX]
        [VPS_CAPT_CH_PER_PORT_MAX * VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];

    /*
     * channel object's for every channel and stream
     */
    Vps_CaptChObj chObj[VPS_CAPT_STREAM_ID_MAX][VPS_CAPT_CH_PER_PORT_MAX];

    /*
     * create arguments
     */
    Vps_CaptCreateParams createArgs;

    /*
     * user callback parameters
     */
    FVID2_DrvCbParams cbPrm;

    /*
     * semphore for waiting until instance is stopped
     */
    Semaphore_Handle semStopDone;

    /*
     * semphore for waiting until instance is started
     */
    Semaphore_Handle semStartDone;

    /*
     * number of iterations of list processing task that
     * instance is in delete state
     */
    UInt16 stateDeleteCount;

    /*
     * VIP resource object
     */
    Vcore_VipResObj resObj;

    /*
     * VIP resource allocation parameters
     */
    Vcore_VipResAllocParams resAllocParams;

    /* VPDMA channel ID */
    VpsHal_VpdmaChannel multiChDummyDescVchannelId;

    Bool resetStatistics;

    UInt8   *listAddr;
    /** Pointer to the start of the list for this capture object */
    UInt32   listSize;
    /**< Size of the list pointed by listAddr */

    volatile UInt32   descMask;
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

    /* These variables would be initialized at initilization time
       and used across */
    Vps_PlatformCpuRev cpuRev;
    /**< Version of the silicon being used */
    Vps_PlatformId platform;
    /**< Current pltform */
    FVID2_BufferFormat frameCapture;
    /**< Capture frames instead or fields. Applicable to interlaced capture.

        If this parameters is true application will be able to capture
        both the fields of interlaced capture as a single frame using a
        single FVID2_queue call. If parameter is false application
        will be able to capture fields for each FVID2_queue call. Application
        needs to allocate buffer twice the size than field capture if this
        parameter is set to true.
     */
    UInt32 returnErrFrm;
     /**< Caputre driver queues back error frame to driver empty queue and
       *  does not return err frame to application. With this IOCTL capture
       *  driver will return err frame to application with error status
       *  flagged. This is required for Linux V4L2 where frame queued and
       *  dequeued should not go out of sync
       */
    UInt32 oneCallBackPerFrm;
    /**< This is to make sure we return only 1 frame at a time from capture
     * driver during dequeue call and we give 1 call back per completed frame
     * for instance object.
     */
    VpsHal_ScFactorConfig curScFactor;
    /**< Currently configured scaling factor for this instance */
} Vps_CaptObj;

/*
  Global/Common driver object
*/
typedef struct
{
    /*
     * list processing task wakeup semaphore
     */
    Semaphore_Handle semListUpdate;

    /*
     * list processing task handle
     */
    Task_Handle tskListUpdate;

    /*
     * clock ticks elasped since last semaphore post
     */
    UInt32 clockTicks;

    /*
     * FVID2 driver ops
     */
    FVID2_DrvOps fvidDrvOps;

    /*
     * capture instance objects
     */
    Vps_CaptObj captureObj[VPS_CAPT_INST_MAX];

    /*
     * user channel number to driver channel number map
     */
    UInt8  fvidChannelNum2lChannelNumMap
        [VPS_CAPT_STREAM_ID_MAX * VPS_CAPT_CH_PER_PORT_MAX * VPS_CAPT_INST_MAX];

    /*
     * exit list processing task
     */
    UInt32 exitListTsk;


    UInt32 descParseErrorCount;
    UInt32 resetCount;
    UInt32 descMissFound;

    Void   *dropDataAddr;
    /* Flag to disable or not to disable VIP Ports on Overflow*/
    UInt32 disPortonOverflow;

} Vps_CaptCommonObj;

typedef struct
{
  UInt32 REG_ADDR;
  UInt32 NUM_REGS;
  UInt32 RSV0[2];
  UInt32 REG_VAL[4];
} Vps_captVipDataPathCfgOverlay;

#define VPS_CAPT_VIP_CFG_MAX_MUX     (15u)

/*
 * Subframes field / frame queue object. For each field / frame that is
 * scheduled for capture, an instance of this strucutres tracks required stats
 * of that field / frame thats being captured or scheduled for capture.
 */
struct Vps_CaptSubFrameQObj_t {
    /*
     * Used to keep track if the frame described by this instance is valid
     * and can be returned to applications. Valid values are FVID2_SOK and
     * FVID2_EFAIL.
     * Default FVID_SOK
     */
    Int32 status;

    /*
     * Place holder to track the current frame under reception.
     */
    FVID2_Frame *pFrame;

    /*
     * Channel for which this frame is being captured.
     */
    Vps_CaptChObj *pChObj;

    /*
     * Applications handle to this drivers instance
     */
    FVID2_Handle handle;

    /*
     * Total number lines of video captured for this frame, On primary channel.
     * Luma in case of YUV data type and Green in case of RGB.
     * Assuming that Green carries the sync on channel
     * Default 0x0
     */
    UInt32 totalNoOfLinesPChan;

    /*
     * Total number lines of video captured for this frame, On secondary channel
     * Chroma in case of YUV data type and Blue in case of RGB.
     * Assuming that Green carries the sync on channel
     * Default 0x0
     */
    UInt32 totalNoOfLinesSChan;

    /*
     * Allocation status
     */
    UInt32 allocStatus;

    /*
     * subFrameCounter
     */
    UInt32 subFrameNo;
};


/* global's */
extern Vps_CaptCommonObj gVps_captCommonObj;

/* function's */

Int32 Vps_captCreateQueues ( Vps_CaptObj * pObj );
Int32 Vps_captCreateFrames ( Vps_CaptObj * pObj );
Int32 Vps_captCreateChObj ( Vps_CaptObj * pObj );

Int32 Vps_captDeleteQueues ( Vps_CaptObj * pObj );
Int32 Vps_captDeleteFrames ( Vps_CaptObj * pObj );
UInt32 Vps_captIsDataAvailable ( Vps_CaptObj * pObj );
UInt32 Vps_captIsDataAvailabeInOutQue1 ( Vps_CaptObj * pObj );

Int32 Vps_captCreateVip ( Vps_CaptObj * pObj );
Int32 Vps_captStartVip ( Vps_CaptObj * pObj );
Int32 Vps_captStopVip ( Vps_CaptObj * pObj );
Int32 Vps_captDeleteVip ( Vps_CaptObj * pObj );
Int32 Vps_captSetFrameStartEvent ( Vps_CaptObj * pObj );

Int32 Vps_captInitList (  );
Int32 Vps_captDeInitList (  );
Void Vps_captIsrListComplete (  );
Void Vps_captTskListUpdate ( UArg arg1, UArg arg2 );
Int32 Vps_captStartList ( Vps_CaptObj * pObj );
Int32 Vps_captStopList ( Vps_CaptObj * pObj );
Int32 Vps_captAddToList ( Vps_CaptObj * pObj, Vps_CaptChObj * pChObj );
Int32 Vps_captAddAbortInList ( Vps_CaptObj * pObj, Vps_CaptChObj * pChObj );
Int32 Vps_captUpdateListDesc ( Vps_CaptObj * pObj,
                               Vps_CaptLmDataDescParseInfo * parseDescInfo,
                               Vps_CaptLmDataDescUpdateInfo * updateDescInfo,
                               int timestamp );

Int32 Vps_captSetFrameSkip ( Vps_CaptObj * pObj,
                             Vps_CaptFrameSkip * frameSkip );

Int32 Vps_captVipSetScParams ( Vps_CaptObj * pObj,
                                  Vps_CaptScParams * scParams );

Int32 Vps_captGetChStatus ( Vps_CaptObj * pObj, Vps_CaptChGetStatusArgs * args,
                            Vps_CaptChStatus * chStatus );

Int32 Vps_captLock ( Vps_CaptObj * pObj );
Int32 Vps_captUnlock ( Vps_CaptObj * pObj );

Int32 Vps_captLockCreate ( Vps_CaptObj * pObj );
Int32 Vps_captLockDelete ( Vps_CaptObj * pObj );

Int32 Vps_captFreePath ( Vps_CaptObj * pObj );
Int32 Vps_captAllocPath ( Vps_CaptObj * pObj );

Int32 Vps_captVipBlockReset(Vps_CaptObj *pObj);
Int32 Vps_captVipPortAndModulesReset(Vps_CaptObj *pObj);

Int32 Vps_captCheckOverflow(Vps_CaptOverFlowStatus *overFlowStatus);
Int32 Vps_captResetAndRestart(Vps_CaptOverFlowStatus *overFlowStatus);

/**
  \brief Return pointer to scaler coeff memory for a given VIP instance

  \param vipInstId  [IN] VIP inst 0 or 1

  \return pointer to scaler coeff memory for a given VIP instance
*/
Ptr Vps_captGetScCoeffMem(UInt32 vipInstId);

/* Static Function Declaration */
Int32 Vps_captTskSubmitFieldList();
Int32 Vps_captTskSubmitFrameList();
Int32 Vps_captMakeList(Vps_CaptObj *pObj);
Int32 Vps_captTskListDoVipStart();
Int32 Vps_captTskUpdateList();
Int32 Vps_captInitializeList(Vps_CaptObj *pObj);
Int32 Vps_captTskListDoCallback();
Int32 Vps_CaptTskPutCaptField(Vps_CaptObj *pObj,
                                    Vps_CaptChObj *pChObj,
                                    Vps_CaptLmDataDescParseInfo *parseDescInfo);
Int32 Vps_CaptTskPutCaptFrm(Vps_CaptObj *pObj,
                                    Vps_CaptChObj *pChObj,
                                    Vps_CaptLmDataDescParseInfo *parseDescInfo);
Int32 Vps_CaptTskUpdateFrm(Vps_CaptObj *pObj,
                                  Vps_CaptChObj *pChObj,
                                  Vps_CaptLmDataDescParseInfo *parseDescInfo);
Void Vps_captProgInitialFrms(Vps_CaptObj *pObj,
                                  Vps_CaptChObj *pChObj,
                                  Vps_CaptLmDataDescParseInfo *parseDescInfo);
Int32 Vps_CaptTskUpdateField(Vps_CaptObj *pObj,
                                  Vps_CaptChObj *pChObj,
                                  Vps_CaptLmDataDescParseInfo *parseDescInfo);
Void Vps_captTskListAddMagicNum(UInt8 *addr, UInt32 pitch);
Int32 Vps_captTskListCheckMagicNum(UInt8 *addr, UInt32 pitch, UInt32 channelNum);
Bool Vps_captIsDisSyncStartSequenceNeeded(Vps_CaptObj *pObj);
Int32 Vps_captTskListDoStop();
Int32 Vps_captTskMvDescTempToEmpty(Vps_CaptObj *pObj);
Void Vps_captUpdateStatitics(Vps_CaptObj *pObj,
                                    Vps_CaptChObj *pChObj,
                                    Vps_CaptLmDataDescParseInfo *parseDescInfo);
Void Vps_captResetStatitics(Vps_CaptObj *pObj, Vps_CaptChObj *pChObj);
Int32 Vps_captAddToList(Vps_CaptObj *pObj, Vps_CaptChObj *pChObj);
Int32 Vps_captDoDisSyncStartSequence(Vps_CaptObj *pObj);
UInt8 *Vps_captGetListAddr(Vps_CaptObj *pObj);

Int32 Vps_captFlush( Fdrv_Handle handle, FVID2_FrameList * frameList );
Int32 Vps_captVipReset(Fdrv_Handle handle);
Int32 Vps_captPostAbortList(Vps_CaptObj *pObj);

Void Vps_captVipWaitForComplete(Vps_CaptObj *pObj);
Int32 Vps_captSetStorageFmt(Fdrv_Handle handle,
                            const Vps_CaptStorageParams *storagePrms);
Int32 Vps_captSetVipCropCfg(Fdrv_Handle handle,
                            const Vps_CaptVipCropParams *vipCropPrms);
Int32 Vps_captGetVipCropCfg(Fdrv_Handle handle,
                            Vps_CaptVipCropParams *vipCropPrms);
Int32 Vps_returnErrFrm(Fdrv_Handle handle, Vps_CaptReturnErrFrms *cmdArgs);
Int32 Vps_oneCbPerFrm(Fdrv_Handle handle, Vps_CaptOneCallBackPerFrm *cmdArgs);

static inline Bool Vps_captIsSingleChDiscSyncMode(UInt32 videoCaptureMode)
{
    Bool isDiscSyncMode = FALSE;

    if ((videoCaptureMode ==
            VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VBLK) ||
        (videoCaptureMode ==
            VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC) ||
        (videoCaptureMode ==
            VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK) ||
        (videoCaptureMode ==
            VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC))
    {
        isDiscSyncMode = TRUE;
    }

    return (isDiscSyncMode);
}

Bool Vps_captSubFrameIsEnabled ( Vps_CaptObj *pObj );
Int32 Vps_captVipSubFrameUnRegisterIsr ( Vps_CaptObj * pObj );
Int32 Vps_captVipSubFrameRegisterIsr ( Vps_CaptObj * pObj );
void Vps_CaptOverflowCallback( );
Int32 Vps_captTimeStampAddElem( Vps_CaptChObj * pChObj, FVID2_Frame *pFrame );
Int32 Vps_captTimeStampFlushQue( Vps_CaptChObj * pChObj );
Int32 Vps_captTimeStampCfgEofInt( Vps_CaptObj * pObj, UInt32 enable );
Int32 Vps_captTimeStampStatsUpdate( Vps_CaptChObj * pChObj, UInt32 currTs );
Vps_CaptSubFrameQObj * Vps_captVipSubFrameAddQObj( Vps_CaptChObj *pChObj,
                                                   FVID2_Frame *pFrame);

#endif /*  _VPS_CAPT_PRIV_H_  */
