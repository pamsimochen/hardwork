/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "deiLink_priv.h"


#pragma DATA_ALIGN(gDeiLink_tskStack, 32)
#pragma DATA_SECTION(gDeiLink_tskStack, ".bss:taskStackSection")
UInt8 gDeiLink_tskStack[DEI_LINK_OBJ_MAX][DEI_LINK_TSK_STACK_SIZE];

DeiLink_Obj gDeiLink_obj[DEI_LINK_OBJ_MAX];


Void DeiLink_tskMain(struct VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl *pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    DeiLink_Obj *pObj;

    pObj = (DeiLink_Obj *)pTsk->appData;

    if(cmd!=SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = DeiLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    VpsUtils_tskAckOrFreeMsg(pMsg, status);

    done = FALSE;
    ackMsg = FALSE;

    while(!done)
    {
        status = VpsUtils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if(status!=FVID2_SOK)
            break;

        cmd = VpsUtils_msgGetCmd(pMsg);

        switch(cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                DeiLink_drvProcessData(pObj);
                break;

            case DEI_LINK_CMD_GET_PROCESSED_DATA:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                DeiLink_drvGetProcessedData(pObj);
                break;

            case SYSTEM_CMD_STOP:
                DeiLink_drvStop(pObj);
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_DELETE:
                DeiLink_drvStop(pObj);
                done = TRUE;
                ackMsg = TRUE;
                break;

            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    DeiLink_drvDelete(pObj);

    if(ackMsg && pMsg!=NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Int32 DeiLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    DeiLink_Obj *pObj;
    char name[32];
    UInt32 objId;

    for(objId=0; objId<DEI_LINK_OBJ_MAX; objId++)
    {
        pObj = &gDeiLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));
#ifdef TI_816X_BUILD
        pObj->linkId = SYSTEM_LINK_ID_DEI_HQ_0 + objId;
#endif /* TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
        pObj->linkId = SYSTEM_LINK_ID_DEI_0 + objId;
#endif /* TI_816X_BUILD || TI_8107_BUILD */

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames  = DeiLink_getFullFrames;
        linkObj.linkPutEmptyFrames = DeiLink_putEmptyFrames;
        linkObj.getLinkInfo = DeiLink_getInfo;

        sprintf(name, "DEI%d   ", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = VpsUtils_tskCreate(
                &pObj->tsk,
                DeiLink_tskMain,
                DEI_LINK_TSK_PRI,
                gDeiLink_tskStack[objId],
                DEI_LINK_TSK_STACK_SIZE,
                pObj,
                name
                );
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    return status;
}


Int32 DeiLink_deInit()
{
    UInt32 objId;
    DeiLink_Obj *pObj;

    for(objId=0; objId<DEI_LINK_OBJ_MAX; objId++)
    {
        pObj = &gDeiLink_obj[objId];

        VpsUtils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

System_LinkInfo *DeiLink_getInfo(VpsUtils_TskHndl *pTsk)
{
    DeiLink_Obj *pObj = (DeiLink_Obj *)pTsk->appData;

    return &pObj->info;
}

Int32 DeiLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    DeiLink_Obj *pObj = (DeiLink_Obj *)pTsk->appData;

    GT_assert( GT_DEFAULT_MASK, queId < DEI_LINK_MAX_OUT_QUE);

    return VpsUtils_bufGetFull(&pObj->outObj[queId].bufOutQue, pFrameList, BIOS_NO_WAIT);
}

Int32 DeiLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    DeiLink_Obj *pObj = (DeiLink_Obj *)pTsk->appData;

    GT_assert( GT_DEFAULT_MASK, queId < DEI_LINK_MAX_OUT_QUE);

    return VpsUtils_bufPutEmpty(&pObj->outObj[queId].bufOutQue, pFrameList);
}
