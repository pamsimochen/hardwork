/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file MosaicDisplay_main.c
 *
 *  \brief Main file invoking the test function.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/heaps/HeapBuf.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/utility/vpsutils_app.h>
#include "MosaicDisplay_test.h"


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

static Void App_mainTestTask(void);
static Int32 App_dispGetTestId(void);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppMainTskStack, 32)
#pragma DATA_SECTION(AppMainTskStack, ".bss:taskStackSection")
static UInt8 AppMainTskStack[APP_TSK_STACK_SIZE];

/* Test complete semaphore used to sync between display and main task. */
Semaphore_Handle AppTestCompleteSem;

/* Configuration for various test cases */
static App_DisplayTestCfg AppTestCfg[] =
{
    {
        "BP0->On-Chip HDMI @ 720P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDMI, FVID2_STD_720P_60
    },
    {
        "BP0->On-Chip HDMI @ 1080I60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDMI, FVID2_STD_1080I_60
    },
    {
        "BP0->On-Chip HDMI @ 1080P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDMI, FVID2_STD_1080P_60
    },
    /* Enable HDCOMP test cases only for TI816X */
#if defined(TI_816X_BUILD)
    {
        "BP0->HDCOMP @ 720P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_720P_60
    },
    {
        "BP0->HDCOMP @ 720P50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_720P_50
    },
    {
        "BP0->HDCOMP @ 1080I60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_1080I_60
    },
    {
        "BP0->HDCOMP @ 1080I50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_1080I_50
    },
    {
        "BP0->HDCOMP @ 1080P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_1080P_60
    },
    {
        "BP0->HDCOMP @ 1080P50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_1080P_50
    },
    {
        "BP0->HDCOMP @ 1080P30",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_1080P_30
    },
    {
        "BP0->HDCOMP @ 480P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_480P
    },
    {
        "BP0->HDCOMP @ 576P50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_576P
    },
    {
        "BP0->HDCOMP @ XGA60 - Need ES2.0 DVR board!!",
        VPS_DISP_INST_BP0, VPS_DC_VENC_HDCOMP, FVID2_STD_XGA_60
    },
#endif  /* #if defined(TI_816X_BUILD) */
    {
        "BP0->DVO2 @ 720P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_720P_60
    },
    {
        "BP0->DVO2 @ 720P50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_720P_50
    },
    {
        "BP0->DVO2 @ 1080I60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_1080I_60
    },
    {
        "BP0->DVO2 @ 1080I50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_1080I_50
    },
    {
        "BP0->DVO2 @ 1080P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_1080P_60
    },
    {
        "BP0->DVO2 @ 1080P50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_1080P_50
    },
    {
        "BP0->DVO2 @ 1080P30",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_1080P_30
    },
    {
        "BP0->DVO2 @ 480P60",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_480P
    },
    {
        "BP0->DVO2 @ 576P50",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_576P
    },
    {
        "BP0->SD VENC @ NTSC",
        VPS_DISP_INST_BP0, VPS_DC_VENC_SD, FVID2_STD_NTSC
    },
    {
        "BP0->SD VENC @ PAL",
        VPS_DISP_INST_BP0, VPS_DC_VENC_SD, FVID2_STD_PAL
    },
#if defined(TI_8107_BUILD)
    {
        "BP0->DVO2 VENC (LCD) @ WSVGA70 (1024x600) - Need VCAM board!!",
        VPS_DISP_INST_BP0, VPS_DC_VENC_DVO2, FVID2_STD_WSVGA_70
    }
#endif
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  main
 *  Application main function.
 */
Int32 main(void)
{
    Task_Params tskParams;

    Task_Params_init(&tskParams);
    tskParams.stack = AppMainTskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    Task_create((Task_FuncPtr) App_mainTestTask, &tskParams, NULL);

    BIOS_start();

    return (0);
}



/**
 *  App_mainTestTask
 *  Application test task.
 */
static Void App_mainTestTask(void)
{
    char                            ch;
    Int32                           retVal, testId;
    UInt8                          *inBuf;
    UInt32                          inFrmSize, pitch;
    Semaphore_Params                semPrms;
    Task_Handle                     dispTaskHandle;
    Vps_PlatformDeviceInitParams    deviceInitPrms;
    Vps_PlatformInitParams          platformInitPrms;
    App_DisplayTestCfg             *testCfg;
    UInt32                          numBuffers;

    /* Create semaphore */
    Semaphore_Params_init(&semPrms);
    AppTestCompleteSem = Semaphore_create(0, &semPrms, NULL);
    if (NULL == AppTestCompleteSem)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* Init Sequence */
    platformInitPrms.isPinMuxSettingReq = TRUE;
    retVal = Vps_platformInit(&platformInitPrms);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    VpsUtils_memInit();

    /* Get the version string */
    Vps_printf("HDVPSS Drivers Version: %s\n", FVID2_getVersionString());

    /* Init FVID2 and VPS */
    retVal = FVID2_init(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Initialize all the on board devices */
#ifdef PLATFORM_ZEBU
    deviceInitPrms.isI2cInitReq = FALSE;
    deviceInitPrms.isI2cProbingReq = FALSE;
#else
    deviceInitPrms.isI2cInitReq = TRUE;
    deviceInitPrms.isI2cProbingReq = TRUE;
#endif
    retVal = Vps_platformDeviceInit(&deviceInitPrms);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Initialize the vpsUtils to get the load */
    VpsUtils_prfInit();
    VpsUtils_appInit();

    /* Print platform information */
    Vps_platformPrintInfo();

    if (FVID2_DF_YUV422SP_UV == DATA_FORMAT)
    {
        pitch = BUFFER_WIDTH;
        inFrmSize = pitch * BUFFER_HEIGHT * 2u;
    }
    else if (FVID2_DF_YUV420SP_UV == DATA_FORMAT)
    {
        pitch = BUFFER_WIDTH;
        inFrmSize = pitch * BUFFER_HEIGHT * 3u / 2u;
    }
    else
    {
        pitch = BUFFER_WIDTH * 2u;
        inFrmSize = pitch * BUFFER_HEIGHT;
    }

    if (VPS_PLATFORM_BOARD_NETCAM == Vps_platformGetBoardId())
    {
        numBuffers = 3u;
    }
    else
    {
        numBuffers = TOTAL_NUM_BUFFERS;
    }

    /* Allocate input buffer */
    inBuf = VpsUtils_memAlloc(
                numBuffers * inFrmSize,
                VPS_BUFFER_ALIGNMENT);
    if (NULL == inBuf)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* Wait for the user to load the image */
    Vps_printf("\n");
    Vps_printf("Load %d %s video frames of size %dx%d to location: 0x%p\n",
        numBuffers, FVID2_getDataFmtString(FVID2_DF_YUV422I_YUYV),
        BUFFER_WIDTH, BUFFER_HEIGHT, inBuf);
    Vps_printf("Enter any key after loading...\n");
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    while (1u)
    {
        /* Get the TestId */
        testId = App_dispGetTestId();
        if ((testId >= 0) &&
            (testId < (sizeof (AppTestCfg) / sizeof (App_DisplayTestCfg))))
        {
            testCfg = &AppTestCfg[testId];
        }
        else
        {
            /* Exit */
            break;
        }

        if ((VPS_DC_VENC_HDCOMP == testCfg->vencId) &&
            ((VPS_PLATFORM_ID_EVM_TI814x == Vps_platformGetId()) ||
                (VPS_PLATFORM_ID_SIM_TI814x == Vps_platformGetId())))
        {
            /* Skip HDCOMP test cases for TI814x*/
            Vps_printf("%s: Skipping HDCOMP tests for TI814x!!\n", APP_NAME);
            continue;
        }

        if ((Vps_platformIsEvm()) &&
            (VPS_DC_VENC_DVO2 == testCfg->vencId) &&
            (VPS_PLATFORM_BOARD_UNKNOWN == Vps_platformGetBoardId()))
        {
            Vps_printf("%s: Unknown/no daughter card connected!!\n", APP_NAME);
            continue;
        }

        if ((VPS_DC_VENC_HDCOMP == testCfg->vencId) &&
            (Vps_platformGetCpuRev() < VPS_PLATFORM_CPU_REV_2_0) &&
            (FVID2_isStandardVesa(testCfg->standard)))
        {
            /* Skip HDCOMP VESA test cases for Non-ES2.0 revision */
            Vps_printf("%s: Skipping VESA HDCOMP tests for Non-ES2.0 "
                "revision!!\n", APP_NAME);
            continue;
        }

        Vps_printf("%s: Running test: %s\n", APP_NAME, testCfg->testDescStr);

        /* Initialize test parameters */
        testCfg->width = BUFFER_WIDTH;
        testCfg->height = BUFFER_HEIGHT;
        testCfg->pitch = pitch;
        testCfg->dataFormat = DATA_FORMAT;
        testCfg->inBuf = inBuf;
        testCfg->inFrmSize = inFrmSize;
        testCfg->numInBuf = numBuffers;

        /* Create display test task */
        dispTaskHandle = App_dispCreateTsk(testCfg);
        if (NULL == dispTaskHandle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            break;
        }

        /* Register the task to the load module for calculating the load */
        VpsUtils_prfLoadRegister(dispTaskHandle, "MosaicDisplay:");

        /* Wait for display test to complete */
        Semaphore_pend(AppTestCompleteSem, BIOS_WAIT_FOREVER);

        /* UnRegister the task from the load module */
        VpsUtils_prfLoadUnRegister(dispTaskHandle);

        /* Delete the display task */
        Task_delete(&dispTaskHandle);
    }

    /* Free the input buffer */
    VpsUtils_memFree(inBuf, (numBuffers * inFrmSize));

    /* De-Init Sequence */
    VpsUtils_appDeInit();
    VpsUtils_prfDeInit();
    Vps_platformDeviceDeInit();
    retVal = FVID2_deInit(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    VpsUtils_memDeInit();
    Vps_platformDeInit();

    Semaphore_delete(&AppTestCompleteSem);

    Vps_printf("Test Successful!!\n");

    return;
}



/**
 *  App_dispGetTestId
 *  Return the test ID to run.
 */
static Int32 App_dispGetTestId(void)
{
    UInt32          testCnt;
    char            inputStr[10];
    static Bool     enableAutoRun = FALSE;
    static Int32    testId = 0;

    Vps_printf("\n");
    Vps_printf("--------------------------------------\n");
    Vps_printf("Select test to run as per below table:\n");
    Vps_printf("--------------------------------------\n");
    Vps_printf("\n");
    for (testCnt = 0;
         testCnt < (sizeof (AppTestCfg) / sizeof (App_DisplayTestCfg));
         testCnt++)
    {
        Vps_printf("%3d: %s\n", testCnt, AppTestCfg[testCnt].testDescStr);
    }
    Vps_printf("%3d: %s\n",
        (sizeof(AppTestCfg) / sizeof(App_DisplayTestCfg)), "Auto Run\n");
    Vps_printf("%3d: EXIT\n", 100);
    Vps_printf("\n");
    Vps_printf("Enter Test to Run: ");

    if (TRUE == enableAutoRun)
    {
        testId++;
        Vps_printf("%d\n", testId);
    }
    else
    {
        while (1u)
        {
            VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
            testId = atoi(inputStr);
            if ((testId >= 0) &&
                (testId < (sizeof(AppTestCfg) / sizeof(App_DisplayTestCfg))))
            {
                break;
            }
            else if (testId == (sizeof(AppTestCfg) / sizeof(App_DisplayTestCfg)))
            {
                enableAutoRun = TRUE;
                testId = 0;
                break;
            }
            else if (100 == testId)
            {
                break;
            }
            Vps_printf("Invalid Test ID. Enter Agian!!\n");
        }
    }

    return (testId);
}
