/** ==================================================================
 *  @file   cbuff1.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/cbuff/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/*****************************************************************************/
/* */
/* Copyright 2008-09 by Texas Instruments Incorporated. All rights */
/* reserved. Property of Texas Instruments Incorporated. Restricted */
/* rights to use, duplicate or disclose this code are granted */
/* through contract.  */
/* "Texas Instruments OMAP(tm) Platform Software" */
/* */
/*****************************************************************************/

/*****************************************************************************/
/* File Name :Cbuff.c */
/* */
/* Description :Program file containing all the functions for configuration,
 * enable and disable * functionality for CBUFF module */
/* */
/* Created : Ducati Team (OMAP4) */
/* */
               /* @rev 1.0 *//*============================================================ *!
               *! Revision History *! ===================================*/

#include <ti/psp/iss/hal/iss/cbuff/cbuff.h>

#ifdef CBUFF_SIMULATION
/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <xdc/std.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Gate.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Swi.h>

#include <osal/timm_osal_task.h>
#include <osal/timm_osal_error.h>
#include <osal/timm_osal_memory.h>
#include <osal/timm_osal_semaphores.h>

/* 
 * #include "inc/cbuff_utils.h" #include "inc/cbuff_reg.h" #include "cbuff.h" */
extern Swi_Handle cbuff_swi_handle;

CBUFF_REGS *pCBUFF1 = (CBUFF_REGS *) 0x85000000;

static int openFlag = 0;

TIMM_OSAL_PTR sim_cbuff_Sem;

TIMM_OSAL_PTR pSIM_CBUFF_Task;

/* ================================================================ */
/* 
 *  Description:- Init should be called only once before calling any other function
 *  
 *
 *  @param   none
 
 *  @return    ISP_RETURN     
 *================================================================== */

/* ===================================================================
 *  @func     sim_cbuff_init                                               
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
CBUFF_RETURN sim_cbuff_init()
{
    CBUFF_REGS *pTMP;

    TIMM_OSAL_Entering();
    if (openFlag)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "SIM CBUFF: SIMULATION IS NOT RUNNING %d \n",
                           __LINE__);
        return CBUFF_FAILURE;
    }
    openFlag = 1;
    pTMP = pCBUFF;
    pCBUFF =
        TIMM_OSAL_MallocExtn(sizeof(CBUFF_REGS), TIMM_OSAL_TRUE, 4,
                             TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    pCBUFF1 = pTMP;
    *pCBUFF = *pCBUFF1;

    TIMM_OSAL_SemaphoreCreate(&sim_cbuff_Sem, 0);

    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "SIM CBUFF: sim_cbuff_init()\n");
    return CBUFF_SUCCESS;
}

/* ===================================================================
 *  @func     sim_cbuff_deInit                                               
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
CBUFF_RETURN sim_cbuff_deInit()
{
    TIMM_OSAL_Entering();
    if (0 == openFlag)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "SIM CBUFF: SIMULATION IS NOT RUNNING %d \n",
                           __LINE__);
        return CBUFF_FAILURE;
    }

    TIMM_OSAL_Free(pCBUFF);
    pCBUFF = pCBUFF1;

    TIMM_OSAL_SemaphoreDelete(sim_cbuff_Sem);

    openFlag = 0;

    return CBUFF_SUCCESS;
}

/* 
 * void sim_cbuff_ctx_interrupt(uint32 number, uint32 type) { pCBUFF->IRQ.r32 
 * &= ~(1<<(CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_READY_SHIFT + number)); }
 * 
 * 
 * 
 * CBUFF_IRQ_Handle_T sim_cbuff_irq_handle;
 * 
 * sim_cbuff_irq_handle.list_next = NULL; sim_cbuff_irq_handle.callback =
 * sim_cbuff_ctx_interrupt; sim_cbuff_irq_handle.arg1 = number;
 * sim_cbuff_irq_handle.arg2 = IRQ_CTX_READY;
 * sim_cbuff_irq_handle.callback_context = CBUFF_CALLBACK_CONTEXT_SWI;
 * sim_cbuff_irq_handle.priority = 255;
 * 
 * CBUFF_Ctx_Hook_Interrupt (number, IRQ_CTX_READY, &sim_cbuff_irq_handle); */

/* ===================================================================
 *  @func     sim_clr_IRQ                                               
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
void sim_clr_IRQ(uint32 irqs)
{
    pCBUFF->IRQ_STATUS.r32 &= (~irqs);
}

/* ===================================================================
 *  @func     sim_cbuff_Fill                                               
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
void sim_cbuff_Fill(TIMM_OSAL_U32 argc, SIM_CBUFF_IMG * img)
{
    uint32 win_size1;

    uint32 win_size2;

    uint32 win_mask;

    uint8 *pDst1;

    uint8 *pDst2;

    CBUFF_CTX_TYPE ctx1;

    CBUFF_CTX_TYPE ctx2;

    uint32 number1;

    uint32 number2;

    uint32 processed;

    // CBUFF_REGS

    TIMM_OSAL_Entering();

    if (2 == img->number_of_planes)
    {
        number2 = img->p2.number;
        *(&ctx2) = *(&pCBUFF->ctx[number2]);               // Copy context

    }

    number1 = img->p1.number;

    *(&ctx1) = *(&pCBUFF->ctx[number1]);                   // Copy context

    if (0 == ctx1.CTX_CTRL.b.CB_ENABLE)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "SIM CBUFF: CONTEXT IS DIABLED \n");
        TIMM_OSAL_Exiting(CBUFF_FAILURE);
        sim_cbuff_deInit();
        return;
    }
    if (0 == openFlag)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "SIM CBUFF: SIMULATION IS NOT RUNNING %d \n",
                           __LINE__);
        TIMM_OSAL_Exiting(CBUFF_FAILURE);
        sim_cbuff_deInit();
        return;
    }

    if (ctx1.WINDOWSIZE % img->p1.stride)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "SIM CBUFF: INVAID STRIDE \n");
        TIMM_OSAL_Exiting(CBUFF_FAILURE);
        sim_cbuff_deInit();
        return;
    }

    win_mask = (2 << ctx1.CTX_CTRL.b.WCOUNT) - 1;

    while (img->p1.size)
    {
        processed = 0;
        win_size1 = ctx1.WINDOWSIZE;
        if (win_size1 > img->p1.size)
            win_size1 = img->p1.size;

        pDst1 = (uint8 *) ctx1.PHY_ADDR;                   // Physical
                                                           // address
                                                           // register for
                                                           // the context

        pDst1 += ctx1.STATUS.b.WA * ctx1.WINDOWSIZE;       // Defines the
                                                           // size of a
                                                           // window for the
                                                           // context 
        if (2 == img->number_of_planes)
        {
            pDst2 = (uint8 *) ctx2.PHY_ADDR;               // Physical
                                                           // address
                                                           // register for
                                                           // the context

            pDst2 += ctx2.STATUS.b.WA * ctx2.WINDOWSIZE;   // Defines the
                                                           // size of a
                                                           // window for the
                                                           // context 
            win_size2 = win_size1 / 2;
        }
        while (win_size1)
        {
            if ((ctx1.VA_START > (uint32) img->p1.pData) ||
                (ctx1.VA_END < (uint32) img->p1.pData))
            {
                TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                                   "SIM CBUFF: INVAID ADDRESS \n");
                TIMM_OSAL_Exiting(CBUFF_FAILURE);
                sim_cbuff_deInit();
                return;
            }

            memcpy(pDst1, img->p1.pData, img->p1.sizeX);

            img->p1.pData = (uint8 *) img->p1.pData + img->p1.stride;
            pDst1 += img->p1.stride;

            win_size1 -= img->p1.stride;
            img->p1.size -= img->p1.stride;
            processed += img->p1.sizeX;

            if ((2 == img->number_of_planes) && (win_size2))
            {
                memcpy(pDst2, img->p2.pData, img->p2.sizeX);
                img->p2.pData = (uint8 *) img->p2.pData + img->p2.stride;
                pDst2 += img->p2.stride;
                if (win_size2 > img->p2.stride)
                {
                    win_size2 -= img->p2.stride;
                }
                else
                {
                    win_size2 = 0;
                }
            }
        }
        if (processed >= ctx1.THRESHOLD_F)
        {
            ctx1.STATUS.b.WA = (ctx1.STATUS.b.WA + 1) & win_mask;
            pCBUFF->IRQ_RAW.r32 =
                1 << (CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_READY_SHIFT + number1);
            pCBUFF->IRQ_STATUS.r32 =
                1 << (CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_READY_SHIFT + number1);
            ctx1.STATUS.b.WB = pCBUFF->ctx[number1].STATUS.b.WB;    // Copy
                                                                    // context 
                                                                    // 1
            *(&pCBUFF->ctx[number1]) = *(&ctx1);

            if (2 == img->number_of_planes)
            {
                ctx2.STATUS.b.WA = (ctx2.STATUS.b.WA + 1) & win_mask;
                ctx2.STATUS.b.WB = pCBUFF->ctx[number2].STATUS.b.WB;    // Copy 
                                                                        // context 
                                                                        // 2
                *(&pCBUFF->ctx[number2]) = *(&ctx2);
            }
            Swi_post(cbuff_swi_handle);
        }
        ctx1.STATUS.b.WB = pCBUFF->ctx[number1].STATUS.b.WB;    // Copy
                                                                // context 1

        if (2 == img->number_of_planes)
            ctx2.STATUS.b.WB = pCBUFF->ctx[number2].STATUS.b.WB;    // Copy
                                                                    // context 
                                                                    // 2

        if (ctx1.STATUS.b.WB == ((ctx1.STATUS.b.WA + 1) & win_mask))
        {
            TIMM_OSAL_SemaphoreObtain(sim_cbuff_Sem, TIMM_OSAL_SUSPEND);
            ctx1.STATUS.b.WB = pCBUFF->ctx[number1].STATUS.b.WB;
            if (2 == img->number_of_planes)
            {
                TIMM_OSAL_SemaphoreObtain(sim_cbuff_Sem, TIMM_OSAL_SUSPEND);
                ctx2.STATUS.b.WB = pCBUFF->ctx[number2].STATUS.b.WB;
            }
        }
    }
    TIMM_OSAL_Exiting(CBUFF_SUCCESS);
}

/* ===================================================================
 *  @func     sim_cbuff_Ctx_Done                                               
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
void sim_cbuff_Ctx_Done(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    uint32 win_mask;

    volatile CBUFF_CTX_TYPE *pCtx;

    TIMM_OSAL_Entering();
    if (0 == openFlag)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "SIM CBUFF: SIMULATION IS NOT RUNNING %d \n",
                           __LINE__);
    }
    pCtx = &pCBUFF->ctx[ctx_number];
    win_mask = (2 << pCtx->CTX_CTRL.b.WCOUNT) - 1;
    if (pCtx->STATUS.b.WB == ((pCtx->STATUS.b.WA + 1) & win_mask))
        TIMM_OSAL_SemaphoreRelease(sim_cbuff_Sem);

    pCtx->STATUS.b.WB = (pCtx->STATUS.b.WB + 1) & win_mask;

    pCtx->CTX_CTRL.b.DONE = 0;
}

/*****************RESET FUnction for CBUFF Module******************/
/****************************************************************/
/* @Cbuff_Reset() to reset the Cbuff
 * 
 * @param reset_state contains the reset state
 * 
 * 
 * @return status CBUFF_SUCCESS Succesfull open \n * Other value = Open
 * failed and the error code is returned.
 * *************************************************************** */

/* ===================================================================
 *  @func     sim_cbuff_Reset                                               
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
CBUFF_RETURN sim_cbuff_Reset(uint8 reset_state)
{
    TIMM_OSAL_Entering();

    if (0 == openFlag)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "SIM CBUFF: SIMULATION IS NOT RUNNING %d \n",
                           __LINE__);
        return CBUFF_FAILURE;
    }

    *pCBUFF = *pCBUFF1;
    return CBUFF_SUCCESS;
}

/* ===================================================================
 *  @func     sim_cbuff_RUN                                               
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
CBUFF_RETURN sim_cbuff_RUN(SIM_CBUFF_IMG * img, int flagBlocking)
{
    TIMM_OSAL_ERRORTYPE ret = CBUFF_SUCCESS;

    TIMM_OSAL_EnteringExt(TIMM_OSAL_TRACEGRP_SYSTEM);
    // sim_cbuff_init();
    if (flagBlocking)
        sim_cbuff_Fill(1, img);
    else
        ret =
            TIMM_OSAL_CreateTask(&pSIM_CBUFF_Task, sim_cbuff_Fill, 0, img, 2048,
                                 10, "CBUFF_SIMULATION");

    TIMM_OSAL_ExitingExt(TIMM_OSAL_TRACEGRP_SYSTEM, ret);
    return (CBUFF_RETURN) ret;
}

#endif                                                     // CBUFF_SIMULATION
