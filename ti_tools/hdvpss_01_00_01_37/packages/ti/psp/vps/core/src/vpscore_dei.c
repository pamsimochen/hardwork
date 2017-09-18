/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_dei.c
 *
 *  \brief VPS DEI path core internal file containing functions related to
 *  DEI HQ and DEI module logic.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/core/src/vpscore_deiPriv.h>


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

static Void vcoreDeiHqCalStateLineAvg(const Vcore_DeiInstObj *instObj,
                                      Vcore_DeiChObj *chObj);
static Void vcoreDeiHqCalStateFldAvg(const Vcore_DeiInstObj *instObj,
                                     Vcore_DeiChObj *chObj);
static Void vcoreDeiHqCalStateEdi(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj);
static Void vcoreDeiHqCalStateTnr(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vcoreDeiProgramDesc
 *  \brief Programs the inbound data descriptor depending on the parameters set.
 *  This is used in case of non-mosaic mode of operation.
 */
UInt32 vcoreDeiProgramDesc(const Vcore_DeiInstObj *instObj,
                           const Vcore_DeiChObj *chObj,
                           const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    Bool                        swapLumaChroma = FALSE;
    UInt32                      cpuRev, platformId;
    UInt32                      cnt, descOffset, tempOffset;
    Void                       *descPtr;
    VpsHal_VpdmaInDescParams    inDescPrm[VCORE_DEI_NUM_DESC_PER_WINDOW];

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));

    cpuRev = Vps_platformGetCpuRev();
    platformId = Vps_platformGetId();
    /* Swap luma and chroma dummy descriptors for TI816x ES2.0 and
     * TI814x ES2.1 where line buffers are added in the chroma path to
     * support YUV422I HD input size. */
    swapLumaChroma = FALSE;
    if (((VPS_PLATFORM_ID_EVM_TI816x == platformId) &&
            (cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
        ((VPS_PLATFORM_ID_EVM_TI814x == platformId) &&
            (cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
        (VPS_PLATFORM_ID_EVM_TI8107 == platformId))
    {
        swapLumaChroma = TRUE;
    }

    /* Set the commom VPDMA parameters between Y and C descriptors */
    for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
    {
        /*
         *  Caution:
         *  Next two statements relies on the fact that VCORE_DEI_FLD0LUMA_IDX
         *  and VCORE_DEI_FLD0CHROMA_IDX are next to each other.
         */
        inDescPrm[cnt].channel =
            instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];
        inDescPrm[cnt].nextChannel =
            instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];
        inDescPrm[cnt].transWidth = chObj->corePrms.fmt.width;
        inDescPrm[cnt].frameWidth = chObj->corePrms.frameWidth;
        inDescPrm[cnt].startX = chObj->corePrms.startX;
        inDescPrm[cnt].notify = FALSE;
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            inDescPrm[cnt].priority = VPS_CFG_DISP_VPDMA_PRIORITY;
        }
        else
        {
            inDescPrm[cnt].priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
        }
        inDescPrm[cnt].memType =
            (VpsHal_VpdmaMemoryType) chObj->corePrms.memType;
        inDescPrm[cnt].is1DMem = FALSE;

        /* When in interlaced bypass display mode, input size is in
         * terms of frame size but DEI needs field size, so dividing
         * input size by 2. */
        if ((VCORE_OPMODE_DISPLAY == instObj->curMode) &&
            (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat))
        {
            inDescPrm[cnt].transHeight = chObj->corePrms.fmt.height / 2u;
            inDescPrm[cnt].frameHeight = chObj->corePrms.frameHeight / 2u;
            inDescPrm[cnt].startY = chObj->corePrms.startY / 2u;

            if (TRUE == chObj->corePrms.fmt.fieldMerged[chObj->bufIndex[cnt]])
            {
                /* Fields are merged - read alternate lines */
                inDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_2;
            }
            else
            {
                /* Fields are separated - read next line */
                inDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_1;
            }
        }
        else
        {
            /* In deinterlacing mode or in progressive/interlaced bypass mode,
             * either field or progressive frame is queued in to the driver.
             * Hence program the entire field/frame. */
            inDescPrm[cnt].transHeight = chObj->corePrms.fmt.height;
            inDescPrm[cnt].frameHeight = chObj->corePrms.frameHeight;
            inDescPrm[cnt].startY = chObj->corePrms.startY;
            inDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_1;
        }

        inDescPrm[cnt].lineStride =
            chObj->corePrms.fmt.pitch[chObj->bufIndex[cnt]];
    }

    /* Set the other VPDMA parameters */
    if ((FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat) 
    		|| (FVID2_DF_YUV422I_UYVY == chObj->corePrms.fmt.dataFormat))
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_YC422;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_YC422;
    }
    else if (FVID2_DF_YUV422SP_UV == chObj->corePrms.fmt.dataFormat)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_Y422;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_C422;
    }
    else if (FVID2_DF_YUV420SP_UV == chObj->corePrms.fmt.dataFormat)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_Y420;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_C420;

        /* Chroma height is half of Luma for YUV420 */
        inDescPrm[VCORE_DEI_CBCR_IDX].transHeight /= 2u;
        inDescPrm[VCORE_DEI_CBCR_IDX].frameHeight /= 2u;
        inDescPrm[VCORE_DEI_CBCR_IDX].startY /= 2u;
    }
    else /* Invalid Data Format */
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (VPS_SOK == retVal)
    {
        for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
        {
            /* Configure data descriptor */
            /*
             *  Caution:
             *  Next statement relies on the fact that VCORE_DEI_FLD0LUMA_IDX
             *  and VCORE_DEI_FLD0CHROMA_IDX are next to each other.
             */
            descOffset = chObj->descOffset[VCORE_DEI_FLD0LUMA_IDX + cnt];
            descPtr = descMem->inDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            retVal = VpsHal_vpdmaCreateInBoundDataDesc(
                         descPtr,
                         &inDescPrm[cnt]);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "VPDMA Descriptor Creation Failed!!\n");
                break;
            }
            else
            {
                /* Print the created descriptor memory */
                GT_1trace(DeiCoreTrace, GT_DEBUG,
                    "Data Descrirptor %d:\n", cnt);
                GT_0trace(DeiCoreTrace, GT_DEBUG, "-------------------\n");
                VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
            }
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Make a dummy data descriptor at the last with zero transfer size
         * if required */
        descOffset = 0u;
        if (TRUE == chObj->isDummyNeeded)
        {
            for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
            {
                inDescPrm[cnt].transWidth = 0u;
                inDescPrm[cnt].transHeight = 0u;
                inDescPrm[cnt].startX = inDescPrm[cnt].frameWidth;
                inDescPrm[cnt].startY = inDescPrm[cnt].frameHeight;
                inDescPrm[cnt].channel =
                    instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];
                inDescPrm[cnt].nextChannel =
                    instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];

                tempOffset = cnt;
                if (TRUE == swapLumaChroma)
                {
                    /* Program chroma descriptor first and then luma */
                    tempOffset = (instObj->numDescPerWindow - 1u) - cnt;
                }

                /* Dummy data descriptor is only descriptor in
                 * multiWinDataDesc */
                descPtr = descMem->multiWinDataDesc[tempOffset];
                GT_assert(DeiCoreTrace, (NULL != descPtr));

                /* Form the descriptor */
                retVal = VpsHal_vpdmaCreateInBoundDataDesc(
                             descPtr,
                             &inDescPrm[cnt]);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "VPDMA Descriptor Creation Failed!!\n");
                    break;
                }
                /* Set some address to make valid FID field */
                VpsHal_vpdmaSetAddress(descPtr, 0u, descPtr);

                /* Set mosaic mode bit for dummy descriptor */
#ifdef VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE
                VpsHal_vpdmaSetMosaicMode(descPtr, TRUE);
#else
                VpsHal_vpdmaSetMosaicMode(descPtr, FALSE);
#endif

                /* Print the created descriptor memory */
                GT_1trace(DeiCoreTrace, GT_DEBUG, "Data Descrirptor %d:\n", cnt);
                GT_0trace(DeiCoreTrace, GT_DEBUG, "-------------------\n");
                VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
            }
            descOffset += instObj->numDescPerWindow;
        }
        else if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            /* Use dummy descriptor instead of dummy data descriptor */
            descPtr = descMem->multiWinDataDesc[0u];
            GT_assert(DeiCoreTrace, (NULL != descPtr));

            for (cnt = 0u; cnt < (instObj->numDescPerWindow * 2u); cnt++)
            {
                VpsHal_vpdmaCreateDummyDesc(descPtr);
                descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            descOffset += instObj->numDescPerWindow;
        }

        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            /* Put SOCH on both luma and chroma first */
            descPtr = descMem->multiWinDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
            {
#ifdef VPS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS
                VpsHal_vpdmaCreateSOCHCtrlDesc(
                    descPtr,
                    instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt]);
#else
                VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif
                descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            /* Increment offset every two ctrl desc as data desc is twice the
             * size of ctrl desc */
            descOffset += (instObj->numDescPerWindow / 2u);

            /* Put Abort on both luma and chroma */
            descPtr = descMem->multiWinDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
            {
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
                VpsHal_vpdmaCreateAbortCtrlDesc(
                    descPtr,
                    instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt]);
#else
                VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif
                descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            /* Increment offset every two ctrl desc as data desc is twice the
             * size of ctrl desc */
            descOffset += (instObj->numDescPerWindow / 2u);
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiProgramMosaicDesc
 *  \brief Configures the data descriptor memory depending on the format and
 *  mosaic configuration. This is used in case of mosaic mode of operation.
 */
UInt32 vcoreDeiProgramMosaicDesc(const Vcore_DeiInstObj *instObj,
                                 Vcore_DeiChObj *chObj,
                                 UInt32 layoutId,
                                 const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    Bool                        swapLumaChroma = FALSE;
    UInt32                      cpuRev, platformId;
    UInt32                      cnt, winCnt;
    UInt32                      descOffset, tempOffset, tempIdx;
    Void                       *descPtr;
    Vcore_DeiMosaicInfo        *mInfo;
    const Vps_WinFormat        *winFmt;
    VpsHal_VpdmaMosaicWinFmt   *mulWinFmt;
    VpsHal_VpdmaInDescParams    inDescPrm[VCORE_DEI_NUM_DESC_PER_WINDOW];
    VpsHal_VpdmaChannel         nextCh[VCORE_DEI_NUM_DESC_PER_WINDOW];
    VpsHal_VpdmaChannel         sochVpdmaChan
                                    [VCORE_DEI_NUM_DESC_PER_WINDOW];
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
    UInt32                      numAbortDesc;
    VpsHal_VpdmaChannel         abortFreeChan
                    [VCORE_DEI_NUM_DESC_PER_WINDOW * VCORE_DEI_MAX_MOSAIC_ROW];
#endif

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));

    cpuRev = Vps_platformGetCpuRev();
    platformId = Vps_platformGetId();
    /* Swap luma and chroma dummy descriptors for TI816x ES2.0 and
     * TI814x ES2.1 where line buffers are added in the chroma path to
     * support YUV422I HD input size. */
    swapLumaChroma = FALSE;
    if (((VPS_PLATFORM_ID_EVM_TI816x == platformId) &&
            (cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
        ((VPS_PLATFORM_ID_EVM_TI814x == platformId) &&
            (cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
        (VPS_PLATFORM_ID_EVM_TI8107 == platformId))
    {
        swapLumaChroma = TRUE;
    }

    /* Set the commom VPDMA parameters between Y and C descriptors */
    for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
    {
        nextCh[cnt] = VPSHAL_VPDMA_CHANNEL_INVALID;
        sochVpdmaChan[cnt] = instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];

        /*
         *  Caution:
         *  Next two statements relies on the fact that VCORE_DEI_FLD0LUMA_IDX
         *  and VCORE_DEI_FLD0CHROMA_IDX are next to each other.
         */
        inDescPrm[cnt].frameWidth = chObj->corePrms.frameWidth;
        inDescPrm[cnt].notify = FALSE;
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            inDescPrm[cnt].priority = VPS_CFG_DISP_VPDMA_PRIORITY;
        }
        else
        {
            inDescPrm[cnt].priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
        }
        inDescPrm[cnt].memType =
            (VpsHal_VpdmaMemoryType) chObj->corePrms.memType;
        inDescPrm[cnt].is1DMem = FALSE;

        /* When in interlaced bypass display mode, input size is in
         * terms of frame size but DEI needs field size, so dividing
         * input size by 2. */
        if ((VCORE_OPMODE_DISPLAY == instObj->curMode) &&
            (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat))
        {
            inDescPrm[cnt].frameHeight = chObj->corePrms.frameHeight / 2u;

            if (TRUE == chObj->corePrms.fmt.fieldMerged[chObj->bufIndex[cnt]])
            {
                /* Fields are merged - read alternate lines */
                inDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_2;
            }
            else
            {
                /* Fields are separated - read next line */
                inDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_1;
            }
        }
        else
        {
            /* In deinterlacing mode or in progressive/interlaced bypass mode,
             * either field or progressive frame is queued in to the driver.
             * Hence program the entire field/frame. */
            inDescPrm[cnt].frameHeight = chObj->corePrms.frameHeight;
            inDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_1;
        }
    }

    /* Set the other VPDMA parameters */
    if (FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_YC422;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_YC422;
    }
    else if (FVID2_DF_YUV422SP_UV == chObj->corePrms.fmt.dataFormat)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_Y422;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_C422;
    }
    else if (FVID2_DF_YUV420SP_UV == chObj->corePrms.fmt.dataFormat)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_Y420;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_C420;

        /* Chroma height is half of Y for YUV420 */
        inDescPrm[VCORE_DEI_CBCR_IDX].frameHeight /= 2u;
        inDescPrm[VCORE_DEI_CBCR_IDX].startY /= 2u;
    }
    else /* Invalid Data Format */
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    /* Get the layout object for the requested layout */
    mInfo = vcoreDeiGetLayoutObj(chObj, layoutId);
    if (NULL == mInfo)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid layout ID!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Program the mosaic descriptors */
    if (VPS_SOK == retVal)
    {
        descOffset = 0u;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        numAbortDesc = 0u;
#endif
        /* Configure each window/descriptor */
        for (winCnt = 0u; winCnt < mInfo->mulWinPrms.numSplitWindows; winCnt++)
        {
            /* Configure for Y and C channels */
            for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
            {
                if (winCnt < mInfo->mulWinPrms.numWindowsFirstRow)
                {
                    /* Program luma first and then chroma for first row */
                    tempOffset = descOffset + cnt;
                    /* Get descriptor pointer for the first row */
                    descPtr = descMem->inDataDesc[tempOffset];
                }
                else
                {
                    if ((winCnt == mInfo->mulWinPrms.numWindowsFirstRow) &&
                        (0u == cnt))
                    {
                        /* Reset the count when moving to 2nd row luma */
                        descOffset = 0u;
                    }

                    tempOffset = descOffset + cnt;
                    if (TRUE == swapLumaChroma)
                    {
                        /* Program chroma first and then luma for non-first
                         * row */
                        tempOffset =
                            descOffset + (instObj->numDescPerWindow - 1u) - cnt;
                    }

                    /* Get descriptor pointer for the non-first row */
                    descPtr = descMem->multiWinDataDesc[tempOffset];
                }
                GT_assert(DeiCoreTrace, (NULL != descPtr));

                /* Configure the parameters of each mosaic window */
                mulWinFmt = &mInfo->mulWinFmt[winCnt];
                winFmt = mulWinFmt->ipWindowFmt;
                GT_assert(DeiCoreTrace, (NULL != winFmt));

                /* Note the free channel of the last window */
                if ((TRUE == mulWinFmt->isLastWin) &&
                    (VCORE_OPMODE_DISPLAY == instObj->curMode) &&
                    (FALSE == mInfo->mulWinPrms.isDummyNeeded) &&
                    (FALSE == mulWinFmt->isActualChanReq))
                {
                    GT_assert(DeiCoreTrace, (mulWinFmt->freeChanIndex <
                        (instObj->numFreeCh / instObj->numDescPerWindow)));
                    tempIdx = cnt +
                        (mulWinFmt->freeChanIndex * instObj->numDescPerWindow);
                    sochVpdmaChan[cnt] = (VpsHal_VpdmaChannel)
                        instObj->freeChList[tempIdx];
                }

                inDescPrm[cnt].transWidth = mulWinFmt->width;
                inDescPrm[cnt].startX = mulWinFmt->startX;
                /* Note: Pitch of split window is same as original buffer
                 * pitch!! */
                inDescPrm[cnt].lineStride = winFmt->pitch[chObj->bufIndex[cnt]];

                /* When in interlaced bypass display mode, input size is in
                 * terms of frame size but DEI needs field size, so dividing
                 * input size by 2. */
                if ((VCORE_OPMODE_DISPLAY == instObj->curMode) &&
                    (FVID2_SF_INTERLACED == chObj->corePrms.fmt.scanFormat))
                {
                    inDescPrm[cnt].transHeight = mulWinFmt->height / 2u;
                    inDescPrm[cnt].startY = mulWinFmt->startY / 2u;
                }
                else
                {
                    inDescPrm[cnt].transHeight = mulWinFmt->height;
                    inDescPrm[cnt].startY = mulWinFmt->startY;
                }

                if (FVID2_DF_YUV420SP_UV == chObj->corePrms.fmt.dataFormat)
                {
                    /* Chroma height is half of Luma for YUV420 */
                    inDescPrm[VCORE_DEI_CBCR_IDX].transHeight >>= 1u;
                    inDescPrm[VCORE_DEI_CBCR_IDX].frameHeight >>= 1u;
                    inDescPrm[VCORE_DEI_CBCR_IDX].startY >>= 1u;
                }

                /* Determine next channel.
                 * Caution: This should be done before assigning channel done
                 * in the the next step */
                if (VPSHAL_VPDMA_CHANNEL_INVALID != nextCh[cnt])
                {
                    /* Use the previously assigned next channel */
                    inDescPrm[cnt].nextChannel = nextCh[cnt];
                }
                else
                {
                    inDescPrm[cnt].nextChannel =
                        instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
                    /* Get the free channel number for the abort descriptors */
                    if (FALSE == mulWinFmt->isActualChanReq)
                    {
                        GT_assert(DeiCoreTrace,
                            (numAbortDesc < (VCORE_DEI_NUM_DESC_PER_WINDOW *
                                             VCORE_DEI_MAX_MOSAIC_ROW)));
                        GT_assert(DeiCoreTrace, (mulWinFmt->freeChanIndex <
                            (instObj->numFreeCh / instObj->numDescPerWindow)));

                        tempIdx = cnt + (mulWinFmt->freeChanIndex *
                                         instObj->numDescPerWindow);
                        abortFreeChan[numAbortDesc] = (VpsHal_VpdmaChannel)
                            instObj->freeChList[tempIdx];
                        numAbortDesc++;
                    }
#endif
                }

                /* Determine channel depending on flag */
                if (TRUE == mulWinFmt->isActualChanReq)
                {
                    /* Use actual channel */
                    inDescPrm[cnt].channel =
                        instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];

                    /* Row is ending - reset next channel */
                    nextCh[cnt] = VPSHAL_VPDMA_CHANNEL_INVALID;
                }
                else
                {
                    /* Use free channel */
                    GT_assert(DeiCoreTrace, (mulWinFmt->freeChanIndex <
                        (instObj->numFreeCh / instObj->numDescPerWindow)));
                    tempIdx = cnt +
                        (mulWinFmt->freeChanIndex * instObj->numDescPerWindow);
                    inDescPrm[cnt].channel = (VpsHal_VpdmaChannel)
                        instObj->freeChList[tempIdx];

                    /* Next channel of next window is this free channel.
                     * This assumes right to left sorting of split windows */
                    nextCh[cnt] = (VpsHal_VpdmaChannel)
                        instObj->freeChList[tempIdx];
                }

                /* Configure data descriptor */
                retVal = VpsHal_vpdmaCreateInBoundDataDesc(
                             descPtr,
                             &inDescPrm[cnt]);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "VPDMA Descriptor Creation Failed!!\n");
                    break;
                }

                /* Set mosaic mode bit for the non-first row base channels */
                VpsHal_vpdmaSetMosaicMode(descPtr, FALSE);
#ifdef VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE
                if ((winCnt >= mInfo->mulWinPrms.numWindowsFirstRow) &&
                    (TRUE == mulWinFmt->isActualChanReq))
                {
                    VpsHal_vpdmaSetMosaicMode(descPtr, TRUE);
                }
#endif

                /* Print the created descriptor memory */
                GT_1trace(DeiCoreTrace, GT_DEBUG,
                    "Data Descrirptor %d:\n",
                    (winCnt * instObj->numDescPerWindow) + cnt);
                GT_0trace(DeiCoreTrace, GT_DEBUG,
                    "---------------------\n");
                VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
            }
            descOffset += instObj->numDescPerWindow;

            /* If error break */
            if (VPS_SOK != retVal)
            {
                break;
            }
        }
    }

    /* Make a dummy data descriptor at the last with zero transfer size
     * if required */
    if ((VPS_SOK == retVal) && (TRUE == mInfo->mulWinPrms.isDummyNeeded))
    {
        /* When only one row is present, descOffset should be reset as it
         * wouldn't have been reset in above for loop */
        if (mInfo->mulWinPrms.numSplitWindows ==
            mInfo->mulWinPrms.numWindowsFirstRow)
        {
            descOffset = 0u;
        }

        /* Make a dummy data descriptor at the last with zero transfer size */
        for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
        {
            inDescPrm[cnt].transWidth = 0u;
            inDescPrm[cnt].transHeight = 0u;
            inDescPrm[cnt].startX = inDescPrm[cnt].frameWidth;
            inDescPrm[cnt].startY = inDescPrm[cnt].frameHeight;
            inDescPrm[cnt].channel =
                instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];
            inDescPrm[cnt].nextChannel =
                instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt];

            tempOffset = descOffset + cnt;
            if (TRUE == swapLumaChroma)
            {
                /* Program chroma descriptor first and then luma */
                tempOffset =
                    descOffset + (instObj->numDescPerWindow - 1u) - cnt;
            }

            descPtr = descMem->multiWinDataDesc[tempOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));

            /* Form the descriptor */
            retVal = VpsHal_vpdmaCreateInBoundDataDesc(
                         descPtr,
                         &inDescPrm[cnt]);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "VPDMA Descriptor Creation Failed!!\n");
                break;
            }
            /* Set some address to make valid FID field */
            VpsHal_vpdmaSetAddress(descPtr, 0u, descPtr);

            /* Set mosaic mode bit for dummy descriptor */
#ifdef VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE
            VpsHal_vpdmaSetMosaicMode(descPtr, TRUE);
#else
            VpsHal_vpdmaSetMosaicMode(descPtr, FALSE);
#endif

            /* Print the created descriptor memory */
            GT_1trace(DeiCoreTrace, GT_DEBUG,
                "Data Descrirptor %d:\n",
                (winCnt * instObj->numDescPerWindow) + cnt);
            GT_0trace(DeiCoreTrace, GT_DEBUG,
                "------------------------\n");
            VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
        }
        descOffset += instObj->numDescPerWindow;
        winCnt++;
    }


    /* Put SOCH on the last window channel - This could be actual channel if
     * the last channel is actual channel or dummy data desc is needed.
     * Otherwise this will be the last window free channel. */
    if ((VPS_SOK == retVal) && (VCORE_OPMODE_DISPLAY == instObj->curMode))
    {
        /* When only one row is present, descOffset should be reset as it
         * wouldn't have been reset in above for loop */
        if ((FALSE == mInfo->mulWinPrms.isDummyNeeded) &&
            (mInfo->mulWinPrms.numSplitWindows ==
             mInfo->mulWinPrms.numWindowsFirstRow))
        {
            descOffset = 0u;
        }

        /* Put SOCH on luma and chroma channels */
        for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
        {
            descPtr = descMem->multiWinDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            descPtr = (Void *)
                ((UInt32) descPtr + cnt * VPSHAL_VPDMA_CTRL_DESC_SIZE);

#ifdef VPS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS
            /* Form SOCH for the last free channel */
            VpsHal_vpdmaCreateSOCHCtrlDesc(descPtr, sochVpdmaChan[cnt]);
#else
            VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif

            GT_2trace(DeiCoreTrace, GT_DEBUG,
                "mInfo: 0x%p, Free Channel Number: %d\n",
                mInfo, sochVpdmaChan[cnt]);
        }
        /* Increment offset every two ctrl desc as data desc is twice the
         * size of ctrl desc */
        descOffset += (instObj->numDescPerWindow / 2u);

        /* Put Abort on luma and chroma channels */
        for (cnt = 0u; cnt < instObj->numDescPerWindow; cnt++)
        {
            descPtr = descMem->multiWinDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            descPtr = (Void *)
                ((UInt32) descPtr + cnt * VPSHAL_VPDMA_CTRL_DESC_SIZE);

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
            VpsHal_vpdmaCreateAbortCtrlDesc(
                descPtr,
                instObj->vpdmaCh[VCORE_DEI_FLD0LUMA_IDX + cnt]);
#else
            VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif
        }
        /* Increment offset every two ctrl desc as data desc is twice the
         * size of ctrl desc */
        descOffset += (instObj->numDescPerWindow / 2u);

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        /* Put abort descriptor on each row's last window free channel */
        for (cnt = 0u; cnt < numAbortDesc; cnt++)
        {
            if ((cnt & 1u) == 0u)
            {
                descPtr = descMem->multiWinDataDesc[descOffset];
                GT_assert(DeiCoreTrace, (NULL != descPtr));
            }
            else
            {
                GT_assert(DeiCoreTrace, (NULL != descPtr));
                descPtr =
                    (Void *) ((UInt32) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE);
                descOffset++;
            }

            GT_1trace(DeiCoreTrace, GT_DEBUG,
                "Abort Free Channel: %d\n", abortFreeChan[cnt]);

            VpsHal_vpdmaCreateAbortCtrlDesc(descPtr, abortFreeChan[cnt]);
        }
        if (numAbortDesc & 1u)
        {
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            descPtr =
                (Void *) ((UInt32) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE);
            /* Use dummy descriptor for odd number of abort descriptors */
            VpsHal_vpdmaCreateDummyDesc(descPtr);

            descOffset++;
        }
#endif
    }

    return (retVal);
}



/**
 *  vcoreDeiProgramPrevFldDesc
 *  \brief Programs the previous field inbound data descriptor depending on the
 *  parameters set.
 */
UInt32 vcoreDeiProgramPrevFldDesc(const Vcore_DeiInstObj *instObj,
                                  const Vcore_DeiChObj *chObj,
                                  const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      cnt, fldCnt;
    UInt32                      tempIdx;
    UInt32                      descOffset;
    UInt32                      prevFldDataFmt;
    Void                       *descPtr;
    VpsHal_VpdmaInDescParams    inDescPrm[VCORE_DEI_NUM_DESC_PER_FIELD];
    VpsHal_VpdmaInDescParams    fldN_1DescPrm[VCORE_DEI_NUM_DESC_PER_FIELD];
    VpsHal_VpdmaInDescParams   *tempDescPrm;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));

    if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
            instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
        (VPSHAL_VPDMA_CHANNEL_INVALID ==
            instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
    {
        /* No write channel available, use the data format of input */
        prevFldDataFmt = chObj->corePrms.fmt.dataFormat;
    }
    else
    {
        /* Use YUV422SP if write descriptor is present */
        prevFldDataFmt = FVID2_DF_YUV422SP_UV;
    }

    /* Set the common VPDMA parameter for previous field inputs -
     * Assume initially compressor/decompressor is disabled for all fields. */
    for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
    {
        inDescPrm[cnt].transWidth = chObj->corePrms.fmt.width;
        inDescPrm[cnt].frameWidth = chObj->corePrms.fmt.width;
        inDescPrm[cnt].startX = 0u;
        inDescPrm[cnt].startY = 0u;
        inDescPrm[cnt].notify = FALSE;
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            inDescPrm[cnt].priority = VPS_CFG_DISP_VPDMA_PRIORITY;
        }
        else
        {
            inDescPrm[cnt].priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
        }

        // KC: Fix to set memType for context field to be equal to current field memType
        #ifdef TI_816X_BUILD
        inDescPrm[cnt].memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
        #else
        inDescPrm[cnt].memType = (VpsHal_VpdmaMemoryType) chObj->corePrms.memType;
        #endif
        inDescPrm[cnt].is1DMem = FALSE;
        inDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_1;

        /* In the Progressive mode, since entire frame is queued,
           size of field is half of frame size */
        if (FVID2_SF_PROGRESSIVE == chObj->corePrms.fmt.scanFormat)
        {
            inDescPrm[cnt].transHeight = chObj->corePrms.fmt.height / 2u;
            inDescPrm[cnt].frameHeight = chObj->corePrms.fmt.height / 2u;
        }
        else /* DEI is de-interlacing, so format contains field size */
        {
            inDescPrm[cnt].transHeight = chObj->corePrms.fmt.height;
            inDescPrm[cnt].frameHeight = chObj->corePrms.fmt.height;
        }

        if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
                instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
            (VPSHAL_VPDMA_CHANNEL_INVALID ==
                instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
        {
            /* No write channel available, use the line stride of input */
            inDescPrm[cnt].lineStride =
                chObj->corePrms.fmt.pitch[chObj->bufIndex[cnt]];
        }
        else
        {
            /* Since the buffer is in YUV 422 semi-planar format, line stride
             * is same as line size for both Y and C buffer */
            inDescPrm[cnt].lineStride = VpsUtils_align(
                                            chObj->corePrms.fmt.width,
                                            VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
        }
    }

    /* Set the other VPDMA parameters */
    if (FVID2_DF_YUV422I_YUYV == prevFldDataFmt)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_YC422;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_YC422;
    }
    else if (FVID2_DF_YUV422SP_UV == prevFldDataFmt)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_Y422;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_C422;
    }
    else if (FVID2_DF_YUV420SP_UV == prevFldDataFmt)
    {
        inDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_Y420;
        inDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_C420;

        /* Chroma height is half of Luma for YUV420 */
        inDescPrm[VCORE_DEI_CBCR_IDX].transHeight /= 2u;
        inDescPrm[VCORE_DEI_CBCR_IDX].frameHeight /= 2u;
        inDescPrm[VCORE_DEI_CBCR_IDX].startY /= 2u;
    }
    else /* Invalid Data Format */
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (VPS_SOK == retVal)
    {
        /* Set the parameters for N-1 field for mode 1 */
        if (VPS_DEIHQ_CTXMODE_APP_N_1 == chObj->corePrms.deiHqCtxMode)
        {
            /* Copy original params and change only what is requried to
             * change */
            VpsUtils_memcpy(fldN_1DescPrm, inDescPrm, sizeof(fldN_1DescPrm));

            if (FVID2_DF_YUV422I_YUYV == chObj->corePrms.inFmtFldN_1.dataFormat)
            {
                fldN_1DescPrm[VCORE_DEI_Y_IDX].dataType =
                    VPSHAL_VPDMA_CHANDT_YC422;
                fldN_1DescPrm[VCORE_DEI_CBCR_IDX].dataType =
                    VPSHAL_VPDMA_CHANDT_YC422;
                fldN_1DescPrm[VCORE_DEI_Y_IDX].lineStride =
                    chObj->corePrms.inFmtFldN_1.pitch[FVID2_YUV_INT_ADDR_IDX];
                fldN_1DescPrm[VCORE_DEI_CBCR_IDX].lineStride =
                    chObj->corePrms.inFmtFldN_1.pitch[FVID2_YUV_INT_ADDR_IDX];
            }
            else
            {
                fldN_1DescPrm[VCORE_DEI_Y_IDX].dataType =
                    VPSHAL_VPDMA_CHANDT_Y422;
                fldN_1DescPrm[VCORE_DEI_CBCR_IDX].dataType =
                    VPSHAL_VPDMA_CHANDT_C422;
                fldN_1DescPrm[VCORE_DEI_Y_IDX].lineStride =
                    chObj->corePrms.inFmtFldN_1.pitch[FVID2_YUV_SP_Y_ADDR_IDX];
                fldN_1DescPrm[VCORE_DEI_CBCR_IDX].lineStride =
                    chObj->corePrms.inFmtFldN_1.pitch
                            [FVID2_YUV_SP_CBCR_ADDR_IDX];
            }
        }

        /* Program the descriptor */
        for (fldCnt = 0u; fldCnt < VCORE_DEI_MAX_PREV_FLD; fldCnt++)
        {
            /*
             *  Caution:
             *  This relies on the fact that the previous fields are in order
             *  starting from VCORE_DEI_FLD1LUMA_IDX.
             */
            tempIdx = VCORE_DEI_FLD1LUMA_IDX +
                (fldCnt * VCORE_DEI_NUM_DESC_PER_FIELD);
            for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
            {
                if (TRUE == chObj->isDescReq[tempIdx + cnt])
                {
                    if ((VPS_DEIHQ_CTXMODE_APP_N_1 ==
                            chObj->corePrms.deiHqCtxMode) && (0u == fldCnt))
                    {
                        tempDescPrm = &fldN_1DescPrm[cnt];
                    }
                    else
                    {
                        tempDescPrm = &inDescPrm[cnt];
                    }

                    /*
                     *  Caution:
                     *  Next statements relies on the fact that luma index
                     *  and chroma index are next to each other.
                     */
                    tempDescPrm->channel = instObj->vpdmaCh[tempIdx + cnt];
                    tempDescPrm->nextChannel = instObj->vpdmaCh[tempIdx + cnt];

                    descOffset = chObj->descOffset[tempIdx + cnt];
                    descPtr = descMem->inDataDesc[descOffset];
                    GT_assert(DeiCoreTrace, (NULL != descPtr));
                    retVal = VpsHal_vpdmaCreateInBoundDataDesc(
                                 descPtr,
                                 tempDescPrm);
                    if (VPS_SOK != retVal)
                    {
                        GT_0trace(DeiCoreTrace, GT_ERR,
                            "VPDMA Descriptor Creation Failed!!\n");
                        break;
                    }
                    else
                    {
                        /* Print the created descriptor memory */
                        GT_2trace(DeiCoreTrace, GT_DEBUG,
                            "Previos Field %d Descrirptor %d:\n", fldCnt, cnt);
                        GT_0trace(DeiCoreTrace, GT_DEBUG,
                            "--------------------------------\n");
                        VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
                    }
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiProgramCurOutDesc
 *  \brief Programs the current output data descriptor depending on the
 *  parameters set.
 */
UInt32 vcoreDeiProgramCurOutDesc(const Vcore_DeiInstObj *instObj,
                                 const Vcore_DeiChObj *chObj,
                                 const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      cnt, fldCnt;
    UInt32                      tempIdx;
    UInt32                      descOffset;
    UInt32                      lineStride;
    Void                       *descPtr;
    VpsHal_VpdmaOutDescParams   outDescPrm[VCORE_DEI_NUM_DESC_PER_FIELD];

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));

    /* Set the common VPDMA parameter for current field output -
     * Assume initially compressor/decompressor is disabled for all fields. */
    for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
    {
        /* Since the buffer is in YUV 422 semi-planar format, line stride
         * is same as line size for both Y and C buffer */
        lineStride = chObj->corePrms.fmt.width;
        lineStride = VpsUtils_align(lineStride, VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
        outDescPrm[cnt].lineStride = lineStride;
        outDescPrm[cnt].lineSkip = VPSHAL_VPDMA_LS_1;
        outDescPrm[cnt].notify = FALSE;
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            outDescPrm[cnt].priority = VPS_CFG_DISP_VPDMA_PRIORITY;
        }
        else
        {
            outDescPrm[cnt].priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR;
        }
        outDescPrm[cnt].memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
        outDescPrm[cnt].maxWidth =
            VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED;
        outDescPrm[cnt].maxHeight =
            VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_UNLIMITED;
    }

    /* Set the other VPDMA parameters */
    outDescPrm[VCORE_DEI_Y_IDX].dataType = VPSHAL_VPDMA_CHANDT_Y422;
    outDescPrm[VCORE_DEI_CBCR_IDX].dataType = VPSHAL_VPDMA_CHANDT_C422;

    /* Program the descriptor */
    for (fldCnt = 0u; fldCnt < VCORE_DEI_MAX_OUT_FLD; fldCnt++)
    {
        /*
         *  Caution:
         *  This relies on the fact that the fields out are in order
         *  starting from VCORE_DEI_WRLUMA_IDX.
         */
        tempIdx = VCORE_DEI_WRLUMA_IDX +
            (fldCnt * VCORE_DEI_NUM_DESC_PER_FIELD);
        for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_FIELD; cnt++)
        {
            /*
             *  Caution:
             *  Next statement relies on the fact that luma index
             *  and chroma index are next to each other.
             */
            if (TRUE == chObj->isDescReq[tempIdx + cnt])
            {
                /*
                 *  Caution:
                 *  Next statement relies on the fact that luma index
                 *  and chroma index are next to each other.
                 */
                outDescPrm[cnt].channel =
                    instObj->vpdmaCh[tempIdx + cnt];
                outDescPrm[cnt].nextChannel =
                    instObj->vpdmaCh[tempIdx + cnt];

#ifdef VPS_HAL_INCLUDE_COMPR
                /* Enable/disable 1D if compression is enabled/disabled */
                outDescPrm[cnt].is1DMem = chObj->comprHalCfg[fldCnt].enable;
#else
                outDescPrm[cnt].is1DMem = FALSE;
#endif

                descOffset = chObj->descOffset[tempIdx + cnt];
                descPtr = descMem->outDataDesc[descOffset];
                GT_assert(DeiCoreTrace, (NULL != descPtr));
                retVal = VpsHal_vpdmaCreateOutBoundDataDesc(
                             descPtr,
                             &outDescPrm[cnt]);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "VPDMA Descriptor Creation Failed!!\n");
                    break;
                }
                else
                {
                    /* Print the created descriptor memory */
                    GT_1trace(DeiCoreTrace, GT_DEBUG,
                        "Current Field Descrirptor %d:\n", cnt);
                    GT_0trace(DeiCoreTrace, GT_DEBUG,
                        "-----------------------------\n");
                    VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiProgramMvDesc
 *  \brief Programs the MV in and out descriptors depending on the parameters
 *  set.
 */
UInt32 vcoreDeiProgramMvDesc(const Vcore_DeiInstObj *instObj,
                             const Vcore_DeiChObj *chObj,
                             const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      mvCnt;
    UInt32                      tempIdx;
    UInt32                      descOffset;
    UInt32                      lineStride;
    Void                       *descPtr;
    VpsHal_VpdmaInDescParams    inDescPrm;
    VpsHal_VpdmaOutDescParams   outDescPrm;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));

    /* Set the common VPDMA parameter for MV inputs */
    inDescPrm.transWidth = chObj->corePrms.fmt.width;
    inDescPrm.frameWidth = chObj->corePrms.fmt.width;
    inDescPrm.transHeight = chObj->corePrms.fmt.height;
    inDescPrm.frameHeight = chObj->corePrms.fmt.height;
    /* 4-bits per pixel. So linesize in bytes is half the linesize in pixels */
    lineStride = chObj->corePrms.fmt.width / 2u;
    lineStride = VpsUtils_align(lineStride, VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
    inDescPrm.lineStride = lineStride;
    inDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
    inDescPrm.startX = 0u;
    inDescPrm.startY = 0u;
    inDescPrm.notify = FALSE;
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        inDescPrm.priority = VPS_CFG_DISP_VPDMA_PRIORITY;
    }
    else
    {
        inDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
    }
    inDescPrm.memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
    inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_MV;
    inDescPrm.is1DMem = FALSE;

    /* Program the descriptor */
    for (mvCnt = 0u; mvCnt < VCORE_DEI_MAX_MV_IN; mvCnt++)
    {
        /*
         *  Caution:
         *  This relies on the fact that the MVs are in order
         *  starting from VCORE_DEI_MV1_IDX.
         */
        tempIdx = VCORE_DEI_MV1_IDX + mvCnt;
        if (TRUE == chObj->isDescReq[tempIdx])
        {
            inDescPrm.channel = instObj->vpdmaCh[tempIdx];
            inDescPrm.nextChannel = instObj->vpdmaCh[tempIdx];

            /* Configure MV data descriptor */
            descOffset = chObj->descOffset[tempIdx];
            descPtr = descMem->inDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr, &inDescPrm);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "VPDMA Descriptor Creation Failed!!\n");
                break;
            }
            else
            {
                /* Print the created descriptor memory */
                GT_1trace(DeiCoreTrace, GT_DEBUG,
                    "MV %d Descrirptor:\n", mvCnt);
                GT_0trace(DeiCoreTrace, GT_DEBUG,
                    "------------------\n");
                VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
            }
        }
    }

    if ((VPS_SOK == retVal) &&
        (TRUE == chObj->isDescReq[VCORE_DEI_MVOUT_IDX]))
    {
        /* Program the MV out descriptor */
        outDescPrm.channel = instObj->vpdmaCh[VCORE_DEI_MVOUT_IDX];
        outDescPrm.nextChannel = instObj->vpdmaCh[VCORE_DEI_MVOUT_IDX];
        outDescPrm.dataType = VPSHAL_VPDMA_CHANDT_MV;
        /* 4-bits per pixel. So linesize in bytes is half the linesize in
         * pixels */
        lineStride = chObj->corePrms.fmt.width / 2u;
        lineStride = VpsUtils_align(lineStride, VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
        outDescPrm.lineStride = lineStride;
        outDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
        outDescPrm.notify = FALSE;
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            outDescPrm.priority = VPS_CFG_DISP_VPDMA_PRIORITY;
        }
        else
        {
            outDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR;
        }
        outDescPrm.memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
        outDescPrm.maxWidth = VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED;
        outDescPrm.maxHeight = VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_UNLIMITED;
        outDescPrm.is1DMem = FALSE;

        /* Configure MV out data descriptor */
        descOffset = chObj->descOffset[VCORE_DEI_MVOUT_IDX];
        descPtr = descMem->outDataDesc[descOffset];
        GT_assert(DeiCoreTrace, (NULL != descPtr));
        retVal = VpsHal_vpdmaCreateOutBoundDataDesc(descPtr, &outDescPrm);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "VPDMA Descriptor Creation Failed!!\n");
        }
        else
        {
            /* Print the created descriptor memory */
            GT_0trace(DeiCoreTrace, GT_DEBUG, "MV Out Descrirptor:\n");
            GT_0trace(DeiCoreTrace, GT_DEBUG, "-------------------\n");
            VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiProgramMvstmDesc
 *  \brief Programs the MV in and out descriptors depending on the parameters
 *  set.
 */
UInt32 vcoreDeiProgramMvstmDesc(const Vcore_DeiInstObj *instObj,
                                const Vcore_DeiChObj *chObj,
                                const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      mvstmCnt;
    UInt32                      tempIdx;
    UInt32                      descOffset;
    UInt32                      lineStride;
    Void                       *descPtr;
    VpsHal_VpdmaInDescParams    inDescPrm;
    VpsHal_VpdmaOutDescParams   outDescPrm;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));

    /* Set the common VPDMA parameter for MVSTM inputs */
    inDescPrm.transWidth = chObj->corePrms.fmt.width;
    inDescPrm.frameWidth = chObj->corePrms.fmt.width;
    inDescPrm.transHeight = chObj->corePrms.fmt.height;
    inDescPrm.frameHeight = chObj->corePrms.fmt.height;
    /* 4-bits per pixel. So linesize in bytes is half the linesize in pixels */
    lineStride = chObj->corePrms.fmt.width / 2u;
    lineStride = VpsUtils_align(lineStride, VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
    inDescPrm.lineStride = lineStride;
    inDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
    inDescPrm.startX = 0u;
    inDescPrm.startY = 0u;
    inDescPrm.notify = FALSE;
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        inDescPrm.priority = VPS_CFG_DISP_VPDMA_PRIORITY;
    }
    else
    {
        inDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
    }
    inDescPrm.memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
    inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_MV;
    inDescPrm.is1DMem = FALSE;

    for (mvstmCnt = 0; mvstmCnt < VCORE_DEI_MAX_MVSTM_IN; mvstmCnt++)
    {
        /*
         *  Caution:
         *  This relies on the fact that the MVSTMs are in order
         *  starting from VCORE_DEI_MVSTM_IDX.
         */
        tempIdx = VCORE_DEI_MVSTM_IDX + mvstmCnt;
        if (TRUE == chObj->isDescReq[tempIdx])
        {
            inDescPrm.channel = instObj->vpdmaCh[tempIdx];
            inDescPrm.nextChannel = instObj->vpdmaCh[tempIdx];

            /* Configure MVSTM data descriptor */
            descOffset = chObj->descOffset[tempIdx];
            descPtr = descMem->inDataDesc[descOffset];
            GT_assert(DeiCoreTrace, (NULL != descPtr));
            retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr, &inDescPrm);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "VPDMA Descriptor Creation Failed!!\n");
                break;
            }
            else
            {
                /* Print the created descriptor memory */
                GT_1trace(DeiCoreTrace, GT_DEBUG,
                    "MVSTM %d Descrirptor:\n", mvstmCnt);
                GT_0trace(DeiCoreTrace, GT_DEBUG,
                    "------------------\n");
                VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
            }
        }
    }

    if ((VPS_SOK == retVal) &&
        (TRUE == chObj->isDescReq[VCORE_DEI_MVSTMOUT_IDX]))
    {
        /* Program the MVSTM out descriptor */
        outDescPrm.channel = instObj->vpdmaCh[VCORE_DEI_MVSTMOUT_IDX];
        outDescPrm.nextChannel = instObj->vpdmaCh[VCORE_DEI_MVSTMOUT_IDX];
        outDescPrm.dataType = VPSHAL_VPDMA_CHANDT_MV;
        /* 4-bits per pixel. So linesize in bytes is half the linesize in
         * pixels */
        lineStride = chObj->corePrms.fmt.width / 2u;
        lineStride = VpsUtils_align(lineStride, VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
        outDescPrm.lineStride = lineStride;
        outDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
        outDescPrm.notify = FALSE;
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            outDescPrm.priority = VPS_CFG_DISP_VPDMA_PRIORITY;
        }
        else
        {
            outDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR;
        }
        outDescPrm.memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
        outDescPrm.maxWidth = VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED;
        outDescPrm.maxHeight = VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_UNLIMITED;
        outDescPrm.is1DMem = FALSE;

        /* Configure MVSTM out data descriptor */
        descOffset = chObj->descOffset[VCORE_DEI_MVSTMOUT_IDX];
        descPtr = descMem->outDataDesc[descOffset];
        GT_assert(DeiCoreTrace, (NULL != descPtr));
        retVal = VpsHal_vpdmaCreateOutBoundDataDesc(descPtr, &outDescPrm);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "VPDMA Descriptor Creation Failed!!\n");
        }
        else
        {
            /* Print the created descriptor memory */
            GT_0trace(DeiCoreTrace, GT_DEBUG, "MVSTM Out Descrirptor:\n");
            GT_0trace(DeiCoreTrace, GT_DEBUG, "-------------------\n");
            VpsHal_vpdmaPrintDesc(descPtr, DeiCoreTrace);
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiHqFigureOutState
 *  \brief Figures out the DEI HQ state machine parameters like previous field
 *  buffers, number of MVSTM buffers, MV buffers etc... based on DEI params
 *  provided at the time of set params.
 */
Int32 vcoreDeiHqFigureOutState(const Vcore_DeiInstObj *instObj,
                               Vcore_DeiChObj *chObj,
                               const Vcore_DeiParams *corePrms)
{
    Int32               retVal = VPS_SOK;
    UInt32              chCnt;
    Vcore_DeiCtxInfo   *ctxInfo;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != corePrms));
    ctxInfo = &chObj->ctxInfo;

    /* Initialize all first */
    for (chCnt = 0u; chCnt < VCORE_DEI_MAX_VPDMA_CH; chCnt++)
    {
        /* Indicates descriptors are not needed */
        chObj->isDescReq[chCnt] = FALSE;
        /* Indicates all of them are dummy */
        chObj->isDescDummy[chCnt] = TRUE;
    }

    if (VPS_DEIHQ_FLDMODE_4FLD == corePrms->deiHqCfg.fldMode)
    {
        ctxInfo->numPrevFld = 2u;
        ctxInfo->numMvIn = 1u;
    }
    else /* 5-Field Mode */
    {
        ctxInfo->numPrevFld = 3u;
        ctxInfo->numMvIn = 2u;
    }

    /* In Low Cost Mode, MVSTM loop is not needed */
    if ((TRUE == corePrms->deiHqCfg.lcModeEnable) &&
        (FALSE == corePrms->deiHqCfg.mvstmEnable))
    {
        ctxInfo->numMvstmIn = 0u;
        ctxInfo->numMvstmOut = 0u;
    }
    else /* Advanced mode or low cost mode with mvstm loop */
    {
        if (VPS_DEIHQ_CTXMODE_APP_N_1 == corePrms->deiHqCtxMode)
        {
            /* 1 input and 1 output buffers are needed
             * for MVSTM loop in Mode1 */
            ctxInfo->numMvstmIn = 1u;
        }
        else
        {
            /* 2 input and 1 output buffers are needed for MVSTM loop */
            ctxInfo->numMvstmIn = 2u;
        }
        ctxInfo->numMvstmOut = 1u;
    }

    if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
            instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
        (VPSHAL_VPDMA_CHANNEL_INVALID ==
            instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
    {
        /* No write channel available */
        ctxInfo->numCurFldOut = 0u;
    }
    else
    {
        /* Current Field out is always one except for Progressive TNR case */
        ctxInfo->numCurFldOut = 1u;
    }

    /* Except for the case of bypass mode or tempBypass is enabled,
     * mv out is always needed */
    ctxInfo->numMvOut = 1u;

    if (TRUE == corePrms->deiHqCfg.bypass)
    {
        /* Bypass mode - no previous, MV and output required */
        ctxInfo->isDeinterlacing = FALSE;
        ctxInfo->isTnrMode = FALSE;
        ctxInfo->numPrevFld = 0u;
        ctxInfo->numMvIn = 0u;
        ctxInfo->numMvOut = 0u;
        ctxInfo->numMvstmIn = 0u;
        ctxInfo->numMvstmOut = 0u;
        ctxInfo->numCurFldOut = 0u;

        /* Only Currect Fields/Frame is needed for the bypass mode */
        chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
        chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;

        /* Set flag to indicate whether the required descriptor is
         * dummy or not. Use buffer memset to zero for dummy read
         * descriptors and set the drop data for dummy write descriptor */
        chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
        chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;

        /* For Progressive mode, if tnr mode is enabled, then it needs one
           previous frame as well it will write out current frame as two
           fields */
        if ((TRUE == corePrms->deiHqCfg.tnrEnable) &&
            (FVID2_SF_PROGRESSIVE == corePrms->fmt.scanFormat))
        {
            ctxInfo->isTnrMode = TRUE;
            vcoreDeiHqCalStateTnr(instObj, chObj);
        }
    }
    else /* DEI is deinterlacing */
    {
        ctxInfo->isTnrMode = FALSE;
        if (VPS_DEIHQ_EDIMODE_LINE_AVG == corePrms->deiHqCfg.inpMode)
        {
            ctxInfo->isDeinterlacing = TRUE;
            vcoreDeiHqCalStateLineAvg(instObj, chObj);
        }
        else if (VPS_DEIHQ_EDIMODE_FLD_AVG == corePrms->deiHqCfg.inpMode)
        {
            ctxInfo->isDeinterlacing = TRUE;
            vcoreDeiHqCalStateFldAvg(instObj, chObj);
        }
        else
        {
            ctxInfo->isDeinterlacing = TRUE;
            vcoreDeiHqCalStateEdi(instObj, chObj);
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiFigureOutState
 *  \brief Figures out the DEI state machine parameters like previous field
 *  buffers, number of MVSTM buffers, MV buffers etc... based on DEI params
 *  provided at the time of set params.
 */
Int32 vcoreDeiFigureOutState(const Vcore_DeiInstObj *instObj,
                             Vcore_DeiChObj *chObj,
                             const Vcore_DeiParams *corePrms)
{
    Int32               retVal = VPS_SOK;
    UInt32              chCnt;
    Vcore_DeiCtxInfo   *ctxInfo;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != corePrms));
    ctxInfo = &chObj->ctxInfo;

    /* Initialize all first */
    for (chCnt = 0u; chCnt < VCORE_DEI_MAX_VPDMA_CH; chCnt++)
    {
        /* Indicates descriptors are not needed */
        chObj->isDescReq[chCnt] = FALSE;
        /* Indicates all of them are dummy */
        chObj->isDescDummy[chCnt] = TRUE;
    }
    ctxInfo->isTnrMode = FALSE;

    if (TRUE == corePrms->deiCfg.bypass)
    {
        /* Bypass mode - no previous, MV and output required */
        ctxInfo->isDeinterlacing = FALSE;
        ctxInfo->numPrevFld = 0u;
        ctxInfo->numMvIn = 0u;
        ctxInfo->numMvOut = 0u;
        ctxInfo->numMvstmIn = 0u;
        ctxInfo->numMvstmOut = 0u;
        ctxInfo->numCurFldOut = 0u;

        /* Set flag to indicate whether a descriptor is required or not */
        chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
        chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;

        /* Set flag to indicate whether the required descriptor is
         * dummy or not. Use buffer memset to zero for dummy read
         * descriptors and set the drop data for dummy write descriptor. */
        chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
        chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;
    }
    else
    {
        ctxInfo->isDeinterlacing = TRUE;
        if (VPS_DEI_EDIMODE_LINE_AVG == corePrms->deiCfg.inpMode)
        {
            ctxInfo->numPrevFld = 2u;
            ctxInfo->numMvIn = 1u;
            ctxInfo->numMvOut = 1u;
            ctxInfo->numMvstmIn = 0u;
            ctxInfo->numMvstmOut = 0u;
            if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
                    instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
                (VPSHAL_VPDMA_CHANNEL_INVALID ==
                    instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
            {
                /* No write channel available */
                ctxInfo->numCurFldOut = 0u;
            }
            else
            {
                ctxInfo->numCurFldOut = 1u;
                chObj->isDescReq[VCORE_DEI_WRLUMA_IDX] = TRUE;
                chObj->isDescReq[VCORE_DEI_WRCHROMA_IDX] = TRUE;
                chObj->isDescDummy[VCORE_DEI_WRLUMA_IDX] = TRUE;
                chObj->isDescDummy[VCORE_DEI_WRCHROMA_IDX] = TRUE;
            }

            /* Set flag to indicate whether a descriptor is required or not */
            chObj->isDescReq[VCORE_DEI_FLD1LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD2LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD2CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_MV1_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_MVOUT_IDX] = TRUE;

            /* Set flag to indicate whether the required descriptor is
             * dummy or not. Use buffer memset to zero for dummy read
             * descriptors and set the drop data for dummy write descriptor. */
            chObj->isDescDummy[VCORE_DEI_FLD1LUMA_IDX] = TRUE;
            chObj->isDescDummy[VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
            chObj->isDescDummy[VCORE_DEI_FLD2LUMA_IDX] = TRUE;
            chObj->isDescDummy[VCORE_DEI_FLD2CHROMA_IDX] = TRUE;
            chObj->isDescDummy[VCORE_DEI_MV1_IDX] = TRUE;
            chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_MVOUT_IDX] = TRUE;
        }
        else if (VPS_DEI_EDIMODE_FLD_AVG == corePrms->deiCfg.inpMode)
        {
            ctxInfo->numPrevFld = 2u;
            ctxInfo->numMvIn = 1u;
            ctxInfo->numMvOut = 1u;
            ctxInfo->numMvstmIn = 0u;
            ctxInfo->numMvstmOut = 0u;
            if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
                    instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
                (VPSHAL_VPDMA_CHANNEL_INVALID ==
                    instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
            {
                /* No write channel available */
                ctxInfo->numCurFldOut = 0u;
            }
            else
            {
                ctxInfo->numCurFldOut = 1u;
                chObj->isDescReq[VCORE_DEI_WRLUMA_IDX] = TRUE;
                chObj->isDescReq[VCORE_DEI_WRCHROMA_IDX] = TRUE;
                chObj->isDescDummy[VCORE_DEI_WRLUMA_IDX] = FALSE;
                chObj->isDescDummy[VCORE_DEI_WRCHROMA_IDX] = FALSE;
            }

            /* Set flag to indicate whether a descriptor is required or not */
            chObj->isDescReq[VCORE_DEI_FLD1LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD2LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD2CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_MV1_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_MVOUT_IDX] = TRUE;

            /* Set flag to indicate whether the required descriptor is
             * dummy or not. Use buffer memset to zero for dummy read
             * descriptors and set the drop data for dummy write descriptor. */
            chObj->isDescDummy[VCORE_DEI_FLD1LUMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD1CHROMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD2LUMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD2CHROMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_MV1_IDX] = TRUE;
            chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_MVOUT_IDX] = TRUE;
        }
        else
        {
            ctxInfo->numPrevFld = 2u;
            ctxInfo->numMvIn = 1u;
            ctxInfo->numMvOut = 1u;
            ctxInfo->numMvstmIn = 0u;
            ctxInfo->numMvstmOut = 0u;
            if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
                    instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
                (VPSHAL_VPDMA_CHANNEL_INVALID ==
                    instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
            {
                /* No write channel available */
                ctxInfo->numCurFldOut = 0u;
            }
            else
            {
                ctxInfo->numCurFldOut = 1u;
                chObj->isDescReq[VCORE_DEI_WRLUMA_IDX] = TRUE;
                chObj->isDescReq[VCORE_DEI_WRCHROMA_IDX] = TRUE;
                chObj->isDescDummy[VCORE_DEI_WRLUMA_IDX] = FALSE;
                chObj->isDescDummy[VCORE_DEI_WRCHROMA_IDX] = FALSE;
            }

            /* Set flag to indicate whether a descriptor is required or not */
            chObj->isDescReq[VCORE_DEI_FLD1LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD2LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD2CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_MV1_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;
            chObj->isDescReq[VCORE_DEI_MVOUT_IDX] = TRUE;

            /* Set flag to indicate whether the required descriptor is
             * dummy or not. Use buffer memset to zero for dummy read
             * descriptors and set the drop data for dummy write descriptor. */
            chObj->isDescDummy[VCORE_DEI_FLD1LUMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD1CHROMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD2LUMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD2CHROMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_MV1_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;
            chObj->isDescDummy[VCORE_DEI_MVOUT_IDX] = FALSE;
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiHqUpdateMode
 *  \brief Update the DEI HQ mode based on the previous state. This is used
 *  for the reset sequence.
 */
Int32 vcoreDeiHqUpdateMode(Vcore_DeiInstObj *instObj,
                           Vcore_DeiChObj *chObj,
                           const Vcore_DescMem *descMem,
                           UInt32 prevFldState)
{
    Int32                   retVal = VPS_SOK;
#ifdef VPS_HAL_INCLUDE_DEIH
    Void                   *cfgOvlyPtr = NULL;
    VpsHal_DeihEdiConfig    ediCfg;

    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX]));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    GT_assert(DeiCoreTrace, (NULL != descMem->shadowOvlyMem));

    /* No need to enter the "change mode state" in line average mode!! */
    if (VPS_DEIHQ_EDIMODE_LINE_AVG != chObj->corePrms.deiHqCfg.inpMode)
    {
        /* Use Line Average Mode for the first few frames */
        if (0u == prevFldState)
        {
            ediCfg.inpMode = VPS_DEIHQ_EDIMODE_LINE_AVG;
            ediCfg.chromaEdiEnable = chObj->corePrms.deiHqCfg.chromaEdiEnable;

            /* Set only the EDI mode */
            cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                + instObj->ovlyOffset[VCORE_DEI_DEI_IDX]);
            retVal = VpsHal_deihSetEdiConfig(
                         instObj->halHandle[VCORE_DEI_DEI_IDX],
                         &ediCfg,
                         cfgOvlyPtr);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "DEI HQ HAL Set EDI Config Overlay Failed!!\n");
            }
        }
        else
        {
            /* Switch to the required mode when all the previous fields are
             * available */
            if (prevFldState == chObj->ctxInfo.numPrevFld)
            {
                ediCfg.inpMode = chObj->corePrms.deiHqCfg.inpMode;
                ediCfg.chromaEdiEnable =
                    chObj->corePrms.deiHqCfg.chromaEdiEnable;

                /* Set only the EDI mode */
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_DEI_IDX]);
                retVal = VpsHal_deihSetEdiConfig(
                             instObj->halHandle[VCORE_DEI_DEI_IDX],
                             &ediCfg,
                             cfgOvlyPtr);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "DEI HQ HAL Set EDI Config Overlay Failed!!\n");
                }
            }
        }
    }
#endif

    return (retVal);
}



/**
 *  vcoreDeiUpdateMode
 *  \brief Update the DEI mode based on the previous state. This is used
 *  for the reset sequence.
 */
Int32 vcoreDeiUpdateMode(Vcore_DeiInstObj *instObj,
                         Vcore_DeiChObj *chObj,
                         const Vcore_DescMem *descMem,
                         UInt32 prevFldState)
{
    Int32                   retVal = VPS_SOK;
    Void                   *cfgOvlyPtr = NULL;
    VpsHal_DeiEdiConfig     ediCfg;

    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != instObj->halHandle[VCORE_DEI_DEI_IDX]));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != descMem));
    GT_assert(DeiCoreTrace, (NULL != descMem->shadowOvlyMem));

    /* No need to enter the "change mode state" in line average mode!! */
    if (VPS_DEI_EDIMODE_LINE_AVG != chObj->corePrms.deiCfg.inpMode)
    {
        /* Use Line Average Mode for the first few frames */
        if (0u == prevFldState)
        {
            ediCfg.inpMode = VPS_DEI_EDIMODE_LINE_AVG;
            ediCfg.tempInpEnable = chObj->corePrms.deiCfg.tempInpEnable;
            ediCfg.tempInpChromaEnable =
                chObj->corePrms.deiCfg.tempInpChromaEnable;

            /* Set only the EDI mode */
            cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                + instObj->ovlyOffset[VCORE_DEI_DEI_IDX]);
            retVal = VpsHal_deiSetEdiConfig(
                         instObj->halHandle[VCORE_DEI_DEI_IDX],
                         &ediCfg,
                         cfgOvlyPtr);
            if (VPS_SOK != retVal)
            {
                GT_0trace(DeiCoreTrace, GT_ERR,
                    "DEI HAL Set EDI Config Overlay Failed!!\n");
            }
        }
        else
        {
            /* Switch to the required mode when all the previous fields are
             * available */
            if (prevFldState == chObj->ctxInfo.numPrevFld)
            {
                ediCfg.inpMode = chObj->corePrms.deiCfg.inpMode;
                ediCfg.tempInpEnable = chObj->corePrms.deiCfg.tempInpEnable;
                ediCfg.tempInpChromaEnable =
                    chObj->corePrms.deiCfg.tempInpChromaEnable;

                /* Set only the EDI mode */
                cfgOvlyPtr = (Void *) ((UInt32) descMem->shadowOvlyMem
                    + instObj->ovlyOffset[VCORE_DEI_DEI_IDX]);
                retVal = VpsHal_deiSetEdiConfig(
                             instObj->halHandle[VCORE_DEI_DEI_IDX],
                             &ediCfg,
                             cfgOvlyPtr);
                if (VPS_SOK != retVal)
                {
                    GT_0trace(DeiCoreTrace, GT_ERR,
                        "DEI HQ HAL Set EDI Config Overlay Failed!!\n");
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiHqCalStateLineAvg
 *  \brief Calculate the descriptors requirements for the Line Average Mode.
 */
static Void vcoreDeiHqCalStateLineAvg(const Vcore_DeiInstObj *instObj,
                                      Vcore_DeiChObj *chObj)
{
    UInt32              cnt, offset;
    Vcore_DeiCtxInfo   *ctxInfo = NULL;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    ctxInfo = &chObj->ctxInfo;

    /* In Line Average mode, previous field descriptors are needed and
       number of previous field descriptors depends on the configuration */
    for (cnt = 0; cnt < ctxInfo->numPrevFld; cnt++)
    {
        offset = cnt * 2u;
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[offset + VCORE_DEI_FLD1LUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1LUMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
    }

    for (cnt = 0; cnt < ctxInfo->numMvIn; cnt++)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[cnt + VCORE_DEI_MV1_IDX] = TRUE;
        chObj->isDescDummy[cnt + VCORE_DEI_MV1_IDX] = TRUE;
    }

    if (ctxInfo->numMvOut > 0)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[VCORE_DEI_MVOUT_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVOUT_IDX] = TRUE;
    }

    if (ctxInfo->numMvstmIn > 0)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[VCORE_DEI_MVSTM_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVSTM_IDX] = TRUE;
    }

    if (ctxInfo->numMvstmOut > 0)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[VCORE_DEI_MVSTMOUT_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVSTMOUT_IDX] = TRUE;
    }

    for (cnt = 0; cnt < ctxInfo->numCurFldOut; cnt++)
    {
        /* These descriptors are needed, but they are dummy */
        offset = cnt * 2u;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRLUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRLUMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = TRUE;
    }
    /* Set flag to indicate whether a descriptor is required or not */
    chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
    chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;

    chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
    chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;

    return;
}



/**
 *  vcoreDeiHqCalStateFldAvg
 *  \brief Calculate the descriptors requirements for the Field Average Mode.
 */
static Void vcoreDeiHqCalStateFldAvg(const Vcore_DeiInstObj *instObj,
                                     Vcore_DeiChObj *chObj)
{
    UInt32              cnt, offset;
    Vcore_DeiCtxInfo   *ctxInfo = NULL;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    ctxInfo = &chObj->ctxInfo;

    /* In Field Average mode, previous field descriptors are needed and
       number of previous descriptors depends on the configuration */
    for (cnt = 0; cnt < ctxInfo->numPrevFld; cnt++)
    {
        /* These descriptors are needed */
        offset = cnt * 2u;
        chObj->isDescReq[offset + VCORE_DEI_FLD1LUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1LUMA_IDX] = FALSE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1CHROMA_IDX] = FALSE;
    }

    for (cnt = 0; cnt < ctxInfo->numMvIn; cnt++)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[cnt + VCORE_DEI_MV1_IDX] = TRUE;
        chObj->isDescDummy[cnt + VCORE_DEI_MV1_IDX] = TRUE;
    }

    if (ctxInfo->numMvOut > 0)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[VCORE_DEI_MVOUT_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVOUT_IDX] = TRUE;
    }

    if (ctxInfo->numMvstmIn > 0)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[VCORE_DEI_MVSTM_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVSTM_IDX] = TRUE;
    }

    if (ctxInfo->numMvstmOut > 0)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[VCORE_DEI_MVSTMOUT_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVSTMOUT_IDX] = TRUE;
    }

    for (cnt = 0; cnt < ctxInfo->numCurFldOut; cnt++)
    {
        /* These descriptors are needed */
        offset = cnt * 2u;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRLUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRLUMA_IDX] = FALSE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = FALSE;
    }
    /* Set flag to indicate whether a descriptor is required or not */
    chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
    chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;

    chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
    chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;

    return;
}



/**
 *  vcoreDeiHqCalStateEdi
 *  \brief Calculate the descriptors requirements for the EDI Mode.
 */
static Void vcoreDeiHqCalStateEdi(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj)
{
    UInt32              cnt, offset;
    Vcore_DeiCtxInfo   *ctxInfo = NULL;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    ctxInfo = &chObj->ctxInfo;

    /* In Line Average mode, previous field descriptors are needed
       depending on the number of fields */
    for (cnt = 0; cnt < ctxInfo->numPrevFld; cnt++)
    {
        offset = cnt * 2u;
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[offset + VCORE_DEI_FLD1LUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1LUMA_IDX] = FALSE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1CHROMA_IDX] = FALSE;
    }

    for (cnt = 0; cnt < ctxInfo->numMvIn; cnt++)
    {
        /* These descriptors are needed, but they are dummy */
        chObj->isDescReq[cnt + VCORE_DEI_MV1_IDX] = TRUE;
        chObj->isDescDummy[cnt + VCORE_DEI_MV1_IDX] = FALSE;
    }

    if (ctxInfo->numMvOut > 0)
    {
        chObj->isDescReq[VCORE_DEI_MVOUT_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVOUT_IDX] = FALSE;
    }

    if (ctxInfo->numMvstmIn > 0)
    {
        chObj->isDescReq[VCORE_DEI_MVSTM_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVSTM_IDX] = FALSE;
    }

    if (ctxInfo->numMvstmOut > 0)
    {
        chObj->isDescReq[VCORE_DEI_MVSTMOUT_IDX] = TRUE;
        chObj->isDescDummy[VCORE_DEI_MVSTMOUT_IDX] = FALSE;
    }

    for (cnt = 0; cnt < ctxInfo->numCurFldOut; cnt++)
    {
        offset = cnt * 2u;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRLUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRLUMA_IDX] = FALSE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = FALSE;
    }
    /* Set flag to indicate whether a descriptor is required or not */
    chObj->isDescReq[VCORE_DEI_FLD0LUMA_IDX] = TRUE;
    chObj->isDescReq[VCORE_DEI_FLD0CHROMA_IDX] = TRUE;

    chObj->isDescDummy[VCORE_DEI_FLD0LUMA_IDX] = FALSE;
    chObj->isDescDummy[VCORE_DEI_FLD0CHROMA_IDX] = FALSE;

    return;
}



/**
 *  vcoreDeiHqCalStateTnr
 *  \brief Calculate the descriptors requirements for the EDI Mode.
 */
static Void vcoreDeiHqCalStateTnr(const Vcore_DeiInstObj *instObj,
                                  Vcore_DeiChObj *chObj)
{
    UInt32              cnt, offset;
    Vcore_DeiCtxInfo   *ctxInfo = NULL;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    ctxInfo = &chObj->ctxInfo;

    ctxInfo->numPrevFld = 2u;
    if ((VPSHAL_VPDMA_CHANNEL_INVALID ==
            instObj->vpdmaCh[VCORE_DEI_WRLUMA_IDX]) ||
        (VPSHAL_VPDMA_CHANNEL_INVALID ==
            instObj->vpdmaCh[VCORE_DEI_WRCHROMA_IDX]))
    {
        /* No write channel available */
        ctxInfo->numCurFldOut = 0u;
    }
    else
    {
        ctxInfo->numCurFldOut = 2u;
    }
    for (cnt = 0; cnt < ctxInfo->numPrevFld; cnt++)
    {
        /* These descriptors are needed */
        offset = cnt * 2u;
        chObj->isDescReq[offset + VCORE_DEI_FLD1LUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_FLD1CHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1LUMA_IDX] = FALSE;
        chObj->isDescDummy[offset + VCORE_DEI_FLD1CHROMA_IDX] = FALSE;
    }
    for (cnt = 0; cnt < ctxInfo->numCurFldOut; cnt++)
    {
        /* These descriptors are needed, but they are dummy */
        offset = cnt * 2u;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRLUMA_IDX] = TRUE;
        chObj->isDescReq[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = TRUE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRLUMA_IDX] = FALSE;
        chObj->isDescDummy[offset + VCORE_DEI_TOP_WRCHROMA_IDX] = FALSE;
    }

    return;
}
