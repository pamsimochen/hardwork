/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*********************************************************************
* file: cslr_vps_csc.h
*
* Brief: This file contains the Register Description for vps_csc
*
*********************************************************************/
#ifndef _CSLR_VPS_CSC_H_
#define _CSLR_VPS_CSC_H_



/* Minimum unit = 1 byte */

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct  {
    volatile Uint32 CSC00;
    volatile Uint32 CSC01;
    volatile Uint32 CSC02;
    volatile Uint32 CSC03;
    volatile Uint32 CSC04;
    volatile Uint32 CSC05;
} CSL_Vps_cscRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_Vps_cscRegs            *CSL_VpsCscRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/

/* csc00 */

#define CSL_VPS_CSC_CSC00_A0_MASK        (0x00001FFFu)
#define CSL_VPS_CSC_CSC00_A0_SHIFT       (0x00000000u)

#define CSL_VPS_CSC_CSC00_B0_MASK        (0x1FFF0000u)
#define CSL_VPS_CSC_CSC00_B0_SHIFT       (0x00000010u)


/* csc01 */

#define CSL_VPS_CSC_CSC01_A1_MASK        (0x1FFF0000u)
#define CSL_VPS_CSC_CSC01_A1_SHIFT       (0x00000010u)

#define CSL_VPS_CSC_CSC01_C0_MASK        (0x00001FFFu)
#define CSL_VPS_CSC_CSC01_C0_SHIFT       (0x00000000u)


/* csc02 */

#define CSL_VPS_CSC_CSC02_B1_MASK        (0x00001FFFu)
#define CSL_VPS_CSC_CSC02_B1_SHIFT       (0x00000000u)

#define CSL_VPS_CSC_CSC02_C1_MASK        (0x1FFF0000u)
#define CSL_VPS_CSC_CSC02_C1_SHIFT       (0x00000010u)


/* csc03 */

#define CSL_VPS_CSC_CSC03_A2_MASK        (0x00001FFFu)
#define CSL_VPS_CSC_CSC03_A2_SHIFT       (0x00000000u)

#define CSL_VPS_CSC_CSC03_B2_MASK        (0x1FFF0000u)
#define CSL_VPS_CSC_CSC03_B2_SHIFT       (0x00000010u)


/* csc04 */

#define CSL_VPS_CSC_CSC04_C2_MASK        (0x00001FFFu)
#define CSL_VPS_CSC_CSC04_C2_SHIFT       (0x00000000u)

#define CSL_VPS_CSC_CSC04_D0_MASK        (0x0FFF0000u)
#define CSL_VPS_CSC_CSC04_D0_SHIFT       (0x00000010u)


/* csc05 */

#define CSL_VPS_CSC_CSC05_BYPASS_MASK    (0x10000000u)
#define CSL_VPS_CSC_CSC05_BYPASS_SHIFT   (0x0000001Cu)

#define CSL_VPS_CSC_CSC05_D1_MASK        (0x00000FFFu)
#define CSL_VPS_CSC_CSC05_D1_SHIFT       (0x00000000u)

#define CSL_VPS_CSC_CSC05_D2_MASK        (0x0FFF0000u)
#define CSL_VPS_CSC_CSC05_D2_SHIFT       (0x00000010u)


#endif
