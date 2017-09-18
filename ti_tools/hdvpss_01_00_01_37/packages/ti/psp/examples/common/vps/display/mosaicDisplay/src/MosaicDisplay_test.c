/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file MosaicDisplay_test.c
 *
 *  \brief VPS Bypass path 0/1 mosaic display example file.
 *
 *  Default Settings:
 *  -----------------
 *  This application creates various mosaic layouts and demonstrates the
 *  dynamic layout change feature of the display driver by changing the layout
 *  while the display is in progress.
 *  By default, the application displays various layouts through the
 *  Bypass path 0.
 *  Since the input buffer size used for the mosaic windows is same, this
 *  application makes use of the buffer pitch to display a smaller window
 *  by cropping the input buffer according to the layout.
 *
 *  This application creates the following layouts - 1x1 (Full screen),
 *  1x1 (Non-Full screen), 2x2 (Full screen), 4x4 (Full screen),
 *  3x3 (Full screen), 6 CH layout, 8 CH layout and 1x2 (Non-Full screen).
 *  Then while the display is in progress, changes the layout every
 *  LAYOUT_SWITCH_RATE frames.
 *
 *  Other Configuration Options:
 *  ----------------------------
 *
 *  Driver Instance:
 *      Change DRIVER_INSTANCE to VPS_DISP_INST_BP1 to display via bypass 1
 *  path.
 *
 *  Layout Change Rate:
 *      Change the LAYOUT_SWITCH_RATE macro to change how often the layout
 *  should change. To disable dynamic layout change, set this to more
 *  than the application loop count.
 *
 *  Input Buffer Parameter:
 *      Change BUFFER_WIDTH and BUFFER_HEIGHT macro according to the input
 *  buffer dimension. The application will automatically change the layout
 *  window sizes according to the buffer size and pitch.
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
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/heaps/HeapBuf.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/utility/vpsutils_app.h>
#include "MosaicDisplay_test.h"


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void App_dispTestTask(UArg arg0, UArg arg1);
static Int32 App_dispMosaicTest(App_DisplayObj *appObj);
static Int32 App_dispCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
static Int32 App_dispErrCbFxn(FVID2_Handle handle,
                              Ptr appData,
                              Void *errList,
                              Ptr reserved);
static Void App_dispUpdateFrameList(App_DisplayObj *appObj,
                                    FVID2_FrameList *fList);
static Void App_dispFreeFrmListMem(App_DisplayObj *appObj,
                                   FVID2_FrameList *fList);
static Void App_dispSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt);

static Int32 App_dispCreateMosaicLayouts(App_DisplayObj *appObj);

static Int32 App_dispInitVariables(App_DisplayObj *appObj);
static Void App_dispFreeVariables(App_DisplayObj *appObj);

static Int32 App_dispGetLayoutsFromTable(App_DisplayObj *appObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppDispTestTaskStack, 32)
#pragma DATA_SECTION(AppDispTestTaskStack, ".bss:taskStackSection")
static UInt8 AppDispTestTaskStack[APP_TSK_STACK_SIZE];

/* Display application object per display instance. */
App_DisplayObj AppDispObjs;

/* Mosaic layout table defining the window parameters for each layout. */
static App_MosaicObj gAppDispLayoutTbl_1080[] =
                                    {APP_DISP_LAYOUT_TABLE_DEF_1080};
static App_MosaicObj gAppDispLayoutTbl_720[] = {APP_DISP_LAYOUT_TABLE_DEF_720};
static App_MosaicObj gAppDispLayoutTbl_480[] = {APP_DISP_LAYOUT_TABLE_DEF_480};
static App_MosaicObj gAppDispLayoutTbl_576[] = {APP_DISP_LAYOUT_TABLE_DEF_576};
static App_MosaicObj gAppDispLayoutTbl_XGA[] = {APP_DISP_LAYOUT_TABLE_DEF_XGA};
static App_MosaicObj gAppDispLayoutTbl_WSVGA[] = {APP_DISP_LAYOUT_TABLE_DEF_WSVGA};

UInt32 gAppDispLoopCount = 0u;

extern Semaphore_Handle AppTestCompleteSem;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  App_dispCreateTsk
 *  Display Application create task function.
 */
Task_Handle App_dispCreateTsk(App_DisplayTestCfg *testCfg)
{
    Task_Params     tskParams;
    Task_Handle     tskHandle;
    App_DisplayObj *appObj;

    appObj = &AppDispObjs;
    memcpy(&appObj->testCfg, testCfg, sizeof (App_DisplayTestCfg));

    Task_Params_init(&tskParams);
    tskParams.stack = AppDispTestTaskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    tskParams.arg0 = (UArg) appObj;
    tskHandle = Task_create((Task_FuncPtr) App_dispTestTask, &tskParams, NULL);

    return (tskHandle);
}



/**
 *  App_dispTestTask
 *  Display application test task.
 */
static Void App_dispTestTask(UArg arg0, UArg arg1)
{
    Int32               retVal, driverRetVal;
    App_DisplayObj     *appObj;
    Vps_DcCreateConfig  dcCreateCfg;
    Vps_DcEdeConfig     dcEdeCfg;
    Vps_CprocConfig     cprocCfg;
    Vps_DcAnalogFmt     aFmt;

    appObj = (App_DisplayObj *) arg0;

    /* Get platform type, board and CPU revisions */
    appObj->platformId = Vps_platformGetId();
    if ((appObj->platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (appObj->platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf("%s: Error Unrecognized platform @ line %d\n",
            __FUNCTION__, __LINE__);
        return;
    }
    appObj->cpuRev = Vps_platformGetCpuRev();
    if (appObj->cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        Vps_printf("%s: Error Unrecognized CPU version @ line %d\n",
            __FUNCTION__, __LINE__);
        return;
    }
    appObj->boardId = Vps_platformGetBoardId();
    if (appObj->boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("%s: Error Unrecognized board @ line %d\n",
            __FUNCTION__, __LINE__);
        return;
    }

    appObj->modeInfo.standard = appObj->testCfg.standard;
    retVal = FVID2_getModeInfo(&appObj->modeInfo);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Invalid display mode %d selected\n",
            APP_NAME, appObj->modeInfo.standard);
        return;
    }

    if (appObj->testCfg.width > appObj->modeInfo.width)
    {
        appObj->testCfg.width = appObj->modeInfo.width;
    }
    if (appObj->testCfg.height > appObj->modeInfo.height)
    {
        appObj->testCfg.height = appObj->modeInfo.height;
    }
    appObj->mosaicEnable = MOSAIC_ENABLE;
    if (TRUE == appObj->mosaicEnable)
    {
        /* Get the layouts from table */
        App_dispGetLayoutsFromTable(appObj);
    }

    /* Configure pixel clock */
    retVal = VpsUtils_appSetVencPixClk(
                 appObj->testCfg.vencId,
                 appObj->modeInfo.standard);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Enable EDE */
    dcEdeCfg.ltiEnable = TRUE;
    dcEdeCfg.horzPeaking = TRUE;
    dcEdeCfg.ctiEnable = TRUE;
    dcEdeCfg.transAdjustEnable = TRUE;
    dcEdeCfg.lumaPeaking = TRUE;
    dcEdeCfg.chromaPeaking = TRUE;
    dcEdeCfg.minClipLuma = 0;
    dcEdeCfg.maxClipLuma = 1023;
    dcEdeCfg.minClipChroma = 0;
    dcEdeCfg.maxClipChroma = 1023;
    dcEdeCfg.bypass = FALSE;
    memset(&dcCreateCfg, 0, sizeof(dcCreateCfg));

    memset(&cprocCfg, 0, sizeof(cprocCfg));
    cprocCfg.ciecamMode = VPS_CPROC_CIECAM_MODE_BT709;

    dcCreateCfg.edeConfig = &dcEdeCfg;
    dcCreateCfg.cprocConfig = &cprocCfg;

    /* Open Display Controller */
    appObj->dcHandle = FVID2_create(
                           FVID2_VPS_DCTRL_DRV,
                           VPS_DCTRL_INST_0,
                           &dcCreateCfg,
                           &driverRetVal,
                           NULL);
    if (NULL == appObj->dcHandle)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, driverRetVal, __LINE__);
        return;
    }

    /* Start VENC */
    aFmt = VPS_DC_A_OUTPUT_COMPONENT;
    if (VPS_DC_VENC_SD == appObj->testCfg.vencId)
    {
        aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
        if (VPS_PLATFORM_ID_EVM_TI814x == appObj->platformId)
        {
            aFmt = VPS_DC_A_OUTPUT_SVIDEO;
        }
    }
    retVal = VpsUtils_appStartVenc(
                 appObj->dcHandle,
                 appObj->testCfg.vencId,
                 appObj->modeInfo.standard,
                 aFmt);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

#if defined(TI_8107_BUILD)
    if (FVID2_STD_WSVGA_70 == appObj->modeInfo.standard)
    {
        Vps_platformEnableLcd(TRUE);
        Vps_platformEnableLcdBackLight(TRUE);
        Vps_platformSetLcdBackLight(0xFFFFFFFC);
    }
#endif

    /* Run the test */
    retVal = App_dispMosaicTest(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

#if defined(TI_8107_BUILD)
    if (FVID2_STD_WSVGA_70 == appObj->modeInfo.standard)
    {
        Vps_platformEnableLcd(FALSE);
    }
#endif

    /* Stop VENC */
    retVal = VpsUtils_appStopVenc(
                 appObj->dcHandle,
                 appObj->testCfg.vencId,
                 appObj->modeInfo.standard);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Close Display Controller */
    retVal = FVID2_delete(appObj->dcHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    Vps_printf("%s: Test Successful!!\n", APP_NAME);

    /* Test complete. Signal it */
    Semaphore_post(AppTestCompleteSem);
}



/**
 *  App_dispMosaicTest
 *  Mosaic display test function.
 */
static Int32 App_dispMosaicTest(App_DisplayObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  frmListCnt;
    FVID2_Format            fmt;
    FVID2_FrameList        *fList;
    FVID2_CbParams          cbParams;
    VpsUtils_PrfTsHndl     *prfTsHandle;
    Vps_DispCreateParams    createPrms =
    {
        VPS_VPDMA_MT_NONTILEDMEM,
        FALSE
    };
    Vps_DispCreateStatus    createStatus;

    /* Initialize and allocate handles */
    retVal = App_dispInitVariables(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Configure display controller - Set the paths */
    retVal = VpsUtils_appSetDctrlPath(
                 appObj->dcHandle,
                 FVID2_VPS_DISP_DRV,
                 appObj->testCfg.driverInst,
                 appObj->testCfg.vencId,
                 FALSE);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Open the driver */
    cbParams.cbFxn = App_dispCbFxn;
    cbParams.errCbFxn = App_dispErrCbFxn;
    cbParams.errList = &appObj->errFrameList;
    cbParams.appData = appObj;
    cbParams.reserved = NULL;
    appObj->fvidHandle = FVID2_create(
                             FVID2_VPS_DISP_DRV,
                             appObj->testCfg.driverInst,
                             &createPrms,
                             &createStatus,
                             &cbParams);
    if (NULL == appObj->fvidHandle)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, createStatus.retVal,
            __LINE__);
        return (createStatus.retVal);
    }
    Vps_printf("%s: Display Driver Opened for Instance %d!!\n",
        APP_NAME, appObj->testCfg.driverInst);
    VpsUtils_appPrintDispCreateStatus(&createStatus);
    appObj->drvMaxLayouts = createStatus.maxMultiWinLayout;

    /* Set the required format */
    App_dispSetFormat(appObj, &fmt);
    retVal = FVID2_setFormat(appObj->fvidHandle, &fmt);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    if (TRUE == appObj->mosaicEnable)
    {
        /* Create the mosaic layouts */
        retVal = App_dispCreateMosaicLayouts(appObj);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }

        /* Select a mosaic layout - start with first layout */
        appObj->curLayoutIdx = 0u;
        appObj->curLayout = &appObj->mosaicInfo[appObj->curLayoutIdx];
        retVal = FVID2_control(
                     appObj->fvidHandle,
                     IOCTL_VPS_SELECT_LAYOUT,
                     &appObj->curLayout->layoutId,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    /* Start the load calculation */
    VpsUtils_prfLoadCalcStart();
    /* Start the TimeStamp Calculation */
    prfTsHandle = VpsUtils_prfTsCreate(APP_NAME":");

    /* Prime Buffers */
    for (frmListCnt = 0u; frmListCnt < NUM_PRIMING; frmListCnt++)
    {
        /* Update the buffer address */
        fList = &appObj->frameList;
        App_dispUpdateFrameList(appObj, fList);

        /* Queue buffers to driver */
        retVal = FVID2_queue(appObj->fvidHandle, fList, 0u);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }
    /* Start driver */
    Vps_printf("%s: Starting Display Driver %d...\n",
        APP_NAME, appObj->testCfg.driverInst);
    retVal = FVID2_start(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    VpsUtils_prfTsBegin(prfTsHandle);

    /* Use the same frame list for all request */
    fList = &appObj->frameList;
    while (1u)
    {
        /* Wait for driver callback to occur */
        Semaphore_pend(appObj->readtyToDqSem, BIOS_WAIT_FOREVER);

        /* Dequeue buffer from driver */
        retVal = FVID2_dequeue(
                     appObj->fvidHandle,
                     fList,
                     0u,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK != retVal)
        {
            /* Driver could return error if no buffers are to be dequeued */
            continue;
        }

        /* Free the dequeued frames and RT params */
        App_dispFreeFrmListMem(appObj, fList);

        /* Update the buffer address */
        App_dispUpdateFrameList(appObj, fList);

        /* Queue the updated frame list back to the driver */
        retVal = FVID2_queue(appObj->fvidHandle, fList, 0u);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            break;
        }

        appObj->completedFrames++;
        gAppDispLoopCount = appObj->completedFrames;
        if (appObj->completedFrames >= TOTAL_LOOP_COUNT)
        {
            break;
        }
    }

    VpsUtils_prfTsEnd(prfTsHandle, TOTAL_LOOP_COUNT);

    /* Stop driver */
    retVal = FVID2_stop(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    Vps_printf("%s: Display Driver %d Stopped!!\n",
        APP_NAME, appObj->testCfg.driverInst);

    /* Stop the load calculation */
    VpsUtils_prfLoadCalcStop();
    /* Print the load */
    VpsUtils_prfLoadPrintAll(TRUE);
    /* print the fps based on TimeStamp */
    VpsUtils_prfTsPrint(prfTsHandle, TRUE);

    /* Delete the timestamp instance */
    VpsUtils_prfTsDelete(prfTsHandle);
    /* Reset the load */
    VpsUtils_prfLoadCalcReset();

    /* Dequeue all the driver held buffers */
    fList = &appObj->frameList;
    while (1u)
    {
        /* Dequeue buffers from driver */
        retVal = FVID2_dequeue(
                     appObj->fvidHandle,
                     fList,
                     0u,
                     FVID2_TIMEOUT_NONE);
        if (FVID2_SOK != retVal)
        {
            break;
        }

        /* Free the dequeued frames and RT params */
        App_dispFreeFrmListMem(appObj, fList);
    }

    if (TRUE == appObj->mosaicEnable)
    {
        /* Delete all mosaic layout */
        retVal = FVID2_control(
                     appObj->fvidHandle,
                     IOCTL_VPS_DELETE_ALL_LAYOUT,
                     NULL,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    /* Print the display status */
    VpsUtils_appPrintDispInstStatus(appObj->fvidHandle);

    /* Close driver */
    retVal = FVID2_delete(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Configure display controller - Remove the settings */
    retVal = VpsUtils_appClearDctrlPath(
                 appObj->dcHandle,
                 FVID2_VPS_DISP_DRV,
                 appObj->testCfg.driverInst,
                 appObj->testCfg.vencId,
                 FALSE);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Free allocated handles */
    App_dispFreeVariables(appObj);

    Vps_printf("%s: Mosaic Display %d Test Successful!!\n",
        APP_NAME, appObj->testCfg.driverInst);

    return (retVal);
}



/**
 *  App_dispCbFxn
 *  Driver callback function.
 */
static Int32 App_dispCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    App_DisplayObj     *appObj = (App_DisplayObj *) appData;

    /* Buffer ready to dequeue. Signal it */
    Semaphore_post(appObj->readtyToDqSem);

    return (FVID2_SOK);
}



/**
 *  App_dispErrCbFxn
 *  Driver error callback function.
 */
static Int32 App_dispErrCbFxn(FVID2_Handle handle,
                              Ptr appData,
                              Void *errList,
                              Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}



/**
 *  App_dispUpdateFrameList
 *  Updates the frame list with the next request buffer information.
 */
static Void App_dispUpdateFrameList(App_DisplayObj *appObj,
                                    FVID2_FrameList *fList)
{
    UInt32              index;
    UInt32              frmCnt;
    FVID2_Frame        *frame;
    Vps_DispRtParams   *rtPrms = NULL;

    /* Change the application layout every LAYOUT_SWITCH_RATE frames */
    if ((TRUE == appObj->mosaicEnable) &&
        (0u != appObj->submittedFrames) &&
        (0u == (appObj->submittedFrames % LAYOUT_SWITCH_RATE)))
    {
        /* Move to the next layout */
        appObj->curLayoutIdx++;
        if (appObj->curLayoutIdx >= appObj->totalLayouts)
        {
            /* Wrap around */
            appObj->curLayoutIdx = 0u;
        }
        appObj->curLayout = &appObj->mosaicInfo[appObj->curLayoutIdx];

        /* Allocate runtime configuration */
        rtPrms = (Vps_DispRtParams *)
            VpsUtils_memAlloc(sizeof(Vps_DispRtParams), sizeof(UInt32));
        if (NULL == rtPrms)
        {
            /* This can't happen as we have allocated sufficient memory */
            Vps_printf("%s: Alloc of RT params failed!!\n", APP_NAME);
        }
        else
        {
            rtPrms->layoutId = &appObj->curLayout->layoutId;
            rtPrms->vcompCropCfg = NULL;
            rtPrms->vcompPosCfg = NULL;
            rtPrms->inFrmPrms = NULL;
            rtPrms->vpdmaPosCfg = NULL;
        }
    }
    fList->perListCfg = rtPrms;

    /* Fill the next set of buffers */
    fList->numFrames = appObj->curLayout->numWindows;
    index = appObj->bufferIndex;
    for (frmCnt = 0u; frmCnt < appObj->curLayout->numWindows; frmCnt++)
    {
        /* Wrap around */
        if (index >= appObj->testCfg.numInBuf)
        {
            index = 0u;
        }

        frame = (FVID2_Frame *) HeapBuf_alloc(
                                    appObj->frameHandle,
                                    sizeof(FVID2_Frame),
                                    sizeof(UInt32),
                                    NULL);
        if (NULL == frame)
        {
            /* This can't happen as we have allocated sufficient memory */
            Vps_printf("%s: Frame alloc failed\n", APP_NAME);
            break;
        }

        frame->channelNum = 0u;
        frame->appData = NULL;
        frame->perFrameCfg = NULL;
        frame->subFrameInfo = NULL;
        frame->reserved = NULL;
        frame->fid = FVID2_FID_FRAME;
        if (FVID2_SF_PROGRESSIVE == appObj->modeInfo.scanFormat)
        {
            frame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                (void *) (appObj->testCfg.inBuf +
                            (index * appObj->testCfg.inFrmSize));
            frame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                ((UInt8 *) frame->addr[FVID2_FRAME_ADDR_IDX]
                                      [FVID2_YUV_SP_Y_ADDR_IDX])
                    + (appObj->testCfg.pitch * BUFFER_HEIGHT);
        }
        else
        {
            frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                (void *) (appObj->testCfg.inBuf +
                            (index * appObj->testCfg.inFrmSize));
            frame->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                (void *) (appObj->testCfg.inBuf +
                            (index * appObj->testCfg.inFrmSize) +
                            appObj->testCfg.pitch);
            frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                ((UInt8 *) frame->addr[FVID2_FIELD_EVEN_ADDR_IDX]
                                      [FVID2_YUV_SP_Y_ADDR_IDX])
                    + (appObj->testCfg.pitch * BUFFER_HEIGHT);
            frame->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                ((UInt8 *) frame->addr[FVID2_FIELD_ODD_ADDR_IDX]
                                      [FVID2_YUV_SP_Y_ADDR_IDX])
                    + (appObj->testCfg.pitch * BUFFER_HEIGHT);
        }
        fList->frames[frmCnt] = frame;

        index++;
    }

    appObj->submittedFrames++;

    /* Increment the input frame index only every FRAME_SWITCH_RATE frames */
    if (0u == (appObj->submittedFrames % FRAME_SWITCH_RATE))
    {
        appObj->bufferIndex += appObj->curLayout->numWindows;
    }
    /* Wrap around */
    if (appObj->bufferIndex >= appObj->testCfg.numInBuf)
    {
        appObj->bufferIndex %= appObj->testCfg.numInBuf;
    }

    return;
}



/**
 *  App_dispFreeFrmListMem
 *  Frees the frames and RT params from the dequeued frame list.
 */
static Void App_dispFreeFrmListMem(App_DisplayObj *appObj,
                                   FVID2_FrameList *fList)
{
    UInt32              frmCnt;
    FVID2_Frame        *frame;

    /* Free the dequeued frames */
    for (frmCnt = 0u; frmCnt < fList->numFrames; frmCnt++)
    {
        frame = fList->frames[frmCnt];
        HeapBuf_free(appObj->frameHandle, frame, sizeof(FVID2_Frame));
        fList->frames[frmCnt] = NULL;
    }

    /* Free the RT params if already allocated */
    if (NULL != fList->perListCfg)
    {
        VpsUtils_memFree(fList->perListCfg, sizeof(Vps_DispRtParams));
        fList->perListCfg = NULL;
    }

    return;
}



/**
 *  App_dispSetFormat
 *  Sets the required/default format.
 */
static Void App_dispSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt)
{
    fmt->channelNum = 0u;
    fmt->width = appObj->testCfg.width;
    fmt->height = appObj->testCfg.height;
    fmt->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->testCfg.pitch;
    fmt->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->testCfg.pitch;
    fmt->scanFormat = appObj->modeInfo.scanFormat;
    if (FVID2_SF_INTERLACED == appObj->modeInfo.scanFormat)
    {
        fmt->fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] = TRUE;
        fmt->fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] = TRUE;
    }
    else
    {
        fmt->fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] = FALSE;
        fmt->fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] = FALSE;
    }
    fmt->dataFormat = appObj->testCfg.dataFormat;
    if (FVID2_DF_YUV420SP_UV == appObj->testCfg.dataFormat)
    {
        fmt->bpp = FVID2_BPP_BITS12;
    }
    else
    {
        fmt->bpp = FVID2_BPP_BITS16;
    }
    fmt->reserved = NULL;

    return;
}



/**
 *  App_dispCreateMosaicLayouts
 *  Creates the various mosaic layouts.
 *  This function places the mosaic window in the center of the frame at equal
 *  distance to each other.
 */
static Int32 App_dispCreateMosaicLayouts(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              layoutCnt;
    App_MosaicObj      *mObj;
    Vps_MultiWinParams  multiWinPrms;

    /* Check if the application requested number of layouts is more than what
     * is supported in the driver */
    if (appObj->totalLayouts > appObj->drvMaxLayouts)
    {
        /* Create whatever is possible by driver */
        appObj->totalLayouts = appObj->drvMaxLayouts;
    }

    for (layoutCnt = 0u; layoutCnt < appObj->totalLayouts; layoutCnt++)
    {
        mObj = &appObj->mosaicInfo[layoutCnt];

        /* Create the layout by calling driver API */
        multiWinPrms.channelNum = 0u;
        multiWinPrms.numWindows = mObj->numWindows;
        multiWinPrms.winFmt = &mObj->winFmt[0u];
        retVal = FVID2_control(
                     appObj->fvidHandle,
                     IOCTL_VPS_CREATE_LAYOUT,
                     &multiWinPrms,
                     &mObj->layoutId);
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
 *  App_dispInitVariables
 *  Initialize the global variables and frame pointers.
 */
static Int32 App_dispInitVariables(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    HeapBuf_Params      frmHeapPrms;
    Semaphore_Params    semParams;

    Semaphore_Params_init(&semParams);
    appObj->readtyToDqSem = Semaphore_create(0, &semParams, NULL);
    if (NULL == appObj->readtyToDqSem)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EALLOC);
    }

    /* Create frame heap */
    HeapBuf_Params_init(&frmHeapPrms);
    frmHeapPrms.numBlocks = NUM_FRAMES;
    frmHeapPrms.blockSize = sizeof(FVID2_Frame);
    frmHeapPrms.bufSize = sizeof(appObj->framePool);
    frmHeapPrms.buf = &appObj->framePool[0u];
    appObj->frameHandle = HeapBuf_create(&frmHeapPrms, NULL);
    if (NULL == appObj->frameHandle)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EALLOC);
    }

    appObj->bufferIndex = 0u;
    appObj->submittedFrames = 0u;
    appObj->completedFrames = 0u;
    appObj->curLayoutIdx = 0u;
    appObj->curLayout = &appObj->mosaicInfo[appObj->curLayoutIdx];
    if (FALSE == appObj->mosaicEnable)
    {
        appObj->curLayout->numWindows = 1u;
    }

    /* Init frame list */
    appObj->frameList.numFrames = MAX_MOSAIC_FRAMES;
    appObj->frameList.perListCfg = NULL;
    appObj->frameList.reserved = NULL;

    /* Init error frame list */
    memset(&appObj->errFrameList, 0u, sizeof (appObj->errFrameList));

    return (retVal);
}



/**
 *  App_dispFreeVariables
 *  Free the allocated handles and buffers.
 */
static Void App_dispFreeVariables(App_DisplayObj *appObj)
{
    HeapBuf_delete(&appObj->frameHandle);
    Semaphore_delete(&appObj->readtyToDqSem);
}



/**
 *  App_dispGetLayoutsFromTable
 *  Gets the layouts from the table.
 */
static Int32 App_dispGetLayoutsFromTable(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              layoutCnt, winCnt;
    UInt32              numLayouts, numWindows;
    App_MosaicObj      *mObj, *mObjTbl;
    Vps_WinFormat      *winFmt;

    if ((FVID2_STD_720P_60 == appObj->modeInfo.standard) ||
        (FVID2_STD_720P_50 == appObj->modeInfo.standard))
    {
        numLayouts = (sizeof (gAppDispLayoutTbl_720)) /
                     (sizeof (App_MosaicObj));
    }
    else if ((FVID2_STD_480P == appObj->modeInfo.standard) ||
             (FVID2_STD_NTSC == appObj->modeInfo.standard))
    {
        numLayouts = (sizeof (gAppDispLayoutTbl_480)) /
                     (sizeof (App_MosaicObj));
    }
    else if ((FVID2_STD_576P == appObj->modeInfo.standard) ||
             (FVID2_STD_PAL == appObj->modeInfo.standard))
    {
        numLayouts = (sizeof (gAppDispLayoutTbl_576)) /
                     (sizeof (App_MosaicObj));
    }
    else if (FVID2_STD_XGA_60 == appObj->modeInfo.standard)
    {
        numLayouts = (sizeof (gAppDispLayoutTbl_XGA)) /
                     (sizeof (App_MosaicObj));
    }
    else if (FVID2_STD_WSVGA_70 == appObj->modeInfo.standard)
    {
        numLayouts = (sizeof (gAppDispLayoutTbl_WSVGA)) /
                     (sizeof (App_MosaicObj));
    }
    else
    {
        numLayouts = (sizeof (gAppDispLayoutTbl_1080)) /
                     (sizeof (App_MosaicObj));
    }
    Vps_printf("%s: Number of Layouts Selected: %d\n", APP_NAME, numLayouts);

    for (layoutCnt = 0u; layoutCnt < numLayouts; layoutCnt++)
    {
        if ((FVID2_STD_720P_60 == appObj->modeInfo.standard) ||
            (FVID2_STD_720P_50 == appObj->modeInfo.standard))
        {
            mObjTbl = &gAppDispLayoutTbl_720[layoutCnt];
        }
        else if ((FVID2_STD_480P == appObj->modeInfo.standard) ||
                 (FVID2_STD_NTSC == appObj->modeInfo.standard))
        {
            mObjTbl = &gAppDispLayoutTbl_480[layoutCnt];
        }
        else if ((FVID2_STD_576P == appObj->modeInfo.standard) ||
                 (FVID2_STD_PAL == appObj->modeInfo.standard))
        {
            mObjTbl = &gAppDispLayoutTbl_576[layoutCnt];
        }
        else if (FVID2_STD_XGA_60 == appObj->modeInfo.standard)
        {
            mObjTbl = &gAppDispLayoutTbl_XGA[layoutCnt];
        }
        else if (FVID2_STD_WSVGA_70 == appObj->modeInfo.standard)
        {
            mObjTbl = &gAppDispLayoutTbl_WSVGA[layoutCnt];
        }
        else
        {
            mObjTbl = &gAppDispLayoutTbl_1080[layoutCnt];
        }
        mObj = &appObj->mosaicInfo[layoutCnt];

        numWindows = mObjTbl->numWindows;
        Vps_printf("%s: Layout %d Number of Windows: %d\n",
            APP_NAME, layoutCnt, numWindows);

        for (winCnt = 0u; winCnt < numWindows; winCnt++)
        {
            winFmt = &mObj->winFmt[winCnt];

            winFmt->winStartX = mObjTbl->winFmt[winCnt].winStartX;
            winFmt->winStartY = mObjTbl->winFmt[winCnt].winStartY;
            winFmt->winWidth = mObjTbl->winFmt[winCnt].winWidth;
            winFmt->winHeight = mObjTbl->winFmt[winCnt].winHeight;
            winFmt->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->testCfg.pitch;
            winFmt->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->testCfg.pitch;
            winFmt->dataFormat = appObj->testCfg.dataFormat;
            winFmt->bpp = FVID2_BPP_BITS16;
            if (FVID2_DF_YUV420SP_UV == appObj->testCfg.dataFormat)
            {
                winFmt->bpp = FVID2_BPP_BITS12;
            }
            else
            {
                winFmt->bpp = FVID2_BPP_BITS16;
            }
            winFmt->priority = mObjTbl->winFmt[winCnt].priority;

            /* Limit the window width and height within frame limits */
            winFmt->winStartX &= (~1u);     /* Make start position as even */
            winFmt->winStartY &= (~1u);     /* Make start position as even */
            if ((winFmt->winWidth + winFmt->winStartX) >
                (appObj->modeInfo.width))
            {
                winFmt->winWidth = appObj->modeInfo.width - winFmt->winStartX;
                winFmt->winWidth &= (~1u);  /* Make width as even */
            }
            if ((winFmt->winHeight + winFmt->winStartY) >
                (appObj->modeInfo.height))
            {
                winFmt->winHeight = appObj->modeInfo.height - winFmt->winStartY;
                winFmt->winHeight &= (~1u); /* Make height as even */
            }
        }
        mObj->numWindows = numWindows;
    }
    appObj->totalLayouts = numLayouts;

    return (retVal);
}
