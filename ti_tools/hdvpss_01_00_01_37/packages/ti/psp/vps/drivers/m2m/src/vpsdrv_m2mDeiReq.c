/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDeiReq.c
 *
 *  \brief VPS DEI M2M driver internal file used for processing the request.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/drivers/m2m/src/vpsdrv_m2mDeiPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 vpsMdrvDeiUpdateChDesc(VpsMdrv_DeiHandleObj *hObj,
                                    VpsMdrv_DeiChObj *chObj,
                                    FVID2_ProcessList *procList,
                                    UInt32 frmCnt,
                                    UInt32 descSet);
static Int32 vpsMdrvDeiUpdateRtParams(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj,
                                      const Vps_M2mDeiRtParams *rtPrms,
                                      UInt32 descSet);
static Int32 vpsMdrvDeiExtractComprSize(VpsMdrv_DeiHandleObj *hObj,
                                        VpsMdrv_DeiQueueObj *qObj);

static Void vpsMdrvDeiRetErrProcList(VpsMdrv_DeiHandleObj *hObj,
                                     VpsMdrv_DeiQueueObj *qObj);

static Int32 vpsMdrvDeiRotateCtxBuf(VpsMdrv_DeiChObj *chObj, UInt32 inFid);
static Int32 vpsMdrvDeiRotateCtxBufMode1(VpsMdrv_DeiChObj *chObj,
                                         UInt32 inFid,
                                         const FVID2_Frame *frame);
static Int32 vpsMdrvDeiRotateTnrBuf(VpsMdrv_DeiChObj *chObj);

static void vpsMdrvDeiSubFrmUpdate(VpsMdrv_SubFrameChInst *slcData,
                                   FVID2_Frame *inFrm,
                                   FVID2_Frame *outFrm,
                                   Vps_M2mDeiRtParams *rtParams);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          FVID2 API Functions                               */
/* ========================================================================== */

/**
 *  VpsMdrv_deiMlmCbFxn
 */
Int32 VpsMdrv_deiMlmCbFxn(Mlm_SubmitReqInfo *reqInfo)
{
    UInt32                  retVal = FVID2_SOK;
    VpsMdrv_DeiQueueObj    *qObj;
    VpsMdrv_DeiHandleObj   *hObj;
    VpsMdrv_DeiInstObj     *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != reqInfo));
    qObj = reqInfo->cbArg;
    GT_assert(VpsMdrvDeiTrace, (NULL != qObj));
    hObj = qObj->hObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* Check whether the request type is frame or coefficient */
    if (VPSMDRV_QOBJ_TYPE_FRAMES == qObj->qObjType)
    {
        /* Extract the compressed size for the current out frame */
        vpsMdrvDeiExtractComprSize(hObj, qObj);

        /* Free the descriptor set */
        vpsMdrvDeiFreeDescSet(hObj, qObj->descSetInUse);

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
        /* Get the field buffers that can be released to apps */
        retVal = vpsMdrvDeiReleaseAgedCtxBufs(hObj, qObj);
#endif  /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

        if (0u != hObj->numUnProcessedReq)
        {
            hObj->numUnProcessedReq--;
        }

        /*  Queue the completed request to the Done Queue */
        VpsUtils_queue(hObj->doneQ, &qObj->qElem, qObj);
        qObj = NULL;

        /* If callback is registered, intimate the application that a
         * request is completed and is ready to be dequeued */
        if (NULL != hObj->fdmCbPrms.fdmCbFxn)
        {
            hObj->fdmCbPrms.fdmCbFxn(hObj->fdmCbPrms.fdmData, NULL);
        }

        if (hObj->isStopped == FALSE)
        {
            /* Check if any request is pending */
            qObj = VpsUtils_dequeue(hObj->reqQ);
            if (NULL != qObj)
            {
                /* Get a free descriptor to work with */
                qObj->descSetInUse = vpsMdrvDeiAllocDescSet(hObj);
                GT_assert(VpsMdrvDeiTrace,
                    (VPSMDRV_DEI_INVALID_DESC_SET != qObj->descSetInUse));

                /* We have a free descriptor set, update the descriptors,
                 * link the channels and queue it to MLM for processing */
                retVal = vpsMdrvDeiProcessReq(hObj, qObj);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_DEBUG,
                        "Process request failed\n");
                }
            }
        }

        /* If error occurs free objects */
        if (FVID2_SOK != retVal)
        {
            if (NULL != qObj)
            {
                /* Return process list to application using error callback */
                vpsMdrvDeiRetErrProcList(hObj, qObj);

                /* Return back the queue object to the free queue */
                VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);
                qObj = NULL;
            }
        }
    }
    else if (VPSMDRV_QOBJ_TYPE_COEFF == qObj->qObjType)
    {
        /* Callback got for programming of coefficients. */
        /* Signal the semaphore */
        Semaphore_post(hObj->ctrlSem);
    }

    return (0u);
}



/**
 *  vpsMdrvDeiProcessReq
 */
Int32 vpsMdrvDeiProcessReq(VpsMdrv_DeiHandleObj *hObj,
                           VpsMdrv_DeiQueueObj *qObj)
{
    Int32                       retVal = FVID2_SOK;
    Bool                        done[VPS_M2M_DEI_SCALAR_ID_MAX]={FALSE, FALSE};
    UInt32                      descSet, chNum;
    UInt32                      frmCnt, numFrames;
    UInt32                      rldSize, listPostSize;
    UInt32                      scalarId, coreCnt;
    Void                       *descPtr = NULL, *listPostDesc = NULL;
    VpsMdrv_DeiChObj           *chObj;
    VpsMdrv_DeiInstObj         *instObj;
    FVID2_ProcessList          *procList;
    VpsHal_VpdmaReloadDesc     *rldDesc = NULL;
    Vps_M2mDeiRtParams         *rtPrms = NULL;
    /* for subframe processing */
    Vps_FrameParams             deiInFrmPrms;
    Vps_FrameParams             deiOutFrmPrms;
    Vps_CropConfig              deiScCropCfg;
    Vps_ScRtConfig              deiScRtCfg;
    Vps_M2mDeiRtParams          locRtPrms;
    Ptr                         inAddrY, inAddrC, outAddr;
    FVID2_Frame                 *inFrm, *outFrm;
    VpsHal_ScFactorConfig       *curScFactor;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != qObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* Get the numFrames from in frame list */
    procList = &qObj->procList;
    GT_assert(VpsMdrvDeiTrace, (NULL != procList->inFrameList[0u]));
    numFrames = procList->inFrameList[0u]->numFrames;
    descSet = qObj->descSetInUse;

    if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
    {
        rldDesc = NULL;
        listPostDesc = NULL;
        listPostSize = 0u;
    }
    else
    {
        /* Initialize the reload pointer to handle descriptors so that the first
         * channel will be linked to it */
        listPostDesc = hObj->firstDesc[descSet];
        listPostSize = hObj->totalDescMem / VPSMDRV_DEI_MAX_DESC_SET;
        rldDesc = hObj->rldDesc[descSet];
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != rldDesc));
    }

    curScFactor = instObj->curScFactor;

    /* Update the descriptors of the requested channels and link them */
    for (frmCnt = 0u; frmCnt < numFrames; frmCnt++)
    {
        if (NULL == procList->inFrameList[0u]->frames[frmCnt])
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
            retVal = FVID2_EBADARGS;
            break;
        }

        /* Get the channel number from the in frame list's frame pointer */
        chNum = procList->inFrameList[0u]->frames[frmCnt]->channelNum;

        /* Check if the channel number is within the allocated one */
        if (chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
            break;
        }

        /* Get the channel object */
        chObj = hObj->chObjs[chNum];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        /* Update runtime params */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            /* Assign the runtime params if present in any of the core frame */
            rtPrms = (Vps_M2mDeiRtParams *)
                procList->inFrameList[0u]->frames[frmCnt]->perFrameCfg;
        }
        else
        {
            rtPrms = (Vps_M2mDeiRtParams *)
                procList->inFrameList[0u]->perListCfg;
        }


        /*
         * Do SubFrame processing if configured for this channel/frame:
         * 1. Initialise subframe instance memory of channel at first subframe
         *    and keep calling SubFrame pre and post calc functions for
         *    subsequent subFrame;
         * 2. Update rtPrms to configure subframe stuff to Descriptors by Core
         *    functions.
         * 3. Make a copy and update inFrm and outFrm addresses here
         *    considering offset values required for subframe Line memory and
         *    pass to VpsMdrvScUpdateDescs() to be programmed in Desc.
         *    These values are restored after VpsMdrvScUpdateDescs() function
         *    returns.
         *    This is needed as APP expects ProcList with No changes.
         *  NOTE: No support for runtime subframe enable/disable support, can be
         *  added later if needed
         */
        if (TRUE == chObj->slcChObj.subFrameMdEnable)
        {
            inFrm  = procList->inFrameList[0u]->frames[frmCnt];
            outFrm = procList->outFrameList[0u]->frames[frmCnt];

            chObj->slcChObj.subFrameNum = inFrm->subFrameInfo->subFrameNum;
            chObj->slcChObj.slcLnsAvailable =inFrm->subFrameInfo->numInLines;

            if (chObj->slcChObj.subFrameNum == 0u)
            {
               vpsMdrvScSubFrameInitInfo(&chObj->slcChObj);
               outFrm->subFrameInfo->numOutLines = 0u;
            }
            vpsMdrvScSubFrameCalcInfo(&chObj->slcChObj);
            if (rtPrms == NULL)
            {
               /* use locally allocated rtPrms memory
                * TODO: To avoid run-time cycles penalty in allocating and
                * assigning the values/ check for NULL in all RtPrms sub
                * structures,
                * Allocate this as global static variable and use across all
                * channels as this is just needed locally and all values are
                * updated for every call */
               locRtPrms.deiInFrmPrms = &deiInFrmPrms;
               locRtPrms.deiOutFrmPrms = &deiOutFrmPrms;
               locRtPrms.deiScCropCfg = &deiScCropCfg;
               locRtPrms.deiScRtCfg = &deiScRtCfg;
               locRtPrms.vipOutFrmPrms = NULL;
               locRtPrms.vipScCropCfg = NULL;
               locRtPrms.vipScRtCfg = NULL;
               locRtPrms.deiRtCfg = NULL;
               rtPrms = &locRtPrms;
               /* initialise non subframe specific params in rtParams */
               deiInFrmPrms.memType = chObj->chPrms.inMemType;
               deiInFrmPrms.dataFormat = chObj->chPrms.inFmt.dataFormat;
               deiOutFrmPrms.memType = chObj->chPrms.outMemTypeDei;
               deiOutFrmPrms.dataFormat = chObj->chPrms.outFmtDei->dataFormat;
            }
            else
            {
               /* TODO: frame level rtPrms change support in subframe mode*/
            }

            /* make local copy of inFrm and outFrm address */
            inAddrY =
                inFrm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX];
            inAddrC =
                inFrm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX];
            outAddr =
                outFrm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX];

            vpsMdrvDeiSubFrmUpdate(
                &chObj->slcChObj,
                inFrm,
                outFrm,
                rtPrms);
        }

        /* Update RT params if they have been provided in this process frames
         * call, OR if the descriptor set is dirty due to RT params being
         * previously provided, and now needs to be updated. */
        if ((NULL != rtPrms) || (TRUE == chObj->isDescDirty[descSet]))
        {
            retVal = vpsMdrvDeiUpdateRtParams(hObj, chObj, rtPrms, descSet);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not update runtime params for channel %d!\n", chNum);
                break;
            }
        }

        /* Update channel descriptors */
        retVal = vpsMdrvDeiUpdateChDesc(hObj, chObj, procList, frmCnt, descSet);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Update of channel descriptor failed for channel %d\n", chNum);
            break;
        }

        /* If Lazy Loading is enabled for any scalar and the scalar is not in
         * bypass, convert the dummy descriptors to coeff descriptors if
         * required. */
        for (scalarId = 0u; scalarId < VPS_M2M_DEI_SCALAR_ID_MAX; scalarId++)
        {
            if (    (TRUE == instObj->enableLazyLoading[scalarId])
                &&  (TRUE != done[scalarId])
                &&  (VPS_SC_HST_MAX != chObj->scFactor[scalarId].hsType)
                &&  (VPS_SC_VST_MAX != chObj->scFactor[scalarId].vsType))
            {
                coreCnt = instObj->scalarCoreId[scalarId];
                /* Check if the scalar is enabled. */
                if (VPSMDRV_DEI_MAX_CORE != coreCnt)
                {
                    /* Check if horizontal scaling coeff desc has to be
                     * created. */
                    if (    (   curScFactor[scalarId].hsType
                             != chObj->scFactor[scalarId].hsType)
                        ||  (   curScFactor[scalarId].hScalingSet
                             != chObj->scFactor[scalarId].hScalingSet))
                    {
                        VpsHal_vpdmaCreateConfigDesc(
                            chObj->coeffDesc
                                [descSet]
                                [  VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC
                                 * scalarId],
                            chObj->coeffConfigDest[coreCnt],
                            VPSHAL_VPDMA_CPT_BLOCK_SET,
                            VPSHAL_VPDMA_CCT_INDIRECT,
                            chObj->horzCoeffOvlySize[coreCnt],
                            chObj->horzCoeffMemPtr[coreCnt],
                            0u,
                            chObj->horzCoeffOvlySize[coreCnt]);

                        /* Now set the current hsType and hScalingSet to that of
                         * this channel.
                         */
                        instObj->curScFactor[scalarId].hsType =
                            chObj->scFactor[scalarId].hsType;
                        instObj->curScFactor[scalarId].hScalingSet =
                            chObj->scFactor[scalarId].hScalingSet;
                    }
                    else /* Create dummy descriptor */
                    {
                        VpsHal_vpdmaCreateDummyDesc(
                            chObj->coeffDesc
                                [descSet]
                                [  VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC
                                 * scalarId]);
                    }

                    /* Check if vertical scaling coeff desc has to be
                     * created. */
                    if (    (   curScFactor[scalarId].vsType
                             != chObj->scFactor[scalarId].vsType)
                        ||  (   curScFactor[scalarId].vScalingSet
                             != chObj->scFactor[scalarId].vScalingSet))
                    {
                        VpsHal_vpdmaCreateConfigDesc(
                            chObj->coeffDesc
                                [descSet]
                                [(  VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC
                                 * scalarId) + 1u],
                            chObj->coeffConfigDest[coreCnt],
                            VPSHAL_VPDMA_CPT_BLOCK_SET,
                            VPSHAL_VPDMA_CCT_INDIRECT,
                            chObj->vertCoeffOvlySize[coreCnt],
                            chObj->vertCoeffMemPtr[coreCnt],
                            (Ptr) (chObj->horzCoeffOvlySize[coreCnt]),
                            chObj->vertCoeffOvlySize[coreCnt]);

                        /* Now set the current vsType and vScalingSet to that of
                         * this channel. */
                        instObj->curScFactor[scalarId].vsType =
                            chObj->scFactor[scalarId].vsType;
                        instObj->curScFactor[scalarId].vScalingSet =
                            chObj->scFactor[scalarId].vScalingSet;
                    }
                    else /* Create dummy descriptor */
                    {
                        VpsHal_vpdmaCreateDummyDesc(
                            chObj->coeffDesc
                                [descSet]
                                [(  VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC
                                 * scalarId) + 1u]);
                    }

                    /* For per-handle mode, the coeff descriptors need to be
                     * programmed only once. */
                    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
                    {
                        done[scalarId] = TRUE;
                    }
                }
            }
        }

        /* Get the descriptor address and size to which the previous reload
         * descriptor should be linked */
        descPtr = chObj->firstDesc[descSet];

        /* Get the reload size. */
        if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
        {
            /* For per-handle mode, no coeff descriptors are included in the
             * channel reload size. */
            rldSize = (chObj->totalDescMem / VPSMDRV_DEI_MAX_DESC_SET);
        }
        else
        {
            /* Reload size to be used depends on whether Lazy Loading is enabled
             * for the scalar.
             * If Lazy Loading is not enabled for one or more scalars in the
             * path, the firstDesc points to the first valid coeff descriptor,
             * or the config descriptor. The config descriptor is the next
             * descriptor after all the coeff descriptors. Hence, to calculate
             * the reload size, the non-valid descriptor size is calculated as
             * the firstDesc minus the very first coeff descriptor. This
             * non-valid descriptor size is subtracted from the total descriptor
             * size for one descSet to get the reload size.
             */
            GT_assert(VpsMdrvDeiTrace,
                (descPtr >= chObj->coeffDesc[descSet][0]));
            rldSize = (chObj->totalDescMem / VPSMDRV_DEI_MAX_DESC_SET)
                - (((UInt32) descPtr) -
                   ((UInt32) chObj->coeffDesc[descSet][0]));
        }

        if (rldDesc != NULL)
        {
            /* Link the reload descriptor of previous channel to the descriptor
             * of this channel */
            VpsHal_vpdmaCreateRLCtrlDesc(rldDesc, descPtr, rldSize);
        }
        else
        {
            /* Per handle configuration, store the first channel's start addr */
            listPostDesc = descPtr;
            listPostSize = rldSize;
        }

        /* Store the reload descriptor of this channel, so that next channel
         * will link to it */
        rldDesc = chObj->rldDesc[descSet];
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != rldDesc));

        /*** Do subframe specific post processing ****/
        if (chObj->slcChObj.subFrameMdEnable == TRUE)
        {
            outFrm->subFrameInfo->subFrameNum = chObj->slcChObj.subFrameNum;
            outFrm->subFrameInfo->numOutLines += chObj->slcChObj.tarH;

            vpsMdrvScSubFrameCalcPostProcInfo(&chObj->slcChObj);
            /* restore inFrm and outFrm addresses to pass processlist
                          intact to application */
            inFrm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX]
                = inAddrY;
            inFrm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX]
                = inAddrC;
            outFrm->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX]
                = outAddr;
        }

    }

    if (FVID2_SOK == retVal)
    {
        /*
         * Update the MLM parameters and submit the request to MLM
         */
        GT_assert(VpsMdrvDeiTrace,
            (hObj->numVpdmaChannels <= MLM_MAX_CHANNELS));
        GT_assert(VpsMdrvDeiTrace, (NULL != listPostDesc));
        GT_assert(VpsMdrvDeiTrace, (NULL != rldDesc));

        /* Always handle descriptor is the start */
        qObj->mlmReqInfo.reqStartAddr = listPostDesc;
        qObj->mlmReqInfo.firstChannelSize = listPostSize;
        /* Last channel reload descriptor */
        qObj->mlmReqInfo.lastRlDescAddr = rldDesc;
        /* We have already put SOCH for each channel in the driver,
         * so tell MLM not to put again */
        qObj->mlmReqInfo.numChannels = 0u;
        qObj->mlmReqInfo.submitReqType = MLM_SRT_NO_SOC;

        qObj->mlmReqInfo.cbArg = qObj;
        retVal = Mlm_submit(instObj->mlmHandle, &qObj->mlmReqInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Submission to MLM failed\n");
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiAllocDescSet
 */
UInt32 vpsMdrvDeiAllocDescSet(VpsMdrv_DeiHandleObj *hObj)
{
    UInt32      allocedDescSet = VPSMDRV_DEI_INVALID_DESC_SET;
    UInt32      descSet;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));

    for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
    {
        if (TRUE == hObj->isDescSetFree[descSet])
        {
            /* Set flag as alloced */
            hObj->isDescSetFree[descSet] = FALSE;
            allocedDescSet = descSet;
            break;
        }
    }

    return (allocedDescSet);
}



/**
 *  vpsMdrvDeiFreeDescSet
 */
Int32 vpsMdrvDeiFreeDescSet(VpsMdrv_DeiHandleObj *hObj, UInt32 descSet)
{
    Int32       retVal = FVID2_SOK;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    /* Assert if index is more than the index */
    GT_assert(VpsMdrvDeiTrace, (descSet < VPSMDRV_DEI_MAX_DESC_SET));

    /* Set flag as free */
    hObj->isDescSetFree[descSet] = TRUE;

    return (retVal);
}



/**
 * VpsMdrvDeiGetScFactorConfig
 * \brief Gets the Scaling Factor Configuration for all channels.
 */
Int32 VpsMdrvDeiGetScFactorConfig(VpsMdrv_DeiHandleObj *hObj,
                                  UInt32 scalarId,
                                  UInt32 chNum,
                                  UInt32 descSet)
{
    Int32 retVal = FVID2_SOK;
    const Vcore_Ops *coreOps = NULL;
    UInt32 numChannels, chCnt, coreCnt, descSetCnt;
    VpsMdrv_DeiChObj *chObj;
    VpsMdrv_DeiInstObj *instObj;
    Vcore_ScCoeffParams coeffPrms;

    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* Get the core to which the scalar ID matches */
    coreCnt = instObj->scalarCoreId[scalarId];
    GT_assert(VpsMdrvDeiTrace, (VPSMDRV_DEI_MAX_CORE != coreCnt));
    coreOps = instObj->coreOps[coreCnt];
    /* Check if the scalar ID is valid */
    if (NULL == coreOps)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid Scalar ID\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Ensure that the core function is valid. */
        GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getScFactorConfig));

        /* Call the core function to get the scalar config for all channels. */
        if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
        {
            /* If the configuration is applied per handle, only the config in
             * channel 0 is valid.
             */
            numChannels = 1u;
        }
        else
        {
            /* Get the scalar config for all the channels if the configuration
             * is different for all the channels of the handle
             */
            numChannels = hObj->numCh;
        }

        /* A value of -1 for descSet means all descriptor sets must be
         * processed.
         */
        if ((UInt32) -1 == descSet)
        {
            descSetCnt = 0u;
        }
        else
        {
            descSetCnt = descSet;
        }

        for (; descSetCnt < VPSMDRV_DEI_MAX_DESC_SET; descSetCnt++)
        {
            /* A value of -1 for chNum means all channels should be processed.
             */
            if ((UInt32) -1 == chNum)
            {
                chCnt = 0u;
            }
            else
            {
                chCnt = chNum;
            }

            for (; chCnt < numChannels; chCnt++)
            {
                chObj = hObj->chObjs[chCnt];
                GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
                retVal = coreOps->getScFactorConfig(
                    hObj->coreHandle[coreCnt],
                    chCnt,
                    &(chObj->scFactor[scalarId]),
                    &(chObj->coreDescMem[descSetCnt][coreCnt]));
                if (FVID2_SOK != retVal)
                {
                    GT_1trace(VpsMdrvDeiTrace,
                        GT_ERR,
                        "EFAIL: Getting the Scaling Factor Config Failed:"
                        " ChNum %d\n",
                        chCnt);
                    break;
                }

                if ((UInt32) -1 != chNum)
                {
                    break;
                }
            }

            /* Break the loop on error. */
            if (FVID2_SOK != retVal)
            {
                break;
            }

            /* Break the loop if only the specified descriptor set is to be
             * processed.
             */
            if ((UInt32) -1 != descSet)
            {
                break;
            }
        }

        /* Now call the function to program SC coeff in order to get the
         * pointers to the corresponding coeff memories based on the scaling
         * factors just received.
         * Even if one descriptor set is already under progress, we can still do
         * this to get the new horizontal & vertical scaling coefficient
         * pointers, since we are not changing any actual coefficient memory,
         * but just setting the pointers in the channel object. The existing
         * descriptor set would already be using the correct coefficients as set
         * previously, and that would not be disturbed. Only the next list
         * submission will use the new scalar coefficient set pointers.
         */
        if (FVID2_SOK == retVal)
        {
            /* A value of -1 for chNum means all channels should be processed.
             */
            if ((UInt32) -1 == chNum)
            {
                chCnt = 0u;
            }
            else
            {
                chCnt = chNum;
            }

            for (; chCnt < numChannels; chCnt++)
            {
                chObj = hObj->chObjs[chCnt];
                GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

                /* Call this only if the scaling factors are valid, indicating
                 * that the scalar is not in bypass.
                 */
                if (    (VPS_SC_HST_MAX != chObj->scFactor[scalarId].hsType)
                    &&  (VPS_SC_VST_MAX != chObj->scFactor[scalarId].vsType))
                {
                    /* NULL pointer check */
                    GT_assert(VpsMdrvDeiTrace,
                        (NULL != coreOps->programScCoeff));

                    coeffPrms.hScalingSet =
                        chObj->scFactor[scalarId].hScalingSet;
                    coeffPrms.vScalingSet =
                        chObj->scFactor[scalarId].vScalingSet;
                    coeffPrms.horzCoeffMemPtr = NULL; /* Returned by core */
                    coeffPrms.vertCoeffMemPtr = NULL; /* Returned by core */
                    coeffPrms.vertBilinearCoeffMemPtr = NULL;
                                                      /* Returned by core */
                    coeffPrms.userCoeffPtr = NULL;

                    /* Get and store the pointers to coefficient memory for
                     * this channel.
                     */
                    retVal = coreOps->programScCoeff(
                                 hObj->coreHandle[coreCnt],
                                 0u,
                                 &coeffPrms);
                    if (FVID2_SOK == retVal)
                    {
                        GT_assert(VpsMdrvDeiTrace,
                            (NULL != coeffPrms.horzCoeffMemPtr));
                        chObj->horzCoeffMemPtr[coreCnt] =
                            coeffPrms.horzCoeffMemPtr;
                        GT_assert(VpsMdrvDeiTrace,
                            (NULL != coeffPrms.vertCoeffMemPtr));
                        chObj->vertCoeffMemPtr[coreCnt] =
                            coeffPrms.vertCoeffMemPtr;
                    }
                    else
                    {
                        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                            "Could not get coeff memory pointers\n");
                    }
                }

                if ((UInt32) -1 != chNum)
                {
                    break;
                }
            }
        }
    }

    return retVal;
}



/**
 *  vpsMdrvDeiUpdateChDesc
 */
static Int32 vpsMdrvDeiUpdateChDesc(VpsMdrv_DeiHandleObj *hObj,
                                    VpsMdrv_DeiChObj *chObj,
                                    FVID2_ProcessList *procList,
                                    UInt32 frmCnt,
                                    UInt32 descSet)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  coreCnt;
    UInt32                  frmListIdx;
    UInt32                  fid;
    UInt32                  firstOut = TRUE;
    FVID2_Frame            *frame;
    FVID2_FrameList        *frmList;
    VpsMdrv_DeiInstObj     *instObj;
    const Vcore_Ops        *coreOps;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != procList));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    deiCoreCtxBuf = &chObj->deiCoreCtxBuf;

    frmListIdx = 0u;
    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        /* Program cores only if needed for that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->updateDesc));

            /* Get the frame list pointer and the frame pointer */
            if (coreCnt < VPSMDRV_DEI_MAX_INPUTS)
            {
                /* Get the frame list from in frame list */
                frmList = procList->inFrameList[frmListIdx];
                frmListIdx++;
            }
            else
            {
                /* Reset index for the first out frame list */
                if (TRUE == firstOut)
                {
                    frmListIdx = 0u;
                    firstOut = FALSE;
                }
                /* Get the frame list from out frame list */
                frmList = procList->outFrameList[frmListIdx];
                frmListIdx++;
            }
            GT_assert(VpsMdrvDeiTrace, (NULL != frmList));
            frame = frmList->frames[frmCnt];
            GT_assert(VpsMdrvDeiTrace, (NULL != frame));

            fid = 0u;
            if ((FVID2_FID_BOTTOM == frame->fid) &&
                (coreCnt < VPSMDRV_DEI_MAX_INPUTS))
            {
                /* Use FID only for input frame. Output frame FID should always
                 * be 0. */
                fid = 1u;
            }

            /* In bypass mode FID should be 0 */
            if ((1u == fid) && (FALSE == chObj->deiCoreCtxInfo.isDeinterlacing))
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "FID should be 0 for DEI bypass mode!!\n");
                /* Override and continue processing */
                fid = 0u;
            }

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
            /* In case of TI814x/TI8107, When we do not have enough contexts,
             * current field is treated as context fields. Also the DEI always
             * generates output based on N-1 field. Hence we should use the
             * input FID for N-1 and ^FID for N and N-2 inputs. Otherwise
             * the first frame output will move up/down depending on input
             * frame FID. */
            if ((FALSE == chObj->isCtxFieldInit) &&
                (TRUE == chObj->deiCoreCtxInfo.isDeinterlacing) &&
                (coreCnt < VPSMDRV_DEI_MAX_INPUTS))
            {
                fid = fid ^ 0x1u;
            }
#endif  /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

            /* Update the core descriptors for the FID and buffer address */
            retVal = coreOps->updateDesc(
                         hObj->coreHandle[coreCnt],
                         &chObj->coreDescMem[descSet][coreCnt],
                         frame,
                         fid);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not update desc for core %d!\n", coreCnt);
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        if ((TRUE == chObj->deiCoreCtxInfo.isDeinterlacing) ||
            (TRUE == chObj->deiCoreCtxInfo.isTnrMode))
        {
            /* Check if context buffers are set by application */
            if (TRUE != chObj->isDeiCtxBufSet)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "DEI context buffers are not set for channel %d!\n",
                    chObj->chNum);
                retVal = FVID2_EFAIL;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (TRUE == chObj->deiCoreCtxInfo.isTnrMode)
        {
            retVal = vpsMdrvDeiRotateTnrBuf(chObj);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Error rotating context buffers in TNR mode!\n");
            }
        }
        else if (TRUE == chObj->deiCoreCtxInfo.isDeinterlacing)
        {
            if (VPS_DEIHQ_CTXMODE_APP_N_1 == hObj->deiHqCtxMode)
            {
                frmList = procList->inFrameList[VPSMDRV_DEI_MAX_INPUTS];
                GT_assert(VpsMdrvDeiTrace, (NULL != frmList));
                frame = frmList->frames[frmCnt];
                GT_assert(VpsMdrvDeiTrace, (NULL != frame));

                retVal = vpsMdrvDeiRotateCtxBufMode1(
                             chObj,
                             procList->inFrameList[0u]->frames[frmCnt]->fid,
                             frame);
            }
            else
            {
                coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
                /* NULL pointer check */
                GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
                GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->updateMode));

                /* Note: It is required to work in Line average mode until
                   enough previous fields are captured. After that, DEI will
                   work in configured mode. */
                if (chObj->prevFldState <= chObj->deiCoreCtxInfo.numPrevFld)
                {
                    retVal = coreOps->updateMode(
                            hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                            chObj->chNum,
                            &chObj->coreDescMem[descSet]
                                               [VPSMDRV_DEI_DEI_CORE_IDX],
                            chObj->prevFldState);
                }

                retVal = vpsMdrvDeiRotateCtxBuf(
                             chObj,
                             procList->inFrameList[0u]->frames[frmCnt]->fid);
            }
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Error rotating context buffers in normal DEI mode!\n");
            }
        }
    }

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    if (FVID2_SOK == retVal)
    {
        /* Age the fields even though interlacing is bypassed, as we require
           to release the current feild. This function would mark the current
           field as completed, when de-interlacing is in bypass mode */
        retVal = vpsMdrvDeiAgeCtxBufs(
                    chObj,
                    procList->inFrameList[0u]->frames[frmCnt]);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Error ageing context buffers in normal DEI mode!\n");
        }
    }
#endif  /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

    if (FVID2_SOK == retVal)
    {
        /* Update the context descriptors in deinterlacing mode */
        if ((TRUE == chObj->deiCoreCtxInfo.isDeinterlacing) ||
            (TRUE == chObj->deiCoreCtxInfo.isTnrMode))
        {
            coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->updateContext));

            /* Update the context descriptors */
            retVal = coreOps->updateContext(
                         hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                         chObj->chNum,
                         &chObj->coreDescMem[descSet][VPSMDRV_DEI_DEI_CORE_IDX],
                         deiCoreCtxBuf);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not update DEI context desc!\n");
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiUpdateRtParams
 */
static Int32 vpsMdrvDeiUpdateRtParams(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj,
                                      const Vps_M2mDeiRtParams *rtPrms,
                                      UInt32 descSet)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cnt, scalarId;
    VpsMdrv_DeiInstObj     *instObj;
    const Vcore_Ops        *coreOps;
    Vcore_DeiRtParams       deiRtPrms;
    Vcore_DwpRtParams       dwpRtPrms;
    Vcore_VipRtParams       vipRtPrms;
    Vcore_DescMem          *coreDescMem;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* Update the DEI runtime parameters if DEI is present */
    if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DEI_CORE_IDX])
    {
        coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
        coreDescMem = &chObj->coreDescMem[descSet][VPSMDRV_DEI_DEI_CORE_IDX];
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
        GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->updateRtMem));

        if ((NULL != rtPrms) &&
               ((NULL != rtPrms->deiInFrmPrms) ||
                (NULL != rtPrms->deiOutFrmPrms) ||
                (NULL != rtPrms->deiScCropCfg) ||
                (NULL != rtPrms->deiScRtCfg) ||
                (NULL != rtPrms->deiRtCfg)))
        {
            /* Copy the DEI runtime parameters */
            deiRtPrms.inFrmPrms = rtPrms->deiInFrmPrms;
            deiRtPrms.posCfg = NULL;
            deiRtPrms.outFrmPrms = rtPrms->deiOutFrmPrms;
            deiRtPrms.scCropCfg = rtPrms->deiScCropCfg;
            deiRtPrms.scRtCfg = rtPrms->deiScRtCfg;
            deiRtPrms.deiRtCfg = rtPrms->deiRtCfg;

            /* Update the runtime configuration in core descriptors */
            retVal = coreOps->updateRtMem(
                         hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                         coreDescMem,
                         chObj->chNum,
                         &deiRtPrms,
                         0u);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not update runtime desc for DEI core!\n");
            }

            /* Check DEI runtime configuration */
            if (NULL != rtPrms->deiRtCfg)
            {
                if (TRUE == rtPrms->deiRtCfg->resetDei)
                {
                    /* If the reset is required, initialize the prevFldState to
                     * zero again so that DEI will start working in Line
                     * average mode for first few fields and builds up the
                     * context again. */
                    chObj->prevFldState = 0u;
                }

                if (TRUE == rtPrms->deiRtCfg->fldRepeat)
                {
                    /* When the application repeats the input frame, don't
                     * rotate the context buffers so that the output will
                     * remain the same as the previous output. */
                    chObj->dontRotateCtxBuf = TRUE;
                }
            }
        }
        else if (TRUE == chObj->isDescDirty[descSet])
        {
            /* Descriptor is dirty, use the core to just update the
             * descriptors without providing any parameters */
            retVal = coreOps->updateRtMem(
                         hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                         coreDescMem,
                         chObj->chNum,
                         NULL,
                         0u);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not update runtime desc for DEI core!\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the DWP runtime parameters if DWP path is present */
        if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DWP_CORE_IDX])
        {
            coreOps = instObj->coreOps[VPSMDRV_DEI_DWP_CORE_IDX];
            coreDescMem =
                &chObj->coreDescMem[descSet][VPSMDRV_DEI_DWP_CORE_IDX];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->updateRtMem));

            if ((NULL != rtPrms) && (NULL != rtPrms->deiOutFrmPrms))
            {
                /* Copy the DWP runtime parameters */
                dwpRtPrms.outFrmPrms = rtPrms->deiOutFrmPrms;

                /* Update the runtime configuration in core descriptors */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_DEI_DWP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             &dwpRtPrms,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Could not update runtime desc for DWP core!\n");
                }
            }
            else if (TRUE == chObj->isDescDirty[descSet])
            {
                /* Descriptor is dirty, use the core to just update the
                 * descriptors without providing any parameters */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_DEI_DWP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             NULL,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Could not update runtime desc for DWP core!\n");
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the VIP runtime parameters if VIP is present */
        if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_VIP_CORE_IDX])
        {
            coreOps = instObj->coreOps[VPSMDRV_DEI_VIP_CORE_IDX];
            coreDescMem =
                &chObj->coreDescMem[descSet][VPSMDRV_DEI_VIP_CORE_IDX];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->updateRtMem));

            if ((NULL != rtPrms) &&
                   ((NULL != rtPrms->deiInFrmPrms) ||
                    (NULL != rtPrms->vipOutFrmPrms) ||
                    (NULL != rtPrms->vipScCropCfg) ||
                    (NULL != rtPrms->vipScRtCfg)))
            {
                /* Copy the VIP runtime parameters */
                vipRtPrms.inFrmPrms = rtPrms->deiInFrmPrms;
                vipRtPrms.outFrmPrms = rtPrms->vipOutFrmPrms;
                vipRtPrms.scCropCfg = rtPrms->vipScCropCfg;
                vipRtPrms.scRtCfg = rtPrms->vipScRtCfg;
                vipRtPrms.isDeinterlacing =
                    chObj->deiCoreCtxInfo.isDeinterlacing;

                /* Update the runtime configuration in core descriptors */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_DEI_VIP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             &vipRtPrms,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Could not update runtime desc for VIP core!\n");
                }
            }
            else if (TRUE == chObj->isDescDirty[descSet])
            {
                /* Descriptor is dirty, use the core to just update the
                 * descriptors without providing any parameters */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_DEI_VIP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             NULL,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Could not update runtime desc for VIP core!\n");
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* If Lazy Loading is enabled, recalculate and set the scaling
         * factor config for the channel for the current descriptor set in use.
         * The other descriptor set may already be under processing, so that
         * must not be touched now. It must just be set to 'dirty' so that the
         * next time when it becomes free, it can be updated.
         */
        for (scalarId = 0u; scalarId < VPS_M2M_DEI_SCALAR_ID_MAX; scalarId++)
        {
            if (TRUE == hObj->instObj->enableLazyLoading[scalarId])
            {
                /* To take care of the updated configuration due to RT
                 * params, go down into the core to get the scaling factor
                 * config for the channel.
                 */
                retVal = VpsMdrvDeiGetScFactorConfig(hObj,
                            scalarId,
                            chObj->chNum,
                            descSet);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvDeiTrace,
                        GT_ERR,
                        "EFAIL: Getting the Scaling Factor Config Failed\n");
                }
                /* The scaling ratio does not change for sub-frame scaling
                 * from what was set originally when the scaling factor
                 * config was calculated. The scaling ratio for each slice
                 * will be the same as that for the whole frame.
                 */
            }
        }

        if (NULL != rtPrms)
        {
            /* Update the dirty flags */
            /* Mark all other sets as dirty except the current descriptor set */
            for (cnt = 0u; cnt < VPSMDRV_DEI_MAX_DESC_SET; cnt++)
            {
                chObj->isDescDirty[cnt] = TRUE;
            }
        }

        /* Mark the current set as clean */
        chObj->isDescDirty[descSet] = FALSE;
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiExtractComprSize
 */
static Int32 vpsMdrvDeiExtractComprSize(VpsMdrv_DeiHandleObj *hObj,
                                        VpsMdrv_DeiQueueObj *qObj)
{
    Int32                       retVal = FVID2_SOK;
    UInt32                      chNum, cnt, fldCnt;
    UInt32                      frmCnt, numFrames;
    UInt32                      isLumaErr, isChromaErr;
    VpsMdrv_DeiChObj           *chObj;
    VpsMdrv_DeiInstObj         *instObj;
    FVID2_ProcessList          *procList;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != qObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* Get the numFrames from in frame list */
    procList = &qObj->procList;
    GT_assert(VpsMdrvDeiTrace, (NULL != procList->inFrameList[0u]));
    numFrames = procList->inFrameList[0u]->numFrames;

    for (frmCnt = 0u; frmCnt < numFrames; frmCnt++)
    {
        if (NULL == procList->inFrameList[0u]->frames[frmCnt])
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
            retVal = FVID2_EBADARGS;
            break;
        }

        /* Get the channel number from the in frame list's frame pointer */
        chNum = procList->inFrameList[0u]->frames[frmCnt]->channelNum;

        /* Check if the channel number is within the allocated one */
        if (chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
            break;
        }

        /* Get the channel object */
        chObj = hObj->chObjs[chNum];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        /* Get the compressed current out buffer size */
        if (TRUE == chObj->chPrms.comprEnable)
        {
            for (fldCnt = 0u;
                 fldCnt < chObj->deiCoreCtxInfo.numCurFldOut;
                 fldCnt++)
            {
                for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
                {
                    chObj->curOutFrmSize[fldCnt][cnt] =
                        VpsHal_vpdmaGetComprFrameSize(
                            chObj->curOutWriteDesc[fldCnt][cnt]);
                    chObj->curOutTxSize[fldCnt][cnt] =
                        VpsHal_vpdmaGetComprTransSize(
                            chObj->curOutWriteDesc[fldCnt][cnt]);

                    /* Check whether frame size and transfer size match */
                    if (chObj->curOutFrmSize[fldCnt][cnt] !=
                        chObj->curOutTxSize[fldCnt][cnt])
                    {
                        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                            "Compressed frame size and transfer size "
                            "does not match!!\n");
                    }

                    /* Reset the write descriptor to a known pattern */
                    VpsUtils_memset(
                        chObj->curOutWriteDesc[fldCnt][cnt],
                        0xFFu,
                        VPSHAL_VPDMA_DATA_DESC_SIZE);
                }

                /* Check whether both luma and chroma size are valid. If
                 * one of them is invalid, use the other valid size. Otherwise
                 * if both are invalid, set the error flag so that the next
                 * context buffer rotation is not done. */
                isLumaErr = FALSE;
                isChromaErr = FALSE;
                if (0 != chObj->prevFldState)
                {
                    /* Check for valid luma frame size */
                    if ((0u == chObj->curOutFrmSize[fldCnt][0u]) ||
                        (0u == chObj->curOutTxSize[fldCnt][0u]) ||
                        (chObj->curOutFrmSize[fldCnt][0u] >
                            (chObj->deiCtxInfo.fldBufSize / 2u)) ||
                        (chObj->curOutTxSize[fldCnt][0u]) >
                            (chObj->deiCtxInfo.fldBufSize / 2u))
                    {
                        /* Try using the chroma value */
                        chObj->curOutFrmSize[fldCnt][0u] =
                            chObj->curOutFrmSize[fldCnt][1u];
                        chObj->curOutTxSize[fldCnt][0u] =
                            chObj->curOutTxSize[fldCnt][1u];

                        /* Increment error counts */
                        isLumaErr = TRUE;
                    }

                    /* Check for valid chroma frame size */
                    if ((0u == chObj->curOutFrmSize[fldCnt][1u]) ||
                        (0u == chObj->curOutTxSize[fldCnt][1u]) ||
                        (chObj->curOutFrmSize[fldCnt][1u] >
                            (chObj->deiCtxInfo.fldBufSize / 2u)) ||
                        (chObj->curOutTxSize[fldCnt][1u]) >
                            (chObj->deiCtxInfo.fldBufSize / 2u))
                    {
                        /* Try using the luma value */
                        chObj->curOutFrmSize[fldCnt][1u] =
                            chObj->curOutFrmSize[fldCnt][0u];
                        chObj->curOutTxSize[fldCnt][1u] =
                            chObj->curOutTxSize[fldCnt][0u];

                        /* Increment error counts */
                        isChromaErr = TRUE;
                    }

                    if ((TRUE == isLumaErr) && (TRUE == isChromaErr))
                    {
                        /* Irrecoverable error - Don't use the current
                         * compressed buffer. Hence disable context buffer
                         * rotation for next submission. */
                        chObj->dontRotateCtxBuf = TRUE;
                    }
                }
            }
        }
        else
        {
            for (fldCnt = 0u;
                 fldCnt < chObj->deiCoreCtxInfo.numCurFldOut;
                 fldCnt++)
            {
                for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
                {
                    chObj->curOutFrmSize[fldCnt][cnt] = 0u;
                    chObj->curOutTxSize[fldCnt][cnt] = 0u;
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiRetErrProcList
 */
static Void vpsMdrvDeiRetErrProcList(VpsMdrv_DeiHandleObj *hObj,
                                     VpsMdrv_DeiQueueObj *qObj)
{
    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != qObj));

    if ((NULL != hObj->fdmCbPrms.fdmErrCbFxn) &&
        (NULL != hObj->fdmCbPrms.errList))
    {
        /* Copy the driver's process list to application's error process list */
        FVID2_copyProcessList(hObj->fdmCbPrms.errList, &qObj->procList);

        /* Call the application's error call back function */
        hObj->fdmCbPrms.fdmErrCbFxn(
            hObj->fdmCbPrms.fdmData,
            hObj->fdmCbPrms.errList,
            NULL);
    }

    /* Decrement the pending request count */
    if (0u != hObj->numPendReq)
    {
        hObj->numPendReq--;
    }

    return;
}



/**
 * Rotate context buffers
 */
static Int32 vpsMdrvDeiRotateCtxBuf(VpsMdrv_DeiChObj *chObj, UInt32 inFid)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  bCnt;
#ifdef TI_816X_BUILD
    UInt32                  fldCnt, cnt;
#endif  /* #ifdef TI_816X_BUILD */
    UInt32                  tempIdx, tempFid, fid;
    UInt32                  width, height, cbcrOffset;
    UInt8                  *tempPtr;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    deiCoreCtxBuf = &chObj->deiCoreCtxBuf;

    /* Calculate CbCr buffer offset - YUV422 input = linestride * height */
    width = chObj->chPrms.inFmt.width;
    height = chObj->chPrms.inFmt.height;
    if (FVID2_SF_PROGRESSIVE == chObj->chPrms.inFmt.scanFormat)
    {
        /* For the progressive mode, height from the format is
           frame height, so for the previous fields, it should
           be field size. */
        height = height / 2u;
    }
    cbcrOffset = VpsUtils_align(width, VPSHAL_VPDMA_LINE_STRIDE_ALIGN) * height;
    if (TRUE == chObj->chPrms.comprEnable)
    {
        /* When compressor is enabled, half the buffer size is assured. */
        cbcrOffset /= 2u;
    }

    /* Rotate context buffers */
    if (FALSE == chObj->dontRotateCtxBuf)
    {
        /* Rotate only if no invalid frame size error occurs */
        if (0u == chObj->fldIdx)
        {
            if (chObj->deiCtxInfo.numFld > 0u)
            {
                chObj->fldIdx = chObj->deiCtxInfo.numFld - 1u;
            }
        }
        else
        {
            chObj->fldIdx--;
        }
        if (0u == chObj->mvIdx)
        {
            if (chObj->deiCtxInfo.numMv > 0u)
            {
                chObj->mvIdx = chObj->deiCtxInfo.numMv - 1u;
            }
        }
        else
        {
            chObj->mvIdx--;
        }
        if (0u == chObj->mvstmIdx)
        {
            if (chObj->deiCtxInfo.numMvstm > 0u)
            {
                chObj->mvstmIdx = chObj->deiCtxInfo.numMvstm - 1u;
            }
        }
        else
        {
            chObj->mvstmIdx--;
        }
    }

    if (FVID2_FID_BOTTOM == inFid)
    {
        fid = 1u;
    }
    else
    {
        fid = 0u;
    }

#ifdef TI_816X_BUILD
    /* Assign the context buffers in core structure */
    tempIdx = chObj->fldIdx;
    tempFid = fid;
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numPrevFld; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.fldBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the field Y and CbCr buffer pointers */
        deiCoreCtxBuf->prevInBuf[bCnt][VCORE_DEI_Y_IDX] = tempPtr;
        deiCoreCtxBuf->prevInBuf[bCnt][VCORE_DEI_CBCR_IDX] =
            tempPtr + cbcrOffset;

        /* Assign field ID - alternatively */
        tempFid = tempFid ^ 1u;
        deiCoreCtxBuf->prevInFid[bCnt] = tempFid;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numFld)
        {
            tempIdx = 0u;
        }
    }
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numCurFldOut; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.fldBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the field Y and CbCr buffer pointers */
        deiCoreCtxBuf->curOutBuf[bCnt][VCORE_DEI_Y_IDX] = tempPtr;
        deiCoreCtxBuf->curOutBuf[bCnt][VCORE_DEI_CBCR_IDX] =
            tempPtr + cbcrOffset;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numFld)
        {
            tempIdx = 0u;
        }
    }
#endif  /* #ifdef TI_816X_BUILD */

    tempIdx = chObj->mvIdx;
    tempFid = fid;
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numMvIn; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.mvBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MV buffer pointer */
        deiCoreCtxBuf->mvInBuf[bCnt] = tempPtr;

        /* Assign field ID - alternatively */
        tempFid = tempFid ^ 1u;
        deiCoreCtxBuf->mvInFid[bCnt] = tempFid;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMv)
        {
            tempIdx = 0u;
        }
    }
    if (chObj->deiCoreCtxInfo.numMvOut > 0u)
    {
        tempPtr = chObj->deiCtxBuf.mvBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MV buffer pointer */
        deiCoreCtxBuf->mvOutBuf = tempPtr;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMv)
        {
            tempIdx = 0u;
        }
    }

#ifdef TI_816X_BUILD
    tempIdx = chObj->mvstmIdx;
    tempFid = fid;
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numMvstmIn; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.mvstmBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MVSTM buffer pointer */
        deiCoreCtxBuf->mvstmInBuf[bCnt] = tempPtr;

        /* Assign field ID - alternatively */
        tempFid = tempFid ^ 1u;
        deiCoreCtxBuf->mvstmInFid[bCnt] = tempFid;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMvstm)
        {
            tempIdx = 0u;
        }
    }
    if (chObj->deiCoreCtxInfo.numMvstmOut > 0u)
    {
        tempPtr = chObj->deiCtxBuf.mvstmBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MVSTM buffer pointer */
        deiCoreCtxBuf->mvstmOutBuf = tempPtr;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMvstm)
        {
            tempIdx = 0u;
        }
    }

    if ((chObj->deiCoreCtxInfo.numPrevFld > 1u) &&
        (FALSE == chObj->dontRotateCtxBuf))
    {
        /* Use the same frame size/field for the next frame as well when
         * invalid frame size error occurs */
        for (fldCnt = chObj->deiCoreCtxInfo.numPrevFld - 1u;
             fldCnt > 0u;
             fldCnt--)
        {
            for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
            {
                deiCoreCtxBuf->frameSize[fldCnt][cnt] =
                    deiCoreCtxBuf->frameSize[fldCnt - 1u][cnt];
                deiCoreCtxBuf->transferSize[fldCnt][cnt] =
                    deiCoreCtxBuf->transferSize[fldCnt - 1u][cnt];
            }
        }
    }
    if (TRUE == chObj->chPrms.comprEnable)
    {
        /* Use the same frame size/field for the next frame as well when
         * invalid frame size error occurs */
        if (FALSE == chObj->dontRotateCtxBuf)
        {
            for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
            {
                deiCoreCtxBuf->frameSize[0u][cnt] =
                    chObj->curOutFrmSize[0u][cnt];
                deiCoreCtxBuf->transferSize[0u][cnt] =
                    chObj->curOutTxSize[0u][cnt];
            }
        }
    }
    else
    {
        for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
        {
            deiCoreCtxBuf->frameSize[0u][cnt] = 0u;
            deiCoreCtxBuf->transferSize[0u][cnt] = 0u;
        }
    }

    for (fldCnt = 0u; fldCnt < chObj->deiCoreCtxInfo.numCurFldOut; fldCnt++)
    {
        for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
        {
            deiCoreCtxBuf->curOutWriteDesc[fldCnt][cnt] =
                chObj->curOutWriteDesc[fldCnt][cnt];
        }
    }

    if (chObj->prevFldState <= chObj->deiCoreCtxInfo.numPrevFld)
    {
        if (0u == chObj->prevFldState)
        {
            /* For the first frame all decompression should be disabled */
            for (cnt = 0u; cnt < VCORE_DEI_MAX_DCOMPR; cnt++)
            {
                deiCoreCtxBuf->dcomprEnable[cnt] = FALSE;
            }
        }
        else if (chObj->prevFldState <= chObj->deiCoreCtxInfo.numPrevFld)
        {
            if (FALSE == chObj->dontRotateCtxBuf)
            {
                /* Enable decompression incrementally depending on whether
                 * compression is enabled or not */
                for (cnt = 0u; cnt < chObj->prevFldState; cnt++)
                {
                    deiCoreCtxBuf->dcomprEnable[cnt] =
                        chObj->chPrms.comprEnable;
                }
            }
        }

        /* If error occurs at the start-up don't move ahead - remain in
         * start-up condition only!! */
        if (FALSE == chObj->dontRotateCtxBuf)
        {
            chObj->prevFldState++;
        }
    }
#endif  /* #ifdef TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    if (chObj->prevFldState <= chObj->deiCoreCtxInfo.numPrevFld)
    {
        /* If error occurs at the start-up don't move ahead - remain in
         * start-up condition only!! */
        if (FALSE == chObj->dontRotateCtxBuf)
        {
            chObj->prevFldState++;
        }
    }
#endif  /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

    /* Reset the flag - rotation should be skipped only once for every error */
    chObj->dontRotateCtxBuf = FALSE;

    return (retVal);
}



/**
 *  Rotate context buffers
 */
static Int32 vpsMdrvDeiRotateCtxBufMode1(VpsMdrv_DeiChObj *chObj,
                                         UInt32 inFid,
                                         const FVID2_Frame *frame)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  bCnt, fldCnt, cnt;
    UInt32                  tempIdx, tempFid, fid;
    UInt32                  width, height, cbcrOffset;
    UInt8                  *tempPtr;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != frame));
    deiCoreCtxBuf = &chObj->deiCoreCtxBuf;

    /* Mode 1 should have only 2 previous field */
    GT_assert(VpsMdrvDeiTrace, (2u == chObj->deiCtxInfo.numFld));
    GT_assert(VpsMdrvDeiTrace, (FVID2_FID_BOTTOM == inFid));

    /* Calculate CbCr buffer offset - YUV422 input = linestride * height */
    width = chObj->chPrms.inFmt.width;
    height = chObj->chPrms.inFmt.height;
    if (FVID2_SF_PROGRESSIVE == chObj->chPrms.inFmt.scanFormat)
    {
        /* For the progressive mode, height from the format is
           frame height, so for the previous fields, it should
           be field size. */
        height = height / 2u;
    }
    cbcrOffset = VpsUtils_align(width, VPSHAL_VPDMA_LINE_STRIDE_ALIGN) * height;
    if (TRUE == chObj->chPrms.comprEnable)
    {
        /* When compressor is enabled, half the buffer size is assured. */
        cbcrOffset /= 2u;
    }

    /* Rotate context buffers */
    if (FALSE == chObj->dontRotateCtxBuf)
    {
        /* Rotate only if no invalid frame size error occurs */
        if (0u == chObj->fldIdx)
        {
            if (chObj->deiCtxInfo.numFld > 0u)
            {
                chObj->fldIdx = chObj->deiCtxInfo.numFld - 1u;
            }
        }
        else
        {
            chObj->fldIdx--;
        }
        if (0u == chObj->mvIdx)
        {
            if (chObj->deiCtxInfo.numMv > 0u)
            {
                chObj->mvIdx = chObj->deiCtxInfo.numMv - 1u;
            }
        }
        else
        {
            chObj->mvIdx--;
        }
        if (0u == chObj->mvstmIdx)
        {
            if (chObj->deiCtxInfo.numMvstm > 0u)
            {
                chObj->mvstmIdx = chObj->deiCtxInfo.numMvstm - 1u;
            }
        }
        else
        {
            chObj->mvstmIdx--;
        }
    }

    /* Assign the context buffers in core structure */
    fid = 1u;

    /* Assigne the N-1 buffer from frame */
    tempPtr = frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX];
    deiCoreCtxBuf->prevInBuf[0u][VCORE_DEI_Y_IDX] = tempPtr;
    tempPtr = frame->addr[FVID2_FIELD_EVEN_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX];
    deiCoreCtxBuf->prevInBuf[0u][VCORE_DEI_CBCR_IDX] = tempPtr;
    /* Assign field ID */
    deiCoreCtxBuf->prevInFid[0u] = fid ^ 1u;

    /* Start from N-2 buffer. So N-2 fid is same as inFid */
    tempIdx = chObj->fldIdx;
    tempFid = fid ^ 1u;
    for (bCnt = 1u; bCnt < chObj->deiCoreCtxInfo.numPrevFld; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.fldBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the field Y and CbCr buffer pointers */
        deiCoreCtxBuf->prevInBuf[bCnt][VCORE_DEI_Y_IDX] = tempPtr;
        deiCoreCtxBuf->prevInBuf[bCnt][VCORE_DEI_CBCR_IDX] =
            tempPtr + cbcrOffset;

        /* Assign field ID - alternatively */
        tempFid = tempFid ^ 1u;
        deiCoreCtxBuf->prevInFid[bCnt] = tempFid;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numFld)
        {
            tempIdx = 0u;
        }
    }
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numCurFldOut; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.fldBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the field Y and CbCr buffer pointers */
        deiCoreCtxBuf->curOutBuf[bCnt][VCORE_DEI_Y_IDX] = tempPtr;
        deiCoreCtxBuf->curOutBuf[bCnt][VCORE_DEI_CBCR_IDX] =
            tempPtr + cbcrOffset;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numFld)
        {
            tempIdx = 0u;
        }
    }

    tempIdx = chObj->mvIdx;
    tempFid = fid;
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numMvIn; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.mvBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MV buffer pointer */
        deiCoreCtxBuf->mvInBuf[bCnt] = tempPtr;

        /* Assign field ID - alternatively */
        tempFid = tempFid ^ 1u;
        deiCoreCtxBuf->mvInFid[bCnt] = tempFid;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMv)
        {
            tempIdx = 0u;
        }
    }
    if (chObj->deiCoreCtxInfo.numMvOut > 0u)
    {
        tempPtr = chObj->deiCtxBuf.mvBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MV buffer pointer */
        deiCoreCtxBuf->mvOutBuf = tempPtr;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMv)
        {
            tempIdx = 0u;
        }
    }

    tempIdx = chObj->mvstmIdx;
    tempFid = fid;
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numMvstmIn; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.mvstmBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MVSTM buffer pointer */
        deiCoreCtxBuf->mvstmInBuf[bCnt] = tempPtr;

        /* Assign field ID - alternatively */
        tempFid = tempFid ^ 1u;
        deiCoreCtxBuf->mvstmInFid[bCnt] = tempFid;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMvstm)
        {
            tempIdx = 0u;
        }
    }
    if (chObj->deiCoreCtxInfo.numMvstmOut > 0u)
    {
        tempPtr = chObj->deiCtxBuf.mvstmBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the MVSTM buffer pointer */
        deiCoreCtxBuf->mvstmOutBuf = tempPtr;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numMvstm)
        {
            tempIdx = 0u;
        }
    }

    if (TRUE == chObj->chPrms.comprEnable)
    {
        /* Use the same frame size/field for the next frame as well when
         * invalid frame size error occurs */
        if (FALSE == chObj->dontRotateCtxBuf)
        {
            for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
            {
                deiCoreCtxBuf->frameSize[1u][cnt] =
                    chObj->curOutFrmSize[0u][cnt];
                deiCoreCtxBuf->transferSize[1u][cnt] =
                    chObj->curOutTxSize[0u][cnt];
            }
        }
    }
    else
    {
        for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
        {
            deiCoreCtxBuf->frameSize[1u][cnt] = 0u;
            deiCoreCtxBuf->transferSize[1u][cnt] = 0u;
        }
    }

    for (fldCnt = 0u; fldCnt < chObj->deiCoreCtxInfo.numCurFldOut; fldCnt++)
    {
        for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
        {
            deiCoreCtxBuf->curOutWriteDesc[fldCnt][cnt] =
                chObj->curOutWriteDesc[fldCnt][cnt];
        }
    }

    if (chObj->prevFldState <= chObj->deiCoreCtxInfo.numPrevFld)
    {
        if (0u == chObj->prevFldState)
        {
            /* For the first frame all decompression should be disabled */
            for (cnt = 0u; cnt < VCORE_DEI_MAX_DCOMPR; cnt++)
            {
                deiCoreCtxBuf->dcomprEnable[cnt] = FALSE;
            }
            /* If error occurs at the start-up don't move ahead - remain in
             * start-up condition only!! */
            if (FALSE == chObj->dontRotateCtxBuf)
            {
                chObj->prevFldState++;
            }
        }
        else if (chObj->prevFldState <= chObj->deiCoreCtxInfo.numPrevFld)
        {
            if (FALSE == chObj->dontRotateCtxBuf)
            {
                /* Enable decompression incrementally depending on compression
                 * is enabled or not */
                for (cnt = 1u; cnt < chObj->prevFldState; cnt++)
                {
                    deiCoreCtxBuf->dcomprEnable[cnt] =
                        chObj->chPrms.comprEnable;
                }
            }
        }

        /* If error occurs at the start-up don't move ahead - remain in
         * start-up condition only!! */
        if (FALSE == chObj->dontRotateCtxBuf)
        {
            chObj->prevFldState++;
        }
    }

    /* Reset the flag - rotation should be skipped only once for every error */
    chObj->dontRotateCtxBuf = FALSE;

    return (retVal);
}



/**
 * Rotate Context for the Progressive TNR mode
 */
static Int32 vpsMdrvDeiRotateTnrBuf(VpsMdrv_DeiChObj *chObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  bCnt;
    UInt32                  tempIdx, tempFid;
    UInt32                  width, height, cbcrOffset;
    UInt8                  *tempPtr;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    deiCoreCtxBuf = &chObj->deiCoreCtxBuf;

    /* Calculate CbCr buffer offset - YUV422 input = linestride * height */
    width = chObj->chPrms.inFmt.width;
    height = chObj->chPrms.inFmt.height;
    if (FVID2_SF_PROGRESSIVE == chObj->chPrms.inFmt.scanFormat)
    {
        /* For the progressive mode, height from the format is
           frame height, so for the previous fields, it should
           be field size. */
        height = height / 2u;
    }
    cbcrOffset = VpsUtils_align(width, VPSHAL_VPDMA_LINE_STRIDE_ALIGN) * height;
    if (TRUE == chObj->chPrms.comprEnable)
    {
        /* When compressor is enabled, half the buffer size is assured. */
        cbcrOffset /= 2u;
    }

    if (0u == chObj->fldIdx)
    {
        chObj->fldIdx = chObj->deiCoreCtxInfo.numPrevFld;
    }
    else
    {
        chObj->fldIdx = 0u;
    }

    /* Previous field is always top field and previous to
       previous field is always bottom field for progressive
       TNR mode */
    tempFid = 1u;
    tempIdx = chObj->fldIdx;
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numPrevFld; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.fldBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the field  Y and CbCr buffer pointers */
        deiCoreCtxBuf->prevInBuf[bCnt][VCORE_DEI_Y_IDX] = tempPtr;
        deiCoreCtxBuf->prevInBuf[bCnt][VCORE_DEI_CBCR_IDX] =
            tempPtr + cbcrOffset;

        /* Assign field ID - alternatively */
        tempFid = tempFid ^ 1u;
        deiCoreCtxBuf->prevInFid[bCnt] = tempFid;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numFld)
        {
            tempIdx = 0u;
        }
    }
    for (bCnt = 0u; bCnt < chObj->deiCoreCtxInfo.numCurFldOut; bCnt++)
    {
        tempPtr = chObj->deiCtxBuf.fldBuf[tempIdx];
        GT_assert(VpsMdrvDeiTrace, (NULL != tempPtr));

        /* Assign the field Y and CbCr buffer pointers */
        deiCoreCtxBuf->curOutBuf[bCnt][VCORE_DEI_Y_IDX] = tempPtr;
        deiCoreCtxBuf->curOutBuf[bCnt][VCORE_DEI_CBCR_IDX] =
            tempPtr + cbcrOffset;

        /* Move to the next buffer */
        tempIdx++;
        if (tempIdx >= chObj->deiCtxInfo.numFld)
        {
            tempIdx = 0u;
        }
    }

    return (retVal);
}



/*
 * Update processList Rtparams with subframe info calculated
 */
static void vpsMdrvDeiSubFrmUpdate(VpsMdrv_SubFrameChInst *slcData,
                                   FVID2_Frame *inFrm,
                                   FVID2_Frame *outFrm,
                                   Vps_M2mDeiRtParams *rtParams)
{

    Vps_FrameParams   *inFrmPrms   = rtParams->deiInFrmPrms;
    Vps_FrameParams   *outFrmPrms  = rtParams->deiOutFrmPrms;
    Vps_ScRtConfig    *scRtparams  = rtParams->deiScRtCfg;
    Vps_CropConfig    *srcCropCfg  = rtParams->deiScCropCfg;
    /* TODO: support for VIP scalar subframe processing */

    /* update outFrm start address with required offsets for current subframe */
    outFrm->addr[FVID2_FRAME_ADDR_IDX]
                [FVID2_YUV_INT_ADDR_IDX] =
                (char *)outFrm->addr[FVID2_FRAME_ADDR_IDX]
                                    [FVID2_YUV_INT_ADDR_IDX]
                + (slcData->tarFrmW*slcData->tarBufOffset*2);

    /* update inFrm start addresses with required offsets for current subframe */
    if ((slcData->dataFmt == FVID2_DF_YUV420SP_UV) ||
        (slcData->dataFmt == FVID2_DF_YUV422SP_UV))
    {
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_SP_Y_ADDR_IDX] =
            (char *)inFrm->addr[FVID2_FRAME_ADDR_IDX]
                               [FVID2_YUV_SP_Y_ADDR_IDX]
                   + (slcData->srcFrmW*slcData->srcBufOffset);
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_SP_CBCR_ADDR_IDX] =
            (char *)inFrm->addr[FVID2_FRAME_ADDR_IDX]
                               [FVID2_YUV_SP_CBCR_ADDR_IDX]
                   + (slcData->srcFrmW*((slcData->srcBufOffset)>>1));
        inFrmPrms->pitch[FVID2_YUV_SP_Y_ADDR_IDX] =
                                          slcData->srcFrmW;
        inFrmPrms->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] =
                                          slcData->srcFrmW;
    } else /* 422 input case*/
    {
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_INT_ADDR_IDX] =
             (char *)inFrm->addr[FVID2_FRAME_ADDR_IDX]
                                [FVID2_YUV_INT_ADDR_IDX]
                    + (slcData->srcFrmW*slcData->srcBufOffset*2u);
        inFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] =
                                          slcData->srcFrmW*2;
    }

    /* update rtPrms with calculated values for current subframe */
    inFrmPrms->width = slcData->srcFrmW;
    //?? 3 for RAV, how it handles for chroma
    inFrmPrms->height = slcData->srcH +
        (slcData->chrUsLnMemSlcTop + slcData->chrUsLnMemSlcBtm);


    outFrmPrms->width = slcData->tarFrmW;
    outFrmPrms->height = slcData->tarH;
    outFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] =
                              slcData->tarFrmW * 2u;

    scRtparams->scBypass       = FALSE;
    scRtparams->scSetPhInfo    = TRUE;

    if (slcData->filtType == VPS_SC_VST_POLYPHASE)
    {
        scRtparams->rowAccInc      = slcData->rowAccIncr;
        scRtparams->rowAccOffset   = slcData->rowAccOffset;
        scRtparams->rowAccOffsetB  = slcData->rowAccOffsetB;
    }
    else
    {
        scRtparams->ravScFactor    = slcData->ravScFactor;
        scRtparams->ravRowAccInit  = slcData->ravRowAccInit;
        scRtparams->ravRowAccInitB = slcData->ravRowAccInitB;
    }
    srcCropCfg->cropHeight     = slcData->srcH;
    srcCropCfg->cropWidth      = slcData->srcFrmW;
    srcCropCfg->cropStartX     = 0u;
    srcCropCfg->cropStartY     = slcData->scCropLnsTop;

    return;
}
