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

#ifndef ISP_COMMON
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define ISP_COMMON

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

#include <ti/psp/iss/hal/iss/isp/common/inc/isp_common_reg.h>
#include <ti/psp/iss/hal/iss/isp/isp5_utils/isp5_sys_types.h>
#include <ti/psp/iss/hal/iss/isp/isp5_utils/isp5_csl_utils.h>

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

typedef volatile CSL_ISP5Regs *isp_regs_ovly;

// #define ISP5_BASE_ADDRESS (0x50010000)
#define ISP5_BASE_ADDRESS (ISS_REGS_BASE_ADDR + 0x10000)

#define MAX_ISP_INTERRUPTS 32

typedef enum {
    ISP_CALLBACK_CONTEXT_SWI = 0,
    ISP_CALLBACK_CONTEXT_HWI = 1,
    ISP_CALLBACK_CONTEXT_MAX = 2
} ISP_CALLBACK_CONTEXT_T;

/* ================================================================ */
/**  Description:- Enum for slecting the interrupt bank
 */
 /*================================================================== */

typedef uint8 isp_interrupt_bank_t;

/* ================================================================ */
/**  Description:-enum for the possible interrupt id's
 */
 /*================================================================== */
typedef enum {

    /* Mapped according to spec */
    ISP_INT_MAX = 32,
    ISP_OCP_ERR_IRQ = 31,
    ISP_Reserved = 30,
    ISP_IPIPE_INT_DPC_RNEW1 = 29,
    ISP_IPIPE_INT_DPC_RNEW0 = 28,
    ISP_IPIPE_INT_DPC_INI = 27,
    ISP_Reserved0 = 26,
    ISP_IPIPE_INT_EOF = 25,
    ISP_H3A_INT_EOF = 24,
    ISP_RSZ_INT_EOF1 = 23,
    ISP_RSZ_INT_EOF0 = 22,
    ISP_Reserved1 = 21,
    ISP_Reserved2 = 20,
    ISP_Reserved3 = 19,
    ISP_RSZ_FIFO_IN_OVF = 18,
    ISP_RSZ_INT_CYC_RZB = 17,
    ISP_RSZ_INT_CYC_RZA = 16,
    ISP_RSZ_INT_DMA = 15,
    ISP_RSZ_INT_LAST_PIX = 14,
    ISP_RSZ_INT_REG = 13,
    ISP_H3A_INT = 12,
    ISP_AF_INT = 11,
    ISP_AEW_INT = 10,
    ISP_IPIPEIF_IRQ = 9,
    ISP_IPIPE_INT_HST = 8,
    ISP_IPIPE_INT_BSC = 7,
    ISP_IPIPE_INT_DMA = 6,
    ISP_IPIPE_INT_LAST_PIX = 5,
    ISP_IPIPE_INT_REG = 4,
    ISP_ISIF_INT_3 = 3,
    ISP_ISIF_INT_2 = 2,
    ISP_ISIF_INT_1 = 1,
    ISP_ISIF_INT_0 = 0
} isp_interrupt_id_t;

/* ================================================================ */
/**  Description:- structure for memory access control
  */
 /*================================================================== */
typedef struct {
    uint16 isif_read_interval;
    uint16 ipipeif_read_interval;
    uint16 h3a_write_interval;
} isp_memory_access_ctrl_t;

/* ================================================================ */
/**  Description:- enum for selecting input port
 */
 /*================================================================== */
/******* this is used to configure the VD_PULSE_EXT*****************/

typedef enum {
    ISP_INPUT_PORT_CSI2 = 1,
    ISP_INPUT_PORT_CCP2_PARALLEL = 0
} ISP_IP_PORT_T;

/* ================================================================ */
/**  Description:-enum for selecting clk polarity
 */
 /*================================================================== */
typedef enum {
    ISP_PIXEL_CLK_NORMAL = 0,
    ISP_PIXEL_CLK_INVERT = 1
} ISP_PIXEL_CLK_POL_T;

/* ================================================================ */
/**  Description:-enum for sync signal control
 */
 /*================================================================== */
typedef enum {
    ISP_SYNC_DISABLE = 0,
    ISP_SYNC_ENABLE = 1
} ISP_SYNC_CTRL_T;

/* ================================================================ */
/**  Description:- enum for postd write ctrl
 */
 /*================================================================== */
typedef enum {

    ISP_POSTED_WRT_DISABLE = 0,
    ISP_POSTED_WRT_ENABLE = 1
} ISP_POSTED_WRT_CTRL_T;

/* ================================================================ */
/**  Description:- enum for slecting sync clk
 */
 /*================================================================== */
typedef enum {
    ISP_PSYNC_CLK_MMR_CLK = 0,
    ISP_PSYNC_CLK_DMA_CLK = 1
} ISP_PSYNC_CLK_SEL_T;

/* ================================================================ */
/**
 *Description:- enum for slecting Video port / pattern genrator
 */
 /*================================================================== */
typedef enum {
    ISP_IP_MUX_VP = 0,                                     // Videop port
    ISP_IP_MUX_PG = 3                                      // Patterg
                                                           // generator
} ISP_IP_MUXSEL_T;

/* ================================================================ */
/**  Description:- enum for ctrl of red gamma table copying into blu and green tables
 */
 /*================================================================== */

typedef enum {

    ISP_COPY_RED_GAMMA_TBL_EN = 1,
    ISP_COPY_RED_GAMMA_TBL_DIS = 1
} ISP_COPY_RED_GAMMA_TBL_T;

/*********MEMORY ACCESS CONFIGURATION**********************************/

/* ================================================================ */
/**
 *Description:- enum for possible table access ctrl's
 */
 /*================================================================== */
typedef enum {

    ISP_ISIF_LIN_TB = 1,
    ISP_ISIF_LSC_TB0,
    ISP_ISIF_LSC_TB1,
    ISP_ISIF_DCLAMP,
    ISP_IPIPE_DPC_TB,
    ISP_IPIPE_GMM_TBB,
    ISP_IPIPE_GMM_TBG,
    ISP_IPIPE_GMM_TBR,
    ISP_IPIPE_YEE_TB,
    ISP_IPIPE_GBC_TB,
    ISP_IPIPE_D3L_TB0,
    ISP_IPIPE_D3L_TB1,
    ISP_IPIPE_D3L_TB2,
    ISP_IPIPE_D3L_TB3,
    ISP_IPIPE_HST_TB0,
    ISP_IPIPE_HST_TB1,
    ISP_IPIPE_HST_TB2,
    ISP_IPIPE_HST_TB3,
    ISP_IPIPE_BSC_TB0,
    ISP_IPIPE_BSC_TB1
} ISP_TABLE_ACCESS_T;

/* ================================================================ */
/**
 *Description:- enum for prioiry of table acess between cpu and dma
 */
 /*================================================================== */
typedef enum {
    ISP_MEM_ACCESS_MODULE = 0,                             // module has
                                                           // higher priority
    ISP_MEM_ACCESS_CPU = 1
} ISP_TABLE_ACCESS_PRIORITY_T;

/* ================================================================ */
/**
 *Description:-Bitmask for setting above mentioned fileds ; use the enum "ISP_MEM_ACCESS_T"
 *as shift value and "ISP_MEM_ACCESS_PRIORITY_T" as the value DEfault is all 0's
 */
 /*================================================================== */
typedef uint32 ISP_TABLE_ACCESS_CTRL_T;

/*************************************************************************/

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

/* ================================================================ */
/**
 *Description:- callback function prototype
 */
 /*================================================================== */
typedef void (*isp_callback_t) (ISP_RETURN status, uint32 arg1, void *arg2);

/* ================================================================ */
/**
 *Description:- struct for the registering the callback
 */
 /*================================================================== */
typedef struct {

    isp_callback_t callback_func;
    uint32 arg1;
    void *arg2;
    ISP_CALLBACK_CONTEXT_T callback_context;

} isp_irq_t;

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

/* ================================================================ */
/**
 *Description:- main configuration structure for isp memory aceess, port ctrl etc
 */
 /*================================================================== */

typedef struct {
    ISP_IP_PORT_T ip_port;

    isp_memory_access_ctrl_t mem_accsess_ctrl;
    ISP_PIXEL_CLK_POL_T pixclk_pol;
    ISP_SYNC_CTRL_T sync_ctrl;
    ISP_PSYNC_CLK_SEL_T psync_clk_sel;
    uint8 max_vbusm_cids;
    uint8 vbusm_priority;
    ISP_POSTED_WRT_CTRL_T posted_wrt_ctrl;
    ISP_IP_MUXSEL_T ip_mux_sel;
    ISP_TABLE_ACCESS_CTRL_T table_access_ctrl;

} isp_common_cfg_t;

/* ================================================================ */
/**
 *Description:- structure for holding pointers to swi, hwi, handles, other fields can be ADDED as necessary
 */
 /*================================================================== */

/* The fields are initialised in isp_common_init() */
typedef struct {
    Swi_Handle swi_handle;
    Hwi_Handle hwi_handle;
    isp_interrupt_bank_t default_int_bank;

} isp_common_dev_data_t;

typedef enum {

    ISP_MSTANDBY_ASSERT_ENABLE = 1,
    ISP_MSTANDBY_ASSERT_DISABLE = 0,                       /* de-assert the
                                                            * standby and
                                                            * enable the
                                                            * module */

} ISP_MSTANDBY_ASSERT_T;

typedef enum {

    ISP_VD_PULSE_EXT_DISABLED = 0,                         /* ccp2 */
    ISP_VD_PULSE_EXT_ENABLED = 1                           /* csi2 */
} ISP_VD_PULSE_EXT_T;

/**************************************************************
  FUNCTION Prototypes
 *******************************************************************/

/* ================================================================ */
/**
 *  Description:- this routine sets up the interrupt callback for the desired interruptonly for the interrupt bank0, it does not enable the interrupt
 *
 *
 *  @param   isp_interrupt_id_t :- is the inpterrupt id for which the handler needs to be registered
 *          isp_callback_t:- is the callback function that needs to be registered
 *          arg1, arg2 are the params that the callback will be invoked with when interrupt occurs
 *  @return        ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     isp_common_set_interrupt_handler
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
ISP_RETURN isp_common_set_interrupt_handler(isp_interrupt_id_t int_id,
                                            isp_callback_t callback_func,
                                            uint32 arg1, void *arg2);

/* ================================================================ */
/**
 *  Description:- this routine unregisters the interrupt handler of the specified interrupt source only for the interrupt bank0
 *
 *
 *  @param int_id  isp_interrupt_id_t  int_id:- is the inpterrupt id for which the handler needs to be unregistered

 *  @return         ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     isp_common_unset_interrupt_handler
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
ISP_RETURN isp_common_unset_interrupt_handler(isp_interrupt_id_t int_id);

/* ================================================================ */
/**
 *  Description:- this routine configurees the read intervals allocated to diff sub -modules of isp
 *
 *
 *  @param   :- isp_memory_access_ctrl_t* pointer to the cfg struct

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
ISP_RETURN isp_common_read_interface_config(isp_memory_access_ctrl_t * cfg);

/* ================================================================ */
/**
 *  Description:- this routine enabled the ISP interrupt at the ISP level
 *
 *
 *
 *  @param   isp_interrupt_id_t :- specifies the interrupt number

 *  @return      ISP_RETURN
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
                                       isp_interrupt_bank_t bank_num);

/* ================================================================ */
/**
 *  Description:- disables the specified interrupt
 *  NOTE!!!!!!!!!   This is the exposed function to user the bank is selected to default 0
 *
 *  @param   isp_interrupt_id_t

 *  @return     ISP_RETURN
 */
 /*================================================================== */
/* This is the exposed function to user the bank is selected to default 0 */
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
                                        isp_interrupt_bank_t bank_num);

/* ================================================================ */
/**
 *  Description:-disables the specified module's clock
 *
 *
 *  @param   ISP_MODULE_CLK_T:- the module name

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
ISP_RETURN isp_common_disable_clk(ISP_MODULE_CLK_T module_id);

/* ================================================================ */
/**
 *  Description:-enables the clock of required module in isp
 *
 *
 *  @param   ISP_MODULE_CLK_T:- the module name

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
ISP_RETURN isp_common_enable_clk(ISP_MODULE_CLK_T module_id);

/* ================================================================ */
/**
 *  Description:- This is a place holder currently for parsing the interrupt register and calling the callback
 *
 *
 *  @param   arg0, and arg1 will be the values while registering the SWI with BIOS

 *  @return        ISP_RETURN
 */
 /*================================================================== */
/* ===================================================================
 *  @func     isp_common_swi_interrupt_handler
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
void isp_common_swi_interrupt_handler(UArg arg0, UArg arg1);

/* ================================================================ */
/**
 *  Description:- configures the ip port of ISP
 *
 *
 *  @param   ISP_IP_PORT_T:- selects between CSI2 and CCP2

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
ISP_RETURN isp_common_ip_port_cfg(ISP_IP_PORT_T ip_port_cfg);

/* ================================================================ */
/**
 *  Description:- polarity of the clocks is selected in this routine
 *
 *
 *  @param   ISP_PIXEL_CLK_POL_T:-selects the polarity of the clocks, nornmal or inverted

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
ISP_RETURN isp_common_pixclk_polarity_cfg(ISP_PIXEL_CLK_POL_T pixclk_pol);

/* ================================================================ */
/**
 *  Description:-cfg's sync ctrl
 *
 *
 *  @param   ISP_SYNC_CTRL_T:- enable/disable

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
ISP_RETURN isp_common_sync_ctrl_cfg(ISP_SYNC_CTRL_T sync_ctrl);

/* ================================================================ */
/**
 *  Description:-configures the pixel clock sync
 *
 *
 *  @param   ISP_PSYNC_CLK_SEL_T:-MMR/DMA clk

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
ISP_RETURN isp_common_pixclk_sync_enable(ISP_PSYNC_CLK_SEL_T psync_clk_sel);

/* ================================================================ */
/**
 *  Description:- controls the write operation of isp.
 *
 *
 *  @param   ISP_POSTED_WRT_CTRL_T:- posted / nonposted write param

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
ISP_RETURN isp_common_posted_write_cfg(ISP_POSTED_WRT_CTRL_T wrt_ctrl);

/* ================================================================ */
/**
 *  Description:-controls the ip from Video port or form pattern genrator
 *
 *
 *  @param   ISP_IP_MUXSEL_T:- enumeration of the 2 possibilities

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
ISP_RETURN isp_common_ip_mux_sel(ISP_IP_MUXSEL_T ip_mux_sel);

/* ================================================================ */
/**
 *  Description:- Configures the priority of the tables access by different sub modules within isp
 *
 *
 *  @param   ISP_TABLE_ACCESS_CTRL_T:- the control parameter

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
                                       table_access_ctrl);

/* ================================================================ */
/**
 *  Description:-configures the VBUSM id's
 *
 *
 *  @param   uint8:- specifies the id's

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
ISP_RETURN isp_common_vbusm_id_cfg(uint8 max_vbusm_cids);

/* ================================================================ */
/**
 *  Description:- configure vbusm priorities
 *
 *
 *  @param   uint8:- specifies the priority

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
ISP_RETURN isp_common_vbusm_priority_cfg(uint8 vbusm_priority);

/* ================================================================ */
/**
 *  Description:- configures the memmory acces ctrl by different sub modules of isp
 *
 *
 *  @param:- isp_memory_access_ctrl_t:- cfg struct    for memory access ctrl

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
                                             mem_accsess_ctrl);

/* ================================================================ */
/**
 *  Description:- this routine does the cfg of the read intervals, posted write,i/p port, clk polarities etc of  isp
 *
 *
 *  @param   :- isp_common_cfg_t* :- pointer to the cfg structure

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
ISP_RETURN isp_common_cfg(isp_common_cfg_t * cfg);

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
ISP_RETURN isp_enable_red_gamma_table_cpy(ISP_COPY_RED_GAMMA_TBL_T enable);

/* ================================================================ */
/**
 *  Description:- this routine enables the appropritae DMA ctrl for copying tables and reading histogram data
 *  NOTE !!!!!!!!!!!!the implemmentation is not complete
 *
 *  @param

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
ISP_RETURN isp_common_enable_dma(ISP_DMA_ID_T dma_id, ISP_DMA_CTRL_T enable);

/* ================================================================ */
/**
 *  Description:-this routine needs to be called before calling any isp submodule's inits, like ipipe_init() etc
 *                  this routine does the basic intialisation of isp, it disables all interrupts crears;swi's and hwi's,
 *                     does a clean reset of ISP etc,and registers interrupt handlers with ISS interrupt manager
 *!NOTE this is not complete
 *  @param   none

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
ISP_RETURN isp_common_init();

/* ================================================================ */
/**
 *  Description:- Thie routine returns the sts bit specified.
 *
 *  @param   isp_interrupt_id_t :- specifies the interrupt number
            uint32* :- pointer to hold the sts value .
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
ISP_RETURN isp_common_get_int_sts(isp_interrupt_id_t int_id, uint32 * sts_p);

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
uint32 isp_common_get_all_int_sts(void);

/* ================================================================ */
/**
 *  isp_unhook_int_handler()
 *  Description :- This routine de-links the argument "handle " froma priority linked list for the  interrupt
 *               specified by the argument "int_id". the handle is de-linked from the swi list or hwi list as specified
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
                                  Interrupt_Handle_T * handle);

/* ================================================================ */
/**
 *  isp_hook_int_handler()
 *  Description :- This routine links the argument "handle " to a priority linked list for the  interrupt
 *               specified by the argument "int_id". the handle is linked to the swi list or hwi list as specified
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
                                Interrupt_Handle_T * handle);

/* ================================================================ */
/**
 *  Description:-this routine enables the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    isp_interrupt_id_t :- is the inpterrupt id which needs to enabled
isp_interrupt_bank_t:- specifies which bank's interrupt is enabled
 *  @return     ISP_RETURN
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
ISP_RETURN isp_enable_interrupt(isp_interrupt_id_t int_id);

/* ================================================================ */
/**
 *  Description:-this routine disabled the specified interrupt
 *  NOTE!!!this is intended to be called internally only
 *
 *  @param    isp_interrupt_id_t :- is the inpterrupt id which needs to disabled
isp_interrupt_bank_t:- specifies which bank's interrupt is disabled

 *  @return        ISP_RETURN
 */
 /*================================================================== */
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
ISP_RETURN isp_disable_interrupt(isp_interrupt_id_t int_id);

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
ISP_RETURN isp_common_assert_mstandby(ISP_MSTANDBY_ASSERT_T assert_val);

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
ISP_RETURN isp_common_cfg_VD_PULSE_EXT(ISP_VD_PULSE_EXT_T sel);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
