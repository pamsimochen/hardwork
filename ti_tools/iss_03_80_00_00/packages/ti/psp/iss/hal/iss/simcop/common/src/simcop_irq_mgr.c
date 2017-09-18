/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file   common.c , 
 *          This file contins the functionality to configure interrupt functionality shared 
              between different Simcop compponents.
 * 
 *
 * @path Centaurus\drivers\drv_isp\src\csl
 *
 * @rev 1.0 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *! 
 *========================================================================= */

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/
#include "../simcop_irq.h"
// #include "../../isp5_utils/isp5_sys_types.h"
// #include "../../isp5_utils/isp5_csl_utils.h"

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>
#include <ti/psp/iss/hal/iss/iss_common/iss_interrupt_config.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_error.h>
/*************************************************************************/

/* ##TODO################# 1) DMA request for Gamma ? 2) MSTANDBY 3)
 * PSYNC_CLK_SEL? 4) VBUSM_CPRIORITY 5) OCP_WRNP 6) ISP5_SYSCONFIG 7)
 * ISP5_IRQ_EOI 8) IPIPE_GAMMA_RGB_COPY
 * 
 */

/*******************************************************************************************/
/* GLOBAL DATA DEFINITION */

/*******************************************************************************************/
// isp_regs_ovly isp_regs;
static CSL_SimcopRegsOvly simcopRegs;

/* simcop_irq_data should be a 2-dimensional array of
 * [MAX_ISP_INTERRUPTS][isp_interrupt_bank3+1] Right now since we d not need
 * multiple interrupts form different banks, we will use the default bank 0
 * 
 */
static simcop_irq_t simcop_irq_data[MAX_SIMCOP_INTERRUPTS];

static simcop_common_dev_data_t simcop_common_dev;

static simcop_common_dev_data_t *simcop_common_devp;

static Interrupt_Handle_T *simcop_hwi_cb_list_head[SIMCOP_MAX_IRQ] = { NULL };
static Interrupt_Handle_T *simcop_swi_cb_list_head[SIMCOP_MAX_IRQ] = { NULL };

static simcop_interrupt_id_t simcop_callback_priority[] = {

    SIMCOP_RESERVED12_IRQ,
    SIMCOP_RESERVED11_IRQ,
    SIMCOP_RESERVED10_IRQ,
    SIMCOP_RESERVED09_IRQ,
    SIMCOP_RESERVED08_IRQ,
    SIMCOP_RESERVED07_IRQ,
    SIMCOP_RESERVED06_IRQ,
    SIMCOP_RESERVED05_IRQ,
    SIMCOP_RESERVED04_IRQ,
    SIMCOP_RESERVED03_IRQ,
    SIMCOP_RESERVED02_IRQ,
    SIMCOP_RESERVED01_IRQ,
    SIMCOP_CPUPROCSTART_IRQ,
    SIMCOP_DMA1_IRQ,
    SIMCOP_MTCR2OCPERR_IRQ,
    SIMCOP_OCPERR_IRQ,
    SIMCOP_VLCDJDECODEERR_IRQ,
    SIMCOP_DMA0_IRQ,
    SIMCOP_STEP0_IRQ,
    SIMCOP_STEP1_IRQ,
    SIMCOP_STEP2_IRQ,
    SIMCOP_STEP3_IRQ,
    SIMCOP_DONE_IRQ,
    SIMCOP_LDCBLOCK_IRQ,
    SIMCOP_RESERVED00_IRQ,
    SIMCOP_ROTA_IRQ,
    SIMCOP_IMXB_IRQ,
    SIMCOP_IMXA_IRQ,
    SIMCOP_NSF_IRQ,
    SIMCOP_VLCDJBLOCK_IRQ,
    SIMCOP_DCT_IRQ,
    SIMCOP_LDCFRAME_IRQ
};

/* ================================================================ */
/**
 *  simcop_unhook_int_handler()
 *  Description :- This routine de-links the argument "handle " froma priority linked list for the  interrupt 
 *			     specified by the argument "int_id". the handle is de-linked from the swi list or hwi list as specified 
 *                       by the parameter "context" within handle type 
 *  
 *
 *  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

 *  @return  :- CSL_Status       
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_unhook_int_handler                                               
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
CSL_Status simcop_unhook_int_handler(simcop_interrupt_id_t int_id,
                                     Interrupt_Handle_T * handle)
{

    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL, *head_ptr =
        (Interrupt_Handle_T *) NULL;
    uint8 flag = 0;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSL_ESYS_FAIL;
    if (int_id >= SIMCOP_MAX_IRQ)
        return (CSL_ESYS_FAIL);
    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        head_ptr = simcop_hwi_cb_list_head[int_id];
    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        head_ptr = simcop_swi_cb_list_head[int_id];
    else
        return CSL_ESYS_FAIL;

    if (head_ptr == NULL)
        return CSL_ESYS_FAIL;

    /* 
     * If the handle is the first element update the head ptr */

    if (handle == head_ptr)
    {

        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        {
            simcop_hwi_cb_list_head[int_id] = head_ptr->list_next;

        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            simcop_swi_cb_list_head[int_id] = head_ptr->list_next;

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
 *  simcop_hook_int_handler()
 *  Description :- This routine links the argument "handle " to a priority linked list for the  interrupt 
 *			     specified by the argument "int_id". the handle is linked to the swi list or hwi list as specified 
 *                       by the parameter "context" within handle type 
 *  
 *
 *  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

 *  @return  :- CSL_Status       
 */
 /*================================================================== */

/* ===================================================================
 *  @func     simcop_hook_int_handler                                               
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
CSL_Status simcop_hook_int_handler(simcop_interrupt_id_t int_id,
                                   Interrupt_Handle_T * handle)
{
    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL,
        *head_ptr = (Interrupt_Handle_T *) NULL;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSL_ESYS_FAIL;

    if (handle->callback == NULL)
        /* return error since both callbacks shouldnt be zero */
        return CSL_ESYS_FAIL;
    if (int_id >= SIMCOP_MAX_IRQ)
        return (CSL_ESYS_FAIL);

    handle->list_next = NULL;

    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        head_ptr = simcop_hwi_cb_list_head[int_id];
    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        head_ptr = simcop_swi_cb_list_head[int_id];
    /* Go to end of list and link the handle at the end */
    ptr_curr = head_ptr;

    /* Need to protect the following piece of code from pre-emption. */
    /* Check if node need to goto the head of the list */
    if ((head_ptr == NULL) || (handle->priority < head_ptr->priority))
    {

        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)

        {
            handle->list_next = simcop_hwi_cb_list_head[int_id];
            simcop_hwi_cb_list_head[int_id] = handle;
        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            handle->list_next = simcop_swi_cb_list_head[int_id];
            simcop_swi_cb_list_head[int_id] = handle;
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

/* ================================================================ */
/**
 *  Description:- this routine sets up the interrupt callback for the desired interruptonly for the interrupt bank0, it does not enable the interrupt
 *                
 *
 *  @param int_id  simcop_interrupt_id_t  int_id:- is the inpterrupt id for which the handler needs to be registered
 *  @param callback_func   simcop_callback_t callback_func:- is the callback function that needs to be registered
 *  @param 		arg1, arg2 are the params that the callback will be invoked with when interrupt occurs	
 *  @return        CSL_Status 
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_register_interrupt_handler                                               
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
CSL_Status simcop_register_interrupt_handler(simcop_interrupt_id_t int_id, simcop_callback_t callback_func, uint32 arg1, void *arg2)    // /*, 
                                                                                                                                        // SIMCOP_CALLBACK_CONTEXT_T 
                                                                                                                                        // context*/
{
    /* If a handler already exists return error */
    if (simcop_irq_data[int_id].callback_func != NULL)
    {

        return CSL_ESYS_FAIL;
    }
    simcop_irq_data[int_id].callback_func = callback_func;
    simcop_irq_data[int_id].arg1 = arg1;
    simcop_irq_data[int_id].arg2 = arg2;

    // simcop_irq_data[int_id].callback_context=context;
    return CSL_SOK;
}

/* ================================================================ */
/**
 *  Description:- this routine unregisters the interrupt handler of the specified interrupt source only for the interrupt bank0
 *  
 *
 *  @param  int_id       simcop_interrupt_id_t int_id:- is the inpterrupt id for which the handler needs to be unregistered

 *  @return         CSL_Status
*/
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_unregister_interrupt_handler                                               
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
CSL_Status simcop_unregister_interrupt_handler(simcop_interrupt_id_t int_id)
{

    /* If no handler already exists return error */
    if (simcop_irq_data[int_id].callback_func == NULL)
    {

        return CSL_ESYS_FAIL;
    }
    simcop_irq_data[int_id].callback_func = NULL;
    simcop_irq_data[int_id].arg1 = 0;
    simcop_irq_data[int_id].arg2 = 0;
    simcop_irq_data[int_id].callback_context = SIMCOP_CALLBACK_CONTEXT_MAX;

    return CSL_SOK;

}

#if 0
/* ================================================================ */
/**
 *  Description:- this routine configurees the read intervals allocated to diff sub -modules of simcop
 *  
 *
 *  @param  cfg  isp_memory_access_ctrl_t* cfg, is the pointer to the config struct

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
    return CSL_SOK;
}

#endif

/* ================================================================ */
/**
 *  Description:-this routine enables the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param   int_id,  simcop_interrupt_id_t int_id:- is the inpterrupt id which needs to enabled
 *  @param   bank_num, simcop_interrupt_bank_t banknum:- specifies which bank's interrupt is enabled
 *  @return   CSL_Status    
*/
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_common_enable_interrupt                                               
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
CSL_Status simcop_common_enable_interrupt(simcop_interrupt_id_t int_id,
                                          simcop_interrupt_bank_t bank_num)
{

    volatile uint32 *reg_add = NULL;

    uint32 reg_val = 0;

    // reg_add=( &(simcopRegs->ISP5_IRQENABLE_SET_0))+(bank_num*0x10);/*0x10
    // is the difference between the register adreeses in each bank*/
    reg_add = (volatile uint32 *) &simcopRegs->HL_IRQ[bank_num].ENABLE_SET;
    reg_val = 0x00000001 << int_id;

    CSL_RINS((*reg_add), reg_val);

    return CSL_SOK;

}

/* ================================================================ */
/**
 *  Description:-this routine disabled the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    int_id, simcop_interrupt_id_t int_id:- is the inpterrupt id which needs to disabled
 *  @param    bank_num, simcop_interrupt_bank_t bank_num:- specifies which bank's interrupt is disabled
 *
 *  @return        CSL_Status
 */
 /*================================================================== */

/* ===================================================================
 *  @func     simcop_common_disable_interrupt                                               
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
CSL_Status simcop_common_disable_interrupt(simcop_interrupt_id_t int_id,
                                           simcop_interrupt_bank_t bank_num)
{
    volatile uint32 *reg_add = NULL;

    uint32 reg_val = 0;

    /* 
     * reg_add= (&simcopRegs->ISP5_IRQENABLE_CLR_0);
     * reg_add+=(bank_num*0x10);//0x10 is the difference between the register 
     * adreeses in each bank// */
    reg_add = (volatile uint32 *) &simcopRegs->HL_IRQ[bank_num].ENABLE_CLR;
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
 *  @param  int_id, simcop_interrupt_id_t :- specifies the interrupt number

 *  @return      CSL_Status
*/
 /*================================================================== */

/* ===================================================================
 *  @func     simcop_enable_interrupt                                               
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
CSL_Status simcop_enable_interrupt(simcop_interrupt_id_t int_id)
{
    simcop_common_enable_interrupt(int_id,
                                   simcop_common_devp->default_int_bank);

    return CSL_SOK;
}

/* ================================================================ */
/**
 *  Description:- disables the specified interrupt
 *  NOTE!!!!!!!!!   This is the exposed function to user the bank is selected to default 0
 *
 *  @param   int_id, simcop_interrupt_id_t int_id

 *  @return     CSL_Status
*/
 /*================================================================== */
/* This is the exposed function to user the bank is selected to default 0 */
/* ===================================================================
 *  @func     simcop_disable_interrupt                                               
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
CSL_Status simcop_disable_interrupt(simcop_interrupt_id_t int_id)
{

    simcop_common_disable_interrupt(int_id,
                                    simcop_common_devp->default_int_bank);
    return CSL_SOK;
}

/* ================================================================ */
/**
 *  Description:- This is the SWI that needs to exist for handling simcop interrupts,
 *  NOTE!!!!!!!!this is incomplete
 *
 *  @param   none

 *  @return     CSL_Status    
*/
 /*================================================================== */
// RAJAT:TODO
extern Swi_Handle simcop_swi_handle;

// Swi_Handle simcop_swi_handle;
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

    simcop_common_devp->swi_handle = simcop_swi_handle;    // Swi_create(isp_common_swi_interrupt_handler, 
                                                           // &swiParams,
                                                           // NULL);

    if (simcop_common_devp->swi_handle == NULL)
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
static uint32 simcop_error_count = 0;

/* ===================================================================
 *  @func     simcop_swi_interrupt_handler                                               
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
void simcop_swi_interrupt_handler(UArg arg0, UArg arg1)
{
    uint32 i = 0;

    volatile uint32 sts_reg = 0;

    Interrupt_Handle_T *tmp = NULL;

    simcop_interrupt_id_t id;

    sts_reg = simcopRegs->HL_IRQ[simcop_common_devp->default_int_bank].STATUS;  // ->ISP5_IRQSTATUS_0;
    for (i = 0; i < SIMCOP_MAX_IRQ; i++)
    {
        id = simcop_callback_priority[i];
        if (id >= SIMCOP_MAX_IRQ)
            continue;
        if (((sts_reg) & (0x00000001 << id)) != 0)
        {
            /* if(id == SIMCOP_DMA1_IRQ || id == SIMCOP_DMA0_IRQ) {
             * if(simcop_irq_data[id].callback_func != NULL)//&&
             * simcop_irq_data[id].callback_context ==
             * SIMCOP_CALLBACK_CONTEXT_SWI {
             * simcop_irq_data[id].callback_func(CSL_SOK,
             * simcop_irq_data[id].arg1, simcop_irq_data[id].arg2); } else {
             * simcop_error_count++; // ERROR , cause no handler is present
             * for this particular interrupt// } //Clear the particular
             * status bit//
             * simcopRegs->HL_IRQ[simcop_common_devp->default_int_bank].STATUS=(0x00000001<<id); 
             * } else */
            {
                tmp = simcop_swi_cb_list_head[id];
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
                            simcop_error_count++;          /* ERROR , because 
                                                            * interrupt
                                                            * ocurred and no
                                                            * client has
                                                            * registered,
                                                            * but list head
                                                            * is still
                                                            * valid!!!! */
                        }
                        tmp = tmp->list_next;
                    }
                }
                else
                {
                    simcop_error_count++;
                    /* ERROR , because interrupt ocurred and no client has
                     * registered */
                }
            }

            /* Clear the particular status bit */
            // simcopRegs->ISP5_IRQSTATUS_0=0x00000001<<id;
            simcopRegs->HL_IRQ[simcop_common_devp->default_int_bank].STATUS = 0x00000001 << id; // Was 
                                                                                                // Bank 
                                                                                                // 0 
                                                                                                // changed 
                                                                                                // to 
                                                                                                // use 
                                                                                                // the 
                                                                                                // value 
                                                                                                // in 
                                                                                                // simcop_common_devp->default_int_bank
        }

    }
    /* Enable ISS level interrupt here */
    iss_enable_interrupt(ISS_INTERRUPT_SIMCOP_IRQ0);

}

/* ================================================================ */
/**
 *  Description:-This is the HWI that needs to exist for handling simcop interrupts,
 *                     it posts a swi if there is a SWI callback registered for that particular interrupt
 *  
 *
 *  @param  arg,	UArg arg 

 *  @return      CSL_Status   
*/
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_hwi_interrupt_handler                                               
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
static void simcop_hwi_interrupt_handler(ISS_RETURN status, uint32 arg1,
                                         void *arg2)
{

    uint32 i = 0;

    volatile uint32 sts_reg = 0;

    uint8 flag = 0;

    Interrupt_Handle_T *tmp = NULL;

    simcop_interrupt_id_t id;

    /* Disable interrupt at M3 level and call SWI, in SWI acknowledge the
     * Interrupt sts register */
    /* Activate swi here only if there is a interrupt that requires a
     * callback in the SWI context */

    // sts_reg=simcopRegs->ISP5_IRQSTATUS_0;
    sts_reg = simcopRegs->HL_IRQ[simcop_common_devp->default_int_bank].STATUS;
    /* cheack all the callbacks that need to be called from HWI context */

    for (i = 0; i < SIMCOP_MAX_IRQ; i++)
    {
        tmp = NULL;
        id = simcop_callback_priority[i];
        if (id >= SIMCOP_MAX_IRQ)
            continue;
        if (((sts_reg) & (0x00000001 << id)) != 0)
        {
            if (id == SIMCOP_DMA1_IRQ || id == SIMCOP_DMA0_IRQ)
            {
                if (simcop_irq_data[id].callback_func != NULL)  /* &&
                                                                 * simcop_irq_data[id].callback_context 
                                                                 * ==
                                                                 * SIMCOP_CALLBACK_CONTEXT_HWI */
                {
                    simcop_irq_data[id].callback_func(CSL_SOK,
                                                      simcop_irq_data[id].arg1,
                                                      simcop_irq_data[id].arg2);
                }
                /* else if(simcop_irq_data[id].callback_func != NULL &&
                 * simcop_irq_data[id].callback_context ==
                 * SIMCOP_CALLBACK_CONTEXT_SWI) { flag = 1; } else {
                 * simcop_error_count++; //ERROR , cause no handler is
                 * present for this particular interrupt// } //Clear the
                 * particular status bit//
                 * simcopRegs->HL_IRQ[simcop_common_devp->default_int_bank].STATUS=(0x00000001<<id); 
                 */
            }
            else
            {
                tmp = simcop_hwi_cb_list_head[id];
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
                            simcop_error_count++;          /* ERROR , because 
                                                            * interrupt
                                                            * ocurred and no
                                                            * client has
                                                            * registered,
                                                            * but list head
                                                            * is still valid */
                        }
                        tmp = tmp->list_next;
                    }
                }
                else
                {
                    simcop_error_count++;
                    /* ERROR , because interrupt ocurred and no client has
                     * registered */
                }
            }

            if (simcop_swi_cb_list_head[id] == NULL)
            {
                /* clear the status bit only if there is no corresponding HWI 
                 * handler. */
                /* the register has one bit for status of HS_VS_IRQ which is
                 * WR and 15 other status bits are RO , instead of handleing
                 * the special case with a "if" statement we just write to
                 * the read only bits */
                // simcopRegs->ISP5_IRQSTATUS_0=0x00000001<<id;
                simcopRegs->HL_IRQ[simcop_common_devp->default_int_bank].
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

    /* for (i=0;i<SIMCOP_MAX_IRQ;i++) { if(((sts_reg)&(0x00000001<<i))!=0){
     * 
     * if(simcop_swi_cb_list_head[i]!=NULL) {
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
        iss_disable_interrupt(ISS_INTERRUPT_SIMCOP_IRQ0);
        Swi_post(simcop_swi_handle);
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
 *  @func     simcop_common_init                                               
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
CSL_Status simcop_common_init()
{

    uint8 i = 0, j = 0;

    CSL_Status retval = CSL_SOK;

    ISS_RETURN retval_iss;

    /* Allocate memory for the simcop struct "simcop_common_dev" and fill it
     * with swi_handle,hwi_hande etc.... */
    simcop_common_devp = &simcop_common_dev;

    /* Set simcop base address here */
    simcopRegs = ((CSL_SimcopRegsOvly) (CSL_SIMCOP_BASE_ADDRESS));

    /* #TODOApply reset thru sysconfig register */

    for (i = 0; ((i < MAX_SIMCOP_INTERRUPTS) && (retval == CSL_SOK)); i++)
    {
        /* Clear all interrupts and set all handles to NULL */
        simcop_irq_data[i].callback_func = NULL;
        simcop_irq_data[i].arg1 = 0;
        simcop_irq_data[i].arg2 = NULL;
        simcop_irq_data[i].callback_context = SIMCOP_CALLBACK_CONTEXT_MAX;

        /* Clear all interrupts which are already existing */

        /* Disable all interrupts for all banks */
        for (j = 0; j < (SIMCOP_INTERRUPT_BANK3 + 1); j++)
            retval =
                simcop_common_disable_interrupt((simcop_interrupt_id_t) i,
                                                (simcop_interrupt_bank_t) j);

        simcop_hwi_cb_list_head[i] = NULL;
        simcop_swi_cb_list_head[i] = NULL;
    }

    /* Set the default bank -id for interrupt management */
    simcop_common_dev.default_int_bank = SIMCOP_DEFAULT_INTERRUPT_BANK;

    /* 
     * for(i=0;((i<ISP_MAX_MODULE_CLOCKS)&&(retval==ISP_SUCCESS));i++) {
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

    retval_iss =
        iss_register_interrupt_handler(ISS_INTERRUPT_SIMCOP_IRQ0,
                                       simcop_hwi_interrupt_handler,
                                       SIMCOP_INTERRUPT_BANK0, NULL);
    /* if(retval_iss==ISS_SUCCESS) retval_iss=iss_register_interrupt_handler( 
     * ISS_INTERRUPT_ISP_IRQ1, isp_Hwi_interrupt_handler,
     * ISP_INTERRUPT_BANK1, NULL); if(retval_iss==ISP_SUCCESS)
     * retval_iss=iss_register_interrupt_handler( ISS_INTERRUPT_ISP_IRQ2,
     * isp_Hwi_interrupt_handler, ISP_INTERRUPT_BANK2, NULL);
     * if(retval_iss==ISP_SUCCESS) retval_iss=iss_register_interrupt_handler( 
     * ISS_INTERRUPT_ISP_IRQ3, isp_Hwi_interrupt_handler,
     * ISP_INTERRUPT_BANK3, NULL); */
    if (retval_iss == 0)
        iss_enable_interrupt(ISS_INTERRUPT_SIMCOP_IRQ0);

    return CSL_SOK;

}

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
/* CSL_Status isp_common_exit() {
 * 
 * return ISP_SUCCESS;
 * 
 * } */

/* ================================================================ */
/**
 *  Description:- Thie routine returns the sts bit specified.
 *
 *  @param   int_id ,	simcop_interrupt_id_t int_id:- specifies the interrupt number
 *  @param   sts_p,	uint32* sts_p:- pointer to hold the sts value .
 *  @return   CSL_Status      
*/
 /*================================================================== */

/* ===================================================================
 *  @func     simcop_common_get_int_sts                                               
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
CSL_Status simcop_common_get_int_sts(simcop_interrupt_id_t int_id,
                                     uint32 * sts_p)
{

    uint32 val = 0;

    // val= simcopRegs->ISP5_IRQSTATUS_RAW_0;
    val = simcopRegs->HL_IRQ[0].STATUS;

    *sts_p = (val >> int_id) & 0x00000001;
    return CSL_SOK;

}
