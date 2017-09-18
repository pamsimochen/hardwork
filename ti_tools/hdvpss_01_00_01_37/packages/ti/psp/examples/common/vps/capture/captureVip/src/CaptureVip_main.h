/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DRV_VIP_CAPTURE_H_
#define _DRV_VIP_CAPTURE_H_


#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/psp/vps/vps_capture.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/devices/vps_videoDecoder.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/utility/vpsutils.h>

/*
  comment this to disable debug logs
*/
//#define CAPTURE_APP_DEBUG_LOG

/*
  if defined execute all capture test cases from gCaptureApp_utParams[]
  else excute only 0th test case from gCaptureApp_utParams[]
*/
#define CAPTUREAPP_TEST_ALL

/*
  test application stack size
*/
#define CAPTURE_APP_TSK_STACK_MAIN    (10*1024)

/*
  test application task priority
*/
#define CAPTURE_APP_TSK_PRI_MAIN      (10)

/* frame allocated per channel  */
#define CAPTURE_APP_FRAMES_PER_CH     (8)

/* Padding required for allocating extra memory for 24 lines
  * Else capture in discreet sync mode will overrite the meta
  * data maintained by bios
  */
#define CAPTURE_APP_PADDING_LINES       (24u)

/* Worst case frames per handle */
#define CAPTURE_MAX_FRAMES_PER_HANDLE \
    (VPS_CAPT_STREAM_ID_MAX* \
     VPS_CAPT_CH_PER_PORT_MAX* \
      CAPTURE_APP_FRAMES_PER_CH \
    )

/* SC enable/disable for different streams  */
#define CAPTURE_SC_DISABLE_ALL  (0x0)
#define CAPTURE_SC_ENABLE_0     (0x1)
#define CAPTURE_SC_ENABLE_1     (0x2)
#define CAPTURE_SC_ENABLE_ALL   (CAPTURE_SC_ENABLE_0|CAPTURE_SC_ENABLE_1)

/* Default run count in secs */
#define CAPTURE_APP_RUN_COUNT   (10)

/* Default loop count */
#define CAPTUREAPP_TEST_LOOP_COUNT  (1)

#define FIELD_MERGED_CAPTURE_DISABLE        (FALSE)
#define FIELD_MERGED_CAPTURE_ENABLE         (TRUE)

#define CAPTURE_SUBFRAME_COUNT  (700)

typedef struct
{
    UInt32 enable[3];
    UInt32 nLines[3];

}captApp_subframeParams;

/*
  Unit test parameters

  The test case execution happens based on values of this structure
*/
typedef struct
{
    /* video decoder Id */
    UInt32 videoDecoderId;

    /*
     * Number of capture handles to run with
     */
    UInt16 numHandles;

    /*
     * Capture mode, single channel, multi-channel etc
     */
    Vps_CaptVideoCaptureMode captureMode;

    /*
     * Number of channel in multi-channel case, msut be 1 for single channel
     */
    UInt16 numCh;

    /*
     * Number of time to call dequeue for this test execution
     */
    Int32 runCount;

    /*
     * 8/16/34-bit mode
     */
    Vps_CaptVideoIfMode videoIfMode;

    /*
     * YUV or RGB data format
     */
    UInt32 inDataFormat;

    /*
     * Number of outputs
     */
    UInt16 numOutputs;

    /*
     * Output format of each output
     */
    UInt32 outFormat[3];

    /*
     * enable scaler in capture path
     * CAPTURE_SC_DISABLE_ALL: SC Disabled,
     * CAPTURE_SC_ENABLE_0: SC enabled for stream 0
     * CAPTURE_SC_ENABLE_1: SC enable for stream 1
     * CAPTURE_SC_ENABLE_ALL: SC enabled for stream 0 and 1
     */
    UInt16 scEnable;

    /*
     * frame skip mask for sequence of 30frames
     * bit0..bit29: bitX = 1, skip frame, else capture frame
     */
    UInt32 frameSkipMask;

    UInt32 testNum;

    /*
     * Capture frames or fields for interlaced capture
     */
    UInt32 fieldMerged;

    /*
     * Sub frame configurations
     */
    captApp_subframeParams subFrameCfg;

} CaptureApp_UtParams;

/*
  Driver instance information
*/
typedef struct
{

    /*
     * Instance ID
     */
    UInt32 instId;

    /*
     * Create time parameters
     */
    Vps_CaptCreateParams createArgs;

    /*
     * Create status returned by driver during FVID2_create()
     */
    Vps_CaptCreateStatus createStatus;

    /*
     * Callback params
     */
    FVID2_CbParams cbPrm;

    /*
     * FVID2 handle
     */
    FVID2_Handle fvidHandle;

    /*
     * FVID2 Frames that will be used for capture
     */
    FVID2_Frame frames[CAPTURE_MAX_FRAMES_PER_HANDLE];

    /*
     * Memory for sub frame information
     */
    FVID2_SubFrameInfo subFrameInfo[CAPTURE_MAX_FRAMES_PER_HANDLE];

    /*
     * Run-time parameter structure for every frame
     */
    Vps_CaptRtParams rtParams[CAPTURE_MAX_FRAMES_PER_HANDLE];

    /*
     * FVID2 handle
     */
    FVID2_Handle videoDecoderHandle;

    Vps_VipPortConfig vipPortCfg;
    Vps_VideoDecoderCreateParams vidDecCreateArgs;
    Vps_VideoDecoderCreateStatus vidDecCreateStatus;
    Vps_VideoDecoderVideoModeParams vidDecVideoModeArgs;

} CaptureApp_DrvObj;

/*
  Test application control data structure
*/
typedef struct
{

    /*
     * Driver objects
     */
    CaptureApp_DrvObj drvObj[VPS_CAPT_INST_MAX];

    /*
     * Current test case parameters
     */
    CaptureApp_UtParams utParams;

    /*
     * Global VIP capture handle
     */
    FVID2_Handle fvidHandleVipAll;

    /*
     * Test task handle
     */
    Task_Handle tskMain;

    /*
     * Error count for this test execution
     */
    UInt32 errorCount;

    /*
     * count of all frames captured
     */
    UInt32 totalFieldCount;

    UInt32 totalCpuLoad;
    UInt32 cpuLoadCount;

    Vps_PlatformBoardId boardId;
    UInt32 platformId, cpuRev;

    UInt32 maxWidth;
    UInt32 maxHeight;

    UInt32 overflowCount[VPS_CAPT_INST_MAX];
    UInt32 totalOverflowCount;

    FVID2_Frame subFrames[CAPTURE_SUBFRAME_COUNT];
    FVID2_SubFrameInfo subFrameInfo[CAPTURE_SUBFRAME_COUNT];
    UInt32 subFrameIdx;

    UInt32 subFrame_YUV422I_counter;
    UInt32 subFrame_YUV422SP_counter;
    UInt32 subFrame_RGB_counter;
    
    UInt32 avgMinTimeDiff;
    UInt32 avgMaxTimeDiff;
    UInt32 lastTs;
    UInt32 errTs;

} CaptureApp_Ctrl;

Int32 CaptureApp_initCreateArgs(CaptureApp_DrvObj *pDrvObj);
Int32 CaptureApp_allocAndQueueFrames ( CaptureApp_DrvObj * pDrvObj,
                                        UInt32 fieldMerged);
Int32 CaptureApp_freeFrames ( CaptureApp_DrvObj * pDrvObj, UInt32 fieldMerged);
UInt32 CaptureApp_isDataFormatTiled ( Vps_CaptCreateParams * createArgs,
                                    UInt16 streamId );

#endif
