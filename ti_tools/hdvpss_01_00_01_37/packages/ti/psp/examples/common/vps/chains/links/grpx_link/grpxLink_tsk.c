/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "grpxLink_priv.h"


#pragma DATA_ALIGN(gGrpxLink_tskStack, 32)
#pragma DATA_SECTION(gGrpxLink_tskStack, ".bss:taskStackSection")
UInt8 gGrpxLink_tskStack[GRPX_LINK_OBJ_MAX][GRPX_LINK_TSK_STACK_SIZE];

GrpxLink_Obj gGrpxLink_obj[GRPX_LINK_OBJ_MAX];


Int32 GrpxLink_tskRun(GrpxLink_Obj *pObj, VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl **pMsg, Bool *done, Bool *ackMsg)
{
    Int32 status=FVID2_SOK;
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
            case GRPX_LINK_CMD_DO_DEQUE:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                status = GrpxLink_drvProcessData(pObj);
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
            case GRPX_LINK_CMD_SWITCH_CH:
                status = GrpxLink_drvSwitchCh(pObj, (UInt32)VpsUtils_msgGetPrm(pRunMsg));
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                break;
            default:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                break;
        }


    }

    GrpxLink_drvStop(pObj);

    if(runAckMsg)
        VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

    return status;
}

Void GrpxLink_tskMain(struct VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl *pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    GrpxLink_Obj *pObj = (GrpxLink_Obj*)pTsk->appData;

    if(cmd!=SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = GrpxLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));

    VpsUtils_tskAckOrFreeMsg(pMsg, status);

    if(status!=FVID2_SOK)
        return;

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
                status = GrpxLink_drvStart(pObj);

                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                if(status==FVID2_SOK)
                {
                    status = GrpxLink_tskRun(pObj, pTsk, &pMsg, &done, &ackMsg);
                }

                break;
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
            case GRPX_LINK_CMD_SWITCH_CH:
                status = GrpxLink_drvSwitchCh(pObj, (UInt32)VpsUtils_msgGetPrm(pMsg));
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    GrpxLink_drvDelete(pObj);

    if(ackMsg && pMsg!=NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

Int32 GrpxLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    UInt32 grpxId;
    GrpxLink_Obj  *pObj;
    char tskName[32];

    for(grpxId=0; grpxId<GRPX_LINK_OBJ_MAX; grpxId++)
    {
        pObj = &gGrpxLink_obj[grpxId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->tskId = SYSTEM_LINK_ID_GRPX_0 + grpxId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames  = NULL;
        linkObj.linkPutEmptyFrames = NULL;
        linkObj.getLinkInfo = NULL;

        System_registerLink(pObj->tskId, &linkObj);

        sprintf(tskName, "GRPX%d", grpxId);

        status = VpsUtils_tskCreate(
                    &pObj->tsk,
                    GrpxLink_tskMain,
                    GRPX_LINK_TSK_PRI,
                    gGrpxLink_tskStack[grpxId],
                    GRPX_LINK_TSK_STACK_SIZE,
                    pObj,
                    tskName
                    );
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    return status;
}


Int32 GrpxLink_deInit()
{
    UInt32 grpxId;

    for(grpxId=0; grpxId<GRPX_LINK_OBJ_MAX; grpxId++)
    {
        VpsUtils_tskDelete(&gGrpxLink_obj[grpxId].tsk);
    }
    return FVID2_SOK;
}

