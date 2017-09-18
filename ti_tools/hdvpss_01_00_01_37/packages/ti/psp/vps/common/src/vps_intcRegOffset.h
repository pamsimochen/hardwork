/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/** ============================================================================
 *   \file  vps_intcRegOffset.h
 *
 *   This file contains the Register Desciptions for VPS
 *  ============================================================================
 */

#ifndef _VPS_INTCREGOFFSET_H
#define _VPS_INTCREGOFFSET_H

#ifdef TI_814X_BUILD
#include <ti/psp/cslr/soc_TI814x.h>
#endif

#ifdef TI_816X_BUILD
#include <ti/psp/cslr/soc_TI816x.h>
#endif

#ifdef TI_8107_BUILD
#include <ti/psp/cslr/soc_TI8107.h>
#endif

/**
 *  \brief This register offset should point to the register intc register base
 *  address for the appropriate interrupt
 */
#define VPDMA_INT_BASE_ADDR               (CSL_VPS_VPDMA_0_REGS + 0x130)
#define VPS_INTC_NUM                      (0x3u)
#define VPS_INTC_BASE_ADDR                (CSL_VPS_INTC_0_REGS + 0x80)

#define VPS_INTC2_NUM                     (0x2u)
#define VPS_INTC2_BASE_ADDR               (CSL_VPS_INTC_0_REGS + 0x60)


#define VPS_INTC_EOI_ADDR                 (CSL_VPS_INTC_0_REGS + 0xA0)

#define VPS_INTC_RAW_SET_REG0             (VPS_INTC_BASE_ADDR + 0x00)
#define VPS_INTC_RAW_SET_REG1             (VPS_INTC_BASE_ADDR + 0x04)
#define VPS_INTC_ENABLED_CLR_REG0         (VPS_INTC_BASE_ADDR + 0x08)
#define VPS_INTC_ENABLED_CLR_REG1         (VPS_INTC_BASE_ADDR + 0x0C)
#define VPS_INTC_ENABLE_SET_REG0          (VPS_INTC_BASE_ADDR + 0x10)
#define VPS_INTC_ENABLE_SET_REG1          (VPS_INTC_BASE_ADDR + 0x14)
#define VPS_INTC_ENABLE_CLR_REG0          (VPS_INTC_BASE_ADDR + 0x18)
#define VPS_INTC_ENABLE_CLR_REG1          (VPS_INTC_BASE_ADDR + 0x1C)


#define VPS_INTC2_ENABLED_CLR_REG0        (VPS_INTC2_BASE_ADDR + 0x08)
#define VPS_INTC2_ENABLE_SET_REG0         (VPS_INTC2_BASE_ADDR + 0x10)

/**
 *  \brief This offset should be 0xE0 but using 0xD0 because simulator still
 *  has not interoduced two register which are int_channel6_stat and
 *  int_channel6_mask registers
 */
#define VPDMA_INT_CHANNEL0_INT_STAT    (VPDMA_INT_BASE_ADDR + 0x00)
#define VPDMA_INT_CHANNEL0_INT_MASK    (VPDMA_INT_BASE_ADDR + 0x04)
#define VPDMA_INT_CHANNEL1_INT_STAT    (VPDMA_INT_BASE_ADDR + 0x08)
#define VPDMA_INT_CHANNEL1_INT_MASK    (VPDMA_INT_BASE_ADDR + 0x0C)
#define VPDMA_INT_CHANNEL2_INT_STAT    (VPDMA_INT_BASE_ADDR + 0x10)
#define VPDMA_INT_CHANNEL2_INT_MASK    (VPDMA_INT_BASE_ADDR + 0x14)
#define VPDMA_INT_CHANNEL3_INT_STAT    (VPDMA_INT_BASE_ADDR + 0x18)
#define VPDMA_INT_CHANNEL3_INT_MASK    (VPDMA_INT_BASE_ADDR + 0x1C)
#define VPDMA_INT_CHANNEL4_INT_STAT    (VPDMA_INT_BASE_ADDR + 0x20)
#define VPDMA_INT_CHANNEL4_INT_MASK    (VPDMA_INT_BASE_ADDR + 0x24)
#define VPDMA_INT_CHANNEL5_INT_STAT    (VPDMA_INT_BASE_ADDR + 0x28)
#define VPDMA_INT_CHANNEL5_INT_MASK    (VPDMA_INT_BASE_ADDR + 0x2C)
#define VPDMA_INT_CHANNEL6_INT_STAT    (VPDMA_INT_BASE_ADDR + 0x30)
#define VPDMA_INT_CHANNEL6_INT_MASK    (VPDMA_INT_BASE_ADDR + 0x34)
#define VPDMA_INT_CLIENT0_INT_STAT     (VPDMA_INT_BASE_ADDR + 0x38)
#define VPDMA_INT_CLIENT0_INT_MASK     (VPDMA_INT_BASE_ADDR + 0x3C)
#define VPDMA_INT_CLIENT1_INT_STAT     (VPDMA_INT_BASE_ADDR + 0x40)
#define VPDMA_INT_CLIENT1_INT_MASK     (VPDMA_INT_BASE_ADDR + 0x44)
#define VPDMA_INT_LIST0_INT_STAT       (VPDMA_INT_BASE_ADDR + 0x48)
#define VPDMA_INT_LIST0_INT_MASK       (VPDMA_INT_BASE_ADDR + 0x4C)



#endif /* End of #ifndef _VPS_INTCREGOFFSET_H */

