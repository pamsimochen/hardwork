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
* @path Centaurus\drivers\csl\iss\cbuff\src
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
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/System.h>

#include "inc/cbuff_reg.h"
#include "../cbuff.h"
#include "inc/cbuff_interrupts.h"

#include <ti/psp/iss/hal/iss/iss_common/iss_interrupt_config.h>

/*************************************************************************/
extern Swi_Handle cbuff_swi_handle;

#define CBUFF_INTERRUPT_NUM       25

/* ================================================================ */
/**  Description:-enum for the possible interrupt id's
 */
 /*================================================================== */
typedef enum {
    /* Mapped according to spec */
    CBUFF_INT_OCP_ERR,                                     // 0
    CBUFF_INT_reserved1,                                   // 1
    CBUFF_INT_reserved2,                                   // 2
    CBUFF_INT_reserved3,                                   // 3
    CBUFF_INT_reserved4,                                   // 4
    CBUFF_INT_reserved5,                                   // 5
    CBUFF_INT_reserved6,                                   // 6
    CBUFF_INT_reserved7,                                   // 7
    CBUFF_INT_CTX0_READY,                                  // 8
    CBUFF_INT_CTX1_READY,                                  // 9
    CBUFF_INT_CTX2_READY,                                  // 10
    CBUFF_INT_CTX3_READY,                                  // 11
    CBUFF_INT_CTX4_READY,                                  // 12
    CBUFF_INT_CTX5_READY,                                  // 13
    CBUFF_INT_CTX6_READY,                                  // 14
    CBUFF_INT_CTX7_READY,                                  // 15
    CBUFF_INT_CTX0_INVALID,                                // 16
    CBUFF_INT_CTX1_INVALID,                                // 17
    CBUFF_INT_CTX2_INVALID,                                // 18
    CBUFF_INT_CTX3_INVALID,                                // 19
    CBUFF_INT_CTX4_INVALID,                                // 20
    CBUFF_INT_CTX5_INVALID,                                // 21
    CBUFF_INT_CTX6_INVALID,                                // 22
    CBUFF_INT_CTX7_INVALID,                                // 23
    CBUFF_INT_CTX0_OVERFLOW,                               // 24
    CBUFF_INT_CTX1_OVERFLOW,                               // 25
    CBUFF_INT_CTX2_OVERFLOW,                               // 26
    CBUFF_INT_CTX3_OVERFLOW,                               // 27
    CBUFF_INT_CTX4_OVERFLOW,                               // 28
    CBUFF_INT_CTX5_OVERFLOW,                               // 29 
    CBUFF_INT_CTX6_OVERFLOW,                               // 30
    CBUFF_INT_CTX7_OVERFLOW,                               // 31
    CBUFF_INT_MAX = 32
} CBUFF_INTERRUPT_ID_T;

#define MK_CONTEXT_INTERRUPT_MASKS(CCTX_NUM)            \
        {                                               \
            CBUFF_INT_OCP_ERR,                          \
            CBUFF_INT_CTX##CCTX_NUM##_READY,            \
            CBUFF_INT_CTX##CCTX_NUM##_INVALID,          \
            CBUFF_INT_CTX##CCTX_NUM##_OVERFLOW          \
        }

const CBUFF_INTERRUPT_ID_T cbuff_interrupt_ids[CBUFF_CTX_MAX][IRQ_TYPE_ID_MAX] = {
    MK_CONTEXT_INTERRUPT_MASKS(0),
    MK_CONTEXT_INTERRUPT_MASKS(1),
    MK_CONTEXT_INTERRUPT_MASKS(2),
    MK_CONTEXT_INTERRUPT_MASKS(3),
    MK_CONTEXT_INTERRUPT_MASKS(4),
    MK_CONTEXT_INTERRUPT_MASKS(5),
    MK_CONTEXT_INTERRUPT_MASKS(6),
    MK_CONTEXT_INTERRUPT_MASKS(7)
};

/*******************************************************************************************/
/* GLOBAL DATA DEFINITION */

/*******************************************************************************************/

uint32 cbuff_error_count = 0;

CBUFF_IRQ_Handle_T *cbuff_hwi_cb_list_head[CBUFF_INT_MAX] = { NULL };
CBUFF_IRQ_Handle_T *cbuff_swi_cb_list_head[CBUFF_INT_MAX] = { NULL };

CBUFF_INTERRUPT_ID_T cbuff_callback_priority[CBUFF_INTERRUPT_NUM] = {
    /* Mapped according to priority */
    CBUFF_INT_OCP_ERR,                                     // 0
    CBUFF_INT_CTX0_INVALID,                                // 1
    CBUFF_INT_CTX1_INVALID,                                // 2
    CBUFF_INT_CTX2_INVALID,                                // 3
    CBUFF_INT_CTX3_INVALID,                                // 4
    CBUFF_INT_CTX4_INVALID,                                // 5
    CBUFF_INT_CTX5_INVALID,                                // 6
    CBUFF_INT_CTX6_INVALID,                                // 7
    CBUFF_INT_CTX7_INVALID,                                // 8
    CBUFF_INT_CTX0_OVERFLOW,                               // 9
    CBUFF_INT_CTX1_OVERFLOW,                               // 10
    CBUFF_INT_CTX2_OVERFLOW,                               // 11
    CBUFF_INT_CTX3_OVERFLOW,                               // 12
    CBUFF_INT_CTX4_OVERFLOW,                               // 13
    CBUFF_INT_CTX5_OVERFLOW,                               // 14
    CBUFF_INT_CTX6_OVERFLOW,                               // 15
    CBUFF_INT_CTX7_OVERFLOW,                               // 16
    CBUFF_INT_CTX0_READY,                                  // 17
    CBUFF_INT_CTX1_READY,                                  // 18
    CBUFF_INT_CTX2_READY,                                  // 19
    CBUFF_INT_CTX3_READY,                                  // 20
    CBUFF_INT_CTX4_READY,                                  // 21
    CBUFF_INT_CTX5_READY,                                  // 22
    CBUFF_INT_CTX6_READY,                                  // 23
    CBUFF_INT_CTX7_READY,                                  // 24
};

/* ================================================================ */
/**
*  cbuff_hook_int_handler()
*  Description :- This routine links the argument "handle " to a priority linked list for the  interrupt 
*			     specified by the argument "int_id". the handle is linked to the swi list or hwi list as specified 
*                       by the parameter "context" within handle type 
*  
*
*  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

*  @return  :- CBUFF_RETURN       
*/
/*================================================================== */
/* ===================================================================
 *  @func     CBUFF_Ctx_Hook_Interrupt                                               
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
CBUFF_RETURN CBUFF_Ctx_Hook_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                      CBUFF_IRQ_ID irq_type,
                                      CBUFF_IRQ_Handle_T * handle)
{
    CBUFF_IRQ_Handle_T **ptr_curr;

    CBUFF_INTERRUPT_ID_T int_id;

    if ((NULL == handle) || (NULL == handle->callback) ||
        (irq_type >= IRQ_TYPE_ID_MAX) || (ctx_number >= CBUFF_CTX_MAX))
        return CBUFF_FAILURE;

    int_id = cbuff_interrupt_ids[ctx_number][irq_type];

    if (CBUFF_INT_MAX <= int_id)
        return CBUFF_FAILURE;

    /* Need to protect the following piece of code from pre-emption. */
    /* Check if node need to goto the head of the list */
    if (ISS_CALLBACK_CONTEXT_HWI == handle->callback_context)
    {

        ptr_curr = &cbuff_hwi_cb_list_head[int_id];

    }
    else
    {

        ptr_curr = &cbuff_swi_cb_list_head[int_id];
    }

    while ((*ptr_curr != NULL) && (handle->priority > (*ptr_curr)->priority))
        ptr_curr = &((*ptr_curr)->list_next);

    if (*ptr_curr != handle)
    {                                                      // Do not insert
                                                           // same handle
                                                           // twice
        handle->list_next = *ptr_curr;
        *ptr_curr = handle;
    }
    return CBUFF_SUCCESS;
}

/* ================================================================ */
/**
*  CBUFF_Ctx_UnHook_Interrupt()
*  Description :- This routine de-links the argument "handle " froma priority linked list for the  interrupt 
*			     specified by the argument "int_id". the handle is de-linked from the swi list or hwi list as specified 
*                       by the parameter "context" within handle type 
*  
*
*  @param   :- CBUFF_CTX_NUMBER_TYPE ctx_number, the enumeration specifying  the context id.
*  @param   :- CBUFF_IRQ_ID          irq_type,   the enumeration specifying  the irq id.
*
*  @return  :- CBUFF_RETURN       
*/
/*================================================================== */
/* ===================================================================
 *  @func     CBUFF_Ctx_UnHook_Interrupt                                               
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
CBUFF_RETURN CBUFF_Ctx_UnHook_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                        CBUFF_IRQ_ID irq_type,
                                        CBUFF_IRQ_Handle_T * handle)
{
    CBUFF_IRQ_Handle_T **ptr_curr;

    CBUFF_INTERRUPT_ID_T int_id;

    if ((NULL == handle) || (irq_type >= IRQ_TYPE_ID_MAX) ||
        (ctx_number >= CBUFF_CTX_MAX))
        return CBUFF_FAILURE;

    int_id = cbuff_interrupt_ids[ctx_number][irq_type];

    if (CBUFF_INT_MAX <= int_id)
        return CBUFF_FAILURE;

    /* Need to protect the following piece of code from pre-emption. */
    /* Check if node need to goto the head of the list */
    if (ISS_CALLBACK_CONTEXT_HWI == handle->callback_context)
    {

        ptr_curr = &cbuff_hwi_cb_list_head[int_id];

    }
    else
    {

        ptr_curr = &cbuff_swi_cb_list_head[int_id];
    }

    while ((*ptr_curr != NULL) && (handle != *ptr_curr))
        ptr_curr = &((*ptr_curr)->list_next);

    if (NULL == *ptr_curr)
        return CBUFF_FAILURE;

    *ptr_curr = (*ptr_curr)->list_next;

    return CBUFF_SUCCESS;
}

/* ================================================================ */
/**
*  Description:- This routine is called when a swi callback has been registered for a particular event that has occured. 
*  
*
*  @param   arg0,
*  @paramand arg1 will be the user arguments while registering the SWI with BIOS
*
*  @return        CBUFF_RETURN 
*/
/*================================================================== */
/* ===================================================================
 *  @func     cbuff_swi_interrupt_handler                                               
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
void cbuff_swi_interrupt_handler(UArg arg0, UArg arg1)
{
    uint32 i = 0;

    uint32 sts_reg;

    CBUFF_IRQ_Handle_T *tmp = NULL;

    CBUFF_INTERRUPT_ID_T id;

    sts_reg = pCBUFF->IRQ_STATUS.r32;

    for (i = 0; i < CBUFF_INTERRUPT_NUM; i++)
    {
        id = cbuff_callback_priority[i];

        if (id >= CBUFF_INT_MAX)
        {
            while (1) ;                                    // needs to be
                                                           // changed to
                                                           // asserts

        }
        if (0 != ((sts_reg) & (1 << id)))
        {
            tmp = cbuff_swi_cb_list_head[id];

            if (tmp != NULL)
            {
                do
                {
                    if (tmp->callback != NULL)
                        tmp->callback(tmp->arg1, tmp->arg2);
                    else
                        cbuff_error_count++;               // ERROR, because
                                                           // interrupt
                                                           // ocurred and no
                                                           // client has
                                                           // registered!!!!

                    tmp = tmp->list_next;
                } while (tmp != NULL);

            }
            else
                cbuff_error_count++;                       /* ERROR , because 
                                                            * interrupt
                                                            * ocurred and no
                                                            * client has
                                                            * registered */
        }
    }

    /* Clear all status bits */
    pCBUFF->IRQ_STATUS.r32 = sts_reg;
#ifdef CBUFF_SIMULATION
    sim_clr_IRQ(sts_reg);
#endif                                                     // CBUFF_SIMULATION
    /* Enable ISS level interrupt here */
    iss_enable_interrupt(ISS_INTERRUPT_CBUFF_IRQ);

}

/****************************************************************/
/**
*    @ CBUFF_Ctx_Enable_Interrupt() Enables CBUFF Interrupt
*    @param          CBUFF_CTX_NUMBER_TYPE ctx_number - context number
*    @param          CBUFF_IRQ_ID          irq_type   - IRQ_OCP_ERR, IRQ_CTX_READY, IRQ_CTX_INVALID or IRQ_CTX_OVR
*    @return         status      1-interrupt pending 0 - no interrupt
*/
/****************************************************************/
/* ===================================================================
 *  @func     CBUFF_Ctx_Enable_Interrupt                                               
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
CBUFF_RETURN CBUFF_Ctx_Enable_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                        CBUFF_IRQ_ID irq_type)
{
    uint32 irq_mask;

    irq_mask = 1 << cbuff_interrupt_ids[ctx_number][irq_type];

    pCBUFF->IRQ_SET.r32 = irq_mask;

    return CBUFF_SUCCESS;
}

/****************************************************************/
/**
*    @ CBUFF_Ctx_Disable_Interrupt() Disables CBUFF Interrupt
*    @param          CBUFF_CTX_NUMBER_TYPE ctx_number - context number
*    @param          CBUFF_IRQ_ID          irq_type   - IRQ_OCP_ERR, IRQ_CTX_READY, IRQ_CTX_INVALID or IRQ_CTX_OVR
*    @return         status      CBUFF_SUCCESS 
*/
/****************************************************************/

/* ===================================================================
 *  @func     CBUFF_Ctx_Disable_Interrupt                                               
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
CBUFF_RETURN CBUFF_Ctx_Disable_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                         CBUFF_IRQ_ID irq_type)
{
    uint32 irq_mask;

    irq_mask = 1 << cbuff_interrupt_ids[ctx_number][irq_type];
    pCBUFF->IRQ_CLR.r32 = irq_mask;
    return CBUFF_SUCCESS;
}

/* ===================================================================
 *  @func     CBUFF_Disable_All_Interrupts                                               
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
void CBUFF_Disable_All_Interrupts(void)
{
    pCBUFF->IRQ_CLR.r32 = ~0;
}

/* ================================================================ */
/**
*  Description:-This is the HWI that needs to exist for handling isp interrupts,
*                     it posts a swi if there is a SWI callback registered for that particular interrupt
*  
*
*  @param  arg,	UArg arg 

*  @return      CBUFF_RETURN   
*/
/*================================================================== */
/* ===================================================================
 *  @func     cbuff_hwi_interrupt_handler                                               
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
Void cbuff_hwi_interrupt_handler(ISS_RETURN status, uint32 arg1, void *arg2)
{
    uint32 i = 0;

    uint32 sts_reg = 0;

    uint8 flag = 0;

    CBUFF_IRQ_Handle_T *tmp = NULL;

    CBUFF_INTERRUPT_ID_T id;

    /* Disable interrupt at M3 level and call SWI, in SWI acknowledge the
     * Interrupt sts register */
    /* Activate swi here only if there is a interrupt that requires a
     * callback in the SWI context */
    sts_reg = pCBUFF->IRQ_STATUS.r32;
    /* cheack all the callbacks that need to be called from HWI context */
    for (i = 0; i < CBUFF_INTERRUPT_NUM; i++)
    {
        tmp = NULL;
        id = cbuff_callback_priority[i];

        while (id >= CBUFF_INT_MAX)
            ;                                              // needs to be
                                                           // changed to
                                                           // assert

        if (0 != ((sts_reg) & (1 << id)))
        {
            tmp = cbuff_hwi_cb_list_head[id];

            if (tmp != NULL)
            {
                do
                {
                    if (tmp->callback != NULL)
                        tmp->callback(tmp->arg1, tmp->arg2);
                    else
                        cbuff_error_count++;               // ERROR, because
                                                           // interrupt
                                                           // ocurred and no
                                                           // valid callback
                                                           // has registered
                } while (tmp != NULL);
            }

            if (NULL == cbuff_swi_cb_list_head[id])
            {
                /* clear the status bit only if there is no corresponding HWI 
                 * handler. */
                /* the register has one bit for status of HS_VS_IRQ which is
                 * WR and 15 other status bits are RO , instead of handleing
                 * the special case with a "if" statement we just write to
                 * the read only bits */
                pCBUFF->IRQ_STATUS.r32 = 0x00000001 << id;

                if (NULL == cbuff_hwi_cb_list_head[id])
                    cbuff_error_count++;                   // ERROR, because
                                                           // interrupt
                                                           // ocurred and no
                                                           // client has
                                                           // registered
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
     * YES activate the same */
    if (flag == 1)
    {
        /* Need to activate SWI, so diasable the interrupt at ISS level to
         * prevent preemption of SWI while calling the callbacks */
        iss_disable_interrupt(ISS_INTERRUPT_CBUFF_IRQ);
        Swi_post(cbuff_swi_handle);
    }
    return;
}

/* ================================================================ */
/**
*  Description:-this routine needs to be called before calling any isp submodule's inits, like ipipe_init() etc
*  				this routine does the basic intialisation of isp, it disables all interrupts crears;swi's and hwi's, 
*                     does a clean reset of ISP etc
*!NOTE this is not complete
*  @param   none
*
*  @return        CBUFF_RETURN 
*/
/*================================================================== */
/* ===================================================================
 *  @func     cbuff_interrupt_init                                               
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
CBUFF_RETURN cbuff_interrupt_init()
{
    uint8 i;

    ISS_RETURN retval_iss;

    for (i = 0; i < CBUFF_INT_MAX; i++)
    {
        cbuff_hwi_cb_list_head[i] = NULL;
        cbuff_swi_cb_list_head[i] = NULL;
    }

    CBUFF_Disable_All_Interrupts();

    /* register the interrupt handler */
    retval_iss =
        iss_register_interrupt_handler(ISS_INTERRUPT_CBUFF_IRQ,
                                       cbuff_hwi_interrupt_handler,
                                       CBUFF_INTERRUPT_BANK0, NULL);

    if (CBUFF_SUCCESS != retval_iss)
        return CBUFF_FAILURE;

    iss_enable_interrupt(ISS_INTERRUPT_CBUFF_IRQ);

    return CBUFF_SUCCESS;
}
