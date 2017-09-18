/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file TriDisplay_main.c
 *
 *  \brief Main file invoking the triple display test function.
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

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/devices/vps_thsfilters.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_mem.h>
#include <ti/psp/examples/utility/vpsutils_app.h>
#include <ti/psp/examples/common/vps/display/triDisplay/src/TriDisplay.h>
#include <ti/psp/devices/vps_sii9022a.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (10u * 1024u)

#define APP_USE_OFF_CHIP_HDMI

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void App_mainTask(void);
extern Task_Handle App_createDispTestTsk(App_DispComboObj *combObj);
static Int32 App_dispSetVencOutput(FVID2_Handle dctrlHandle);
static Int32 App_dispSelectClkSrc(FVID2_Handle dctrlHandle);
static Int32 App_dispSetPixClk(Vps_SystemVPllClk *vpllCfg);
static Int32 App_dispConfigPixClk();

#ifdef APP_USE_OFF_CHIP_HDMI
static Int32 App_dispInitHdmi(FVID2_Handle *hdmiHandle);
static Int32 App_dispDeInitHdmi(FVID2_Handle *hdmiHandle);
#endif

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppMainTaskStack, 32)
#pragma DATA_SECTION(AppMainTaskStack, ".bss:taskStackSection")
static UInt8 AppMainTaskStack[APP_TSK_STACK_SIZE];

/* Test complete semaphore used to sync between display and main task. */
Semaphore_Handle AppTestCompleteSem;

/* Display Controller Configuration */
#ifdef TI_816X_BUILD

static App_DispComboObj AppDispComboObj[] =
{
    {
        "(HDMI & GRPX)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_HDMI_BLEND},
                {VPS_DC_GRPX1_INPUT_PATH, VPS_DC_DVO2_BLEND},
            },
            8u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                },
                0u,                             /* Tied VENC bit mask */
                2u                              /* Number of VENCs */
            }
        },
        {TRUE, FALSE, FALSE},
        {TRUE, FALSE, FALSE}
    },
    {
        "(HDMI & GRPX) + (DVO2 & GRPX)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_HDMI_BLEND},
                {VPS_DC_GRPX1_INPUT_PATH, VPS_DC_DVO2_BLEND},
            },
            8u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                },
                0u,                             /* Tied VENC bit mask */
                2u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, FALSE},
        {TRUE, TRUE, FALSE}
    },
    {
        "(HDMI) + (DVO2)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
            },
            6u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                },
                0u,                             /* Tied VENC bit mask */
                2u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, FALSE},
        {FALSE, FALSE, FALSE}
    },
    {
        "(HDMI) + (DVO2) + SD Display",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_SEC1_INPUT_PATH, VPS_DC_SDVENC_MUX},
                {VPS_DC_SDVENC_MUX, VPS_DC_SDVENC_BLEND},
            },
            8u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_SD, {FVID2_STD_NTSC}}
                },
                0u,                             /* Tied VENC bit mask */
                3u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, TRUE},
        {FALSE, FALSE, FALSE}
    },
    {
        "(HDMI & GRPX) + (DVO2 & GRPX) + SD Display",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_SEC1_INPUT_PATH, VPS_DC_SDVENC_MUX},
                {VPS_DC_SDVENC_MUX, VPS_DC_SDVENC_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_HDMI_BLEND},
                {VPS_DC_GRPX1_INPUT_PATH, VPS_DC_DVO2_BLEND},
            },
            10u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_SD, {FVID2_STD_NTSC}}
                },
                0u,                             /* Tied VENC bit mask */
                3u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, TRUE},
        {TRUE, TRUE, FALSE}
    },
    {
        "(HDMI) + (DVO2 & GRPX)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_DVO2_BLEND},
            },
            7u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                },
                0u,                             /* Tied VENC bit mask */
                2u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, FALSE},
        {TRUE, FALSE, FALSE}
    },
    {
        "(HDMI) + (DVO2 & GRPX)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_DVO2_BLEND},
            },
            7u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                },
                0u,                             /* Tied VENC bit mask */
                2u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, FALSE},
        {TRUE, FALSE, FALSE}
    },
    {
        "(DVO2 & GRPX1 & GRPX2)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX1_INPUT_PATH, VPS_DC_DVO2_BLEND},
            },
            5u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                },
                0u,                             /* Tied VENC bit mask */
                1u                              /* Number of VENCs */
            }
        },
        {TRUE, FALSE, FALSE},
        {TRUE, TRUE, FALSE}
    },
    {
        "(HDMI & GRPX1 & GRPX2)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_HDMI_BLEND},
                {VPS_DC_GRPX1_INPUT_PATH, VPS_DC_HDMI_BLEND},
            },
            5u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                },
                0u,                             /* Tied VENC bit mask */
                1u                              /* Number of VENCs */
            }
        },
        {TRUE, FALSE, FALSE},
        {TRUE, TRUE, FALSE}
    },
    {
        "(HDMI & GRPX0) + (DVO2 & GRPX1) + (SD Display + GRPX2)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP0_INPUT_PATH, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP1_INPUT_PATH, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_SEC1_INPUT_PATH, VPS_DC_SDVENC_MUX},
                {VPS_DC_SDVENC_MUX, VPS_DC_SDVENC_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_HDMI_BLEND},
                {VPS_DC_GRPX1_INPUT_PATH, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX2_INPUT_PATH, VPS_DC_SDVENC_BLEND},
            },
            11u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_SD, {FVID2_STD_NTSC}}
                },
                0u,                             /* Tied VENC bit mask */
                3u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, TRUE},
        {TRUE, TRUE, TRUE}
    }
};

#else
static App_DispComboObj AppDispComboObj[] =
{
    {
        "(HDMI & GRPX) + (DVO2 & GRPX)",
        {
            VPS_DC_USERSETTINGS,                /* Use Case */
            /* Edge information */
            {
                {VPS_DC_BP1_INPUT_PATH/*VPS_DC_BP0_INPUT_PATH */, VPS_DC_VCOMP_MUX},
                {VPS_DC_VCOMP_MUX, VPS_DC_VCOMP},
                {VPS_DC_CIG_NON_CONSTRAINED_OUTPUT, VPS_DC_HDMI_BLEND},
                {VPS_DC_BP0_INPUT_PATH/*VPS_DC_BP1_INPUT_PATH*/, VPS_DC_HDCOMP_MUX},
                {VPS_DC_HDCOMP_MUX, VPS_DC_CIG_PIP_INPUT},
                {VPS_DC_CIG_PIP_OUTPUT, VPS_DC_DVO2_BLEND},
                {VPS_DC_SEC1_INPUT_PATH, VPS_DC_SDVENC_MUX},
                {VPS_DC_SDVENC_MUX, VPS_DC_SDVENC_BLEND},
                {VPS_DC_GRPX0_INPUT_PATH, VPS_DC_HDMI_BLEND},
                {VPS_DC_GRPX1_INPUT_PATH, VPS_DC_DVO2_BLEND},
                {VPS_DC_GRPX2_INPUT_PATH, VPS_DC_SDVENC_BLEND},
            },
            11u,                                /* Number of edges */
            /* VENC information */
            {
                /* Mode information */
                {
                    {VPS_DC_VENC_HDMI, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_DVO2, {FVID2_STD_1080I_60}},
                    {VPS_DC_VENC_SD, {FVID2_STD_NTSC}}
                },
                0u,                             /* Tied VENC bit mask */
                3u                              /* Number of VENCs */
            }
        },
        {TRUE, TRUE, TRUE},
        {TRUE, TRUE, TRUE}
    }
};
#endif

#define APP_NUM_DISP_COMBO  ((sizeof(AppDispComboObj)) /                       \
                             (sizeof(App_DispComboObj)))

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
    tskParams.stack = AppMainTaskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    Task_create((Task_FuncPtr) App_mainTask, &tskParams, NULL);

    BIOS_start();

    return (0);
}



/**
 *  App_mainTask
 *  Application test task.
 */
static Void App_mainTask(void)
{
    char                ch, inputStr[10];
    Int32               retVal, driverRetVal;
    UInt32              cnt, comboNo;
    FVID2_Handle        dctrlHandle;
    Task_Handle         dispTaskHandle;
    Semaphore_Params    semPrms;
    const Char          *versionStr;
    Vps_PlatformDeviceInitParams deviceInitPrms;
    Vps_PlatformInitParams       platformInitPrms;
    FVID2_Handle        hdmiHandle;
    Vps_PlatformId      platformId;

    platformInitPrms.isPinMuxSettingReq = TRUE;
    retVal = Vps_platformInit(&platformInitPrms);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Create semaphore */
    Semaphore_Params_init(&semPrms);
    AppTestCompleteSem = Semaphore_create(0, &semPrms, NULL);
    if (NULL == AppTestCompleteSem)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return;
    }

    /* Utils Init */
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
    retVal = Vps_platformDeviceInit(&deviceInitPrms);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    VpsUtils_appInit();

    platformId = Vps_platformGetId();
    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf(
            "%s: Error Unrecognized platform@ line %d\n",
            __FUNCTION__, __LINE__);
        return;
    }

    if (VPS_PLATFORM_ID_EVM_TI816x == platformId)
    {
        /* Enable SD filter in the THS7360 */
        retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_ENABLE_MODULE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }

        /* Enable SD filter in the THS7360 */
        retVal = Vps_ths7360SetSfParams(VPS_THS7360_SF_TRUE_HD_MODE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }

    /* Open display controller */
    dctrlHandle = FVID2_create(
                      FVID2_VPS_DCTRL_DRV,
                      VPS_DCTRL_INST_0,
                      NULL,
                      &driverRetVal,
                      NULL);
    if (NULL == dctrlHandle)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, driverRetVal, __LINE__);
        return;
    }

    retVal = App_dispSetVencOutput(dctrlHandle);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return ;
    }

    retVal = App_dispSelectClkSrc(dctrlHandle);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return ;
    }

    retVal = App_dispConfigPixClk();
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return ;
    }

    /* Select the combo */
    while(1u)
    {
        do
        {
            Vps_printf(
                "--------------------------------------------------------\n");
            Vps_printf("Select the combo to run\n");
            Vps_printf(
                "--------------------------------------------------------\n");
            for (cnt = 0u; cnt < APP_NUM_DISP_COMBO; cnt ++)
            {
                Vps_printf("%d - %s\n", cnt, AppDispComboObj[cnt].comboName);
            }

            Vps_printf("%s: Enter any of above value and press enter...\n",
                APP_NAME);
            VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
            comboNo = atoi(inputStr);

            if (comboNo >= APP_NUM_DISP_COMBO)
            {
                Vps_printf("Invalid Combo Selected\n");
                Vps_printf("Try Again\n");
            }
        } while(comboNo >= APP_NUM_DISP_COMBO);

         /* Configure display controller */
        retVal = FVID2_control(
                     dctrlHandle,
                     IOCTL_VPS_DCTRL_SET_CONFIG,
                     &AppDispComboObj[comboNo].dcConfig,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
            return;
        }
        Vps_printf("%s: Display controller configured!!\n", APP_NAME);

#ifdef APP_USE_OFF_CHIP_HDMI
        /* Config HDMI */
        retVal = App_dispInitHdmi(&hdmiHandle);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal,
                __LINE__);
            return ;
        }
#endif

        /* Create display test task */
        dispTaskHandle = App_createDispTestTsk(&AppDispComboObj[comboNo]);
        if (NULL == dispTaskHandle)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }

        /* Wait for display test to complete */
        Semaphore_pend(AppTestCompleteSem, BIOS_WAIT_FOREVER);

        /* Delete the display task */
        Task_delete(&dispTaskHandle);

#ifdef APP_USE_OFF_CHIP_HDMI
        /* Config HDMI */
        retVal = App_dispDeInitHdmi(&hdmiHandle);
        if (FVID2_SOK != retVal)
        {
            Vps_printf(
                "%s: Error %d @ line %d\n", __FUNCTION__, retVal,
                __LINE__);
            return ;
        }
#endif

        /* Remove display controller configuration */
        retVal = FVID2_control(
                     dctrlHandle,
                     IOCTL_VPS_DCTRL_CLEAR_CONFIG,
                     &AppDispComboObj[comboNo].dcConfig,
                     NULL);
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

    /* Close display controller */
    retVal = FVID2_delete(dctrlHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    if (VPS_PLATFORM_ID_EVM_TI816x == platformId)
    {
        /* Disable SD filter in the THS7360 */
        retVal = Vps_ths7360SetSdParams(VPS_THSFILTER_DISABLE_MODULE);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }

        /* Disable HDDAC filter in the THS7360 */
        retVal = Vps_ths7360SetSfParams(VPS_THS7360_DISABLE_SF);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return;
        }
    }

    /* DeInit FVID2 and VPS */
    VpsUtils_appDeInit();
    retVal = FVID2_deInit(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    /* Utils Deinit */
    VpsUtils_memDeInit();

    Semaphore_delete(&AppTestCompleteSem);

    Vps_printf("Test Successful!!\n");
}



/**
 *  App_dispSetVencOutput
 *  Configure Venc output.
 */
static Int32 App_dispSetVencOutput(FVID2_Handle dctrlHandle)
{
    Int32               retVal = FVID2_EFAIL;
    UInt32              boardId,platformId;
    Vps_DcOutputInfo    dcOutputInfo;

    platformId = Vps_platformGetId();
    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf("%s: Error Unrecognized platform @ line %d\n",
            __FUNCTION__, __LINE__);
        return retVal;
    }

    dcOutputInfo.vencNodeNum = VPS_DC_VENC_HDMI;
    dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
    dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
    dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
    dcOutputInfo.dvoFidPolarity = VPS_DC_POLARITY_ACT_HIGH;
    dcOutputInfo.dvoVsPolarity = VPS_DC_POLARITY_ACT_HIGH;
    dcOutputInfo.dvoHsPolarity = VPS_DC_POLARITY_ACT_HIGH;
    dcOutputInfo.dvoActVidPolarity = VPS_DC_POLARITY_ACT_HIGH;
    retVal = FVID2_control(
                 dctrlHandle,
                 IOCTL_VPS_DCTRL_SET_VENC_OUTPUT,
                 &dcOutputInfo,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    boardId = Vps_platformGetBoardId();
    if(boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("%s: UNKNOWN board detected, Defaulting to VS Board\n",
                APP_NAME);
        boardId = VPS_PLATFORM_BOARD_VS;
    }

    dcOutputInfo.vencNodeNum = VPS_DC_VENC_DVO2;
    if (VPS_PLATFORM_BOARD_VC == boardId)
    {
        dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_DISCSYNC;
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
        dcOutputInfo.dataFormat = FVID2_DF_RGB24_888;
    }
    else
    {
        dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_DOUBLECHAN;
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
        dcOutputInfo.dataFormat = FVID2_DF_YUV422SP_UV;
    }
    retVal = FVID2_control(
                 dctrlHandle,
                 IOCTL_VPS_DCTRL_SET_VENC_OUTPUT,
                 &dcOutputInfo,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    dcOutputInfo.vencNodeNum = VPS_DC_VENC_HDCOMP;
    dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC;
    dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPONENT;
    dcOutputInfo.dataFormat = FVID2_DF_YUV444P;
    retVal = FVID2_control(
                 dctrlHandle,
                 IOCTL_VPS_DCTRL_SET_VENC_OUTPUT,
                 &dcOutputInfo,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    dcOutputInfo.vencNodeNum = VPS_DC_VENC_SD;
    dcOutputInfo.dvoFmt = VPS_DC_DVOFMT_TRIPLECHAN_EMBSYNC;
    if ((VPS_PLATFORM_ID_EVM_TI816x == platformId) ||
        (VPS_PLATFORM_ID_EVM_TI8107 == platformId))
    {
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_COMPOSITE;
    }
    else
    {
        /* For 814x, sVideo is the default. Composite video out requires
           base board modification. */
        dcOutputInfo.aFmt = VPS_DC_A_OUTPUT_SVIDEO;
    }

    dcOutputInfo.dataFormat = FVID2_DF_YUV444P;
    retVal = FVID2_control(
                 dctrlHandle,
                 IOCTL_VPS_DCTRL_SET_VENC_OUTPUT,
                 &dcOutputInfo,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    return (FVID2_SOK);
}



/**
 *  App_dispSelectClkSrc
 *  Select the clock source for the DVO2 and HDCOMP.
 */
static Int32 App_dispSelectClkSrc(FVID2_Handle dctrlHandle)
{
    Int32               retVal;
    Vps_DcVencClkSrc    clkSrc;
    Vps_PlatformId      platformId;
    Vps_PlatformCpuRev  cpuRev;

    platformId = Vps_platformGetId();
    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf(
            "%s: Error Unrecognized platform@ line %d\n",
            __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }
    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        Vps_printf(
            "%s: Error Unrecognized CPU version@ line %d\n",
            __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }

    if (platformId != VPS_PLATFORM_ID_EVM_TI814x)
    {
            clkSrc.venc = VPS_DC_VENC_HDCOMP;
            clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
            retVal = FVID2_control(
                         dctrlHandle,
                         IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC,
                         &clkSrc,
                         NULL);
            if (FVID2_SOK != retVal)
            {
                Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                return (retVal);
            }
    }
    clkSrc.venc = VPS_DC_VENC_DVO2;
    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD_DIV2;
    if (((platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
            (cpuRev > VPS_PLATFORM_CPU_REV_1_0)) ||
        platformId == VPS_PLATFORM_ID_EVM_TI814x)
    {
        clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
    }
    /* Clock Source is not selectable in TI814X. It is fixed to VENCD
       clock for HDMI and VENCA clock for DVO2. These clocks can
       only be divided by 2 within HDVPSS */
    if (platformId == VPS_PLATFORM_ID_EVM_TI814x)
    {
        clkSrc.clkSrc = VPS_DC_CLKSRC_VENCA;
    }

    retVal = FVID2_control(
                 dctrlHandle,
                 IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC,
                 &clkSrc,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    clkSrc.venc = VPS_DC_VENC_HDMI;
    clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD_DIV2;
    if (((platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
            (cpuRev > VPS_PLATFORM_CPU_REV_1_0)) ||
        platformId == VPS_PLATFORM_ID_EVM_TI814x)
    {
        clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
    }
    if (platformId == VPS_PLATFORM_ID_EVM_TI814x)
    {
        clkSrc.clkSrc = VPS_DC_CLKSRC_VENCD;
    }

    retVal = FVID2_control(
                 dctrlHandle,
                 IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC,
                 &clkSrc,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    return (FVID2_SOK);
}



/**
 *  App_dispConfigPixClk
 *  Configure Pixel Clock for All Vencs.
 */
static Int32 App_dispConfigPixClk()
{
    Int32               retVal;
    Vps_SystemVPllClk   vpllCfg;
    Vps_PlatformId      platformId;
    Vps_PlatformCpuRev  cpuRev;

    platformId = Vps_platformGetId();
    if ((platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf(
            "%s: Error Unrecognized platform@ line %d\n",
            __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }
    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        Vps_printf(
            "%s: Error Unrecognized CPU version@ line %d\n",
            __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }
    if (platformId != VPS_PLATFORM_ID_EVM_TI814x)
    {
        vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_A;
        vpllCfg.outputClk = 74250u;
        retVal = App_dispSetPixClk(&vpllCfg);
        if (FVID2_SOK != retVal)
        {
            Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
            return (retVal);
        }
    }

    vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;
    vpllCfg.outputClk = 148500;
    if (((platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
            (cpuRev > VPS_PLATFORM_CPU_REV_1_0)) ||
        (platformId == VPS_PLATFORM_ID_EVM_TI814x))
    {
        vpllCfg.outputClk = 74250u;
    }
    retVal = App_dispSetPixClk(&vpllCfg);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    vpllCfg.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_RF;
    if (platformId == VPS_PLATFORM_ID_EVM_TI816x)
    {
        vpllCfg.outputClk = 216000u;
    }
    else
    {
        vpllCfg.outputClk = 54000u;
    }
    retVal = App_dispSetPixClk(&vpllCfg);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    return (FVID2_SOK);
}

/**
 *  App_dispSetPixClk
 *  Configure Pixel Clock.
 */
static Int32 App_dispSetPixClk(Vps_SystemVPllClk *vpllCfg)
{
    Int32           retVal = FVID2_SOK;
    FVID2_Handle    systemDrvHandle;

    systemDrvHandle = FVID2_create(
                          FVID2_VPS_VID_SYSTEM_DRV,
                          0u,
                          NULL,
                          NULL,
                          NULL);
    if (NULL == systemDrvHandle)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (FVID2_EFAIL);
    }

    retVal = FVID2_control(
                 systemDrvHandle,
                 IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                 vpllCfg,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    FVID2_delete(systemDrvHandle, NULL);

    return (FVID2_SOK);
}

#ifdef APP_USE_OFF_CHIP_HDMI
/**
 *  App_dispConfigHdmi
 *  Configures the display controller to connect the paths, enabling muxes
 *  and configuring blenders and VENCs.
 */
static Int32 App_dispInitHdmi(FVID2_Handle *hdmiHandle)
{
    Int32                           retVal;
    Vps_HdmiChipId                  hdmiId;
    Vps_SiI9022aHpdPrms             hpdPrms;
    Vps_SiI9022aModeParams          modePrms;
    Vps_VideoEncoderCreateParams    encCreateParams;
    Vps_VideoEncoderCreateStatus    encCreateStatus;
    Vps_PlatformBoardId             boardId;

    boardId = Vps_platformGetBoardId();

    if(boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        Vps_printf("%s: UNKNOWN board detected, Defaulting to VS Board\n",
                APP_NAME);
        boardId = VPS_PLATFORM_BOARD_VS;
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
        encCreateParams.clkEdge = TRUE;
    }
    else
    {
        encCreateParams.syncMode = VPS_VIDEO_ENCODER_EMBEDDED_SYNC;
        encCreateParams.clkEdge = FALSE;
    }
    /* Requires to be falling adge for 814x always*/
#ifdef TI_814X_BUILD
    encCreateParams.clkEdge = FALSE;
#endif /* TI_814X_BUILD */

    *hdmiHandle = FVID2_create(
                             FVID2_VPS_VID_ENC_SII9022A_DRV,
                             0u,
                             &encCreateParams,
                             &encCreateStatus,
                             NULL);
    if (NULL == *hdmiHandle)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (FVID2_EFAIL);
    }

    retVal = FVID2_control(
                 *hdmiHandle,
                 IOCTL_VPS_SII9022A_GET_DETAILED_CHIP_ID,
                 &hdmiId,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Could not get detailed chip ID\n", APP_NAME);
        return (retVal);
    }

    retVal = FVID2_control(
                 *hdmiHandle,
                 IOCTL_VPS_SII9022A_QUERY_HPD,
                 &hpdPrms,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Could not detect HPD\n", APP_NAME);
        return (retVal);
    }

    modePrms.standard = FVID2_STD_1080I_60;
    retVal = FVID2_control(
                 *hdmiHandle,
                 IOCTL_VPS_VIDEO_ENCODER_SET_MODE,
                 &modePrms,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("%s: Could not set mode\n", APP_NAME);
    }

    Vps_printf("%s: Starting HDMI Transmitter ...\n", APP_NAME);
    retVal = FVID2_start(*hdmiHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }

    return (retVal);
}


static Int32 App_dispDeInitHdmi(FVID2_Handle *hdmiHandle)
{
    Int32 retVal;

    /* Stop HDMI transmitter */
    retVal = FVID2_stop(*hdmiHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return (retVal);
    }
    Vps_printf("%s: HDMI Transmitter Stopped!!\n", APP_NAME);

    /* Close HDMI transmitter */
    retVal = FVID2_delete(*hdmiHandle, NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return retVal;
    }

    hdmiHandle = NULL;

    return (retVal);
}
#endif
