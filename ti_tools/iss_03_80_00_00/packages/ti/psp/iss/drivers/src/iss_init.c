/** ==================================================================
 *  @file   iss_init.c
 *
 *  @path   /ti/psp/iss/drivers/src/
 *
 *  @desc   This  File contains.
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012
 *
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 * \file iss_init_ti814x.c
 *
 * \brief Iss initialization file for TI814X platform
 * This file implements Iss initialization and de-initialization APIs
 *
 */

/* ==========================================================================
 */
/* Include Files */
/* ==========================================================================
 */

#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Cache.h>

#include <ti/psp/cslr/soc_TI814x.h>

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>

#include <ti/psp/vps/fvid2.h>
#include <ti/psp/iss/iss.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/common/vps_utils.h>

#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/iss/drivers/iss_init.h>


#include <ti/psp/iss/drivers/capture/issdrv_capture.h>

#include <ti/psp/platforms/iss_platform.h>
#include <ti/psp/platforms/ti814x/iss_platformTI814x.h>

#include  <ti/psp/iss/core/inc/iss_comp.h>
#include <ti/psp/iss/hal/iss/isp/isp.h>

#include <ti/psp/iss/hal/iss/simcop/simcop_global/csl_simcop.h>
#include <ti/psp/iss/hal/iss/simcop/simcop_global/cslr_simcop_2.h>
#include <ti/psp/iss/alg/rm/inc/rm.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop_irq.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop_dma_irq.h>
#include <ti/psp/iss/alg/evf/inc/cpisCore.h>
#include <ti/psp/iss/core/inc/csi2rx_api.h>

#include <ti/psp/iss/common/iss_evtMgr.h>

#define __DEBUG__

#ifdef __DEBUG__
#define ISS_PRINT1(a)          Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a);
#define ISS_PRINT2(a, b)       Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a, b);
#define ISS_PRINT3(a, b, c)    Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a, b, c);
#define ISS_PRINT4(a, b, c, d) Vps_printf("%s (%d):", __FUNCTION__,        \
                                __LINE__);                                     \
                                Vps_printf(a, b, c, d);

#define ISS_ENTER_FUNC         Vps_printf("%s (%d): E", __FUNCTION__,      \
                                __LINE__);
#define ISS_EXIT_FUNC          Vps_printf("%s (%d): L", __FUNCTION__,      \
                                __LINE__);
#else
#define ISS_PRINT1(a)
#define ISS_PRINT2(a, b)
#define ISS_PRINT3(a, b, c)
#define ISS_PRINT4(a, b, c, d)
#define ISS_ENTER_FUNC
#define ISS_EXIT_FUNC
#endif


CPIS_Init vicpInit;
Semaphore_Handle cpisSync;

void System_simcopCacheWbInv(void *addr, Uint32 size, Bool wait)
{
    Cache_wbInv(addr, size, TRUE, NULL);
}
/* ==========================================================================
 */
/* Macros & Typedefs */
/* ==========================================================================
 */

/* ==========================================================================
 */
/* Structure Declarations */
/* ==========================================================================
 */

/* None */

/* ==========================================================================
 */
/* LOCAL FUNCTION DECLARATION */
/* ==========================================================================
 */

/* ===================================================================
 *  @func     issCommonInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static Int32 issCommonInit(Bool initSimcop);

/* ===================================================================
 *  @func     issCommonDeInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static Int32 issCommonDeInit(void);

/* ===================================================================
 *  @func     issDriverInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static Int32 issDriverInit(void);

/* ==========================================================================
 */
/* Global Variables */
/* ==========================================================================
 */

// TODO: Put all initializations here

/* ==========================================================================
 */
/* FUNCTION DEFINITIONS */
/* ==========================================================================
 */

/* ===================================================================
 *  @func     issCommonInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static Int32 issCommonInit(Bool initSimcop)
{
    Int32 retVal = 0;
    Bool simcopInit = FALSE;

#ifdef POWER_OPT_DSS_OFF
    simcopInit = FALSE;
#else
    simcopInit = TRUE;
#endif

    CSL_SimcopRegsOvly pSimcopRegs =
        (CSL_SimcopRegsOvly) CSL_SIMCOP_BASE_ADDRESS;
    CSL_FINS(pSimcopRegs->CLKCTRL, SIMCOP_CLKCTRL_DMA_CLK,
             CSL_SIMCOP_CLKCTRL_DMA_CLK_SUBMODULE_ENABLE_REQ);

    *(unsigned int *) 0x55050010 = 0x023;       //Soft-reset ISS IP and make auto-standby=TRUE

    retVal = iss_core_init(initSimcop);
    if (0 != retVal)
    {
        ISS_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    iss_module_clk_ctrl(ISS_ISP_CLK, ISS_CLK_ENABLE);

    iss_module_clk_ctrl(ISS_CCP2_CLK, ISS_CLK_DISABLE);
    iss_module_clk_ctrl(ISS_CSI2B_CLK, ISS_CLK_DISABLE);

    iss_isp_input_config(ISS_ISP_INPUT_PI);

    retVal = isp_common_init(NULL);
    if (0 != retVal)
    {
        ISS_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    if (initSimcop)
    {
        if(simcopInit == TRUE)
        {
            iss_module_clk_ctrl(ISS_SIMCOP_CLK, ISS_CLK_ENABLE);
            *(unsigned int *) 0x55060064 = 0x1FF; //SIMCOP_CLKCTRL

            retVal = simcop_common_init(NULL);
            if (0 != retVal)
            {
                ISS_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                return (retVal);
            }
            retVal = simcop_dma_common_init(NULL);
            if (0 != retVal)
            {
                ISS_PRINT3("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
                return (retVal);
            }
        } else {
            iss_module_clk_ctrl(ISS_SIMCOP_CLK, ISS_CLK_DISABLE);
        }
    }
    retVal = issDrvInit();
    if (0 != retVal)
    {
        ISS_PRINT3("%s: Error @ line %d\n", __FUNCTION__,
        __LINE__);
        return (retVal);
    }

#ifdef USE_MIPI_MODE
    *(unsigned int *) 0x4814069C = 0xfc;
#endif

    return (retVal);
}

/* ===================================================================
 *  @func     issDriverInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static Int32 issDriverInit(void)
{
    Int32 retVal = 0;

    if (0 == retVal)
    {
        retVal = Iss_captInit();
        retVal |= Iss_ispInit();
    }

    return (retVal);
}

/* ===================================================================
 *  @func     Iss_init
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_init(Ptr arg)
{
    Int32 retVal = 0;

    Vps_rprintf(" Iss_init called !!!!!! \n");
    if (0 != retVal)
    {
        Vps_printf("%s: Error @ line %d\n", __FUNCTION__, __LINE__);
        return (retVal);
    }

    if (0 == retVal)
    {
        retVal = issCommonInit(TRUE);
    }

    retVal = Iem_init(NULL);
    if (0 != retVal)
    {
        ISS_PRINT3("%s: Error @ line %d\n", __FUNCTION__,
        __LINE__);
        return (retVal);
    }

    if (0 == retVal)
    {
        retVal = issDriverInit();
    }
    if (0 == retVal)
    {
        RM_SIMCOP_Init();
    }

    {
    vicpInit.cacheWbInv =  System_simcopCacheWbInv;
    vicpInit.staticDmaAlloc= 1;
    /* In the IPNC system, we have only 10 EDMAs left for vision algorithm */
    vicpInit.maxNumDma= CPIS_MAX_CHANNELS;
    vicpInit.maxNumProcFunc= 1;
    vicpInit.memSize= CPIS_getMemSize(vicpInit.maxNumProcFunc);
    vicpInit.mem= memalign(64,vicpInit.memSize);
    GT_assert(GT_DEFAULT_MASK, vicpInit.mem != 0);
    /*
    In this test, we don't need to lock/unlock VICP usage
    but if there are other software components in the system using VICP HW,
    then need to implement lock/unlock through semaphore function
    */
    vicpInit.lock= NULL;
    vicpInit.unlock= NULL;
    /*
    On DM365, this member initFC is used to tell the VICP library
    if it should also initialize components related
    to the framework components: IRES manager, codec engine.
    Setting it to CPIS_INIT_FC_NONE means that VICP lib doesn't do any FC related initialization.
    The application may need to do it itself.
    Setting it to CPIS_INIT_FC_ALL means that VICP lib does all the FC related initialization.
    Selective initializaiton is possible by setting corresponding bits. See
    description of CPIS_Init in header file or documentation,
    */
    vicpInit.initFC= CPIS_INIT_FC_ALL;
    /* engineName must match Engine.create's argument in XDC config file vicpLib365.cfg
       Only needed if bit #1 of initFC set to 1
    */
    vicpInit.engineName= "alg_server";
    /* If vicpInit.initFC= CPIS_INIT_FC_NONE or any value that hasn't been ORed with CPIS_INIT_CE
     then vicpInit.codecEngineHandle should be set to a valid handler that has been obtained by the application
     when calling Engine_open()
    */
    vicpInit.codecEngineHandle= NULL;
    if (CPIS_init(&vicpInit)== -1) {
        Vps_printf("\nCPIS_init error!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        retVal = -1;
        return retVal;
    };

    /* Semaphore for the optimization fo synchronization of various CPIS Algorithms */
    {
        Semaphore_Params semParams;

        Semaphore_Params_init(&semParams);
        semParams.mode = Semaphore_Mode_BINARY;
        cpisSync = Semaphore_create(1, &semParams, NULL);
        if(cpisSync == NULL)
        {
          retVal = -1;
          return retVal;
        }

    }


    Vps_rprintf(" CPIS_init DONE !!!!!! \n");

    }


    /* FD interrupt mapping */
    *((unsigned int *) 0x48140F74) = 0x00000C00;

    // Enabling the TURBO Mode of SIMCOP to clock @ 400MHz
    iss_module_clk_ctrl(ISS_CSI2B_CLK, ISS_CLK_DISABLE);
    *(unsigned int *) 0x55060064 = 0x1FF;

    return (retVal);
}

Int32 Iss_reInit(Ptr arg)
{
    Int32 retVal = 0;

    if (0 == retVal)
    {
        retVal = issCommonInit(FALSE);
    }

    /* FD interrupt mapping */
    *((unsigned int *) 0x48140F74) = 0x00000C00;

    // Enabling the TURBO Mode of SIMCOP to clock @ 400MHz
    iss_module_clk_ctrl(ISS_CSI2B_CLK, ISS_CLK_DISABLE);
    *(unsigned int *) 0x55060064 = 0x1FF;

    return (retVal);
}

/* ===================================================================
 *  @func     issCommonDeInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
static Int32 issCommonDeInit(void)
{
    Int32 retVal = 0;

    free(vicpInit.mem);
    CPIS_deInit();
/* Soft reset done in ISS Capture.*/
    //  retVal = issDrvDeinit();

    // To be Removed from next release
    *(unsigned int *) (0x55041410) = 0x00002000;
    // To be Removed from next release
    *(unsigned int *) (0x55041010) = 0x00002000;

    iss_module_clk_ctrl(ISS_ISP_CLK, ISS_CLK_DISABLE);
    iss_module_clk_ctrl(ISS_CCP2_CLK, ISS_CLK_DISABLE);
    iss_module_clk_ctrl(ISS_CSI2B_CLK, ISS_CLK_DISABLE);
    iss_module_clk_ctrl(ISS_CSI2A_CLK, ISS_CLK_DISABLE);
    iss_module_clk_ctrl(ISS_SIMCOP_CLK, ISS_CLK_DISABLE);

    return (retVal);
}

/* ===================================================================
 *  @func     Iss_deInit
 *
 *  @desc     Function does the following
 *
 *  @modif    This function modifies the following structures
 *
 *  @inputs   This function takes the following inputs
 *            <argument name>
 *            Description of usage
 *            <argument name>
 *            Description of usage
 *
 *  @outputs  <argument name>
 *            Description of usage
 *
 *  @return   Return value of this function if any
 *  ==================================================================
 */
Int32 Iss_deInit(Ptr arg)
{
    Int32 retVal = 0;

    RM_SIMCOP_DeInit();

    retVal |= Iss_captDeInit();
    retVal |= issCommonDeInit();
    retVal |= Iss_ispDeInit();
    retVal |= Iem_deInit(NULL);

    Semaphore_delete(&cpisSync);


    // retVal |= VpsUtils_deInit(NULL);
    return (retVal);
}
