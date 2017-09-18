/** ==================================================================
 *  @file   displayHwMsLink_tsk.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/displayHwMs/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
 *  \file displayHwMsLink_tsk.c
 *
 *  \brief Display mosaic link main task file.
 *
 */

/* ========================================================================== 
 */
/* Include Files */
/* ========================================================================== 
 */

#include "displayHwMsLink_priv.h"

/* ========================================================================== 
 */
/* Macros & Typedefs */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Structure Declarations */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Function Declarations */
/* ========================================================================== 
 */

/* None */

/* ========================================================================== 
 */
/* Global Variables */
/* ========================================================================== 
 */

#pragma DATA_ALIGN(gDispHwMsLink_tskStack, 32)
#pragma DATA_SECTION(gDispHwMsLink_tskStack, ".bss:taskStackSection")
/** \brief Display mosaic main task stack memory. */
UInt8 gDispHwMsLink_tskStack[DISP_HWMS_LINK_OBJ_MAX]
    [DISPLAY_HWMS_LINK_TSK_STACK_SIZE];

/** \brief Display mosaic objects. */
DispHwMsLink_Obj gDispHwMsLink_obj[DISP_HWMS_LINK_OBJ_MAX];

/* ========================================================================== 
 */
/* Function Definitions */
/* ========================================================================== 
 */

/* ===================================================================
 *  @func     DispHwMsLink_tskRun                                               
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
Int32 DispHwMsLink_tskRun(DispHwMsLink_Obj * pObj,
                          VpsUtils_TskHndl * pTsk,
                          VpsUtils_MsgHndl ** pMsg, Bool * done, Bool * ackMsg)
{
    Bool runDone, runAckMsg;

    Int32 status = FVID2_SOK;

    UInt32 cmd;

    VpsUtils_MsgHndl *pRunMsg;

    *done = FALSE;
    *ackMsg = FALSE;
    runDone = FALSE;
    runAckMsg = FALSE;
    *pMsg = NULL;
    while (!runDone)
    {
        /* Wait for new frames/events once start is done */
        status = VpsUtils_tskRecvMsg(pTsk, &pRunMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
        {
            break;
        }

        /* Take necessary action depending upon message received */
        cmd = VpsUtils_msgGetCmd(pRunMsg);
        switch (cmd)
        {
            case DISP_HWMS_LINK_CMD_DO_DEQUE:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                status = DispHwMsLink_drvProcessData(pObj);
                if (status != FVID2_SOK)
                {
                    runDone = TRUE;
                    runAckMsg = TRUE;
                }
                break;
#if 0
            case SYSTEM_CMD_NEW_DATA:
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                status = DispHwMsLink_drvRcvFrames(pObj);
                if (status != FVID2_SOK)
                {
                    runDone = TRUE;
                    runAckMsg = TRUE;
                    Vps_printf(" %d: DISPLAY: Error receiving frames !!!\n",
                               Clock_getTicks());
                }
                break;
#endif
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

    DispHwMsLink_drvStop(pObj);

    if (runAckMsg)
    {
        VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
    }

    return (status);
}

/* ===================================================================
 *  @func     DispHwMsLink_tskMain                                               
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
Void DispHwMsLink_tskMain(struct VpsUtils_TskHndl * pTsk,
                          VpsUtils_MsgHndl * pMsg)
{
    Int32 status;

    Bool ackMsg, done;

    UInt32 cmd;

    DispHwMsLink_Obj *pObj = (DispHwMsLink_Obj *) pTsk->appData;

    cmd = VpsUtils_msgGetCmd(pMsg);
    if (SYSTEM_CMD_CREATE != cmd)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* Create link */
    status = DispHwMsLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));
    VpsUtils_tskAckOrFreeMsg(pMsg, status);
    if (status != FVID2_SOK)
    {
        return;
    }

    done = FALSE;
    ackMsg = FALSE;
    while (!done)
    {
        /* Wait for new message/command from chains */
        status = VpsUtils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
        {
            break;
        }

        cmd = VpsUtils_msgGetCmd(pMsg);
        switch (cmd)
        {
            case SYSTEM_CMD_START:
                /* Start link/driver */
                status = DispHwMsLink_drvStart(pObj);
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                if (FVID2_SOK == status)
                {
                    /* Started!! Display incoming frames still stop command */
                    status = DispHwMsLink_tskRun(pObj,
                                                 pTsk, &pMsg, &done, &ackMsg);
                }
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

    DispHwMsLink_drvDelete(pObj);
    if (ackMsg && pMsg != NULL)
    {
        VpsUtils_tskAckOrFreeMsg(pMsg, status);
    }

    return;
}

/* ===================================================================
 *  @func     DispHwMsLink_init                                               
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
Int32 DispHwMsLink_init(void)
{
    char tskName[32];

    Int32 status;

    UInt32 displayId;

    System_LinkObj linkObj;

    DispHwMsLink_Obj *pObj;

    for (displayId = 0u; displayId < DISP_HWMS_LINK_OBJ_MAX; displayId++)
    {
        pObj = &gDispHwMsLink_obj[displayId];
        memset(pObj, 0u, sizeof(*pObj));
        pObj->taskId = SYSTEM_LINK_ID_DISPLAY_HW_MS_0 + displayId;

        /* Register the link with system */
        linkObj.pTsk = &pObj->taskHndl;
        linkObj.linkGetFullFrames = NULL;
        linkObj.linkPutEmptyFrames = NULL;
        linkObj.getLinkInfo = NULL;
        System_registerLink(pObj->taskId, &linkObj);

        /* Create main task for the link */
        sprintf(tskName, "DISP_HWMS%d", displayId);
        status = VpsUtils_tskCreate(&pObj->taskHndl,
                                    DispHwMsLink_tskMain,
                                    DISPLAY_LINK_TSK_PRI,
                                    gDispHwMsLink_tskStack[displayId],
                                    DISPLAY_HWMS_LINK_TSK_STACK_SIZE,
                                    pObj, tskName);
        GT_assert(GT_DEFAULT_MASK, FVID2_SOK == status);
    }

    return (status);
}

/* ===================================================================
 *  @func     DispHwMsLink_deInit                                               
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
Int32 DispHwMsLink_deInit(void)
{
    UInt32 displayId;

    for (displayId = 0u; displayId < DISP_HWMS_LINK_OBJ_MAX; displayId++)
    {
        VpsUtils_tskDelete(&gDispHwMsLink_obj[displayId].taskHndl);
    }

    return (FVID2_SOK);
}
