/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_deiHal.c
 *
 *  \brief VPS DEI Path Core internal file containing HAL related functions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/core/src/vpscore_deiPriv.h>
#include <ti/psp/vps/vps_advCfgDeiHq.h>


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

static Int32 vcoreDeiSetHalCfg(const Vcore_DeiInstObj *instObj,
                               Vcore_DeiChObj *chObj);
static Int32 vcoreDeiSetChrusHalCfg(Vcore_DeiChObj *chObj);
static Int32 vcoreDeiSetComprHalCfg(Vcore_DeiChObj *chObj);
static Int32 vcoreDeiSetDcomprHalCfg(Vcore_DeiChObj *chObj);
static Int32 vcoreDeiSetDeiHqHalCfg(const Vcore_DeiInstObj *instObj,
                                    Vcore_DeiChObj *chObj);
static Int32 vcoreDeiSetDeiHalCfg(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj);
static Int32 vcoreDeiSetDrnHalCfg(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj);
static Int32 vcoreDeiSetScHalCfg(const Vcore_DeiInstObj *instObj,
                                 Vcore_DeiChObj *chObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vcoreDeiInitHalParams
 *  \brief Sets the HAL overlay pointers and function pointers.
 */
void vcoreDeiInitHalParams(Vcore_DeiInstObj *instObj,
                           const Vcore_DeiInitParams *initPrms)
{
    UInt32          ovlyOffset;
    UInt32          tempIdx, chIdx;
    UInt32          halCnt, fsCnt;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != initPrms));

    /* Assign get overlay size function pointer */
    instObj->getCfgOvlySize[VCORE_DEI_CHRUS0_IDX] =
        VpsHal_chrusGetConfigOvlySize;
    instObj->getCfgOvlySize[VCORE_DEI_CHRUS1_IDX] =
        VpsHal_chrusGetConfigOvlySize;
    instObj->getCfgOvlySize[VCORE_DEI_CHRUS2_IDX] =
        VpsHal_chrusGetConfigOvlySize;
#ifdef VPS_HAL_INCLUDE_DRN
    instObj->getCfgOvlySize[VCORE_DEI_DRN_IDX] =
        VpsHal_drnGetConfigOvlySize;
#else
    instObj->getCfgOvlySize[VCORE_DEI_DRN_IDX] = NULL;
#endif
    if (TRUE == instObj->isHqDei)
    {
#ifdef VPS_HAL_INCLUDE_DEIH
        instObj->getCfgOvlySize[VCORE_DEI_DEI_IDX] =
            VpsHal_deihGetConfigOvlySize;
#else
        instObj->getCfgOvlySize[VCORE_DEI_DEI_IDX] = NULL;
#endif
    }
    else
    {
        instObj->getCfgOvlySize[VCORE_DEI_DEI_IDX] =
            VpsHal_deiGetConfigOvlySize;
    }
    instObj->getCfgOvlySize[VCORE_DEI_SC_IDX] = VpsHal_scGetConfigOvlySize;
#ifdef VPS_HAL_INCLUDE_COMPR
    instObj->getCfgOvlySize[VCORE_DEI_COMPR0_IDX] =
        VpsHal_comprGetConfigOvlySize;
    instObj->getCfgOvlySize[VCORE_DEI_COMPR1_IDX] =
        VpsHal_comprGetConfigOvlySize;
#else
    instObj->getCfgOvlySize[VCORE_DEI_COMPR0_IDX] = NULL;
    instObj->getCfgOvlySize[VCORE_DEI_COMPR1_IDX] = NULL;
#endif
#ifdef VPS_HAL_INCLUDE_DCOMPR
    instObj->getCfgOvlySize[VCORE_DEI_DCOMPR0_IDX] =
        VpsHal_dcomprGetConfigOvlySize;
    instObj->getCfgOvlySize[VCORE_DEI_DCOMPR1_IDX] =
        VpsHal_dcomprGetConfigOvlySize;
    instObj->getCfgOvlySize[VCORE_DEI_DCOMPR2_IDX] =
        VpsHal_dcomprGetConfigOvlySize;
#else
    instObj->getCfgOvlySize[VCORE_DEI_DCOMPR0_IDX] = NULL;
    instObj->getCfgOvlySize[VCORE_DEI_DCOMPR1_IDX] = NULL;
    instObj->getCfgOvlySize[VCORE_DEI_DCOMPR2_IDX] = NULL;
#endif

    /* Assign create overlay function pointer */
    instObj->createCfgOvly[VCORE_DEI_CHRUS0_IDX] =
        VpsHal_chrusCreateConfigOvly;
    instObj->createCfgOvly[VCORE_DEI_CHRUS1_IDX] =
        VpsHal_chrusCreateConfigOvly;
    instObj->createCfgOvly[VCORE_DEI_CHRUS2_IDX] =
        VpsHal_chrusCreateConfigOvly;
#ifdef VPS_HAL_INCLUDE_DRN
    instObj->createCfgOvly[VCORE_DEI_DRN_IDX] = VpsHal_drnCreateConfigOvly;
#else
    instObj->createCfgOvly[VCORE_DEI_DRN_IDX] = NULL;
#endif
    if (TRUE == instObj->isHqDei)
    {
#ifdef VPS_HAL_INCLUDE_DEIH
        instObj->createCfgOvly[VCORE_DEI_DEI_IDX] = VpsHal_deihCreateConfigOvly;
#else
        instObj->createCfgOvly[VCORE_DEI_DEI_IDX] = NULL;
#endif
    }
    else
    {
        instObj->createCfgOvly[VCORE_DEI_DEI_IDX] =
            VpsHal_deiCreateConfigOvly;
    }
    instObj->createCfgOvly[VCORE_DEI_SC_IDX] = VpsHal_scCreateConfigOvly;
#ifdef VPS_HAL_INCLUDE_COMPR
    instObj->createCfgOvly[VCORE_DEI_COMPR0_IDX] = VpsHal_comprCreateConfigOvly;
    instObj->createCfgOvly[VCORE_DEI_COMPR1_IDX] = VpsHal_comprCreateConfigOvly;
#else
    instObj->createCfgOvly[VCORE_DEI_COMPR0_IDX] = NULL;
    instObj->createCfgOvly[VCORE_DEI_COMPR1_IDX] = NULL;
#endif
#ifdef VPS_HAL_INCLUDE_DCOMPR
    instObj->createCfgOvly[VCORE_DEI_DCOMPR0_IDX] =
        VpsHal_dcomprCreateConfigOvly;
    instObj->createCfgOvly[VCORE_DEI_DCOMPR1_IDX] =
        VpsHal_dcomprCreateConfigOvly;
    instObj->createCfgOvly[VCORE_DEI_DCOMPR2_IDX] =
        VpsHal_dcomprCreateConfigOvly;
#else
    instObj->createCfgOvly[VCORE_DEI_DCOMPR0_IDX] = NULL;
    instObj->createCfgOvly[VCORE_DEI_DCOMPR1_IDX] = NULL;
    instObj->createCfgOvly[VCORE_DEI_DCOMPR2_IDX] = NULL;
#endif

    /* Initialize HAL variables */
    ovlyOffset = 0u;
    for (halCnt = 0u; halCnt < VCORE_DEI_MAX_HAL; halCnt++)
    {
        /* Get the HAL handle */
        instObj->halHandle[halCnt] = initPrms->halHandle[halCnt];

        instObj->ovlyOffset[halCnt] = ovlyOffset;
        /* Get the overlay size for each of the modules */
        if (NULL != instObj->halHandle[halCnt])
        {
            instObj->halShadowOvlySize[halCnt] =
                instObj->getCfgOvlySize[halCnt](instObj->halHandle[halCnt]);
            ovlyOffset += instObj->halShadowOvlySize[halCnt];
        }
        else
        {
            instObj->halShadowOvlySize[halCnt] = 0u;
        }
    }

    /* Initialize the register offsets for the frame start registers
     * which are used to form the VPDMA configuration overlay */
    instObj->numFsEvtReg = 0u;
    for (halCnt = 0u; halCnt < VCORE_DEI_MAX_CHR_US; halCnt++)
    {
        tempIdx = halCnt + VCORE_DEI_CHRUS0_IDX;
        if (NULL != instObj->halHandle[tempIdx])
        {
            instObj->numFsEvtReg += VCORE_DEI_NUM_DESC_PER_FIELD;
            for (fsCnt = 0u; fsCnt < VCORE_DEI_NUM_DESC_PER_FIELD; fsCnt++)
            {
                if (VCORE_DEI_CHRUS0_IDX == tempIdx)
                {
                    chIdx = VCORE_DEI_FLD0LUMA_IDX + fsCnt;
                    instObj->fsRegOffset[fsCnt +
                        (halCnt * VCORE_DEI_NUM_DESC_PER_FIELD)] =
                        VpsHal_vpdmaGetClientRegAdd(instObj->vpdmaCh[chIdx]);
                }
                else
                {
                    chIdx = VCORE_DEI_FLD1LUMA_IDX + fsCnt +
                        ((halCnt - 1u) * VCORE_DEI_NUM_DESC_PER_FIELD);
                    instObj->fsRegOffset[fsCnt +
                        (halCnt * VCORE_DEI_NUM_DESC_PER_FIELD)] =
                        VpsHal_vpdmaGetClientRegAdd(instObj->vpdmaCh[chIdx]);
                }
            }
        }
    }

    /* Get the size of the overlay for DEI registers and the relative
     * virtual offset for the above registers when VPDMA config register
     * overlay is formed */
    instObj->fsCfgOvlySize = VpsHal_vpdmaCalcRegOvlyMemSize(
                                 instObj->fsRegOffset,
                                 instObj->numFsEvtReg,
                                 instObj->fsVirRegOffset);

    return;
}



/**
 *  vcoreDeiSetChParams
 *  \brief Sets the channel parameter.
 */
Int32 vcoreDeiSetChParams(const Vcore_DeiInstObj *instObj,
                          Vcore_DeiChObj *chObj,
                          const Vcore_DeiParams *corePrms)
{
    Int32           retVal = VPS_SOK;
    UInt32          descOffset;
    UInt32          vpdmaChCnt;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != corePrms));

    /* Copy the params to the local structure */
    VpsUtils_memcpy(&chObj->corePrms, corePrms, sizeof(Vcore_DeiParams));

    /* Figure out the DEI state depending up on the DEI parameter */
    if (TRUE == instObj->isHqDei)
    {
        retVal = vcoreDeiHqFigureOutState(instObj, chObj, corePrms);
    }
    else
    {
        retVal = vcoreDeiFigureOutState(instObj, chObj, corePrms);
    }

    if (VPS_SOK == retVal)
    {
        /* Set the configuration for each of the HALs */
        retVal = vcoreDeiSetHalCfg(instObj, chObj);
    }

    if (VPS_SOK == retVal)
    {
        /* Dummy data descriptor is not needed if frame size and buffer
         * bottom-right coordinates match */
        chObj->numExtraDesc = 0u;
        if (((corePrms->startX + corePrms->fmt.width) < corePrms->frameWidth) ||
            ((corePrms->startY + corePrms->fmt.height) < corePrms->frameHeight))
        {
            chObj->isDummyNeeded = TRUE;
            chObj->numExtraDesc += instObj->numDescPerWindow;
        }
        else
        {
            chObj->isDummyNeeded = FALSE;
            if (VCORE_OPMODE_DISPLAY == instObj->curMode)
            {
                /* Always assume dummy is required for display mode as
                 * during runtime it might be needed when width/height or
                 * startX/startY changes. */
                chObj->numExtraDesc += instObj->numDescPerWindow;
            }
        }

        /* Add data descriptor for SOCH and abort of actual channel
         * in display mode */
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            chObj->numExtraDesc += instObj->numDescPerWindow;
        }

        /* Set the channel information depending on DEIs requirement. */
        vcoreDeiSetDescInfo(
            instObj,
            chObj,
            &chObj->descInfo,
            instObj->numDescPerWindow,
            chObj->numExtraDesc);

        /* Figure out the descriptor offset for inbound descriptors */
        descOffset = 0u;
        for (vpdmaChCnt = VCORE_DEI_START_IN_CH;
             vpdmaChCnt < (VCORE_DEI_START_IN_CH + VCORE_DEI_MAX_IN_CH);
             vpdmaChCnt++)
        {
            chObj->descOffset[vpdmaChCnt] = descOffset;
            /* Add the offset only when the programming is required */
            if (TRUE == chObj->isDescReq[vpdmaChCnt])
            {
                descOffset++;
            }
        }

        /* Figure out the descriptor offset for outbound descriptors */
        descOffset = 0u;
        for (vpdmaChCnt = VCORE_DEI_START_OUT_CH;
             vpdmaChCnt < (VCORE_DEI_START_OUT_CH + VCORE_DEI_MAX_OUT_CH);
             vpdmaChCnt++)
        {
            chObj->descOffset[vpdmaChCnt] = descOffset;
            /* Add the offset only when the programming is required */
            if (TRUE == chObj->isDescReq[vpdmaChCnt])
            {
                descOffset++;
            }
        }

        /* Figure out the Y and C buffer index based on data format */
        if (FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat)
        {
            /* YUV 422 interleaved format - C buffer is also same as Y */
            chObj->bufIndex[VCORE_DEI_Y_IDX] = FVID2_YUV_INT_ADDR_IDX;
            chObj->bufIndex[VCORE_DEI_CBCR_IDX] = FVID2_YUV_INT_ADDR_IDX;
        }
        else
        {
            /* YUV 420/422 semi-planar format */
            chObj->bufIndex[VCORE_DEI_Y_IDX] = FVID2_YUV_SP_Y_ADDR_IDX;
            chObj->bufIndex[VCORE_DEI_CBCR_IDX] =
                FVID2_YUV_SP_CBCR_ADDR_IDX;
        }

        /* Set proper state - parameter is set */
        chObj->state.isParamsSet = TRUE;
    }

    return (retVal);
}



/**
 *  vcoreDeiSetDescInfo
 *  \brief Sets the descriptor information based on the parameters set.
 */
Int32 vcoreDeiSetDescInfo(const Vcore_DeiInstObj *instObj,
                          const Vcore_DeiChObj *chObj,
                          Vcore_DescInfo *descInfo,
                          UInt32 numFirstRowDesc,
                          UInt32 numMultiDesc)
{
    Int32           retVal = VPS_SOK;
    UInt32          halCnt;
    UInt32          vpdmaChCnt, socIdx;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descInfo));

    /* Calculate the number of inbound descriptors required */
    descInfo->numInDataDesc =
        numFirstRowDesc +
        (chObj->ctxInfo.numPrevFld * VCORE_DEI_NUM_DESC_PER_FIELD) +
        chObj->ctxInfo.numMvIn;
    /* If the MVSTM loop is needed, it needs two input buffers,
     * so context information indicates two buffers but number
     * of in bound descriptors is one only for MVSTM loop.
     * This should be removed if mvstm itself needs two input
     * descriptors. */
    if (chObj->ctxInfo.numMvstmIn > 0u)
    {
        if (VPS_DEIHQ_CTXMODE_APP_N_1 == chObj->corePrms.deiHqCtxMode)
        {
            descInfo->numInDataDesc += chObj->ctxInfo.numMvstmIn;
        }
        else
        {
            descInfo->numInDataDesc += (chObj->ctxInfo.numMvstmIn - 1u);
        }
    }
    descInfo->numMultiWinDataDesc = numMultiDesc;

    /* Calculate the number of outbound descriptors required */
    descInfo->numOutDataDesc =
        (chObj->ctxInfo.numCurFldOut * VCORE_DEI_NUM_DESC_PER_FIELD) +
        chObj->ctxInfo.numMvOut +
        chObj->ctxInfo.numMvstmOut;

    /* Calculate the size of shadow overlay memory required by adding
     * the shadow overlay size of each HAL */
    descInfo->shadowOvlySize = 0u;
    for (halCnt = 0u; halCnt < VCORE_DEI_MAX_HAL; halCnt++)
    {
        descInfo->shadowOvlySize += instObj->halShadowOvlySize[halCnt];
    }

    /* Only in M2M mode, FS event will be programmed through overlay */
    if (VCORE_OPMODE_MEMORY == instObj->curMode)
    {
        descInfo->nonShadowOvlySize = instObj->fsCfgOvlySize;
    }
    else
    {
        descInfo->nonShadowOvlySize = 0u;
    }

    /* Get the horizontal, vertical and bilinear (if applicable) coeff
     * overlay sizes if scalar HAL is present.
     */
    if (NULL != instObj->halHandle[VCORE_DEI_SC_IDX])
    {
        VpsHal_scGetCoeffOvlySize(instObj->halHandle[VCORE_DEI_SC_IDX],
                                  &(descInfo->horzCoeffOvlySize),
                                  &(descInfo->vertCoeffOvlySize),
                                  &(descInfo->vertBilinearCoeffOvlySize));
        descInfo->coeffConfigDest = VpsHal_scGetVpdmaConfigDest(
                                        instObj->halHandle[VCORE_DEI_SC_IDX]);
    }
    else
    {
        descInfo->horzCoeffOvlySize = 0u;
        descInfo->vertCoeffOvlySize = 0u;
        descInfo->vertBilinearCoeffOvlySize = 0u;
    }

    /*
     * Assign the channels that needs to be used for SOCH descriptors.
     *
     * H/W Bug:
     * -------
     * If the video input data to the compressor for the next
     * frame starts before the compressor has completed sending out the
     * last frame, it will lock up. When it locks up, the input request
     * for the compressor goes low and stays low. This will cause the
     * current luma and chroma channels to lock up.
     *
     * Workaround:
     * ----------
     * Put a "Sync on Channel compress outputs" at the end of each
     * channel descriptors to send data to DEI. This will ensure that
     * the next field of video to send to DEI won’t start until
     * compress has completed sending the previous field out. Hence
     * using SOCH for all the possible input and output channels.
     *
     * Also when compression is enabled, driver should process the write
     * descriptor written to memory (to know the compressed data size)
     * only after VPDMA completes it. Hence SOCH is required on the
     * write channels to avoid this race condition.
     * Note: When compression is disabled, this may not be needed. But
     * still using it for uniformity.
     *
     * Otherwise SOCH on the main input channels and the output channels
     * is sufficient.
     */
    socIdx = 0u;
    for (vpdmaChCnt = 0u; vpdmaChCnt < VCORE_DEI_MAX_VPDMA_CH; vpdmaChCnt++)
    {
        if (TRUE == chObj->isDescReq[vpdmaChCnt])
        {
            descInfo->socChNum[socIdx++] = instObj->vpdmaCh[vpdmaChCnt];
        }
    }
    descInfo->numChannels = socIdx;

    return (retVal);
}



/**
 *  vcoreDeiSetFsEvent
 *  \brief Sets the frame start event to the descriptor memory or directly
 *  to the VPDMA CSTAT registers.
 */
Int32 vcoreDeiSetFsEvent(const Vcore_DeiInstObj *instObj,
                         const Vcore_DeiChObj *chObj,
                         const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chCnt;
    VpsHal_VpdmaChannel     vpdmaCh;
    VpsHal_VpdmaLineMode    lineMode[VCORE_DEI_NUM_DESC_PER_FIELD];

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Figure out line mode based on data format and scan format */
#ifdef VPS_HAL_INCLUDE_DRN
    if ((FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat) ||
        (FVID2_DF_YUV422SP_UV == chObj->corePrms.fmt.dataFormat))
    {
        if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
        {
            /* YUV422 data to CHR_US and DEI is in deinterlacing mode or
             * in interlaced bypass mode */
            lineMode[VCORE_DEI_Y_IDX] = VPSHAL_VPDMA_LM_1;
            lineMode[VCORE_DEI_CBCR_IDX] = VPSHAL_VPDMA_LM_1;
        }
        else
        {
            /* YUV422 data to CHR_US and DEI is in progressive bypass mode */
            lineMode[VCORE_DEI_Y_IDX] = VPSHAL_VPDMA_LM_3;
            lineMode[VCORE_DEI_CBCR_IDX] = VPSHAL_VPDMA_LM_3;
        }
    }
    else
    {
        if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
        {
            /* YUV420 data to CHR_US and DEI is in deinterlacing mode or
             * in interlaced bypass mode */
            lineMode[VCORE_DEI_Y_IDX] = VPSHAL_VPDMA_LM_1;
            lineMode[VCORE_DEI_CBCR_IDX] = VPSHAL_VPDMA_LM_0;
        }
        else
        {
            /* YUV420 data to CHR_US and DEI is in progressive bypass mode */
            lineMode[VCORE_DEI_Y_IDX] = VPSHAL_VPDMA_LM_3;
            lineMode[VCORE_DEI_CBCR_IDX] = VPSHAL_VPDMA_LM_2;
        }
    }
#else
    if ((FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat) ||
        (FVID2_DF_YUV422SP_UV == chObj->corePrms.fmt.dataFormat))
    {
        /* YUV422 data to CHR_US and DEI is in deinterlacing mode or
         * in interlaced bypass mode. Progressive bypass not supported.
         * Line mode for luma is reserved, set to 0 */
        lineMode[VCORE_DEI_Y_IDX] = VPSHAL_VPDMA_LM_0;
        lineMode[VCORE_DEI_CBCR_IDX] = VPSHAL_VPDMA_LM_1;
    }
    else
    {
        /* YUV420 data to CHR_US and DEI is in deinterlacing mode or
         * in interlaced bypass mode. Progressive bypass not supported.
         * Line mode for luma is reserved, set to 0 */
        lineMode[VCORE_DEI_Y_IDX] = VPSHAL_VPDMA_LM_0;
        lineMode[VCORE_DEI_CBCR_IDX] = VPSHAL_VPDMA_LM_0;
    }
#endif

    /* Program the frame start event of Y and CbCr channels in
     * overlay memory if requested for */
    if ((NULL != descMem) && (NULL != descMem->nonShadowOvlyMem))
    {
        /* Create overlay memory for VPDMA frame start event registers */
        retVal = VpsHal_vpdmaCreateRegOverlay(
                     instObj->fsRegOffset,
                     instObj->numFsEvtReg,
                     descMem->nonShadowOvlyMem);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Create Overlay Failed for VPDMA FS Event Reg!!\n");
        }
        else
        {
            for (chCnt = 0u; chCnt < instObj->numFsEvtReg; chCnt++)
            {
                if (chCnt < VCORE_DEI_NUM_DESC_PER_FIELD)
                {
                    vpdmaCh =
                        instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + chCnt];
                }
                else
                {
                    vpdmaCh =
                        instObj->vpdmaCh[VCORE_DEI_FLD1LUMA_IDX +
                            (chCnt - VCORE_DEI_NUM_DESC_PER_FIELD)];
                }

                /* Program the frame start event */
                VpsHal_vpdmaSetFrameStartEvent(
                    vpdmaCh,
                    chObj->fsEvent,
                    lineMode[chCnt % VCORE_DEI_NUM_DESC_PER_FIELD],
                    0u,
                    descMem->nonShadowOvlyMem,
                    instObj->fsVirRegOffset[chCnt]);
            }
        }
    }
    else
    {
        /*
         * Program it directly in VPDMA regsiter
         */
        /* Program the frame start event for all inbound channels.
         * This is not required for outbound channels. In fact
         * for outbound channels, frame start should not be programmed
         * as the frame start of outbound channel is used by display
         * controller/DLM!! */
        for (chCnt = VCORE_DEI_START_IN_CH;
             chCnt < (VCORE_DEI_START_IN_CH + VCORE_DEI_MAX_IN_CH);
             chCnt++)
        {
            /* Set the frame start only if the descriptor is required.
             * This way it also makes sure that it programs frame start
             * event for the DEI_M channels correctly */
            if (TRUE == chObj->isDescReq[chCnt])
            {
                VpsHal_vpdmaSetFrameStartEvent(
                    instObj->vpdmaCh[chCnt],
                    chObj->fsEvent,
                    VPSHAL_VPDMA_LM_0,
                    0u,
                    NULL,
                    0u);
            }
        }

        /* Program the correct frame start event for the required
         * channels. */
        for (chCnt = 0u; chCnt < instObj->numFsEvtReg; chCnt++)
        {
            if (chCnt < VCORE_DEI_NUM_DESC_PER_FIELD)
            {
                vpdmaCh =
                    instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + chCnt];
            }
            else
            {
                vpdmaCh =
                    instObj->vpdmaCh[VCORE_DEI_FLD1LUMA_IDX +
                        (chCnt - VCORE_DEI_NUM_DESC_PER_FIELD)];
            }
            /* Program the frame start event */
            VpsHal_vpdmaSetFrameStartEvent(
                vpdmaCh,
                chObj->fsEvent,
                lineMode[chCnt % VCORE_DEI_NUM_DESC_PER_FIELD],
                0u,
                NULL,
                0u);
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiProgramReg
 *  \brief Programs all the HAL registers either through direct register writes
 *  or update them in overlay memory.
 */
Int32 vcoreDeiProgramReg(const Vcore_DeiInstObj *instObj,
                         const Vcore_DeiChObj *chObj,
                         const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  cnt, tempIdx;
    Void                   *cfgOvlyPtr = NULL;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Configure Chrus module */
    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < VCORE_DEI_MAX_CHR_US; cnt++)
        {
            tempIdx = cnt + VCORE_DEI_CHRUS0_IDX;
            if (NULL != instObj->halHandle[tempIdx])
            {
                cfgOvlyPtr = NULL;
                if (NULL != descMem)
                {
                    cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                        + instObj->ovlyOffset[tempIdx]);
                }
                retVal = VpsHal_chrusSetConfig(
                            instObj->halHandle[tempIdx],
                            &chObj->chrusHalCfg,
                            cfgOvlyPtr);
                if (VPS_SOK != retVal)
                {
                    GT_1trace(DeiCoreTrace, GT_ERR,
                        "CHRUS(%d) HAL Config Overlay Failed!!\n", cnt);
                    break;
                }
            }
        }
    }

    /* Configure compressor modules */
    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < VCORE_DEI_MAX_COMPR; cnt++)
        {
            tempIdx = cnt + VCORE_DEI_COMPR0_IDX;
            if (NULL != instObj->halHandle[tempIdx])
            {
                cfgOvlyPtr = NULL;
                if (NULL != descMem)
                {
                    cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                        + instObj->ovlyOffset[tempIdx]);
                }
#ifdef VPS_HAL_INCLUDE_COMPR
                retVal = VpsHal_comprSetConfig(
                             instObj->halHandle[tempIdx],
                             &chObj->comprHalCfg[cnt],
                             cfgOvlyPtr);
#else
                retVal = VPS_SOK;
#endif
                if (VPS_SOK != retVal)
                {
                    GT_1trace(DeiCoreTrace, GT_ERR,
                        "COMPR(%d) HAL Config Overlay Failed!!\n", cnt);
                    break;
                }
            }
        }
    }

    /* Configure decompressor modules */
    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < VCORE_DEI_MAX_DCOMPR; cnt++)
        {
            tempIdx = cnt + VCORE_DEI_DCOMPR0_IDX;
            if (NULL != instObj->halHandle[tempIdx])
            {
                cfgOvlyPtr = NULL;
                if (NULL != descMem)
                {
                    cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                        + instObj->ovlyOffset[tempIdx]);
                }
#ifdef VPS_HAL_INCLUDE_DCOMPR
                retVal = VpsHal_dcomprSetConfig(
                             instObj->halHandle[tempIdx],
                             &chObj->dcomprHalCfg[cnt],
                             cfgOvlyPtr);
#else
                retVal = VPS_SOK;
#endif
                if (VPS_SOK != retVal)
                {
                    GT_1trace(DeiCoreTrace, GT_ERR,
                        "DECOMP(%d) HAL Config Overlay Failed!!\n", cnt);
                    break;
                }
            }
        }
    }

    /* Configure deinterlacer module */
    if (VPS_SOK == retVal)
    {
        if (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX])
        {
            if (NULL != descMem)
            {
                cfgOvlyPtr = NULL;
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_DEI_IDX]);
            }
            if (TRUE == instObj->isHqDei)
            {
#ifdef VPS_HAL_INCLUDE_DEIH
                retVal = VpsHal_deihSetConfig(
                             instObj->halHandle[VCORE_DEI_DEI_IDX],
                             &chObj->deihHalCfg,
                             cfgOvlyPtr);
#else
                retVal = VPS_SOK;
#endif
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "DEIH HAL Config Overlay Failed!!\n");
                }
            }
            else
            {
                retVal = VpsHal_deiSetConfig(
                             instObj->halHandle[VCORE_DEI_DEI_IDX],
                             &chObj->deiHalCfg,
                             cfgOvlyPtr);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "DEI HAL Config Overlay Failed!!\n");
                }
            }
        }
    }

    /* Configure DRN module */
    if (VPS_SOK == retVal)
    {
        if (NULL != instObj->halHandle[VCORE_DEI_DRN_IDX])
        {
            cfgOvlyPtr = NULL;
            if (NULL != descMem)
            {
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_DRN_IDX]);
            }
#ifdef VPS_HAL_INCLUDE_DRN
            retVal = VpsHal_drnSetConfig(
                         instObj->halHandle[VCORE_DEI_DRN_IDX],
                         &chObj->drnHalCfg,
                         cfgOvlyPtr);
#else
            retVal = VPS_SOK;
#endif
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "DRN HAL Config Overlay Failed!!\n");
            }
        }
    }

    /* Configure scalar module */
    if (VPS_SOK == retVal)
    {
        if (NULL != instObj->halHandle[VCORE_DEI_SC_IDX])
        {
            cfgOvlyPtr = NULL;
            if (NULL != descMem)
            {
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_SC_IDX]);
            }
            retVal = VpsHal_scSetConfig(
                         instObj->halHandle[VCORE_DEI_SC_IDX],
                         &chObj->scHalCfg,
                         cfgOvlyPtr);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "SCALAR HAL Config Overlay Failed!!\n");
            }
        }
    }

    return (retVal);
}


/**
 *  vcoreDeiProgramReg
 *  \brief Programs all the HAL registers either through direct register writes
 *  or update them in overlay memory.
 */
Int32 vcoreDeiProgramFrmSizeReg(const Vcore_DeiInstObj *instObj,
                                const Vcore_DeiChObj *chObj,
                                const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  cnt, tempIdx;
    Void                   *cfgOvlyPtr = NULL;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Configure Chrus module */
#if 0
    /* Caution: There is no input format change, so no change is required
        But when DEI is tunered on or off at run time, this requires change.
        Currently there is no support for DEI. */
    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < VCORE_DEI_MAX_CHR_US; cnt++)
        {
            tempIdx = cnt + VCORE_DEI_CHRUS0_IDX;
            if (NULL != instObj->halHandle[tempIdx])
            {
                cfgOvlyPtr = NULL;
                if (NULL != descMem)
                {
                    cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                        + instObj->ovlyOffset[tempIdx]);
                }
                retVal = VpsHal_chrusSetConfig(
                            instObj->halHandle[tempIdx],
                            &chObj->chrusHalCfg,
                            cfgOvlyPtr);
                if (VPS_SOK != retVal)
                {
                    GT_1trace(DeiCoreTrace, GT_ERR,
                        "CHRUS(%d) HAL Config Overlay Failed!!\n", cnt);
                    break;
                }
            }
        }
    }
#endif

    /* Configure compressor modules */
    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < VCORE_DEI_MAX_COMPR; cnt++)
        {
            tempIdx = cnt + VCORE_DEI_COMPR0_IDX;
            if (NULL != instObj->halHandle[tempIdx])
            {
                cfgOvlyPtr = NULL;
                if (NULL != descMem)
                {
                    cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                        + instObj->ovlyOffset[tempIdx]);
                }
#ifdef VPS_HAL_INCLUDE_COMPR
                retVal = VpsHal_comprSetConfig(
                             instObj->halHandle[tempIdx],
                             &chObj->comprHalCfg[cnt],
                             cfgOvlyPtr);
#else
                retVal = VPS_SOK;
#endif
                if (VPS_SOK != retVal)
                {
                    GT_1trace(DeiCoreTrace, GT_ERR,
                        "COMPR(%d) HAL Config Overlay Failed!!\n", cnt);
                    break;
                }
            }
        }
    }

    /* Configure decompressor modules */
    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < VCORE_DEI_MAX_DCOMPR; cnt++)
        {
            tempIdx = cnt + VCORE_DEI_DCOMPR0_IDX;
            if (NULL != instObj->halHandle[tempIdx])
            {
                cfgOvlyPtr = NULL;
                if (NULL != descMem)
                {
                    cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                        + instObj->ovlyOffset[tempIdx]);
                }
#ifdef VPS_HAL_INCLUDE_DCOMPR
                retVal = VpsHal_dcomprSetConfig(
                             instObj->halHandle[tempIdx],
                             &chObj->dcomprHalCfg[cnt],
                             cfgOvlyPtr);
#else
                retVal = VPS_SOK;
#endif
                if (VPS_SOK != retVal)
                {
                    GT_1trace(DeiCoreTrace, GT_ERR,
                        "DECOMP(%d) HAL Config Overlay Failed!!\n", cnt);
                    break;
                }
            }
        }
    }

    /* Configure deinterlacer module */
    if (VPS_SOK == retVal)
    {
        if (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX])
        {
            if (NULL != descMem)
            {
                cfgOvlyPtr = NULL;
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_DEI_IDX]);
            }
            if (TRUE == instObj->isHqDei)
            {
#ifdef VPS_HAL_INCLUDE_DEIH
                retVal = VpsHal_deihSetFrameSize(
                             instObj->halHandle[VCORE_DEI_DEI_IDX],
                             &chObj->deihHalCfg,
                             cfgOvlyPtr);
#else
                retVal = VPS_SOK;
#endif
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "DEIH HAL Config Overlay Failed!!\n");
                }
            }
            else
            {
                retVal = VpsHal_deiSetFrameSize(
                             instObj->halHandle[VCORE_DEI_DEI_IDX],
                             &chObj->deiHalCfg,
                             cfgOvlyPtr);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "DEI HAL Config Overlay Failed!!\n");
                }
            }
        }
    }

    /* Configure DRN module */
    if (VPS_SOK == retVal)
    {
        if (NULL != instObj->halHandle[VCORE_DEI_DRN_IDX])
        {
            cfgOvlyPtr = NULL;
            if (NULL != descMem)
            {
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_DRN_IDX]);
            }
#ifdef VPS_HAL_INCLUDE_DRN
            retVal = VpsHal_drnSetConfig(
                         instObj->halHandle[VCORE_DEI_DRN_IDX],
                         &chObj->drnHalCfg,
                         cfgOvlyPtr);
#else
            retVal = VPS_SOK;
#endif
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "DRN HAL Config Overlay Failed!!\n");
            }
        }
    }

    /* Configure scalar module */
    if (VPS_SOK == retVal)
    {
        if (NULL != instObj->halHandle[VCORE_DEI_SC_IDX])
        {
            cfgOvlyPtr = NULL;
            if (NULL != descMem)
            {
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_SC_IDX]);
            }
            retVal = VpsHal_scSetConfig(
                         instObj->halHandle[VCORE_DEI_SC_IDX],
                         &chObj->scHalCfg,
                         cfgOvlyPtr);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "SCALAR HAL Config Overlay Failed!!\n");
            }
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiHqGetAdvCfgIoctl
 *  \brief Reads the advance DEI HQ configuration.
 */
Int32 vcoreDeiHqGetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               Vps_DeiHqRdWrAdvCfg *advCfg)
{
    Int32                    retVal = VPS_SOK;
#ifdef VPS_HAL_INCLUDE_DEIH
    Vcore_DeiChObj          *chObj;
    Vcore_DeiInstObj        *instObj;
    VpsHal_DeihConfig       *deihHalCfg;
    VpsHal_DeihMdtMiscConfig miscCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));
    GT_assert(DeiCoreTrace, (NULL != advCfg));
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX]));
    chObj = hObj->chObjs[advCfg->chNum];
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Get the configuration from the local structure */
    deihHalCfg = &chObj->deihHalCfg;

    advCfg->bypass = deihHalCfg->bypass;
    advCfg->width = deihHalCfg->width;
    advCfg->height = deihHalCfg->height;
    advCfg->scanFormat = deihHalCfg->scanFormat;
    advCfg->tempInpEnable = deihHalCfg->tempInpEnable;
    advCfg->tempInpChromaEnable = deihHalCfg->tempInpChromaEnable;

    /* Copy MDT configuration */
    advCfg->mdtCfg.fldMode = deihHalCfg->mdt.fldMode;
    advCfg->mdtCfg.spatMaxBypass = deihHalCfg->mdt.spatMaxBypass;
    advCfg->mdtCfg.tempMaxBypass = deihHalCfg->mdt.tempMaxBypass;
    advCfg->mdtCfg.lcModeEnable = deihHalCfg->mdt.lcModeEnable;
    advCfg->mdtCfg.mvIIR = deihHalCfg->mdt.mvIIR;

    retVal = VpsHal_deihGetMdtMiscConfig(
                instObj->halHandle[VCORE_DEI_DEI_IDX],
                &miscCfg);
    /* Copy Misc configuration */
    advCfg->mdtCfg.adaptiveCoring = miscCfg.adaptiveCoring;
    advCfg->mdtCfg.fldComp = miscCfg.fldComp;
    advCfg->mdtCfg.edgeKd = miscCfg.edgeKd;
    advCfg->mdtCfg.edgeVarThrld = miscCfg.edgeVarThrld;
    advCfg->mdtCfg.forceSlomoDiff = miscCfg.forceSlomoDiff;
    advCfg->mdtCfg.forceF02Diff = miscCfg.forceF02Diff;
    advCfg->mdtCfg.forceMv3D = miscCfg.forceMv3D;
    advCfg->mdtCfg.forceMv2D = miscCfg.forceMv2D;
    advCfg->mdtCfg.useDynGain = miscCfg.useDynGain;
    advCfg->mdtCfg.enable3PixelFilt = miscCfg.enable3PixelFilt;

    /* Copy EDI configuration */
    advCfg->ediCfg.inpMode = deihHalCfg->edi.inpMode;
    advCfg->ediCfg.chromaEdiEnable = deihHalCfg->edi.chromaEdiEnable;

    /* Copy TNR configuration */
    advCfg->tnrCfg.luma = deihHalCfg->tnr.luma;
    advCfg->tnrCfg.chroma = deihHalCfg->tnr.chroma;
    advCfg->tnrCfg.adaptive = deihHalCfg->tnr.adaptive;
    advCfg->tnrCfg.advMode = deihHalCfg->tnr.advMode;
    advCfg->tnrCfg.advScaleFactor = deihHalCfg->tnr.advScaleFactor;
    advCfg->tnrCfg.skt = deihHalCfg->tnr.skt;
    advCfg->tnrCfg.maxSktGain = deihHalCfg->tnr.maxSktGain;
    advCfg->tnrCfg.minChromaSktThrld = deihHalCfg->tnr.minChromaSktThrld;

    /* Copy FMD configuration */
    advCfg->fmdCfg.filmMode = deihHalCfg->fmd.filmMode;
    advCfg->fmdCfg.bed = deihHalCfg->fmd.bed;
    advCfg->fmdCfg.window = deihHalCfg->fmd.window;
    advCfg->fmdCfg.lock = deihHalCfg->fmd.lock;
    advCfg->fmdCfg.jamDir = deihHalCfg->fmd.jamDir;
    advCfg->fmdCfg.windowMinx = deihHalCfg->fmd.windowMinx;
    advCfg->fmdCfg.windowMiny = deihHalCfg->fmd.windowMiny;
    advCfg->fmdCfg.windowMaxx = deihHalCfg->fmd.windowMaxx;
    advCfg->fmdCfg.windowMaxy = deihHalCfg->fmd.windowMaxy;

    /* Copy SNR configuration */
    advCfg->snrCfg.gnrChroma = deihHalCfg->snr.gnrChroma;
    advCfg->snrCfg.gnrLuma = deihHalCfg->snr.gnrLuma;
    advCfg->snrCfg.gnrAdaptive = deihHalCfg->snr.gnrAdaptive;
    advCfg->snrCfg.inrMode = deihHalCfg->snr.inrMode;
    advCfg->snrCfg.gnmDevStableThrld = deihHalCfg->snr.gnmDevStableThrld;

    /* Get the configuration from the HAL */
    retVal |= VpsHal_deiHqGetAdvConfig(
                instObj->halHandle[VCORE_DEI_DEI_IDX],
                advCfg);
#endif

    return (retVal);
}



/**
 *  vcoreDeiHqSetAdvCfgIoctl
 *  \brief Writes the advance DEI HQ configuration.
 */
Int32 vcoreDeiHqSetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               const Vps_DeiHqRdWrAdvCfg *advCfg)
{
    Int32                    retVal = VPS_SOK;
#ifdef VPS_HAL_INCLUDE_DEIH
    Vcore_DeiInstObj        *instObj;
    Vcore_DeiChObj          *chObj;
    VpsHal_DeihConfig       *deihHalCfg;
    VpsHal_DeihMdtMiscConfig miscCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));
    GT_assert(DeiCoreTrace, (NULL != advCfg));
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX]));
    chObj = hObj->chObjs[advCfg->chNum];
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Get the configuration from the local structure */
    deihHalCfg = &chObj->deihHalCfg;

    deihHalCfg->bypass = advCfg->bypass;
    deihHalCfg->width = advCfg->width;
    deihHalCfg->height = advCfg->height;
    deihHalCfg->scanFormat = advCfg->scanFormat;
    deihHalCfg->tempInpEnable = advCfg->tempInpEnable;
    deihHalCfg->tempInpChromaEnable = advCfg->tempInpChromaEnable;

    /* Copy MDT configuration */
    deihHalCfg->mdt.fldMode = advCfg->mdtCfg.fldMode;
    deihHalCfg->mdt.spatMaxBypass = advCfg->mdtCfg.spatMaxBypass;
    deihHalCfg->mdt.tempMaxBypass = advCfg->mdtCfg.tempMaxBypass;
    deihHalCfg->mdt.lcModeEnable = advCfg->mdtCfg.lcModeEnable;
    deihHalCfg->mdt.mvIIR = advCfg->mdtCfg.mvIIR;

    /* Copy Misc configuration */
    miscCfg.adaptiveCoring = advCfg->mdtCfg.adaptiveCoring;
    miscCfg.fldComp = advCfg->mdtCfg.fldComp;
    miscCfg.edgeKd = advCfg->mdtCfg.edgeKd;
    miscCfg.edgeVarThrld = advCfg->mdtCfg.edgeVarThrld;
    miscCfg.forceSlomoDiff = advCfg->mdtCfg.forceSlomoDiff;
    miscCfg.forceF02Diff = advCfg->mdtCfg.forceF02Diff;
    miscCfg.forceMv3D = advCfg->mdtCfg.forceMv3D;
    miscCfg.forceMv2D = advCfg->mdtCfg.forceMv2D;
    miscCfg.useDynGain = advCfg->mdtCfg.useDynGain;
    miscCfg.enable3PixelFilt = advCfg->mdtCfg.enable3PixelFilt;
    retVal = VpsHal_deihSetMdtMiscConfig(
                instObj->halHandle[VCORE_DEI_DEI_IDX],
                &miscCfg,
                NULL);

    /* Copy EDI configuration */
    deihHalCfg->edi.inpMode = advCfg->ediCfg.inpMode;
    deihHalCfg->edi.chromaEdiEnable = advCfg->ediCfg.chromaEdiEnable;

    /* Copy TNR configuration */
    deihHalCfg->tnr.luma = advCfg->tnrCfg.luma;
    deihHalCfg->tnr.chroma = advCfg->tnrCfg.chroma;
    deihHalCfg->tnr.adaptive = advCfg->tnrCfg.adaptive;
    deihHalCfg->tnr.advMode = advCfg->tnrCfg.advMode;
    deihHalCfg->tnr.advScaleFactor = advCfg->tnrCfg.advScaleFactor;
    deihHalCfg->tnr.skt = advCfg->tnrCfg.skt;
    deihHalCfg->tnr.maxSktGain = advCfg->tnrCfg.maxSktGain;
    deihHalCfg->tnr.minChromaSktThrld = advCfg->tnrCfg.minChromaSktThrld;

    /* Copy FMD configuration */
    deihHalCfg->fmd.filmMode = advCfg->fmdCfg.filmMode;
    deihHalCfg->fmd.bed = advCfg->fmdCfg.bed;
    deihHalCfg->fmd.window = advCfg->fmdCfg.window;
    deihHalCfg->fmd.lock = advCfg->fmdCfg.lock;
    deihHalCfg->fmd.jamDir = advCfg->fmdCfg.jamDir;
    deihHalCfg->fmd.windowMinx = advCfg->fmdCfg.windowMinx;
    deihHalCfg->fmd.windowMiny = advCfg->fmdCfg.windowMiny;
    deihHalCfg->fmd.windowMaxx = advCfg->fmdCfg.windowMaxx;
    deihHalCfg->fmd.windowMaxy = advCfg->fmdCfg.windowMaxy;

    /* Copy SNR configuration */
    deihHalCfg->snr.gnrChroma = advCfg->snrCfg.gnrChroma;
    deihHalCfg->snr.gnrLuma = advCfg->snrCfg.gnrLuma;
    deihHalCfg->snr.gnrAdaptive = advCfg->snrCfg.gnrAdaptive;
    deihHalCfg->snr.inrMode = advCfg->snrCfg.inrMode;
    deihHalCfg->snr.gnmDevStableThrld = advCfg->snrCfg.gnmDevStableThrld;

    /* Set the advance configuration */
    retVal |= VpsHal_deiHqSetAdvConfig(
                instObj->halHandle[VCORE_DEI_DEI_IDX],
                advCfg,
                NULL);
#endif

    return (retVal);
}



/**
 *  vcoreDeiGetAdvCfgIoctl
 *  \brief Reads the advance DEI configuration.
 */
Int32 vcoreDeiGetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                             Vps_DeiRdWrAdvCfg *advCfg)
{
    Int32                    retVal = VPS_SOK;
    Vcore_DeiChObj          *chObj;
    Vcore_DeiInstObj        *instObj;
    VpsHal_DeiConfig        *deiHalCfg;
    VpsHal_DeiFmdStatus      fmdStatus;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));
    GT_assert(DeiCoreTrace, (NULL != advCfg));
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX]));
    chObj = hObj->chObjs[advCfg->chNum];
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Get the advance configuration */
    retVal |= VpsHal_deiGetAdvConfig(
                  instObj->halHandle[VCORE_DEI_DEI_IDX],
                  advCfg);

    /* For all other configuration get them from the channel object. */
    deiHalCfg = &chObj->deiHalCfg;
    advCfg->bypass = deiHalCfg->bypass;
    advCfg->width = deiHalCfg->width;
    advCfg->height = deiHalCfg->height;
    advCfg->scanFormat = deiHalCfg->scanFormat;
    advCfg->fieldFlush = deiHalCfg->fieldFlush;

    /* Copy MDT configuration */
    advCfg->mdtCfg.spatMaxBypass = deiHalCfg->mdt.spatMaxBypass;
    advCfg->mdtCfg.tempMaxBypass = deiHalCfg->mdt.tempMaxBypass;

    /* Copy EDI configuration */
    advCfg->ediCfg.inpMode = deiHalCfg->edi.inpMode;
    advCfg->ediCfg.tempInpEnable = deiHalCfg->edi.tempInpEnable;
    advCfg->ediCfg.tempInpChromaEnable = deiHalCfg->edi.tempInpChromaEnable;

    /* Copy FMD configuration */
    advCfg->fmdCfg.filmMode = deiHalCfg->fmd.filmMode;
    advCfg->fmdCfg.bed = deiHalCfg->fmd.bed;
    advCfg->fmdCfg.window = deiHalCfg->fmd.window;
    advCfg->fmdCfg.lock = deiHalCfg->fmd.lock;
    advCfg->fmdCfg.jamDir = deiHalCfg->fmd.jamDir;
    advCfg->fmdCfg.windowMinx = deiHalCfg->fmd.windowMinx;
    advCfg->fmdCfg.windowMiny = deiHalCfg->fmd.windowMiny;
    advCfg->fmdCfg.windowMaxx = deiHalCfg->fmd.windowMaxx;
    advCfg->fmdCfg.windowMaxy = deiHalCfg->fmd.windowMaxy;

    /* Read FMD status */
    retVal |= VpsHal_deiGetFmdStatus(
                  instObj->halHandle[VCORE_DEI_DEI_IDX],
                  &fmdStatus);
    if (VPS_SOK == retVal)
    {
        /* Copy FMD status */
        advCfg->fmdCfg.frameDiff = fmdStatus.frameDiff;
        advCfg->fmdCfg.fldDiff = fmdStatus.fldDiff;
        advCfg->fmdCfg.reset = fmdStatus.reset;
        advCfg->fmdCfg.caf = fmdStatus.caf;
    }

    return (retVal);
}



/**
 *  vcoreDeiSetAdvCfgIoctl
 *  \brief Writes the advance DEI configuration.
 */
Int32 vcoreDeiSetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                             const Vps_DeiRdWrAdvCfg *advCfg)
{
    Int32                    retVal = VPS_SOK;
    Vcore_DeiChObj          *chObj;
    Vcore_DeiInstObj        *instObj;
    VpsHal_DeiConfig        *deiHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));
    GT_assert(DeiCoreTrace, (NULL != advCfg));
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX]));
    chObj = hObj->chObjs[advCfg->chNum];
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Set the advance configuration */
    retVal |= VpsHal_deiSetAdvConfig(
                  instObj->halHandle[VCORE_DEI_DEI_IDX],
                  advCfg,
                  NULL);

    /* For all other configuration set them to the channel object. */
    deiHalCfg = &chObj->deiHalCfg;
    deiHalCfg->bypass = advCfg->bypass;
    deiHalCfg->width = advCfg->width;
    deiHalCfg->height = advCfg->height;
    deiHalCfg->scanFormat = advCfg->scanFormat;
    deiHalCfg->fieldFlush = advCfg->fieldFlush;

    /* Copy MDT configuration */
    deiHalCfg->mdt.spatMaxBypass = advCfg->mdtCfg.spatMaxBypass;
    deiHalCfg->mdt.tempMaxBypass = advCfg->mdtCfg.tempMaxBypass;

    /* Copy EDI configuration */
    deiHalCfg->edi.inpMode = advCfg->ediCfg.inpMode;
    deiHalCfg->edi.tempInpEnable = advCfg->ediCfg.tempInpEnable;
    deiHalCfg->edi.tempInpChromaEnable = advCfg->ediCfg.tempInpChromaEnable;

    /* Copy FMD configuration */
    deiHalCfg->fmd.filmMode = advCfg->fmdCfg.filmMode;
    deiHalCfg->fmd.bed = advCfg->fmdCfg.bed;
    deiHalCfg->fmd.window = advCfg->fmdCfg.window;
    deiHalCfg->fmd.lock = advCfg->fmdCfg.lock;
    deiHalCfg->fmd.jamDir = advCfg->fmdCfg.jamDir;
    deiHalCfg->fmd.windowMinx = advCfg->fmdCfg.windowMinx;
    deiHalCfg->fmd.windowMiny = advCfg->fmdCfg.windowMiny;
    deiHalCfg->fmd.windowMaxx = advCfg->fmdCfg.windowMaxx;
    deiHalCfg->fmd.windowMaxy = advCfg->fmdCfg.windowMaxy;

    return (retVal);
}



/**
 *  vcoreDeiScGetAdvCfgIoctl
 *  \brief Reads the advance scalar configuration.
 */
Int32 vcoreDeiScGetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               Vcore_ScCfgParams *scCfgPrms)
{
    Int32                   retVal = VPS_EFAIL;
    Void                   *cfgOvlyPtr = NULL;
    Vcore_DeiInstObj       *instObj;
    VpsHal_ScConfig         scHalCfg;
    Vps_ScPeakingConfig     peakingCfg;
    Vps_ScEdgeDetectConfig  edgeDetectCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));
    GT_assert(DeiCoreTrace, (NULL != scCfgPrms));
    GT_assert(DeiCoreTrace, (NULL != scCfgPrms->scAdvCfg));
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    if (NULL != instObj->halHandle[VCORE_DEI_SC_IDX])
    {
        cfgOvlyPtr = NULL;
        if (NULL != scCfgPrms->descMem)
        {
            cfgOvlyPtr = (Void *) ((UInt32) scCfgPrms->descMem->shadowOvlyMem
                + instObj->ovlyOffset[VCORE_DEI_SC_IDX]);
        }

        /* Get the scalar configuration from HAL */
        scHalCfg.peakingCfg = &peakingCfg;
        scHalCfg.edgeDetectCfg = &edgeDetectCfg;
        retVal = VpsHal_scGetConfig(
                     instObj->halHandle[VCORE_DEI_SC_IDX],
                     &scHalCfg,
                     cfgOvlyPtr);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "SCALAR HAL Get Config Overlay Failed!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Copy the configuration */
        scCfgPrms->scAdvCfg->tarWidth = scHalCfg.tarWidth;
        scCfgPrms->scAdvCfg->tarHeight = scHalCfg.tarHeight;
        scCfgPrms->scAdvCfg->srcWidth = scHalCfg.srcWidth;
        scCfgPrms->scAdvCfg->srcHeight = scHalCfg.srcHeight;
        scCfgPrms->scAdvCfg->cropStartX = scHalCfg.cropStartX;
        scCfgPrms->scAdvCfg->cropStartY = scHalCfg.cropStartY;
        scCfgPrms->scAdvCfg->cropWidth = scHalCfg.cropWidth;
        scCfgPrms->scAdvCfg->cropHeight = scHalCfg.cropHeight;
        scCfgPrms->scAdvCfg->inFrameMode = scHalCfg.inFrameMode;
        scCfgPrms->scAdvCfg->outFrameMode = scHalCfg.outFrameMode;
        scCfgPrms->scAdvCfg->hsType = scHalCfg.hsType;
        scCfgPrms->scAdvCfg->nonLinear = scHalCfg.nonLinear;
        scCfgPrms->scAdvCfg->stripSize = scHalCfg.stripSize;
        scCfgPrms->scAdvCfg->vsType = scHalCfg.vsType;
        scCfgPrms->scAdvCfg->fidPol = scHalCfg.fidPol;
        scCfgPrms->scAdvCfg->selfGenFid = scHalCfg.selfGenFid;
        scCfgPrms->scAdvCfg->defConfFactor = scHalCfg.defConfFactor;
        scCfgPrms->scAdvCfg->biLinIntpType = scHalCfg.biLinIntpType;
        scCfgPrms->scAdvCfg->enableEdgeDetect = scHalCfg.enableEdgeDetect;
        scCfgPrms->scAdvCfg->hPolyBypass = scHalCfg.hPolyBypass;
        scCfgPrms->scAdvCfg->enablePeaking = scHalCfg.enablePeaking;
        scCfgPrms->scAdvCfg->bypass = scHalCfg.bypass;
        scCfgPrms->scAdvCfg->rowAccInc = scHalCfg.rowAccInc;
        scCfgPrms->scAdvCfg->rowAccOffset = scHalCfg.rowAccOffset;
        scCfgPrms->scAdvCfg->rowAccOffsetB = scHalCfg.rowAccOffsetB;
        scCfgPrms->scAdvCfg->ravScFactor = scHalCfg.ravScFactor;
        scCfgPrms->scAdvCfg->ravRowAccInit = scHalCfg.ravRowAccInit;
        scCfgPrms->scAdvCfg->ravRowAccInitB = scHalCfg.ravRowAccInitB;
        if (NULL != scCfgPrms->scAdvCfg->peakingCfg)
        {
            VpsUtils_memcpy(
                scCfgPrms->scAdvCfg->peakingCfg,
                scHalCfg.peakingCfg,
                sizeof(Vps_ScPeakingConfig));
        }
        if (NULL != scCfgPrms->scAdvCfg->edgeDetectCfg)
        {
            VpsUtils_memcpy(
                scCfgPrms->scAdvCfg->edgeDetectCfg,
                scHalCfg.edgeDetectCfg,
                sizeof(Vps_ScEdgeDetectConfig));
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiScSetAdvCfgIoctl
 *  \brief Writes the advance scalar configuration.
 */
Int32 vcoreDeiScSetAdvCfgIoctl(Vcore_DeiHandleObj *hObj,
                               const Vcore_ScCfgParams *scCfgPrms)
{
    Int32                   retVal = VPS_EFAIL;
    Void                   *cfgOvlyPtr = NULL;
    Vcore_DeiInstObj       *instObj;
    VpsHal_ScConfig         scHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));
    GT_assert(DeiCoreTrace, (NULL != scCfgPrms));
    GT_assert(DeiCoreTrace, (NULL != scCfgPrms->scAdvCfg));
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    if (NULL != instObj->halHandle[VCORE_DEI_SC_IDX])
    {
        /* Copy the configuration */
        scHalCfg.tarWidth = scCfgPrms->scAdvCfg->tarWidth;
        scHalCfg.tarHeight = scCfgPrms->scAdvCfg->tarHeight;
        scHalCfg.srcWidth = scCfgPrms->scAdvCfg->srcWidth;
        scHalCfg.srcHeight = scCfgPrms->scAdvCfg->srcHeight;
        scHalCfg.cropStartX = scCfgPrms->scAdvCfg->cropStartX;
        scHalCfg.cropStartY = scCfgPrms->scAdvCfg->cropStartY;
        scHalCfg.cropWidth = scCfgPrms->scAdvCfg->cropWidth;
        scHalCfg.cropHeight = scCfgPrms->scAdvCfg->cropHeight;
        scHalCfg.inFrameMode =
            (Vps_ScanFormat) scCfgPrms->scAdvCfg->inFrameMode;
        scHalCfg.outFrameMode =
            (Vps_ScanFormat) scCfgPrms->scAdvCfg->outFrameMode;
        scHalCfg.hsType = scCfgPrms->scAdvCfg->hsType;
        scHalCfg.nonLinear = scCfgPrms->scAdvCfg->nonLinear;
        scHalCfg.stripSize = scCfgPrms->scAdvCfg->stripSize;
        scHalCfg.vsType = scCfgPrms->scAdvCfg->vsType;
        scHalCfg.fidPol = (Vps_FidPol) scCfgPrms->scAdvCfg->fidPol;
        scHalCfg.selfGenFid =
            (Vps_ScSelfGenFid) scCfgPrms->scAdvCfg->selfGenFid;
        scHalCfg.defConfFactor = scCfgPrms->scAdvCfg->defConfFactor;
        scHalCfg.biLinIntpType = scCfgPrms->scAdvCfg->biLinIntpType;
        scHalCfg.enableEdgeDetect = scCfgPrms->scAdvCfg->enableEdgeDetect;
        scHalCfg.hPolyBypass = scCfgPrms->scAdvCfg->hPolyBypass;
        scHalCfg.enablePeaking = scCfgPrms->scAdvCfg->enablePeaking;
        scHalCfg.bypass = scCfgPrms->scAdvCfg->bypass;
        scHalCfg.phInfoMode = VPS_SC_SET_PHASE_INFO_FROM_APP;
        scHalCfg.rowAccInc = scCfgPrms->scAdvCfg->rowAccInc;
        scHalCfg.rowAccOffset = scCfgPrms->scAdvCfg->rowAccOffset;
        scHalCfg.rowAccOffsetB = scCfgPrms->scAdvCfg->rowAccOffsetB;
        scHalCfg.ravScFactor = scCfgPrms->scAdvCfg->ravScFactor;
        scHalCfg.ravRowAccInit = scCfgPrms->scAdvCfg->ravRowAccInit;
        scHalCfg.ravRowAccInitB = scCfgPrms->scAdvCfg->ravRowAccInitB;
        scHalCfg.peakingCfg = scCfgPrms->scAdvCfg->peakingCfg;
        scHalCfg.edgeDetectCfg = scCfgPrms->scAdvCfg->edgeDetectCfg;

        cfgOvlyPtr = NULL;
        if (NULL != scCfgPrms->descMem)
        {
            cfgOvlyPtr = (Void *) ((UInt32) scCfgPrms->descMem->shadowOvlyMem
                + instObj->ovlyOffset[VCORE_DEI_SC_IDX]);
        }

        /* Set the scalar configuration to HAL */
        retVal = VpsHal_scSetAdvConfig(
                     instObj->halHandle[VCORE_DEI_SC_IDX],
                     &scHalCfg,
                     cfgOvlyPtr);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "SCALAR HAL Set Config Overlay Failed!!\n");
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiSetHalCfg
 *  \brief Sets the configuration in the HAL structures.
 */
static Int32 vcoreDeiSetHalCfg(const Vcore_DeiInstObj *instObj,
                               Vcore_DeiChObj *chObj)
{
    Int32       retVal;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    retVal = vcoreDeiSetChrusHalCfg(chObj);
    retVal |= vcoreDeiSetComprHalCfg(chObj);
    retVal |= vcoreDeiSetDcomprHalCfg(chObj);
    if (TRUE == instObj->isHqDei)
    {
        retVal |= vcoreDeiSetDeiHqHalCfg(instObj, chObj);
    }
    else
    {
        retVal |= vcoreDeiSetDeiHalCfg(instObj, chObj);
    }
    retVal |= vcoreDeiSetDrnHalCfg(instObj, chObj);
    retVal |= vcoreDeiSetScHalCfg(instObj, chObj);

    return (retVal);
}



/**
 *  vcoreDeiSetChrusHalCfg
 *  \brief Sets the Chroma Upsampler HAL configuration based on params set.
 */
static Int32 vcoreDeiSetChrusHalCfg(Vcore_DeiChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
    VpsHal_ChrusConfig     *chrusHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != chObj));

    chrusHalCfg = &chObj->chrusHalCfg;
    chrusHalCfg->coeff = NULL;
    if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
    {
        chrusHalCfg->mode = VPS_SF_INTERLACED;
    }
    else
    {
        chrusHalCfg->mode = VPS_SF_PROGRESSIVE;
    }

    /* Figure out line mode based on data format and deinterlacing mode */
#ifdef VPS_HAL_INCLUDE_DRN
    if ((FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat) ||
        (FVID2_DF_YUV422SP_UV == chObj->corePrms.fmt.dataFormat))
    {
        if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
        {
            /* YUV422 data to CHR_US and DEI is in deinterlacing mode or
             * in interlaced bypass mode */
            chrusHalCfg->cfgMode = VPSHAL_CHRUS_CFGMODE_B;
        }
        else
        {
            /* YUV422 data to CHR_US and DEI is in progressive bypass mode */
            chrusHalCfg->cfgMode = VPSHAL_CHRUS_CFGMODE_D;
        }
    }
    else
    {
        if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
        {
            /* YUV420 data to CHR_US and DEI is in deinterlacing mode or
             * in interlaced bypass mode */
            chrusHalCfg->cfgMode = VPSHAL_CHRUS_CFGMODE_A;
        }
        else
        {
            /* YUV420 data to CHR_US and DEI is in progressive bypass mode */
            chrusHalCfg->cfgMode = VPSHAL_CHRUS_CFGMODE_C;
        }
    }
#else
    if ((FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat) ||
        (FVID2_DF_YUV422SP_UV == chObj->corePrms.fmt.dataFormat))
    {
        /* YUV422 data to CHR_US and DEI is in deinterlacing mode or
         * in interlaced bypass mode. Progressive bypass not supported. */
        chrusHalCfg->cfgMode = VPSHAL_CHRUS_CFGMODE_B;
    }
    else
    {
        /* YUV420 data to CHR_US and DEI is in deinterlacing mode or
         * in interlaced bypass mode. Progressive bypass not supported. */
        chrusHalCfg->cfgMode = VPSHAL_CHRUS_CFGMODE_A;
    }
#endif

    return (retVal);
}



/**
 *  vcoreDeiSetComprHalCfg
 *  \brief Sets the Compressor HAL configuration based on params set.
 */
Int32 vcoreDeiSetComprHalCfg(Vcore_DeiChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
#ifdef VPS_HAL_INCLUDE_COMPR
    UInt32                  comprCnt;
    VpsHal_ComprConfig     *comprHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != chObj));

    for (comprCnt = 0u; comprCnt < VCORE_DEI_MAX_COMPR; comprCnt++)
    {
        comprHalCfg = &chObj->comprHalCfg[comprCnt];
        if ((TRUE == chObj->ctxInfo.isDeinterlacing) ||
            (TRUE == chObj->ctxInfo.isTnrMode))
        {
            comprHalCfg->enable = chObj->corePrms.comprEnable[comprCnt];
        }
        else
        {
            /* When compressor is not needed, disable it and override user
             * provided settings */
            chObj->corePrms.comprEnable[comprCnt] = FALSE;
            comprHalCfg->enable = FALSE;
        }
        comprHalCfg->width = chObj->corePrms.frameWidth;
    }
#endif

    return (retVal);
}



/**
 *  vcoreDeiSetDcomprHalCfg
 *  \brief Sets the Decompressor HAL configuration based on params set.
 */
static Int32 vcoreDeiSetDcomprHalCfg(Vcore_DeiChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
#ifdef VPS_HAL_INCLUDE_DCOMPR
    UInt32                  dcomprCnt;
    VpsHal_DcomprConfig    *dcomprHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != chObj));

    for (dcomprCnt = 0u; dcomprCnt < VCORE_DEI_MAX_DCOMPR; dcomprCnt++)
    {
        dcomprHalCfg = &chObj->dcomprHalCfg[dcomprCnt];
        if ((TRUE == chObj->ctxInfo.isDeinterlacing) ||
            (TRUE == chObj->ctxInfo.isTnrMode))
        {
            dcomprHalCfg->enable = chObj->corePrms.dcomprEnable[dcomprCnt];
        }
        else
        {
            /* When decompressor is not needed, disable it and override user
             * provided settings */
            chObj->corePrms.dcomprEnable[dcomprCnt] = FALSE;
            dcomprHalCfg->enable = FALSE;
        }
        dcomprHalCfg->width = chObj->corePrms.frameWidth;
    }
#endif

    return (retVal);
}



/**
 *  vcoreDeiSetDeiHqHalCfg
 *  \brief Sets the DEI HQ HAL configuration based on params set.
 */
static Int32 vcoreDeiSetDeiHqHalCfg(const Vcore_DeiInstObj *instObj,
                                    Vcore_DeiChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
#ifdef VPS_HAL_INCLUDE_DEIH
    VpsHal_DeihConfig      *deihHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    deihHalCfg = &chObj->deihHalCfg;
    deihHalCfg->width = (UInt16) chObj->corePrms.frameWidth;
    deihHalCfg->tempInpEnable = chObj->corePrms.deiHqCfg.tempInpEnable;
    deihHalCfg->tempInpChromaEnable =
        chObj->corePrms.deiHqCfg.tempInpChromaEnable;

    /* When in interlaced bypass display mode, input size is in
     * terms of frame size but DEI needs field size, so dividing
     * input size by 2. */
    if ((VCORE_OPMODE_DISPLAY == instObj->curMode) &&
        (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat))
    {
        deihHalCfg->height = (UInt16) chObj->corePrms.frameHeight / 2u;
    }
    else
    {
        deihHalCfg->height = (UInt16) chObj->corePrms.frameHeight;
    }

    if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
    {
        deihHalCfg->scanFormat = VPS_SF_INTERLACED;
    }
    else
    {
        deihHalCfg->scanFormat = VPS_SF_PROGRESSIVE;
    }

    /* If TNR needs to be enabled, then DEI should not be in bypass mode */
    if (TRUE == chObj->corePrms.deiHqCfg.tnrEnable)
    {
        deihHalCfg->bypass = FALSE;
    }
    else
    {
        deihHalCfg->bypass = chObj->corePrms.deiHqCfg.bypass;
    }

    /* Configure default values for MDT params */
    if (VPS_DEIHQ_FLDMODE_4FLD == chObj->corePrms.deiHqCfg.fldMode)
    {
        deihHalCfg->mdt.fldMode = VPS_DEIHQ_FLDMODE_4FLD;
    }
    else
    {
        deihHalCfg->mdt.fldMode = VPS_DEIHQ_FLDMODE_5FLD;
    }
    deihHalCfg->mdt.spatMaxBypass = chObj->corePrms.deiHqCfg.spatMaxBypass;
    deihHalCfg->mdt.tempMaxBypass = chObj->corePrms.deiHqCfg.tempMaxBypass;
    deihHalCfg->mdt.lcModeEnable = chObj->corePrms.deiHqCfg.lcModeEnable;

    /* Disable MVSTM loop if it is not required for the low cost mode */
    if ((TRUE == deihHalCfg->mdt.lcModeEnable) &&
        (FALSE == chObj->corePrms.deiHqCfg.mvstmEnable))
    {
        deihHalCfg->mdt.mvIIR = FALSE;
    }
    else
    {
        deihHalCfg->mdt.mvIIR = TRUE;
    }

    /* Configure default values for EDI params */
    if (VPS_DEIHQ_EDIMODE_LINE_AVG == chObj->corePrms.deiHqCfg.inpMode)
    {
        deihHalCfg->edi.inpMode = VPS_DEIHQ_EDIMODE_LINE_AVG;
    }
    else if (VPS_DEIHQ_EDIMODE_FLD_AVG == chObj->corePrms.deiHqCfg.inpMode)
    {
        deihHalCfg->edi.inpMode = VPS_DEIHQ_EDIMODE_FLD_AVG;
    }
    else if (VPS_DEIHQ_EDIMODE_EDI_SMALL_WINDOW ==
        chObj->corePrms.deiHqCfg.inpMode)
    {
        deihHalCfg->edi.inpMode = VPS_DEIHQ_EDIMODE_EDI_SMALL_WINDOW;
    }
    else
    {
        deihHalCfg->edi.inpMode = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
    }
    deihHalCfg->edi.chromaEdiEnable = chObj->corePrms.deiHqCfg.chromaEdiEnable;

    /* Configure default values for TNR params */
    deihHalCfg->tnr.luma = chObj->corePrms.deiHqCfg.tnrEnable;
    deihHalCfg->tnr.chroma = chObj->corePrms.deiHqCfg.tnrEnable;
    deihHalCfg->tnr.adaptive = FALSE;
    deihHalCfg->tnr.advMode = VPS_DEIHQ_TNRADVMODE_REGULAR;
    deihHalCfg->tnr.advScaleFactor = 0x02;
    deihHalCfg->tnr.skt = TRUE;
    deihHalCfg->tnr.maxSktGain = 0x0C;
    deihHalCfg->tnr.minChromaSktThrld = 0x0A;
    deihHalCfg->tnr.skt = chObj->corePrms.deiHqCfg.sktEnable;

    /* Configure default values for FMD params */
    deihHalCfg->fmd.filmMode = FALSE;
    deihHalCfg->fmd.bed = TRUE;
    deihHalCfg->fmd.window = FALSE;
    deihHalCfg->fmd.lock = FALSE;
    deihHalCfg->fmd.jamDir = VPS_DEI_FMDJAMDIR_PREV_FLD;
    deihHalCfg->fmd.windowMinx = 0u;
    deihHalCfg->fmd.windowMiny = 0u;
    deihHalCfg->fmd.windowMaxx = (UInt16) (chObj->corePrms.frameWidth - 1u);
    /* Since deiHalCfg->height is already calculated depending on
       the scanFormat, using this height here for fmd window configuration */
    deihHalCfg->fmd.windowMaxy = (UInt16) (deihHalCfg->height - 1u);

    /* Configure default values for SNR params */
    deihHalCfg->snr.gnrChroma = chObj->corePrms.deiHqCfg.snrEnable;
    deihHalCfg->snr.gnrLuma = chObj->corePrms.deiHqCfg.snrEnable;
    deihHalCfg->snr.gnrAdaptive = FALSE;
    deihHalCfg->snr.inrMode = VPS_DEIHQ_SNRINRMODE_NONE;
    deihHalCfg->snr.gnmDevStableThrld = 0x04;
#endif

    return (retVal);
}



/**
 *  vcoreDeiSetDeiHalCfg
 *  \brief Sets the DEI HAL configuration based on params set.
 */
static Int32 vcoreDeiSetDeiHalCfg(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
    VpsHal_DeiConfig       *deiHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    deiHalCfg = &chObj->deiHalCfg;
    deiHalCfg->width = (UInt16) chObj->corePrms.frameWidth;

    /* When in interlaced bypass display mode, input size is in
     * terms of frame size but DEI needs field size, so dividing
     * input size by 2. */
    if ((VCORE_OPMODE_DISPLAY == instObj->curMode) &&
        (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat))
    {
        deiHalCfg->height = (UInt16) chObj->corePrms.frameHeight / 2u;
    }
    else
    {
        deiHalCfg->height = (UInt16) chObj->corePrms.frameHeight;
    }

    if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
    {
        deiHalCfg->scanFormat = VPS_SF_INTERLACED;
    }
    else
    {
        deiHalCfg->scanFormat = VPS_SF_PROGRESSIVE;
    }
    deiHalCfg->bypass = chObj->corePrms.deiCfg.bypass;
    deiHalCfg->fieldFlush = TRUE;

    /* Configure default values for MDT params */
    deiHalCfg->mdt.spatMaxBypass = chObj->corePrms.deiCfg.spatMaxBypass;
    deiHalCfg->mdt.tempMaxBypass = chObj->corePrms.deiCfg.tempMaxBypass;

    /* Configure default values for EDI params */
    deiHalCfg->edi.inpMode = chObj->corePrms.deiCfg.inpMode;
    deiHalCfg->edi.tempInpEnable = chObj->corePrms.deiCfg.tempInpEnable;
    deiHalCfg->edi.tempInpChromaEnable =
        chObj->corePrms.deiCfg.tempInpChromaEnable;

    /* Configure default values for FMD params */
    deiHalCfg->fmd.filmMode = FALSE;
    deiHalCfg->fmd.bed = TRUE;
    deiHalCfg->fmd.window = FALSE;
    deiHalCfg->fmd.lock = FALSE;
    deiHalCfg->fmd.jamDir = VPS_DEI_FMDJAMDIR_PREV_FLD;
    deiHalCfg->fmd.windowMinx = 0u;
    deiHalCfg->fmd.windowMiny = 0u;
    deiHalCfg->fmd.windowMaxx = (UInt16) (chObj->corePrms.frameWidth - 1u);
    /* Since deiHalCfg->height is already calculated depending on
       the scanFormat, using this height here for fmd window configuration */
    deiHalCfg->fmd.windowMaxy = (UInt16) (deiHalCfg->height - 1u);

    return (retVal);
}



/**
 *  vcoreDeiSetDrnHalCfg
 *  \brief Sets the DRN HAL configuration based on params set.
 */
static Int32 vcoreDeiSetDrnHalCfg(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
#ifdef VPS_HAL_INCLUDE_DRN
    VpsHal_DrnConfig       *drnHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    drnHalCfg = &chObj->drnHalCfg;
    if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
    {
        drnHalCfg->scanFormat = VPS_SF_INTERLACED;
    }
    else
    {
        drnHalCfg->scanFormat = VPS_SF_PROGRESSIVE;
    }
    drnHalCfg->dilateTileSize = VPSHAL_DRN_TILE_SIZE_8x8;
    drnHalCfg->width = (UInt16) chObj->corePrms.frameWidth;

    /* When in interlaced bypass display mode, input size is in
     * terms of frame size but DEI needs field size, so dividing
     * input size by 2. */
    if ((VCORE_OPMODE_DISPLAY == instObj->curMode) &&
        (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat))
    {
        drnHalCfg->height = (UInt16) chObj->corePrms.frameHeight / 2u;
    }
    else
    {
        drnHalCfg->height = (UInt16) chObj->corePrms.frameHeight;
    }

    if (TRUE == chObj->corePrms.drnEnable)
    {
        drnHalCfg->bypass = FALSE;
    }
    else
    {
        drnHalCfg->bypass = TRUE;
    }
#endif

    return (retVal);
}



/**
 *  vcoreDeiSetScHalCfg
 *  \brief Sets the Scalar HAL configuration based on params set.
 */
static Int32 vcoreDeiSetScHalCfg(const Vcore_DeiInstObj *instObj,
                                 Vcore_DeiChObj *chObj)
{
    Int32                   retVal = VPS_SOK;
    VpsHal_ScConfig        *scHalCfg;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    scHalCfg = &chObj->scHalCfg;
    scHalCfg->srcWidth = chObj->corePrms.frameWidth;
    scHalCfg->tarWidth = chObj->corePrms.tarWidth;
    scHalCfg->tarHeight = chObj->corePrms.tarHeight;
    scHalCfg->cropStartX = chObj->corePrms.cropCfg.cropStartX;
    scHalCfg->cropStartY = chObj->corePrms.cropCfg.cropStartY;
    scHalCfg->cropWidth = chObj->corePrms.cropCfg.cropWidth;
    scHalCfg->cropHeight = chObj->corePrms.cropCfg.cropHeight;
    if (TRUE == chObj->ctxInfo.isDeinterlacing)
    {
        /* When deinterlacing input to scalar will be twice that of DEI */
        scHalCfg->srcHeight = chObj->corePrms.frameHeight * 2u;
        scHalCfg->inFrameMode = VPS_SF_PROGRESSIVE;
    }
    else
    {
        /* When in interlaced bypass display mode, input size is in
         * terms of frame size but DEI needs field size, so dividing
         * input size by 2. */
        if ((VCORE_OPMODE_DISPLAY == instObj->curMode) &&
            (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat))
        {
            scHalCfg->srcHeight = chObj->corePrms.frameHeight / 2u;
            scHalCfg->cropHeight = scHalCfg->cropHeight / 2u;
            scHalCfg->tarHeight = scHalCfg->tarHeight / 2u;
            scHalCfg->cropStartY = scHalCfg->cropStartY / 2u;
        }
        else
        {
            scHalCfg->srcHeight = chObj->corePrms.frameHeight;
        }

        if (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat)
        {
            scHalCfg->inFrameMode = VPS_SF_INTERLACED;
        }
        else
        {
            scHalCfg->inFrameMode = VPS_SF_PROGRESSIVE;
        }
    }

    if (FVID2_SF_INTERLACED == chObj->corePrms.secScanFmt)
    {
        scHalCfg->outFrameMode = VPS_SF_INTERLACED;
    }
    else
    {
        scHalCfg->outFrameMode = VPS_SF_PROGRESSIVE;
    }
    scHalCfg->hsType = VPS_SC_HST_AUTO;
    scHalCfg->hsType = chObj->corePrms.scCfg.hsType;
    scHalCfg->nonLinear = chObj->corePrms.scCfg.nonLinear;
    scHalCfg->stripSize = chObj->corePrms.scCfg.stripSize;
    scHalCfg->vsType = chObj->corePrms.scCfg.vsType;
    scHalCfg->fidPol = VPS_FIDPOL_NORMAL;
    scHalCfg->selfGenFid = VPS_SC_SELF_GEN_FID_DISABLE;
    scHalCfg->defConfFactor = 0u;
    scHalCfg->biLinIntpType = VPS_SC_BINTP_MODIFIED;
    scHalCfg->enableEdgeDetect = chObj->corePrms.scCfg.enableEdgeDetect;
    if (VPS_SC_HST_AUTO == chObj->corePrms.scCfg.hsType)
        scHalCfg->hPolyBypass = TRUE;
    else
        scHalCfg->hPolyBypass = FALSE;
    scHalCfg->bypass = chObj->corePrms.scCfg.bypass;
    scHalCfg->enablePeaking = chObj->corePrms.scCfg.enablePeaking;

    scHalCfg->phInfoMode = VPS_SC_SET_PHASE_INFO_DEFAULT;

    return (retVal);
}
