/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDeiPlatform.c
 *
 *  \brief VPS DEI M2M driver internal file used for platform specific
 *  functions.
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

static Int32 vpsMdrvDeiClearInFrameList(FVID2_FrameList *framesList,
                                        UInt32 numFrames);
static Int32 vpsMdrvDeiReleaseCtxFields(VpsMdrv_DeiHandleObj *hObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          FVID2 API Functions                               */
/* ========================================================================== */

/**
 * vpsMdrvDeiAgeCtxBufs
 * Age stored context buffers - This function is responsible for releasing
 * context fields held by the driver back to application, when dequeued
 * and also for storing required fields for context
 */
Int32 vpsMdrvDeiAgeCtxBufs(VpsMdrv_DeiChObj *chObj, FVID2_Frame *curFrame)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  currFid, agedTth;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;
    FVID2_Frame            *agedField, *ctxField;
    Int32                   ctxFldIdx;

    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    deiCoreCtxBuf = &chObj->deiCoreCtxBuf;

    ctxField    =   NULL;
    agedField   =   NULL;
    while (TRUE)
    {
        /* If we are not de-interlacing - just put the current input field
           in the aged Q, when completed, the same would be populated in the
           inFrameList */
        if (FALSE == chObj->deiCoreCtxInfo.isDeinterlacing)
        {
            retVal = VpsUtils_quePut(&chObj->agedCtxFieldQ,
                                    (Ptr) curFrame,
                                    BIOS_NO_WAIT);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not put aged fields into channels Q");
            }
            break;
        }

        /* Determine the field indexs of context buffer - based on current
            fields fid. Requires the context fields fid to be in sync with
            current fields fid. */
        if (FVID2_FID_BOTTOM == curFrame->fid)
        {
            currFid = 1u;
        }
        else
        {
            currFid = 0u;
        }
        /* Ageing of context fields
         * 1. If context buffer are initialized
         *      1.1. Pull out the last context field, put into Q if not
         *              duplicated field.
         *      1.2. Age others and move privious N field into N - 1,
         *              N - 1 to N - 2, etc...
         * 2. Otherwise
         *      2.1 Initialize context field with current field with TTH huge no
         *      2.2 Remmember current field with tth as
         *          VPSMDRV_DEI_MAX_FIELD_CONTEXT
         */
         if (TRUE == chObj->isCtxFieldInit)
         {
            /* Age fields */
            agedField =
                chObj->ctxFieldFifo[VPSMDRV_DEI_MAX_FIELD_CONTEXT].fieldCtxBuf;
            agedTth   = chObj->ctxFieldFifo[VPSMDRV_DEI_MAX_FIELD_CONTEXT].tth;

            for (ctxFldIdx = VPSMDRV_DEI_MAX_FIELD_CONTEXT;
                 ctxFldIdx > 0u;
                 ctxFldIdx--)
            {
                chObj->ctxFieldFifo[ctxFldIdx].fieldCtxBuf =
                    chObj->ctxFieldFifo[ctxFldIdx - 1u].fieldCtxBuf;
                chObj->ctxFieldFifo[ctxFldIdx].tth =
                    chObj->ctxFieldFifo[ctxFldIdx - 1u].tth;
                chObj->ctxFieldFifo[ctxFldIdx].tth--;
            }
            /* Release aged field
               Do not release duped fields - For a valid context field the tth
               would be 0. As current field has tth of value
               VPSMDRV_DEI_MAX_FIELD_CONTEXT, and reduced every time new fields
               are available for de-interlacing */
            if (0u == agedTth)
            {
                if (agedField != NULL)
                {
                    retVal = VpsUtils_quePut(
                                 &chObj->agedCtxFieldQ,
                                 (Ptr) agedField,
                                 BIOS_NO_WAIT);
                    if (FVID2_SOK != retVal)
                    {
                        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                            "Could not put aged fields into channels Q");
                        break;
                    }
                }
            } /* Otherwise field is duplicate field - ignore them */
            else
            {
                /* Why would this happen... */
                if ((agedTth != (VPSMDRV_DEI_DUPE_FIELD_CONTEXT - 1)) &&
                    (agedTth != (VPSMDRV_DEI_DUPE_FIELD_CONTEXT - 2)) &&
                    (agedTth != (VPSMDRV_DEI_DUPE_FIELD_CONTEXT)))
                {
                    GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                        "Fields Age is in-correct %d", agedTth);
                }
            }
         }
         else
         {
            /* When we do not have enough contexts, current field is treated as
               context fields. */
            for (ctxFldIdx = 1u;
                 ctxFldIdx <= VPSMDRV_DEI_MAX_FIELD_CONTEXT;
                 ctxFldIdx++)
            {
                chObj->ctxFieldFifo[ctxFldIdx].fieldCtxBuf = curFrame;
                chObj->ctxFieldFifo[ctxFldIdx].tth =
                    VPSMDRV_DEI_DUPE_FIELD_CONTEXT;
            }

            /* Also invert the field ID so that the deinterlaced output of the
             * first frame is not shifted up/down. */
            currFid = currFid ^ 1u;

            chObj->isCtxFieldInit = TRUE;
         }

        /* Remmember the current field, used as N - 1 context field for the
           field to be de-interlaced */
        chObj->ctxFieldFifo[0u].fieldCtxBuf = curFrame;
        chObj->ctxFieldFifo[0u].tth = VPSMDRV_DEI_MAX_FIELD_CONTEXT;

        /* Update the cores context information with latest context fields */
        for (ctxFldIdx = 0u;
             ctxFldIdx < chObj->deiCoreCtxInfo.numPrevFld;
             ctxFldIdx++)
        {
            ctxField = chObj->ctxFieldFifo[ctxFldIdx + 1u].fieldCtxBuf;
            GT_assert(VpsMdrvDeiTrace, (NULL != ctxField));

            /* Assign the field Y and CbCr buffer pointers */
            deiCoreCtxBuf->prevInBuf[ctxFldIdx][FVID2_YUV_SP_Y_ADDR_IDX] =
                ctxField->addr[FVID2_FIELD_MODE_ADDR_IDX]
                              [FVID2_YUV_SP_Y_ADDR_IDX];
            deiCoreCtxBuf->prevInBuf[ctxFldIdx][FVID2_YUV_SP_CBCR_ADDR_IDX] =
                ctxField->addr[FVID2_FIELD_MODE_ADDR_IDX]
                              [FVID2_YUV_SP_CBCR_ADDR_IDX];

            /* Assign field ID - alternatively */
            currFid = currFid ^ 1u;
            deiCoreCtxBuf->prevInFid[ctxFldIdx] = currFid;
        }

        break;
    }

    return (retVal);
}



/**
 * vpsMdrvDeiReleaseAgedCtxBufs
 * Updates the inFrame list with the inFrames that are no longer need by the
 * driver for context.
 */
Int32 vpsMdrvDeiReleaseAgedCtxBufs(VpsMdrv_DeiHandleObj *hObj,
                                   VpsMdrv_DeiQueueObj *qObj)
{
    Int32 retVal = FVID2_SOK;
    UInt32 chNum;
    VpsMdrv_DeiChObj *chObj;
    FVID2_Frame *agedField = NULL;
    FVID2_FrameList *inFramesList = NULL;

    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != qObj));

    /* We will have to update only one instance of frame list. */
    inFramesList =
        qObj->procList.inFrameList[VPSMDRV_DEI_INST_IN_FRAME_LIST_USED];
    /* Inframe list will point to apps given fields, currently might be stored
       context fields, ensure they stand cleared */
    retVal = vpsMdrvDeiClearInFrameList(inFramesList, hObj->numCh);
    /* Though we do not return any field contexts, set the numInList to 1
       to ensure inFrameList pointer of type FVID2_FrameList allocated by the
       application is not lost */
    qObj->procList.numInLists = 1u;
    /* When stopped, we would have released all the input fields. */
    if (hObj->isStopped == FALSE)
    {
        for (chNum = 0u; chNum < hObj->numCh; chNum++)
        {
            chObj = hObj->chObjs[chNum];
            GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
            retVal = VpsUtils_queGet(
                         &chObj->agedCtxFieldQ,
                         (Ptr) &(agedField),
                         1u,
                         BIOS_NO_WAIT);
            if (FVID2_SOK == retVal)
            {
                GT_assert(VpsMdrvDeiTrace, (NULL != agedField));

                inFramesList->frames[chNum] = agedField;
            }/* Otherwise, there were no aged fields available for this channel
                ignore, move on to next channel */
        }
        /* Ensure to update the no of frames - set it to max, apps are expected
           to look for valid frames */
        inFramesList->numFrames = hObj->numCh;
    }/*Otherwise, we do not anything else to be done here... */
    retVal = FVID2_SOK; /* Require to let know that everything went well
                           even in case there were no aged fields */

    return (retVal);
}



/**
 * vpsMdrvDeiStop
 * Stops processing of feilds queued up.
 */
Int32 vpsMdrvDeiStop(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32       retVal = FVID2_SOK;
    UInt32      cookie;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));

    cookie = Hwi_disable();
    while(TRUE)
    {
        /* Check if have pending request, if so, release them */
        if (0u != hObj->numPendReq)
        {
            /* Let the mlmCb know that we are trying to stop, do not process
             * Queued up fields and mlmCb is reponsible of releasing up the
             * context fields */
            hObj->isStopped = TRUE;
        }

        /* Q up the stored context feilds as completed feilds */
        retVal = vpsMdrvDeiReleaseCtxFields(hObj);
        break;
    }
    Hwi_restore(cookie);

    return (retVal);
}



/**
 * vpsMdrvDeiIsCtxFieldsSet
 * Check if any of the channel(s) has its context field set
 */
UInt32 vpsMdrvDeiIsCtxFieldsSet(VpsMdrv_DeiHandleObj *hObj)
{
    UInt32 retVal = FALSE;
    VpsMdrv_DeiChObj *chObj;
    UInt32 chIndex;

    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    for(chIndex = 0u; chIndex < hObj->numCh; chIndex++)
    {
        chObj = hObj->chObjs[chIndex];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        if (chObj->isCtxFieldInit != FALSE)
        {
            retVal = TRUE;
            break;
        }
    }
    return (retVal);
}



/**
 *  vpsMdrvDeiOverridePrevFldBufIoctl
 */
Int32 vpsMdrvDeiOverridePrevFldBufIoctl(VpsMdrv_DeiHandleObj *hObj,
                                        Ptr cmdArgs)
{
    Int32                           retVal = FVID2_SOK;
    Bool                            isDeiPresent;
    UInt32                          platformId;
    UInt32                          chNum;
    UInt32                          fldCnt, frmCnt, numFrames;
    FVID2_Frame                    *frame;
    FVID2_Frame                    *prevFrame[VPSMDRV_DEI_MAX_FIELD_CONTEXT];
    VpsMdrv_DeiChObj               *chObj;
    Vps_M2mDeiOverridePrevFldBuf   *prevFldBuf;

    /* Check for NULL pointers */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    if (FVID2_SOK == retVal)
    {
        platformId = Vps_platformGetId();
        if (!((VPS_PLATFORM_ID_EVM_TI814x == platformId) ||
              (VPS_PLATFORM_ID_SIM_TI814x == platformId) ||
              (VPS_PLATFORM_ID_EVM_TI8107 == platformId)))
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "IOCTL not supported for this platform\n");
            retVal = FVID2_EUNSUPPORTED_CMD;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Don't allow to override buffer address when requests are pending
         * with driver */
        if (0u != hObj->numUnProcessedReq)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Can't override previous field buffers when %d requests "
                "are pending!!\n", hObj->numUnProcessedReq);
            retVal = FVID2_EAGAIN;
        }
    }

    if (FVID2_SOK == retVal)
    {
        prevFldBuf = (Vps_M2mDeiOverridePrevFldBuf *) cmdArgs;
        if (prevFldBuf->numFldBufLists != VPSMDRV_DEI_MAX_FIELD_CONTEXT)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Number of field buffers should be %d\n",
                VPSMDRV_DEI_MAX_FIELD_CONTEXT);
            retVal = FVID2_EINVALID_PARAMS;
        }
        else
        {
            for (fldCnt = 0u; fldCnt < prevFldBuf->numFldBufLists; fldCnt++)
            {
                if (NULL == prevFldBuf->fldBufFrameList[fldCnt])
                {
                    retVal = FVID2_EBADARGS;
                    break;
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        numFrames = prevFldBuf->fldBufFrameList[0u]->numFrames;
        for (frmCnt = 0u; frmCnt < numFrames; frmCnt++)
        {
            frame = prevFldBuf->fldBufFrameList[0u]->frames[frmCnt];
            GT_assert(VpsMdrvDeiTrace, (NULL != frame));

            /* Get the channel number frame list's frame pointer */
            chNum = frame->channelNum;

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

            /* Store the driver held buffers locally so that it could be
             * returned back to the application in the same IOCTL after
             * copying */
            for (fldCnt = 0u; fldCnt < VPSMDRV_DEI_MAX_FIELD_CONTEXT; fldCnt++)
            {
                /* Skip current input frame (0th index) as it is stored in
                 * the last index in next step */
                prevFrame[fldCnt] = chObj->ctxFieldFifo[fldCnt + 1].fieldCtxBuf;
            }

            /* Store the current input frame at the last index so that it is
             * put in context queue while doing rotation in next queue call */
            chObj->ctxFieldFifo[VPSMDRV_DEI_MAX_FIELD_CONTEXT].fieldCtxBuf =
                chObj->ctxFieldFifo[0u].fieldCtxBuf;

            for (fldCnt = 0u; fldCnt < prevFldBuf->numFldBufLists; fldCnt++)
            {
                /* Copy the user provided frame to the channel object
                 * context FIFO. While copying, copy N-1 (0th index of
                 * fldBufFrameList represents N-1 frame) to N, N-2 (1st index)
                 * to N-1 and so on, so that when age context buffer
                 * function is called during next queue call, frames will
                 * fall in place!! */
                frame = prevFldBuf->fldBufFrameList[fldCnt]->frames[frmCnt];
                GT_assert(VpsMdrvDeiTrace, (NULL != frame));
                chObj->ctxFieldFifo[fldCnt].fieldCtxBuf = frame;

                /* Return the replaced frame to the application in the same
                 * structure */
                prevFldBuf->fldBufFrameList[fldCnt]->frames[frmCnt] =
                    prevFrame[fldCnt];
            }

            if (FALSE == chObj->isCtxFieldInit)
            {
                for (fldCnt = 0u;
                     fldCnt <= VPSMDRV_DEI_MAX_FIELD_CONTEXT;
                     fldCnt++)
                {
                    chObj->ctxFieldFifo[fldCnt].tth =
                        VPSMDRV_DEI_MAX_FIELD_CONTEXT - fldCnt;
                }
                chObj->isCtxFieldInit = TRUE;
            }
        }
    }

    return (retVal);
}



/**
 * vpsMdrvDeiClearInFrameList
 * Set the frames held by inFrameList to NULL
 */
static Int32 vpsMdrvDeiClearInFrameList(FVID2_FrameList *framesList,
                                        UInt32 numFrames)
{
    Int32   retVal = FVID2_SOK;
    UInt32  frmCnt;

    GT_assert(VpsMdrvDeiTrace, (NULL != framesList));

    for (frmCnt = 0u; frmCnt < numFrames; frmCnt++)
    {
        framesList->frames[frmCnt] = NULL;
    }

    return (retVal);
}



/**
 * vpsMdrvDeiReleaseCtxFields
 * Relenquishes all the feilds held by the driver. This function requires to be
 * called with interrupts disabled.
 */
static Int32 vpsMdrvDeiReleaseCtxFields(VpsMdrv_DeiHandleObj *hObj)
{
    Int32 retVal = FVID2_SOK;
    VpsMdrv_DeiChObj *chObj;
    FVID2_Frame *heldFields;
    UInt32 chIndex, fifoIndex, frameIndex, ctxFldIdx;
    VpsMdrv_DeiQueueObj *qObj;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;

    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    while (TRUE)
    {
        qObj = VpsUtils_dequeue(hObj->freeQ);
        if (NULL == qObj)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_DEBUG,
                "Failed to allocate queue object. Required to stop!!\n");
            retVal = FVID2_EAGAIN;
            break;
        }
        else
        {
            qObj->qObjType = VPSMDRV_QOBJ_TYPE_FRAMES;
        }
        hObj->tempFrameList.numFrames = 0u;
        frameIndex = 0u;
        for(chIndex = 0u; chIndex < hObj->numCh; chIndex++)
        {
            chObj = hObj->chObjs[chIndex];
            GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

            /* Mark the context fields as reset - would require to refill
               context in case apps decided to restart without closing the
               driver
               In case context was not initialized - nothing to release for
               this channel - move on to next channel */
            if (chObj->isCtxFieldInit == FALSE)
            {
                /* move on to next channel */
                continue;
            }
            else
            {
                chObj->isCtxFieldInit = FALSE;
            }
            /* For each channel - get the context buffers - including the
               current field */
            for (fifoIndex = 0u;
                 fifoIndex <= VPSMDRV_DEI_MAX_FIELD_CONTEXT;
                 fifoIndex++)
            {
                heldFields = chObj->ctxFieldFifo[fifoIndex].fieldCtxBuf;
                GT_assert(VpsMdrvDeiTrace, (NULL != heldFields));

                /* Ensure not to release duplicated context, for the current
                   feild, VPSMDRV_DEI_MAX_FIELD_CONTEXT would be the TTH value.
                   Duplicated context will be > VPSMDRV_DEI_MAX_FIELD_CONTEXT */
                if (chObj->ctxFieldFifo[fifoIndex].tth <=
                        VPSMDRV_DEI_MAX_FIELD_CONTEXT)
                {
                    hObj->tempFrameList.frames[frameIndex] = heldFields;
                    frameIndex++;
                    GT_assert(VpsMdrvDeiTrace,
                            (FVID2_MAX_FVID_FRAME_PTR >= frameIndex));
                }
            }
            if (retVal != FVID2_SOK)
            {
                break;
            }
            deiCoreCtxBuf = &chObj->deiCoreCtxBuf;
            GT_assert(VpsMdrvDeiTrace, (NULL != deiCoreCtxBuf));
            for (ctxFldIdx = 0u;
                 ctxFldIdx < chObj->deiCoreCtxInfo.numPrevFld;
                 ctxFldIdx++)
            {
                /* Clear out Y and CbCr buffer pointers */
                deiCoreCtxBuf->prevInBuf[ctxFldIdx][FVID2_YUV_SP_Y_ADDR_IDX]
                    = NULL;
                deiCoreCtxBuf->prevInBuf[ctxFldIdx][FVID2_YUV_SP_CBCR_ADDR_IDX]
                    = NULL;
                deiCoreCtxBuf->prevInFid[ctxFldIdx] = 0u;
            }

        }
        /* Check if we indeed have fields to be released. Otherwise, when
           release context fields is called twice, we would endup queuing
           a dummy completed request */
        /* Its OK to compare with 0, as minimum context fields required is
           > 0u and we release context for a given channel at once */
        if (frameIndex != 0u)
        {
            hObj->tempFrameList.numFrames = frameIndex;
            qObj->procList.inFrameList[0u] = &hObj->tempFrameList;
            qObj->procList.numInLists = 1u;
            /* Ensure to clear up the outFrameList - as we do not have any in this
               list */
            qObj->procList.numOutLists = 0u;
            VpsUtils_queue(hObj->doneQ, &qObj->qElem, qObj);
        }
        else
        {
            /* Put back into free Q */
            VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);
        }
        break;
    }

    return (retVal);
}
