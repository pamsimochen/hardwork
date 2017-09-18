/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file PrintDesc_main.c
 *
 *  \brief Main file to print the VPDMA descriptor.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#ifdef TI_816X_BUILD
#include <ti/psp/cslr/soc_TI816x.h>
#endif
#ifdef TI_814X_BUILD
#include <ti/psp/cslr/soc_TI814x.h>
#endif
#ifdef TI_8107_BUILD
#include <ti/psp/cslr/soc_TI8107.h>
#endif
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/cslr/cslr_hd_vps_vpdma.h>

#include <ti/psp/examples/utility/vpsutils.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (10u * 1024u)

#ifdef PLATFORM_SIM

#ifdef VPS_CFG_DESC_IN_OCMC
/* OCMC (0x403XXXXX or 0x404XXXXX) is not supported on simulator.
 * Hence data should be loaded from 0x80000000 memory location. */
#define DESC_ADDR_MASK                  (0x000FFFFFu)
#define DESC_ADDR_OFFSET                (0x80000000u)
#else
#define DESC_ADDR_MASK                  (0xFFFFFFFFu)
#define DESC_ADDR_OFFSET                (0x00000000u)
#endif

#else

/* One to one mapping on EVM */
#define DESC_ADDR_MASK                  (0xFFFFFFFFu)
#define DESC_ADDR_OFFSET                (0x00000000u)

#endif

#define MAX_RELOADS                     (300u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void App_mainTestTask(void);
static Void App_printList(UInt32 listAddr, UInt32 listSize, UInt32 printLevel);
static UInt32 isDescAddrAlreadyPresent(UInt32 listAddr, UInt32 curIndex);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppMainTskStack, 32)
#pragma DATA_SECTION(AppMainTskStack, ".bss:taskStackSection")
static UInt8 AppMainTskStack[APP_TSK_STACK_SIZE];

/* History of list addresses to check for loops in list reloads. */
static UInt32 AppListAddrHistory[MAX_RELOADS];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  main
 *  Application main function.
 */
void main(void)
{
    Task_Params tskParams;

    Task_Params_init(&tskParams);
    tskParams.stack = AppMainTskStack;
    tskParams.stackSize = APP_TSK_STACK_SIZE;
    Task_create((Task_FuncPtr) App_mainTestTask, &tskParams, NULL);

    BIOS_start();
}



/**
 *  App_mainTestTask
 *  Application test task.
 */
static Void App_mainTestTask(void)
{
    UInt32      printLevel;
    UInt32      listAddr, listSize;
    char inputStr[20];

    Vps_printf("\n************************************************************\n");
    Vps_printf("VPDMA Descriptor Print App Start...\n\n");

#ifdef VPS_CFG_DESC_IN_OCMC
#ifdef PLATFORM_SIM
    Vps_printf("Simulator build: Kindly load descriptors to 0x80000000 location\n");
#endif
#endif

    VpsHal_vpdmaInitDebug(CSL_VPS_VPDMA_0_REGS);

    while (1u)
    {
        Vps_printf("Enter list address in hex (0 will exit app): ");
        VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
        listAddr = (UInt32)xstrtoi(inputStr);
        if (0u == listAddr)
        {
            break;
        }
        Vps_printf("Enter list size in VPDMA words: ");
        VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
        listSize = (UInt32)atoi(inputStr);

        Vps_printf("Print levels:\n");
        Vps_printf("0 - Prints only Errors and Warnings\n");
        Vps_printf("1 - Prints descriptor information in addition to level 0\n");
        Vps_printf("2 - Prints register overlay in addition to level 1\n");
        Vps_printf("Enter print level: ");
        VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
        printLevel = (UInt32)atoi(inputStr);

        App_printList(listAddr, listSize, printLevel);

        VpsUtils_remoteSendChar('e');
    }

    Vps_printf("\nVPDMA Descriptor Print App Successful!!\n");
    Vps_printf("************************************************************\n");

    VpsUtils_remoteSendChar('x');

    return;
}



static Void App_printList(UInt32 listAddr, UInt32 listSize, UInt32 printLevel)
{
    UInt32      curIndex;
    UInt32      rlListAddr, rlListSize;

    Vps_printf("\n************************************************************\n");
    Vps_printf("VPDMA Descriptor Print Start...\n\n");

    curIndex = 0u;
    memset(AppListAddrHistory, 0u, sizeof (AppListAddrHistory));

    while (1u)
    {
        /* Convert list address as per how descriptors are loaded in CCS */
        listAddr = (listAddr & DESC_ADDR_MASK) + DESC_ADDR_OFFSET;

        /* Check for wrap around as happens in the case of DLM */
        if ((TRUE == isDescAddrAlreadyPresent(listAddr, curIndex)) ||
            (curIndex >= MAX_RELOADS))
        {
            break;
        }
        AppListAddrHistory[curIndex] = listAddr;

        /* Print the list */
        VpsHal_vpdmaPrintList(
            listAddr,
            listSize,
            &rlListAddr,
            &rlListSize,
            printLevel);
        if (0u == rlListAddr)
        {
            break;
        }

        listAddr = rlListAddr;
        listSize = rlListSize;
        curIndex++;
    }

    Vps_printf("\nVPDMA Descriptor Print Successful!!\n");
    Vps_printf("************************************************************\n");

    return;
}



static UInt32 isDescAddrAlreadyPresent(UInt32 listAddr, UInt32 curIndex)
{
    UInt32      index;
    UInt32      foundFlag = FALSE;

    for (index = 0u; index < curIndex; index++)
    {
        if (listAddr == AppListAddrHistory[index])
        {
            foundFlag = TRUE;
            break;
        }
    }

    return (foundFlag);
}
