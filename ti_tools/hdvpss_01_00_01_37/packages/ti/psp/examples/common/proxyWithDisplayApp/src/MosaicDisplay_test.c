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
#include <ti/psp/devices/vps_sii9022a.h>
#include <ti/psp/devices/vps_thsfilters.h>
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

static Void App_dispTestTask(void);
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
static Int32 App_dispConfigDctrl(const App_DisplayObj *appObj);
static Int32 App_dispSetPixClk(App_DisplayObj *appObj);
static Void App_dispSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt);

static Int32 App_dispConfigHdmi(App_DisplayObj *appObj);

static Int32 App_dispCreateMosaicLayouts(App_DisplayObj *appObj);

static Int32 App_dispInitVariables(App_DisplayObj *appObj);
static Void App_dispFreeVariables(App_DisplayObj *appObj);

static void App_dispGetInputFromUser(App_DisplayObj *appObj);
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

/* Display Controller Configuration */
static Vps_DcConfig AppDispDcConfig;

/* Mosaic layout table defining the window parameters for each layout. */
static App_MosaicObj gAppDispLayoutTbl_1080[] =
                                    {APP_DISP_LAYOUT_TABLE_DEF_1080};
static App_MosaicObj gAppDispLayoutTbl_720[] = {APP_DISP_LAYOUT_TABLE_DEF_720};
static App_MosaicObj gAppDispLayoutTbl_D1[] = {APP_DISP_LAYOUT_TABLE_DEF_D1};

UInt32 gAppDispLoopCount = 0u;

extern Semaphore_Handle AppTestCompleteSem1;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  App_dispCreateTsk
 *  Display Application create task function.
 */
Task_Handle App_dispCreateTsk(void)
{
    Task_Params tskParams;
    Task_Handle tskHandle;

    Task_Params_init(&tskParams);
    tskParams.stack = AppDispTestTaskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    tskHandle = Task_create((Task_FuncPtr) App_dispTestTask, &tskParams, NULL);

    return (tskHandle);
}



/**
 *  App_dispTestTask
 *  Display application test task.
 */
static Void App_dispTestTask(void)
{
    Int32               retVal;
    App_DisplayObj     *appObj;
#ifndef PLATFORM_ZEBU
    Vps_Ths7360SfCtrl   thsCtrl;
#endif

    appObj = &AppDispObjs;

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

    /* Get the test parameters from the user */
    appObj->mosaicEnable = MOSAIC_ENABLE;

    App_dispGetInputFromUser(appObj);

#ifndef PLATFORM_ZEBU
    /* Configure THS filter for component output */
    if ((VPS_DC_VENC_HDCOMP == appObj->vencId) &&
        (VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId))
    {
        switch (appObj->modeInfo.standard)
        {
            case FVID2_STD_480P:
            case FVID2_STD_576P:
                thsCtrl = VPS_THS7360_SF_ED_MODE;
                break;

            case FVID2_STD_720P_60:
            case FVID2_STD_720P_50:
            case FVID2_STD_1080I_60:
            case FVID2_STD_1080I_50:
            case FVID2_STD_1080P_30:
                thsCtrl = VPS_THS7360_SF_HD_MODE;
                break;

            default:
            case FVID2_STD_1080P_60:
            case FVID2_STD_1080P_50:
                thsCtrl = VPS_THS7360_SF_TRUE_HD_MODE;
                break;
        }
        retVal = Vps_ths7360SetSfParams(thsCtrl);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }

    /* Configure THS filter for SD output */
    if ((VPS_DC_VENC_SD == appObj->vencId) &&
        (VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId))
    {
        /* Enable SD filter in the THS7360 */
        retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_ENABLE_MODULE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }
#endif

    /* Allocate input buffer */
    appObj->inBuf = VpsUtils_memAlloc(
                        (TOTAL_NUM_BUFFERS * appObj->inFrmSize),
                        VPS_BUFFER_ALIGNMENT);
    if (NULL == appObj->inBuf)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* Configure pixel clock */
    retVal = App_dispSetPixClk(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Run the test */
    retVal = App_dispMosaicTest(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Free the input buffer */
    VpsUtils_memFree(appObj->inBuf, (TOTAL_NUM_BUFFERS * appObj->inFrmSize));

#ifndef PLATFORM_ZEBU
    if ((VPS_DC_VENC_HDCOMP == appObj->vencId) &&
        (VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId))
    {
        thsCtrl = VPS_THS7360_DISABLE_SF;
        retVal = Vps_ths7360SetSfParams(thsCtrl);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }

    if ((VPS_DC_VENC_SD == appObj->vencId) &&
        (VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId))
    {
        /* Disable SD filter in the THS7360 */
        retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_DISABLE_MODULE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }
#endif

    Vps_printf("%s: Test Successful!!\n", APP_NAME);

    /* Test complete. Signal it */
    Semaphore_post(AppTestCompleteSem1);
}


/**
 *  App_dispMosaicTest
 *  Mosaic display test function.
 */
static Int32 App_dispMosaicTest(App_DisplayObj *appObj)
{
    Int32                   retVal = FVID2_SOK, driverRetVal;
    UInt32                  frmListCnt;
    FVID2_Format            fmt;
    FVID2_FrameList        *fList;
    FVID2_CbParams          cbParams;
    Vps_DcCreateConfig      dcCreateCfg;
    Vps_DcEdeConfig         dcEdeCfg;
    Vps_CprocConfig         cprocCfg;
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
        return FVID2_EFAIL;
    }

    /* Configure display controller - Set the paths */
    retVal = App_dispConfigDctrl(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    if (Vps_platformIsEvm() && (VPS_DC_VENC_DVO2 == appObj->vencId))
    {
        /* Config HDMI */
        retVal = App_dispConfigHdmi(appObj);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    /* Open the driver */
    cbParams.cbFxn = App_dispCbFxn;
    cbParams.errCbFxn = App_dispErrCbFxn;
    cbParams.errList = &appObj->errFrameList;
    cbParams.appData = appObj;
    cbParams.reserved = NULL;
    appObj->fvidHandle = FVID2_create(
                             FVID2_VPS_DISP_DRV,
                             appObj->driverInst,
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
    retVal = FVID2_start(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    if (Vps_platformIsEvm() && (VPS_DC_VENC_DVO2 == appObj->vencId))
    {
        retVal = FVID2_start(appObj->hdmiHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

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

    if (Vps_platformIsEvm() && (VPS_DC_VENC_DVO2 == appObj->vencId))
    {
        /* Stop HDMI transmitter */
        retVal = FVID2_stop(appObj->hdmiHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    /* Stop driver */
    retVal = FVID2_stop(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    Vps_printf("%s: Display Driver %d Stopped!!\n",
        APP_NAME, appObj->driverInst);

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

    /* Close driver */
    retVal = FVID2_delete(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Configure display controller - Remove the settings */
    retVal = FVID2_control(
                 appObj->dcHandle,
                 IOCTL_VPS_DCTRL_CLEAR_CONFIG,
                 &AppDispDcConfig,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    if (Vps_platformIsEvm() && (VPS_DC_VENC_DVO2 == appObj->vencId))
    {
        /* Close HDMI transmitter */
        retVal = FVID2_delete(appObj->hdmiHandle, NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    /* Close Display Controller */
    retVal = FVID2_delete(appObj->dcHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Free allocated handles */
    App_dispFreeVariables(appObj);

    Vps_printf("%s: Mosaic Display %d Test Successful!!\n",
        APP_NAME, appObj->driverInst);

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
        if (index >= TOTAL_NUM_BUFFERS)
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
                (void *) (appObj->inBuf + (index * appObj->inFrmSize));
            frame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                ((UInt8 *) frame->addr[FVID2_FRAME_ADDR_IDX]
                                      [FVID2_YUV_SP_Y_ADDR_IDX])
                    + (appObj->pitch * BUFFER_HEIGHT);
        }
        else
        {
            frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                (void *) (appObj->inBuf + (index * appObj->inFrmSize));
            frame->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX] =
                (void *)
                (appObj->inBuf + (index * appObj->inFrmSize) + appObj->pitch);
            frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                ((UInt8 *) frame->addr[FVID2_FIELD_EVEN_ADDR_IDX]
                                      [FVID2_YUV_SP_Y_ADDR_IDX])
                    + (appObj->pitch * BUFFER_HEIGHT);
            frame->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                ((UInt8 *) frame->addr[FVID2_FIELD_ODD_ADDR_IDX]
                                      [FVID2_YUV_SP_Y_ADDR_IDX])
                    + (appObj->pitch * BUFFER_HEIGHT);
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
    if (appObj->bufferIndex >= TOTAL_NUM_BUFFERS)
    {
        appObj->bufferIndex %= TOTAL_NUM_BUFFERS;
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
 *  App_dispConfigDctrl
 *  Configures the display controller to connect the paths, enabling muxes
 *  and configuring blenders and VENCs.
 */
static Int32 App_dispConfigDctrl(const App_DisplayObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  edge;
    Vps_DcConfig           *dctrlCfg = &AppDispDcConfig;
    Vps_DcOutputInfo        dcOutputInfo;
    Vps_DcVencClkSrc        clkSrc;

    /* Update edge info */
    edge = 0u;
    dctrlCfg->useCase = VPS_DC_USERSETTINGS;
    if (VPS_DISP_INST_BP0 == appObj->driverInst)
    {
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_BP0_INPUT_PATH;
    }
    else if (VPS_DISP_INST_BP1 == appObj->driverInst)
    {
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_BP1_INPUT_PATH;
    }
    else if (VPS_DISP_INST_MAIN_DEIH_SC1 == appObj->driverInst)
    {
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_MAIN_INPUT_PATH;
    }
    else if (VPS_DISP_INST_AUX_DEI_SC2 == appObj->driverInst)
    {
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_AUX_INPUT_PATH;
    }
    else if (VPS_DISP_INST_MAIN_DEI_SC1 == appObj->driverInst)
    {
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_MAIN_INPUT_PATH;
    }
    else if (VPS_DISP_INST_AUX_SC2 == appObj->driverInst)
    {
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_AUX_INPUT_PATH;
    }

    if (VPS_DC_VENC_SD != appObj->vencId)
    {
        if ((VPS_DISP_INST_MAIN_DEIH_SC1 == appObj->driverInst) ||
            (VPS_DISP_INST_MAIN_DEI_SC1 == appObj->driverInst))
        {
            dctrlCfg->edgeInfo[edge].endNode = VPS_DC_VCOMP;
            edge++;
        }
        else
        {
            dctrlCfg->edgeInfo[edge].endNode = VPS_DC_VCOMP_MUX;
            edge++;
            dctrlCfg->edgeInfo[edge].startNode = VPS_DC_VCOMP_MUX;
            dctrlCfg->edgeInfo[edge].endNode = VPS_DC_VCOMP;
            edge++;
        }
    }

    if (VPS_DC_VENC_HDMI == appObj->vencId)
    {
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_CIG_NON_CONSTRAINED_OUTPUT;
        dctrlCfg->edgeInfo[edge].endNode = VPS_DC_HDMI_BLEND;
        edge++;
    }
    else if (VPS_DC_VENC_HDCOMP == appObj->vencId)
    {
        /* In TI816x, CIG output goes to HDCOMP and in other platforms CIG
         * output goes to DVO2 */
        if ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
            (VPS_PLATFORM_ID_SIM_TI816x == appObj->platformId))
        {
            dctrlCfg->edgeInfo[edge].startNode = VPS_DC_CIG_CONSTRAINED_OUTPUT;
        }
        else
        {
            dctrlCfg->edgeInfo[edge].startNode =
                VPS_DC_CIG_NON_CONSTRAINED_OUTPUT;
        }
        dctrlCfg->edgeInfo[edge].endNode = VPS_DC_HDCOMP_BLEND;
        edge++;
    }
    else if (VPS_DC_VENC_DVO2 == appObj->vencId)
    {
        /* In TI816x, CIG output goes to HDCOMP and in other platforms CIG
         * output goes to DVO2 */
        if ((VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId) ||
            (VPS_PLATFORM_ID_SIM_TI816x == appObj->platformId))
        {
            dctrlCfg->edgeInfo[edge].startNode =
                VPS_DC_CIG_NON_CONSTRAINED_OUTPUT;
        }
        else
        {
            dctrlCfg->edgeInfo[edge].startNode = VPS_DC_CIG_CONSTRAINED_OUTPUT;
        }
        dctrlCfg->edgeInfo[edge].endNode = VPS_DC_DVO2_BLEND;
        edge++;
    }
    else
    {
        dctrlCfg->edgeInfo[edge].endNode = VPS_DC_SDVENC_MUX;
        edge++;
        dctrlCfg->edgeInfo[edge].startNode = VPS_DC_SDVENC_MUX;
        dctrlCfg->edgeInfo[edge].endNode = VPS_DC_SDVENC_BLEND;
        edge++;
    }
    dctrlCfg->numEdges = edge;

    /* Update VENC info */
    dctrlCfg->vencInfo.modeInfo[0u].vencId = appObj->vencId;
    dctrlCfg->vencInfo.modeInfo[0u].mInfo.standard = appObj->modeInfo.standard;
    dctrlCfg->vencInfo.tiedVencs = 0u;
    dctrlCfg->vencInfo.numVencs = 1u;

    /* Set the output format */
    dcOutputInfo.dvoFidPolarity = VPS_DC_POLARITY_ACT_HIGH;
    dcOutputInfo.dvoVsPolarity = VPS_DC_POLARITY_ACT_HIGH;
    dcOutputInfo.dvoHsPolarity = VPS_DC_POLARITY_ACT_HIGH;
    dcOutputInfo.dvoActVidPolarity = VPS_DC_POLARITY_ACT_HIGH;
    if (VPS_DC_VENC_DVO2 == appObj->vencId)
    {
        dcOutputInfo.vencNodeNum = VPS_DC_VENC_DVO2;
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
        if (VPS_PLATFORM_BOARD_VC == appObj->boardId)
        {
            dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
            dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
        }
        else
        {
            dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_DOUBLECHAN;
            dcOutputInfo.dataFormat = FVID2_DF_YUV422SP_UV;
        }
    }
    else if (VPS_DC_VENC_HDCOMP == appObj->vencId)
    {
        dcOutputInfo.vencNodeNum = VPS_DC_VENC_HDCOMP;
        dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC;
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPONENT;
        dcOutputInfo.dataFormat = FVID2_DF_YUV444P;
    }
    else if (VPS_DC_VENC_HDMI == appObj->vencId)
    {
        dcOutputInfo.vencNodeNum = VPS_DC_VENC_HDMI;
        dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
        dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
    }
    else
    {
        dcOutputInfo.vencNodeNum = VPS_DC_VENC_SD;
        dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
        if (VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId)
        {
            dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
        }
        else
        {
            /* For 814x, sVideo is the default. Composite video out requires
               base board modification. */
            dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_SVIDEO;
        }
        dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
    }
    retVal = FVID2_control(
                 appObj->dcHandle,
                 IOCTL_VPS_DCTRL_SET_VENC_OUTPUT,
                 &dcOutputInfo,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Display Controller set output failed for %d!!\n",
            APP_NAME, appObj->driverInst);
        return (retVal);
    }

    if (VPS_DC_VENC_SD != appObj->vencId)
    {
        /* Set the Clock source for DVO2 */
        if (VPS_DC_VENC_HDCOMP == appObj->vencId)
        {
            clkSrc.venc = VPS_DC_VENC_HDCOMP;
            clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
        }
        else
        {
            if (VPS_DC_VENC_DVO2 == appObj->vencId)
            {
                clkSrc.venc = VPS_DC_VENC_DVO2;
            }
            else
            {
                clkSrc.venc = VPS_DC_VENC_HDMI;
            }
            clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
            if ((appObj->platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (appObj->cpuRev == VPS_PLATFORM_CPU_REV_1_0))
            {
                clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD_DIV2;
            }

            /* Clock Source is not selectable in TI814X. It is fixed to VENCD
               clock for HDMI and VENCA clock for DVO2. These clocks can
               only be divided by 2 within HDVPSS */
            if (appObj->platformId == VPS_PLATFORM_ID_EVM_TI814x)
            {
                if (VPS_DC_VENC_DVO2 == appObj->vencId)
                {
                    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
                }
                else
                {
                    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
                }
            }
        }

        retVal = FVID2_control(
                     appObj->dcHandle,
                     IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC,
                     &clkSrc,
                     NULL);
    }

    /* Use custom way of specifying mode for 720P mode */
    if (FVID2_STD_720P_60 == appObj->modeInfo.standard)
    {
        /* Set the standard to custom in modeinfo structure */
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.standard = FVID2_STD_CUSTOM;

        /* Set the Active Video Size and ScanFormat */
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.width = 1280;
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.height = 720;
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.scanFormat = FVID2_SF_PROGRESSIVE;

        /* Set the timing information of the mode */
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.hFrontPorch = 110;
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.hBackPorch = 220;
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.hSyncLen = 40;
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.vFrontPorch = 5;
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.vBackPorch = 20;
        dctrlCfg->vencInfo.modeInfo[0u].mInfo.vSyncLen = 5;
        dctrlCfg->vencInfo.modeInfo[0u].mode = 3;
    }

    /* Set the display controller configuration */
    retVal |= FVID2_control(
                  appObj->dcHandle,
                  IOCTL_VPS_DCTRL_SET_CONFIG,
                  dctrlCfg,
                  NULL);
    if (FVID2_SOK == retVal)
    {
        Vps_printf("%s: Display Controller Path configured for %d!!\n",
            APP_NAME, appObj->driverInst);
    }
    else
    {
        Vps_printf("%s: Display Controller Path configuration failed!!\n",
            APP_NAME);
    }

    return (retVal);
}



/**
 *  App_dispSetPixClk
 *  Configure Pixel Clock.
 */
static Int32 App_dispSetPixClk(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    FVID2_Handle        systemDrvHandle;
    Vps_SystemVPllClk   vpllCfg;

    systemDrvHandle = FVID2_create(
                          FVID2_VPS_VID_SYSTEM_DRV,
                          0u,
                          NULL,
                          NULL,
                          NULL);
    if (NULL == systemDrvHandle)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (FVID2_EFAIL);
    }

    if (VPS_DC_VENC_SD == appObj->vencId)
    {
        vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_RF;
        if (VPS_PLATFORM_ID_EVM_TI816x == appObj->platformId)
        {
            vpllCfg.outputClk = 216000u;
        }
        else
        {
            vpllCfg.outputClk = 54000u;
        }
    }
    else if (VPS_DC_VENC_HDCOMP == appObj->vencId)
    {
        vpllCfg.outputClk = appObj->modeInfo.pixelClock;
        vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_A;
    }
    else
    {
        vpllCfg.outputClk = appObj->modeInfo.pixelClock;
        vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;

        /* Revision 1.0 of TI816x require twice the pixel clock, as the
         * divider is not selectable */
        if ((appObj->platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
            (appObj->cpuRev == VPS_PLATFORM_CPU_REV_1_0))
        {
            vpllCfg.outputClk *= 2u;
        }

        /* Clock Source is not selectable in TI814X. It is fixed to HDMI
         * clock for HDMI and VENCD clock for DVO2. These clocks can
         * only be divided by 2 within HDVPSS */
        if (appObj->platformId == VPS_PLATFORM_ID_EVM_TI814x)
        {
            if (VPS_DC_VENC_DVO2 == appObj->vencId)
            {
                vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;
            }
            else
            {
                /* HDMI needs 10x clock */
                vpllCfg.outputClk *= 10u;
                vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_HDMI;
            }
        }
    }

    retVal = FVID2_control(
                 systemDrvHandle,
                 IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                 &vpllCfg,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    retVal = FVID2_delete(systemDrvHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
    }

    return (retVal);
}



/**
 *  App_dispSetFormat
 *  Sets the required/default format.
 */
static Void App_dispSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt)
{
    fmt->channelNum = 0u;
    fmt->width = appObj->width;
    fmt->height = appObj->height;
    fmt->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->pitch;
    fmt->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->pitch;
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
    fmt->dataFormat = appObj->dataFormat;
    if (FVID2_DF_YUV420SP_UV == appObj->dataFormat)
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
 *  App_dispConfigHdmi
 *  Configures the display controller to connect the paths, enabling muxes
 *  and configuring blenders and VENCs.
 */
static Int32 App_dispConfigHdmi(App_DisplayObj *appObj)
{
    Int32                           retVal;
    Vps_HdmiChipId                  hdmiId;
    Vps_SiI9022aHpdPrms             hpdPrms;
    Vps_SiI9022aModeParams          modePrms;
    Vps_VideoEncoderCreateParams    encCreateParams;
    Vps_VideoEncoderCreateStatus    encCreateStatus;

    /* Open HDMI Tx */
    encCreateParams.deviceI2cInstId = Vps_platformGetI2cInstId();
    encCreateParams.deviceI2cAddr =
            Vps_platformGetVidEncI2cAddr(FVID2_VPS_VID_ENC_SII9022A_DRV);
    encCreateParams.inpClk = 0u;
    encCreateParams.hdmiHotPlugGpioIntrLine = 0u;
    encCreateParams.clkEdge = FALSE;
    if (VPS_PLATFORM_BOARD_VC == appObj->boardId)
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EXTERNAL_SYNC;
    }
    else
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EMBEDDED_SYNC;
    }

    appObj->hdmiHandle = FVID2_create(
                             FVID2_VPS_VID_ENC_SII9022A_DRV,
                             0u,
                             &encCreateParams,
                             &encCreateStatus,
                             NULL);
    if (NULL == appObj->hdmiHandle)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }

    retVal = FVID2_control(
                 appObj->hdmiHandle,
                 IOCTL_VPS_SII9022A_GET_DETAILED_CHIP_ID,
                 &hdmiId,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Could not get detailed chip ID\n", APP_NAME);
        return (retVal);
    }

    retVal = FVID2_control(
                 appObj->hdmiHandle,
                 IOCTL_VPS_SII9022A_QUERY_HPD,
                 &hpdPrms,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Could not detect HPD\n", APP_NAME);
        return (retVal);
    }

    modePrms.standard = appObj->modeInfo.standard;
    retVal = FVID2_control(
                 appObj->hdmiHandle,
                 IOCTL_VPS_VIDEO_ENCODER_SET_MODE,
                 &modePrms,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Could not set mode\n", APP_NAME);
    }

    return (retVal);
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
 *  App_dispGetInputFromUser
 *  Get the test parameters from the user.
 */
static void App_dispGetInputFromUser(App_DisplayObj *appObj)
{
    Int32       retVal;
    UInt32      standard;

    appObj->driverInst = DRIVER_INSTANCE;
    Vps_printf("%s: Display Instance Selected: %d\n",
        APP_NAME, appObj->driverInst);

    /* Hard coding Venc to off-chip HDMI */
    appObj->vencId = VPS_DC_VENC_DVO2;

    /* Hard coding output format to 1080p60 */
    standard = FVID2_STD_1080P_60;

    if (0xFFFFFFFFu != standard)
    {
        /* Get other information for the selected standard */
        appObj->modeInfo.standard = standard;
        retVal = FVID2_getModeInfo(&appObj->modeInfo);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Invalid display mode %d selected\n",
                APP_NAME, standard);
            standard = 0xFFFFFFFFu;
        }
    }

    /* Initialize test parameters */
    appObj->width = BUFFER_WIDTH;
    appObj->height = BUFFER_HEIGHT;
    if (appObj->width > appObj->modeInfo.width)
    {
        appObj->width = appObj->modeInfo.width;
    }
    if (appObj->height > appObj->modeInfo.height)
    {
        appObj->height = appObj->modeInfo.height;
    }
    appObj->dataFormat = DATA_FORMAT;
    if (FVID2_DF_YUV422SP_UV == appObj->dataFormat)
    {
        appObj->pitch = BUFFER_WIDTH;
        appObj->inFrmSize = appObj->pitch * BUFFER_HEIGHT * 2u;
    }
    else if (FVID2_DF_YUV420SP_UV == appObj->dataFormat)
    {
        appObj->pitch = BUFFER_WIDTH;
        appObj->inFrmSize = appObj->pitch * BUFFER_HEIGHT * 3u / 2u;
    }
    else
    {
        appObj->pitch = BUFFER_WIDTH * 2u;
        appObj->inFrmSize = appObj->pitch * BUFFER_HEIGHT;
    }

    if (TRUE == appObj->mosaicEnable)
    {
        /* Get the layouts from table */
        App_dispGetLayoutsFromTable(appObj);
    }

    return;
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
             (FVID2_STD_576P == appObj->modeInfo.standard) ||
             (FVID2_STD_NTSC == appObj->modeInfo.standard) ||
             (FVID2_STD_PAL == appObj->modeInfo.standard))
    {
        numLayouts = (sizeof (gAppDispLayoutTbl_D1)) /
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
                 (FVID2_STD_576P == appObj->modeInfo.standard) ||
                 (FVID2_STD_NTSC == appObj->modeInfo.standard) ||
                 (FVID2_STD_PAL == appObj->modeInfo.standard))
        {
            mObjTbl = &gAppDispLayoutTbl_D1[layoutCnt];
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
            winFmt->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = appObj->pitch;
            winFmt->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = appObj->pitch;
            winFmt->dataFormat = appObj->dataFormat;
            winFmt->bpp = FVID2_BPP_BITS16;
            if (FVID2_DF_YUV420SP_UV == appObj->dataFormat)
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

