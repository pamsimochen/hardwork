/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _NSF_APP_H_
#define _NSF_APP_H_


#include <string.h>
#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/utils/Load.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/vps_m2mNsf.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/utility/vpsutils.h>
/*
  Stack size of test application
*/
#define NSF_APP_TSK_STACK_MAIN    (10*1024)

/*
  Task priority of test application
*/
#define NSF_APP_TSK_PRI_MAIN      (10)

/*
  Maximum frames to allocate per channel in test application
*/
#define NSF_APP_MAX_FRAMES_PER_CH  (2)


#define NSF_TEST_FRAME_PITCH       (1920) /* in pixels */
#define NSF_TEST_FRAME_HEIGHT      (1088) /* in lines */

/* Test frame size in bytes */
#define NSF_TEST_FRAME_SIZE        (NSF_TEST_FRAME_PITCH*NSF_TEST_FRAME_HEIGHT*2)

#define NSF_TEST_NUM_FRAMES        (NSF_APP_MAX_FRAMES_PER_CH)

/*
  Print all test application logs'
*/
//#define NSF_APP_PRINT_ALL
/* TODO TNF bypass is not working */
/*#define NSFAPP_TNF_BYPASS_TEST */

/*
  When defined Run all test cases present in gNsfApp_utParams[]
  Else only run first test case from gNsfApp_utParams[]
*/
#define NSFAPP_TEST_ALL

/* Enable this to run tiler examples */
#define NSFAPP_TEST_TILER

/*
  Channel information
*/
typedef struct
{

    /*
     * Input frame information
     */
    FVID2_Frame inFrame[NSF_APP_MAX_FRAMES_PER_CH];

    /*
     * Output frame information
     */
    FVID2_Frame outFrame[NSF_APP_MAX_FRAMES_PER_CH];

    /*
     * Frame buffer memory base address
     */
    Ptr memBaseAddr;

    /*
     * Total size of allocation memory
     */
    UInt32 memSizeTotal;

    /*
     * Current frame index that is being processed
     */
    UInt32 curFrame;

} NsfApp_ChObj;

/*
  NSF driver request information
*/
typedef struct
{
    /*
     * Input frame list
     * 0 - for input frames
     * 1 - for previous output frames
     */
    FVID2_FrameList inFrameList[2];

    /*
     * Output frame list
     * 0 - for output frames
     */
    FVID2_FrameList outFrameList[1];

    /*
     * Process list thats submitted to driver in this request
     */
    FVID2_ProcessList processList;

} NsfApp_ReqObj;

/*
  NSF driver application object
  Contains info specific to a NF driver handle
*/
typedef struct
{

    /*
     * NF driver handle ID
     */
    UInt32 handleId;

    /*
     * NF driver FVID2 handle
     */
    FVID2_Handle fvidHandle;

    /*
     * Create time arguments
     */
    Vps_NsfCreateParams createArgs;

    /*
     * data format per CH
     */
    Vps_NsfDataFormat dataFormat[VPS_NSF_MAX_CH_PER_HANDLE];

    /*
     * NF processing params per CH
     */
    Vps_NsfProcessingCfg processingCfg[VPS_NSF_MAX_CH_PER_HANDLE];

    /*
     * Create status
     */
    Vps_NsfCreateStatus createStatus;

    /*
     * FVID2 callback for this handle
     */
    FVID2_CbParams cbPrm;

    /*
     * Channel information
     */
    NsfApp_ChObj chObj[VPS_NSF_MAX_CH_PER_HANDLE];

    /*
     * Request information
     */
    NsfApp_ReqObj reqObj[NSF_APP_MAX_FRAMES_PER_CH];

    /*
     * Current request obj that is being submitted
     */
    Int32 curReq;

    /*
     * All frame processing completion semaphore
     */
    Semaphore_Handle semAllComplete;

} NsfApp_DrvObj;

/*
  Unit test parameters
*/
typedef struct
{

    /*
     * Number of handle in test case
     */
    UInt32 numHandles;

    /*
     * Number of channels in test case
     */
    UInt32 numCh;

    /*
     * Number of secs the test case should run
     */
    UInt32 runCount;

    /*
     * TRUE: Update NF processing parameters at run-time via IOCTL
     * FALSE: Do not update parameters at run time
     */
    UInt32 updateParamsRt;

    /*
     * NF mode, TNF+SNF or TNF only or SNF only or Chroma downsample only
     */
    Vps_NsfBypassMode bypassMode;

    /* Use Tiler memory for NSF output */
    Bool useTiler;

} NsfApp_UtParams;

/*
  test application control data structure
*/
typedef struct
{

    /*
     * Test application task handle
     */
    Task_Handle tskMain;

    /*
     * Current unit test case parameters
     */
    NsfApp_UtParams curUtParams;

    /*
     * NF Handle information
     */
    NsfApp_DrvObj drvObj[VPS_NSF_MAX_HANDLES];

    /*
     * Run-time statistics
     */
    UInt32 totalFrames;
    UInt32 totalPixels;
    UInt32 totalCpuLoad;
    UInt32 cpuLoadCount;

    /* input frame buffer */
    UInt8 *inMemAddr;

    VpsUtils_PrfTsHndl      *prfTsHandle;

    /* Paltform and CPU version */
    Vps_PlatformCpuRev      cpuVer;
    Vps_PlatformId          platform;

} NsfApp_Ctrl;

Int32 NsfApp_initCreateArgs ( Vps_NsfCreateParams * createArgs );

#endif
