/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*
    Channel Numbers in capture link
    ===============================

    Capture driver needs that 'channelNum' be unique across all capture
    driver instances.

    Capture link requires that 'channelNum' of frames put in the output
    queue start from 0 and go upto ((number of channels in que) - 1).

    This is done so that the subsequent link need not have to do any channel
    number remapping and complexity of channel number remapping is retricted
    within the capture link.

    Due to this it could happen that 'channelNum' of frame returned by
    capture driver could be, say, 0xNN, and 'channelNum' of the same frame
    when put in output queue needs to be, say, 0xMM.

    This 'channelNum' remapping between capture driver and capture link output
    queue is done as below.

    For a given capture driver channelNum,
    - first 4bits (CAPTURE_LINK_MAX_CH_PER_OUT_QUE) is CH ID of channel in QUE
      and
    - rest of the bits are output queue number 0..CAPTURE_LINK_MAX_OUT_QUE-1

    When a frame is put in the output queue only the CH ID of channel in QUE is
    retained in the FVID2_Frame

    When a frame is returned back to the capture driver this CH ID is combined
    with QUE ID to form 'channelNum' for capture driver.

    Examples,

    For 4CH per port input and single output queue channelNum will be as follows

    Capture driver              Output      Output Que
    channelNum                  QUE ID      channelNum
    0           CH0 VIP0 PortA  0           0
    1           CH1 VIP0 PortA  0           1
    2           CH2 VIP0 PortA  0           2
    3           CH3 VIP0 PortA  0           3
    4           CH0 VIP0 PortB  0           4
    5           CH1 VIP0 PortB  0           5
    6           CH2 VIP0 PortB  0           6
    7           CH3 VIP0 PortB  0           7
    8           CH0 VIP1 PortA  0           8
    9           CH1 VIP1 PortA  0           9
    10          CH2 VIP1 PortA  0           10
    11          CH3 VIP1 PortA  0           11
    12          CH0 VIP1 PortB  0           12
    13          CH1 VIP1 PortB  0           13
    14          CH2 VIP1 PortB  0           14
    15          CH3 VIP1 PortB  0           15

    For 1CH input and 2 outputs per port and 4 output queue channelNum will be as follows

    Capture driver                          Output      Output Que
    channelNum                              QUE ID      channelNum
    0           CH0 VIP0 PortA - output 0     0           0
    16          CH0 VIP0 PortA - output 1     1           0
    32          CH0 VIP1 PortA - output 0     2           0
    64          CH0 VIP1 PortA - output 1     3           0

    For 1CH input and 2 outputs per port and 1 output queue channelNum will be as follows

    Capture driver                          Output      Output Que
    channelNum                              QUE ID      channelNum
    0           CH0 VIP0 PortA - output 0     0           0
    1           CH0 VIP0 PortA - output 1     0           1
    2           CH0 VIP1 PortA - output 0     0           2
    3           CH0 VIP1 PortA - output 1     0           3

    For 1CH input and 2 outputs per port and 2 output queue channelNum will be as follows

    Capture driver                          Output      Output Que
    channelNum                              QUE ID      channelNum
    0           CH0 VIP0 PortA - output 0     0           0
    1           CH0 VIP0 PortA - output 1     0           1
    16          CH0 VIP1 PortA - output 0     1           0
    17          CH0 VIP1 PortA - output 1     1           1

*/
#ifndef _CAPTURE_LINK_PRIV_H_
#define _CAPTURE_LINK_PRIV_H_

#include <ti/psp/examples/common/vps/chains/links/captureLink.h>
#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>
#include <ti/sysbios/utils/Load.h>

#ifdef PLATFORM_EVM_SI
#define CAPTURE_LINK_TSK_TRIGGER_COUNT   (2)
#else
#define CAPTURE_LINK_TSK_TRIGGER_COUNT   (4)
#endif

#define CAPTURE_LINK_OUT_SIZE_SWITCH_FREQ (200)

#define CAPTURE_LINK_TMP_BUF_SIZE        (1920*1080*2)

#define CAPTURE_LINK_HEIGHT_PAD_LINES    (48)

#define CAPTURE_LINK_RAW_VBI_LINES       (40)

/* frame allocated per channel - new capture approach needs more frames */
#define CAPTURE_LINK_FRAMES_PER_CH     (SYSTEM_LINK_FRAMES_PER_CH+2)

/* Worst case frames per handle */
#define CAPTURE_LINK_MAX_FRAMES_PER_HANDLE \
    (VPS_CAPT_STREAM_ID_MAX* \
     VPS_CAPT_CH_PER_PORT_MAX* \
      CAPTURE_LINK_FRAMES_PER_CH \
    )


/* make capture driver channelNum */
static inline UInt32 CaptureLink_makeChannelNum(UInt32 queId, UInt32 queChId, UInt32 numChPerQue)
{
    return numChPerQue * queId + queChId;
}

/* extract que ID from capture driver channelNum */
static inline UInt32 CaptureLink_getQueId(UInt32 channelNum, UInt32 numChPerQue)
{
    return channelNum / numChPerQue;
}

/* extract que CH ID from capture driver channelNum */
static inline UInt32 CaptureLink_getQueChId(UInt32 channelNum, UInt32 numChPerQue)
{
    return channelNum % numChPerQue;
}

/*
  Driver instance information
*/
typedef struct
{

    /*
     * VIP capture driver Instance ID
     */
    UInt32 instId;

    /*
     * Capture driver create time parameters
     */
    Vps_CaptCreateParams createArgs;

    /*
     * Create status returned by capture driver during FVID2_create()
     */
    Vps_CaptCreateStatus createStatus;

    /*
     * Capture driver Callback params
     */
    FVID2_CbParams cbPrm;

    /*
     * Capture driver FVID2 handle
     */
    FVID2_Handle captureVipHandle;

    /*
     * FVID2 Frames that will be used for capture
     */
    FVID2_Frame frames[CAPTURE_LINK_MAX_FRAMES_PER_HANDLE];

    /*
     * Run-time parameter structure for every frame
     * One run-time parameter is associated to one FVID2_Frame during link create
     */
    System_FrameInfo frameInfo[CAPTURE_LINK_MAX_FRAMES_PER_HANDLE];

    /*
     * Video decoder device FVID2 handle
     */
    FVID2_Handle videoDecoderHandle;

    Vps_VipPortConfig vipPortCfg;

    /* Video decoder device create args */
    Vps_VideoDecoderCreateParams vidDecCreateArgs;

    /* Video decoder device create status */
    Vps_VideoDecoderCreateStatus vidDecCreateStatus;

    /* Video decoder device video mode setup args */
    Vps_VideoDecoderVideoModeParams vidDecVideoModeArgs;
} CaptureLink_InstObj;

/* Capture link information */
typedef struct
{
    /* Capture link task */
    VpsUtils_TskHndl tsk;

    /* Capture link create arguments */
    CaptureLink_CreateParams createArgs;

    /* Capture link output queues */
    VpsUtils_BufHndl bufQue[CAPTURE_LINK_MAX_OUT_QUE];

    /* Global capture driver handle */
    FVID2_Handle fvidHandleVipAll;

    /* Max possible width x height across all channels
        Capture buffers are allocated for this width x height
    */
    UInt32 maxWidth;
    UInt32 maxHeight;

    /* Capture driver instance specific information */
    CaptureLink_InstObj instObj[VPS_CAPT_INST_MAX];

    /* Capture link info that is returned when queried by next link */
    System_LinkInfo info;

    /* capture link run-time statistics for debug */
    UInt32 captureDequeuedFrameCount;
    UInt32 captureQueuedFrameCount;
    UInt32 cbCount;
    UInt32 cbCountServicedCount;
    UInt32 captureFrameCount[CAPTURE_LINK_MAX_OUT_QUE][CAPTURE_LINK_MAX_CH_PER_OUT_QUE];

    UInt32 startTime;
    UInt32 prevTime;
    UInt32 prevFrameCount;
    UInt32 exeTime;

    UInt32 totalCpuLoad;
    Uint32 cpuLoadCount;

    UInt8  *tmpBufAddr;

    Bool   enableCheckOverflowDetect;

    UInt32 resetCount;
    UInt32 resetTotalTime;
    UInt32 prevResetTime;

    Bool isPalMode;

    Int32 brightness;
    Int32 contrast;
    Int32 saturation;

    UInt32 numChPerQue;

    /* Holds the frame count at which last output size switch was done. */
    UInt32 outSizeSwitchFrameCnt;
} CaptureLink_Obj;

extern CaptureLink_Obj gCaptureLink_obj;

System_LinkInfo *CaptureLink_getInfo(VpsUtils_TskHndl *pTsk);
Int32 CaptureLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList);
Int32 CaptureLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList);

Int32 CaptureLink_drvCreate(CaptureLink_Obj *pObj, CaptureLink_CreateParams *pPrm);
Int32 CaptureLink_drvDetectVideo(CaptureLink_Obj *pObj, UInt32 timeout);
Int32 CaptureLink_drvStart(CaptureLink_Obj *pObj);
Int32 CaptureLink_drvProcessData(CaptureLink_Obj *pObj);
Int32 CaptureLink_drvStop(CaptureLink_Obj *pObj);
Int32 CaptureLink_drvDelete(CaptureLink_Obj *pObj);
Int32 CaptureLink_drvAllocAndQueueFrames ( CaptureLink_Obj *pObj,
                                           CaptureLink_InstObj * pDrvObj,
                                           UInt32 frameCaputreMode);
Int32 CaptureLink_drvFreeFrames ( CaptureLink_Obj *pObj,
                                  CaptureLink_InstObj * pDrvObj,
                                  UInt32 frameCaputreMode);

UInt32 CaptureLink_drvIsDataFormatTiled ( Vps_CaptCreateParams * createArgs,
                                    UInt16 streamId );
Int32 CaptureLink_drvInitCreateArgs ( Vps_CaptCreateParams * createArgs );

Int32 CaptureLink_drvPutEmptyFrames(CaptureLink_Obj *pObj, FVID2_FrameList *pFrameList);
Int32 CaptureLink_getCpuLoad();

Int32 CaptureLink_drvPrintStatus(CaptureLink_Obj *pObj);
Int32 CaptureLink_drvOverflowDetectAndReset(CaptureLink_Obj *pObj, Bool doForceReset);

Int32 CaptureLink_drvDetectVideoStandard(CaptureLink_Obj *pObj);

Int32 CaptureLink_drvSetColor(CaptureLink_Obj *pObj, Int32 contrast, Int32 brightness, Int32 saturation);

#endif
