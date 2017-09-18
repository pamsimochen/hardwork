/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csi2_irq.c
*
* This file contains interrupt handling data-structures and functions of the CSI2 module of Ducati subsystem  in OMAP4/Monica
*
* @path drv_csi2/src
*
* @rev 1.0
*
* @developer:
*/
/*========================================================================*/

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <ti/psp/iss/hal/iss/csi2/csi2.h>
#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

extern csi2_regs_ovly csi2A_regs;

csi2_dev_data csi2_common_dev;

CSI2_GLOBAL_INTERRUPT_ID_T csi2_context_irq_tbl[] = {
    CSI2_CONTEXT0_IRQ,
    CSI2_CONTEXT1_IRQ,
    CSI2_CONTEXT2_IRQ,
    CSI2_CONTEXT3_IRQ,
    CSI2_CONTEXT4_IRQ,
    CSI2_CONTEXT5_IRQ,
    CSI2_CONTEXT6_IRQ,
    CSI2_CONTEXT7_IRQ
};

extern Swi_Handle csi2_context_irq_swi_handle;

/* ===================================================================
 *  @func     csi2_complex_io_hwi_interrupt_handler                                               
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
void csi2_complex_io_hwi_interrupt_handler(CSI2_RETURN status,
                                           uint32 device_num, void *arg2);
/* ===================================================================
 *  @func     csi2_context_irq_hwi_interrupt_handler                                               
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
void csi2_context_irq_hwi_interrupt_handler(CSI2_RETURN status,
                                            uint32 device_num, void *arg2);

Interrupt_Handle_T
    *csi2_complex_io_hwi_cb_list_head[CSI2_DEVICE_MAX][CSI2_IO_MAX]
    [CSI2_COMPLEX_IO_IRQ_MAX];
Interrupt_Handle_T
    *csi2_complex_io_swi_cb_list_head[CSI2_DEVICE_MAX][CSI2_IO_MAX]
    [CSI2_COMPLEX_IO_IRQ_MAX];

char *unexpected_isr_list[] = {
    "CONTEXT0",
    "CONTEXT1",
    "CONTEXT2",
    "CONTEXT3",
    "CONTEXT4",
    "CONTEXT5",
    "CONTEXT6",
    "CONTEXT7",
    "FIFO OVF IRQ",
    "COMPLEXIO1_ERR_IRQ",
    "COMPLEXIO2_ERR_IRQ",
    "ECC_NO_CORRECTION_IRQ",
    "ECC_CORRECTION_IRQ",
    "SHORT_PACKET IRQ",
    "OCP_ERR_IRQ"
};

/* ================================================================ */
/**
*   csi2_complex_io_hook_int_handler()
*  Description:-csi2_complex_io_hook_int_handler, this routine registers a handler for the interrupt specified by int_id, for the  
*                     complex specified by complex_io_num, and the devicenum is the particular instance of the csi2 device.
*  
*
*  @param CSI2_DEVICE_T devicenum,  is the csi2 peripheral instance
*  @param CSI2_COMPLEXIO_NUMBER_T complex_io_num, is the complex io inside each csi2 peripheral instance.
*  @param csi2_complexio_interrup_id_t int_id, is the interrupt id witin the complex io module
*  @param Interrupt_Handle_T * handle, handle is the user parameter which has the callback and other data.
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_complex_io_hook_int_handler                                               
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
CSI2_RETURN csi2_complex_io_hook_int_handler(CSI2_DEVICE_T devicenum,
                                             CSI2_COMPLEXIO_NUMBER_T
                                             complex_io_num,
                                             csi2_complexio_interrup_id_t
                                             int_id,
                                             Interrupt_Handle_T * handle)
{
    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL,
        *head_ptr = (Interrupt_Handle_T *) NULL;

    if (devicenum >= CSI2_DEVICE_MAX)
        return CSI2_FAILURE;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSI2_FAILURE;

    if (handle->callback == NULL)
        /* return error since both callbacks shouldnt be zero */
        return CSI2_FAILURE;

    handle->list_next = NULL;

    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        head_ptr =
            csi2_complex_io_hwi_cb_list_head[devicenum][complex_io_num][int_id];
    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        head_ptr =
            csi2_complex_io_swi_cb_list_head[devicenum][complex_io_num][int_id];
    /* Go to end of list and link the handle at the end */
    ptr_curr = head_ptr;

    /* Need to protect the following piece of code from pre-emption. */
    /* Check if node need to goto the head of the list */
    if ((head_ptr == NULL) || (handle->priority < head_ptr->priority))
    {

        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)

        {
            handle->list_next =
                csi2_complex_io_hwi_cb_list_head[devicenum][complex_io_num]
                [int_id];
            csi2_complex_io_hwi_cb_list_head[devicenum][complex_io_num][int_id]
                = handle;
        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            handle->list_next =
                csi2_complex_io_swi_cb_list_head[devicenum][complex_io_num]
                [int_id];
            csi2_complex_io_swi_cb_list_head[devicenum][complex_io_num][int_id]
                = handle;
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

    return CSI2_SUCCESS;
}

Interrupt_Handle_T
    *csi2_context_irq_hwi_cb_list_head[CSI2_DEVICE_MAX][CSI2_CONTEXT_MAX]
    [CSI2_CONTEXT_IRQ_MAX];
Interrupt_Handle_T
    *csi2_context_irq_swi_cb_list_head[CSI2_DEVICE_MAX][CSI2_CONTEXT_MAX]
    [CSI2_CONTEXT_IRQ_MAX];

/* One for eacj device */
Uint8 csi2_context_flag[CSI2_DEVICE_MAX] = { 0, 0 };

/* ================================================================ */
/**
*   csi2_context_irq_hook_int_handler()
*  Description:-csi2_context_irq_hook_int_handler, this routine registers a handler for the interrupt specified by int_id, for the  
*                     context specified by context_num, and the devicenum is the particular instance of the csi2 device.
*  
*
*  @param CSI2_DEVICE_T devicenum,  is the csi2 peripheral instance
*  @param CSI2_CONTEXT_T context_num, is the context inside each csi2 peripheral instance.
*  @param csi2_complexio_interrup_id_t int_id, is the interrupt id witin the complex io module
*  @param Interrupt_Handle_T * handle, handle is the user parameter which has the callback and other data.
*  @return    CSI2_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     csi2_context_irq_hook_int_handler                                               
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
CSI2_RETURN csi2_context_irq_hook_int_handler(CSI2_DEVICE_T devicenum,
                                              CSI2_CONTEXT_T context_num,
                                              CSI2_CTX_INTERRUPT_ID_T int_id,
                                              Interrupt_Handle_T * handle)
{
    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL,
        *head_ptr = (Interrupt_Handle_T *) NULL;

    if (context_num >= CSI2_CONTEXT_MAX)
        return CSI2_FAILURE;

    if (devicenum >= CSI2_DEVICE_MAX)
        return CSI2_FAILURE;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSI2_FAILURE;

    if (handle->callback == NULL)
        /* return error since both callbacks shouldnt be zero */
        return CSI2_FAILURE;

    handle->list_next = NULL;

    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        head_ptr =
            csi2_context_irq_hwi_cb_list_head[devicenum][context_num][int_id];
    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        head_ptr =
            csi2_context_irq_swi_cb_list_head[devicenum][context_num][int_id];
    /* Go to end of list and link the handle at the end */
    ptr_curr = head_ptr;

    /* Need to protect the following piece of code from pre-emption. */
    /* Check if node need to goto the head of the list */
    if ((head_ptr == NULL) || (handle->priority < head_ptr->priority))
    {

        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        {
            handle->list_next =
                csi2_context_irq_hwi_cb_list_head[devicenum][context_num]
                [int_id];
            csi2_context_irq_hwi_cb_list_head[devicenum][context_num][int_id] =
                handle;
        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            handle->list_next =
                csi2_context_irq_swi_cb_list_head[devicenum][context_num]
                [int_id];
            csi2_context_irq_swi_cb_list_head[devicenum][context_num][int_id] =
                handle;
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

    return CSI2_SUCCESS;
}

csi2_callback_info_t csi2_cbk_info[CSI2_DEVICE_MAX][CSI2_GLOBAL_INTERRUPT_MAX];

/* ================================================================ */
/**
*   
*  Description:-This routine will register a interrupt handler at csi2 interrupt management level.
*  
*
*  @param CSI2_DEVICE_T dev_num,
*  @param CSI2_GLOBAL_INTERRUPT_ID_T irq_id,
*
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_register_interrupt_handler                                               
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
CSI2_RETURN csi2_register_interrupt_handler(CSI2_DEVICE_T dev_num,
                                            CSI2_GLOBAL_INTERRUPT_ID_T irq_id,
                                            csi2_irq_callback_t callback_func,
                                            uint32 arg1, void *arg2)
{

    /* Check if already interrupt handler is registered */
    if (csi2_cbk_info[dev_num][irq_id].cbk_func != NULL)
    {
        return CSI2_FAILURE;
    }

    csi2_cbk_info[dev_num][irq_id].cbk_func = callback_func;
    csi2_cbk_info[dev_num][irq_id].arg1 = arg1;
    csi2_cbk_info[dev_num][irq_id].arg2 = arg2;

    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
*   csi2_enable_interrupt()
*  Description:-this routine will enable a interrupt at the csi2 level, the interrupt is specified ny int_id.
*  
*
*  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
*  @param CSI2_GLOBAL_INTERRUPT_ID_T int_id, is the interrupt which needs to be enabled.
*
*/
/*================================================================== */

/* ===================================================================
 *  @func     csi2_enable_interrupt                                               
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
CSI2_RETURN csi2_enable_interrupt(CSI2_DEVICE_T dev_num,
                                  CSI2_GLOBAL_INTERRUPT_ID_T int_id)
{

    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    csi2_regs->CSI2_IRQENABLE |= (0x00000001 << int_id);

    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
*   csi2_disable_interrupt()
*  Description:-this routine will disable a interrupt at the csi2 level, the interrupt is specified ny int_id.
*  
*
*  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
*  @param CSI2_GLOBAL_INTERRUPT_ID_T int_id, is the interrupt which needs to be enabled.
*
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_disable_interrupt                                               
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
CSI2_RETURN csi2_disable_interrupt(CSI2_DEVICE_T dev_num,
                                   CSI2_GLOBAL_INTERRUPT_ID_T int_id)
{
    csi2_regs_ovly csi2_regs = NULL;

    csi2_regs = (csi2A_regs);

    csi2_regs->CSI2_IRQENABLE &= ~(0x00000001 << int_id);

    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
*  csi2_complex_io_unhook_int_handler()
*  Description:-csi2_context_irq_hook_int_handler, this routine un-registers a handler for the interrupt specified by int_id, for the  
*                     context specified by context_num, and the devicenum is the particular instance of the csi2 device.
*  
*
*  @param CSI2_DEVICE_T devicenum,  is the csi2 peripheral instance
*  @param CSI2_COMPLEXIO_NUMBER_T complex_io_num, is the complex io inside each csi2 peripheral instance.
*  @param csi2_complexio_interrup_id_t int_id, is the interrupt id within the complex io module
*  @param Interrupt_Handle_T * handle, handle is the user parameter which has the callback and other data.
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_complex_io_unhook_int_handler                                               
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
CSI2_RETURN csi2_complex_io_unhook_int_handler(CSI2_DEVICE_T devicenum,
                                               CSI2_COMPLEXIO_NUMBER_T
                                               complex_io_num,
                                               csi2_complexio_interrup_id_t
                                               int_id,
                                               Interrupt_Handle_T * handle)
{

    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL, *head_ptr =
        (Interrupt_Handle_T *) NULL;
    uint8 flag = 0;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSI2_FAILURE;

    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)

        head_ptr =
            csi2_complex_io_hwi_cb_list_head[devicenum][complex_io_num][int_id];

    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)

        head_ptr =
            csi2_complex_io_swi_cb_list_head[devicenum][complex_io_num][int_id];

    else
        return CSI2_FAILURE;

    if (head_ptr == NULL)
        return CSI2_FAILURE;

    /* 
     * If the handle is the first element update the head ptr */

    if (handle == head_ptr)
    {
        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        {
            csi2_complex_io_hwi_cb_list_head[devicenum][complex_io_num][int_id]
                = head_ptr->list_next;
        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            csi2_complex_io_swi_cb_list_head[devicenum][complex_io_num][int_id]
                = head_ptr->list_next;
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
            else
            {
                ptr_curr = ptr_curr->list_next;            // Move to next
                                                           // element
            }
        }
        if (flag == 0)
            return CSI2_FAILURE;
    }
    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
*   csi2_context_irq_unhook_int_handler()
*  Description:-csi2_context_irq_unhook_int_handler, this routine un-registers a handler for the interrupt specified by int_id, for the  
*                     context specified by context_num, and the devicenum is the particular instance of the csi2 device.
*  
*
*  @param CSI2_DEVICE_T devicenum,  is the csi2 peripheral instance
*  @param CSI2_CONTEXT_T context_num, is the context inside each csi2 peripheral instance.
*  @param csi2_complexio_interrup_id_t int_id, is the interrupt id witin the complex io module
*  @param Interrupt_Handle_T * handle, handle is the user parameter which has the callback and other data.
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_unhook_int_handler                                               
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
CSI2_RETURN csi2_context_irq_unhook_int_handler(CSI2_DEVICE_T devicenum,
                                                CSI2_CONTEXT_T context_num,
                                                CSI2_CTX_INTERRUPT_ID_T int_id,
                                                Interrupt_Handle_T * handle)
{

    Interrupt_Handle_T *ptr_curr = (Interrupt_Handle_T *) NULL, *head_ptr =
        (Interrupt_Handle_T *) NULL;
    uint8 flag = 0;

    if (handle == (Interrupt_Handle_T *) NULL)
        return CSI2_FAILURE;

    if (handle->context == ISS_CALLBACK_CONTEXT_HWI)

        head_ptr =
            csi2_context_irq_hwi_cb_list_head[devicenum][context_num][int_id];

    else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)

        head_ptr =
            csi2_context_irq_hwi_cb_list_head[devicenum][context_num][int_id];

    else
        return CSI2_FAILURE;

    if (head_ptr == NULL)
        return CSI2_FAILURE;

    /* 
     * If the handle is the first element update the head ptr */

    if (handle == head_ptr)
    {
        if (handle->context == ISS_CALLBACK_CONTEXT_HWI)
        {
            csi2_context_irq_hwi_cb_list_head[devicenum][context_num][int_id] =
                head_ptr->list_next;
        }
        else if (handle->context == ISS_CALLBACK_CONTEXT_SWI)
        {
            csi2_context_irq_hwi_cb_list_head[devicenum][context_num][int_id] =
                head_ptr->list_next;
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
            else
            {
                ptr_curr = ptr_curr->list_next;            // Move to next
                                                           // element
            }
        }
        if (flag == 0)
            return CSI2_FAILURE;

    }

    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
*  csi2_complexi_io_enable_interrupt()
*  Description:-This routine enables the complex io interrupt int_id, in the csi2 peripherral instance specified by dev_num.
*  
*
*  @param CSI2_DEVICE_T dev_num,  is the csi2 peripheral instance
*  @param CSI2_COMPLEXIO_NUMBER_T complex_io_num, is the complex io inside each csi2 peripheral instance.
*  @param CSI2_CTX_INTERRUPT_ID_T int_id, is the complex_io interrupt.
*
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_complexi_io_enable_interrupt                                               
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
CSI2_RETURN csi2_complexi_io_enable_interrupt(CSI2_DEVICE_T dev_num,
                                              CSI2_COMPLEXIO_NUMBER_T io_num,
                                              CSI2_CTX_INTERRUPT_ID_T int_id)
{
    uint32 *reg_add;

    csi2_regs_ovly csi2_regs;

    csi2_regs = (csi2A_regs);

    reg_add =
        (uint32 *) ((CSI2_IO_1 == io_num)
                    ? (&(csi2_regs->CSI2_COMPLEXIO1_IRQENABLE))
                    : (&(csi2_regs->CSI2_COMPLEXIO2_IRQENABLE)));

    *reg_add |= (0x00000001 << int_id);

    if (CSI2_IO_1 == io_num)
    {
        csi2_enable_interrupt(dev_num, CSI2_COMPLEXIO1_ERR_IRQ);
    }
    else
    {
        csi2_enable_interrupt(dev_num, CSI2_COMPLEXIO2_ERR_IRQ);
    }

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
*   csi2_complexi_io_disable_interrupt()
*   Description:-This routine disables the complex io interrupt int_id, in the csi2 peripherral instance specified by dev_num.
*  
*
*  @param  CSI2_DEVICE_T dev_num,  is the csi2 peripheral instance
*  @param  CSI2_COMPLEXIO_NUMBER_T complex_io_num, is the complex io inside each csi2 peripheral instance.
*  @param  CSI2_CTX_INTERRUPT_ID_T int_id, is the complex_io interrupt.
*
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_complexi_io_disable_interrupt                                               
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
CSI2_RETURN csi2_complexi_io_disable_interrupt(CSI2_DEVICE_T dev_num,
                                               CSI2_COMPLEXIO_NUMBER_T io_num,
                                               CSI2_CTX_INTERRUPT_ID_T int_id)
{
    uint32 *reg_add;

    csi2_regs_ovly csi2_regs;

    csi2_regs = (csi2A_regs);

    reg_add =
        (uint32 *) ((CSI2_IO_1 == io_num)
                    ? (&(csi2_regs->CSI2_COMPLEXIO1_IRQENABLE))
                    : (&(csi2_regs->CSI2_COMPLEXIO2_IRQENABLE)));

    *reg_add &= ~(0x00000001 << int_id);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
*   csi2_context_irq_enable_interrupt()
*  Description:- This routine will enable the context interrupt specified by int_id, in the csi2 peripheral instance specified by dev_num
*                      and the context specified in context_num.
*  
*
*  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
*  @param CSI2_CONTEXT_T context_num, is the complex io inside each csi2 peripheral instance.
*  @param  CSI2_CTX_INTERRUPT_ID_T int_id, is the context interrupt.
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_enable_interrupt                                               
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
CSI2_RETURN csi2_context_irq_enable_interrupt(CSI2_DEVICE_T dev_num,
                                              CSI2_CONTEXT_T context_num,
                                              CSI2_CTX_INTERRUPT_ID_T int_id)
{
    uint32 *reg_add;

    uint32 base_add;

    uint32 *sts_reg_add;

    base_add =
        (uint32) ((csi2A_regs));

    sts_reg_add = (uint32 *) CSI2_CTX_IRQSTATUS(base_add, context_num);

    reg_add = (uint32 *) CSI2_CTX_IRQENABLE(base_add, context_num);

    *sts_reg_add |= (0x00000001 << int_id);                // clear Intr
    *reg_add |= (0x00000001 << int_id);                    // enable Intr

    csi2_enable_interrupt(dev_num, csi2_context_irq_tbl[context_num]);
    return CSI2_SUCCESS;

}

/* ================================================================ */
/**
*   csi2_context_irq_disable_interrupt()
*  Description:- This routine will disable the context interrupt specified by int_id, in the csi2 peripheral instance specified by dev_num
*                      and the context specified in context_num.
*  
*
*  @param CSI2_DEVICE_T dev_num, is the csi2 peripheral instance
*  @param CSI2_CONTEXT_T context_num, is the complex io inside each csi2 peripheral instance.
*  @param  CSI2_CTX_INTERRUPT_ID_T int_id, is the context interrupt.
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_disable_interrupt                                               
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
CSI2_RETURN csi2_context_irq_disable_interrupt(CSI2_DEVICE_T dev_num,
                                               CSI2_CONTEXT_T context_num,
                                               CSI2_CTX_INTERRUPT_ID_T int_id)
{

    uint32 *reg_add;

    uint32 base_add;

    base_add =
        (uint32) ((csi2A_regs));

    reg_add = (uint32 *) CSI2_CTX_IRQENABLE(base_add, context_num);

    *reg_add &= ~(0x00000001 << int_id);

    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
*   csi2_complex_io_irq_init()
*  Description:- this routine will initialise the data structures used by the complex io interrupt management SW
*                      and also registers the interrupt handlers for the complex io interrupts with the csi2 interrupt manager.
*  
*
*  @param none
*
*  @return    none
*/
/*================================================================== */
csi2_complex_io_irq_init()
{
    CSI2_DEVICE_T i;

    /* Clear all the interrupts */
    /* Register handler for complex io irq's and enable all the complex io
     * interrupts at the CSI2 level. */

    for (i = CSI2_DEVICE_A; i < CSI2_DEVICE_MAX; i++)
    {

        csi2_register_interrupt_handler(i, CSI2_COMPLEXIO1_ERR_IRQ,
                                        csi2_complex_io_hwi_interrupt_handler,
                                        i, (void *) CSI2_IO_1);
        // csi2A_regs are not initialized
        // csi2_enable_interrupt(i,CSI2_COMPLEXIO1_ERR_IRQ);
        csi2_register_interrupt_handler(i, CSI2_COMPLEXIO2_ERR_IRQ,
                                        csi2_complex_io_hwi_interrupt_handler,
                                        i, (void *) CSI2_IO_2);
        // csi2_enable_interrupt(i,CSI2_COMPLEXIO2_ERR_IRQ);
    }
}

/* ================================================================ */
/**
*   
*  Description:-
*  
*
*  @param 
*  @param 
*
*  @return    void
*/
/*================================================================== */
/* ARG2 is the complex IO number */
/* ===================================================================
 *  @func     csi2_unexpected_hwi_interrupt_handler                                               
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
void csi2_unexpected_hwi_interrupt_handler(CSI2_RETURN status,
                                           uint32 device_num,
                                           CSI2_GLOBAL_INTERRUPT_ID_T arg2)
{
    CSI2_GLOBAL_INTERRUPT_ID_T irq_num = (CSI2_GLOBAL_INTERRUPT_ID_T) arg2;

    csi2_disable_interrupt((CSI2_DEVICE_T) device_num, irq_num);
    // TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_DRIVERS, "\r\n!!!!!! Unexpected
    // CSI-%d interrupt No= %d: ", device_num, (int)arg2);
    // TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_DRIVERS, " %s !!!!\r\n",
    // unexpected_isr_list[irq_num]);
}

/* ================================================================ */
/**
*   csi2_other_irq_init()
*  Description:- this routine will initialise the data structures used by the interrupt management SW
*                      and also registers the interrupt handlers.
*  
*
*  @param none
*
*  @return    none
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_other_irq_init                                               
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
void csi2_other_irq_init(void)
{
    CSI2_DEVICE_T i;

    for (i = CSI2_DEVICE_A; i < CSI2_DEVICE_MAX; i++)
    {

        csi2_register_interrupt_handler(i, CSI2_OCP_ERR_IRQ,
                                        (csi2_irq_callback_t)
                                        csi2_unexpected_hwi_interrupt_handler,
                                        i, (void *) CSI2_OCP_ERR_IRQ);
        csi2_register_interrupt_handler(i, CSI2_SHORT_PACKET_IRQ,
                                        (csi2_irq_callback_t)
                                        csi2_unexpected_hwi_interrupt_handler,
                                        i, (void *) CSI2_SHORT_PACKET_IRQ);
        csi2_register_interrupt_handler(i, CSI2_ECC_CORRECTION_IRQ,
                                        (csi2_irq_callback_t)
                                        csi2_unexpected_hwi_interrupt_handler,
                                        i, (void *) CSI2_ECC_CORRECTION_IRQ);
        csi2_register_interrupt_handler(i, CSI2_ECC_NO_CORRECTION_IRQ,
                                        (csi2_irq_callback_t)
                                        csi2_unexpected_hwi_interrupt_handler,
                                        i, (void *) CSI2_ECC_NO_CORRECTION_IRQ);
        // csi2_register_interrupt_handler(i, CSI2_FIFO_OVF_IRQ,
        // csi2_unexpected_hwi_interrupt_handler,i,(void*)CSI2_FIFO_OVF_IRQ);
    }
}

/* ================================================================ */
/**
*   
*  Description:-this routine will initialise the data structures used by the context's  interrupt management SW
*                      and also registers the interrupt handlers for the context's io interrupts with the csi2 interrupt manager.
*  
*
* 
*  @param none
*
*  @return    none
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_context_irq_init                                               
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
CSI2_RETURN csi2_context_irq_init()
{
    CSI2_DEVICE_T i;

    uint8 j, k;

    /* Register handler for complex io irq's and enable all the complex io
     * interrupts at the CSI2 level. */
    for (i = CSI2_DEVICE_A; i < CSI2_DEVICE_MAX; i++)
    {
        for (j = 0; j < CSI2_CONTEXT_MAX; j++)
        {
            for (k = 0; k < CSI2_CONTEXT_IRQ_MAX; k++)
            {
                csi2_context_irq_hwi_cb_list_head[i][j][k] = NULL;
                csi2_context_irq_swi_cb_list_head[i][j][k] = NULL;
            }
        }
    }

    for (i = CSI2_DEVICE_A; i < CSI2_DEVICE_MAX; i++)
    {

        csi2_register_interrupt_handler(i, CSI2_CONTEXT0_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_0);
        // csi2A_regs are not initialized
        // csi2_enable_interrupt(i,CSI2_CONTEXT0_IRQ);

        csi2_register_interrupt_handler(i, CSI2_CONTEXT1_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_1);
        // csi2_enable_interrupt(i,CSI2_CONTEXT1_IRQ);

        csi2_register_interrupt_handler(i, CSI2_CONTEXT2_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_2);
        // csi2_enable_interrupt(i,CSI2_CONTEXT2_IRQ);

        csi2_register_interrupt_handler(i, CSI2_CONTEXT3_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_3);
        // csi2_enable_interrupt(i,CSI2_CONTEXT3_IRQ);

        csi2_register_interrupt_handler(i, CSI2_CONTEXT4_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_4);
        // csi2_enable_interrupt(i,CSI2_CONTEXT4_IRQ);

        csi2_register_interrupt_handler(i, CSI2_CONTEXT5_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_5);
        // csi2_enable_interrupt(i,CSI2_CONTEXT5_IRQ);

        csi2_register_interrupt_handler(i, CSI2_CONTEXT6_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_6);
        // csi2_enable_interrupt(i,CSI2_CONTEXT6_IRQ);

        csi2_register_interrupt_handler(i, CSI2_CONTEXT7_IRQ,
                                        csi2_context_irq_hwi_interrupt_handler,
                                        i, (void *) CSI2_CONTEXT_7);
        // csi2_enable_interrupt(i,CSI2_CONTEXT7_IRQ);

    }
    return CSI2_SUCCESS;
}

/* ================================================================ */
/**
*   
*  Description:-
*  
*
*  @param 
*  @param 
*
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_unregister_interrupt_handler                                               
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
CSI2_RETURN csi2_unregister_interrupt_handler(CSI2_GLOBAL_INTERRUPT_ID_T int_id)
{

    return CSI2_FAILURE;
}

/* ================================================================ */
/**
*   
*  Description:-
*  
*
*  @param 
*  @param 
*
*  @return    void
*/
/*================================================================== */
/* ARG2 is the complex IO number */
/* ===================================================================
 *  @func     csi2_complex_io_hwi_interrupt_handler                                               
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
void csi2_complex_io_hwi_interrupt_handler(CSI2_RETURN status,
                                           uint32 device_num, void *arg2)
{

    csi2_regs_ovly csi2_regs = NULL;

    uint32 i = 0;

    volatile uint32 *sts_reg_add = 0;

    uint8 flag = 0;

    Interrupt_Handle_T *tmp = NULL;

    // isp_interrupt_id_t id;
    uint32 sts_reg = 0;

    CSI2_COMPLEXIO_NUMBER_T complex_io_num =
        (CSI2_COMPLEXIO_NUMBER_T) ((uint32) arg2);
    CSI2_GLOBAL_INTERRUPT_ID_T csi2_int_id;

    csi2_int_id =
        (CSI2_IO_1 ==
         complex_io_num) ? (CSI2_COMPLEXIO1_ERR_IRQ)
        : (CSI2_COMPLEXIO2_ERR_IRQ);

    csi2_regs = (csi2A_regs);

    /* Activate swi here only if there is a interrupt that requires a
     * callback in the SWI context */
    sts_reg_add =
        (complex_io_num ==
         CSI2_IO_1) ? (&(csi2_regs->CSI2_COMPLEXIO1_IRQSTATUS)) : (&(csi2_regs->
                                                                     CSI2_COMPLEXIO2_IRQSTATUS));

    sts_reg = *sts_reg_add;

    /* cheack all the callbacks that need to be called from HWI context */

    for (i = 0; i < CSI2_COMPLEX_IO_IRQ_MAX; i++)
    {
        tmp = NULL;
        // id=isp_callback_priority[i];

        if (((sts_reg) & (0x00000001 << i)) != 0)
        {

            tmp =
                csi2_complex_io_hwi_cb_list_head[device_num][complex_io_num][i];
            if (tmp != NULL)
            {

                while (tmp != NULL)
                {

                    if (tmp->callback != NULL)
                    {
                        tmp->callback(CSI2_SUCCESS, tmp->arg1, tmp->arg2);

                    }
                    else
                    {
                        // isp_error_count++;
                        /* ERROR , because interrupt ocurred and no client
                         * has registered, but list head is still valid */
                    }
                    tmp = tmp->list_next;
                }
            }
            else
            {
                // isp_error_count++;
                /* ERROR , because interrupt ocurred and no client has
                 * registered */
            }
            if (csi2_complex_io_swi_cb_list_head[device_num][complex_io_num][i]
                == NULL)
            {
                /* clear the status bit only if there is no corresponding HWI 
                 * handler. */
                /* the register has one bit for status of HS_VS_IRQ which is
                 * WR and 15 other status bits are RO , instead of handleing
                 * the special case with a "if" statement we just write to
                 * the read only bits */
                // isp_regs->ISP5_IRQSTATUS_0=0x00000001<<id;
                *sts_reg_add = 0x00000001 << i;
            }
            else
            {
                /* a particular interrupt occured for which there is a valid
                 * swi callback */
                flag = 1;
            }
        }
    }
    if (flag == 1)
    {
        /* Need to activate SWI, so diasable the interrupt at ISS level to
         * prevent preemption of SWI while calling the callbacks */
        csi2_disable_interrupt((CSI2_DEVICE_T) device_num, csi2_int_id);
        // Swi_post(csi2_complex_io_swi_handle);
    }
}

/* ================================================================ */
/**
*   
*  Description:-
*  
*
*  @param 
*  @param 
*
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* arg2 is the context number */
/* ===================================================================
 *  @func     csi2_context_irq_hwi_interrupt_handler                                               
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
void csi2_context_irq_hwi_interrupt_handler(CSI2_RETURN status,
                                            uint32 device_num, void *arg2)
{
    uint32 i = 0;

    volatile uint32 *sts_reg_add = 0;

    uint8 flag = 0;

    Interrupt_Handle_T *tmp = NULL;

    // isp_interrupt_id_t id;
    uint32 sts_reg = 0, reg_enable;

    uint32 base_add = 0;

    CSI2_CONTEXT_T context_num = (CSI2_CONTEXT_T) ((uint32) arg2);

    CSI2_GLOBAL_INTERRUPT_ID_T csi2_int_id;

    csi2_int_id =
        (CSI2_GLOBAL_INTERRUPT_ID_T) (CSI2_CONTEXT0_IRQ + context_num);

    /* Activate swi here only if there is a interrupt that requires a
     * callback in the SWI context */
    base_add =
        (uint32)(csi2A_regs);

    sts_reg_add = (uint32 *) CSI2_CTX_IRQSTATUS(base_add, context_num);

    sts_reg = *sts_reg_add;

    reg_enable = *(uint32 *) CSI2_CTX_IRQENABLE(base_add, context_num);

    sts_reg &= reg_enable;

    /* cheack all the callbacks that need to be called from HWI context */
    for (i = 0; i < CSI2_CONTEXT_IRQ_MAX; i++)
    {
        tmp = NULL;
        // id=isp_callback_priority[i];

        if (((sts_reg) & (0x00000001 << i)) != 0)
        {

            tmp = csi2_context_irq_hwi_cb_list_head[device_num][context_num][i];
            if (tmp != NULL)
            {

                while (tmp != NULL)
                {
                    if (tmp->callback != NULL)
                    {
                        tmp->callback(CSI2_SUCCESS, tmp->arg1, tmp->arg2);

                    }
                    else
                    {
                        // isp_error_count++;
                        /* ERROR , because interrupt ocurred and no client
                         * has registered, but list head is still valid */
                    }
                    tmp = tmp->list_next;
                }
            }
            else
            {
                // isp_error_count++;
                /* ERROR , because interrupt ocurred and no client has
                 * registered */
            }

            if (csi2_context_irq_swi_cb_list_head[device_num][context_num][i] ==
                NULL)
            {
                /* clear the status bit only if there is no correspondingSWI
                 * handler. */

                *sts_reg_add = 0x00000001 << i;
            }
            else
            {
                /* a particular interrupt occured for which there is a valid
                 * swi callback */
                flag = 1;
            }
        }
    }

    if (flag == 1)
    {
        /* Need to activate SWI, so diasable the interrupt at ISS level to
         * prevent preemption of SWI while calling the callbacks */

        csi2_disable_interrupt((CSI2_DEVICE_T) device_num, csi2_int_id);

        /* Set a flag , to be used in SWI to figyre out which context it
         * really was. */
        csi2_context_flag[device_num] |= (0x01 << context_num);
        Swi_post(csi2_context_irq_swi_handle);
    }
}

extern void csi2_context_irq_swi_interrupt_handler_1(CSI2_DEVICE_T device_num,
                                                     CSI2_CONTEXT_T arg1);

/* ===================================================================
 *  @func     csi2_context_irq_swi_interrupt_handler                                               
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
void csi2_context_irq_swi_interrupt_handler(UArg arg0, UArg arg1)
{
    CSI2_CONTEXT_T i;

    CSI2_DEVICE_T j;

    for (i = CSI2_CONTEXT_0; i < CSI2_CONTEXT_MAX; i++)
    {
        for (j = CSI2_DEVICE_A; j < CSI2_DEVICE_MAX; j++)
        {
            if ((csi2_context_flag[j] & (0x01 << i)) != 0)
            {
                csi2_context_irq_swi_interrupt_handler_1(j, i);
            }
            /* Clear the flag */
            ((csi2_context_flag[j]) &= (~(0x01 << i)));
        }
    }

}

void csi2_context_irq_swi_interrupt_handler_1(CSI2_DEVICE_T device_num,
                                              CSI2_CONTEXT_T arg1)
{
    uint32 i = 0;

    volatile uint32 *sts_reg_add = 0;

    Interrupt_Handle_T *tmp = NULL;

    // isp_interrupt_id_t id;
    uint32 sts_reg = 0;

    uint32 base_add = 0;

    CSI2_CONTEXT_T context_num = (CSI2_CONTEXT_T) arg1;

    CSI2_GLOBAL_INTERRUPT_ID_T csi2_int_id;

    csi2_int_id =
        (CSI2_GLOBAL_INTERRUPT_ID_T) (CSI2_CONTEXT0_IRQ + context_num);

    /* Activate swi here only if there is a interrupt that requires a
     * callback in the SWI context */

    base_add =
        (uint32)(csi2A_regs);

    sts_reg_add = (uint32 *) CSI2_CTX_IRQSTATUS(base_add, context_num);

    sts_reg = *sts_reg_add;

    sts_reg_add = (uint32 *) CSI2_CTX_IRQENABLE(base_add, context_num);

    sts_reg &= (*sts_reg_add);

    /* cheack all the callbacks that need to be called from HWI context */
    for (i = 0; i < CSI2_CONTEXT_IRQ_MAX; i++)
    {
        tmp = NULL;
        // id=isp_callback_priority[i];

        if (((sts_reg) & (0x00000001 << i)) != 0)
        {

            tmp = csi2_context_irq_swi_cb_list_head[device_num][context_num][i];
            if (tmp != NULL)
            {

                while (tmp != NULL)
                {
                    if (tmp->callback != NULL)
                    {
                        tmp->callback(CSI2_SUCCESS, tmp->arg1, tmp->arg2);
                        *sts_reg_add = sts_reg << i;

                    }
                    else
                    {
                        // isp_error_count++;
                        /* ERROR , because interrupt ocurred and no client
                         * has registered, but list head is still valid */
                    }
                    tmp = tmp->list_next;
                }
            }
            else
            {
                // isp_error_count++;
                /* ERROR , because interrupt ocurred and no client has
                 * registered */
            }
        }
        /* clear the status bits */
        *sts_reg_add = 0x00000001 << i;

    }
    csi2_enable_interrupt(device_num, csi2_int_id);
}

/* ================================================================ */
/**
*   
*  Description:-
*  
*
*  @param 
*  @param 
*
*  @return    CSI2_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     csi2_hwi_interrupt_handler                                               
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
void csi2_hwi_interrupt_handler(ISS_RETURN status, uint32 arg1, void *arg2)
{
    uint8 i = 0;

    csi2_regs_ovly csi2_regs = NULL;

    uint32 sts = 0;

    CSI2_DEVICE_T device_num = (CSI2_DEVICE_T) arg1;

    csi2_regs = (csi2A_regs);
    sts = csi2_regs->CSI2_IRQSTATUS;
    sts &= csi2_regs->CSI2_IRQENABLE;

    for (i = CSI2_CONTEXT0_IRQ; i < CSI2_GLOBAL_INTERRUPT_MAX; i++)
    {
        if (((sts) & (0x00000001 << i)) != 0)
        {
            if (NULL != csi2_cbk_info[device_num][i].cbk_func)
            {
                csi2_cbk_info[device_num][i].cbk_func(CSI2_SUCCESS,
                                                      csi2_cbk_info[device_num]
                                                      [i].arg1,
                                                      csi2_cbk_info[device_num]
                                                      [i].arg2);
            }

            csi2_regs->CSI2_IRQSTATUS = (0x00000001 << i);
        }
    }
}

/* ================================================================ */
/**
*   
*  Description:-
*  
*
*  @param 
*  @param 
*
*  @return    CSI2_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     csi2_irq_init                                               
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
CSI2_RETURN csi2_irq_init()
{
    /* Register with the ISS COMMON Interrupt manager and enable the CSI2
     * interrupts */
    uint8 i = 0, j = 0;

    for (j = 0; j < CSI2_DEVICE_MAX; j++)
    {
        for (i = 0; i < CSI2_GLOBAL_INTERRUPT_MAX; i++)
        {
            csi2_cbk_info[j][i].cbk_func = NULL;
            csi2_cbk_info[j][i].arg1 = 0;
            csi2_cbk_info[j][i].arg2 = NULL;
        }
    }

    iss_register_interrupt_handler(ISS_INTERRUPT_CSI2A_IRQ,
                                   csi2_hwi_interrupt_handler, CSI2_DEVICE_A,
                                   NULL);
    iss_register_interrupt_handler(ISS_INTERRUPT_CSI2B_IRQ,
                                   csi2_hwi_interrupt_handler, CSI2_DEVICE_B,
                                   NULL);

    iss_enable_interrupt(ISS_INTERRUPT_CSI2A_IRQ);
    iss_enable_interrupt(ISS_INTERRUPT_CSI2B_IRQ);

    csi2_context_irq_init();
    csi2_complex_io_irq_init();
    csi2_other_irq_init();

    return CSI2_SUCCESS;

}
