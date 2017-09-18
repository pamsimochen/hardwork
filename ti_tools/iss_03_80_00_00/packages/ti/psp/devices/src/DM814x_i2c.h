/** ==================================================================
 *  @file   DM814x_i2c.h                                                  
 *                                                                    
 *  @path   /ti/psp/devices/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
// << PUT CORRECT COPYRIGHT NOTICE HERE >>

/**
 * \file    DM814x_i2c.h
 *
 * \brief   Defines the module register defines for I2C
 *
 * This header file contains the Macros definitions and the
 * register overlay structure for DM814x I2C controller.
 *
 * \author  0043
 *
 * \version 01a,13aug,2010 Created.
 */

#ifndef _DM814x_i2c_H_
#define _DM814x_i2c_H_

#ifdef __cplusplus
extern "C" {
#endif

    /* 
     *====================
     * Includes
     *====================
     */
#include "DM814x_types.h"

/**
@addtogroup DM814x I2C
@{
*/

    /* 
     *====================
     * Defines
     *====================
     */
#define I2C0_BASE                    0x48028000
#define I2C1_BASE                    0x4802A000
#define I2C2_BASE                    0x4819C000
#define I2C3_BASE                    0x4819E000

    /* PRCM registers for I2C */
#define CM_ALWON_I2C_0_CLKCTRL		(0x48181564u)
#define CM_ALWON_I2C_1_CLKCTRL		(0x48181568u)

    /* I2C Interrupt Enable Register (I2C_IE): */
#define I2C_IE_XDR     (1u << 14)                          /* TX Buffer drain 
                                                            * int enable */
#define I2C_IE_RDR     (1u << 13)                          /* RX Buffer drain 
                                                            * int enable */
#define I2C_IE_XRDY    (1u << 4)                           /* TX data ready
                                                            * int enable */
#define I2C_IE_RRDY    (1u << 3)                           /* RX data ready
                                                            * int enable */
#define I2C_IE_ARDY    (1u << 2)                           /* Access ready
                                                            * int enable */
#define I2C_IE_NACK    (1u << 1)                           /* No ack
                                                            * interrupt
                                                            * enable */
#define I2C_IE_AL      (1u << 0)                           /* Arbitration
                                                            * lost int ena */

    /* I2C Status Register (I2C_STAT): */
#define I2C_STAT_XDR   (1u << 14)                          /* TX Buffer
                                                            * draining */
#define I2C_STAT_RDR   (1u << 13)                          /* RX Buffer
                                                            * draining */
#define I2C_STAT_BB    (1u << 12)                          /* Bus busy */
#define I2C_STAT_ROVR  (1u << 11)                          /* Receive overrun 
                                                            */
#define I2C_STAT_XUDF  (1u << 10)                          /* Transmit
                                                            * underflow */
#define I2C_STAT_AAS   (1u << 9)                           /* Address as
                                                            * slave */
#define I2C_STAT_AD0   (1u << 8)                           /* Address zero */
#define I2C_STAT_XRDY  (1u << 4)                           /* Transmit data
                                                            * ready */
#define I2C_STAT_RRDY  (1u << 3)                           /* Receive data
                                                            * ready */
#define I2C_STAT_ARDY  (1u << 2)                           /* Register access 
                                                            * ready */
#define I2C_STAT_NACK  (1u << 1)                           /* No ack
                                                            * interrupt
                                                            * enable */
#define I2C_STAT_AL    (1u << 0)                           /* Arbitration
                                                            * lost int ena */

    /* I2C WE wakeup enable register */
#define I2C_WE_XDR_WE  (1u << 14)                          /* TX drain wakup */
#define I2C_WE_RDR_WE  (1u << 13)                          /* RX drain wakeup 
                                                            */
#define I2C_WE_AAS_WE  (1u << 9)                           /* Address as
                                                            * slave wakeup */
#define I2C_WE_BF_WE   (1u << 8)                           /* Bus free wakeup 
                                                            */
#define I2C_WE_STC_WE  (1u << 6)                           /* Start condition 
                                                            * wakeup */
#define I2C_WE_GC_WE   (1u << 5)                           /* General call
                                                            * wakeup */
#define I2C_WE_DRDY_WE (1u << 3)                           /* TX/RX data
                                                            * ready wakeup */
#define I2C_WE_ARDY_WE (1u << 2)                           /* Reg access
                                                            * ready wakeup */
#define I2C_WE_NACK_WE (1u << 1)                           /* No
                                                            * acknowledgment
                                                            * wakeup */
#define I2C_WE_AL_WE   (1u << 0)                           /* Arbitration
                                                            * lost wakeup */

#define I2C_WE_ALL     (I2C_WE_XDR_WE | I2C_WE_RDR_WE | \
                I2C_WE_AAS_WE | I2C_WE_BF_WE | \
                I2C_WE_STC_WE | I2C_WE_GC_WE | \
                I2C_WE_DRDY_WE | I2C_WE_ARDY_WE | \
                I2C_WE_NACK_WE | I2C_WE_AL_WE)

    /* I2C Buffer Configuration Register (I2C_BUF): */
#define I2C_BUF_RDMA_EN    (1u << 15)                      /* RX DMA channel
                                                            * enable */
#define I2C_BUF_RXFIF_CLR  (1u << 14)                      /* RX FIFO Clear */
#define I2C_BUF_XDMA_EN    (1u << 7)                       /* TX DMA channel
                                                            * enable */
#define I2C_BUF_TXFIF_CLR  (1u << 6)                       /* TX FIFO Clear */

    /* I2C Configuration Register (I2C_CON): */
#define I2C_CON_EN          (1u << 15)                     /* I2C module
                                                            * enable */
#define I2C_CON_BE          (1u << 14)                     /* Big endian mode 
                                                            */
#define I2C_CON_OPMODE_HS   (1u << 12)                     /* High Speed
                                                            * support */
#define I2C_CON_STB         (1u << 11)                     /* Start byte mode 
                                                            * (master) */
#define I2C_CON_MST         (1u << 10)                     /* Master/slave
                                                            * mode */
#define I2C_CON_TRX         (1u << 9)                      /* TX/RX mode
                                                            * (master only) */
#define I2C_CON_TX			(1u << 9)                      /* Tx Mode of
                                                            * operation in
                                                            * Master mode
                                                            * only */
#define I2C_CON_RX			(0u << 9)                      /* Rx Mode */
#define I2C_CON_XA          (1u << 8)                      /* Expand address */
#define I2C_CON_RM          (1u << 2)                      /* Repeat mode
                                                            * (master only) */
#define I2C_CON_STP         (1u << 1)                      /* Stop cond
                                                            * (master only) */
#define I2C_CON_STT         (1u << 0)                      /* Start condition 
                                                            * (master) */

    /* I2C SCL time value when Master */
#define I2C_SCLL_HSSCLL    (8u)
#define I2C_SCLH_HSSCLH    (8u)

    /* OCP_SYSSTATUS bit definitions */
#define SYSS_RESETDONE_MASK         (1u << 0)

    /* OCP_SYSCONFIG bit definitions */
#define SYSC_CLOCKACTIVITY_MASK     (0x3u << 8)
#define SYSC_CLOCKACTIVITY_SHIFT    (8u)
#define SYSC_SIDLEMODE_MASK         (0x3u << 3)
#define SYSC_IDLEMODE_SHIFT         (3u)
#define SYSC_ENAWAKEUP_MASK         (1u << 2)
#define SYSC_SOFTRESET_MASK         (1u << 1)
#define SYSC_AUTOIDLE_MASK          (1u << 0)

#define SYSC_IDLEMODE_SMART         (0x2u)
#define SYSC_CLOCKACTIVITY_FCLK     (0x2u)

#define I2C_SA_MASK                 0x03FF                 // Bit Mask
#define I2C_OA_MASK					0x03FF
/**
 * Max I2C driver opens allowed.
 * Note: If required, this number can be increased
 */
#define I2C_NUM_OPENS                       (4u)

/** Min I2C bus frequency - 1 KHz */
#define I2C_MIN_BUS_FREQ                    (1u)
/** Max I2C bus frequency - 400 KHz */
#define I2C_MAX_BUS_FREQ                    (400u)

/** Bus Busy Timeout (counter, should depend upon CPU speed) */
#define I2C_BUS_BUSY_TIMEOUT_CNT            (10000000u)
/** Bus Busy Timeout (mSecs) */
#define I2C_BUS_BUSY_TIMEOUT_MSECS          (10u)

    /* I2C data transfer timeout (in mSecs) */
#define I2C_DATA_TRANSFER_TIMEOUT_SECS      (1000u)

    /* I2C device busy timeout (in mSecs) */
#define I2C_DEVICE_BUSY_TIMEOUT_SECS        (5000u)

/** Maximum buffer length which could be transferred */
#define I2C_BUFFER_MAXLENGTH                (65536u)

#define I2C_DELAY_MULTIPLIER                (100u)

    /* 
     *====================
     * Structures
     *====================
     */
/**
 * \brief One line description of the structure
 *
 *  Detailed description of the structure
 */
    typedef struct {
        volatile UINT32 I2C_REVNB_LO;                     /* 0x00 *//**< Revision ID of I2C controller */
        volatile UINT32 I2C_REVNB_HI;                     /* 0x04 *//**< Revision ID High of I2C controller */
        volatile UINT32 RSVD0[2];
        volatile UINT32 I2C_SYSC;                         /* 0x10 *//**< System configuration register */
        volatile UINT32 RSVD1[3];
        volatile UINT32 I2C_EOI;                          /* 0x20 *//**< End of Interrupt register */
        volatile UINT32 I2C_IRQSTATUS_RAW;                /* 0x24 *//**< IRQ Status Raw register */
        volatile UINT32 I2C_IRQSTATUS;                    /* 0x28 *//**< IRQ status register */
        volatile UINT32 I2C_IRQENABLE_SET;                /* 0x2C *//**< Interrupt Enable Set register */
        volatile UINT32 I2C_IRQENABLE_CLR;                /* 0x30 *//**< Interrupt Enable clear register */
        volatile UINT32 I2C_WE;                           /* 0x34 *//**< Wake up enable register */
        volatile UINT32 I2C_DMARXENABLE_SET;              /* 0x38 *//**< Receive DMA enable set register */
        volatile UINT32 I2C_DMATXENABLE_SET;              /* 0x3C *//**< Transmit DMA enable Set register */
        volatile UINT32 I2C_DMARXENABLE_CLR;              /* 0x40 *//**< Receive DMA enable Clear register */
        volatile UINT32 I2C_DMATXENABLE_CLR;              /* 0x44 *//**< Transmit DMA enable Clear register */
        volatile UINT32 I2C_DMARXWAKE_EN;                 /* 0x48 *//**< Receive DMA wake enable */
        volatile UINT32 I2C_DMATXWAKE_EN;                 /* 0x4C *//**< Transmit DMA Wake Enable  */
        volatile UINT32 RSVD2[13];
        volatile UINT32 I2C_IE;                            /* 0x84 */
        volatile UINT32 I2C_STAT;                          /* 0x88 */
        volatile UINT32 RSVD3;                             /* 0x8C */
        volatile UINT32 I2C_SYSS;                         /* 0x90 *//**< System status register */
        volatile UINT32 I2C_BUF;                          /* 0x94 *//**< Buffer configuration register */
        volatile UINT32 I2C_CNT;                          /* 0x98 *//**< Count configuration register */
        volatile UINT32 I2C_DATA;                         /* 0x9C *//**< Data access register */
        volatile UINT32 RSVD4;                             /* 0xA0 */
        volatile UINT32 I2C_CON;                          /* 0xA4 *//**< Configuration register */
        volatile UINT32 I2C_OA;                           /* 0xA8 *//**< Own address configuration register */
        volatile UINT32 I2C_SA;                           /* 0xAC *//**< Slave address configuration register */
        volatile UINT32 I2C_PSC;                          /* 0xB0 *//**< Pre-Scaller configuration register */
        volatile UINT32 I2C_SCLL;                         /* 0xB4 *//**< SCL Low time configuration register */
        volatile UINT32 I2C_SCLH;                         /* 0xB8 *//**< SCL High time configuraiton register */
        volatile UINT32 I2C_SYSTEST;                      /* 0xBC *//**< System test register */
        volatile UINT32 I2C_BUFSTAT;                      /* 0xC0 *//**< Buffer Status register */
        volatile UINT32 I2C_OA1;                          /* 0xC4 *//**< Own address 1 register */
        volatile UINT32 I2C_OA2;                          /* 0xC8 *//**< Own address 2 register */
        volatile UINT32 I2C_OA3;                          /* 0xCC *//**< Own address 3 register */
        volatile UINT32 I2C_ACTOA;                        /* 0xD0 *//**< Active own address register */
        volatile UINT32 I2C_SBLOCK;                       /* 0xD4 *//**< Clock Blocking enable register */
    } __DM814X_I2C_REGS_, *DM814X_I2C_REGS;

    /* 
     *====================
     * Function declarations
     *====================
     */
/** **************************************************************************
 * \n \brief Routine to initialize the I2C - 0
 *
 * This routine initializes the I2C instance 0. We are not configuring the 
 * interrupts here as the code is intended to carry out all I/O operations
 * in polled mode.
 *
 * \param u8BusNum		[IN]	Bus Instance number
 *
 * \return
 * \n      return SUCCESS for success  - Description
 * \n      return FAILED for error   - Description
 */
    STATUS DM814xI2cInit(UINT8 u8BusNum);
/** **************************************************************************
 * \n \brief Routine to read data from slave deice the I2C - 0
 *
 * This routine initializes the I2C controller for reading from the slave
 * peripheral connected to the bus. This function takes the slave address
 * as a parameter along with the read buffer pointer and the length of buffer.
 * This function returns the number of bytes read into the output len pointer.
 *
 * \param u8BusNum 		[IN]		I2C Bus number
 * \param u16SlaveAddr 	[IN]		I2C Slave address to read from
 * \param *u8Buffer		[OUT]		Buffer pointer to copy data into
 * \param u8BufLen 	[IN]		Length of the input buffer pointer
 * \param *u8DataRead	[OUT]		Place holder to return read number of bytes
 *
 * \return
 * \n      return SUCCESS for success  - Description
 * \n      return FAILED for error   - Description
 */

    STATUS DM814xI2cRead
        (UINT8 u8BusNum,
         UINT16 u16SlaveAddr,
         UINT8 * u8Buffer, UINT8 u8BufLen, UINT8 * u8DataRead);
/** **************************************************************************
 * \n \brief Routine to write data from slave deice the I2C - 0
 *
 * This routine initializes the I2C controller for writing to the slave
 * peripheral connected to the bus. This function takes the slave address
 * as a parameter along with the write buffer pointer and the length of buffer.
 * This function returns the number of bytes writen into the output len pointer.
 *
 * \param u8BusNum 		[IN]		I2C Bus number
 * \param u16SlaveAddr 	[IN]		I2C Slave address to read from
 * \param *u8Buffer		[OUT]		Buffer pointer to copy data into
 * \param u8BufLen 		[IN]		Length of the input buffer pointer
 * \param *u8DataWritten[OUT]		Place holder to return read number of bytes
 *
 * \return
 * \n      return SUCCESS for success  - Description
 * \n      return FAILED for error   - Description
 */
    STATUS DM814xI2cWite
        (UINT8 u8BusNum,
         UINT16 u16SlaveAddr,
         UINT8 * u8Buffer, UINT8 u8BufLen, UINT8 * u8DataWritten);

/** **************************************************************************
 * \n \brief Routine to Probe slave device on the I2C Bus
 *
 * This routine tries to write a byte to the slave device and looks for NACk.
 * If there is no NACK, it assumes the device is present.
 *
 * \param u8BusNum 		[IN]		I2C Bus number
 * \param u16SlaveAddr 	[IN]		I2C Slave address to read from
 *
 * \return
 * \n      return SUCCESS for success  - Description
 * \n      return FAILED for error   - Description
 */
    STATUS DM814xI2cProbe(UINT8 u8BusNum, UINT16 u16SlaveAddr);

    /* @} *//* End of I2C */

#ifdef __cplusplus
}                                                          /* End of extern C 
                                                            */
#endif                                                     /* #ifdef
                                                            * __cplusplus */
#endif                                                     /* End of
                                                            * _DM814x_i2c_H_ */
