/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "deiLink_priv.h"

//#define DEI_LINK_QUEUE_REQ

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
static Int32 DeiLink_drvReleaseContextField(DeiLink_Obj *pObj);
#endif /* TI_814X_BUILD || TI_8107_BUILD */

Int32 DeiLink_drvFvidCb(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    DeiLink_Obj *pObj = (DeiLink_Obj *)appData;

    #ifdef DEI_LINK_QUEUE_REQ
    VpsUtils_tskSendCmd(&pObj->tsk, DEI_LINK_CMD_GET_PROCESSED_DATA);
    #else
    Semaphore_post(pObj->complete);
    #endif
    return FVID2_SOK;
}

Int32 DeiLink_drvFvidErrCb(
                FVID2_Handle handle,
                Ptr appData,
                Ptr errList,
                Ptr reserved)
{
    return FVID2_SOK;
}

Int32 DeiLink_drvCreateReqObj(DeiLink_Obj *pObj)
{
    Int32 status;
    UInt32 reqId;

    memset(pObj->reqObj, 0, sizeof(pObj->reqObj));

    status = VpsUtils_queCreate(
                &pObj->reqQue,
                DEI_LINK_MAX_REQ,
                pObj->reqQueMem,
                VPSUTILS_QUE_FLAG_NO_BLOCK_QUE
                );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->reqQueCount = 0;
    pObj->isReqPend = FALSE;

    for(reqId=0; reqId<DEI_LINK_MAX_REQ; reqId++)
    {
        status = VpsUtils_quePut(&pObj->reqQue, &pObj->reqObj[reqId], BIOS_NO_WAIT);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    pObj->reqNumOutLists = 1;
    if (VpsUtils_appM2mDeiIsDualOutInst(pObj->drvInstId))
    {
        pObj->reqNumOutLists = 2;
    }

    return FVID2_SOK;
}

Int32 DeiLink_drvCreateOutObj(DeiLink_Obj *pObj)
{
    DeiLink_OutObj *pOutObj;
    Int32 status;
    UInt32 frameId, outId, chId;
    FVID2_Format *pFormat;
    UInt32 maxOutWidth = 0u, maxOutHeight = 0u;
    System_LinkChInfo *pInChInfo;
    System_LinkChInfo *pOutChInfo;
    const Sys_DeiLinkCfg *pSysDeiCfg = NULL;

    memset(&pObj->outFrameDrop, 0, sizeof(pObj->outFrameDrop));

    if (pObj->createArgs.deiCfgId != SYS_DEI_CFG_DEFAULT)
    {
        pSysDeiCfg = Sys_getDeiLinkCfg(pObj->createArgs.deiCfgId);
        GT_assert( GT_DEFAULT_MASK, NULL != pSysDeiCfg);
        GT_assert( GT_DEFAULT_MASK, pObj->inQueInfo.numCh <= pSysDeiCfg->numCh);
    }

    for (outId = 0u; outId < DEI_LINK_MAX_OUT_QUE; outId++)
    {
        if (pObj->createArgs.deiCfgId != SYS_DEI_CFG_DEFAULT)
        {
            for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
            {
                if (pSysDeiCfg->deiOutWidth[outId][chId] > maxOutWidth)
                {
                    maxOutWidth = pSysDeiCfg->deiOutWidth[outId][chId];
                }
                if (pSysDeiCfg->deiOutHeight[outId][chId] > maxOutHeight)
                {
                    maxOutHeight = pSysDeiCfg->deiOutHeight[outId][chId];
                }
            }
        }
        else
        {
            maxOutWidth  = pObj->inQueInfo.chInfo[0].width;
            pInChInfo = &pObj->inQueInfo.chInfo[0u];
            if (pInChInfo->scanFormat == FVID2_SF_INTERLACED)
            {
                maxOutHeight = pObj->inQueInfo.chInfo[0].height * 2;
            }
            else
            {
                maxOutHeight = pObj->inQueInfo.chInfo[0].height;
            }
        }

        pOutObj = &pObj->outObj[outId];

        pOutObj->outNumFrames =
            pObj->inQueInfo.numCh * DEI_LINK_MAX_OUT_FRAMES_PER_CH;

        GT_assert( GT_DEFAULT_MASK, pObj->inQueInfo.numCh <= DEI_LINK_MAX_CH);
        GT_assert( GT_DEFAULT_MASK, pOutObj->outNumFrames <= DEI_LINK_MAX_OUT_FRAMES);

        status = VpsUtils_bufCreate(&pOutObj->bufOutQue, TRUE, FALSE);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        pFormat = &pOutObj->outFormat;

        pFormat->channelNum     = 0;
        pFormat->width          = maxOutWidth;
        pFormat->height         = maxOutHeight;
        pFormat->fieldMerged[0] = FALSE;
        pFormat->fieldMerged[1] = FALSE;
        pFormat->fieldMerged[2] = FALSE;

        pFormat->pitch[0]       = VpsUtils_align(pFormat->width, VPS_BUFFER_ALIGNMENT);

        switch(outId)
        {
            case DEI_LINK_OUT_QUE_DEI_SC:
                pFormat->dataFormat     = FVID2_DF_YUV422I_YUYV;
                pFormat->pitch[0]      *= 2;
                pFormat->pitch[1]       = pFormat->pitch[0];
                break;
            case DEI_LINK_OUT_QUE_VIP_SC:
                if(pObj->createArgs.setVipScYuv422Format)
                {
                    pFormat->dataFormat     = FVID2_DF_YUV422I_YUYV;
                    pFormat->pitch[0]      *= 2;
                }
                else
                {
                    pFormat->dataFormat     = FVID2_DF_YUV420SP_UV;
                }
                pFormat->pitch[1]       = pFormat->pitch[0];
                if(pObj->createArgs.tilerEnable && pFormat->dataFormat == FVID2_DF_YUV420SP_UV)
                {
                    pFormat->pitch[0]       = VPSUTILS_TILER_CNT_8BIT_PITCH;
                    pFormat->pitch[1]       = VPSUTILS_TILER_CNT_16BIT_PITCH;
                }
                break;
        }
        pFormat->pitch[2]       = 0;

        pFormat->scanFormat     = FVID2_SF_PROGRESSIVE;
        pFormat->bpp            = FVID2_BPP_BITS16;
        pFormat->reserved       = NULL;

        if(pObj->createArgs.tilerEnable && outId==DEI_LINK_OUT_QUE_VIP_SC && pFormat->dataFormat == FVID2_DF_YUV420SP_UV)
        {
            status = VpsUtils_tilerFrameAlloc(
                        &pOutObj->outFormat,
                        pOutObj->outFrames,
                        pOutObj->outNumFrames
                        );
        }
        else
        {
            status = VpsUtils_memFrameAlloc(
                        &pOutObj->outFormat,
                        pOutObj->outFrames,
                        pOutObj->outNumFrames
                        );
        }
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        for(frameId=0; frameId<pOutObj->outNumFrames; frameId++)
        {
            status = VpsUtils_bufPutEmptyFrame(&pOutObj->bufOutQue, &pOutObj->outFrames[frameId]);
            GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

            pOutObj->outFrames[frameId].appData = &pOutObj->frameInfo[frameId];

            #ifdef SYSTEM_VERBOSE_PRINTS
            {
                Vps_rprintf(" %d: DEI: OUT%d: %2d: 0x%08x, %d x %d, %08x B\n",
                    Clock_getTicks(),
                    outId,
                    frameId, pOutObj->outFrames[frameId].addr[0][0],
                    pOutObj->outFormat.pitch[0]/2, pOutObj->outFormat.height, pOutObj->outFormat.height*pOutObj->outFormat.pitch[0]);
            }
            #endif

        }
        #ifdef SYSTEM_VERBOSE_PRINTS
        if(outId==0)
        {
            Vps_rprintf(" %d: DEI: 0x%08x %08x B\n",
                Clock_getTicks(),
                pOutObj->outFrames[0].addr[0][0],
                pOutObj->outFormat.height*pOutObj->outFormat.pitch[0] * frameId);
        }
        #endif
    }

    pObj->info.numQue = DEI_LINK_MAX_OUT_QUE;
    for (outId = 0u; outId < DEI_LINK_MAX_OUT_QUE; outId++)
    {
        pObj->info.queInfo[outId].numCh = 0;
        if (TRUE == pObj->createArgs.enableOut[outId])
        {
            pObj->info.queInfo[outId].numCh = pObj->inQueInfo.numCh;
        }
    }

    for (chId = 0u; chId < pObj->inQueInfo.numCh; chId++)
    {
        pInChInfo = &pObj->inQueInfo.chInfo[chId];

        for (outId = 0u; outId < DEI_LINK_MAX_OUT_QUE; outId++)
        {
            if (TRUE == pObj->createArgs.enableOut[outId])
            {
                pOutChInfo = &pObj->info.queInfo[outId].chInfo[chId];

                pFormat = &pObj->outObj[outId].outFormat;

                pOutChInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
                if(outId==DEI_LINK_OUT_QUE_VIP_SC && pObj->createArgs.tilerEnable && pFormat->dataFormat == FVID2_DF_YUV420SP_UV)
                {
                    pOutChInfo->memType = (Vps_VpdmaMemoryType)VPS_VPDMA_MT_TILEDMEM;
                }
                pOutChInfo->dataFormat = (FVID2_DataFormat)pFormat->dataFormat;
                if (pObj->createArgs.deiCfgId != SYS_DEI_CFG_DEFAULT)
                {
                    pOutChInfo->width = pSysDeiCfg->deiOutWidth[outId][chId];
                    pOutChInfo->height = pSysDeiCfg->deiOutHeight[outId][chId];
                }
                else
                {
                    pOutChInfo->width = pInChInfo->width;
                    if (pInChInfo->scanFormat == FVID2_SF_INTERLACED)
                    {
                        pOutChInfo->height = pInChInfo->height * 2u;
                    }
                    else
                    {
                        pOutChInfo->height = pInChInfo->height;
                    }
                }
                pOutChInfo->pitch[0] = pFormat->pitch[0];
                pOutChInfo->pitch[1] = pFormat->pitch[1];
                pOutChInfo->pitch[2] = pFormat->pitch[2];
                pOutChInfo->scanFormat = (FVID2_ScanFormat)pFormat->scanFormat;
            }
        }
    }

    return (status);
}

Int32 DeiLink_drvCreateChObj(DeiLink_Obj *pObj, UInt32 chId)
{
    DeiLink_ChObj *pChObj;
    System_LinkChInfo *pInChInfo;
    System_LinkChInfo *pOutChInfo;
    Vps_M2mDeiChParams *pDrvChParams;
    FVID2_Format *pFormat;
    Int32 status;
    Bool deiBypass;
    UInt32 outId;

    pChObj = &pObj->chObj[chId];

    status = VpsUtils_bufCreate(&pChObj->inQue, FALSE, FALSE);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pChObj->nextFid = 0;

    pInChInfo = &pObj->inQueInfo.chInfo[chId];

    if (pInChInfo->scanFormat == FVID2_SF_INTERLACED)
    {
        deiBypass = FALSE;
    }
    else
    {
        deiBypass = TRUE;
    }

    pDrvChParams = &pObj->drvChArgs[chId];

    pFormat = &pDrvChParams->inFmt;

    pFormat->channelNum     = chId;
    pFormat->width          = pInChInfo->width;
    pFormat->height         = pInChInfo->height;
    pFormat->fieldMerged[0] = FALSE;
    pFormat->fieldMerged[1] = pFormat->fieldMerged[0];
    pFormat->fieldMerged[2] = pFormat->fieldMerged[0];
    pFormat->pitch[0]       = pInChInfo->pitch[0];
    pFormat->pitch[1]       = pInChInfo->pitch[1];
    pFormat->pitch[2]       = pInChInfo->pitch[2];
    pFormat->dataFormat     = pInChInfo->dataFormat;
    pFormat->scanFormat     = pInChInfo->scanFormat;
    pFormat->bpp            = FVID2_BPP_BITS16;
    pFormat->reserved       = NULL;

    pDrvChParams->inMemType     = pInChInfo->memType;
    pDrvChParams->outMemTypeDei = VPS_VPDMA_MT_NONTILEDMEM;
    pDrvChParams->outMemTypeVip = VPS_VPDMA_MT_NONTILEDMEM;
    pDrvChParams->drnEnable     = FALSE;
    pDrvChParams->comprEnable   = pObj->createArgs.comprEnable;

    for (outId = 0u; outId < DEI_LINK_MAX_OUT_QUE; outId++)
    {
        if (TRUE == pObj->createArgs.enableOut[outId])
        {
            pOutChInfo = &pObj->info.queInfo[outId].chInfo[chId];

            pFormat = &pChObj->outFormat[outId];

            if(pInChInfo->width < pOutChInfo->width
                    ||
                pInChInfo->height < pOutChInfo->height
            )
            {
                pObj->loadUpsampleCoeffs = TRUE;
            }

            pFormat->channelNum     = chId;
            pFormat->width          = pOutChInfo->width;
            pFormat->height         = pOutChInfo->height;
            pFormat->fieldMerged[0] = FALSE;
            pFormat->fieldMerged[1] = FALSE;
            pFormat->fieldMerged[2] = FALSE;
            pFormat->pitch[0]       = pOutChInfo->pitch[0];
            pFormat->pitch[1]       = pOutChInfo->pitch[1];
            pFormat->pitch[2]       = pOutChInfo->pitch[2];
            pFormat->dataFormat     = pOutChInfo->dataFormat;
            pFormat->scanFormat     = pOutChInfo->scanFormat;
            pFormat->bpp            = FVID2_BPP_BITS16;
            pFormat->reserved       = NULL;

            if(outId==DEI_LINK_OUT_QUE_DEI_SC)
            {
                pDrvChParams->outMemTypeDei = pOutChInfo->memType;
            }
            if(outId==DEI_LINK_OUT_QUE_VIP_SC)
            {
                pDrvChParams->outMemTypeVip = pOutChInfo->memType;
            }

        }

        pChObj->scCfg[outId].bypass       = FALSE;
        pChObj->scCfg[outId].nonLinear    = FALSE;
        pChObj->scCfg[outId].stripSize    = 0;
        pChObj->scCfg[outId].vsType       = VPS_SC_VST_POLYPHASE;

        pChObj->scCropConfig[outId].cropStartX = 0;
        pChObj->scCropConfig[outId].cropStartY = 0;
        pChObj->scCropConfig[outId].cropWidth  = pInChInfo->width;
        if (pInChInfo->scanFormat == FVID2_SF_INTERLACED)
        {
            pChObj->scCropConfig[outId].cropHeight = pInChInfo->height * 2;
        }
        else
        {
            pChObj->scCropConfig[outId].cropHeight = pInChInfo->height;
        }
    }

    pChObj->deiHqCfg.bypass              = deiBypass;
    pChObj->deiHqCfg.inpMode             = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
    pChObj->deiHqCfg.tempInpEnable       = TRUE;
    pChObj->deiHqCfg.tempInpChromaEnable = TRUE;
    pChObj->deiHqCfg.spatMaxBypass       = FALSE;
    pChObj->deiHqCfg.tempMaxBypass       = FALSE;
    pChObj->deiHqCfg.fldMode             = VPS_DEIHQ_FLDMODE_5FLD;
    pChObj->deiHqCfg.lcModeEnable        = TRUE;
    pChObj->deiHqCfg.mvstmEnable         = TRUE;
    pChObj->deiHqCfg.tnrEnable           = TRUE;
    pChObj->deiHqCfg.snrEnable           = TRUE;
    pChObj->deiHqCfg.sktEnable           = FALSE;
    pChObj->deiHqCfg.chromaEdiEnable     = TRUE;

    pChObj->deiCfg.bypass               = deiBypass;
    pChObj->deiCfg.inpMode              = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
    pChObj->deiCfg.tempInpEnable        = TRUE;
    pChObj->deiCfg.tempInpChromaEnable  = TRUE;
    pChObj->deiCfg.spatMaxBypass        = FALSE;
    pChObj->deiCfg.tempMaxBypass        = FALSE;

    return FVID2_SOK;
}

Int32 DeiLink_drvSetScCoeffs(DeiLink_Obj *pObj, Bool loadAll)
{
    Int32               retVal = FVID2_SOK;
    Vps_ScCoeffParams   coeffPrms;
    Vps_ScLazyLoadingParams lazyLoadingParams;

    if(pObj->loadUpsampleCoeffs)
    {
        Vps_rprintf(" %d: DEI     : Loading Up-scaling Co-effs\n", Clock_getTicks());

        coeffPrms.hScalingSet = VPS_SC_US_SET;
        coeffPrms.vScalingSet = VPS_SC_US_SET;
    }
    else
    {
        Vps_rprintf(" %d: DEI     : Loading Down-scaling Co-effs\n", Clock_getTicks());

        coeffPrms.hScalingSet = VPS_SC_DS_SET_8_16;
        coeffPrms.vScalingSet = VPS_SC_DS_SET_8_16;
    }
    coeffPrms.coeffPtr = NULL;
    coeffPrms.scalarId = VPS_M2M_DEI_SCALAR_ID_DEI_SC;

    lazyLoadingParams.enableLazyLoading = TRUE;
    lazyLoadingParams.enableFilterSelect = TRUE;

    if(loadAll)
    {
        /* Program DEI scalar coefficient - Always used */
        retVal = FVID2_control(
                     pObj->fvidHandle,
                     IOCTL_VPS_SET_COEFFS,
                     &coeffPrms,
                     NULL);
        GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);

        /* Enable Lazy Loading for DEI scalar */
        lazyLoadingParams.scalarId = VPS_M2M_DEI_SCALAR_ID_DEI_SC;
        lazyLoadingParams.enableLazyLoading = TRUE;
        lazyLoadingParams.enableFilterSelect = TRUE;
        retVal = FVID2_control(pObj->fvidHandle,
                    IOCTL_VPS_SC_SET_LAZY_LOADING,
                    &lazyLoadingParams,
                    NULL);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);
    }

    /* Program the second scalar coefficient if needed */
    if (VpsUtils_appM2mDeiIsVipInst(pObj->drvInstId))
    {
        /* Program VIP scalar coefficients */
        coeffPrms.scalarId = VPS_M2M_DEI_SCALAR_ID_VIP_SC;
        retVal = FVID2_control(
                     pObj->fvidHandle,
                     IOCTL_VPS_SET_COEFFS,
                     &coeffPrms,
                     NULL);
        GT_assert( GT_DEFAULT_MASK, FVID2_SOK == retVal);

        /* Enable Lazy Loading for VIP scalar */
        lazyLoadingParams.scalarId = VPS_M2M_DEI_SCALAR_ID_VIP_SC;
        retVal = FVID2_control(pObj->fvidHandle,
                   IOCTL_VPS_SC_SET_LAZY_LOADING,
                   &lazyLoadingParams,
                   NULL);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == retVal);
    }

    return (retVal);
}

Int32 DeiLink_drvCreateFvidObj(DeiLink_Obj *pObj)
{
    Vps_M2mDeiChParams *pChParams;
    DeiLink_ChObj *pChObj;
    UInt32 chId;
    FVID2_CbParams cbParams;

    pObj->drvCreateArgs.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    pObj->drvCreateArgs.numCh = pObj->inQueInfo.numCh;
    pObj->drvCreateArgs.deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;
    pObj->drvCreateArgs.chParams = (const Vps_M2mDeiChParams *)pObj->drvChArgs;
    pObj->drvCreateArgs.isVipScReq = FALSE;

    if (TRUE == pObj->createArgs.enableOut[DEI_LINK_OUT_QUE_VIP_SC])
    {
        pObj->drvCreateArgs.isVipScReq = TRUE;
    }

    for (chId = 0u; chId < pObj->drvCreateArgs.numCh; chId++)
    {
        pChParams   = &pObj->drvChArgs[chId];
        pChObj      = &pObj->chObj[chId];

        pChParams->outFmtDei    = NULL;
        pChParams->outFmtVip    = NULL;
        pChParams->inFmtFldN_1  = NULL;
        pChParams->deiHqCfg     = NULL;
        pChParams->deiCfg       = NULL;
        pChParams->scCfg        = NULL;
        pChParams->deiCropCfg   = NULL;
        pChParams->vipScCfg     = NULL;
        pChParams->vipCropCfg   = NULL;
        pChParams->subFrameParams  = NULL;
#ifdef TI_816X_BUILD
        if (pObj->linkId==SYSTEM_LINK_ID_DEI_HQ_0
            || pObj->linkId==SYSTEM_LINK_ID_DEI_HQ_1)
        {
            pChParams->deiHqCfg = &pChObj->deiHqCfg;

            pObj->drvInstId = VPS_M2M_INST_MAIN_DEIH_SC1_WB0;
            pObj->vipInstId = SYSTEM_VIP_0;

            if(pObj->createArgs.enableOut[DEI_LINK_OUT_QUE_VIP_SC])
            {
                pObj->drvInstId = VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0;
            }
        }

        if (pObj->linkId==SYSTEM_LINK_ID_DEI_0
            || pObj->linkId==SYSTEM_LINK_ID_DEI_1)
        {
            pChParams->deiCfg = &pChObj->deiCfg;

            pObj->drvInstId = VPS_M2M_INST_AUX_DEI_SC2_WB1;
            pObj->vipInstId = SYSTEM_VIP_1;

            if(pObj->createArgs.enableOut[DEI_LINK_OUT_QUE_VIP_SC])
            {
                pObj->drvInstId = VPS_M2M_INST_AUX_DEI_SC2_SC4_WB1_VIP1;
            }
        }
#endif /* TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
        if (pObj->linkId==SYSTEM_LINK_ID_DEI_0
            || pObj->linkId==SYSTEM_LINK_ID_DEI_1)
        {
            pChParams->deiCfg = &pChObj->deiCfg;

            pObj->drvInstId = VPS_M2M_INST_MAIN_DEI_SC1_WB0;
            pObj->vipInstId = SYSTEM_VIP_0;

            if(pObj->createArgs.enableOut[DEI_LINK_OUT_QUE_VIP_SC])
            {
                pObj->drvInstId = VPS_M2M_INST_MAIN_DEI_SC1_SC3_WB0_VIP0;
            }
        }
#endif /* TI_814X_BUILD || TI_8107_BUILD */

        if (pObj->createArgs.enableOut[DEI_LINK_OUT_QUE_DEI_SC])
        {
            pChParams->scCfg      = &pChObj->scCfg[DEI_LINK_OUT_QUE_DEI_SC];
            pChParams->deiCropCfg = &pChObj->scCropConfig[DEI_LINK_OUT_QUE_DEI_SC];
            pChParams->outFmtDei  = &pChObj->outFormat[DEI_LINK_OUT_QUE_DEI_SC];
        }
        if (pObj->createArgs.enableOut[DEI_LINK_OUT_QUE_VIP_SC])
        {
            pChParams->vipScCfg   = &pChObj->scCfg[DEI_LINK_OUT_QUE_VIP_SC];
            pChParams->vipCropCfg = &pChObj->scCropConfig[DEI_LINK_OUT_QUE_VIP_SC];
            pChParams->outFmtVip  = &pChObj->outFormat[DEI_LINK_OUT_QUE_VIP_SC];
        }
    }

    memset(&cbParams, 0, sizeof(cbParams));

    cbParams.cbFxn    = DeiLink_drvFvidCb;
    cbParams.errCbFxn = DeiLink_drvFvidErrCb;
    cbParams.errList  = &pObj->errProcessList;
    cbParams.appData  = pObj;

    pObj->fvidHandle  = FVID2_create(
                            FVID2_VPS_M2M_DEI_DRV,
                            pObj->drvInstId,
                            &pObj->drvCreateArgs,
                            &pObj->drvCreateStatus,
                            &cbParams
                          );
    GT_assert( GT_DEFAULT_MASK, pObj->fvidHandle!=NULL);

    return FVID2_SOK;
}


Int32 DeiLink_drvCreate(DeiLink_Obj *pObj, DeiLink_CreateParams *pPrm)
{
    UInt32 chId;
    Semaphore_Params semParams;

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DEI    : Create in progress !!!\n", Clock_getTicks());
    #endif

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert( GT_DEFAULT_MASK, pObj->pInTskInfo != NULL);
    GT_assert( GT_DEFAULT_MASK, pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    memcpy(&pObj->inQueInfo,
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->inQueInfo)
            );

    pObj->inFrameGetCount               = 0;
    pObj->inFrameSkipCount              = 0;
    pObj->inFramePutCount               = 0;
    pObj->outFrameGetCount[0]           = 0;
    pObj->outFrameGetCount[1]           = 0;
    pObj->outFramePutCount[0]           = 0;
    pObj->outFramePutCount[1]           = 0;
    pObj->processFrameReqPendCount      = 0;
    pObj->processFrameReqPendSubmitCount= 0;
    pObj->processFrameCount             = 0;
    pObj->getProcessFrameCount          = 0;
    pObj->processFrameReqCount          = 0;
    pObj->getProcessFrameReqCount       = 0;
    pObj->totalTime                     = 0;
    pObj->curTime                       = 0;
    pObj->givenInFrames                 = 0x0;
    pObj->returnedInFrames              = 0x0;
    pObj->loadUpsampleCoeffs            = FALSE;

    /* Create semaphores */
    Semaphore_Params_init (&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    pObj->complete = Semaphore_create (0u, &semParams, NULL);
    GT_assert( GT_DEFAULT_MASK, pObj->complete != NULL);


    DeiLink_drvCreateOutObj(pObj);

    for(chId=0; chId<pObj->inQueInfo.numCh; chId++)
        DeiLink_drvCreateChObj(pObj, chId);

    DeiLink_drvCreateFvidObj(pObj);
    VpsUtils_appAllocDeiCtxBuffer(pObj->fvidHandle, pObj->drvCreateArgs.numCh);
    DeiLink_drvSetScCoeffs(pObj, TRUE);
    DeiLink_drvCreateReqObj(pObj);

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DEI    : Create Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}

Int32 DeiLink_drvQueueFramesToChQue(DeiLink_Obj *pObj)
{
    UInt32 frameId, freeFrameNum;
    FVID2_Frame *pFrame;
    System_LinkInQueParams *pInQueParams;
    FVID2_FrameList frameList;
    DeiLink_ChObj *pChObj;
    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    System_getLinksFullFrames(pInQueParams->prevLinkId, pInQueParams->prevLinkQueId, &frameList);

    if(frameList.numFrames)
    {
        #ifdef SYSTEM_DEBUG_DEI_RT
        Vps_printf(" %d: DEI    : Received %d IN frames !!!\n", Clock_getTicks(), frameList.numFrames);
        #endif

        pObj->inFrameGetCount+=frameList.numFrames;

        freeFrameNum = 0;

        for(frameId=0; frameId < frameList.numFrames; frameId++)
        {
            pFrame = frameList.frames[frameId];

            pChObj = &pObj->chObj[pFrame->channelNum];

            //pFrame->fid = pChObj->nextFid;

            if(pChObj->nextFid == pFrame->fid)
            {
                // frame is of the expected FID use it, else drop it
                status = VpsUtils_bufPutFullFrame(&pChObj->inQue, pFrame);
                GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

                pChObj->nextFid ^= 1; // toggle to next required FID
            }
            else
            {
                pObj->inFrameSkipCount++;

                // frame is not of expected FID, so release frame
                frameList.frames[freeFrameNum] = pFrame;
                freeFrameNum++;
            }
        }

        if(freeFrameNum)
        {
            frameList.numFrames = freeFrameNum;

            #ifdef SYSTEM_DEBUG_DEI_RT
            Vps_printf(" %d: DEI    : Skipped %d IN frames !!!\n", Clock_getTicks(), frameList.numFrames);
            #endif

            pObj->inFramePutCount+=freeFrameNum;

            System_putLinksEmptyFrames(pInQueParams->prevLinkId, pInQueParams->prevLinkQueId, &frameList);
        }
    }

    return FVID2_SOK;
}

Int32 DeiLink_drvMakeFrameLists(DeiLink_Obj *pObj, FVID2_FrameList *inFrameList, FVID2_FrameList outFrameList[DEI_LINK_MAX_OUT_QUE])
{
    DeiLink_ChObj *pChObj;
    UInt32 chId, outId, frameId;
    FVID2_Frame *pInFrame, *pOutFrame;
    Int32 status;
    Bool doFrameDrop;

    frameId=0;

    for(chId=0; chId<pObj->inQueInfo.numCh; chId++)
    {
        pChObj = &pObj->chObj[chId];

        VpsUtils_bufGetFullFrame(&pChObj->inQue, &pInFrame, BIOS_NO_WAIT);

        if(pInFrame)
        {
            GT_assert(GT_DEFAULT_MASK, frameId < FVID2_MAX_FVID_FRAME_PTR);
            inFrameList->frames[frameId] = pInFrame;

            for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
            {
                pOutFrame = NULL;

                if(pObj->createArgs.enableOut[outId])
                {
                    doFrameDrop = FALSE;

                    if(pObj->createArgs.skipAlternateFrames)
                    {
                        if( pInFrame->fid==1)
                        {
                            doFrameDrop = TRUE;
                        }
                    }

                    if(doFrameDrop)
                    {
                        pOutFrame = &pObj->outFrameDrop;
                    }
                    else
                    {
                        status = VpsUtils_bufGetEmptyFrame(
                                        &pObj->outObj[outId].bufOutQue,
                                        &pOutFrame,
                                        BIOS_WAIT_FOREVER
                                    );
                        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
                        GT_assert( GT_DEFAULT_MASK, pOutFrame != NULL);

                        pObj->outFrameGetCount[outId]++;
                    }

                    pOutFrame->channelNum = pInFrame->channelNum;


                }
                outFrameList[outId].frames[frameId] = pOutFrame;
            }
            frameId++;
        }
    }

    inFrameList->numFrames = frameId;
    for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
        outFrameList[outId].numFrames = frameId;

    return FVID2_SOK;
}
#ifdef TI_816X_BUILD
Int32 DeiLink_drvReleaseFrames(DeiLink_Obj *pObj, FVID2_FrameList *inFrameList, FVID2_FrameList outFrameList[DEI_LINK_MAX_OUT_QUE])
{
    UInt32 frameId, outId, sendCmd[DEI_LINK_MAX_OUT_QUE];
    FVID2_Frame *pFrame;
    System_LinkInQueParams *pInQueParams;
    Int32 status;

    pInQueParams = &pObj->createArgs.inQueParams;

    for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
        sendCmd[outId] = FALSE;

    for(frameId=0; frameId<inFrameList->numFrames; frameId++)
    {
        for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
        {
            if(pObj->createArgs.enableOut[outId])
            {
                pFrame = outFrameList[outId].frames[frameId];

                if(pFrame && pFrame != &pObj->outFrameDrop )
                {
                    status = VpsUtils_bufPutFullFrame(
                                &pObj->outObj[outId].bufOutQue,
                                pFrame
                        );
                    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

                    sendCmd[outId] = TRUE;

                    pObj->outFramePutCount[outId]++;
                }
            }
        }
    }

    #ifdef SYSTEM_DEBUG_DEI_RT
    Vps_printf(" %d: DEI    : Released %d IN frames !!!\n", Clock_getTicks(), inFrameList->numFrames);
    #endif

    pObj->inFramePutCount+=inFrameList->numFrames;

    System_putLinksEmptyFrames(pInQueParams->prevLinkId, pInQueParams->prevLinkQueId, inFrameList);

    for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
    {
        if(sendCmd[outId])
        {
            System_sendLinkCmd(
                pObj->createArgs.outQueParams[outId].nextLink,
                SYSTEM_CMD_NEW_DATA
                );
        }
    }

    return FVID2_SOK;
}
#endif /* TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
Int32 DeiLink_drvReleaseFrames(DeiLink_Obj *pObj, FVID2_FrameList *inFrameList, FVID2_FrameList outFrameList[DEI_LINK_MAX_OUT_QUE])
{
    UInt32 frameId, outId, sendCmd[DEI_LINK_MAX_OUT_QUE];
    FVID2_Frame *pFrame;
    System_LinkInQueParams *pInQueParams;
    Int32 status;
    Int32 actualFrameIdx;

    pInQueParams = &pObj->createArgs.inQueParams;

    for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
        sendCmd[outId] = FALSE;

    for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
    {
        if(pObj->createArgs.enableOut[outId])
        {
            for(frameId=0; frameId<outFrameList[outId].numFrames; frameId++)
            {
                pFrame = outFrameList[outId].frames[frameId];

                if((pFrame) && (pFrame != &pObj->outFrameDrop ))
                {
                    status = VpsUtils_bufPutFullFrame(
                                &pObj->outObj[outId].bufOutQue,
                                pFrame
                        );
                    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

                    sendCmd[outId] = TRUE;

                    pObj->outFramePutCount[outId]++;
                }
            }
        }
    }

    if (inFrameList->numFrames > 0x0)
    {
        #ifdef SYSTEM_DEBUG_DEI_RT
        Vps_printf(" %d: DEI    : Released %d IN frames !!!\n",
            Clock_getTicks(), inFrameList->numFrames);
        #endif

        /* There could be holes in the frame list, as the driver could have
           decided to hold back couple of frames as context, ensure the frame
           list is compacted */
        actualFrameIdx = -1;
        for (frameId = 0x0; frameId < inFrameList->numFrames; frameId++)
        {
            if (NULL != inFrameList->frames[frameId])
            {
                actualFrameIdx++;
                inFrameList->frames[actualFrameIdx] =
                    inFrameList->frames[frameId];
            }
            else
            {
                /* Do nothing */
                continue;
            }
        }

        if (actualFrameIdx != -1)
        {
            inFrameList->numFrames = actualFrameIdx + 0x01u;
            pObj->inFramePutCount+=inFrameList->numFrames;
            pObj->returnedInFrames += inFrameList->numFrames;

            System_putLinksEmptyFrames(pInQueParams->prevLinkId,
                                    pInQueParams->prevLinkQueId, inFrameList);
        }
    }

    for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
    {
        if(sendCmd[outId])
        {
            System_sendLinkCmd(
                pObj->createArgs.outQueParams[outId].nextLink,
                SYSTEM_CMD_NEW_DATA
                );
        }
    }

    return FVID2_SOK;
}
#endif /* TI_814X_BUILD || TI_8107_BUILD */

Int32 DeiLink_drvSubmitData(DeiLink_Obj *pObj)
{
    Int32               status;
    DeiLink_ReqObj     *pReqObj;

    status = VpsUtils_queGet(&pObj->reqQue, (Ptr*)&pReqObj, 1, BIOS_NO_WAIT);
    if(status!=FVID2_SOK)
    {
        #ifdef SYSTEM_DEBUG_DEI_RT
        Vps_printf(" %d: DEI    : Pending request !!!\n", Clock_getTicks());
        #endif

        pObj->processFrameReqPendCount++;

        pObj->isReqPend = TRUE;

        // cannot process more frames to process
        return FVID2_EFAIL;
    }

    if(pObj->processFrameReqPendCount==pObj->processFrameReqPendSubmitCount)
        pObj->isReqPend = FALSE;

    pReqObj->processList.inFrameList[0] = &pReqObj->inFrameList;
    pReqObj->processList.outFrameList[0] = &pReqObj->outFrameList[0];
    pReqObj->processList.outFrameList[1] = &pReqObj->outFrameList[1];
    pReqObj->processList.numInLists = 1;
    pReqObj->processList.numOutLists = pObj->reqNumOutLists;

    pReqObj->inFrameList.appData = pReqObj;

    DeiLink_drvMakeFrameLists(pObj, &pReqObj->inFrameList, pReqObj->outFrameList);

    #ifdef SYSTEM_DEBUG_DEI_RT
    Vps_printf(" %d: DEI    : Submitting %d frames !!!\n", Clock_getTicks(), pReqObj->inFrameList.numFrames);
    #endif

    if(pReqObj->inFrameList.numFrames)
    {
        pObj->reqQueCount++;

        pObj->processFrameCount += pReqObj->inFrameList.numFrames;
        pObj->processFrameReqCount++;

        pObj->givenInFrames += pReqObj->inFrameList.numFrames;

        System_lockVip(pObj->vipInstId);

        if(System_clearVipResetFlag(pObj->vipInstId))
        {
            /* VIP was reset since last frame processing, so we need to
             * reload VIP-SC co-effs */
            DeiLink_drvSetScCoeffs(pObj, FALSE);
        }

        pObj->curTime = Clock_getTicks();
        status = FVID2_processFrames(pObj->fvidHandle, &pReqObj->processList);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        #ifndef DEI_LINK_QUEUE_REQ
        Semaphore_pend(pObj->complete, BIOS_WAIT_FOREVER);

        DeiLink_drvGetProcessedData(pObj);;
        #endif

        System_unlockVip(pObj->vipInstId);
    }
    else
    {

        status = VpsUtils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        // no more frames to process
        status = FVID2_EFAIL;
    }

    return status;
}

Int32 DeiLink_drvProcessData(DeiLink_Obj *pObj)
{
    Int32 status;

    DeiLink_drvQueueFramesToChQue(pObj);

    do
    {
        status = DeiLink_drvSubmitData(pObj);
    } while(status==FVID2_SOK);

    return FVID2_SOK;
}

Int32 DeiLink_drvGetProcessedData(DeiLink_Obj *pObj)
{
    DeiLink_ReqObj *pReqObj;
    Int32 status;
    FVID2_ProcessList processList;

    status = FVID2_getProcessedFrames(pObj->fvidHandle, &processList, BIOS_NO_WAIT);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->curTime = Clock_getTicks() - pObj->curTime;
    pObj->totalTime += pObj->curTime;

    #ifdef SYSTEM_DEBUG_DEI_RT
    Vps_printf(" %d: DEI    : Completed %d frames !!!\n", Clock_getTicks(), processList.outFrameList[0]->numFrames);
    #endif

    pObj->getProcessFrameCount += processList.outFrameList[0]->numFrames;
    pObj->getProcessFrameReqCount++;

    pReqObj = (DeiLink_ReqObj*)processList.inFrameList[0]->appData;

    DeiLink_drvReleaseFrames(pObj, &pReqObj->inFrameList, pReqObj->outFrameList);

    status = VpsUtils_quePut(&pObj->reqQue, pReqObj, BIOS_NO_WAIT);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->reqQueCount--;

    if(pObj->isReqPend)
    {
        #ifdef SYSTEM_DEBUG_DEI_RT
        Vps_printf(" %d: DEI    : Submitting pending request !!!\n", Clock_getTicks());
        #endif

        pObj->processFrameReqPendSubmitCount++;

        DeiLink_drvSubmitData(pObj);
    }
    return FVID2_SOK;
}

Int32 DeiLink_drvStop(DeiLink_Obj *pObj)
{
    Int32   rtnValue = FVID2_SOK;

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DEI    : Stop in progress, %d requests pending !!!\n", Clock_getTicks(), pObj->reqQueCount);
    #endif

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    rtnValue = FVID2_stop(pObj->fvidHandle, NULL);
    if (rtnValue != FVID2_SOK)
    {
        #ifdef SYSTEM_DEBUG_DEI
        Vps_printf(" %d: DEI    : Stop Fails !!!\n", Clock_getTicks());
        #endif
    }
#endif /* TI_814X_BUILD || TI_8107_BUILD */

    while(pObj->reqQueCount)
    {
        VpsUtils_tskWaitCmd(&pObj->tsk, NULL, DEI_LINK_CMD_GET_PROCESSED_DATA);
        DeiLink_drvGetProcessedData(pObj);
    }
    /* Even though all the requests are addressed, the driver would have held
        back couple of input fields as context fields, get them */
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    if ((pObj->returnedInFrames - pObj->givenInFrames) != 0x0)
    {
        DeiLink_drvReleaseContextField(pObj);
    }
#endif /* TI_814X_BUILD || TI_8107_BUILD */
    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DEI    : Stop Done !!!\n", Clock_getTicks());
    #endif

    return (rtnValue);
}

Int32 DeiLink_drvDelete(DeiLink_Obj *pObj)
{
    UInt32 outId, chId;
    DeiLink_ChObj *pChObj;
    DeiLink_OutObj *pOutObj;
    Bool tilerUsed = FALSE;

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DEI    : Fields = %d (fps = %d) !!!\n",
        Clock_getTicks(),
        pObj->getProcessFrameCount,
        pObj->getProcessFrameCount*100/(pObj->totalTime/10)
        );
    #endif

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DEI    : Delete in progress !!!\n", Clock_getTicks());
    #endif

    VpsUtils_appFreeDeiCtxBuffer(pObj->fvidHandle, pObj->drvCreateArgs.numCh);

    FVID2_delete(pObj->fvidHandle, NULL);

    /* Delete semaphores */
    Semaphore_delete(&pObj->complete);

    for(outId=0; outId<DEI_LINK_MAX_OUT_QUE; outId++)
    {
        {
            pOutObj = &pObj->outObj[outId];

            VpsUtils_bufDelete(&pOutObj->bufOutQue);

            if(pObj->createArgs.tilerEnable && outId==DEI_LINK_OUT_QUE_VIP_SC && pOutObj->outFormat.dataFormat == FVID2_DF_YUV420SP_UV)
            {
                tilerUsed = TRUE;
            }
            else
            {
                VpsUtils_memFrameFree(&pOutObj->outFormat, pOutObj->outFrames, pOutObj->outNumFrames);
            }
        }
    }

    if (tilerUsed)
    {
        VpsUtils_tilerFreeAll ();
    }

    for(chId=0; chId<DEI_LINK_MAX_CH; chId++)
    {
        pChObj = &pObj->chObj[chId];

        VpsUtils_bufDelete(&pChObj->inQue);
    }

    VpsUtils_queDelete(&pObj->reqQue);

    #ifdef SYSTEM_DEBUG_DEI
    Vps_printf(" %d: DEI    : Delete Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
static Int32 DeiLink_drvReleaseContextField(DeiLink_Obj *pObj)
{
    Int32 status, index;
    FVID2_ProcessList processList;
    static FVID2_FrameList outFrameList[DEI_LINK_MAX_OUT_QUE];

    for (index = 0x0u; index<DEI_LINK_MAX_OUT_QUE; index++)
    {
        outFrameList[index].numFrames = 0x0u;
    }

    status = FVID2_getProcessedFrames(pObj->fvidHandle, &processList, BIOS_NO_WAIT);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    GT_assert( GT_DEFAULT_MASK, processList.numOutLists == 0x0u);

    DeiLink_drvReleaseFrames(pObj, processList.inFrameList[0], outFrameList);

    return FVID2_SOK;
}
#endif /* TI_814X_BUILD || TI_8107_BUILD */
