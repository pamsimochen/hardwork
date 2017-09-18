/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "nsfLink_priv.h"


Int32 NsfLink_drvCallback (FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    NsfLink_Obj *pObj = (NsfLink_Obj *)appData;

    Semaphore_post(pObj->complete);

    return FVID2_SOK;
}


Int32 NsfLink_drvFvidErrCb(FVID2_Handle handle, Ptr appData,
        Ptr errList, Ptr reserved)
{
    #ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Error Callback !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}


Int32 NsfLink_drvCreate(NsfLink_Obj *pObj, NsfLink_CreateParams *pPrm)
{
    Int32 status;
    UInt32 queId;
    Semaphore_Params semParams;
    FVID2_CbParams cbParams;

    #ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Create in progress !!!\n", Clock_getTicks());
    #endif

    /* Store the create params coming from the app */
    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));


    pObj->inFrameReceiveCount       = 0;
    pObj->inFrameGivenCount         = 0;
    pObj->outFrameReceiveCount      = 0;
    pObj->outFrameGivenCount        = 0;
    pObj->processFrameReqCount      = 0;
    pObj->getProcessFrameReqCount   = 0;
    pObj->getFrames                 = 0;
    pObj->totalTime                 = 0;
    pObj->curTime                   = 0;
    pObj->curEvenFieldFrame         = 0;

    /* Info about the NSF link */
    pObj->info.numQue = NSF_LINK_MAX_OUT_QUE;
    if(pObj->createArgs.enableEvenFieldOutput)
        pObj->info.numQue++;

    for(queId = 0; queId < NSF_LINK_MAX_OUT_QUE; queId++)
    {
        status = VpsUtils_bufCreate(&pObj->bufOutQue[queId], FALSE, FALSE);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        pObj->info.queInfo[queId].numCh = 0;
    }

    status = VpsUtils_bufCreate(&pObj->bufEvenFieldOutQue, FALSE, FALSE);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->info.queInfo[2].numCh = 0;


    /* Copy previous link (capture) info */
    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert( GT_DEFAULT_MASK, pObj->pInTskInfo != NULL);
    GT_assert( GT_DEFAULT_MASK, pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    /* Copy previous link's queue info */
    memcpy(&pObj->inQueInfo,
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo));

    /* Create semaphores */
    Semaphore_Params_init (&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->complete = Semaphore_create (0u, &semParams, NULL);
    GT_assert( GT_DEFAULT_MASK, pObj->complete != NULL);

    /*
     * Configure the channels, allocate frames for each channel and
     * put those frames in channel specific buffer queue.
     */
    NsfLink_drvInitCh(pObj);

    /* Callback functions */
    memset(&cbParams, 0, sizeof(cbParams));

    cbParams.cbFxn = NsfLink_drvCallback;
    cbParams.appData = pObj;
    cbParams.errCbFxn = NsfLink_drvFvidErrCb;
    cbParams.errList = &pObj->errCbProcessList;

    /* Create NSF handle */
    pObj->fvidHandleNsf = FVID2_create(
                                        FVID2_VPS_M2M_NSF_DRV,
                                        VPS_M2M_INST_NF0,
                                        &pObj->nsfCreateParams,
                                        &pObj->nsfCreateStatus,
                                        &cbParams);
    GT_assert( GT_DEFAULT_MASK, pObj->fvidHandleNsf != NULL);

    #ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Create Done !!!\n", Clock_getTicks());
    #endif

    return status;
}


/* This function gets called when capture driver has captured frames.
 * This function takes those frames from the capture driver and stores them
 * in the channel specific buf-queue for noise filtering stage.
 */
Int32 NsfLink_drvProcessData(NsfLink_Obj *pObj)
{
    UInt32 frameId;
    FVID2_FrameList frameList;
    FVID2_Frame *pFrame;
    System_LinkInQueParams *pInQueParams;
    Int32 status;

    /* Pointer to the input link's queue */
    pInQueParams = &pObj->createArgs.inQueParams;

    /* Get the captured frames from the capture driver */
    System_getLinksFullFrames(pInQueParams->prevLinkId,
            pInQueParams->prevLinkQueId, &frameList);

    if(frameList.numFrames)
    {
        pObj->inFrameReceiveCount += frameList.numFrames;

        /* For each captured frame, check the channel no and
         * copy it accordingly in the fullQueue.
         */
        for(frameId = 0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            GT_assert( GT_DEFAULT_MASK, pFrame->channelNum < pObj->nsfCreateParams.numCh);

            status = VpsUtils_bufPutFullFrame(&pObj->linkChInfo[pFrame->channelNum].bufInQue, pFrame);
            GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
        }
    }

    do
    {
        status = NsfLink_drvDoNsfFilter(pObj);
    } while(status==FVID2_SOK);

    return FVID2_SOK;
}


Int32 NsfLink_drvDoNsfFilter(NsfLink_Obj *pObj)
{
    static FVID2_FrameList inFrameList[2], outFrameList;
    FVID2_ProcessList processList;
    FVID2_Frame *pFullFrame;
    FVID2_Frame *pEmptyFrame;
    Int32 status;
    UInt32 chId, frameId, queueId = 0, chPerQueue;
    FVID2_Frame *pFrame;
    UInt32 sendMsgToTsk = 0;
    UInt32 perChCount, numFrames;

    /* Initialize the process list with different frame lists */
    processList.inFrameList[0] = &inFrameList[0];
    processList.inFrameList[1] = &inFrameList[1];
    processList.outFrameList[0] = &outFrameList;

    processList.numInLists = 2;
    processList.numOutLists = 1;

    processList.drvData = NULL;
    processList.reserved = NULL;

    numFrames = 0;

    pObj->getFrames++;

    /* For all the available channels, look into the respective
     * bufInQue.fullQueue and take the frames out of that queue and add them
     * to the inFrameList.
     * Take the same number of frames from the respective bufInQue.emptyQueue
     * and add them to the outFrameList.
     * This process will make the desired processList, ready for noise
     * filtering.
     */
    for (chId = 0 ; chId < pObj->nsfCreateParams.numCh ; chId++)
    {
        /* While there are captured frames... and less than what NSF could
         * consume per request per channel. */
        perChCount = 0u;
        while (1)
        {
            status = VpsUtils_bufGetFullFrame(&pObj->linkChInfo[chId].bufInQue,
                        &pFullFrame, BIOS_NO_WAIT);
            if(status!=FVID2_SOK)
                break;

            /* First check whether it can be accomodated or not */
            status = VpsUtils_bufGetEmptyFrame(&pObj->linkChInfo[chId].bufInQue,
                &pEmptyFrame, BIOS_WAIT_FOREVER);

            GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

            /* Add both the frames in the respective frame lists */
            inFrameList[0].frames[numFrames] = pFullFrame;
            inFrameList[1].frames[numFrames] = pObj->linkChInfo[chId].pPrevOutFrame;
            outFrameList.frames[numFrames] = pEmptyFrame;

            /* next previous output frame is current output frame */
            pObj->linkChInfo[chId].pPrevOutFrame = pEmptyFrame;

            numFrames++;
            perChCount++;
            if (perChCount > 0 /*pObj->nsfCreateStatus.maxFramesPerChInQueue*/)
                break;
        }
    }

    pObj->getFrames--;

    inFrameList[0].numFrames = numFrames;
    inFrameList[1].numFrames = numFrames;
    outFrameList.numFrames = numFrames;

    /* Start noise filtering */
    if(inFrameList[0].numFrames)
    {
        #ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: Noise Filtering %d frames !!!\n", Clock_getTicks(),
            inFrameList[0].numFrames);
        #endif

        pObj->processFrameReqCount++;

        pObj->curTime = Clock_getTicks();

        status = FVID2_processFrames(pObj->fvidHandleNsf, &processList);
        GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK);

        Semaphore_pend(pObj->complete, BIOS_WAIT_FOREVER);

        status = FVID2_getProcessedFrames(pObj->fvidHandleNsf, &processList,
            BIOS_NO_WAIT);
        GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK);

        pObj->curTime = Clock_getTicks() - pObj->curTime ;

        pObj->totalTime += pObj->curTime;

        pObj->getProcessFrameReqCount++;

        #ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: Noise Filtering of %d frames Done !!!\n",
            Clock_getTicks(), inFrameList[0].numFrames);
        #endif

        /* Put the processed frames in the appropriate output queue */
        for(frameId = 0; frameId < outFrameList.numFrames; frameId++)
        {
            pFrame = outFrameList.frames[frameId];

            if(pObj->createArgs.enableEvenFieldOutput)
            {
                /* if enabled, queue the even field to another link */
                if(pFrame->fid==0)
                {
                    FVID2_Frame *pEvenFieldFrame;

                    pEvenFieldFrame = &pObj->evenFieldFrames[pObj->curEvenFieldFrame];

                    pObj->curEvenFieldFrame = (pObj->curEvenFieldFrame+1)%(NSF_LINK_FRAMES_PER_CH*NSF_LINK_MAX_CH_PER_QUE);

                    memcpy(pEvenFieldFrame, pFrame, sizeof(*pFrame));

                    status = VpsUtils_bufPutFullFrame(&pObj->bufEvenFieldOutQue, pEvenFieldFrame);

                    /* in case of error ignore this frame */
                    if(status==FVID2_SOK)
                    {
                        sendMsgToTsk |= (1 << 2);
                    }
                }
            }

            /* Split the frames into both the output queues,
             * if they are enabled. Else use output queue 0 only.
             * Also, if output queue 1 is used, frames sent to this queue
             * should be modified before submitting so that the
             * pFrame->channelNum should start with 0 and not with
             * (pObj->nsfCreateParams.numCh / 2).
             */
            chPerQueue =
                (pObj->nsfCreateParams.numCh / pObj->createArgs.numOutQue);
            queueId = (pFrame->channelNum / chPerQueue);
            pFrame->channelNum = pFrame->channelNum % chPerQueue;

            /* Set the flag whether queue is filled or not */
            sendMsgToTsk |= (1 << queueId);

            status = VpsUtils_bufPutFullFrame(&pObj->bufOutQue[queueId], pFrame);
            GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK);
        }

        pObj->outFrameGivenCount += outFrameList.numFrames;

        if(pObj->createArgs.enableEvenFieldOutput)
        {
            if(sendMsgToTsk & (1<<2))
            {
                /* Send data available message to next task  */
                System_sendLinkCmd(pObj->createArgs.enableEvenFieldOutputQueParams.nextLink,
                    SYSTEM_CMD_NEW_DATA);
            }
        }

        /* Send new data available command to the next link */
        for(queueId = 0; queueId < NSF_LINK_MAX_OUT_QUE; queueId++)
        {
            if(sendMsgToTsk & 0x1)
            {
                /* Send data available message to next task  */
                System_sendLinkCmd(pObj->createArgs.outQueParams[queueId].nextLink,
                    SYSTEM_CMD_NEW_DATA);
            }

            sendMsgToTsk >>= 1;
            if(sendMsgToTsk == 0)
                break;
        }

        pObj->inFrameGivenCount += inFrameList[0].numFrames;

        /* Return frames back to the capture link as well */
        System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
                                 pObj->createArgs.inQueParams.prevLinkQueId,
                                 &inFrameList[0]);


        status = FVID2_SOK;
    }
    else
    {
        #ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: No frames available for filtering !!!\n",
            Clock_getTicks());
        #endif

        /* no more frame availble so exit the loop outside of this function */
        status = FVID2_EFAIL;
    }

    return status;
}


/* This function will be called from the next link in the chain
 * when the frames are no more required. This function restores
 * channel number for each frame from the appData and pushes the frame
 * back to the channel specific bufInQue.emptyQueue.
 */
Int32 NsfLink_drvPutEmptyFrames(NsfLink_Obj *pObj,
                        FVID2_FrameList *pFrameList)
{
    Int32 status;
    UInt32 frameId;
    FVID2_Frame *pFrame;
    System_FrameInfo *pFrameInfo;

    pObj->outFrameReceiveCount += pFrameList->numFrames;
    if(pFrameList->numFrames)
    {
        for(frameId = 0; frameId < pFrameList->numFrames; frameId++)
        {
            pFrame = pFrameList->frames[frameId];

            /* Channel number might have changed, restore it with the
             * orignial stored value.
             */
            pFrameInfo = (System_FrameInfo*)pFrame->appData;
            GT_assert( GT_DEFAULT_MASK, pFrameInfo != NULL);
            pFrame->channelNum = pFrameInfo->nsfChannelNum;

            GT_assert( GT_DEFAULT_MASK, pFrame->channelNum < pObj->nsfCreateParams.numCh);

            /* Return each frame to its original channel specific
             * NsfLink_ChObj.bufInQue.emptyQue.
             */
            status = VpsUtils_bufPutEmptyFrame(&pObj->linkChInfo[pFrame->channelNum].bufInQue,
                pFrame);
            GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK);
        }

        #ifdef SYSTEM_DEBUG_NSF_RT
        Vps_printf(" %d: NSF: Returned %d frames to NF channels !!!\n",
            Clock_getTicks(), pFrameList->numFrames);
        #endif
    }

    return FVID2_SOK;
}

Int32 NsfLink_drvDelete(NsfLink_Obj *pObj)
{
    UInt32 queId, chId;
    NsfLink_ChObj *nsfChObj;

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: NSF    : Fields = %d (fps = %d) !!!\n",
        Clock_getTicks(),
        pObj->outFrameGivenCount,
        pObj->outFrameGivenCount*100/(pObj->totalTime/10)
        );
    #endif

    #ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Delete in progress !!!\n", Clock_getTicks());
    #endif

    /* NSF handle */
    FVID2_delete(pObj->fvidHandleNsf, NULL);

    /* Free the allocated frames */
    NsfLink_drvFreeFrames(pObj);

    /* Free the NSF link output queues */
    for(queId = 0; queId < NSF_LINK_MAX_OUT_QUE; queId++)
    {
        VpsUtils_bufDelete(&pObj->bufOutQue[queId]);
    }

    VpsUtils_bufDelete(&pObj->bufEvenFieldOutQue);

    /* Free the channel specific buf-queue */
    for(chId = 0 ; chId < pObj->nsfCreateParams.numCh ; chId++)
    {
        nsfChObj = &pObj->linkChInfo[chId];
        VpsUtils_bufDelete(&nsfChObj->bufInQue);
    }

    /* Delete semaphores */
    Semaphore_delete(&pObj->complete);

    #ifdef SYSTEM_DEBUG_NSF
    Vps_printf(" %d: NSF: Delete Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}


Int32 NsfLink_drvInitCh(NsfLink_Obj *pObj)
{
    Int32 status;
    UInt32 i, chId, frameId, queueId, outChId, chPerQueue;
    System_LinkChInfo *pInChInfo;
    Vps_NsfDataFormat *nsfDataFmt;
    Vps_NsfProcessingCfg *nsfProcCfg;
    FVID2_Frame *frames;
    FVID2_Format format;
    NsfLink_ChObj *nsfChObj;

    GT_assert( GT_DEFAULT_MASK, pObj != NULL);

    /* Fill the nsfCreateParams structure */
    /* Find no of channels from input (capture) queue */
    pObj->nsfCreateParams.numCh = pObj->inQueInfo.numCh;

    /*
     * Point to memory for data format structure for each channel
     */
    pObj->nsfCreateParams.dataFormat = &pObj->nsfDataFormat[0];

    /*
     * Point to memory for processing params structure for each channel
     */
    pObj->nsfCreateParams.processingCfg = &pObj->nsfProcCfg[0];

    /*
     * For each channel, do the initialization and
     * FVID2 frame / buffer allocation.
     */
    for (chId = 0 ; chId < pObj->nsfCreateParams.numCh ; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];
        nsfDataFmt = &pObj->nsfDataFormat[chId];
        nsfProcCfg = &pObj->nsfProcCfg[chId];
        nsfChObj = &pObj->linkChInfo[chId];

        /* Set the channel number first */
        nsfChObj->channelId = chId;

        /* Initialize the createParams.dataFormat */
        nsfDataFmt->channelNum = chId;
        nsfDataFmt->inMemType = pInChInfo->memType;

        if(pObj->createArgs.tilerEnable)
            nsfDataFmt->outMemType = VPS_VPDMA_MT_TILEDMEM;
        else
            nsfDataFmt->outMemType = VPS_VPDMA_MT_NONTILEDMEM;

        nsfDataFmt->inDataFormat = pInChInfo->dataFormat;
        nsfDataFmt->inFrameWidth = pInChInfo->width;
        nsfDataFmt->inFrameHeight = pInChInfo->height;

        nsfDataFmt->inPitch = pInChInfo->pitch[0];
        /* Only one output format is supported */
        nsfDataFmt->outDataFormat = FVID2_DF_YUV420SP_UV;

        if (nsfDataFmt->outMemType == VPS_VPDMA_MT_TILEDMEM)
        {
            nsfDataFmt->outPitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
            nsfDataFmt->outPitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
        }
        else
        {
            nsfDataFmt->outPitch[0] =
                VpsUtils_align (pInChInfo->width, VPS_BUFFER_ALIGNMENT*2);
            nsfDataFmt->outPitch[1] =
                VpsUtils_align (pInChInfo->width, VPS_BUFFER_ALIGNMENT*2);
        }
        nsfDataFmt->outPitch[2] = 0;

        /* Initialize the createParams.processingCfg */
        nsfProcCfg->channelNum = chId;
        /* TBD: Check later on */
        nsfProcCfg->bypassMode = (pObj->createArgs.bypassNsf ?
            VPS_NSF_DISABLE_SNF_TNF : VPS_NSF_DISABLE_NONE);
        nsfProcCfg->frameNoiseAutoCalcEnable = TRUE;
        nsfProcCfg->frameNoiseCalcReset = FALSE;
        nsfProcCfg->subFrameModeEnable = FALSE;
        nsfProcCfg->numLinesPerSubFrame = 128;

        for (i = 0; i < 3; i++)
        {
            nsfProcCfg->staticFrameNoise[i] = 0;
            nsfProcCfg->spatialStrengthLow[i] = VPS_NSF_PROCESSING_CFG_DEFAULT;
            nsfProcCfg->spatialStrengthHigh[i] = VPS_NSF_PROCESSING_CFG_DEFAULT;
        }

        nsfProcCfg->temporalStrength = VPS_NSF_PROCESSING_CFG_DEFAULT;
        nsfProcCfg->temporalTriggerNoise = VPS_NSF_PROCESSING_CFG_DEFAULT;
        nsfProcCfg->noiseIirCoeff = VPS_NSF_PROCESSING_CFG_DEFAULT;
        nsfProcCfg->maxNoise = VPS_NSF_PROCESSING_CFG_DEFAULT;
        nsfProcCfg->pureBlackThres = VPS_NSF_PROCESSING_CFG_DEFAULT;
        nsfProcCfg->pureWhiteThres = VPS_NSF_PROCESSING_CFG_DEFAULT;

        /*
         * Per channel bufInQue structure needs to be created & initialized.
         * bufInQue.fullQue will be populated with captured frames and
         * bufInQue.emptyQuewill be allocated here so that they can be
         * used later on for noise filtering.
         * Frames need to be allocated for bufInQue.emptyQue here.
         */
        /* Create the per channel bufInQue */
        status = VpsUtils_bufCreate(&nsfChObj->bufInQue, TRUE, FALSE);
        GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK);

        /* Fill format with channel specific values to allocate frame */
        format.channelNum = chId;
        format.width = pInChInfo->width;
        format.height = VpsUtils_align(pInChInfo->height, VPS_BUFFER_ALIGNMENT*2);
        format.pitch[0] = nsfDataFmt->outPitch[0];
        format.pitch[1] = nsfDataFmt->outPitch[1];
        format.pitch[2] = nsfDataFmt->outPitch[2];
        format.fieldMerged[0] = FALSE;
        format.fieldMerged[1] = FALSE;
        format.fieldMerged[2] = FALSE;
        format.dataFormat = FVID2_DF_YUV420SP_UV;
        format.scanFormat = FVID2_SF_PROGRESSIVE;
        format.bpp = FVID2_BPP_BITS8; /* ignored */

        /*
         * Alloc memory based on 'format'
         * Allocated frame info is put in frames[]
         * NSF_LINK_FRAMES_PER_CH is the number of frames per channel to
         * allocate.
         */
        if (nsfDataFmt->outMemType == VPS_VPDMA_MT_TILEDMEM)
        {
            VpsUtils_tilerFrameAlloc (&format, nsfChObj->frames,
                                       NSF_LINK_FRAMES_PER_CH);
        }
        else
        {
            VpsUtils_memFrameAlloc (&format, nsfChObj->frames,
                                     NSF_LINK_FRAMES_PER_CH);
        }

        /* Set remaining parameters for the allocated frames */
        for (frameId = 0; frameId < NSF_LINK_FRAMES_PER_CH; frameId++)
        {
            /* Point to the frame's array */
            frames = &nsfChObj->frames[frameId];
            frames->perFrameCfg = NULL;
            frames->subFrameInfo = NULL;
            frames->appData = &nsfChObj->frameInfo[frameId];

            nsfChObj->frameInfo[frameId].nsfChannelNum =
                frames->channelNum;

            /* Finally, add this frame to the NsfLink_ChObj.bufInQue.emptyQue */
            status = VpsUtils_bufPutEmptyFrame(&nsfChObj->bufInQue, frames);
            GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK);
        }

        /* make initial previous frame point to first frame
            for first frame of NF this wont be used
        */
        nsfChObj->pPrevOutFrame = &nsfChObj->frames[0];

        /* Populate the remaining fileds of
         * NsfLink_Obj.System_LinkInfo.System_LinkQueInfo.System_LinkChInfo
         * This information will be used by the next link to configure itself
         * properly.
         * Structure used: info.queInfo[].chInfo[]
         * Channels will be divided equally between output queues
         */
        chPerQueue = (pObj->nsfCreateParams.numCh / pObj->createArgs.numOutQue);
        outChId = chId % chPerQueue;
        queueId = chId / chPerQueue;
        GT_assert( GT_DEFAULT_MASK, queueId < pObj->info.numQue);

        pObj->info.queInfo[queueId].chInfo[outChId].dataFormat = FVID2_DF_YUV420SP_UV;
        pObj->info.queInfo[queueId].chInfo[outChId].memType =
            (Vps_VpdmaMemoryType)nsfDataFmt->outMemType;
        pObj->info.queInfo[queueId].chInfo[outChId].width = nsfDataFmt->inFrameWidth;
        pObj->info.queInfo[queueId].chInfo[outChId].height = nsfDataFmt->inFrameHeight;
        pObj->info.queInfo[queueId].chInfo[outChId].pitch[0] = nsfDataFmt->outPitch[0];
        pObj->info.queInfo[queueId].chInfo[outChId].pitch[1] = nsfDataFmt->outPitch[1];
        pObj->info.queInfo[queueId].chInfo[outChId].pitch[2] = nsfDataFmt->outPitch[2];
        pObj->info.queInfo[queueId].chInfo[outChId].scanFormat = pInChInfo->scanFormat;

        /* Increase the number of channels now */
        pObj->info.queInfo[queueId].numCh++;

        /* same applies for the third queue */
        queueId = 2;
        pObj->info.queInfo[queueId].chInfo[chId].dataFormat = FVID2_DF_YUV420SP_UV;
        pObj->info.queInfo[queueId].chInfo[chId].memType =
            (Vps_VpdmaMemoryType)nsfDataFmt->outMemType;
        pObj->info.queInfo[queueId].chInfo[chId].width = nsfDataFmt->inFrameWidth;
        pObj->info.queInfo[queueId].chInfo[chId].height = nsfDataFmt->inFrameHeight;
        pObj->info.queInfo[queueId].chInfo[chId].pitch[0] = nsfDataFmt->outPitch[0];
        pObj->info.queInfo[queueId].chInfo[chId].pitch[1] = nsfDataFmt->outPitch[1];
        pObj->info.queInfo[queueId].chInfo[chId].pitch[2] = nsfDataFmt->outPitch[2];
        pObj->info.queInfo[queueId].chInfo[chId].scanFormat = FVID2_SF_PROGRESSIVE;

        /* Increase the number of channels now */
        pObj->info.queInfo[queueId].numCh++;

    }

    return FVID2_SOK;
}


/*
  Free allocated frames

  pObj - NSF driver information
*/
Int32 NsfLink_drvFreeFrames(NsfLink_Obj *pObj)
{
    UInt16 chId;
    FVID2_Format format;
    UInt32 tilerUsed = FALSE;
    Vps_NsfDataFormat *nsfDataFmt;
    NsfLink_ChObj *nsfChObj;

    for (chId = 0; chId < pObj->nsfCreateParams.numCh; chId++)
    {
        nsfDataFmt = &pObj->nsfDataFormat[chId];
        nsfChObj = &pObj->linkChInfo[chId];

        /* fill format with channel specific values  */
        format.channelNum = chId;
        format.width = nsfDataFmt->inFrameWidth;
        format.height = VpsUtils_align(nsfDataFmt->inFrameHeight, VPS_BUFFER_ALIGNMENT*2);
        format.pitch[0] = nsfDataFmt->outPitch[0];
        format.pitch[1] = nsfDataFmt->outPitch[1];
        format.pitch[2] = nsfDataFmt->outPitch[2];
        format.fieldMerged[0] = FALSE;
        format.fieldMerged[1] = FALSE;
        format.fieldMerged[2] = FALSE;
        format.dataFormat = FVID2_DF_YUV420SP_UV;
        format.scanFormat = FVID2_SF_PROGRESSIVE;
        format.bpp = FVID2_BPP_BITS8; /* ignored */

        if (nsfDataFmt->outMemType == VPS_VPDMA_MT_TILEDMEM)
        {
            /*
             * Cannot free tiled frames
             */
            tilerUsed = TRUE;
        }
        else
        {
            /*
             * Free frames for this channel, based on format
             */
            VpsUtils_memFrameFree (&format, nsfChObj->frames,
                                    NSF_LINK_FRAMES_PER_CH);
        }
    }

    if (tilerUsed)
    {
        VpsUtils_tilerFreeAll ();
    }

    return FVID2_SOK;
}

