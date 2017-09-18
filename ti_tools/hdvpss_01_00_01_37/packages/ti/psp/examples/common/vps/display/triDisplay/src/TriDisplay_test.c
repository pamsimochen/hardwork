/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file TriDisplay_test.c
 *
 *  \brief Triple display example file.
 *
 *  This example illustrates the triple display use case.
 *  This example displays video through BP0 path on HDMI VENC, a second video
 *  through BP1 path on HDCOMP VENC, a thrid SD video through SEC1 path on
 *  SD VENC.
 *  Along with the video, TI logo is also blended at the top of these videos
 *  using GRPX 0, 1 and 2 paths.
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
#include <ti/sysbios/heaps/HeapBuf.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_app.h>
#include <ti/psp/examples/common/vps/display/triDisplay/src/TriDisplay.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (10u * 1024u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void App_dispTestTask(UArg arg0, UArg arg1);
static Void App_dispVideoTestTask(UArg arg0, UArg arg1);
static Int32 App_dispVideoTest(App_DisplayObj *appObj);
static Int32 App_dispCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
static Int32 App_dispErrCbFxn(FVID2_Handle handle,
                              Ptr appData,
                              Void *errList,
                              Ptr reserved);
static Void App_dispUpdateFrmList(App_DisplayObj *appObj,
                                  FVID2_FrameList *fList);
static Void App_dispSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt);
static Int32 App_dispConfigMosaic(App_DisplayObj *appObj);
static Int32 App_dispInitVariables(App_DisplayObj *appObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Display test task stack. */
#pragma DATA_ALIGN(AppDisplayTaskStack, 32)
#pragma DATA_SECTION(AppDisplayTaskStack, ".bss:taskStackSection")
static UInt8 AppDisplayTaskStack[APP_TSK_STACK_SIZE];

/* Video display test task stack. */
#pragma DATA_ALIGN(AppVideoDisplayTaskStack, 32)
#pragma DATA_SECTION(AppVideoDisplayTaskStack, ".bss:taskStackSection")
static UInt8 AppVideoDisplayTaskStack[NUM_VIDEO_DISPLAYS][APP_TSK_STACK_SIZE];

/* Display application object per video instance. */
static App_DisplayObj AppDispVideoObjs[NUM_VIDEO_DISPLAYS];

/* Video test complete semaphore used to sync between display and video task. */
static Semaphore_Handle VideoTestCompleteSem;

extern Semaphore_Handle AppTestCompleteSem;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  App_createDispTestTsk
 *  Display application create task function.
 */
Task_Handle App_createDispTestTsk(App_DispComboObj *combObj)
{
    Task_Params tskParams;
    Task_Handle tskHandle;

    Task_Params_init(&tskParams);
    tskParams.stack = AppDisplayTaskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    tskParams.arg0 = (UArg)combObj;
    tskHandle = Task_create((Task_FuncPtr) App_dispTestTask, &tskParams, NULL);

    return (tskHandle);
}



/**
 *  App_dispTestTask
 *  Tri display application test task.
 */
static Void App_dispTestTask(UArg arg0, UArg arg1)
{
    char                ch;
    UInt32              dispCnt;
    UInt8              *inBuf;
    App_DispComboObj   *comboObj;
    App_DisplayObj     *appObj;
    Semaphore_Params    semPrms;
    Task_Params         tskParams[NUM_VIDEO_DISPLAYS];
    Task_Handle         tskHandle[NUM_VIDEO_DISPLAYS];

    comboObj = (App_DispComboObj *)arg0;

    /* Create semaphore */
    Semaphore_Params_init(&semPrms);
    VideoTestCompleteSem = Semaphore_create(0, &semPrms, NULL);
    if (NULL == VideoTestCompleteSem)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* Allocate input buffer */
    inBuf =  VpsUtils_memAlloc(
                 (TOTAL_NUM_BUFFERS * BUFFER_SIZE),
                 VPS_BUFFER_ALIGNMENT);
    if (NULL == inBuf)
    {
        Vps_printf(
            "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* Wait for the user to load the image */
    Vps_printf("Load %d %s video frames of size %dx%d to location: 0x%p\n",
        TOTAL_NUM_BUFFERS, FVID2_getDataFmtString(FVID2_DF_YUV422I_YUYV),
        BUFFER_WIDTH, BUFFER_HEIGHT, inBuf);
    Vps_printf("Enter any key after loading...\n");
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    /* Initialize the test parameters */
    for (dispCnt = 0u; dispCnt < NUM_VIDEO_DISPLAYS; dispCnt++)
    {
        appObj = &AppDispVideoObjs[dispCnt];
        appObj->driverId = FVID2_VPS_DISP_DRV;
        appObj->driverInst = VPS_DISP_INST_BP0 + dispCnt;
        appObj->inBuf = inBuf;
        appObj->inFrmSize = BUFFER_SIZE;
        appObj->numInBuf = TOTAL_NUM_BUFFERS;
        if (VPS_DISP_INST_SEC1 == appObj->driverInst)
        {
            appObj->frameWidth = SD_FRAME_WIDTH;
            appObj->frameHeight = SD_FRAME_HEIGHT;
            appObj->windowWidth = SD_WINDOW_WIDTH;
            appObj->windowHeight = SD_WINDOW_HEIGHT;
            appObj->scanFormat = SD_SCAN_FORMAT;
            appObj->mosaicInfo.numCol = SD_NUM_MOSAIC_COL;
            appObj->mosaicInfo.numRow = SD_NUM_MOSAIC_ROW;
            appObj->mosaicInfo.numWindows =
                appObj->mosaicInfo.numCol * appObj->mosaicInfo.numRow;
        }
        else
        {
            appObj->frameWidth = HD_FRAME_WIDTH;
            appObj->frameHeight = HD_FRAME_HEIGHT;
            appObj->windowWidth = HD_WINDOW_WIDTH;
            appObj->windowHeight = HD_WINDOW_HEIGHT;
            appObj->scanFormat = HD_SCAN_FORMAT;
            appObj->mosaicInfo.numCol = HD_NUM_MOSAIC_COL;
            appObj->mosaicInfo.numRow = HD_NUM_MOSAIC_ROW;
            appObj->mosaicInfo.numWindows =
                appObj->mosaicInfo.numCol * appObj->mosaicInfo.numRow;
        }
    }

    /* Create separate task for each display */
    for (dispCnt = 0u; dispCnt < NUM_VIDEO_DISPLAYS; dispCnt++)
    {
        if (FALSE == comboObj->isDispActive[dispCnt])
        {
            continue;
        }
        Task_Params_init(&tskParams[dispCnt]);
        tskParams[dispCnt].stack = &AppVideoDisplayTaskStack[dispCnt][0u];
        tskParams[dispCnt].stackSize = APP_TSK_STACK_SIZE;
        tskParams[dispCnt].arg0 = (UArg) &AppDispVideoObjs[dispCnt];
        tskHandle[dispCnt] = Task_create(
                                 (Task_FuncPtr) App_dispVideoTestTask,
                                 &tskParams[dispCnt],
                                 NULL);
    }

    /* Start GRPX planes */
    App_dispInitGrpx(comboObj);

    /* Wait for video display tasks to complete */
    for (dispCnt = 0u; dispCnt < NUM_VIDEO_DISPLAYS; dispCnt++)
    {
        if (FALSE == comboObj->isDispActive[dispCnt])
        {
            continue;
        }
        Semaphore_pend(VideoTestCompleteSem, BIOS_WAIT_FOREVER);
    }

    /* Stop GRPX planes */
    App_dispDeInitGrpx(comboObj);

    /* Delete the tasks*/
    for (dispCnt = 0u; dispCnt < NUM_VIDEO_DISPLAYS; dispCnt++)
    {
        if (FALSE == comboObj->isDispActive[dispCnt])
        {
            continue;
        }
        Task_delete(&tskHandle[dispCnt]);
    }

    /* Free the input buffer */
    VpsUtils_memFree(inBuf, (TOTAL_NUM_BUFFERS * BUFFER_SIZE));
    Semaphore_delete(&VideoTestCompleteSem);

    Vps_printf("%s: Test Successful!!\n", APP_NAME);

    /* Test complete. Signal it */
    Semaphore_post(AppTestCompleteSem);
}



/**
 *  App_dispVideoTestTask
 *  Video display test task.
 */
static Void App_dispVideoTestTask(UArg arg0, UArg arg1)
{
    Int32           retVal;
    App_DisplayObj *appObj;

    /* Run the test */
    appObj = (App_DisplayObj *) arg0;
    retVal = App_dispVideoTest(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Test complete. Signal it */
    Semaphore_post(VideoTestCompleteSem);
}



/**
 *  App_dispVideoTest
 *  Video display test function.
 */
static Int32 App_dispVideoTest(App_DisplayObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  fListCnt;
    FVID2_Format            fmt;
    FVID2_FrameList        *fList;
    FVID2_CbParams          cbParams;
    Vps_DispCreateParams    createPrms =
    {
        VPS_VPDMA_MT_NONTILEDMEM,
        FALSE
    };

    /* Initialize and allocate handles */
    retVal = App_dispInitVariables(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Open driver */
    cbParams.cbFxn = App_dispCbFxn;
    cbParams.errCbFxn = App_dispErrCbFxn;
    cbParams.errList = &appObj->errFrameList;
    cbParams.appData = appObj;
    cbParams.reserved = NULL;
    appObj->fvidHandle = FVID2_create(
                             appObj->driverId,
                             appObj->driverInst,
                             &createPrms,
                             NULL,
                             &cbParams);
    if (NULL == appObj->fvidHandle)
    {
        Vps_printf(
            "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }
    Vps_printf("%s: Display Driver Opened for Instance %d!!\n",
        APP_NAME, appObj->driverInst);

    /* Set the required format */
    App_dispSetFormat(appObj, &fmt);
    retVal = FVID2_setFormat(appObj->fvidHandle, &fmt);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    if (appObj->mosaicInfo.numWindows > 1u)
    {
        /* Create and select mosaic layouts */
        retVal = App_dispConfigMosaic(appObj);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
    }

    /* Prime Buffers */
    for (fListCnt = 0u; fListCnt < NUM_PRIMING; fListCnt++)
    {
        /* Update the buffer address */
        fList = &appObj->frameList[fListCnt];
        App_dispUpdateFrmList(appObj, fList);

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
        APP_NAME, appObj->driverInst);
    retVal = FVID2_start(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Use the same frame list for all request */
    fList = &appObj->frameList[0u];
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

        /* Update the buffer address */
        App_dispUpdateFrmList(appObj, fList);

        /* Queue the updated frame list back to the driver */
        retVal = FVID2_queue(appObj->fvidHandle, fList, 0u);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            break;
        }

        appObj->completedFrames++;
        if (appObj->completedFrames >= TOTAL_LOOP_COUNT)
        {
            break;
        }
    }

    /* Stop driver */
    retVal = FVID2_stop(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    Vps_printf("%s: Display Driver %d Stopped!!\n",
        APP_NAME, appObj->driverInst);

    /* Dequeue all the driver held buffers */
    fList = &appObj->frameList[0u];
    while (1)
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
    }

    /* Print the display status */
    VpsUtils_appPrintDispInstStatus(appObj->fvidHandle);

    /* Close driver */
    retVal = FVID2_delete(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    Semaphore_delete(&appObj->readtyToDqSem);

    Vps_printf("%s: Video Display %d Test Successful!!\n",
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
 *  App_dispUpdateFrmList
 *  Updates the frame list with the next request buffer information.
 */
static Void App_dispUpdateFrmList(App_DisplayObj *appObj,
                                  FVID2_FrameList *fList)
{
    UInt32          index, frmCnt;
    FVID2_Frame    *frame;

    /* Fill the next set of buffers */
    fList->numFrames = appObj->mosaicInfo.numWindows;
    for (frmCnt = 0u; frmCnt < appObj->mosaicInfo.numWindows; frmCnt++)
    {
        if (appObj->numInBuf == appObj->bufferIndex)
        {
            appObj->bufferIndex = 0u;
        }
        index = appObj->bufferIndex;

        frame = fList->frames[frmCnt];
        if (FVID2_SF_PROGRESSIVE == appObj->scanFormat)
        {
            frame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                (void *) (appObj->inBuf + (index * appObj->inFrmSize));
        }
        else
        {
            frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                (void *) (appObj->inBuf + (index * appObj->inFrmSize));
            frame->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX] =
                (void *)
                (appObj->inBuf + (index * appObj->inFrmSize) + BUFFER_PITCH);
        }
        appObj->bufferIndex++;
    }

    appObj->submittedFrames++;

    return;
}



/**
 *  App_dispSetFormat
 *  Sets the required/default format.
 */
static Void App_dispSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt)
{
    fmt->channelNum = 0u;
    fmt->width = appObj->windowWidth;
    fmt->height = appObj->windowHeight;
    fmt->pitch[FVID2_YUV_INT_ADDR_IDX] = BUFFER_PITCH;
    fmt->scanFormat = appObj->scanFormat;
    if (FVID2_SF_INTERLACED == appObj->scanFormat)
    {
        fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = TRUE;
    }
    else
    {
        fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    }
    fmt->dataFormat = FVID2_DF_YUV422I_YUYV;
    fmt->bpp = FVID2_BPP_BITS16;
    fmt->reserved = NULL;

    return;
}



/**
 *  App_dispConfigMosaic
 *  Creates the mosaic layouts and selects it.
 *  This function places the mosaic window in the center of the frame at equal
 *  distance to each other.
 */
static Int32 App_dispConfigMosaic(App_DisplayObj *appObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  row, col;
    UInt32                  hGap, vGap;
    UInt32                  winStartX, winStartY;
    App_MosaicObj          *mObj;
    Vps_WinFormat          *winFmt;
    Vps_MultiWinParams      multiWinPrms;
    Vps_WinFormat           mosaicWinFmt[MAX_MOSAIC_FRAMES];

    mObj = &appObj->mosaicInfo;

    /* Calculate the window offsets */
    winFmt = &mosaicWinFmt[0u];
    vGap = (appObj->frameHeight - (mObj->numRow * appObj->windowHeight))
        / (2u * mObj->numRow);
    winStartY = 0u;
    for (row = 0u; row < mObj->numRow; row++)
    {
        winStartY += vGap;
        winStartX = 0u;
        hGap = (appObj->frameWidth - (mObj->numCol * appObj->windowWidth))
            / (2u * mObj->numCol);
        for (col = 0u; col < mObj->numCol; col++)
        {
            winStartX += hGap;
            winFmt->winStartX = winStartX;
            winFmt->winStartY = winStartY;
            winFmt->winWidth = appObj->windowWidth;
            winFmt->winHeight = appObj->windowHeight;
            winFmt->pitch[FVID2_YUV_INT_ADDR_IDX] = BUFFER_PITCH;
            winFmt->dataFormat = FVID2_DF_YUV422I_YUYV;
            winFmt->bpp = FVID2_BPP_BITS16;
            winFmt->priority = 0u;

            winStartX += (hGap + appObj->windowWidth);
            winFmt++;
        }
        winStartY += (vGap + appObj->windowHeight);
    }

    /* Create the layout by calling driver API */
    multiWinPrms.channelNum = 0u;
    multiWinPrms.numWindows = mObj->numWindows;
    multiWinPrms.winFmt = &mosaicWinFmt[0u];
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

    /* Select a mosaic layout */
    retVal = FVID2_control(
                 appObj->fvidHandle,
                 IOCTL_VPS_SELECT_LAYOUT,
                 &mObj->layoutId,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
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
    UInt32              fListCnt, frmCnt;
    FVID2_Frame        *frame;
    FVID2_FrameList    *fList;
    Semaphore_Params    semPrms;

    Semaphore_Params_init(&semPrms);
    appObj->readtyToDqSem = Semaphore_create(0, &semPrms, NULL);
    if (NULL == appObj->readtyToDqSem)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EALLOC);
    }

    appObj->bufferIndex = 0u;
    appObj->submittedFrames = 0u;
    appObj->completedFrames = 0u;

    /* Init frame list */
    for (fListCnt = 0u; fListCnt < NUM_FRAMELIST; fListCnt++)
    {
        fList = &appObj->frameList[fListCnt];
        fList->numFrames = appObj->mosaicInfo.numWindows;
        fList->perListCfg = NULL;
        fList->reserved = NULL;
        for (frmCnt = 0u; frmCnt < appObj->mosaicInfo.numWindows; frmCnt++)
        {
            frame = &appObj->frames[fListCnt][frmCnt];
            fList->frames[frmCnt] = frame;
            frame->channelNum = 0u;
            frame->appData = NULL;
            frame->perFrameCfg = NULL;
            frame->subFrameInfo = NULL;
            frame->reserved = NULL;
            frame->fid = FVID2_FID_FRAME;
        }
    }

    /* Init error frame list */
    memset(&appObj->errFrameList, 0u, sizeof (appObj->errFrameList));

    return (retVal);
}
