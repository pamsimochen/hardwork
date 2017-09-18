/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file iss_common.c
*     this file implements the routines required to configure iss top level functionality.
*
*
* @path Centaurus\drivers\drv_isp\src\csl
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*!
*========================================================================= */

#include "../iss_common.h"
#include "../iss_interrupt_config.h"
#include "../inc/iss_common_utils.h"

#include <xdc/std.h>
#include  <ti/sysbios/hal/Hwi.h>
#include <xdc/runtime/Gate.h>
#include <ti/sysbios/family/arm/ducati/Core.h>

#define ISS_SET_REG32(reg_out,reg_in,val,shift,mask)    \
    (REG_WRITE32(reg_out, (REG_READ32(reg_in)&(~mask))|(val<<shift)))

iss_regs_ovly iss_regs;

iss_callback_info_t iss_cbk_info[ISS_INTERRUPT_MAX];

/* ================================================================ */
/**
*  iss_init()
*  Description :- This routine will initialise the base address of the iss module and initialises
*                       the interrupt callback functions. This need sto called before calling other functions in this file
*
*
*  @param :- none
*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_core_init                                               
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
ISS_RETURN iss_core_init(Bool initSimcop)
{
    uint8 i = 0;

    iss_regs = (iss_regs_ovly) ISS_REGS_BASE_ADDR;

    for (i = 0; i < ISS_INTERRUPT_MAX; i++)
    {
    	if (!initSimcop)
			if ((i >= ISS_INTERRUPT_SIMCOP_IRQ0) && (i <= ISS_INTERRUPT_SIMCOP_IRQ3))
				continue;
        iss_cbk_info[i].cbk_func = NULL;
    }

    ISS_SET32(iss_regs->ISS_CTRL, 0xf,
              CSL_ISS_REGS_ISS_CTRL_CCP2R_TAG_CNT_SHIFT,
              CSL_ISS_REGS_ISS_CTRL_CCP2R_TAG_CNT_MASK);
    ISS_SET32(iss_regs->ISS_CTRL, 0xf,
              CSL_ISS_REGS_ISS_CTRL_CCP2W_TAG_CNT_SHIFT,
              CSL_ISS_REGS_ISS_CTRL_CCP2W_TAG_CNT_MASK);
    ISS_SET32(iss_regs->ISS_CTRL, 0xf,
              CSL_ISS_REGS_ISS_CTRL_CSI2_A_TAG_CNT_SHIFT,
              CSL_ISS_REGS_ISS_CTRL_CSI2_A_TAG_CNT_MASK);
    ISS_SET32(iss_regs->ISS_CTRL, (uint32) 0xf,
              CSL_ISS_REGS_ISS_CTRL_CSI2_B_TAG_CNT_SHIFT,
              CSL_ISS_REGS_ISS_CTRL_CSI2_B_TAG_CNT_MASK);

    return ISS_SUCCESS;
}

/* ================================================================ */
/**
*  iss_module_clk_ctrl()
*  Description :- This routine will modify the clock status of the module specified.
*
*
*  @param :- ISS_MODULE_CLK_T module, the parameter which specifies the module whose clk is to be modified
*   @param :-ISS_CLK_CTRL_T ctrl, the parameter which specifies the clk is to be enabled or disabled.
*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_module_clk_ctrl                                               
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
ISS_RETURN iss_module_clk_ctrl(ISS_MODULE_CLK_T module, ISS_CLK_CTRL_T ctrl)
{
    // REG_WRITE32(iss_regs->ISS_CLKCTRL, (REG_READ32(iss_regs->ISS_CLKSTAT)
    // & (~(0x00000001<<module)));
    ISS_SET_REG32(iss_regs->ISS_CLKCTRL, &iss_regs->ISS_CLKSTAT, ctrl, module,
                  (0x00000001 << module));
    // ISS_SET32(iss_regs->ISS_CLKCTRL,ctrl,module,(0x00000001<<module));

    return ISS_SUCCESS;

}

/* ================================================================ */
/**
*  iss_get_module_clk_status()
*  Description :- This routine will query the clock status of the module specified
*
*  @param :-ISS_MODULE_CLK_T module, the parameter which specifies the module whose clk is to be checked
*  @param :-ISS_CLK_CTRL_T * ctrl_p, the pointer to a location which gives the clk status
*
*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_get_module_clk_status                                               
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
ISS_RETURN iss_get_module_clk_status(ISS_MODULE_CLK_T module,
                                     ISS_CLK_CTRL_T * ctrl_p)
{
    *ctrl_p =
        (ISS_CLK_CTRL_T) ISS_FEXT(iss_regs->ISS_CLKSTAT, module,
                                  (0x00000001 << module));

    return ISS_SUCCESS;
}

/* ================================================================ */
/**
*  iss_get_pm_status()
*  Description :- This routine will query the power management status of the module specified
*
*
*  @param :- ISS_MODULE_PM_T module,the parameter which specifies the module whose power status is to be checked
*   @param :- ISS_PM_STATE_T * state,he pointer to a location which gives the power status
*
*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_get_pm_status                                               
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
ISS_RETURN iss_get_pm_status(ISS_MODULE_PM_T module, ISS_PM_STATE_T * state)
{

    *state =
        (ISS_PM_STATE_T) ISS_FEXT(iss_regs->ISS_PM_STATUS, module,
                                  (0x00000003 << module));
    return ISS_SUCCESS;
}

/* ================================================================ */
/**
*  iss_clk_config()
*  Description :- This routine will enable/disable the specified modules clock at ISS level
*
*
*  @param :- ISS_CLK_DIV_T div_val, the parameter specifying the divider value
*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_clk_config                                               
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
ISS_RETURN iss_clk_config(ISS_CLK_DIV_T div_val)
{

    ISS_SET32(iss_regs->ISS_CTRL, div_val,
              CSL_ISS_REGS_ISS_CTRL_ISS_CLK_DIV_SHIFT,
              CSL_ISS_REGS_ISS_CTRL_ISS_CLK_DIV_MASK);
    return ISS_SUCCESS;
}

/* ================================================================ */
/**
*  iss_isp_input_config()
*  Description :- This routine will configure the input to ISP
*
*
*  @param :- ISS_ISP_INPUT_SEL_T ip_sel, the parameter which selects the input to ISP
*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_isp_input_config                                               
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
ISS_RETURN iss_isp_input_config(ISS_ISP_INPUT_SEL_T ip_sel)
{

    ISS_SET32(iss_regs->ISS_CTRL, ip_sel, CSL_ISS_REGS_ISS_CTRL_INPUT_SEL_SHIFT,
              CSL_ISS_REGS_ISS_CTRL_INPUT_SEL_MASK);

    return ISS_SUCCESS;

}

/* ================================================================ */
/**
*  iss_sync_detect_config()
*  Description :- This routine will set the active transition type of HS/VS signals
*
*
*  @param :- ISS_SYNC_DETECT_T sync, the parameter which specifies the HS VS active transitions.
*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_sync_detect_config                                               
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
ISS_RETURN iss_sync_detect_config(ISS_SYNC_DETECT_T sync)
{
    ISS_SET32(iss_regs->ISS_CTRL, sync, CSL_ISS_REGS_ISS_CTRL_SYNC_DETECT_SHIFT,
              CSL_ISS_REGS_ISS_CTRL_SYNC_DETECT_MASK);
    return ISS_SUCCESS;

}

/* ================================================================ */
/**
*  iss_enable_interrupt()
*  Description :- This routine will enable the interrupt specified by the parameter irq_id in t
*                       the default interrupt bank associated with the module generating the particular interrupt.
*
*
*  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_enable_interrupt                                               
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
ISS_RETURN iss_enable_interrupt(ISS_INTERRUPT_ID_T irq_id)
{

    uint32 *regadd = NULL;

    uint32 reg_diff = 0;

    reg_diff =
        (uint32) & iss_regs->ISS_HL_IRQENABLE_SET_1 -
        (uint32) & iss_regs->ISS_HL_IRQENABLE_SET_0;
    regadd = (uint32 *) & iss_regs->ISS_HL_IRQENABLE_SET_0;

    /* check if handler is registered else return error */
    /* Check if already interrupt handler is registered */
    if (iss_cbk_info[irq_id].cbk_func == NULL)
    {
        return ISS_FAILURE;
    }

    if (irq_id <= ISS_INTERRUPT_ISP_IRQ3)
    {
        /* enable in ISS_DEFAULT_ISP_INTERRUPT_BANK */
        reg_diff = reg_diff * ISS_DEFAULT_ISP_INTERRUPT_BANK;

    }

    else if ((irq_id >= ISS_INTERRUPT_SIMCOP_IRQ0) &&
             (irq_id <= ISS_INTERRUPT_SIMCOP_IRQ3))
    {
        /* enable in ISS_DEFAULT_SIMCOP_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_SIMCOP_INTERRUPT_BANK;
    }

    else if ((irq_id >= ISS_INTERRUPT_CSI2A_IRQ) &&
             (irq_id <= ISS_INTERRUPT_CSI2B_IRQ))
    {
        /* enable in ISS_DEFAULT_CSI2A_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_CSI2A_INTERRUPT_BANK;
    }
    else if ((irq_id >= ISS_INTERRUPT_CCP2_IRQ0) &&
             (irq_id <= ISS_INTERRUPT_CCP2_IRQ3))
    {
        /* enable in ISS_DEFAULT_CCP2_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_CCP2_INTERRUPT_BANK;
    }

    else if ((irq_id == ISS_INTERRUPT_BTE_IRQ))
    {
        /* enable in ISS_DEFAULT_BTE_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_BTE_INTERRUPT_BANK;
    }

    else if ((irq_id == ISS_INTERRUPT_CBUFF_IRQ))
    {

        /* enable in ISS_DEFAULT_CBUFF_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_CBUFF_INTERRUPT_BANK;
    }

    regadd = (uint32 *) ((uint32) regadd + (reg_diff));

    /* Set the bit in the particular register */
    ISS_WRITE32(*regadd, 0x00000001 << irq_id);

    return ISS_SUCCESS;

}

/* ================================================================ */
/**
*  iss_disable_interrupt()
*  Description :- This routine will disable the interrupt specified by the parameter irq_id in t
*                       the default interrupt bank associated with the module generating the particular interrupt.
*
*
*  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_disable_interrupt                                               
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
ISS_RETURN iss_disable_interrupt(ISS_INTERRUPT_ID_T irq_id)
{

    uint32 *regadd = NULL;

    uint32 reg_diff = 0;

    reg_diff =
        (uint32) & iss_regs->ISS_HL_IRQENABLE_CLR_1 -
        (uint32) & iss_regs->ISS_HL_IRQENABLE_CLR_0;
    regadd = (uint32 *) & iss_regs->ISS_HL_IRQENABLE_CLR_0;

    /* check if handler is registered else return error */
    /* Check if already interrupt handler is registered */
    if (iss_cbk_info[irq_id].cbk_func == NULL)
    {
        return ISS_FAILURE;
    }

    if (irq_id <= ISS_INTERRUPT_ISP_IRQ3)
    {
        /* enable in ISS_DEFAULT_ISP_INTERRUPT_BANK */
        reg_diff = reg_diff * ISS_DEFAULT_ISP_INTERRUPT_BANK;

    }

    else if ((irq_id >= ISS_INTERRUPT_SIMCOP_IRQ0) &&
             (irq_id <= ISS_INTERRUPT_SIMCOP_IRQ3))
    {
        /* enable in ISS_DEFAULT_SIMCOP_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_SIMCOP_INTERRUPT_BANK;
    }

    else if ((irq_id >= ISS_INTERRUPT_CSI2A_IRQ) &&
             (irq_id <= ISS_INTERRUPT_CSI2B_IRQ))
    {
        /* enable in ISS_DEFAULT_CSI2A_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_CSI2A_INTERRUPT_BANK;
    }
    else if ((irq_id >= ISS_INTERRUPT_CCP2_IRQ0) &&
             (irq_id <= ISS_INTERRUPT_CCP2_IRQ3))
    {
        /* enable in ISS_DEFAULT_CCP2_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_CSI2B_INTERRUPT_BANK;
    }

    else if ((irq_id >= ISS_INTERRUPT_BTE_IRQ))
    {
        /* enable in ISS_DEFAULT_BTE_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_BTE_INTERRUPT_BANK;
    }

    else if ((irq_id >= ISS_INTERRUPT_CBUFF_IRQ))
    {

        /* enable in ISS_DEFAULT_CBUFF_BANK */
        reg_diff = reg_diff * ISS_DEFUALT_CBUFF_INTERRUPT_BANK;
    }

    regadd = (uint32 *) ((uint32) regadd + (reg_diff));

    /* Set the bit in the particular register */
    ISS_WRITE32(*regadd, 0x00000001 << irq_id);

    return ISS_SUCCESS;

}

/* ================================================================ */
/**
*  iss_unregister_interrupt_handler()
*  Description :- This routine will unregister the interrupt handler for the specified interrupt , specified by the
*                       by the parameter irq_id .
*
*
*  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.

*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_unregister_interrupt_handler                                               
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
ISS_RETURN iss_unregister_interrupt_handler(ISS_INTERRUPT_ID_T irq_id)
{

    /* Check if already interrupt handler is registered */
    if (iss_cbk_info[irq_id].cbk_func == NULL)
    {
        return ISS_FAILURE;
    }

    iss_cbk_info[irq_id].cbk_func = NULL;
    iss_cbk_info[irq_id].arg1 = 0;
    iss_cbk_info[irq_id].arg2 = 0;

    return ISS_SUCCESS;

}

/* ================================================================ */
/**
*  iss_unregister_interrupt_handler()
*  Description :- This routine will register the interrupt handler for the interrupt specified by irq_id , the callback and the arguments
*                       are "callback_func, arg1,arg2 respectively.when the interrupt occurs the callback_func will be called
*                       with arg1 and arg2 as parameters to the callback function.
*
*
*  @param   :- ISS_INTERRUPT_ID_T irq_id, the enumeration specifying  the interrupt id.
*  @param   :- iss_irq_callback_t callback_func, the callback routine that needs to be called.
*  @param   :-  uint32 arg1 , is the user

*  @return  :- ISS_RETURN
*/
/*================================================================== */

/* ===================================================================
 *  @func     iss_register_interrupt_handler                                               
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
ISS_RETURN iss_register_interrupt_handler(ISS_INTERRUPT_ID_T irq_id,
                                          iss_irq_callback_t callback_func,
                                          uint32 arg1, void *arg2)
{

    /* Check if already interrupt handler is registered */
    if (iss_cbk_info[irq_id].cbk_func != NULL)
    {
        return ISS_FAILURE;
    }

    iss_cbk_info[irq_id].cbk_func = callback_func;
    iss_cbk_info[irq_id].arg1 = arg1;
    iss_cbk_info[irq_id].arg2 = arg2;

    return ISS_SUCCESS;

}

extern Hwi_Handle iss_hwi_handle_0;

extern Hwi_Handle iss_hwi_handle_1;

extern Hwi_Handle iss_hwi_handle_2;

extern Hwi_Handle iss_hwi_handle_3;

extern Hwi_Handle iss_hwi_handle_4;

extern Hwi_Handle iss_hwi_handle_5;

/* ================================================================ */
/**
*  iss_interrupt_hwi_0()
*  Description :- This routine is the HWI callback for inerrupt line 0 of ISS (IRQ 29 currently).This routine goes thru the
*                       registered callbacks and calls them if a particular interrupt ,associated with the IRQ line, has occured.
*
*  @param   :- UArg arg, the argument use while creating the HWI
*  @return  :- ISS_RETURN
*/
/*================================================================== */
void iss_interrupt_hwi_0(UArg arg)
{
    uint32 sts = 0;

    ISS_INTERRUPT_ID_T i;

    Uint8 core_id = 0xFF;

    core_id = Core_getId();
    if (core_id == 1)
    {
        /* Disable HWI while calling the callbacks */
        Hwi_disableInterrupt(32);

        sts = iss_regs->ISS_HL_IRQSTATUS_0;

        /* We know that it line o can get activated only if a ISP Event has
         * occurred since the mapping is static */
        for (i = ISS_INTERRUPT_ISP_IRQ0; i <= ISS_INTERRUPT_ISP_IRQ3; i++)
        {
            if (((sts) & (0x00000001 << i)) != 0)
            {
                if (NULL != iss_cbk_info[i].cbk_func)
                {

                    iss_cbk_info[i].cbk_func(ISS_SUCCESS, iss_cbk_info[i].arg1,
                                             iss_cbk_info[i].arg2);

                }
                else
                {
                    /* ERROR , cause no handler is present for this
                     * particular interrupt */
                }

                /* Clear the particular status bit */
                iss_regs->ISS_HL_IRQSTATUS_0 = (0x00000001 << i);
            }
        }

        Hwi_enableInterrupt(32);
    }
}

/* ================================================================ */
/**
*  iss_interrupt_hwi_1()
*  Description :- This routine is the HWI callback for inerrupt line 0 of ISS (IRQ 29 currently).This routine goes thru the
*                       registered callbacks and calls them if a particular interrupt ,associated with the IRQ line, has occured.
*
*  @param   :- UArg arg, the argument use while creating the HWI
*  @return  :- ISS_RETURN
*/
/*================================================================== */

void iss_interrupt_hwi_1(UArg arg)
{
    uint32 sts = 0;

    ISS_INTERRUPT_ID_T i;

    Uint8 core_id = 0xFF;

    core_id = Core_getId();
    if (core_id == 1)
    {
        /* Disable HWI while calling the callbacks */
        Hwi_disableInterrupt(33);

        sts = iss_regs->ISS_HL_IRQSTATUS_1;

        /* We know that it line o can get activated only if a ISP Event has
         * occurred since the mapping is static */
        for (i = ISS_INTERRUPT_SIMCOP_IRQ0; i <= ISS_INTERRUPT_SIMCOP_IRQ3; i++)
        {
            if (((sts) & (0x00000001 << i)) != 0)
            {
                if (NULL != iss_cbk_info[i].cbk_func)
                {
                    iss_cbk_info[i].cbk_func(ISS_SUCCESS, iss_cbk_info[i].arg1,
                                             iss_cbk_info[i].arg2);

                }
                else
                {
                    /* ERROR , cause no handler is present for this
                     * particular interrupt */
                }

                /* Clear the particular status bit */
                iss_regs->ISS_HL_IRQSTATUS_1 = (0x00000001 << i);
            }
        }

        Hwi_enableInterrupt(33);
    }
}

/* ================================================================ */
/**
*  iss_interrupt_hwi_2()
*  Description :- This routine is the HWI callback for inerrupt line 0 of ISS (IRQ 29 currently).This routine goes thru the
*                       registered callbacks and calls them if a particular interrupt ,associated with the IRQ line, has occured.
*
*  @param   :- UArg arg, the argument use while creating the HWI
*  @return  :- ISS_RETURN
*/
/*================================================================== */
void iss_interrupt_hwi_2(UArg arg)
{
    uint32 sts = 0;

    ISS_INTERRUPT_ID_T i;

    Uint8 core_id = 0xFF;

    core_id = Core_getId();
    if (core_id == 1)
    {
        /* Disable HWI while calling the callbacks */
        Hwi_disableInterrupt(34);
        sts = iss_regs->ISS_HL_IRQSTATUS_2;

        /* We know that it line o can get activated only if a ISP Event has
         * occurred since the mapping is static */
        for (i = ISS_INTERRUPT_CSI2A_IRQ; i <= ISS_INTERRUPT_CSI2B_IRQ; i++)
        {
            if (((sts) & (0x00000001 << i)) != 0)
            {
                if (NULL != iss_cbk_info[i].cbk_func)
                {
                    iss_cbk_info[i].cbk_func(ISS_SUCCESS, iss_cbk_info[i].arg1,
                                             iss_cbk_info[i].arg2);

                }
                else
                {
                    /* ERROR , cause no handler is present for this
                     * particular interrupt */
                }

                /* Clear the particular status bit */
                iss_regs->ISS_HL_IRQSTATUS_2 = (0x00000001 << i);
            }
        }

        Hwi_enableInterrupt(34);
    }
}

/* ================================================================ */
/**
*  iss_interrupt_hwi_3()
*  Description :- This routine is the HWI callback for inerrupt line 0 of ISS (IRQ 29 currently).This routine goes thru the
*                       registered callbacks and calls them if a particular interrupt ,associated with the IRQ line, has occured.
*
*  @param   :- UArg arg, the argument use while creating the HWI
*  @return  :- ISS_RETURN
*/
/*================================================================== */
void iss_interrupt_hwi_3(UArg arg)
{
    uint32 sts = 0;

    ISS_INTERRUPT_ID_T i = ISS_INTERRUPT_ISP_IRQ0;

    Uint8 core_id = 0xFF;

    core_id = Core_getId();
    if (core_id == 1)
    {

        /* Disable HWI while calling the callbacks */
        Hwi_disableInterrupt(35);
        sts = iss_regs->ISS_HL_IRQSTATUS_3;

        /* We know that it line o can get activated only if a ISP Event has
         * occurred since the mapping is static */
        for (i = ISS_INTERRUPT_CCP2_IRQ0; i <= ISS_INTERRUPT_CCP2_IRQ3; i++)
        {
            if (((sts) & (0x00000001 << i)) != 0)
            {
                if (NULL != iss_cbk_info[i].cbk_func)
                {
                    iss_cbk_info[i].cbk_func(ISS_SUCCESS, iss_cbk_info[i].arg1,
                                             iss_cbk_info[i].arg2);
                }
                else
                {
                    /* ERROR , cause no handler is present for this
                     * particular interrupt */
                }
                /* Clear the particular status bit */
                iss_regs->ISS_HL_IRQSTATUS_3 = (0x00000001 << i);

            }
        }

        Hwi_enableInterrupt(35);
    }
}

/* ================================================================ */
/**
*  iss_interrupt_hwi_4()
*  Description :- This routine is the HWI callback for inerrupt line 0 of ISS (IRQ 29 currently).This routine goes thru the
*                       registered callbacks and calls them if a particular interrupt ,associated with the IRQ line, has occured.
*
*  @param   :- UArg arg, the argument use while creating the HWI
*  @return  :- ISS_RETURN
*/
/*================================================================== */
void iss_interrupt_hwi_4(UArg arg)
{
    uint32 sts = 0;

    ISS_INTERRUPT_ID_T i;

    Uint8 core_id = 0xFF;

    core_id = Core_getId();
    if (core_id == 1)
    {
        /* Disable HWI while calling the callbacks */
        Hwi_disableInterrupt(36);

        sts = iss_regs->ISS_HL_IRQSTATUS_4;

        /* We know that it line o can get activated only if a ISP Event has
         * occurred since the mapping is static */
        for (i = ISS_INTERRUPT_BTE_IRQ; i <= ISS_INTERRUPT_BTE_IRQ; i++)
        {
            if (((sts) & (0x00000001 << i)) != 0)
            {
                if (NULL != iss_cbk_info[i].cbk_func)
                {
                    iss_cbk_info[i].cbk_func(ISS_SUCCESS, iss_cbk_info[i].arg1,
                                             iss_cbk_info[i].arg2);
                }
                else
                {

                    /* ERROR , cause no handler is present for this
                     * particular interrupt */
                }

                /* Clear the particular status bit */
                iss_regs->ISS_HL_IRQSTATUS_4 = (0x00000001 << i);
            }
        }

        Hwi_enableInterrupt(36);
    }
}

/* ================================================================ */
/**
*  iss_interrupt_hwi_5()
*  Description :- This routine is the HWI callback for inerrupt line 0 of ISS (IRQ 29 currently).This routine goes thru the
*                       registered callbacks and calls them if a particular interrupt ,associated with the IRQ line, has occured.
*
*  @param   :- UArg arg, the argument use while creating the HWI
*  @return  :- ISS_RETURN
*/
/*================================================================== */
void iss_interrupt_hwi_5(UArg arg)
{
    uint32 sts = 0;

    ISS_INTERRUPT_ID_T i;

    Uint8 core_id = 0xFF;

    core_id = Core_getId();
    if (core_id == 1)
    {
        /* Disable HWI while calling the callbacks */
        Hwi_disableInterrupt(37);
        sts = iss_regs->ISS_HL_IRQSTATUS_5;

        /* We know that it line o can get activated only if a ISP Event has
         * occurred since the mapping is static */
        for (i = ISS_INTERRUPT_CBUFF_IRQ; i <= ISS_INTERRUPT_CBUFF_IRQ; i++)
        {
            if (((sts) & (0x00000001 << i)) != 0)
            {
                if (NULL != iss_cbk_info[i].cbk_func)
                {
                    iss_cbk_info[i].cbk_func(ISS_SUCCESS, iss_cbk_info[i].arg1,
                                             iss_cbk_info[i].arg2);
                }
                else
                {
                    /* ERROR , cause no handler is present for this
                     * particular interrupt */
                }
                /* Clear the particular status bit */
                iss_regs->ISS_HL_IRQSTATUS_5 = (0x00000001 << i);
            }
        }
        Hwi_clearInterrupt(37);
        Hwi_enableInterrupt(37);
    }
}

/* ================================================================ */
/**
*  iss_interrupt_hwi_1()
*  Description :- This routine is the HWI callback for inerrupt line 0 of ISS (IRQ 29 currently).This routine goes thru the
*                       registered callbacks and calls them if a particular interrupt ,associated with the IRQ line, has occured.
*
*  @param   :- UArg arg, the argument use while creating the HWI
*  @return  :- ISS_RETURN
*/
/*================================================================== */
/* ===================================================================
 *  @func     fd_isr_callback                                               
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
void fd_isr_callback(UArg arg)
{
    Hwi_disableInterrupt(56);

    /* call the FD app callback */
    if (iss_cbk_info[ISS_INTERRUPT_FD].cbk_func != NULL)
    {
        iss_cbk_info[ISS_INTERRUPT_FD].cbk_func(ISS_SUCCESS,
                                                iss_cbk_info[ISS_INTERRUPT_FD].
                                                arg1,
                                                iss_cbk_info[ISS_INTERRUPT_FD].
                                                arg2);
    }

    Hwi_clearInterrupt(56);
    Hwi_enableInterrupt(56);
}
