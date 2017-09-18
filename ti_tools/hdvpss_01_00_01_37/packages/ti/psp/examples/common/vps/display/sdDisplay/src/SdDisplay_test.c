/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file SdDisplay_main.c
 *
 * \brief VPS SD display example file.
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

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/utility/vpsutils_app.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Driver instance to test. */
#define DRIVER_INSTANCE                 (VPS_DISP_INST_SEC1)

/* Input buffer parameters. Make sure that pitch is a multiple of 16 bytes. */
#define BUFFER_WIDTH_NTSC               (720u)
#define BUFFER_HEIGHT_NTSC              (480u)
#define BUFFER_WIDTH_PAL                (720u)
#define BUFFER_HEIGHT_PAL               (576u)

/* Number of loop count to run the application. */
#define TOTAL_LOOP_COUNT                (2000u)

/* Input frames will be increment only every this many frames. */
#define FRAME_SWITCH_RATE               (60u)   /* Every sec with 60 FPS */

/* Total number of buffers to allocate.
 * This should be the number of frames that should be loaded in to the
 * input buffer. */
#define TOTAL_NUM_BUFFERS               (10u)

/* Number of buffers to prime at the start. */
#define NUM_PRIMING                     (3u)

/* Number of FVID2 frame objects to allocate. */
#define NUM_FRAMES                      (NUM_PRIMING)

/* Application name string used in print statements. */
#define APP_NAME                        "SdDisplay"

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (10u * 1024u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Display application object.
 */
typedef struct
{
    UInt32                  driverInst;
    /**< Display driver instance. */
    FVID2_ModeInfo          modeInfo;
    /**< Information about the mode/standard. */
    UInt32                  vencId;
    /**< VENC ID to display. */

    UInt32                  width;
    /*< Buffer width. */
    UInt32                  height;
    /*< Buffer height. */
    UInt32                  pitch;
    /*< Buffer pitch. */
    UInt32                  dataFormat;
    /**< Data format - YUV420SP, YUV422I or YUV422SP. */

    UInt32                   videoFormat;
    /**< Video format - S-Video, COMPOSITE. */

    FVID2_Handle            fvidHandle;
    /**< Display driver handle. */
    FVID2_Handle            dcHandle;
    /**< Display controller handle. */

    FVID2_FrameList         frameList;
    /**< Frame list used for queue/dequeue operations. */
    FVID2_Frame             framePool[NUM_FRAMES];
    /**< Frame memory pool used for queue/dequeue operations. */
    FVID2_FrameList         errFrameList;
    /**< Frame list used for error operations. */

    UInt8                  *inBuf;
    /**< Input buffer used to load the input video frames. */
    UInt32                  inFrmSize;
    /**< Input frame size. */

    UInt32                  bufferIndex;
    /**< Current buffer index. Used to wrap around after TOTAL_NUM_BUFFERS
         iteration. */
    UInt32                  submittedFrames;
    /**< Total number of frames submitted to driver. */
    UInt32                  completedFrames;
    /**< Total number of completed frames. */
    Semaphore_Handle        readtyToDqSem;
    /**< Frame complete semaphore used to sync between callback and task. */

    Vps_PlatformId          platformId;
    /**< Platform identifier. */
    Vps_PlatformCpuRev      cpuRev;
    /**< CPU version identifier. */
    Vps_PlatformBoardId     boardId;
    /**< Board identifier. */
} App_DisplayObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void App_dispTestTask(void);
static Int32 App_dispSdTest(App_DisplayObj *appObj);
static Int32 App_dispCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
static Int32 App_dispErrCbFxn(FVID2_Handle handle,
                              Ptr appData,
                              Void *errList,
                              Ptr reserved);
static Void App_dispUpdateFrameList(App_DisplayObj *appObj,
                                    FVID2_FrameList *fList,
                                    FVID2_Frame *frame);
static Void App_dispSetFormat(App_DisplayObj *appObj, FVID2_Format *fmt);

static Int32 App_dispInitVariables(App_DisplayObj *appObj);
static Void App_dispFreeVariables(App_DisplayObj *appObj);

static void App_dispGetInputFromUser(App_DisplayObj *appObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppDispTestTaskStack, 32)
#pragma DATA_SECTION(AppDispTestTaskStack, ".bss:taskStackSection")
static UInt8 AppDispTestTaskStack[APP_TSK_STACK_SIZE];

/* Display application object per display instance. */
static App_DisplayObj AppDispObjs;

extern Semaphore_Handle AppTestCompleteSem;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  App_sdDispCreateTsk
 *  SD Display Application create task function.
 */
Task_Handle App_sdDispCreateTsk(void)
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
 *  SD Display application test task.
 */
static Void App_dispTestTask(void)
{
    char                ch;
    Int32               retVal, driverRetVal;
    App_DisplayObj     *appObj;

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
    App_dispGetInputFromUser(appObj);

    /* Allocate input buffer */
    appObj->inBuf = VpsUtils_memAlloc(
                        (TOTAL_NUM_BUFFERS * appObj->inFrmSize),
                        VPS_BUFFER_ALIGNMENT);
    if (NULL == appObj->inBuf)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* Wait for the user to load the image */
    Vps_printf("Load %d %s video frames of size %dx%d to location: 0x%p\n",
        TOTAL_NUM_BUFFERS, FVID2_getDataFmtString(appObj->dataFormat),
        appObj->width, appObj->height, appObj->inBuf);
    Vps_printf("Enter any key after loading...\n");
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    /* Configure pixel clock */
    retVal = VpsUtils_appSetVencPixClk(
                 appObj->vencId,
                 appObj->modeInfo.standard);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Open Display Controller */
    appObj->dcHandle = FVID2_create(
                           FVID2_VPS_DCTRL_DRV,
                           VPS_DCTRL_INST_0,
                           NULL,
                           &driverRetVal,
                           NULL);
    if (NULL == appObj->dcHandle)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, driverRetVal, __LINE__);
        return;
    }

    /* Start VENC */
    retVal = VpsUtils_appStartVenc(
                 appObj->dcHandle,
                 appObj->vencId,
                 appObj->modeInfo.standard,
                 (Vps_DcAnalogFmt) appObj->videoFormat);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Run the test */
    retVal = App_dispSdTest(appObj);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Stop VENC */
    retVal = VpsUtils_appStopVenc(
                 appObj->dcHandle,
                 appObj->vencId,
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

    /* Free the input buffer */
    VpsUtils_memFree(appObj->inBuf, (TOTAL_NUM_BUFFERS * appObj->inFrmSize));

    Vps_printf("%s: Test Successful!!\n", APP_NAME);

    /* Test complete. Signal it */
    Semaphore_post(AppTestCompleteSem);
}



/**
 *  App_dispSdTest
 *  SD display test function.
 */
static Int32 App_dispSdTest(App_DisplayObj *appObj)
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
                 appObj->driverInst,
                 appObj->vencId,
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
    Vps_printf("%s: Display Driver Opened for Instance %d!!\n",
        APP_NAME, appObj->driverInst);
    VpsUtils_appPrintDispCreateStatus(&createStatus);

    /* Set the required format */
    App_dispSetFormat(appObj, &fmt);
    retVal = FVID2_setFormat(appObj->fvidHandle, &fmt);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
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
        App_dispUpdateFrameList(
            appObj,
            fList,
            &appObj->framePool[frmListCnt]);

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

        /* Update the buffer address */
        App_dispUpdateFrameList(appObj, fList, fList->frames[0u]);

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

    VpsUtils_prfTsEnd(prfTsHandle, TOTAL_LOOP_COUNT);

    /* Stop driver */
    retVal = FVID2_stop(appObj->fvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    Vps_printf("%s: Display Driver %d Stopped!!\n",
        APP_NAME, appObj->driverInst);

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
                 appObj->driverInst,
                 appObj->vencId,
                 FALSE);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Free allocated handles */
    App_dispFreeVariables(appObj);

    Vps_printf("%s: SD Display %d Test Successful!!\n",
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
                                    FVID2_FrameList *fList,
                                    FVID2_Frame *frame)
{
    UInt32              index;

    /* Fill the next set of buffers */
    index = appObj->bufferIndex;
    /* Wrap around */
    if (index >= TOTAL_NUM_BUFFERS)
    {
        index = 0u;
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
                + (appObj->pitch * appObj->height);
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
                + (appObj->pitch * appObj->height);
        frame->addr[FVID2_FIELD_ODD_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
            ((UInt8 *) frame->addr[FVID2_FIELD_ODD_ADDR_IDX]
                                  [FVID2_YUV_SP_Y_ADDR_IDX])
                + (appObj->pitch * appObj->height);
    }
    fList->frames[0u] = frame;
    index++;

    appObj->submittedFrames++;

    /* Increment the input frame index only every FRAME_SWITCH_RATE frames */
    if (0u == (appObj->submittedFrames % FRAME_SWITCH_RATE))
    {
        appObj->bufferIndex++;
    }
    /* Wrap around */
    if (appObj->bufferIndex >= TOTAL_NUM_BUFFERS)
    {
        appObj->bufferIndex %= TOTAL_NUM_BUFFERS;
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
 *  App_dispInitVariables
 *  Initialize the global variables and frame pointers.
 */
static Int32 App_dispInitVariables(App_DisplayObj *appObj)
{
    Int32               retVal = FVID2_SOK;
    Semaphore_Params    semParams;

    Semaphore_Params_init(&semParams);
    appObj->readtyToDqSem = Semaphore_create(0, &semParams, NULL);
    if (NULL == appObj->readtyToDqSem)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EALLOC);
    }

    appObj->bufferIndex = 0u;
    appObj->submittedFrames = 0u;
    appObj->completedFrames = 0u;

    /* Init frame list */
    appObj->frameList.numFrames = 1u;
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
    Semaphore_delete(&appObj->readtyToDqSem);
}



/**
 *  App_dispGetInputFromUser
 *  Get the test parameters from the user.
 */
static void App_dispGetInputFromUser(App_DisplayObj *appObj)
{
    Int32       retVal;
    char        inputStr[10];
    UInt32      standard, dataFormat,videoFormat;

    appObj->driverInst = DRIVER_INSTANCE;
    appObj->vencId = VPS_DC_VENC_SD;

    /* Get the mode to run from user */
    Vps_printf("-------------------------------------------------------\n");
    Vps_printf("Select display mode as per below table:\n");
    Vps_printf("-------------------------------------------------------\n");
    Vps_printf("1 - NTSC\n");
    Vps_printf("2 - PAL\n");
    Vps_printf("-------------------------------------------------------\n");
    standard = 0xFFFFFFFFu;
    do
    {
        Vps_printf("%s: Enter any of above value and press enter...\n",
            APP_NAME);
        VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
        standard = atoi(inputStr);

        switch (standard)
        {
            case 1u:
                standard = FVID2_STD_NTSC;
                break;

            case 2u:
                standard = FVID2_STD_PAL;
                break;

            default:
                Vps_printf("%s: Invalid display mode %d selected\n",
                    APP_NAME, standard);
                standard = 0xFFFFFFFFu;
                break;
        }

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
    } while (0xFFFFFFFFu == standard);
    Vps_printf("%s: Display Mode Selected: %d\n", APP_NAME, standard);

    dataFormat = FVID2_DF_YUV422I_YUYV;
    if ((VPS_DISP_INST_SEC1 == appObj->driverInst) ||
        (VPS_DISP_INST_AUX_DEI_SC2 == appObj->driverInst))
    {
        /* Get the format to run from user */
        Vps_printf("-------------------------------------------------------\n");
        Vps_printf("Select buffer format as per below table:\n");
        Vps_printf("-------------------------------------------------------\n");
        Vps_printf("1 - YUV420SP\n");
        Vps_printf("2 - YUYV422I\n");
        Vps_printf("3 - YUV422SP\n");
        Vps_printf("-------------------------------------------------------\n");
        dataFormat = 0xFFFFFFFFu;
        do
        {
            Vps_printf("%s: Enter any of above value and press enter...\n",
                APP_NAME);
            VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
            dataFormat = atoi(inputStr);

            switch (dataFormat)
            {
                case 1u:
                    dataFormat = FVID2_DF_YUV420SP_UV;
                    break;

                case 2u:
                    dataFormat = FVID2_DF_YUV422I_YUYV;
                    break;

                case 3u:
                    dataFormat = FVID2_DF_YUV422SP_UV;
                    break;

                default:
                    Vps_printf("%s: Invalid format %d selected\n",
                        APP_NAME, dataFormat);
                    dataFormat = 0xFFFFFFFFu;
                    break;
            }
        } while (0xFFFFFFFFu == dataFormat);
    }
    Vps_printf("%s: Format Selected: %d\n", APP_NAME, dataFormat);
    appObj->dataFormat = dataFormat;

    /* Get the output format from user */
    Vps_printf("-------------------------------------------------------\n");
    Vps_printf("Select output video format as per below table:\n");
    Vps_printf("-------------------------------------------------------\n");
    Vps_printf("1 - S-Video\n");
    Vps_printf("2 - COMPOSITE (Only Composite output for TI8107)\n");
    Vps_printf("-------------------------------------------------------\n");
    videoFormat = 0xFFFFFFFFu;
    do
    {
        Vps_printf("%s: Enter any of above value and press enter...\n",
            APP_NAME);
        VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
        videoFormat = atoi(inputStr);

        switch (videoFormat)
        {
            case 1u:
                videoFormat = VPS_DC_A_OUTPUT_SVIDEO;
                break;

            case 2u:
                videoFormat = VPS_DC_A_OUTPUT_COMPOSITE;
                break;

            default:
                Vps_printf("%s: Invalid format %d selected\n",
                    APP_NAME, videoFormat);
                videoFormat = 0xFFFFFFFFu;
                break;
        }
    } while (0xFFFFFFFFu == videoFormat);
    appObj->videoFormat = videoFormat;

    /* Initialize test parameters */
    appObj->width = BUFFER_WIDTH_NTSC;
    appObj->height = BUFFER_HEIGHT_NTSC;
    if (FVID2_STD_PAL == appObj->modeInfo.standard)
    {
        appObj->width = BUFFER_WIDTH_PAL;
        appObj->height = BUFFER_HEIGHT_PAL;
    }
    if (appObj->width > appObj->modeInfo.width)
    {
        appObj->width = appObj->modeInfo.width;
    }
    if (appObj->height > appObj->modeInfo.height)
    {
        appObj->height = appObj->modeInfo.height;
    }
    if (FVID2_DF_YUV422SP_UV == appObj->dataFormat)
    {
        appObj->pitch = appObj->width;
        appObj->inFrmSize = appObj->pitch * appObj->height * 2u;
    }
    else if (FVID2_DF_YUV420SP_UV == appObj->dataFormat)
    {
        appObj->pitch = appObj->width;
        appObj->inFrmSize = appObj->pitch * appObj->height * 3u / 2u;
    }
    else
    {
        appObj->pitch = appObj->width * 2u;
        appObj->inFrmSize = appObj->pitch * appObj->height;
    }

    return;
}
