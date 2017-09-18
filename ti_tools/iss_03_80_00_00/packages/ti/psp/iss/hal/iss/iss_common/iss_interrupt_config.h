/* =======================================================================
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * ======================================================================== */
/**
 * @file ipipe.h
 *           This file contains the prototypes and definitions to configure ipipe
 * 
 *
 * @path     Centaurus\drivers\drv_isp\inc\csl
 *
 * @rev 1.0
 */
/*========================================================================
 *!
 *! Revision History
 *! 
 */
 /*========================================================================= */
#ifndef ISS_INT_CONFIG_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define ISS_INT_CONFIG_H

/* Number of banks in ISP */
#define ISP_INTERRUPT_BANK0 0
#define ISP_INTERRUPT_BANK1 1
#define ISP_INTERRUPT_BANK2 2
#define ISP_INTERRUPT_BANK3 3

/* Number of banks in SIMCOP */
#define SIMCOP_INTERRUPT_BANK0 0
#define SIMCOP_INTERRUPT_BANK1 1
#define SIMCOP_INTERRUPT_BANK2 2
#define SIMCOP_INTERRUPT_BANK3 3

/* Number of banks in CSI2 */
#define CSI2A_INTERRUPT_BANK0 0
#define CSI2B_INTERRUPT_BANK0 0

/* Number of banks in CCP2 */
#define CCP2_INTERRUPT_BANK0 0
#define CCP2_INTERRUPT_BANK1 1
#define CCP2_INTERRUPT_BANK2 2
#define CCP2_INTERRUPT_BANK3 3

/* Number of banks in CBUFF */
#define CBUFF_INTERRUPT_BANK0 0

/* Number of banks in BTE */
#define BTE_INTERRUPT_BANK0 0

/* Select the default banks for the submodules here */

#define ISP_DEFAULT_INTERRUPT_BANK          ISP_INTERRUPT_BANK0

#define SIMCOP_DEFAULT_INTERRUPT_BANK       SIMCOP_INTERRUPT_BANK0

#define CSI2A_DEFAULT_INTERRUPT_BANK        CSI2A_INTERRUPT_BANK0

#define CSI2B_DEFAULT_INTERRUPT_BANK        CSI2B_INTERRUPT_BANK0

#define CBUFF_DEFAULT_INTERRUPT_BANK        CBUFF_INTERRUPT_BANK0

#define BTE_DEFAULT_INTERRUPT_BANK          BTE_INTERRUPT_BANK0

/* ISS Level Configuration */

typedef enum {

    ISS_INTERRUPT_BANK_0 = 0,
    ISS_INTERRUPT_BANK_1 = 1,
    ISS_INTERRUPT_BANK_2 = 2,
    ISS_INTERRUPT_BANK_3 = 3,
    ISS_INTERRUPT_BANK_4 = 4,
    ISS_INTERRUPT_BANK_5 = 5
} ISS_INTERRUPT_BANK_T;

/* The default mapping of interrupts from sub modules to ISS interrupt merger */

/* default mapping of interrupts to irq banks */
#define ISS_DEFAULT_ISP_INTERRUPT_BANK      ISS_INTERRUPT_BANK_0
#define ISS_DEFUALT_SIMCOP_INTERRUPT_BANK	ISS_INTERRUPT_BANK_1
#define ISS_DEFUALT_CSI2A_INTERRUPT_BANK	ISS_INTERRUPT_BANK_2
#define ISS_DEFUALT_CSI2B_INTERRUPT_BANK    ISS_INTERRUPT_BANK_2
#define ISS_DEFUALT_CCP2_INTERRUPT_BANK     ISS_INTERRUPT_BANK_3
#define ISS_DEFUALT_BTE_INTERRUPT_BANK      ISS_INTERRUPT_BANK_4
#define ISS_DEFUALT_CBUFF_INTERRUPT_BANK    ISS_INTERRUPT_BANK_5

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
