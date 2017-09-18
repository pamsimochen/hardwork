/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file cslr_imx4_1.h
*
* This file contains register level structure definition for IMX module in SIMCOP
*
* @path ???
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 25-Sep 2008 Phanish: Initial Version! 
*========================================================================= */
#ifndef _CSLR__IMX4_1_H_
#define _CSLR__IMX4_1_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ti/psp/iss/hal/iss/simcop/common/cslr.h>

#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>

    /* Minimum unit = 2 bytes */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
    typedef struct {
        volatile Uint16 START;
        volatile Uint16 CMDADR;
        volatile Uint16 CMDPTR;
        volatile Uint16 CLKCNTRL;
        volatile Uint16 MINMAX_ID;
        volatile Uint16 MINMAX_VAL;
        volatile Uint16 BRKPTR;
    } CSL_Imx4Regs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

    /* START */

#define CSL_IMX4_START_START_MASK (0x0001u)
#define CSL_IMX4_START_START_SHIFT (0x0000u)
#define CSL_IMX4_START_START_RESETVAL (0x0000u)
/*----START Tokens----*/
#define CSL_IMX4_START_START_IDLE (0x0000u)
#define CSL_IMX4_START_START_BUSY (0x0001u)

#define CSL_IMX4_START_RESETVAL (0x0000u)

    /* CMDADR */

#define CSL_IMX4_CMDADR_ADR_MASK (0x1FFFu)
#define CSL_IMX4_CMDADR_ADR_SHIFT (0x0000u)
#define CSL_IMX4_CMDADR_ADR_RESETVAL (0x0000u)

#define CSL_IMX4_CMDADR_RESETVAL (0x0000u)

    /* CMDPTR */

#define CSL_IMX4_CMDPTR_CMDPTR_MASK (0x1FFFu)
#define CSL_IMX4_CMDPTR_CMDPTR_SHIFT (0x0000u)
#define CSL_IMX4_CMDPTR_CMDPTR_RESETVAL (0x0000u)

#define CSL_IMX4_CMDPTR_RESETVAL (0x0000u)

    /* CLKCNTRL */

#define CSL_IMX4_CLKCNTRL_CLKCNTRL_MASK (0x0001u)
#define CSL_IMX4_CLKCNTRL_CLKCNTRL_SHIFT (0x0000u)
#define CSL_IMX4_CLKCNTRL_CLKCNTRL_RESETVAL (0x0000u)
/*----CLKCNTRL Tokens----*/
#define CSL_IMX4_CLKCNTRL_CLKCNTRL_ON (0x0000u)
#define CSL_IMX4_CLKCNTRL_CLKCNTRL_OFF (0x0001u)

#define CSL_IMX4_CLKCNTRL_RESETVAL (0x0000u)

    /* MINMAX_ID */

#define CSL_IMX4_MINMAX_ID_ID_MASK (0xFFFFu)
#define CSL_IMX4_MINMAX_ID_ID_SHIFT (0x0000u)
#define CSL_IMX4_MINMAX_ID_ID_RESETVAL (0x0000u)

#define CSL_IMX4_MINMAX_ID_RESETVAL (0x0000u)

    /* MINMAX_VAL */

#define CSL_IMX4_MINMAX_VAL_VAL_MASK (0xFFFFu)
#define CSL_IMX4_MINMAX_VAL_VAL_SHIFT (0x0000u)
#define CSL_IMX4_MINMAX_VAL_VAL_RESETVAL (0x0000u)

#define CSL_IMX4_MINMAX_VAL_RESETVAL (0x0000u)

    /* BRKPTR */

#define CSL_IMX4_BRKPTR_STAT_MASK (0x4000u)
#define CSL_IMX4_BRKPTR_STAT_SHIFT (0x000Eu)
#define CSL_IMX4_BRKPTR_STAT_RESETVAL (0x0000u)

#define CSL_IMX4_BRKPTR_EN_MASK (0x2000u)
#define CSL_IMX4_BRKPTR_EN_SHIFT (0x000Du)
#define CSL_IMX4_BRKPTR_EN_RESETVAL (0x0000u)
/*----EN Tokens----*/
#define CSL_IMX4_BRKPTR_EN_DISABLE (0x0000u)

#define CSL_IMX4_BRKPTR_ADR_MASK (0x1FFFu)
#define CSL_IMX4_BRKPTR_ADR_SHIFT (0x0000u)
#define CSL_IMX4_BRKPTR_ADR_RESETVAL (0x0000u)

#define CSL_IMX4_BRKPTR_RESETVAL (0x0000u)

#ifdef __cplusplus
}
#endif
#endif
