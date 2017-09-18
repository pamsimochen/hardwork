/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "nsfLink_priv.h"


#pragma DATA_ALIGN(gNsfLink_tskStack, 32)
#pragma DATA_SECTION(gNsfLink_tskStack, ".bss:taskStackSection")
UInt8 gNsfLink_tskStack[NSF_LINK_OBJ_MAX][NSF_LINK_TSK_STACK_SIZE];

NsfLink_Obj gNsfLink_obj[NSF_LINK_OBJ_MAX];

Int32 NsfLink_tskRun(NsfLink_Obj *pObj,
    VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl **pMsg, Bool *done, Bool *ackMsg)
{
    Int32 status = FVID2_SOK;
    Bool runDone, runAckMsg;
    VpsUtils_MsgHndl *pRunMsg;
    UInt32 cmd;

    *done = FALSE;
    *ackMsg = FALSE;

    runDone = FALSE;
    runAckMsg = FALSE;

    *pMsg = NULL;

    while(!runDone)
    {
        status = VpsUtils_tskRecvMsg(pTsk, &pRunMsg, BIOS_WAIT_FOREVER);
        if(status!=FVID2_SOK)
            break;

        cmd = VpsUtils_msgGetCmd(pRunMsg);

        switch(cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                status = NsfLink_drvProcessData(pObj);
                if(status!=FVID2_SOK)
                {
                    runDone = TRUE;
                    runAckMsg = TRUE;
                }
                break;

            case SYSTEM_CMD_STOP:
                runDone = TRUE;
                runAckMsg = TRUE;
                break;

            case SYSTEM_CMD_DELETE:
                *done = TRUE;
                *ackMsg = TRUE;
                *pMsg = pRunMsg;
                runDone = TRUE;
                break;

            default:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                break;
        }


    }

    if(runAckMsg)
        VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

    return status;
}

Void NsfLink_tskMain(struct VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl *pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    NsfLink_Obj *pObj;

    pObj = (NsfLink_Obj *)pTsk->appData;

    if(cmd!=SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = NsfLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));
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
            case SYSTEM_CMD_START:

                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                status = NsfLink_tskRun(pObj, pTsk, &pMsg, &done, &ackMsg);

                break;

            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    NsfLink_drvDelete(pObj);

    if(ackMsg && pMsg!=NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Int32 NsfLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    NsfLink_Obj *pObj;
    char name[32];
    UInt32 objId;

    for(objId = 0 ; objId < NSF_LINK_OBJ_MAX ; objId++)
    {
        pObj = &gNsfLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = NsfLink_getFullFrames;
        linkObj.linkPutEmptyFrames = NsfLink_putEmptyFrames;
        linkObj.getLinkInfo = NsfLink_getInfo;

        sprintf(name, "NSF%d", objId);

        System_registerLink(SYSTEM_LINK_ID_NSF_0 + objId, &linkObj);

        status = VpsUtils_tskCreate(
                    &pObj->tsk,
                    NsfLink_tskMain,
                    NSF_LINK_TSK_PRI,
                    gNsfLink_tskStack[objId],
                    NSF_LINK_TSK_STACK_SIZE,
                    pObj,
                    name
                    );
        GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return status;
}


Int32 NsfLink_deInit()
{
    UInt32 objId;
    NsfLink_Obj *pObj;

    for(objId = 0; objId < NSF_LINK_OBJ_MAX; objId++)
    {
        pObj = &gNsfLink_obj[objId];

        VpsUtils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

System_LinkInfo *NsfLink_getInfo(VpsUtils_TskHndl *pTsk)
{
    NsfLink_Obj *pObj = (NsfLink_Obj *)pTsk->appData;

    return &pObj->info;
}

Int32 NsfLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    NsfLink_Obj *pObj = (NsfLink_Obj *)pTsk->appData;
    Int32 status;

    if(queId < NSF_LINK_MAX_OUT_QUE)
        status = VpsUtils_bufGetFull(&pObj->bufOutQue[queId], pFrameList, BIOS_NO_WAIT);
    else
        status = VpsUtils_bufGetFull(&pObj->bufEvenFieldOutQue, pFrameList, BIOS_NO_WAIT);

    return status;
}

Int32 NsfLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    NsfLink_Obj *pObj = (NsfLink_Obj *)pTsk->appData;

    if(queId < NSF_LINK_MAX_OUT_QUE)
        NsfLink_drvPutEmptyFrames(pObj, pFrameList);

    return FVID2_SOK;
}

