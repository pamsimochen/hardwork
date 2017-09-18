/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file M2mComp_main.c
 *
 * \brief   Blender and Scalar5 writeback2 path with Bypass path as input memory
 *          to memory driver example. This examples shows the single channel
 *          memory to memory operation per handle.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/vps_m2mComp.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Driver instance to test. */
#define DRIVER_INSTANCE                 (VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB)

/* Output width for each successive channel for the multichannel will decrease
 * by this value for the upscaling case.  While for the down scaling case
 * it will decrease by OUTPUT_WIDTH_CHANGE/10.  So for upscaling from
 * 720X480 --> 1920X1080 first channel width will be 1080, second will be
 * 980 and so on. Pitch will remain same for all the cases.
 */
#define OUTPUT_WIDTH_CHANGE             (100u)
#define OUT_DATA_FORMAT                 (FVID2_DF_YUV422I_YUYV)
#define OUT_NUM_FRAMELIST               (1u)


/* Buffer alignment required by the hardware */
#define BUFFER_ALIGNMENT        (16)
/* In case of YUV 420 semiplanar data we will have two planes one for Y
 * an another for CB and CR
 */
#define MAX_PLANES              2u

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (10u * 1024u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* Strucutre is used to configure the driver for different configurations */
typedef struct
{
    Char    testDesc[200];
    UInt32  driverId;
    UInt32  cfgId;
    UInt32  loopCount;
    UInt32  chPerHndl;
    UInt32  inputWidth;
    UInt32  inputHeight;
    UInt32  outputWidth;
    UInt32  outputHeight;
    UInt32  inputDataFmt;
    UInt32  outputDataFmt;
    UInt32  vsType;
    UInt32  cropStartX;
    UInt32  cropStartY;
    UInt32  cropWidth;
    UInt32  cropHeight;
    UInt32  nonlinearScaling;
    UInt32  stripsize;
    UInt32  scBypass;
    UInt32  fps;
    UInt32  subFrameProcEn;
    UInt32  NumSubFrames;

}vpsAppM2mCompCfg;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void vpsCompTestTask(void);
void vpsCompSetConfigParams();
static Int32 vpsCompTest(UInt32 driverInst);
static Int32 vpsCompCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
static Int32 vpsCompErrCbFxn(FVID2_Handle handle,
                            Ptr appData,
                            Void *errList,
                            Ptr reserved);
static Void vpsCompInitVariables(void);
static Void vpsCompFreeInOutBufs();
static Int32  vpsCompAllocOutBufs();
static Int32 vpsCompAllocInBufs();
static Int32 vpsCompRunTestCases(UInt32 cfgCnt);
static Int32 vpsCompGetTstId();


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* Configuration for various test cases */
vpsAppM2mCompCfg appCompCfg[] =
{
    /* 1 channel YUV422 SD ---> YUV422 BP0*/
    {
        "1  CH YUV422 SD ---> YUV422 HD, Driver: VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB",
        VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB,4,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV422I_YUYV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    }
};

/* Pointer to the currently selected configuration */
vpsAppM2mCompCfg *appCompCurCfg;

/* Mem2Mem Driver handle */
static FVID2_Handle fvid2Handle;

/* Process list for submitting request */
static FVID2_ProcessList processList[1];

/* Input Framelist */
static FVID2_FrameList inFrameList[1];
/* Output FrameList */
static FVID2_FrameList outFrameList[1];

/* In FrameBuffer*/
static FVID2_Frame inFrames[VPS_M2M_COMP_MAX_CH];
/* Out FrameBuffer Pointers */
static FVID2_Frame outFrames[VPS_M2M_COMP_MAX_CH];

/* Pointers to input and output buffers */
UInt8 *inputBuffer[VPS_M2M_COMP_MAX_CH][MAX_PLANES];
UInt8 *outputBuffer[VPS_M2M_COMP_MAX_CH][MAX_PLANES];

/* Error processList */
static FVID2_ProcessList errProcessList;

/* Stream complete semaphoreused to sync between callback and task. */
static Semaphore_Handle ReqProcessed;
/* Task complete semaphore */
static Semaphore_Handle tskComplete;

/* Per handle parameters for each driver */
Vps_M2mCompCreateParams createPrmsComp;
/* Configuration parameters for each channel*/
Vps_M2mCompChParams chPrmsComp[VPS_M2M_COMP_MAX_CH];
/* Parameters for setting coefficients */
Vps_ScCoeffParams coeffParams;

/* Instance of the channel scalar config */
Vps_ScConfig        chSc5Cfg[VPS_M2M_COMP_MAX_CH];
/* Instance of the crop config */
Vps_CropConfig      chSc5CropCfg[VPS_M2M_COMP_MAX_CH];
/* Instance of the crop config */
FVID2_Format        outFmtSc5[VPS_M2M_COMP_MAX_CH];

/* Test application stack. */
#pragma DATA_ALIGN(App_mainTskStack, 32)
#pragma DATA_SECTION(App_mainTskStack, ".bss:taskStackSection")
static UInt8 App_mainTskStack[APP_TSK_STACK_SIZE];

/* Flag to indicate when to start taking load */
UInt32 startLoadUpdate = 0;

/* Pointer where utils will return the different loads */
VpsUtils_PrfLoad *prfLoad;
/* Total load will be stored for each iteration */
VpsUtils_PrfLoad totalLoad;
/* Total number of times the update load will be called */
Int32 loadCount = 0;
/* Handle to get the Timestamp */
VpsUtils_PrfTsHndl   *prfTsHandle = NULL;
/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Application main function.
 */
Int32 main(void)
{
    Task_Params             tskParams;
    Semaphore_Params        semParams;
    Int32                   retVal = FVID2_SOK;
    Task_Handle             tskHandle;

    Semaphore_Params_init(&semParams);
    tskComplete = Semaphore_create(0, &semParams, NULL);
    if (NULL == tskComplete)
    {
        Vps_printf(
            "M2mCompExample:%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EFAIL;
    }
    if (FVID2_SOK == retVal)
    {
        VpsUtils_prfInit();
        prfTsHandle = VpsUtils_prfTsCreate("M2mCompExample");
        if (NULL == prfTsHandle)
        {
            retVal = FVID2_EFAIL;
            Vps_printf(
            "M2mCompExample:%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        }
        Task_Params_init(&tskParams);
        tskParams.stack = App_mainTskStack;
        tskParams.stackSize = APP_TSK_STACK_SIZE;
        /* Start the Testing Task */
        tskHandle = Task_create((Task_FuncPtr) vpsCompTestTask, &tskParams, NULL);
        if (NULL == tskHandle)
        {
            Vps_printf(
            "M2mCompExample:%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            retVal = FVID2_EFAIL;
        }
        VpsUtils_prfLoadRegister(tskHandle, "M2mCompExample:");
        BIOS_start();
    }
    if (FVID2_SOK == retVal)
    {
        /* Wait for task to complete */
        Semaphore_pend(tskComplete, BIOS_WAIT_FOREVER);
        VpsUtils_prfTsDelete(prfTsHandle);
        Semaphore_delete(&tskComplete);
        VpsUtils_prfLoadUnRegister(tskHandle);
        VpsUtils_prfDeInit();
        Task_delete(&tskHandle);
    }

    return (retVal);
}

/**
 *  Application test task.
 */
static Void vpsCompTestTask(void)
{
    const Char  *versionStr;
    Int32       retVal;
    Int32       tstCnt, testId;
    Vps_PlatformInitParams       platformInitPrms;

    /* Initialize the platform */
    platformInitPrms.isPinMuxSettingReq = TRUE;
    retVal = Vps_platformInit(&platformInitPrms);
    /* Init memory allocator */
    VpsUtils_memInit();

    /*
     * Get the version string
     */
    versionStr = FVID2_getVersionString();
    Vps_printf("HDVPSS Drivers Version: %s\n", versionStr);
    /* FVID2 Init */
    retVal = FVID2_init(NULL);

    /* Run the test cases */
    do {
        /* Get the TestId */
        testId = vpsCompGetTstId();
        if ((testId >= 0) && (testId != 100))
        {
            retVal = vpsCompRunTestCases(testId);
        }
        else if (testId == 100)
        {
            break;
        }
        else if (testId == 10)
        {
            for ( tstCnt = 0; tstCnt < sizeof(appCompCfg)/sizeof(vpsAppM2mCompCfg); tstCnt++)
            {
                retVal = vpsCompRunTestCases(tstCnt);
                /* Manual tests will always run with 0th configuration which is user
                 * selected */
            }
        }
        else
        {
            Vps_printf("M2mCompExample:Invalid TestID Enter Again\n");
        }
    }while (testId != 100);

    retVal = FVID2_deInit(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "M2mCompExample:%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
     /* De-Init memory allocator */
    VpsUtils_memDeInit();
    /* De-Initialize the platform */
    Vps_platformDeInit();

    Vps_printf("M2mCompExample:Test Successfull!!\n");
    Semaphore_post(tskComplete);
}

static Int32 vpsCompRunTestCases(UInt32 cfgCnt)
{
    Int32 retVal = 0;

    appCompCurCfg = &appCompCfg[cfgCnt];
    /* Loop count is always 10 for simulator as its very slow */
    if (Vps_platformIsSim())
    {
        appCompCurCfg->loopCount = 1;
    }

    retVal = vpsCompAllocInBufs();
    retVal += vpsCompAllocOutBufs();
    if (retVal != FVID2_SOK)
    {
        Vps_printf(
            "M2mCompExample:Allocation of Buffers Failed...\n");
        retVal = FVID2_EALLOC;
        return retVal;
    }
    Vps_printf(
            "M2mCompExample:Input an numeric key and press enter after loading...\n");
    //?LK debug scanf("%d", &retVal);
    retVal = vpsCompTest(appCompCurCfg->driverId);

    vpsCompFreeInOutBufs();
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "M2mCompExample:%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EALLOC;
        return retVal;
    }
    else
    {
        Vps_printf("M2mCompExample:Frames completed processing successfully\n");
    }
    return retVal;
}

void vpsCompSetConfigParams()
{
    UInt32 configCnt;
    /* Configuration per each channel */
    createPrmsComp.mode = VPS_M2M_CONFIG_PER_CHANNEL;
//    createPrmsComp.mode = VPS_M2M_CONFIG_PER_HANDLE;

    /* Number of channels per handle */
    createPrmsComp.numCh = appCompCurCfg->chPerHndl;

    chPrmsComp[0].inFmt.channelNum = 0;
    chPrmsComp[0].inFmt.width = appCompCurCfg->inputWidth;
    chPrmsComp[0].inFmt.height = appCompCurCfg->inputHeight;

    if (FVID2_DF_YUV420SP_UV == appCompCurCfg->inputDataFmt ||
            (FVID2_DF_YUV422SP_UV == appCompCurCfg->inputDataFmt))
    {
        chPrmsComp[0].inFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appCompCurCfg->inputWidth;
        chPrmsComp[0].inFmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appCompCurCfg->inputWidth;
    }
    else
    {
        chPrmsComp[0].inFmt.pitch[FVID2_YUV_INT_ADDR_IDX] = appCompCurCfg->inputWidth * 2u;
    }

    chPrmsComp[0].inFmt.fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] = FALSE;
    chPrmsComp[0].inFmt.fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] = FALSE;
    chPrmsComp[0].inFmt.dataFormat = appCompCurCfg->inputDataFmt;
    chPrmsComp[0].inFmt.scanFormat = FVID2_SF_PROGRESSIVE;

    chPrmsComp[0].outFmtSc5->channelNum = 0;
    chPrmsComp[0].outFmtSc5->width = appCompCurCfg->outputWidth;
    chPrmsComp[0].outFmtSc5->height = appCompCurCfg->outputHeight;

     if (FVID2_DF_YUV420SP_UV == appCompCurCfg->outputDataFmt ||
            (FVID2_DF_YUV422SP_UV == appCompCurCfg->outputDataFmt))
    {
        chPrmsComp[0].outFmtSc5->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appCompCurCfg->outputWidth;
        chPrmsComp[0].outFmtSc5->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appCompCurCfg->outputWidth;
    }
    else
    {
        chPrmsComp[0].outFmtSc5->pitch[FVID2_YUV_INT_ADDR_IDX] = appCompCurCfg->outputWidth * 2u;
    }



    chPrmsComp[0].outFmtSc5->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chPrmsComp[0].outFmtSc5->dataFormat = appCompCurCfg->outputDataFmt;
    chPrmsComp[0].outFmtSc5->scanFormat = FVID2_SF_PROGRESSIVE;

    chPrmsComp[0].sc5CropCfg->cropStartX = appCompCurCfg->cropStartX;
    chPrmsComp[0].sc5CropCfg->cropStartY =  appCompCurCfg->cropStartY;
    chPrmsComp[0].sc5CropCfg->cropWidth = appCompCurCfg->cropWidth;
    chPrmsComp[0].sc5CropCfg->cropHeight = appCompCurCfg->cropHeight;

    chPrmsComp[0].sc5Cfg->nonLinear = appCompCurCfg->nonlinearScaling;
    chPrmsComp[0].sc5Cfg->stripSize = appCompCurCfg->stripsize;
    chPrmsComp[0].sc5Cfg->vsType = appCompCurCfg->vsType;
    chPrmsComp[0].sc5Cfg->bypass = appCompCurCfg->scBypass;
    chPrmsComp[0].outMemTypeSc5 = VPS_VPDMA_MT_NONTILEDMEM;
    chPrmsComp[0].inMemType = VPS_VPDMA_MT_NONTILEDMEM;

    createPrmsComp.chParams = chPrmsComp;
    /* Copy same configuration parameters for all channel.  Each channel
     * can have different configuraton parameters also.
     */
    for (configCnt = 1; configCnt < appCompCurCfg->chPerHndl; configCnt++)
    {
        memcpy(&chPrmsComp[configCnt].inFmt, &chPrmsComp[0].inFmt,
            sizeof(FVID2_Format));
        memcpy(&chPrmsComp[configCnt].outFmtSc5, &chPrmsComp[0].outFmtSc5,
            sizeof(FVID2_Format));
        memcpy(chPrmsComp[configCnt].sc5Cfg, chPrmsComp[0].sc5Cfg,
            sizeof(Vps_ScConfig));
        memcpy(chPrmsComp[configCnt].sc5CropCfg, chPrmsComp[0].sc5CropCfg,
            sizeof(Vps_CropConfig));
        /* Output width of each channel is reducing by OUTPUT_WIDTH_CHANGE
           for upscaling, For downScaling its reducing by
           OUTPUT_WIDTH_CHANGE/10 */
#if 0
        if (appCompCurCfg->cfgId < 6)
        {
            chPrmsComp[configCnt].outFmtSc5->width =
                    chPrmsComp[configCnt - 1].outFmtSc5->width -
                                OUTPUT_WIDTH_CHANGE;
        }
        else
        {
             chPrmsComp[configCnt].outFmtSc5->width =
                    chPrmsComp[configCnt - 1].outFmtSc5->width -
                                (OUTPUT_WIDTH_CHANGE / 10);
        }
#endif
        /* Copy the input and output mem type for each channel */
        chPrmsComp[configCnt].inMemType = chPrmsComp[0].inMemType;
        chPrmsComp[configCnt].outMemTypeSc5 = chPrmsComp[0].outMemTypeSc5;
    }
}

/**
 *  Memory driver test function.
 */
static Int32 vpsCompTest(UInt32 driverInst)
{
    Int32                   retVal = FVID2_SOK;
    Vps_M2mCompCreateStatus status;
    UInt32                  reqCnt, subFrameNum;
    Semaphore_Params        semParams;
    FVID2_CbParams          cbParams;
    Vps_ScLazyLoadingParams lazyLoadingParams;

    Semaphore_Params_init(&semParams);
    ReqProcessed = Semaphore_create(0, &semParams, NULL);

    /* Initialize the frame pointers */
    vpsCompInitVariables();

    /* Set the configuration parameters for each channel */
    vpsCompSetConfigParams();
    /* Open the driver */
    cbParams.cbFxn = vpsCompCbFxn;
    cbParams.errCbFxn = vpsCompErrCbFxn;
    cbParams.errList = &errProcessList;
    cbParams.appData = NULL;
    cbParams.reserved = NULL;
    /* Open the driver */
    fvid2Handle = FVID2_create(
                        FVID2_VPS_M2M_COMP_DRV,
                        driverInst,
                        &createPrmsComp,
                        &status,
                        &cbParams);
    if (NULL == fvid2Handle)
    {
        Vps_printf(
            "M2mCompExample:%s: Error %d @ line %d\n", __FUNCTION__, status.retVal, __LINE__);
        Vps_printf("M2mCompExample:Maximum handles supported %d\n", status.maxHandles);
        Vps_printf("M2mCompExample:Maximum channels supported %d\n", status.maxChPerHandle);
        Vps_printf("M2mCompExample:Maximum number of same channels that can be"
            "submitted in single request %d\n",
            status.maxSameChPerRequest);
        Vps_printf("M2mCompExample:Maxumim number of requests that can be queued %d\n",
            status.maxReqInQueue);
        return (status.retVal);
    }
    /* Set the coefficients.  Coefficients programming is allowed only once
    before submitting the first request */
    /* Currently doing upsampling so set coefficients for the up sampling. */
    /* Set the horizontal scaling set */
    coeffParams.hScalingSet  = VPS_SC_US_SET;
    /* Set the vertical scaling set */
    coeffParams.vScalingSet = VPS_SC_US_SET;
    /* Set to null  if user is not providing the coefficients */
    coeffParams.coeffPtr = NULL;
    coeffParams.scalarId = 0; //?
    retVal = FVID2_control(fvid2Handle, IOCTL_VPS_SET_COEFFS,
        &coeffParams, NULL);
    if (0 != retVal)
    {
        Vps_printf("M2mCompExample:Setting of the coefficients failed\n");
    }

    /* Enable Lazy Loading for the scalar */
    lazyLoadingParams.scalarId = 0u; /* scalarId is ignored here. */
    lazyLoadingParams.enableLazyLoading = TRUE;
    lazyLoadingParams.enableFilterSelect = TRUE;
    retVal = FVID2_control(fvid2Handle,
                 IOCTL_VPS_SC_SET_LAZY_LOADING,
                 &lazyLoadingParams,
                 NULL);
    if (0 != retVal)
    {
        Vps_printf("M2mCompExample:Enabling Lazy Loading failed\n");
    }

    VpsUtils_prfLoadCalcStart();
    VpsUtils_prfTsBegin(prfTsHandle);
    for (reqCnt = 0; reqCnt < appCompCurCfg->loopCount; reqCnt++)
    {
      if (!appCompCurCfg->subFrameProcEn)
      {   /* FRAME based processing */
        retVal = FVID2_processFrames(fvid2Handle, &processList[0]);
        if (0 != retVal)
        {
            Vps_printf("M2mCompExample:Processing of the frame failed\n\n");
        }
        /* Wait till frames gets processed */
        Semaphore_pend(ReqProcessed, BIOS_WAIT_FOREVER);
        /* Clear the processlist before getting it from driver */
        memset(&processList[0], 0, sizeof(processList[0]));
        /* Get the processed request */
        retVal = FVID2_getProcessedFrames(fvid2Handle, &processList[0], 0);
          if (0 != retVal)
          {
              Vps_printf("M2mCompExample:Getting of the process frames function Failed\n\n");
          }
       }else
       {
          /* SUB-FRAME based processing */
          for (subFrameNum = 0; subFrameNum < appCompCurCfg->NumSubFrames; subFrameNum++)
          {
            FVID2_Frame *inFrm;

            /* update subframe info in process list */
            inFrm  = processList[0].inFrameList[0]->frames[0];
            inFrm->subFrameInfo->subFrameNum  = subFrameNum;
            inFrm->subFrameInfo->numInLines = (subFrameNum+1)*(appCompCurCfg->inputHeight/
                                                           appCompCurCfg->NumSubFrames);
            /* do subframe processing */
            retVal = FVID2_processFrames(fvid2Handle, &processList[0]);
            if (0 != retVal)
            {
                Vps_printf("M2mCompExample:Processing of the frame failed\n\n");
            }
            /* Wait till frames gets processed */
            Semaphore_pend(ReqProcessed, BIOS_WAIT_FOREVER);
            //Vps_printf("IN subFrameNum=%d, inLns=%d \n",inFrm->subFrameInfo->subFrameNum,inFrm->subFrameInfo->numInLines);
            /* Clear the processlist before getting it from driver */
            memset(&processList[0], 0, sizeof(processList[0]));
            /* Get the processed request */
            retVal = FVID2_getProcessedFrames(fvid2Handle, &processList[0], 0);
            if (0 != retVal)
            {
                Vps_printf("M2mCompExample:Getting of the process frames function Failed\n\n");
            }
          }
        }
        if ((reqCnt % 250) == 0)
        {
            Vps_printf("ReqCnt = %d\n", reqCnt);
        }
    }
    VpsUtils_prfLoadCalcStop();
    VpsUtils_prfTsEnd(prfTsHandle,
        appCompCurCfg->loopCount * appCompCurCfg->chPerHndl);
    VpsUtils_prfLoadPrintAll(TRUE);
    VpsUtils_prfTsPrint(prfTsHandle, TRUE);
    VpsUtils_prfLoadCalcReset();
    if (0 == retVal)
    {
        Vps_printf("M2mCompExample:Processing of the frames passed\n");
    }
    retVal = FVID2_delete(fvid2Handle, NULL);
    Semaphore_delete(&ReqProcessed);
    return retVal;
}



/**
 *  Driver callback function.
 */
static Int32 vpsCompCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    /* Request completed processing */
    Semaphore_post(ReqProcessed);

    return (retVal);
}



/**
 *  Driver error callback function.
 */
static Int32 vpsCompErrCbFxn(FVID2_Handle handle,
                            Ptr appData,
                            Void *errList,
                            Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}


/**
 *  Initialize the global variables and frame pointers.
 */
static Void vpsCompInitVariables(void)
{
    UInt32              frmListCnt, reqCnt, frmCnt, chCnt;
    FVID2_FrameList     *inFrameListPtr;
    FVID2_FrameList     *outFrameListPtr;

    for (chCnt = 0;  chCnt < appCompCurCfg->chPerHndl; chCnt++)
    {
        chPrmsComp[chCnt].outFmtSc5 = &outFmtSc5[chCnt];
        chPrmsComp[chCnt].sc5Cfg  = &chSc5Cfg[chCnt];
        chPrmsComp[chCnt].sc5CropCfg = &chSc5CropCfg[chCnt];
        chPrmsComp[chCnt].grpxPrms     = NULL;
        chPrmsComp[chCnt].deiScCfg     = NULL;
        chPrmsComp[chCnt].deiScCropCfg = NULL;
        chPrmsComp[chCnt].outFmtVip0   = NULL;
        chPrmsComp[chCnt].outFmtVip0Sc = NULL;
        chPrmsComp[chCnt].outFmtVip1Sc = NULL;
        chPrmsComp[chCnt].vip0CropCfg  = NULL;
        chPrmsComp[chCnt].vip0ScCfg    = NULL;
        chPrmsComp[chCnt].vip1CropCfg  = NULL;
        chPrmsComp[chCnt].vip1ScCfg    = NULL;
    }

    for (reqCnt = 0; reqCnt < 1; reqCnt++)
    {
        processList[reqCnt].inFrameList[0] = &inFrameList[reqCnt];
        processList[reqCnt].outFrameList[0] = &outFrameList[reqCnt];
        processList[reqCnt].numInLists = 1;
        processList[reqCnt].numOutLists = 1;
        processList[reqCnt].reserved = NULL;
        for (frmListCnt = 0u; frmListCnt < 1; frmListCnt++)
        {
            inFrameListPtr = processList[reqCnt].inFrameList[0];
            inFrameListPtr->numFrames = appCompCurCfg->chPerHndl;
            inFrameListPtr->perListCfg = NULL;
            inFrameListPtr->reserved = NULL;
            for (frmCnt = 0; frmCnt < appCompCurCfg->chPerHndl; frmCnt++)
            {
                inFrameListPtr->frames[frmCnt] = &inFrames[
                                    (reqCnt * 1) + frmCnt];
                inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_SP_Y_ADDR_IDX] =
                    inputBuffer[(reqCnt * 1) + frmCnt][0];
                inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_SP_CBCR_ADDR_IDX] =
                    inputBuffer[(reqCnt * 1) + frmCnt][1];
                 inFrameListPtr->frames[frmCnt]->fid = FVID2_FID_FRAME;
                 inFrameListPtr->frames[frmCnt]->channelNum = frmCnt;
                 inFrameListPtr->frames[frmCnt]->appData = NULL;
                 inFrameListPtr->frames[frmCnt]->perFrameCfg = NULL;
                 if (appCompCurCfg->subFrameProcEn){
                     inFrameListPtr->frames[frmCnt]->subFrameInfo = NULL;
                 }
                 inFrameListPtr->frames[frmCnt]->reserved = NULL;
             }
        }
        for (frmListCnt = 0u; frmListCnt < OUT_NUM_FRAMELIST; frmListCnt++)
        {
            outFrameListPtr = processList[reqCnt].outFrameList[0];
            outFrameListPtr->numFrames = appCompCurCfg->chPerHndl;
            outFrameListPtr->perListCfg = NULL;
            outFrameListPtr->reserved = NULL;
            for (frmCnt = 0; frmCnt < appCompCurCfg->chPerHndl; frmCnt++)
            {
                outFrameListPtr->frames[frmCnt] =
                    &outFrames[(reqCnt * 1) + frmCnt];;
                outFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_INT_ADDR_IDX] =
                    outputBuffer[(reqCnt * 1) + frmCnt][0];
                 outFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_SP_CBCR_ADDR_IDX] =
                    outputBuffer[(reqCnt * 1) + frmCnt][1];
                 outFrameListPtr->frames[frmCnt]->fid = FVID2_FID_FRAME;
                 outFrameListPtr->frames[frmCnt]->channelNum = frmCnt;
                 outFrameListPtr->frames[frmCnt]->appData = NULL;
                 outFrameListPtr->frames[frmCnt]->perFrameCfg = NULL;
                 outFrameListPtr->frames[frmCnt]->subFrameInfo = NULL;
                 outFrameListPtr->frames[frmCnt]->reserved = NULL;
             }
        }
    }

    /* Init error process list */
    memset(&errProcessList, 0u, sizeof (errProcessList));
}

static Int32 vpsCompAllocInBufs()
{
    UInt32      inputBufferSize;
    UInt32      chCnt;
    Int32       retVal = 0;
    UInt8       *inputTempbuffer;
    /* Allocate temporary buffer for loading data through CCS scripting console
     */
    if (appCompCurCfg->inputDataFmt == FVID2_DF_YUV420SP_UV)
    {
        inputBufferSize = (appCompCurCfg->inputWidth *
                         appCompCurCfg->inputHeight * 3) >> 1;
    }
    else
    {
        inputBufferSize = appCompCurCfg->inputWidth *
                          appCompCurCfg->inputHeight * 2;
    }
    inputTempbuffer = (UInt8 *)VpsUtils_memAlloc(
                                            appCompCurCfg->chPerHndl *
                                            inputBufferSize,
                                            BUFFER_ALIGNMENT);
    if (NULL != inputTempbuffer)
    {
        Vps_printf("M2mCompExample:Load %d buffer of size %dx%d to location: 0x%p\n",
            appCompCurCfg->chPerHndl, appCompCurCfg->inputWidth, appCompCurCfg->inputHeight, inputTempbuffer);
    }
    else
    {
        Vps_printf(
        "M2mCompExample: %s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EALLOC;
        return retVal;
    }
    /* If input is from non tiled space only update the buffer pointers.
     * No need to copy data
     */
    for (chCnt = 0; chCnt < appCompCurCfg->chPerHndl; chCnt++)
    {

        inputBuffer[chCnt][0] = &inputTempbuffer[chCnt * inputBufferSize];

        /* For YUV420 SP UV plane Don't care for YUV422I  format */
        inputBuffer[chCnt][1] =
            &inputTempbuffer[(chCnt * inputBufferSize) +
                (appCompCurCfg->inputWidth * appCompCurCfg->inputHeight)];
    }
    return retVal;
}

static Int32  vpsCompAllocOutBufs()
{
    UInt32      outputBufferSize;
    UInt32      chCnt;
    Int32       retVal = 0;
    UInt8       *outTempBuffer;

    /* Allocate temporary buffer for loading data through CCS scripting console
     */
    if (appCompCurCfg->outputDataFmt == FVID2_DF_YUV420SP_UV)
    {
        outputBufferSize = (appCompCurCfg->outputWidth *
                         appCompCurCfg->outputHeight * 3) >> 1;
    }
    else
    {
        outputBufferSize = appCompCurCfg->outputWidth *
                          appCompCurCfg->outputHeight * 2;
    }

    outTempBuffer = (UInt8 *)VpsUtils_memAlloc(
                                outputBufferSize * appCompCurCfg->chPerHndl,
                                BUFFER_ALIGNMENT);
    Vps_printf("M2mCompExample:Output Buffers Start Address 0x%p\n", outTempBuffer);
    if (NULL == outTempBuffer)
    {
        Vps_printf(
        "M2mCompExample: %s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EALLOC;
        return retVal;
    }

    /* If input is from non tiled space only update the buffer pointers.
     * No need to copy data
     */
    for (chCnt = 0; chCnt < appCompCurCfg->chPerHndl; chCnt++)
    {

        outputBuffer[chCnt][0] = &outTempBuffer[chCnt * outputBufferSize];

        /* For YUV420 SP UV plane Don't care for YUV422I  format */
        outputBuffer[chCnt][1] =
            &outTempBuffer[(chCnt * outputBufferSize) +
                (appCompCurCfg->outputWidth * appCompCurCfg->outputHeight)];
    }
    return retVal;
}

static Void vpsCompFreeInOutBufs()
{
    UInt32 inputBufferSize, outputBufferSize;


    if (FVID2_DF_YUV420SP_UV == appCompCurCfg->inputDataFmt)
    {
        inputBufferSize = (appCompCurCfg->inputWidth *
                         appCompCurCfg->inputHeight * 3) >> 1;
    }
    else
    {
        inputBufferSize = appCompCurCfg->inputWidth *
                          appCompCurCfg->inputHeight * 2;
    }
    if (FVID2_DF_YUV420SP_UV == appCompCurCfg->outputDataFmt)
    {
        outputBufferSize = (appCompCurCfg->outputWidth *
                         appCompCurCfg->outputHeight * 3) >> 1;
    }
    else
    {
        outputBufferSize = appCompCurCfg->outputWidth *
                         appCompCurCfg->outputHeight * 2;
    }

    VpsUtils_memFree(outputBuffer[0][0],
        outputBufferSize * appCompCurCfg->chPerHndl);
    VpsUtils_memFree(inputBuffer[0][0],
            inputBufferSize * appCompCurCfg->chPerHndl);
}

static Int32 vpsCompGetTstId()
{
    Int32 testId;
    UInt32 tstCnt;
    Vps_printf("Enter TestCase ID:\n");
    for (tstCnt = 0; tstCnt < sizeof(appCompCfg)/sizeof(vpsAppM2mCompCfg); tstCnt++)
    {
        Vps_printf("%d:\t%s\n", tstCnt, appCompCfg[tstCnt].testDesc);
    }
    Vps_printf("%d:\t%s\n", 100,"EXIT\n");

    //scanf("%d", &testId);
    testId=0; //?LK debug
    return testId;
}
