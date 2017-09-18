/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/*
    This file implements the state machine logic for this link.
    A message command will cause the state machine to take some action and then
    move to a different state.

    The state machine table is as shown below

    Cmds| CREATE | DETECT_VIDEO | START | NEW_DATA  | STOP   | DELETE |
 States |========|==============|=======|===========|========|========|
   IDLE | READY  | -            | -     | -         | -      | -      |
   READY| -      | READY        | RUN   | -         | READY  | IDLE   |
   RUN  | -      | -            | -     | RUN       | READY  | IDLE   |

*/

#include "captureLink_priv.h"

/* link stack */
#pragma DATA_ALIGN(gCaptureLink_tskStack, 32)
#pragma DATA_SECTION(gCaptureLink_tskStack, ".bss:taskStackSection")
UInt8 gCaptureLink_tskStack[CAPTURE_LINK_TSK_STACK_SIZE];

/* link object, stores all link related information */
CaptureLink_Obj gCaptureLink_obj;

/*
    Run state implementation

    In this state link captures frames from VIP ports and sends it to the next link.
*/
Int32 CaptureLink_tskRun(CaptureLink_Obj *pObj, VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl **pMsg, Bool *done, Bool *ackMsg)
{
    Int32 status=FVID2_SOK;
    Bool runDone, runAckMsg;
    VpsUtils_MsgHndl *pRunMsg;
    UInt32 cmd;
    Int32 value;

    /* READY loop done and ackMsg status */
    *done = FALSE;
    *ackMsg = FALSE;
    *pMsg = NULL;

    /* RUN loop done and ackMsg status */
    runDone = FALSE;
    runAckMsg = FALSE;

    /* RUN state loop */
    while(!runDone)
    {
        /* wait for message */
        status = VpsUtils_tskRecvMsg(pTsk, &pRunMsg, BIOS_WAIT_FOREVER);
        if(status!=FVID2_SOK)
            break;

        /* extract message command from message */
        cmd = VpsUtils_msgGetCmd(pRunMsg);

        switch(cmd)
        {
            case SYSTEM_CMD_NEW_DATA:
                /* new data frames have been captured, process them */

                /* ACK or free message before proceding */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                status = CaptureLink_drvProcessData(pObj);
                if(status!=FVID2_SOK)
                {
                    /* in case of error exit RUN loop */
                    runDone = TRUE;

                    /* since message is already ACK'ed or free'ed do not
                        ACK or free it again */
                    runAckMsg = FALSE;
                }
                break;

            case CAPTURE_LINK_CMD_FORCE_RESET:
                /* new data frames have been captured, process them */

                /* ACK or free message before proceding */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                CaptureLink_drvOverflowDetectAndReset(pObj, TRUE);
                break;

            case CAPTURE_LINK_CMD_PRINT_ADV_STATISTICS:
                /* new data frames have been captured, process them */

                /* ACK or free message before proceding */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                CaptureLink_drvPrintStatus(pObj);

                break;
            case CAPTURE_LINK_CMD_HALT_EXECUTION:

                /* ACK or free message before proceding */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                System_haltExecution();
                break;

            case CAPTURE_LINK_CMD_CHANGE_BRIGHTNESS:

                value = (Int32)VpsUtils_msgGetPrm(pRunMsg);

                /* ACK or free message before proceding */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                CaptureLink_drvSetColor(pObj, 0, value, 0);
                break;

            case CAPTURE_LINK_CMD_CHANGE_CONTRAST:

                value = (Int32)VpsUtils_msgGetPrm(pRunMsg);

                /* ACK or free message before proceding */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                CaptureLink_drvSetColor(pObj, value, 0, 0);
                break;

            case CAPTURE_LINK_CMD_CHANGE_SATURATION:

                value = (Int32)VpsUtils_msgGetPrm(pRunMsg);

                /* ACK or free message before proceding */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

                CaptureLink_drvSetColor(pObj, 0, 0, value);
                break;

            case SYSTEM_CMD_STOP:
                /* stop RUN loop and goto READY state */
                runDone = TRUE;

                /* ACK message after actually stopping the driver outside the RUN loop */
                runAckMsg = TRUE;
                break;
            case SYSTEM_CMD_DELETE:

                /* stop RUN loop and goto IDLE state */

                /* exit RUN loop */
                runDone = TRUE;

                /* exit READY loop */
                *done = TRUE;

                /* ACK message after exiting READY loop */
                *ackMsg = TRUE;

                /* Pass the received message to the READY loop */
                *pMsg = pRunMsg;

                break;
            default:

                /* invalid command for this state
                    ACK it and continue RUN loop
                */
                VpsUtils_tskAckOrFreeMsg(pRunMsg, status);
                break;
        }


    }

    /* RUN loop exited, stop driver */
    CaptureLink_drvStop(pObj);

    /* ACK message if not ACKed earlier */
    if(runAckMsg)
        VpsUtils_tskAckOrFreeMsg(pRunMsg, status);

    return status;
}

/* IDLE and READY state implementation */
Void CaptureLink_tskMain(struct VpsUtils_TskHndl *pTsk, VpsUtils_MsgHndl *pMsg)
{
    UInt32 cmd = VpsUtils_msgGetCmd(pMsg);
    Bool ackMsg, done;
    Int32 status;
    CaptureLink_Obj *pObj;

    /* IDLE state */

    pObj = (CaptureLink_Obj *)pTsk->appData;

    if(cmd!=SYSTEM_CMD_CREATE)
    {
        /* invalid command recived in IDLE status, be in IDLE state
            and ACK with error status */
        VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_EFAIL);
        return;
    }

    /* Create command received, create the driver */
    status = CaptureLink_drvCreate(pObj, VpsUtils_msgGetPrm(pMsg));
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    /* ACK based on create status */
    VpsUtils_tskAckOrFreeMsg(pMsg, status);

    /* create success, entering READY state */
    done = FALSE;
    ackMsg = FALSE;

    /* READY state loop */
    while(!done)
    {
        /* wait for message */
        status = VpsUtils_tskRecvMsg(pTsk, &pMsg, BIOS_WAIT_FOREVER);
        if(status!=FVID2_SOK)
            break;

        /* extract message command from message */
        cmd = VpsUtils_msgGetCmd(pMsg);

        switch(cmd)
        {
            case CAPTURE_LINK_CMD_DETECT_VIDEO:
                /* detect video source, remain in READY state */
                status = CaptureLink_drvDetectVideo(pObj, (UInt32)VpsUtils_msgGetPrm(pMsg));
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;

            case SYSTEM_CMD_START:
                /* Start capture driver */
                status = CaptureLink_drvStart(pObj);

                /* ACK based on create status */
                VpsUtils_tskAckOrFreeMsg(pMsg, status);

                /* if start status is error then remain in READY state */
                if(status==FVID2_SOK)
                {
                    /* start success, entering RUN state */
                    status = CaptureLink_tskRun(pObj, pTsk, &pMsg, &done, &ackMsg);

                    /* done = FALSE, exit RUN state
                       done = TRUE, exit RUN and READY state
                    */
                }

                break;
            case SYSTEM_CMD_DELETE:

                /* exit READY state */
                done = TRUE;
                ackMsg = TRUE;
                break;
            default:
                /* invalid command for this state
                    ACK it and continue READY loop
                */
                VpsUtils_tskAckOrFreeMsg(pMsg, status);
                break;
        }
    }

    /* exiting READY state, delete driver */
    CaptureLink_drvDelete(pObj);

    /* ACK message if not previously ACK'ed */
    if(ackMsg && pMsg!=NULL)
        VpsUtils_tskAckOrFreeMsg(pMsg, status);

    /* entering IDLE state */
    return;
}

Int32 CaptureLink_init()
{
    Int32 status;
    System_LinkObj linkObj;
    CaptureLink_Obj *pObj;

    /* register link with system API */

    pObj = &gCaptureLink_obj;

    memset(pObj, 0, sizeof(*pObj));

    linkObj.pTsk = &pObj->tsk;
    linkObj.linkGetFullFrames  = CaptureLink_getFullFrames;
    linkObj.linkPutEmptyFrames = CaptureLink_putEmptyFrames;
    linkObj.getLinkInfo = CaptureLink_getInfo;

    System_registerLink(SYSTEM_LINK_ID_CAPTURE, &linkObj);

    /* Create link task, task remains in IDLE state
        CaptureLink_tskMain is called when a message command is received
    */

    status = VpsUtils_tskCreate(
                &pObj->tsk,
                CaptureLink_tskMain,
                CAPTURE_LINK_TSK_PRI,
                gCaptureLink_tskStack,
                CAPTURE_LINK_TSK_STACK_SIZE,
                pObj,
                "CAPTURE "
                );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    return status;
}


Int32 CaptureLink_deInit()
{
    CaptureLink_Obj *pObj;

    pObj = &gCaptureLink_obj;

    /*
        Delete link task
    */
    VpsUtils_tskDelete(&pObj->tsk);

    return FVID2_SOK;
}

/* return capture link info to the next link calling this API via system API */
System_LinkInfo *CaptureLink_getInfo(VpsUtils_TskHndl *pTsk)
{
    CaptureLink_Obj *pObj = (CaptureLink_Obj *)pTsk->appData;

    return &pObj->info;
}

/* return captured frames to the next link calling this API via system API */
Int32 CaptureLink_getFullFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    CaptureLink_Obj *pObj = (CaptureLink_Obj *)pTsk->appData;

    GT_assert( GT_DEFAULT_MASK, queId<CAPTURE_LINK_MAX_OUT_QUE);

    return VpsUtils_bufGetFull(&pObj->bufQue[queId], pFrameList, BIOS_NO_WAIT);
}

/* release captured frames to driver when the next link calls this API via system API */
Int32 CaptureLink_putEmptyFrames(VpsUtils_TskHndl *pTsk, UInt16 queId, FVID2_FrameList *pFrameList)
{
    CaptureLink_Obj *pObj = (CaptureLink_Obj *)pTsk->appData;

    GT_assert( GT_DEFAULT_MASK, queId<CAPTURE_LINK_MAX_OUT_QUE);

    return CaptureLink_drvPutEmptyFrames(pObj, pFrameList);
}
