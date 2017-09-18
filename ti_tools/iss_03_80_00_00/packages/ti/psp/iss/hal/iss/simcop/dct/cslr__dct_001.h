/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file cslr_dct_001.h
*
* This file contains register level values for DCT module on SIMCOP in OMAP4/Monica
*[auto generated file using CSL gen tools]
*
* @path DCT\inc
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 20-July    2008 Anandhi Ramesh: Initial Release
*! 9-Sep     2008 Sowmya Priya : Updates in functions
*!
*========================================================================= */

#ifndef _CSLR__DCT_001_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR__DCT_001_H_

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <ti/psp/iss/hal/iss/simcop/common/cslr.h>
#include <ti/psp/iss/hal/iss/simcop/common/csl_types.h>

/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct {
    volatile Uint32 REVISION;
    volatile Uint32 CTRL;
    volatile Uint32 CFG;
    volatile Uint32 SPTR;
    volatile Uint32 FPTR;
} CSL_DctRegs;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* REVISION */

#define CSL_DCT_REVISION_REV_MASK (0x000000FFu)
#define CSL_DCT_REVISION_REV_SHIFT (0x00000000u)
#define CSL_DCT_REVISION_REV_RESETVAL (0x00000010u)

#define CSL_DCT_REVISION_RESETVAL (0x00000010u)

/* CTRL */

#define CSL_DCT_CTRL_BUSY_MASK (0x00008000u)
#define CSL_DCT_CTRL_BUSY_SHIFT (0x0000000Fu)
#define CSL_DCT_CTRL_BUSY_RESETVAL (0x00000000u)
/*----BUSY Tokens----*/
#define CSL_DCT_CTRL_BUSY_IDLE (0x00000000u)
#define CSL_DCT_CTRL_BUSY_BUSY (0x00000001u)

#define CSL_DCT_CTRL_EN_MASK (0x00000001u)
#define CSL_DCT_CTRL_EN_SHIFT (0x00000000u)
#define CSL_DCT_CTRL_EN_RESETVAL (0x00000000u)

#define CSL_DCT_CTRL_RESETVAL (0x00000000u)

/* CFG */

#define CSL_DCT_CFG_NMCUS_MASK (0x00003F00u)
#define CSL_DCT_CFG_NMCUS_SHIFT (0x00000008u)
#define CSL_DCT_CFG_NMCUS_RESETVAL (0x00000000u)

#define CSL_DCT_CFG_AUTOGATING_MASK (0x00000020u)
#define CSL_DCT_CFG_AUTOGATING_SHIFT (0x00000005u)
#define CSL_DCT_CFG_AUTOGATING_RESETVAL (0x00000001u)
/*----AUTOGATING Tokens----*/
#define CSL_DCT_CFG_AUTOGATING_FREE_RUNNING (0x00000000u)
#define CSL_DCT_CFG_AUTOGATING_GATED (0x00000001u)

#define CSL_DCT_CFG_TRIG_SRC_MASK (0x00000010u)
#define CSL_DCT_CFG_TRIG_SRC_SHIFT (0x00000004u)
#define CSL_DCT_CFG_TRIG_SRC_RESETVAL (0x00000000u)
/*----TRIG_SRC Tokens----*/
#define CSL_DCT_CFG_TRIG_SRC_MMR_WRITE (0x00000000u)
#define CSL_DCT_CFG_TRIG_SRC_HW_START (0x00000001u)

#define CSL_DCT_CFG_INTEN_MASK (0x00000008u)
#define CSL_DCT_CFG_INTEN_SHIFT (0x00000003u)
#define CSL_DCT_CFG_INTEN_RESETVAL (0x00000000u)
/*----INTEN Tokens----*/
#define CSL_DCT_CFG_INTEN_DISABLE (0x00000000u)
#define CSL_DCT_CFG_INTEN_ENABLE (0x00000001u)

#define CSL_DCT_CFG_FMT_MASK (0x00000006u)
#define CSL_DCT_CFG_FMT_SHIFT (0x00000001u)
#define CSL_DCT_CFG_FMT_RESETVAL (0x00000000u)
/*----FMT Tokens----*/
#define CSL_DCT_CFG_FMT_YUV420 (0x00000000u)
#define CSL_DCT_CFG_FMT_YUV422 (0x00000001u)
#define CSL_DCT_CFG_FMT_SEQBLOCKS (0x00000002u)

#define CSL_DCT_CFG_MODE_MASK (0x00000001u)
#define CSL_DCT_CFG_MODE_SHIFT (0x00000000u)
#define CSL_DCT_CFG_MODE_RESETVAL (0x00000000u)
/*----MODE Tokens----*/
#define CSL_DCT_CFG_MODE_DCT (0x00000000u)
#define CSL_DCT_CFG_MODE_IDCT (0x00000001u)

#define CSL_DCT_CFG_RESETVAL (0x00000020u)

/* SPTR */

#define CSL_DCT_SPTR_ADDR_MASK (0x00001FE0u)
#define CSL_DCT_SPTR_ADDR_SHIFT (0x00000005u)
#define CSL_DCT_SPTR_ADDR_RESETVAL (0x00000000u)

#define CSL_DCT_SPTR_RESETVAL (0x00000000u)

/* FPTR */

#define CSL_DCT_FPTR_ADDR_MASK (0x00003FF0u)
#define CSL_DCT_FPTR_ADDR_SHIFT (0x00000004u)
#define CSL_DCT_FPTR_ADDR_RESETVAL (0x00000000u)

#define CSL_DCT_FPTR_RESETVAL (0x00000000u)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
