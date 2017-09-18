/*
 * sample_tda2xx_cfg.c
 *
 * SoC specific EDMA3 hardware related information like number of transfer
 * controllers, various interrupt ids etc. It is used while interrupts
 * enabling / disabling. It needs to be ported for different SoCs.
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

#include <ti/sdo/edma3/drv/edma3_drv.h>
#ifdef BUILD_TDA2XX_IPU
#include <ti/sysbios/family/arm/ducati/Core.h> 

#endif

/* Number of EDMA3 controllers present in the system */
#define NUM_EDMA3_INSTANCES         2u
const unsigned int numEdma3Instances = NUM_EDMA3_INSTANCES;

/* Number of DSPs present in the system */
#define NUM_DSPS                    1u
const unsigned int numDsps = NUM_DSPS;

/* Determine the processor id by reading DNUM register. */
/* Statically allocate the region numbers with cores. */
int myCoreNum;
#define PID0_ADDRESS 0xE00FFFE0
#define CORE_ID_C0 0x0
#define CORE_ID_C1 0x1

unsigned short determineProcId()
{
unsigned short regionNo = numEdma3Instances;
#ifdef BUILD_TDA2XX_DSP
extern __cregister volatile unsigned int DNUM;
#endif
myCoreNum = numDsps;
#ifdef BUILD_TDA2XX_MPU

    asm ("    push    {r0-r2} \n\t"
	     "    MRC p15, 0, r0, c0, c0, 5\n\t"
		 "    LDR      r1, =myCoreNum\n\t"
		 "    STR      r0, [r1]\n\t"
		 "    pop    {r0-r2}\n\t");
	if((myCoreNum & 0x03) == 1)
		regionNo = 1;
	else
		regionNo = 0;
#elif defined(BUILD_TDA2XX_IPU)
myCoreNum = (*(unsigned int *)(PID0_ADDRESS));
if(Core_getIpuId() == 1){
	if(myCoreNum == CORE_ID_C0)
		regionNo = 4;
	else if (myCoreNum == CORE_ID_C1)
		regionNo = 5;
}
if(Core_getIpuId() == 2){
	if(myCoreNum == CORE_ID_C0)
		regionNo = 6;
	else if (myCoreNum == CORE_ID_C1)
		regionNo = 7;
}
#elif defined BUILD_TDA2XX_DSP

	myCoreNum = DNUM;
	if(myCoreNum == 0)
		regionNo = 2;
	else
		regionNo = 3;
#endif
	return regionNo;
}

signed char*  getGlobalAddr(signed char* addr)
{
     return (addr); /* The address is already a global address */
}
unsigned short isGblConfigRequired(unsigned int dspNum)
{
    (void) dspNum;
	return 1;
}

/* Semaphore handles */
EDMA3_OS_Sem_Handle semHandle[NUM_EDMA3_INSTANCES] = {NULL};

/** Number of PaRAM Sets available                                            */
#define EDMA3_NUM_PARAMSET                              (512u)

/** Number of TCCS available                                                  */
#define EDMA3_NUM_TCC                                   (64u)

/** Number of DMA Channels available                                          */
#define EDMA3_NUM_DMA_CHANNELS                          (64u)

/** Number of QDMA Channels available                                         */
#define EDMA3_NUM_QDMA_CHANNELS                         (8u)

/** Number of Event Queues available                                          */
#define EDMA3_NUM_EVTQUE                                (4u)

/** Number of Transfer Controllers available                                  */
#define EDMA3_NUM_TC                                    (2u)

/** Number of Regions                                                         */
#define EDMA3_NUM_REGIONS                               (8u)

/** Interrupt no. for Transfer Completion */
#define EDMA3_CC_XFER_COMPLETION_INT_A15                (66u)
#define EDMA3_CC_XFER_COMPLETION_INT_DSP                (38u)
#define EDMA3_CC_XFER_COMPLETION_INT_IPU_C0               (34u)
#define EDMA3_CC_XFER_COMPLETION_INT_IPU_C1               (33u)
/** Based on the interrupt number to be mapped define the XBAR instance number */
#define COMPLETION_INT_A15_XBAR_INST_NO                 (29u)
#define COMPLETION_INT_DSP_XBAR_INST_NO                 (7u)
#define COMPLETION_INT_IPU_C0_XBAR_INST_NO                (12u)
#define COMPLETION_INT_IPU_C1_XBAR_INST_NO                (11u)

/** Interrupt no. for CC Error */
#define EDMA3_CC_ERROR_INT_A15                          (67u)
#define EDMA3_CC_ERROR_INT_DSP                          (39u)
#define EDMA3_CC_ERROR_INT_IPU                         (35u)

/** Based on the interrupt number to be mapped define the XBAR instance number */
#define CC_ERROR_INT_A15_XBAR_INST_NO                   (30u)
#define CC_ERROR_INT_DSP_XBAR_INST_NO                   (8u)
#define CC_ERROR_INT_IPU_XBAR_INST_NO                  (13u)

/** Interrupt no. for TCs Error */
#define EDMA3_TC0_ERROR_INT_A15                         (68u)
#define EDMA3_TC0_ERROR_INT_DSP                         (40u)
#define EDMA3_TC0_ERROR_INT_IPU                        (36u)
#define EDMA3_TC1_ERROR_INT_A15                         (69u)
#define EDMA3_TC1_ERROR_INT_DSP                         (41u)
#define EDMA3_TC1_ERROR_INT_IPU                        (37u)
/** Based on the interrupt number to be mapped define the XBAR instance number */
#define TC0_ERROR_INT_A15_XBAR_INST_NO                  (31u)
#define TC0_ERROR_INT_DSP_XBAR_INST_NO                  (9u)
#define TC0_ERROR_INT_IPU_XBAR_INST_NO                 (14u)
#define TC1_ERROR_INT_A15_XBAR_INST_NO                  (32u)
#define TC1_ERROR_INT_DSP_XBAR_INST_NO                  (10u)
#define TC1_ERROR_INT_IPU_XBAR_INST_NO                 (15u)

#ifdef BUILD_TDA2XX_MPU
#define EDMA3_CC_XFER_COMPLETION_INT                    EDMA3_CC_XFER_COMPLETION_INT_A15
#define EDMA3_CC_ERROR_INT                              EDMA3_CC_ERROR_INT_A15
#define CC_ERROR_INT_XBAR_INST_NO                       CC_ERROR_INT_A15_XBAR_INST_NO
#define EDMA3_TC0_ERROR_INT                             EDMA3_TC0_ERROR_INT_A15
#define EDMA3_TC1_ERROR_INT                             EDMA3_TC1_ERROR_INT_A15
#define TC0_ERROR_INT_XBAR_INST_NO                      TC0_ERROR_INT_A15_XBAR_INST_NO
#define TC1_ERROR_INT_XBAR_INST_NO                      TC1_ERROR_INT_A15_XBAR_INST_NO

#elif defined BUILD_TDA2XX_DSP
#define EDMA3_CC_XFER_COMPLETION_INT                    EDMA3_CC_XFER_COMPLETION_INT_DSP
#define EDMA3_CC_ERROR_INT                              EDMA3_CC_ERROR_INT_DSP
#define CC_ERROR_INT_XBAR_INST_NO                       CC_ERROR_INT_DSP_XBAR_INST_NO
#define EDMA3_TC0_ERROR_INT                             EDMA3_TC0_ERROR_INT_DSP
#define EDMA3_TC1_ERROR_INT                             EDMA3_TC1_ERROR_INT_DSP
#define TC0_ERROR_INT_XBAR_INST_NO                      TC0_ERROR_INT_DSP_XBAR_INST_NO
#define TC1_ERROR_INT_XBAR_INST_NO                      TC1_ERROR_INT_DSP_XBAR_INST_NO

#elif defined BUILD_TDA2XX_IPU
#define EDMA3_CC_XFER_COMPLETION_INT                    EDMA3_CC_XFER_COMPLETION_INT_IPU_C0
#define EDMA3_CC_ERROR_INT                              EDMA3_CC_ERROR_INT_IPU
#define CC_ERROR_INT_XBAR_INST_NO                       CC_ERROR_INT_IPU_XBAR_INST_NO
#define EDMA3_TC0_ERROR_INT                             EDMA3_TC0_ERROR_INT_IPU
#define EDMA3_TC1_ERROR_INT                             EDMA3_TC1_ERROR_INT_IPU
#define TC0_ERROR_INT_XBAR_INST_NO                      TC0_ERROR_INT_IPU_XBAR_INST_NO
#define TC1_ERROR_INT_XBAR_INST_NO                      TC1_ERROR_INT_IPU_XBAR_INST_NO

#else
#define EDMA3_CC_XFER_COMPLETION_INT                    (0u)
#define EDMA3_CC_ERROR_INT                              (0u)
#define CC_ERROR_INT_XBAR_INST_NO                       (0u)
#define EDMA3_TC0_ERROR_INT                             (0u)
#define EDMA3_TC1_ERROR_INT                             (0u)
#define TC0_ERROR_INT_XBAR_INST_NO                      TC0_ERROR_INT_A15_XBAR_INST_NO
#define TC1_ERROR_INT_XBAR_INST_NO                      TC1_ERROR_INT_A15_XBAR_INST_NO
#endif

#define EDMA3_TC2_ERROR_INT                             (0u)
#define EDMA3_TC3_ERROR_INT                             (0u)
#define EDMA3_TC4_ERROR_INT                             (0u)
#define EDMA3_TC5_ERROR_INT                             (0u)
#define EDMA3_TC6_ERROR_INT                             (0u)
#define EDMA3_TC7_ERROR_INT                             (0u)

#define DSP1_EDMA3_CC_XFER_COMPLETION_INT               (19u)
#define DSP2_EDMA3_CC_XFER_COMPLETION_INT               (20u)
#define DSP1_EDMA3_CC_ERROR_INT                         (27u)
#define DSP1_EDMA3_TC0_ERROR_INT                        (28u)
#define DSP1_EDMA3_TC1_ERROR_INT                        (29u)

/** XBAR interrupt source index numbers for EDMA interrupts */
#define XBAR_EDMA_TPCC_IRQ_REGION0                      (361u)
#define XBAR_EDMA_TPCC_IRQ_REGION1                      (362u)
#define XBAR_EDMA_TPCC_IRQ_REGION2                      (363u)
#define XBAR_EDMA_TPCC_IRQ_REGION3                      (364u)
#define XBAR_EDMA_TPCC_IRQ_REGION4                      (365u)
#define XBAR_EDMA_TPCC_IRQ_REGION5                      (366u)
#define XBAR_EDMA_TPCC_IRQ_REGION6                      (367u)
#define XBAR_EDMA_TPCC_IRQ_REGION7                      (368u)

#define XBAR_EDMA_TPCC_IRQ_ERR                          (359u)
#define XBAR_EDMA_TC0_IRQ_ERR                           (370u)
#define XBAR_EDMA_TC1_IRQ_ERR                           (371u)

/**
 * EDMA3 interrupts (transfer completion, CC error etc.) correspond to different
 * ECM events (SoC specific). These ECM events come
 * under ECM block XXX (handling those specific ECM events). Normally, block
 * 0 handles events 4-31 (events 0-3 are reserved), block 1 handles events
 * 32-63 and so on. This ECM block XXX (or interrupt selection number XXX)
 * is mapped to a specific HWI_INT YYY in the tcf file.
 * Define EDMA3_HWI_INT_XFER_COMP to specific HWI_INT, corresponding
 * to transfer completion interrupt.
 * Define EDMA3_HWI_INT_CC_ERR to specific HWI_INT, corresponding
 * to CC error interrupts.
 * Define EDMA3_HWI_INT_TC_ERR to specific HWI_INT, corresponding
 * to TC error interrupts.
 */
/* EDMA 0 */

#define EDMA3_HWI_INT_XFER_COMP                           (7u)
#define EDMA3_HWI_INT_CC_ERR                              (7u)
#define EDMA3_HWI_INT_TC0_ERR                             (10u)
#define EDMA3_HWI_INT_TC1_ERR                             (10u)
#define EDMA3_HWI_INT_TC2_ERR                             (10u)
#define EDMA3_HWI_INT_TC3_ERR                             (10u)

/**
 * \brief Mapping of DMA channels 0-31 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_DRV_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_DRV_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 * 
 * For Vayu Since the xbar can be used to map event to any EDMA channel,
 * If the application is assigning events to other channel this variable 
 * should be modified
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
                                                      /* 31     0 */
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA       (0x3FC0C06Eu)  /* TBD */
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_DSPEDMA        (0x000FFFFFu)  /* TBD */


/**
 * \brief Mapping of DMA channels 32-63 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_DRV_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_DRV_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA       (0xF3FFFFFCu) /* TBD */
#define EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_DSPEDMA        (0x00000000u) /* TBD */


/* Variable which will be used internally for referring number of Event Queues*/
unsigned int numEdma3EvtQue[NUM_EDMA3_INSTANCES] =  {
                                                        EDMA3_NUM_EVTQUE,
                                                    };

/* Variable which will be used internally for referring number of TCs.        */
unsigned int numEdma3Tc[NUM_EDMA3_INSTANCES] =  {
                                                    EDMA3_NUM_TC,
                                                };

/**
 * Variable which will be used internally for referring transfer completion
 * interrupt.
 */
unsigned int ccXferCompInt[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
{
    {
        EDMA3_CC_XFER_COMPLETION_INT_A15, EDMA3_CC_XFER_COMPLETION_INT_A15,
		EDMA3_CC_XFER_COMPLETION_INT_DSP, EDMA3_CC_XFER_COMPLETION_INT_DSP,
		EDMA3_CC_XFER_COMPLETION_INT_IPU_C0, EDMA3_CC_XFER_COMPLETION_INT_IPU_C1,
        EDMA3_CC_XFER_COMPLETION_INT_IPU_C0, EDMA3_CC_XFER_COMPLETION_INT_IPU_C1,
    },
    {
        0u, 0u, DSP1_EDMA3_CC_XFER_COMPLETION_INT, DSP2_EDMA3_CC_XFER_COMPLETION_INT,
        0u, 0u, 0u, 0u,
    },
};
/** These are the Xbar instance numbers corresponding to interrupt numbers */
unsigned int ccXferCompIntXbarInstNo[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
{
    {
        COMPLETION_INT_A15_XBAR_INST_NO, COMPLETION_INT_A15_XBAR_INST_NO,
		COMPLETION_INT_DSP_XBAR_INST_NO, COMPLETION_INT_DSP_XBAR_INST_NO,
		COMPLETION_INT_IPU_C0_XBAR_INST_NO, COMPLETION_INT_IPU_C1_XBAR_INST_NO,
        COMPLETION_INT_IPU_C0_XBAR_INST_NO, COMPLETION_INT_IPU_C1_XBAR_INST_NO,
    },
};

/** These are the Interrupt Crossbar Index For EDMA Completion for different regions */
unsigned int ccCompEdmaXbarIndex[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
{
	{
		XBAR_EDMA_TPCC_IRQ_REGION0, XBAR_EDMA_TPCC_IRQ_REGION1, XBAR_EDMA_TPCC_IRQ_REGION2, XBAR_EDMA_TPCC_IRQ_REGION3,
		XBAR_EDMA_TPCC_IRQ_REGION4, XBAR_EDMA_TPCC_IRQ_REGION5, XBAR_EDMA_TPCC_IRQ_REGION6, XBAR_EDMA_TPCC_IRQ_REGION7,
	}
};

/**
 * Variable which will be used internally for referring channel controller's
 * error interrupt.
 */
unsigned int ccErrorInt[NUM_EDMA3_INSTANCES] = {
                                                    EDMA3_CC_ERROR_INT,DSP1_EDMA3_CC_ERROR_INT,
                                               };
unsigned int ccErrorIntXbarInstNo[NUM_EDMA3_INSTANCES] = {
                                                    CC_ERROR_INT_XBAR_INST_NO,
                                               };
unsigned int ccErrEdmaXbarIndex[NUM_EDMA3_INSTANCES] = 
{
	XBAR_EDMA_TPCC_IRQ_ERR,
};

/**
 * Variable which will be used internally for referring transfer controllers'
 * error interrupts.
 */
unsigned int tcErrorInt[NUM_EDMA3_INSTANCES][8] =
{
   {
       EDMA3_TC0_ERROR_INT, EDMA3_TC1_ERROR_INT,
       EDMA3_TC2_ERROR_INT, EDMA3_TC3_ERROR_INT,
       EDMA3_TC4_ERROR_INT, EDMA3_TC5_ERROR_INT,
       EDMA3_TC6_ERROR_INT, EDMA3_TC7_ERROR_INT,
   },
   {
       EDMA3_TC0_ERROR_INT, EDMA3_TC1_ERROR_INT,
       EDMA3_TC2_ERROR_INT, EDMA3_TC3_ERROR_INT,
       EDMA3_TC4_ERROR_INT, EDMA3_TC5_ERROR_INT,
       DSP1_EDMA3_TC0_ERROR_INT, DSP1_EDMA3_TC1_ERROR_INT,
   }
};
unsigned int tcErrorIntXbarInstNo[NUM_EDMA3_INSTANCES][8] =
{
   {
       TC0_ERROR_INT_XBAR_INST_NO, TC1_ERROR_INT_XBAR_INST_NO,
       0u, 0u,
       0u, 0u,
       0u, 0u,
   }
};

unsigned int tcErrEdmaXbarIndex[NUM_EDMA3_INSTANCES][8] =
{
   {
       XBAR_EDMA_TC0_IRQ_ERR, XBAR_EDMA_TC1_IRQ_ERR,
	   0u, 0u,
       0u, 0u, 0u, 0u,
   }
};


/**
 * Variables which will be used internally for referring the hardware interrupt
 * for various EDMA3 interrupts.
 */
unsigned int hwIntXferComp[NUM_EDMA3_INSTANCES] = {
                                                    EDMA3_HWI_INT_XFER_COMP, EDMA3_HWI_INT_XFER_COMP,
                                                  };

unsigned int hwIntCcErr[NUM_EDMA3_INSTANCES] = {
                                                   EDMA3_HWI_INT_CC_ERR, EDMA3_HWI_INT_CC_ERR,
                                               };

unsigned int hwIntTcErr[NUM_EDMA3_INSTANCES][8] = {
                                                     {
                                                        EDMA3_HWI_INT_TC0_ERR,
                                                        EDMA3_HWI_INT_TC1_ERR,
                                                        EDMA3_HWI_INT_TC2_ERR,
                                                        EDMA3_HWI_INT_TC3_ERR
                                                     },
                                                     {
                                                        EDMA3_HWI_INT_TC0_ERR,
                                                        EDMA3_HWI_INT_TC1_ERR,
                                                        EDMA3_HWI_INT_TC2_ERR,
                                                        EDMA3_HWI_INT_TC3_ERR
                                                     }
                                               };

/**
 * \brief Base address as seen from the different cores may be different
 * And is defined based on the core
 */
#if ((defined BUILD_TDA2XX_MPU) || (defined BUILD_TDA2XX_DSP))
#define EDMA3_CC_BASE_ADDR                          ((void *)(0x43300000))
#define EDMA3_TC0_BASE_ADDR                         ((void *)(0x43400000))
#define EDMA3_TC1_BASE_ADDR                         ((void *)(0x43500000))
#elif (defined BUILD_TDA2XX_IPU)
#define EDMA3_CC_BASE_ADDR                          ((void *)(0x63300000))
#define EDMA3_TC0_BASE_ADDR                         ((void *)(0x63400000))
#define EDMA3_TC1_BASE_ADDR                         ((void *)(0x63500000))
#else
#define EDMA3_CC_BASE_ADDR                          ((void *)(0x0))
#define EDMA3_TC0_BASE_ADDR                         ((void *)(0x0))
#define EDMA3_TC1_BASE_ADDR                         ((void *)(0x0))
#endif

#define DSP1_EDMA3_CC_BASE_ADDR                     ((void *)(0x01D10000))
#define DSP1_EDMA3_TC0_BASE_ADDR                    ((void *)(0x01D05000))
#define DSP1_EDMA3_TC1_BASE_ADDR                    ((void *)(0x01D06000))

/* Driver Object Initialization Configuration */
EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams[NUM_EDMA3_INSTANCES] =
{
    {
        /* EDMA3 INSTANCE# 0 */
        /** Total number of DMA Channels supported by the EDMA3 Controller    */
        EDMA3_NUM_DMA_CHANNELS,
        /** Total number of QDMA Channels supported by the EDMA3 Controller   */
        EDMA3_NUM_QDMA_CHANNELS,
        /** Total number of TCCs supported by the EDMA3 Controller            */
        EDMA3_NUM_TCC,
        /** Total number of PaRAM Sets supported by the EDMA3 Controller      */
        EDMA3_NUM_PARAMSET,
        /** Total number of Event Queues in the EDMA3 Controller              */
        EDMA3_NUM_EVTQUE,
        /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller*/
        EDMA3_NUM_TC,
        /** Number of Regions on this EDMA3 controller                        */
        EDMA3_NUM_REGIONS,

        /**
         * \brief Channel mapping existence
         * A value of 0 (No channel mapping) implies that there is fixed association
         * for a channel number to a parameter entry number or, in other words,
         * PaRAM entry n corresponds to channel n.
         */
        1u,

        /** Existence of memory protection feature */
        0u,

        /** Global Register Region of CC Registers */
        EDMA3_CC_BASE_ADDR,
        /** Transfer Controller (TC) Registers */
        {
	        EDMA3_TC0_BASE_ADDR,
	        EDMA3_TC1_BASE_ADDR,
	        (void *)NULL,
	        (void *)NULL,
            (void *)NULL,
            (void *)NULL,
            (void *)NULL,
            (void *)NULL
        },
        /** Interrupt no. for Transfer Completion */
        EDMA3_CC_XFER_COMPLETION_INT,
        /** Interrupt no. for CC Error */
        EDMA3_CC_ERROR_INT,
        /** Interrupt no. for TCs Error */
        {
            EDMA3_TC0_ERROR_INT,
            EDMA3_TC1_ERROR_INT,
            EDMA3_TC2_ERROR_INT,
            EDMA3_TC3_ERROR_INT,
            EDMA3_TC4_ERROR_INT,
            EDMA3_TC5_ERROR_INT,
            EDMA3_TC6_ERROR_INT,
            EDMA3_TC7_ERROR_INT
        },

        /**
         * \brief EDMA3 TC priority setting
         *
         * User can program the priority of the Event Queues
         * at a system-wide level.  This means that the user can set the
         * priority of an IO initiated by either of the TCs (Transfer Controllers)
         * relative to IO initiated by the other bus masters on the
         * device (ARM, DSP, USB, etc)
         */
        {
            0u,
            1u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u
        },
        /**
         * \brief To Configure the Threshold level of number of events
         * that can be queued up in the Event queues. EDMA3CC error register
         * (CCERR) will indicate whether or not at any instant of time the
         * number of events queued up in any of the event queues exceeds
         * or equals the threshold/watermark value that is set
         * in the queue watermark threshold register (QWMTHRA).
         */
        {
            16u,
            16u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u
        },

        /**
         * \brief To Configure the Default Burst Size (DBS) of TCs.
         * An optimally-sized command is defined by the transfer controller
         * default burst size (DBS). Different TCs can have different
         * DBS values. It is defined in Bytes.
         */
            {
            16u,
            16u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u
            },

        /**
         * \brief Mapping from each DMA channel to a Parameter RAM set,
         * if it exists, otherwise of no use.
         */
            {
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP
			},

         /**
          * \brief Mapping from each DMA channel to a TCC. This specific
          * TCC code will be returned when the transfer is completed
          * on the mapped channel.
          */
            {
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
            },

        /**
         * \brief Mapping of DMA channels to Hardware Events from
         * various peripherals, which use EDMA for data transfer.
         * All channels need not be mapped, some can be free also.
         */
            {
            EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA,
            EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA
            }
        },
    {
        /* EDMA3 INSTANCE# 1 */
        /** Total number of DMA Channels supported by the EDMA3 Controller    */
        EDMA3_NUM_DMA_CHANNELS,
        /** Total number of QDMA Channels supported by the EDMA3 Controller   */
        EDMA3_NUM_QDMA_CHANNELS,
        /** Total number of TCCs supported by the EDMA3 Controller            */
        EDMA3_NUM_TCC,
        /** Total number of PaRAM Sets supported by the EDMA3 Controller      */
        EDMA3_NUM_PARAMSET,
        /** Total number of Event Queues in the EDMA3 Controller              */
        EDMA3_NUM_EVTQUE,
        /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller*/
        EDMA3_NUM_TC,
        /** Number of Regions on this EDMA3 controller                        */
        EDMA3_NUM_REGIONS,

        /**
         * \brief Channel mapping existence
         * A value of 0 (No channel mapping) implies that there is fixed association
         * for a channel number to a parameter entry number or, in other words,
         * PaRAM entry n corresponds to channel n.
         */
        1u,

        /** Existence of memory protection feature */
        0u,

        /** Global Register Region of CC Registers */
        DSP1_EDMA3_CC_BASE_ADDR,
        /** Transfer Controller (TC) Registers */
        {
	        DSP1_EDMA3_TC0_BASE_ADDR,
	        DSP1_EDMA3_TC1_BASE_ADDR,
	        (void *)NULL,
	        (void *)NULL,
            (void *)NULL,
            (void *)NULL,
            (void *)NULL,
            (void *)NULL
        },
        /** Interrupt no. for Transfer Completion */
        DSP1_EDMA3_CC_XFER_COMPLETION_INT,
        /** Interrupt no. for CC Error */
        DSP1_EDMA3_CC_ERROR_INT,
        /** Interrupt no. for TCs Error */
        {
            DSP1_EDMA3_TC0_ERROR_INT,
            DSP1_EDMA3_TC1_ERROR_INT,
            EDMA3_TC2_ERROR_INT,
            EDMA3_TC3_ERROR_INT,
            EDMA3_TC4_ERROR_INT,
            EDMA3_TC5_ERROR_INT,
            EDMA3_TC6_ERROR_INT,
            EDMA3_TC7_ERROR_INT
        },

        /**
         * \brief EDMA3 TC priority setting
         *
         * User can program the priority of the Event Queues
         * at a system-wide level.  This means that the user can set the
         * priority of an IO initiated by either of the TCs (Transfer Controllers)
         * relative to IO initiated by the other bus masters on the
         * device (ARM, DSP, USB, etc)
         */
        {
            0u,
            1u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u
        },
        /**
         * \brief To Configure the Threshold level of number of events
         * that can be queued up in the Event queues. EDMA3CC error register
         * (CCERR) will indicate whether or not at any instant of time the
         * number of events queued up in any of the event queues exceeds
         * or equals the threshold/watermark value that is set
         * in the queue watermark threshold register (QWMTHRA).
         */
        {
            16u,
            16u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u
        },

        /**
         * \brief To Configure the Default Burst Size (DBS) of TCs.
         * An optimally-sized command is defined by the transfer controller
         * default burst size (DBS). Different TCs can have different
         * DBS values. It is defined in Bytes.
         */
            {
            16u,
            16u,
            0u,
            0u,
            0u,
            0u,
            0u,
            0u
            },

        /**
         * \brief Mapping from each DMA channel to a Parameter RAM set,
         * if it exists, otherwise of no use.
         */
            {
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP,
			EDMA3_RM_CH_NO_PARAM_MAP, EDMA3_RM_CH_NO_PARAM_MAP
            },

         /**
          * \brief Mapping from each DMA channel to a TCC. This specific
          * TCC code will be returned when the transfer is completed
          * on the mapped channel.
          */
            {
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
			EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP, EDMA3_RM_CH_NO_TCC_MAP,
            },

        /**
         * \brief Mapping of DMA channels to Hardware Events from
         * various peripherals, which use EDMA for data transfer.
         * All channels need not be mapped, some can be free also.
         */
            {
            EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_DSPEDMA,
            EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_DSPEDMA
            }
    },
};

/**
 * \brief Resource splitting defines for Own/Reserved DMA/QDMA channels and TCCs
 * For PaRAMs explicit defines are not present but should be replaced in the structure sampleInstInitConfig
 * Default configuration has all resources owned by all cores and none reserved except for first 64 PaRAMs corrosponding to DMA channels
 * Resources to be Split properly by application and rebuild the sample library to avoid resource conflict
 *
 * Only Resources owned by a perticular core are allocated by Driver
 * Reserved resources are not allocated if requested for any available resource
 */
 
/* Driver Instance Initialization Configuration */
EDMA3_DRV_InstanceInitConfig sampleInstInitConfig[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
    {
		/* EDMA3 INSTANCE# 0 */
		{
			/* Resources owned/reserved by region 0 (Associated to MPU core 0)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

			/* Resources owned/reserved by region 1 (Associated to MPU core 1) */
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 2 (Associated to any DSP1)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 3 (Associated to any DSP2)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 4 (Associated to any IPU1 core 0)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 5 (Associated to any IPU1 core 1)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 6 (Associated to any IPU2 core 0)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 7 (Associated to any IPU2 core 1)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_STSTEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_STSTEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},
	    },
		/* EDMA3 INSTANCE# 1 DSP1 EDMA*/
		{
	        /* Resources owned/reserved by region 0 (Not Associated to any core supported)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* ownTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},
			},

			/* Resources owned/reserved by region 1 (Not Associated to any core supported) */
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* ownTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},
			},

	        /* Resources owned/reserved by region 2 (Associated to any DSP core 0)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_DSPEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_DSPEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 3 (Associated to any DSP core 1)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 159  128     191  160     223  192     255  224 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 287  256     319  288     351  320     383  352 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu,
				/* 415  384     447  416     479  448     511  480 */
				 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x000000FFu},

				/* ownTccs */
				/* 31     0     63    32 */
				{0xFFFFFFFFu, 0xFFFFFFFFu},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0xFFFFFFFFu, 0xFFFFFFFFu, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_0_DSPEDMA, EDMA3_DMA_CHANNEL_TO_EVENT_MAPPING_1_DSPEDMA},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 4 (Not Associated to any core supported)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* ownTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},
			},

	        /* Resources owned/reserved by region 5 (Not Associated to any core supported)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* ownTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},
			},

	        /* Resources owned/reserved by region 6 (Not Associated to any core supported)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* ownTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},
			},

	        /* Resources owned/reserved by region 7 (Not Associated to any core supported)*/
			{
				/* ownPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* ownDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* ownQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* ownTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdPaRAMSets */
				/* 31     0     63    32     95    64     127   96 */
				{0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 159  128     191  160     223  192     255  224 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 287  256     319  288     351  320     383  352 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
				/* 415  384     447  416     479  448     511  480 */
				 0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u},

				/* resvdDmaChannels */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00000000u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00000000u, 0x00000000u},
			},
	    },
	};

/* Driver Instance Cross bar event to channel map Initialization Configuration */
EDMA3_DRV_GblXbarToChanConfigParams sampleXbarChanInitConfig[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
{
    /* EDMA3 INSTANCE# 0 */
    {
        /* Event to channel map for region 0 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 1 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 2 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 3 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 4 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 5 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 6 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 7 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
    }
};

/* End of File */

