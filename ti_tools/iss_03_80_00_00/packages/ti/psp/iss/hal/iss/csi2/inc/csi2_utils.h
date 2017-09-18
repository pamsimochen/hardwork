/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file csi2_utils.h
*
* This file contains utilities and macros used in CSI2 driver code for MONICA/ OMAP-4 Ducati. 
*
* @path drv_csi2/
*
* @rev 1.0
*
* @developer: 
*/
/*========================================================================*/

#ifndef _CSLR_CSI2_UTILS_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_CSI2_UTILS_H_

/****************************************************************
*  INCLUDE FILES                                                 
*****************************************************************/
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Gate.h>
#include <xdc/runtime/Startup.h>
#include <ti/sysbios/knl/Swi.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/System.h>
#include <string.h>

#define CSI2_SUCCESS 0
#define CSI2_FAILURE 1
#define CSI2_BUSY 2
#define CSI2_INVALID_INPUT 3

#include "../../common/csl_utils/csl_types.h"
#include "../../common/csl_utils/csl_utils.h"
#include "../../iss_common/iss_common.h"

/* ================================================================ */
/* Macro to set a bit-field of a register to a given value
 * ================================================================== */
#define CSI2_SET32  REG_SET32

/* ================================================================ */
/* Macro to write a register with a given value
 * ================================================================== */
#define CSI2_WRITE32 REG_WRITE32

/* ================================================================ */
/* Macro to read a register value
 * ================================================================== */
#define CSI2_READ32 REG_READ32

/* ================================================================ */
/* Macro to extract a bit-field of a register
 * ================================================================== */
#define CSI2_FEXT REG_FEXT

typedef uint32 CSI2_RETURN;

/********IMPORTANT: Enter the correct  CSI2_BASE_ADDRESS  instead of the dummy value given*********/
#define CSI2_BASE_ADDRESS 		(ISS_REGS_BASE_ADDR + 0x1000)   // (0x50001000u)
#define CSI2B_BASE_ADDRESS 		(CSI2_BASE_ADDRESS + 0x400) // (0x50001000u)

#define CSI2_CTX_CTRL1_BASE(base_address) 	(base_address + 0x00000070)
#define CSI2_CTX_CTRL2_BASE(base_address) 	(base_address + 0x00000074)
#define CSI2_CTX_CTRL3_BASE(base_address) 	(base_address + 0x0000008C)
#define CSI2_CTX_DAT_OFST_BASE(base_address) 	(base_address + 0x00000078)
#define CSI2_CTX_DAT_PING_ADDR_BASE(base_address) 	(base_address + 0x0000007C)
#define CSI2_CTX_DAT_PONG_ADDR_BASE(base_address)	(base_address + 0x00000080)
#define CSI2_CTX_IRQENABLE_BASE(base_address) 	(base_address + 0x00000084)
#define CSI2_CTX_IRQSTATUS_BASE(base_address) 	(base_address + 0x00000088)
#define CSI2_PHY_BASE(base_address)   		(base_address + 0x00000170)
#define CSI2_COMPLEXIO_CFG_BASE(base_address)		(base_address + 0x00000054)
#define CSI2_CTX_TRANSCODEH_BASE(base_address)		(base_address + 0x000001C0)
#define CSI2_CTX_TRANSCODEV_BASE(base_address)		(base_address + 0x000001C4)

/************************************************************************/
/* Macros to enable the easy register access of context-specific registers
 * ********** */
/************************************************************************/
#define CSI2_CTX_OFF(CTX)                 ((0x20)*(CTX))
#define CSI2_CTX_TRANSC_OFF(CTX)          (8 * CTX)
#define CSI2_CTX_CTRL1(base_address,CTX)               (CSI2_CTX_CTRL1_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_CTRL2(base_address,CTX)               (CSI2_CTX_CTRL2_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_CTRL3(base_address,CTX)               (CSI2_CTX_CTRL3_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_DAT_OFST(base_address,CTX)            (CSI2_CTX_DAT_OFST_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_DAT_PING_ADDR(base_address,CTX)       (CSI2_CTX_DAT_PING_ADDR_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_DAT_PONG_ADDR(base_address,CTX)       (CSI2_CTX_DAT_PONG_ADDR_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_IRQENABLE(base_address,CTX)           (CSI2_CTX_IRQENABLE_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_IRQSTATUS(base_address,CTX)           (CSI2_CTX_IRQSTATUS_BASE(base_address) + CSI2_CTX_OFF(CTX))
#define CSI2_CTX_TRANSCODEH(base_address,CTX)         (CSI2_CTX_TRANSCODEH_BASE(base_address) + CSI2_CTX_TRANSC_OFF(CTX))
#define CSI2_CTX_TRANSCODEV(base_address,CTX)         (CSI2_CTX_TRANSCODEV_BASE(base_address) + CSI2_CTX_TRANSC_OFF(CTX))

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
