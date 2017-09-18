/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file   common.c ,
*          This file contins the functionality to configure  ISP5 common functionality shared
between different ISP compponents like,interrupt handling clock management etc.
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
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>

#include "../inc/isp_common.h"
#include "../../isp5_utils/isp5_sys_types.h"
#include "../../isp5_utils/isp5_csl_utils.h"

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>
#include <ti/psp/iss/hal/iss/iss_common/iss_interrupt_config.h>

/*************************************************************************/

/* ##TODO################# 1) DMA request for Gamma ? 2) MSTANDBY 3)
 * PSYNC_CLK_SEL? 4) VBUSM_CPRIORITY 5) OCP_WRNP 6) ISP5_SYSCONFIG 7)
 * ISP5_IRQ_EOI 8) IPIPE_GAMMA_RGB_COPY
 *
 */

/*******************************************************************************************/
/* GLOBAL DATA DEFINITION */

/*******************************************************************************************/
isp_regs_ovly isp_regs;

/* isp_irq_data should be a 2-dimensional array of
 * [MAX_ISP_INTERRUPTS][isp_interrupt_bank3+1] Right now since we d not need
 * multiple interrupts form different banks, we will use the default bank 0
 *
 */
isp_irq_t isp_irq_data[MAX_ISP_INTERRUPTS];

isp_common_dev_data_t isp_common_dev;

isp_common_dev_data_t *isp_common_devp;

/* ================================================================ */
/**
*  Description:- this routine sets up the interrupt callback for the desired interruptonly for the interrupt bank0, it does not enable the interrupt
*
*
*  @param int_id  isp_interrupt_id_t  int_id:- is the inpterrupt id for which the handler needs to be registered
*  @param callback_func   isp_callback_t callback_func:- is the callback function that needs to be registered
*  @param       arg1, arg2 are the params that the callback will be invoked with when interrupt occurs
*  @return        ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_register_interrupt_handler
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
ISP_RETURN isp_register_interrupt_handler(isp_interrupt_id_t int_id,
                                          isp_callback_t callback_func,
                                          uint32 arg1, void *arg2,
                                          ISP_CALLBACK_CONTEXT_T context)
{
    /* If a handler already exists return error */
    if (isp_irq_data[int_id].callback_func != NULL)
    {

        return ISP_FAILURE;
    }
    isp_irq_data[int_id].callback_func = callback_func;
    isp_irq_data[int_id].arg1 = arg1;
    isp_irq_data[int_id].arg2 = arg2;

    isp_irq_data[int_id].callback_context = context;
    return ISP_SUCCESS;
}

Interrupt_Handle_T *isp_hwi_cb_list_head[ISP_INT_MAX] = { NULL };
Interrupt_Handle_T *isp_swi_cb_list_head[ISP_INT_MAX] = { NULL };

isp_interrupt_id_t isp_callback_priority[] = {
    ISP_Reserved,
    ISP_OCP_ERR_IRQ,
    ISP_IPIPE_INT_DPC_RNEW1,
    ISP_IPIPE_INT_DPC_RNEW0,
    ISP_IPIPE_INT_DPC_INI,
    ISP_Reserved0,
    ISP_IPIPE_INT_EOF,
    ISP_H3A_INT_EOF,
    ISP_RSZ_INT_EOF1,
    ISP_RSZ_INT_EOF0,
    ISP_Reserved1,
    ISP_Reserved2,
    ISP_Reserved3,
    ISP_RSZ_FIFO_IN_OVF,
    ISP_RSZ_INT_CYC_RZB,
    ISP_RSZ_INT_CYC_RZA,
    ISP_RSZ_INT_DMA,
    ISP_RSZ_INT_LAST_PIX,
    ISP_RSZ_INT_REG,
    ISP_H3A_INT,
    ISP_AF_INT,
    ISP_AEW_INT,
    ISP_IPIPEIF_IRQ,
    ISP_IPIPE_INT_HST,
    ISP_IPIPE_INT_BSC,
    ISP_IPIPE_INT_DMA,
    ISP_IPIPE_INT_LAST_PIX,
    ISP_IPIPE_INT_REG,
    ISP_ISIF_INT_3,
    ISP_ISIF_INT_2,
    ISP_ISIF_INT_1, ISP_ISIF_INT_0
};

/* ================================================================ */
/**
*  isp_unhook_int_handler()
*  Description :- This routine de-links the argument "handle " froma priority linked list for the  interrupt
*                specified by the argument "int_id". the handle is de-linked from the swi list or hwi list as specified
*                       by the parameter "context" within handle type
*
*
*  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

*  @return  :- ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_unhook_int_handler
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
ISP_RETURN isp_unhook_int_handler(isp_interrupt_id_t int_id,
                                  Interrupt_Handle_T * handle)
{

    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL, *head_ptr =
        (Interrupt_Handle_T *) NULL;
    uint8 flag = 0;

    if (int_id >= ISP_INT_MAX)
        return ISP_FAILURE;
    if (handle == (Interrupt_Handle_T *) NULL)
        return ISP_FAILURE;
    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        head_ptr = isp_hwi_cb_list_head[int_id];
    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        head_ptr = isp_swi_cb_list_head[int_id];
    else
        return ISP_FAILURE;

    if (head_ptr == NULL)
        return ISP_FAILURE;

    /*
     * If the handle is the first element update the head ptr */

    if (handle == head_ptr)
    {

        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        {
            isp_hwi_cb_list_head[int_id] = head_ptr->list_next;

        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            isp_swi_cb_list_head[int_id] = head_ptr->list_next;

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
            return ISP_FAILURE;

    }

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  isp_hook_int_handler()
*  Description :- This routine links the argument "handle " to a priority linked list for the  interrupt
*                specified by the argument "int_id". the handle is linked to the swi list or hwi list as specified
*                       by the parameter "context" within handle type
*
*
*  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

*  @return  :- ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_hook_int_handler
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
ISP_RETURN isp_hook_int_handler(isp_interrupt_id_t int_id,
                                Interrupt_Handle_T * handle)
{
    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL;

    Interrupt_Handle_T *head_ptr = (Interrupt_Handle_T *) NULL;

    if (handle == (Interrupt_Handle_T *) NULL)
        return ISP_FAILURE;

    if (handle->callback == NULL)
        /* return error since both callbacks shouldnt be zero */
        return ISP_FAILURE;

    if (int_id >= ISP_INT_MAX)
        return ISP_FAILURE;

    handle->list_next = NULL;

    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
    {
        head_ptr = isp_hwi_cb_list_head[int_id];
    }
    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
    {
        head_ptr = isp_swi_cb_list_head[int_id];
    }
    /* Go to end of list and link the handle at the end */
    ptr_curr = head_ptr;

    /* Need to protect the following piece of code from pre-emption. */
    /* Check if node need to goto the head of the list */
    if ((head_ptr == NULL) || (handle->priority < head_ptr->priority))
    {
        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        {
            handle->list_next = isp_hwi_cb_list_head[int_id];
            isp_hwi_cb_list_head[int_id] = handle;
        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            handle->list_next = isp_swi_cb_list_head[int_id];
            isp_swi_cb_list_head[int_id] = handle;
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

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- this routine unregisters the interrupt handler of the specified interrupt source only for the interrupt bank0
*
*
*  @param  int_id       isp_interrupt_id_t int_id:- is the inpterrupt id for which the handler needs to be unregistered

*  @return         ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_unregister_interrupt_handler
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
ISP_RETURN isp_unregister_interrupt_handler(isp_interrupt_id_t int_id)
{

    /* If no handler already exists return error */
    if (isp_irq_data[int_id].callback_func == NULL)
    {

        return ISP_FAILURE;
    }
    isp_irq_data[int_id].callback_func = NULL;
    isp_irq_data[int_id].arg1 = 0;
    isp_irq_data[int_id].arg2 = 0;
    isp_irq_data[int_id].callback_context = ISP_CALLBACK_CONTEXT_MAX;

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- this routine configurees the read intervals allocated to diff sub -modules of isp
*
*
*  @param  cfg  isp_memory_access_ctrl_t* cfg, is the pointer to the config struct

*  @return         ISP_RETURN
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
ISP_RETURN isp_common_read_interface_config(isp_memory_access_ctrl_t * cfg)
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

/* ================================================================ */
/**
*  Description:-this routine enables the specified interrupt
*  NOTE!!!this is intended to be called internally only
*
*  @param   int_id,  isp_interrupt_id_t int_id:- is the inpterrupt id which needs to enabled
*  @param   bank_num, isp_interrupt_bank_t banknum:- specifies which bank's interrupt is enabled
*  @return   ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_enable_interrupt
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
ISP_RETURN isp_common_enable_interrupt(isp_interrupt_id_t int_id,
                                       isp_interrupt_bank_t bank_num)
{

    volatile uint32 *reg_add = NULL;

    uint32 reg_val = 0;

    reg_add = (volatile uint32 *) ((&(isp_regs->ISP5_IRQSTATUS_0)) + (bank_num * 0x10));    /* 0x10
                                                                                             * is
                                                                                             * the
                                                                                             * difference
                                                                                             * between
                                                                                             * the
                                                                                             * register
                                                                                             * adreeses
                                                                                             * in
                                                                                             * each
                                                                                             * bank */
    reg_val = 0x00000001 << int_id;

    ISP_WRITE32((*reg_add), reg_val);

    reg_add = (volatile uint32 *) (&(isp_regs->ISP5_IRQENABLE_SET_0) + (bank_num * 0x10));  /* 0x10
                                                                                             * is
                                                                                             * the
                                                                                             * difference
                                                                                             * between
                                                                                             * the
                                                                                             * register
                                                                                             * adreeses
                                                                                             * in
                                                                                             * each
                                                                                             * bank */
    reg_val = 0x00000001 << int_id;

    ISP_WRITE32((*reg_add), reg_val);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:-this routine disabled the specified interrupt
*  NOTE!!!this is intended to be called internally only
*
*  @param    int_id, isp_interrupt_id_t int_id:- is the inpterrupt id which needs to disabled
*  @param    bank_num, isp_interrupt_bank_t bank_num:- specifies which bank's interrupt is disabled
*
*  @return        ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_common_disable_interrupt
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
ISP_RETURN isp_common_disable_interrupt(isp_interrupt_id_t int_id,
                                        isp_interrupt_bank_t bank_num)
{
    volatile uint32 *reg_add = NULL;

    uint32 reg_val = 0;

    reg_add = (&isp_regs->ISP5_IRQENABLE_CLR_0);
    reg_add += (bank_num * 0x10);                          /* 0x10 is the
                                                            * difference
                                                            * between the
                                                            * register
                                                            * adreeses in
                                                            * each bank */
    reg_val = 0x00000001 << int_id;

    ISP_WRITE32((*reg_add), reg_val);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:- this routine enabled the ISP interrupt at the ISP level
*
*
*
*  @param  int_id, isp_interrupt_id_t :- specifies the interrupt number

*  @return      ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_enable_interrupt
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
ISP_RETURN isp_enable_interrupt(isp_interrupt_id_t int_id)
{
    isp_common_enable_interrupt(int_id, isp_common_devp->default_int_bank);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- disables the specified interrupt
*  NOTE!!!!!!!!!   This is the exposed function to user the bank is selected to default 0
*
*  @param   int_id, isp_interrupt_id_t int_id

*  @return     ISP_RETURN
*/
/*================================================================== */
/* This is the exposed function to user the bank is selected to default 0 */
/* ===================================================================
 *  @func     isp_disable_interrupt
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
ISP_RETURN isp_disable_interrupt(isp_interrupt_id_t int_id)
{

    isp_common_disable_interrupt(int_id, isp_common_devp->default_int_bank);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:-disables the specified module's clock
*
*
*  @param   module_id, ISP_MODULE_CLK_T module_id:- the module name

*  @return    ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_disable_clk
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
ISP_RETURN isp_common_disable_clk(ISP_MODULE_CLK_T module_id)
{

    /* Disable the module clk in ISP5_ctrl register */
    ISP_SET32(isp_regs->ISP5_CTRL, ISP_CLK_DISABLE, module_id,
              (0x00000001 << module_id));
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:-enables the clock of required module in isp
*
*
*  @param  module_id, ISP_MODULE_CLK_T module_id:- the module name

*  @return       ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_enable_clk
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
ISP_RETURN isp_common_enable_clk(ISP_MODULE_CLK_T module_id)
{
    ISP_SET32(isp_regs->ISP5_CTRL, ISP_CLK_ENABLE, module_id,
              (0x00000001 << module_id));

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- configures the ip port of ISP
*
*
*  @param  ip_port_cfg, ISP_IP_PORT_T ip_port_cfg:- selects between CSI2 and CCP2

*  @return    ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_common_ip_port_cfg
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
ISP_RETURN isp_common_ip_port_cfg(ISP_IP_PORT_T ip_port_cfg)
{

    ISP_SET32(isp_regs->ISP5_CTRL, ip_port_cfg,
              CSL_ISP5_CTRL_VD_PULSE_EXT_SHIFT,
              CSL_ISP5_CTRL_VD_PULSE_EXT_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- polarity of the clocks is selected in this routine
*
*
*  @param   pixclk_pol,  ISP_PIXEL_CLK_POL_T    pixclk_pol:-selects the polarity of the clocks, nornmal or inverted

*  @return         ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_common_pixclk_polarity_cfg
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
ISP_RETURN isp_common_pixclk_polarity_cfg(ISP_PIXEL_CLK_POL_T pixclk_pol)
{
    ISP_SET32(isp_regs->ISP5_CTRL, pixclk_pol, CSL_ISP5_CTRL_PCLK_INV_SHIFT,
              CSL_ISP5_CTRL_PCLK_INV_MASK);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:-cfg's sync ctrl
*
*
*  @param   sync_ctrl ISP_SYNC_CTRL_T sync_ctrl:- enable/disable param

*  @return       ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_sync_ctrl_cfg
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
ISP_RETURN isp_common_sync_ctrl_cfg(ISP_SYNC_CTRL_T sync_ctrl)
{

    ISP_SET32(isp_regs->ISP5_CTRL, sync_ctrl, CSL_ISP5_CTRL_SYNC_EN_SHIFT,
              CSL_ISP5_CTRL_SYNC_EN_MASK);
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:-configures the pixel clock sync
*
*
*  @param   psync_clk_sel, ISP_PSYNC_CLK_SEL_T psync_clk_sel:-MMR/DMA clk

*  @return        ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_pixclk_sync_enable
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
ISP_RETURN isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_SEL_T psync_clk_sel)
{

    ISP_SET32(isp_regs->ISP5_CTRL, psync_clk_sel,
              CSL_ISP5_CTRL_PSYNC_CLK_SEL_SHIFT,
              CSL_ISP5_CTRL_PSYNC_CLK_SEL_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- controls the write operation of isp.
*
*
*  @param   wrt_ctrl,  ISP_POSTED_WRT_CTRL_T    wrt_ctrl:- posted / nonposted write param

*  @return      ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_posted_write_cfg
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
ISP_RETURN isp_common_posted_write_cfg(ISP_POSTED_WRT_CTRL_T wrt_ctrl)
{

    ISP_SET32(isp_regs->ISP5_CTRL, wrt_ctrl, CSL_ISP5_CTRL_OCP_WRNP_SHIFT,
              CSL_ISP5_CTRL_OCP_WRNP_MASK);
    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:-controls the ip from Video port or form pattern genrator
*
*
*  @param   ip_mux_sel, ISP_IP_MUXSEL_T  ip_mux_sel:- enumeration of the 2 possibilities

*  @return      ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_ip_mux_sel
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
ISP_RETURN isp_common_ip_mux_sel(ISP_IP_MUXSEL_T ip_mux_sel)
{
    ISP_SET32(isp_regs->ISP5_PG, ip_mux_sel, CSL_ISP5_PG_SRC_SEL_SHIFT,
              CSL_ISP5_PG_SRC_SEL_MASK);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:- Configures the priority of the tables access by different sub modules within isp
*
*
*  @param   table_access_ctrl,  ISP_TABLE_ACCESS_CTRL_T  table_access_ctrl:- the control parameter

*  @return      ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_table_access_cfg
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
ISP_RETURN isp_common_table_access_cfg(ISP_TABLE_ACCESS_CTRL_T
                                       table_access_ctrl)
{

    ISP_SET32(isp_regs->ISP5_MPSR, table_access_ctrl,
              CSL_ISP5_MPSR_TABLE_CFG_SHIFT, CSL_ISP5_MPSR_TABLE_CFG_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:-configures the VBUSM id's
*
*
*  @param   max_vbusm_cids, uint8 max_vbusm_cids :- specifies the id's

*  @return       ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_vbusm_id_cfg
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
ISP_RETURN isp_common_vbusm_id_cfg(uint8 max_vbusm_cids)
{
    ISP_SET32(isp_regs->ISP5_CTRL, max_vbusm_cids,
              CSL_ISP5_CTRL_VBUSM_CIDS_SHIFT, CSL_ISP5_CTRL_VBUSM_CIDS_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- configure vbusm priorities
*
*
*  @param   vbusm_priority, uint8 vbusm_priority:- specifies the priority

*  @return       ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_vbusm_priority_cfg
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
ISP_RETURN isp_common_vbusm_priority_cfg(uint8 vbusm_priority)
{

    ISP_SET32(isp_regs->ISP5_CTRL, vbusm_priority,
              CSL_ISP5_CTRL_VBUSM_CPRIORITY_SHIFT,
              CSL_ISP5_CTRL_VBUSM_CPRIORITY_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- configures the memmory acces ctrl by different sub modules of isp
*
*
*  @param:- mem_accsess_ctrl, isp_memory_access_ctrl_t mem_accsess_ctrl:- cfg struct    for memory access ctrl

*  @return       ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_memory_access_ctrl_cfg
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
ISP_RETURN isp_common_memory_access_ctrl_cfg(isp_memory_access_ctrl_t
                                             mem_accsess_ctrl)
{
    ISP_SET32(isp_regs->ISP5_BL_MTC_1, mem_accsess_ctrl.isif_read_interval,
              CSL_ISP5_BL_MTC1_ISIF_R_SHIFT, CSL_ISP5_BL_MTC1_ISIF_R_MASK);
    ISP_SET32(isp_regs->ISP5_BL_MTC_1, mem_accsess_ctrl.ipipeif_read_interval,
              CSL_ISP5_BL_MTC1_IPIPEIF_R_SHIFT,
              CSL_ISP5_BL_MTC1_IPIPEIF_R_MASK);
    ISP_SET32(isp_regs->ISP5_BL_MTC_2, mem_accsess_ctrl.h3a_write_interval,
              CSL_ISP5_BL_MTC2_H3A_W_SHIFT, CSL_ISP5_BL_MTC2_H3A_W_MASK);

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- this routine does the cfg of the read intervals, posted write,i/p port, clk polarities etc of  isp
*
*
*  @param   :-cfg,       isp_common_cfg_t*  cfg:- pointer to the cfg structure

*  @return     ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_cfg
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
ISP_RETURN isp_common_cfg(isp_common_cfg_t * cfg)
{

    ISP_RETURN retval = ISP_SUCCESS;

    retval = isp_common_ip_port_cfg(cfg->ip_port);
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_ip_mux_sel(cfg->ip_mux_sel);
    }
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_vbusm_id_cfg(cfg->max_vbusm_cids);
    }

    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_memory_access_ctrl_cfg(cfg->mem_accsess_ctrl);
    }
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_pixclk_polarity_cfg(cfg->pixclk_pol);
    }
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_posted_write_cfg(cfg->posted_wrt_ctrl);
    }
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_sync_ctrl_cfg(cfg->sync_ctrl);
    }
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_pixclk_sync_enable(cfg->psync_clk_sel);
    }
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_table_access_cfg(cfg->table_access_ctrl);
    }
    if (retval == ISP_SUCCESS)
    {
        retval = isp_common_vbusm_priority_cfg(cfg->vbusm_priority);
    }

    return retval;

}

/* ================================================================ */
/**
*  Description:- This is the SWI that needs to exist for handling isp interrupts,
*  NOTE!!!!!!!!this is incomplete
*
*  @param   none

*  @return     ISP_RETURN
*/
/*================================================================== */

// RAJAT:TODO
extern Swi_Handle isp_swi_handle;

// Swi_Handle isp_swi_handle;
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
static ISP_RETURN create_swi()
{

    Swi_Params swiParams;

    // Task_Params taskParams;
    // Clock_Params clkParams;

    Swi_Params_init(&swiParams);
    swiParams.arg0 = 1;
    swiParams.arg1 = 0;
    swiParams.priority = 2;
    swiParams.trigger = 0;

    isp_common_devp->swi_handle = isp_swi_handle;          // Swi_create(isp_common_swi_interrupt_handler,
                                                           // &swiParams,
                                                           // NULL);

    if (isp_common_devp->swi_handle == NULL)
        return ISP_FAILURE;

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- This routine is called when a swi callback has been registered for a particular event that has occured.
*
*
*  @param   arg0,
*  @paramand arg1 will be the user arguments while registering the SWI with BIOS
*
*  @return        ISP_RETURN
*/
/*================================================================== */
uint32 isp_error_count = 0;

/* ===================================================================
 *  @func     isp_swi_interrupt_handler
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
void isp_swi_interrupt_handler(UArg arg0, UArg arg1)
{

    uint32 i = 0;

    volatile uint32 sts_reg = 0;

    Interrupt_Handle_T *tmp = NULL;

    isp_interrupt_id_t id;

    sts_reg = isp_regs->ISP5_IRQSTATUS_0;

    for (i = 0; i < ISP_INT_MAX; i++)
    {
        id = isp_callback_priority[i];
        if (id >= ISP_INT_MAX)
        {
            while (1) ;                                    // needs to be
                                                           // changed to
                                                           // asserts

        }
        if (((sts_reg) & (0x00000001 << id)) != 0)
        {

            tmp = isp_swi_cb_list_head[id];

            if (tmp != NULL)
            {
                while (tmp != NULL)
                {
                    if (tmp->callback != NULL)
                    {
                        tmp->callback(ISP_SUCCESS, tmp->arg1, tmp->arg2);
                    }
                    else
                    {
                        isp_error_count++;                 /* ERROR , because
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
                isp_error_count++;
                /* ERROR , because interrupt ocurred and no client has
                 * registered */
            }
            /* Clear the particular status bit */
            isp_regs->ISP5_IRQSTATUS_0 = 0x00000001 << id;
        }
    }

    /* Enable ISS level interrupt here */
    iss_enable_interrupt(ISS_INTERRUPT_ISP_IRQ0);

}

/* ================================================================ */
/**
*  Description:-This is the HWI that needs to exist for handling isp interrupts,
*                     it posts a swi if there is a SWI callback registered for that particular interrupt
*
*
*  @param  arg, UArg arg

*  @return      ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_hwi_interrupt_handler
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
Void isp_hwi_interrupt_handler(ISS_RETURN status, uint32 arg1, void *arg2)
{

    uint32 i = 0;

    volatile uint32 sts_reg = 0;

    uint8 flag = 0;

    Interrupt_Handle_T *tmp = NULL;

    isp_interrupt_id_t id;

    /* Disable interrupt at M3 level and call SWI, in SWI acknowledge the
     * Interrupt sts register */
    /* Activate swi here only if there is a interrupt that requires a
     * callback in the SWI context */
    sts_reg = isp_regs->ISP5_IRQSTATUS_0;
    /* cheack all the callbacks that need to be called from HWI context */
    for (i = 0; i < ISP_INT_MAX; i++)
    {
        tmp = NULL;
        id = isp_callback_priority[i];
        if (id >= ISP_INT_MAX)
        {
            while (1) ;                                    // needs to be
                                                           // changed to
                                                           // asserts
        }
        if (((sts_reg) & (0x00000001 << id)) != 0)
        {
            tmp = isp_hwi_cb_list_head[id];
            if (tmp != NULL)
            {

                while (tmp != NULL)
                {
                    if (tmp->callback != NULL)
                    {
                        tmp->callback(ISP_SUCCESS, tmp->arg1, tmp->arg2);
                    }
                    else
                    {
                        isp_error_count++;                 /* ERROR , because
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
                isp_error_count++;
                /* ERROR , because interrupt ocurred and no client has
                 * registered */
            }

            if (isp_swi_cb_list_head[id] == NULL)
            {
                /* clear the status bit only if there is no corresponding HWI
                 * handler. */
                /* the register has one bit for status of HS_VS_IRQ which is
                 * WR and 15 other status bits are RO , instead of handleing
                 * the special case with a "if" statement we just write to
                 * the read only bits */
                isp_regs->ISP5_IRQSTATUS_0 = 0x00000001 << id;
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

    /* for (i=0;i<ISP_INT_MAX;i++) { if(((sts_reg)&(0x00000001<<i))!=0){
     *
     * if(isp_swi_cb_list_head[i]!=NULL) {
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
        iss_disable_interrupt(ISS_INTERRUPT_ISP_IRQ0);
        Swi_post(isp_swi_handle);
    }
    return;
}

/* ================================================================ */
/**
*  Description:-this routine enables the copying of red gamma table to blue and green tables while writing from memory
*
*
*  @param   :- ISP_COPY_RED_GAMMA_TBL_T:- enum specifying the enable/disable of the feature

*  @return        ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_enable_red_gamma_table_cpy
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
ISP_RETURN isp_enable_red_gamma_table_cpy(ISP_COPY_RED_GAMMA_TBL_T enable)
{

    ISP_SET32(isp_regs->ISP5_MPSR, enable,
              CSL_ISP5_MPSR_IPIPE_GAMMA_RGB_COPY_SHIFT,
              CSL_ISP5_MPSR_IPIPE_GAMMA_RGB_COPY_MASK);
    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- this routine enables the appropritae DMA ctrl for copying tables and reading histogram data
*  NOTE !!!!!!!!!!!!the implemmentation is not complete
*
*  @param   dma_id, ISP_DMA_ID_T dma_id
*  @param   enable,     ISP_DMA_CTRL_T enable
*
*  @return        ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_common_enable_dma
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
ISP_RETURN isp_common_enable_dma(ISP_DMA_ID_T dma_id, ISP_DMA_CTRL_T enable)
{
    if (enable == ISP_DMA_ENABLE)
        ISP_SET32(isp_regs->ISP5_DMAENABLE_SET, 1, dma_id,
                  (0x00000001 << dma_id));
    else if (enable == ISP_DMA_DISABLE)
        ISP_SET32(isp_regs->ISP5_DMAENABLE_CLR, 1, dma_id,
                  (0x00000001 << dma_id));

    /* #######TODO Need to Configure ISP5_CTRL.DMA_CFG */

    else
        return ISP_FAILURE;

    return ISP_SUCCESS;
}

/* ================================================================ */
/**
*  Description:-this routine needs to be called before calling any isp submodule's inits, like ipipe_init() etc
*               this routine does the basic intialisation of isp, it disables all interrupts crears;swi's and hwi's,
*                     does a clean reset of ISP etc
*!NOTE this is not complete
*  @param   none
*
*  @return        ISP_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     isp_common_init
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
ISP_RETURN isp_common_init()
{

    uint8 i = 0, j = 0;

    ISP_RETURN retval = ISP_SUCCESS;

    ISS_RETURN retval_iss;

    /* Allocate memory for the isp struct "isp_common_dev" and fill it with
     * swi_handle,hwi_hande etc.... */
    isp_common_devp = &isp_common_dev;

    /* Set ISP base address here */
    isp_regs = ((isp_regs_ovly) (ISP5_BASE_ADDRESS));

    // ~~~~~~~~~

    /* ES2.0 specific : Input from Mirval, Murielle
     * --------------------------------------------- Before issuing Soft
     * reset Set ISS_CLKCTRL.ISP = 1 (Enable no gating of clock and allow
     * register writes) Set ISP5_SYSCONFIG.STANDBYMODE = 2 (smart standby)
     * Set ISP5_CTRL.MSTANDBY to 1 Poll for ISP5_CTRL.MSTANDBY_WAIT = 1. */
    ISP_SET32(isp_regs->ISP5_SYSCONFIG, 2, CSL_ISP5_SYSCONFIG_STANDBYMODE_SHIFT, CSL_ISP5_SYSCONFIG_STANDBYMODE_MASK);  // for
                                                                                                                        // ES2.0
                                                                                                                        //
    ISP_SET32(isp_regs->ISP5_CTRL, 1, CSL_ISP5_CTRL_MSTANDBY_SHIFT, CSL_ISP5_CTRL_MSTANDBY_MASK);   // for
                                                                                                    // ES2.0
                                                                                                    //
    while (ISP_FEXT
           (isp_regs->ISP5_CTRL, CSL_ISP5_CTRL_MSTANDBY_WAIT_SHIFT,
            CSL_ISP5_CTRL_MSTANDBY_WAIT_MASK) != 1)
    {
    }                                                      // for ES2.0

    /* FIXME: Ashara, Amit (hw design team )suggested to set it to
     * "No-Standby mode" for now - since there is something wrong happening
     * at PRCM side, when its in smart standby - at the time of soft reset */
    // ISP_SET32(isp_regs->ISP5_SYSCONFIG, 1,
    // CSL_ISP5_SYSCONFIG_STANDBYMODE_SHIFT,
    // CSL_ISP5_SYSCONFIG_STANDBYMODE_MASK);//for ES2.0

    /* ISP soft reset */
    ISP_SET32(isp_regs->ISP5_SYSCONFIG, 1, CSL_ISP5_SYSCONFIG_RESET_SHIFT,
              CSL_ISP5_SYSCONFIG_RESET_MASK);
    /* Wait for some cycles before accessing ISP register to check for
     * RESET_DONE */
    for (i = 0; i < 32; i++) ;
    while (ISP_FEXT
           (isp_regs->ISP5_SYSCONFIG, CSL_ISP5_SYSCONFIG_RESET_SHIFT,
            CSL_ISP5_SYSCONFIG_RESET_MASK))
    {
    }

    /* #TODOApply reset thru sysconfig register */

    for (i = 0; ((i < MAX_ISP_INTERRUPTS) && (retval == ISP_SUCCESS)); i++)
    {
        /* Clear all interrupts and set all handles to NULL */
        isp_irq_data[i].callback_func = NULL;
        isp_irq_data[i].arg1 = 0;
        isp_irq_data[i].arg2 = NULL;
        isp_irq_data[i].callback_context = ISP_CALLBACK_CONTEXT_MAX;

        /* Clear all interrupts which are already existing */

        /* Disable all interrupts for all banks */
        for (j = 0; j < (ISP_INTERRUPT_BANK3 + 1); j++)
            retval =
                isp_common_disable_interrupt((isp_interrupt_id_t) i,
                                             (isp_interrupt_bank_t) j);

        isp_hwi_cb_list_head[i] = NULL;
        isp_swi_cb_list_head[i] = NULL;
    }

    /* Set the default bank -id for interrupt management */
    isp_common_dev.default_int_bank = ISP_DEFAULT_INTERRUPT_BANK;

    for (i = 0; ((i < ISP_MAX_MODULE_CLOCKS) && (retval == ISP_SUCCESS)); i++)
    {
        /* Disable all clocks */
        retval = isp_common_disable_clk((ISP_MODULE_CLK_T) i);
    }

    /* #TODO in SYSCONFIG regEnable Autogating */

    /* Create SWI and associated structures. */
    if (create_swi() == ISP_FAILURE)
    {
        return ISP_FAILURE;
    }

    /* register the interrupt handler */
    iss_module_clk_ctrl(ISS_ISP_CLK, ISS_CLK_ENABLE);
    retval_iss =
        iss_register_interrupt_handler(ISS_INTERRUPT_ISP_IRQ0,
                                       isp_hwi_interrupt_handler,
                                       ISP_INTERRUPT_BANK0, NULL);
    /*
     * if(retval_iss==ISS_SUCCESS) retval_iss=iss_register_interrupt_handler(
     * ISS_INTERRUPT_ISP_IRQ1, isp_Hwi_interrupt_handler,
     * ISP_INTERRUPT_BANK1, NULL); if(retval_iss==ISP_SUCCESS)
     * retval_iss=iss_register_interrupt_handler( ISS_INTERRUPT_ISP_IRQ2,
     * isp_Hwi_interrupt_handler, ISP_INTERRUPT_BANK2, NULL);
     * if(retval_iss==ISP_SUCCESS) retval_iss=iss_register_interrupt_handler(
     * ISS_INTERRUPT_ISP_IRQ3, isp_Hwi_interrupt_handler,
     * ISP_INTERRUPT_BANK3, NULL); */

    if (retval_iss == ISP_SUCCESS)
        iss_enable_interrupt(ISS_INTERRUPT_ISP_IRQ0);

    return ISP_SUCCESS;
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
ISP_RETURN isp_common_exit()
{

    uint8 i = 0, j = 0;

    ISP_RETURN retval = ISP_SUCCESS;

    ISS_RETURN retval_iss;

    for (i = 0; ((i < MAX_ISP_INTERRUPTS) && (retval == ISP_SUCCESS)); i++)
    {
        /* Clear all interrupts and set all handles to NULL */
        isp_irq_data[i].callback_func = NULL;
        isp_irq_data[i].arg1 = 0;
        isp_irq_data[i].arg2 = NULL;
        isp_irq_data[i].callback_context = ISP_CALLBACK_CONTEXT_MAX;

        /* Clear all interrupts which are already existing */

        /* Disable all interrupts for all banks */
        for (j = 0; j < (ISP_INTERRUPT_BANK3 + 1); j++)
            retval =
                isp_common_disable_interrupt((isp_interrupt_id_t) i,
                                             (isp_interrupt_bank_t) j);

        isp_hwi_cb_list_head[i] = NULL;
        isp_swi_cb_list_head[i] = NULL;
    }

    for (i = 0; ((i < ISP_MAX_MODULE_CLOCKS) && (retval == ISP_SUCCESS)); i++)
    {
        /* Disable all clocks */
        retval = isp_common_disable_clk((ISP_MODULE_CLK_T) i);

    }
    retval_iss = iss_module_clk_ctrl(ISS_ISP_CLK, ISS_CLK_DISABLE);
    if (retval_iss == ISP_SUCCESS)
        iss_disable_interrupt(ISS_INTERRUPT_ISP_IRQ0);
    if (retval_iss == ISP_SUCCESS)
        iss_unregister_interrupt_handler(ISS_INTERRUPT_ISP_IRQ0);

    return ISP_SUCCESS;

}

/* ================================================================ */
/**
*  Description:- Thie routine returns the sts bit specified.
*
*  @param   int_id ,    isp_interrupt_id_t int_id:- specifies the interrupt number
*  @param   sts_p,  uint32* sts_p:- pointer to hold the sts value .
*  @return   ISP_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     isp_common_get_int_sts
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
ISP_RETURN isp_common_get_int_sts(isp_interrupt_id_t int_id, uint32 * sts_p)
{

    uint32 val = 0;

    val = isp_regs->ISP5_IRQSTATUS_RAW_0;

    *sts_p = (val >> int_id) & 0x00000001;
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     isp_common_get_all_int_sts
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
uint32 isp_common_get_all_int_sts(void)
{
    return isp_regs->ISP5_IRQSTATUS_RAW_0;
}

/* ===================================================================
 *  @func     isp_common_assert_mstandby
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
ISP_RETURN isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_T assert_val)
{
    ISP_SET32(isp_regs->ISP5_CTRL, assert_val, CSL_ISP5_CTRL_MSTANDBY_SHIFT,
              CSL_ISP5_CTRL_MSTANDBY_MASK);
    return ISP_SUCCESS;
}

/* ===================================================================
 *  @func     isp_common_cfg_VD_PULSE_EXT
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
ISP_RETURN isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_T val)
{
    ISP_SET32(isp_regs->ISP5_CTRL, val, CSL_ISP5_CTRL_VD_PULSE_EXT_SHIFT,
              CSL_ISP5_CTRL_VD_PULSE_EXT_MASK);
    return ISP_SUCCESS;
}
