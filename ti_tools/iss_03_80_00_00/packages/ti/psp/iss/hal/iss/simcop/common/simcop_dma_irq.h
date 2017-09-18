/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ====================================================================== */
/**
 * @file commn.h
 *      This file contains the prototypes and definitions to configure  
 * ISP5 common functionality shared between different ISP compponents like,
 * interrupt handling clock management etc.
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
 /*===================================================================== */

#ifndef SIMCOP_DMA_IRQ_COMMON
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define SIMCOP_DMA_IRQ_COMMON

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
#include "simcop.h"
// typedef volatile CSL_ISP5Regs * isp_regs_ovly;

// #define ISP5_BASE_ADDRESS (0x50010000)

#define MAX_SIMCOP_DMA_INTERRUPTS 32

/* Number of banks in SIMCOP DMA */

#define SIMCOP_DMA_INTERRUPT_BANK0 0
#define SIMCOP_DMA_INTERRUPT_BANK1 1

#define SIMCOP_DMA_DEFAULT_INTERRUPT_BANK SIMCOP_DMA_INTERRUPT_BANK0

typedef enum {
    SIMCOP_DMA_CALLBACK_CONTEXT_SWI = 0,
    SIMCOP_DMA_CALLBACK_CONTEXT_HWI = 1,
    SIMCOP_DMA_CALLBACK_CONTEXT_MAX = 2
} SIMCOP_DMA_CALLBACK_CONTEXT_T;

/* ================================================================ */
/**  Description:- Enum for slecting the interrupt bank
 */
 /*================================================================== */
typedef uint8 simcop_dma_interrupt_bank_t;

/* ================================================================ */
/**  Description:-enum for the possible interrupt id's
 */
 /*================================================================== */
typedef enum {

    /* Mapped according to spec */
    SIMCOP_DMA_MAX_IRQ = 32,
    SIMCOP_DMA_CHAN7_FRAME_DONE_IRQ = 31,
    SIMCOP_DMA_CHAN6_FRAME_DONE_IRQ = 30,
    SIMCOP_DMA_CHAN5_FRAME_DONE_IRQ = 29,
    SIMCOP_DMA_CHAN4_FRAME_DONE_IRQ = 28,
    SIMCOP_DMA_CHAN3_FRAME_DONE_IRQ = 27,
    SIMCOP_DMA_CHAN2_FRAME_DONE_IRQ = 26,
    SIMCOP_DMA_CHAN1_FRAME_DONE_IRQ = 25,
    SIMCOP_DMA_CHAN0_FRAME_DONE_IRQ = 24,
    SIMCOP_DMA_CHAN7_BLOCK_DONE_IRQ = 23,
    SIMCOP_DMA_CHAN6_BLOCK_DONE_IRQ = 2,
    SIMCOP_DMA_CHAN5_BLOCK_DONE_IRQ = 21,
    SIMCOP_DMA_CHAN4_BLOCK_DONE_IRQ = 20,
    SIMCOP_DMA_CHAN3_BLOCK_DONE_IRQ = 19,
    SIMCOP_DMA_CHAN2_BLOCK_DONE_IRQ = 18,
    SIMCOP_DMA_CHAN1_BLOCK_DONE_IRQ = 17,
    SIMCOP_DMA_CHAN0_BLOCK_DONE_IRQ = 16,
    SIMCOP_DMA_RESERVED14_IRQ = 15,
    SIMCOP_DMA_RESERVED13_IRQ = 14,
    SIMCOP_DMA_RESERVED12_IRQ = 13,
    SIMCOP_DMA_RESERVED11_IRQ = 12,
    SIMCOP_DMA_RESERVED10_IRQ = 11,
    SIMCOP_DMA_RESERVED09_IRQ = 10,
    SIMCOP_DMA_RESERVED08_IRQ = 9,
    SIMCOP_DMA_RESERVED07_IRQ = 8,
    SIMCOP_DMA_RESERVED06_IRQ = 7,
    SIMCOP_DMA_RESERVED05_IRQ = 6,
    SIMCOP_DMA_RESERVED04_IRQ = 5,
    SIMCOP_DMA_RESERVED03_IRQ = 4,
    SIMCOP_DMA_RESERVED02_IRQ = 3,
    SIMCOP_DMA_RESERVED01_IRQ = 2,
    SIMCOP_DMA_RESERVED00_IRQ = 1,
    SIMCOP_DMA_OCPERR = 0
} simcop_dma_interrupt_id_t;

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
typedef void (*simcop_dma_callback_t) (CSL_Status status, uint32 arg1,
                                       void *arg2);

/* ================================================================ */
/**
 *Description:- struct for the registering the callback
 */
 /*================================================================== */
typedef struct {

    simcop_dma_callback_t callback_func;
    uint32 arg1;
    void *arg2;
    SIMCOP_DMA_CALLBACK_CONTEXT_T callback_context;

} simcop_dma_irq_t;

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

/* The fields are initialised in simcop_dma_common_init() */
typedef struct {
    Swi_Handle swi_handle;
    Hwi_Handle hwi_handle;
    simcop_dma_interrupt_bank_t default_int_bank;

} simcop_dma_common_dev_data_t;

/**************************************************************
 * FUNCTION Prototypes
 *******************************************************************/
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
CSL_Status simcop_dma_common_init();

/* ================================================================ */
/**
 *  Description:- this routine sets up the interrupt callback for the 
 *  desired interruptonly for the interrupt bank0, it does not enable the 
 *  interrupt
 *
 *  @param  simcop_dma_interrupt_id_t :- is the inpterrupt id for which the handler needs to be registered
 *			simcop_dma_callback_t:- is the callback function that needs to be registered
 *			arg1, arg2 are the params that the callback will be invoked with when interrupt occurs
 *  @return        CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_set_interrupt_handler                                               
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
CSL_Status simcop_dma_set_interrupt_handler(simcop_dma_interrupt_id_t int_id,
                                            simcop_dma_callback_t callback_func,
                                            uint32 arg1, void *arg2);

/* ================================================================ */
/**
 *  Description:- this routine unregisters the interrupt handler of the 
 *  specified interrupt source only for the interrupt bank0
 *
 *  @param int_id  simcop_dma_interrupt_id_t :- is the inpterrupt id for 
 *          which the handler needs to be unregistered
 *
 *  @return         CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_unset_interrupt_handler                                               
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
CSL_Status simcop_dma_unset_interrupt_handler(simcop_dma_interrupt_id_t int_id);

/* ================================================================ */
/**
 *  Description:- this routine enabled the ISP interrupt at the ISP level
 *
 *
 *
 *  @param   simcop_dma_interrupt_id_t  :- specifies the interrupt number
 *           simcop_dma_interrupt_bank_t :-
 *
 *  @return      CSL_Status
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
                                              bank_num);

/* ================================================================ */
/**
 *  Description:- disables the specified interrupt
 *  NOTE!!!!!!!!!   This is the exposed function to user the bank is selected to default 0
 *
 *  @param   simcop_dma_interrupt_id_t
 *
 *  @return     CSL_Status
 */
 /*================================================================== */
/* This is the exposed function to user the bank is selected to default 0 */
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
                                               bank_num);

/* ================================================================ */
/**
 *  Description:- This is a place holder currently for parsing the interrupt register and calling the callback
 *
 *
 *  @param   arg0, and arg1 will be the values while registering the SWI with BIOS
 *
 *  @return        
 */
 /*================================================================== */
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
void simcop_dma_swi_interrupt_handler(UArg arg0, UArg arg1);

/* ================================================================ */
/**
 *  Description:- Thie routine returns the sts bit specified.
 *
 *  @param   simcop_dma_interrupt_id_t :- specifies the interrupt number
 * uint32* :- pointer to hold the sts value .
 *  @return   CSL_Status
 */
 /*================================================================== */
/* ===================================================================
 *  @func     simcop_dma_get_int_sts                                               
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
CSL_Status simcop_dma_get_int_sts(simcop_dma_interrupt_id_t int_id,
                                  uint32 * sts_p);

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
                                         Interrupt_Handle_T * handle);

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
                                       Interrupt_Handle_T * handle);

/* ================================================================ */
/**
 *  Description:-this routine enables the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    simcop_dma_interrupt_id_t :- is the inpterrupt id which needs to enabled
 * simcop_dma_interrupt_bank_t:- specifies which bank's interrupt is enabled
 *  @return     CSL_Status
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
CSL_Status simcop_dma_enable_interrupt(simcop_dma_interrupt_id_t int_id);

/* ================================================================ */
/**
 *  Description:-this routine disabled the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    simcop_dma_interrupt_id_t :- is the inpterrupt id which needs to disabled
 * simcop_dma_interrupt_bank_t:- specifies which bank's interrupt is disabled
 *
 *  @return        CSL_Status
 */
 /*================================================================== */
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
CSL_Status simcop_dma_disable_interrupt(simcop_dma_interrupt_id_t int_id);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
