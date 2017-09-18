/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/sysbios/BIOS.h>
#include "M2mNsf_main_subframe.h"

/* Test application stack */
#pragma DATA_ALIGN(gNsfApp_tskStackMain, 32)
#pragma DATA_SECTION(gNsfApp_tskStackMain, ".bss:taskStackSection")
UInt8 gNsfApp_tskStackMain[NSF_APP_TSK_STACK_MAIN];

/* NF application control data structure  */
NsfApp_Ctrl gNsfApp_ctrl;

Semaphore_Handle gAppCompleteSem;

UInt16 NsfApp_testId = 0;

/*
  Width x height for all channels for handle

  { width, height }, ....
*/
UInt16 gNsfApp_chWidthHeightList[32][2] = {
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
    {1920, 1080} , {1920, 1080} , {1920, 1080} , {1920, 1080},
};


/*
  Unit test cases that will be executed by the test application
*/
NsfApp_UtParams gNsfApp_utParams[] = {
/*
    Number of handles  ,
    Number of channels ,
    Run count in secs  ,
    Update RT Params   ,
    NSF bypass mode    ,
    Use Tiler
*/
    {1,  1,  10, FALSE, VPS_NSF_DISABLE_NONE     , FALSE},

#ifdef NSFAPP_TEST_TILER

    {1,  1,  10, FALSE, VPS_NSF_DISABLE_NONE     , TRUE},

#endif
};


/*
  Driver complete callback
*/
Int32 NsfApp_drvObjCompleteCallback ( FVID2_Handle handle,
                                      Ptr appData, Ptr reserved )
{

    Semaphore_post ( gAppCompleteSem );

    return 0;

}

/*
  Make a request for submission to driver

  drvObj - Driver handle information
  reqObj - Request information thats created inside this function
  count  - current iteration of test case
  reqId  - req obj ID
  isLast - TRUE if this is the last requested that will be
           submitted to the driver
*/
Int32 NsfApp_drvObjMakeReq ( NsfApp_DrvObj * drvObj, NsfApp_ReqObj * reqObj)
{
    UInt32 chId;
    Int32 prevOutFrameId, numFramesInReq;
    NsfApp_ChObj *chObj;
    FVID2_Frame **pInFrames, **pOutFrames, **pPrevOutFrames;

    /*
     * A request consists of frame from every channel.
     * Note, this is just the way test case is written, as such a reqeust can
     * mix of requests for same or different channels and that too in any order
     */
    numFramesInReq = drvObj->createArgs.numCh;


    /*
     * Point to frames in frame list
     */
    pInFrames = reqObj->inFrameList[0].frames;
    pPrevOutFrames = reqObj->inFrameList[1].frames;
    pOutFrames = reqObj->outFrameList[0].frames;

    /*
     * for each channel in request obj do ...
     */
    for ( chId = 0; chId < numFramesInReq; chId++ )
    {
        /*
         * Set input and output frames
         */

        chObj = &drvObj->chObj[chId];

        pInFrames[chId] = &chObj->inFrame[chObj->curFrame];
        pOutFrames[chId] = &chObj->outFrame[chObj->curFrame];


        /*
         * set previous output frame
         */
        prevOutFrameId = chObj->curFrame - 1;
        if ( prevOutFrameId < 0 )
            prevOutFrameId = NSF_APP_MAX_FRAMES_PER_CH - 1;

        pPrevOutFrames[chId] = &chObj->outFrame[prevOutFrameId];

        /*
         * Increment next free frame Idx for that channel
         */
        chObj->curFrame = ( chObj->curFrame + 1 ) % NSF_APP_MAX_FRAMES_PER_CH;
    }

    /*
     * set number of frames in request
     */
    reqObj->inFrameList[0].numFrames = numFramesInReq;
    reqObj->inFrameList[1].numFrames = numFramesInReq;
    reqObj->outFrameList[0].numFrames = numFramesInReq;

    /* setup process list pointers  */
    reqObj->processList.inFrameList[0] = &reqObj->inFrameList[0];
    reqObj->processList.inFrameList[1] = &reqObj->inFrameList[1];
    reqObj->processList.outFrameList[0] = &reqObj->outFrameList[0];

    return FVID2_SOK;
}



/*
  Run-time update of parameters

  Changes input size for all channels in this driver object

  drvObj - driver information
*/
Int32 NsfApp_drvObjUpdateParams ( NsfApp_DrvObj * drvObj )
{
    Int32 status;
    UInt32 chId;
    Vps_NsfDataFormat *nsfDataFormat;

    #ifdef NSF_APP_PRINT_ALL
    Vps_printf ( " NSFAPP: %d: Changing input size for channels\n" );
    #endif

    /*
     * for all channels
     */
    for ( chId = 0; chId < drvObj->createArgs.numCh; chId++ )
    {
        /*
         * channel data format
         */
        nsfDataFormat = &drvObj->createArgs.dataFormat[chId];

        /*
         * half the input width x height for the channel
         */
        nsfDataFormat->inFrameWidth = gNsfApp_chWidthHeightList[chId][0] / 2;
        nsfDataFormat->inFrameHeight = gNsfApp_chWidthHeightList[chId][1] / 2;


#ifdef NSF_APP_PRINT_ALL
        /*
         * Print new channel information
         */
        Vps_printf ( " NSFAPP: %d: Changing CH %d input size to %d x %d\n",
                        drvObj->handleId,
                        chId,
                        nsfDataFormat->inFrameWidth,
                        nsfDataFormat->inFrameHeight );
#endif

        /*
         * Do IOCTL to change the new channel information
         * This updated information will get reflected from
         * next submission to the driver
         * Pending submission will still use old channel information
         */
        status = FVID2_control ( drvObj->fvidHandle,
                                 IOCTL_VPS_NSF_SET_DATA_FORMAT,
                                 nsfDataFormat, NULL );

        GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK );
    }

    return 0;
}



/*
  Initialize all request obj

  drvObj - driver information
*/
Int32 NsfApp_drvObjResetReq ( NsfApp_DrvObj * drvObj )
{
    UInt32 reqId;
    NsfApp_ReqObj *reqObj;

    /*
     * for all requests
     */
    for ( reqId = 0; reqId < NSF_APP_MAX_FRAMES_PER_CH; reqId++ )
    {
        reqObj = &drvObj->reqObj[reqId];

        /*
         * init inFrameList [0]
         */
        reqObj->inFrameList[0].numFrames = 0;
        reqObj->inFrameList[0].perListCfg = NULL;
        reqObj->inFrameList[0].reserved = NULL;

        /*
         * init inFrameList [1]
         */
        reqObj->inFrameList[1].numFrames = 0;
        reqObj->inFrameList[1].perListCfg = NULL;
        reqObj->inFrameList[1].reserved = NULL;

        /*
         * init outFrameList [0]
         */
        reqObj->outFrameList[0].numFrames = 0;
        reqObj->outFrameList[0].perListCfg = NULL;
        reqObj->outFrameList[0].reserved = NULL;

        /*
         * init processList
         */
        reqObj->processList.inFrameList[0] = &reqObj->inFrameList[0];
        reqObj->processList.inFrameList[1] = &reqObj->inFrameList[1];
        reqObj->processList.outFrameList[0] = &reqObj->outFrameList[0];
        reqObj->processList.numInLists = 2;
        reqObj->processList.numOutLists = 1;
        reqObj->processList.reserved = NULL;
    }

    return FVID2_SOK;
}

/*
  Initialize driver information and create the driver

  drvObj - driver information
  handleId - handle ID of this creation
*/
Int32 NsfApp_drvObjInit ( NsfApp_DrvObj * drvObj, int handleId, NsfApp_UtParams *pUtPrms )
{
    UInt32 chId, frameId;
    NsfApp_ChObj *chObj;
    UInt32 outSize, cOffset;
    UInt8 *curMemAddr = NULL;
    Vps_NsfDataFormat *nsfDataFormat;
    Vps_NsfProcessingCfg *nsfProcessingCfg;
    Semaphore_Params semParams;

    drvObj->handleId = handleId;

    /*
     * Create all request completion semaphore
     */

    Semaphore_Params_init ( &semParams );

    drvObj->semAllComplete = Semaphore_create ( 0, &semParams, NULL );
    GT_assert( GT_DEFAULT_MASK, drvObj->semAllComplete != NULL );

    /*
     * point to memory for data format structure for each CH
     */
    drvObj->createArgs.dataFormat = &drvObj->dataFormat[0];

    /*
     * point to memory for processing params structure for each CH
     */
    drvObj->createArgs.processingCfg = &drvObj->processingCfg[0];

    /*
     * set createArgs to default values
     */
    NsfApp_initCreateArgs ( &drvObj->createArgs );

    /*
     * Number of channels in this handle
     */
    drvObj->createArgs.numCh = gNsfApp_ctrl.curUtParams.numCh;

    /*
     * for each channel ...
     */
    for ( chId = 0; chId < drvObj->createArgs.numCh; chId++ )
    {

        chObj = &drvObj->chObj[chId];

        /*
         * channel data format
         */
        nsfDataFormat = &drvObj->createArgs.dataFormat[chId];

        if(pUtPrms->useTiler)
        {
            nsfDataFormat->outMemType = VPS_VPDMA_MT_TILEDMEM;
        }

        /*
         * channel processing params
         */
        nsfProcessingCfg = &drvObj->createArgs.processingCfg[chId];

        /*
         * set data format and processing params based on unit test parameters
         */
        nsfProcessingCfg->bypassMode = gNsfApp_ctrl.curUtParams.bypassMode;

        /*
         * for first 31 channels use frame noise auto-calculation,
         * for others use static frame noise calculation,
         * since NF HW can have only 32 sets of frame noise calculators
         * The 32nd frame noise calculator is used for all
         * static frame noise calculator channels
         */
        if ( handleId == 0 && chId < VPS_NSF_MAX_CH_PER_HANDLE - 1 )
            nsfProcessingCfg->frameNoiseAutoCalcEnable = TRUE;
        else
            nsfProcessingCfg->frameNoiseAutoCalcEnable = FALSE;

        /*
         * get channel input width x height for gNsfApp_chWidthHeightList[]
         */
        nsfDataFormat->inFrameWidth = gNsfApp_chWidthHeightList[chId][0];
        nsfDataFormat->inFrameHeight = gNsfApp_chWidthHeightList[chId][1];

        /*
         * set channel data format
         */
        nsfDataFormat->channelNum = chId;
        nsfDataFormat->inDataFormat = FVID2_DF_YUV422I_YUYV;
        nsfDataFormat->outDataFormat = FVID2_DF_YUV420SP_UV;


        /*
         * in/out pitch must be multiple of VPS_BUFFER_ALIGNMENT
         */
        nsfDataFormat->inPitch =
            VpsUtils_align ( NSF_TEST_FRAME_PITCH * 2,
                             VPS_BUFFER_ALIGNMENT );

        if ( nsfDataFormat->outMemType == VPS_VPDMA_MT_TILEDMEM )
        {

            nsfDataFormat->outPitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
            nsfDataFormat->outPitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;

            cOffset = 0;
            outSize = 0;

        }
        else
        {
            nsfDataFormat->outPitch[0] =
                VpsUtils_align ( nsfDataFormat->inFrameWidth,
                                 VPS_BUFFER_ALIGNMENT );

            nsfDataFormat->outPitch[1] =
                VpsUtils_align ( nsfDataFormat->inFrameWidth,
                                 VPS_BUFFER_ALIGNMENT );

            cOffset =
                nsfDataFormat->outPitch[0] *
                    VpsUtils_align (
                        nsfDataFormat->inFrameHeight,
                        VPS_BUFFER_ALIGNMENT
                        )
                        ;
            outSize =
                cOffset +
                nsfDataFormat->outPitch[1] *
                    VpsUtils_align (
                        nsfDataFormat->inFrameHeight/2,
                        VPS_BUFFER_ALIGNMENT
                        )
                        ;

        }

        chObj->memSizeTotal = ( outSize ) * NSF_APP_MAX_FRAMES_PER_CH;

        if(chObj->memSizeTotal)
        {
            chObj->memBaseAddr = VpsUtils_memAlloc ( chObj->memSizeTotal,
                                                 VPS_BUFFER_ALIGNMENT );

            GT_assert( GT_DEFAULT_MASK, chObj->memBaseAddr != NULL );

            curMemAddr = chObj->memBaseAddr;

            /*
             * reset output buffer with known data
             */
    #ifdef VPS_CLEAR_ALL_MEM
            memset ( curMemAddr, 0x0, outSize * NSF_APP_MAX_FRAMES_PER_CH );
    #endif
        }

        for ( frameId = 0; frameId < NSF_APP_MAX_FRAMES_PER_CH; frameId++ )
        {

            /*
             * set FVID2_Frame values appropiately
             */
            chObj->inFrame[frameId].addr[0][0] = gNsfApp_ctrl.inMemAddr + frameId*NSF_TEST_FRAME_SIZE;
            chObj->inFrame[frameId].channelNum = chId;
            chObj->inFrame[frameId].perFrameCfg = NULL;
        }

        for ( frameId = 0; frameId < NSF_APP_MAX_FRAMES_PER_CH; frameId++ )
        {

            /*
             * set FVID2_Frame values appropiately
             */
            if ( nsfDataFormat->outMemType == VPS_VPDMA_MT_TILEDMEM )
            {

                /*
                 * Y data
                 */
                chObj->outFrame[frameId].addr[0][0] =
                    ( Ptr ) VpsUtils_tilerAlloc ( VPSUTILS_TILER_CNT_8BIT,
                                                  nsfDataFormat->inFrameWidth,
                                                  VpsUtils_align (
                                                        nsfDataFormat->inFrameHeight,
                                                        VPS_BUFFER_ALIGNMENT
                                                             )

                                                );

                /*
                 * C data
                 */
                chObj->outFrame[frameId].addr[0][1] =
                    ( Ptr ) VpsUtils_tilerAlloc ( VPSUTILS_TILER_CNT_16BIT,
                                                  nsfDataFormat->inFrameWidth,
                                                  VpsUtils_align (
                                                        nsfDataFormat->inFrameHeight/2,
                                                        VPS_BUFFER_ALIGNMENT
                                                             )
                                                 );

            }
            else
            {
                /*
                 * Y data
                 */
                chObj->outFrame[frameId].addr[0][0] = curMemAddr;
                /*
                 * C data
                 */
                chObj->outFrame[frameId].addr[0][1] = curMemAddr + cOffset;
            }

            chObj->outFrame[frameId].channelNum = chId;
            curMemAddr += outSize;
        }
    }

    /*
     * init request objects
     */
    NsfApp_drvObjResetReq ( drvObj );

    memset(&drvObj->cbPrm, 0, sizeof(drvObj->cbPrm));

    drvObj->cbPrm.cbFxn = NsfApp_drvObjCompleteCallback;

    /*
     * set driver level appData
     */
    drvObj->cbPrm.appData = drvObj;

    /*
     * Create the driver
     */
    drvObj->fvidHandle = FVID2_create ( FVID2_VPS_M2M_NSF_DRV,
                                        VPS_M2M_INST_NF0,
                                        &drvObj->createArgs,
                                        &drvObj->createStatus, &drvObj->cbPrm );

    GT_assert( GT_DEFAULT_MASK, drvObj->fvidHandle != NULL );

    /*
     * init current request to be submitted to 0
     */
    drvObj->curReq = 0;

    Vps_printf ( " NSFAPP: %d: NsfApp_initDrvObj() - DONE !!!\n",
                    drvObj->handleId );

    return FVID2_SOK;
}

/*
  Close and free driver resources

  drvObj - driver info
*/
Int32 NsfApp_drvObjDeInit ( NsfApp_DrvObj * drvObj )
{
    UInt32 chId;
    NsfApp_ChObj *chObj;

    /*
     * delete the driver
     */
    FVID2_delete ( drvObj->fvidHandle, NULL );

    /*
     * Free the channel memory buffers
     */
    for ( chId = 0; chId < drvObj->createArgs.numCh; chId++ )
    {

        chObj = &drvObj->chObj[chId];

        if(chObj->memSizeTotal)
            VpsUtils_memFree ( chObj->memBaseAddr, chObj->memSizeTotal );
    }

    VpsUtils_tilerFreeAll (  );

    /*
     * delete test application  semaphore's
     */
    Semaphore_delete ( &drvObj->semAllComplete );

    Vps_printf ( " NSFAPP: %d: NsfApp_deInitDrvObj() - DONE !!!\n",
                    drvObj->handleId );

    return FVID2_SOK;
}

/*
  Set createArgs with default values

  createArgs - driver create time arguments
*/
Int32 NsfApp_initCreateArgs ( Vps_NsfCreateParams * createArgs )
{
    UInt32 chId;

    createArgs->numCh = VPS_NSF_MAX_CH_PER_HANDLE;

    for ( chId = 0; chId < createArgs->numCh; chId++ )
    {

        createArgs->dataFormat[chId].channelNum = chId;

        createArgs->dataFormat[chId].inMemType = VPS_VPDMA_MT_NONTILEDMEM;
        createArgs->dataFormat[chId].outMemType = VPS_VPDMA_MT_NONTILEDMEM;

        createArgs->dataFormat[chId].inDataFormat = FVID2_DF_YUV422I_YUYV;
        createArgs->dataFormat[chId].inFrameWidth = 360;
        createArgs->dataFormat[chId].inFrameHeight = 120;

        createArgs->dataFormat[chId].inPitch
            =
            VpsUtils_align ( createArgs->dataFormat[chId].inFrameWidth * 2,
                             32 );

        createArgs->dataFormat[chId].outDataFormat = FVID2_DF_YUV420SP_UV;

        createArgs->dataFormat[chId].outPitch[FVID2_YUV_SP_Y_ADDR_IDX]
            =
            VpsUtils_align ( createArgs->dataFormat[chId].inFrameWidth,
                             32 );

        createArgs->dataFormat[chId].outPitch[FVID2_YUV_SP_CBCR_ADDR_IDX]
            =
            VpsUtils_align ( createArgs->dataFormat[chId].inFrameWidth,
                             32 );

        createArgs->processingCfg[chId].channelNum = chId;
        createArgs->processingCfg[chId].bypassMode = VPS_NSF_DISABLE_TNF;
        createArgs->processingCfg[chId].frameNoiseAutoCalcEnable = TRUE;
        createArgs->processingCfg[chId].frameNoiseCalcReset = FALSE;
        createArgs->processingCfg[chId].subFrameModeEnable = TRUE;
        createArgs->processingCfg[chId].numLinesPerSubFrame = 128;
        createArgs->processingCfg[chId].staticFrameNoise[0] = 0;
        createArgs->processingCfg[chId].staticFrameNoise[1] = 0;
        createArgs->processingCfg[chId].staticFrameNoise[2] = 0;

        createArgs->processingCfg[chId].spatialStrengthLow[0]
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].spatialStrengthLow[1]
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].spatialStrengthLow[2]
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].spatialStrengthHigh[0]
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].spatialStrengthHigh[1]
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].spatialStrengthHigh[2]
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].temporalStrength
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].temporalTriggerNoise
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].noiseIirCoeff
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].maxNoise
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].pureBlackThres
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
        createArgs->processingCfg[chId].pureWhiteThres
            = VPS_NSF_PROCESSING_CFG_DEFAULT;
    }

    return 0;
}

/*
  System init
*/
Int32 NsfApp_init (  )
{
    Int32         status;
    const Char    *versionStr;
    Vps_PlatformInitParams       platformInitPrms;

    /* Initialize the platform */
    platformInitPrms.isPinMuxSettingReq = TRUE;
    status = Vps_platformInit(&platformInitPrms);
    /*
     * Init memory allocator
     */
    VpsUtils_memInit (  );

    /*
     * Init tiler memory
     */
    VpsUtils_tilerInit (  );

#ifdef NSF_APP_PRINT_ALL
    VpsUtils_tilerDebugLogEnable ( TRUE );
#endif

    /*
     * Get the version string
     */
    versionStr = FVID2_getVersionString();
    Vps_printf("HDVPSS Drivers Version: %s\n", versionStr);

    /*
     * Initialize the vpsUtils to get the load
     */
    VpsUtils_prfInit();

    /* Create the TimeStamp Calculation */
    gNsfApp_ctrl.prfTsHandle = VpsUtils_prfTsCreate("NSFAPP:");

    /*
     * FVID2 system init
     */
    status = FVID2_init ( NULL );

    GT_assert( GT_DEFAULT_MASK, status == 0 );

    /*
     * Pltform details
     */
    gNsfApp_ctrl.platform   = Vps_platformGetId();
    GT_assert( GT_DEFAULT_MASK,
        gNsfApp_ctrl.platform != VPS_PLATFORM_ID_UNKNOWN );
    gNsfApp_ctrl.cpuVer     = Vps_platformGetCpuRev();
    GT_assert( GT_DEFAULT_MASK,
        gNsfApp_ctrl.cpuVer != VPS_PLATFORM_CPU_REV_UNKNOWN );

    gNsfApp_ctrl.inMemAddr = VpsUtils_memAlloc(
                                NSF_TEST_FRAME_SIZE*NSF_TEST_NUM_FRAMES,
                                VPS_BUFFER_ALIGNMENT
                             );

    GT_assert( GT_DEFAULT_MASK, gNsfApp_ctrl.inMemAddr!=NULL);

    Vps_printf ( " NSFAPP: Load YUYV422 test data ( %d x %d, %d frames ) @ 0x%08x !!!\n",
        NSF_TEST_FRAME_PITCH,
        NSF_TEST_FRAME_HEIGHT,
        NSF_TEST_NUM_FRAMES,
        gNsfApp_ctrl.inMemAddr
        );

    Vps_printf ( " loadRaw( 0x%x, 0, \"<my folder>\\NSF_IN_1920x1088x5.YUYV\", 32, false); \n",
        gNsfApp_ctrl.inMemAddr
        );

    Vps_printf ( " NSFAPP: Press Any Key to Continue ... !!!\n");

    getchar();

    Vps_printf ( " \r\n");

    Vps_printf ( " NSFAPP: NsfApp_init() - DONE !!!\n" );

    return 0;
}

Int32 NsfApp_getCpuLoad()
{
    gNsfApp_ctrl.totalCpuLoad += Load_getCPULoad();
    gNsfApp_ctrl.cpuLoadCount++;

    return 0;
}

Int32 NsfApp_resetStatistics()
{
    gNsfApp_ctrl.totalFrames = 0;
    gNsfApp_ctrl.totalCpuLoad = 0;
    gNsfApp_ctrl.cpuLoadCount = 0;
    gNsfApp_ctrl.totalPixels = 0;

    return 0;
}

Int32 NsfApp_printStatistics(UInt32 runTime)
{
    UInt32 mpixelsPerSec, numD1;

    mpixelsPerSec = (gNsfApp_ctrl.totalPixels)/(runTime*1000);

    numD1 = (mpixelsPerSec*1000)/(72*24*6);

    Vps_printf(" \r\n");
    Vps_printf(" Execution      Total Total   Total     Total D1@60Hz  CPU \r\n");
    Vps_printf("      Time     Frames   FPS Mpixels Mpixels/s    Ch's Load \r\n");
    Vps_printf(" ========================================================= \r\n");
    Vps_printf(" %5d.%d s %7d %5d %7d %9d %7d %4d \r\n",
            runTime/1000,
            runTime%1000,
            gNsfApp_ctrl.totalFrames,
            (gNsfApp_ctrl.totalFrames*1000)/runTime,
            gNsfApp_ctrl.totalPixels/1000000,
            mpixelsPerSec,
            numD1,
            gNsfApp_ctrl.totalCpuLoad/gNsfApp_ctrl.cpuLoadCount
        );
    Vps_printf(" \r\n");

    return 0;
}


/*
  Run test case

  utParams - test case parameters
*/
Int32 NsfApp_run ( NsfApp_UtParams * utParams )
{
    Int32 count = 0u, handleId, status;
    UInt32 startTime, printTime, runTime, elapsedTime; /* in msecs   */
    UInt32  num_of_sub_frames = 0,i=0,totalPixels =0;
    NsfApp_DrvObj * drvObj;
    NsfApp_ReqObj *reqObj;
    FVID2_Frame *inframe;
    Vps_NsfCreateParams *createArgs;
    Vps_NsfDataFormat *nsfDataFormat;
    NsfApp_ChObj  *chObj;
    char ch;

    Semaphore_Params semPrms;

    status = FVID2_SOK;

    /* Check if we can support the configured feature on this version of the
       silicon */
    if ((gNsfApp_ctrl.platform == VPS_PLATFORM_ID_EVM_TI814x) &&
        (gNsfApp_ctrl.cpuVer < VPS_PLATFORM_CPU_REV_2_1 ))
    {
        /* On 1.0 We do not support SNF and TNF bypass modes */
        if (utParams->bypassMode == VPS_NSF_DISABLE_SNF_TNF)
        {
            Vps_printf ( "\n NSFAPP: %s is not supported - skipping this config !!!\n\n",
                "VPS_NSF_DISABLE_SNF_TNF");
            status = FVID2_EFAIL;
        }
        if (utParams->bypassMode == VPS_NSF_DISABLE_TNF)
        {
            Vps_printf ( "\n NSFAPP: %s is not supported - skipping this config !!!\n\n",
                "VPS_NSF_DISABLE_TNF");
            status = FVID2_EFAIL;
        }
    }

    if (status != FVID2_SOK)
    {
        return (status);
    }

    /* Create semaphore */
    Semaphore_Params_init(&semPrms);
    gAppCompleteSem = Semaphore_create(0, &semPrms, NULL);
    if (NULL == gAppCompleteSem)
    {
        System_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return 0;
    }


    NsfApp_resetStatistics();

    memcpy ( &gNsfApp_ctrl.curUtParams, utParams,
             sizeof ( gNsfApp_ctrl.curUtParams ) );

    /*
     * Print test case parameters
     */
    Vps_printf ( " NSFAPP: HANDLES %d: CHANNELS %d : RUN COUNT %d: "
                    "UPDATE_PRM_RT %d: MODE: %d !!!\n",
                    gNsfApp_ctrl.curUtParams.numHandles,
                    gNsfApp_ctrl.curUtParams.numCh,
                    gNsfApp_ctrl.curUtParams.runCount,
                    gNsfApp_ctrl.curUtParams.updateParamsRt,
                    gNsfApp_ctrl.curUtParams.bypassMode );

    printTime     = 5*1000;
    runTime       = utParams->runCount*1000;

    if (Vps_platformIsSim())
    {
        printTime /= 100;
        runTime   /= 100;
    }

    /*
     * Create driver
     */
    for ( handleId = 0; handleId < gNsfApp_ctrl.curUtParams.numHandles;
          handleId++ )
    {
        NsfApp_drvObjInit (
                &gNsfApp_ctrl.drvObj[handleId],
                handleId,
                &gNsfApp_ctrl.curUtParams
            );
    }

    startTime = Clock_getTicks();

    /* Start the load calculation */
    VpsUtils_prfLoadCalcStart();

    VpsUtils_prfTsReset(gNsfApp_ctrl.prfTsHandle);
    VpsUtils_prfTsBegin(gNsfApp_ctrl.prfTsHandle);
    /*
     * Process frames
     */


    for ( handleId = 0; handleId < gNsfApp_ctrl.curUtParams.numHandles;
        handleId++ )
    {

        drvObj = &gNsfApp_ctrl.drvObj[handleId];

        chObj = &drvObj->chObj[0];

        createArgs = &drvObj->createArgs;

        reqObj = &drvObj->reqObj[drvObj->curReq];

        nsfDataFormat = &drvObj->createArgs.dataFormat[0];

        NsfApp_drvObjMakeReq ( drvObj, reqObj );

        inframe=reqObj->processList.inFrameList[0]->frames[0];

        num_of_sub_frames = (nsfDataFormat->inFrameHeight)/
            (createArgs->processingCfg[0].numLinesPerSubFrame) + 1;

        totalPixels = 0;

        for(i=0;i<num_of_sub_frames;i++)
        {

            inframe->subFrameInfo->subFrameNum=i;

            /*
            * Submit  request to driver
            */
            status =
                FVID2_processFrames ( drvObj->fvidHandle, &reqObj->processList );

            if ( status != FVID2_SOK )
            {
                /*
                * Error in request submission
                */
                Vps_printf
                ( " NSFAPP: %d: %2d: FVID2_processFrames() - ERROR !!!\n",
                drvObj->handleId, count );
            }

            Semaphore_pend ( gAppCompleteSem, BIOS_WAIT_FOREVER );

            status = FVID2_getProcessedFrames ( drvObj->fvidHandle,
                &reqObj->processList, BIOS_WAIT_FOREVER );

            if ( status != FVID2_SOK )
            {
                /*
                * Error in getting processed frames
                */
                System_printf ( " NSFAPP: %d: FVID2_getProcessedFrames() - ERROR !!!\n",
                drvObj->handleId );
            }
            else
            {
                totalPixels += nsfDataFormat->inFrameWidth*createArgs->processingCfg[0]
                    .numLinesPerSubFrame;

                Vps_printf(
                    " NSFAPP: Save output file with command \n :saveRaw(0, 0x%.8x, "
                    "\"C:\\\\%dnsfSubFrameWbOut_nv12_prog_packed_%d_%d.tigf\", "
                    "%d, 32, true);\n",
                    chObj->memBaseAddr,
                    i,
                    VpsUtils_align(nsfDataFormat->inFrameWidth, VPS_BUFFER_ALIGNMENT),
                    VpsUtils_align(nsfDataFormat->inFrameHeight, VPS_BUFFER_ALIGNMENT),
                    (chObj->memSizeTotal/8));

                Vps_printf ("\n Enter any key after saving \n");
                fflush(stdin);
                VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);
            }

        }

        if(i == num_of_sub_frames)
        {

            gNsfApp_ctrl.totalFrames += reqObj->processList.inFrameList[0]->numFrames;
            gNsfApp_ctrl.totalPixels += totalPixels;

        }

    }



    elapsedTime = Clock_getTicks() - startTime;

    /* End of timestamp addition */
    VpsUtils_prfTsEnd(gNsfApp_ctrl.prfTsHandle, count);

    /* Stop the load calculation */
    VpsUtils_prfLoadCalcStop();

    /*
     * Delete driver
     */
    for ( handleId = 0; handleId < gNsfApp_ctrl.curUtParams.numHandles;
          handleId++ )
        NsfApp_drvObjDeInit ( &gNsfApp_ctrl.drvObj[handleId] );

    /*
     * Print test case parameters
     */
    Vps_printf
        ( " NSFAPP: HANDLES %d: CHANNELS %d : RUN COUNT %d: "
          "UPDATE_PRM_RT %d!!! - DONE\n\n",
          gNsfApp_ctrl.curUtParams.numHandles, gNsfApp_ctrl.curUtParams.numCh,
          gNsfApp_ctrl.curUtParams.runCount,
          gNsfApp_ctrl.curUtParams.updateParamsRt );

    NsfApp_printStatistics(elapsedTime);

    /* Print the load */
    VpsUtils_prfLoadPrintAll(TRUE);
    /* print the fps based on TimeStamp */
    VpsUtils_prfTsPrint(gNsfApp_ctrl.prfTsHandle, TRUE);

    /* Reset the load */
    VpsUtils_prfLoadCalcReset();

    return 0;
    }

/*
  System de-init
*/
Int32 NsfApp_deInit (  )
{
    VpsUtils_memFree(gNsfApp_ctrl.inMemAddr, NSF_TEST_FRAME_SIZE*NSF_TEST_NUM_FRAMES);

    /* Delete the timestamp instance */
    VpsUtils_prfTsDelete(gNsfApp_ctrl.prfTsHandle);

    /*
     * De-Initialize the vpsUtils to get the load
     */
    VpsUtils_prfDeInit();
    /*
     * FVID2 system de-init
     */
    FVID2_deInit ( NULL );

    /*
     * De-Init tiler memory
     */
    VpsUtils_tilerDeInit (  );

    /*
     * De-init memory allocator
     */
    VpsUtils_memDeInit (  );
    /* De-Initialize the platform */
    Vps_platformDeInit();

    Vps_printf ( " NSFAPP: NsfApp_deInit() - DONE !!!\n" );

    return 0;
}

/*
  Test application task main
*/
Void NsfApp_tskMain ( UArg arg1, UArg arg2 )
{
    /*
     * System init
     */
    NsfApp_init (  );

    /*
     * Run test cases
     */
#ifdef NSFAPP_TEST_ALL
    /*
     * Run all test cases
     */
    for ( NsfApp_testId = 0;
          NsfApp_testId < sizeof ( gNsfApp_utParams ) / sizeof ( gNsfApp_utParams[0] );
          NsfApp_testId++ )
    {
#if 1
        NsfApp_run ( &gNsfApp_utParams[NsfApp_testId] );
        Vps_printf (" NSFAPP: Press Any Key to continue after saving"
            " output image ... !!!\n");

        getchar();
#else
        printf("Enter the testCaseId to run\n");
        scanf("%d",&NsfApp_testId_orig);
        printf("TestId running is %d\n\n\n\n\n", testId_orig);
        NsfApp_run ( &gNsfApp_utParams[testId_orig] );
        NsfApp_testId--;
        if (100 == testId_orig)
            break;
#endif
    }
#else
    /*
     * Run only 0th test case
     */
    NsfApp_testId = 0;

    NsfApp_run ( &gNsfApp_utParams[NsfApp_testId] );
#endif

    /*
     * System de-init
     */
    NsfApp_deInit (  );
}

/*
  Create test task
*/
Void NsfApp_createTsk (  )
{
    Task_Params tskParams;

    /*
     * Reset all values to 0
     */
    memset ( &gNsfApp_ctrl, 0, sizeof ( gNsfApp_ctrl ) );

    /*
     * Create test task
     */
    Task_Params_init ( &tskParams );

    tskParams.priority = NSF_APP_TSK_PRI_MAIN;
    tskParams.stack = gNsfApp_tskStackMain;
    tskParams.stackSize = sizeof ( gNsfApp_tskStackMain );

    gNsfApp_ctrl.tskMain = Task_create ( NsfApp_tskMain, &tskParams, NULL );

    GT_assert( GT_DEFAULT_MASK, gNsfApp_ctrl.tskMain != NULL );

    /* Register the task to the load module for calculating the load */
    VpsUtils_prfLoadRegister(gNsfApp_ctrl.tskMain, "NSFAPP:");

}

/*
  Application main
*/
Int32 main ( void )
{
    /*
     * Create task
     */
    NsfApp_createTsk (  );

    /*
     * Start BIOS
     */
    BIOS_start (  );

    return (0);
}
