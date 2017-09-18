/** ==================================================================
 *  @file   simcop.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/simcop/common/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/* ========================================================================== 
 * Copyright (c) Texas Instruments Inc , 2006 Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied provided
 * ========================================================================== */
#ifndef _SIMCOP_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _SIMCOP_H

/*****************************************************************************/
 /** \file simcop.h
 * 
 * \brief This file contains the Chip Description for OMAP4 SIMCOP (ARM side)
 * 
 *****************************************************************************/

#include "cslr.h"
#include "csl_types.h"

#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

#ifndef CSL_IDEF_INLINE
#define CSL_IDEF_INLINE static
#endif

/*****************************************************************************\
* Include files for all the modules in the device
\*****************************************************************************/

#include <ti/psp/iss/hal/iss/simcop/rot/cslr__rot_1.h>
#include <ti/psp/iss/hal/iss/simcop/dct/cslr__dct_001.h>
#include <ti/psp/iss/hal/iss/simcop/nsf/cslr__nsf.h>
#include <ti/psp/iss/hal/iss/simcop/vlcdj/cslr__vlcdj_1.h>
#include <ti/psp/iss/hal/iss/simcop/imx/cslr__imx4_1.h>

#include <ti/psp/iss/hal/iss/simcop/ldc/cslr__ldc.h>
#include <ti/psp/iss/hal/iss/simcop/simcop_global/cslr_simcop_2.h>
#include <ti/psp/iss/hal/iss/simcop/dma/cslr_simcop_dma_1.h>
/*****************************************************************************\
* Peripheral Instance counts
\*****************************************************************************/

#define CSL_ROT_PER_CNT                  1
#define CSL_DCT_PER_CNT                  1
#define CSL_LDC_PER_CNT                  1
#define CSL_NSF_PER_CNT                  1
#define CSL_VLCDJ_PER_CNT                1
#define CSL_IMX_PER_CNT                  2
#define CSL_SIMCOP_PER_CNT               1
#define CSL_COPDMA_PER_CNT               1
#define CSL_SET  1
#define CSL_RESET 0

/*****************************************************************************\
* Peripheral Base Address
\*****************************************************************************/
#ifndef TARGET
#define ISS_BASE_ADDRESS ISS_REGS_BASE_ADDR                // 0x50000000
#else
#define ISS_BASE_ADDRESS ISS_REGS_BASE_ADDR
#endif
#define CSL_SIMCOP_BASE_ADDRESS (ISS_BASE_ADDRESS+0x00020000)

#define CSL_SIMCOP_REGS            ((CSL_SimcopRegsOvly) CSL_SIMCOP_BASE_ADDRESS )
#define CSL_LDC_0_REGS             ((CSL_LdcRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x0100))
#define CSL_IMX_A_REGS             ((CSL_ImxRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x0180))
#define CSL_IMX_B_REGS             ((CSL_ImxRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x01C0))
#define CSL_COPDMA_0_REGS          ((CSL_SimcopDmaRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x0200))
#define CSL_VLCDJ_0_REGS           ((CSL_VlcdjRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x0600))
#define CSL_ROT_0_REGS             ((CSL_RotRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x0700))
#define CSL_NSF_2_REGS             ((CSL_NsfRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x0780))
#define CSL_DCT_0_REGS             ((CSL_DctRegsOvly) (CSL_SIMCOP_BASE_ADDRESS+0x0800))

#define CSL_SIMCOP_BUFS            ((CSL_SimcopBufsOvly) CSL_BITSTREAM_BUFFER )

#define CSL_BITSTREAM_BUFFER       ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x1000))
#define CSL_BITSTREAM_BUFFER_LEN   (0x1000)
#define CSL_HUFFMAN_TABLES         ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x2000))
#define CSL_HUFFMAN_TABLES_LEN     (0x1000)
#define CSL_IMXB_CMDMEM            ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x4000))
#define CSL_IMXB_CMDMEM_LEN        (0x2000)
#define CSL_IMXA_CMDMEM            ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x6000))
#define CSL_IMXA_CMDMEM_LEN        (0x1000)
#define CSL_QUANT_TABLES           ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x7000))
#define CSL_QUANT_TABLES_LEN       (0x400)
#define CSL_LDC_LUT                ((CSL_LdcLutOvly) (CSL_SIMCOP_BASE_ADDRESS+0x7400))
#define CSL_LDC_LUT_LEN            (0x200)

#define CSL_IMGBUF_LEN             (0x1000)
#define CSL_IMGBUF_A               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x8000))
#define CSL_IMGBUF_B               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x9000))
#define CSL_IMGBUF_C               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0xA000))
#define CSL_IMGBUF_D               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0xB000))
#define CSL_IMGBUF_E               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0xC000))
#define CSL_IMGBUF_F               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0xD000))
#define CSL_IMGBUF_G               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0xE000))
#define CSL_IMGBUF_H               ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0xF000))
#define CSL_IMXA_COEFFMEM          ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x10000))
#define CSL_IMXA_COEFFMEM_LEN      (0x4000)
#define CSL_IMXB_COEFFMEM          ((Uint32) (CSL_SIMCOP_BASE_ADDRESS+0x14000))
#define CSL_IMXB_COEFFMEM_LEN      (0x4000)

// IBuff Offset values
#define CSL_IMGBUF_A_OFST              (0x8000)
#define CSL_IMGBUF_B_OFST              (0x9000)
#define CSL_IMGBUF_C_OFST              (0xA000)
#define CSL_IMGBUF_D_OFST              (0xB000)
#define CSL_IMGBUF_E_OFST              (0xC000)
#define CSL_IMGBUF_F_OFST              (0xD000)
#define CSL_IMGBUF_G_OFST              (0xE000)
#define CSL_IMGBUF_H_OFST              (0xF000)
#define CSL_BITSTRBUFF_OFST	     (0x1000)

/** \brief ROT Module Instances
* 
*/
typedef enum {
    CSL_ROT_ANY = -1,  /**< <b>: Any instance of ROT module</b> */
    CSL_ROT_0 = 0      /**< <b>: CCDC Instance 0</b> */
} CSL_RotNum;

/** \brief CFALD Module Instances
* 
*/
typedef enum {
    CSL_DCT_ANY = -1,  /**< <b>: Any instance of DCT module</b> */
    CSL_DCT_0 = 0      /**< <b>: DCT Instance 0</b> */
} CSL_DctNum;

/** \brief LDC Module Instances
* 
*/
typedef enum {
    CSL_LDC_ANY = -1,  /**< <b>: Any instance of LDC module</b> */
    CSL_LDC_0 = 0      /**< <b>: LDC Instance 0</b> */
} CSL_LdcNum;

/** \brief NSF Module Instances
* 
*/
typedef enum {
    CSL_NSF_ANY = -1,  /**< <b>: Any instance of NSF module</b> */
    CSL_NSF_0 = 0      /**< <b>: NSF Instance 0</b> */
} CSL_NsfNum;

/** \brief VLCDJ Module Instances
* 
*/
typedef enum {
    CSL_VLCDJ_ANY = -1,  /**< <b>: Any instance of VLCDJ module</b> */
    CSL_VLCDJ_0 = 0      /**< <b>: VLCDJ Instance 0</b> */
} CSL_VlcdjNum;

/** \brief IMX Module Instances
* 
*/
typedef enum {
    CSL_IMX_ANY = -1,  /**< <b>: Any instance of IMX module</b> */
    CSL_IMX_A = 0,      /**< <b>: IMX Instance 0</b> */
    CSL_IMX_B = 1      /**< <b>: IMX Instance 1</b> */
} CSL_ImxInstNum;

/** \brief SIMCOP Module Instances
* 
*/
typedef enum {
    CSL_SIMCOP_ANY = -1,  /**< <b>: Any instance of SIMCOP module</b> */
    CSL_SIMCOP_0 = 0      /**< <b>: SIMCOP Instance 0</b> */
} CSL_SimcopNum;

/** \brief COPDMA Module Instances
* 
*/
typedef enum {
    CSL_COPDMA_ANY = -1,  /**< <b>: Any instance of COPDMA module</b> */
    CSL_COPDMA_0 = 0      /**< <b>: COPDMA Instance 0</b> */
} CSL_SimcopDmaNum;

/********************************************************************************
*
* CSL_HwAutoGatingType enumeration for setting Autogating on or off
*
* @param CSL_SIMCOP_AUTOGATING_OFF    Disable Autogating, Free running mode
* @param CSL_SIMCOP_AUTOGATING_ON     Enable Autogating, Gated Clock mode
*
*********************************************************************************/
typedef enum {
    CSL_SIMCOP_AUTOGATING_OFF = 0,
    CSL_SIMCOP_AUTOGATING_ON = 1
} CSL_HwAutoGatingType;

/********************************************************************************
*
* CSL_HwEnableType enumeration for setting Autogating on or off
*
* @param CSL_SIMCOP_HW_DISABLE    Disable Hardware
* @param CSL_SIMCOP_HW_ENABLE     Enable Hardware
*
*********************************************************************************/
typedef enum {
    CSL_SIMCOP_HW_DISABLE = 0,
    CSL_SIMCOP_HW_ENABLE = 1
} CSL_HwEnableType;

/* TRIGGER SOURCE TYPES */
/********************************************************************************
*
* CSL_HwTrgSrcType enumeration for checking trigger source
*
* @param CSL_SIMCOP_HW_TRIG_MMR_WRITE         Source is MMR Write
* @param CSL_SIMCOP_HW_TRIG_HWSTART_SIGNAL    Source is Hardware Start Signal
*
*********************************************************************************/
typedef enum {
    CSL_SIMCOP_HW_TRIG_MMR_WRITE = 0,
    CSL_SIMCOP_HW_TRIG_HWSTART_SIGNAL = 1
} CSL_HwTrgSrcType;

/********************************************************************************
*
* CSL_HwStatus enumeration for checking busy status of ROT
*
* @param CSL_SIMCOP_HWSTATUS_IDLE    HW is Idle
* @param CSL_SIMCOP_HWSTATUS_BUSY    HW is Busy
*
*********************************************************************************/
typedef enum {
    CSL_SIMCOP_HWSTATUS_IDLE = 0,
    CSL_SIMCOP_HWSTATUS_BUSY = 1
} CSL_HwStatusType;

/********************************************************************************
*
* CSL_HwIntEnableType enumeration for setting Autogating on or off
*
* @param CSL_SIMCOP_HW_INT_DISABLE    Disable Interrupt
* @param CSL_SIMCOP_HW_INT_ENABLE     Enable Interrupt
*
*********************************************************************************/
typedef enum {
    CSL_SIMCOP_HW_INT_DISABLE = 0,
    CSL_SIMCOP_HW_INT_ENABLE = 1
} CSL_HwIntEnableType;

/*****************************************************************************\
* Peripheral Overlay Structures
\*****************************************************************************/

typedef volatile CSL_RotRegs *CSL_RotRegsOvly;

typedef volatile CSL_DctRegs *CSL_DctRegsOvly;

typedef volatile CSL_LdcRegs *CSL_LdcRegsOvly;

typedef volatile CSL_NsfRegs *CSL_NsfRegsOvly;

typedef volatile CSL_VlcdjRegs *CSL_VlcdjRegsOvly;

typedef volatile CSL_Imx4Regs *CSL_ImxRegsOvly;

typedef volatile CSL_SimcopRegs *CSL_SimcopRegsOvly;

typedef volatile CSL_Simcop_dmaRegs *CSL_SimcopDmaRegsOvly;

/**************************************************************************\
* Buffers and Coefficient memory Structure
\**************************************************************************/
typedef struct {
    volatile Uint16 LUT[CSL_LDC_LUT_LEN];
} CSL_LdcLut;

typedef struct {
    volatile Uint8 BITSTREAM[CSL_BITSTREAM_BUFFER_LEN];
    volatile Uint8 HUFFMAN[CSL_HUFFMAN_TABLES_LEN];
    volatile Uint8 RSVD0[4096];
    volatile Uint8 IMX_B_CMDMEM[CSL_IMXB_CMDMEM_LEN];
    volatile Uint8 IMX_A_CMDMEM[CSL_IMXA_CMDMEM_LEN];
    volatile Uint8 QUANT[CSL_QUANT_TABLES_LEN];
    volatile Uint8 RSVD1[512];
    volatile Uint8 LDC_LUT[CSL_LDC_LUT_LEN];
    volatile Uint8 RSVD2[2560];
    volatile Uint8 IMGBUFA[CSL_IMGBUF_LEN];
    volatile Uint8 IMGBUFB[CSL_IMGBUF_LEN];
    volatile Uint8 IMGBUFC[CSL_IMGBUF_LEN];
    volatile Uint8 IMGBUFD[CSL_IMGBUF_LEN];
    volatile Uint8 IMGBUFE[CSL_IMGBUF_LEN];
    volatile Uint8 IMGBUFF[CSL_IMGBUF_LEN];
    volatile Uint8 IMGBUFG[CSL_IMGBUF_LEN];
    volatile Uint8 IMGBUFH[CSL_IMGBUF_LEN];
    volatile Uint8 IMX_A_COEFFMEM[CSL_IMXA_COEFFMEM_LEN];
    volatile Uint8 IMX_B_COEFFMEM[CSL_IMXB_COEFFMEM_LEN];
} CSL_SimcopBufs;

/*****************************************************************************\
* Buffers and Coefficient memories
\*****************************************************************************/
typedef volatile CSL_LdcLut *CSL_LdcLutOvly;

typedef volatile CSL_SimcopBufs *CSL_SimcopBufsOvly;

/* 
 * extern void * _CSL_rotlookup[]; extern void * _CSL_dctlookup[]; extern
 * void * _CSL_nsflookup[]; extern void * _CSL_ldclookup[]; extern void *
 * _CSL_vlcdjlookup[]; extern void * _CSL_imxlookup[]; extern void *
 * _CSL_simcoplookup[]; extern void * _CSL_copdmalookup[]; */
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
