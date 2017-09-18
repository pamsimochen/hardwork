/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file M2mDeiScale_test.c
 *
 *  \brief VPS DEI memory to memory driver example file.
 *
 *  Default Settings:
 *  -----------------
 *
 *  This illustrates the DEIH/DEI dual scale memory to memory operation.
 *  It takes 10 frames of 720x240 YUV420 (NV12) interlaced input from the
 *  user. It performs deinterlacing and dual scale it to 360x240 YUYV422
 *  progressive output via WB0/1 path and to 720x480 YUV420 (NV12) progressive
 *  output via VIP0/1. The VIP scalars are not used by default.
 *
 *  The user can select the instance to run while running the application via
 *  console input. Depending on the instance selected, the application
 *  does single/dual scaling via WB0/1 and/or VIP0/1 outputs.
 *
 *  Other Configuration Options:
 *  ----------------------------
 *
 *  Input Format:
 *      Set DEI_IN_FMT to FVID2_DF_YUV422I_YUYV or FVID2_DF_YUV420SP_UV or
 *  FVID2_DF_YUV422SP_UV.
 *
 *  VIP Output Format:
 *      Set DEI_OUT1_FMT to FVID2_DF_YUV422I_YUYV or FVID2_DF_YUV420SP_UV.
 *
 *  Input Scan Format:
 *      Set DEI_IN_SCAN_FORMAT to FVID2_SF_INTERLACED/FVID2_SF_PROGRESSIVE.
 *  This will determine whether deinterlacer will be used or not.
 *
 *  Input/Output Buffer Dimension:
 *      Set the corresponding macros below DEI_XXX_WIDTH and DEI_XXX_HEIGHT
 *  to the desired value.
 *
 *  Number of Channels:
 *      Set DEI_NUM_CHANNELS to desired value. In the current application,
 *  all the channels uses the same input buffer but allocation of output
 *  buffers is separate for each of the channels.
 *
 *  DEI Input and VIP Output Memory Type:
 *      Use the executable with _Tiler suffix to operate in tiled mode.
 *  When tiler is used, this application uses tiled buffer to perform the
 *  deinterlacing operation. But the input and output buffers used to load and
 *  store video is still in non-tiled memory space so that user could use
 *  non-tiled video content for load/store. This application does the
 *  non-tiled to tiled conversion by copying the input buffer to tiled space
 *  before submitting request to driver and by copying the tiled output buffer
 *  to non-tiled space after the request is complete.
 *  Note: When input is in tiled mode, DEI_IN_FMT could not be set to
 *  FVID2_DF_YUV422I_YUYV.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mDei.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/utility/vpsutils_app.h>

#include "M2mDeiScale_utils.h"


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Driver mode of operation. */
#define DEI_DRIVER_MODE                 (VPS_M2M_CONFIG_PER_CHANNEL)

/* Number of channels to submit per call. */
#define DEI_NUM_CHANNELS                (1u)

/* DEI input/output memory type - Tiled or Non-Tiled. */
#define DEI_IN_MEM_TYPE                 (VPS_VPDMA_MT_NONTILEDMEM)
#define DEI_VIP_OUT_MEM_TYPE            (VPS_VPDMA_MT_NONTILEDMEM)

/* Tiler orientation bits. Set this to VPSUTILS_TILER_ORI_NONE,
 * VPSUTILS_TILER_ORI_X_FLIP, VPSUTILS_TILER_ORI_Y_FLIP or
 * (VPSUTILS_TILER_ORI_X_FLIP | VPSUTILS_TILER_ORI_Y_FLIP).
 * Note that XY swap is not supported in this appplication. */
#define DEI_IN_ORI_MODE                 (VPSUTILS_TILER_ORI_NONE)
#define DEI_OUT_ORI_MODE                (VPSUTILS_TILER_ORI_NONE)

/* Input frame video parameters. */
#define DEI_IN_WIDTH                    (720u)

/* DEI writeback output frame video parameters. */
#define DEI_OUT0_WIDTH                  (360u)
#define DEI_OUT0_HEIGHT                 (240u)

/* DEI-VIP output frame video parameters. */
#define DEI_OUT1_WIDTH                  (704u)
#define DEI_OUT1_HEIGHT                 (480u)
#define DEI_OUT1_FMT                    (FVID2_DF_YUV420SP_UV)

/* Number of loop count. */
#define DEI_TOTAL_LOOP_COUNT            (1u * DEI_NUM_IN_BUFFERS)

/* Uncomment this to get DEI peformance numbers */
#define                                 DEI_PRF_TEST

#ifdef DEI_PRF_TEST
#undef DEI_TOTAL_LOOP_COUNT
#define DEI_TOTAL_LOOP_COUNT            (10000u)
#endif

#ifndef SC_APP_TEST_SUBFRAME

/* Number of input buffers to allocate. This should be less than
 * DEI_MAX_IN_BUFFERS macro. */
#define DEI_NUM_IN_BUFFERS              (10u)
/*
 * Height of DEI input frame. When DEI is deinterlacing, it should be
 * height of a field, otherwise it should be height of the entire frame.
 */
#define DEI_IN_HEIGHT                   (240u)
#define DEI_IN_FMT                      (FVID2_DF_YUV420SP_UV)
/*
 * Input/output buffer scan format. This will determine whether deinterlacer
 * will be used or not.
 */
#define DEI_IN_SCAN_FORMAT              (FVID2_SF_INTERLACED)
#define DEI_OUT_SCAN_FORMAT             (FVID2_SF_PROGRESSIVE)

#else /* #ifndef SC_APP_TEST_SUBFRAME */

/* Number of subframes per frame for subframe based processing */
#define SUBFRAMES_PER_FRAME             (4)
/* Number of input buffers to allocate. This should be less than
 * DEI_MAX_IN_BUFFERS macro. */
#define DEI_NUM_IN_BUFFERS              (1u)
/*
 * Height of DEI input frame. When DEI is deinterlacing, it should be
 * height of a field, otherwise it should be height of the entire frame.
 */
#define DEI_IN_HEIGHT                   (480u)
#define DEI_IN_FMT                      (FVID2_DF_YUV420SP_UV)
/*
 * Input/output buffer scan format. This will determine whether deinterlacer
 * will be used or not.
 */
#define DEI_IN_SCAN_FORMAT              (FVID2_SF_PROGRESSIVE)
#define DEI_OUT_SCAN_FORMAT             (FVID2_SF_PROGRESSIVE)

#endif /* #ifndef SC_APP_TEST_SUBFRAME */

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (10u * 1024u)

/* Check whether user provided buffer is more than what we could support */
#if (DEI_NUM_IN_BUFFERS > DEI_MAX_IN_BUFFERS)
#error "Increase DEI_MAX_IN_BUFFERS as per the changed DEI_NUM_IN_BUFFERS"
#endif


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void App_m2mDeiTestTask(void);
static Int32 App_m2mDeiTest(App_M2mDeiObj *appObj);
static Int32 App_m2mDeiAppCbFxn(FVID2_Handle handle,
                                Ptr appData,
                                Ptr reserved);
static Int32 App_m2mDeiAppErrCbFxn(FVID2_Handle handle,
                                   Ptr appData,
                                   Void *errList,
                                   Ptr reserved);
static Void App_m2mDeiUpdateProcList(App_M2mDeiObj *appObj,
                                     FVID2_ProcessList *pList);
static Void App_m2mDeiInitVariables(App_M2mDeiObj *appObj);
static Void App_m2mDeiInitFrame(FVID2_Frame *frm, UInt32 chanNo);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppM2mDeiTestTaskStack, 32)
#pragma DATA_SECTION(AppM2mDeiTestTaskStack, ".bss:taskStackSection")
static UInt8 AppM2mDeiTestTaskStack[APP_TSK_STACK_SIZE];

/* M2M DEI application object per handle. */
static App_M2mDeiObj M2mDeiObjs;

extern Semaphore_Handle AppTestCompleteSem;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  App_m2mDeiCreateTsk
 *  M2M DEI Application create task function.
 */
Task_Handle App_m2mDeiCreateTsk(void)
{
    Task_Params tskParams;
    Task_Handle tskHandle;

    Task_Params_init(&tskParams);
    tskParams.stack = AppM2mDeiTestTaskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    tskHandle =
        Task_create((Task_FuncPtr) App_m2mDeiTestTask, &tskParams, NULL);

    return (tskHandle);
}



/**
 *  App_m2mDeiTestTask
 *  M2M DEI Application test task.
 */
static Void App_m2mDeiTestTask(void)
{
    char                ch, inputStr[10];
    Int32               retVal;
    Int32               drvInst;
    App_M2mDeiObj      *appObj;

    /* Init application object with selected option */
    appObj = &M2mDeiObjs;
    appObj->mode = DEI_DRIVER_MODE;
    appObj->numCh = DEI_NUM_CHANNELS;
    appObj->inMemType = DEI_IN_MEM_TYPE;
    appObj->outMemTypeVip = DEI_VIP_OUT_MEM_TYPE;
    appObj->numInBuffers = DEI_NUM_IN_BUFFERS;
    appObj->inWidth = DEI_IN_WIDTH;
    appObj->inHeight = DEI_IN_HEIGHT;
    appObj->inDataFmt = DEI_IN_FMT;
    appObj->outWidthDei = DEI_OUT0_WIDTH;
    appObj->outHeightDei = DEI_OUT0_HEIGHT;
    appObj->outWidthVip = DEI_OUT1_WIDTH;
    appObj->outHeightVip = DEI_OUT1_HEIGHT;
    appObj->outDataFmtVip = DEI_OUT1_FMT;
    appObj->drnEnable = FALSE;
    appObj->comprEnable = FALSE;

    /* Get the driver instance to run from user */
    Vps_printf(
        "--------------------------------------------------------\n");
    Vps_printf(
        "Select the Driver Instance to run as per below table:\n");
    Vps_printf(
        "--------------------------------------------------------\n");
#ifdef TI_816X_BUILD
    Vps_printf("%d - DEIH-WB0 Single Scale memory driver\n",
        VPS_M2M_INST_MAIN_DEIH_SC1_WB0);
    Vps_printf("%d - DEI-WB1 Single Scale memory driver\n",
        VPS_M2M_INST_AUX_DEI_SC2_WB1);
    Vps_printf("%d - DEIH-VIP0 Single Scale memory driver\n",
        VPS_M2M_INST_MAIN_DEIH_SC3_VIP0);
    Vps_printf("%d - DEI-VIP1 Single Scale memory driver\n",
        VPS_M2M_INST_AUX_DEI_SC4_VIP1);
    Vps_printf("%d - DEIH-WB0-VIP0 Dual Scale memory driver\n",
        VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0);
    Vps_printf("%d - DEI-WB1-VIP1 Dual Scale memory driver\n",
        VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1);
#else
    Vps_printf("%d - DEI-WB0 Single Scale memory driver\n",
        VPS_M2M_INST_MAIN_DEI_SC1_WB0);
    Vps_printf("%d - DEI-VIP0 Single Scale memory driver\n",
        VPS_M2M_INST_MAIN_DEI_SC3_VIP0);
    Vps_printf("%d - DEI-WB0-VIP0 Dual Scale memory driver\n",
        VPS_M2M_INST_MAIN_DEI_SC1_SC3_WB0_VIP0);
    Vps_printf("%d - SC-WB1 Single Scale memory driver - No DEI,
        only progressive scaling\n", VPS_M2M_INST_AUX_SC2_WB1);
    Vps_printf("%d - SC-VIP1 Single Scale memory driver - No DEI,
        only progressive scaling\n", VPS_M2M_INST_AUX_SC4_VIP1);
    Vps_printf("%d - SC-WB1-VIP1 Dual Scale memory driver - No DEI,
        only progressive scaling\n", VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1);
#endif
    Vps_printf(
        "--------------------------------------------------------\n");
    Vps_printf("%s: Enter value from %d to %d and press enter...\n",
        APP_NAME, 0, (VPS_M2M_DEI_INST_MAX - 1));
    VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
    drvInst = atoi(inputStr);

    while ((drvInst < 0) || (drvInst >= VPS_M2M_DEI_INST_MAX))
    {
        Vps_printf(
            "%s: Invalid Driver Instance %d selected\n", APP_NAME, drvInst);
        Vps_printf(
            "%s: Enter value from %d to %d and press enter...\n",
            APP_NAME, 0, (VPS_M2M_DEI_INST_MAX - 1));
        VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
        drvInst = atoi(inputStr);
    }
    Vps_printf("%s: Driver Instance Selected: %d\n", APP_NAME, drvInst);
    appObj->drvInst = drvInst;

    appObj->isDeiPresent = VpsUtils_appM2mDeiIsDeiPresent(drvInst);
    if (TRUE == appObj->isDeiPresent)
    {
        appObj->inScanFmt  = DEI_IN_SCAN_FORMAT;
        appObj->outScanFmt = DEI_OUT_SCAN_FORMAT;
    }
    else
    {
        appObj->inScanFmt  = FVID2_SF_PROGRESSIVE;
        appObj->outScanFmt = FVID2_SF_PROGRESSIVE;
    }

    if (1u == appObj->numCh)
    {
        appObj->numOutBuffers = appObj->numInBuffers;
    }
    else
    {
        appObj->numOutBuffers = appObj->numInBuffers * appObj->numCh;
    }

    /* Allocate input and output buffers */
    retVal = App_m2mDeiAllocBuf(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    Vps_printf(
        "%s: Load %d %s video frames/fields of size %dx%d to location: 0x%p\n",
        APP_NAME, appObj->numInBuffers,
        FVID2_getDataFmtString(appObj->inDataFmt), appObj->inWidth,
        appObj->inHeight, appObj->inLoadBuf);
    Vps_printf("%s: Enter any key after loading...\n", APP_NAME);
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    /* Copy input video to tiled input buffer */
    App_m2mDeiCopyToTilerMem(appObj);

    /* Run the test */
    retVal = App_m2mDeiTest(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
    }

    /* Copy processed tiled output video to output save buffer */
    App_m2mDeiCopyFromTilerMem(appObj);

    /* Wait for user to save the output buffers */
    if (FVID2_SOK == retVal)
    {
        Vps_printf("%s: Save output buffers to file...\n", APP_NAME);
    }
    Vps_printf("%s: Enter any key ...\n", APP_NAME);
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    /* Free the allocated buffer memory */
    retVal = App_m2mDeiFreeBuf(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
    }

    Vps_printf("%s: Test Successfull!!\n", APP_NAME);

    /* Test complete. Signal it */
    Semaphore_post(AppTestCompleteSem);
}



/**
 *  App_m2mDeiTest
 *  M2M DEI test function.
 */
static Int32 App_m2mDeiTest(App_M2mDeiObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    Int32                   cleanupRetVal = FVID2_SOK;
    Semaphore_Params        semParams;
    FVID2_CbParams          cbParams;
    FVID2_ProcessList      *pList;
    Vps_M2mDeiCreateStatus  createStatus;
    VpsUtils_PrfTsHndl      *prfTsHandle;
#ifdef  SC_APP_TEST_SUBFRAME
    Int32                   subFrameNum;
#else
    Vps_M2mDeiRtParams     *rtParamsptr[VPS_M2M_DEI_MAX_CH_PER_INST];
    Vps_M2mDeiRtParams      rtParams[VPS_M2M_DEI_MAX_CH_PER_INST];
    Vps_FrameParams         deiOutFrmParams[VPS_M2M_DEI_MAX_CH_PER_INST];
    Vps_FrameParams         vipOutFrmParams[VPS_M2M_DEI_MAX_CH_PER_INST];
    UInt32                  frmCnt;
    UInt32                  deiHorzVal, deiVertVal, vipHorzVal, vipVertVal;
#endif

    Semaphore_Params_init(&semParams);
    appObj->reqCompleteSem = Semaphore_create(0, &semParams, NULL);

    /* Start the TimeStamp Calculation */
    prfTsHandle = VpsUtils_prfTsCreate(APP_NAME":");

    /* Initialize the frame pointers and other variables */
    App_m2mDeiInitVariables(appObj);

    /* Open the driver */
    cbParams.cbFxn = App_m2mDeiAppCbFxn;
    cbParams.errCbFxn = App_m2mDeiAppErrCbFxn;
    cbParams.errList = &appObj->errProcessList;
    cbParams.appData = appObj;
    cbParams.reserved = NULL;
    appObj->fvidHandle = FVID2_create(
                             FVID2_VPS_M2M_DEI_DRV,
                             appObj->drvInst,
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

    /* Print the driver status information */
    Vps_printf("%s: M2M DEI Driver Opened for Instance %d!!\n",
        APP_NAME, appObj->drvInst);
    VpsUtils_appPrintM2mDeiCreateStatus(&createStatus);

    /* Program the scalar coefficients */
    retVal = App_m2mDeiProgramScCoeff(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Programming of coefficients failed\n", APP_NAME);
        return (retVal);
    }

    /* Allocate Context buffers only if DEI is present */
    if (TRUE == appObj->isDeiPresent)
    {
        /* Allocate DEI context buffers */
        retVal = VpsUtils_appAllocDeiCtxBuffer(
                     appObj->fvidHandle,
                     appObj->numCh);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Allocation of context buffer failed\n", APP_NAME);
            return (retVal);
        }
    }

    appObj->fid = 0u;
    /* Use the same process list for all request */
    pList = &appObj->processList;

    /* Start the load calculation */
    VpsUtils_prfLoadCalcStart();
    VpsUtils_prfTsBegin(prfTsHandle);

    while (appObj->completedLoopCnt < DEI_TOTAL_LOOP_COUNT)
    {
        /* Update the buffer address */
        App_m2mDeiUpdateProcList(appObj, pList);

#ifndef SC_APP_TEST_SUBFRAME
        for (frmCnt = 0; frmCnt < appObj->numCh; frmCnt++)
        {
            if (0 == appObj->completedLoopCnt)
            {
                rtParamsptr[frmCnt]                = &rtParams[frmCnt];
                rtParamsptr[frmCnt]->deiOutFrmPrms = &deiOutFrmParams[frmCnt];
                rtParamsptr[frmCnt]->vipOutFrmPrms = &vipOutFrmParams[frmCnt];
                rtParamsptr[frmCnt]->deiInFrmPrms  = NULL;
                rtParamsptr[frmCnt]->deiScCropCfg  = NULL;
                rtParamsptr[frmCnt]->vipScCropCfg  = NULL;
                rtParamsptr[frmCnt]->deiScRtCfg    = NULL;
                rtParamsptr[frmCnt]->vipScRtCfg    = NULL;
                rtParamsptr[frmCnt]->deiRtCfg      = NULL;
                deiHorzVal = (appObj->outWidthDei / appObj->numCh);
                deiVertVal = (appObj->outHeightDei / appObj->numCh);
                rtParamsptr[frmCnt]->deiOutFrmPrms->width = VpsUtils_align((
                    appObj->outWidthDei - (frmCnt * deiHorzVal)),
                    VPS_BUFFER_ALIGNMENT);
                /* Minimum horizontal scaling ratio is 1/16 */
                if (    rtParamsptr[frmCnt]->deiOutFrmPrms->width
                    <   (appObj->outWidthDei / 16u))
                {
                    rtParamsptr[frmCnt]->deiOutFrmPrms->width =
                        appObj->outWidthDei / 16u;
                }
                rtParamsptr[frmCnt]->deiOutFrmPrms->height = VpsUtils_align((
                    appObj->outHeightDei - (frmCnt * deiVertVal)),
                    VPS_BUFFER_ALIGNMENT);
                rtParamsptr[frmCnt]->deiOutFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] =
                    appObj->outPitchDei;
                rtParamsptr[frmCnt]->deiOutFrmPrms->memType =
                    VPS_VPDMA_MT_NONTILEDMEM;
                rtParamsptr[frmCnt]->deiOutFrmPrms->dataFormat =
                    FVID2_DF_YUV422I_YUYV;

                vipHorzVal = (appObj->outWidthVip / appObj->numCh);
                vipVertVal = (appObj->outHeightVip / appObj->numCh);
                rtParamsptr[frmCnt]->vipOutFrmPrms->width =
                    appObj->outWidthVip - (frmCnt * vipHorzVal);
                /* Minimum horizontal scaling ratio is 1/16 */
                if (    rtParamsptr[frmCnt]->vipOutFrmPrms->width
                    <   (appObj->outWidthVip / 16u))
                {
                    rtParamsptr[frmCnt]->vipOutFrmPrms->width =
                        appObj->outWidthVip / 16u;
                }
                rtParamsptr[frmCnt]->vipOutFrmPrms->height =
                    appObj->outHeightVip - (frmCnt * vipVertVal);
                if (   (FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip)
                    || (FVID2_DF_YUV422SP_UV == appObj->outDataFmtVip))
                {
                    rtParamsptr[frmCnt]->vipOutFrmPrms->pitch
                        [FVID2_YUV_SP_Y_ADDR_IDX] = appObj->outPitchYVip;
                    rtParamsptr[frmCnt]->vipOutFrmPrms->pitch
                        [FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->outPitchCbCrVip;
                }
                else
                {
                    rtParamsptr[frmCnt]->vipOutFrmPrms->pitch
                        [FVID2_YUV_INT_ADDR_IDX] = appObj->outPitchYVip;
                }
                rtParamsptr[frmCnt]->vipOutFrmPrms->memType =
                    appObj->outMemTypeVip;
                rtParamsptr[frmCnt]->vipOutFrmPrms->dataFormat =
                    appObj->outDataFmtVip;

                pList->inFrameList[0]->frames[frmCnt]->perFrameCfg =
                    rtParamsptr[frmCnt];
            }
            else
            {
                pList->inFrameList[0]->frames[frmCnt]-> perFrameCfg = NULL;
            }
        }

        /* Process frames */
        retVal = FVID2_processFrames(appObj->fvidHandle, pList);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Processing of the request failed\n", APP_NAME);
            break;
        }

        /* Wait till frames get processed */
        Semaphore_pend(appObj->reqCompleteSem, BIOS_WAIT_FOREVER);

        /* Get the processed frames */
        retVal = FVID2_getProcessedFrames(
                     appObj->fvidHandle,
                     pList,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Get processed request failed\n", APP_NAME);
            break;
        }

#else   /* subframe based processing */

        /* subframe by subframe processing. */
        for (subFrameNum = 0; subFrameNum < SUBFRAMES_PER_FRAME; subFrameNum++)
        {
            FVID2_Frame *inFrm;

            inFrm  = pList->inFrameList[0]->frames[0];

            /* update subframe info in process list */
            inFrm->subFrameInfo->subFrameNum = subFrameNum;
            inFrm->subFrameInfo->numInLines =
                (subFrameNum + 1)*(appObj->inHeight / SUBFRAMES_PER_FRAME);

            /* Process frames */
            retVal = FVID2_processFrames(appObj->fvidHandle, pList);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("%s: Processing of the request failed\n",
                    APP_NAME);
                break;
            }

            /* Wait till frames get processed */
            Semaphore_pend(appObj->reqCompleteSem, BIOS_WAIT_FOREVER);

            /* Get the processed frames */
            retVal = FVID2_getProcessedFrames(
                         appObj->fvidHandle,
                         pList,
                         FVID2_TIMEOUT_NONE);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("%s: Get processed request failed\n", APP_NAME);
                break;
            }
         }
#endif

        appObj->completedLoopCnt++;
    }

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    /* Release the context fields */
    retVal = FVID2_stop(appObj->fvidHandle, NULL);
    if (retVal != FVID2_SOK)
    {
        Vps_printf("%s: Releasing held context fields failed\n", APP_NAME);
    }
#endif  /* #if defined(TI_814X_BUILD) || defined(TI_8107_BUILD) */

    /* Dequeue all the request from driver */
    while (1u)
    {
        /* Get the processed frames */
        cleanupRetVal = FVID2_getProcessedFrames(
                     appObj->fvidHandle,
                     pList,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK == cleanupRetVal)
        {
            appObj->completedLoopCnt++;
            Vps_printf("%s: Loop count: %d\n",
                APP_NAME, appObj->completedLoopCnt);
        }
        else if (FVID2_ENO_MORE_BUFFERS == cleanupRetVal)
        {
            Vps_printf("%s: All request dequeued from driver\n", APP_NAME);
            break;
        }
        else if (FVID2_EAGAIN == cleanupRetVal)
        {
            /* Still request pending with driver. Try after sometime. */
            Task_yield();
        }
        else
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, cleanupRetVal, __LINE__);
            break;
        }
    }

    /* End of timestamp addition */
    VpsUtils_prfTsEnd(prfTsHandle, DEI_TOTAL_LOOP_COUNT * appObj->numCh);
     /* Stop the load calculation */
    VpsUtils_prfLoadCalcStop();

/* Print load only if its enabled else it might be wrong for very less frames */
#ifdef DEI_PRF_TEST
    /* Print the load */
    VpsUtils_prfLoadPrintAll(TRUE);
    /* print the fps based on TimeStamp */
    VpsUtils_prfTsPrint(prfTsHandle, TRUE);
#endif

    /* Reset the load */
    VpsUtils_prfLoadCalcReset();
    /* Delete the timestamp instance */
    VpsUtils_prfTsDelete(prfTsHandle);

    if (TRUE == appObj->isDeiPresent)
    {
        /* Free DEI context buffers */
        cleanupRetVal = VpsUtils_appFreeDeiCtxBuffer(
                            appObj->fvidHandle,
                            appObj->numCh);
        if (FVID2_SOK != cleanupRetVal)
        {
            Vps_printf("%s: Freeing of context buffer failed\n", APP_NAME);
            if (FVID2_SOK == retVal)
            {
                /* If processing was successful, return status of cleanup. */
                retVal = cleanupRetVal;
            }
            return (retVal);
        }
    }

    /* Close driver */
    cleanupRetVal = FVID2_delete(appObj->fvidHandle, NULL);
    if (FVID2_SOK != cleanupRetVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        if (FVID2_SOK == retVal)
        {
            /* If processing was successful, return status of cleanup. */
            retVal = cleanupRetVal;
        }
        return (retVal);
    }

    Semaphore_delete(&appObj->reqCompleteSem);
    if (FVID2_SOK == retVal)
    {
        Vps_printf("%s: DEI Memory Driver Test Successfull!!\n", APP_NAME);
    }
    else
    {
        Vps_printf("%s: DEI Memory Driver Test Failed!!\n", APP_NAME);
    }

    return (retVal);
}



/**
 *  App_m2mDeiAppCbFxn
 *  Driver callback function.
 */
static Int32 App_m2mDeiAppCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32           retVal = FVID2_SOK;
    App_M2mDeiObj *appObj = (App_M2mDeiObj *) appData;

    /* Transfer complete. Signal it */
    Semaphore_post(appObj->reqCompleteSem);

    return (retVal);
}



/**
 *  App_m2mDeiAppErrCbFxn
 *  Driver error callback function.
 */
static Int32 App_m2mDeiAppErrCbFxn(FVID2_Handle handle,
                                  Ptr appData,
                                  Void *errList,
                                  Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}



/**
 *  App_m2mDeiUpdateProcList
 *  Updates the process list with the next request buffer information.
 */
static Void App_m2mDeiUpdateProcList(App_M2mDeiObj *appObj,
                                     FVID2_ProcessList *pList)
{
    UInt32                  frmCnt, tempVar;
    UInt32                  cbCrHeight;
    UInt32                  inAddrY, inAddrCbCr, outAddrY, outAddrCbCr;
    FVID2_Frame            *frm;
    FVID2_FrameList        *inFrmList;
    FVID2_FrameList        *deiOutFrmList = NULL, *vipOutFrmList = NULL;

    inFrmList = pList->inFrameList[0u];
    /* Assign frame list pointer according to the instance */
    if (VpsUtils_appM2mDeiIsWbInst(appObj->drvInst))
    {
        /* DEI WB output is always at offset 0, if required */
        deiOutFrmList = pList->outFrameList[0u];
    }
    else if (VpsUtils_appM2mDeiIsVipInst(appObj->drvInst))
    {
        /* For single output VIP instances, frame list offset is 0 */
        vipOutFrmList = pList->outFrameList[0u];
    }
    if (VpsUtils_appM2mDeiIsDualOutInst(appObj->drvInst))
    {
        /* For dual output VIP instances, frame list offset is 1 */
        vipOutFrmList = pList->outFrameList[1u];
    }

    /* Use the same input buffer for all channels */
    if (appObj->numInBuffers == appObj->inBufIdx)
    {
        appObj->inBufIdx = 0u;
    }

    for (frmCnt = 0u; frmCnt < appObj->numCh; frmCnt++)
    {
        /* Use different output buffer for each channel */
        if (appObj->numOutBuffers == appObj->outBufIdx)
        {
            appObj->outBufIdx = 0u;
        }

        if (VPS_VPDMA_MT_TILEDMEM == appObj->inMemType)
        {
            inAddrY = VpsUtils_tilerGetOriAddr(
                          (UInt32) appObj->inBufY[appObj->inBufIdx],
                          VPSUTILS_TILER_CNT_8BIT,
                          DEI_IN_ORI_MODE,
                          appObj->inWidth,
                          appObj->inHeight);
            cbCrHeight = appObj->inHeight;
            if (FVID2_DF_YUV420SP_UV == appObj->inDataFmt)
            {
                cbCrHeight /= 2u;
            }
            inAddrCbCr = VpsUtils_tilerGetOriAddr(
                             (UInt32) appObj->inBufCbCr[appObj->inBufIdx],
                             VPSUTILS_TILER_CNT_16BIT,
                             DEI_IN_ORI_MODE,
                             appObj->inWidth,
                             cbCrHeight);
        }
        else
        {
            inAddrY = (UInt32) appObj->inBufY[appObj->inBufIdx];
            inAddrCbCr = (UInt32) appObj->inBufCbCr[appObj->inBufIdx];
        }

        /* Update the DEI In list */
        frm = inFrmList->frames[frmCnt];
        /* Check if given frame was held back by driver, if so get the space
           for frame description from context frames space */
        if (frm == NULL)
        {
            if (appObj->deiBypassFlag == TRUE)
            {
                Vps_printf("%s: Frames not returned in bypass@ line %d\n",
                    __FUNCTION__, __LINE__);
            }

            tempVar = appObj->freeCntxFrameIndex[frmCnt];
            if (tempVar < DEI_NO_OF_BUF_REQ_BEFORE_RES)
            {
                frm = &(appObj->cntxFrames[frmCnt][tempVar]);
                appObj->freeCntxFrameIndex[frmCnt]++;
            }
            else
            {
                Vps_printf("%s: Error Out of context buf@ line %d\n",
                    __FUNCTION__, __LINE__);
                break;
            }
            inFrmList->frames[frmCnt] = frm;
        }
        if (appObj->deiBypassFlag == TRUE)
        {
            frm->fid = FVID2_FID_FRAME;
            frm->subFrameInfo = &appObj->subFrameInfoIn[frmCnt];
            if ((FVID2_DF_YUV420SP_UV == appObj->inDataFmt) ||
                (FVID2_DF_YUV422SP_UV == appObj->inDataFmt))
            {
                frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                    (UInt8 *) inAddrY;
                frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                    (UInt8 *) inAddrCbCr;
            }
            else
            {
                frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                    (UInt8 *) inAddrY;
            }
        }
        else
        {
            if (0u == appObj->fid)
            {
                frm->fid = FVID2_FID_TOP;
            }
            else
            {
                frm->fid = FVID2_FID_BOTTOM;
            }
            if ((FVID2_DF_YUV420SP_UV == appObj->inDataFmt) ||
                (FVID2_DF_YUV422SP_UV == appObj->inDataFmt))
            {
                frm->addr[FVID2_FIELD_MODE_ADDR_IDX]
                         [FVID2_YUV_SP_Y_ADDR_IDX] = (UInt8 *) inAddrY;
                frm->addr[FVID2_FIELD_MODE_ADDR_IDX]
                         [FVID2_YUV_SP_CBCR_ADDR_IDX] = (UInt8 *) inAddrCbCr;
            }
            else
            {
                frm->addr[FVID2_FIELD_MODE_ADDR_IDX]
                         [FVID2_YUV_INT_ADDR_IDX] = (UInt8 *) inAddrY;
            }
        }

        /* Update the DEI writeback Out list */
        if (NULL != deiOutFrmList)
        {
            frm = deiOutFrmList->frames[frmCnt];
            frm->fid = FVID2_FID_FRAME;
            frm->subFrameInfo = &appObj->subFrameInfoOutDei[frmCnt];
            frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                appObj->outBufDei[appObj->outBufIdx];
        }

        /* Update the DEI-VIP Out list */
        if (NULL != vipOutFrmList)
        {
            if (VPS_VPDMA_MT_TILEDMEM == appObj->outMemTypeVip)
            {
                outAddrY = VpsUtils_tilerGetOriAddr(
                               (UInt32) appObj->outBufYVip[appObj->outBufIdx],
                               VPSUTILS_TILER_CNT_8BIT,
                               DEI_OUT_ORI_MODE,
                               appObj->outWidthVip,
                               appObj->outHeightVip);
                cbCrHeight = appObj->outHeightVip;
                if (FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip)
                {
                    cbCrHeight /= 2u;
                }
                outAddrCbCr = VpsUtils_tilerGetOriAddr(
                                  (UInt32)
                                    appObj->outBufCbCrVip[appObj->outBufIdx],
                                  VPSUTILS_TILER_CNT_16BIT,
                                  DEI_OUT_ORI_MODE,
                                  appObj->outWidthVip,
                                  appObj->outHeightVip);
            }
            else
            {
                outAddrY = (UInt32) appObj->outBufYVip[appObj->outBufIdx];
                outAddrCbCr = (UInt32) appObj->outBufCbCrVip[appObj->outBufIdx];
            }

            frm = vipOutFrmList->frames[frmCnt];
            frm->fid = FVID2_FID_FRAME;
            if ((FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip) ||
                (FVID2_DF_YUV422SP_UV == appObj->outDataFmtVip))
            {
                frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                    (UInt8 *)outAddrY;
                frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                    (UInt8 *)outAddrCbCr;
            }
            else
            {
                frm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                    (UInt8 *)outAddrY;
            }
        }

        appObj->outBufIdx++;
    }

    /* Updated the number of frames present - would have been cleared */
    inFrmList->numFrames = appObj->numCh;
    pList->numInLists = 1u;
    appObj->fid = appObj->fid ^ 1u;
    appObj->inBufIdx++;

    return;
}



/**
 *  vpsInitDefaultDeiHqPrms
 *  Sets the default DEI HQ parameters.
 */
static Void vpsInitDefaultDeiHqPrms(App_M2mDeiObj *appObj,
                                    Vps_DeiHqConfig *hqCfg)
{
    hqCfg->bypass = appObj->deiBypassFlag;
    hqCfg->inpMode = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
    hqCfg->tempInpEnable = TRUE;
    hqCfg->tempInpChromaEnable = TRUE;
    hqCfg->spatMaxBypass = FALSE;
    hqCfg->tempMaxBypass = FALSE;
    hqCfg->fldMode = VPS_DEIHQ_FLDMODE_5FLD;
    hqCfg->lcModeEnable = TRUE;
    hqCfg->mvstmEnable = TRUE;
    hqCfg->tnrEnable = FALSE;
    hqCfg->snrEnable = TRUE;
    hqCfg->sktEnable = FALSE;
    hqCfg->chromaEdiEnable = TRUE;
}



/**
 *  vpsInitDefaultDeiPrms
 *  Sets the default DEI parameters.
 */
static Void vpsInitDefaultDeiPrms(App_M2mDeiObj *appObj,
                                  Vps_DeiConfig *deiCfg)
{
    deiCfg->bypass = appObj->deiBypassFlag;
    deiCfg->inpMode = VPS_DEI_EDIMODE_LUMA_CHROMA;
    deiCfg->tempInpEnable = TRUE;
    deiCfg->tempInpChromaEnable = TRUE;
    deiCfg->spatMaxBypass = FALSE;
    deiCfg->tempMaxBypass = FALSE;
}

/**
 *  vpsInitDefaultSubFramePrms
 *  Sets the default parameters for sub-frame level processing .
 */
#ifdef SC_APP_TEST_SUBFRAME
static Void vpsInitDefaultSubFramePrms(App_M2mDeiObj *appObj,
                                    Vps_SubFrameParams *slcPrms)
{
    if (appObj->deiBypassFlag == TRUE)
    {
        slcPrms->subFrameModeEnable   = TRUE;
        slcPrms->numLinesPerSubFrame  = (appObj->inHeight/SUBFRAMES_PER_FRAME);
    } else
    {  /* SubFrame mode will not work if DEI is active */
        slcPrms->subFrameModeEnable   = FALSE;
        slcPrms->numLinesPerSubFrame  = (appObj->inHeight/SUBFRAMES_PER_FRAME);
        Vps_printf(
           "SubFrame Mode disabled as DEI is enabled: disable DEI to use subframe mode\n");
    }
}
#endif

/**
 *  vpsInitDefaultScCropPrms
 *  Sets the default scalar and crop parameters.
 */
static Void vpsInitDefaultScCropPrms(App_M2mDeiObj *appObj,
                                     Vps_ScConfig *scCfg,
                                     Vps_CropConfig *cropCfg)
{
    scCfg->bypass = FALSE;
    scCfg->nonLinear = FALSE;
    scCfg->stripSize = 0u;
    scCfg->vsType = VPS_SC_VST_POLYPHASE;

    cropCfg->cropStartX = 0u;
    cropCfg->cropStartY = 0u;
    cropCfg->cropWidth = appObj->inWidth;
    if (TRUE == appObj->deiBypassFlag)
    {
        cropCfg->cropHeight = appObj->inHeight;
    }
    else
    {
       cropCfg->cropHeight = appObj->inHeight * 2u;
    }
    scCfg->enablePeaking = TRUE;
    /* Edge detect is enabled for the SC_H and is dont care for all other
     * scalars
     */
    scCfg->enableEdgeDetect = TRUE;
}



/**
 *  vpsSetDefaultChPrms
 *  Sets the default channel parameters.
 */
static Void vpsSetDefaultChPrms(App_M2mDeiObj *appObj,
                                Vps_M2mDeiChParams *chPrms,
                                UInt32 chNum)
{
    FVID2_Format       *fmt;

    /* Init the module parameters */
#ifdef SC_APP_TEST_SUBFRAME
    chPrms->drnEnable = FALSE;
    vpsInitDefaultSubFramePrms(appObj, chPrms->subFrameParams);
#else
    chPrms->drnEnable = appObj->drnEnable;
#endif
    chPrms->comprEnable = appObj->comprEnable;
    vpsInitDefaultDeiHqPrms(appObj, chPrms->deiHqCfg);
    vpsInitDefaultDeiPrms(appObj, chPrms->deiCfg);
    vpsInitDefaultScCropPrms(appObj, chPrms->scCfg, chPrms->deiCropCfg);
    vpsInitDefaultScCropPrms(appObj, chPrms->vipScCfg, chPrms->vipCropCfg);

    /* Disable scalars if input and output dimensions are same */
    if (TRUE == appObj->deiBypassFlag)
    {
        if ((appObj->inWidth == appObj->outWidthDei) &&
            (appObj->inHeight == appObj->outHeightDei))
        {
            chPrms->scCfg->bypass = TRUE;
        }
        else
        {
            chPrms->scCfg->bypass = FALSE;
        }
    }
    else
    {
        if ((appObj->inWidth == appObj->outWidthDei) &&
            ((appObj->inHeight * 2u) == appObj->outHeightDei))
        {
            chPrms->scCfg->bypass = TRUE;
        }
        else
        {
            chPrms->scCfg->bypass = FALSE;
        }
    }
    if (TRUE == appObj->deiBypassFlag)
    {
        if ((appObj->inWidth == appObj->outWidthVip) &&
            (appObj->inHeight == appObj->outHeightVip))
        {
            chPrms->vipScCfg->bypass = TRUE;
        }
        else
        {
            chPrms->vipScCfg->bypass = FALSE;
        }
    }
    else
    {
        if ((appObj->inWidth == appObj->outWidthVip) &&
            ((appObj->inHeight * 2u) == appObj->outHeightVip))
        {
            chPrms->vipScCfg->bypass = TRUE;
        }
        else
        {
            chPrms->vipScCfg->bypass = FALSE;
        }
    }

    /* Set the FVID2 format for DEI input */
    fmt = &chPrms->inFmt;
    fmt->channelNum = chNum;
    fmt->width = appObj->inWidth;
    fmt->height = appObj->inHeight;
    if ((FVID2_DF_YUV420SP_UV == appObj->inDataFmt) ||
        (FVID2_DF_YUV422SP_UV == appObj->inDataFmt))
    {
        fmt->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->inPitchY;
        fmt->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->inPitchCbCr;
        fmt->fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] = FALSE;
        fmt->fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] = FALSE;
        if (FVID2_DF_YUV420SP_UV == appObj->inDataFmt)
        {
            fmt->bpp = FVID2_BPP_BITS12;
        }
        else
        {
            fmt->bpp = FVID2_BPP_BITS16;
        }
    }
    else
    {
        fmt->pitch[FVID2_YUV_INT_ADDR_IDX] = appObj->inPitchY;
        fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
        fmt->bpp = FVID2_BPP_BITS16;
    }
    fmt->dataFormat = appObj->inDataFmt;
    fmt->scanFormat = appObj->inScanFmt;
    fmt->reserved = NULL;

    /* Set the FVID2 format for DEI writeback output */
    fmt = chPrms->outFmtDei;
    fmt->channelNum = chNum;
    fmt->width = appObj->outWidthDei;
    fmt->height = appObj->outHeightDei;
    fmt->pitch[FVID2_YUV_INT_ADDR_IDX] = appObj->outPitchDei;
    fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    fmt->dataFormat = FVID2_DF_YUV422I_YUYV;
    fmt->scanFormat = appObj->outScanFmt;
    fmt->bpp = FVID2_BPP_BITS16;
    fmt->reserved = NULL;

    /* Set the FVID2 format for DEI-VIP output */
    fmt = chPrms->outFmtVip;
    fmt->channelNum = chNum;
    fmt->width = appObj->outWidthVip;
    fmt->height = appObj->outHeightVip;
    if ((FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip) ||
        (FVID2_DF_YUV422SP_UV == appObj->outDataFmtVip))
    {
        fmt->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->outPitchYVip;
        fmt->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->outPitchCbCrVip;
        fmt->fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] = FALSE;
        fmt->fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] = FALSE;
        if (FVID2_DF_YUV420SP_UV == appObj->outDataFmtVip)
        {
            fmt->bpp = FVID2_BPP_BITS12;
        }
        else
        {
            fmt->bpp = FVID2_BPP_BITS16;
        }
    }
    else
    {
        fmt->pitch[FVID2_YUV_INT_ADDR_IDX] = appObj->outPitchYVip;
        fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
        fmt->bpp = FVID2_BPP_BITS16;
    }
    fmt->dataFormat = appObj->outDataFmtVip;
    fmt->scanFormat = appObj->outScanFmt;
    fmt->reserved = NULL;

    return;
}



/**
 *  App_m2mDeiInitVariables
 *  Initialize the global variables and frame pointers.
 */
static Void App_m2mDeiInitVariables(App_M2mDeiObj *appObj)
{
    UInt32              chCnt, frmCnt, fListCnt, cntxIndex;
    FVID2_Frame        *frm;
    FVID2_FrameList    *fList;
    FVID2_ProcessList  *pList;
    Vps_M2mDeiChParams *chPrms;

    /* Set this first as other functions need this to be done first */
    if (appObj->inScanFmt == appObj->outScanFmt)
    {
        appObj->deiBypassFlag = TRUE;
    }
    else
    {
        appObj->deiBypassFlag = FALSE;
    }

    /* Init create params */
    appObj->createParams.mode = appObj->mode;
    appObj->createParams.numCh = appObj->numCh;
    appObj->createParams.deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;
    appObj->createParams.chParams = &appObj->chPrms[0u];

    /* Select whether VIP scalar is required or not */
    if (VpsUtils_appM2mDeiIsVipInst(appObj->drvInst))
    {
        if (TRUE == appObj->deiBypassFlag)
        {
            if ((appObj->inWidth == appObj->outWidthVip) &&
                (appObj->inHeight == appObj->outHeightVip))
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
            if ((appObj->inWidth == appObj->outWidthVip) &&
                ((appObj->inHeight * 2u) == appObj->outHeightVip))
            {
                appObj->createParams.isVipScReq = FALSE;
            }
            else
            {
                appObj->createParams.isVipScReq = TRUE;
            }
        }
    }
    else
    {
        appObj->createParams.isVipScReq = FALSE;
    }

    for (chCnt = 0u; chCnt < appObj->numCh; chCnt++)
    {
        chPrms = &appObj->chPrms[chCnt];

        /* Assign the memories for the pointer members */
        chPrms->outFmtDei = &appObj->outFmtDei[chCnt];
        chPrms->outFmtVip = &appObj->outFmtVip[chCnt];
        chPrms->inFmtFldN_1 = NULL;
        chPrms->inMemType = appObj->inMemType;
        chPrms->outMemTypeDei = VPS_VPDMA_MT_NONTILEDMEM;
        chPrms->outMemTypeVip = appObj->outMemTypeVip;
        if (TRUE == appObj->isDeiPresent)
        {
            chPrms->deiHqCfg = &appObj->deiHqCfg[chCnt];
            chPrms->deiCfg = &appObj->deiCfg[chCnt];
        }
        else
        {
            chPrms->deiHqCfg = NULL;
            chPrms->deiCfg = NULL;
        }
        chPrms->scCfg = &appObj->scCfg[chCnt];
        chPrms->vipScCfg = &appObj->vipScCfg[chCnt];
        chPrms->vipCropCfg = &appObj->vipScCropCfg[chCnt];
        chPrms->deiCropCfg = &appObj->scCropCfg[chCnt];
#ifdef SC_APP_TEST_SUBFRAME
        chPrms->subFrameParams = &appObj->subFramePrms[chCnt];
#else
        chPrms->subFrameParams = NULL;
#endif
        /* Init channel params */
        vpsSetDefaultChPrms(appObj, chPrms, chCnt);
    }

    /* Init other globals used in buffer exchange */
    appObj->inBufIdx = 0;
    appObj->outBufIdx = 0;
    appObj->completedLoopCnt = 0;

    pList = &appObj->processList;
    pList->inFrameList[0u] = &appObj->frameList[0u];
    pList->outFrameList[0u] = &appObj->frameList[1u];
    pList->outFrameList[1u] = &appObj->frameList[2u];
    pList->numInLists = 1u;
    if (VpsUtils_appM2mDeiIsDualOutInst(appObj->drvInst))
    {
        pList->numOutLists = 2u;
    }
    else
    {
        pList->numOutLists = 1u;
    }
    pList->reserved = NULL;

    for (fListCnt = 0u; fListCnt < DEI_NUM_FRAMELIST_PER_PLIST; fListCnt++)
    {
        fList = &appObj->frameList[fListCnt];
        fList->numFrames = appObj->numCh;
        fList->perListCfg = NULL;
        fList->reserved = NULL;
        for (frmCnt = 0u; frmCnt < appObj->numCh; frmCnt++)
        {
            frm = &appObj->frames[fListCnt][frmCnt];
            fList->frames[frmCnt] = frm;
            App_m2mDeiInitFrame(frm, frmCnt);
        }
    }

    for (chCnt = 0x0; chCnt < VPS_M2M_DEI_MAX_CH_PER_INST; chCnt++)
    {
        appObj->freeCntxFrameIndex[chCnt] = 0x0;
        for (cntxIndex = 0x0;
             cntxIndex < DEI_NO_OF_BUF_REQ_BEFORE_RES;
             cntxIndex++)
        {
            App_m2mDeiInitFrame(&(appObj->cntxFrames[chCnt][cntxIndex]),
                                chCnt);
        }
    }

    /* Init error process list */
    memset(&appObj->errProcessList, 0u, sizeof (appObj->errProcessList));
}

static Void App_m2mDeiInitFrame(FVID2_Frame *frm, UInt32 chanNo)
{
    frm->channelNum = chanNo;
    frm->appData = NULL;
    frm->perFrameCfg = NULL;
            frm->subFrameInfo = NULL;
    frm->reserved = NULL;
    frm->fid = FVID2_FID_TOP;
    frm->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX]
        = NULL;
    frm->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX]
        = NULL;
    frm->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX]
        = NULL;
    frm->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX]
        = NULL;
}

