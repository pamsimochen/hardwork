/** ==================================================================
 *  @file   cbuff.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/cbuff/                                                  
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
/* File Name :cbuff.h */
/* */
/* Description :Program file containing the register level values for cbuff */
/* */
/* Created : Ducati Team (OMAP4) */
/* */
/* @rev 1.0 */
/*========================================================================
*!
*! Revision History
*! ===================================*/

#ifndef _CSLR_CBUFF_H_
#define _CSLR_CBUFF_H_

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

#ifdef ISS_SIMULATOR
#define CBUFF_SIMULATION
#endif

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "inc/cbuff_sys_types.h"

    /* CBUFF TYPES */
#define CBUFF_SUCCESS               0
#define CBUFF_FAILURE               1
#define CBUFF_RESOURCE_UNAVAILABLE  2
#define CBUFF_BUSY                  3
#define CBUFF_INVALID_INPUT         4

#define CBUFF_IRQ_OCP_ERR_SHIFT     0
#define CBUFF_IRQ_READY_SHIFT       8
#define CBUFF_IRQ_INVALID_SHIFT     16
#define CBUFF_IRQ_OVR_SHIFT         24

#define CBUFF_MAX_WINDOWS           16
#define CBUFF_MIN_WINDOWS            2

    typedef enum {
        CBUFF_CTX_0,
        CBUFF_CTX_1,
        CBUFF_CTX_2,
        CBUFF_CTX_3,
        CBUFF_CTX_4,
        CBUFF_CTX_5,
        CBUFF_CTX_6,
        CBUFF_CTX_7,
        CBUFF_CTX_MAX
    } CBUFF_CTX_NUMBER_TYPE;

    typedef void *CBUFF_PHYSICAL_ADDR_TYPE;
    typedef void *CBUFF_VIRTUAL_ADDR_TYPE;

    /* BCF stands for Bandwidth Control feedback */
    typedef uint16 CBUFF_BCF;

#define CBUFF_ENABLE                1
#define CBUFF_DISABLE               0

    /* ================================================================ */
/** Description:- struct to hold device specific data
* @param opened to hold device open data
*/
/*================================================================== */
    typedef struct {
        uint8 opened;
    } cbuff_dev_data_t;

    /* ======================================================================= 
     */
/** CBUFF_CONTEXT_IRQ_ID enumeration for CBUFF_CONTEXT_IRQ
*
* @param INTERRUPT_EN  interrupt enabled mode
* @param INTERRUPT_DIS interrupt disabled mode
*/
    /* ======================================================================= 
     */

    typedef enum {
        INTERRUPT_ENABLE = 1,
        INTERRUPT_DISABLE = 0
    } CBUFF_CONTEXT_IRQ_ID;

/*======================================================================= */
/** CBUFF_CONTEXTS enumeration for the number of cbuff contexts
*
* @param TWO_CONTEXTS  two contexts
* @param FOUR_CONTEXTS four contexts
* @param EIGHT_CONTEXTS eight contexts
*/
    /* ======================================================================= 
     */
    typedef enum {
        TWO_CONTEXTS = 0,
        FOUR_CONTEXTS = 1,
        EIGHT_CONTEXTS = 2
    } CBUFF_CONTEXTS;
    /* ======================================================================= 
     */
/** CBUFF_ENABLE_FRAG enumeration for the fragmentation support of context 0
*
* @param FRAG_DIS fragmentation diabled for context 0
* @param FRAG_EN fragmentation enabled for context 0
* ======================================================================= */

    typedef enum {
        FRAG_DISABLE = 0,
        FRAG_ENABLE = 1
    } CBUFF_ENABLE_FRAG;

    /* ======================================================================= 
     */
/** This is  a structure description of cbuff_hl_hwinfo object
* @param CTXS number of contexts for cbuff
* @param FRAG_EN to enable fragmentation for context 0
* ======================================================================= */

    typedef struct {
        CBUFF_CONTEXTS CTXS;
        /* this is to set the number of contexts for cbuff */

        CBUFF_ENABLE_FRAG FRAG_EN;
        /* This is to enable/disable fragmentation support for context 0 */
    } cbuff_hl_hwinfo;

    /* ======================================================================= 
     */
/** CBUFF_POWER_STATE  enumeration for the power state of CBUFF

* @param FORCE_IDLE forced idle state of the cbuff
* @param NO_IDLE non idle state of the cbuff
* @param SMART_IDLE smart idle state of the cbuff
*/
    /* ======================================================================= 
     */

    typedef enum {
        FORCE_IDLE = 0,
        NO_IDLE = 1,
        SMART_IDLE = 2
    } CBUFF_POWER_STATE;

    /* ======================================================================= 
     */
/** CBUFF_CTX_MODE   enumeration for the mode of cbuff

* @param CBUFF_WRITE_MODE write mode of cbuff
* @param CBUFF_READ_MODE read mode of cbuff
* @param CBUFF_READ_WRITE_MODE read write mode of cbuff
*/
    /* ======================================================================= 
     */

    typedef enum {
        CBUFF_WRITE_MODE = 0,
        CBUFF_READ_MODE = 1,
        CBUFF_READ_WRITE_MODE = 2
    } CBUFF_CTX_MODE;

    /* ======================================================================= 
     */
/** CBUFF_WCOUNT enumeration for the cbuff window count

@param TWO_WINDOWS  two windows
@param FOUR_WINDOWS  four windows
@param EIGHT_WINDOWS eight windows
@param SIXTEEN_WINDOWS sixteen windows
* ======================================================================= */
    typedef enum {
        TWO_WINDOWS = 0,
        FOUR_WINDOWS = 1,
        EIGHT_WINDOWS = 2,
        SIXTEEN_WINDOWS = 3
    } CBUFF_WCOUNT;

    /* ======================================================================= 
     */
    /* 
     * This is a structure description for cbuff_ctx_ctrl Object.
     *
     * ======================================================================= */
    typedef struct {
        CBUFF_CTX_MODE CTX_MODE;
        /* To select the cbuff mode */
        CBUFF_WCOUNT WINCOUNT;
        /* To set the number of windows for a context */
        uint8 TILER;
        /* To set the tiler support */
        CBUFF_BCF BCF;
        /* To set the bandwidth control feedback for a context when
         * fragmentation is disabled */
        uint32 *CBUFF_FRAG_ADDR;
        /* pointer to array with fragmented physical addresses - available
         * only for context 0 and if _FRAGMENTATION parameter has been set
         * if NULL - CBUFF_CTX_PHY_X will be used */
        uint32 CBUFF_CTX_START_X;
        /* Virtual start adddress of context X */
        uint32 CBUFF_CTX_END_X;
        /* Virtual end adddress of context X */
        uint32 CBUFF_CTX_WINDOWSIZE_X;
        /* to set the window size of a context */
        uint32 CBUFF_CTX_THRESHOLD_F_X;
        /* Threshold value used to check if a write window for context X is
         * full. */
        uint32 CBUFF_CTX_THRESHOLD_S_X;
        /* Threshold value used to control the BCF synchronization mechanism
         * for context X */
        uint32 CBUFF_CTX_PHY_X;
        /* Start address of the physical buffer managed by the context */
    } CBUFF_CTX_CTRL;

    /* ======================================================================= 
     */
/** CBUFF_IRQ_ID  enumeration for cbuff irq types

@param IRQ_OCP_ERR ocp error
@param IRQ_CTX0_READY  context ready error
@param IRQ_CTX0_INVALID context invalid error
@param IRQ_CTX0_OVR context overflow error
* ======================================================================= */
    typedef enum {
        IRQ_OCP_ERR = 0,
        IRQ_CTX_READY = 1,
        IRQ_CTX_INVALID = 2,
        IRQ_CTX_OVR = 3,
        IRQ_TYPE_ID_MAX = 4
    } CBUFF_IRQ_ID;

    // ----------------------- CBUFF functions ------------------------------
    CBUFF_RETURN cbuff_Init(void);
    CBUFF_RETURN cbuff_Open(void);
    CBUFF_RETURN cbuff_Close(void);
    CBUFF_RETURN cbuff_Config_Power(CBUFF_POWER_STATE power_state);
    CBUFF_RETURN cbuff_Reset(void);
    void cbuff_Set_EOI(uint32 line_number);

    // ----------------------- Context Functions --------------------------
    void cbuff_Ctx_Enable(CBUFF_CTX_NUMBER_TYPE ctx_number);
    void cbuff_Ctx_Disable(CBUFF_CTX_NUMBER_TYPE ctx_number);
    void *cbuff_Ctx_Get_Client_ReadAddress(CBUFF_CTX_NUMBER_TYPE ctx_number);
    void cbuff_Ctx_Done(CBUFF_CTX_NUMBER_TYPE ctx_number);
    CBUFF_RETURN cbuff_Ctx_Config(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                  CBUFF_CTX_CTRL * pCTX);

    // ----------------------- CBUFF Interrupt Functions
    // --------------------------

    // Poll in IRQ RAW register for pending irq
    CBUFF_RETURN cbuff_Ctx_Poll_Ready_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number);
    CBUFF_RETURN cbuff_Ctx_Poll_Ovr_Flow_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number);
    CBUFF_RETURN cbuff_Ctx_Poll_Invalid_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number);
    CBUFF_RETURN cbuff_Ctx_Poll_Ocp_Error_Irq(void);

    // Checks in IRQ STATUS register for pending irq
    CBUFF_RETURN cbuff_Ctx_Check_Ready_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number);
    CBUFF_RETURN cbuff_Ctx_Check_Ovr_Flow_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number);
    CBUFF_RETURN cbuff_Ctx_Check_Invalid_Irq(CBUFF_CTX_NUMBER_TYPE ctx_number);
    CBUFF_RETURN cbuff_Ctx_Check_Ocp_Error_Irq(void);

    void CBUFF_Ctx_Get_Status(CBUFF_CTX_NUMBER_TYPE ctx_number, uint32 * pWA,
                              uint32 * pWB);

#include <ti/psp/iss/hal/iss/cbuff/inc/cbuff_interrupts.h>

#ifdef CBUFF_SIMULATION

    typedef struct SIM_CBUFF_IMG_PLANE_TAG {
        CBUFF_CTX_NUMBER_TYPE number;
        void *pData;
        uint32 sizeX;
        uint32 stride;
        uint32 size;
    } SIM_CBUFF_IMG_PLANE;

    typedef struct SIM_CBUFF_IMG_TAG {
        int number_of_planes;
        SIM_CBUFF_IMG_PLANE p1;
        SIM_CBUFF_IMG_PLANE p2;
    } SIM_CBUFF_IMG;

    CBUFF_RETURN sim_cbuff_RUN(SIM_CBUFF_IMG * img, int flagBlocking);

#endif                                                     // CBUFF_SIMULATION

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     // _CSLR_CBUFF_H_
