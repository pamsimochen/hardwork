/** ==================================================================
 *  @file   displayLink_tsk.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/display/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include "displayLink_priv.h"

#pragma DATA_ALIGN(gDisplayLink_tskStack, 32)
#pragma DATA_SECTION(gDisplayLink_tskStack, ".bss:taskStackSection")
UInt8 gDisplayLink_tskStack[DISPLAY_LINK_OBJ_MAX][DISPLAY_LINK_TSK_STACK_SIZE];

DisplayLink_Obj gDisplayLink_obj[DISPLAY_LINK_OBJ_MAX];

/* ===================================================================
 *  @func     DisplayLink_tskRun                                               
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
Int32 DisplayLink_tskRun(DisplayLink_Obj * pObj, VpsUtils_TskHndl * pTsk,
                         VpsUtils_MsgHndl ** pMsg, Bool * done, Bool * ackMsg)
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

    while (!runDone)
    {
        status = VpsUtils_tskRecvMsg(pTsk, &pRunMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = VpsUtils_msgGetCmd(pRunMsg);

        switch (cmd)
        {
            case DISPLAY_LINK_CMD_DO_DEQUE:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                status = DisplayLink_drvProcessData(pObj);
                if (status != FVID2_SOK)
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
            case DISPLAY_LINK_CMD_SWITCH_CH:
                status =
                    DisplayLink_drvSwitchCh(pObj,
                                            (UInt32)
                                            VpsUtils_msgGetPrm(pRunMsg));
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                break;
            default:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                break;
        }

    }

    DisplayLink_drvStop(pObj);

    if (runAckMsg)
        VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

    return status;
}

/* ===================================================================
 *  @func     DisplayLink_tskMain                                               
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
Void DisplayLink_tskMain(struct VpsUtils_TskHndl * pTsk,
                         VpsUtils_MsgHndl * pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);

    Bool ackMsg, done;

    Int32 status;

    DisplayLink_Obj *pObj = (DisplayLink_Obj *) pTsk->appData;

    if (cmd != SYSTEM_CMD_CREATE)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    status = DisplayLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));

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
                status = DisplayLink_drvStart(pObj);

                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                if (status == FVID2_SOK)
                {
                    status =
                        DisplayLink_tskRun(pObj, pTsk, &pMsg, &done, &ackMsg);
                }

                break;
            case SYSTEM_CMD_DELETE:
                done = TRUE;
                ackMsg = TRUE;
                break;
            case DISPLAY_LINK_CMD_SWITCH_CH:
                status =
                    DisplayLink_drvSwitchCh(pObj,
                                            (UInt32) VpsUtils_msgGetPrm(pMsg));
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
            default:
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    DisplayLink_drvDelete(pObj);

    if (ackMsg && pMsg != NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    return;
}

/* ===================================================================
 *  @func     DisplayLink_init                                               
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
Int32 DisplayLink_init()
{
    Int32 status;

    System_LinkObj linkObj;

    UInt32 displayId;

    DisplayLink_Obj *pObj;

    char tskName[32];

    for (displayId = 0; displayId < DISPLAY_LINK_OBJ_MAX; displayId++)
    {
        pObj = &gDisplayLink_obj[displayId];

        memset(pObj, 0, sizeof(*pObj));

        pObj->tskId = SYSTEM_LINK_ID_DISPLAY_0 + displayId;

        linkObj.pTsk = &pObj->tsk;
        linkObj.linkGetFullFrames = NULL;
        linkObj.linkPutEmptyFrames = NULL;
        linkObj.getLinkInfo = NULL;

        System_registerLink(pObj->tskId, &linkObj);

        sprintf(tskName, "DISPLAY%d", displayId);

        status = VpsUtils_tskCreate(&pObj->tsk,
                                    DisplayLink_tskMain,
                                    DISPLAY_LINK_TSK_PRI,
                                    gDisplayLink_tskStack[displayId],
                                    DISPLAY_LINK_TSK_STACK_SIZE, pObj, tskName);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return status;
}

/* ===================================================================
 *  @func     DisplayLink_deInit                                               
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
Int32 DisplayLink_deInit()
{
    UInt32 displayId;

    for (displayId = 0; displayId < DISPLAY_LINK_OBJ_MAX; displayId++)
    {
        VpsUtils_tskDelete(&gDisplayLink_obj[displayId].tsk);
    }
    return FVID2_SOK;
}
