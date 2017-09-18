/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file cbuff_interrupts.h
 *           This file contains the prototypes and definitions to handle cbuff 
 *           interrupts 
 *
 * @path     Centaurus\drivers\csl\iss\cbuff\inc\
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *! 
 */
 /*========================================================================= */

#ifndef _CBUFF_INTERRUPTS_H
#define _CBUFF_INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif                                                     /* __cplusplus */

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/

#include "cbuff_sys_types.h"
#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

    typedef enum {
        CBUFF_CALLBACK_CONTEXT_SWI = 0,
        CBUFF_CALLBACK_CONTEXT_HWI = 1,
        CBUFF_CALLBACK_CONTEXT_MAX = 2
    } CBUFF_CALLBACK_CONTEXT_T;

    /* ================================================================ */
/**
 *Description:- callback function prototype
 */
 /*================================================================== */
    typedef void (*cbuff_callback_t) (uint32 arg1, uint32 arg2);

    /* ================================================================ */
/**
 *Description:- struct for the registering the callback
*/
 /*================================================================== */
    typedef struct CBUFF_IRQ_Handle_Tag {
        struct CBUFF_IRQ_Handle_Tag *list_next;
        cbuff_callback_t callback;
        uint32 arg1;
        uint32 arg2;
        CBUFF_CALLBACK_CONTEXT_T callback_context;
        uint8 priority;                                    /* :- 255 Lowest
                                                            * priority, 0 :-
                                                            * highest
                                                            * priority */

    } CBUFF_IRQ_Handle_T;

    void CBUFF_Disable_All_Interrupts(void);
    CBUFF_RETURN CBUFF_Ctx_Enable_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                            CBUFF_IRQ_ID irq_type);
    CBUFF_RETURN CBUFF_Ctx_Disable_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                             CBUFF_IRQ_ID irq_type);
    CBUFF_RETURN CBUFF_Ctx_Hook_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                          CBUFF_IRQ_ID irq_type,
                                          CBUFF_IRQ_Handle_T * handle);
    CBUFF_RETURN CBUFF_Ctx_UnHook_Interrupt(CBUFF_CTX_NUMBER_TYPE ctx_number,
                                            CBUFF_IRQ_ID irq_type,
                                            CBUFF_IRQ_Handle_T * handle);

#ifdef __cplusplus
}
#endif                                                     /* __cplusplus */
#endif                                                     // _CBUFF_INTERRUPTS_H
