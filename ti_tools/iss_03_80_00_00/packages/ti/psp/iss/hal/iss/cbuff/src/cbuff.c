/** ==================================================================
 *  @file   cbuff.c                                                  
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
/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**  File Name 	:cbuff.c
 *
 *  Description :Program file containing all the functions for configuration, 
 *   enable and disable functionality for CBUFF 
 *
 * @path     Centaurus\drivers\csl\iss\cbuff\inc
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *! 
 */
 /*========================================================================= */

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include "inc/cbuff_utils.h"
#include "inc/cbuff_reg.h"
#include "cbuff.h"

/****************************************************************
*  Extern definitions
*****************************************************************/
extern CBUFF_RETURN cbuff_interrupt_init(void);

typedef volatile CSL_Cbuff_Regs *cbuff_regs_ovly;

#define CBUFF_RETRY_CNT             5
#define CBUFF_READ_RESET_DONE       0
#define CBUFF_WRITE_SW_RESET        1

#if 0

CSL_Cbuff_Regs cbuff_regs_samp;

cbuff_regs_ovly cbuff_reg;
#else
cbuff_regs_ovly cbuff_reg = (cbuff_regs_ovly) CBUFF_REG_BASE_ADDRESS;

CBUFF_REGS *pCBUFF = (CBUFF_REGS *) CBUFF_REG_BASE_ADDRESS;
#endif
cbuff_dev_data_t *cbuff_devp;

cbuff_dev_data_t cbuff_dev;

/* ================================================================ */
/* 
 *  Description:- Init should be called only once before calling any other function
 *  
 *
 *  @param   none
 
 *  @return    ISP_RETURN     
 *================================================================== */

/* ===================================================================
 *  @func     cbuff_Init                                               
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
CBUFF_RETURN cbuff_Init(void)
{
    /* This memory can be malloc'ed */
    cbuff_devp = &cbuff_dev;
    cbuff_interrupt_init();
    cbuff_devp->opened = CBUFF_DISABLE;
    return CBUFF_SUCCESS;
}

/* ================================================================ */
/* 
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler
 *  
 *
 *  @param   :- none
 
 *  @return  :- ISP_RETURN       
 *================================================================== */
/* ===================================================================
 *  @func     cbuff_Open                                               
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
CBUFF_RETURN cbuff_Open(void)
{

    if (CBUFF_ENABLE == cbuff_devp->opened)
    {
        return CBUFF_FAILURE;
    }

    cbuff_devp->opened = CBUFF_ENABLE;

    return CBUFF_SUCCESS;

}

/* ================================================================ */
/* 
 *  Description:- close will de-init the CSL reg ptr removes the int handler 
 *  
 *
 *  @param   none
 
 *  @return   ISP_RETURN      
 *================================================================== */
/* ===================================================================
 *  @func     cbuff_Close                                               
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
CBUFF_RETURN cbuff_Close(void)
{
    if (CBUFF_ENABLE != cbuff_devp->opened)
    {
        return CBUFF_FAILURE;
    }

    cbuff_devp->opened = CBUFF_DISABLE;

    return CBUFF_SUCCESS;
}

// ============================Cbuff
// Functions==================================================

/****************************************************************/
/* @cbuff_Config_Power(CBUFF_POWER_STATE power_state) configures the power
 * for cbuff module. * @param power_state is instance of object
 * CBUFF_POWER_STATE * * @return status CBUFF_SUCCESS Succesfull *
 * CBUFF_INVALID_INPUT
 * *************************************************************** */
/* ===================================================================
 *  @func     cbuff_Config_Power                                               
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
CBUFF_RETURN cbuff_Config_Power(CBUFF_POWER_STATE power_state)
{
    CBUFF_HL_SYSCONFIG_TYPE sysCfg;

    sysCfg.r32 = pCBUFF->SYSCONFIG.r32;
    switch (power_state)
    {
        case FORCE_IDLE:
        case NO_IDLE:
        case SMART_IDLE:
            sysCfg.b.IDLEMODE = power_state;
            break;
        default:
            return CBUFF_INVALID_INPUT;                    // incorrect input 
                                                           // parameter
    }
    pCBUFF->SYSCONFIG.r32 = sysCfg.r32;
    return CBUFF_SUCCESS;
}

/****************************************************************/
/* @Cbuff_Reset() to reset the Cbuff * * @param reset_state contains the
 * reset state * * * @return status CBUFF_SUCCESS Succesfull * CBUFF_FAILURE
 * *************************************************************** */

/* ===================================================================
 *  @func     cbuff_Reset                                               
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
CBUFF_RETURN cbuff_Reset(void)
{
    uint16 retrycnt = 0;

    CBUFF_HL_SYSCONFIG_TYPE sysCfg;

    sysCfg.r32 = pCBUFF->SYSCONFIG.r32;

    sysCfg.b.SOFTRESET = CBUFF_WRITE_SW_RESET;

    // Reset the CBUFF_SYSCONFIG register. 
    pCBUFF->SYSCONFIG.r32 = sysCfg.r32;

    do
    {
        sysCfg.r32 = pCBUFF->SYSCONFIG.r32;
        if (CBUFF_READ_RESET_DONE == sysCfg.b.SOFTRESET)
            return CBUFF_SUCCESS;
        // no sleep introduced here as register read will itself take more
        // than one cycle
    } while (retrycnt++ <= CBUFF_RETRY_CNT);

    return CBUFF_FAILURE;
}

/****************************************************************/
/**
*    @ CBUFF_Set_EOI() Software End Of Interrupt (EOI) control
*    @param          Cuint32 line_number
*    @return         NONE
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Set_EOI                                               
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
void cbuff_Set_EOI(uint32 line_number)
{
    pCBUFF->CBUFF_HL_IRQ_EOI = line_number;
}

/****************************************************************/
/* Cbuff_Ctx_Enable() Enables specific Context of the Cbuff module * @param
 * Context_Number context number of the Cbuff * * @return NONE
 * *************************************************************** */

/* ===================================================================
 *  @func     cbuff_Ctx_Enable                                               
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
void cbuff_Ctx_Enable(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    CBUFF_HL_CTX_CTRL_TYPE Ctrl;

    Ctrl.r32 = pCBUFF->ctx[ctx_number].CTX_CTRL.r32;
    Ctrl.b.CB_ENABLE = CBUFF_ENABLE;
    pCBUFF->ctx[ctx_number].CTX_CTRL.r32 = Ctrl.r32;
}

/****************************************************************/
/* Cbuff_Ctx_Disable() Enables specific Context of the Cbuff module * @param 
 * Context_Number context number of the Cbuff * * @return NONE
 * *************************************************************** */

/* ===================================================================
 *  @func     cbuff_Ctx_Disable                                               
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
void cbuff_Ctx_Disable(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    CBUFF_HL_CTX_CTRL_TYPE Ctrl;

    Ctrl.r32 = pCBUFF->ctx[ctx_number].CTX_CTRL.r32;

    Ctrl.b.CB_ENABLE = CBUFF_DISABLE;
    pCBUFF->ctx[ctx_number].CTX_CTRL.r32 = Ctrl.r32;

}

/****************************************************************/
/* 
 *  cbuff_ctx_Done() To  reset the CBUFF_CTX_CTRL.DONE bit when CPU is done with processing it
 *  @param number   the context to be configured  
 *
 *  @return         NONE
 ****************************************************************/

/* ===================================================================
 *  @func     cbuff_Ctx_Done                                               
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
void cbuff_Ctx_Done(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    CBUFF_HL_CTX_CTRL_TYPE Ctrl;

    Ctrl.r32 = pCBUFF->ctx[ctx_number].CTX_CTRL.r32;
    Ctrl.b.DONE = 1;
    pCBUFF->ctx[ctx_number].CTX_CTRL.r32 = Ctrl.r32;
#ifdef CBUFF_SIMULATION
    {
        void sim_cbuff_Ctx_Done(CBUFF_CTX_NUMBER_TYPE ctx_number);

        sim_cbuff_Ctx_Done(ctx_number);
    }
#endif                                                     // CBUFF_SIMULATION
}

/****************************************************************/
/**
*    @ CBUFF_Ctx_Get_Status() Disables CBUFF Interrupt
*    @param          CBUFF_CTX_NUMBER_TYPE ctx_number - context number
*    @return         Returns pinter to first not consumed window in the 
*                    Circular buffer. After window consumation Client
*                    should call cbuff_Ctx_Done function to release it.
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Get_Client_ReadAddress                                               
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
void *cbuff_Ctx_Get_Client_ReadAddress(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    volatile CBUFF_CTX_TYPE *pCtx = &pCBUFF->ctx[ctx_number];

    return (void *) (pCtx->PHY_ADDR + pCtx->STATUS.b.WB * pCtx->WINDOWSIZE);
}

// ===============================Configure_Cbuff_Context================================================

/****************************************************************/
/* Cbuff_Config_Ctx() configures the specific context * @param number the
 * context to be configured * @param cbuff_ctx instance of cbuff_ctx_ctrl to 
 * set control params for a context * @return status CBUFF_SUCCESS
 * *************************************************************** */
/* ===================================================================
 *  @func     cbuff_Ctx_Config                                               
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
CBUFF_RETURN cbuff_Ctx_Config(CBUFF_CTX_NUMBER_TYPE ctx_number,
                              CBUFF_CTX_CTRL * pCtxCfg)
{
    volatile CBUFF_CTX_TYPE *pCtx = &pCBUFF->ctx[ctx_number];

    CBUFF_HL_CTX_CTRL_TYPE Ctrl;

    CBUFF_HL_HWINFO_TYPE HWINFO;

    Ctrl.r32 = pCBUFF->ctx[ctx_number].CTX_CTRL.r32;
    HWINFO.r32 = pCBUFF->HWINFO.r32;

    Ctrl.b.CB_MODE = pCtxCfg->CTX_MODE;
    Ctrl.b.BCF = pCtxCfg->BCF;
    Ctrl.b.TILERMODE = pCtxCfg->TILER;
    Ctrl.b.WCOUNT = pCtxCfg->WINCOUNT;
    pCBUFF->ctx[ctx_number].CTX_CTRL.r32 = Ctrl.r32;

    // Set threshold for the context
    pCtx->THRESHOLD_F = pCtxCfg->CBUFF_CTX_THRESHOLD_F_X;
    pCtx->THRESHOLD_S = pCtxCfg->CBUFF_CTX_THRESHOLD_S_X;

    // Set virtual address for the context
    pCtx->VA_START = pCtxCfg->CBUFF_CTX_START_X;
    pCtx->VA_END = pCtxCfg->CBUFF_CTX_END_X;

    pCtx->WINDOWSIZE = pCtxCfg->CBUFF_CTX_WINDOWSIZE_X;

    if ((0 == ctx_number) && (FRAG_ENABLE == HWINFO.b.ENABLE_FRAGMENTATION))
    {                                                      // Only for
                                                           // Context 0 and
                                                           // if
                                                           // fragmentation
                                                           // is enabled
        int cnt;

        if (NULL != pCtxCfg->CBUFF_FRAG_ADDR)
        {
            for (cnt = 0; cnt < CBUFF_FRAG_ADDR_NUM; cnt++)
            {
                pCBUFF->CBUFF_FRAG_ADDR[cnt] = pCtxCfg->CBUFF_FRAG_ADDR[cnt];
            }

        }
        else
        {
            uint32 pAddr = pCtxCfg->CBUFF_CTX_PHY_X;

            for (cnt = 0; cnt < CBUFF_FRAG_ADDR_NUM; cnt++)
            {
                pCBUFF->CBUFF_FRAG_ADDR[cnt] = pAddr;
                pAddr += pCtxCfg->CBUFF_CTX_WINDOWSIZE_X;
            }
        }
    }
    pCtx->PHY_ADDR = pCtxCfg->CBUFF_CTX_PHY_X;

    return CBUFF_SUCCESS;
}

// =========================== Poll for Interrupt functions
// ===============================================
/****************************************************************/
/**
*  @cbuff_Ctx_Poll_Ready_Irq(Context_Number number) checks the ctx RAW ready irq 
*  @param number the context to be configured 
*  @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Poll_Ready_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Poll_Ready_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    return (CBUFF_RETURN) (1 &
                           (pCBUFF->IRQ_RAW.
                            r32 >> (CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_READY_SHIFT
                                    + ctx_number)));
}

/****************************************************************/
/**
*  @ cbuff_Ctx_Poll_Ovr_Flow_Irq(Context_Number number) checks the ctx RAW over flow irq
*  @param number the context to be configured 
*  @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Poll_Ovr_Flow_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Poll_Ovr_Flow_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    return (CBUFF_RETURN) (1 &
                           (pCBUFF->IRQ_RAW.
                            r32 >> (CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_OVRSHIFT
                                    + ctx_number)));
}

/****************************************************************/
/**
*   @ cbuff_Poll_Ctx_Invalid_Irq(Context_Number number) checks the ctx invalid irq
*   @param number the context to be configured 
*   @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Poll_Invalid_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Poll_Invalid_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    return (CBUFF_RETURN) (1 &
                           (pCBUFF->IRQ_RAW.
                            r32 >>
                            (CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_INVALID_SHIFT +
                             ctx_number)));
}

/****************************************************************/
/**
*    @ cbuff_Check_Ctx_Ocp_Error_Irq(Context_Number number) checks the ctx ocp error irq
*    @param          NONE
*    @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Poll_Ocp_Error_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Poll_Ocp_Error_Irq(void)
{
    CBUFF_HL_IRQ_TYPE IRQ;

    IRQ.r32 = pCBUFF->IRQ_RAW.r32;
    return (CBUFF_RETURN) IRQ.b.OCP_ERR;
}

// =========================== Check for Interrupt functions
// ===============================================

/****************************************************************/
/**
*  @cbuff_Ctx_Check_Ready_Irq(Context_Number number) checks the ctx ready irq
*  @param number the context to be configured 
*  @return         status      1-interrupt pending 0 - no interrupt
Other value = Open failed and the error code is returned.
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Check_Ready_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Check_Ready_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    return (CBUFF_RETURN) (1 &
                           (pCBUFF->IRQ_STATUS.
                            r32 >> (CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_READY_SHIFT
                                    + ctx_number)));
}

/****************************************************************/
/**
*  @ cbuff_Ctx_Check_Ovr_Flow_Irq(Context_Number number) checks the ctx over flow irq
*  @param number the context to be configured 
*  @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/

/* ===================================================================
 *  @func     cbuff_Ctx_Check_Ovr_Flow_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Check_Ovr_Flow_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    return (CBUFF_RETURN) (1 &
                           (pCBUFF->IRQ_STATUS.
                            r32 >> (CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_OVRSHIFT
                                    + ctx_number)));
}

/****************************************************************/
/**
*   @ cbuff_Check_Ctx_Invalid_Irq(Context_Number number) checks the ctx RAW invalid irq
*   @param number the context to be configured 
*   @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Check_Invalid_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Check_Invalid_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number)
{
    return (CBUFF_RETURN) (1 &
                           (pCBUFF->IRQ_STATUS.
                            r32 >>
                            (CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_INVALID_SHIFT +
                             ctx_number)));
}

/****************************************************************/
/**
*    @ cbuff_Check_Ctx_Ocp_Error_Irq(Context_Number number) checks the ctx ocp error irq
*    @param          NONE
*    @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/
/* ===================================================================
 *  @func     cbuff_Ctx_Check_Ocp_Error_Irq                                               
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
CBUFF_RETURN cbuff_Ctx_Check_Ocp_Error_Irq(void)
{
    CBUFF_HL_IRQ_TYPE IRQ;

    IRQ.r32 = pCBUFF->IRQ_STATUS.r32;
    return (CBUFF_RETURN) IRQ.b.OCP_ERR;
}

/****************************************************************/
/**
*    @ CBUFF_Ctx_Get_Status() Disables CBUFF Interrupt
*    @param          CBUFF_CTX_NUMBER_TYPE ctx_number - context number
*    @param          uint32* pWA  - pointer to variable that will receive current WA value
*    @param          uint32* pWB  - pointer to variable that will receive current WB value
*    @return         NONE
*/
/****************************************************************/
/* ===================================================================
 *  @func     CBUFF_Ctx_Get_Status                                               
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
void CBUFF_Ctx_Get_Status(CBUFF_CTX_NUMBER_TYPE ctx_number, uint32 * pWA,
                          uint32 * pWB)
{
    CBUFF_HL_CTX_STATUS status;

    status.r32 = pCBUFF->ctx[ctx_number].STATUS.r32;
    *pWA = status.b.WA;
    *pWB = status.b.WB;
}

#ifdef CBUFF_SIMULATION
#include "cbuff1.c"
#endif                                                     // CBUFF_SIMULATION
