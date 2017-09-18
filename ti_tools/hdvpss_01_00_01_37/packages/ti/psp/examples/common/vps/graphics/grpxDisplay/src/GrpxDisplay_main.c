/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file GrpxDisplay_main.c
 *
 * \brief VPS Graphics display example file.
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
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_grpx.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/devices/vps_sii9022a.h>
#include <ti/psp/devices/vps_thsfilters.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Driver instance to test. */
#define DRIVER_INSTANCE                 (VPS_DISP_INST_GRPX0)

/* Number of loop count to run the application. */
#ifdef PLATFORM_EVM_SI
#define TOTAL_LOOP_COUNT                (3000u)
#define RT_SWITCH_RATE                  (200u)
#else
#define TOTAL_LOOP_COUNT                (30u)
#define RT_SWITCH_RATE                  (1u)
#endif

#define GRPX_WIDTH                      (240u)
#define GRPX_HEIGHT                     (160u)
#define GRPX_BPP                        (24u)
#define GRPX_FORMAT                     (FVID2_DF_RGB24_888)
/* Frame and input video parameters. */
#define REGION_WIDTH                    GRPX_WIDTH
#define REGION_HEIGHT                   GRPX_HEIGHT


/* Number of input buffers to load. */
#define NUM_BUFFERS                     (3u)

/* Number of frames per framelist. */
#define NUM_REGIONS_PER_LIST            (1u)

/* Total number of buffers to allocate.
 * This should be the number of frames that should be loaded in to the
 * input buffer. */
#define TOTAL_NUM_BUFFERS               (NUM_REGIONS_PER_LIST * NUM_BUFFERS)

/* Number of framelist used for priming. */
#define NUM_FRAMELIST                   (3u)

/* Test application stack size. */
#define GRPXAPP_TSK_STACK_SIZE           (10u * 1024u)
#define APP_NAME                        "SingleRegGRPX"

/* Uncomment below to configure off chip hdmi */
#define APP_GRPX_CONFIG_HDMI

/* Uncomment below to configure off chip hdmi */
//#define APP_GRPX_CONFIG_LCD_OUTPUT

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void grpxTestTask(void);
static Int32 vpsGrpxDisplayTest(UInt32 driverInst);
static Int32 vpsGrpxProcessRegion(FVID2_Handle handle);
static Int32 vpsGrpxAppCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
static Int32 vpsGrpxAppErrCbFxn(FVID2_Handle handle,
                            Ptr appData,
                            Void *errList,
                            Ptr reserved);
static Int32 vpsGrpxConfigDctrl(UInt32 driverInst, UInt32 clear);
static Void vpsGrpxSetFormat(FVID2_Format *fmt);
static Void vpsGrpxSetParams(void);
static Void vpsGrpxInitVariables(void);
static Int32 setPixelClk(UInt32 standard);
static void scEnableSet(UInt32 Flag);
static void stenEnableSet(UInt32 Flag);
#ifdef APP_GRPX_CONFIG_HDMI
static Int32 vpsGrpxConfigHdmi(FVID2_Handle *hdmiHandle);
#endif
static Int32 vpsGrpxGetPlatformData(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(GrpxApp_mainTskStack, 32)
#pragma DATA_SECTION(GrpxApp_mainTskStack, ".bss:taskStackSection")
static UInt8 GrpxApp_mainTskStack[GRPXAPP_TSK_STACK_SIZE];


/* Display driver handle. */
static FVID2_Handle          GrpxAppFvidHandle;
/* Display controller handle. */
static FVID2_Handle             GrpxAppDcHandle;

#ifdef APP_GRPX_CONFIG_HDMI
/*hdmi driver handle*/
static FVID2_Handle           GrpxHdmiHandle;
#endif

/* Input buffer. Align the buffers properly. */
static UInt8                 *AppGrpxBuffer;

/* Frame list/frame/frame pointer objects used for queue/dequeue operations. */
static FVID2_FrameList       GrpxAppFrameList;
static FVID2_Frame           GrpxAppFrames[NUM_FRAMELIST];

/* Frame list/frame/frame pointer objects used for error operations. */
static FVID2_FrameList       GrpxAppErrFrameList;

/* Current buffer index. Used to wrap around after NUM_BUFFERS iteration. */
static UInt32                GrpxAppBufIndex = 0u;
/* Total number of completed frames. */
static UInt32                GrpxAppDoneRegions = 0u;

/* Stream complete semaphoreused to sync between callback and task. */
static Semaphore_Handle      GrpxTestCompleteSem;
static Semaphore_Handle      GrpxTestCallbackSem;
/*graphics region list*/
static Vps_GrpxParamsList    List;

/*parameters*/
static Vps_GrpxRtParams      rtParams;
/*scaler parameter*/
static Vps_GrpxScParams      scParams;

/* CLUT Table data*/
static unsigned long         *clutData;

/*Stenciling data*/
static unsigned char         *stenData;

/*stenciling data pitch*/
static UInt32                stenPitch;
/*Grpx Data pitch*/
static UInt32                dataPitch;

/*GRPX buffer size*/
static UInt32               bufferSize;

static UInt32                venc;

Vps_PlatformId          platformId;
/**< Platform identifier. */
Vps_PlatformCpuRev      cpuRev;
/**< CPU version identifier. */
Vps_PlatformBoardId     boardId;
/**< Board identifier. */

/*grpx dc config*/
#ifdef PLATFORM_SIM
static Vps_DcConfig          grpxDcConfig =
{
    VPS_DC_USERSETTINGS,
    {
        {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_HDMI_BLEND}
    },
    1,
    {{{VPS_DC_VENC_HDMI, FALSE, FVID2_STD_1080P_60}},
       0, 1u}
};

#else
static Vps_DcConfig          grpxDcConfig =
{
    VPS_DC_USERSETTINGS,
    {
        {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_DVO2_BLEND}
    },
    1,
    {
       {{VPS_DC_VENC_DVO2, FALSE, FVID2_STD_1080P_30}},
       0, 1u}
};


#endif
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
    tskParams.stack = GrpxApp_mainTskStack;
    tskParams.stackSize = GRPXAPP_TSK_STACK_SIZE;

    Task_create((Task_FuncPtr) grpxTestTask, NULL, NULL);
    BIOS_start();

    return (0);
}



/**
 *  Application test task.
 */
static Void grpxTestTask(void)
{
    Int32                    retVal;
    Int32                    driverRtVal;
    const Char               *versionStr;
    Char                     ch;
    Vps_PlatformDeviceInitParams deviceInitPrms;
    Vps_PlatformInitParams       platformInitPrms;

    /* Initialize the platform and set THS to ED Mode */
    platformInitPrms.isPinMuxSettingReq = TRUE;
    Vps_platformInit(&platformInitPrms);
    VpsUtils_memInit();

    /*
     * Get the version string
     */
    versionStr = FVID2_getVersionString();
    Vps_printf("HDVPSS Drivers Version: %s\n", versionStr);
    /* Init FVID2 and VPS */
    retVal = FVID2_init(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    deviceInitPrms.isI2cInitReq = TRUE;
    deviceInitPrms.isI2cProbingReq = TRUE;
    Vps_platformDeviceInit(&deviceInitPrms);

    /* Initialize the vpsUtils to get the load */
    VpsUtils_prfInit();

    /* Open Display Controller */
    GrpxAppDcHandle = FVID2_create(
                      FVID2_VPS_DCTRL_DRV,
                      VPS_DCTRL_INST_0,
                      NULL,
                      &driverRtVal,
                      NULL);
    if (NULL == GrpxAppDcHandle)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, driverRtVal, __LINE__);
        return;
    }

    while (1u)
    {
        /* Run the test */
        retVal = vpsGrpxDisplayTest(DRIVER_INSTANCE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return;
        }

        fflush(stdin);
        Vps_printf("Enter 0 to exit or any other key to continue testing...\n");
        VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);
        if ('0' == ch)
        {
            break;
        }
    }

    /* Close Display Controller */
    retVal = FVID2_delete(GrpxAppDcHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

#ifdef APP_GRPX_CONFIG_LCD_OUTPUT
    Vps_platformEnableLcd(FALSE);
#endif

    /* De-Init Sequence */
    VpsUtils_prfDeInit();
    Vps_platformDeviceDeInit();
    retVal = FVID2_deInit(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    VpsUtils_memDeInit();
    Vps_platformDeInit();

    Vps_printf("Test Successfull!!\n");
}



/**
 *  SD display test function.
 */
static Int32 vpsGrpxDisplayTest(UInt32 driverInst)
{
    char                     inputStr[10];
    Int32                    retVal = FVID2_SOK;
    UInt32                   frmListCnt;
    FVID2_Format             fmt;
    Semaphore_Params         semParams;
    FVID2_CbParams           cbParams;
    Vps_GrpxCreateParams     createParams =
    {
        VPS_VPDMA_MT_NONTILEDMEM,
        VPS_GRPX_STREAMING_MODE,
        FALSE
    };
    Vps_GrpxCreateStatus    createStatus;
    UInt32                  bufCnt;
    UInt32                  offset = 0u;
    VpsUtils_PrfTsHndl     *prfTsHandle;
    UInt32                  standard;

    if (vpsGrpxGetPlatformData() != FVID2_SOK)
    {
        return -1;
    }
    /*data pitch should be aligned on 16btypes*/
    dataPitch = VpsUtils_getPitch(REGION_WIDTH, GRPX_BPP);
    bufferSize = dataPitch * REGION_HEIGHT;

    AppGrpxBuffer = VpsUtils_memAlloc(TOTAL_NUM_BUFFERS * bufferSize,
        VPS_BUFFER_ALIGNMENT);
    if (NULL == AppGrpxBuffer)
    {
        Vps_printf("Failed to allocate GRPX Data buffer.\n");
        return -1;
    }


    clutData = VpsUtils_memAlloc(1024,VPS_BUFFER_ALIGNMENT);
    if (NULL == clutData)
    {
        Vps_printf("Failed to allocate GRPX CLUT buffer.\n");
        return -1;
    }

    /*stride should be aligned on 16bytes*/
    stenPitch = VpsUtils_getPitch(REGION_WIDTH, 1);

    stenData = VpsUtils_memAlloc(REGION_HEIGHT * stenPitch,
        VPS_BUFFER_ALIGNMENT);
    if (NULL == stenData)
    {
        Vps_printf("Failed to allocate STEN Buffer.\n");
        return -1;
    }
    Vps_printf(
        "--------------------------------------------------------\n");
    Vps_printf(
        "Select display Venc as per below table:\n");
    Vps_printf(
        "--------------------------------------------------------\n");
    if ((VPS_PLATFORM_ID_EVM_TI816x == platformId) ||
        (VPS_PLATFORM_ID_SIM_TI816x == platformId))
    {

        Vps_printf("%d - HD-DAC\n", VPS_DC_VENC_HDCOMP);
    }
    Vps_printf("%d - DVO2\n", VPS_DC_VENC_DVO2);
    Vps_printf("%d - SDVENC\n", VPS_DC_VENC_SD);
    Vps_printf(
        "--------------------------------------------------------\n");
    do
    {
        Vps_printf("%s: Enter any of above values and press enter...\n",
            APP_NAME);
        VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
        venc = atoi(inputStr);

        if ((venc == VPS_DC_VENC_DVO2) ||
            (venc == VPS_DC_VENC_HDCOMP) ||
            (venc == VPS_DC_VENC_SD))
        {
            if ((VPS_DC_VENC_HDCOMP == venc) &&
                ((VPS_PLATFORM_ID_EVM_TI814x == platformId) ||
                    (VPS_PLATFORM_ID_SIM_TI814x == platformId)))
            {
                Vps_printf("%s: Invalid VENC %d selected\n",
                    APP_NAME, venc);
                venc = 0u;
                continue;
            }

#ifdef APP_GRPX_CONFIG_HDMI
            if ((Vps_platformIsEvm()) &&
                (VPS_DC_VENC_DVO2 == venc) &&
                ((VPS_PLATFORM_BOARD_VS != boardId) &&
                    (VPS_PLATFORM_BOARD_VC != boardId)))
            {
                Vps_printf("%s: Unknown/no daughter card connected!!\n",
                    APP_NAME);
                venc = 0u;
                continue;
            }
#endif
            break;
        }
        else
        {
            Vps_printf("%s: Invalid VENC %d selected\n", APP_NAME, venc);
            venc = 0u;
        }

    } while (venc == 0u);
    Vps_printf("%s: Display VENC Selected: %d\n", APP_NAME, venc);

    Vps_printf(
        "--------------------------------------------------------\n");
    Vps_printf(
        "Select display mode as per below table:\n");
    Vps_printf(
        "--------------------------------------------------------\n");
    if (venc != VPS_DC_VENC_SD)
    {
        Vps_printf("%d - 1080p60\n", 0);
        Vps_printf("%d - 1080p30 \n", 1);
        Vps_printf("%d - 1080i60\n", 2);
        Vps_printf("%d - 720p60\n", 3);
        Vps_printf("%d - 1080p50\n", 4);
        Vps_printf("%d - 1080i50\n", 5);
        Vps_printf("%d - 720p50\n", 6);
#ifdef APP_GRPX_CONFIG_LCD_OUTPUT
        Vps_printf("%d - LCD Output\n", 7);
#endif
        Vps_printf(
            "--------------------------------------------------------\n");
        Vps_printf("%s: Enter value from %d to %d and press enter...\n",
        APP_NAME, 0, 6);
    }
    else {
        Vps_printf("%d - ntsc\n", 8);
        Vps_printf("%d - pal\n",9);
        Vps_printf(
            "--------------------------------------------------------\n");
        Vps_printf("%s: Enter value from %d to %d and press enter...\n",
        APP_NAME, 7, 8);

    }
    VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
    standard = atoi(inputStr);
    if(venc != VPS_DC_VENC_SD)
    {
#ifdef APP_GRPX_CONFIG_LCD_OUTPUT
        while (standard > 7)
#else
        while (standard > 6)
#endif
        {
            Vps_printf(
                "%s: Invalid display mode %d selected\n", APP_NAME, standard);
            Vps_printf("%s: Enter value from %d to %d and press enter...\n",
                APP_NAME, 0, 6);
            VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
            standard = atoi(inputStr);
        }
    }
    else
    {
        while ((standard > 9) || (standard < 8))
        {
            Vps_printf(
                "%s: Invalid display mode %d selected\n", APP_NAME, standard);
            Vps_printf("%s: Enter value from %d to %d and press enter...\n",
                APP_NAME, 7, 8);
            VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
            standard = atoi(inputStr);
        }

    }
    Vps_printf("%s: Display Mode Selected: %d\n", APP_NAME, standard);
    retVal = setPixelClk(standard);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return retVal;
    }
    /* Wait for the user to load the image */
    Vps_printf(
        "Loading %d %s graphics frames of size %dx%d to location: 0x%p\n",
        TOTAL_NUM_BUFFERS, FVID2_getDataFmtString(GRPX_FORMAT),
        REGION_WIDTH, REGION_HEIGHT, AppGrpxBuffer);
    for (bufCnt = 0u; bufCnt < NUM_BUFFERS; bufCnt++)
    {
        VpsUtils_grpxGenPattern(AppGrpxBuffer + offset,
                                GRPX_FORMAT,
                                REGION_WIDTH,
                                REGION_HEIGHT,
                                0,
                                0);
        offset += bufferSize;
    }
    Vps_printf("%s: Image loading done...\n", APP_NAME);

    Semaphore_Params_init(&semParams);
    GrpxTestCompleteSem = Semaphore_create(0, &semParams, NULL);

    Semaphore_Params_init(&semParams);
    GrpxTestCallbackSem = Semaphore_create(0, &semParams, NULL);

    /* Initialize the frame pointers */
    vpsGrpxInitVariables();

    /* Configure display controller - Set the paths */
    retVal = vpsGrpxConfigDctrl(driverInst, TRUE);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return retVal;
    }

#ifdef APP_GRPX_CONFIG_LCD_OUTPUT
    Vps_platformEnableLcd(TRUE);
    Vps_platformEnableLcdBackLight(TRUE);
    Vps_platformSetLcdBackLight(0xFFFFFFFC);
#endif

    if (Vps_platformIsEvm())
    {
        if (venc == VPS_DC_VENC_DVO2)
        {
#ifdef APP_GRPX_CONFIG_HDMI
            retVal = vpsGrpxConfigHdmi(&GrpxHdmiHandle);
            if (FVID2_SOK != retVal)
            {
                Vps_printf(
                    "%s: Error %d @line %d\n", __FUNCTION__, retVal, __LINE__);
                return retVal;
            }
#endif
        }
    }

    /* Open the driver */
    cbParams.cbFxn = vpsGrpxAppCbFxn;
    cbParams.errCbFxn = vpsGrpxAppErrCbFxn;
    cbParams.errList = &GrpxAppErrFrameList;
    cbParams.appData = NULL;
    cbParams.reserved = NULL;
    GrpxAppFvidHandle = FVID2_create(
                        FVID2_VPS_DISP_GRPX_DRV,
                        driverInst,
                        &createParams,
                        &createStatus,
                        &cbParams);
    if (NULL == GrpxAppFvidHandle)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, createStatus.retVal, __LINE__);
        return (createStatus.retVal);
    }
    Vps_printf("%s: Graphics Driver Opened!!\n", APP_NAME);
    Vps_printf("%s: Minimum Number of Buffers to Start: %d\n",
        APP_NAME, createStatus.minBufNum);
    Vps_printf("%s: Maximum Request in Input Queue: %d\n",
        APP_NAME, createStatus.maxReq);
    Vps_printf("%s: Maximum Number of Regions Supported: %d\n",
        APP_NAME, createStatus.maxRegions);

    /* Set the required format */
    vpsGrpxSetFormat(&fmt);
    retVal = FVID2_setFormat(GrpxAppFvidHandle, &fmt);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    vpsGrpxSetParams();
    retVal = FVID2_control(GrpxAppFvidHandle,IOCTL_VPS_SET_GRPX_PARAMS,&List,NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error %d @ line %d\n",__FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Start the load calculation */
    VpsUtils_prfLoadCalcStart();
    /* Start the TimeStamp Calculation */
    prfTsHandle = VpsUtils_prfTsCreate(APP_NAME":");

    /* Prime Buffers */
    for (frmListCnt = 0u; frmListCnt < NUM_BUFFERS; frmListCnt++)
    {
        /* Queue buffers to driver */
        GrpxAppFrameList.frames[0] = &GrpxAppFrames[GrpxAppBufIndex];
        retVal = FVID2_queue(GrpxAppFvidHandle, &GrpxAppFrameList, 0u);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return (retVal);
        }
        GrpxAppBufIndex++;
    }
    /* Start driver */
    Vps_printf("%s: Starting Graphics Driver...\n", APP_NAME);
    retVal = FVID2_start(GrpxAppFvidHandle, NULL);

    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    if (Vps_platformIsEvm())
    {
        if (venc == VPS_DC_VENC_DVO2)
        {
#ifdef APP_GRPX_CONFIG_HDMI
            retVal = FVID2_start(GrpxHdmiHandle, NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
                return (retVal);
            }
#endif
        }
    }
    VpsUtils_prfTsBegin(prfTsHandle);

    do
    {
        Semaphore_pend(GrpxTestCallbackSem, BIOS_WAIT_FOREVER);
        vpsGrpxProcessRegion(GrpxAppFvidHandle);
    } while(GrpxAppDoneRegions != TOTAL_LOOP_COUNT);

    Semaphore_pend(GrpxTestCompleteSem, BIOS_WAIT_FOREVER);

    VpsUtils_prfTsEnd(prfTsHandle, TOTAL_LOOP_COUNT);

    if (Vps_platformIsEvm())
    {
        if (venc == VPS_DC_VENC_DVO2)
        {
#ifdef APP_GRPX_CONFIG_HDMI
            /* Stop HDMI transmitter */
            retVal = FVID2_stop(GrpxHdmiHandle, NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
                return (retVal);
            }
#endif
        }
    }

    /* Stop driver */
    retVal = FVID2_stop(GrpxAppFvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    Vps_printf("%s: Graphics Driver Stopped!!\n", APP_NAME);

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
    while (1)
    {
        /* Dequeue buffers from driver */
        retVal = FVID2_dequeue(
                GrpxAppFvidHandle,
                &GrpxAppFrameList,
                0u,
                FVID2_TIMEOUT_NONE);
        if (FVID2_SOK != retVal)
        {
            break;
        }
    }

    /* Close driver */
    retVal = FVID2_delete(GrpxAppFvidHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Configure display controller - Remove the settings */
    retVal = vpsGrpxConfigDctrl(driverInst, FALSE);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return retVal;
    }

    if (Vps_platformIsEvm())
    {
        if (venc == VPS_DC_VENC_DVO2)
        {
#ifdef APP_GRPX_CONFIG_HDMI
            /* Close HDMI transmitter */
            retVal = FVID2_delete(GrpxHdmiHandle, NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf(
                    "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
                return retVal;
            }
#endif
        }
    }

    VpsUtils_memFree(AppGrpxBuffer, (TOTAL_NUM_BUFFERS * bufferSize));
    VpsUtils_memFree(stenData,(REGION_HEIGHT * stenPitch));
    VpsUtils_memFree(clutData,1024);

    Semaphore_delete(&GrpxTestCompleteSem);
    Semaphore_delete(&GrpxTestCallbackSem);

    Vps_printf("%s: GRPX Test Successfull!!\n", APP_NAME);

    return (retVal);
}



/**
 *  Driver callback function.
 */
static Int32 vpsGrpxAppCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32                    retVal = FVID2_SOK;

    Semaphore_post(GrpxTestCallbackSem);

    return (retVal);
}

static Int32 vpsGrpxProcessRegion(FVID2_Handle handle)
{
    Int32                    retVal = FVID2_SOK;
    UInt32 index;
    FVID2_Frame *frame;
    UInt32 width, height;
    /* Dequeue buffer from driver */
    retVal = FVID2_dequeue(
                     GrpxAppFvidHandle,
                     &GrpxAppFrameList,
                     0u,
                     FVID2_TIMEOUT_NONE);

    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    /* Fill the next set of buffers */
    if (NUM_BUFFERS == GrpxAppBufIndex)
    {
        GrpxAppBufIndex = 0u;
    }


    if (grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard == FVID2_STD_720P_60)
    {
        width = 1280;
        height = 720;
    }
    else
    {
        width = 1920;
        height = 1080;
    }

    GrpxAppBufIndex++;
    index = (GrpxAppBufIndex - 1u);
    frame = GrpxAppFrameList.frames[0u];

    frame->addr[FVID2_RGB_ADDR_IDX][FVID2_RGB_ADDR_IDX] = (void *)
           (AppGrpxBuffer + index * bufferSize);
    if (((GrpxAppDoneRegions % RT_SWITCH_RATE) == 0) &&
        (venc != VPS_DC_VENC_SD) &&
        (GrpxAppDoneRegions != 0))
    {

        if( TRUE == rtParams.regParams.scEnable)
            scEnableSet(FALSE);
        else
            scEnableSet(TRUE);

        if (VPS_GRPX_BLEND_NO == rtParams.regParams.blendType)
            rtParams.regParams.blendType = VPS_GRPX_BLEND_REGION_GLOBAL;
        else
            rtParams.regParams.blendType = VPS_GRPX_BLEND_NO;

        if(rtParams.regParams.scEnable == TRUE)
        {
            /* In worst case, scaler output two extra lines and two extra pixels*/
            rtParams.regParams.regionPosX += 15;
            if(rtParams.regParams.regionPosX + scParams.outWidth + 2 >= width)
            {
                rtParams.regParams.regionPosX = 10;
            }
            rtParams.regParams.regionPosY +=10;
            if(rtParams.regParams.regionPosY + scParams.outHeight + 2 >= height )
            {
                rtParams.regParams.regionPosY = 30;
            }
        }
        else
        {
            rtParams.regParams.regionPosX +=15;
            if(rtParams.regParams.regionPosX + REGION_WIDTH >= width)
            {
                rtParams.regParams.regionPosX = 10;
            }

            rtParams.regParams.regionPosY +=10;
            if(rtParams.regParams.regionPosY + REGION_HEIGHT >= height)
            {
                rtParams.regParams.regionPosY = 30;
            }
        }

        if (TRUE == rtParams.regParams.stencilingEnable)
            stenEnableSet(FALSE);
        else
            stenEnableSet(TRUE);


    }
     /* Queue the filled buffer back to the driver */
    retVal = FVID2_queue(GrpxAppFvidHandle, &GrpxAppFrameList, 0u);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    GrpxAppDoneRegions++;
    #ifdef PLATFORM_SIM
    if ((GrpxAppDoneRegions % RT_SWITCH_RATE) == 0)
        Vps_printf("%s: %d Regions Displayed\n", APP_NAME, GrpxAppDoneRegions);
    #endif
    if (GrpxAppDoneRegions >= TOTAL_LOOP_COUNT)
    {
        Semaphore_post(GrpxTestCompleteSem);
    }
    return (retVal);
}



/**
 *  Driver error callback function.
 */
static Int32 vpsGrpxAppErrCbFxn(FVID2_Handle handle,
                            Ptr appData,
                            Void *errList,
                            Ptr reserved)
{
    Int32                    retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}

#ifdef APP_GRPX_CONFIG_HDMI
/**
 *  App_dispConfigHdmi
 *  Configures the display controller to connect the paths, enabling muxes
 *  and configuring blenders and VENCs.
 */
static Int32 vpsGrpxConfigHdmi(FVID2_Handle *handle)
{
    Int32                           retVal;
    Vps_HdmiChipId                  hdmiId;
    Vps_SiI9022aHpdPrms             hpdPrms;
    Vps_SiI9022aModeParams          modePrms;
    Vps_VideoEncoderCreateParams    encCreateParams;
    Vps_VideoEncoderCreateStatus    encCreateStatus;
    FVID2_Handle                    hdmiHandle;
    Vps_PlatformBoardId             boardId;

    boardId = Vps_platformGetBoardId();
    if(boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("%s: UNKNOWN board detected, Cannot proceed\n",
                __FUNCTION__);
        return (FVID2_EFAIL);
    }

    /* Open HDMI Tx */
    encCreateParams.deviceI2cInstId = Vps_platformGetI2cInstId();
    encCreateParams.deviceI2cAddr =
            Vps_platformGetVidEncI2cAddr(FVID2_VPS_VID_ENC_SII9022A_DRV);
    encCreateParams.inpClk = 0u;
    encCreateParams.hdmiHotPlugGpioIntrLine = 0u;
    if (VPS_PLATFORM_BOARD_VC == boardId)
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EXTERNAL_SYNC;
    }
    else
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EMBEDDED_SYNC;
    }
    encCreateParams.clkEdge = FALSE;

    if (boardId == VPS_PLATFORM_BOARD_VC)
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EXTERNAL_SYNC;
    }
    *handle = NULL;
    hdmiHandle = FVID2_create(
                             FVID2_VPS_VID_ENC_SII9022A_DRV,
                             0u,
                             &encCreateParams,
                             &encCreateStatus,
                             NULL);
    if (NULL == hdmiHandle)
    {
        Vps_printf(
            "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }

    retVal = FVID2_control(
                 hdmiHandle,
                 IOCTL_VPS_SII9022A_GET_DETAILED_CHIP_ID,
                 &hdmiId,
                 NULL);
    if (FVID2_SOK == retVal)
    {
        Vps_printf("%s: DevId %x Prod RevId %x TPI RevId %x HDCP RevId %x\n",
            APP_NAME, hdmiId.deviceId, hdmiId.deviceProdRevId, hdmiId.tpiRevId,
            hdmiId.hdcpRevTpi);
    }
    else
    {
        Vps_printf("Could not Get Detailed Chip Id\n");
        return (retVal);
    }

    retVal = FVID2_control(
                 hdmiHandle,
                 IOCTL_VPS_SII9022A_QUERY_HPD,
                 &hpdPrms,
                 NULL);
    if (FVID2_SOK == retVal)
    {
        Vps_printf("%s: hpdEvtPending %x busError %x hpdStatus %x\n",
            APP_NAME, hpdPrms.hpdEvtPending, hpdPrms.busError, hpdPrms.hpdStatus);
    }
    else
    {
        Vps_printf("Could not Get HPD\n");
        return (retVal);
    }

    modePrms.standard = grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard;
    retVal = FVID2_control(
                 hdmiHandle,
                 IOCTL_VPS_VIDEO_ENCODER_SET_MODE,
                 &modePrms,
                 NULL);
    if (FVID2_SOK == retVal)
    {
        Vps_printf("%s: ModeSet in HDMI\n", APP_NAME);
    }
    else
    {
        Vps_printf("Could not Set in HDMI\n");
    }

    *handle = hdmiHandle;
    return (retVal);
}
#endif

/**
 *  Configures the display controller to connect the paths, enabling muxes
 *  and configuring blenders and VENCs.
 */
static Int32 vpsGrpxConfigDctrl(UInt32 driverInst, UInt32 clear)
{
    Int32                    retVal= FVID2_SOK;
    Vps_DcOutputInfo        dcOutputInfo;
    Vps_DcVencClkSrc        clkSrc;

    if (VPS_DISP_INST_GRPX0 == driverInst)
    {
        grpxDcConfig.edgeInfo[0].startNode = VPS_DC_GRPX0_INPUT_PATH;
    }
    else if (VPS_DISP_INST_GRPX1 == driverInst)
    {
        grpxDcConfig.edgeInfo[0].startNode = VPS_DC_GRPX1_INPUT_PATH;

    }
    else
    {
        grpxDcConfig.edgeInfo[0].startNode = VPS_DC_GRPX2_INPUT_PATH;
    }

    if (TRUE == clear)
    {

        dcOutputInfo.dvoFidPolarity = VPS_DC_POLARITY_ACT_HIGH;
        dcOutputInfo.dvoVsPolarity = VPS_DC_POLARITY_ACT_HIGH;
        dcOutputInfo.dvoHsPolarity = VPS_DC_POLARITY_ACT_HIGH;
        dcOutputInfo.dvoActVidPolarity = VPS_DC_POLARITY_ACT_HIGH;
        switch(venc)
        {
            case VPS_DC_VENC_HDCOMP:
                if (platformId == VPS_PLATFORM_ID_EVM_TI816x)
                {
                    dcOutputInfo.vencNodeNum = VPS_DC_VENC_HDCOMP;
                    dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC;
                    dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPONENT;
                    dcOutputInfo.dataFormat = FVID2_DF_YUV422SP_UV;
                    if (grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard ==
                            FVID2_STD_1080P_60)
                    {
                        retVal =
                            Vps_ths7360SetSfParams(VPS_THS7360_SF_TRUE_HD_MODE);
                    }
                    else
                    {
                        retVal = Vps_ths7360SetSfParams(VPS_THS7360_SF_HD_MODE);
                    }
                    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
                    clkSrc.venc = VPS_DC_VENC_HDCOMP;
                }
                break;
            case VPS_DC_VENC_DVO2:
                if (boardId == VPS_PLATFORM_BOARD_VC)
                {
                    dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
                    dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
                }
                else
                {
                    dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_DOUBLECHAN;
                    dcOutputInfo.dataFormat = FVID2_DF_YUV422SP_UV;
                }
#ifdef APP_GRPX_CONFIG_LCD_OUTPUT
                dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
                dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
#endif
                dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
                dcOutputInfo.vencNodeNum = VPS_DC_VENC_DVO2;
                clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
                if ((platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                    (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                {
                    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD_DIV2;
                }
                if (platformId == VPS_PLATFORM_ID_EVM_TI814x)
                {
                    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
                }
                clkSrc.venc = VPS_DC_VENC_DVO2;

            break;
            case VPS_DC_VENC_SD:
                dcOutputInfo.vencNodeNum = VPS_DC_VENC_SD;
                if ((VPS_PLATFORM_ID_EVM_TI816x == platformId) ||
                    (VPS_PLATFORM_ID_EVM_TI8107 == platformId))
                {
                    dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
                    retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_ENABLE_MODULE);
                }
                else
                {
                    dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_SVIDEO;
                }
                dcOutputInfo.dataFormat = FVID2_DF_YUV422SP_UV;
                break;
        }


        retVal |= FVID2_control(
                     GrpxAppDcHandle,
                     IOCTL_VPS_DCTRL_SET_VENC_OUTPUT,
                     &dcOutputInfo,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Display Controller set output failed!!\n",
                APP_NAME);
            return (retVal);
        }
        if (venc != VPS_DC_VENC_SD)
        {
            retVal |= FVID2_control(GrpxAppDcHandle,
                IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC,
                &clkSrc,
                NULL);
        }
        /* Create a mesh for graphics path connected to the HDMI */
        retVal |= FVID2_control(
                     GrpxAppDcHandle,
                     IOCTL_VPS_DCTRL_SET_CONFIG,
                     &grpxDcConfig,
                     NULL);
    }
    else
    {
        if (VPS_PLATFORM_ID_EVM_TI816x == platformId)
        {
            retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_DISABLE_MODULE);
            retVal = Vps_ths7360SetSfParams(VPS_THS7360_DISABLE_SF);
        }

        /* Remove the already created path */
        retVal = FVID2_control(
                     GrpxAppDcHandle,
                     IOCTL_VPS_DCTRL_CLEAR_CONFIG,
                     &grpxDcConfig,
                     NULL);
    }
    if (FVID2_SOK == retVal)
    {
        Vps_printf("%s: Display Controller Path configured!!\n", APP_NAME);
    }
    else
    {
        Vps_printf("%s: Display Controller Path configuration failed!!\n", APP_NAME);
    }

    return (retVal);
}



/**
 *  Sets the required/default format.
 */
static Void vpsGrpxSetFormat(FVID2_Format *fmt)
{
    UInt32 cnt;

    fmt->channelNum = 0u;
    fmt->width = REGION_WIDTH;
    fmt->height = REGION_HEIGHT;
    fmt->scanFormat = FVID2_SF_PROGRESSIVE;
    for (cnt =0; cnt< FVID2_MAX_PLANES;cnt++)
    {
        fmt->fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    }
    fmt->dataFormat = GRPX_FORMAT;
    if(GRPX_BPP == 32u)
    {
        fmt->bpp = FVID2_BPP_BITS32;
    }
    else if(GRPX_BPP == 24u)
    {
         fmt->bpp = FVID2_BPP_BITS24;
    }
    else if(GRPX_BPP == 16u)
    {
         fmt->bpp = FVID2_BPP_BITS16;
    }
    else if(GRPX_BPP == 8u)
    {
         fmt->bpp = FVID2_BPP_BITS8;
    }
    else if(GRPX_BPP == 4u)
    {
         fmt->bpp = FVID2_BPP_BITS4;
    }
    else if(GRPX_BPP == 2u)
    {
         fmt->bpp = FVID2_BPP_BITS2;
    }
    else if(GRPX_BPP == 1u)
    {
         fmt->bpp = FVID2_BPP_BITS1;
    }

    fmt->pitch[FVID2_RGB_ADDR_IDX] = dataPitch;
    fmt->reserved = NULL;
}

static Void vpsGrpxSetParams(void)
{

    rtParams.regParams.regionPosX = 0;
    rtParams.regParams.regionPosY = 0;
    rtParams.regParams.firstRegion = 1u;
    rtParams.regParams.lastRegion = 1u;
    rtParams.regParams.regionWidth = REGION_WIDTH;
    rtParams.regParams.regionHeight = REGION_HEIGHT;
    rtParams.regParams.dispPriority = 1;
    rtParams.regParams.bbEnable = 0;
    rtParams.regParams.bbAlpha = 20;
    rtParams.regParams.blendAlpha = 40;
    rtParams.regParams.blendType = VPS_GRPX_BLEND_NO;
    rtParams.regId = 0;
    rtParams.format = GRPX_FORMAT;
    rtParams.rotation = VPS_MEM_0_ROTATION;
    rtParams.pitch[FVID2_RAW_ADDR_IDX] = dataPitch;
    /*stride should be aligned on 16bytes*/
    rtParams.stenPitch = stenPitch;
    /*all rtParams share the same regId, this is only for the single region display*/
    rtParams.stenPtr = stenData;
    VpsUtils_memset(stenData,0xF, stenPitch * REGION_HEIGHT);
    if (venc != VPS_DC_VENC_SD)
    {
        scEnableSet(TRUE);
        stenEnableSet(FALSE);
    }
    return;
}



/**
 *  Initialize the global variables and frame pointers.
 */
static Void vpsGrpxInitVariables(void)
{
    UInt32                   index;
    UInt32                   frmListCnt;

    GrpxAppBufIndex = 0;
    GrpxAppDoneRegions = 0;
    index = 0;
    GrpxAppFrameList.numFrames = NUM_REGIONS_PER_LIST;
    GrpxAppFrameList.perListCfg = NULL;
    GrpxAppFrameList.reserved = NULL;

    for (frmListCnt = 0u; frmListCnt < NUM_FRAMELIST; frmListCnt++)
    {

        GrpxAppFrameList.frames[frmListCnt] = &GrpxAppFrames[frmListCnt];
        GrpxAppFrames[frmListCnt].channelNum = 0u;
        GrpxAppFrames[frmListCnt].appData = NULL;
        GrpxAppFrames[frmListCnt].perFrameCfg = (Ptr)&rtParams;
        GrpxAppFrames[frmListCnt].subFrameInfo = NULL;
        GrpxAppFrames[frmListCnt].reserved = NULL;
        GrpxAppFrames[frmListCnt].fid = FVID2_FID_FRAME;
        GrpxAppFrames[frmListCnt].addr[FVID2_RGB_ADDR_IDX]
            [FVID2_RGB_ADDR_IDX] = (void *)
            (AppGrpxBuffer + index * bufferSize);
        index++;
    }

    /* Init error frame list */
    memset(&GrpxAppErrFrameList, 0u, sizeof (GrpxAppErrFrameList));

    List.numRegions = 1u;
    List.gParams = &rtParams;
    List.clutPtr = clutData;
    List.scParams = &scParams;

    rtParams.scParams = NULL;
    rtParams.regId = index;
    rtParams.format = FVID2_DF_ARGB32_8888;
    rtParams.pitch[0] = 0;
    rtParams.stenPitch = 0;
    VpsUtils_memset(&rtParams.regParams, 0u, sizeof(Vps_GrpxRegionParams));

    /* This is not the real CLUT data, it just a sample data to test*/
    if (clutData != NULL)
    {
        for (index=0;index<256;index++)
        {
             clutData[index] = index;
        }
    }
    scParams.horFineOffset = 0;
    scParams.verFineOffset = 0;
    scParams.inHeight = REGION_HEIGHT;
    scParams.inWidth = REGION_WIDTH ;
    scParams.outHeight = REGION_HEIGHT * 1.5;
    scParams.outWidth = REGION_WIDTH  * 1.5;
    scParams.scCoeff = NULL;


}
static Int32 vpsGrpxGetPlatformData(void)
{
        /* Get platform type, board and CPU revisions */
    platformId = Vps_platformGetId();
    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf("%s: Error Unrecognized platform @ line %d\n",
            __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }
    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        Vps_printf("%s: Error Unrecognized CPU version @ line %d\n",
            __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }
    boardId = Vps_platformGetBoardId();
    if (boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("%s: Error Unrecognized board @ line %d\n",
            __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }

    return FVID2_SOK;
}
static Int32 setPixelClk(UInt32 standard)
{
    Int32                   retVal = FVID2_SOK;
    FVID2_Handle            systemDrvHandle;
    Vps_SystemVPllClk       vpllCfg;

    systemDrvHandle = FVID2_create(
                        FVID2_VPS_VID_SYSTEM_DRV,
                        0,
                        NULL,
                        NULL,
                        NULL);
    if (NULL == systemDrvHandle)
    {
        Vps_printf(
            "%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }


    /* Initialize the test parameters */
    switch (venc)
    {
#ifdef TI_816X_BUILD
        case VPS_DC_VENC_HDCOMP:
            grpxDcConfig.edgeInfo[0].endNode = VPS_DC_HDCOMP_BLEND;
            grpxDcConfig.vencInfo.modeInfo[0].vencId = VPS_DC_VENC_HDCOMP;
            vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_A;
            break;
#endif
        case VPS_DC_VENC_DVO2:
            grpxDcConfig.edgeInfo[0].endNode = VPS_DC_DVO2_BLEND;
            grpxDcConfig.vencInfo.modeInfo[0].vencId = VPS_DC_VENC_DVO2;
            vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;

            break;
        case VPS_DC_VENC_SD:
            grpxDcConfig.edgeInfo[0].endNode = VPS_DC_SDVENC_BLEND;
            grpxDcConfig.vencInfo.modeInfo[0].vencId = VPS_DC_VENC_SD;
            vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_RF;
            break;

    }
    switch (standard)
    {
        case 0:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_1080P_60;
            if ((venc == VPS_DC_VENC_DVO2) &&
                (platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                    vpllCfg.outputClk = 297000u;
            else
                vpllCfg.outputClk = 148500u;
            break;

        case 1:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_1080P_30;
            if ((venc == VPS_DC_VENC_DVO2) &&
                (platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                vpllCfg.outputClk = 148500u;
            else
                vpllCfg.outputClk = 74250u;
            break;

        case 2:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_1080I_60;
            if ((venc == VPS_DC_VENC_DVO2) &&
                (platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                vpllCfg.outputClk = 148500u;
            else
                vpllCfg.outputClk = 74250u;
            break;

        case 3:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_720P_60;
            if ((venc == VPS_DC_VENC_DVO2) &&
                (platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                vpllCfg.outputClk = 148500u;
            else
                vpllCfg.outputClk = 74250u;
            break;
        case 4:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_1080P_50;
            if ((venc == VPS_DC_VENC_DVO2) &&
                (platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                vpllCfg.outputClk = 297000u;
            else
                vpllCfg.outputClk = 148500u;

            break;
        case 5:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_1080I_50;
            if ((venc == VPS_DC_VENC_DVO2) &&
                (platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                vpllCfg.outputClk = 148500u;
            else
                vpllCfg.outputClk = 74250u;
            break;
        case 6:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_720P_50;
            if ((venc == VPS_DC_VENC_DVO2) &&
                (platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (cpuRev == VPS_PLATFORM_CPU_REV_1_0))
                vpllCfg.outputClk = 148500u;
            else
                vpllCfg.outputClk = 74250;

            break;
#ifdef APP_GRPX_CONFIG_LCD_OUTPUT
        case 7:
            vpllCfg.outputClk = 50800;
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_CUSTOM;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.scanFormat = FVID2_SF_PROGRESSIVE;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.width = 1024u;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.height = 600u;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.pixelClock = 50800;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.hFrontPorch = 36;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.hBackPorch = 60;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.hSyncLen = 30;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.vFrontPorch = 10;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.vBackPorch = 11;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.vSyncLen = 10;
            grpxDcConfig.vencInfo.modeInfo[0u].mInfo.fps = 70;
            grpxDcConfig.vencInfo.modeInfo[0u].mode = 1u;
            break;
#endif

        case 8:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_NTSC;
            if ((platformId == VPS_PLATFORM_ID_EVM_TI816x) ||
                (platformId == VPS_PLATFORM_ID_SIM_TI816x))
                vpllCfg.outputClk = 216000u;
            else
                vpllCfg.outputClk = 54000u;
            break;
        case 9:
            grpxDcConfig.vencInfo.modeInfo[0].mInfo.standard =
                FVID2_STD_PAL;
            if ((platformId == VPS_PLATFORM_ID_EVM_TI816x) ||
                (platformId == VPS_PLATFORM_ID_SIM_TI816x))
                vpllCfg.outputClk = 216000u;
            else
                vpllCfg.outputClk = 54000u;

            break;
        default:
            vpllCfg.outputClk = 148500u;
            break;
    }

    retVal = FVID2_control(
                systemDrvHandle,
                IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                &vpllCfg,
                NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return retVal;
    }

    FVID2_delete(systemDrvHandle, NULL);

    return retVal;
}

static void scEnableSet(UInt32 Flag)
{
    rtParams.regParams.scEnable = Flag;
}

static void stenEnableSet(UInt32 Flag)
{
#ifdef TI_816X_BUILD
    rtParams.regParams.stencilingEnable = Flag;
#endif
}
