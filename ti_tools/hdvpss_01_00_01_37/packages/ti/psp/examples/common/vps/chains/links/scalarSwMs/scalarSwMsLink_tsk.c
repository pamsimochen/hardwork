/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "scalarSwMsLink_priv.h"


#pragma DATA_ALIGN(gScalarSwMsLink_tskStack, 32)
#pragma DATA_SECTION(gScalarSwMsLink_tskStack, ".bss:taskStackSection")
UInt8 gScalarSwMsLink_tskStack[SCALAR_SW_MS_LINK_OBJ_MAX][SCALAR_LINK_TSK_STACK_SIZE];

ScalarSwMsLink_Obj gScalarSwMsLink_obj[SCALAR_SW_MS_LINK_OBJ_MAX];

Int32 ScalarSwMsLink_tskRun(ScalarSwMsLink_Obj *pObj, VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl **pMsg, Bool *done, Bool *ackMsg)
{
    Int32 status=FVID2_SOK;
    Bool runDone, runAckMsg;
    VpsUtils_MsgHndl *pRunMsg;
    UInt32 cmd;
    ScalarSwMsLink_LayoutParams *pScLayoutPrm;

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

                status = ScalarSwMsLink_drvProcessData(pObj);
                if(status!=FVID2_SOK)
                {
                    runDone = TRUE;
                    runAckMsg = TRUE;
                }
                break;

            case SCALAR_SW_MS_LINK_CMD_DO_SCALING:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                ScalarSwMsLink_drvDoScaling(pObj);
                break;

            case SCALAR_SW_MS_LINK_CMD_SWITCH_LAYOUT:
                pScLayoutPrm = VpsUtils_msgGetPrm(pRunMsg);

                status = ScalarSwMsLink_drvSwitchLayout(pObj, pObj->createArgs.outLayoutMode, pScLayoutPrm->startWinChId);

                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
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

    ScalarSwMsLink_drvStop(pObj);

    if(runAckMsg)
        VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

    return status;
}

Void ScalarSwMsLink_tskMain(struct VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl *pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    ScalarSwMsLink_LayoutParams *pScLayoutPrm;
    ScalarSwMsLink_Obj *pObj;

    pObj = (ScalarSwMsLink_Obj *)pTsk->appData;

    if(cmd!=SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = ScalarSwMsLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));
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
                status = ScalarSwMsLink_drvStart(pObj);

                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                if(status==FVID2_SOK)
                {
                    status = ScalarSwMsLink_tskRun(pObj, pTsk, &pMsg, &done, &ackMsg);
                }

                break;
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            case SCALAR_SW_MS_LINK_CMD_SWITCH_LAYOUT:
                pScLayoutPrm = VpsUtils_msgGetPrm(pMsg);

                status = ScalarSwMsLink_drvSwitchLayout(pObj, pObj->createArgs.outLayoutMode, pScLayoutPrm->startWinChId);

                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;

            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    ScalarSwMsLink_drvDelete(pObj);

    if(ackMsg && pMsg!=NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Int32 ScalarSwMsLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    ScalarSwMsLink_Obj *pObj;
    char name[32];
    UInt32 objId;

    for(objId=0; objId<SCALAR_SW_MS_LINK_OBJ_MAX; objId++)
    {
        pObj = &gScalarSwMsLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->linkId = SYSTEM_LINK_ID_SCALAR_SW_MS_0 + objId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames  = ScalarSwMsLink_getFullFrames;
        linkObj.linkPutEmptyFrames = ScalarSwMsLink_putEmptyFrames;
        linkObj.getLinkInfo = ScalarSwMsLink_getInfo;

        sprintf(name, "SC_SWMS%d", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = VpsUtils_tskCreate(
                &pObj->tsk,
                ScalarSwMsLink_tskMain,
                SCALAR_LINK_TSK_PRI,
                gScalarSwMsLink_tskStack[objId],
                SCALAR_LINK_TSK_STACK_SIZE,
                pObj,
                name
                );
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    return status;
}


Int32 ScalarSwMsLink_deInit()
{
    UInt32 objId;
    ScalarSwMsLink_Obj *pObj;

    for(objId=0; objId<SCALAR_SW_MS_LINK_OBJ_MAX; objId++)
    {
        pObj = &gScalarSwMsLink_obj[objId];

        VpsUtils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

System_LinkInfo *ScalarSwMsLink_getInfo(VpsUtils_TskHndl *pTsk)
{
    ScalarSwMsLink_Obj *pObj = (ScalarSwMsLink_Obj *)pTsk->appData;

    return &pObj->info;
}

Int32 ScalarSwMsLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    ScalarSwMsLink_Obj *pObj = (ScalarSwMsLink_Obj *)pTsk->appData;

    return VpsUtils_bufGetFull(&pObj->bufOutQue, pFrameList, BIOS_NO_WAIT);
}

Int32 ScalarSwMsLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    ScalarSwMsLink_Obj *pObj = (ScalarSwMsLink_Obj *)pTsk->appData;

    return VpsUtils_bufPutEmpty(&pObj->bufOutQue, pFrameList);
}
