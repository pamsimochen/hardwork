/** ==================================================================
 *  @file   dupLink_tsk.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/dup/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "dupLink_priv.h"

#pragma DATA_ALIGN(gDupLink_tskStack, 32)
#pragma DATA_SECTION(gDupLink_tskStack, ".bss:taskStackSection")
UInt8 gDupLink_tskStack[DUP_LINK_OBJ_MAX][DUP_LINK_TSK_STACK_SIZE];

DupLink_Obj gDupLink_obj[DUP_LINK_OBJ_MAX];

/* ===================================================================
 *  @func     DupLink_drvCreate                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DupLink_drvCreate(DupLink_Obj * pObj, DupLink_CreateParams * pPrm)
{
    UInt32 outId, frameId;

    Int32 status;

    FVID2_Frame *pFrame;

    System_FrameInfo *pFrameInfo;

    Semaphore_Params semParams;

    memcpy(&pObj->createArgs, pPrm, sizeof(pObj->createArgs));
    GT_assert(GT_DEFAULT_MASK,
              pObj->createArgs.numOutQue < DUP_LINK_MAX_OUT_QUE);

    pObj->putFrameCount = 0;
    pObj->getFrameCount = 0;

    memset(pObj->frames, 0, sizeof(pObj->frames));
    memset(pObj->frameInfo, 0, sizeof(pObj->frameInfo));

    pObj->pInTskInfo = System_getLinkInfo(pPrm->inQueParams.prevLinkId);
    GT_assert(GT_DEFAULT_MASK, pObj->pInTskInfo != NULL);
    GT_assert(GT_DEFAULT_MASK,
              pPrm->inQueParams.prevLinkQueId < pObj->pInTskInfo->numQue);

    pObj->info.numQue = pObj->createArgs.numOutQue;

    memcpy(&pObj->info.queInfo[0],
           &pObj->pInTskInfo->queInfo[pPrm->inQueParams.prevLinkQueId],
           sizeof(pObj->pInTskInfo->queInfo[0]));

    for (outId = 1; outId < pObj->info.numQue; outId++)
    {
        memcpy(&pObj->info.queInfo[outId],
               &pObj->info.queInfo[0], sizeof(pObj->info.queInfo[0]));
    }

    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;

    pObj->lock = Semaphore_create(1u, &semParams, NULL);

    for (outId = 0; outId < DUP_LINK_MAX_OUT_QUE; outId++)
    {
        status = VpsUtils_bufCreate(&pObj->outFrameQue[outId], FALSE, FALSE);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

        for (frameId = 0; frameId < DUP_LINK_MAX_FRAMES_PER_OUT_QUE; frameId++)
        {
            pFrame =
                &pObj->frames[DUP_LINK_MAX_FRAMES_PER_OUT_QUE * outId +
                              frameId];
            pFrameInfo =
                &pObj->frameInfo[DUP_LINK_MAX_FRAMES_PER_OUT_QUE * outId +
                                 frameId];

            pFrame->appData = pFrameInfo;

            status =
                VpsUtils_bufPutEmptyFrame(&pObj->outFrameQue[outId], pFrame);
            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
        }
    }

#ifdef SYSTEM_DEBUG_DUP
    Vps_printf(" %d: DUP   : Create Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DupLink_drvDelete                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DupLink_drvDelete(DupLink_Obj * pObj)
{
    UInt32 outId;

    Int32 status;

    for (outId = 0; outId < DUP_LINK_MAX_OUT_QUE; outId++)
    {
        status = VpsUtils_bufDelete(&pObj->outFrameQue[outId]);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    Semaphore_delete(&pObj->lock);

#ifdef SYSTEM_DEBUG_DUP
    Vps_printf(" %d: DUP   : Delete Done !!!\n", Clock_getTicks());
#endif

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DupLink_getLinkInfo                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
System_LinkInfo *DupLink_getLinkInfo(VpsUtils_TskHndl * pTsk)
{
    DupLink_Obj *pObj = (DupLink_Obj *) pTsk->appData;

    return &pObj->info;
}

/* ===================================================================
 *  @func     DupLink_getFullFrames                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DupLink_getFullFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                            FVID2_FrameList * pFrameList)
{
    DupLink_Obj *pObj = (DupLink_Obj *) pTsk->appData;

    GT_assert(GT_DEFAULT_MASK, queId < DUP_LINK_MAX_OUT_QUE);
    GT_assert(GT_DEFAULT_MASK, queId < pObj->createArgs.numOutQue);

    return VpsUtils_bufGetFull(&pObj->outFrameQue[queId], pFrameList,
                               BIOS_NO_WAIT);
}

/* ===================================================================
 *  @func     DupLink_drvProcessFrames                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DupLink_drvProcessFrames(DupLink_Obj * pObj)
{
    UInt32 frameId, outId;

    FVID2_Frame *pFrame, *pOrgFrame;

    System_FrameInfo *pFrameInfo, *pOrgFrameInfo;

    Int32 status;

    DupLink_CreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;

    System_getLinksFullFrames(pCreateArgs->inQueParams.prevLinkId,
                              pCreateArgs->inQueParams.prevLinkQueId,
                              &pObj->inFrameList);

    if (pObj->inFrameList.numFrames)
    {
        pObj->getFrameCount += pObj->inFrameList.numFrames;

        for (outId = 0; outId < pCreateArgs->numOutQue; outId++)
        {
            pObj->outFrameList[outId].numFrames = 0;
        }

        for (frameId = 0; frameId < pObj->inFrameList.numFrames; frameId++)
        {
            pOrgFrame = pObj->inFrameList.frames[frameId];
            if (pOrgFrame == NULL)
                continue;

            pOrgFrameInfo = (System_FrameInfo *) pOrgFrame->appData;
            GT_assert(GT_DEFAULT_MASK, pOrgFrameInfo != NULL);

            pOrgFrameInfo->dupCount = pCreateArgs->numOutQue;

            for (outId = 0; outId < pCreateArgs->numOutQue; outId++)
            {
                status = VpsUtils_bufGetEmptyFrame(&pObj->outFrameQue[outId],
                                                   &pFrame, BIOS_NO_WAIT);
                GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
                GT_assert(GT_DEFAULT_MASK, pFrame != NULL);

                pFrameInfo = (System_FrameInfo *) pFrame->appData;
                GT_assert(GT_DEFAULT_MASK, pFrameInfo != NULL);

                memcpy(pFrame, pOrgFrame, sizeof(*pOrgFrame));
                memcpy(pFrameInfo, pOrgFrameInfo, sizeof(*pOrgFrameInfo));

                pFrame->appData = pFrameInfo;

                pFrameInfo->pDupOrgFrame = pOrgFrame;

                pObj->outFrameList[outId].frames[pObj->outFrameList[outId].
                                                 numFrames] = pFrame;

                pObj->outFrameList[outId].numFrames++;
            }
        }

        for (outId = 0; outId < pCreateArgs->numOutQue; outId++)
        {
            status =
                VpsUtils_bufPutFull(&pObj->outFrameQue[outId],
                                    &pObj->outFrameList[outId]);
            GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

            if (pCreateArgs->notifyNextLink)
            {
                System_sendLinkCmd(pCreateArgs->outQueParams[outId].nextLink,
                                   SYSTEM_CMD_NEW_DATA);
            }
        }
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DupLink_putEmptyFrames                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DupLink_putEmptyFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
    DupLink_Obj *pObj = (DupLink_Obj *) pTsk->appData;

    FVID2_FrameList freeFrameList;

    UInt32 frameId;

    FVID2_Frame *pFrame, *pOrgFrame;

    System_FrameInfo *pFrameInfo, *pOrgFrameInfo;

    Int32 status;

    GT_assert(GT_DEFAULT_MASK, queId < DUP_LINK_MAX_OUT_QUE);
    GT_assert(GT_DEFAULT_MASK, queId < pObj->createArgs.numOutQue);

    freeFrameList.numFrames = 0;

    Semaphore_pend(pObj->lock, BIOS_WAIT_FOREVER);

    for (frameId = 0; frameId < pFrameList->numFrames; frameId++)
    {
        pFrame = pFrameList->frames[frameId];
        if (pFrame == NULL)
            continue;

        pFrameInfo = (System_FrameInfo *) pFrame->appData;
        GT_assert(GT_DEFAULT_MASK, pFrameInfo != NULL);

        pOrgFrame = pFrameInfo->pDupOrgFrame;
        GT_assert(GT_DEFAULT_MASK, pOrgFrame != NULL);

        pOrgFrameInfo = (System_FrameInfo *) pOrgFrame->appData;
        GT_assert(GT_DEFAULT_MASK, pOrgFrameInfo != NULL);

        pOrgFrameInfo->dupCount--;

        if (pOrgFrameInfo->dupCount == 0)
        {
            freeFrameList.frames[freeFrameList.numFrames] = pOrgFrame;
            freeFrameList.numFrames++;
        }
    }

    pObj->putFrameCount += freeFrameList.numFrames;

    System_putLinksEmptyFrames(pObj->createArgs.inQueParams.prevLinkId,
                               pObj->createArgs.inQueParams.prevLinkQueId,
                               &freeFrameList);

    Semaphore_post(pObj->lock);

    status = VpsUtils_bufPutEmpty(&pObj->outFrameQue[queId], pFrameList);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     DupLink_tskMain                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Void DupLink_tskMain(struct VpsUtils_TskHndl * pTsk, VpsUtils_MsgHndl * pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status;

    DupLink_Obj *pObj = (DupLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = DupLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));

    VpsUtils_tskAckOrFreeMsg(pMsg, status);

    if (status != FVID2_SOK)
        return;

    done = FALSE;
    ackMsg = FALSE;

    while (!done)
    {
        status = VpsUtils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = VpsUtils_msgGetCmd(pMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
            case SYSTEM_CMD_NEW_DATA:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                DupLink_drvProcessFrames(pObj);
                break;
            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    DupLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/* ===================================================================
 *  @func     DupLink_init                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DupLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 dupId;

    DupLink_Obj *pObj;

    char tskName[32];

    for (dupId = 0; dupId < DUP_LINK_OBJ_MAX; dupId++)
    {
        pObj = &gDupLink_obj[dupId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->tskId = SYSTEM_LINK_ID_DUP_0 + dupId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = DupLink_getFullFrames;
        linkObj.linkPutEmptyFrames = DupLink_putEmptyFrames;
        linkObj.getLinkInfo = DupLink_getLinkInfo;

        System_registerLink(pObj->tskId, &linkObj);

        sprintf(tskName, "DUP%d", dupId);

        status = VpsUtils_tskCreate(&pObj->tsk,
                                    DupLink_tskMain,
                                    DUP_LINK_TSK_PRI,
                                    gDupLink_tskStack[dupId],
                                    DUP_LINK_TSK_STACK_SIZE, pObj, tskName);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     DupLink_deInit                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
Int32 DupLink_deInit()
{
    UInt32 dupId;

    for (dupId = 0; dupId < DUP_LINK_OBJ_MAX; dupId++)
    {
        VpsUtils_tskDelete(&gDupLink_obj[dupId].tsk);
    }
    return FVID2_SOK;
}
