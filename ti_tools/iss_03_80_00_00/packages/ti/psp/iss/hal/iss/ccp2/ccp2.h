/** ==================================================================
 *  @file   ccp2.h                                                  
 *                                                                    
 *  @path   /ti/psp/iss/hal/iss/ccp2/                                                  
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
/* File Name :Ccp2.h */
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

#ifndef _CSLR_CCP2_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _CSLR_CCP2_H_
/****************************************************************
 *  INCLUDE FILES                                                 
 *****************************************************************/

#include "inc/ccp2_reg.h"
#include "inc/ccp2_utils.h"
#include <ti/psp/iss/hal/iss/iss_common/iss_common.h>

/* The Following are the generic params for CCP2 */
#define CCP2_NUM_DEVICES 1
#define CCP2_DRV_NUMCHANS 4

// #define CCP2_BASE_ADDRESS 0x50001C00
#define CCP2_BASE_ADDRESS (ISS_REGS_BASE_ADDR + 0x1C00)

typedef uint8 Channel_Number;

typedef volatile CSL_Ccp2Regs *ccp2_regs_ovly;

typedef uint32 PhyAddress_t;

#define CCP2_RETRY_CNT  10
#define ENABLE 1
#define DISABLE 0

/* FOR INTERRUPTS */
#define MAX_CCP2_INTERRUPTS  14
#define MAX_CCP2_CHANNELS 3
#define MAX_CCP2_LCX_INTERRUPTS 12
#define MAX_CCP2_LCM_INTERRUPTS 2

/* ================================================================ */
/* Description:- struct to hold device specific data */
/* ======================================================================= */
typedef struct {

    uint8 opened;

} ccp2_dev_data_t;

/* ======================================================================= */
 /**
  *  Ccp2_Module structure for ccp2_module 
  *   @param deviceinUse number of active ccp2 devices in use 
  */
/* ======================================================================= */

typedef struct {
    int deviceinUse[CCP2_NUM_DEVICES];
} Ccp2_Module;

/* ======================================================================= */
 /**
  * ccp2_generic_params structure for  the generic parameters of CCP2
  *   @param ocp ocp port & DPCM there if this is enabled
  *   @param no_of_channels number of logical channels of ccp2
  *   @param fifo_depth Output FIFO size in multiple of 64 bits
 */
/* ======================================================================= */
typedef struct {
    uint16 ocp;
    uint16 no_of_channels;
    uint16 fifo_depth;
} ccp2_generic_params;

/* ======================================================================= */
/**
 * CCP2_CONTROL_BIT  enumeration for ccp2 control (enable\disable)
  *   @param CONTROL_DISABLE disables ccp2 module
  *   @param CONTROL_ENABLE  enables CCp2 module
 */
/* ======================================================================= */

typedef enum {
    CONTROL_DISABLE = 0,
    CONTROL_ENABLE = 1
} CCP2_CONTROL_BIT;

/* Structure for Code Register for Logical Channel X of CCP2 */
/* ======================================================================= */
 /**
 *ccp2_lcx_code structure for synchronisation codes for logical channel X
  *   @param LCX_CODE_FEC frame end sync code identifier for  LOGICAL CHANNEL X
  *   @param LCX_CODE_LSC line start  sync code identifier for  LOGICAL CHANNEL X
  *   @param LCX_CODE_LEC line end    sync code identifier for  LOGICAL CHANNEL X
  *   @param LCX_CODE_FSC  frame start sync code identifier for  LOGICAL CHANNEL X
  *   @param  LCX_CODE_CHAN_ID  ID  for  LOGICAL CHANNEL X
 */
/* ======================================================================= */

typedef struct {
    uint16 LCX_CODE_LSC;
    uint16 LCX_CODE_LEC;
    uint16 LCX_CODE_FSC;
    uint16 LCX_CODE_FEC;
    uint8 LCX_CODE_CHAN_ID;
} ccp2_lcx_code;

/* Structure for Stat_Start for logical channel X of CCP2 */
/* ======================================================================= */
 /**
  * ccp2_lcx_stat_start structure for status line start register for channel X
  *   @param LCX_STAT_START_EOF the number of EOF status lines
  *   @param LCX_STAT_START_SOF the number of SOF status lines
 */
/* ======================================================================= */

typedef struct {
    uint32 LCX_STAT_START_SOF;
    uint32 LCX_STAT_START_EOF;
} ccp2_lcx_stat_start;

typedef enum {
    CCSP2_LCM_OCPERROR = 1,                                /* These two are
                                                            * LCM only
                                                            * interrupts */
    CCP2_LCM_EOF = 0,
    CCP2_LC0_MAX_IRQ = 12,
    CCP2_LC0_FS_IRQ = 11,
    CCP2_LC0_LE_IRQ = 10,
    CCP2_LC0_LS_IRQ = 9,
    CCP2_LC0_FE_IRQ = 8,
    CCP2_LC0_COUNT_IRQ = 7,
    CCP2_Reserved = 6,
    CCP2_LC0_FIFO_OVF_IRQ = 5,
    CCP2_LC0_CRC_IRQ = 4,
    CCP2_LC0_FSP_IRQ = 3,
    CCP2_LC0_FW_IRQ = 2,
    CCP2_LC0_FSC_IRQ = 1,
    CCP2_LC0_SSC_IRQ = 0
} ccp2_interrupt_id_t;

/* Structure for Stat_Size for logical channel X of CCP2 */
/* ======================================================================= */
 /**
  * ccp2_lcx_stat_size structure for 
  * @param LCX_STAT_SIZE_EOF vertical position of the EOF status lines in
       regards of the FSC sync code
  * @param  LCX_STAT_SIZE_SOF the vertical position of the EOF status lines in regards of the FSC sync code
 */
/* ======================================================================= */

typedef struct {
    uint32 LCX_STAT_SIZE_SOF;
    uint32 LCX_STAT_SIZE_EOF;
} ccp2_lcx_stat_size;

/* Structure for SOF & EOF , LCX DAT REGISTERS */
/* ======================================================================= */
 /**
  * ccp2_lcx_dat_addr structure for logical channel X addres registers
  *   @param LCX_SOF_ADDR SOF STATUS LINE MEM ADDRESS for logical channel X
  *   @param LCX_EOF_ADDR EOF STATUS LINE MEM ADDRESS for logical channel X
  *   @param LCX_DAT_START DATA START REGISTER for logical channel X
  *   @param LCX_DAT_SIZE DATA SIZE REGISTER for logical channel X
  *   @param LCX_DAT_PING_ADDR DATA MEM PING ADDRESS REGISTER for logical channel X
  *   @param LCX_DAT_PONG_ADDR DATA MEM PONG ADDRESS REGISTER for logical channel X
  *   @param LCX_DAT_OFST DATA MEM ADDRESS OFFSET REGISTER for logical channel X
   */
/* ======================================================================= */

typedef struct {
    uint32 LCX_SOF_ADDR;
    uint32 LCX_EOF_ADDR;
    uint32 LCX_DAT_START;
    uint32 LCX_DAT_SIZE;
    uint32 LCX_DAT_PING_ADDR;
    uint32 LCX_DAT_PONG_ADDR;
    uint32 LCX_DAT_OFST;
} ccp2_lcx_dat_addr;

/* Structure for Global Control of CCP2 */
/* ======================================================================= */
/**
 * CCP2_CTRL_FRAME enumeration for frame disabling 
  *   @param FRAME_DIS_IMME frame disable immediately
  *   @param FRAME_DIS_AFTER_FEC frame disabled after frame end code recieved
 */
/* ======================================================================= */

typedef enum {
    FRAME_DIS_IMME = 0,
    FRAME_DIS_AFTER_FEC = 1
} CCP2_CTRL_FRAME;

/* ======================================================================= */
  /**
   * CCP2_CTRL_PHY_SEL enumeration for Physical layer protocol selection
  *   @param DATA_CLOCK_PHY_SEL clock physical layer
  *   @param DATA_STROBE_PHY_SEL strobe physical layer
 */
/* ======================================================================= */

typedef enum {
    DATA_CLOCK_PHY_SEL = 0,
    DATA_STROBE_PHY_SEL = 1
} CCP2_CTRL_PHY_SEL;

/* ======================================================================= */
/* CCP2_CTRL_IO_OUT_SEL enumeration for output mode selection * @param
 * OUTPUT_SERIAL output is serial * @param OUTPUT_PARALLEL output is parallel */
 /* ======================================================================= */

typedef enum {
    OUTPUT_SERIAL = 0,
    OUTPUT_PARALLEL = 1
} CCP2_CTRL_IO_OUT_SEL;

/* ======================================================================= */
/**
 * CCP2_CTRL_IF_EN enumeration for  interface disabling
  *   @param INTERFACE_DISABLE_IMME interface disabled immediately
  *   @param INTERFACE_DISABLE_AFTER_FEC interface disabled fater fec code recieved
 */
/* ======================================================================= */

typedef enum {
    INTERFACE_DISABLE_IMME = 0,
    INTERFACE_DISABLE_AFTER_FEC = 1
} CCP2_CTRL_IF_EN;

/* ======================================================================= */
/**
 * CCP2_CTRL_BURST  enumeration for ccp2 burst size selection
  *   @param CTRL_BURST_ONE_X_SIXTY_FOUR
  *   @param CTRL_BURST_TWO_X_SIXTY_FOUR
  *   @param CTRL_BURST_FOUR_X_SXITY_FOUR
  *   @param CTRL_BURST_EIGHT_X_SIXTY_FOUR 
  *   @param CTRL_BURST_SIXTEEN_X_SIXTY_FOUR
 */
/* ======================================================================= */

typedef enum {
    CTRL_BURST_ONE_X_SIXTY_FOUR = 0,
    CTRL_BURST_TWO_X_SIXTY_FOUR = 1,
    CTRL_BURST_FOUR_X_SXITY_FOUR = 2,
    CTRL_BURST_EIGHT_X_SIXTY_FOUR = 3,
    CTRL_BURST_SIXTEEN_X_SIXTY_FOUR = 4
} CCP2_CTRL_BURST;

/* ======================================================================= */
/**
 * CCP2_CTRL_VP_CLK_POL VP enumeration for clock polarity
  *   @param RISING rising edge
  *   @param FALLING falling edge
 */
/* ======================================================================= */

typedef enum {
    RISING = 1,
    FALLING = 0
} CCP2_CTRL_VP_CLK_POL;

/* ======================================================================= */
/**
 * CCP2_CTRL_INV enumeration for Strobe / clock inversion control signal
  *   @param INV
  *   @param NON_INV
 */
/* ======================================================================= */

typedef enum {
    INV = 1,
    NON_INV = 0
} CCP2_CTRL_INV;

/* ======================================================================= */
/**
 * CCP2_MODE enumeration for  ccp2 mode selection
  *   @param MIPI_MODE
  *   @param NONMIPI_CCP2_MODE
 */
/* ======================================================================= */

typedef enum {
    MIPI_MODE = 0,
    NONMIPI_CCP2_MODE = 1
} CCP2_MODE;

/* ======================================================================= */
/**
 * CCP2_POSTED  enumeration for enabling posting
 * @param POSTED
 * @param NON_POSTED
 */
/* ======================================================================= */

typedef enum {
    POSTED = 1,
    NON_POSTED = 0
} CCP2_POSTED;

typedef enum {
    VP_CLOCK_DISABLE,
    VP_CLOCK_ENABLE
} CCP2_CTRL_VP_CLK_FORCE;

/* ======================================================================= */
/* CCP2_CTRL1_LEVH enumeration for ontrolling generation of MFlag * @param
 * FIFO_LEV_LESS_THAN_LEVL_FOR_LEVH * @param LEVL_LESS_THAN_FIFO_LEV_FOR_LEVH
 * * @param LEVH_LESS_THAN_FIFO_LEV_FOR_LEVH */
/* ======================================================================= */

typedef enum {
    FIFO_LEV_LESS_THAN_LEVL_FOR_LEVH = 0,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVH = 0x2u,
    LEVH_LESS_THAN_FIFO_LEV_FOR_LEVH = 0x3u
} CCP2_CTRL1_LEVH;

/* ======================================================================= */
/**
 * CCP2_CTRL1_LEVL enumeration for controlling generation of MFlag
  *   @param FIFO_LEV_LESS_THAN_LEVL_FOR_LEVL
  *   @param LEVL_LESS_THAN_FIFO_LEV_FOR_LEVL
  *   @param LEVH_LESS_THAN_FIFO_LEV_FOR_LEVL
 */
/* ======================================================================= */

typedef enum {
    FIFO_LEV_LESS_THAN_LEVL_FOR_LEVL = 0,
    LEVL_LESS_THAN_FIFO_LEV_FOR_LEVL = 0x2u,
    LEVH_LESS_THAN_FIFO_LEV_FOR_LEVL = 0x3u
} CCP2_CTRL1_LEVL;

/* ======================================================================= */
/**
 * CCP2_CTRL1_BLANKING   enumeration for frame blanking cycles
  *   @param FOUR_CYCLES
  *   @param SIXTEEN_CYCLES
  *   @param SIXTYFOUR_CYCLES
  *   @param FREE_RUNNING
 */
/* ======================================================================= */

typedef enum {
    FOUR_CYCLES = 0,
    SIXTEEN_CYCLES = 0x1u,
    SIXTYFOUR_CYCLES = 0x2u,
    FREE_RUNNING = 0x3u
} CCP2_CTRL1_BLANKING;

/* ======================================================================= */
 /**
  * ccp2_global_ctrl structure for 
  *   @param VP_CLK_POL
  *   @param CCP2_CTRL_VP_ONLY_EN
  *   @param INV
  *   @param CCP2_CTRL_DEBUG_EN
  *   @param BURST
  *   @param MODE
  *   @param FRAME
  *   @param IO_OUT_SEL
  *   @param PHY_SEL
  *   @param  IF_EN
  *   @param POSTED
  *   @param FRACDIV
  *   @param LEVH
  *   @param LEVL
  *   @param BLANKING
 */
/* ======================================================================= */

typedef struct {

    CCP2_CTRL_VP_CLK_POL VP_CLK_POL;
    uint8 CCP2_CTRL_VP_ONLY_EN;
    CCP2_CTRL_INV INV;
    uint8 CCP2_CTRL_DEBUG_EN;
    CCP2_CTRL_BURST BURST;
    CCP2_MODE MODE;
    CCP2_CTRL_FRAME FRAME;
    CCP2_CTRL_IO_OUT_SEL IO_OUT_SEL;
    CCP2_CTRL_PHY_SEL PHY_SEL;
    CCP2_CTRL_IF_EN IF_EN;
    CCP2_POSTED POSTED;
    uint32 FRACDIV;
    CCP2_CTRL_VP_CLK_FORCE VP_CLK_FORCE_ON;
    /* THE BELOW 3 PARAMS BELONG TO CTRL1 REGISTER */
    CCP2_CTRL1_LEVH LEVH;
    CCP2_CTRL1_LEVL LEVL;
    CCP2_CTRL1_BLANKING BLANKING;

} ccp2_global_ctrl;

/* Structure to identify the field causing the interrupt */
/* ======================================================================= */
/**
 * CCP2_LCX_IRQ_NUM enumeration for 
  *   @param FS_IRQ frame start irq id 
  *   @param LE_IRQ line end irq id 
  *   @param LS_IRQ line start irq id 
  *   @param FE_IRQ frame end irq id 
  *   @param COUNT_IRQ count reached irq id 
  *   @param FIFO_OVF_IRQ fifo overflow irq id 
  *   @param CRC_IRQ crc error irq id
  *   @param FSP_IRQ fsp error irq id
  *   @param FW_IRQ frame width irq id
  *   @param FSC_IRQ frame sync code  error irq id
  *   @param SSC_IRQ shifted sync code irq id
 */
/* ======================================================================= */

typedef enum {
    FS_IRQ = 0,
    LE_IRQ = 1,
    LS_IRQ = 2,
    FE_IRQ = 3,
    COUNT_IRQ = 4,
    FIFO_OVF_IRQ = 5,
    CRC_IRQ = 6,
    FSP_IRQ = 7,
    FW_IRQ = 8,
    FSC_IRQ = 9,
    SSC_IRQ = 10,
    OCP_ERR_IRQ = 11
} CCP2_LCX_IRQ_NUM;

/* Structure for Enabling interrupts & checking status for Logical Channels */

/* ======================================================================= */
 /**
  * ccp2_lcx1_irqenable_status structure for 
  *   @param LCX1_FS_IRQ logical channel 0(or 2) frame start irq
  *   @param LCX1_LE_IRQ logical channel 0(or 2) line end irq
  *   @param LCX1_LS_IRQ logical channel 0(or 2) line start irq
  *   @param LCX1_FE_IRQ logical channel 0(or 2) frame end irq
  *   @param  LCX1_COUNT_IRQ logical channel 0(or 2)  count reached irq
  *   @param LCX1_FIFO_OVF_IRQ logical channel 0(or 2) fifo overflow irq
  *   @param LCX1_CRC_IRQ logical channel 0(or 2) crc error irq
  *   @param LCX1_FSP_IRQ logical channel 0(or 2) fsp error irq
  *   @param LCX1_FW_IRQ logical channel 0(or 2) frame width irq
  *   @param LCX1_FSC_IRQ logical channel 0(or 2) frame sync code error irq
  *   @param LCX1_SSC_IRQ logical channel 0(or 2) shifted sync code irq
  *   @param LCX_FS_IRQ logical channel 1(or 3) frame start irq
  *   @param LCX_LE_IRQ logical channel 1(or 3) line end irq
  *   @param LCX_LS_IRQ logical channel 1(or 3) line start irq
  *   @param LCX_FE_IRQ logical channel 1(or 3) frame end irq
  *   @param LCX_COUNT_IRQ logical channel 1(or 3)  count reached irq
  *   @param LCX_FIFO_OVF_IRQ logical channel 1(or 3) fifo overflow irq
  *   @param LCX_CRC_IRQ logical channel 1(or 3) crc error irq
  *   @param LCX_FSP_IRQ logical channel 1(or 3) fsp error irq
  *   @param LCX_FW_IRQ logical channel 1(or 3) frame width irq
  *   @param LCX_FSC_IRQ logical channel 1(or 3) frame sync code error irq
  *   @param LCX_SSC_IRQ logical channel 1(or 3) shifted sync code irq
  */
/* ======================================================================= */

typedef struct {
    uint8 LCX1_FS_IRQ;
    uint8 LCX1_LE_IRQ;
    uint8 LCX1_LS_IRQ;
    uint8 LCX1_FE_IRQ;
    uint8 LCX1_COUNT_IRQ;
    uint8 LCX1_FIFO_OVF_IRQ;
    uint8 LCX1_CRC_IRQ;
    uint8 LCX1_FSP_IRQ;
    uint8 LCX1_FW_IRQ;
    uint8 LCX1_FSC_IRQ;
    uint8 LCX1_SSC_IRQ;
    uint8 LCX_FS_IRQ;
    uint8 LCX_LE_IRQ;
    uint8 LCX_LS_IRQ;
    uint8 LCX_FE_IRQ;
    uint8 LCX_COUNT_IRQ;
    uint8 LCX_FIFO_OVF_IRQ;
    uint8 LCX_CRC_IRQ;
    uint8 LCX_FSP_IRQ;
    uint8 LCX_FW_IRQ;
    uint8 LCX_FSC_IRQ;
    uint8 LCX_SSC_IRQ;
} ccp2_lcx1_irqenable_status;

/* Structure for Enabling & Checking interrupts for Memory Channel */
/* ======================================================================= */
/**
 * CCP2_LCM_IRQ_NUM enumeration for the read from memory irq IDs
  *   @param OCPERROR_IRQ ocp eror irq id
  *   @param EOF_IRQ end of frame irq id
 */
/* ======================================================================= */

typedef enum {
    OCPERROR_IRQ = 0,
    EOF_IRQ = 1
} CCP2_LCM_IRQ_NUM;

/* ======================================================================= */
/**
 * ccp2_lcm_irqenable_status  structure for memory channel irq enabling
  *   @param LCM_OCPERROR memory channel ocp error
  *   @param LCM_EOF memory channel end of frame id
 */
/* ======================================================================= */

typedef struct {
    uint8 LCM_OCPERROR;
    uint8 LCM_EOF;
} ccp2_lcm_irqenable_status;

/* Structure for controlling power modes */
/* ======================================================================= */
/**
 * CCP2_SYSCONFIG ccp2 power mode selection
  *   @param MSTANDBY_MODE
  *   @param SOFT_RESET
  *   @param AUTO_IDLE
  *   @param RESET_DONE
 */
/* ======================================================================= */

typedef enum {
    MSTANDBY_MODE,
    SOFT_RESET,
    AUTO_IDLE,
    RESET_DONE
} CCP2_SYSCONFIG;

/* ======================================================================= */
/**
 * CCP2_LCX_COUNT_UNLOCK logical channel count unlock
  *   @param UNLOCK
  *   @param LOCK 
 */
/* ======================================================================= */

typedef enum {
    UNLOCK = 1,
    LOCK = 0
} CCP2_LCX_COUNT_UNLOCK;

/* ======================================================================= */
/**
 * CCP2_LCX_CTRL_FORMAT enumeration for logical channel X format selection
  *   @param YUV422LE
  *   @param YUV422BE
  *   @param YUV420 
  *   @param YUV422VPRAW8VP
  *   @param RGB444
  *   @param  RGB565
  *   @param RGB888
  *   @param RGB888EXP32
  *   @param RAW6EXP8
  *   @param RAW6DPCM10EXP16
  *   @param RAW6DPCM10VP
  *   @param RAW10RAW6DPCM
  *   @param RAW7EXP8
  *   @param RAW7DPCM10EXP16
  *   @param RAW7DPCM10VP
  *   @param RAW10RAW6DPCMEXP8
  *   @param RAW10RAW8
  *   @param RAW10
  *   @param RAW10EXP16
  *   @param RAW10VP
  *   @param RAW10RAW7DPCMEXP8
  *   @param RAW8
  *   @param RAW8DPCM10EXP16
  *   @param RAW8DPCM10VP
  *   @param RAW10RAW7DPCM
  *   @param RAW12
  *   @param RAW12EXP16
  *   @param RAW12VP
  *   @param RAW10RAWDPCM
  *   @param JPEG8FSP
  *   @param JPEG8
    
 */
/* ======================================================================= */

typedef enum {
    CCP2_CONTEXT_0 = 0,
    CCP2_CONTEXT_1 = 1,
    CCP2_CONTEXT_2 = 2,
    CCP2_CONTEXT_3 = 3,
    CCP2_CONTEXT_M = 4,
    CCP2_CONTEXT_MAX = 5
} CCP2_CONTEXT_T;

typedef enum {
    YUV422LE = 0,
    YUV422BE = 1,
    _YUV420 = 2,
    YUV422VPRAW8VP = 3,
    RGB444 = 4,
    RGB565 = 5,
    RGB888 = 6,
    RGB888EXP32 = 7,
    RAW6EXP8 = 8,
    RAW6DPCM10EXP16 = 9,
    RAW6DPCM10VP = 10,
    RAW10RAW6DPCM = 11,
    RAW7EXP8 = 12,
    RAW7DPCM10EXP16 = 13,
    RAW7DPCM10VP = 14,
    RAW10RAW6DPCMEXP8 = 15,
    RAW8 = 16,
    RAW8DPCM10EXP16 = 17,
    RAW8DPCM10VP = 18,
    RAW10RAW7DPCM = 19,
    RAW10 = 20,
    RAW10EXP16 = 21,
    RAW10VP = 22,
    RAW10RAW7DPCMEXP8 = 23,
    RAW12 = 24,
    RAW12EXP16 = 25,
    RAW12VP = 26,
    RAW10RAWDPCM = 27,
    JPEG8FSP = 28,
    JPEG8 = 29,
    RAW10RAW8 = 30
} CCP2_LCX_CTRL_FORMAT;

/* ======================================================================= */
/**
 * CCP2_LCX_DPCM_PRED enumeration for memory channel predictor selection
  *   @param ADV_PRED
  *   @param SIMPLE_PRED
 */
/* ======================================================================= */

typedef enum {
    ADV_PRED = 1,
    SIMPLE_PRED = 0
} CCP2_LCX_DPCM_PRED;

/* ======================================================================= */
/**
 * CCP2_LCX_PING_PONG enumeration for ping pong buffer selection
  *   @param PING 
  *   @param PONG
 */
/* ======================================================================= */

typedef enum {
    PING = 0,
    PONG = 1
} CCP2_LCX_PING_PONG;

/* Structure for Logical channel X of CCP2 */

/* ======================================================================= */
 /**
  * ccp2_lcx_chan structure for  
  *   @param CCP2_LCX_ENABLE_CTRL logical channel X  enable
  *   @param CCP2_LCX_REGION_EN_CTRL logical channel X region enable
  *   @param CCP2_LCX_CRC_EN_CTRL logical channel X crc enable
  *   @param LCX_FORMAT logical channel X format
  *   @param LCX_ALPHA logical channel X Alpha value for RGB888 and RBG444
  *   @param COUNT_UNLOCK Unlock writes to the COUNT bit field.
  *   @param PING_PONG Indicates whether the PING or PONG destination
address
  *   @param DPCM_PRED Selects the DPCM predictor to be used for the RAW6
and RAW7 data formats
  *   @param COUNT Sets the number of frame to acquire
 */
/* ======================================================================= */

typedef struct {

    CCP2_CONTROL_BIT CCP2_LCX_ENABLE_CTRL;
    CCP2_CONTROL_BIT CCP2_LCX_REGION_EN_CTRL;
    CCP2_CONTROL_BIT CCP2_LCX_CRC_EN_CTRL;

    CCP2_LCX_CTRL_FORMAT LCX_FORMAT;
    uint32 LCX_ALPHA;
    CCP2_LCX_COUNT_UNLOCK COUNT_UNLOCK;

    CCP2_LCX_PING_PONG PING_PONG;
    CCP2_LCX_DPCM_PRED DPCM_PRED;
    uint32 COUNT;

} ccp2_lcx_chan;

/* ======================================================================= */
 /**
  * ccp2_chan_enable_t structure for 
  *   @param chan_num handle to  Channel_Number
  *   @param chan_lcx_code instance of object ccp2_lcx_code
  *   @param chan_lcx_stat_start instance of object ccp2_lcx_stat_start
  *   @param chan_lcx_stat_size instance of object ccp2_lcx_stat_size
  *   @param chan_lcx_dat_addr instance of object ccp2_lcx_dat_addr 
  *   @param chan_lcx_ctrl instance of object ccp2_lcx_chan
  *   @param gen_ric_params instance of object  ccp2_generic_params 
   */
/* ======================================================================= */

typedef struct {
    Channel_Number chan_num;
    ccp2_lcx_code chan_lcx_code;
    ccp2_lcx_stat_start chan_lcx_stat_start;
    ccp2_lcx_stat_size chan_lcx_stat_size;
    ccp2_lcx_dat_addr chan_lcx_dat_addr;
    ccp2_lcx_chan chan_lcx_ctrl;
    ccp2_generic_params gen_ric_params;
} ccp2_chan_enable_t;

/* ======================================================================= */
 /**
  * ccp2_path_enable_t structure for 
  *   @param ctrl_global instance of  object ccp2_global_ctrl
  *   @param gen_params instance of object ccp2_generic_params
  */
/* ======================================================================= */

typedef struct {
    ccp2_global_ctrl ctrl_global;
    ccp2_generic_params gen_params;
} ccp2_path_enable_t;

/* ======================================================================= */
/**
 * CCP2_LCM_DST_PACK enumeration for destination pack enabling
  *   @param DST_PACK_DISABLED  destination pack disabled
  *   @param DST_PACK_ENABLED destination pack enabled
  */
/* ======================================================================= */

typedef enum {
    DST_PACK_DISABLED = 0,
    DST_PACK_ENABLED = 1
} CCP2_LCM_DST_PACK;

/* ======================================================================= */
/**
 * CCP2_LCM_SRC_PACK enumeration for source pack enabling
  *   @param SRC_PACK_ENABLED source pack enabled
  *   @param SRC_PACK_DISABLED source pack disabled
  */
/* ======================================================================= */

typedef enum {
    SRC_PACK_ENABLED = 1,
    SRC_PACK_DISABLED = 0
} CCP2_LCM_SRC_PACK;

/* ======================================================================= */
/**
 * CCP2_LCM_DPCM_PRED  enumeration for meory channel DPCM predictor
  *   @param ADV_PREDICTOR advanced predictor selected
  *   @param SIMPLE_PREDICTOR simple predictor selected
  */
/* ======================================================================= */

typedef enum {
    ADV_PREDICTOR = 1,
    SIMPLE_PREDICTOR = 0
} CCP2_LCM_DPCM_PRED;

/* ======================================================================= */
/**
 * CCP2_DST_PORT enumeration for destination porrt of ccp2
  *   @param SEND_TO_VIDEO_NO_COMPRESSION destination is video port without compression
  *   @param SEND_TO_MEMORY destination is memory
  */
/* ======================================================================= */

typedef enum {
    SEND_TO_VIDEO_NO_COMPRESSION = 0,
    SEND_TO_MEMORY = 1
} CCP2_DST_PORT;

/* ======================================================================= */
 /**
  * CCP2_LCM_READ_THROTTLE enumeration for memory read max data rate speed
  *   @param FULL_SPEED
  *   @param HALF_SPEED
  *   @param ONE_FOURTH_SPEED
  *   @param ONE_EIGHT_SPEED
  */
/* ======================================================================= */

typedef enum {
    FULL_SPEED = 0,
    HALF_SPEED = 0X1,
    ONE_FOURTH_SPEED = 0X2,
    ONE_EIGHT_SPEED = 0X3
} CCP2_LCM_READ_THROTTLE;

/* ======================================================================= */
/**  CCP2_LCM_FORMAT  enumeration of formats for memory to memory transfer
  *   @param LCM_RAW6
  *   @param LCM_RAW7
  *   @param LCM_RAW8
  *   @param LCM_RAW10 
  *   @param LCM_RAW12 
  *   @param LCM_RAW14
  */
/* ======================================================================= */

typedef enum {
    LCM_RAW6 = 0X0,
    LCM_RAW7 = 0X1,
    LCM_RAW8 = 0X2,
    LCM_RAW10 = 0X3,
    LCM_RAW12 = 0X4,
    LCM_RAW14 = 0X5
} CCP2_LCM_FORMAT;

/* ======================================================================= */
/**  CCP2_LCM_DST_COMPR  enumeration of compressions for memory store
  *   @param LCM_DST_COMPR_NO
  *   @param LCM_DST_COMPR_ALAW
  *   @param LCM_DST_COMPR_DPCM
  */
/* ======================================================================= */

typedef enum {
    LCM_DST_COMPR_NO,
    LCM_DST_COMPR_ALAW,
    LCM_DST_COMPR_DPCM
} CCP2_LCM_DST_COMPR;

/* ======================================================================= */
/**  CCP2_LCM_SRC_DECOMPR  enumeration of decompressions for memory read
  *   @param LCM_SRC_DECOMPR_NO
  *   @param LCM_SRC_DECOMPR_ALAW
  *   @param LCM_SRC_DECOMPR_DPCM_RAW10
  *   @param LCM_SRC_DECOMPR_DPCM_RAW12
  */
/* ======================================================================= */

typedef enum {
    LCM_SRC_DECOMPR_NO,
    LCM_SRC_DECOMPR_ALAW,
    LCM_SRC_DECOMPR_DPCM_RAW10,
    LCM_SRC_DECOMPR_DPCM_RAW12
} CCP2_LCM_SRC_DECOMPR;

/* Structure for LCM register control */
/* ======================================================================= */
 /**
  * ccp2_lcm_ctrl structure for Control  for the memory channel
  *   @param LCM_DST_PACK Data PACKING ENABLING  before it's send to memory.
  *   @param LCM_DST_DPCM_PRED Selects the DPCM predictor to be used for the RAW6
  *               and RAW7 data formats for DPCM encode 
  *   @param LCM_DST_COMPRESSION Enable DPCM compression of data sent to memory
  *   @param LCM_DST_FORMAT Output format selection
  *   @param LCM_SRC_PACK Data stored in memory is packed or unpaked
  *   @param LCM_SRC_DPCM_PRED Selects the DPCM predictor to be used for the RAW6 & RAW 7
  *   @param LCM_SRC_DECOPRESS Enable DPCM decompression of incoming data
  *   @param LCM_SRC_FORMAT Data format of the data stored in memory
  *   @param LCM_BURST_SIZE Defines the burst size of the master read port
  *   @param LCM_READ_THROTTLE Limit maximum data read speed for memory to memory
  *              operation
  *   @param DST_PORT  Select the destination port
  *   @param DEBUG debug value 
  */
/* ======================================================================= */

typedef struct {
    CCP2_LCM_DST_PACK LCM_DST_PACK;
    CCP2_LCM_DPCM_PRED LCM_DST_DPCM_PRED;
    CCP2_LCM_DST_COMPR LCM_DST_COMPRESSION;
    CCP2_LCM_FORMAT LCM_DST_FORMAT;
    CCP2_LCM_SRC_PACK LCM_SRC_PACK;
    CCP2_LCM_DPCM_PRED LCM_SRC_DPCM_PRED;
    CCP2_LCM_SRC_DECOMPR LCM_SRC_DECOPRESS;
    CCP2_LCM_FORMAT LCM_SRC_FORMAT;
    CCP2_CTRL_BURST LCM_BURST_SIZE;
    CCP2_LCM_READ_THROTTLE LCM_READ_THROTTLE;
    CCP2_DST_PORT DST_PORT;
    uint32 debug;

} ccp2_lcm_ctrl;

/* Structure for LCM register config */
/* ======================================================================= */
 /**
  * ccp2_lcm_config structure for configuring memory channel
  *   @param LCM_VSIZE_COUNT Defines the line count to be read from memory
  *   @param LCM_HSIZE_COUNT Horizontal count of pixels to output after the skipped
pixels
  *   @param LCM_HSIZE_SKIP Horizontal count of pixels to skip after the start of the
line
  *   @param LCM_PREFETCH_SWORDS 64 bit words to read from memory for each line of the
image.
  *   @param LCM_SRC_ADDR Memory channel source address
  *   @param LCM_SRC_OFST Memory channel source offset
  *   @param LCM_DST_ADDR Memory channel destination address.
  *   @param LCM_DST_OFST Memory channel destination offset
  */
/* ======================================================================= */

typedef struct {
    uint32 LCM_VSIZE_COUNT;
    uint32 LCM_HSIZE_COUNT;
    uint32 LCM_HSIZE_SKIP;
    uint16 LCM_PREFETCH_SWORDS;
    uint32 LCM_SRC_ADDR;
    uint32 LCM_SRC_OFST;
    uint32 LCM_DST_ADDR;
    uint32 LCM_DST_OFST;
} ccp2_lcm_config;

/* Structure for LCM ENABLING */

/* ======================================================================= */
 /**
  * ccp2_lcm_enable_t structure for enabling memory channel
  *   @param mem_chan_ctrl instance of ccp2_lcm_ctrl
  *   @param mem_chan_config instancce of ccp2_lcm_config
  */
/* ======================================================================= */

typedef struct {
    ccp2_lcm_ctrl mem_chan_ctrl;
    ccp2_lcm_config mem_chan_config;
} ccp2_lcm_enable_t;

/* ======================================================================= */
/**
 * ccp2_global_ctrl_flag enumeration for ccp global control flag
  *   @param MEMORY_READ for read from memory channel
  *   @param CHANNEL_WRITE for write through channel
  */
/* ======================================================================= */

typedef enum {
    MEMORY_READ = 0,
    CHANNEL_WRITE = 1
} ccp2_global_ctrl_flag;

/* ======================================================================= */
/**
 * CCP2_DISABLE enumeration for ccp2 disable
  *   @param CCP2_DISABLE_AFTER_FEC diable ccp2 after frame end code
  *   @param CCP2_DISABLE_IMMEDIATELY diable ccp2 immediately
  */
/* ======================================================================= */

typedef enum {
    CCP2_DISABLE_AFTER_FEC = 0,
    CCP2_DISABLE_IMMEDIATELY = 1
} CCP2_DISABLE;

/*************************************FOR INTERRUPT HANDLING****************************************/
/* 
 * typedef struct{ Swi_Handle swi_handle; Hwi_Handle hwi_handle;
 * 
 * }ccp2_dev_data; */

/* ======================================================================= */
/**
 * ccp2_global_interrupt_id enumeration for ccp2 global interrupt id
  *   @param CCP2_LCM_IRQ ccp2 memory channel irq id
  *   @param CCP2_CHANNEL_IRQ ccp2 logical channel irq  id
  */
/* ======================================================================= */

typedef enum {
    CCP2_LCM_IRQ = 1,
    CCP2_CHANNEL_IRQ = 0
} ccp2_global_interrupt_id;

/* ======================================================================= */
/**
 * ccp2_lcx_interrupt_id enumeration for ccp2 logical channel interrupt id
  *   @param CCP2_LCX_OCPERROR_IRQ logical channel X ocp error irq id 
  *   @param CCP2_LCX_FS_IRQ logical channel X frame start irq id
  *   @param CCP2_LCX_LE_IRQ logical channel X line end irq id
  *   @param CCP2_LCX_LS_IRQ logical channel X line start irq id
  *   @param CCP2_LCX_FE_IRQ logical channel X frame end irq id
  *   @param CCP2_LCX_COUNT_IRQ logical channel X count irq id
  *   @param CCP2_LCX_FIFO_OVF_IRQ logical channel X fifo over flow irq id
  *   @param CCP2_LCX_CRC_IRQ logical channel X CRC error irq id
  *   @param CCP2_LCX_FSP_IRQ logical channel X frame start processing irq id
  *   @param CCP2_LCX_FW_IRQ  logical channel X frame width irq id
  *   @param CCP2_LCX_FSC_IRQ logical channel X false sync code irq id
  *   @param CCP2_LCX_SSC_IRQ  logical channel X shifted sync code irq id
    
  */
/* ======================================================================= */

typedef enum {
    // Mapped according to the bit-fields in CCP2_LCX_IRQSTATUS
    CCP2_LCX_OCPERROR_IRQ = 15,
    CCP2_LCX_FS_IRQ = 11,
    CCP2_LCX_LE_IRQ = 10,
    CCP2_LCX_LS_IRQ = 9,
    CCP2_LCX_FE_IRQ = 8,
    CCP2_LCX_COUNT_IRQ = 7,
    CCP2_LCX_FIFO_OVF_IRQ = 5,
    CCP2_LCX_CRC_IRQ = 4,
    CCP2_LCX_FSP_IRQ = 3,
    CCP2_LCX_FW_IRQ = 2,
    CCP2_LCX_FSC_IRQ = 1,
    CCP2_LCX_SSC_IRQ = 0
} ccp2_lcx_interrupt_id;

/* ======================================================================= */
/**
 * ccp2_lcm_interrupt_id  enumeration for ccp2 memory channel interrupt id
  *   @param CCP2_LCM_EOF_IRQ memory channel end of frame irq id 
  *   @param CCP2_LCM_OCPERROR_IRQ memory channel ocp error irq id
  */
/* ======================================================================= */

typedef enum {
    // Mapped according to the bit-fields in CCP2_LCM_IRQSTATUS
    CCP2_LCM_EOF_IRQ = 1,
    CCP2_LCM_OCPERROR_IRQ = 0
} ccp2_lcm_interrupt_id;

typedef void (*ccp2_callback_t) (CCP2_RETURN status, uint32 arg1, void *arg2);

/* ======================================================================= */
 /**
  * ccp2_irq_handle_t structure for handle to ccp2 irq.
  *   @param callback_func callback function
  *   @param arg1 uint32 argument
  *   @param arg2 void argument
  */
/* ======================================================================= */

typedef struct {
    ccp2_callback_t callback_func;
    uint32 arg1;
    void *arg2;

} ccp2_irq_handle_t;

/***************************************************************************/
/* FUNCTION PROTOTYPES */
/***************************************************************************/
/* ================================================================ */
/* 
 *  Description:- Init should be called only once before calling any other function
 *  
 *
 *   @param   none
 
 *   @return    CCP2_RETURN     
 */
 /*================================================================== */

/* ===================================================================
 *  @func     ccp2_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_init();

/* ======================================================================= */
/* Reset_CCP2() resets CCP2
 * 
 * @param reset_state instance of CCP2_SYSCONFIG @return status CCP2_SUCCESS
 * Succesfull open \n Other value = Open failed and the error code is
 * returned. */
 /* ======================================================================= */

CCP2_RETURN Reset_CCP2(CCP2_SYSCONFIG reset_state);

/* ======================================================================= */
/* Config_CCP2_Power configues the power state of CCP2 */
/* @param power_state instance of CCP2_SYSCONFIG @return status CCP2_SUCCESS
 * Succesfull open \n Other value = Open failed and the error code is
 * returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     Config_CCP2_Power                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN Config_CCP2_Power(CCP2_SYSCONFIG power_state);

/* ======================================================================= */
/* ccp2_config() configures the ccp2 global control @ param ccp2_cfg pointer 
 * to instance of object ccp2_path_enable_t * @return status CCP2_SUCCESS
 * Succesfull open \n Other value = Open failed and the error code is
 * returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     ccp2_config                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_config(ccp2_path_enable_t * ccp2_cfg);

/* ======================================================================= */
/* ccp2_start() enables CCp2
 * 
 * * @return status CCP2_SUCCESS Succesfull open \n Other value = Open failed 
 * and the error code is returned. */
 /* ======================================================================= */
/* ===================================================================
 *  @func     ccp2_start                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_start(ccp2_global_ctrl_flag flag);

/* ======================================================================= */
/* ccp2_disable() disables CCp2 * @param dis_status instance of enum
 * CCP2_DISABLE * @return status CCP2_SUCCESS Succesfull open \n Other value
 * = Open failed and the error code is returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     ccp2_disable                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_disable(CCP2_DISABLE dis_status);

/* ======================================================================= */
/** 
*   ccp2_disableLCM()  disables CCp2
*   @param None
*   @return         status      CCP2_SUCCESS Succesfull open \n
*                                     Other value = Open failed and the error code is returned.
 */
 /*=======================================================================*/

/* ===================================================================
 *  @func     ccp2_disableLCM                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_disableLCM(void);

/* ================================================================ */
/* 
 *  Description :- open call will set up the CSL register pointers to
 *                 appropriate values, register the int handler, enable ipipe clk
 *  
 *
 *   @param   :- none
 
 *   @return  :- CCP2_RETURN       
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ccp2_open                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_open();

/* ================================================================ */
/* 
 *  Description:- close will de-init the CSL reg ptr, cut ipipe clk,
 *                removes the int handler 
 *  
 *
 *   @param   none
 
 *   @return   CCP2_RETURN      
 */
 /*================================================================== */
/* ===================================================================
 *  @func     ccp2_close                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_close();

/* ======================================================================= */
/* ccp2_channel_config configues ccp2 channel * @param ccp2_chan_cfg pointer
 * to instance of object ccp2_chan_enable_t * @param lcx_flag instance of
 * ccp2_global_ctrl_flag * @return status CCP2_SUCCESS Succesfull open \n
 * Other value = Open failed and the error code is returned. */
 /* ======================================================================= */
/* ===================================================================
 *  @func     ccp2_channel_config                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN ccp2_channel_config(ccp2_chan_enable_t * ccp2_chan_cfg,
                                ccp2_global_ctrl_flag lcx_flag);

/* ======================================================================= */
     /* Configure_CCP2_Lcx_Interrupt() configures the logical channel
      * interrupt * @param status handle to the enum CCP2_CONTROL_BIT *
      * @param irq_num handle to the enum CCP2_LCX_IRQ_NUM * @return status
      * CCP2_SUCCESS Succesfull open \n * Other value = Open failed and the
      * error code is returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     Configure_CCP2_Lcx_Interrupt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN Configure_CCP2_Lcx_Interrupt(Channel_Number number,
                                         CCP2_CONTROL_BIT status,
                                         CCP2_LCX_IRQ_NUM irq_num);

/* ======================================================================= */
/* Configure_CCP2_Lcm_Interrupt() configures the memory channel interrupt for 
 * CCP2 * @param lcm_status handle to the enum CCP2_CONTROL_BIT * @param
 * lcm_num handle to the enum CCP2_LCM_IRQ_NUM * @return status CCP2_SUCCESS
 * Succesfull open \n Other value = Open failed and the error code is
 * returned. */
 /* ======================================================================= */
/* ===================================================================
 *  @func     Configure_CCP2_Lcm_Interrupt                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN Configure_CCP2_Lcm_Interrupt(CCP2_CONTROL_BIT lcm_status,
                                         CCP2_LCM_IRQ_NUM lcm_num);

/* ======================================================================= */
/* CCP2_Read_from_Memory() configures the read from memory channel * @param
 * lcm_enable instance of object ccp2_lcm_enable_t * @param flag handle to
 * the ccp2_global_ctrl_flag * @return status CCP2_SUCCESS Succesfull open \n 
 * Other value = Open failed and the error code is returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     CCP2_Read_from_Memory_Config                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN CCP2_Read_from_Memory_Config(ccp2_lcm_enable_t * lcm_enable,
                                         ccp2_global_ctrl_flag flag);

/* ======================================================================= */
/* CCP2_Lcm_Set_Read_Address() configures read address for the memory channel
 * * @param lcm_src_addr source address where CCP to read from * @return
 * status CCP2_SUCCESS Succesfull open \n Other value = Open failed and the
 * error code is returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     CCP2_Lcm_Set_Read_Address                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN CCP2_Lcm_Set_Read_Address(void *lcm_src_addr);

/* ======================================================================= */
/* Set_CCP2_Lcx_Ping_Buffer() writes to the ping buffer of a channel * @param 
 * number instance of the Channel_Number * @param address uint32 physical
 * address for ping buffer of channel * @param offset uint32 offset for
 * physical address for ping buffer of channel * @return status CCP2_SUCCESS
 * Succesfull open \n Other value = Open failed and the error code is
 * returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     Set_CCP2_Lcx_Ping_Buffer                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN Set_CCP2_Lcx_Ping_Buffer(Channel_Number number,
                                     PhyAddress_t address, PhyAddress_t offset);

/* ======================================================================= */
/* Set_CCP2_Lcx_Pong_Buffer() writes to the pong buffer of a channel * @param 
 * number instance of the Channel_Number * @param address uint32 physical
 * address for pong buffer of channel * @param offset uint32 offset for
 * physical address for pong buffer of channel * @return status CCP2_SUCCESS
 * Succesfull open \n Other value = Open failed and the error code is
 * returned. */
 /* ======================================================================= */

/* ===================================================================
 *  @func     Set_CCP2_Lcx_Pong_Buffer                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN Set_CCP2_Lcx_Pong_Buffer(Channel_Number number,
                                     PhyAddress_t address, PhyAddress_t offset);

/* ======================================================================= */
/* CCP2_Write_Debug() writes to the debug register of CCP2 for debugging *
 * @param value the data to be written to debug register * @return status
 * CCP2_SUCCESS Succesfull open \n Other value = Open failed and the error
 * code is returned. */
 /* ======================================================================= */
/* ===================================================================
 *  @func     CCP2_Write_Debug                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN CCP2_Write_Debug(uint32 value);

/* ===================================================================
 *  @func     CCP2_clear_irq_status                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
CCP2_RETURN CCP2_clear_irq_status(CCP2_LCM_IRQ_NUM lcm_num);

/* ===================================================================
 *  @func     CCP2_get_irq_status                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
uint8 CCP2_get_irq_status(CCP2_LCM_IRQ_NUM lcm_num);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
