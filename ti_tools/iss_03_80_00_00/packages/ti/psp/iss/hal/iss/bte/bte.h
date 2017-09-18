/** ==================================================================
 *  @file   bte.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/bte/                                                  
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
/* File Name :bte.h */
/* */
/* Description :Program file containing the register level values for Ccp2 */
/* */
/* Created : Ducati Team (OMAP4) */
/* */
/* @rev 1.0 */
/*========================================================================
*!
*! Revision History
*! ===================================*/

#ifndef _CSLR_BTE_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_BTE_H_

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include "inc/bte_reg.h"
#include "inc/bte_utils.h"
#include <ti/psp/iss/hal/iss/bte/bte_drv.h>
#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

// typedef uint8 Channel_Number;
typedef volatile CSL_Bte_Regs *bte_regs_ovly;

/* The Following are the generic params for BTE */
#define BTE_NUM_DEVICES 1
#define BTE_DRV_NUMCHANS 4

#define BTE_BASE_OFFSET		(0x2000)
#define BTE_BASE_ADDRESS	(ISS_REGS_BASE_ADDR + BTE_BASE_OFFSET)

#define BTE_PITCH (1024 * 64)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
