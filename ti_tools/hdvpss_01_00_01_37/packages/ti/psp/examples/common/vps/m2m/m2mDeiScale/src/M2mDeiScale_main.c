/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file M2mDeiScale_main.c
 *
 *  \brief Main file invoking the test function.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
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
#include <ti/psp/examples/utility/vpsutils_tiler.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>
#include <ti/psp/examples/utility/vpsutils_app.h>

#include "M2mDeiScale_utils.h"


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
extern Task_Handle App_m2mDeiCreateTsk(void);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppMainTskStack, 32)
#pragma DATA_SECTION(AppMainTskStack, ".bss:taskStackSection")
static UInt8 AppMainTskStack[APP_TSK_STACK_SIZE];

/* Test complete semaphore used to sync between test task and main task. */
Semaphore_Handle AppTestCompleteSem;


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
    char                        ch;
    Int32                       retVal;
    Semaphore_Params            semPrms;
    Task_Handle                 m2mDeiTaskHandle;
    Vps_PlatformInitParams      platformInitPrms;

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
        System_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    VpsUtils_memInit();
    VpsUtils_tilerInit();
    VpsUtils_tilerDebugLogEnable(FALSE);

    /* Get the version string */
    Vps_printf("%s: HDVPSS Drivers Version: %s\n",
        APP_NAME, FVID2_getVersionString());

    /* Init FVID2 and VPS */
    retVal = FVID2_init(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Initialize the vpsUtils to get the load */
    VpsUtils_prfInit();
    VpsUtils_appInit();

    while (1u)
    {
        /* Create test task */
        m2mDeiTaskHandle = App_m2mDeiCreateTsk();
        if (NULL == m2mDeiTaskHandle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }

        /* Register the task to the load module for calculating the load */
        VpsUtils_prfLoadRegister(m2mDeiTaskHandle, APP_NAME":");

        /* Wait for test to complete */
        Semaphore_pend(AppTestCompleteSem, BIOS_WAIT_FOREVER);

        /* UnRegister the task from the load module */
        VpsUtils_prfLoadUnRegister(m2mDeiTaskHandle);

        /* Delete the task */
        Task_delete(&m2mDeiTaskHandle);

        fflush(stdin);
        Vps_printf("Enter 0 to exit or any other key to continue testing...\n");
        VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);
        if ('0' == ch)
        {
            break;
        }
    }

    /* De-Init Sequence */
    VpsUtils_appDeInit();
    VpsUtils_prfDeInit();
    retVal = FVID2_deInit(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
    }
    VpsUtils_tilerDeInit();
    VpsUtils_memDeInit();
    Vps_platformDeInit();

    Semaphore_delete(&AppTestCompleteSem);

    Vps_printf("%s: Test Successfull!!\n", APP_NAME);

    return;
}
