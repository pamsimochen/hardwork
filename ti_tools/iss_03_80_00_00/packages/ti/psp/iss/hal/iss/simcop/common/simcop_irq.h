/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file commn.h
 *      This file contains the prototypes and definitions to configure  ISP5 common functionality shared 
              between different ISP compponents like,interrupt handling clock management etc.
 * 
 *
 * @path Centaurus\drivers\drv_isp\inc\csl
 *
 * @rev  1.0
 */
/*========================================================================
 *!
 *! Revision History
 *! 
 */
 /*========================================================================= */

#ifndef SIMCOP_IRQ_COMMON
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define SIMCOP_IRQ_COMMON

/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/

/* include other header files here */
#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>

// #include "isp_common_reg.h"
// #include "../../isp5_utils/isp5_sys_types.h"
// #include "../../isp5_utils/isp5_csl_utils.h"
#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>
#include <ti/psp/iss/hal/iss/simcop/simcop_global/cslr_simcop_2.h>
#include "simcop.h"                                        // iss/hal/iss/simcop/common/
#include "csl_types.h"
// typedef volatile CSL_ISP5Regs * isp_regs_ovly;

// #define ISP5_BASE_ADDRESS (0x50010000)

#define MAX_SIMCOP_INTERRUPTS 32

typedef enum {
    SIMCOP_CALLBACK_CONTEXT_SWI = 0,
    SIMCOP_CALLBACK_CONTEXT_HWI = 1,
    SIMCOP_CALLBACK_CONTEXT_MAX = 2
} SIMCOP_CALLBACK_CONTEXT_T;

/* ================================================================ */
/**  Description:- Enum for slecting the interrupt bank
 */
 /*================================================================== */

typedef uint8 simcop_interrupt_bank_t;

/* ================================================================ */
/**  Description:-enum for the possible interrupt id's
 */
 /*================================================================== */
typedef enum {

    /* Mapped according to spec */
    SIMCOP_MAX_IRQ = 32,
    SIMCOP_RESERVED12_IRQ = 31,
    SIMCOP_RESERVED11_IRQ = 30,
    SIMCOP_RESERVED10_IRQ = 29,
    SIMCOP_RESERVED09_IRQ = 28,
    SIMCOP_RESERVED08_IRQ = 27,
    SIMCOP_RESERVED07_IRQ = 26,
    SIMCOP_RESERVED06_IRQ = 25,
    SIMCOP_RESERVED05_IRQ = 24,
    SIMCOP_RESERVED04_IRQ = 23,
    SIMCOP_RESERVED03_IRQ = 2,
    SIMCOP_RESERVED02_IRQ = 21,
    SIMCOP_RESERVED01_IRQ = 20,
    SIMCOP_CPUPROCSTART_IRQ = 19,
    SIMCOP_DMA1_IRQ = 18,
    SIMCOP_MTCR2OCPERR_IRQ = 17,
    SIMCOP_OCPERR_IRQ = 16,
    SIMCOP_VLCDJDECODEERR_IRQ = 15,
    SIMCOP_DONE_IRQ = 14,
    SIMCOP_STEP3_IRQ = 13,
    SIMCOP_STEP2_IRQ = 12,
    SIMCOP_STEP1_IRQ = 11,
    SIMCOP_STEP0_IRQ = 10,
    SIMCOP_LDCBLOCK_IRQ = 9,
    SIMCOP_RESERVED00_IRQ = 8,
    SIMCOP_ROTA_IRQ = 7,
    SIMCOP_IMXB_IRQ = 6,
    SIMCOP_IMXA_IRQ = 5,
    SIMCOP_NSF_IRQ = 4,
    SIMCOP_VLCDJBLOCK_IRQ = 3,
    SIMCOP_DCT_IRQ = 2,
    SIMCOP_LDCFRAME_IRQ = 1,
    SIMCOP_DMA0_IRQ = 0
} simcop_interrupt_id_t;

/*************************************************************************/
#if 0

/* ================================================================ */
/**
 *Description:- enum for dma ctrl
 */
 /*================================================================== */
typedef enum {
    ISP_DMA_ENABLE = 1,
    ISP_DMA_DISABLE = 0
} ISP_DMA_CTRL_T;

/* ================================================================ */
/**
 *Description:- enum for the dma possibilities
 */
 /*================================================================== */
typedef enum {
    ISP_IPIPE_DPC_1 = 4,                                   /* ISP5_DMA_REQ[2] */
    ISP_IPIPE_LAST_PIX = 3,                                /* ISP5_DMA_REQ[3] */
    ISP_IPIPE_DPC_0 = 2,                                   /* ISP5_DMA_REQ[2] */
    ISP_IPIPE_HIST = 1,                                    /* ISP5_DMA_REQ[1] */
    ISP_IPIPE_BSC = 0                                      /* ISP5_DMA_REQ[0] */
} ISP_DMA_ID_T;

#endif

/* ================================================================ */
/**
 *Description:- callback function prototype
 */
 /*================================================================== */
typedef void (*simcop_callback_t) (CSL_Status status, uint32 arg1, void *arg2);

/* ================================================================ */
/**
 *Description:- struct for the registering the callback
 */
 /*================================================================== */
typedef struct {

    simcop_callback_t callback_func;
    uint32 arg1;
    void *arg2;
    SIMCOP_CALLBACK_CONTEXT_T callback_context;

} simcop_irq_t;

#if 0
/* ================================================================ */
/**
 *Description:- enum for the different sub-module clks in isp
 */
 /*================================================================== */

typedef enum {
    /* The enumerations are chosen so that they same as the bit positions
     * witihn the register ISP5_ctrl, no need of case statements while
     * enabling / disabling */
    ISP_BL_CLK = 15,
    ISP_ISIF_CLK = 14,
    ISP_H3A_CLK = 13,
    ISP_RSZ_CLK = 12,
    ISP_IPIPE_CLK = 11,
    ISP_IPIPEIF_CLK = 10,
    ISP_MAX_MODULE_CLOCKS = 6
} ISP_MODULE_CLK_T;

/* ================================================================ */
/**
 *Description:- enum for the clk ctrl
 */
 /*================================================================== */

typedef enum {
    ISP_CLK_ENABLE = 1,
    ISP_CLK_DISABLE = 0
} ISP_CLK_CTRL_T;

#endif

/* ================================================================ */
/**
 *Description:- structure for holding pointers to swi, hwi, handles, other fields can be ADDED as necessary
 */
 /*================================================================== */

/* The fields are initialised in simcop_common_init() */
typedef struct {
    Swi_Handle swi_handle;
    Hwi_Handle hwi_handle;
    simcop_interrupt_bank_t default_int_bank;

} simcop_common_dev_data_t;

/**************************************************************
  FUNCTION Prototypes
 *******************************************************************/

/* ================================================================ */
/**
 *  Description:- this routine sets up the interrupt callback for the desired interruptonly for the interrupt bank0, it does not enable the interrupt
 *                
 *
 *  @param   simcop_interrupt_id_t :- is the inpterrupt id for which the handler needs to be registered
 *			simcop_callback_t:- is the callback function that needs to be registered
 *			arg1, arg2 are the params that the callback will be invoked with when interrupt occurs	
 *  @return        CSL_Status 
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_common_set_interrupt_handler                                               
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
CSL_Status simcop_common_set_interrupt_handler(simcop_interrupt_id_t int_id,
                                               simcop_callback_t callback_func,
                                               uint32 arg1, void *arg2);

/* ================================================================ */
/**
 *  Description:- this routine unregisters the interrupt handler of the specified interrupt source only for the interrupt bank0
 *  
 *
 *  @param int_id  simcop_interrupt_id_t  int_id:- is the inpterrupt id for which the handler needs to be unregistered

 *  @return         CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_common_unset_interrupt_handler                                               
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
CSL_Status simcop_common_unset_interrupt_handler(simcop_interrupt_id_t int_id);

/* ================================================================ */
/**
 *  Description:- this routine configurees the read intervals allocated to diff sub -modules of isp
 *  
 *
 *  @param   :- isp_memory_access_ctrl_t* pointer to the cfg struct

 *  @return         ISP_RETURN
 */
 /*================================================================== */
// ISP_RETURN isp_common_read_interface_config(isp_memory_access_ctrl_t *
// cfg);

/* ================================================================ */
/**
 *  Description:- this routine enables the simcop interrupts at the simcop level
 * 
 *  
 *
 *  @param   simcop_interrupt_id_t :- specifies the interrupt number

 *  @return      CSL_Status
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
                                          simcop_interrupt_bank_t bank_num);

/* ================================================================ */
/**
 *  Description:- disables the specified interrupt
 *  NOTE!!!!!!!!!   This is the exposed function to user the bank is selected to default 0
 *
 *  @param   simcop_interrupt_id_t

 *  @return     CSL_Status
 */
 /*================================================================== */
/* This is the exposed function to user the bank is selected to default 0 */
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
                                           simcop_interrupt_bank_t bank_num);

/* ================================================================ */
/**
 *  Description:-disables the specified module's clock
 *  
 *
 *  @param   ISP_MODULE_CLK_T:- the module name

 *  @return    ISP_RETURN     
 */
 /*================================================================== */
// ISP_RETURN isp_common_disable_clk(ISP_MODULE_CLK_T module_id);

/* ================================================================ */
/**
 *  Description:-enables the clock of required module in isp
 *  
 *
 *  @param   ISP_MODULE_CLK_T:- the module name

 *  @return       ISP_RETURN  
 */
 /*================================================================== */
// ISP_RETURN isp_common_enable_clk(ISP_MODULE_CLK_T module_id);

/* ================================================================ */
/**
 *  Description:- This is a place holder currently for parsing the interrupt register and calling the callback
 *  
 *
 *  @param   arg0, and arg1 will be the values while registering the SWI with BIOS

 *  @return        
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_common_swi_interrupt_handler                                               
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
void simcop_common_swi_interrupt_handler(UArg arg0, UArg arg1);

/* ================================================================ */
/**
 *  Description:-this routine needs to be called before calling any simcop submodule's inits, like ipipe_init() etc
 *  				this routine does the basic intialisation of simcop, it disables all interrupts crears;swi's and hwi's, 
 *                     does a clean reset of SIMCOP etc,and registers interrupt handlers with ISS interrupt manager
 *!NOTE this is not complete
 *  @param   none

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
CSL_Status simcop_common_init();

/* ================================================================ */
/**
 *  Description:- Thie routine returns the sts bit specified.
 *
 *  @param   simcop_interrupt_id_t :- specifies the interrupt number
			uint32* :- pointer to hold the sts value .
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
                                     uint32 * sts_p);

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
                                     Interrupt_Handle_T * handle);

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
                                   Interrupt_Handle_T * handle);

/* ================================================================ */
/**
 *  Description:-this routine enables the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    simcop_interrupt_id_t :- is the inpterrupt id which needs to enabled
simcop_interrupt_bank_t:- specifies which bank's interrupt is enabled
 *  @return     CSL_Status    
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
CSL_Status simcop_enable_interrupt(simcop_interrupt_id_t int_id);

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
CSL_Status simcop_register_interrupt_handler(simcop_interrupt_id_t int_id, simcop_callback_t callback_func, uint32 arg1, void *arg2);   // /*, 
                                                                                                                                        // SIMCOP_CALLBACK_CONTEXT_T 
                                                                                                                                        // context*/

/* ================================================================ */
/**
 *  Description:-this routine disabled the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    simcop_interrupt_id_t :- is the inpterrupt id which needs to disabled
simcop_interrupt_bank_t:- specifies which bank's interrupt is disabled

 *  @return        CSL_Status
 */
 /*================================================================== */
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
CSL_Status simcop_disable_interrupt(simcop_interrupt_id_t int_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
