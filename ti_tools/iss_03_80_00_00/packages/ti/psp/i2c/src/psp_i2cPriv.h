/** ==================================================================
 *  @file   psp_i2cPriv.h                                                  
 *                                                                    
 *  @path   /ti/psp/i2c/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#ifndef _PSP_I2CPRIV_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _PSP_I2CPRIV_H_

#include <string.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/i2c/psp_i2c.h>

/**************************************************************************\
* Register Overlay Structure
\**************************************************************************/
typedef struct {
    volatile Uint32 I2C_REVNB_LO;                          /* 0x00 */
    volatile Uint32 I2C_REVNB_HI;                          /* 0x04 */
    volatile Uint32 RSVD0[2];
    volatile Uint32 I2C_SYSC;                              /* 0x10 */
    volatile Uint32 RSVD1[3];
    volatile Uint32 I2C_EOI;                               /* 0x20 */
    volatile Uint32 I2C_IRQSTATUS_RAW;                     /* 0x24 */
    volatile Uint32 I2C_IRQSTATUS;                         /* 0x28 */
    volatile Uint32 I2C_IRQENABLE_SET;                     /* 0x2C */
    volatile Uint32 I2C_IRQENABLE_CLR;                     /* 0x30 */
    volatile Uint32 I2C_WE;                                /* 0x34 */
    volatile Uint32 I2C_DMARXENABLE_SET;                   /* 0x38 */
    volatile Uint32 I2C_DMATXENABLE_SET;                   /* 0x3C */
    volatile Uint32 I2C_DMARXENABLE_CLR;                   /* 0x40 */
    volatile Uint32 I2C_DMATXENABLE_CLR;                   /* 0x44 */
    volatile Uint32 I2C_DMARXWAKE_EN;                      /* 0x48 */
    volatile Uint32 I2C_DMATXWAKE_EN;                      /* 0x4C */
    volatile Uint32 RSVD2[13];
    volatile Uint32 I2C_IE;                                /* 0x84 */
    volatile Uint32 I2C_STAT;                              /* 0x88 */
    volatile Uint32 RSVD3;                                 /* 0x8C */
    volatile Uint32 I2C_SYSS;                              /* 0x90 */
    volatile Uint32 I2C_BUF;                               /* 0x94 */
    volatile Uint32 I2C_CNT;                               /* 0x98 */
    volatile Uint32 I2C_DATA;                              /* 0x9C */
    volatile Uint32 RSVD4;                                 /* 0xA0 */
    volatile Uint32 I2C_CON;                               /* 0xA4 */
    volatile Uint32 I2C_OA;                                /* 0xA8 */
    volatile Uint32 I2C_SA;                                /* 0xAC */
    volatile Uint32 I2C_PSC;                               /* 0xB0 */
    volatile Uint32 I2C_SCLL;                              /* 0xB4 */
    volatile Uint32 I2C_SCLH;                              /* 0xB8 */
    volatile Uint32 I2C_SYSTEST;                           /* 0xBC */
    volatile Uint32 I2C_BUFSTAT;                           /* 0xC0 */
    volatile Uint32 I2C_OA1;                               /* 0xC4 */
    volatile Uint32 I2C_OA2;                               /* 0xC8 */
    volatile Uint32 I2C_OA3;                               /* 0xCC */
    volatile Uint32 I2C_ACTOA;                             /* 0xD0 */
    volatile Uint32 I2C_SBLOCK;                            /* 0xD4 */
} CSL_I2cRegs;

/**************************************************************************\
* Overlay structure typedef definition
\**************************************************************************/
typedef volatile CSL_I2cRegs *CSL_I2cRegsOvly;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/
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

/**
 * \brief I2C driver state
 *
 *  These enums are used to maintain the current state of the driver
 */
typedef enum {
    I2C_DELETED = 0u,       /**< Instance Deleted */
    I2C_CREATED,           /**< Instance Created */
    I2C_OPENED,            /**< Instance Open */
    I2C_CLOSED             /**< Instance Closed */
} PSP_I2cState;

/**
 *  \brief I2C Instance Object - One for each I2C controller
 *
 *  This structure basically holds the hardware configuration for each instance.
 *  One such storage exists for each instance of the I2C peripheral.
 */
typedef struct {
    /** SOC Related Details */
    CSL_I2cRegs *i2cRegs;                       /**< I2C peripheral register map */
    UInt32 intNum;                              /**< Interrupt number */

    /* I2C Peripheral details */
    UInt32 isMasterMode;                        /**< Master/Slave mode of operation*/
    PSP_I2cOpMode opMode;                       /**< Driver operating mode - polled, interrupt, dma */
    UInt32 i2cBusFreq;                          /**< I2C Bus Frequency (in KHz) */
    UInt32 is10BitAddr;                         /**< 7bit/10bit Addressing mode */
    UInt32 i2cOwnAddr;                          /**< Own address (7 or 10 bit) */
    UInt32 repeatMode;                          /**< Repeat Mode */

    /* Driver internal members */
    UInt32 instanceId;                          /**< I2C Instance number */
    PSP_I2cState state;                         /**< State of driver */
    UInt32 numOpens;                            /**< Number of active opens */
    UInt32 pendingState;                        /**< Shows whether IO is in pending state or not    */

    /* Driver internal semaphores */
    Semaphore_Handle sem;                       /**< To protect shared data structs */
    Semaphore_Handle devBusySem;                /**< Device busy semaphore handle */
    Semaphore_Handle completionSem;             /**< Device busy semaphore handle */
    Hwi_Handle hwiHandle;                       /**< Handle to Hardware Interrupt */

    /* Driver internal Transmission details */
    Int currError;                              /**< Error code */
    UInt32 currFlags;                           /**< Current Flags for read/write */
    UInt8 *currBuffer;                          /**< User buffer for read/write */
    UInt32 currBufferLen;                       /**< User buffer length */
} PSP_I2cInstObj;

/**
 * \brief    Default I2C Instance Object values
 */
#define I2C_INSTANCE_OBJ_DEFAULT                            \
        {                                                   \
        /* CSL regs */              NULL,                   \
        /* Interrupt num */         0,                      \
        /* master/slave mode */     TRUE,                   \
        /* operating mode */        I2C_OPMODE_POLLED,      \
        /* bus freq */              0,                      \
        /* 7/10 bit address */      0,                      \
        /* own address*/            0,                      \
        /* repeat mode */           0,                      \
        /* instanceId */            0,                      \
        /* state */                 I2C_DELETED,            \
        /* numopens */              0,                      \
        /* pending state */         0,                      \
        /* sem */                   NULL,                   \
        /* devBusySem */            NULL,                   \
        /* completionSem */         NULL,                   \
        /* error */                 0,                      \
        /* flags */                 0,                      \
        /* buffer */                NULL,                   \
        /* buf len */               0,                      \
        }

/* ! \brief I2C Driver Object - One for each opened I2C driver handle */
typedef struct {
    PSP_I2cAppCallback callBack;
    /**< Callback from application */

    Ptr appData;
    /**< application data which will be returned with callback */

    PSP_I2cInstObj *pi2cInstHandle;
    /**< i2c instance object handle */

} PSP_I2cDriverObject;

/**
 * \brief    Default I2C Driver Object values
 */
#define I2C_DRIVER_OBJECT_DEFAULT               \
        {                                       \
         /* callback */                 NULL,   \
         /* callback data */            NULL,   \
         /* I2C instance obj ptr */     NULL    \
        }

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif                                                     /* _PSP_I2CPRIV_H_ 
                                                            */
