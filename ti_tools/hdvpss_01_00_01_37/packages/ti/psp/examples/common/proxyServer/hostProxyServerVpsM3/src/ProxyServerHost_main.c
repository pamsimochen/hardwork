/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file ProxyServerHost_main.c
 *
 * \brief This application would initialize proxy server. The proxy server is
 *        configured to support A8 as the only client.
 *        The server could be re-configured to support M3.Video as client
 *        instead of A8, by un-defining the macro VPS_PROXY_SERVER_SUPPORT_A8.
 *
 * \par   Server that would initialize IPC and proxy server. Once initalized
 *        FVID2 calls is available to other cores, via proxy server.
 *        Note that FVID2_Init () API would have been called, before the
 *        proxy server is initialized.
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/ipc/MultiProc.h>
#include <ti/ipc/Notify.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/common/vps_utils.h>

#include <ti/psp/proxyServer/vps_proxyServer.h>

/* To get the sharead address configurations */
#include <ti/psp/platforms/vps_platform.h>

#define VPS_PROXY_SERVER_SUPPORT_A8

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VPS_PS_MAX_NO_OF_CORES              (0x02u)
#define VPS_PS_STARTING_EVENT_NO            (0x0Au)
#define VPS_PS_TASK_PRIORITY_TO_START       (0x08u)
#define VPS_PS_RESERVED_EVENT_NO            (0x09u)
/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void vpsPsHostTestTask(void);
static Int32 initProxyServer(void);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

static  UInt32              vpsPsHostTestTaskStack[0x2000];
static  Semaphore_Params    semParams;
static  Semaphore_Handle    tempSema;
static  Task_Params         taskParams;

static  VPS_PSrvInitParams  psInitParams;
#ifdef VPS_PROXY_SERVER_SUPPORT_A8
static  Char coreA8[]       ={"HOST"};
#else
static  Char coreVideo[]    ={"VIDEO-M3"};
#endif

static UInt8 hostNotifyMem[1*1024*1024];
#pragma DATA_SECTION(hostNotifyMem,".vpss:extMemNonCache:notify")

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Application main function.
 */
Int32 main(void)
{
    Task_Params_init(&taskParams);
    taskParams.priority         = 0x07;
    taskParams.stack            = (Ptr)&(vpsPsHostTestTaskStack[0]);
    taskParams.stackSize        = 0x2000;
    Task_create((Task_FuncPtr) vpsPsHostTestTask, &taskParams, NULL);

    BIOS_start();

    return (0);
}


/**
 *  Application task, that would initialize proxy server and exit.
 */
static Void vpsPsHostTestTask(void)
{
    Int32                           retVal;
    Vps_PlatformDeviceInitParams deviceInitPrms;
    Vps_PlatformInitParams platformInitPrms;

    /* Initialize the platform and set THS to ED Mode */
    platformInitPrms.isPinMuxSettingReq = FALSE;
    retVal = Vps_platformInit(&platformInitPrms);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    retVal = FVID2_init(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    deviceInitPrms.isI2cInitReq = FALSE;
    deviceInitPrms.isI2cProbingReq = FALSE;
    retVal = Vps_platformDeviceInit(&deviceInitPrms);
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }

    retVal = initProxyServer();
    if (FVID2_SOK != retVal)
    {
        Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        return;
    }
    /* Wait indefinitely */
    Semaphore_Params_init(&semParams);
    tempSema = Semaphore_create(1u, &semParams, NULL);
    Semaphore_pend(tempSema , BIOS_WAIT_FOREVER);
    Vps_printf(
            "%s: Going to block now. %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
    Semaphore_pend(tempSema , BIOS_WAIT_FOREVER);
}

/*
 * Initialize the proxy server
 */
static Int32 initProxyServer(void)
{
    Int32   rtnVal;
    UInt32  noOfCores, noOfEvents, eventNos, taskIndex, taskPri;
    UInt16 remoteProcId;
    psInitParams.sysLnkNoOfCores        =   0x01;

    for(noOfCores = 0x0; noOfCores < VPS_PS_MAX_NO_OF_CORES; noOfCores++)
    {
        eventNos    =   VPS_PS_STARTING_EVENT_NO;
        psInitParams.sysLnkNoOfNtyEvt[noOfCores]    =   16;
        for(noOfEvents = 0x0;
                noOfEvents < psInitParams.sysLnkNoOfNtyEvt[noOfCores];
                        noOfEvents++)
        {
            psInitParams.sysLnkNtyEvtNo[noOfCores][noOfEvents]  =   eventNos;
            eventNos++;
        }
        psInitParams.resSysLnkNtyNo[noOfCores]      =   VPS_PS_RESERVED_EVENT_NO;
    }
    taskPri =   VPS_PS_TASK_PRIORITY_TO_START;
    for (taskIndex = 0x0; taskIndex < 0x05u; taskIndex++)
    {
        psInitParams.taskPriority[taskIndex]        =   taskPri;
        taskPri--;
    }

    psInitParams.completionCbTaskPri = VPS_PS_TASK_PRIORITY_TO_START + 1;

    remoteProcId = MultiProc_getId(coreA8);

    do {
        rtnVal = Notify_attach(remoteProcId, (Ptr)hostNotifyMem);
        System_printf ("notify_attach  rtnVal  %d\n",
                   rtnVal);
    } while (rtnVal < 0);



#ifdef VPS_PROXY_SERVER_SUPPORT_A8
    psInitParams.sysLnkCoreNames[0x0]       =   coreA8;
#else
    psInitParams.sysLnkCoreNames[0x0]       =   coreVideo;
#endif

    rtnVal  =   VPS_PSrvInit(&psInitParams);

    return (rtnVal);
}
