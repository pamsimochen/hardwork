/*
//----------------------------------------------------------------------------
//           Copyright (C) 2005 Texas Instruments Incorporated.
//                           All Rights Reserved
//----------------------------------------------------------------------------
// MODULE NAME... Centaurus sIMCOP Chip Support
// FILENAME...... imcopcsl.h
// DATE CREATED.. May 2011
// PROJECT....... Centaurus
//----------------------------------------------------------------------------
// HISTORY:
//   
//
//----------------------------------------------------------------------------
// DESCRIPTION:  Provides the hardware specific information for DM360
//               
//----------------------------------------------------------------------------
*/

#ifndef __IMCOPCSL_CENTAURUS_H__
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define __IMCOPCSL_CENTAURUS_H__

#define _CENTAURUS

#include <csl_types.h>
#include "simcop.h"

#define WORDS (sizeof(Uint16))
#define BYTES (sizeof(Uint8))

typedef short* DmPtr;

// Macro used in dmaSetup.c for code compatibility with older device
// here it doesn't do anything
#define DM350MSL_getPhysAddr(a) (a)

// Align pointer to number of words specified by variable align
#define memAlign(adr, align)   ( (DmPtr) (( ((Uint32)(adr)+(((align)*WORDS)-1))/((align*WORDS)) ) * ((align)*WORDS) ) )
 
/* iMX Info */

#define NO_OF_MACS       4
/* dma info */
#define DMHAL_BURSTWIDTH 2
/* Width of input data for most efficient computation on DSP */
#define DSPALIGNWIDTH 4

/* These types are used by CACHE_<..> function */
typedef unsigned long   CACHE_Addr_t ;
typedef unsigned long   CACHE_Size_t ;
typedef void (*CACHE_Cb)(CACHE_Addr_t addr, CACHE_Size_t size);

typedef struct {

  CACHE_Cb wbCb;     /* Callback for D-cache write back API */
  CACHE_Cb invCb;    /* Callback for D-cache invalidate API */
  CACHE_Cb wbInvCb;  /* Callback for D-cache write back and invalidate API */

} CACHE_InitPrm_t;

/* Initialize callbacks to write back invalidate D-cache */
void CACHE_init(CACHE_InitPrm_t *pPrm);

/*
  Write back data from cache

  addr    buffer address
  size    size of buffer
*/
void CACHE_writeBack(CACHE_Addr_t addr, CACHE_Size_t size, unsigned short wait);

/*
  Invalidate data from cache

  addr    buffer address
  size    size of buffer
*/
void CACHE_invalidate(CACHE_Addr_t addr, CACHE_Size_t size, unsigned short wait);

/*
  Write back and invalidate data from cache

  addr    buffer address
  size    size of buffer
*/
void CACHE_wbInv(CACHE_Addr_t addr, CACHE_Size_t size, unsigned short wait);

/* --------------------------------- */
/*     image buffer definitions      */
/* --------------------------------- */

/*
    Define image buffers, coefficient and command memories base addresses
*/

/* Byte addresses */
#define IMGBUF_BASE         (CSL_SIMCOP_BASE_ADDRESS+0x8000)   /* Image buffer A&B ping-pong view */
#define IMGBUF_A_BASE       IMGBUF_BASE
#define IMGBUF_B_BASE       (IMGBUF_BASE + 0x1000)
#define IMGBUF_C_BASE       (IMGBUF_BASE + 0x2000)
#define IMGBUF_D_BASE       (IMGBUF_BASE + 0x3000)
#define IMGBUF_E_BASE       (IMGBUF_BASE + 0x4000)
#define IMGBUF_F_BASE       (IMGBUF_BASE + 0x5000)
#define IMGBUF_G_BASE       (IMGBUF_BASE + 0x6000)
#define IMGBUF_H_BASE       (IMGBUF_BASE + 0x7000)
#define COEFFBUF_BASE       (CSL_SIMCOP_BASE_ADDRESS+0x10000)   /* coefficient memory */
#define COEFF1BUF_BASE      (COEFFBUF_BASE + 0x04000)   // iMX1 coef memory

#define CMDBUF_BASE         (CSL_SIMCOP_BASE_ADDRESS+0x6000)   /* command memory */
#define CMD0BUF_BASE		CMDBUF_BASE
#define CMD1BUF_BASE        (CSL_SIMCOP_BASE_ADDRESS+0x4000)   // iMX1 command memory

/* For compatibility with old naming convention */
#define    IMGBUFA          IMGBUF_A_BASE
#define    IMGBUFB          IMGBUF_B_BASE
#define    IMGBUFC          IMGBUF_C_BASE
#define    IMGBUFD          IMGBUF_D_BASE
#define    IMGBUFE          IMGBUF_E_BASE
#define    iMXCOEFFBUF      IMXCOEFFBUF_BASE
#define    iMXCMDBUF        IMXCMDBUF_BASE
#define    iMX0COEFFBUF      IMXCOEFFBUF_BASE
#define    iMX0CMDBUF        IMXCMDBUF_BASE
#define    iMX1COEFFBUF      IMX1COEFFBUF_BASE
#define    iMX1CMDBUF        IMX1CMDBUF_BASE
#define    VLCDQBUF         QIQBUF_BASE
#define    VLCDHUFFBUF      VLCDBUF_BASE


#define IMX0COEFFBUF_BASE   IMXCOEFFBUF_BASE
#define IMX0CMDBUF_BASE     IMXCMDBUF_BASE

#define IMXBUF_BASE         IMXCMDBUF_BASE      /* iMX Coefficient/Command buffer base address */
#define IMX0BUF_BASE        IMXCMDBUF_BASE      /* iMX Coefficient/Command buffer base address */
#define SEQ_PMEM_BASE       SEQMEM_BASE         /* Sequencer Program memory base address */
#define IMXBUF_COEFF_BASE   IMXCOEFFBUF_BASE  
#define IMX0BUF_COEFF_BASE   IMXCOEFFBUF_BASE      

#define IMGBUF_BASEIMX      0x0000
#define IMXCOEFFBUF_BASEIMX 0x8000 // to be updated

/*
    Define size symbols
*/
/* Below symbols are used to express size in number of 16-bits words */
#define IMGBUF_SIZE         0x800   /* 4 kW in full view */
#define IMGBUF_A_SIZE       0x800   /* Each image buffer is 2 kW */
#define IMGBUF_B_SIZE       0x800
#define IMGBUF_C_SIZE       0x800
#define IMGBUF_D_SIZE       0x800
#define IMGBUF_E_SIZE       0x800
#define IMGBUF_F_SIZE       0x800
#define IMGBUF_G_SIZE       0x800
#define IMGBUF_H_SIZE       0x800
#define COEFFBUF_SIZE       0x2000   /* Coefficient memory is 8 kW */
#define COEFF1BUF_SIZE      0x2000   /* Coefficient memory is 8 kW */
#define CMDBUF_SIZE         0x800   /* Command memory is 2 kW */
#define CMD1BUF_SIZE        0x1000   /* Command memory is 2 kW */

/* Below symbols are used to express size in number bytes */
#define IMGBUF_SIZE_BYTES       0x1000   /* 8 kb in full view */
#define IMGBUF_A_SIZE_BYTES     0x1000   /* Each image buffer is 4 kb */
#define IMGBUF_B_SIZE_BYTES     0x1000
#define IMGBUF_C_SIZE_BYTES     0x1000
#define IMGBUF_D_SIZE_BYTES     0x1000
#define IMGBUF_E_SIZE_BYTES     0x1000
#define IMGBUF_F_SIZE_BYTES     0x1000
#define IMGBUF_G_SIZE_BYTES     0x1000
#define IMGBUF_H_SIZE_BYTES     0x1000
#define COEFFBUF_SIZE_BYTES     0x4000   /* Coefficient memory is 16 kb */
#define COEFF1BUFSIZE_BYTES     0x4000   /* Coefficient memory is 16 kb */
#define CMDBUF_SIZE_BYTES       0x1000   /* Command memory is 4 kb */
#define CMD1BUF_SIZE_BYTES      0x2000   /* Command memory is 8 kb */

#define    IMGBUFASIZE      IMGBUF_A_SIZE
#define    IMGBUFBSIZE      IMGBUF_B_SIZE
#define    IMGBUFCSIZE      IMGBUF_C_SIZE
#define    IMGBUFDSIZE      IMGBUF_D_SIZE
#define    IMGBUFESIZE      IMGBUF_E_SIZE
#define    iMXCOEFFBUFSIZE  IMXCOEFFBUF_SIZE
#define    iMXCMDBUFSIZE    IMXCMDBUF_SIZE
#define    iMX0COEFFBUFSIZE  IMXCOEFFBUF_SIZE
#define    iMX0CMDBUFSIZE    IMXCMDBUF_SIZE
#define    iMX1COEFFBUFSIZE iMX1COEFFBUF_SIZE
#define    iMX1CMDBUFSIZE   iMX1CMDBUF_SIZE

/* ----------------------------------------- */
/* image buffer switches  for SwitchBuffer() */      
/* ----------------------------------------- */ 
#define OTHERMEMSHIFT 32
#define CMDMEMSHIFT   32
#define COEFMEMSHIFT  32

//                                                                                                                            IMG BUF
//										                                                                              31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 */
#define    SELIMGBUFA       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_MASK /*               0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 0 0 0 0 0 0 0 1 1 1 */
#define    SELIMGBUFB       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_MASK /*               0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 0 0 0 1 1 1 0 0 0 0 */
#define    SELIMGBUFC       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_MASK /*               0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1 1 1 0 0 0 0 0 0 0 0 */
#define    SELIMGBUFD       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_MASK /*               0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  1  1  0  0 0 0 0 0 0 0 0 0 0 0 */
#define    SELIMGBUFE       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_MASK /*               0  0  0  0  0  0  0  0  0  0  0  0  0  1  1  1  0  0  0  0  0  0 0 0 0 0 0 0 0 0 0 0 */
#define    SELIMGBUFF       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_MASK /*               0  0  0  0  0  0  0  0  0  1  1  1  0  0  0  0  0  0  0  0  0  0 0 0 0 0 0 0 0 0 0 0 */
#define    SELIMGBUFG       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_MASK /*               0  0  0  0  0  1  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0 0 0 0 0 0 0 0 0 0 0 */
#define    SELIMGBUFH       CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_MASK /*               0  1  1  1  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 0 0 0 0 0 0 0 0 0 0 */

//                                                                                                                            COEF and CMD BUF
//                                                                                                          31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0 */
#define    SELCOEFBUF       ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR_MASK<<COEFMEMSHIFT)/*  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 0 0 0 0 0 0 0 1 1 1 */
#define    SELCOEF1BUF      ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR_MASK<<COEFMEMSHIFT)/*  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0 0 0 0 1 1 1 0 0 0 0 */
#define    SELCMDBUF        ((Uint64)CSL_SIMCOP_CTRL_IMX_A_CMD_MASK<<CMDMEMSHIFT)                        /*  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  1  0  0 0 0 0 0 0 0 0 0 0 0 */
#define    SELCMD1BUF       ((Uint64)CSL_SIMCOP_CTRL_IMX_B_CMD_MASK<<CMDMEMSHIFT)                        /*  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  1  0  0  0  0 0 0 0 0 0 0 0 0 0 0 */

#define    SELALLBUF        0xFFFFFFFFFFFFFFFF

#define    SELCMD0BUF SELCMDBUF 
#define    SELCOEF0BUF SELCOEFBUF

/* ----------------------------------------- */
/* image buffer switches  for SwitchBuffer() */      
/* ----------------------------------------- */ 
#define    IMGBUFADSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_SHIFT) // 0
#define    IMGBUFAiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_IMXA_IMBUFF << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_SHIFT) // 2
#define    IMGBUFAiMX_L     IMGBUFAiMX
#define    IMGBUFAiMX_H     IMGBUFAiMX
#define    IMGBUFAiMX0      IMGBUFAiMX
#define    IMGBUFAiMX0_L    IMGBUFAiMX_L
#define    IMGBUFAiMX0_H    IMGBUFAiMX_H
#define    IMGBUFAiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_IMXB_IMBUFF << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_SHIFT) // 3
#define    IMGBUFAiMX1_L    IMGBUFAiMX1
#define    IMGBUFAiMX1_H    IMGBUFAiMX1
#define    IMGBUFADMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_A_OVR_SHIFT)  // 1
#define    IMGBUFANONE      IMGBUFADSP
#define    IMGBUFACOPRB     IMGBUFANONE

#define    IMGBUFBDSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_SHIFT) // 0
#define    IMGBUFBiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_IMX_A << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_SHIFT) // 0x20
#define    IMGBUFBiMX_L     IMGBUFBiMX
#define    IMGBUFBiMX_H     IMGBUFBiMX
#define    IMGBUFBiMX0      IMGBUFBiMX
#define    IMGBUFBiMX0_L    IMGBUFBiMX_L
#define    IMGBUFBiMX0_H    IMGBUFBiMX_H
#define    IMGBUFBiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_IMX_B << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_SHIFT) // 0x30
#define    IMGBUFBiMX1_L    IMGBUFBiMX1
#define    IMGBUFBiMX1_H    IMGBUFBiMX1
#define    IMGBUFBDMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_B_OVR_SHIFT)  // 0x10
#define    IMGBUFBNONE      IMGBUFBDSP
#define    IMGBUFBCOPRB     IMGBUFBNONE

#define    IMGBUFCDSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_SHIFT) // 0
#define    IMGBUFCiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_IMX_A << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_SHIFT) // 0x200
#define    IMGBUFCiMX_L     IMGBUFCiMX
#define    IMGBUFCiMX_H     IMGBUFCiMX
#define    IMGBUFCiMX0      IMGBUFCiMX
#define    IMGBUFCiMX0_L    IMGBUFCiMX_L
#define    IMGBUFCiMX0_H    IMGBUFCiMX_H
#define    IMGBUFCiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_IMX_B << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_SHIFT) // 0x300
#define    IMGBUFCiMX1_L    IMGBUFCiMX1
#define    IMGBUFCiMX1_H    IMGBUFCiMX1
#define    IMGBUFCDMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_C_OVR_SHIFT)  // 0x100
#define    IMGBUFCNONE      IMGBUFCDSP
#define    IMGBUFCCOPRB     IMGBUFCNONE

#define    IMGBUFDDSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_SHIFT) // 0
#define    IMGBUFDiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_IMX_A << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_SHIFT) // 0x2000
#define    IMGBUFDiMX_L     IMGBUFDiMX
#define    IMGBUFDiMX_H     IMGBUFDiMX
#define    IMGBUFDiMX0      IMGBUFDiMX
#define    IMGBUFDiMX0_L    IMGBUFDiMX_L
#define    IMGBUFDiMX0_H    IMGBUFDiMX_H
#define    IMGBUFDiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_IMX_B << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_SHIFT) // 0x3000
#define    IMGBUFDiMX1_L    IMGBUFDiMX1
#define    IMGBUFDiMX1_H    IMGBUFDiMX1
#define    IMGBUFDDMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_D_OVR_SHIFT)  // 0x1000
#define    IMGBUFDNONE      IMGBUFDDSP
#define    IMGBUFDCOPRB     IMGBUFDNONE

#define    IMGBUFEDSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_SHIFT) // 0
#define    IMGBUFEiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_IMX_A << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_SHIFT) // 0x20000
#define    IMGBUFEiMX_L     IMGBUFEiMX
#define    IMGBUFEiMX_H     IMGBUFEiMX
#define    IMGBUFEiMX0      IMGBUFEiMX
#define    IMGBUFEiMX0_L    IMGBUFEiMX_L
#define    IMGBUFEiMX0_H    IMGBUFEiMX_H
#define    IMGBUFEiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_IMX_B << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_SHIFT) // 0x30000
#define    IMGBUFEiMX1_L    IMGBUFEiMX1
#define    IMGBUFEiMX1_H    IMGBUFEiMX1
#define    IMGBUFEDMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_E_OVR_SHIFT)  // 0x10000
#define    IMGBUFENONE      IMGBUFEDSP
#define    IMGBUFECOPRB     IMGBUFENONE

#define    IMGBUFFDSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_SHIFT) // 0
#define    IMGBUFFiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_IMX_A << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_SHIFT) // 0x200000
#define    IMGBUFFiMX_L     IMGBUFFiMX
#define    IMGBUFFiMX_H     IMGBUFFiMX
#define    IMGBUFFiMX0      IMGBUFFiMX
#define    IMGBUFFiMX0_L    IMGBUFFiMX_L
#define    IMGBUFFiMX0_H    IMGBUFFiMX_H
#define    IMGBUFFiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_IMX_B << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_SHIFT) // 0x300000
#define    IMGBUFFiMX1_L    IMGBUFFiMX1
#define    IMGBUFFiMX1_H    IMGBUFFiMX1
#define    IMGBUFFDMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_F_OVR_SHIFT)  // 0x100000
#define    IMGBUFFNONE      IMGBUFFDSP
#define    IMGBUFFCOPRB     IMGBUFFNONE

#define    IMGBUFGDSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_SHIFT) // 0
#define    IMGBUFGiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_IMX_A << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_SHIFT) // 0x2000000
#define    IMGBUFGiMX_L     IMGBUFGiMX
#define    IMGBUFGiMX_H     IMGBUFGiMX
#define    IMGBUFGiMX0      IMGBUFGiMX
#define    IMGBUFGiMX0_L    IMGBUFGiMX_L
#define    IMGBUFGiMX0_H    IMGBUFGiMX_H
#define    IMGBUFGiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_IMX_B << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_SHIFT) // 0x3000000
#define    IMGBUFGiMX1_L    IMGBUFGiMX1
#define    IMGBUFGiMX1_H    IMGBUFGiMX1
#define    IMGBUFGDMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_G_OVR_SHIFT)  // 0x1000000
#define    IMGBUFGNONE      IMGBUFGDSP
#define    IMGBUFGCOPRB     IMGBUFGNONE

#define    IMGBUFHDSP       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_COP_BUS << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_SHIFT) // 0
#define    IMGBUFHiMX       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_IMX_A << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_SHIFT) // 0x20000000
#define    IMGBUFHiMX_L     IMGBUFHiMX
#define    IMGBUFHiMX_H     IMGBUFHiMX
#define    IMGBUFHiMX0      IMGBUFHiMX
#define    IMGBUFHiMX0_L    IMGBUFHiMX_L
#define    IMGBUFHiMX0_H    IMGBUFHiMX_H
#define    IMGBUFHiMX1      (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_IMX_B << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_SHIFT) // 0x30000000
#define    IMGBUFHiMX1_L    IMGBUFHiMX1
#define    IMGBUFHiMX1_H    IMGBUFHiMX1
#define    IMGBUFHDMA       (CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_SIMCOP_DMA << CSL_SIMCOP_HWSEQ_STEP_SWITCH_OVERRIDE_IMBUFF_H_OVR_SHIFT)  // 0x10000000
#define    IMGBUFHNONE      IMGBUFHDSP
#define    IMGBUFHCOPRB     IMGBUFHNONE


#define    iMXCMDBUFDSP     ((Uint64)CSL_SIMCOP_CTRL_IMX_A_CMD_COPROCESSOR_BUS << (CSL_SIMCOP_CTRL_IMX_A_CMD_SHIFT + OTHERMEMSHIFT)) // 0
#define    iMXCMDBUFiMX     ((Uint64)CSL_SIMCOP_CTRL_IMX_A_CMD_IMX_A_IR_IW << (CSL_SIMCOP_CTRL_IMX_A_CMD_SHIFT + OTHERMEMSHIFT)) // 1 << (12 + 32)
#define    iMXCMDBUFiMX1    ((Uint64)CSL_SIMCOP_CTRL_IMX_A_CMD_IMX_B_IR_IW << (CSL_SIMCOP_CTRL_IMX_A_CMD_SHIFT + OTHERMEMSHIFT)) // 2 << (12 + 32)
#define    iMXCMDBUFNONE    iMXCMDBUFDSP
#define    iMXCMDBUFCOPRB   iMXCMDBUFNONE
#define    iMX0CMDBUFDSP     iMXCMDBUFDSP
#define    iMX0CMDBUFiMX0    iMXCMDBUFiMX
#define    iMX0CMDBUFiMX1    iMXCMDBUFiMX1
#define    iMX0CMDBUFDMA     iMXCMDBUFDMA   
#define    iMX0CMDBUFNONE    iMXCMDBUFNONE 
#define    iMX0CMDBUFCOPRB   iMXCMDBUFCOPRB 

#define    iMX1CMDBUFDSP     ((Uint64)CSL_SIMCOP_CTRL_IMX_B_CMD_COPROCESSOR_BUS << (CSL_SIMCOP_CTRL_IMX_B_CMD_SHIFT + OTHERMEMSHIFT)) // 0
#define    iMX1CMDBUFiMX1    ((Uint64)CSL_SIMCOP_CTRL_IMX_B_CMD_IMX_B_IR_IW << (CSL_SIMCOP_CTRL_IMX_B_CMD_SHIFT + OTHERMEMSHIFT)) // 1 << (14 + 32)
#define    iMX1CMDBUFNONE    iMX1CMDBUFDSP
#define    iMX1CMDBUFCOPRB   iMX1CMDBUFNONE 

#define    iMXCOEFFBUFDSP   ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR_COPROCESSOR_BUS << (CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR_SHIFT + OTHERMEMSHIFT)) // 0
#define    iMXCOEFFBUFiMX   ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR_IMX_A << (CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR_SHIFT + OTHERMEMSHIFT)) // 2 << (0 + 32)
#define    iMXCOEFFBUFDMA   ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR_SIMCOP_DMA << (CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_A_OVR_SHIFT + OTHERMEMSHIFT)) // 1 << (0 + 32)
#define    iMXCOEFFBUFNONE  iMXCOEFFBUFDSP
#define    iMXCOEFFBUFCOPRB iMXCOEFFBUFNONE
#define    iMX0COEFFBUFDSP   iMXCOEFFBUFDSP
#define    iMX0COEFFBUFiMX0  iMXCOEFFBUFiMX
#define    iMX0COEFFBUFDMA   iMXCOEFFBUFDMA
#define    iMX0COEFFBUFNONE  iMXCOEFFBUFNONE
#define    iMX0COEFFBUFCOPRB iMXCOEFFBUFCOPRB 

#define    iMX1COEFFBUFDSP   ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR_COPROCESSOR_BUS << (CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR_SHIFT + OTHERMEMSHIFT)) // 0
#define    iMX1COEFFBUFiMX1  ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR_IMX_B << (CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR_SHIFT + OTHERMEMSHIFT)) // 3 << (4 + 32)
#define    iMX1COEFFBUFDMA   ((Uint64)CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR_SIMCOP_DMA << (CSL_SIMCOP_HWSEQ_STEP_CTRL2_OVERRIDE_COEFF_B_OVR_SHIFT + OTHERMEMSHIFT)) // 1 << (4 + 32)
#define    iMX1COEFFBUFNONE  iMX1COEFFBUFDSP
#define    iMX1COEFFBUFCOPRB iMX1COEFFBUFNONE

#define    SEQBUFDSP        0
#define    SEQBUFSEQ        0
#define    SEQBUFDMA        0
#define    SEQBUFNONE       0
#define    SEQBUFAUTO       0

#define    ALLBUFDSP        (IMGBUFADSP | IMGBUFBDSP | IMGBUFCDSP | IMGBUFDDSP | IMGBUFEDSP | IMGBUFFDSP | IMGBUFGDSP | IMGBUFHDSP | iMX0CMDBUFDSP |  iMX0COEFFBUFDSP | iMX1CMDBUFDSP |  iMX1COEFFBUFDSP )
#define    ALLBUFNONE       ALLBUFDSP


/*
 * Define overlaid registers structure's addresses
*/
#define CSL_IMX0_REGS			      CSL_IMX_A_REGS
#define CSL_IMX1_REGS			      CSL_IMX_B_REGS

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*  __DMCSL_CENTAURUS_H__  */





