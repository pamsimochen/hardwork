/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ====================================================================== */
/**
 * @file   simcop_dma_irq_mgr.c
 *         This file contains the functionality to configure interrupt
 *         functionality shared between different Simcop compponents.
 *
 *
 * @path Centaurus\drivers\csl\iss\simcop\dma\
 *
 * @rev 1.0 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *!
 *====================================================================== */

/****************************************************************
 *  INCLUDE FILES
 *****************************************************************/
#include <ti/psp/iss/hal/iss/simcop/common/simcop_dma_irq.h>
#include <ti/psp/iss/hal/iss/simcop/common/simcop_irq.h>
// #include "../../isp5_utils/isp5_sys_types.h"
// #include "../../isp5_utils/isp5_csl_utils.h"

/* #include <ti/psp/iss/hal/iss/iss_common/iss_common.h" #include
 * <ti/psp/iss/hal/iss/iss_common/iss_interrupt_config.h" */
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_error.h>
/*************************************************************************/

/* ##TODO################# 1) DMA request for Gamma ? 2) MSTANDBY 3)
 * PSYNC_CLK_SEL? 4) VBUSM_CPRIORITY 5) OCP_WRNP 6) ISP5_SYSCONFIG 7)
 * ISP5_IRQ_EOI 8) IPIPE_GAMMA_RGB_COPY
 * 
 */

/*************************************************************************/
/* GLOBAL DATA DEFINITION */
/*************************************************************************/

// isp_regs_ovly isp_regs;
CSL_SimcopDmaRegsOvly simcopDmaRegs;

/* simcop_irq_data should be a 2-dimensional array of
 * [MAX_ISP_INTERRUPTS][isp_interrupt_bank3+1] Right now since we d not need
 * multiple interrupts form different banks, we will use the default bank 0
 * 
 */
#if 0
static simcop_dma_irq_t simcop_dma_irq_data[MAX_SIMCOP_DMA_INTERRUPTS];
#endif
simcop_dma_common_dev_data_t simcop_dma_common_dev = { NULL, NULL };

static simcop_dma_common_dev_data_t *simcop_dma_common_devp;

#if 0
/* ================================================================ */
/**
 *  Description:- this routine sets up the interrupt callback for the desired interruptonly for the interrupt bank0, it does not enable the interrupt
 *
 *
 *  @param int_id  simcop_dma_interrupt_id_t  int_id:- is the inpterrupt id for which the handler needs to be registered
 *  @param callback_func   simcop_dma_callback_t callback_func:- is the callback function that needs to be registered
 *  @param 		arg1, arg2 are the params that the callback will be invoked with when interrupt occurs
 *  @return        CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_register_interrupt_handler                                               
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
CSL_Status simcop_dma_register_interrupt_handler(simcop_dma_interrupt_id_t
                                                 int_id,
                                                 simcop_dma_callback_t
                                                 callback_func, uint32 arg1,
                                                 void *arg2,
                                                 SIMCOP_DMA_CALLBACK_CONTEXT_T
                                                 context)
{
    /* If a handler already exists return error */
    if (simcop_dma_irq_data[int_id].callback_func != NULL)
    {
        return CSL_ESYS_FAIL;
    }
    simcop_dma_irq_data[int_id].callback_func = callback_func;
    simcop_dma_irq_data[int_id].arg1 = arg1;
    simcop_dma_irq_data[int_id].arg2 = arg2;

    simcop_dma_irq_data[int_id].callback_context = context;
    return ISP_SUCCESS;
}

#endif

static Interrupt_Handle_T *simcop_dma_hwi_cb_list_head[SIMCOP_DMA_MAX_IRQ] =
    { NULL };
static Interrupt_Handle_T *simcop_dma_swi_cb_list_head[SIMCOP_DMA_MAX_IRQ] =
    { NULL };
static simcop_dma_interrupt_id_t simcop_dma_callback_priority[] = {

    // SIMCOP_DMA_MAX_IRQ,
    SIMCOP_DMA_CHAN7_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN6_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN5_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN4_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN3_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN2_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN1_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN0_FRAME_DONE_IRQ,
    SIMCOP_DMA_CHAN7_BLOCK_DONE_IRQ,
    SIMCOP_DMA_CHAN6_BLOCK_DONE_IRQ,
    SIMCOP_DMA_CHAN5_BLOCK_DONE_IRQ,
    SIMCOP_DMA_CHAN4_BLOCK_DONE_IRQ,
    SIMCOP_DMA_CHAN3_BLOCK_DONE_IRQ,
    SIMCOP_DMA_CHAN2_BLOCK_DONE_IRQ,
    SIMCOP_DMA_CHAN1_BLOCK_DONE_IRQ,
    SIMCOP_DMA_CHAN0_BLOCK_DONE_IRQ,
    SIMCOP_DMA_RESERVED14_IRQ,
    SIMCOP_DMA_RESERVED13_IRQ,
    SIMCOP_DMA_RESERVED12_IRQ,
    SIMCOP_DMA_RESERVED11_IRQ,
    SIMCOP_DMA_RESERVED10_IRQ,
    SIMCOP_DMA_RESERVED09_IRQ,
    SIMCOP_DMA_RESERVED08_IRQ,
    SIMCOP_DMA_RESERVED07_IRQ,
    SIMCOP_DMA_RESERVED06_IRQ,
    SIMCOP_DMA_RESERVED05_IRQ,
    SIMCOP_DMA_RESERVED04_IRQ,
    SIMCOP_DMA_RESERVED03_IRQ,
    SIMCOP_DMA_RESERVED02_IRQ,
    SIMCOP_DMA_RESERVED01_IRQ,
    SIMCOP_DMA_RESERVED00_IRQ,
    SIMCOP_DMA_OCPERR
};

/* ================================================================ */
/**
 *  simcop_dma_unhook_int_handler()
 *  Description :- This routine de-links the argument "handle " froma priority linked list for the  interrupt
 *			     specified by the argument "int_id". the handle is de-linked from the swi list or hwi list as specified
 *                       by the parameter "context" within handle type
 *
 *
 *  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.
 *
 *  @return  :- CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_unhook_int_handler                                               
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
CSL_Status simcop_dma_unhook_int_handler(simcop_dma_interrupt_id_t int_id,
                                         Interrupt_Handle_T * handle)
{

    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL, *head_ptr =
        (Interrupt_Handle_T *) NULL;
    uint8 flag = 0;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSL_ESYS_FAIL;
    if (int_id >= SIMCOP_DMA_MAX_IRQ)
        return (CSL_ESYS_FAIL);
    if (handle->context == SIMCOP_CALLBACK_CONTEXT_HWI)
        head_ptr = simcop_dma_hwi_cb_list_head[int_id];
    else if (handle->context == SIMCOP_CALLBACK_CONTEXT_SWI)
        head_ptr = simcop_dma_swi_cb_list_head[int_id];
    else
        return CSL_ESYS_FAIL;

    if (head_ptr == NULL)
        return CSL_ESYS_FAIL;

    /* 
     * If the handle is the first element update the head ptr */

    if (handle == head_ptr)
    {

        if (handle->context == SIMCOP_CALLBACK_CONTEXT_HWI)
        {
            simcop_dma_hwi_cb_list_head[int_id] = head_ptr->list_next;

        }
        else if (handle->context == SIMCOP_CALLBACK_CONTEXT_SWI)
        {
            simcop_dma_swi_cb_list_head[int_id] = head_ptr->list_next;

        }

    }
    else
    {
        ptr_curr = head_ptr;
        while (ptr_curr->list_next != NULL)
        {
            if (ptr_curr->list_next == handle)
            {                                              /* found the
                                                            * element to be
                                                            * removed and
                                                            * de-linked from
                                                            * the list */
                flag = 1;
                ptr_curr->list_next = handle->list_next;
            }
        }
        if (flag == 0)
            return CSL_ESYS_FAIL;

    }

    return CSL_SOK;

}

/* ================================================================ */
/**
 *  simcop_dma_hook_int_handler()
 *  Description :- This routine links the argument "handle " to a priority linked list for the  interrupt
 *			     specified by the argument "int_id". the handle is linked to the swi list or hwi list as specified
 *                       by the parameter "context" within handle type
 *
 *
 *  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.
 *
 *  @return  :- CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_hook_int_handler                                               
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
CSL_Status simcop_dma_hook_int_handler(simcop_dma_interrupt_id_t int_id,
                                       Interrupt_Handle_T * handle)
{
    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL,
        *head_ptr = (Interrupt_Handle_T *) NULL;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSL_ESYS_FAIL;
    if (int_id >= SIMCOP_DMA_MAX_IRQ)
        return (CSL_ESYS_FAIL);
    if (handle->callback == NULL)
        /* return error since both callbacks shouldnt be zero */
        return CSL_ESYS_FAIL;

    handle->list_next = NULL;

    if (handle->context == SIMCOP_CALLBACK_CONTEXT_HWI)
        head_ptr = simcop_dma_hwi_cb_list_head[int_id];
    else if (handle->context == SIMCOP_CALLBACK_CONTEXT_SWI)
        head_ptr = simcop_dma_swi_cb_list_head[int_id];
    /* Go to end of list and link the handle at the end */
    ptr_curr = head_ptr;

    /* Need to protect the following piece of code from pre-emption. */
    /* Check if node need to goto the head of the list */
    if ((head_ptr == NULL) || (handle->priority < head_ptr->priority))
    {

        if (handle->context == SIMCOP_CALLBACK_CONTEXT_HWI)

        {
            handle->list_next = simcop_dma_hwi_cb_list_head[int_id];
            simcop_dma_hwi_cb_list_head[int_id] = handle;
        }
        else if (handle->context == SIMCOP_CALLBACK_CONTEXT_SWI)
        {
            handle->list_next = simcop_dma_swi_cb_list_head[int_id];
            simcop_dma_swi_cb_list_head[int_id] = handle;
        }

    }
    else
    {
        while (ptr_curr->list_next != NULL)
        {

            if ((handle->priority > ptr_curr->priority) &&
                (handle->priority < ptr_curr->list_next->priority))
            {
                /* Insert the element */
                handle->list_next = ptr_curr->list_next;
                ptr_curr->list_next = handle;
                break;
            }
            else
            {

                ptr_curr = ptr_curr->list_next;

            }

        }

        if (ptr_curr->list_next == NULL)
        {
            /* Insert the element at the end of the node */
            ptr_curr->list_next = handle;
            handle->list_next = NULL;

        }

    }

    return CSL_SOK;
}

#if 0
/* ================================================================ */
/**
 *  Description:- this routine unregisters the interrupt handler of the specified interrupt source only for the interrupt bank0
 *
 *
 *  @param  int_id       simcop_dma_interrupt_id_t int_id:- is the inpterrupt id for which the handler needs to be unregistered
 *
 *  @return         CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_unregister_interrupt_handler                                               
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
CSL_Status simcop_dma_unregister_interrupt_handler(simcop_dma_interrupt_id_t
                                                   int_id)
{

    /* If no handler already exists return error */
    if (simcop_dma_irq_data[int_id].callback_func == NULL)
    {

        return CSL_ESYS_FAIL;
    }
    simcop_dma_irq_data[int_id].callback_func = NULL;
    simcop_dma_irq_data[int_id].arg1 = 0;
    simcop_dma_irq_data[int_id].arg2 = 0;
    simcop_dma_irq_data[int_id].callback_context =
        SIMCOP_DMA_CALLBACK_CONTEXT_MAX;

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
 *  Description:- this routine configurees the read intervals allocated to diff sub -modules of simcop
 *
 *
 *  @param  cfg  isp_memory_access_ctrl_t* cfg, is the pointer to the config struct
 *
 *  @return         CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     isp_common_read_interface_config                                               
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
CSL_Status isp_common_read_interface_config(isp_memory_access_ctrl_t * cfg)
{
    ISP_SET32(isp_regs->ISP5_BL_MTC_1, cfg->ipipeif_read_interval,
              CSL_ISP5_BL_MTC1_IPIPEIF_R_SHIFT,
              CSL_ISP5_BL_MTC1_IPIPEIF_R_MASK);
    ISP_SET32(isp_regs->ISP5_BL_MTC_1, cfg->isif_read_interval,
              CSL_ISP5_BL_MTC1_ISIF_R_SHIFT, CSL_ISP5_BL_MTC1_ISIF_R_MASK);
    ISP_SET32(isp_regs->ISP5_BL_MTC_2, cfg->h3a_write_interval,
              CSL_ISP5_BL_MTC2_H3A_W_SHIFT, CSL_ISP5_BL_MTC2_H3A_W_MASK);
    return ISP_SUCCESS;
}

#endif

/* ================================================================ */
/**
 *  Description:-this routine enables the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param   int_id,  simcop_dma_interrupt_id_t int_id:- is the inpterrupt id which needs to enabled
 *  @param   bank_num, simcop_dma_interrupt_bank_t banknum:- specifies which bank's interrupt is enabled
 *  @return   CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_common_enable_interrupt                                               
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
CSL_Status simcop_dma_common_enable_interrupt(simcop_dma_interrupt_id_t int_id,
                                              simcop_dma_interrupt_bank_t
                                              bank_num)
{

    volatile uint32 *reg_add = NULL;

    uint32 reg_val = 0;

    // reg_add=(
    // &(simcopDmaRegs->ISP5_IRQENABLE_SET_0))+(bank_num*0x10);/*0x10 is the
    // difference between the register adreeses in each bank*/
    reg_add = (volatile uint32 *) &simcopDmaRegs->DMAIRQ[bank_num].ENABLESET;
    reg_val = 0x00000001 << int_id;

    CSL_RINS((*reg_add), reg_val);

    return CSL_SOK;

}

/* ================================================================ */
/**
 *  Description:-this routine disabled the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    int_id, simcop_dma_interrupt_id_t int_id:- is the inpterrupt id which needs to disabled
 *  @param    bank_num, simcop_dma_interrupt_bank_t bank_num:- specifies which bank's interrupt is disabled
 *
 *  @return        CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_common_disable_interrupt                                               
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
CSL_Status simcop_dma_common_disable_interrupt(simcop_dma_interrupt_id_t int_id,
                                               simcop_dma_interrupt_bank_t
                                               bank_num)
{
    volatile uint32 *reg_add = NULL;

    uint32 reg_val = 0;

    /* 
     * reg_add= (&simcopDmaRegs->ISP5_IRQENABLE_CLR_0);
     * reg_add+=(bank_num*0x10);//0x10 is the difference between the register 
     * adreeses in each bank// */
    reg_add = (volatile uint32 *) &simcopDmaRegs->DMAIRQ[bank_num].ENABLECLR;
    reg_val = 0x00000001 << int_id;

    // ISP_WRITE32((*reg_add),reg_val);
    CSL_RINS((*reg_add), reg_val);

    return CSL_SOK;

}

/* ================================================================ */
/**
 *  Description:- this routine enabled the simcop interrupt at the simcop level
 *
 *
 *
 *  @param  int_id, simcop_dma_interrupt_id_t :- specifies the interrupt number
 *
 *  @return      CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_enable_interrupt                                               
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
CSL_Status simcop_dma_enable_interrupt(simcop_dma_interrupt_id_t int_id)
{
    simcop_dma_common_enable_interrupt(int_id,
                                       simcop_dma_common_devp->
                                       default_int_bank);

    return CSL_SOK;
}

/* ================================================================ */
/**
 *  Description:- disables the specified interrupt
 *  NOTE!!!!!!!!!   This is the exposed function to user the bank is selected to default 0
 *
 *  @param   int_id, simcop_dma_interrupt_id_t int_id
 *
 *  @return     CSL_Status
 */
 /*================================================================== */
/* This is the exposed function to user the bank is selected to default 0 */
/* ===================================================================
 *  @func     simcop_dma_disable_interrupt                                               
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
CSL_Status simcop_dma_disable_interrupt(simcop_dma_interrupt_id_t int_id)
{

    simcop_dma_common_disable_interrupt(int_id,
                                        simcop_dma_common_devp->
                                        default_int_bank);
    return CSL_SOK;
}

/* ================================================================ */
/**
 *  Description:- This is the SWI that needs to exist for handling simcop interrupts,
 *  NOTE!!!!!!!!this is incomplete
 *
 *  @param   none
 *
 *  @return     CSL_Status
 */
 /*================================================================== */
// extern Swi_Handle simcop_dma_swi_handle;
/* ===================================================================
 *  @func     create_swi                                               
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
static CSL_Status create_swi()
{

    Swi_Params swiParams;

    // Task_Params taskParams;
    // Clock_Params clkParams;

    Swi_Params_init(&swiParams);
    swiParams.arg0 = 1;
    swiParams.arg1 = 0;
    swiParams.priority = 2;
    swiParams.trigger = 0;

    // simcop_dma_common_devp->swi_handle=
    // simcop_dma_swi_handle;//Swi_create(isp_common_swi_interrupt_handler,
    // &swiParams, NULL);
    if (NULL == simcop_dma_common_devp->swi_handle)
    {
        simcop_dma_common_devp->swi_handle =
            Swi_create(simcop_dma_swi_interrupt_handler, &swiParams, NULL);
    }

    if (simcop_dma_common_devp->swi_handle == NULL)
        return CSL_ESYS_FAIL;

    return CSL_SOK;
}

/* ================================================================ */
/**
 *  Description:- This routine is called when a swi callback has been registered for a particular event that has occured.
 *
 *
 *  @param   arg0,
 *  @paramand arg1 will be the user arguments while registering the SWI with BIOS
 *
 *  @return        CSL_Status
 */
 /*================================================================== */
static uint32 simcop_dma_error_count = 0;

/* ===================================================================
 *  @func     simcop_dma_swi_interrupt_handler                                               
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
void simcop_dma_swi_interrupt_handler(UArg arg0, UArg arg1)
{

    uint32 i = 0;

    volatile uint32 sts_reg = 0;

    Interrupt_Handle_T *tmp = NULL;

    simcop_dma_interrupt_id_t id;

    sts_reg = simcopDmaRegs->DMAIRQ[simcop_dma_common_dev.default_int_bank].STATUS; // ->ISP5_IRQSTATUS_0;

    for (i = 0; i < SIMCOP_DMA_MAX_IRQ; i++)
    {
        id = simcop_dma_callback_priority[i];
        if (id >= SIMCOP_DMA_MAX_IRQ)
            continue;
        if (((sts_reg) & (0x00000001 << id)) != 0)
        {

            tmp = simcop_dma_swi_cb_list_head[id];
            if (tmp != NULL)
            {

                while (tmp != NULL)
                {
                    if (tmp->callback != NULL)
                    {
                        tmp->callback(CSL_SOK, tmp->arg1, tmp->arg2);
                    }
                    else
                    {
                        simcop_dma_error_count++;          /* ERROR , because 
                                                            * interrupt
                                                            * ocurred and no
                                                            * client has
                                                            * registered, but 
                                                            * list head is
                                                            * still valid!!!! */
                    }
                    tmp = tmp->list_next;
                }

            }
            else
            {
                simcop_dma_error_count++;
                /* ERROR , because interrupt ocurred and no client has
                 * registered */
            }

            /* Clear the particular status bit */
            // simcopDmaRegs->ISP5_IRQSTATUS_0=0x00000001<<id;
            simcopDmaRegs->DMAIRQ[simcop_dma_common_dev.default_int_bank].
                STATUS = 0x00000001 << id;
        }

    }

    /* Enable ISS level interrupt here */
    simcop_enable_interrupt(SIMCOP_DMA0_IRQ);

}

/* ================================================================ */
/**
 *  Description:-This is the HWI that needs to exist for handling simcop interrupts,
 *                     it posts a swi if there is a SWI callback registered for that particular interrupt
 *
 *
 *  @param  arg,	UArg arg
 *
 *  @return      CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_hwi_interrupt_handler                                               
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
static Void simcop_dma_hwi_interrupt_handler(CSL_Status status, uint32 arg1,
                                             void *arg2)
{

    uint32 i = 0;

    volatile uint32 sts_reg = 0;

    uint8 flag = 0;

    Interrupt_Handle_T *tmp = NULL;

    simcop_dma_interrupt_id_t id;

    /* Disable interrupt at M3 level and call SWI, in SWI acknowledge the
     * Interrupt sts register */

    /* Activate swi here only if there is a interrupt that requires a
     * callback in the SWI context */

    // sts_reg=simcopDmaRegs->ISP5_IRQSTATUS_0;
    sts_reg =
        simcopDmaRegs->DMAIRQ[simcop_dma_common_dev.default_int_bank].STATUS;
    /* check all the callbacks that need to be called from HWI context */

    for (i = 0; i < SIMCOP_DMA_MAX_IRQ; i++)
    {
        tmp = NULL;
        id = simcop_dma_callback_priority[i];
        if (id >= SIMCOP_DMA_MAX_IRQ)
            continue;
        if (((sts_reg) & (0x00000001 << id)) != 0)
        {

            tmp = simcop_dma_hwi_cb_list_head[id];
            if (tmp != NULL)
            {

                while (tmp != NULL)
                {
                    if (tmp->callback != NULL)
                    {
                        tmp->callback(CSL_SOK, tmp->arg1, tmp->arg2);

                    }
                    else
                    {
                        simcop_dma_error_count++;          /* ERROR , because 
                                                            * interrupt
                                                            * ocurred and no
                                                            * client has
                                                            * registered, but 
                                                            * list head is
                                                            * still valid */
                    }
                    tmp = tmp->list_next;
                }

            }
            else
            {
                simcop_dma_error_count++;
                /* ERROR , because interrupt ocurred and no client has
                 * registered */
            }

            if (simcop_dma_swi_cb_list_head[id] == NULL)
            {
                /* clear the status bit only if there is no corresponding HWI 
                 * handler. */
                /* the register has one bit for status of HS_VS_IRQ which is
                 * WR and 15 other status bits are RO , instead of handleing
                 * the special case with a "if" statement we just write to
                 * the read only bits */
                // simcopDmaRegs->ISP5_IRQSTATUS_0=0x00000001<<id;
                simcopDmaRegs->DMAIRQ[simcop_dma_common_dev.default_int_bank].
                    STATUS = 0x00000001 << id;
            }
            else
            {
                /* a particular interrupt occured for which there is a valid
                 * swi callback */
                flag = 1;
            }

        }

    }

    /* check if any of the callbacks are to be called in SWI context only if
     * YEs activate the same */

    /* for (i=0;i<SIMCOP_DMA_MAX_IRQ;i++) {
     * if(((sts_reg)&(0x00000001<<i))!=0){
     * 
     * if(simcop_dma_swi_cb_list_head[i]!=NULL) {
     * iss_disable_interrupt(ISS_INTERRUPT_ISP_IRQ0);
     * Swi_post(isp_swi_handle);
     * 
     * 
     * }
     * 
     * } } */

    if (flag == 1)
    {
        /* Need to activate SWI, so diasable the interrupt at ISS level to
         * prevent preemption of SWI while calling the callbacks */

        simcop_disable_interrupt(SIMCOP_DMA0_IRQ);
        // Swi_post(simcop_dma_swi_handle);
        Swi_post(simcop_dma_common_devp->swi_handle);
    }
}

/* ================================================================ */
/**
 *  Description:-this routine needs to be called before calling any simcop submodule's inits, like ipipe_init() etc
 *  				this routine does the basic intialisation of simcop, it disables all interrupts crears;swi's and hwi's,
 *                     does a clean reset of simcop etc
 *!NOTE this is not complete
 *  @param   none
 *
 *  @return        CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_common_init                                               
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
CSL_Status simcop_dma_common_init()
{

    uint8 i = 0, j = 0;

    CSL_Status retval = CSL_SOK;

    // ISS_RETURN retval_iss;

    /* Allocate memory for the simcop struct "simcop_dma_common_dev" and fill 
     * it with swi_handle,hwi_hande etc.... */
    simcop_dma_common_devp = &simcop_dma_common_dev;

    /* Set simcop dma base address here */
    simcopDmaRegs = ((CSL_SimcopDmaRegsOvly) (CSL_COPDMA_0_REGS));

    /* #TODOApply reset thru sysconfig register */

    for (i = 0; ((i < MAX_SIMCOP_DMA_INTERRUPTS) && (retval == CSL_SOK)); i++)
    {
#if 0
        /* Clear all interrupts and set all handles to NULL */
        simcop_dma_irq_data[i].callback_func = NULL;
        simcop_dma_irq_data[i].arg1 = 0;
        simcop_dma_irq_data[i].arg2 = NULL;
        simcop_dma_irq_data[i].callback_context =
            SIMCOP_DMA_CALLBACK_CONTEXT_MAX;
#endif
        /* Clear all interrupts which are already existing */

        /* Disable all interrupts for all banks */
        for (j = 0; j < (SIMCOP_DMA_INTERRUPT_BANK1 + 1); j++)
            retval =
                simcop_dma_common_disable_interrupt((simcop_dma_interrupt_id_t)
                                                    i,
                                                    (simcop_dma_interrupt_bank_t)
                                                    j);

        simcop_dma_hwi_cb_list_head[i] = NULL;
        simcop_dma_swi_cb_list_head[i] = NULL;
    }

    /* Set the default bank -id for interrupt management */
    simcop_dma_common_dev.default_int_bank = SIMCOP_DMA_DEFAULT_INTERRUPT_BANK;

    /* 
     * for(i=0;((i<ISP_MAX_MODULE_CLOCKS)&&(retval==CSL_SOK));i++) {
     * //Disable all clocks//
     * retval=isp_common_disable_clk((ISP_MODULE_CLK_T)i);
     * 
     * } */

    /* #TODO in SYSCONFIG regEnable Autogating */

    /* Create SWI and associated structures. */
    if (create_swi() == CSL_ESYS_FAIL)
    {
        return CSL_ESYS_FAIL;
    }

    /* register the interrupt handler */

    retval =
        simcop_register_interrupt_handler(SIMCOP_DMA0_IRQ,
                                          simcop_dma_hwi_interrupt_handler,
                                          SIMCOP_DMA_INTERRUPT_BANK0, NULL);
    /* if(retval==ISS_SUCCESS) retval=iss_register_interrupt_handler(
     * ISS_INTERRUPT_ISP_IRQ1, isp_Hwi_interrupt_handler,
     * ISP_INTERRUPT_BANK1, NULL); if(retval==CSL_SOK)
     * retval=iss_register_interrupt_handler( ISS_INTERRUPT_ISP_IRQ2,
     * isp_Hwi_interrupt_handler, ISP_INTERRUPT_BANK2, NULL);
     * if(retval==CSL_SOK) retval=iss_register_interrupt_handler(
     * ISS_INTERRUPT_ISP_IRQ3, isp_Hwi_interrupt_handler,
     * ISP_INTERRUPT_BANK3, NULL); */
    if (retval == CSL_SOK)
        simcop_enable_interrupt(SIMCOP_DMA0_IRQ);

    return CSL_SOK;

}

#if 0
/* ================================================================ */
/**
 *  Description:- this should be called while exiting usage of isp, it will release the swi, hwi and other resources
 *  NOTE!!!!!!!this is not complete
 *
 *  @param
 *
 *  @return
 */
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_exit                                               
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
CSL_Status isp_common_exit()
{

    return CSL_SOK;

}
#endif

/* ================================================================ */
/**
 *  Description:- Thie routine returns the sts bit specified.
 *
 *  @param   int_id ,	simcop_dma_interrupt_id_t int_id:- specifies the interrupt number
 *  @param   sts_p,	uint32* sts_p:- pointer to hold the sts value .
 *  @return   CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_common_get_int_sts                                               
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
CSL_Status simcop_dma_common_get_int_sts(simcop_dma_interrupt_id_t int_id,
                                         uint32 * sts_p)
{

    uint32 val = 0;

    // val= simcopDmaRegs->ISP5_IRQSTATUS_RAW_0;
    val = simcopDmaRegs->DMAIRQ[simcop_dma_common_dev.default_int_bank].STATUS;

    *sts_p = (val >> int_id) & 0x00000001;
    return CSL_SOK;

}
