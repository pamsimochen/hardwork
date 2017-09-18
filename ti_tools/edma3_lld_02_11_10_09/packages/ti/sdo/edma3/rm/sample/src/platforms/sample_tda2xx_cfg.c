/*
 * sample_omapl138_cfg.c
 *
 * Platform specific EDMA3 hardware related information like number of transfer
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

#include <ti/sdo/edma3/rm/edma3_rm.h>

/* Number of EDMA3 controllers present in the system */
#define NUM_EDMA3_INSTANCES         1u
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
#define EDMA3_0_NUM_EVTQUE                              (4u)

/** Number of Transfer Controllers available                                  */
#define EDMA3_0_NUM_TC                                  (4u)

/** Number of Regions                                                         */
#define EDMA3_0_NUM_REGIONS                             (2u)


/** Interrupt no. for Transfer Completion                                     */
#define EDMA3_0_CC_XFER_COMPLETION_INT                  (34u)
/** Interrupt no. for CC Error                                                */
#define EDMA3_0_CC_ERROR_INT                            (35u)
/** Interrupt no. for TCs Error                                               */
#define EDMA3_0_TC0_ERROR_INT                           (36u)
#define EDMA3_0_TC1_ERROR_INT                           (37u)
#define EDMA3_0_TC2_ERROR_INT                           (0u)
#define EDMA3_0_TC3_ERROR_INT                           (0u)
#define EDMA3_0_TC4_ERROR_INT                           (0u)
#define EDMA3_0_TC5_ERROR_INT                           (0u)
#define EDMA3_0_TC6_ERROR_INT                           (0u)
#define EDMA3_0_TC7_ERROR_INT                           (0u)

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

#define EDMA3_0_HWI_INT_XFER_COMP                           (7u)
#define EDMA3_0_HWI_INT_CC_ERR                              (7u)
#define EDMA3_0_HWI_INT_TC0_ERR                             (7u)
#define EDMA3_0_HWI_INT_TC1_ERR                             (7u)
#define EDMA3_0_HWI_INT_TC2_ERR                             (7u)
#define EDMA3_0_HWI_INT_TC3_ERR                             (7u)


/**
 * \brief Mapping of DMA channels 0-31 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
                                                      /* 31     0 */
#define EDMA3_0_DMA_CHANNEL_TO_EVENT_MAPPING_0       (0x3FC0C06Eu)  /* TBD */


/**
 * \brief Mapping of DMA channels 32-63 to Hardware Events from
 * various peripherals, which use EDMA for data transfer.
 * All channels need not be mapped, some can be free also.
 * 1: Mapped
 * 0: Not mapped
 *
 * This mapping will be used to allocate DMA channels when user passes
 * EDMA3_RM_DMA_CHANNEL_ANY as dma channel id (for eg to do memory-to-memory
 * copy). The same mapping is used to allocate the TCC when user passes
 * EDMA3_RM_TCC_ANY as tcc id (for eg to do memory-to-memory copy).
 *
 * To allocate more DMA channels or TCCs, one has to modify the event mapping.
 */
#define EDMA3_0_DMA_CHANNEL_TO_EVENT_MAPPING_1       (0xF3FFFFFCu) /* TBD */


/* Variable which will be used internally for referring number of Event Queues*/
unsigned int numEdma3EvtQue[NUM_EDMA3_INSTANCES] =  {
                                                        EDMA3_0_NUM_EVTQUE,
                                                    };

/* Variable which will be used internally for referring number of TCs.        */
unsigned int numEdma3Tc[NUM_EDMA3_INSTANCES] =  {
                                                    EDMA3_0_NUM_TC,
                                                };

/**
 * Variable which will be used internally for referring transfer completion
 * interrupt.
 */
unsigned int ccXferCompInt[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
{
    {
        0u, EDMA3_0_CC_XFER_COMPLETION_INT, 0u, 0u, 0u, 0u, 0u, 0u,
    },
};

/**
 * Variable which will be used internally for referring channel controller's
 * error interrupt.
 */
unsigned int ccErrorInt[NUM_EDMA3_INSTANCES] = {
                                                    EDMA3_0_CC_ERROR_INT,
                                               };

/**
 * Variable which will be used internally for referring transfer controllers'
 * error interrupts.
 */
unsigned int tcErrorInt[NUM_EDMA3_INSTANCES][8] =
{
   {
       EDMA3_0_TC0_ERROR_INT, EDMA3_0_TC1_ERROR_INT,
       EDMA3_0_TC2_ERROR_INT, EDMA3_0_TC3_ERROR_INT,
       EDMA3_0_TC4_ERROR_INT, EDMA3_0_TC5_ERROR_INT,
       EDMA3_0_TC6_ERROR_INT, EDMA3_0_TC7_ERROR_INT,
   }
};

/**
 * Variables which will be used internally for referring the hardware interrupt
 * for various EDMA3 interrupts.
 */
unsigned int hwIntXferComp[NUM_EDMA3_INSTANCES] = {
                                                    EDMA3_0_HWI_INT_XFER_COMP
                                                  };

unsigned int hwIntCcErr[NUM_EDMA3_INSTANCES] = {
                                                   EDMA3_0_HWI_INT_CC_ERR
                                               };

unsigned int hwIntTcErr[NUM_EDMA3_INSTANCES][8] = {
                                                     {
                                                        EDMA3_0_HWI_INT_TC0_ERR,
                                                        EDMA3_0_HWI_INT_TC1_ERR,
                                                        EDMA3_0_HWI_INT_TC2_ERR,
                                                        EDMA3_0_HWI_INT_TC3_ERR
                                                     }
                                               };

#define EDMA3_CC_BASE_ADDR                          ((void *)(0x43300000))
#define EDMA3_TC0_BASE_ADDR                         ((void *)(0x43400000))
#define EDMA3_TC1_BASE_ADDR                         ((void *)(0x43500000))
/* Driver Object Initialization Configuration                                 */
EDMA3_RM_GblConfigParams sampleEdma3GblCfgParams[NUM_EDMA3_INSTANCES] =
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
        EDMA3_0_NUM_EVTQUE,
        /** Total number of Transfer Controllers (TCs) in the EDMA3 Controller*/
        EDMA3_0_NUM_TC,
        /** Number of Regions on this EDMA3 controller                        */
        EDMA3_0_NUM_REGIONS,

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
        EDMA3_0_CC_XFER_COMPLETION_INT,
        /** Interrupt no. for CC Error */
        EDMA3_0_CC_ERROR_INT,
        /** Interrupt no. for TCs Error */
        {
            EDMA3_0_TC0_ERROR_INT,
            EDMA3_0_TC1_ERROR_INT,
            EDMA3_0_TC2_ERROR_INT,
            EDMA3_0_TC3_ERROR_INT,
            EDMA3_0_TC4_ERROR_INT,
            EDMA3_0_TC5_ERROR_INT,
            EDMA3_0_TC6_ERROR_INT,
            EDMA3_0_TC7_ERROR_INT
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
            2u,
            3u,
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
            16u,
            16u,
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
            0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u,
            8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u,
            16u, 17u, 18u, 19u, 20u, 21u, 22u, 23u,
            24u, 25u, 26u, 27u, 28u, 29u, 30u, 31u,
            32u, 33u, 34u, 35u, 36u, 37u, 38u, 39u, 
            40u, 41u, 42u, 43u, 44u, 45u, 46u, 47u,
            48u, 49u, 50u, 51u, 52u, 53u, 54u, 55u,
            56u, 57u, 58u, 59u, 60u, 61u, 62u, 63u
            },

         /**
          * \brief Mapping from each DMA channel to a TCC. This specific
          * TCC code will be returned when the transfer is completed
          * on the mapped channel.
          */
            {
            0u, 1u, 2u, 3u,
            4u, 5u, 6u, 7u,
            8u, 9u, 10u, 11u,
            12u, 13u, 14u, 15u,
            16u, 17u, 18u, 19u,
            20u, 21u, 22u, 23u,
            24u, 25u, 26u, 27u,
            28u, 29u, 30u, 31u,
            32u, 33u, 34u, 35u,
            36u, 37u, 38u, 39u,
            40u, 41u, 42u, 43u,
            44u, 45u, 46u, 47u,
            48u, 49u, 50u, 51u,
            52u, 53u, 54u, 55u,
            56u, 57u, 58u, 59u,
            60u, 61u, 62u, 63u
            },

        /**
         * \brief Mapping of DMA channels to Hardware Events from
         * various peripherals, which use EDMA for data transfer.
         * All channels need not be mapped, some can be free also.
         */
            {
            EDMA3_0_DMA_CHANNEL_TO_EVENT_MAPPING_0,
            EDMA3_0_DMA_CHANNEL_TO_EVENT_MAPPING_1
            }
        },

};


/* Driver Instance Initialization Configuration */
EDMA3_RM_InstanceInitConfig sampleInstInitConfig[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
{
    /* EDMA3 INSTANCE# 0 */
		{
			/* Resources owned/reserved by region 0 (Associated to any MPU core)*/
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
				{0x00u, 0x00u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

			/* Resources owned/reserved by region 1 (Associated to any DSP core) */
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
				{0x00u, 0x00u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 2 (Associated to any IPU0 core)*/
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
				{0x00u, 0x00u},

				/* resvdQdmaChannels */
				/* 31     0 */
				{0x00u},

				/* resvdTccs */
				/* 31     0     63    32 */
				{0x00u, 0x00u},
			},

	        /* Resources owned/reserved by region 3 (Associated to any IPU1 core)*/
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
				{0x00u, 0x00u},

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
EDMA3_RM_GblXbarToChanConfigParams sampleXbarChanInitConfig[NUM_EDMA3_INSTANCES][EDMA3_MAX_REGIONS] =
{
    /* EDMA3 INSTANCE# 0 */
    {
        /* Event to channel map for region 0 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 1 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, 26, 27, -1, -1, -1, -1}
        },
        /* Event to channel map for region 2 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 3 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 4 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 5 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 6 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
        /* Event to channel map for region 7 */
        {
            {-1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1}
        },
    }
};

/* End of File */

