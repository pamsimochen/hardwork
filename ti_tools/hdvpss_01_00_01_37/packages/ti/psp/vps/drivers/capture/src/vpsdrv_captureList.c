/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/drivers/capture/src/vpsdrv_capturePriv.h>
#include <ti/sysbios/hal/Cache.h>

#define VPS_CAPT_DONT_USE_SOC

/*
  This file implements the capture list processing and
  has the list processing task

  This uses APIs from capture list manager (CLM) to implement the logic
  for capture list processing
*/

#define VPS_CAPT_SC_COEFF_MEM_SIZE          (VPS_CAPT_VIP_SC_COEFF_SIZE *      \
                                             VPSHAL_VIP_INST_MAX)
/**< There are at max 10 muxes required to be configured. In addition to this,
     directly connect vip to vpdma requires 3 more mux configuration,
     multich-enable, rgb_low enable and vip enable. So total memory required is
     13 * overlay memory size for a mux. Allocating 15*overlay memory size for a mux. */
#define VPS_CAPT_VIP_CFG_MEM_SIZE           (VPS_CAPT_VIP_CFG_MAX_MUX * VPSHAL_VPDMA_CTRL_DESC_SIZE * 2u)
#define VPS_CAPT_ABORT_DESC_MEM_SIZE        (VPSHAL_VPDMA_CTRL_DESC_SIZE * 256u)
#define VPS_CAPT_ABORT_DESC_PORT_MEM_SIZE   (VPSHAL_VPDMA_CTRL_DESC_SIZE * 256u)
#define VPS_CAPT_CHANGE_CLIENT_INT_MEM_SIZE (VPSHAL_VPDMA_CTRL_DESC_SIZE * 2u)

/* Decriptor memory used by list's that are submitted by CLM  */
UInt8 *gVps_captListMem;
/* Memory used for scaler co-eff's */
UInt8 *gVps_captScCoeffMem;
/* Memory used for VIP data path select config desc */
UInt8 *gVps_captVipConfigMem;
UInt8 *gVps_captAbortDescMem;
/* Abort desc per port: to overcome descriptor not taken by VPDMA case */
UInt8 *gVps_captAbortDescMemPort;
/**< Control descriptor to change interrupt source for client. Space for two
     control descriptor is enough, as this would be used only during create
     time. */
UInt8 *gVps_captChangeClientInt;

/* Stack for capture list processing task */
#pragma DATA_ALIGN(gVps_captTskStackListUpdate, 32)
#pragma DATA_SECTION(gVps_captTskStackListUpdate, ".bss:taskStackSection")
UInt8 gVps_captTskStackListUpdate[VPS_CAPT_TSK_STACK_LIST_UPDATE];

/* Forward declarations */
static Int32 Vps_captSubFrameEnableInt( Vps_CaptObj *pObj );
static Int32 Vps_captSubFrameCfgClientInt ( Vps_CaptObj *pObj );
static Int32 Vps_captAllocListMem(void);
static Int32 Vps_captFreeListMem(void);

/* Init capture list processing */
Int32 Vps_captInitList()
{
    UInt32 instId;
    Semaphore_Params semParams;
    Task_Params tskParams;
    Int32 status;
    Vps_CaptObj *pObj = NULL;

    /* Initialize the capture Object */
    for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId ++)
    {
        pObj = &gVps_captCommonObj.captureObj[instId];

        pObj->listAddr = NULL;
        pObj->listSize = 0u;
    }

    status = Vps_captAllocListMem();
    if (status == FVID2_SOK)
    {
        /* Init CLM, register driver callback to CLM */
        status = Vps_captLmInit(Vps_captIsrListComplete);
        if (status != FVID2_SOK)
        {
            Vps_captFreeListMem();
        }
    }

    if (status == FVID2_SOK)
    {
        /* reset VIP's */
        Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP0_PORTA]);
        Vps_captVipBlockReset(&gVps_captCommonObj.captureObj[VPS_CAPT_INST_VIP1_PORTA]);

        /* create semaphore and task for list processing task */
        Semaphore_Params_init(&semParams);
        Task_Params_init(&tskParams);

        semParams.mode = Semaphore_Mode_BINARY;

        gVps_captCommonObj.semListUpdate =
            Semaphore_create(0, &semParams, NULL);

        if (gVps_captCommonObj.semListUpdate == NULL)
        {
            Vps_captLmDeInit();
            Vps_captFreeListMem();
            return FVID2_EFAIL;
        }

        /* mark to not exit in list processing */
        gVps_captCommonObj.exitListTsk = FALSE;

        tskParams.priority = VPS_CAPT_TSK_PRI_LIST_UPDATE;
        tskParams.stack = gVps_captTskStackListUpdate;
        tskParams.stackSize = sizeof (gVps_captTskStackListUpdate);

        gVps_captCommonObj.tskListUpdate = Task_create(
                                            Vps_captTskListUpdate,
                                            &tskParams,
                                            NULL);

        if (gVps_captCommonObj.tskListUpdate == NULL)
        {

            /* Error - free previoulsy allocated resources */
            Vps_captLmDeInit ();
            Semaphore_delete (&gVps_captCommonObj.semListUpdate);
            Vps_captFreeListMem();
            return (FVID2_EFAIL);
        }
    }

    return status;
}

/* De-Init capture list processing */
Int32 Vps_captDeInitList()
{
    UInt32 sleepTime = 16; /* in OS ticks */

    /* mark to exit list processing task */
    gVps_captCommonObj.exitListTsk = TRUE;

    /* Wake up task to exit it */
    Semaphore_post(gVps_captCommonObj.semListUpdate);

    /* wait for list processing task to get exited */
    Task_sleep(1);

    /* confirm task termination */
    while (Task_Mode_TERMINATED !=
                Task_getMode(gVps_captCommonObj.tskListUpdate))
    {
        Task_sleep(sleepTime);
        sleepTime >>= 1u;
        if(sleepTime == 0u)
        {
          GT_assert( GT_DEFAULT_MASK, 0u);
        }
    }

    /* delete the task */
    Task_delete(&gVps_captCommonObj.tskListUpdate);

    /* de-init CLM */
    Vps_captLmDeInit();

    /* delete semaphore */
    Semaphore_delete(&gVps_captCommonObj.semListUpdate);

    Vps_captFreeListMem();

    return (FVID2_SOK);
}

static Int32 Vps_captAllocListMem(void)
{
    Int32   retVal = FVID2_SOK;

    gVps_captListMem = NULL;
    gVps_captScCoeffMem = NULL;
    gVps_captVipConfigMem = NULL;
    gVps_captAbortDescMem = NULL;
    gVps_captAbortDescMemPort = NULL;
    gVps_captChangeClientInt = NULL;

    gVps_captListMem = VpsUtils_allocDescMem(
                           VPS_CAPT_LIST_SIZE_MAX,
                           VPSHAL_VPDMA_DATA_DESC_SIZE * 16u);
    gVps_captScCoeffMem = VpsUtils_allocDescMem(
                              VPS_CAPT_SC_COEFF_MEM_SIZE,
                              VPSHAL_VPDMA_DATA_DESC_SIZE);
    gVps_captVipConfigMem = VpsUtils_allocDescMem(
                                VPS_CAPT_VIP_CFG_MEM_SIZE,
                                VPSHAL_VPDMA_DATA_DESC_SIZE);
    gVps_captAbortDescMem = VpsUtils_allocDescMem(
                                VPS_CAPT_ABORT_DESC_MEM_SIZE,
                                VPSHAL_VPDMA_DATA_DESC_SIZE);
    gVps_captAbortDescMemPort = VpsUtils_allocDescMem(
                                    VPS_CAPT_ABORT_DESC_PORT_MEM_SIZE,
                                    VPSHAL_VPDMA_DATA_DESC_SIZE);
    gVps_captChangeClientInt = VpsUtils_allocDescMem(
                                   VPS_CAPT_CHANGE_CLIENT_INT_MEM_SIZE,
                                   VPSHAL_VPDMA_DATA_DESC_SIZE);
    if ((NULL == gVps_captListMem) ||
        (NULL == gVps_captScCoeffMem) ||
        (NULL == gVps_captVipConfigMem) ||
        (NULL == gVps_captAbortDescMem) ||
        (NULL == gVps_captAbortDescMemPort) ||
        (NULL == gVps_captChangeClientInt))
    {
        Vps_captFreeListMem();
        retVal = FVID2_EALLOC;
    }
    else
    {
        /* Initialize all memory to zero */
        memset(gVps_captListMem, 0, VPS_CAPT_LIST_SIZE_MAX);
        memset(gVps_captScCoeffMem, 0, VPS_CAPT_SC_COEFF_MEM_SIZE);
        memset(gVps_captVipConfigMem, 0, VPS_CAPT_VIP_CFG_MEM_SIZE);
        memset(gVps_captAbortDescMem, 0, VPS_CAPT_ABORT_DESC_MEM_SIZE);
        memset(gVps_captAbortDescMemPort, 0, VPS_CAPT_ABORT_DESC_PORT_MEM_SIZE);
        memset(gVps_captChangeClientInt, 0, VPS_CAPT_CHANGE_CLIENT_INT_MEM_SIZE);
    }

    return (retVal);
}

static Int32 Vps_captFreeListMem(void)
{
    Int32   retVal = FVID2_SOK;

    if (NULL != gVps_captListMem)
    {
        VpsUtils_freeDescMem(gVps_captListMem, VPS_CAPT_LIST_SIZE_MAX);
        gVps_captListMem = NULL;
    }

    if (NULL != gVps_captScCoeffMem)
    {
        VpsUtils_freeDescMem(gVps_captScCoeffMem, VPS_CAPT_SC_COEFF_MEM_SIZE);
        gVps_captScCoeffMem = NULL;
    }

    if (NULL != gVps_captVipConfigMem)
    {
        VpsUtils_freeDescMem(gVps_captVipConfigMem, VPS_CAPT_VIP_CFG_MEM_SIZE);
        gVps_captVipConfigMem = NULL;
    }

    if (NULL != gVps_captAbortDescMem)
    {
        VpsUtils_freeDescMem(gVps_captAbortDescMem,
            VPS_CAPT_ABORT_DESC_MEM_SIZE);
        gVps_captAbortDescMem = NULL;
    }

    if (NULL != gVps_captAbortDescMemPort)
    {
        VpsUtils_freeDescMem(
            gVps_captAbortDescMemPort,
            VPS_CAPT_ABORT_DESC_PORT_MEM_SIZE);
        gVps_captAbortDescMemPort = NULL;
    }

    if (NULL != gVps_captChangeClientInt)
    {
        VpsUtils_freeDescMem(
            gVps_captChangeClientInt,
            VPS_CAPT_CHANGE_CLIENT_INT_MEM_SIZE);
        gVps_captChangeClientInt = NULL;
    }

    return (retVal);
}

/* Add to list descriptor for a given capture instance Start the list
   if not already started */
Int32 Vps_captStartList(Vps_CaptObj *pObj)
{
    Int32 lmState;

    /* take CLM lock */
    Vps_captLmLock();

    lmState = Vps_captLmGetState();

    /* Prepare the List of descriptor for this object */
    Vps_captMakeList(pObj);
    /* Mark all the descriptors to be updated */
    pObj->descMask = 0xFFFFFFFFu;
    if (FVID2_BUF_FMT_FRAME == pObj->frameCapture)
    {
        Vps_captInitializeList(pObj);
    }
    /* Mark object state as START state
     * list processing thread will add descriptor to next list that is submitted.
     * It will also start the VIP and change instance state
     * from START to RUNNING */
    pObj->state = VPS_CAPT_STATE_DO_START;

    if (VPS_CAPT_LM_STATE_IDLE == lmState)
    {
        /* Start Timer and mark CLM as running */
        Vps_captLmSubmit(VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS);

#ifdef VPS_CAPT_DEBUG_LIST
        Vps_printf ( " %s:%d: Starting VIP for instance %d !!!\n",
                        __FUNCTION__, __LINE__, pObj->instanceId );
#endif
    }

    /* release lock so that list processing can take place */
    Vps_captLmUnlock();

    /* wait until current instance descriptors get added to
     * next list submission
     */
    Semaphore_pend(pObj->semStartDone, BIOS_WAIT_FOREVER);

#ifndef VPS_CAPT_DONT_USE_SOC
    Vps_captSubFrameCfgClientInt ( pObj );
#endif /* VPS_CAPT_DONT_USE_SOC */

    /* mark instance state as running since descriptors are submitted
     * and VIP is started
     */
    pObj->state = VPS_CAPT_STATE_RUNNING;

    return (FVID2_SOK);
}

Int32 Vps_captPostAbortList(Vps_CaptObj *pObj)
{
    UInt8 *listAddr;
    UInt32 listSize;
    UInt32 chId, streamId, vChId;

    listAddr = &gVps_captAbortDescMem[0];
    listSize = 0;

    for (chId = 0u; chId < pObj->numCh; chId++)
    {
        for (streamId = 0u; streamId < pObj->numStream; streamId ++)
        {
            for (vChId = 0; vChId < pObj->chObj[streamId][chId].vChannelNum; vChId ++)
            {
                GT_assert(GT_DEFAULT_MASK,
                    (vChId < VPS_CAPT_VCH_PER_LCH_MAX));
                VpsHal_vpdmaCreateAbortCtrlDesc(
                    listAddr+listSize,
                    pObj->chObj[streamId][chId].vChannelId[vChId]);

                listSize += VPSHAL_VPDMA_CTRL_DESC_SIZE;

                GT_assert(GT_DEFAULT_MASK,
                    listSize < VPS_CAPT_ABORT_DESC_MEM_SIZE);
            }
        }
    }

    Vps_captLmPostList(listAddr, listSize);

    return (FVID2_SOK);
}

/* Stop list for a driver instance Requests CLM to do the list
   list stop sequence and waits until its complete */
Int32 Vps_captStopList(Vps_CaptObj *pObj)
{
    volatile unsigned int delayinMs = 1;

    /* lock CLM */
    Vps_captLmLock();

    /* change state to STOP */
    pObj->stateDeleteCount = 0;
    pObj->state = VPS_CAPT_STATE_DO_STOP;

    Vps_captStopVip(pObj);

    /* Move all descriptors from the Temp Queue to Empty Queue */
    Vps_captTskMvDescTempToEmpty(pObj);

    pObj->state = VPS_CAPT_STATE_STOPPED;

    /* list processing will add abort descriptor in next list submission
     * It will also stop the VIP and also change object state to DELETE */

    /* unlock CLM so that list processing can continue */
    Vps_captLmUnlock();

#ifdef VPS_CAPT_DEBUG_LIST
    Vps_printf ( " %s:%d: Waiting VIP stop for instance %d !!!\n",
                    __FUNCTION__, __LINE__, pObj->instanceId );
#endif

    return FVID2_SOK;
}

Int32 Vps_captPostAllAbortList(VpsHal_VipInst vipInstId)
{
    UInt8 *listAddr;
    UInt32 listSize;
    VpsHal_VpdmaChannel vChannel, vChStart, vChEnd;

    listAddr = &gVps_captAbortDescMem[0];
    listSize = 0;

    vChStart = VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC0;
    vChEnd   = VPSHAL_VPDMA_CHANNEL_VIP0_PORTB_RGB;

    if (vipInstId == VPSHAL_VIP_INST_1)
    {
        vChStart = VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC0;
        vChEnd   = VPSHAL_VPDMA_CHANNEL_VIP1_PORTB_RGB;
    }

    for (vChannel = vChStart; vChannel <= vChEnd; vChannel ++)
    {
        VpsHal_vpdmaCreateAbortCtrlDesc(listAddr+listSize, vChannel);

        listSize += VPSHAL_VPDMA_CTRL_DESC_SIZE;

        GT_assert(GT_DEFAULT_MASK,
            listSize < VPS_CAPT_ABORT_DESC_MEM_SIZE);
    }

    Vps_captLmPostList(listAddr, listSize);

    return 0;
}

/* for creatign and posting abort descriptors when there is descriptor miss*/
Int32 Vps_captPostAbortListPerPort(Vps_CaptObj *pObj)
{
    UInt8 *listAddr;
    UInt32 listSize, numvChs=0,vChId, streamId, chId;
    VpsHal_VpdmaChannel vChannel;

    listAddr = &gVps_captAbortDescMemPort[0];
    listSize = 0;

    for (streamId=0; streamId < pObj->numStream;streamId++)
    {
        for (chId=0; chId < pObj->numCh;chId++)
        {
            numvChs = pObj->chObj[streamId][chId].vChannelNum;
            for (vChId=0; vChId <numvChs; vChId++)
            {
                GT_assert(GT_DEFAULT_MASK,
                    (vChId < VPS_CAPT_VCH_PER_LCH_MAX));
                vChannel = pObj->chObj[streamId][chId].vChannelId[vChId];
                VpsHal_vpdmaCreateAbortCtrlDesc(listAddr+listSize, vChannel);

                listSize += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                GT_assert(GT_DEFAULT_MASK,
                    listSize < VPS_CAPT_ABORT_DESC_PORT_MEM_SIZE);
            }

        }
    }

    Vps_captLmPostList(listAddr, listSize);

    return 0;
}

Ptr Vps_captGetScCoeffMem(UInt32 vipInstId)
{
    GT_assert( GT_DEFAULT_MASK,  vipInstId < VPSHAL_VIP_INST_MAX );

    return &gVps_captScCoeffMem[VPS_CAPT_VIP_SC_COEFF_SIZE * vipInstId];
}


/* Driver callback called by CLM with timer expires */
Void Vps_captIsrListComplete()
{
    /* get current time */
    gVps_captCommonObj.clockTicks = Clock_getTicks();

    /* trigger list processing task */
    Semaphore_post(gVps_captCommonObj.semListUpdate);
}

Void Vps_captResetStatitics(Vps_CaptObj *pObj, Vps_CaptChObj *pChObj)
{
    if(!pObj->resetStatistics)
        return;

    pChObj->printFid = 0;

    pChObj->frameCount = 0;
    pChObj->droppedFrameCount = 0;
    pChObj->captureFrameCount = 0;

    pChObj->fieldCount[0] = 0;
    pChObj->fieldCount[1] = 0;

    pChObj->lastFrameFid = 0;
    pChObj->lastFrameTimestamp = 0;
    pChObj->lastFrameWidth = 0;
    pChObj->lastFrameHeight = 0;

    pChObj->minRecvFrameWidth = 0xFFFF;
    pChObj->maxRecvFrameWidth = 0;

    pChObj->minRecvFrameHeight = 0xFFFF;
    pChObj->maxRecvFrameHeight = 0;

    pChObj->frameNotWrittenCount = 0u;
    pChObj->secFrameNotWrittenCount = 0u;
    pChObj->descNotWrittenCount = 0u;
    pChObj->secDescNotWrittenCount = 0u;

    pObj->firstDescMissMatch = 0;
    pObj->secDescMissMatch = 0;
    pObj->thirdDescMissMatch = 0;

#ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
    memset(&(pChObj->errTimeStamps), 0x0, sizeof (pChObj->errTimeStamps));
    pChObj->errTimeStampIndex = 0x0;
#endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */

#ifdef VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE
    memset (&(pChObj->fidRptTimeStamps), 0x0, sizeof(pChObj->fidRptTimeStamps));
    pChObj->fidRptIndex = 0x0;
#endif /* VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE */

    pChObj->fidRepeatCount = 0;

    pChObj->numDropData = 0u;
    
    /* Frame / Fields Time stamp stats */
    GT_assert( GT_DEFAULT_MASK,  pChObj->getCurrTime != NULL);
#ifdef VPS_CAPT_PRINT_TS_STATS
    pChObj->tsMinDiff   = 0xFFFFFFF;
    pChObj->tsMaxDiff   = 0;
    pChObj->tsErr       = 0;
    pChObj->tsLast      = 0;
    pChObj->tsCummulative           = 0;
    pChObj->tsMissedFrameCnt        = 0;
    pChObj->tsMultiStampFrameCnt    = 0;
#endif /* VPS_CAPT_PRINT_TS_STATS */

#ifdef VPS_CAPTURE_TS_DEBUG
    pChObj->tsDrvDbgIndex = 0;
    pChObj->tsDrvHDbgIndex = 0;
    pChObj->tsIsrDbgIndex = 0;
    pChObj->tsIsrHDbgIndex = 0;
    pChObj->tsDbgLastTs = 0;
#endif /* VPS_CAPTURE_TS_DEBUG */

}

Void Vps_captUpdateStatitics(Vps_CaptObj *pObj,
                                    Vps_CaptChObj *pChObj,
                                    Vps_CaptLmDataDescParseInfo *parseDescInfo)
{
    if ((pChObj->lastFrameWidth < pChObj->minRecvFrameWidth) &&
        (0u != pChObj->lastFrameWidth)
            &&
        (1u != pChObj->lastFrameWidth)
        )
    {
        pChObj->minRecvFrameWidth = pChObj->lastFrameWidth;
    }
    if (pChObj->lastFrameWidth > pChObj->maxRecvFrameWidth)
    {
        pChObj->maxRecvFrameWidth = pChObj->lastFrameWidth;
    }

    if ((pChObj->lastFrameHeight < pChObj->minRecvFrameHeight) &&
        (0u != pChObj->lastFrameHeight)
            &&
        (1u != pChObj->lastFrameHeight)
        )
    {
        pChObj->minRecvFrameHeight = pChObj->lastFrameHeight;
    }
    if (pChObj->lastFrameHeight > pChObj->maxRecvFrameHeight)
    {
        pChObj->maxRecvFrameHeight = pChObj->lastFrameHeight;
    }

    pChObj->fieldCount[pChObj->lastFrameFid & 1] ++;
}

Int32 Vps_captTskMvDescTempToEmpty(Vps_CaptObj *pObj)
{
    Int32 retVal = FVID2_SOK;
    UInt32 chId, streamId;
    Vps_CaptChObj *pChObj;
    FVID2_Frame   *pFrame;

    for (chId = 0u; chId < pObj->numCh; chId ++)
    {
        for (streamId = 0u; streamId < pObj->numStream; streamId ++)
        {
            pChObj = &pObj->chObj[streamId][chId];
            /* Move all the channels from Temp Queue to EmptyQueue as
               they are all aborted */
            do
            {
                retVal = VpsUtils_queGet(
                            &pChObj->tmpQue,
                            (Ptr *)&pFrame,
                            1,
                            BIOS_NO_WAIT );

                if (FVID2_SOK == retVal)
                {
                    GT_assert( GT_DEFAULT_MASK, pFrame != NULL );

                    if (pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID)
                    {
                        retVal = VpsUtils_quePut(
                                    &pChObj->emptyQue,
                                    pFrame,
                                    BIOS_NO_WAIT );
                        GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);
                    }
                }
            } while(FVID2_SOK == retVal);

            if (pChObj->tsUseBestEffort == TRUE)
            {
                Vps_captTimeStampFlushQue( pChObj );
            }

            /*
             * since pTmpFrame is not in tmp queue, move that also to empty queue
             */
            if (FVID2_BUF_FMT_FIELD == pChObj->frameCapture)
            {
                if (pChObj->pTmpFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID)
                {
                    retVal = VpsUtils_quePut(
                                &pChObj->emptyQue,
                                pChObj->pTmpFrame,
                                BIOS_NO_WAIT);

                    GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);
                }
            }
            else
            {
                if ((NULL != pChObj->pPrevFrame) &&
                    (pChObj->pPrevFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID))
                {
                    retVal = VpsUtils_quePut(
                                &pChObj->emptyQue,
                                pChObj->pPrevFrame,
                                BIOS_NO_WAIT);

                    GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);
                }
                if ((NULL != pChObj->pNxtFrame) && (pChObj->pPrevFrame != pChObj->pNxtFrame) &&
                    (pChObj->pNxtFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID))
                {
                    retVal = VpsUtils_quePut(
                                &pChObj->emptyQue,
                                pChObj->pNxtFrame,
                                BIOS_NO_WAIT);
                }
            }
        }
    }

    return (FVID2_SOK);
}

Int32 Vps_captTskListDoStop()
{
    UInt32 instId;
    Vps_CaptObj *pObj;

    /* for all instances */
    for (instId = 0; instId < VPS_CAPT_INST_MAX; instId++ )
    {
        pObj = &gVps_captCommonObj.captureObj[instId];

        if (pObj->state == VPS_CAPT_STATE_DO_STOP)
        {
            /* Stop the VIP */
            Vps_captStopVip(pObj);

            /* Move all descriptors from the Temp Queue to Empty Queue */
            Vps_captTskMvDescTempToEmpty(pObj);

            pObj->state = VPS_CAPT_STATE_STOPPED;

            /* post semaphore to wake up calling function */
            Semaphore_post(pObj->semStopDone);
        }
    }

    return FVID2_SOK;
}


Int32 Vps_captTskListDoVipStart()
{
    UInt32 instId;
    Vps_CaptObj *pObj;

    /* for every instance */
    for (instId = 0; instId < VPS_CAPT_INST_MAX; instId ++)
    {
        pObj = &gVps_captCommonObj.captureObj[instId];

        if (pObj->state == VPS_CAPT_STATE_DO_START)
        {

            /* instance is in start state, descriptor were added
             * and list submitted earlier now start VIP port for
             * this instance and mark state as RUNNING */

#ifdef VPS_CAPT_DEBUG_LIST
            Vps_printf ( " %s:%d: Starting VIP for instance !!!\n",
                            __FUNCTION__, __LINE__, instId );
#endif

            Vps_captStartVip(pObj);
            pObj->state = VPS_CAPT_STATE_RUNNING;
            Semaphore_post(pObj->semStartDone);
        }
    }

    return FVID2_SOK;
}


Int32 Vps_captTskListDoCallback()
{
    UInt32 instId;
    Vps_CaptObj *pObj;

    /* for all instances */
    for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId ++)
    {
        pObj = &gVps_captCommonObj.captureObj[instId];

        if ((pObj->state == VPS_CAPT_STATE_RUNNING)
             && (pObj->cbPrm.fdmCbFxn != NULL))
        {
            /* if instance state is running and user callback registered */
            if (pObj->createArgs.periodicCallbackEnable ||
                Vps_captIsDataAvailable(pObj))
            {
                /* callback is to called always  OR
                 * data available at any of the channels associated
                 * with this instance */

                /* call user callback */
                if (TRUE == pObj->oneCallBackPerFrm)
                {
                    if (TRUE == Vps_captIsDataAvailabeInOutQue1(pObj))
                    {
                        pObj->cbPrm.fdmCbFxn(pObj->cbPrm.fdmData, pObj);
                    }
                }
                else
                {
                    pObj->cbPrm.fdmCbFxn(pObj->cbPrm.fdmData, pObj);
                }
            }
        }
    }

    return FVID2_SOK;
}

/* list processing task */
Void Vps_captTskListUpdate(UArg arg1, UArg arg2)
{
    Int32 status;

    VpsHal_vpdmaSetPerfMonRegs();

    /* loop for ever */
    while(1u)
    {

        /* wait from trigger from CLM callback, i.e when timer expires */
        status = Semaphore_pend(
                    gVps_captCommonObj.semListUpdate,
                    BIOS_WAIT_FOREVER);

        if (gVps_captCommonObj.exitListTsk)
        {
#ifdef VPS_CAPT_DEBUG_LIST
            Vps_printf ( " %s:%d: Exiting LM Task !!!\n", __FUNCTION__,
                            __LINE__ );
#endif

            return;
        }

        if (!status)
        {
            /* error - continue looping */
            Vps_printf ( " CAPTURE_LM: %s:%d: ERROR !!! \n",
                            __FUNCTION__, __LINE__ );
            continue;
        }

        /* lock CLM */
        Vps_captLmLock();

        Vps_captTskListDoVipStart();

        /* First post the list and then updated the list for framebased
         * capture
         */
        /* Post the new list */
        Vps_captTskSubmitFrameList();

        /* Update Buffer Address in the Descriptor as the mask */
        Vps_captTskUpdateList();

        /* Post the new list */
        Vps_captTskSubmitFieldList();

        #if 0
        /* Stopping the Port */
        Vps_captTskListDoStop();
        #endif

        /* unlock CLM */
        Vps_captLmUnlock();
        
        /* call user callbacks if required */
        Vps_captTskListDoCallback();
    }
}

/*
  Add channel descriptor to list
*/
Int32 Vps_captAddToList(Vps_CaptObj *pObj, Vps_CaptChObj *pChObj)
{
    UInt16 vChId, writeDescSet;
    Vps_CaptLmDataDescAddInfo dataDescInfo;

    Vps_captResetStatitics(pObj, pChObj);

    pChObj->pTmpFrame = &pChObj->dropFrame[0];

    if (pChObj->isSubFrameEnabled == TRUE)
    {
        /* Queup the current field being captured */
        pChObj->subFrameObj.currCaptField =
            Vps_captVipSubFrameAddQObj ( pChObj, pChObj->pTmpFrame );

        GT_assert( GT_DEFAULT_MASK,
                  (pChObj->subFrameObj.currCaptField != NULL) );
    }

    /* add a descritpor for every associated VPDMA channel */
    for (vChId = 0; vChId < pChObj->vChannelNum; vChId ++)
    {
        memset(&dataDescInfo, 0, sizeof(dataDescInfo));

        /* descritpor information */
        dataDescInfo.dataType = pChObj->vChannelDataType[vChId];
        dataDescInfo.priority = VPS_CFG_CAPT_VPDMA_PRIORITY;
        dataDescInfo.memType = (VpsHal_VpdmaMemoryType) pChObj->memType;
        dataDescInfo.maxOutHeight = pChObj->maxOutHeight;
        dataDescInfo.maxOutWidth  = pChObj->maxOutWidth ;
        dataDescInfo.lineSkip = VPSHAL_VPDMA_LS_1;
        dataDescInfo.lineStride = pChObj->pitch[vChId];
        dataDescInfo.enableDescSkip = TRUE;

        dataDescInfo.addr = pChObj->pTmpFrame->addr[0][vChId];

        dataDescInfo.vChannel = pChObj->vChannelId[vChId];

        /*
         * map VPDMA channel to driver logical channel
         */
        Vps_captLmMapChannel(pChObj->vChannelId[vChId],
                             pChObj->lChannelNum,
                             vChId);

        /* Calculate the descriptor address */
        dataDescInfo.descAddr = pObj->listAddr + pObj->listSize;
        pObj->listSize += VPSHAL_VPDMA_DATA_DESC_SIZE;

        /* add descriptor to list */
        Vps_captLmAddDataDesc(&dataDescInfo);

        /* assert if list size exceeds max list size - this should not
         * happen in normal case */
        GT_assert( GT_DEFAULT_MASK,  pObj->listSize < VPS_CAPT_DATA_DESC_SIZE_PER_PORT);

        /* Initialize the Write Descriptor Address */
        for (writeDescSet = 0u;
                writeDescSet < VPS_CAPT_NUM_WRITE_DATA_DESC_SET;
                writeDescSet ++)
        {
            pChObj->writeDescAddr[vChId][writeDescSet] =
                dataDescInfo.descAddr +
                VPS_CAPT_NUM_DATA_DESC_SIZE +
                VPS_CAPT_NUM_DATA_DESC_SIZE * writeDescSet;

            memset(&pChObj->dropFrame[writeDescSet], 0, sizeof(pChObj->dropFrame[writeDescSet]));
            pChObj->dropFrame[writeDescSet].channelNum = VPS_CAPT_DROP_FRAME_CH_ID;
            pChObj->dropFrame[writeDescSet].addr[0][0] = gVps_captCommonObj.dropDataAddr;
            pChObj->dropFrame[writeDescSet].addr[0][1] = gVps_captCommonObj.dropDataAddr;
            /* Requires to be initialized with some postive number, to ensure
                we do not time stamp the dummy frames */
            pChObj->dropFrame[writeDescSet].timeStamp  = VPS_CAPT_DROP_FRAME_CH_ID;
        }

        pChObj->writeDescIdx[vChId] = 0u;
    }

    return (FVID2_SOK);
}

/*
  Add channel descriptor to list
*/
Int32 Vps_captAddFrmToList(Vps_CaptObj *pObj, Vps_CaptChObj *pChObj, UInt32
                            descWrAddOffset)
{
    UInt16 vChId, writeDescSet;
    Vps_CaptLmDataDescAddInfo dataDescInfo;

    Vps_captResetStatitics(pObj, pChObj);
    pChObj->pPrevFrame = NULL;
    pChObj->pNxtFrame = pChObj->pPrevFrame;
    pChObj->programmedField[0] = FVID2_FID_EVEN;
    pChObj->programmedField[1] = FVID2_FID_ODD;
    pChObj->expectedDescSet = 0;
    pChObj->frameCaptureState = VPS_CAPT_STATE_0;


    pChObj->pTmpFrame = &pChObj->dropFrame[0];

    /* add a descritpor for every associated VPDMA channel */
    for (vChId = 0; vChId < pChObj->vChannelNum; vChId ++)
    {
        memset(&dataDescInfo, 0, sizeof(dataDescInfo));

        /* descritpor information */
        dataDescInfo.dataType = pChObj->vChannelDataType[vChId];
        dataDescInfo.priority = VPS_CFG_CAPT_VPDMA_PRIORITY;
        dataDescInfo.memType = (VpsHal_VpdmaMemoryType) pChObj->memType;
        dataDescInfo.maxOutHeight = pChObj->maxOutHeight;
        dataDescInfo.maxOutWidth  = pChObj->maxOutWidth ;
        if ((FVID2_BUF_FMT_FRAME == pChObj->frameCapture) &&
            (TRUE == pChObj->fieldsMerged))
        {
            dataDescInfo.lineSkip = VPSHAL_VPDMA_LS_2;
        }
        else
        {
            dataDescInfo.lineSkip = VPSHAL_VPDMA_LS_1;
        }
        dataDescInfo.lineStride = pChObj->pitch[vChId];
        dataDescInfo.enableDescSkip = TRUE;

        dataDescInfo.addr = pChObj->pTmpFrame->addr[0][vChId];

        dataDescInfo.vChannel = (VpsHal_VpdmaChannel) pChObj->vChannelId[vChId];

        /*
         * map VPDMA channel to driver logical channel
         */
        Vps_captLmMapChannel(pChObj->vChannelId[vChId],
                             pChObj->lChannelNum,
                             vChId);

        /* Calculate the descriptor address */
        dataDescInfo.descAddr = pObj->listAddr + pObj->listSize;
        pObj->listSize += VPSHAL_VPDMA_DATA_DESC_SIZE;

        /* add descriptor to list */
        Vps_captLmAddDataDesc(&dataDescInfo);

        /* assert if list size exceeds max list size - this should not
         * happen in normal case */
        GT_assert( GT_DEFAULT_MASK,  pObj->listSize < VPS_CAPT_DATA_DESC_SIZE_PER_PORT);

        /* Initialize the Write Descriptor Address */
        for (writeDescSet = 0u;
                writeDescSet < VPS_CAPT_NUM_WRITE_DATA_DESC_SET;
                writeDescSet ++)
        {
            pChObj->writeDescAddr[vChId][writeDescSet] =
                (Ptr)(descWrAddOffset + (VPSHAL_VPDMA_DATA_DESC_SIZE * vChId) +
                VPS_CAPT_NUM_DATA_DESC_SIZE * writeDescSet);

            memset(&pChObj->dropFrame[writeDescSet], 0, sizeof(pChObj->dropFrame[writeDescSet]));
            pChObj->dropFrame[writeDescSet].channelNum = VPS_CAPT_DROP_FRAME_CH_ID;
            pChObj->dropFrame[writeDescSet].addr[0][0] = gVps_captCommonObj.dropDataAddr;
            pChObj->dropFrame[writeDescSet].addr[0][1] = gVps_captCommonObj.dropDataAddr;
            pChObj->dropFrame[writeDescSet].addr[1][0] = gVps_captCommonObj.dropDataAddr;
            pChObj->dropFrame[writeDescSet].addr[1][1] = gVps_captCommonObj.dropDataAddr;
            
            /* Requires to be initialized with some postive number, to ensure
                we do not time stamp the dummy frames */
            pChObj->dropFrame[writeDescSet].timeStamp  = VPS_CAPT_DROP_FRAME_CH_ID;
        }

        pChObj->writeDescIdx[vChId] = 0u;
    }

    return (FVID2_SOK);
}

UInt8 *Vps_captGetListAddr(Vps_CaptObj *pObj)
{
    return (UInt8 *)(&gVps_captListMem[pObj->instanceId *
                        VPS_CAPT_DATA_DESC_SIZE_PER_PORT]);
}

Int32 Vps_captTskUpdateList()
{
    Int32                           status = FVID2_SOK;
    UInt32                          instId, streamId, chId, descCnt, descCnt1;
    UInt32                          numDesc;
    Vps_CaptLmDataDescParseInfo     parseDescInfo, parseDescInfo1;
    Vps_CaptObj                    *pObj = NULL;
    Vps_CaptChObj                  *pChObj = NULL;

    for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId ++)
    {
        /* Get the Capture Object */
        pObj = &gVps_captCommonObj.captureObj[instId];

        /* If capture is not running or starting, there is no need to
           update it */
        if (pObj->state != VPS_CAPT_STATE_RUNNING)
        {
            continue;
        }

        pObj->loopCount ++;

        numDesc = pObj->listSize / VPSHAL_VPDMA_DATA_DESC_SIZE;

        /* for each descriptor in the list, see if it is required to update
           the buffer address */
        for (descCnt = 0u; descCnt < numDesc; descCnt ++)
        {
            if (((pObj->descMask >> descCnt) & 0x1u) == 0x0u)
            {
                continue;
            }

            /* Calculate the descriptor pointer */
            parseDescInfo.descAddr =
                pObj->listAddr + VPSHAL_VPDMA_DATA_DESC_SIZE * descCnt;

            /* Get the channel number from the descriptor */
            status = Vps_captLmParseDataDesc(&parseDescInfo);

            if (status != FVID2_SOK)
            {
                /* error in parsing skip it */
#ifdef VPS_CAPT_DEBUG_LIST
                    Vps_printf ( " %s:%d: Desc %d parsing error !!!\n",
                                    __FUNCTION__, __LINE__, parseDescInfo.descAddr );
#endif
                gVps_captCommonObj.descParseErrorCount++;
                continue;
            }
            /* get driver inst associated with this descriptor */
            streamId = Vps_captGetStreamId(parseDescInfo.lChannel);
            chId = Vps_captGetChId(parseDescInfo.lChannel);

            /* validate streamId and channelId */
            if (streamId >= pObj->numStream)
            {
                Vps_printf
                    ( " %s:%d: ERROR: Received stream Id %d >= Number of streams %d (%d) !!!\n",
                      __FUNCTION__, __LINE__, streamId, pObj->numStream,
                      parseDescInfo.lChannel );

                return -1;
            }
            if (chId >= pObj->numCh)
            {
                Vps_printf
                    ( " %s:%d: ERROR: Received stream Id %d >= Number of streams %d (%d) !!!\n",
                      __FUNCTION__, __LINE__, chId, pObj->numCh,
                      parseDescInfo.lChannel );

                return -1;
            }

            /* get associated channel object */
            pChObj = &pObj->chObj[streamId][chId];
            pChObj->vChannelRecv ++;
            if (pChObj->tsUseBestEffort == FALSE)
            {
                GT_assert( GT_DEFAULT_MASK,  pChObj->getCurrTime != NULL);
                pChObj->lastFrameTimestamp = pChObj->getCurrTime(NULL);
            }
            /* Put the captured frame in the output Queue so that
               it can be Dequeued */
            if ((FVID2_BUF_FMT_FRAME == pChObj->frameCapture) &&
                    (pChObj->frameCaptureState >= VPS_CAPT_STATE_3))
            {
                Vps_CaptTskPutCaptFrm(pObj, pChObj, &parseDescInfo);
            }
            else if (FVID2_BUF_FMT_FIELD == pChObj->frameCapture)
            {
                Vps_CaptTskPutCaptField(pObj, pChObj, &parseDescInfo);
            }

            /* If this channel needs more than one descriptor and first
               descriptor is received, it is assumed here that all
               descriptor would have completed by this time, so
               setting flag descMask to make sure that buffer
               address is getting programmed correctly */
            if ((pChObj->vChannelNum > 1u) && (1u == pChObj->vChannelRecv))
            {
                for (descCnt1 = descCnt+1; descCnt1 < numDesc; descCnt1 ++)
                {
                    /* Calculate the descriptor pointer */
                    parseDescInfo1.descAddr =
                        pObj->listAddr + VPSHAL_VPDMA_DATA_DESC_SIZE * descCnt1;

                    /* Get the channel number from the descriptor */
                    status = Vps_captLmParseDataDesc(&parseDescInfo1);
                    if (parseDescInfo1.lChannel == parseDescInfo.lChannel)
                    {
                        if (0 == (pObj->descMask & (1u << descCnt1)))
                        {
                            pChObj->secDescNotWrittenCount ++;
                        }
                        pObj->descMask |= (1u << descCnt1);
                    }
                }
            }
            if ((FVID2_BUF_FMT_FRAME == pChObj->frameCapture) &&
                    (pChObj->frameCaptureState >= VPS_CAPT_STATE_3))
            {
                /* Update the Descriptor with the new buffer address */
                Vps_CaptTskUpdateFrm(pObj, pChObj, &parseDescInfo);
            }
            else if (FVID2_BUF_FMT_FIELD == pChObj->frameCapture)
            {
                /* Update the Descriptor with the new buffer address */
                Vps_CaptTskUpdateField(pObj, pChObj, &parseDescInfo);
            }
            if ((FVID2_BUF_FMT_FRAME == pChObj->frameCapture) &&
                        (pChObj->frameCaptureState < VPS_CAPT_STATE_3))
            {
                Vps_captProgInitialFrms(pObj, pChObj, &parseDescInfo);
            }

            /* If all the descriptors required for this channel
               are received, reset the total descriptor count to zero */
            if (pChObj->vChannelRecv >= pChObj->vChannelNum)
            {
                pChObj->vChannelRecv = 0u;

                /* Increment frame count and start from 0 if it reaches to 30 as
                   frameSkipMask can have only 30 bits */

                pChObj->frameCount = (pChObj->frameCount + 1u) % 30u;
            }
        }
    }

    return (status);
}

Int32 Vps_captInitializeList(Vps_CaptObj *pObj)
{
    Int32                           status = FVID2_SOK;
    UInt32                          streamId, chId, descCnt, descCnt1;
    UInt32                          numDesc;
    Vps_CaptLmDataDescParseInfo     parseDescInfo, parseDescInfo1;
    Vps_CaptChObj                  *pChObj = NULL;

    pObj->loopCount ++;

    numDesc = pObj->listSize / VPSHAL_VPDMA_DATA_DESC_SIZE;

    /* for each descriptor in the list, see if it is required to update
       the buffer address */
    for (descCnt = 0u; descCnt < numDesc; descCnt ++)
    {
        if (((pObj->descMask >> descCnt) & 0x1u) == 0x0u)
        {
            continue;
        }

        /* Calculate the descriptor pointer */
        parseDescInfo.descAddr =
            pObj->listAddr + VPSHAL_VPDMA_DATA_DESC_SIZE * descCnt;

        /* Get the channel number from the descriptor */
        status = Vps_captLmParseDataDesc(&parseDescInfo);

        if (status != FVID2_SOK)
        {
            /* error in parsing skip it */
#ifdef VPS_CAPT_DEBUG_LIST
                Vps_printf ( " %s:%d: Desc %d parsing error !!!\n",
                                __FUNCTION__, __LINE__, descId );
#endif
            gVps_captCommonObj.descParseErrorCount++;
            continue;
        }
        /* get driver inst associated with this descriptor */
        streamId = Vps_captGetStreamId(parseDescInfo.lChannel);
        chId = Vps_captGetChId(parseDescInfo.lChannel);

        /* validate streamId and channelId */
        if (streamId >= pObj->numStream)
        {
            Vps_printf
                ( " %s:%d: ERROR: Received stream Id %d >= Number of streams %d (%d) !!!\n",
                  __FUNCTION__, __LINE__, streamId, pObj->numStream,
                  parseDescInfo.lChannel );

            return -1;
        }
        if (chId >= pObj->numCh)
        {
            Vps_printf
                ( " %s:%d: ERROR: Received stream Id %d >= Number of streams %d (%d) !!!\n",
                  __FUNCTION__, __LINE__, chId, pObj->numCh,
                  parseDescInfo.lChannel );

            return -1;
        }

        /* get associated channel object */
        pChObj = &pObj->chObj[streamId][chId];
        pChObj->vChannelRecv ++;
        if (pChObj->tsUseBestEffort == FALSE)
        {
            GT_assert( GT_DEFAULT_MASK,  pChObj->getCurrTime != NULL);
            pChObj->lastFrameTimestamp = pChObj->getCurrTime(NULL);
        }

        /* If this channel needs more than one descriptor and first
           descriptor is received, it is assumed here that all
           descriptor would have completed by this time, so
           setting flag descMask to make sure that buffer
           address is getting programmed correctly */
        if ((pChObj->vChannelNum > 1u) && (1u == pChObj->vChannelRecv))
        {
            for (descCnt1 = descCnt+1; descCnt1 < numDesc; descCnt1 ++)
            {
                /* Calculate the descriptor pointer */
                parseDescInfo1.descAddr =
                    pObj->listAddr + VPSHAL_VPDMA_DATA_DESC_SIZE * descCnt1;

                /* Get the channel number from the descriptor */
                status = Vps_captLmParseDataDesc(&parseDescInfo1);
                if (parseDescInfo1.lChannel == parseDescInfo.lChannel)
                {
                    if (0 == (pObj->descMask & (1u << descCnt1)))
                    {
                        pChObj->secDescNotWrittenCount ++;
                    }
                    pObj->descMask |= (1u << descCnt1);
                }
            }
        }
        Vps_captProgInitialFrms(pObj, pChObj, &parseDescInfo);
        /* If all the descriptors required for this channel
           are received, reset the total descriptor count to zero */
        if (pChObj->vChannelRecv >= pChObj->vChannelNum)
        {
            pChObj->vChannelRecv = 0u;

            /* Increment frame count and start from 0 if it reaches to 30 as
               frameSkipMask can have only 30 bits */

            pChObj->frameCount = (pChObj->frameCount + 1u) % 30u;
        }
    }

    return (status);
}

Void Vps_captProgInitialFrms(Vps_CaptObj *pObj,
                                  Vps_CaptChObj *pChObj,
                                  Vps_CaptLmDataDescParseInfo *parseDescInfo)
{
    UInt32                          getNewFrame = FALSE, writeDescIdx;
    Vps_CaptLmDataDescUpdateInfo    updateDescInfo;
    Ptr                             descWrAddr;
    UInt32                          fieldToProgram = 0xFFFFFFFF;
    UInt16                          vChCnt;
    Int32                           retVal;
   /* In a frame we have two fields to be progammed,
    * so we get new frame for alternate set of descriptors.
    */
    if ((VPS_CAPT_STATE_0 == pChObj->frameCaptureState) ||
                (VPS_CAPT_STATE_2 == pChObj->frameCaptureState ))
    {
        /* We have to get the new frame for the first descriptor only  for the
         * stream requiring multiple descriptors.
         */
        if (pChObj->vChannelNum > 1u)
        {
            if (1u == pChObj->vChannelRecv)
            {
                getNewFrame = TRUE;
            }
            else
            {
                getNewFrame = FALSE;
            }
        }
        /* For channel, which requires single VPDMA channel, always need
           to dequeue frame from empty queue. */
        else
        {

            getNewFrame = TRUE;
        }
    }
    else
    {
        getNewFrame = FALSE;
    }
    writeDescIdx = pChObj->writeDescIdx[parseDescInfo->vChannelIdx];

    if (TRUE == getNewFrame)
    {
        /* Update the frame pointers, PrevFrame is queued to either
         * output queue or the empty queue based on the fields received
         */
        pChObj->pPrevFrame = pChObj->pNxtFrame;
        retVal = VpsUtils_queGet(&pChObj->emptyQue,
                        (Ptr *)&(pChObj->pNxtFrame),
                        1,
                        BIOS_NO_WAIT);
        /* We didn't get the new frame from input queue, so program the
         * blank frame, For blank frame data will be dropped but still the
         * descriptor will be written at descriptor write addr
         */
        if (FVID2_SOK != retVal)
        {
            pChObj->pNxtFrame = &pChObj->dropFrame[writeDescIdx];
        }
        /* Tmp frame is the one from which the addresses gets programmed to the
         * descriptor based on which we are programming even or odd field
         */
        pChObj->pTmpFrame = pChObj->pNxtFrame;
        if (VPS_CAPT_STATE_0 != pChObj->frameCaptureState)
        {
            /* Increment the writeDescIndex for all the descriptors of same
             * channel.
             */
            for (vChCnt = 0; vChCnt < pChObj->vChannelNum; vChCnt++)
            {
                writeDescIdx = pChObj->writeDescIdx[vChCnt];
                writeDescIdx++;
                if (writeDescIdx >= VPS_CAPT_NUM_WRITE_DATA_DESC_SET)
                {
                    writeDescIdx = 0u;
                }
                pChObj->writeDescIdx[vChCnt] = writeDescIdx;
            }
        }
    }
    descWrAddr = pChObj->writeDescAddr[parseDescInfo->vChannelIdx][writeDescIdx];

    /* pFrame points to next frame output address */
    if(TRUE == getNewFrame)
    {
        /* Store the Descriptor Write Address in the drvData so that it can
           be used at the time of Dequeue */
        pChObj->pTmpFrame->drvData = descWrAddr;
    }
    fieldToProgram = (pChObj->frameCaptureState & VPS_CAPT_STATE_1);
    /* Add Magic Number in the Frame: only for Non-tiled memory */
    if (pChObj->memType != VPSHAL_VPDMA_MT_TILEDMEM ){
        Vps_captTskListAddMagicNum(
            pChObj->pTmpFrame->addr[fieldToProgram][parseDescInfo->vChannelIdx],
            pChObj->pitch[0]);
    }

    /* Update the descriptor information with bufferaddress, descWradr, etc
     * "programmedFid" is updated to keep track of we have programmed descriptor
     * for which fid we have programmed descriptor. This is updated in the
     * "expectedFid" field once we are able to post the descriptor to VPDMA
     */
    updateDescInfo.bufAddr = pChObj->pTmpFrame->addr[fieldToProgram]
                                            [parseDescInfo->vChannelIdx];
    pChObj->programmedFid = fieldToProgram;

    updateDescInfo.enableDescSkip = TRUE;

    updateDescInfo.descWrAddr = (Ptr)((char *)descWrAddr + (fieldToProgram
                                * pChObj->vChannelNum *
                                VPSHAL_VPDMA_DATA_DESC_SIZE));
    *((UInt32 *)updateDescInfo.descWrAddr + 3) = 0xFFFFFFFFu;

    updateDescInfo.memType = pChObj->memType;

    /*
     * mark to output completed descriptor: For Tiled mode, writeback desc is disabled
     * as address needs to be tiled space. Giving non-tiled address cause VIP parser overflow
     */
    if (pChObj->memType == VPSHAL_VPDMA_MT_TILEDMEM)
        updateDescInfo.enableDescWb = FALSE;
    else
        updateDescInfo.enableDescWb = TRUE;
    /* Mark max out width height */
    updateDescInfo.maxOutHeight = pChObj->maxOutHeight;
    updateDescInfo.maxOutWidth  = pChObj->maxOutWidth ;
    updateDescInfo.vChannel = parseDescInfo->vChannel;
    updateDescInfo.descAddr = parseDescInfo->descAddr;
    updateDescInfo.dropData = pChObj->numDropData;
    Vps_captLmUpdateDataDesc(&updateDescInfo);
    if (0 < pChObj->numDropData)
    {
        pChObj->numDropData --;
    }

    if (pChObj->vChannelNum == pChObj->vChannelRecv)
    {
        pChObj->frameCaptureState++;
    }
}

Int32 Vps_captTskSubmitFieldList()
{
    UInt8                          *listAddr = NULL;
    UInt32                          instId, numDesc, cnt = 0u;
    volatile UInt32                 pCnt, streamId, rePostList = 0;
    UInt16                          listSize;
    volatile UInt32                 pid, bit0, bit1;
    Vps_CaptObj                    *pObj = NULL;

    for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId ++)
    {
        /* Get the Capture Object */
        pObj = &gVps_captCommonObj.captureObj[instId];

        /* If capture is not running or starting, there is no need to
           update it */
        if (pObj->state != VPS_CAPT_STATE_RUNNING ||
            (FVID2_BUF_FMT_FRAME == pObj->frameCapture))
        {
            continue;
        }

        numDesc = pObj->listSize / VPSHAL_VPDMA_DATA_DESC_SIZE;

        GT_assert( GT_DEFAULT_MASK, numDesc <= VPSHAL_VPDMA_MAX_SKIP_DESC);

        listAddr = pObj->listAddr;
        pObj->descMask = 0u;
        cnt = 0u;
        do
        {
            rePostList = 0;
            if (numDesc < VPSHAL_VPDMA_MAX_SKIP_DESC)
            {
                listSize = numDesc * VPSHAL_VPDMA_DATA_DESC_SIZE;
                numDesc = 0u;
            }
            else
            {
                listSize =
                    VPSHAL_VPDMA_MAX_SKIP_DESC * VPSHAL_VPDMA_DATA_DESC_SIZE;
                numDesc -= VPSHAL_VPDMA_MAX_SKIP_DESC;
            }

            pid = Vps_captLmPostDescList(listAddr, listSize);
#ifndef PLATFORM_ZEBU
            Task_sleep(1);
#endif

            if (pObj->numCh == 1u)
            {
                pCnt = 0;

                for (streamId = 0; streamId < pObj->numStream; streamId++ )
                {
                    if (pObj->strVChCnt[streamId] > 1)
                    {
                        bit0 = ((pid & (1 << pCnt)) >> pCnt);
                        bit1 = ((pid & (1 << (pCnt + 1))) >> (pCnt + 1));
                        if (bit0 ^ bit1)
                        {
                            rePostList = 1;
                        }
                    }
                    pCnt += pObj->strVChCnt[streamId];
                }

#ifndef VPS_CAPT_DO_REPOST_LIST_FOR_YC_TEAR_ISSUE
                rePostList = FALSE;
#endif
                if (rePostList)
                {
                    pObj->firstDescMissMatch ++;

                    #ifdef VPS_CAPTURE_RT_DEBUG
                    Vps_rprintf(" %d: CAPT_DRV: Desc Miss Match 1 (VIP%d Port%d, count = %d) !!!\n",
                        Clock_getTicks(),
                        pObj->vipInstId,
                        pObj->vipPortId,
                        pObj->firstDescMissMatch
                    );
                    #endif

                    pid |= Vps_captLmPostDescList(listAddr, listSize);
                    Task_sleep(1);

                    pCnt = 0;
                    for (streamId = 0; streamId < pObj->numStream; streamId++ )
                    {
                        if (pObj->strVChCnt[streamId] > 1)
                        {
                            bit0 = ((pid & (1 << pCnt)) >> pCnt);
                            bit1 = ((pid & (1 << (pCnt + 1))) >> (pCnt + 1));
                            if (bit0 ^ bit1)
                            {
                                pObj->secDescMissMatch ++;
                                #ifdef VPS_CAPTURE_RT_DEBUG
                                Vps_rprintf(" %d: CAPT_DRV: Desc Miss Match 2 (VIP%d Port%d, count = %d) !!!\n",
                                    Clock_getTicks(),
                                    pObj->vipInstId,
                                    pObj->vipPortId,
                                    pObj->secDescMissMatch
                                );
                                #endif

#ifdef VPS_CAPT_DO_ABORT_LIST_FOR_YC_TEAR_ISSUE
                                /* When descriptor is not accepted
                                   (either Y or c. if both are not accepted its OK),
                                   we would require to abort in case of TI816x,
                                   failing which, subsequent desriptors would not be
                                   accepted. In case of TI814x, we do not need this
                                   abort descriptor */

                                /* create abrort desc's for all channels and post list */
                                Vps_captPostAbortListPerPort(pObj);
                                Task_sleep(1);

                                /* repost the list after abort */
                                pid |= Vps_captLmPostDescList(listAddr, listSize);
                                Task_sleep(1);
                                if (pid != pObj->allDescMask)
                                {
                                    pObj->thirdDescMissMatch++;

                                    #ifdef VPS_CAPTURE_RT_DEBUG
                                    Vps_rprintf(" %d: CAPT_DRV: Desc Miss Match 3 (VIP%d Port%d, count = %d) !!!\n",
                                        Clock_getTicks(),
                                        pObj->vipInstId,
                                        pObj->vipPortId,
                                        pObj->thirdDescMissMatch
                                    );
                                    #endif
                                }
#endif /* #ifdef VPS_CAPT_DO_ABORT_LIST_FOR_YC_TEAR_ISSUE */
                            }
                        }
                        pCnt += pObj->strVChCnt[streamId];
                    }
                }
            }

            listAddr += listSize;
            pObj->descMask |=
                (pid << (VPSHAL_VPDMA_MAX_SKIP_DESC * cnt));
            cnt ++;

            pObj->listPostCnt ++;
            if (0u == pid)
            {
                pObj->pidZeroCnt ++;
            }
        } while (numDesc > 0u);
    }

    return (FVID2_SOK);
}

Int32 Vps_captTskSubmitFrameList()
{
    UInt8                          *listAddr = NULL;
    UInt32                          instId, numDesc, cnt = 0u;
    UInt16                          listSize, streamId;
    volatile UInt32                 pCnt, rePostList = 0;
    volatile UInt32                 pid, bit0, bit1;
    Vps_CaptObj                    *pObj = NULL;

    for (instId = 0u; instId < VPS_CAPT_INST_MAX; instId ++)
    {
        /* Get the Capture Object */
        pObj = &gVps_captCommonObj.captureObj[instId];

        /* If capture is not running or starting, there is no need to
           update it */
        if ((pObj->state != VPS_CAPT_STATE_RUNNING) ||
            (FVID2_BUF_FMT_FIELD == pObj->frameCapture))
        {
            continue;
        }

        numDesc = pObj->listSize / VPSHAL_VPDMA_DATA_DESC_SIZE;

        GT_assert( GT_DEFAULT_MASK, numDesc <= VPSHAL_VPDMA_MAX_SKIP_DESC);

        listAddr = pObj->listAddr;
        pObj->descMask = 0u;
        cnt = 0u;
        do
        {
            rePostList = 0;
            if (numDesc < VPSHAL_VPDMA_MAX_SKIP_DESC)
            {
                listSize = numDesc * VPSHAL_VPDMA_DATA_DESC_SIZE;
                numDesc = 0u;
            }
            else
            {
                listSize =
                    VPSHAL_VPDMA_MAX_SKIP_DESC * VPSHAL_VPDMA_DATA_DESC_SIZE;
                numDesc -= VPSHAL_VPDMA_MAX_SKIP_DESC;
            }

            pid = Vps_captLmPostDescList(listAddr, listSize);
            Task_sleep(1);

            if (pObj->numCh == 1u)
            {
                pCnt = 0;

                for (streamId = 0; streamId < pObj->numStream; streamId++ )
                {
                    if (pObj->strVChCnt[streamId] > 1)
                    {
                        bit0 = ((pid & (1 << pCnt)) >> pCnt);
                        bit1 = ((pid & (1 << (pCnt + 1))) >> (pCnt + 1));
                        if (bit0 ^ bit1)
                        {
                            rePostList = 1;
                        }
                    }
                    pCnt += pObj->strVChCnt[streamId];
                }

#ifndef VPS_CAPT_DO_REPOST_LIST_FOR_YC_TEAR_ISSUE
                rePostList = FALSE;
#endif
                if (rePostList)
                {
                    pObj->firstDescMissMatch ++;

                    #ifdef VPS_CAPTURE_RT_DEBUG
                    Vps_rprintf(" %d: CAPT_DRV: Desc Miss Match 1 (VIP%d Port%d, count = %d) !!!\n",
                        Clock_getTicks(),
                        pObj->vipInstId,
                        pObj->vipPortId,
                        pObj->firstDescMissMatch
                    );
                    #endif

                    pid |= Vps_captLmPostDescList(listAddr, listSize);
                    Task_sleep(1);

                    pCnt = 0;
                    for (streamId = 0; streamId < pObj->numStream; streamId++ )
                    {
                        if (pObj->strVChCnt[streamId] > 1)
                        {
                            bit0 = ((pid & (1 << pCnt)) >> pCnt);
                            bit1 = ((pid & (1 << (pCnt + 1))) >> (pCnt + 1));
                            if (bit0 ^ bit1)
                            {
                                pObj->secDescMissMatch ++;
                                 #ifdef VPS_CAPTURE_RT_DEBUG
                                Vps_rprintf(" %d: CAPT_DRV: Desc Miss Match 2 (VIP%d Port%d, count = %d) !!!\n",
                                    Clock_getTicks(),
                                    pObj->vipInstId,
                                    pObj->vipPortId,
                                    pObj->secDescMissMatch
                                );
                                #endif

#ifdef VPS_CAPT_DO_ABORT_LIST_FOR_YC_TEAR_ISSUE
                                /* When descriptor is not accepted
                                   (either Y or c. if both are not accepted its OK),
                                   we would require to abort in case of TI816x,
                                   failing which, subsequent desriptors would not be
                                   accepted. In case of TI814x, we do not need this
                                   abort descriptor */

                                /* create abrort desc's for all channels and post list */
                                Vps_captPostAbortListPerPort(pObj);
                                Task_sleep(1);

                                /* repost the list after abort */
                                pid |= Vps_captLmPostDescList(listAddr, listSize);
                                Task_sleep(1);
                                if (pid != pObj->allDescMask)
                                {
                                    pObj->thirdDescMissMatch++;

                                    #ifdef VPS_CAPTURE_RT_DEBUG
                                    Vps_rprintf(" %d: CAPT_DRV: Desc Miss Match 3 (VIP%d Port%d, count = %d) !!!\n",
                                        Clock_getTicks(),
                                        pObj->vipInstId,
                                        pObj->vipPortId,
                                        pObj->thirdDescMissMatch
                                    );
                                    #endif
                                }
#endif /* #ifdef VPS_CAPT_DO_ABORT_LIST_FOR_YC_TEAR_ISSUE */
                            }
                        }
                        pCnt += pObj->strVChCnt[streamId];
                     }
                }
            }

            listAddr += listSize;
            pObj->descMask |=
                (pid << (VPSHAL_VPDMA_MAX_SKIP_DESC * cnt));
            cnt ++;

            pObj->listPostCnt ++;
            if (0u == pid)
            {
                pObj->pidZeroCnt ++;
            }
        } while (numDesc > 0u);
    }

    return (FVID2_SOK);
}

/* Function to get the Completed Frame from the Temp Queue and Put
   it on the full Queue so that it can be Dequeued by the Application. */
Int32 Vps_CaptTskPutCaptField(Vps_CaptObj *pObj,
                                    Vps_CaptChObj *pChObj,
                                    Vps_CaptLmDataDescParseInfo *parseDescInfo)
{
    Int32                status, status1=FVID2_SOK, status2=FVID2_SOK;
    UInt32               curFid;
    FVID2_Frame         *pFrame = NULL;
    Vps_CaptRtParams    *pRtParams = NULL;

    /* Put the completed frame in the output Queue. This
       should be done only for the first descriptor in a
       stream which requires multiple VPDMA channels. For multi-channel
       capture, it will never be more than one. */
    if (1u == pChObj->vChannelRecv)
    {
        /* This will dequeue a frame only if queue has 2 or more
           than 2 frames. */
        VpsUtils_queGet(
            &pChObj->tmpQue,
            (Ptr *)&pFrame,
            2,
            BIOS_NO_WAIT);

        if (NULL != pFrame)
        {
            /* update timestamp for the FVID2 Frame */
            if (pChObj->tsUseBestEffort == FALSE)
            {
                pFrame->timeStamp = pChObj->lastFrameTimestamp;
            }
            /* Otherwise, the ISR would have updated the time stamp */
            /* Update the stats */
            if (pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID)
            {
                Vps_captTimeStampStatsUpdate( pChObj, pFrame->timeStamp );
            }

            /* Get the frame information from the write descriptor */
            if(pFrame->drvData)
            {
                status = Vps_captLmParseDescWrInfo(
                                        pFrame->drvData,
                                        pFrame->addr[0][parseDescInfo->vChannelIdx],
                                        parseDescInfo->vChannel,
                                        &curFid,
                                        &pChObj->lastFrameWidth,
                                        &pChObj->lastFrameHeight);
                pFrame->drvData = NULL;

                if(FVID2_SOK == status)
                {
                    if (FVID2_SF_INTERLACED == pObj->createArgs.inScanFormat)
                    {
                        if(curFid==pChObj->lastFrameFid)
                        {
                            #ifdef VPS_CAPTURE_RT_DEBUG
                            Vps_rprintf(" %d: CAPT_DRV: Fid repeat (cur=%d, prev=%d) (VPDMA CH = %d) !!!\n",
                                Clock_getTicks(),
                                curFid,
                                pChObj->lastFrameFid,
                                parseDescInfo->vChannel
                            );
                            #endif
#ifdef VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE
                            if (pChObj->fidRptIndex < VPS_CAPT_FID_RPT_TIME_TRACK_SIZE)
                            {
                                pChObj->fidRptTimeStamps[pChObj->fidRptIndex].chanNo =
                                    pChObj->lChannelNum;
                                pChObj->fidRptTimeStamps[pChObj->fidRptIndex].fidRptTimeStamp =
                                    Clock_getTicks();
                                pChObj->fidRptIndex++;
                            }
#endif /* VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE */
                            pChObj->fidRepeatCount++;

                            pChObj->printFid = 5;
                        }
                    }

                    /* if desc parse is success then use FID since its valid */
                    pChObj->lastFrameFid = curFid;

                    /* Multiply Last Frame Height for C420 Data */
                    if (VPSHAL_VPDMA_CHANDT_C420 ==
                            pChObj->vChannelDataType[parseDescInfo->vChannelIdx])
                    {
                        if (FVID2_DF_RGB24_888 != pChObj->dataFormat)
                        {
                            pChObj->lastFrameHeight *= 2u;
                        }
                    }

                }
                else
                {
                    pChObj->descNotWrittenCount ++;

    #ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
                    if (pChObj->errTimeStampIndex < VPS_CAPT_ERR_TIME_TRACK_SIZE)
                    {
                        pChObj->errTimeStamps[pChObj->errTimeStampIndex].chanNo =
                            pChObj->lChannelNum;
                        pChObj->errTimeStamps[pChObj->errTimeStampIndex].descNotWrittenTimeStamp =
                            Clock_getTicks();
                        pChObj->errTimeStampIndex++;
                    }
    #endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */

                    pChObj->printFid = 5;

                    if (FVID2_SF_INTERLACED == pObj->createArgs.inScanFormat)
                    {
                        /* if desc parse is not success then toggle FID
                           relative to previously found FID */
                        pChObj->lastFrameFid ^= 1;
                    }
                }
            }
            /* update field ID for the FVID2 Frame */
            pFrame->fid = (UInt32)pChObj->lastFrameFid;

            /* invert FID is applicable only for YUV420SP */
            if(pChObj->invertFid )
                pFrame->fid = !pFrame->fid;

            /* if perFrameCfg is not NULL */
            if (NULL != pFrame->perFrameCfg)
            {
                pRtParams = (Vps_CaptRtParams *)pFrame->perFrameCfg;

                /* udpate width x height in run time per frame config */
                pRtParams->captureOutWidth = pChObj->lastFrameWidth;
                pRtParams->captureOutHeight = pChObj->lastFrameHeight;
            }

            /* write and check magic num for only nontiled memory*/
            if (pChObj->memType != VPSHAL_VPDMA_MT_TILEDMEM )
            {
                status1 = Vps_captTskListCheckMagicNum(
                               pFrame->addr[0][0],
                               pChObj->pitch[0],
                               parseDescInfo->vChannel
                               );
            }

            if(pChObj->vChannelNum>1)
            {
                /* write and check magic num for only nontiled memory*/
                if (pChObj->memType != VPSHAL_VPDMA_MT_TILEDMEM)
                {
                    status2 = Vps_captTskListCheckMagicNum(
                                  pFrame->addr[0][1],
                                  pChObj->pitch[1],
                                  parseDescInfo->vChannel
                                  );
                }
             }

            if (FVID2_SOK != status1)
            {
                pChObj->frameNotWrittenCount ++;
    #ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
                if (pChObj->errTimeStampIndex < VPS_CAPT_ERR_TIME_TRACK_SIZE)
                {
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].chanNo =
                        pChObj->lChannelNum;
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].frameNotWrittenTimeStamp =
                        Clock_getTicks();
                    pChObj->errTimeStampIndex++;
                }
    #endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */
            }
            if (FVID2_SOK != status2)
            {
                pChObj->secFrameNotWrittenCount ++;
    #ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
                if (pChObj->errTimeStampIndex < VPS_CAPT_ERR_TIME_TRACK_SIZE)
                {
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].chanNo =
                        pChObj->lChannelNum;
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].secFrameNotWrittenTimeStamp =
                        Clock_getTicks();
                    pChObj->errTimeStampIndex++;
                }
    #endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */
            }

            #ifdef VPS_CAPTURE_RT_DEBUG
            if(pChObj->printFid)
            {
                pChObj->printFid--;
                Vps_rprintf(" %d: VPDMA CH %d: Fid=%d\n",
                    Clock_getTicks(),
                    parseDescInfo->vChannel,
                    pChObj->lastFrameFid
                    );
            }
            #endif

            if (FVID2_SOK == status1 && FVID2_SOK == status2)
            {
                /* Put the Complete frame in the full Queue. */
                if(pFrame->channelNum!=VPS_CAPT_DROP_FRAME_CH_ID)
                {
                    pChObj->captureFrameCount ++;

                    VpsUtils_quePut(
                        &pObj->fullQue[Vps_captGetStreamId(parseDescInfo->lChannel)],
                        pFrame,
                        BIOS_WAIT_FOREVER);
                }
                else
                {
                    #ifdef VPS_CAPTURE_RT_DEBUG
                    Vps_rprintf(" %d: CAPT_DRV: Frame Drop (VPDMA CH = %d) !!!\n",
                        Clock_getTicks(),
                        parseDescInfo->vChannel
                    );
                    #endif

                    pChObj->droppedFrameCount++;
                }
            }
            else
            {
                if(pFrame->channelNum!=VPS_CAPT_DROP_FRAME_CH_ID)
                {
                    /* Luma part of frame not written, put it back to empty queue */
                    VpsUtils_quePut(&pChObj->emptyQue, pFrame, BIOS_WAIT_FOREVER);
                }
            }

            Vps_captUpdateStatitics(pObj, pChObj, parseDescInfo);
        }

        /* The frame described by pTmpFrame has been accepted, and will
            receive data for the next frame. */
        status = VpsUtils_quePut(
                    &pChObj->tmpQue,
                    pChObj->pTmpFrame,
                    BIOS_NO_WAIT);
        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK);
    }

    return (FVID2_SOK);
}

/* Function to get the Completed Frame from the Temp Queue and Put
   it on the full Queue so that it can be Dequeued by the Application. */
Int32 Vps_CaptTskPutCaptFrm(Vps_CaptObj *pObj,
                                    Vps_CaptChObj *pChObj,
                                    Vps_CaptLmDataDescParseInfo *parseDescInfo)
{
    Int32               status, status1=FVID2_SOK, status2=FVID2_SOK;
    UInt32              descWrAddr, expectedFid, writeDescIdx;
    UInt32              frameCaptured = FALSE;
    UInt16              vChCnt;
    FVID2_Frame         *pFrame = NULL;
    Vps_CaptRtParams    *pRtParams = NULL;
    char*               tempDescWrAdd;
    debugData           *dbgData;


    /* Put the completed frame in the output Queue. This
       should be done only for the first descriptor in a
       stream which requires multiple VPDMA channels. For multi-channel
       capture, it will never be more than one. */
    if (1u == pChObj->vChannelRecv)
    {
        /* Update the debug data, can be used for debugging in future */
        dbgData = &pChObj->dbgData[pChObj->dbgDataIndex];
        dbgData->timeTicks = Clock_getTicks();

        pFrame = pChObj->pPrevFrame;
        if (NULL == pChObj->pPrevFrame)
        {
            pFrame = pChObj->pNxtFrame;
        }
        if (NULL != pFrame)
        {
            /* update timestamp for the FVID2 Frame */
            if (pChObj->tsUseBestEffort == FALSE)
            {
                pFrame->timeStamp = pChObj->lastFrameTimestamp;
            }
            /* Otherwise, the ISR would have updated the frame time stamp */
            /* Update the stats */
            if (pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID)
            {
                Vps_captTimeStampStatsUpdate( pChObj, pFrame->timeStamp );
            }
            pChObj->expectedFid = pChObj->programmedField[pChObj->expectedDescSet];

            /* Get the expected FID for received descriptor */
            expectedFid = pChObj->expectedFid;
            dbgData->expectedFid = expectedFid;
            /* Get the descWrAddr for the frame we are going to received */
            descWrAddr = (UInt32)pFrame->drvData;
            tempDescWrAdd = (char *)descWrAddr;
            /* DescwrAddr will differ based on we are expecting odd or even
             * field, DescWrAddr are arrange in a packed sequence like
             * Desc0_Field0, Desc1_field0, Descn_field0, Desc0_field1,
             * Desc1_field1, Descn_field1, so applying proper equation to get
             * the correct descWrAddr, drvData is always programmed with
             * Desc0_field0
             */
            tempDescWrAdd += (pChObj->expectedFid * pChObj->vChannelNum *
                                VPSHAL_VPDMA_DATA_DESC_SIZE);
            descWrAddr = (UInt32)tempDescWrAdd;

            /* Get the frame information from the write descriptor */
            if(pFrame->drvData)
            {
                /* Parse the descriptor written by VPDMA for the capture field,
                 * Based on result we will be able to determine whether the
                 * FID is received is as expected or inverted then expected.
                 * We take steps accordinly in next section based on
                 * expected and recevied FID
                 */
                status = Vps_captLmParseDescWrInfo(
                        (Ptr)descWrAddr,
                        pFrame->addr[expectedFid][parseDescInfo->vChannelIdx],
                        parseDescInfo->vChannel,
                        &pChObj->curFid,
                        &pChObj->lastFrameWidth,
                        &pChObj->lastFrameHeight);
                if(FVID2_SOK == status)
                {
                    /* Error handling and statistics updation based on received
                     * FID
                     */
                    if ((pChObj->curFid != FVID2_FID_EVEN) &&
                        (pChObj->curFid != FVID2_FID_ODD))
                    {
                        System_printf("Error %d\n", __LINE__);
                    }
                    if (FVID2_SF_INTERLACED == pObj->createArgs.inScanFormat)
                    {
                        if(pChObj->curFid==pChObj->lastFrameFid)
                        {
#ifdef VPS_CAPTURE_RT_DEBUG
                            Vps_rprintf(" %d: CAPT_DRV: Fid repeat (cur=%d, prev=%d) (VPDMA CH = %d) !!!\n",
                                Clock_getTicks(),
                                curFid,
                                pChObj->lastFrameFid,
                                parseDescInfo->vChannel
                            );
#endif
#ifdef VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE
                            if (pChObj->fidRptIndex < VPS_CAPT_FID_RPT_TIME_TRACK_SIZE)
                            {
                                pChObj->fidRptTimeStamps[pChObj->fidRptIndex].chanNo =
                                    pChObj->lChannelNum;
                                pChObj->fidRptTimeStamps[pChObj->fidRptIndex].fidRptTimeStamp =
                                    Clock_getTicks();
                                pChObj->fidRptIndex++;
                            }
#endif /* VPS_CPAT_FID_RPT_TIME_TRACK_ENABLE */
                            pChObj->fidRepeatCount++;

                            pChObj->printFid = 5;
                        }
                    }

                    /* if desc parse is success then use FID since its valid */
                    /* TODO update this at last */
                    pChObj->lastFrameFid = pChObj->curFid;

                    /* Multiply Last Frame Height for C420 Data */
                    if (VPSHAL_VPDMA_CHANDT_C420 ==
                            pChObj->vChannelDataType[parseDescInfo->vChannelIdx])
                    {
                        if (FVID2_DF_RGB24_888 != pChObj->dataFormat)
                        {
                            pChObj->lastFrameHeight *= 2u;
                        }
                    }

                }
                else
                {
                    pChObj->descNotWrittenCount ++;

    #ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
                    if (pChObj->errTimeStampIndex < VPS_CAPT_ERR_TIME_TRACK_SIZE)
                    {
                        pChObj->errTimeStamps[pChObj->errTimeStampIndex].chanNo =
                            pChObj->lChannelNum;
                        pChObj->errTimeStamps[pChObj->errTimeStampIndex].descNotWrittenTimeStamp =
                            Clock_getTicks();
                        pChObj->errTimeStampIndex++;
                    }
    #endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */

                    pChObj->printFid = 5;

                    if (FVID2_SF_INTERLACED == pObj->createArgs.inScanFormat)
                    {
                        pChObj->curFid = (pChObj->lastFrameFid ^ 1) & 0x1;
                    }
                }
            }
            /* update field ID for the FVID2 Frame */
            pFrame->fid = (UInt32)FVID2_FID_TOP;
            if (pChObj->isSubFrameEnabled != TRUE)
            {
                /* TODO Why this is required */
                /* invert FID is applicable only for YUV420SP */
                if(pChObj->invertFid )
                    pFrame->fid = !pFrame->fid;

                /* if perFrameCfg is not NULL */
                if (NULL != pFrame->perFrameCfg)
                {
                    pRtParams = (Vps_CaptRtParams *)pFrame->perFrameCfg;

                    /* udpate width x height in run time per frame config */
                    pRtParams->captureOutWidth = pChObj->lastFrameWidth;
                    pRtParams->captureOutHeight = pChObj->lastFrameHeight;
                }
            }

            /* write and check magic num for only nontiled memory*/
            if (pChObj->memType != VPSHAL_VPDMA_MT_TILEDMEM )
            {
                status1 = Vps_captTskListCheckMagicNum(
                               pFrame->addr[expectedFid][0],
                               pChObj->pitch[0],
                               parseDescInfo->vChannel
                               );
                if ((pChObj->curFid != FVID2_FID_EVEN) &&
                        (pChObj->curFid != FVID2_FID_ODD))
                {
                    System_printf("Error %d\n", __LINE__);
                }
            }

            if(pChObj->vChannelNum>1)
            {
                /* write and check magic num for only nontiled memory*/
                if (pChObj->memType != VPSHAL_VPDMA_MT_TILEDMEM)
                {
                    status2 = Vps_captTskListCheckMagicNum(
                                  pFrame->addr[expectedFid][1],
                                  pChObj->pitch[1],
                                  parseDescInfo->vChannel
                                  );
                    if ((pChObj->curFid != FVID2_FID_EVEN) &&
                        (pChObj->curFid != FVID2_FID_ODD))
                    {
                        System_printf("Error %d\n", __LINE__);
                    }
                }
            }
            /* Update statistics if frame is not written */
            if (FVID2_SOK != status1)
            {
                pChObj->frameNotWrittenCount ++;
    #ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
                if (pChObj->errTimeStampIndex < VPS_CAPT_ERR_TIME_TRACK_SIZE)
                {
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].chanNo =
                        pChObj->lChannelNum;
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].frameNotWrittenTimeStamp =
                        Clock_getTicks();
                    pChObj->errTimeStampIndex++;
                }
    #endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */
            }
            if (FVID2_SOK != status2)
            {
                pChObj->secFrameNotWrittenCount ++;
    #ifdef VPS_CPAT_ERR_TIME_TRACK_ENABLE
                if (pChObj->errTimeStampIndex < VPS_CAPT_ERR_TIME_TRACK_SIZE)
                {
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].chanNo =
                        pChObj->lChannelNum;
                    pChObj->errTimeStamps[pChObj->errTimeStampIndex].secFrameNotWrittenTimeStamp =
                        Clock_getTicks();
                    pChObj->errTimeStampIndex++;
                }
    #endif /* VPS_CPAT_ERR_TIME_TRACK_ENABLE */
            }

            #ifdef VPS_CAPTURE_RT_DEBUG
            if(pChObj->printFid)
            {
                pChObj->printFid--;
                Vps_rprintf(" %d: VPDMA CH %d: Fid=%d\n",
                    Clock_getTicks(),
                    parseDescInfo->vChannel,
                    pChObj->lastFrameFid
                    );
            }
            #endif
           /* This condition checks, actually field is written to memory or not.
            * because of hardware bug, sometimes VPDMA doesn't write field to
            * memory, although it tells it has written.
            */
            if (FVID2_SOK == status1 && FVID2_SOK == status2)
            {
                /* If received FID is same as expected FID */
                if (pChObj->curFid == pChObj->expectedFid)
                {
                    /* If we have received the ODD Field */
                    if (pChObj->curFid == FVID2_FID_ODD)
                    {
                        /* If even field is received previously and frame is not
                         * drop frame then queue the frame to the output queue
                         * for the application to receive it.
                         */
                        if ((NULL != pChObj->pPrevFrame) &&
                            (TRUE == pChObj->evenFieldCaptured) &&
                             (pChObj->pPrevFrame->channelNum !=
                                VPS_CAPT_DROP_FRAME_CH_ID))
                        {
                            frameCaptured = TRUE;
                            VpsUtils_quePut(
                                &pObj->fullQue[Vps_captGetStreamId(parseDescInfo->lChannel)],
                                pChObj->pPrevFrame,
                                BIOS_WAIT_FOREVER);
                            pChObj->pPrevFrame = NULL;
                        }
                        /* If even field is not received then put the
                         * frame back to empty queue,
                         */
                        else if ((NULL != pChObj->pPrevFrame) &&
                            (FALSE == pChObj->evenFieldCaptured) &&
                             (pChObj->pPrevFrame->channelNum !=
                                VPS_CAPT_DROP_FRAME_CH_ID))
                        {
                            /* If application does not want error frame,
                             * queue back error buffer
                             */
                            if (FALSE == pObj->returnErrFrm)
                            {
                                 VpsUtils_quePut(
                                        &pChObj->emptyQue,
                                        pChObj->pPrevFrame,
                                        BIOS_WAIT_FOREVER);
                            }
                            /* If application want error frame give back error
                             * frame and mark it as ERROR
                             */
                            else
                            {
                                VpsUtils_quePut(
                                    &pObj->fullQue[Vps_captGetStreamId(parseDescInfo->lChannel)],
                                    pChObj->pPrevFrame,
                                    BIOS_WAIT_FOREVER);
                            }
                            pChObj->pPrevFrame = NULL;
                        }
                        pChObj->evenFieldCaptured = FALSE;
                    }
                    /* If even field is received */
                    if (pChObj->curFid == FVID2_FID_EVEN)
                    {
                        /* Mark that even field is received */
                        frameCaptured = TRUE;
                        pChObj->evenFieldCaptured = TRUE;
                    }
                }
                /* If received FID doesn't match expected FID */
                if (pChObj->curFid != pChObj->expectedFid)
                {
                    /* we were expecting the ODD FID, that means we have
                     * programmed descriptor for even also, But since
                     * atleast we didnt get the ODD FID as expected, queue
                     * the frame back to empty queue
                     */
                    if ((NULL != pChObj->pPrevFrame) &&
                        (pChObj->pPrevFrame->channelNum) !=
                            (VPS_CAPT_DROP_FRAME_CH_ID) &&
                            (FVID2_FID_ODD == pChObj->expectedFid))
                    {
                        /* If application does not want error frame,
                         * queue back error buffer
                         */
                        if (FALSE == pObj->returnErrFrm)
                        {
                            VpsUtils_quePut(
                                    &pChObj->emptyQue,
                                    pChObj->pPrevFrame,
                                    BIOS_WAIT_FOREVER);
                        }
                        else
                        {
                            /* If application want error frame give back error
                             * frame and mark it as ERROR
                             */
                            VpsUtils_quePut(
                                &pObj->fullQue[Vps_captGetStreamId(parseDescInfo->lChannel)],
                                pChObj->pPrevFrame,
                                BIOS_WAIT_FOREVER);
                        }
                        pChObj->pPrevFrame = NULL;
                    }
                    /* We were expecting EVEN FID but received ODD, so we have
                     * already programmed for ODD, for this frame we flag that
                     * EVEN field is not received, so even if we receive EVEN
                     * ODD field next, we have to queue back frame to
                     * empty queue
                     */
                    else  if (FVID2_FID_EVEN == pChObj->expectedFid)
                    {
                        pChObj->evenFieldCaptured = FALSE;
                    }
                }
                /* Increment the framecaptued count irrespective of whether
                 * FID is received as expected or not. Actually
                 * captureFrameCount is a captureFieldCount for interlaced
                 * capture
                 */
                if(pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID &&
                    (TRUE == frameCaptured))
                {
                    pChObj->captureFrameCount ++;
                }
                /* If we have programed the drop frame the increment the
                 * drop frame count
                 */
                else if (pFrame->channelNum == VPS_CAPT_DROP_FRAME_CH_ID)
                {
                    #ifdef VPS_CAPTURE_RT_DEBUG
                    Vps_rprintf(" %d: CAPT_DRV: Frame Drop (VPDMA CH = %d) !!!\n",
                        Clock_getTicks(),
                        parseDescInfo->vChannel
                    );
                    #endif

                    pChObj->droppedFrameCount++;
                }
                /* Update the frame pointers to get the new frame from the
                 * input queue once we have programmed the descriptor for the
                 * ODD field, since now we have to program the descriptor for
                 * even field from next frame.
                 */
                if (FVID2_FID_ODD == pChObj->programmedFid)
                {
                    /* We never queued back the frame since we never had our
                     * expected FID as ODD because of connect disconnect.
                     * but since we have to get new frame. queue back old
                     * frame
                     */
                    if ((NULL != pChObj->pPrevFrame) &&
                        (pChObj->pPrevFrame->channelNum) !=
                            (VPS_CAPT_DROP_FRAME_CH_ID))
                    {
                        /* If application does not want error frame,
                         * queue back error buffer
                         */
                        if (FALSE == pObj->returnErrFrm)
                        {
                            VpsUtils_quePut(&pChObj->emptyQue, pChObj->pPrevFrame,
                                BIOS_WAIT_FOREVER);

                        }
                        else
                        {
                            /* If application want error frame give back error
                             * frame and mark it as ERROR
                             */
                            VpsUtils_quePut(
                                &pObj->fullQue[Vps_captGetStreamId(parseDescInfo->lChannel)],
                                pChObj->pPrevFrame,
                                BIOS_WAIT_FOREVER);
                        }
                        pChObj->pPrevFrame = NULL;
                    }
                    writeDescIdx = pChObj->writeDescIdx[parseDescInfo->vChannelIdx];
                    pChObj->pPrevFrame = pChObj->pNxtFrame;
                    dbgData->prevFrmAddr = pChObj->pPrevFrame;
                    /* Once you take next frame, we have to increment the
                     * writedesc index for all descriptors of same channel
                     */
                    for (vChCnt = 0; vChCnt < pChObj->vChannelNum; vChCnt++)
                    {
                        writeDescIdx = pChObj->writeDescIdx[vChCnt];
                        writeDescIdx++;
                        if (writeDescIdx >= VPS_CAPT_NUM_WRITE_DATA_DESC_SET)
                        {
                            writeDescIdx = 0u;
                        }
                        pChObj->writeDescIdx[vChCnt] = writeDescIdx;
                    }
                    /* Get new frame from input queue */
                    status = VpsUtils_queGet(&pChObj->emptyQue,
                                    (Ptr *)&(pChObj->pNxtFrame),
                                    1,
                                    BIOS_NO_WAIT);
                    /* If we dont get new frame, get the drop frame*/
                    if (FVID2_SOK != status)
                    {
                        pChObj->pNxtFrame = &pChObj->dropFrame[writeDescIdx];
                    }
                    dbgData->nextFrmAddr = pChObj->pNxtFrame;
                }

            }
            /* If field is not captured, frame is not written etc. This is
             * because of the hardware bugs where data doesnt get written
             */
            else
            {
                /* If we were expecting ODD field then queue back the
                 * received frame back to driver input queue
                 */
                if((pFrame->channelNum!=VPS_CAPT_DROP_FRAME_CH_ID) &&
                        (NULL != pChObj->pPrevFrame) &&
                        (FVID2_FID_ODD == pChObj->expectedFid))
                {
                    /* Luma part of frame not written, put it back to empty queue */
                    VpsUtils_quePut(
                            &pChObj->emptyQue,
                            pChObj->pPrevFrame,
                            BIOS_WAIT_FOREVER);
                    pChObj->pPrevFrame = NULL;
                }
                /* If we were expecting the EVEN field then marked field as
                 * not captured
                 */
                if((pFrame->channelNum!=VPS_CAPT_DROP_FRAME_CH_ID) &&
                           (FVID2_FID_EVEN == pChObj->expectedFid))
                {
                    pChObj->evenFieldCaptured = FALSE;
                }
            }

            /* Update statistics after all operations */
            Vps_captUpdateStatitics(pObj, pChObj, parseDescInfo);
        }
        /* TODO what is this. */
#if 0
        /* if pFrame->channelNum is VPS_CAPT_DROP_FRAME_CH_ID, that means frame is being skipped */
        status = VpsUtils_quePut(
                    &pChObj->tmpQue,
                    pChObj->pTmpFrame,
                    BIOS_NO_WAIT);
        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK);
#endif
    }

    return (FVID2_SOK);
}



Int32 Vps_CaptTskUpdateField(Vps_CaptObj *pObj,
                                  Vps_CaptChObj *pChObj,
                                  Vps_CaptLmDataDescParseInfo *parseDescInfo)
{
    UInt32                          getNewFrame = FALSE, writeDescIdx;
    Vps_CaptLmDataDescUpdateInfo    updateDescInfo;
    Ptr                             descWrAddr;
    Vps_CaptSubFrameQObj            *subFrameObj;

    /* For the channel which requires multiple descriptor, need to get
       frame for the first VPDMA channel only. For all other
       VPDMA channels, use the same FVID2_Frame */
    if (pChObj->vChannelNum > 1u)
    {
        if (1u == pChObj->vChannelRecv)
        {
            /* Get a free frame from empty queue, so
               initialize tmpFrame to NULL*/
            getNewFrame = TRUE;
        }
        else
        {
            getNewFrame = FALSE;
        }
    }
    else
    {
        /* For channel, which requires single VPDMA channel, always need
           to dequeue frame from empty queue. */
        getNewFrame = TRUE;
    }

    writeDescIdx = pChObj->writeDescIdx[parseDescInfo->vChannelIdx];

    if (((pChObj->frameSkipMask >> pChObj->frameCount) & 0x1) == 0u)
    {
        /* Get a frame from Empty Queue */
        if (TRUE == getNewFrame)
        {
            /* Get a new frame from empty queue, so
               initialize tmpFrame to NULL */
            pChObj->pTmpFrame = NULL;

            VpsUtils_queGet(&pChObj->emptyQue,
                            (Ptr *)&(pChObj->pTmpFrame),
                            1,
                            BIOS_NO_WAIT);

            if (pChObj->pTmpFrame == NULL)
            {
                /* drop data */
                pChObj->pTmpFrame = &pChObj->dropFrame[writeDescIdx];

            }
        }
    }
    else
    {
        /* drop data */
        pChObj->pTmpFrame = &pChObj->dropFrame[writeDescIdx];
    }

    descWrAddr = pChObj->writeDescAddr[parseDescInfo->vChannelIdx][writeDescIdx];
    writeDescIdx ++;
    if (writeDescIdx >= VPS_CAPT_NUM_WRITE_DATA_DESC_SET)
    {
        writeDescIdx = 0u;
    }
    pChObj->writeDescIdx[parseDescInfo->vChannelIdx] = writeDescIdx;

    /* pFrame points to next frame output address */
    if(TRUE == getNewFrame)
    {
        /* Store the Descriptor Write Address in the drvData so that it can
           be used at the time of Dequeue */
        pChObj->pTmpFrame->drvData = descWrAddr;
    }
    /* Add Magic Number in the Frame: only for Non-tiled memory */
    if (pChObj->memType != VPSHAL_VPDMA_MT_TILEDMEM ){
        Vps_captTskListAddMagicNum(
            pChObj->pTmpFrame->addr[0][parseDescInfo->vChannelIdx],
            pChObj->pitch[0]);
    }

    /* if pFrame is not NULL,
     * then update recevied descriptor address with
     * corresponding next frame address for the same plane
     */
    updateDescInfo.bufAddr = pChObj->pTmpFrame->addr[0][parseDescInfo->vChannelIdx];

    updateDescInfo.enableDescSkip = TRUE;

    updateDescInfo.descWrAddr = descWrAddr;
    /* Resetting Value at the 3rd word of data descriptor */
    *((UInt32 *)descWrAddr + 3) = 0xFFFFFFFFu;

    updateDescInfo.memType = pChObj->memType;

    /*
     * mark to output completed descriptor: For Tiled mode, writeback desc is disabled
     * as address needs to be tiled space. Giving non-tiled address cause VIP parser overflow
     */
    if (pChObj->memType == VPSHAL_VPDMA_MT_TILEDMEM)
        updateDescInfo.enableDescWb = FALSE;
    else
        updateDescInfo.enableDescWb = TRUE;

    /* Mark max out width height */
    updateDescInfo.maxOutHeight = pChObj->maxOutHeight;
    updateDescInfo.maxOutWidth  = pChObj->maxOutWidth ;
    updateDescInfo.vChannel = parseDescInfo->vChannel;
    updateDescInfo.descAddr = parseDescInfo->descAddr;
    updateDescInfo.dropData = pChObj->numDropData;

    /* In case sub frame based processing is enabled, update the subFrameQ
       with new frame being programmed */
    if ( (TRUE == getNewFrame) && (TRUE == pChObj->isSubFrameEnabled) )
    {
        subFrameObj = Vps_captVipSubFrameAddQObj ( pChObj, pChObj->pTmpFrame );
        GT_assert( GT_DEFAULT_MASK,  ( subFrameObj != NULL ));
    }

    if ( (TRUE == getNewFrame) && (pChObj->tsUseBestEffort == TRUE) )
    {
        if (Vps_captTimeStampAddElem( pChObj, pChObj->pTmpFrame) != FVID2_SOK)
        {
            GT_assert( GT_DEFAULT_MASK, FALSE);
        }
    }

    Vps_captLmUpdateDataDesc(&updateDescInfo);
    if (0u < pChObj->numDropData)
    {
        pChObj->numDropData --;
    }

    return (FVID2_SOK);
}

Int32 Vps_CaptTskUpdateFrm(Vps_CaptObj *pObj,
                                  Vps_CaptChObj *pChObj,
                                  Vps_CaptLmDataDescParseInfo *parseDescInfo)
{
    UInt32                          writeDescIdx, previousExpectedDescSet;
    Vps_CaptLmDataDescUpdateInfo    updateDescInfo;
    Ptr                             descWrAddr;
    debugData                       *dbgData;

    writeDescIdx = pChObj->writeDescIdx[parseDescInfo->vChannelIdx];
    descWrAddr = pChObj->writeDescAddr[parseDescInfo->vChannelIdx][writeDescIdx];

    dbgData = &pChObj->dbgData[pChObj->dbgDataIndex];

    /* if Current FID is EVEN program for ODD field, if currentFID is odd
     * program for ODD */
    if (1 == pChObj->vChannelRecv)
    {
        pChObj->programmedField[pChObj->expectedDescSet] = pChObj->programmedFid;
    }
    //dbgData->programmedField = pChObj->curFid ^ 1;
    pChObj->pTmpFrame = pChObj->pNxtFrame;
    if (FVID2_FID_ODD == pChObj->curFid)
    {
        pChObj->programmedFid = FVID2_FID_EVEN;
    }
    if (FVID2_FID_EVEN == pChObj->curFid)
    {
        pChObj->programmedFid = FVID2_FID_ODD;
    }
    /* For each successful post of list invert the expected descriptor set
     */
    if (pChObj->vChannelNum == pChObj->vChannelRecv)
    {
        previousExpectedDescSet = pChObj->expectedDescSet;
        pChObj->expectedDescSet ^= 1;
        if (pChObj->expectedDescSet == previousExpectedDescSet)
        {
            GT_assert(GT_DEFAULT_MASK,  FALSE);
        }
        dbgData->expectedDescSet = pChObj->expectedDescSet;

    }
    /* Get the writedesc addr */
    descWrAddr = pChObj->writeDescAddr[parseDescInfo->vChannelIdx][writeDescIdx];

    /* Set drvdata to descaddr for the first descriptor of the same channel */
    if (1 == pChObj->vChannelRecv)
    {
        /* if pFrame is not NULL,
         * then update recevied descriptor address with
         * corresponding next frame address for the same plane
         */
        pChObj->pTmpFrame->drvData = descWrAddr;
    }

    /* Add Magic Number in the Frame: only for Non-tiled memory */
    if (pChObj->memType != VPSHAL_VPDMA_MT_TILEDMEM ){
        Vps_captTskListAddMagicNum(
            pChObj->pTmpFrame->addr[pChObj->programmedFid][parseDescInfo->vChannelIdx],
            pChObj->pitch[0]);
    }

    /* Update the buffer address in descriptor */
    updateDescInfo.bufAddr = pChObj->pTmpFrame->addr[pChObj->programmedFid]
                                [parseDescInfo->vChannelIdx];

    updateDescInfo.enableDescSkip = TRUE;
    /* Update the descriptor write address based on FID and vpdma channel
     * index
     */
    updateDescInfo.descWrAddr = (Ptr)((UInt32)descWrAddr +
        ( pChObj->programmedFid * pChObj->vChannelNum * VPSHAL_VPDMA_DATA_DESC_SIZE));
    dbgData->descWrAddr = (UInt32)updateDescInfo.descWrAddr;
    /* Resetting Value at the 3rd word of data descriptor */
    *((UInt32 *)descWrAddr + 3) = 0xFFFFFFFFu;

    updateDescInfo.memType = pChObj->memType;

    /*
     * mark to output completed descriptor: For Tiled mode, writeback desc is disabled
     * as address needs to be tiled space. Giving non-tiled address cause VIP parser overflow
     */
    if (pChObj->memType == VPSHAL_VPDMA_MT_TILEDMEM)
        updateDescInfo.enableDescWb = FALSE;
    else
        updateDescInfo.enableDescWb = TRUE;

    /* Mark max out width height */
    updateDescInfo.maxOutHeight = pChObj->maxOutHeight;
    updateDescInfo.maxOutWidth  = pChObj->maxOutWidth ;
    updateDescInfo.vChannel = parseDescInfo->vChannel;
    updateDescInfo.descAddr = parseDescInfo->descAddr;
    updateDescInfo.dropData = pChObj->numDropData;
    Vps_captLmUpdateDataDesc(&updateDescInfo);
    if (0 < pChObj->numDropData)
    {
        pChObj->numDropData --;
    }
    if (pChObj->vChannelNum == pChObj->vChannelRecv)
    {
        pChObj->dbgDataIndex++;
        if (pChObj->dbgDataIndex >= 200)
        {
            pChObj->dbgDataIndex = 0;
        }
    }
    return (FVID2_SOK);
}

Int32 Vps_captMakeList(Vps_CaptObj *pObj)
{
    UInt16 chId, streamId, vChId;
    UInt32 descCnt = 0, descWrAddOffset = 0;

#ifdef VPS_CAPT_DEBUG_LIST
        Vps_printf ( " %s:%d: Adding descriptors for instance %d !!!\n",
                        __FUNCTION__, __LINE__, pObj->instanceId );
#endif

    /* Get the list address */
    pObj->listAddr = Vps_captGetListAddr(pObj);
    pObj->listSize = 0u;

    /* Add scaler coeff programming desc, if needed */
    if (Vcore_vipIsScAlloc(&pObj->resObj))
    {
        /* Create the Config Descriptor and post a new list
           to configure scalar coefficient */
        Vps_captLmAddScCoeffConfigDesc(
            pObj->vipInstId,
            pObj->listAddr,
            Vps_captGetScCoeffMem(pObj->vipInstId));
    }

    if (Vps_captIsDisSyncStartSequenceNeeded(pObj))
    {
        Vps_captDoDisSyncStartSequence(pObj);
    }

    if (Vps_captSubFrameIsEnabled(pObj) == TRUE)
    {
        Vps_captSubFrameEnableInt(pObj);
    }

    pObj->allDescMask = 0;
    descWrAddOffset = (UInt32)(pObj->listAddr + VPS_CAPT_NUM_DATA_DESC_SIZE);
    for (chId = 0; chId < pObj->numCh; chId++ )
    {
        for (streamId = 0; streamId < pObj->numStream; streamId++ )
        {
            pObj->strVChCnt[streamId] = 0;
            if ((FVID2_SF_INTERLACED == pObj->createArgs.inScanFormat) &&
                    (FVID2_BUF_FMT_FRAME ==
                        pObj->chObj[streamId][chId].frameCapture))
            {
                /* add descriptors associated with this channel
                   to current list */
                Vps_captAddFrmToList(pObj, &pObj->chObj[streamId][chId], descWrAddOffset);
                descWrAddOffset += pObj->chObj[streamId][chId].vChannelNum *
                        VPSHAL_VPDMA_DATA_DESC_SIZE * 2;
                pObj->chObj[streamId][chId].frameCaptureState = VPS_CAPT_STATE_0;

            }
            else
            {
                /* add descriptors associated with this channel
                   to current list */
                Vps_captAddToList(pObj, &pObj->chObj[streamId][chId]);
                descWrAddOffset += pObj->chObj[streamId][chId].vChannelNum *
                        VPSHAL_VPDMA_DATA_DESC_SIZE;

            }

            for (vChId = 0; vChId < pObj->chObj[streamId][chId].vChannelNum; vChId ++)
            {
                pObj->strVChCnt[streamId] ++;
                pObj->allDescMask |= (1 << descCnt);
                descCnt ++;
            }
        }
    }

    /* If the list size is zero, numStreams and numCh are zero. */
    GT_assert( GT_DEFAULT_MASK, 0u != pObj->listSize);

    return (FVID2_SOK);
}

Bool Vps_captIsDisSyncStartSequenceNeeded(Vps_CaptObj *pObj)
{
    Bool isNeeded = FALSE;
    Int32 streamId;
    UInt32 outDataFormat;

    if (pObj->cpuRev > VPS_PLATFORM_CPU_REV_1_1)
    {
        isNeeded = FALSE;
    }
    else if (Vps_captIsSingleChDiscSyncMode(pObj->createArgs.videoCaptureMode))
    {
        if (FVID2_DF_RGB24_888 == pObj->createArgs.inDataFormat)
        {
            for(streamId=0; streamId<pObj->createArgs.numStream; streamId ++)
            {
                outDataFormat = pObj->createArgs.outStreamInfo[streamId].dataFormat;
                if((outDataFormat==FVID2_DF_YUV422I_YUYV) ||
                   (outDataFormat==FVID2_DF_YUV420SP_UV) ||
                   (outDataFormat==FVID2_DF_YUV422SP_UV))
                {
                    isNeeded = TRUE;
                }
            }
        }
        else
        {
            isNeeded = TRUE;
        }
    }

    return (isNeeded);
}

/*
    This sequence is applicable only when
    - VIP0 Port A AND
    - 24-bit mode AND
    - discrete sync mode AND
    - RGB input format

    MAKE SURE NOT DATA IS FOLLOWING THRU THIS VIP
    OTHER THAN THIS CAPTURE during this startup sequence

    The following happens in the startup seqeuence

    - Add the following to the list
      - Data desc for PORTB_RGB
        with drop data = 1, write desc = 0
        i.e do not write data to DDR and do not write
        descriptor to DDR
      - Sync On Client
        with VPDMA CH = PORTB_RGB
             event = EOF
      - Config Desc which configs VIP MUX
        according to actual needed setup
    - Program VIP MUX such that data goes directly to VPDMA
    - enable the VIP port so that data is flowing
      before submitting the list
*/
Int32 Vps_captDoDisSyncStartSequence(Vps_CaptObj *pObj)
{
    UInt8 *memPtr, *ovlyPtr;
    UInt32 vChId;
    VpsHal_VpsVipInst vpsVipInst;
    VpsHal_VipInst vipInst;
    VpsHal_VipPort vipPort;
    VpsHal_VpdmaChannel vChannel;
    Vps_CaptChObj *pChObj;

    memPtr = pObj->listAddr;

    if ((VPS_CAPT_INST_VIP0_PORTA == pObj->instanceId) ||
        (VPS_CAPT_INST_VIP0_PORTB == pObj->instanceId))
    {
        vpsVipInst = VPSHAL_VPS_VIP_INST_VIP0;
        vipInst = VPSHAL_VIP_INST_0;
    }
    else
    {
        vpsVipInst = VPSHAL_VPS_VIP_INST_VIP1;
        vipInst = VPSHAL_VIP_INST_1;
    }

    if (VPSHAL_VPS_VIP_INST_VIP0 == vpsVipInst)
    {
        if (VPS_CAPT_VIDEO_IF_MODE_24BIT == pObj->createArgs.videoIfMode)
        {
            vChannel = VPSHAL_VPDMA_CHANNEL_VIP0_PORTB_RGB;
            vipPort = VPSHAL_VIP_PORT_A;
        }
        else
        {
            if (VPS_CAPT_INST_VIP0_PORTA == pObj->instanceId)
            {
                vChannel = VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTA_SRC0;
                vipPort = VPSHAL_VIP_PORT_A;
            }
            else
            {
                vChannel = VPSHAL_VPDMA_CHANNEL_VIP0_MULT_PORTB_SRC0;
                vipPort = VPSHAL_VIP_PORT_B;
            }
        }
    }
    else
    {
        if (VPS_CAPT_VIDEO_IF_MODE_24BIT == pObj->createArgs.videoIfMode)
        {
            vChannel = VPSHAL_VPDMA_CHANNEL_VIP1_PORTB_RGB;
            vipPort = VPSHAL_VIP_PORT_A;
        }
        else
        {
            if (VPS_CAPT_INST_VIP1_PORTA == pObj->instanceId)
            {
                vChannel = VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTA_SRC0;
                vipPort = VPSHAL_VIP_PORT_A;
            }
            else
            {
                vChannel = VPSHAL_VPDMA_CHANNEL_VIP1_MULT_PORTB_SRC0;
                vipPort = VPSHAL_VIP_PORT_B;
            }
        }
    }

    /* Reset allocated muxes */
    Vcore_vipRestoreMux(&pObj->resObj);

    ovlyPtr = gVps_captVipConfigMem;

    /* RGB Input format */
    VpsHal_vpsVipCreateConfigOvly(vpsVipInst, ovlyPtr);
    VpsHal_vpsVipMultiChEnable(vpsVipInst, TRUE, ovlyPtr);
    ovlyPtr += VpsHal_vpsVipGetConfigOvlySize(vpsVipInst);

    /* Create overlay to connect input to directly VPDMA client */
    if (VPS_CAPT_VIDEO_IF_MODE_24BIT == pObj->createArgs.videoIfMode)
    {
        VpsHal_vpsVipCreateConfigOvly(vpsVipInst, ovlyPtr);
        VpsHal_vpsVipRgbLowEnable(vpsVipInst, TRUE, ovlyPtr);
        ovlyPtr += VpsHal_vpsVipGetConfigOvlySize(vpsVipInst);
    }
    else
    {
        VpsHal_vpsVipCreateConfigOvly(vpsVipInst, ovlyPtr);
        VpsHal_vpsVipRgbLowEnable(vpsVipInst, FALSE, ovlyPtr);
        ovlyPtr += VpsHal_vpsVipGetConfigOvlySize(vpsVipInst);
    }

    /* enable VIP port */
    VpsHal_vipInstPortEnableCreateOvly(vipInst,
        vipPort,
        (UInt32 *)ovlyPtr);
    VpsHal_vipInstPortEnable(vipInst, vipPort, TRUE, (UInt32 *)ovlyPtr);
    ovlyPtr += VpsHal_vipInstPortEnableGetCfgOvlySize(vipInst, vipPort);

    VpsHal_vpdmaCreateConfigDesc(
        memPtr,
        VPSHAL_VPDMA_CONFIG_DEST_MMR,
        VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
        VPSHAL_VPDMA_CCT_INDIRECT,
        (UInt32)ovlyPtr - (UInt32)gVps_captVipConfigMem,
        gVps_captVipConfigMem,
        NULL,
        0);

    /* Increment memory pointer */
    memPtr = memPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;

    /*  add data desc for PORTB_RGB channel drop the data and
        do not writeback desc to memory */
    Vps_captLmAddDummyDisSyncDesc(memPtr, vChannel);

    /* Increment memory pointer */
    memPtr = memPtr + VPSHAL_VPDMA_DATA_DESC_SIZE;

    /* add SOC with event as EOF */
    VpsHal_vpdmaCreateSOCCtrlDesc(
        memPtr,
        vChannel,
        VPSHAL_VPDMA_SOC_EOF,
        0,
        0);

    /* Increment memory pointer */
    memPtr = memPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;

    /* Create new overlay */
    Vcore_vipCreateConfigOvly(&pObj->resObj, ovlyPtr);
    Vcore_vipSetMux(&pObj->resObj, ovlyPtr);

    /* add Config desc to program the VIP mux */
    VpsHal_vpdmaCreateConfigDesc(
        memPtr,
        VPSHAL_VPDMA_CONFIG_DEST_MMR,
        VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
        VPSHAL_VPDMA_CCT_INDIRECT,
        Vcore_vipGetConfigOvlySize(&pObj->resObj),
        (UInt8*)ovlyPtr,
        NULL,
        0);

    /* Increment memory pointer */
    memPtr = memPtr + VPSHAL_VPDMA_CONFIG_DESC_SIZE;

    /* Add Data Descriptor for the Actual Channel */
    pChObj = &pObj->chObj[0u][0u];
    for (vChId = 0; vChId < pChObj->vChannelNum; vChId ++)
    {
        Vps_captLmAddDummyDisSyncDesc(memPtr, pChObj->vChannelId[vChId]);
        memPtr = memPtr + VPSHAL_VPDMA_DATA_DESC_SIZE;
    }

    /* Post the List */
    Vps_captLmPostList(
        pObj->listAddr,
        ((UInt32)memPtr) - ((UInt32)pObj->listAddr));

    return FVID2_SOK;
}


Void Vps_captTskListCheckDesc(UInt8 *addr, UInt32 pitch, UInt32 lineNum, UInt32 channelNum)
{
    volatile VpsHal_VpdmaInDataDesc *pDataDesc;
    UInt32 descSize;

    pDataDesc = (VpsHal_VpdmaInDataDesc*)(addr + pitch*lineNum);

    descSize = sizeof(*pDataDesc);

    /* Invalidate Cache */
    #ifdef VPS_CAPT_DO_CACHE_OPS
    Cache_inv((xdc_Ptr)pDataDesc, descSize, Cache_Type_ALL, TRUE);
    #endif

    if (VPSHAL_VPDMA_PT_DATA == pDataDesc->descType)
    {
        gVps_captCommonObj.descMissFound++;
        #ifdef VPS_CAPTURE_RT_DEBUG
        Vps_rprintf(" %d: CAPT_DRV: Found a desc in frame buffer @ line %d (VPDMA CH = %d, count = %d) !!!\n",
            Clock_getTicks(),
            lineNum,
            channelNum,
            gVps_captCommonObj.descMissFound
        );
        #endif
        #if 0
        VpsHal_vpdmaPrintInBoundDesc((const void*)pDataDesc, GT_DEFAULT_MASK | GT_INFO);
        #endif

        memset((void*)pDataDesc, 0, descSize);

        #ifdef VPS_CAPT_DO_CACHE_OPS
        Cache_wb((xdc_Ptr)pDataDesc, descSize, Cache_Type_ALL, TRUE);
        #endif
    }
}

#define VPS_CAPT_MAGIC_NUM      (0xDEADBEEFu)
#define VPS_CAPT_MAGICNUM_LINE  (0)

/* Function to check whether magic number is present in the frame or not */
Int32 Vps_captTskListCheckMagicNum(UInt8 *addr, UInt32 pitch, UInt32 channelNum)
{
#ifdef VPS_CAPT_DO_FRAME_TAGGING_FOR_FRAME_DROP_ISSUE
    Int32 status = FVID2_SOK;

    volatile UInt32 *pAddr;

    if(addr)
    {
        #if 0 /* Enable to check for desc in video data area */
        Vps_captTskListCheckDesc(addr, pitch, 243,channelNum);
        Vps_captTskListCheckDesc(addr, pitch, 244,channelNum);
        Vps_captTskListCheckDesc(addr, pitch, 288,channelNum);
        #endif

        pAddr = (UInt32*)(addr + pitch*VPS_CAPT_MAGICNUM_LINE);

        #ifdef VPS_CAPT_DO_CACHE_OPS
        /* Invalidate Cache */
        Cache_inv((xdc_Ptr)pAddr, 4u, Cache_Type_ALL, TRUE);
        #endif

        if(*pAddr == VPS_CAPT_MAGIC_NUM)
        {
            #ifdef VPS_CAPTURE_RT_DEBUG
            Vps_rprintf(" %d: CAPT_DRV: Frame not written (VPDMA CH = %d)!!!\n",
                Clock_getTicks(),
                channelNum
            );
            #endif
            status = FVID2_EFAIL;
        }
    }

    return status;
#else
    return (FVID2_SOK);
#endif
}

/* Function to Add Magic number in the Frame */
Void Vps_captTskListAddMagicNum(UInt8 *addr, UInt32 pitch)
{
#ifdef VPS_CAPT_DO_FRAME_TAGGING_FOR_FRAME_DROP_ISSUE

    volatile UInt32 *pAddr;

    if(addr)
    {
        pAddr = (UInt32*)(addr + pitch*VPS_CAPT_MAGICNUM_LINE);
        *pAddr = VPS_CAPT_MAGIC_NUM;
        #ifdef VPS_CAPT_DO_CACHE_OPS
        /* Writeback the cache */
        Cache_wb((xdc_Ptr)pAddr, 4u, Cache_Type_ALL, TRUE);
        #endif
    }
#endif
}


/**
 *  Vps_captSubFrameEnableInt
 *  \brief In this function, for every stream for which sub frame is enabled,
 *         would configure VPDMA to issue an interrupt and register the callback
 *         with the event manager.
 *
 *  \param pObj   Pointer to instance object.
 *
 *  \return       Returns TRUE is sub frame based capture is enabled, FALSE
 *                otherwise.
 */
static Int32 Vps_captSubFrameEnableInt( Vps_CaptObj *pObj )
{
    Int32 rtnValue = FVID2_SOK;

#ifdef VPS_CAPT_DONT_USE_SOC
    rtnValue = Vps_captSubFrameCfgClientInt ( pObj );
#endif /* VPS_CAPT_DONT_USE_SOC */

    if (rtnValue == FVID2_SOK)
    {
        rtnValue = Vps_captVipSubFrameRegisterIsr ( pObj );
    }

    return (rtnValue);
}


/**
 *  Vps_captSubFrameCfgClientInt
 *  \brief In this function, configures the VPDMA to issue a interrupt, on
 *         reception of X lines, uses "Change Client Interrupt Source" control
 *         descriptor and enables channel completion interrupt.
 *
 *  \param pObj   Pointer to instance object.
 *
 *  \return       Returns TRUE is sub frame based capture is enabled, FALSE
 *                otherwise.
 */

static Int32 Vps_captSubFrameCfgClientInt ( Vps_CaptObj *pObj )
{
    Int32 rtnValue = FVID2_EFAIL;
    UInt32 streamId, chId, subFrameEn, listSize;
    Vps_CaptChObj *pChObj;
    UInt8 *listAddr = NULL;

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        subFrameEn =
            pObj->createArgs.outStreamInfo[streamId].subFrameModeEnable;

        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            pChObj = &pObj->chObj[streamId][chId];
            if (subFrameEn == TRUE)
            {
                listAddr = gVps_captChangeClientInt;

                GT_assert( GT_DEFAULT_MASK, (chId == 0x0));
                /*
                 * Configuring client interrupts for the primary channel,
                 * In case we have to look at secondary channel (Chroma / Luma)
                 * configure client interrupt for secondary also
                 */
#ifdef VPS_CAPT_DONT_USE_SOC
                VpsHal_vpdmaCreateCCISCtrlDesc(
                                    listAddr,
                                    pChObj->vChannelId[0],
                                    VPSHAL_VPDMA_SOC_EOEL,
                                    pChObj->subFrameObj.subFrameSize,
                                    0x0);
#endif /* VPS_CAPT_DONT_USE_SOC */

#ifndef VPS_CAPT_DONT_USE_SOC
                VpsHal_vpdmaCreateSOCCtrlDesc(
                                    listAddr,
                                    pChObj->vChannelId[0],
                                    VPSHAL_VPDMA_SOC_EOEL,
                                    pChObj->subFrameObj->subFrameSize,
                                    0x0);
#endif /* VPS_CAPT_DONT_USE_SOC */

                listSize = VPSHAL_VPDMA_CTRL_DESC_SIZE;

                #ifdef VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL
                if ( (pChObj->vChannelNum > 0x0u) && (rtnValue == FVID2_SOK) )
                {
                    VpsHal_vpdmaCreateCCISCtrlDesc(
                                        listAddr + listSize,
                                        pChObj->vChannelId[1],
                                        VPSHAL_VPDMA_SOC_EOEL,
                                        pChObj->subFrameObj->subFrameSize,
                                        0x0);
                    listSize += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                }
                #endif /* VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL */

                    /* Post and wait for the descriptor to be consumed */
                rtnValue = Vps_captLmPostList(listAddr,
                                              listSize);
            }
        }
    }
    return (rtnValue);
}
