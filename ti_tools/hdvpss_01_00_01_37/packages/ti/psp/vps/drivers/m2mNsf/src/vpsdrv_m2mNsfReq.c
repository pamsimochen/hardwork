/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/drivers/m2mNsf/src/vpsdrv_m2mNsfPriv.h>

/*
  Create and init request list related data structures
*/
Int32 Vps_nsfReqCreate ( Vps_NsfObj * pObj )
{
    Int32 status;
    UInt32 queFlags, reqListId;

    pObj->numPendSubmission = 0;

    /*
     * check how to block on free request list not available
     */

    /*
     * non-blocking
     */
    queFlags = VPSUTILS_QUE_FLAG_NO_BLOCK_QUE;
    pObj->reqListFreeTimeout = BIOS_NO_WAIT;

    if ( TRUE == pObj->blockOnReqQueFull )
    {
        /*
         * blocking wait required by user
         */
        queFlags = VPSUTILS_QUE_FLAG_BLOCK_QUE_GET;
        pObj->reqListFreeTimeout = BIOS_WAIT_FOREVER;
    }

    /*
     * create the free que
     */
    status = VpsUtils_queCreate ( &pObj->reqListFreeQue,
                                  VPS_NSF_MAX_REQ_LIST,
                                  pObj->reqListFreeQueMem, queFlags );

    if ( status == FVID2_SOK )
    {
        /*
         * add request objects to free que
         */
        for ( reqListId = 0; reqListId < VPS_NSF_MAX_REQ_LIST; reqListId++ )
        {
            status = VpsUtils_quePut ( &pObj->reqListFreeQue,
                                       &pObj->reqList[reqListId],
                                       BIOS_NO_WAIT );
        }

        /*
         * create ready que
         */
        status = VpsUtils_queCreate ( &pObj->reqListReadyQue,
                                      VPS_NSF_MAX_REQ_LIST,
                                      pObj->reqListReadyQueMem,
                                      VPSUTILS_QUE_FLAG_BLOCK_QUE_GET );

        if ( status != FVID2_SOK )
        {
            /*
             * error - delete previously create free que
             */
            status = VpsUtils_queDelete ( &pObj->reqListFreeQue );

            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
        }
    }

    if ( status == FVID2_SOK )
    {
        /*
         * success - mark request related data structure's are created
         */
        pObj->reqIsCreate = TRUE;
    }

    return status;
}

/*
  Free request related data structure's
*/
Int32 Vps_nsfReqDelete ( Vps_NsfObj * pObj )
{
    Int32 status = FVID2_SOK;

    if ( TRUE == pObj->reqIsCreate )
    {
        /*
         * delete free que
         */
        status = VpsUtils_queDelete ( &pObj->reqListFreeQue );

        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

        /*
         * delete ready que
         */
        status = VpsUtils_queDelete ( &pObj->reqListReadyQue );

        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

        /*
         * request list's are no longer in create state
         */
        pObj->reqIsCreate = FALSE;
    }

    return status;
}

/*
  Get free request list

  pObj - NSF driver object
  processList - user supplied process list
*/
Vps_NsfReqList *Vps_nsfReqGetFreeList ( Vps_NsfObj * pObj,
                                        FVID2_ProcessList * processList )
{
    Int32 status, numReq;
    Vps_NsfReqList *reqList;

    /*
     * extract number of requests in this process list
     */
    numReq = processList->inFrameList[VPS_NSF_IN_FRAME_LIST_IDX]->numFrames;

    /*
     * check if numReq is within limits
     */
    if ( numReq <= 0 || numReq > VPS_NSF_MAX_REQ_PER_LIST )
        return NULL;

    /*
     * Get a free request list, optionally block based on earlier user supplied arguments
     */
    status = VpsUtils_queGet ( &pObj->reqListFreeQue,
                               ( Ptr * ) & reqList,
                               1, pObj->reqListFreeTimeout );

    if ( status != 0 )
        return NULL;    /* free request list not found */

    /*
     * init numReq in request list
     */
    reqList->numReq = numReq;

    /*
     * copy user supplied process list to request list
     */
    FVID2_copyProcessList ( &reqList->processList, processList );

    /*
     * set pointer to parent NSF driver object
     */
    reqList->pNsfObj = pObj;

    /*
     * return the initialized request list
     */
    return reqList;
}

/*
  Abort request list

  reqList - request list
  validReqNum - number of valid request in this list which need to be aborted
*/
Int32 Vps_nsfReqAbort ( Vps_NsfObj * pObj,
                        Vps_NsfReqList * reqList, UInt16 validReqNum )
{
    Int32 reqId, status;

    /*
     * fro all valid request's
     */
    for ( reqId = 0; reqId < validReqNum; reqId++ )
    {
        /*
         * free descriptor set
         */
        Vps_nsfChannelFreeDescSet ( pObj, &reqList->reqObj[reqId] );
    }

    /*
     * free request list - add to free que
     */
    status = VpsUtils_quePut ( &pObj->reqListFreeQue, reqList, BIOS_NO_WAIT );
    GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

    return status;
}

/*
  Submit request list to HW for processing

  reqList - request list
*/
Int32 Vps_nsfReqSubmit ( Vps_NsfObj * pObj, Vps_NsfReqList * reqList )
{
    Int32 status;
    UInt32 i;
    Vps_NsfReq *curReq, *prevReq;
    UInt32 cookie;

    GT_assert( GT_DEFAULT_MASK,  reqList->numReq > 0 );

    /*
     * link descriptor set's of request's in request list
     */
    curReq = prevReq = &reqList->reqObj[0];

    for ( i = 1; i < reqList->numReq; i++ )
    {
        curReq = &reqList->reqObj[i];

        Vps_nsfChannelLinkDescSet ( prevReq, curReq );

        prevReq = curReq;
    }

    /*
     * link last request to NULL
     */
    Vps_nsfChannelLinkDescSet ( curReq, NULL );

    /*
     * create MLM request
     */

    /*
     * descriptor start address
     */
    reqList->mlmReq.reqStartAddr = reqList->reqObj[0].descSetMem;

    /*
     * first descriptor set size
     */
    reqList->mlmReq.firstChannelSize = VPS_NSF_CONFIG_DESC_OFFSET;

    /*
     * last descriptor set reload address
     */
    reqList->mlmReq.lastRlDescAddr =
        Vps_nsfChannelGetDescSetReloadAddr ( curReq );

    /*
     * MLM request type
     */

#if(VPS_NSF_NUM_SOCH_DESC>0)
    /*
     * MLM will not add SOC, NSF driver takes care of adding SOC
     */
    reqList->mlmReq.submitReqType = MLM_SRT_NO_SOC;
#else
    /*
     * MLM will add SOC, NSF driver need not take care of adding SOC
     */
    reqList->mlmReq.submitReqType = MLM_SRT_USE_SOC;
#endif

    /*
     * MLM callback argument is driver 'reqList'
     */
    reqList->mlmReq.cbArg = reqList;

    /*
     * SOCH VPDMA descriptor to use
     */
    reqList->mlmReq.channelNum[0] = VPSHAL_VPDMA_CHANNEL_NF_WR_LUMA;
    reqList->mlmReq.channelNum[1] = VPSHAL_VPDMA_CHANNEL_NF_WR_CHROMA;
    reqList->mlmReq.channelNum[2] = VPSHAL_VPDMA_CHANNEL_NF_PREV_LUMA;
    reqList->mlmReq.channelNum[3] = VPSHAL_VPDMA_CHANNEL_NF_PREV_CHROMA;
    reqList->mlmReq.channelNum[4] = VPSHAL_VPDMA_CHANNEL_NF_RD;
    reqList->mlmReq.numChannels = 5;

    cookie = Hwi_disable();
    pObj->numPendSubmission++;
    Hwi_restore(cookie);

    /*
     * Submit request list to MLM
     */
    status = Mlm_submit ( gVps_nsfCommonObj.mlmHandle, &reqList->mlmReq );

    return status;
}

/*
  Request list complete callback from MLM

  mlmReq - original submitted MLM request
*/
Int32 Vps_nsfReqComplete ( Mlm_SubmitReqInfo * mlmReq )
{
    Vps_NsfObj *pObj;
    Vps_NsfReqList *reqList;
    Vps_NsfReq *req;
    Int32 status;
    UInt32 reqId;

    /*
     * extract driver request list
     */
    reqList = ( Vps_NsfReqList * ) mlmReq->cbArg;
    GT_assert( GT_DEFAULT_MASK,  reqList != NULL );

    /*
     * extract driver object
     */
    pObj = reqList->pNsfObj;

    /*
     * fatal error condition checks - should not happen in normal case
     */
    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  reqList->numReq > 0 );
    GT_assert( GT_DEFAULT_MASK,  reqList->numReq <= VPS_NSF_MAX_REQ_PER_LIST );

    /*
     * for each request in request list ...
     */
    for ( reqId = 0; reqId < reqList->numReq; reqId++ )
    {
        req = &reqList->reqObj[reqId];

        /*
         * free descriptor set associated with this request
         */
        Vps_nsfChannelFreeDescSet ( pObj, req );
    }

    /*
     * add request list to ready/completed request list que
     */
    status = VpsUtils_quePut ( &pObj->reqListReadyQue, reqList, BIOS_NO_WAIT );

    GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

    /*
     * call user supplied user callback
     */
    if ( pObj->cbParams.fdmCbFxn )
        pObj->cbParams.fdmCbFxn ( pObj->cbParams.fdmData, pObj );

    return status;
}

/*
  Get processed frames

  processList - user supplied process list
  timeout - controls blocking or non-blocking
*/
Int32 Vps_nsfReqGetProcessedFrames ( Vps_NsfObj * pObj,
                                     FVID2_ProcessList * processList,
                                     UInt32 timeout )
{
    Vps_NsfReqList *reqList;
    Int32 status;
    UInt32 cookie, numPendSubmission;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  processList != NULL );

    /*
     * get completed request list from ready/complete que
     */
    status =
        VpsUtils_queGet ( &pObj->reqListReadyQue, ( Ptr * ) & reqList, 1,
                          timeout );

    if ( status != FVID2_SOK )
    {
        numPendSubmission = pObj->numPendSubmission;

        if(numPendSubmission==0)
        {
            /* NO pending submission's present so return error
                so that user need not check again
            */
            status = FVID2_ENO_MORE_BUFFERS;
        }
        else
        {
            /* pending submission's present so return error
                so that user can check again later
            */
            status = FVID2_EAGAIN;
        }


        return status;  /* no completed request list found in que */
    }

    cookie = Hwi_disable();
    if(pObj->numPendSubmission)
        pObj->numPendSubmission--;
    Hwi_restore(cookie);

    GT_assert( GT_DEFAULT_MASK,  reqList->numReq > 0 );
    GT_assert( GT_DEFAULT_MASK,  reqList->numReq <= VPS_NSF_MAX_REQ_PER_LIST );

    /*
     * request list found, copy associated process list to user process list
     */
    FVID2_copyProcessList ( processList, &reqList->processList );

    /*
     * return request list to free request list queue
     */
    status = VpsUtils_quePut ( &pObj->reqListFreeQue, reqList, BIOS_NO_WAIT );

    GT_assert( GT_DEFAULT_MASK,  status == 0 );

    return status;
}
