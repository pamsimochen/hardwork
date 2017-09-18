/** ==================================================================
 *  @file   swMsLink_tsk.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/swMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "swMsLink_priv.h"

#pragma DATA_ALIGN(gSwMsLink_tskStack, 32)
#pragma DATA_SECTION(gSwMsLink_tskStack, ".bss:taskStackSection")
UInt8 gSwMsLink_tskStack[SW_MS_LINK_OBJ_MAX][SW_MS_LINK_TSK_STACK_SIZE];

SwMsLink_Obj gSwMsLink_obj[SW_MS_LINK_OBJ_MAX];

/* ===================================================================
 *  @func     SwMsLink_tskRun                                               
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
Int32 SwMsLink_tskRun(SwMsLink_Obj * pObj, VpsUtils_TskHndl * pTsk,
                      VpsUtils_MsgHndl ** pMsg, Bool * done, Bool * ackMsg)
{
    Int32 status = FVID2_SOK;

    Bool runDone, runAckMsg;

    VpsUtils_MsgHndl *pRunMsg;

    UInt32 cmd;

    Void *pPrm;

    *done = FALSE;
    *ackMsg = FALSE;

    runDone = FALSE;
    runAckMsg = FALSE;

    *pMsg = NULL;

    while (!runDone)
    {
        status = VpsUtils_tskRecvMsg(pTsk, &pRunMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = VpsUtils_msgGetCmd(pRunMsg);

        switch (cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                status = SwMsLink_drvProcessData(pObj);
                if (status != FVID2_SOK)
                {
                    runDone = TRUE;
                    runAckMsg = FALSE;
                }
                break;

            case SW_MS_LINK_CMD_DO_SCALING:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                SwMsLink_drvDoScaling(pObj);
                break;

            case SYSTEM_SW_MS_LINK_CMD_SWITCH_LAYOUT:
                pPrm = VpsUtils_msgGetPrm(pRunMsg);

                status = SwMsLink_drvSwitchLayout(pObj, pPrm);

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

    SwMsLink_drvStop(pObj);

    if (runAckMsg)
        VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

    return status;
}

/* ===================================================================
 *  @func     SwMsLink_tskMain                                               
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
Void SwMsLink_tskMain(struct VpsUtils_TskHndl * pTsk, VpsUtils_MsgHndl * pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status;

    SwMsLink_Obj *pObj;

    Void *pPrm;

    pObj = (SwMsLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = SwMsLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));

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
            case SYSTEM_CMD_START:
                status = SwMsLink_drvStart(pObj);

                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                if (status == FVID2_SOK)
                {
                    status = SwMsLink_tskRun(pObj, pTsk, &pMsg, &done, &ackMsg);
                }

                break;
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;

            case SYSTEM_SW_MS_LINK_CMD_SWITCH_LAYOUT:
                pPrm = VpsUtils_msgGetPrm(pMsg);

                status = SwMsLink_drvSwitchLayout(pObj, pPrm);

                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;

            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    SwMsLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/* ===================================================================
 *  @func     SwMsLink_init                                               
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
Int32 SwMsLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    SwMsLink_Obj *pObj;

    char name[32];

    UInt32 objId;

    for (objId = 0; objId < SW_MS_LINK_OBJ_MAX; objId++)
    {
        pObj = &gSwMsLink_obj[objId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->linkId = SYSTEM_LINK_ID_SW_MS_DEI_0 + objId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = SwMsLink_getFullFrames;
        linkObj.linkPutEmptyFrames = SwMsLink_putEmptyFrames;
        linkObj.getLinkInfo = SwMsLink_getInfo;

        sprintf(name, "SWMS%d", objId);

        System_registerLink(pObj->linkId, &linkObj);

        status = VpsUtils_tskCreate(&pObj->tsk,
                                    SwMsLink_tskMain,
                                    SW_MS_LINK_TSK_PRI,
                                    gSwMsLink_tskStack[objId],
                                    SW_MS_LINK_TSK_STACK_SIZE, pObj, name);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     SwMsLink_deInit                                               
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
Int32 SwMsLink_deInit()
{
    UInt32 objId;

    SwMsLink_Obj *pObj;

    for (objId = 0; objId < SW_MS_LINK_OBJ_MAX; objId++)
    {
        pObj = &gSwMsLink_obj[objId];

        VpsUtils_tskDelete(&pObj->tsk);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     SwMsLink_getInfo                                               
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
System_LinkInfo *SwMsLink_getInfo(VpsUtils_TskHndl * pTsk)
{
    SwMsLink_Obj *pObj = (SwMsLink_Obj *) pTsk->appData;

    return &pObj->info;
}

/* ===================================================================
 *  @func     SwMsLink_getFullFrames                                               
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
Int32 SwMsLink_getFullFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                             FVID2_FrameList * pFrameList)
{
    SwMsLink_Obj *pObj = (SwMsLink_Obj *) pTsk->appData;

    return VpsUtils_bufGetFull(&pObj->bufOutQue, pFrameList, BIOS_NO_WAIT);
}

/* ===================================================================
 *  @func     SwMsLink_putEmptyFrames                                               
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
Int32 SwMsLink_putEmptyFrames(VpsUtils_TskHndl * pTsk, UInt16 queId,
                              FVID2_FrameList * pFrameList)
{
    SwMsLink_Obj *pObj = (SwMsLink_Obj *) pTsk->appData;

    return VpsUtils_bufPutEmpty(&pObj->bufOutQue, pFrameList);
}
