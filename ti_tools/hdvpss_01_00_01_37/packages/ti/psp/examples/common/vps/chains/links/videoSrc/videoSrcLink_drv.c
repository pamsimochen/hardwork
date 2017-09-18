/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
 *  \file videoSrcLink_drv.c
 *
 *  \brief This file implements Video Source link as alternate
     of capture link.
 *
    This link will submit preloaded frames to the next links
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include "videoSrclink_priv.h"

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         PreProcessor Directives                            */
/* ========================================================================== */

#pragma DATA_ALIGN(gVideoSrcLink_tskStack, 32)
#pragma DATA_SECTION(gVideoSrcLink_tskStack, ".bss:taskStackSection")

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

UInt8 gVideoSrcLink_tskStack[VIDEO_SRC_LINK_OBJ_MAX][VIDEO_SRC_LINK_TSK_STACK_SIZE];

UInt8 gVideoSrcsenddata_tskStack[VIDEO_SRC_LINK_OBJ_MAX][4*KB];

VideoSrcLink_Obj gVideoSrcLink_obj[VIDEO_SRC_LINK_OBJ_MAX];



/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* return Video source link info ,
 * to the next link calling this API via system API */
System_LinkInfo *VideoSrcLink_getInfo(VpsUtils_TskHndl *pTsk)
{
    VideoSrcLink_Obj *pObj = (VideoSrcLink_Obj *)pTsk->appData;

    return &pObj->info;
}

Int32 VideoSrcLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId,
            FVID2_FrameList *pFrameList)
{
    VideoSrcLink_Obj *pObj = (VideoSrcLink_Obj *)pTsk->appData;

    GT_assert( GT_DEFAULT_MASK, queId < VIDEO_SRC_LINK_MAX_OUT_QUE);

    return VpsUtils_bufGetFull(&pObj->bufOutQue[queId], pFrameList, BIOS_NO_WAIT);
}

Int32 VideoSrcLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId,
            FVID2_FrameList *pFrameList)
{
    VideoSrcLink_Obj *pObj = (VideoSrcLink_Obj *)pTsk->appData;

    GT_assert( GT_DEFAULT_MASK, queId < VIDEO_SRC_LINK_MAX_OUT_QUE);

    return VpsUtils_bufPutEmpty(&pObj->bufOutQue[queId], pFrameList);
}

Int32 VideoSrcLink_drvProcessFrames(VideoSrcLink_Obj *pObj)
{

    Int32 status,sendCmd[VIDEO_SRC_LINK_MAX_OUT_QUE];
    UInt32 frameId,queChId,numofoutchanl;
    FVID2_Frame *pFrame = NULL;
    VideoSrcLink_OutObj *pOutObj;

    if(pObj->info.queInfo[0].chInfo[0].dataFormat == FVID2_DF_YUV422I_YUYV)
        pOutObj = &pObj->outObj[0];

    else if(pObj->info.queInfo[0].chInfo[0].dataFormat == FVID2_DF_YUV420SP_UV)
        pOutObj = &pObj->outObj[1];

    else if(pObj->info.queInfo[0].chInfo[0].dataFormat == FVID2_DF_YUV422SP_UV)
        pOutObj = &pObj->outObj[2];

    else
    {
        GT_assert( GT_DEFAULT_MASK, FALSE);
    }

    numofoutchanl=pObj->createArgs.numchannels;

    for(frameId=0; frameId<numofoutchanl; frameId++)
    {

        pFrame= NULL;
        status = VpsUtils_bufGetEmptyFrame(
        &pObj->bufOutQue[0],
        &pFrame,
        BIOS_WAIT_FOREVER);

        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        pFrame = &pOutObj->outFrames[frameId];

        pFrame->channelNum = frameId;
        queChId = VideoSrcLink_getQueChId(pFrame->channelNum,numofoutchanl);

        GT_assert( GT_DEFAULT_MASK, queChId<numofoutchanl);

        pFrame->channelNum = queChId;
        status = VpsUtils_bufPutFullFrame(&pObj->bufOutQue[0], pFrame);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    }
    sendCmd[0]=TRUE;


    if (sendCmd[0])
    {
        System_sendLinkCmd(
            pObj->createArgs.OutQueParams[0].nextLink,
            SYSTEM_CMD_NEW_DATA);
    }

    return FVID2_SOK;
}

Int32 VideoSrcLink_drvDelete(VideoSrcLink_Obj *pObj)
{
    UInt32 queId;
    Int32 status = FVID2_SOK;

    for(queId=0; queId<pObj->info.numQue ; queId++)
    {
        status =  VpsUtils_bufDelete(&pObj->bufOutQue[queId]);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    /* Delete semaphores */
    Semaphore_delete(&pObj->senddatacomplete);


#ifdef SYSTEM_DEBUG_NULL
    Vps_printf(" %d: VIDEO SRC   : Delete Done !!!\n", Clock_getTicks());
#endif

    return status;

}


Void VideoSrcLink_senddata(struct VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl *pMsg)
{


    VideoSrcLink_Obj *pObj = (VideoSrcLink_Obj*)pTsk->appData;

    while(pObj->senddata)
    {

        VpsUtils_tskSendCmd(&pObj->tsk, SYSTEM_CMD_NEW_DATA);
     Task_sleep(300);
    }
    /*Post the semaphore - end of senddata*/
    Semaphore_post(pObj->senddatacomplete);
    return;
}


Int32 VideoSrcLink_drvCreate(VideoSrcLink_Obj *pObj,
        VideoSrcLink_CreateParams *pPrm)
{

    Int32 status;
    UInt32 queId,ChId;
    VideoSrc_Input *InChInfo = NULL;
    VideoSrcLink_OutObj *pOutObj = NULL;
    UInt32 frameId,numofoutchanl,outNumFrames;
    Semaphore_Params    semPrms;


    Semaphore_Params_init(&semPrms);

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));
    GT_assert( GT_DEFAULT_MASK, pObj->createArgs.numOutQue<VIDEO_SRC_LINK_MAX_OUT_QUE);

    numofoutchanl=pObj->createArgs.numchannels;

    pObj->senddatacomplete = Semaphore_create(0, &semPrms, NULL);
    if (NULL == pObj->senddatacomplete)
    {
        Vps_printf("%s: Sem create failed\n", __FUNCTION__);
        return FVID2_EALLOC;
    }

    pObj->receviedFrameCount = 0;

    pObj->info.numQue = pPrm->numOutQue;

    pObj->senddata = TRUE;

    for(queId=0; queId<pObj->info.numQue ; queId++)
    {

        status = VpsUtils_bufCreate(&pObj->bufOutQue[queId], TRUE, FALSE);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        pObj->info.queInfo[queId].numCh = pPrm->numchannels;
    }


    for(queId=0;queId<pObj->info.numQue;queId++)
    {

        for(ChId=0;ChId<pObj->info.queInfo[queId].numCh;ChId++)
        {
            InChInfo = &pPrm->InputInfo[ChId];

            pObj->info.queInfo[queId].chInfo[ChId].pitch[0] =InChInfo->pitch[0];
            pObj->info.queInfo[queId].chInfo[ChId].pitch[1] =InChInfo->pitch[1];
            pObj->info.queInfo[queId].chInfo[ChId].pitch[2] =InChInfo->pitch[2];
            pObj->info.queInfo[queId].chInfo[ChId].height   = InChInfo->height;
            pObj->info.queInfo[queId].chInfo[ChId].width    = InChInfo->width ;
            pObj->info.queInfo[queId].chInfo[ChId].scanFormat
                = InChInfo->scanFormat;
            pObj->info.queInfo[queId].chInfo[ChId].memType = InChInfo->memType;
            pObj->info.queInfo[queId].chInfo[ChId].dataFormat
                = InChInfo->dataFormat;

        }
    }

    GT_assert(GT_DEFAULT_MASK, InChInfo != NULL);
    if (InChInfo->dataFormat == FVID2_DF_YUV422I_YUYV)
    {
        pOutObj = &pObj->outObj[0];
    }
    else if (InChInfo->dataFormat == FVID2_DF_YUV420SP_UV)
    {
        pOutObj = &pObj->outObj[1];
    }
    else if (InChInfo->dataFormat == FVID2_DF_YUV422SP_UV)
    {
        pOutObj = &pObj->outObj[2];
    }
    else
    {
        GT_assert(GT_DEFAULT_MASK, FALSE);
    }

    outNumFrames = numofoutchanl * VIDEO_SRC_LINK_MAX_OUT_FRAMES_PER_CH;

    for(frameId=0; frameId<outNumFrames; frameId++)
    {
        status = VpsUtils_bufPutEmptyFrame(&pObj->bufOutQue[0],
                    &pOutObj->outFrames[frameId]);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }


    #ifdef SYSTEM_DEBUG_NULL
    Vps_printf(" %d: VIDEO SRC   : Create Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}

Int32 videoSrcLink_CreateSendTsk(VideoSrcLink_Obj *pObj)
{

    char tskName[32];

    Int32 status;

    sprintf(tskName, "VideosrcSend");

    status = VpsUtils_tskCreate(
            &pObj->tsksend,
            VideoSrcLink_senddata,
            VIDEO_SRC_LINK_TSK_PRI,
            gVideoSrcsenddata_tskStack[0],
            (4 * KB),
            pObj,
            tskName
            );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    VpsUtils_tskSendCmd(&pObj->tsksend, SYSTEM_CMD_START);

    return FVID2_SOK;

}

Int32 videoSrcLink_DeleteSendTsk(VideoSrcLink_Obj *pObj)
{
    VpsUtils_tskDelete(&pObj->tsksend);

    return FVID2_SOK;
}

Void VideoSrcLink_tskMain(struct VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl *pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    VideoSrcLink_Obj *pObj = (VideoSrcLink_Obj*)pTsk->appData;

    if(cmd!=SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = VideoSrcLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));

    VpsUtils_tskAckOrFreeMsg(pMsg, status);

    if(status!=FVID2_SOK)
        return;

    done    = FALSE;
    ackMsg  = FALSE;

    while(!done)
    {

        #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
        Vps_printf(" %d: waiting for next message Video src!!!\n"
             , Clock_getTicks());
        #endif

        status = VpsUtils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if(status!=FVID2_SOK)
        break;

        cmd = VpsUtils_msgGetCmd(pMsg);

        switch(cmd)
        {

            case SYSTEM_CMD_DELETE:

                done = TRUE;
                ackMsg = TRUE;

                #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
                Vps_printf(" %d: Got SYSTEM_CMD_DELETE for Video src!!!\n"
                    , Clock_getTicks());
                #endif

                status = VideoSrcLink_drvDelete(pObj);
                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                break;

            case SYSTEM_CMD_STOP:

                #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
                Vps_printf(" %d: Got SYSTEM_CMD_STOP for Video src!!!\n"
                , Clock_getTicks());
                #endif

                pObj->senddata = FALSE;
                Semaphore_pend(pObj->senddatacomplete, BIOS_WAIT_FOREVER);
                videoSrcLink_DeleteSendTsk(pObj);
                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                break;

            case SYSTEM_CMD_START:

                #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
                Vps_printf(" %d: Got SYSTEM_CMD_START for Video src!!!\n"
                    , Clock_getTicks());
                #endif
                pObj->senddata = TRUE;
                videoSrcLink_CreateSendTsk(pObj);
                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                break;

            case SYSTEM_CMD_NEW_DATA:

                #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
                Vps_printf(" %d: Got SYSTEM_CMD_NEW_DATA for Video src!!!\n"
                    , Clock_getTicks());
                #endif

                VideoSrcLink_drvProcessFrames(pObj);

                #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
                Vps_printf(" %d: After processing frames Video src!!!\n"
                    , Clock_getTicks());
                #endif

                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                break;

            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
    Vps_printf(" %d: Video SRC: Delete Done !!!\n", Clock_getTicks());
    #endif

    if(ackMsg && pMsg!=NULL)
       // VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

void VideoSrcLink_LoadFrames(VideoSrcLink_Obj  *pObj)
{

    FVID2_Format *pFormat;
    UInt32 maxOutWidth, maxOutHeight,i;
    VideoSrcLink_OutObj *pOutObj;
    Int32 status;
    char ch;


    maxOutWidth = 1920u;
    maxOutHeight = 1080u;

    pOutObj = &pObj->outObj[0];
    pFormat = &pOutObj->outFormat;

    pFormat->channelNum     = 0;
    pFormat->width          = maxOutWidth;
    pFormat->height         = maxOutHeight;
    pFormat->fieldMerged[0] = FALSE;
    pFormat->fieldMerged[1] = FALSE;
    pFormat->fieldMerged[2] = FALSE;

    pOutObj->outNumFrames = 4;

    pFormat->pitch[0]       = VpsUtils_align(pFormat->width,
         VPS_BUFFER_ALIGNMENT);


    pFormat->dataFormat     = FVID2_DF_YUV422I_YUYV;
    pFormat->pitch[0]      *= 2;
    pFormat->pitch[1]       = pFormat->pitch[0];
    pFormat->pitch[2]       = 0;
    pFormat->scanFormat     = FVID2_SF_PROGRESSIVE;
    pFormat->bpp            = FVID2_BPP_BITS16;
    pFormat->reserved       = NULL;


     status = VpsUtils_memFrameAlloc(
                    &pOutObj->outFormat,
                    &pOutObj->outFrames[0],
                    pOutObj->outNumFrames
                    );
     GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

     /*Repeat the first 4 buffers */
     /*48 is the max number of channels 16 * usual depth for each channel (6)/2*/
     for(i=4;i<48;i++)
     {
            memcpy(&pOutObj->outFrames[i],&pOutObj->outFrames[i %4],
                sizeof(pOutObj->outFrames[0]));
     }

     Vps_printf(" %d: Video src :-Load %d frames at location 0x%p!!!
        of YUV422I Format\n",Clock_getTicks(),pOutObj->outNumFrames,pOutObj->outFrames[0]);
     scanf("%c", &ch);

     pOutObj = &pObj->outObj[1];
     pFormat = &pOutObj->outFormat;

     pFormat->channelNum     = 0;
     pFormat->width          = maxOutWidth;
     pFormat->height         = maxOutHeight;
     pFormat->fieldMerged[0] = FALSE;
     pFormat->fieldMerged[1] = FALSE;
     pFormat->fieldMerged[2] = FALSE;

     pOutObj->outNumFrames = 4;

     pFormat->pitch[0]       = VpsUtils_align(pFormat->width,
            VPS_BUFFER_ALIGNMENT);

     pFormat->dataFormat     = FVID2_DF_YUV420SP_UV;
     pFormat->pitch[1]       = pFormat->pitch[0];
     pFormat->pitch[2]       = 0;
     pFormat->scanFormat     = FVID2_SF_PROGRESSIVE;
     pFormat->bpp            = FVID2_BPP_BITS12;
     pFormat->reserved       = NULL;

     status = VpsUtils_memFrameAlloc(
         &pOutObj->outFormat,
         &pOutObj->outFrames[0],
         pOutObj->outNumFrames
     );
     GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

     /*Repeat the first 4 buffers */
     for(i=4;i<48;i++)
     {
            memcpy(&pOutObj->outFrames[i],&pOutObj->outFrames[i %4],
                sizeof(pOutObj->outFrames[0]));
     }

    Vps_printf(" %d: Video src :-Load %d frames at location 0x%p!!! of YUV420SP \n",
        Clock_getTicks(),pOutObj->outNumFrames,pOutObj->outFrames[0]);
    scanf("%c", &ch);


     pOutObj = &pObj->outObj[2];
     pFormat = &pOutObj->outFormat;

     pFormat->channelNum     = 0;
     pFormat->width          = maxOutWidth;
     pFormat->height         = maxOutHeight;
     pFormat->fieldMerged[0] = FALSE;
     pFormat->fieldMerged[1] = FALSE;
     pFormat->fieldMerged[2] = FALSE;

     pOutObj->outNumFrames   = 4;

     pFormat->pitch[0]       = VpsUtils_align(pFormat->width,
            VPS_BUFFER_ALIGNMENT);

     pFormat->dataFormat     = FVID2_DF_YUV422SP_UV;
     pFormat->pitch[1]       = pFormat->pitch[0];
     pFormat->pitch[2]       = 0;
     pFormat->scanFormat     = FVID2_SF_PROGRESSIVE;
     pFormat->bpp            = FVID2_BPP_BITS16;
     pFormat->reserved       = NULL;

     status = VpsUtils_memFrameAlloc(
         &pOutObj->outFormat,
         &pOutObj->outFrames[0],
         pOutObj->outNumFrames
     );
     GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

     /*Repeat the first 4 buffers */
     for(i=4;i<48;i++)
     {
            memcpy(&pOutObj->outFrames[i],&pOutObj->outFrames[i %4],
                sizeof(pOutObj->outFrames[0]));
     }

     Vps_printf(" %d: Video src :-Load %d frames at location 0x%p!!! of YUV422SP format\n",
        Clock_getTicks(),pOutObj->outNumFrames,pOutObj->outFrames[0]);
     scanf("%c", &ch);

     #ifdef SYSTEM_DEBUG_VIDEOSRC_RT
        Vps_printf("processing ");
     #endif

}


Int32 VideoSrcLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    UInt32 VideoSrcId;
    VideoSrcLink_Obj  *pObj;
    char tskName[32];


    for(VideoSrcId=0; VideoSrcId<VIDEO_SRC_LINK_OBJ_MAX; VideoSrcId++)
    {

        pObj = &gVideoSrcLink_obj[VideoSrcId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->tskId = SYSTEM_LINK_ID_VIDEO_SRC_0+VideoSrcId;

        VideoSrcLink_LoadFrames(pObj);

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames  = VideoSrcLink_getFullFrames;
        linkObj.linkPutEmptyFrames = VideoSrcLink_putEmptyFrames;
        linkObj.getLinkInfo = VideoSrcLink_getInfo;

        System_registerLink(pObj->tskId, &linkObj);

        sprintf(tskName, "VideosrcId%d", VideoSrcId);

        status = VpsUtils_tskCreate(
                    &pObj->tsk,
                    VideoSrcLink_tskMain,
                    VIDEO_SRC_LINK_TSK_PRI,
                    gVideoSrcLink_tskStack[VideoSrcId],
                    (VIDEO_SRC_LINK_TSK_STACK_SIZE),
                    pObj,
                    tskName
                    );
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    return status;
}

Int32 VideoSrcLink_deInit()
{
    UInt32 VideoSrcId;

    for(VideoSrcId=0; VideoSrcId<VIDEO_SRC_LINK_OBJ_MAX; VideoSrcId++)
    {
        VpsUtils_tskDelete(&gVideoSrcLink_obj[VideoSrcId].tsk);
    }
    return FVID2_SOK;
}

