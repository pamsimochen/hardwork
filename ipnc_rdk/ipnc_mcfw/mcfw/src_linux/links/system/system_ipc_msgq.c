/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "system_priv_common.h"
#include "system_priv_ipc.h"

Int32 System_ipcMsgQHeapCreate()
{
    Int32 status;
    UInt32 retryCount;

    {
        /* open heap */
        retryCount = 10;

        while(retryCount)
        {
            OSA_printf(" %u: SYSTEM: Opening MsgQ Heap [%s] ...\n",
                OSA_getCurTimeInMsec(),
                SYSTEM_IPC_MSGQ_HEAP_NAME
            );

            status = HeapMemMP_open(SYSTEM_IPC_MSGQ_HEAP_NAME, &gSystem_ipcObj.msgQHeapHndl);
            if (status == HeapMemMP_E_NOTFOUND) {
                /* Sleep for a while before trying again. */
                OSA_waitMsecs (1000);
            } else
            if (status == HeapMemMP_S_SUCCESS) {
                break;
            }
            retryCount--;
            if(retryCount<=0)
                UTILS_assert(0);
        }
    }

    /* Register this heap with MessageQ */
    MessageQ_registerHeap(
        (IHeap_Handle)gSystem_ipcObj.msgQHeapHndl,
        SYSTEM_IPC_MSGQ_HEAP
        );

    return OSA_SOK;
}

Int32 System_ipcMsgQHeapDelete()
{
    Int32 status;

    MessageQ_unregisterHeap(SYSTEM_IPC_MSGQ_HEAP);

    {
        /* close heap */
        status = HeapMemMP_close(&gSystem_ipcObj.msgQHeapHndl);
        UTILS_assert(status==OSA_SOK);
    }

    return OSA_SOK;
}

Int32 System_ipcMsgQTskCreate()
{
    Int32 status;

    status = OSA_mutexCreate(&gSystem_ipcObj.msgQLock);
    UTILS_assert(status==OSA_SOK);
    /*
     * Create task
     */
    status = OSA_thrCreate(
                &gSystem_ipcObj.msgQTask,
                System_ipcMsgQTaskMain,
                SYSTEM_MSGQ_TSK_PRI,
                SYSTEM_MSGQ_TSK_STACK_SIZE,
                NULL
            );
    UTILS_assert(status==OSA_SOK);

    return OSA_SOK;
}

Int32 System_ipcMsgQTskDelete()
{
    Int32 status;

    /* unblock task */
    MessageQ_unblock(gSystem_ipcObj.selfMsgQ);

    /* wait for command to be received
       and task to be exited */
    OSA_waitMsecs(10);

    OSA_thrDelete(&gSystem_ipcObj.msgQTask);

    status = OSA_mutexDelete ( &gSystem_ipcObj.msgQLock );
    UTILS_assert(status==OSA_SOK);

    return OSA_SOK;
}

Int32 System_ipcMsgQCreate()
{
    UInt32 i;
    UInt32 procId;
    Int32 status;
    Int32 retryCount;
    MessageQ_Params msgQParams;
    char msgQName[64];
    char ackMsgQName[64];

    i=0;

    while(gSystem_ipcEnableProcId[i]!=SYSTEM_PROC_MAX)
    {
        procId = gSystem_ipcEnableProcId[i];

        if (procId != SYSTEM_PROC_INVALID)
        {
            System_ipcGetMsgQName(procId, msgQName, ackMsgQName);

            if(procId==System_getSelfProcId())
            {
                /* create MsgQ */
                MessageQ_Params_init(&msgQParams);

                OSA_printf(" %u: SYSTEM: Creating MsgQ [%s] ...\n",
                    OSA_getCurTimeInMsec(),
                    msgQName
                );

                gSystem_ipcObj.selfMsgQ = MessageQ_create(msgQName, &msgQParams);
                UTILS_assert(gSystem_ipcObj.selfMsgQ!=NULL);

                MessageQ_Params_init(&msgQParams);

                OSA_printf(" %u: SYSTEM: Creating MsgQ [%s] ...\n",
                    OSA_getCurTimeInMsec(),
                    ackMsgQName
                );

                gSystem_ipcObj.selfAckMsgQ = MessageQ_create(ackMsgQName, &msgQParams);
                UTILS_assert(gSystem_ipcObj.selfMsgQ!=NULL);
            }
            else
            {
                /* open MsgQ */

                retryCount=10;
                while(retryCount)
                {
                    OSA_printf(" %u: SYSTEM: Opening MsgQ [%s] ...\n",
                        OSA_getCurTimeInMsec(),
                        msgQName
                    );

                    status = MessageQ_open(msgQName, &gSystem_ipcObj.remoteProcMsgQ[procId]);
                    if(status==MessageQ_E_NOTFOUND)
                        OSA_waitMsecs(1000);
                    else
                    if(status==MessageQ_S_SUCCESS)
                        break;

                    retryCount--;
                    if(retryCount<=0)
                        UTILS_assert(0);
                }

                /* no need to open ack msgq,
                    since ack msgq id is embeeded in the received message
                */
            }
        }
        i++;
    }
    return OSA_SOK;
}

Int32 System_ipcMsgQDelete()
{
    UInt32 i;
    UInt32 procId;
    Int32 status;

    i=0;

    while(gSystem_ipcEnableProcId[i]!=SYSTEM_PROC_MAX)
    {
        procId = gSystem_ipcEnableProcId[i];

        if (procId != SYSTEM_PROC_INVALID)
        {
	        if(procId==System_getSelfProcId())
	        {
	            /* delete MsgQ */

	            status = MessageQ_delete(&gSystem_ipcObj.selfMsgQ);
	            UTILS_assert(status==0);

	            status = MessageQ_delete(&gSystem_ipcObj.selfAckMsgQ);
	            UTILS_assert(status==0);
	        }
	        else
	        {
	            status = MessageQ_close(&gSystem_ipcObj.remoteProcMsgQ[procId]);
	            UTILS_assert(status==0);

	            /* no need to close ack msgq */
	        }
        }
        i++;
    }
    return OSA_SOK;
}

Int32 System_ipcMsgQInit()
{
    System_ipcMsgQHeapCreate();
    System_ipcMsgQCreate();
    System_ipcMsgQTskCreate();

    return OSA_SOK;
}

Int32 System_ipcMsgQDeInit()
{
    System_ipcMsgQTskDelete();
    System_ipcMsgQDelete();
    System_ipcMsgQHeapDelete();

    return OSA_SOK;
}

Void *System_ipcMsgQTaskMain(Void *arg)
{
    UInt32 prmSize;
    SystemIpcMsgQ_Msg *pMsgCommon;
    Void *pPrm;
    Int32 status;

    while(1)
    {
        status = MessageQ_get(gSystem_ipcObj.selfMsgQ, (MessageQ_Msg*)&pMsgCommon, OSA_TIMEOUT_FOREVER);
		if(pMsgCommon == NULL)
		    break;
        if(status==MessageQ_E_UNBLOCKED)
            break;

        if(status!=MessageQ_S_SUCCESS)
        {
            OSA_printf(" %u: MSGQ: MsgQ get failed !!!\n",
                        OSA_getCurTimeInMsec()
                        );
            continue;
        }

        #if 0
        OSA_printf(" %u: MSGQ: Received command [0x%04x] (prmSize = %d) for [%s][%02d] (waitAck=%d)\n",
            OSA_getCurTimeInMsec(),
            pMsgCommon->cmd,
            pMsgCommon->prmSize,
            System_getProcName(SYSTEM_GET_PROC_ID(pMsgCommon->linkId)),
            SYSTEM_GET_LINK_ID(pMsgCommon->linkId),
            pMsgCommon->waitAck
            );
        #endif

        prmSize = pMsgCommon->prmSize;

        pPrm = SYSTEM_IPC_MSGQ_MSG_PAYLOAD_PTR(pMsgCommon);

        if(pMsgCommon->cmd==SYSTEM_CMD_GET_INFO)
        {
            UTILS_assert(prmSize == sizeof(System_LinkInfo));

            pMsgCommon->status = System_linkGetInfo_local(pMsgCommon->linkId, pPrm);
        }
        else
        {
            pMsgCommon->status = System_linkControl_local(
                                    pMsgCommon->linkId,
                                    pMsgCommon->cmd,
                                    pPrm,
                                    prmSize,
                                    pMsgCommon->waitAck
                                 );
        }
        if(pMsgCommon->waitAck)
        {
            MessageQ_QueueId replyMsgQ;

            replyMsgQ = MessageQ_getReplyQueue(pMsgCommon);

            status = MessageQ_put(replyMsgQ, (MessageQ_Msg)pMsgCommon);

            if(status!=MessageQ_S_SUCCESS)
            {
                OSA_printf(" %u: MSGQ: MsgQ Ack put failed !!!\n",
                        OSA_getCurTimeInMsec()
                        );
                MessageQ_free((MessageQ_Msg)pMsgCommon);
            }
        }
        else
        {
            MessageQ_free((MessageQ_Msg)pMsgCommon);
        }
    }

    return NULL;
}

Int32 System_ipcMsgQSendMsg(UInt32 linkId, UInt32 cmd, Void *pPrm, UInt32 prmSize, Bool waitAck, UInt32 timeout)
{
    Int32 status=OSA_SOK;
    SystemIpcMsgQ_Msg *pMsgCommon;
    UInt32 procId;
    Void *pMsgPrm;

    UTILS_assert(prmSize<=SYSTEM_IPC_MSGQ_MSG_SIZE_MAX);

    procId = SYSTEM_GET_PROC_ID(linkId);

    #ifdef TI_8107_BUILD
    if(procId==SYSTEM_PROC_DSP)
    {
        OSA_printf(" %u: MSGQ: WARNING: Trying to send command [0x%04x] to link [%d] on processor [%s], BUT [%s] is NOT present on this platform !!!\n",
                        OSA_getCurTimeInMsec(),
                        cmd,
                        SYSTEM_GET_LINK_ID(linkId),
                        System_getProcName(procId),
                        System_getProcName(procId)
                        );

        /* return SUCCESS so that calling API can continue */
        return status;
    }
    #endif

    OSA_mutexLock(&gSystem_ipcObj.msgQLock);

    UTILS_assert(  procId < SYSTEM_PROC_MAX);

    pMsgCommon = (SystemIpcMsgQ_Msg *)MessageQ_alloc(
                    SYSTEM_IPC_MSGQ_HEAP,
                    sizeof(*pMsgCommon)+prmSize
                    );

    UTILS_assert(pMsgCommon!=NULL);

    if(prmSize && pPrm)
    {
        pMsgPrm = SYSTEM_IPC_MSGQ_MSG_PAYLOAD_PTR(pMsgCommon);
        memcpy(pMsgPrm, pPrm, prmSize);
    }

    pMsgCommon->linkId = linkId;
    pMsgCommon->cmd = cmd;
    pMsgCommon->prmSize = prmSize;
    pMsgCommon->waitAck = waitAck;
    pMsgCommon->status = OSA_SOK;

    MessageQ_setReplyQueue(gSystem_ipcObj.selfAckMsgQ, (MessageQ_Msg)pMsgCommon);
    MessageQ_setMsgId(pMsgCommon, linkId);

    status = MessageQ_put(gSystem_ipcObj.remoteProcMsgQ[procId], (MessageQ_Msg)pMsgCommon);
    if(status!=MessageQ_S_SUCCESS)
    {
        OSA_printf(" %u: MSGQ: MsgQ put for [%s] failed !!!\n",
                        OSA_getCurTimeInMsec(),
                        System_getProcName(procId)
                        );
        MessageQ_free((MessageQ_Msg)pMsgCommon);
        OSA_mutexUnlock(&gSystem_ipcObj.msgQLock);
        return status;
    }

    if(waitAck)
    {
        SystemIpcMsgQ_Msg *pAckMsg;
        Bool retryMsgGet = FALSE;
        UInt32 retryCount = 1000;
        if (OSA_TIMEOUT_FOREVER == timeout)
        {
            timeout = MessageQ_FOREVER;
        }

        do {
            status = MessageQ_get(gSystem_ipcObj.selfAckMsgQ, (MessageQ_Msg*)&pAckMsg, timeout);
            if(status==MessageQ_E_TIMEOUT)
            {
                /* if timeout then break out of retry loop */
                break;
            }
            if(status!=MessageQ_S_SUCCESS)
            {
                /* normally this situation should not happen, this more for safety and debug purpose */
                retryCount--;

                OSA_printf(" %u: MSGQ: MsgQ Ack get from [%s] failed for link %d, cmdId 0x%04x !!! (retrying - %d times)\n",
                           OSA_getCurTimeInMsec(),
                           System_getProcName(procId),
                           SYSTEM_GET_LINK_ID(linkId),
                           cmd,
                           retryCount
                            );

                if(retryCount==0)
                {
                    retryMsgGet = FALSE;
                }
                else
                {
                    retryMsgGet = TRUE;
                }
            }
            if (status == MessageQ_S_SUCCESS)
            {
                if (!((pAckMsg->linkId == linkId)
                      &&
                      (pAckMsg->cmd    == cmd)))
                {
                    /* normally this situation should not happen, this more for safety and debug purpose */

                    OSA_printf(" %u: MSGQ: MsgQ Ack get from [%s] failed for link %d, cmdId 0x%04x !!! \n"
                           "           Received unexpected Ack from [%s] link %d, cmdId 0x%04x !!! \n",
                           OSA_getCurTimeInMsec(),
                           System_getProcName(procId),
                           SYSTEM_GET_LINK_ID(linkId),
                           cmd,
                           System_getProcName(SYSTEM_GET_PROC_ID(pAckMsg->linkId)),
                           SYSTEM_GET_LINK_ID(pAckMsg->linkId),
                           pAckMsg->cmd
                           );

                    MessageQ_free((MessageQ_Msg)pAckMsg);
                    retryMsgGet = TRUE;
                }
            }

        } while(TRUE == retryMsgGet);

        if (status!=MessageQ_S_SUCCESS)
        {
            /* Do not free msg if MsgQ_Get timesout.
             * Since we dont have Msg ownership, we should not free the msg
             */
            /* MessageQ_free((MessageQ_Msg)pMsgCommon); */
            OSA_mutexUnlock(&gSystem_ipcObj.msgQLock);
            return status;
        }

        if(prmSize && pPrm)
        {
            pMsgPrm = SYSTEM_IPC_MSGQ_MSG_PAYLOAD_PTR(pAckMsg);
            memcpy(pPrm, pMsgPrm, prmSize);
        }

        status = pAckMsg->status;

        MessageQ_free((MessageQ_Msg)pAckMsg);
    }

    OSA_mutexUnlock(&gSystem_ipcObj.msgQLock);

    return status;
}
