/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*
  This file implements the FVID2 interface top level APIs.

  See ti/psp/vps/driver/m2mNsf/vpsdrv_m2mNsf.h for function documentation
*/

#include <ti/psp/vps/drivers/m2mNsf/src/vpsdrv_m2mNsfPriv.h>

/*
 * NSF Common glabal object defined in NsfCommon.c
 */
extern  Vps_NsfCommonObj gVps_nsfCommonObj;

Fdrv_Handle Vps_nsfCreate ( UInt32 drvId,
                            UInt32 instanceId,
                            Vps_NsfCreateParams * createArgs,
                            Vps_NsfCreateStatus * createStatus,
                            const FVID2_CbParams * cbParams )
{
    Vps_NsfObj *pObj;
    Vps_NsfChObj *chObj;
    UInt32 chId;
    Int32 status = FVID2_SOK;
    Mlm_ClientInfo mlmClientInfo;

    if ( createStatus == NULL )
        return NULL;

    /*
     * parameter check
     */
    if ( instanceId >= VPS_NSF_INST_ID_MAX
         || createArgs == NULL
         || cbParams == NULL
         || ( drvId != FVID2_VPS_M2M_NSF_DRV )
         || createArgs->numCh > VPS_NSF_MAX_CH_PER_HANDLE
         || createArgs->numCh == 0
       )
    {
        createStatus->retVal = FVID2_EBADARGS;
        return NULL;
    }

    /*
     * alloc driver object
     */
    pObj = Vps_nsfAllocObj (  );
    if ( pObj == NULL )
    {
        createStatus->retVal = FVID2_EALLOC;
        return NULL;    /* Error in allocation - return NULL */
    }

    /*
     * copy user arguments to driver
     */
    memcpy ( &pObj->createArgs, createArgs, sizeof ( pObj->createArgs ) );

    pObj->createArgs.dataFormat = &pObj->dataFormat[0];
    pObj->createArgs.processingCfg = &pObj->processingCfg[0];

    memcpy ( &pObj->createArgs.dataFormat[0],
             &createArgs->dataFormat[0],
             sizeof ( Vps_NsfDataFormat ) * createArgs->numCh );

    memcpy ( &pObj->createArgs.processingCfg[0],
             &createArgs->processingCfg[0],
             sizeof ( Vps_NsfProcessingCfg ) * createArgs->numCh );

    memcpy ( &pObj->cbParams, cbParams, sizeof ( pObj->cbParams ) );

    /*
     * init status structure
     */
    pObj->createStatus.maxFramesInProcessFrames = VPS_NSF_MAX_REQ_PER_LIST;
    pObj->createStatus.maxFramesPerChInQueue = VPS_NSF_MAX_DESC_SET_PER_CH;
    pObj->createStatus.maxReqInQueue = VPS_NSF_MAX_REQ_LIST;

    /*
     * copy status structure to user structure
     */
    memcpy ( createStatus, &pObj->createStatus, sizeof ( pObj->createStatus ) );

    /*
     * create channel specific objects
     */
    for ( chId = 0; chId < pObj->createArgs.numCh; chId++ )
    {

        /*
        * get channel specific object
        */
        chObj = &pObj->chObj[chId];

        chObj->subFrameChPrms.subFrameMdEnable =
            pObj->processingCfg[chId].subFrameModeEnable;

        chObj->subFrameChPrms.slcSz =
            pObj->processingCfg[chId].numLinesPerSubFrame;


        status = Vps_nsfChannelCreate ( pObj, chId );
        if ( status != FVID2_SOK )
            break;
    }

    if ( status == FVID2_SOK )
    {
        /*
         * create request processing related object's
         */
        status = Vps_nsfReqCreate ( pObj );
    }

    if ( status != FVID2_SOK )
    {
        /*
         * Error - release previously allocated resources
         */
        Vps_nsfDelete ( pObj, NULL );
        pObj = NULL;
    }

    createStatus->retVal = status;

    if ( status == FVID2_SOK )
    {
        /*
        * Take global lock to avoid race condition
        */
        Vps_nsfLock (  );

        gVps_nsfCommonObj.openCnt++;

        if ( gVps_nsfCommonObj.openCnt == 1 )
        {

            /*Opened for the first time , register with mlm*/
            /*
            * Open MLM handle, allocates HW list internally
            */
            mlmClientInfo.reqCompleteCb = Vps_nsfReqComplete;
            mlmClientInfo.isChannelStatic = TRUE;
            mlmClientInfo.channelNum[0] = VPSHAL_VPDMA_CHANNEL_NF_WR_LUMA;
            mlmClientInfo.channelNum[1] = VPSHAL_VPDMA_CHANNEL_NF_WR_CHROMA;
            mlmClientInfo.channelNum[2] = VPSHAL_VPDMA_CHANNEL_NF_PREV_LUMA;
            mlmClientInfo.channelNum[3] = VPSHAL_VPDMA_CHANNEL_NF_PREV_CHROMA;
            mlmClientInfo.channelNum[4] = VPSHAL_VPDMA_CHANNEL_NF_RD;
            mlmClientInfo.numChannels = 5;

            gVps_nsfCommonObj.mlmHandle = Mlm_register ( &mlmClientInfo, NULL );

            if ( gVps_nsfCommonObj.mlmHandle == NULL )
            {
                /*
                * Error - free acquired resources
                */
                status = FVID2_EALLOC;
                Vps_nsfDelete ( pObj, NULL );
                pObj = NULL;

            }
        }
        /*
        * Release global lock
        */
        Vps_nsfUnlock (  );
    }


    /*
     * success return pointer to object
     */
    return pObj;
}

Int32 Vps_nsfDelete ( Fdrv_Handle handle, Ptr reserved )
{
    Vps_NsfObj *pObj = ( Vps_NsfObj * ) handle;
    UInt32 chId;
    UInt32 numPendSubmission;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );

    numPendSubmission = pObj->numPendSubmission;

    if(numPendSubmission!=0)
    {
        /* pending submission's present so cannot delete at this point
           user should do "getProcessFrames" to remove pending submission
            and then call delete
        */
        return FVID2_EAGAIN;
    }

    /*
     * delete channel specific resources
     */
    for ( chId = 0; chId < pObj->createArgs.numCh; chId++ )
    {
        Vps_nsfChannelDelete ( pObj, chId );
    }

    /*
     * delete request processing related resources
     */
    Vps_nsfReqDelete ( pObj );

    /*
     * free driver object
     */
    Vps_nsfFreeObj ( pObj );


     /*
     * Take global lock to avoid race condition
     */
     Vps_nsfLock (  );

     gVps_nsfCommonObj.openCnt--;

     if ( 0 == gVps_nsfCommonObj.openCnt )
     {
        /*
        * All handles are deleted, unregister with mlm
        * Release MLM handle, releases HW list internally
        */
        Mlm_unRegister ( gVps_nsfCommonObj.mlmHandle );
     }

     /*
      * Release global lock
      */
     Vps_nsfUnlock (  );


    return 0;
}

Int32 Vps_nsfProcessFrames ( Fdrv_Handle handle,
                             FVID2_ProcessList * processList )
{
    Vps_NsfObj *pObj = ( Vps_NsfObj * ) handle;
    Vps_NsfReqList *reqList;
    Int32 status = FVID2_SOK;
    UInt32 reqId;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  processList != NULL );

    /*
     * parameter check
     */
    if ( processList->numInLists != VPS_NSF_IN_FRAME_LIST_NUM
         || processList->numOutLists != VPS_NSF_OUT_FRAME_LIST_NUM )
    {
        return FVID2_EBADARGS;
    }

    /*
     * get a free request list
     */
    reqList = Vps_nsfReqGetFreeList ( pObj, processList );
    if ( reqList == NULL )
        return FVID2_EFAIL;

    /*
     * driver lock
     */
    Vps_nsfObjLock ( pObj );

    /*
     * for each request  ...
     */
    for ( reqId = 0; reqId < reqList->numReq; reqId++ )
    {
        /*
         * make channel descriptor set for this request
         */
        status = Vps_nsfChannelMakeDescSet ( pObj,
                                             &reqList->reqObj[reqId],
                                             &reqList->processList, reqId );
        if ( status != FVID2_SOK )
            break;  /* Error - break from loop */
    }

    /*
     * driver unlock
     */
    Vps_nsfObjUnlock ( pObj );

    if ( status == FVID2_SOK )
    {
        /*
         * sunmit request
         */
        status = Vps_nsfReqSubmit ( pObj, reqList );
    }

    if ( status != FVID2_SOK )
    {
        /*
         * Error - abort request
         */
        Vps_nsfReqAbort ( pObj, reqList, reqId );
    }

    return status;
}

Int32 Vps_nsfGetProcessedFrames ( Fdrv_Handle handle,
                                  FVID2_ProcessList * processList,
                                  UInt32 timeout )
{
    Vps_NsfObj *pObj = ( Vps_NsfObj * ) handle;
    Int32 status;

    /*
     * get ready/completed request list
     * and return the frames to user process list
     */
    status = Vps_nsfReqGetProcessedFrames ( pObj, processList, timeout );

    return status;
}

Int32 Vps_nsfControl ( Fdrv_Handle handle,
                       UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Int32 status = FVID2_EBADARGS;

    switch ( cmd )
    {
        case IOCTL_VPS_NSF_SET_DATA_FORMAT:
            /*
             * set channel data format
             */
            status = Vps_nsfSetDataFormat ( handle, cmdArgs );
            break;
        case IOCTL_VPS_NSF_SET_PROCESSING_CFG:
            /*
             * set channel processing params
             */
            status = Vps_nsfSetProcessingCfg ( handle, cmdArgs );
            break;
        case IOCTL_VPS_READ_ADV_NSF_CFG:
            /*
             * Get Advanced Config params
             */
            status = Vps_nsfGetAdvCfg( handle, cmdArgs );
            break;
    }

    return status;
}

Int32 Vps_nsfSetDataFormat ( Fdrv_Handle handle, Vps_NsfDataFormat * format )
{
    Int32 status;
    Vps_NsfObj *pObj = ( Vps_NsfObj * ) handle;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  format != NULL );

    /*
     * driver lock
     */
    Vps_nsfObjLock ( pObj );

    /*
     * set channel format
     */
    status = Vps_nsfChannelSetFormat ( pObj, format );

    /*
     * driver unlock
     */
    Vps_nsfObjUnlock ( pObj );

    return status;
}

Int32 Vps_nsfSetProcessingCfg ( Fdrv_Handle handle,
                                Vps_NsfProcessingCfg * params )
{
    Int32 status;
    Vps_NsfObj *pObj = ( Vps_NsfObj * ) handle;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  params != NULL );

    /*
     * driver lock
     */
    Vps_nsfObjLock ( pObj );

    /*
     * set channel processing params
     */
    status = Vps_nsfChannelSetProcessingCfg ( pObj, params );

    /*
     * driver unlock
     */
    Vps_nsfObjUnlock ( pObj );

    return status;
}

Int32 Vps_nsfGetAdvCfg ( Fdrv_Handle handle,
                                Vps_NsfAdvConfig * params )
{
    Int32 status;
    Vps_NsfObj *pObj = ( Vps_NsfObj * ) handle;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  params != NULL );

    /*
    * driver lock
    */
    Vps_nsfObjLock ( pObj );

    /*
    * Get Advance channel params
    */
    status = Vps_nsfChannelGetAdvanceCfg ( pObj, params );

    /*
    * driver unlock
    */
    Vps_nsfObjUnlock ( pObj );

    return status;

}
