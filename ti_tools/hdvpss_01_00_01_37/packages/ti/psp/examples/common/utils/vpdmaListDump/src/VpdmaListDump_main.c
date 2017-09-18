/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file VpdmaListDump_main.c
 *
 *  \brief Main file to dump the VPDMA list manager debug information.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
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
#include <ti/psp/cslr/cslr_vps_comp.h>

#include <ti/psp/examples/utility/vpsutils.h>

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


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* Test application stack. */
#pragma DATA_ALIGN(AppMainTskStack, 32)
#pragma DATA_SECTION(AppMainTskStack, ".bss:taskStackSection")
static UInt8 AppMainTskStack[APP_TSK_STACK_SIZE];


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

static void App_dumpListStatus(void)
{
    UInt32      listNum, listStatus;

    Vps_printf(" \n");
    Vps_printf("List Status Dump\n");
    Vps_printf("----------------\n");

    /* Dump the status of all the lists */
    for (listNum = 0u; listNum < VPSHAL_VPDMA_MAX_LIST; listNum++)
    {
        listStatus = VpsHal_vpdmaGetListStatus(listNum);
        Vps_printf("List Status %d: 0x%0.8X\n", listNum, listStatus);
    }

    return;
}

static void App_printCompStatus(void)
{
    UInt32              regVal, flag = FALSE;
    CSL_Vps_compRegs   *compRegOvly;

    Vps_printf(" \n");
    Vps_printf("Comp Status:\n");
    Vps_printf("------------\n");

    compRegOvly = (CSL_Vps_compRegs *) CSL_VPS_COMP_0_REGS;

    /* Check HDMI */
    regVal = compRegOvly->HDMI_SETTINGS;
    if (regVal & CSL_VPS_COMP_HDMI_SETTINGS_ERROR_MASK)
    {
        Vps_printf("HDMI COMP Error!! One or more of the enabled input "
            "sources is not the proper size. Reg Value (0x%0.8X)\n", regVal);
        flag = TRUE;
    }

#if defined(TI_816X_BUILD) || defined(TI_8107_BUILD)
    /* Check HDCOMP */
    regVal = compRegOvly->HDCOMP_SETTINGS;
    if (regVal & CSL_VPS_COMP_HDCOMP_SETTINGS_ERROR_MASK)
    {
        Vps_printf("HDCOMP COMP Error!! One or more of the enabled input "
            "sources is not the proper size. Reg Value (0x%0.8X)\n", regVal);
        flag = TRUE;
    }
#endif

    /* Check DVO2 */
    regVal = compRegOvly->DVO2_SETTINGS;
    if (regVal & CSL_VPS_COMP_DVO2_SETTINGS_ERROR_MASK)
    {
        Vps_printf("DVO2 COMP Error!! One or more of the enabled input "
            "sources is not the proper size. Reg Value (0x%0.8X)\n", regVal);
        flag = TRUE;
    }

    /* Check SD */
    regVal = compRegOvly->SD_SETTINGS;
    if (regVal & CSL_VPS_COMP_SD_SETTINGS_ERROR_MASK)
    {
        Vps_printf("DVO2 COMP Error!! One or more of the enabled input "
            "sources is not the proper size. Reg Value (0x%0.8X)\n", regVal);
        flag = TRUE;
    }

    if (!flag)
    {
        Vps_printf("No COMP errors!!!\n");
    }

    return;
}

static void App_printVpdmaLmReg(UInt32 regOffset, UInt32 numReg)
{
    UInt32      loop, lmRegVal;
    static char tmpBuf[64];
    static char buf[1024];

    /* Change print's to do Vps_printf() as a single function call
     * in order to make the output appear correctly when doing LM dump from
     * A8 side */
    buf[0] = 0;
    sprintf(tmpBuf, "0x%0.4X:", regOffset);
    strcat(buf, tmpBuf);
    for (loop = 0u; loop < numReg; loop++)
    {
        lmRegVal = VpsHal_vpdmaReadLmReg(0u, regOffset);
        sprintf(tmpBuf, " 0x%0.4X", lmRegVal);
        strcat(buf, tmpBuf);
        regOffset++;
    }
    strcat(buf, "\n");
    Vps_printf(buf);

    return;
}

static void App_dumpAllLmReg(void)
{
    UInt32      regOffset;

    Vps_printf(" \n");
    Vps_printf("LM Register Dump\n");
    Vps_printf("----------------\n");

    /* Print internal LM registers */
    for (regOffset = 0x8000u; regOffset <= 0x80FFu;)
    {
        App_printVpdmaLmReg(regOffset, 16u);
        regOffset += 16u;
    }

    /* Print the shared memory space */
    for (regOffset = 0x0000u; regOffset <= 0x2000u;)
    {
        App_printVpdmaLmReg(regOffset, 16u);
        regOffset += 16u;
    }

    return;
}

/**
 *  App_mainTestTask
 *  Application test task.
 */
static Void App_mainTestTask(void)
{
    CSL_VpsVpdmaRegsOvly    vpdmaRegOvly;

    Vps_printf(" \n");
    Vps_printf("***********************************************************\n");
    Vps_printf("VPDMA Dump Start...\n");
    Vps_printf(" \n");

    VpsHal_vpdmaInitDebug(CSL_VPS_VPDMA_0_REGS);

    vpdmaRegOvly = (CSL_VpsVpdmaRegsOvly) CSL_VPS_VPDMA_0_REGS;
    Vps_printf("VPDMA Firmware Version: 0x%0.8x\n", vpdmaRegOvly->PID);
    Vps_printf(
        "VPDMA List Busy Status: 0x%0.8x\n", vpdmaRegOvly->LIST_STAT_SYNC);

    App_printCompStatus();
    VpsHal_vpdmaPrintCStat();
    App_dumpListStatus();
    App_dumpAllLmReg();

    Vps_printf(" \n");
    Vps_printf("VPDMA Dump Successful!!\n");
    Vps_printf("***********************************************************\n");
    Vps_printf(" \n");

    VpsUtils_remoteSendChar('x');

    return;
}
