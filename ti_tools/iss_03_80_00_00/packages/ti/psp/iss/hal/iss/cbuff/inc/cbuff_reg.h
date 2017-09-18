/** ==================================================================
 *  @file   cbuff_reg.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/cbuff/inc/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*****************************************************************************/
/* */
/* Copyright 2008-09 by Texas Instruments Incorporated. All rights */
/* reserved. Property of Texas Instruments Incorporated. Restricted */
/* rights to use, duplicate or disclose this code are granted */
/* through contract.  */
/* "Texas Instruments OMAP(tm) Platform Software" */
/* */
/*****************************************************************************/

/*****************************************************************************/
/* File Name :Cbuf_reg.h */
/* */
/* Description :Program file containing the register level values for Cbuf */
/* */
/* Created : Ducati Team (OMAP4) */
/* */
/* @rev 1.0 */
/*========================================================================
*!
*! Revision History
*! ===================================*/

#ifndef _CSLR_CBUFF_REG_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_CBUFF_REG_H

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "cbuff_sys_types.h"

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

#define CBUFF_REG_BASE_ADDRESS (ISS_REGS_BASE_ADDR + 0x00001800)

#define CBUFF_FRAG_ADDR_0_IDX        0
#define CBUFF_FRAG_ADDR_1_IDX        1
#define CBUFF_FRAG_ADDR_2_IDX        2
#define CBUFF_FRAG_ADDR_3_IDX        3
#define CBUFF_FRAG_ADDR_4_IDX        4
#define CBUFF_FRAG_ADDR_5_IDX        5
#define CBUFF_FRAG_ADDR_6_IDX        6
#define CBUFF_FRAG_ADDR_7_IDX        7
#define CBUFF_FRAG_ADDR_8_IDX        8
#define CBUFF_FRAG_ADDR_9_IDX        9
#define CBUFF_FRAG_ADDR_10_IDX       10
#define CBUFF_FRAG_ADDR_11_IDX       11
#define CBUFF_FRAG_ADDR_12_IDX       12
#define CBUFF_FRAG_ADDR_13_IDX       13
#define CBUFF_FRAG_ADDR_14_IDX       14
#define CBUFF_FRAG_ADDR_15_IDX       15
#define CBUFF_FRAG_ADDR_NUM          16

typedef union {
    struct {
        volatile uint32 MINOR:6;
        volatile uint32 CUSTOM:2;
        volatile uint32 X_MAJOR:3;
        volatile uint32 R_RTL_MASK:5;
        volatile uint32 FUNC_MASK:12;
        volatile uint32 reserved:2;
        volatile uint32 SCHEME_MASK:2;
    } b;
    volatile uint32 r32;
} CBUFF_HL_REVISION_TYPE;

typedef union {
    struct {
        volatile uint32 ENABLE_FRAGMENTATION:1;
        volatile uint32 NUMBER_OF_CONTEXT:2;
        volatile uint32 padd:29;
    } b;
    volatile uint32 r32;
} CBUFF_HL_HWINFO_TYPE;

typedef union {
    struct {
        volatile uint32 SOFTRESET:1;                       // 1 - RESET CBUFF
        volatile uint32 RESERVED:1;
        volatile uint32 IDLEMODE:2;                        // 0 - Force-idle; 
                                                           // 1 - No-idle
                                                           // mode; 2 -
                                                           // Smart-idle mode
        volatile uint32 padd:28;
    } b;
    volatile uint32 r32;
} CBUFF_HL_SYSCONFIG_TYPE;

typedef union {
    struct {
        volatile uint32 OCP_ERR:1;
        volatile uint32 reserved:7;
        volatile uint32 CTX0_READY:1;
        volatile uint32 CTX1_READY:1;
        volatile uint32 CTX2_READY:1;
        volatile uint32 CTX3_READY:1;
        volatile uint32 CTX4_READY:1;
        volatile uint32 CTX5_READY:1;
        volatile uint32 CTX6_READY:1;
        volatile uint32 CTX7_READY:1;
        volatile uint32 CTX0_INVALID:1;
        volatile uint32 CTX1_INVALID:1;
        volatile uint32 CTX2_INVALID:1;
        volatile uint32 CTX3_INVALID:1;
        volatile uint32 CTX4_INVALID:1;
        volatile uint32 CTX5_INVALID:1;
        volatile uint32 CTX6_INVALID:1;
        volatile uint32 CTX7_INVALID:1;
        volatile uint32 CTX0_OVERFLOW:1;
        volatile uint32 CTX1_OVERFLOW:1;
        volatile uint32 CTX2_OVERFLOW:1;
        volatile uint32 CTX3_OVERFLOW:1;
        volatile uint32 CTX4_OVERFLOW:1;
        volatile uint32 CTX5_OVERFLOW:1;
        volatile uint32 CTX6_OVERFLOW:1;
        volatile uint32 CTX7_OVERFLOW:1;
    } b;
    volatile uint32 r32;

} CBUFF_HL_IRQ_TYPE;

typedef union {
    struct {
        volatile uint32 CB_ENABLE:1;                       // 1 enable
        volatile uint32 CB_MODE:2;                         // 0 - Write mode; 
                                                           // 1 - Read mode;
                                                           // 2 - Read/Write
                                                           // mode
        volatile uint32 reserved:1;
        volatile uint32 BCF:4;                             // 0: Control loop 
                                                           // disabled; 1-15: 
                                                           // The control
                                                           // feedback loop
                                                           // enabled 
        volatile uint32 WCOUNT:2;                          // 0: 2 windows;
                                                           // 1: 4 windows;
                                                           // 2: 8 windows;
                                                           // 3: 16 windows; 
        volatile uint32 DONE:1;
        volatile uint32 TILERMODE:1;
        volatile uint32 padd:20;
    } b;
    volatile uint32 r32;

} CBUFF_HL_CTX_CTRL_TYPE;

typedef union {
    struct {
        volatile uint32 WB:4;
        volatile uint32 reserved:4;
        volatile uint32 WA:4;
        volatile uint32 padd:20;
    } b;
    volatile uint32 r32;
} CBUFF_HL_CTX_STATUS;

typedef struct {
    volatile CBUFF_HL_CTX_CTRL_TYPE CTX_CTRL;              // Context control 
                                                           // register 
    volatile uint32 VA_START;                              // Start address
                                                           // of the virtual
                                                           // space managed
                                                           // by the context 
    volatile uint32 VA_END;                                // End address of
                                                           // the virtual
                                                           // space managed
                                                           // by the context 
    volatile uint32 WINDOWSIZE;                            // Defines the
                                                           // size of a
                                                           // window for the
                                                           // context 
    volatile uint32 THRESHOLD_F;                           // Threshold value 
                                                           // used to check
                                                           // if a write
                                                           // window is full
                                                           // for the context
    volatile uint32 THRESHOLD_S;                           // Threshold value 
                                                           // used to control 
                                                           // the BCF
                                                           // synchronization 
                                                           // mechanism for
                                                           // the context 
    volatile CBUFF_HL_CTX_STATUS STATUS;                   // Status register 
                                                           // for the context
    volatile uint32 PHY_ADDR;                              // Physical
                                                           // address
                                                           // register for
                                                           // the context
} CBUFF_CTX_TYPE;

typedef struct {

    volatile CBUFF_HL_REVISION_TYPE REVISION;              // Used by
                                                           // software to
                                                           // track features, 
                                                           // bugs, and
                                                           // compatibility
    volatile CBUFF_HL_HWINFO_TYPE HWINFO;                  // Information
                                                           // about the IP
                                                           // module's
                                                           // hardware
                                                           // configuration. 

    volatile uint32 reserve[2];

    volatile CBUFF_HL_SYSCONFIG_TYPE SYSCONFIG;            // Configuration
                                                           // of the local
                                                           // target state
                                                           // management mode 
                                                           // i.e SOFT RESET
                                                           // OR IDLE MODE

    volatile uint32 reserve1[2];

    volatile uint32 CBUFF_HL_IRQ_EOI;                      // Software End Of 
                                                           // Interrupt (EOI) 
                                                           // control.

    volatile CBUFF_HL_IRQ_TYPE IRQ_RAW;                    // Per-event raw
                                                           // interrupt
                                                           // status vector 

    volatile CBUFF_HL_IRQ_TYPE IRQ_STATUS;                 // Per-event
                                                           // "enabled"
                                                           // interrupt
                                                           // status vector.

    volatile CBUFF_HL_IRQ_TYPE IRQ_SET;                    // Per-event
                                                           // interrupt
                                                           // enable bit
                                                           // vector

    volatile CBUFF_HL_IRQ_TYPE IRQ_CLR;                    // Per-event
                                                           // interrupt
                                                           // enable bit
                                                           // vector, line #0 
                                                           // 

    volatile uint32 reserve2[20];

    volatile uint32 CBUFF_FRAG_ADDR[16];                   // CBUFF_FRAG_ADDR_0 
                                                           // TO
                                                           // CBUFF_FRAG_ADDR_15 
                                                           // Start address
                                                           // of the physical 
                                                           // buffer of the
                                                           // circular buffer 
                                                           // context 0.
    // This register only exists when fragmentation support is enabled.*/

    volatile uint32 reserve3[16];

    volatile CBUFF_CTX_TYPE ctx[8];                        // Contexts from 0 
                                                           // to 8

} CBUFF_REGS;

/* ======================================================================= */
/* This is a structure description of CSL_Cbuf_Regs object
 * 
 * 
 * * ======================================================================= */

typedef struct {
    uint32 CBUFF_HL_REVISION;
    /* Used by software to track features, bugs, and compatibility */
    uint32 CBUFF_HL_HWINFO;
    /* Information about the IP module's hardware configuration. */

    volatile uint32 reserve[2];

    uint32 CBUFF_HL_SYSCONFIG;
    /* Configuration of the local target state management mode i.e SOFT RESET 
     * OR IDLE MODE */

    volatile uint32 reserve1[2];

    uint32 CBUFF_HL_IRQ_EOI;
    /* Software End Of Interrupt (EOI) control.  */
    uint32 CBUFF_HL_IRQSTATUS_RAW;
    /* Per-event raw interrupt status vector */

    uint32 CBUFF_HL_IRQSTATUS;
    /* Per-event "enabled" interrupt status vector. */

    uint32 CBUFF_HL_IRQENABLE_SET;
    /* Per-event interrupt enable bit vector */

    uint32 CBUFF_HL_IRQENABLE_CLR;
    /* Per-event interrupt enable bit vector, line #0 */

    volatile uint32 reserve2[20];

    uint32 CBUFF_FRAG_ADDR_0;
    uint32 CBUFF_FRAG_ADDR_1;
    uint32 CBUFF_FRAG_ADDR_2;
    uint32 CBUFF_FRAG_ADDR_3;
    uint32 CBUFF_FRAG_ADDR_4;
    uint32 CBUFF_FRAG_ADDR_5;
    uint32 CBUFF_FRAG_ADDR_6;
    uint32 CBUFF_FRAG_ADDR_7;
    uint32 CBUFF_FRAG_ADDR_8;
    uint32 CBUFF_FRAG_ADDR_9;
    uint32 CBUFF_FRAG_ADDR_10;
    uint32 CBUFF_FRAG_ADDR_11;
    uint32 CBUFF_FRAG_ADDR_12;
    uint32 CBUFF_FRAG_ADDR_13;
    uint32 CBUFF_FRAG_ADDR_14;
    uint32 CBUFF_FRAG_ADDR_15;
    /* CBUFF_FRAG_ADDR_0 TO CBUFF_FRAG_ADDR_15 Start address of the physical
     * buffer of the circular buffer context 0. This register only exists
     * when fragmentation support is enabled. */

    volatile uint32 reserve3[16];
    uint32 CBUFF_CTX_CTRL_0;
    /* Context 0 control register */
    uint32 CBUFF_CTX_START_0;
    /* Start address of the virtual space managed by the context 0 */
    uint32 CBUFF_CTX_END_0;
    /* End address of the virtual space managed by the context 0 */
    uint32 CBUFF_CTX_WINDOWSIZE_0;
    /* Defines the size of a window for context 0 */
    uint32 CBUFF_CTX_THRESHOLD_F_0;
    /* Threshold value used to check if a write window is full for context 0 */
    uint32 CBUFF_CTX_THRESHOLD_S_0;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 0 */
    uint32 CBUFF_CTX_STATUS_0;
    /* Status register for context 0 */
    uint32 CBUFF_CTX_PHY_0;
    /* Physical address register for context 0 */

    uint32 CBUFF_CTX_CTRL_1;
    /* Context 1 control register */
    uint32 CBUFF_CTX_START_1;
    /* Start address of the virtual space managed by the context 1 */
    uint32 CBUFF_CTX_END_1;
    /* End address of the virtual space managed by the context 1 */
    uint32 CBUFF_CTX_WINDOWSIZE_1;
    /* Defines the size of a window for context 1 */
    uint32 CBUFF_CTX_THRESHOLD_F_1;
    /* Threshold value used to check if a write window is full for context 1 */
    uint32 CBUFF_CTX_THRESHOLD_S_1;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 1 */
    uint32 CBUFF_CTX_STATUS_1;
    /* Status register for context 1 */
    uint32 CBUFF_CTX_PHY_1;
    /* Physical address register for context 1 */

    uint32 CBUFF_CTX_CTRL_2;
    /* Context 2 control register */
    uint32 CBUFF_CTX_START_2;
    /* Start address of the virtual space managed by the context 2 */
    uint32 CBUFF_CTX_END_2;
    /* End address of the virtual space managed by the context 2 */
    uint32 CBUFF_CTX_WINDOWSIZE_2;
    /* Defines the size of a window for context 2 */
    uint32 CBUFF_CTX_THRESHOLD_F_2;
    /* Threshold value used to check if a write window is full for context 2 */
    uint32 CBUFF_CTX_THRESHOLD_S_2;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 2 */
    uint32 CBUFF_CTX_STATUS_2;
    /* Status register for context 2 */
    uint32 CBUFF_CTX_PHY_2;
    /* Physical address register for context 2 */

    uint32 CBUFF_CTX_CTRL_3;
    /* Context 3 control register */
    uint32 CBUFF_CTX_START_3;
    /* Start address of the virtual space managed by the context 3 */
    uint32 CBUFF_CTX_END_3;
    /* End address of the virtual space managed by the context 3 */
    uint32 CBUFF_CTX_WINDOWSIZE_3;
    /* Defines the size of a window for context 3 */
    uint32 CBUFF_CTX_THRESHOLD_F_3;
    /* Threshold value used to check if a write window is full for context 3 */
    uint32 CBUFF_CTX_THRESHOLD_S_3;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 3 */
    uint32 CBUFF_CTX_STATUS_3;
    /* Status register for context 3 */
    uint32 CBUFF_CTX_PHY_3;
    /* Physical address register for context 3 */

    uint32 CBUFF_CTX_CTRL_4;
    /* Context 4 control register */
    uint32 CBUFF_CTX_START_4;
    /* Start address of the virtual space managed by the context 4 */
    uint32 CBUFF_CTX_END_4;
    /* End address of the virtual space managed by the context 4 */
    uint32 CBUFF_CTX_WINDOWSIZE_4;
    /* Defines the size of a window for context 4 */
    uint32 CBUFF_CTX_THRESHOLD_F_4;
    /* Threshold value used to check if a write window is full for context 4 */
    uint32 CBUFF_CTX_THRESHOLD_S_4;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 4 */
    uint32 CBUFF_CTX_STATUS_4;
    /* Status register for context 4 */
    uint32 CBUFF_CTX_PHY_4;
    /* Physical address register for context 4 */

    uint32 CBUFF_CTX_CTRL_5;
    /* Context 5 control register */
    uint32 CBUFF_CTX_START_5;
    /* Start address of the virtual space managed by the context 5 */
    uint32 CBUFF_CTX_END_5;
    /* End address of the virtual space managed by the context 5 */
    uint32 CBUFF_CTX_WINDOWSIZE_5;
    /* Defines the size of a window for context 5 */
    uint32 CBUFF_CTX_THRESHOLD_F_5;
    /* Threshold value used to check if a write window is full for context 5 */
    uint32 CBUFF_CTX_THRESHOLD_S_5;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 5 */
    uint32 CBUFF_CTX_STATUS_5;
    /* Status register for context 5 */
    uint32 CBUFF_CTX_PHY_5;
    /* Physical address register for context 5 */

    uint32 CBUFF_CTX_CTRL_6;
    /* Context 6 control register */
    uint32 CBUFF_CTX_START_6;
    /* Start address of the virtual space managed by the context 6 */
    uint32 CBUFF_CTX_END_6;
    /* End address of the virtual space managed by the context 6 */
    uint32 CBUFF_CTX_WINDOWSIZE_6;
    /* Defines the size of a window for context 6 */
    uint32 CBUFF_CTX_THRESHOLD_F_6;
    /* Threshold value used to check if a write window is full for context 6 */
    uint32 CBUFF_CTX_THRESHOLD_S_6;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 6 */
    uint32 CBUFF_CTX_STATUS_6;
    /* Status register for context 6 */
    uint32 CBUFF_CTX_PHY_6;
    /* Physical address register for context 6 */

    uint32 CBUFF_CTX_CTRL_7;
    /* Context 7 control register */
    uint32 CBUFF_CTX_START_7;
    /* Start address of the virtual space managed by the context 7 */
    uint32 CBUFF_CTX_END_7;
    /* End address of the virtual space managed by the context 7 */
    uint32 CBUFF_CTX_WINDOWSIZE_7;
    /* Defines the size of a window for context 7 */
    uint32 CBUFF_CTX_THRESHOLD_F_7;
    /* Threshold value used to check if a write window is full for context 7 */
    uint32 CBUFF_CTX_THRESHOLD_S_7;
    /* Threshold value used to control the BCF synchronization mechanism for
     * context 7 */
    uint32 CBUFF_CTX_STATUS_7;
    /* Status register for context 7 */
    uint32 CBUFF_CTX_PHY_7;
    /* Physical address register for context 7 */

} CSL_Cbuff_Regs;

/* -------------------- CBUFF HL REVISION -------------------------- */

#define CSL_CBUFF_HL_REVISION_Y_MINOR_MASK (0X0000003Fu)
#define CSL_CBUFF_HL_REVISION_Y_MINOR_SHIFT (0X00000000u)
#define CSL_CBUFF_HL_REVISION_Y_MINOR_RESETVAL (0X00000000u)

#define CSL_CBUFF_HL_REVISION_CUSTOM_MASK (0X000000C0u)
#define CSL_CBUFF_HL_REVISION_CUSTOM_SHIFT (0X00000006u)
#define CSL_CBUFF_HL_REVISION_CUSTOM_RESETVAL (0X00000000u)

#define CSL_CBUFF_HL_REVISION_X_MAJOR_MASK (0X00000700u)
#define CSL_CBUFF_HL_REVISION_X_MAJOR_SHIFT (0X00000008u)
#define CSL_CBUFF_HL_REVISION_X_MAJOR_RESETVAL (0X00000002u)

#define CSL_CBUFF_HL_REVISION_R_RTL_MASK (0X0000F800u)
#define CSL_CBUFF_HL_REVISION_R_RTL_SHIFT (0X0000000Bu)
#define CSL_CBUFF_HL_REVISION_R_RTL_RESETVAL (0X00000000u)

#define CSL_CBUFF_HL_REVISION_FUNC_MASK (0X0FFF0000u)
#define CSL_CBUFF_HL_REVISION_FUNC_SHIFT (0X00000010u)
#define CSL_CBUFF_HL_REVISION_FUNC_RESETVAL (0X00000000u)

#define CSL_CBUFF_HL_REVISION_SCHEME_MASK (0XC0000000u)
#define CSL_CBUFF_HL_REVISION_SCHEME_SHIFT (0X0000001Eu)
#define CSL_CBUFF_HL_REVISION_SCHEME_RESETVAL (0X00000002u)

/* -------------------- CBUFF HL HWINFO -------------------------- */

#define CSL_CBUFF_HL_HWINFO__CONTEXTS_MASK (0X00000006u)
#define CSL_CBUFF_HL_HWINFO__CONTEXTS_SHIFT (0X00000001u)

#define CSL_CBUFF_HL_HWINFO_ENABLE_FRAGMENTATION_MASK (0X00000001u)
#define CSL_CBUFF_HL_HWINFO_ENABLE_FRAGMENTATION_SHIFT (0X00000000u)

/* -------------------- CBUFF HL SYSCONFIG -------------------------- */

#define CSL_CBUFF_HL_SYSCONFIG_SOFTRESET_MASK (0X00000001u)
#define CSL_CBUFF_HL_SYSCONFIG_SOFTRESET_SHIFT (0X00000000u)

#define CSL_CBUFF_HL_SYSCONFIG_IDLEMODE_MASK (0X0000000Cu)
#define CSL_CBUFF_HL_SYSCONFIG_IDLEMODE_SHIFT (0X00000002u)

/* -------------------- CBUFF HL IRQ EOI -------------------------- */

#define CSL_CBUFF_HL_IRQ_EOI_LINE_NUMBER_MASK (0X00000001u)
#define CSL_CBUFF_HL_IRQ_EOI_LINE_NUMBER_SHIFT (0X00000000u)

/* -------------------- CBUFF HL IRQ RAW -------------------------- */

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_OCP_ERR_MASK (0X00000001u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_OCP_ERR_SHIFT (0X00000000u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_READY_MASK (0X00000100u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_READY_SHIFT (0X00000008u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX1_READY_MASK (0X00000200u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX1_READY_SHIFT (0X00000009u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX2_READY_MASK (0X00000400u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX2_READY_SHIFT (0X0000000Au)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX3_READY_MASK (0X00000800u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX3_READY_SHIFT (0X0000000Bu)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX4_READY_MASK (0X00001000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX4_READY_SHIFT (0X0000000Cu)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX5_READY_MASK (0X00002000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX5_READY_SHIFT (0X0000000Du)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX6_READY_MASK (0X00004000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX6_READY_SHIFT (0X0000000Eu)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX7_READY_MASK (0X00008000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX7_READY_SHIFT (0X0000000Fu)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_INVALID_MASK (0X00010000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_INVALID_SHIFT (0X00000010u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX1_INVALID_MASK (0X00020000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX1_INVALID_SHIFT (0X00000011u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX2_INVALID_MASK (0X00040000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX2_INVALID_SHIFT (0X00000012u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX3_INVALID_MASK (0X00080000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX3_INVALID_SHIFT (0X00000013u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX4_INVALID_MASK (0X00100000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX4_INVALID_SHIFT (0X00000014u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX5_INVALID_MASK (0X00200000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX5_INVALID_SHIFT (0X00000015u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX6_INVALID_MASK (0X00400000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX6_INVALID_SHIFT (0X00000016u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX7_INVALID_MASK (0X00800000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX7_INVALID_SHIFT (0X00000017u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_OVR_MASK (0X01000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX0_OVRSHIFT (0X00000018u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX1_OVR_MASK (0X02000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX1_OVRSHIFT (0X00000019u)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX2_OVR_MASK (0X04000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX2_OVRSHIFT (0X0000001Au)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX3_OVR_MASK (0X08000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX3_OVRSHIFT (0X0000001Bu)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX4_OVR_MASK (0X10000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX4_OVRSHIFT (0X0000001Cu)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX5_OVR_MASK (0X20000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX5_OVRSHIFT (0X0000001Du)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX6_OVR_MASK (0X40000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX6_OVRSHIFT (0X0000001Eu)

#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX7_OVR_MASK (0X80000000u)
#define CSL_CBUFF_HL_IRQSTATUS_RAW_IRQ_CTX7_OVRSHIFT (0X0000001Fu)

/* -------------------- CBUFF HL IRQ -------------------------- */

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_OCP_ERR_MASK (0X00000001u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_OCP_ERR_SHIFT (0X00000000u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_READY_MASK (0X00000100u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_READY_SHIFT (0X00000008u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX1_READY_MASK (0X00000200u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX1_READY_SHIFT (0X00000009u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX2_READY_MASK (0X00000400u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX2_READY_SHIFT (0X0000000Au)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX3_READY_MASK (0X00000800u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX3_READY_SHIFT (0X0000000Bu)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX4_READY_MASK (0X00001000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX4_READY_SHIFT (0X0000000Cu)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX5_READY_MASK (0X00002000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX5_READY_SHIFT (0X0000000Du)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX6_READY_MASK (0X00004000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX6_READY_SHIFT (0X0000000Eu)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX7_READY_MASK (0X00008000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX7_READY_SHIFT (0X0000000Fu)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_INVALID_MASK (0X00010000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_INVALID_SHIFT (0X00000010u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX1_INVALID_MASK (0X00020000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX1_INVALID_SHIFT (0X00000011u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX2_INVALID_MASK (0X00040000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX2_INVALID_SHIFT (0X00000012u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX3_INVALID_MASK (0X00080000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX3_INVALID_SHIFT (0X00000013u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX4_INVALID_MASK (0X00100000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX4_INVALID_SHIFT (0X00000014u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX5_INVALID_MASK (0X00200000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX5_INVALID_SHIFT (0X00000015u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX6_INVALID_MASK (0X00400000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX6_INVALID_SHIFT (0X00000016u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX7_INVALID_MASK (0X00800000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX7_INVALID_SHIFT (0X00000017u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_OVR_MASK (0X01000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX0_OVR_SHIFT (0X00000018u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX1_OVR_MASK (0X02000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX1_OVR_SHIFT (0X00000019u)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX2_OVR_MASK (0X04000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX2_OVR_SHIFT (0X0000001Au)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX3_OVR_MASK (0X08000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX3_OVR_SHIFT (0X0000001Bu)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX4_OVR_MASK (0X10000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX4_OVR_SHIFT (0X0000001Cu)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX5_OVR_MASK (0X20000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX5_OVR_SHIFT (0X0000001Du)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX6_OVR_MASK (0X40000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX6_OVR_SHIFT (0X0000001Eu)

#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX7_OVR_MASK (0X80000000u)
#define CSL_CBUFF_HL_IRQSTATUS_IRQ_CTX7_OVR_SHIFT (0X0000001Fu)

/* -------------------- CBUFF HL IRQ SET -------------------------- */

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_OCP_ERR_MASK (0X00000001u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_OCP_ERR_SHIFT (0X00000000u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX0_INVALID_MASK (0X00000100u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX0_INVALID_SHIFT (0X00000010u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX1_INVALID_MASK (0X00000200u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX1_INVALID_SHIFT (0X00000011u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX2_INVALID_MASK (0X00000400u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX2_INVALID_SHIFT (0X00000012u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX3_INVALID_MASK (0X00000800u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX3_INVALID_SHIFT (0X00000013u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX4_INVALID_MASK (0X00001000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX4_INVALID_SHIFT (0X00000014u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX5_INVALID_MASK (0X00002000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX5_INVALID_SHIFT (0X00000015u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX6_INVALID_MASK (0X00004000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX6_INVALID_SHIFT (0X00000016u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX7_INVALID_MASK (0X00008000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX7_INVALID_SHIFT (0X00000017u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX0_READY_MASK (0X00010000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX0_READY_SHIFT (0X00000008u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX1_READY_MASK (0X00020000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX1_READY_SHIFT (0X0000009u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX2_READY_MASK (0X00040000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX2_READY_SHIFT (0X0000000Au)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX3_READY_MASK (0X00080000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX3_READY_SHIFT (0X0000000Bu)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX4_READY_MASK (0X00100000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX4_READY_SHIFT (0X0000000Cu)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX5_READY_MASK (0X00200000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX5_READY_SHIFT (0X0000000Du)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX6_READY_MASK (0X00400000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX6_READY_SHIFT (0X0000000Eu)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX7_READY_MASK (0X00800000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX7_READY_SHIFT (0X0000000Fu)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX0_OVR_MASK (0X01000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX0_OVR_SHIFT (0X00000018u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX1_OVR_MASK (0X02000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX1_OVR_SHIFT (0X00000019u)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX2_OVR_MASK (0X04000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX2_OVR_SHIFT (0X0000001Au)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX3_OVR_MASK (0X08000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX3_OVR_SHIFT (0X0000001Bu)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX4_OVR_MASK (0X10000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX4_OVR_SHIFT (0X0000001Cu)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX5_OVR_MASK (0X20000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX5_OVR_SHIFT (0X0000001Du)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX6_OVR_MASK (0X40000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX6_OVR_SHIFT (0X0000001Eu)

#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX7_OVR_MASK (0X80000000u)
#define CSL_CBUFF_HL_IRQENABLE_SET_IRQ_CTX7_OVR_SHIFT (0X0000001Fu)

/* -------------------- CBUFF HL IRQ CLR -------------------------- */

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_OCP_ERR_MASK (0X00000001u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_OCP_ERR_SHIFT (0X00000000u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX0_INVALID_MASK (0X00000100u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX0_INVALID_SHIFT (0X00000010u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX1_INVALID_MASK (0X00000200u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX1_INVALID_SHIFT (0X00000011u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX2_INVALID_MASK (0X00000400u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX2_INVALID_SHIFT (0X00000012u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX3_INVALID_MASK (0X00000800u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX3_INVALID_SHIFT (0X00000013u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX4_INVALID_MASK (0X00001000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX4_INVALID_SHIFT (0X00000014u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX5_INVALID_MASK (0X00002000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX5_INVALID_SHIFT (0X00000015u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX6_INVALID_MASK (0X00004000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX6_INVALID_SHIFT (0X00000016u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX7_INVALID_MASK (0X00008000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX7_INVALID_SHIFT (0X00000017u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX0_READY_MASK (0X00010000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX0_READY_SHIFT (0X00000008u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX1_READY_MASK (0X00020000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX1_READY_SHIFT (0X00000009u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX2_READY_MASK (0X00040000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX2_READY_SHIFT (0X0000000Au)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX3_READY_MASK (0X00080000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX3_READY_SHIFT (0X0000000Bu)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX4_READY_MASK (0X00100000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX4_READY_SHIFT (0X0000000Cu)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX5_READY_MASK (0X00200000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX5_READY_SHIFT (0X0000000Du)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX6_READY_MASK (0X00400000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX6_READY_SHIFT (0X0000000Eu)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX7_READY_MASK (0X00800000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX7_READY_SHIFT (0X0000000Fu)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX0_OVR_MASK (0X01000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX0_OVR_SHIFT (0X00000018u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX1_OVR_MASK (0X02000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX1_OVR_SHIFT (0X00000019u)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX2_OVR_MASK (0X04000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX2_OVR_SHIFT (0X0000001Au)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX3_OVR_MASK (0X08000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX3_OVR_SHIFT (0X0000001Bu)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX4_OVR_MASK (0X10000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX4_OVR_SHIFT (0X0000001Cu)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX5_OVR_MASK (0X20000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX5_OVR_SHIFT (0X0000001Du)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX6_OVR_MASK (0X40000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX6_OVR_SHIFT (0X0000001Eu)

#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX7_OVR_MASK (0X80000000u)
#define CSL_CBUFF_HL_IRQENABLE_CLR_IRQ_CTX7_OVR_SHIFT (0X0000001Fu)

/* -------------------- CBUFF HL FRAG ADDR -------------------------- */

#define CSL_CBUFF_FRAG_ADDR__0_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__0_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__1_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__1_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__2_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__2_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__3_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__3_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__4_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__4_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__5_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__5_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__6_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__6_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__7_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__7_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__8_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__8_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__9_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__9_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__10_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__10_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__11_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__11_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__12_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__12_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__13_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__13_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__14_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__14_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_FRAG_ADDR__15_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_FRAG_ADDR__15_ADDR_SHIFT (0X00000004u)

/* -------------------- CBUFF HL CTRL -------------------------- */

#define CSL_CBUFF_CTX_CTRL__0_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__0_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__0_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__0_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__0_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__0_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__0_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__0_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__0_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__0_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__0_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__0_TILERMODE_SHIFT (0X0000000Bu)

#define CSL_CBUFF_CTX_CTRL__1_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__1_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__1_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__1_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__1_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__1_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__1_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__1_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__1_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__1_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__1_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__1_TILERMODE_SHIFT (0X0000000Bu)

#define CSL_CBUFF_CTX_CTRL__2_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__2_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__2_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__2_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__2_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__2_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__2_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__2_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__2_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__2_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__2_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__2_TILERMODE_SHIFT (0X0000000Bu)

#define CSL_CBUFF_CTX_CTRL__3_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__3_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__3_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__3_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__3_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__3_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__3_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__3_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__3_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__3_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__3_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__3_TILERMODE_SHIFT (0X0000000Bu)

#define CSL_CBUFF_CTX_CTRL__4_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__4_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__4_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__4_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__4_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__4_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__4_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__4_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__4_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__4_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__4_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__4_TILERMODE_SHIFT (0X0000000Bu)

#define CSL_CBUFF_CTX_CTRL__5_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__5_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__5_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__5_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__5_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__5_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__5_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__5_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__5_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__5_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__5_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__5_TILERMODE_SHIFT (0X0000000Bu)

#define CSL_CBUFF_CTX_CTRL__6_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__6_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__6_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__6_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__6_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__6_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__6_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__6_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__6_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__6_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__6_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__6_TILERMODE_SHIFT (0X0000000Bu)

#define CSL_CBUFF_CTX_CTRL__7_ENABLE_MASK (0X00000001u)
#define CSL_CBUFF_CTX_CTRL__7_ENABLE_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_CTRL__7_MODE_MASK (0X00000006u)
#define CSL_CBUFF_CTX_CTRL__7_MODE_SHIFT (0X00000001u)

#define CSL_CBUFF_CTX_CTRL__7_BCF_MASK (0X000000F0u)
#define CSL_CBUFF_CTX_CTRL__7_BCF_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_CTRL__7_WCOUNT_MASK (0X00000300u)
#define CSL_CBUFF_CTX_CTRL__7_WCOUNT_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_CTRL__7_DONE_MASK (0X00000400u)
#define CSL_CBUFF_CTX_CTRL__7_DONE_SHIFT (0X0000000Au)

#define CSL_CBUFF_CTX_CTRL__7_TILERMODE_MASK (0X00000800u)
#define CSL_CBUFF_CTX_CTRL__7_TILERMODE_SHIFT (0X0000000Bu)

/* -------------------- CBUFF HL START -------------------------- */

#define CSL_CBUFF_CTX_START__0_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__0_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_START__1_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__1_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_START__2_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__2_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_START__3_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__3_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_START__4_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__4_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_START__5_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__5_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_START__6_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__6_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_START__7_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_START__7_ADDR_SHIFT (0X00000004u)

/* -------------------- CBUFF HL END -------------------------- */

#define CSL_CBUFF_CTX_END__0_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__0_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_END__1_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__1_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_END__2_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__2_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_END__3_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__3_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_END__4_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__4_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_END__5_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__5_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_END__6_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__6_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_END__7_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_END__7_ADDR_SHIFT (0X00000004u)

/* -------------------- CBUFF HL WINDOWSIZE -------------------------- */

#define CSL_CBUFF_CTX_WINDOWSIZE__0_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__0_SIZE_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_WINDOWSIZE__1_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__1_SIZE_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_WINDOWSIZE__2_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__2_SIZE_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_WINDOWSIZE__3_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__3_SIZE_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_WINDOWSIZE__4_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__4_SIZE_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_WINDOWSIZE__5_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__5_SIZE_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_WINDOWSIZE__6_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__6_SIZE_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_WINDOWSIZE__7_SIZE_MASK (0X00FFFFF0u)
#define CSL_CBUFF_CTX_WINDOWSIZE__7_SIZE_SHIFT (0X00000004u)

/* -------------------- CBUFF HL F THRESHOLD -------------------------- */

#define CSL_CBUFF_CTX_THRESHOLD_F_0_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_0_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_F_1_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_1_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_F_2_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_2_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_F_3_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_3_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_F_4_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_4_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_F_5_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_5_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_F_6_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_6_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_F_7_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_F_7_THRESHOLD_SHIFT (0X00000000u)

/* -------------------- CBUFF HL S THRESHOLD -------------------------- */

#define CSL_CBUFF_CTX_THRESHOLD_S_0_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_0_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_S_1_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_1_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_S_2_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_2_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_S_3_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_3_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_S_4_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_4_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_S_5_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_5_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_S_6_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_6_THRESHOLD_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_THRESHOLD_S_7_THRESHOLD_MASK (0X00FFFFFFu)
#define CSL_CBUFF_CTX_THRESHOLD_S_7_THRESHOLD_SHIFT (0X00000000u)

/* -------------------- CBUFF HL STATUS -------------------------- */

#define CSL_CBUFF_CTX_STATUS__0_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__0_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__0_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__0_WA_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_STATUS__1_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__1_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__1_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__1_WA_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_STATUS__2_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__2_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__2_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__2_WA_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_STATUS__3_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__3_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__3_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__3_WA_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_STATUS__4_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__4_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__4_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__4_WA_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_STATUS__5_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__5_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__5_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__5_WA_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_STATUS__6_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__6_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__6_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__6_WA_SHIFT (0X00000008u)

#define CSL_CBUFF_CTX_STATUS__7_WB_MASK (0X0000000Fu)
#define CSL_CBUFF_CTX_STATUS__7_WB_SHIFT (0X00000000u)

#define CSL_CBUFF_CTX_STATUS__7_WA_MASK (0X00000F00u)
#define CSL_CBUFF_CTX_STATUS__7_WA_SHIFT (0X00000008u)

/* -------------------- CBUFF HL PHY ADDR -------------------------- */
#define CSL_CBUFF_CTX_PHY__0_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__0_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_PHY__1_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__1_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_PHY__2_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__2_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_PHY__3_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__3_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_PHY__4_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__4_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_PHY__5_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__5_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_PHY__6_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__6_ADDR_SHIFT (0X00000004u)

#define CSL_CBUFF_CTX_PHY__7_ADDR_MASK (0XFFFFFFF0u)
#define CSL_CBUFF_CTX_PHY__7_ADDR_SHIFT (0X00000004u)

extern CBUFF_REGS *pCBUFF;

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
