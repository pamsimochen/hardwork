/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file ipipe.h
*           This file contains the prototypes and definitions to configure ipipe
* 
*
* @path     Centaurus\drivers\drv_isp\inc\csl
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! 
*/
/*========================================================================= */
#ifndef ISS_COMMON_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define ISS_COMMON_H

#include <ti/psp/iss/hal/iss/common/csl_utils/csl_types.h>
#include "./inc/iss_common_reg.h"

#define ISS_SUCCESS CSL_SUCCESS
#define ISS_FAILURE CSL_FAILURE
#define ISS_RESOURCE_UNAVAILABLE  CSL_RESOURCE_UNAVAILABLE
#define ISS_BUSY CSL_BUSY
#define ISS_INVALID_INPUT CSL_INVALID_INPUT
typedef CSL_RETURN ISS_RETURN;

typedef volatile CSL_Iss_regs_Regs *iss_regs_ovly;

#define ISS_REGS_BASE_ADDR 0x55040000

#define SL2_MEMORY_SIZE     (256*1024)
#define SL2_MEMORY_ADDRESS  0x5B000000

/*=======================================================================*/
/**
* Enumeration to select the clk source that needs to be modified
*
*/
/*=======================================================================*/

typedef enum {
    ISS_SIMCOP_CLK = 0,
    ISS_ISP_CLK = 1,
    ISS_CSI2A_CLK = 2,
    ISS_CSI2B_CLK = 3,
    ISS_CCP2_CLK = 4
} ISS_MODULE_CLK_T;

/*=======================================================================*/
/**
* Enumeration to ctrl the clk source that is selected by using the enumeration ISS_MODULE_CLK_T"
*
*/
/*=======================================================================*/

typedef enum {

    ISS_CLK_ENABLE = 1,
    ISS_CLK_DISABLE = 0
} ISS_CLK_CTRL_T;

typedef enum {
    ISS_CSI2A_PM = 0,
    ISS_CSI2_PM = 2,
    ISS_CCP2_PM = 4,
    ISS_ISP_PM = 6,
    ISS_SIMCOP_PM = 8,
    ISS_BTE_PM = 10,
    ISS_CBUFF_PM = 12
} ISS_MODULE_PM_T;

typedef enum {
    ISS_PM_STANDBY = 0,
    ISS_PM_TRANSITION = 1,
    ISS_PM_FUNCTIONAL = 2
} ISS_PM_STATE_T;

typedef enum {

    ISS_CLK_DIV_2 = 0,
    ISS_CLK_DIV_4 = 1,
    ISS_CLK_DIV_8 = 3
} ISS_CLK_DIV_T;

typedef enum {
    ISS_SYNC_HS_FALLING_EDGE = 0,
    ISS_SYNC_HS_RISING_EDGE = 1,
    ISS_SYNC_VS_FALLING_EDGE = 2,
    ISS_SYNC_VS_RISING_EDGE = 3
} ISS_SYNC_DETECT_T;

typedef enum {

    ISS_ISP_INPUT_CSI2_A = 0,
    ISS_ISP_INPUT_CSI2_B = 1,
    ISS_ISP_INPUT_CCCP2 = 2,
    ISS_ISP_INPUT_PI = 3                                   /* Parallel
                                                            * interface */
} ISS_ISP_INPUT_SEL_T;

/* ================================================================ */
/**
*ISS_INTERRUPT_ID_T:- enumeration to represent the available interrupts at ISS level
*/
/*================================================================== */

typedef enum {

    ISS_INTERRUPT_ISP_IRQ0 = 0,
    ISS_INTERRUPT_ISP_IRQ1 = 1,
    ISS_INTERRUPT_ISP_IRQ2 = 2,
    ISS_INTERRUPT_ISP_IRQ3 = 3,
    ISS_INTERRUPT_CSI2A_IRQ = 4,
    ISS_INTERRUPT_CSI2B_IRQ = 5,
    ISS_INTERRUPT_CCP2_IRQ0 = 6,
    ISS_INTERRUPT_CCP2_IRQ1 = 7,
    ISS_INTERRUPT_CCP2_IRQ2 = 8,
    ISS_INTERRUPT_CCP2_IRQ3 = 9,
    ISS_INTERRUPT_CBUFF_IRQ = 10,
    ISS_INTERRUPT_BTE_IRQ = 11,
    ISS_INTERRUPT_SIMCOP_IRQ0 = 12,
    ISS_INTERRUPT_SIMCOP_IRQ1 = 13,
    ISS_INTERRUPT_SIMCOP_IRQ2 = 14,
    ISS_INTERRUPT_SIMCOP_IRQ3 = 15,
    ISS_INTERRUPT_CCP2_IRQ8 = 16,
    ISS_INTERRUPT_HS_VS_IRQ = 17,
    ISS_INTERRUPT_FD = 18,
    ISS_INTERRUPT_MAX = 19
} ISS_INTERRUPT_ID_T;

/* ================================================================ */
/**
*iss_irq_callback_t:-  type of the callback registered with ISS interrupt handler
*/
/*================================================================== */

typedef void (*iss_irq_callback_t) (ISS_RETURN status, uint32 arg1, void *arg2);

/* ================================================================ */
/**
*iss_callback_info_t :- this is the structure which is used to store the callback info  passed by the user 
*/
/*================================================================== */

typedef struct {

    iss_irq_callback_t cbk_func;
    uint32 arg1;
    void *arg2;

} iss_callback_info_t;

/* ================================================================ */
/**
*ISS_CALLBACK_CONTEXT_T :- is a enumeration for the possible callback types, this will be used by the 
*                                              ISS sub-module like ISP SIMCOP etc 
*/
/*================================================================== */

typedef enum {

    ISS_CALLBACK_CONTEXT_SWI = 0,

    ISS_CALLBACK_CONTEXT_HWI = 1,

    ISS_CALLBACK_CONTEXT_MAX = 2
} ISS_CALLBACK_CONTEXT_T;

/* ================================================================ */
/**
*Interrupt_Handle_T :- this structure is a handle to registering the interrupts
*
* @param :- list_next - pointer to the next element in the list, user should not use/modify this field
* @param :- context  - this field specifies the HWI or SWI context 
* @param :- priority :- this is the priority of the callbacks for the same interrupt 0:- Lowest priority; 255:- highest priority
* @param :- arg1,arg2:- user arguments that will be passed to the callback when interrupt occurs
*/
/*================================================================== */

typedef struct Interrupt_handle_t {

    struct Interrupt_handle_t *list_next;

    ISS_CALLBACK_CONTEXT_T context;

    uint8 priority;                                        /* :- 255 Lowest
                                                            * priority, 0 :-
                                                            * highest
                                                            * priority */
    iss_irq_callback_t callback;
    uint32 arg1;
    void *arg2;

} Interrupt_Handle_T;

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
ISS_RETURN iss_core_init(Bool initSimcop);

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
ISS_RETURN iss_module_clk_ctrl(ISS_MODULE_CLK_T module, ISS_CLK_CTRL_T ctrl);

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
                                     ISS_CLK_CTRL_T * ctrl_p);

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
ISS_RETURN iss_get_pm_status(ISS_MODULE_PM_T module, ISS_PM_STATE_T * state);

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
ISS_RETURN iss_clk_config(ISS_CLK_DIV_T div_val);

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
ISS_RETURN iss_isp_input_config(ISS_ISP_INPUT_SEL_T ip_sel);

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
ISS_RETURN iss_sync_detect_config(ISS_SYNC_DETECT_T sync);

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
ISS_RETURN iss_enable_interrupt(ISS_INTERRUPT_ID_T irq_id);

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
ISS_RETURN iss_disable_interrupt(ISS_INTERRUPT_ID_T irq_id);

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
ISS_RETURN iss_unregister_interrupt_handler(ISS_INTERRUPT_ID_T irq_id);

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
                                          uint32 arg1, void *arg2);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
