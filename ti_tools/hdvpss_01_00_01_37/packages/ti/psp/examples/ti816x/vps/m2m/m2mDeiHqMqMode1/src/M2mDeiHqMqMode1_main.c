/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file M2mDeiHqMqMode1_main.c
 *
 *  \brief VPS DEI HQ dual scale output memory to memory driver example file
 *  illustrating the Mode 1 mode of the driver - N-1 field buffer is
 *  managed by application and is provided to HQ driver by taking the even field
 *  output from DEI driver.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mDei.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Driver instance to test. DEIH and DEIM M2M driver supports 3 instances i.e.
 * dual output, single vip output and single sc output. This
 * instances represents how many and which outputs will be used in this
 * instance. */
#define DEIHQ_DRIVER_INSTANCE       (VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0)
#define DEI_DRIVER_INSTANCE         (VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1)

/* Number of channels to submit per call. */
#define DEI_NUM_CHANNELS            (1u)

/* Number of input buffers to load.
 * Note: This should be greater than or equal to DEI_NUM_CHANNELS and multiple
 * of 2 for mode 1. */
#define DEI_NUM_IN_BUFFERS          (20u)

/* Number of output buffers to allocate.
 * Note: Fore mode 1, every two field generates one frame. Hence only half
 * the number of input buffers are sufficient. */
#define DEI_NUM_OUT_BUFFERS         (DEI_NUM_IN_BUFFERS / 2u)

/* Number of loop count. */
#define DEI_TOTAL_LOOP_COUNT        (DEI_NUM_IN_BUFFERS / 2u)

/* Width of the DEI input frame. */
#define DEI_IN_WIDTH                (720u)
/* Height of the DEI input frame. Since DEI is deinterlacing, it should be
 * height of the field */
/* Make pitch to the nearest multiple of 16. */
#define DEI_IN_PITCH                (720u)
#define DEI_IN_HEIGHT               (240u)
#define DEI_IN_BUF_SIZE             (DEI_IN_PITCH * DEI_IN_HEIGHT * 3u / 2u)

/* DEIHQ output frame video parameters. Scaling using DEI Scalar. These
   parameters can change depending on the scaling required using SC_HQ. */
#define DEIHQ_OUT0_WIDTH            (360u)
/* Make pitch to the nearest multiple of 16. */
#define DEIHQ_OUT0_PITCH            (720u)  /* Pitch = 2xwidth for intereaved */
#define DEIHQ_OUT0_HEIGHT           (240u)
#define DEIHQ_OUT0_BUF_SIZE         (DEIHQ_OUT0_PITCH * DEIHQ_OUT0_HEIGHT)

/* DEI-VIP output frame video parameters. Scaling using VIP scalar. */
#define DEIHQ_OUT1_WIDTH            (720u)
/* Make pitch to the nearest multiple of 16. */
#define DEIHQ_OUT1_PITCH            (720u)
#define DEIHQ_OUT1_HEIGHT           (480u)
#define DEIHQ_OUT1_BUF_SIZE     (DEIHQ_OUT1_PITCH * DEIHQ_OUT1_HEIGHT * 3u / 2u)

/* DEI output frame video parameters. Scalar in the DEI is not used in
   DEIHQ Mode-1. So these parameters should be same as the input parameters. */
#define DEI_OUT0_WIDTH              (DEI_IN_WIDTH)
/* Make pitch to the nearest multiple of 16. */
#define DEI_OUT0_PITCH              (DEI_OUT0_WIDTH * 2u)
#define DEI_OUT0_HEIGHT             (DEI_IN_HEIGHT)
#define DEI_OUT0_BUF_SIZE           (DEI_OUT0_PITCH * DEI_OUT0_HEIGHT)

/* DEI-VIP output frame video parameters. Scaling using VIP scalar. */
#define DEI_OUT1_WIDTH              (360u)
/* Make pitch to the nearest multiple of 16. */
#define DEI_OUT1_PITCH              (368u)
#define DEI_OUT1_HEIGHT             (240u)
#define DEI_OUT1_BUF_SIZE           (DEI_OUT1_PITCH * DEI_OUT1_HEIGHT * 3u / 2u)

/* Number of framelist object to allocate. */
#define DEI_NUM_FRAMELIST           (3u)

/* Number of error framelist object to allocate. */
#define DEI_NUM_ERR_FRAMELIST       (4u)

/* Number of FVID2 frame object to allocate per framelist */
#define DEI_NUM_FRAMES_PER_LIST     (DEI_NUM_CHANNELS)

/* Application name string used in print statements. */
#define APP_NAME                    "DeiMode1"

/* Test application stack size. */
#define APP_TSK_STACK_SIZE          (10u * 1024u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief DEI M2M application object.
 */
typedef struct
{
    FVID2_Handle            fvidHandle;
    /**< DEI M2M driver handle. */
    UInt32                  driverInst;
    /**< DEI M2M driver instance. */
    UInt32                  mode;
    /**< Mode of operation - per channel or per handle. */
    UInt32                  numCh;
    /**< Number of channels per handle. */

    UInt8                  *inBuf;
    /**< Input buffer. */
    UInt8                  *out0Buf;
    /**< DEI out buffer. */
    UInt8                  *out1Buf;
    /**< VIP out buffer. */

    UInt32                  numInBuffers;
    /**< Number of input buffers. */
    UInt32                  numOutBuffers;
    /**< Number of output buffers. */
    UInt32                  inBufIdx;
    /**< Current input buffer index. Used to wrap around input buffer. */
    UInt32                  outBufIdx;
    /**< Current output buffer index. Used to wrap around output buffer. */
    UInt32                  completedLoopCnt;
    /**< Total number of completed frames. */
    Semaphore_Handle        reqCompleteSem;
    /**< Request complete semaphore used to sync between callback and task. */

    UInt32                  inWidth;
    /**< Input width in pixels. */
    UInt32                  inPitch;
    /**< Input pitch in bytes. */
    UInt32                  inHeight;
    /**< Input height. */
    UInt32                  inBufSize;
    /**< Input buffer size. */

    UInt32                  out0Width;
    /**< DEI output width in pixels. */
    UInt32                  out0Pitch;
    /**< DEI output pitch in bytes. */
    UInt32                  out0Height;
    /**< DEI output height. */
    UInt32                  out0BufSize;
    /**< DEI output buffer size. */

    UInt32                  out1Width;
    /**< VIP output width in pixels. */
    UInt32                  out1Pitch;
    /**< VIP output pitch in bytes. */
    UInt32                  out1Height;
    /**< VIP output height. */
    UInt32                  out1BufSize;
    /**< VIP output buffer size. */

    FVID2_ProcessList       processList;
    /**< Process list used for queue/dequeue operations. */
    FVID2_FrameList         frameList[DEI_NUM_FRAMELIST];
    /**< Frame list used for queue/dequeue operations. */
    FVID2_Frame             frames[DEI_NUM_FRAMELIST]
                                  [DEI_NUM_FRAMES_PER_LIST];
    /**< Frames used for queue/dequeue operations. */

    FVID2_ProcessList       errProcessList;
    /**< Process list used for error operations. */

    Vps_M2mDeiCreateParams  createParams;
    /**< DEI M2M driver create parameters. */
    Vps_M2mDeiChParams      chParams[DEI_NUM_CHANNELS];
    /**< DEI M2M driver channel parameters for each of the channels. */
    FVID2_Format            outFmtDei[DEI_NUM_CHANNELS];
    /**< Memory for DEI out format. */
    FVID2_Format            outFmtVip[DEI_NUM_CHANNELS];
    /**< Memory for VIP out format. */
    FVID2_Format            inFmtFldN_1[DEI_NUM_CHANNELS];
    /**< Memory for the N-1 field input format. */

    Vps_DeiHqConfig         deiHqCfg[DEI_NUM_CHANNELS];
    /**< Memory for DEI HQ config. */
    Vps_DeiConfig           deiCfg[DEI_NUM_CHANNELS];
    /**< Memory for DEI config. */
    Vps_ScConfig            scCfg[DEI_NUM_CHANNELS];
    /**< Memory for DEI scalar config. */
    Vps_ScConfig            vipScCfg[DEI_NUM_CHANNELS];
    /**< Memory for VIP scalar config. */
    Vps_CropConfig          vipScCropCfg[DEI_NUM_CHANNELS];
    /**< Memory for the VIP crop configuration */
    Vps_CropConfig          scCropCfg[DEI_NUM_CHANNELS];
    /**< Memory for the DEI crop configuration */
} App_DeiM2mObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void vpsDeiMode1M2mTestTask(void);
static Int32 vpsDeiMode1M2mTest(App_DeiM2mObj *appDeiHqObj,
                                App_DeiM2mObj *appDeiObj);
static Int32 vpsDeiMode1OpenDriver(App_DeiM2mObj *appObj);
static Int32 vpsDeiMode1CloseDriver(App_DeiM2mObj *appObj);
static Int32 vpsDeiMode1M2mAppCbFxn(FVID2_Handle handle,
                                    Ptr appData,
                                    Ptr reserved);
static Int32 vpsDeiMode1M2mAppErrCbFxn(FVID2_Handle handle,
                                       Ptr appData,
                                       Void *errList,
                                       Ptr reserved);
static Void vpsDeiHqM2mUpdateProcList(App_DeiM2mObj *appObj,
                                      FVID2_ProcessList *pList);
static Void vpsDeiM2mUpdateProcList(App_DeiM2mObj *appObj,
                                    FVID2_ProcessList *pList);
static Void vpsDeiMode1M2mInitVariables(App_DeiM2mObj *appObj);
static Int32 vpsDeiAllocCtxBuffer(App_DeiM2mObj *appObj);
static Int32 vpsDeiFreeCtxBuffer(App_DeiM2mObj *appObj);
static Int32 vpsDeiProgramScCoeff(App_DeiM2mObj *appObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(App_mainTskStack, 32)
#pragma DATA_SECTION(App_mainTskStack, ".bss:taskStackSection")
static UInt8 App_mainTskStack[APP_TSK_STACK_SIZE];

/* DEI M2M application object per handle. */
static App_DeiM2mObj DeiHqM2mObjs, DeiM2mObjs;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Application main function.
 */
Int32 main(void)
{
    Task_Params tskParams;

    Task_Params_init(&tskParams);
    tskParams.stack = App_mainTskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    Task_create((Task_FuncPtr) vpsDeiMode1M2mTestTask, &tskParams, NULL);

    BIOS_start();

    return (0);
}



/**
 *  vpsDeiMode1M2mTestTask
 *  Application test task.
 */
static Void vpsDeiMode1M2mTestTask(void)
{
    char                        ch;
    Int32                       retVal;
    App_DeiM2mObj               *appDeiHqObj, *appDeiObj;
    Vps_PlatformInitParams      platformInitPrms;

    /* Init Sequence */
    platformInitPrms.isPinMuxSettingReq = TRUE;
    retVal = Vps_platformInit(&platformInitPrms);
    VpsUtils_memInit();

    /* Get the version string */
    Vps_printf("HDVPSS Drivers Version: %s\n", FVID2_getVersionString());

    /* Init FVID2 and VPS */
    retVal = FVID2_init(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Configure DEI HQ objects */
    appDeiHqObj = &DeiHqM2mObjs;
    appDeiHqObj->driverInst = DEIHQ_DRIVER_INSTANCE;
    appDeiHqObj->mode = VPS_M2M_CONFIG_PER_CHANNEL;
    appDeiHqObj->numCh = DEI_NUM_CHANNELS;
    appDeiHqObj->inBuf = VpsUtils_memAlloc(
                             DEI_NUM_IN_BUFFERS * DEI_IN_BUF_SIZE,
                             VPS_BUFFER_ALIGNMENT);
    if (NULL == appDeiHqObj->inBuf)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    appDeiHqObj->out0Buf = VpsUtils_memAlloc(
                               DEI_NUM_OUT_BUFFERS * DEIHQ_OUT0_BUF_SIZE,
                               VPS_BUFFER_ALIGNMENT);
    if (NULL == appDeiHqObj->out0Buf)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    appDeiHqObj->out1Buf = VpsUtils_memAlloc(
                               DEI_NUM_OUT_BUFFERS * DEIHQ_OUT1_BUF_SIZE,
                               VPS_BUFFER_ALIGNMENT);
    if (NULL == appDeiHqObj->out1Buf)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    appDeiHqObj->numInBuffers = DEI_NUM_IN_BUFFERS;
    appDeiHqObj->numOutBuffers = DEI_NUM_OUT_BUFFERS;
    appDeiHqObj->inWidth = DEI_IN_WIDTH;
    appDeiHqObj->inPitch = DEI_IN_PITCH;
    appDeiHqObj->inHeight = DEI_IN_HEIGHT;
    appDeiHqObj->out0Width = DEIHQ_OUT0_WIDTH;
    appDeiHqObj->out0Pitch = DEIHQ_OUT0_PITCH;
    appDeiHqObj->out0Height = DEIHQ_OUT0_HEIGHT;
    appDeiHqObj->out1Width = DEIHQ_OUT1_WIDTH;
    appDeiHqObj->out1Pitch = DEIHQ_OUT1_PITCH;
    appDeiHqObj->out1Height = DEIHQ_OUT1_HEIGHT;

    /* Configure DEI objects */
    appDeiObj = &DeiM2mObjs;
    appDeiObj->driverInst = DEI_DRIVER_INSTANCE;
    appDeiObj->mode = VPS_M2M_CONFIG_PER_CHANNEL;
    appDeiObj->numCh = DEI_NUM_CHANNELS;
    appDeiObj->inBuf = appDeiHqObj->inBuf;
    appDeiObj->out0Buf = VpsUtils_memAlloc(
                             DEI_NUM_OUT_BUFFERS * DEI_OUT0_BUF_SIZE,
                             VPS_BUFFER_ALIGNMENT);
    if (NULL == appDeiObj->out0Buf)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    appDeiObj->out1Buf = VpsUtils_memAlloc(
                             DEI_NUM_OUT_BUFFERS * DEI_OUT1_BUF_SIZE,
                               VPS_BUFFER_ALIGNMENT);
    if (NULL == appDeiObj->out1Buf)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    appDeiObj->numInBuffers = DEI_NUM_IN_BUFFERS;
    appDeiObj->numOutBuffers = DEI_NUM_OUT_BUFFERS;
    appDeiObj->inWidth = DEI_IN_WIDTH;
    appDeiObj->inPitch = DEI_IN_PITCH;
    appDeiObj->inHeight = DEI_IN_HEIGHT;
    appDeiObj->out0Width = DEI_OUT0_WIDTH;
    appDeiObj->out0Pitch = DEI_OUT0_PITCH;
    appDeiObj->out0Height = DEI_OUT0_HEIGHT;
    appDeiObj->out1Width = DEI_OUT1_WIDTH;
    appDeiObj->out1Pitch = DEI_OUT1_PITCH;
    appDeiObj->out1Height = DEI_OUT1_HEIGHT;

    /* Wait for the user to load the image */
    Vps_printf(
        "Load %d video frames/fields of size %dx%d to location: 0x%p\n",
        DEI_NUM_IN_BUFFERS, DEI_IN_WIDTH, DEI_IN_HEIGHT,
        appDeiHqObj->inBuf);
    Vps_printf("%s: Enter any key after loading...\n", APP_NAME);
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    Vps_printf("%s: DEI In Buffer Address       : 0x%p, Size: 0x%p\n",
        APP_NAME, appDeiHqObj->inBuf,
        DEI_NUM_IN_BUFFERS * DEI_IN_BUF_SIZE);
    Vps_printf("%s: DEIHQ Out Buffer Address    : 0x%p, Size: 0x%p\n",
        APP_NAME, appDeiHqObj->out0Buf,
        DEI_NUM_OUT_BUFFERS * DEIHQ_OUT0_BUF_SIZE);
    Vps_printf("%s: DEI Out Buffer Address    : 0x%p, Size: 0x%p\n",
        APP_NAME, appDeiObj->out0Buf,
        DEI_NUM_OUT_BUFFERS * DEI_OUT0_BUF_SIZE);
    Vps_printf("%s: DEIHQ-VIP Out Buffer Address: 0x%p, Size: 0x%p\n",
        APP_NAME, appDeiHqObj->out1Buf,
        DEI_NUM_OUT_BUFFERS * DEIHQ_OUT1_BUF_SIZE);
    Vps_printf("%s: DEI-VIP Out Buffer Address: 0x%p, Size: 0x%p\n",
        APP_NAME, appDeiObj->out1Buf,
        DEI_NUM_OUT_BUFFERS * DEI_OUT1_BUF_SIZE);

    /* Run the test */
    vpsDeiMode1M2mTest(appDeiHqObj, appDeiObj);

    /* Free buffers */
    VpsUtils_memFree(
        appDeiHqObj->inBuf,
        DEI_NUM_IN_BUFFERS * DEI_IN_BUF_SIZE);
    VpsUtils_memFree(
        appDeiHqObj->out0Buf,
        DEI_NUM_OUT_BUFFERS * DEIHQ_OUT0_BUF_SIZE);
    VpsUtils_memFree(
        appDeiHqObj->out1Buf,
        DEI_NUM_OUT_BUFFERS * DEIHQ_OUT1_BUF_SIZE);
    VpsUtils_memFree(
        appDeiObj->out1Buf,
        DEI_NUM_OUT_BUFFERS * DEI_OUT1_BUF_SIZE);

    Vps_printf("%s: Enter any key after saving...\n", APP_NAME);
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    /* De-Init Sequence */
    retVal = FVID2_deInit(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    VpsUtils_memDeInit();
    Vps_platformDeInit();

    Vps_printf("%s: Test Successfull!!\n", APP_NAME);
}



/**
 *  vpsDeiMode1M2mTest
 *  DEI M2M test function.
 */
static Int32 vpsDeiMode1M2mTest(App_DeiM2mObj *appDeiHqObj,
                                App_DeiM2mObj *appDeiObj)
{
    Int32                   retVal = FVID2_SOK;
    FVID2_ProcessList      *pListDeiHq, *pListDei;

    /* Open DEIHQ driver */
    retVal = vpsDeiMode1OpenDriver(appDeiHqObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Open DEI driver */
    retVal = vpsDeiMode1OpenDriver(appDeiObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    pListDeiHq = &appDeiHqObj->processList;
    pListDei = &appDeiObj->processList;
    while (appDeiObj->completedLoopCnt < DEI_TOTAL_LOOP_COUNT)
    {
        /* Update DEI buffer address */
        vpsDeiM2mUpdateProcList(appDeiObj, pListDei);

        /* Process frames */
        retVal = FVID2_processFrames(appDeiObj->fvidHandle, pListDei);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Processing of the request failed\n", APP_NAME);
            break;
        }

        /* Wait till frames gets processed */
        Semaphore_pend(appDeiObj->reqCompleteSem, BIOS_WAIT_FOREVER);

        /* Get processed frames */
        retVal = FVID2_getProcessedFrames(
                     appDeiObj->fvidHandle,
                     pListDei,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Get processed request failed\n", APP_NAME);
            break;
        }

        /* Update DEIHQ buffer address */
        vpsDeiHqM2mUpdateProcList(appDeiHqObj, pListDeiHq);

        /* Copy output framelist of DEI to output framelist of DEIHQ as
         * N-1 field input framelist */
        pListDeiHq->inFrameList[1u] = pListDei->outFrameList[0u];

        /* Process frames */
        retVal = FVID2_processFrames(appDeiHqObj->fvidHandle, pListDeiHq);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Processing of the request failed\n", APP_NAME);
            break;
        }

        /* Wait till frames gets processed */
        Semaphore_pend(appDeiHqObj->reqCompleteSem, BIOS_WAIT_FOREVER);

        /* Get processed frames */
        retVal = FVID2_getProcessedFrames(
                     appDeiHqObj->fvidHandle,
                     pListDeiHq,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Get processed request failed\n", APP_NAME);
            break;
        }

        appDeiObj->completedLoopCnt++;
        appDeiHqObj->completedLoopCnt++;
        Vps_printf("%s: Loop count: %d\n",
            APP_NAME, appDeiObj->completedLoopCnt);
    }

    /* Close DEIHQ driver */
    retVal = vpsDeiMode1CloseDriver(appDeiHqObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Close DEI driver */
    retVal = vpsDeiMode1CloseDriver(appDeiObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    return (retVal);
}



/*
 *  vpsDeiMode1OpenDriver
 *  Opens and configures the driver instance.
 */
static Int32 vpsDeiMode1OpenDriver(App_DeiM2mObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    Semaphore_Params        semParams;
    FVID2_CbParams          cbParams;
    Vps_M2mDeiCreateStatus  createStatus;

    Semaphore_Params_init(&semParams);
    appObj->reqCompleteSem = Semaphore_create(0, &semParams, NULL);

    /* Initialize frame pointers */
    vpsDeiMode1M2mInitVariables(appObj);

    /* Open driver */
    cbParams.cbFxn = vpsDeiMode1M2mAppCbFxn;
    cbParams.errCbFxn = vpsDeiMode1M2mAppErrCbFxn;
    cbParams.errList = &appObj->errProcessList;
    cbParams.appData = appObj;
    cbParams.reserved = NULL;
    appObj->fvidHandle = FVID2_create(
                             FVID2_VPS_M2M_DEI_DRV,
                             appObj->driverInst,
                             &appObj->createParams,
                             &createStatus,
                             &cbParams);
    if (NULL == appObj->fvidHandle)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, createStatus.retVal,
            __LINE__);
        return (createStatus.retVal);
    }
    Vps_printf("%s: M2M DEI Driver Opened for Instance %d!!\n",
        APP_NAME,
        appObj->driverInst);
    Vps_printf("%s: Maximum Handles: %d\n",
        APP_NAME,
        createStatus.maxHandles);
    Vps_printf("%s: Maximum Channels per Handle: %d\n",
        APP_NAME,
        createStatus.maxChPerHandle);
    Vps_printf("%s: Maximum same Channels per Request: %d\n",
        APP_NAME,
        createStatus.maxSameChPerRequest);
    Vps_printf("%s: Maximum Request in Input Queue: %d\n",
        APP_NAME,
        createStatus.maxReqInQueue);

    /* Program scalar coefficients */
    retVal = vpsDeiProgramScCoeff(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Programming of coefficients failed\n", APP_NAME);
        return (retVal);
    }

    /* Allocate DEI context buffers and provide them to the driver */
    retVal = vpsDeiAllocCtxBuffer(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Allocation of context buffer failed\n", APP_NAME);
        return (retVal);
    }

    return (retVal);
}



/*
 *  vpsDeiMode1CloseDriver
 *  Closes the driver instance.
 */
static Int32 vpsDeiMode1CloseDriver(App_DeiM2mObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    FVID2_ProcessList      *pList;

    /* Dequeue all pending request from driver */
    pList= &appObj->processList;
    while (1u)
    {
        /* Get processed frames */
        retVal = FVID2_getProcessedFrames(
                     appObj->fvidHandle,
                     pList,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK == retVal)
        {
            appObj->completedLoopCnt++;
            Vps_printf("%s: Loop count: %d\n",
                APP_NAME, appObj->completedLoopCnt);
        }
        else if (FVID2_ENO_MORE_BUFFERS == retVal)
        {
            Vps_printf("%s: All request dequeued from driver\n", APP_NAME);
            break;
        }
        else if (FVID2_EAGAIN == retVal)
        {
            /* Still request pending with driver. Try after sometime. */
            Task_yield();
        }
        else
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            break;
        }
    }

    /* Free context buffers */
    retVal = vpsDeiFreeCtxBuffer(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Freeing of context buffer failed\n", APP_NAME);
        return (retVal);
    }

    /* Close driver */
    retVal = FVID2_delete(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    Semaphore_delete(&appObj->reqCompleteSem);

    return (retVal);
}



/**
 *  vpsDeiMode1M2mAppCbFxn
 *  Driver callback function.
 */
static Int32 vpsDeiMode1M2mAppCbFxn(FVID2_Handle handle,
                                    Ptr appData,
                                    Ptr reserved)
{
    Int32               retVal = FVID2_SOK;
    App_DeiM2mObj      *appObj = (App_DeiM2mObj *) appData;

    /* Transfer complete. Signal it */
    Semaphore_post(appObj->reqCompleteSem);

    return (retVal);
}



/**
 *  vpsDeiMode1M2mAppErrCbFxn
 *  Driver error callback function.
 */
static Int32 vpsDeiMode1M2mAppErrCbFxn(FVID2_Handle handle,
                                       Ptr appData,
                                       Void *errList,
                                       Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}



/**
 *  vpsDeiHqM2mUpdateProcList
 */
static Void vpsDeiHqM2mUpdateProcList(App_DeiM2mObj *appObj,
                                      FVID2_ProcessList *pList)
{
    UInt32                  frmCnt;
    UInt8                  *tempPtr;
    FVID2_Frame            *frm;
    FVID2_FrameList        *in0FrmList, *out0FrmList, *out1FrmList;

    in0FrmList = pList->inFrameList[0u];
    out0FrmList = pList->outFrameList[0u];
    out1FrmList = pList->outFrameList[1u];

    /* Use the same input buffer for all channels */
    if (appObj->numInBuffers <= appObj->inBufIdx)
    {
        /* DEIHQ operates on odd fields */
        appObj->inBufIdx = 1u;
    }

    for (frmCnt = 0u; frmCnt < appObj->numCh; frmCnt++)
    {
        if (appObj->numOutBuffers <= appObj->outBufIdx)
        {
            appObj->outBufIdx = 0u;
        }

        /* Update the DEI input buffer */
        frm = in0FrmList->frames[frmCnt];
        frm->fid = FVID2_FID_BOTTOM;
        tempPtr = appObj->inBuf + appObj->inBufIdx * appObj->inBufSize;
        frm->addr[FVID2_FIELD_MODE_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
            tempPtr;
        frm->addr[FVID2_FIELD_MODE_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
            tempPtr + (appObj->inPitch * appObj->inHeight);

        /* Update the DEI writeback out buffer */
        frm = out0FrmList->frames[frmCnt];
        frm->fid = FVID2_FID_FRAME;
        tempPtr = appObj->out0Buf + appObj->outBufIdx * appObj->out0BufSize;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] = tempPtr;

        /* Update the DEI-VIP out buffer */
        frm = out1FrmList->frames[frmCnt];
        frm->fid = FVID2_FID_FRAME;
        tempPtr = appObj->out1Buf + appObj->outBufIdx * appObj->out1BufSize;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
            tempPtr;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
            tempPtr + (appObj->out1Pitch * appObj->out1Height);

        appObj->outBufIdx++;
    }

    appObj->inBufIdx += 2u;

    return;
}



/**
 *  vpsDeiHqM2mUpdateProcList
 */
static Void vpsDeiM2mUpdateProcList(App_DeiM2mObj *appObj,
                                    FVID2_ProcessList *pList)
{
    UInt32                  frmCnt;
    UInt8                  *tempPtr;
    FVID2_Frame            *frm;
    FVID2_FrameList        *in0FrmList, *out0FrmList, *out1FrmList;

    in0FrmList = pList->inFrameList[0u];
    out0FrmList = pList->outFrameList[0u];
    out1FrmList = pList->outFrameList[1u];

    /* Use the same input buffer for all channels */
    if (appObj->numInBuffers <= appObj->inBufIdx)
    {
        /* DEI operates on even fields */
        appObj->inBufIdx = 0u;
    }

    for (frmCnt = 0u; frmCnt < appObj->numCh; frmCnt++)
    {
        /* Use different output buffer for each channel */
        if (appObj->numOutBuffers <= appObj->outBufIdx)
        {
            appObj->outBufIdx = 0u;
        }

        /* Update the DEI input buffer */
        frm = in0FrmList->frames[frmCnt];
        frm->fid = FVID2_FID_FRAME;
        tempPtr = appObj->inBuf + appObj->inBufIdx * appObj->inBufSize;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
            tempPtr;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
            tempPtr + (appObj->inPitch * appObj->inHeight);

        /* Update the DEI writeback out buffer */
        frm = out0FrmList->frames[frmCnt];
        frm->fid = FVID2_FID_FRAME;
        tempPtr = appObj->out0Buf + appObj->outBufIdx * appObj->out0BufSize;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] = tempPtr;

        /* Update the DEI-VIP out buffer */
        frm = out1FrmList->frames[frmCnt];
        frm->fid = FVID2_FID_FRAME;
        tempPtr = appObj->out1Buf + appObj->outBufIdx * appObj->out1BufSize;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
            tempPtr;
        frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
            tempPtr + (appObj->out1Pitch * appObj->out1Height);

        appObj->outBufIdx++;
    }

    appObj->inBufIdx += 2u;

    return;
}



/**
 *  vpsInitDefaultScCropParams
 *  Sets the default scalar parameters.
 */
static Void vpsInitDefaultScCropParams(App_DeiM2mObj *appObj,
                                   Vps_ScConfig *scCfg,
                                   Vps_CropConfig *cropCfg)
{
    scCfg->bypass = FALSE;
    cropCfg->cropStartX = 0u;
    cropCfg->cropStartY = 0u;
    cropCfg->cropWidth = appObj->inWidth;
    cropCfg->cropHeight = appObj->inHeight * 2u;
    scCfg->nonLinear = FALSE;
    scCfg->stripSize = 0u;
    scCfg->vsType = VPS_SC_VST_POLYPHASE;
    scCfg->enablePeaking = TRUE;
    /* Edge detect is enabled for the SC_H and is dont care for all other
     * scalars
     */
    scCfg->enableEdgeDetect = TRUE;

    return;
}



/**
 *  vpsInitDefaultDeiHqParams
 *  Sets the default DEIHQ parameters.
 */
static Void vpsInitDefaultDeiHqParams(App_DeiM2mObj *appObj,
                                      Vps_DeiHqConfig *hqCfg)
{
    hqCfg->bypass = FALSE;
    hqCfg->inpMode = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
    hqCfg->tempInpEnable = TRUE;
    hqCfg->tempInpChromaEnable = TRUE;
    hqCfg->spatMaxBypass = FALSE;
    hqCfg->tempMaxBypass = FALSE;
    hqCfg->fldMode = VPS_DEIHQ_FLDMODE_4FLD;
    hqCfg->lcModeEnable = TRUE;
    hqCfg->mvstmEnable = TRUE;
    hqCfg->tnrEnable = FALSE;
    hqCfg->snrEnable = TRUE;
    hqCfg->sktEnable = FALSE;
    hqCfg->chromaEdiEnable = TRUE;

    return;
}



/**
 *  vpsInitDefaultDeiParams
 *  Sets the default DEI parameters.
 */
static Void vpsInitDefaultDeiParams(App_DeiM2mObj *appObj,
                                    Vps_DeiConfig *deiCfg)
{
    /* DEI is operating in bypass mode in DEIHQ Mode-1. */
    deiCfg->bypass = TRUE;
    deiCfg->inpMode = VPS_DEI_EDIMODE_LUMA_CHROMA;
    deiCfg->tempInpEnable = TRUE;
    deiCfg->tempInpChromaEnable = TRUE;
    deiCfg->spatMaxBypass = FALSE;
    deiCfg->tempMaxBypass = FALSE;
}



/**
 *  vpsDeiSetDefaultChParams
 *  Sets the default channel parameters.
 */
static Void vpsDeiSetDefaultChParams(App_DeiM2mObj *appObj,
                                     Vps_M2mDeiChParams *chParams,
                                     UInt32 chNum)
{
    vpsInitDefaultScCropParams(appObj, chParams->scCfg, chParams->deiCropCfg);
    vpsInitDefaultScCropParams(appObj,
                               chParams->vipScCfg,
                               chParams->vipCropCfg);
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst))
    {
        /* Configuring DEIHQ Parameters */
        vpsInitDefaultDeiHqParams(appObj, chParams->deiHqCfg);

        chParams->drnEnable = TRUE;
        chParams->comprEnable = FALSE;

        /* Enable scalars only if input and output size are not same
         * DEIHQ is in deinterlacing mode, multiply height by 2 */
        if ((appObj->inWidth == appObj->out0Width) &&
            ((appObj->inHeight * 2u) == appObj->out0Height))
        {
            chParams->scCfg->bypass = TRUE;
        }
        if ((appObj->inWidth == appObj->out1Width) &&
            ((appObj->inHeight * 2u) == appObj->out1Height))
        {
            chParams->vipScCfg->bypass = TRUE;
        }
        chParams->vipCropCfg->cropHeight = appObj->inHeight * 2u;
        chParams->deiCropCfg->cropHeight = appObj->inHeight * 2u;
    }
    else
    {
        vpsInitDefaultDeiParams(appObj, chParams->deiCfg);
        chParams->drnEnable = FALSE;
        chParams->comprEnable = FALSE;

        /* Enable scalars only if input and output size are not same
         * DEI is in bypass mode, DON'T multiply height by 2 */
        if ((appObj->inWidth == appObj->out0Width) &&
            ((appObj->inHeight) == appObj->out0Height))
        {
            chParams->scCfg->bypass = TRUE;
        }
        if ((appObj->inWidth == appObj->out1Width) &&
            ((appObj->inHeight) == appObj->out1Height))
        {
            chParams->vipScCfg->bypass = TRUE;
        }
        chParams->deiCropCfg->cropHeight = appObj->inHeight;
        chParams->vipCropCfg->cropHeight = appObj->inHeight;
    }

    /* Set the FVID2 format for DEI input */
    chParams->inFmt.channelNum = chNum;
    chParams->inFmt.width = appObj->inWidth;
    chParams->inFmt.height = appObj->inHeight;
    chParams->inFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->inPitch;
    chParams->inFmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->inPitch;
    chParams->inFmt.fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] = FALSE;
    chParams->inFmt.fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] = FALSE;
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst))
    {
        /* DEIHQ is in deinterlacing mode */
        chParams->inFmt.scanFormat = FVID2_SF_INTERLACED;
    }
    else
    {
        /* DEI is in progressive bypass mode even though buffer is field */
        chParams->inFmt.scanFormat = FVID2_SF_PROGRESSIVE;
    }
    chParams->inFmt.dataFormat = FVID2_DF_YUV420SP_UV;
    chParams->inFmt.bpp = FVID2_BPP_BITS12;

    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst))
    {
        /* Configure Format for N-1 field */
        chParams->inFmtFldN_1->channelNum = chNum;
        chParams->inFmtFldN_1->width = appObj->inWidth;
        chParams->inFmtFldN_1->height = appObj->inHeight;
        chParams->inFmtFldN_1->pitch[FVID2_YUV_INT_ADDR_IDX] =
            appObj->inPitch * 2u;
        chParams->inFmtFldN_1->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
        chParams->inFmtFldN_1->dataFormat = FVID2_DF_YUV422I_YUYV;
        chParams->inFmtFldN_1->scanFormat = FVID2_SF_INTERLACED;
        chParams->inFmtFldN_1->bpp = FVID2_BPP_BITS16;
    }
    else
    {
        chParams->inFmtFldN_1 = NULL;
    }

    /* Set the FVID2 format for DEI writeback output */
    chParams->outFmtDei->channelNum = chNum;
    chParams->outFmtDei->width = appObj->out0Width;
    chParams->outFmtDei->height = appObj->out0Height;
    chParams->outFmtDei->pitch[FVID2_YUV_INT_ADDR_IDX] = appObj->out0Pitch;
    chParams->outFmtDei->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chParams->outFmtDei->scanFormat = FVID2_SF_PROGRESSIVE;
    chParams->outFmtDei->dataFormat = FVID2_DF_YUV422I_YUYV;
    chParams->outFmtDei->bpp = FVID2_BPP_BITS16;

    /* Set the FVID2 format for DEI-VIP output */
    chParams->outFmtVip->channelNum = chNum;
    chParams->outFmtVip->width = appObj->out1Width;
    chParams->outFmtVip->height = appObj->out1Height;
    chParams->outFmtVip->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->out1Pitch;
    chParams->outFmtVip->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->out1Pitch;
    chParams->outFmtVip->fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] = FALSE;
    chParams->outFmtVip->fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] = FALSE;
    chParams->outFmtVip->scanFormat = FVID2_SF_PROGRESSIVE;
    chParams->outFmtVip->dataFormat = FVID2_DF_YUV420SP_UV;
    chParams->outFmtVip->bpp = FVID2_BPP_BITS12;

    return;
}



/**
 *  vpsDeiMode1M2mInitVariables
 *  Initialize the global variables and frame pointers.
 */
static Void vpsDeiMode1M2mInitVariables(App_DeiM2mObj *appObj)
{
    UInt32              fListCnt, frmCnt;
    FVID2_ProcessList  *pList;
    FVID2_FrameList    *fList;
    FVID2_Frame        *frm;
    UInt32              chCnt;
    Vps_M2mDeiChParams *chParams;

    /* Init create params */
    appObj->createParams.mode = appObj->mode;
    appObj->createParams.numCh = appObj->numCh;
    /* Since DEIHQ is operating in Mode-1, N-1 Field's context buffer
       should come from the application */
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst))
    {
        appObj->createParams.deiHqCtxMode = VPS_DEIHQ_CTXMODE_APP_N_1;
        /* Enable scalars only if input and output size are not same
         * DEIHQ is in deinterlacing mode, multiply height by 2 */
        if ((appObj->inWidth == appObj->out1Width) &&
            ((appObj->inHeight * 2u) == appObj->out1Height))
        {
            appObj->createParams.isVipScReq = FALSE;
        }
        else
        {
            appObj->createParams.isVipScReq = TRUE;
        }
    }
    else
    {
        appObj->createParams.deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;
        /* Enable scalars only if input and output size are not same
         * DEI is in bypass mode, DON'T multiply height by 2 */
        if ((appObj->inWidth == appObj->out1Width) &&
            ((appObj->inHeight) == appObj->out1Height))
        {
            appObj->createParams.isVipScReq = FALSE;
        }
        else
        {
            appObj->createParams.isVipScReq = TRUE;
        }
    }
    appObj->createParams.chParams = &appObj->chParams[0u];

    for (chCnt = 0u; chCnt < appObj->numCh; chCnt++)
    {
        chParams = &appObj->chParams[chCnt];

        /* Assign the memories for the pointer members */
        chParams->outFmtDei = &appObj->outFmtDei[chCnt];
        chParams->outFmtVip = &appObj->outFmtVip[chCnt];
        if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
            (VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == appObj->driverInst) ||
            (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst))
        {
            chParams->inFmtFldN_1 = &appObj->inFmtFldN_1[chCnt];
            chParams->deiHqCfg = &appObj->deiHqCfg[chCnt];
            chParams->deiCfg = NULL;
        }
        else
        {
            chParams->inFmtFldN_1 = NULL;
            chParams->deiHqCfg = NULL;
            chParams->deiCfg = &appObj->deiCfg[chCnt];
        }
        chParams->inMemType = VPS_VPDMA_MT_NONTILEDMEM;
        chParams->outMemTypeDei = VPS_VPDMA_MT_NONTILEDMEM;
        chParams->outMemTypeVip = VPS_VPDMA_MT_NONTILEDMEM;
        chParams->scCfg = &appObj->scCfg[chCnt];
        chParams->vipScCfg = &appObj->vipScCfg[chCnt];
        chParams->vipCropCfg = &appObj->vipScCropCfg[chCnt];
        chParams->deiCropCfg = &appObj->scCropCfg[chCnt];
        chParams->subFrameParams = NULL;

        /* Init channel params */
        vpsDeiSetDefaultChParams(appObj, chParams, chCnt);
    }

    appObj->inBufSize = appObj->inPitch * appObj->inHeight * 3u / 2u;
    appObj->out0BufSize = appObj->out0Pitch * appObj->out0Height;
    appObj->out1BufSize = appObj->out1Pitch * appObj->out1Height * 3u / 2u;
    /* Same input buffer is used for both the DEI drivers. DEIHQ will
       operate on odd fields and DEI will operate on even fields */
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst))
    {
        appObj->inBufIdx = 1u;
    }
    else
    {
        appObj->inBufIdx = 0u;
    }
    appObj->outBufIdx = 0u;
    appObj->completedLoopCnt = 0u;

    /* Init process list */
    pList = &appObj->processList;
    pList->inFrameList[0u] = &appObj->frameList[0u];
    pList->outFrameList[0u] = &appObj->frameList[1u];
    pList->outFrameList[1u] = &appObj->frameList[2u];
    /* Since DEIHQ will requires N-1 buffers in Mode-1, number of input
       frame lists is 2. */
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst))
    {
        pList->numInLists = 2u;
    }
    else
    {
        pList->numInLists = 1u;
    }
    if ((VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1 == appObj->driverInst))
    {
        pList->numOutLists = 2u;
    }
    else
    {
        pList->numOutLists = 1u;
    }
    pList->reserved = NULL;
    for (fListCnt = 0u; fListCnt < DEI_NUM_FRAMELIST; fListCnt++)
    {
        fList = &appObj->frameList[fListCnt];
        fList->numFrames = appObj->numCh;
        fList->perListCfg = NULL;
        fList->reserved = NULL;
        for (frmCnt = 0u; frmCnt < appObj->numCh; frmCnt++)
        {
            frm = &appObj->frames[fListCnt][frmCnt];
            fList->frames[frmCnt] = frm;
            frm->channelNum = frmCnt;
            frm->appData = NULL;
            frm->perFrameCfg = NULL;
            frm->subFrameInfo = NULL;
            frm->reserved = NULL;

            frm->fid = FVID2_FID_FRAME;
            frm->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                NULL;
            frm->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                NULL;
            frm->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                NULL;
            frm->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                NULL;
        }
    }

    /* Init error process list */
    memset(&appObj->errProcessList, 0u, sizeof (appObj->errProcessList));

    return;
}



/**
 *  vpsDeiAllocCtxBuffer
 */
static Int32 vpsDeiAllocCtxBuffer(App_DeiM2mObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    Vps_DeiCtxInfo      deiCtxInfo;
    Vps_DeiCtxBuf       deiCtxBuf;
    UInt32              chCnt, bCnt;

    for (chCnt = 0u; chCnt < appObj->numCh; chCnt++)
    {
        /* Get the number of buffers to allocate */
        deiCtxInfo.channelNum = chCnt;
        retVal = FVID2_control(
                    appObj->fvidHandle,
                    IOCTL_VPS_GET_DEI_CTX_INFO,
                    &deiCtxInfo,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }

        /* Allocate the buffers as requested by the driver */
        for (bCnt = 0u; bCnt < deiCtxInfo.numFld; bCnt++)
        {
            deiCtxBuf.fldBuf[bCnt] = VpsUtils_memAlloc(
                                         deiCtxInfo.fldBufSize,
                                         VPS_BUFFER_ALIGNMENT);
            if (NULL == deiCtxBuf.fldBuf[bCnt])
            {
                Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
                return (retVal);
            }
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMv; bCnt++)
        {
            deiCtxBuf.mvBuf[bCnt] = VpsUtils_memAlloc(
                                        deiCtxInfo.mvBufSize,
                                        VPS_BUFFER_ALIGNMENT);
            if (NULL == deiCtxBuf.mvBuf[bCnt])
            {
                Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
                return (retVal);
            }
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMvstm; bCnt++)
        {
            deiCtxBuf.mvstmBuf[bCnt] = VpsUtils_memAlloc(
                                           deiCtxInfo.mvstmBufSize,
                                           VPS_BUFFER_ALIGNMENT);
            if (NULL == deiCtxBuf.mvstmBuf[bCnt])
            {
                Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
                return (retVal);
            }
        }

        /* Provided the allocated buffer to driver */
        deiCtxBuf.channelNum = chCnt;
        retVal = FVID2_control(
                    appObj->fvidHandle,
                    IOCTL_VPS_SET_DEI_CTX_BUF,
                    &deiCtxBuf,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    return (retVal);
}



/**
 *  vpsDeiFreeCtxBuffer
 */
static Int32 vpsDeiFreeCtxBuffer(App_DeiM2mObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    Vps_DeiCtxInfo      deiCtxInfo;
    Vps_DeiCtxBuf       deiCtxBuf;
    UInt32              chCnt, bCnt;

    for (chCnt = 0u; chCnt < appObj->numCh; chCnt++)
    {
        /* Get the number of buffers to allocate */
        deiCtxInfo.channelNum = chCnt;
        retVal = FVID2_control(
                    appObj->fvidHandle,
                    IOCTL_VPS_GET_DEI_CTX_INFO,
                    &deiCtxInfo,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }

        /* Get the allocated buffer back from the driver */
        deiCtxBuf.channelNum = chCnt;
        retVal = FVID2_control(
                    appObj->fvidHandle,
                    IOCTL_VPS_GET_DEI_CTX_BUF,
                    &deiCtxBuf,
                    NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }

        /* Free the buffers */
        for (bCnt = 0u; bCnt < deiCtxInfo.numFld; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.fldBuf[bCnt], deiCtxInfo.fldBufSize);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMv; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.mvBuf[bCnt], deiCtxInfo.mvBufSize);
        }
        for (bCnt = 0u; bCnt < deiCtxInfo.numMvstm; bCnt++)
        {
            VpsUtils_memFree(deiCtxBuf.mvstmBuf[bCnt], deiCtxInfo.mvstmBufSize);
        }
    }

    return (retVal);
}



/**
 *  vpsDeiProgramScCoeff
 */
static Int32 vpsDeiProgramScCoeff(App_DeiM2mObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    Vps_ScCoeffParams   coeffPrms;
    Vps_ScLazyLoadingParams lazyLoadingParams;

    coeffPrms.hScalingSet = VPS_SC_DS_SET_8_16;
    coeffPrms.vScalingSet = VPS_SC_DS_SET_8_16;
    coeffPrms.coeffPtr = NULL;
    coeffPrms.scalarId = VPS_M2M_DEI_SCALAR_ID_DEI_SC;

    /* Program the scalar coefficients */
    Vps_printf("%s: Programming DEI Scalar coefficients ...\n", APP_NAME);
    retVal = FVID2_control(
                 appObj->fvidHandle,
                 IOCTL_VPS_SET_COEFFS,
                 &coeffPrms,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Programming of coefficients failed\n", APP_NAME);
        return (retVal);
    }

    /* Enable Lazy Loading */
    lazyLoadingParams.scalarId = VPS_M2M_DEI_SCALAR_ID_DEI_SC;
    lazyLoadingParams.enableLazyLoading = TRUE;
    lazyLoadingParams.enableFilterSelect = TRUE;
    retVal = FVID2_control(appObj->fvidHandle,
                           IOCTL_VPS_SC_SET_LAZY_LOADING,
                           &lazyLoadingParams,
                           NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s:Enabling Lazy Loading failed for scalar ID %d\n",
            APP_NAME,
            VPS_M2M_DEI_SCALAR_ID_DEI_SC);
    }

    /* Program the VIP scalar coefficient if needed */
    if ((VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_AUX_DEI_SC4_VIP1 == appObj->driverInst) ||
        (VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == appObj->driverInst) ||
        (VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1 == appObj->driverInst))
    {
        if (FALSE == appObj->chParams[0u].vipScCfg->bypass)
        {
            /* Program the VIP scalar coefficients */
            Vps_printf(
                "%s: Programming VIP Scalar coefficients ...\n", APP_NAME);
            coeffPrms.scalarId = VPS_M2M_DEI_SCALAR_ID_VIP_SC;
            retVal = FVID2_control(
                         appObj->fvidHandle,
                         IOCTL_VPS_SET_COEFFS,
                         &coeffPrms,
                         NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("%s: Programming of coefficients failed\n",
                    APP_NAME);
                return (retVal);
            }

            lazyLoadingParams.scalarId = VPS_M2M_DEI_SCALAR_ID_VIP_SC;
            retVal = FVID2_control(appObj->fvidHandle,
                                   IOCTL_VPS_SC_SET_LAZY_LOADING,
                                   &lazyLoadingParams,
                                   NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("%s:Enabling Lazy Loading failed for scalar ID %d\n",
                    APP_NAME,
                    VPS_M2M_DEI_SCALAR_ID_VIP_SC);
            }
        }
    }

    return (retVal);
}
