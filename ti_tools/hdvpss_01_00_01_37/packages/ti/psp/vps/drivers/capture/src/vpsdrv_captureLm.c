/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <stdio.h>

#include <ti/psp/vps/drivers/capture/src/vpsdrv_capturePriv.h>
#include <ti/psp/vps/drivers/capture/src/vpsdrv_captureLmPriv.h>

/*
  This file implements capture list manager (CLM)

  This file deals with processing the HW list registers.

  Actual list processing task calls these APIs to do the list processing
  logic for capture.

  See vpsdrv_captureList.c for capture list processing
*/

/* see vpsdrv_captureLm.h for function documentation  */


/* CLM Object */
Vps_CaptLmObj gVps_captLmObj;



/* List complete ISR - this is called when timer expires */
Void Vps_captLmCompleteIsr ( UArg arg )
{
    /* driver supplied CLM callback */
    gVps_captLmObj.completeCb (  );
}

Void Vps_captLmResetStatitics()
{
    gVps_captLmObj.maxListPostTime  = 0;
    gVps_captLmObj.minListPostTime  = 0xFFFF;
    gVps_captLmObj.totalListPostTime= 0;
    gVps_captLmObj.avgListPostTime  = 0;
    gVps_captLmObj.listPostCount    = 0;
    gVps_captLmObj.errorDescCount   = 0;
    gVps_captLmObj.listStallCount   = 0;
    gVps_captLmObj.extraDescProgrammedCount = 0;
    gVps_captLmObj.errorDescWrCount = 0u;
    gVps_captLmObj.intcTimeoutCount = 0u;
    gVps_captLmObj.intcTimeoutCount2= 0u;
    gVps_captLmObj.pidSpecialConditionCount = 0u;
    gVps_captLmObj.minLoopCnt = 0x7FFFFFFFu;
    gVps_captLmObj.minLoop2Cnt = 0x7FFFFFFFu;
    gVps_captLmObj.loopNum = 0;
    gVps_captLmObj.loop2Num = 0;

}

Void Vps_captLmUpdateStatitics(UInt32 listPostTime)
{
    if(listPostTime > gVps_captLmObj.maxListPostTime)
    {
        gVps_captLmObj.maxListPostTime = listPostTime;
    }

    if(listPostTime < gVps_captLmObj.minListPostTime)
    {
        gVps_captLmObj.minListPostTime = listPostTime;
    }

    gVps_captLmObj.totalListPostTime += listPostTime;
    gVps_captLmObj.listPostCount++;
    gVps_captLmObj.avgListPostTime =
        gVps_captLmObj.totalListPostTime / gVps_captLmObj.listPostCount;

    if(listPostTime > VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS)
    {
        if(gVps_captLmObj.listStallCount < VPS_CAPT_LOG_INFO_MAX)
        {
            gVps_captLmObj.listStallTimestamp[gVps_captLmObj.listStallCount] =
                Clock_getTicks() - listPostTime;
        }
        gVps_captLmObj.listStallCount++;
    }
}

Int32 Vps_captLmInit(Vps_CaptLmCompleteCb completeCb)
{
    UInt32 cnt;
    Semaphore_Params semParams;

#ifdef VPS_CFG_CAPT_USE_GP_TIMER
    Timer_Params timerParams;
#else
    Clock_Params clockParams;
#endif
    Int32 retVal = FVID2_SOK;
    UInt8 hwListNum;
    UInt16 vChannel;

    GT_assert( GT_DEFAULT_MASK, completeCb != NULL);

    /* set to 0's for list object and descriptor memory */
    memset(&gVps_captLmObj, 0, sizeof(gVps_captLmObj));

    /* set to 0xFF's for channel map */
    for(vChannel=0; vChannel<VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS; vChannel++)
    {
        gVps_captLmObj.vChannelInfo[vChannel].vChannel2lChannelMap = 0xFFFF;
    }

    Vps_captLmResetStatitics();

    /* set state to IDLE */
    gVps_captLmObj.state = VPS_CAPT_LM_STATE_IDLE;

    gVps_captLmObj.dropFrameCount = 0;

    /* set driver callback */
    gVps_captLmObj.completeCb = completeCb;

    gVps_captLmObj.hwListId = 0u;

    for(hwListNum = 0; hwListNum < VPS_CAPT_LM_MAX_LIST; hwListNum ++)
    {
        /* Get HW list from resource manager */
        retVal |= Vrm_allocList(&gVps_captLmObj.hwListNum[hwListNum], VRM_LIST_TYPE_CAPTURE);
        if(VPS_CAPT_LM_MAX_LIST > 1u)
        {
            Vps_printf(" %d: CAPTLM: HW LIST %d\r\n", Clock_getTicks(), gVps_captLmObj.hwListNum[hwListNum]);
        }
    }

    if ((FVID2_SOK == retVal) && (hwListNum < VPSHAL_VPDMA_MAX_LIST))
    {
        /* create timer object for periodic trigger of driver list processing */
#ifdef VPS_CFG_CAPT_USE_GP_TIMER
        Timer_Params_init(&timerParams);

        /* Create GP Timer handle */
        gVps_captLmObj.timerHndl = Timer_create(VPS_CFG_CAPT_GP_TIMER_ID,
                                                Vps_captLmCompleteIsr,
                                                &timerParams,
                                                NULL );

        GT_assert( GT_DEFAULT_MASK, gVps_captLmObj.timerHndl != NULL);
#else
        Clock_Params_init(&clockParams);

        gVps_captLmObj.clockHndl = Clock_create(Vps_captLmCompleteIsr,
                                                1,
                                                &clockParams,
                                                NULL );

        GT_assert( GT_DEFAULT_MASK, gVps_captLmObj.clockHndl != NULL);
#endif

        /* create locking semaphore */
        Semaphore_Params_init(&semParams);

        semParams.mode = Semaphore_Mode_BINARY;

        gVps_captLmObj.lock = Semaphore_create(1u, &semParams, NULL);

        if (gVps_captLmObj.lock == NULL)
        {
            Vps_printf ( " CAPTURE:%s:%d: Semaphore_create() failed !!!\n",
                            __FUNCTION__, __LINE__ );
            retVal = FVID2_EALLOC;
        }
    }

    /* Create the PID Mask */
    gVps_captLmObj.descPidMask = 0u;
    for (cnt = 0u; cnt < VPSHAL_VPDMA_MAX_SKIP_DESC; cnt ++)
    {
        gVps_captLmObj.descPidMask |= (1u << cnt);
    }

    return retVal;
}

Int32 Vps_captLmDeInit()
{
    UInt16 hwListId;

#ifdef VPS_CFG_CAPT_USE_GP_TIMER
    /* stop timer */
    Timer_stop(gVps_captLmObj.timerHndl);

    /* delete timer */
    Timer_delete(&gVps_captLmObj.timerHndl);
#else
    /* stop timer */
    Clock_stop(gVps_captLmObj.clockHndl);

    /* delete timer */
    Clock_delete(&gVps_captLmObj.clockHndl);
#endif

    for(hwListId = 0u; hwListId < VPS_CAPT_LM_MAX_LIST ; hwListId ++)
    {
        /* release list */
        Vrm_releaseList(gVps_captLmObj.hwListNum[hwListId]);
    }
    /* delete semaphore */
    Semaphore_delete(&gVps_captLmObj.lock);

    return FVID2_SOK;
}

/* set CLM state - driver takes care of mutual exclusion */
Int32 Vps_captLmSetState(UInt16 state)
{
    /* set state */
    gVps_captLmObj.state = state;

    return FVID2_SOK;
}

/* get current CLM state - driver takes care of mutual exclusion  */
Int32 Vps_captLmGetState()
{
    return gVps_captLmObj.state;
}

/* CLM lock */
Int32 Vps_captLmLock()
{
    /* take semaphore for locking */
    Semaphore_pend(gVps_captLmObj.lock, BIOS_WAIT_FOREVER);

    return FVID2_SOK;
}

/* CLM unlock */
Int32 Vps_captLmUnlock()
{
    /*
     * release semaphore for un-locking
     */
    Semaphore_post ( gVps_captLmObj.lock );

    return FVID2_SOK;
}

/* map driver logical channel to VPDMA channel  */
Int32 Vps_captLmMapChannel(VpsHal_VpdmaChannel vChannel,
                           UInt16 lChannel,
                           UInt16 vChannelIdx)
{
    Vps_CaptLmVpdmaChannelInfo *vChannelInfo;

    GT_assert( GT_DEFAULT_MASK, vChannel < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS);
    GT_assert( GT_DEFAULT_MASK, lChannel < VPS_CAPT_CH_MAX * VPS_CAPT_STREAM_ID_MAX);

    vChannelInfo = &gVps_captLmObj.vChannelInfo[vChannel];

    /* map driver logical channel to VPDMA channel */
    vChannelInfo->vChannel2lChannelMap = lChannel;

    /* map driver logical plane (Y/C) to VPDMA channel */
    vChannelInfo->vChannel2vChannelIdxMap = vChannelIdx;

    vChannelInfo->state = VPS_CAPT_LM_VCH_STATE_NORMAL;
    vChannelInfo->lastBufAddr = NULL;

    return FVID2_SOK;
}

/* get driver logical channel associated with this VPDMA channel  */
UInt16 Vps_captLmGetLogicalChannelId(VpsHal_VpdmaChannel vChannel)
{
    Vps_CaptLmVpdmaChannelInfo *vChannelInfo;

    GT_assert( GT_DEFAULT_MASK, vChannel < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS);

    vChannelInfo = &gVps_captLmObj.vChannelInfo[vChannel];

    return vChannelInfo->vChannel2lChannelMap;
}

/* get driver logical channel plane (Y/C) associated with this VPDMA channel  */
UInt16 Vps_captLmGetVpdmaChannelIdx(VpsHal_VpdmaChannel vChannel)
{
    Vps_CaptLmVpdmaChannelInfo *vChannelInfo;

    GT_assert( GT_DEFAULT_MASK, vChannel < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS);

    vChannelInfo = &gVps_captLmObj.vChannelInfo[vChannel];

    return vChannelInfo->vChannel2vChannelIdxMap;
}

/* add descriptor to current list */
Int32 Vps_captLmAddDataDesc(Vps_CaptLmDataDescAddInfo *info)
{
    VpsHal_VpdmaOutDescParams descPrm;
    Vps_CaptLmVpdmaChannelInfo *vChInfo;

    /*
     * make descriptor structure based on supplied parameters
     */
    descPrm.channel = info->vChannel;
    descPrm.dataType = info->dataType;
    descPrm.lineStride = info->lineStride;
    descPrm.lineSkip = info->lineSkip;
    descPrm.notify = FALSE;
    descPrm.priority = info->priority;
    descPrm.memType = info->memType;
    descPrm.nextChannel = info->vChannel;
    descPrm.maxWidth = (VpsHal_VpdmaOutBoundMaxWidth)info->maxOutWidth;
    descPrm.maxHeight = (VpsHal_VpdmaOutBoundMaxHeight)info->maxOutHeight;
    descPrm.is1DMem = FALSE;

    vChInfo  = &gVps_captLmObj.vChannelInfo[info->vChannel];

    vChInfo->curDescId ^= 1;

    /*
     * set to 0's descriptor area
     */
#ifdef VPS_CLEAR_ALL_MEM
    memset (info->descAddr, 0, VPSHAL_VPDMA_DATA_DESC_SIZE);
#endif

    /*
     * create descriptor
     */
    VpsHal_vpdmaCreateOutBoundDataDesc(info->descAddr, &descPrm);

    /*
     * mark to output completed descriptor: For Tiled mode, writeback desc is disabled
     * as address needs to be tiled space. Giving non-tiled address cause VIP parser overflow
     */
    if (descPrm.memType == VPSHAL_VPDMA_MT_TILEDMEM)
        VpsHal_vpdmaSetWriteDesc (info->descAddr, 0);
    else
        VpsHal_vpdmaSetWriteDesc (info->descAddr, 1);

    /*
     * mark to indicate usage of VPDMA complete desciptor info registers
     */
    VpsHal_vpdmaSetUseDescReg (info->descAddr, 0);

    if ( info->addr == NULL )
    {
        /*
         * data address is NULL, mark to drop drop
         */
        VpsHal_vpdmaSetDropData (info->descAddr, 1 );
        VpsHal_vpdmaSetMemType (info->descAddr, VPSHAL_VPDMA_MT_NONTILEDMEM);
    }

    /* Enable Skip Descriptor Bit */
    VpsHal_vpdmaEnableDescSkip(info->descAddr, info->enableDescSkip);

    /*
     * set data address
     */
    VpsHal_vpdmaSetAddress (info->descAddr, 0, info->addr );

    return FVID2_SOK;
}

/*
  Add abort desriptor for a given VPDMA channel
*/
Int32 Vps_captLmAddAbortDesc(UInt8 *memPtr,  VpsHal_VpdmaChannel vChannel)
{
    GT_assert( GT_DEFAULT_MASK, vChannel < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS);

    /*
     * create abort descriptor
     */
    VpsHal_vpdmaCreateAbortCtrlDesc(memPtr, vChannel);

    return (FVID2_SOK);
}

/*
  Submit list to HW

  checkIntervalInTicks - time is OS ticks after which timer interrupt
                          should trigger

  Timer interrupt is used by driver to do list processing
*/
Int32 Vps_captLmSubmit(Int16 checkIntervalInTicks)
{
#ifdef VPS_CFG_CAPT_USE_GP_TIMER
    /*
     * set timer expiry time in micro seconds
     */
    Timer_setPeriodMicroSecs(
        gVps_captLmObj.timerHndl,
        VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS * 1000u);

    /*
     * start timer
     */
    Timer_start ( gVps_captLmObj.timerHndl );
#else
    /*
     * set timer expiry time in OS ticks
     */
    Clock_setTimeout(
        gVps_captLmObj.clockHndl,
        VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS);
    Clock_setPeriod(
        gVps_captLmObj.clockHndl,
        VPS_CAPT_LIST_UPDATE_TIME_IN_TICKS);

    /*
     * start timer
     */
    Clock_start ( gVps_captLmObj.clockHndl );
#endif

    /*
     * mark CLM state as running
     */
    gVps_captLmObj.state = VPS_CAPT_LM_STATE_RUNNING;

    return FVID2_SOK;
}

/*
  Parse data descriptor in current list position
*/
Int32 Vps_captLmParseDataDesc (Vps_CaptLmDataDescParseInfo *info)
{
    volatile VpsHal_VpdmaInDataDesc *pDataDesc;

    /*
     * goto current list position
     */
    pDataDesc = (VpsHal_VpdmaInDataDesc *) (info->descAddr);

    /*
     * parse information from descriptor
     */
    VpsHal_vpdmaGetFrameDim(info->descAddr,
                            &info->frameWidth,
                            &info->frameHeight);
    VpsHal_vpdmaGetFid(info->descAddr, &info->fid);
    VpsHal_vpdmaGetChannel(info->descAddr, &info->vChannel);

#if 0
    info->fid = pDataDesc->fieldId;
    info->frameWidth = pDataDesc->transferWidth;
    info->frameHeight = pDataDesc->transferHeight;
    info->vChannel = (VpsHal_VpdmaChannel) pDataDesc->channel;
#endif

    GT_assert( GT_DEFAULT_MASK,  info->vChannel < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS );

    info->lChannel = Vps_captLmGetLogicalChannelId (info->vChannel);
    info->vChannelIdx = Vps_captLmGetVpdmaChannelIdx (info->vChannel);

    if(info->lChannel==0xFFFF)
    {
        /* VPDMA channel not mapped to logical channel */
        #ifdef VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
        Vps_captLmLogErrorDesc(pDataDesc);
        #endif

        return FVID2_EFAIL;
    }
    return FVID2_SOK;
}

/* Function to parse the write descriptor. It returns FID, frame
   size if the descriptor is written out by the VPDMA. It will
   check Data Desc. Packet Type and channel number to determine
   whether descriptor is written by VPDMA or not. */
Int32 Vps_captLmParseDescWrInfo(UInt8 *pDescPtr,
                                UInt8 *pDataAddr,
                                UInt32 channelNum,
                                UInt32 *fid,
                                UInt32 *frameWidth,
                                UInt32 *frameHeight)
{
    Int32                   retVal = FVID2_SOK;
    VpsHal_VpdmaInDataDesc *pDataDesc;

    GT_assert( GT_DEFAULT_MASK, NULL != pDescPtr);
    GT_assert( GT_DEFAULT_MASK, channelNum < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS);

    pDataDesc = (VpsHal_VpdmaInDataDesc *)pDescPtr;

    if ((VPSHAL_VPDMA_PT_DATA != pDataDesc->descType) ||
        (pDataDesc->channel != channelNum))
    {
        gVps_captLmObj.errorDescWrCount++;
        retVal = FVID2_EFAIL;

        #ifdef VPS_CAPTURE_RT_DEBUG
        if(VPSHAL_VPDMA_PT_DATA != pDataDesc->descType)
        {
            Vps_rprintf(" %d: CAPT_DRV: NO Desc written (VPDMA CH = %d) !!!\n",
                Clock_getTicks(),
                channelNum
            );
        } else
        {
            Vps_rprintf(" %d: CAPT_DRV: Desc with wrong CH %d written (VPDMA CH = %d) !!!\n",
                Clock_getTicks(),
                pDataDesc->channel,
                channelNum
            );
        }
        #endif
    }
    if(retVal==FVID2_SOK)
    {
        #ifdef VPS_CAPTURE_RT_DEBUG
        if((UInt32)pDataAddr != pDataDesc->address)
        {
            Vps_rprintf(" %d: CAPT_DRV: Desc with wrong data address (0x%08X != 0x%08X) (VPDMA CH = %d) !!!\n",
                Clock_getTicks(),
                pDataDesc->address,
                pDataAddr,
                channelNum
            );
        }

        if(pDataDesc->transferHeight==243)
        {
            if(pDataDesc->fieldId==0)
            {
                Vps_rprintf(" %d: CAPT_DRV: Desc with wrong FID (%d) (VPDMA CH = %d) !!!\n",
                    Clock_getTicks(),
                    pDataDesc->fieldId,
                    channelNum
                );

              pDataDesc->fieldId = 1;
            }
        }
        if(pDataDesc->transferHeight==244)
        {
            if(pDataDesc->fieldId==1)
            {
                Vps_rprintf(" %d: CAPT_DRV: Desc with wrong FID (%d) (VPDMA CH = %d) !!!\n",
                    Clock_getTicks(),
                    pDataDesc->fieldId,
                    channelNum
                );

              pDataDesc->fieldId = 0;
            }
        }

        if(pDataDesc->transferHeight<=1||pDataDesc->transferWidth<=1)
        {
            Vps_rprintf(" %d: CAPT_DRV: Desc with wrong W x H (%d x %d) (VPDMA CH = %d) !!!\n",
                Clock_getTicks(),
                pDataDesc->transferWidth,
                pDataDesc->transferHeight,
                channelNum
            );
        }
        #endif

        *fid = pDataDesc->fieldId;
        *frameWidth = pDataDesc->transferWidth;
        *frameHeight = pDataDesc->transferHeight;
    }

    pDataDesc->descType = 0;
    pDataDesc->channel  = 0;
    pDataDesc->address  = 0;

    return (retVal);
}



/*
  Update a parsed data desc in current list position
*/
Int32 Vps_captLmUpdateDataDesc (Vps_CaptLmDataDescUpdateInfo *info)
{
    volatile VpsHal_VpdmaOutDataDescOverlay *pDataDesc;
    volatile UInt32 value;

    GT_assert( GT_DEFAULT_MASK, info->vChannel < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS);

    /*
     * goto current list position
     */
    pDataDesc = (VpsHal_VpdmaOutDataDescOverlay *) (info->descAddr);

    /* clear field ID bit */
    pDataDesc->dataInfo &= ~0x01000000;

    pDataDesc->reserved = 0;

    /*
     * mark descriptor as outbound
     */
    pDataDesc->channelInfo |=
        (   ( VPSHAL_VPDMA_OUTBOUND_DATA_DESC << 25 )
        |   ( ((UInt32)info->memType & 0x1) << 26 )
        );

    /* clear SkipDesc Bit */
    pDataDesc->channelInfo &= ~(0x00008000u);
    pDataDesc->channelInfo |= ((info->enableDescSkip & 0x1) << 15u);

    /*
     * Use Descriptor write address from DEscriptor
     */
    value = 0u;

    /*
     * mark enable write descriptor field
     */
    value |= ( ( info->enableDescWb & 0x1 ) << 2 );

    if (info->enableDescWb)
    {
        value |= ((UInt32)info->descWrAddr & 0xFFFFFFE0u);
    }

    if ((info->bufAddr == NULL) ||
        (0 != info->dropData))
    {
        /*
         * if address is NULL, then mark drop data. And if drop data flag is 
         * set, then Write Descriptor bit should be set according to VPDMA spec.
         */
        value |= (1 << 2) | (1 << 1);

        /* When drop is set, MODE should be the same as captured data  */
        if (VPSHAL_VPDMA_MT_TILEDMEM == info->memType)
        {
            pDataDesc->channelInfo |= 1 << 26;
        }
        else
        {
            pDataDesc->channelInfo &= ~(1 << 26);
        }

        /* set info->addr with some non-zero value */
        gVps_captLmObj.dropFrameCount++;
    }

    /*
     * update value in descritpor
     */
    pDataDesc->descWriteInfo = value;

    /*
     * update address field
     */
    pDataDesc->startAddr = VpsHal_vpdmaVirtToPhy((Ptr)info->bufAddr);

    /*
     * set max width x height
     * max height is set by user, max width is set to 0, i.e unlimited
     */
    pDataDesc->maxWidthHeight = (info->maxOutWidth<<4) | info->maxOutHeight;

    return FVID2_SOK;
}

UInt32 Vps_captLmPostList(UInt8 *listAddr, UInt16 listSize)
{
    Int32 retVal;
    UInt32 listId;

    listId = gVps_captLmObj.hwListNum[0u];

    /* Make Sure list is free */
    retVal = VpsHal_vpdmaListWaitComplete(
                listId,
                VPS_CAPT_LM_LIST_POST_TIMEOUT);
    GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);

    retVal = VpsHal_vpdmaPostList(
                listId,
                VPSHAL_VPDMA_LT_NORMAL,
                listAddr,
                listSize,
                TRUE);

    retVal = VpsHal_vpdmaListWaitComplete(
                listId,
                VPS_CAPT_LM_LIST_POST_TIMEOUT);

    if(FVID2_SOK != retVal)
    {
        /* list post hang, print advanced capture statistics */
        Vps_captPrintAdvancedStatistics(Clock_getTicks());
    }
    GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);

    return (FVID2_SOK);
}

UInt32 Vps_captLmPostDescList(UInt8 *listAddr, UInt16 listSize)
{
    Int32            retVal;
    UInt32           curTime, cookie;
    volatile UInt32  pid;
    volatile Int32   loopCount=1000;
    volatile Int32   loopCount2=1000;

    curTime = Clock_getTicks();

    /* check list is free */
    retVal = VpsHal_vpdmaListWaitComplete(
                gVps_captLmObj.hwListNum[0u],
                VPS_CAPT_LM_LIST_POST_TIMEOUT);

    GT_assert( GT_DEFAULT_MASK, retVal==FVID2_SOK);

    cookie = Task_disable();

    /* Clear any pending list complete interrupt */
    VpsHal_vpdmaClearListCompleteStatus(gVps_captLmObj.hwListNum[0u]);

    retVal = VpsHal_vpdmaPostList(
                gVps_captLmObj.hwListNum[0u],
                VPSHAL_VPDMA_LT_NORMAL,
                listAddr,
                listSize,
                TRUE);

    GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);

    /* poll until list complete INTC bit is set */
    while(VpsHal_vpdmaGetListCompleteStatus(gVps_captLmObj.hwListNum[0u]) == 0u)
    {
        loopCount--;
        if(loopCount <= 0)
        {
            gVps_captLmObj.intcTimeoutCount++;
            break;
        }
    }

    gVps_captLmObj.loopCntLog[gVps_captLmObj.loopNum] = loopCount;
    if (loopCount < gVps_captLmObj.minLoopCnt)
    {
        gVps_captLmObj.minLoopCnt = loopCount;
    }
    gVps_captLmObj.loopNum++;
    gVps_captLmObj.loopNum %= VPS_CAPT_LOOP_CNT_NUM_LOG;

    VpsHal_vpdmaClearListCompleteStatus(gVps_captLmObj.hwListNum[0u]);

    pid = VpsHal_vpdmaGetDeadListStatus(
            gVps_captLmObj.hwListNum[0u],
            0xDEAD0000u);

    pid = pid & 0xFF;

    if ((0xFE == pid) || (0xFF == pid))
    {
        volatile UInt32 tmpPid;

        tmpPid = VpsHal_vpdmaGetDeadListStatus(
                gVps_captLmObj.hwListNum[0u],
                0x00000000);

        tmpPid = (tmpPid & 0xFF);

        if((tmpPid & 0x2) != 0x2)
        {
            pid = (tmpPid & 0x3);
            gVps_captLmObj.pidSpecialConditionCount++;
        }
    }

    VpsHal_vpdmaStopList(gVps_captLmObj.hwListNum[0u], VPSHAL_VPDMA_LT_NORMAL);

    /* 9c. (NEW) Poll INT0_LIST0_INT_STAT until bit 0 is set */
    while (VpsHal_vpdmaGetListCompleteStatus(gVps_captLmObj.hwListNum[0u]) == 0u)
    {
        loopCount2--;
        if(loopCount2 <= 0)
        {
            gVps_captLmObj.intcTimeoutCount2++;
            break;
        }
    }
    gVps_captLmObj.loop2CntLog[gVps_captLmObj.loop2Num] = loopCount2;
    if (loopCount2 < gVps_captLmObj.minLoop2Cnt)
    {
        gVps_captLmObj.minLoop2Cnt = loopCount2;
    }
    gVps_captLmObj.loop2Num ++;
    gVps_captLmObj.loop2Num %= VPS_CAPT_LOOP_CNT_NUM_LOG;

    Task_restore(cookie);

    retVal = VpsHal_vpdmaListWaitComplete(
                gVps_captLmObj.hwListNum[0u],
                VPS_CAPT_LM_LIST_POST_TIMEOUT);

    if(retVal!=FVID2_SOK)
    {
        /* list post hang, print advanced capture statistics */
        Vps_captPrintAdvancedStatistics(Clock_getTicks());
    }
    GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);

    curTime = Clock_getTicks() - curTime;

    #ifdef VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
    Vps_captLmUpdateStatitics(curTime);
    #endif

    if (0u == pid)
    {
        gVps_captLmObj.zeroPidCnt ++;
    }

    #ifdef VPS_CAPTURE_RT_DEBUG
    if(loopCount2<=0 || loopCount<=0)
    {
        Vps_rprintf(" %d: CAPT_DRV: INTC Timeout hit !!! (%d, %d) !!!\n",
            Clock_getTicks(),
            loopCount,
            loopCount2
        );
    }
    #endif

    return (pid & gVps_captLmObj.descPidMask);
}


Int32 Vps_captLmAddScCoeffConfigDesc(UInt32 vipInstId, Ptr listAddr, Ptr coeffMem)
{

    GT_assert( GT_DEFAULT_MASK, vipInstId < VPSHAL_VIP_INST_MAX);

    /* Create a configuration descriptor and post it */
    VpsHal_vpdmaCreateConfigDesc(
        listAddr,
        (VpsHal_VpdmaConfigDest)(VPSHAL_VPDMA_CONFIG_DEST_SC3 + vipInstId),
        VPSHAL_VPDMA_CPT_BLOCK_SET,
        VPSHAL_VPDMA_CCT_INDIRECT,
        VPS_CAPT_VIP_SC_COEFF_SIZE,
        coeffMem,
        NULL,
        VPS_CAPT_VIP_SC_COEFF_SIZE);

    Vps_captLmPostList(listAddr, VPSHAL_VPDMA_CTRL_DESC_SIZE);

    return (FVID2_SOK);
}

Int32 Vps_captLmAddDummyDisSyncDesc(Ptr memPtr, VpsHal_VpdmaChannel vChannel)
{
    VpsHal_VpdmaOutDescParams descPrm;

    /* make descriptor structure based on supplied parameters */
    descPrm.channel = vChannel;
    descPrm.dataType = VPSHAL_VPDMA_CHANDT_RGB888;
    descPrm.lineStride = 32;
    descPrm.lineSkip = VPSHAL_VPDMA_LS_1;
    descPrm.notify = FALSE;
    descPrm.priority = VPS_CFG_CAPT_VPDMA_PRIORITY;
    descPrm.memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
    descPrm.nextChannel = vChannel;
    descPrm.maxWidth = (VpsHal_VpdmaOutBoundMaxWidth)0;
    descPrm.maxHeight = (VpsHal_VpdmaOutBoundMaxHeight)0;
    descPrm.is1DMem = FALSE;

    /* set to 0's descriptor area */
#ifdef VPS_CLEAR_ALL_MEM
    memset(memPtr, 0, VPSHAL_VPDMA_DATA_DESC_SIZE);
#endif

    /* create descriptor */
    VpsHal_vpdmaCreateOutBoundDataDesc(memPtr, &descPrm);

    /* data address is NULL, mark to drop drop */
    VpsHal_vpdmaSetDropData(memPtr, 1);

    return FVID2_SOK;
}

Int32 Vps_captLmResetCurDescRecv()
{
    Vps_CaptLmVpdmaChannelInfo *vChannelInfo;
    UInt16 vChannel;

    for(vChannel = 0; vChannel<VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS; vChannel++)
    {
        vChannelInfo = &gVps_captLmObj.vChannelInfo[vChannel];

        vChannelInfo->curDescRecv = 0;
    }

    return 0;
}

#if 0
Int32 Vps_captLmHwQueReset()
{
    Vps_CaptLmHwQueObj *hwQue;

    hwQue = &gVps_captLmObj.hwQueObj;

    hwQue->hwQueStartAddr = &gVps_captListMem[VPS_CAPT_LM_LIST_SIZE_MAX];
    hwQue->hwQueEndAddr   = hwQue->hwQueStartAddr + VPS_CAPT_LM_LIST_SIZE_MAX;
    hwQue->hwQueCurAddr   = hwQue->hwQueStartAddr;
    hwQue->hwQuePrevAddr  = hwQue->hwQueStartAddr;

    VpsHal_vpdmaSetDescWriteAddrReg(
        hwQue->hwQueStartAddr,
        VPS_CAPT_LM_LIST_SIZE_MAX
        );

    GT_assert( GT_DEFAULT_MASK, hwQue->hwQueCurAddr==VpsHal_vpdmaGetCurrentDescWriteAddr());

    return 0;
}

UInt32 Vps_captLmHwGetNumDescs(Vps_CaptLmHwQueObj *hwQue)
{
    UInt32 numDescs;

    /* previous address is last current address value */
    hwQue->hwQuePrevAddr = hwQue->hwQueCurAddr;

    /* update current to new value */
    hwQue->hwQueCurAddr = VpsHal_vpdmaGetCurrentDescWriteAddr();

    /* wrap around if needed */
    if(hwQue->hwQueCurAddr>=hwQue->hwQueEndAddr)
    {
        Vps_printf("currentAddr = %x\n", hwQue->hwQueCurAddr);
        GT_assert( GT_DEFAULT_MASK, FALSE);
        hwQue->hwQueCurAddr = hwQue->hwQueStartAddr;


    }

    if(hwQue->hwQueCurAddr>=hwQue->hwQuePrevAddr)
    {
        /* no wrap around case */
        numDescs =
            (hwQue->hwQueCurAddr-hwQue->hwQuePrevAddr)
            /
            VPSHAL_VPDMA_DATA_DESC_SIZE
            ;
    }
    else
    {
        /* wrap around case */

        /* there are some desc's at the bottom */
        numDescs =
            (hwQue->hwQueEndAddr-hwQue->hwQuePrevAddr)
            /
            VPSHAL_VPDMA_DATA_DESC_SIZE
            ;

        /* and there are some desc's at the top */
        numDescs +=
            (hwQue->hwQueCurAddr-hwQue->hwQueStartAddr)
            /
            VPSHAL_VPDMA_DATA_DESC_SIZE
            ;
    }

    /* wait for desc's to actually get written to memory */
    Task_sleep(1);

    return numDescs;
}

/*
    Copy complete descriptor from HW que to list post descriptor area
*/
Int32 Vps_captLmSwitch ()
{
    Vps_CaptLmHwQueObj *hwQue;
    volatile VpsHal_VpdmaInDataDescOverlay *pSrcDesc, *pDestDesc;
    volatile VpsHal_VpdmaInDataDesc *pDataDesc, *pInDestDesc;
    UInt16 numRecvDescs, numValidDescs, descId;
    Vps_CaptLmVpdmaChannelInfo *vChannelInfo;
    UInt16 vChannel;
    Bool errorDesc;

    hwQue = &gVps_captLmObj.hwQueObj;

    numRecvDescs = Vps_captLmHwGetNumDescs(hwQue);

    pDestDesc = (VpsHal_VpdmaInDataDescOverlay*)gVps_captLmObj.listObj.listAddr;
    pSrcDesc  = (VpsHal_VpdmaInDataDescOverlay*)hwQue->hwQuePrevAddr;

    numValidDescs=0;

    Vps_captLmResetCurDescRecv();

    for(descId=0; descId<numRecvDescs; descId++)
    {
        /* wrap around HW que if needed */
        if((UInt8*)pSrcDesc>=hwQue->hwQueEndAddr)
        {
            pSrcDesc = (VpsHal_VpdmaInDataDescOverlay*)hwQue->hwQueStartAddr;
        }

        pDataDesc = (VpsHal_VpdmaInDataDesc *)pSrcDesc;

        #ifdef VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
        Vps_captLmLogRecvDesc(pDataDesc);
        #endif

        pDataDesc->channel &= 0xFF;

        errorDesc=FALSE;

        if(pDataDesc->descType != VPSHAL_VPDMA_PT_DATA)
        {
            errorDesc = TRUE;
        }
        else
        {

            UInt8 *curBufAddr;

            curBufAddr = (UInt8*)pSrcDesc->startAddr;

            vChannel = pDataDesc->channel;

            GT_assert( GT_DEFAULT_MASK,  vChannel < VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS );

            vChannelInfo = &gVps_captLmObj.vChannelInfo[vChannel];

            if(       pDataDesc->transferHeight == 1
                  &&  pDataDesc->transferWidth == 1
              )
            {
                if(vChannelInfo->lastBufAddr==curBufAddr)
                {
                    errorDesc = TRUE;
                }
            }

            if(!errorDesc)
            {
                vChannelInfo->curDescRecv++;

                if(vChannelInfo->curDescRecv<=2)
                {
                    if(vChannelInfo->lastBufAddr==curBufAddr)
                        vChannelInfo->state = VPS_CAPT_LM_VCH_STATE_REPEAT_ADDR;
                    else
                        vChannelInfo->state = VPS_CAPT_LM_VCH_STATE_NORMAL;

                    vChannelInfo->lastBufAddr  = curBufAddr;

                    /* copy desc from HW que area to list post area
                       copy only contents that will really be needed
                    */
                    pDestDesc->dataInfo             = pSrcDesc->dataInfo;
                    pDestDesc->transferWidthHeight  = pSrcDesc->transferWidthHeight;
                    pDestDesc->channelInfo          = pSrcDesc->channelInfo;
                    pDestDesc->startAddr            = (UInt32)curBufAddr;
                    pDestDesc->frameWidthHeight     = 0;
                    pDestDesc->horzVertStart        = 0;
                    pDestDesc->clientSpecific[0]    = 0;
                    pDestDesc->clientSpecific[1]    = 0;

                    pInDestDesc  = (VpsHal_VpdmaInDataDesc *)pDestDesc;
                    pInDestDesc->nextChannel = pDataDesc->channel |
                        (vChannelInfo->curDescId << 0x8);
                    vChannelInfo->curDescId ^= 0x1;

                    pDestDesc++;
                    numValidDescs++;

                    if (vChannelInfo->programExtraDesc == TRUE)
                    {
                        vChannelInfo->numProgramExtraDesc++;
                        gVps_captLmObj.extraDescProgrammedCount++;

                        pDestDesc->dataInfo             = pSrcDesc->dataInfo;
                        pDestDesc->transferWidthHeight  = pSrcDesc->transferWidthHeight;
                        pDestDesc->channelInfo          = pSrcDesc->channelInfo;
                        pDestDesc->startAddr            = 0;
                        pDestDesc->frameWidthHeight     = 0;
                        pDestDesc->horzVertStart        = 0;
                        pDestDesc->clientSpecific[0]    = 0;
                        pDestDesc->clientSpecific[1]    = 0;

                        pInDestDesc  = (VpsHal_VpdmaInDataDesc *)pDestDesc;
                        pInDestDesc->nextChannel = pDataDesc->channel |
                            (vChannelInfo->curDescId << 0x8);
                        vChannelInfo->curDescId ^= 0x1;

                        pDestDesc++;
                        numValidDescs++;
                    }
                }
                else
                {
                    errorDesc = TRUE;
                }
            }
        }

        if(errorDesc)
        {
            #if defined(VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG) && defined(VPS_CAPT_LIST_ENABLE_ASSERT_ON_ERROR)
            Vps_captLmLogHwQueInfo(hwQue, numRecvDescs, numValidDescs);
            #endif

            /* error desc */
            #ifdef VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
            Vps_captLmLogErrorDesc(pDataDesc);
            #endif

            #ifdef VPS_CAPT_LIST_ENABLE_ASSERT_ON_ERROR
            Vps_captPrintAdvancedStatistics(Clock_getTicks());
            GT_assert( GT_DEFAULT_MASK, 0);
            #endif
        }

        /* clear descType field in HW que area desc to 0 */
        pSrcDesc->channelInfo &= 0x00FFFFFF;
        pSrcDesc++;
    }

    #ifdef VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
    Vps_captLmLogHwQueInfo(hwQue, numRecvDescs, numValidDescs);
    #endif

    return numValidDescs;
}
#endif


Void Vps_captLmCheckDescId(volatile VpsHal_VpdmaInDataDesc *pSrcDesc)
{
    UInt32 recvDescId;
    Vps_CaptLmVpdmaChannelInfo *vChInfo;

    vChInfo = &gVps_captLmObj.vChannelInfo[pSrcDesc->channel];
    vChInfo->programExtraDesc = FALSE;

    recvDescId = (pSrcDesc->nextChannel & 0x100) >> 8;
    if (recvDescId == vChInfo->prevDescId)
    {
        vChInfo->programExtraDesc = TRUE;
    }
    vChInfo->prevDescId = recvDescId;
}

Int32 Vps_captLmLogHwQueInfo(Vps_CaptLmHwQueObj *hwQue, UInt16 numRecvDescs, UInt16 numValidDescs)
{
    Vps_CaptLmHwQueInfoLog *hwQueInfo;

    hwQueInfo = &gVps_captLmObj.hwQueInfo[gVps_captLmObj.hwQueInfoCount%VPS_CAPT_LOG_INFO_MAX];
    gVps_captLmObj.hwQueInfoCount++;

    hwQueInfo->timestamp = Clock_getTicks();
    hwQueInfo->numRecvDescs = numRecvDescs;
    hwQueInfo->curAddr = hwQue->hwQueCurAddr;
    hwQueInfo->prevAddr = hwQue->hwQuePrevAddr;
    hwQueInfo->numValidDescs = numValidDescs;

    return 0;
}

Int32 Vps_captLmLogErrorDesc(volatile VpsHal_VpdmaInDataDesc *pDataDesc)
{
    Vps_CaptLmDescInfo *pErrorDescInfo;

    pErrorDescInfo = &gVps_captLmObj.errorDescInfo[gVps_captLmObj.errorDescCount%VPS_CAPT_LOG_INFO_MAX];

    pErrorDescInfo->vChannel  = (VpsHal_VpdmaChannel)pDataDesc->channel;
    pErrorDescInfo->width = pDataDesc->transferWidth;
    pErrorDescInfo->height = pDataDesc->transferHeight;
    pErrorDescInfo->addr    = pDataDesc->address;
    pErrorDescInfo->descAddr    = (UInt32)pDataDesc;
    pErrorDescInfo->timestamp = Clock_getTicks();

    gVps_captLmObj.errorDescCount++;
#if 0
    if (FALSE == gVps_captLmObj.gotErrDesc)
    {
        gVps_captLmObj.lastErrDescTs = pErrorDescInfo->timestamp;
        gVps_captLmObj.gotErrDesc = TRUE;
        Vps_captLmPrintAdvancedStatistics();
        PrintBuffers();
        GT_assert( GT_DEFAULT_MASK, FALSE);
    }
#endif

    return 0;
}
Int32 Vps_captLmLogRecvDesc(volatile VpsHal_VpdmaInDataDesc *pDataDesc)
{
    Vps_CaptLmDescInfo *pDescInfo;
    pDescInfo = &gVps_captLmObj.recvDescInfo[gVps_captLmObj.recvDescCount%VPS_CAPT_LOG_INFO_MAX];

    pDescInfo->vChannel  = (VpsHal_VpdmaChannel)pDataDesc->channel;
    pDescInfo->width = pDataDesc->transferWidth;
    pDescInfo->height = pDataDesc->transferHeight;
    pDescInfo->addr    = pDataDesc->address;
    pDescInfo->fid     = pDataDesc->fieldId;
    pDescInfo->timestamp = Clock_getTicks();
    (gVps_captLmObj.recvDescCount++) % VPS_CAPT_LOG_INFO_MAX;

    return 0;
}

Int32 Vps_captLmPrintRecvDesc()
{
    UInt32 recvDescCnt;

    for (recvDescCnt = 0; recvDescCnt < VPS_CAPT_LOG_INFO_MAX; recvDescCnt++)
    {
        Vps_printf(" %6d: %6dms: CH%03x (%d)\n",
            recvDescCnt,
            gVps_captLmObj.recvDescInfo[recvDescCnt].timestamp,
            gVps_captLmObj.recvDescInfo[recvDescCnt].vChannel,
            gVps_captLmObj.recvDescInfo[recvDescCnt].fid
            );
    }
    return FVID2_SOK;
}

Int32 Vps_captLmPrintAdvancedStatistics()
{
    Vps_CaptLmDescInfo *pDescInfo;
    UInt32 idx, numIdx;

    Vps_printf(" \r\n");
    Vps_printf(" *** Capture List Manager Advanced Statistics *** \r\n");
    Vps_printf(" \r\n");

    Vps_printf(" List Post Count        : %d \r\n", gVps_captLmObj.listPostCount);
    Vps_printf(" List Stall Count       : %d \r\n", gVps_captLmObj.listStallCount);
    Vps_printf(" List Post Time (ms)    : Max = %d, Min = %d, Avg = %d, Total = %d \r\n",
        gVps_captLmObj.maxListPostTime,
        gVps_captLmObj.minListPostTime,
        gVps_captLmObj.avgListPostTime,
        gVps_captLmObj.totalListPostTime
        );
    Vps_printf(" INTC Timeout Count     : (%d, %d) (Min timeout value = %d, %d) \r\n",
            gVps_captLmObj.intcTimeoutCount,
            gVps_captLmObj.intcTimeoutCount2,
            gVps_captLmObj.minLoopCnt,
            gVps_captLmObj.minLoop2Cnt
        );

    Vps_printf(" Descriptor miss found count : %d \r\n", gVps_captCommonObj.descMissFound);

    #if 0
    Vps_printf(" Error descriptor count : %d \r\n", gVps_captLmObj.errorDescCount);
    Vps_printf(" Recv  descriptor count : %d \r\n", gVps_captLmObj.recvDescCount);
    Vps_printf(" Extra descriptor programmed count : %d \r\n", gVps_captLmObj.extraDescProgrammedCount);
    #endif
    Vps_printf(" \r\n");

    if(gVps_captLmObj.errorDescCount)
    {
        Vps_printf(" Error descriptor Info, \r\n");

        numIdx = gVps_captLmObj.errorDescCount;
        if(numIdx>VPS_CAPT_LOG_INFO_MAX)
            numIdx = VPS_CAPT_LOG_INFO_MAX;

        for(idx=0; idx<numIdx; idx++)
        {
            pDescInfo = &gVps_captLmObj.errorDescInfo[idx];

            {
                Vps_printf(" %3d: %8d ms: VPDMA CH %3d %3dx%3d @ 0x%08x descAddr:0x%08x\r\n",
                    idx,
                    pDescInfo->timestamp,
                    pDescInfo->vChannel,
                    pDescInfo->width,
                    pDescInfo->height,
                    pDescInfo->addr,
                    pDescInfo->descAddr
                );
            }
        }
        Vps_printf(" \r\n");
    }
    if(gVps_captLmObj.listStallCount)
    {
        Vps_printf(" List Stall Info, \r\n");

        numIdx = gVps_captLmObj.listStallCount;
        if(numIdx>VPS_CAPT_LOG_INFO_MAX)
            numIdx = VPS_CAPT_LOG_INFO_MAX;

        for(idx=0; idx<numIdx; idx++)
        {
            Vps_printf(" %3d: %8d ms\r\n",
                idx,
                gVps_captLmObj.listStallTimestamp[idx]
            );
        }
        Vps_printf(" \r\n");
    }
    #ifdef VPS_CAPT_LIST_ENABLE_ASSERT_ON_ERROR
    if(gVps_captLmObj.hwQueInfoCount)
    {
        Vps_CaptLmHwQueInfoLog *hwQueInfo;

        Vps_printf(" HW Que Info, \r\n");

        numIdx = gVps_captLmObj.hwQueInfoCount;
        if(numIdx>VPS_CAPT_LOG_INFO_MAX)
            numIdx = VPS_CAPT_LOG_INFO_MAX;

        for(idx=0; idx<numIdx; idx++)
        {
            hwQueInfo = &gVps_captLmObj.hwQueInfo[idx];

            Vps_printf(" %3d: %8d ms (%d, %d) desc @ 0x%08x .. 0x%08x\r\n",
                idx,
                hwQueInfo->timestamp,
                hwQueInfo->numRecvDescs,
                hwQueInfo->numValidDescs,
                hwQueInfo->prevAddr,
                hwQueInfo->curAddr
            );
        }
        Vps_printf(" \r\n");
    }
    if(gVps_captLmObj.recvDescCount)
    {
        Vps_printf(" Recv descriptor Info, \r\n");

        numIdx = gVps_captLmObj.recvDescCount;
        if(numIdx>VPS_CAPT_LOG_INFO_MAX)
            numIdx = VPS_CAPT_LOG_INFO_MAX;

        for(idx=0; idx<numIdx; idx++)
        {
            pDescInfo = &gVps_captLmObj.recvDescInfo[idx];

            Vps_printf(" %3d: %8d ms: VPDMA CH %3d %3dx%3d, @ 0x%08x\r\n",
                idx,
                pDescInfo->timestamp,
                pDescInfo->vChannel,
                pDescInfo->width,
                pDescInfo->height,
                pDescInfo->addr
            );
        }
        Vps_printf(" \r\n");
    }

    Vps_captLmWriteHwQueToFile();
    Vps_captLmPrintRecvDesc();

    #endif

    VpsHal_vipPrintDebugInfo();


    return 0;
}

#ifdef VPS_CAPT_LIST_ENABLE_ADV_STATISTICS_LOG
Int32 Vps_captLmWriteHwQueToFile()
{
    FILE *fp;
    UInt32 size, i;
    static UInt32 tmpBuf[100*1024];
    UInt8 *curAddr;

    size = gVps_captLmObj.hwQueObj.hwQueEndAddr - gVps_captLmObj.hwQueObj.hwQueStartAddr;

    Vps_printf(" %d: CAPTLM: Writing HW Que mem 0x%08x .. 0x%08x to file (%d bytes) ...\r\n",
        Clock_getTicks(),
        gVps_captLmObj.hwQueObj.hwQueStartAddr,
        gVps_captLmObj.hwQueObj.hwQueEndAddr,
        size
        );

    GT_assert( GT_DEFAULT_MASK, size<sizeof(tmpBuf));

    fp = fopen("HWQUE.BIN","wb");
    GT_assert( GT_DEFAULT_MASK, fp!=NULL);

    curAddr = gVps_captLmObj.hwQueObj.hwQueStartAddr;
    for(i=0; i<size; i+=4)
    {
        tmpBuf[i/4] = (curAddr[i] << 24) | (curAddr[i+1] << 16) | (curAddr[i+2] << 8) | (curAddr[i+3] << 0);
    }

    fwrite(tmpBuf, 1, size, fp);

    fclose(fp);

    Vps_printf(" %d: CAPTLM: File write Done !!!\r\n",
        Clock_getTicks()
    );

    return 0;
}
#endif
