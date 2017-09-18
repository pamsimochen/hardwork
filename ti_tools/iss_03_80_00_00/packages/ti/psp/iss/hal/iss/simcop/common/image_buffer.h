/** ==================================================================
 *  @file   image_buffer.h                                                  
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
#ifndef _IMAGE_BUFFER_H
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _IMAGE_BUFFER_H

/*******************************************
 * Register set of SIMCOP Module
 * Spec Version
 *******************************************/
#include "./inc/iss_mem_map.h"

#define SIMCOP_IRQ_STATUS__0  (SIMCOP_START_ADDRESS +0x0024)

// IBuff Offset values
#define IBUFFER_A_OFST              (0x8000)
#define IBUFFER_B_OFST              (0x9000)
#define IBUFFER_C_OFST              (0xA000)
#define IBUFFER_D_OFST              (0xB000)
#define IBUFFER_E_OFST              (0xC000)
#define IBUFFER_F_OFST              (0xD000)
#define IBUFFER_G_OFST              (0xE000)
#define IBUFFER_H_OFST              (0xF000)
#define BITSTRBUFF_OFST	     (0x1000)

#define BITSTREAM_BUFFER_ADR      (SIMCOP_START_ADDRESS +BITSTRBUFF_OFST)
#define HUFF_MEM_ADR                     (SIMCOP_START_ADDRESS +0x2000)
#define IMXB_CMD_MEM_ADR             (SIMCOP_START_ADDRESS +0x4000)
#define IMXA_CMD_MEM_ADR             (SIMCOP_START_ADDRESS +0x6000)
#define QUANT_MEM_ADR                   (SIMCOP_START_ADDRESS +0x7000)
#define LDCLUT_MEM_ADR                  (SIMCOP_START_ADDRESS +0x7400)

#define IMAGE_BUFFER_LEN        (0x1000)
#define IMAGE_BUFFER_A_ADR            (SIMCOP_START_ADDRESS +IBUFFER_A_OFST)
#define IMAGE_BUFFER_B_ADR             (SIMCOP_START_ADDRESS +IBUFFER_B_OFST)
#define IMAGE_BUFFER_C_ADR             (SIMCOP_START_ADDRESS +IBUFFER_C_OFST)
#define IMAGE_BUFFER_D_ADR            (SIMCOP_START_ADDRESS +IBUFFER_D_OFST)
#define IMAGE_BUFFER_E_ADR             (SIMCOP_START_ADDRESS +IBUFFER_E_OFST)
#define IMAGE_BUFFER_F_ADR             (SIMCOP_START_ADDRESS +IBUFFER_F_OFST)
#define IMAGE_BUFFER_G_ADR            (SIMCOP_START_ADDRESS +IBUFFER_G_OFST)
#define IMAGE_BUFFER_H_ADR            (SIMCOP_START_ADDRESS +IBUFFER_H_OFST)
#define IMXA_COEFF_MEM_ADR          (SIMCOP_START_ADDRESS +0x10000)
#define IMXB_COEFF_MEM_ADR          (SIMCOP_START_ADDRESS +0x14000)

#define HMEM_WIDTH 8
#define HMEM_HEIGHT 256
#define HMEM_NUM_ENTRIES (HMEM_WIDTH * HMEM_HEIGHT)        /* 4096 bytes =
                                                            * 2048 short */

#define ENC_QMEM_WIDTH 8
#define ENC_QMEM_HEIGHT 16
#define ENC_QMEM_NUM_ENTRIES (ENC_QMEM_WIDTH * ENC_QMEM_HEIGHT)

#define DEC_QMEM_WIDTH 8
#define DEC_QMEM_HEIGHT 16
#define DEC_QMEM_NUM_ENTRIES (DEC_QMEM_WIDTH * DEC_QMEM_HEIGHT)

#define QMEM_NUM_ENTRIES (ENC_QMEM_NUM_ENTRIES + DEC_QMEM_NUM_ENTRIES)  /* 512 
                                                                         * bytes 
                                                                         * =
                                                                         * 256 
                                                                         * short 
                                                                         */

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
