/** ==================================================================
 *  @file   issutils_mbx.c                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/platforms/utility/issutils_mbx.h>
#include <ti/sysbios/hal/Hwi.h>

#define ISSUTILS_MBX_MSG_POOL_MAX   (10240)

static IssUtils_QueHandle gIssUtils_mbxMsgPoolFreeQue;

static Ptr gIssUtils_mbxMsgPoolFreeQueMem[ISSUTILS_MBX_MSG_POOL_MAX];

static IssUtils_MsgHndl gIssUtils_mbxMsgPool[ISSUTILS_MBX_MSG_POOL_MAX];

/* ===================================================================
 *  @func     IssUtils_mbxGetFreeMsgCount                                               
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
UInt32 IssUtils_mbxGetFreeMsgCount()
{
    UInt32 cookie;

    UInt32 freeMsg;

    cookie = Hwi_disable();

    freeMsg = gIssUtils_mbxMsgPoolFreeQue.count;

    Hwi_restore(cookie);

    return freeMsg;
}

/* ===================================================================
 *  @func     IssUtils_mbxAllocMsg                                               
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
IssUtils_MsgHndl *IssUtils_mbxAllocMsg(UInt32 timeout)
{
    IssUtils_MsgHndl *pMsg = NULL;

    IssUtils_queGet(&gIssUtils_mbxMsgPoolFreeQue, (Ptr *) & pMsg, 1, timeout);

    return pMsg;
}

/* ===================================================================
 *  @func     IssUtils_mbxFreeMsg                                               
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
Int32 IssUtils_mbxFreeMsg(IssUtils_MsgHndl * pMsg, UInt32 timeout)
{
    Int32 status;

    status = IssUtils_quePut(&gIssUtils_mbxMsgPoolFreeQue, pMsg, timeout);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_mbxInit                                               
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
Int32 IssUtils_mbxInit()
{
    Int32 status, msgId;

    status = IssUtils_queCreate(&gIssUtils_mbxMsgPoolFreeQue,
                                ISSUTILS_MBX_MSG_POOL_MAX,
                                gIssUtils_mbxMsgPoolFreeQueMem,
                                ISSUTILS_QUE_FLAG_BLOCK_QUE);

    if (status != 0)
        return status;

    for (msgId = 0; msgId < ISSUTILS_MBX_MSG_POOL_MAX; msgId++)
    {
        status =
            IssUtils_mbxFreeMsg(&gIssUtils_mbxMsgPool[msgId], BIOS_NO_WAIT);

        if (status != 0)
        {
            IssUtils_mbxDeInit();
            break;
        }
    }

    return status;
}

/* ===================================================================
 *  @func     IssUtils_mbxDeInit                                               
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
Int32 IssUtils_mbxDeInit()
{
    Int32 status;

    status = IssUtils_queDelete(&gIssUtils_mbxMsgPoolFreeQue);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_mbxCreate                                               
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
Int32 IssUtils_mbxCreate(IssUtils_MbxHndl * pMbx)
{
    Int32 status;

    /* create queues */

    status = IssUtils_queCreate(&pMbx->recvQue,
                                ISSUTILS_MBX_RECV_QUE_LEN_MAX,
                                pMbx->memRecvQue, ISSUTILS_QUE_FLAG_BLOCK_QUE);

    if (status != 0)
        return status;

    status = IssUtils_queCreate(&pMbx->ackQue,
                                ISSUTILS_MBX_ACK_QUE_LEN_MAX,
                                pMbx->memAckQue, ISSUTILS_QUE_FLAG_BLOCK_QUE);

    if (status != 0)
    {
        IssUtils_queDelete(&pMbx->recvQue);
    }

    return status;
}

/* ===================================================================
 *  @func     IssUtils_mbxDelete                                               
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
Int32 IssUtils_mbxDelete(IssUtils_MbxHndl * pMbx)
{
    Int32 status = 0;

    /* delete queues */

    status |= IssUtils_queDelete(&pMbx->recvQue);
    status |= IssUtils_queDelete(&pMbx->ackQue);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_mbxSendCmd                                               
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
Int32 IssUtils_mbxSendCmd(IssUtils_MbxHndl * pTo, UInt32 cmd)
{
    IssUtils_MsgHndl *pSentMsg;

    Int32 retVal = 0;

    if (pTo == NULL)
        return -1;

    /* alloc message */
    pSentMsg = IssUtils_mbxAllocMsg(BIOS_NO_WAIT);
    if (pSentMsg == NULL)
        return -1;

    /* set message fields */
    pSentMsg->pFrom = NULL;
    pSentMsg->flags = 0;
    pSentMsg->cmd = cmd;
    pSentMsg->result = 0;
    pSentMsg->pPrm = NULL;

    /* send message */
    retVal = IssUtils_quePut(&pTo->recvQue, pSentMsg, BIOS_NO_WAIT);

    if (retVal != 0)
    {
        retVal |= IssUtils_mbxFreeMsg(pSentMsg, BIOS_NO_WAIT);
    }

    return retVal;
}

/* ===================================================================
 *  @func     IssUtils_mbxSendMsg                                               
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
Int32 IssUtils_mbxSendMsg(IssUtils_MbxHndl * pFrom,
                          IssUtils_MbxHndl * pTo,
                          UInt32 cmd, Void * pPrm, UInt32 msgFlags)
{
    IssUtils_MsgHndl *pSentMsg, *pRcvMsg;

    Bool waitAck;

    Int32 retVal = 0;

    IssUtils_QueHandle *ackQue;

    if (pTo == NULL)
        return -1;

    /* set ACK que */
    if (pFrom == NULL)
    {
        /* sender mailbox not specified by user */
        if (msgFlags & ISSUTILS_MBX_FLAG_WAIT_ACK)
        {
            /* ERROR: if sender mail box is NULL, then cannot wait for ACK */
            return -1;
        }
        ackQue = NULL;
    }
    else
    {
        /* sender mail box */
        ackQue = &pFrom->ackQue;
    }

    /* alloc message */
    pSentMsg = IssUtils_mbxAllocMsg(BIOS_WAIT_FOREVER);
    if (pSentMsg == NULL)
        return -1;

    /* set message fields */
    pSentMsg->pFrom = pFrom;
    pSentMsg->flags = msgFlags;
    pSentMsg->cmd = cmd;
    pSentMsg->result = 0;
    pSentMsg->pPrm = pPrm;

    /* send message */
    retVal = IssUtils_quePut(&pTo->recvQue, pSentMsg, BIOS_WAIT_FOREVER);

    if (retVal != 0)
        return retVal;

    if ((msgFlags & ISSUTILS_MBX_FLAG_WAIT_ACK) && ackQue != NULL)
    {
        /* need to wait for ACK */
        waitAck = TRUE;

        do
        {
            /* wait for ACK */
            retVal = IssUtils_queGet(&pFrom->ackQue,
                                     (Ptr *) & pRcvMsg, 1, BIOS_WAIT_FOREVER);
            if (retVal != 0)
                return retVal;

            if (pRcvMsg == pSentMsg)
            {
                /* ACK received for sent MSG */
                waitAck = FALSE;

                /* copy ACK status to return value */
                retVal = pRcvMsg->result;
            }                                              /* else ACK
                                                            * received for
                                                            * some other
                                                            * message */

            /* free message */
            retVal |= IssUtils_mbxFreeMsg(pRcvMsg, BIOS_WAIT_FOREVER);

        } while (waitAck);
    }

    return retVal;
}

/* ===================================================================
 *  @func     IssUtils_mbxRecvMsg                                               
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
Int32 IssUtils_mbxRecvMsg(IssUtils_MbxHndl * pMbxHndl, IssUtils_MsgHndl ** pMsg,
                          UInt32 timeout)
{
    Int32 retVal;

    /* wait for message to arrive */
    retVal = IssUtils_queGet(&pMbxHndl->recvQue, (Ptr *) pMsg, 1, timeout);

    return retVal;
}

/* ===================================================================
 *  @func     IssUtils_mbxAckOrFreeMsg                                               
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
Int32 IssUtils_mbxAckOrFreeMsg(IssUtils_MsgHndl * pMsg, Int32 ackRetVal)
{
    Int32 retVal = 0;

    if (pMsg == NULL)
        return -1;

    /* check ACK flag */
    if (pMsg->flags & ISSUTILS_MBX_FLAG_WAIT_ACK)
    {
        /* ACK flag is set */

        /* Set ACK status */
        pMsg->result = ackRetVal;

        /* Send ACK to sender */
        if (pMsg->pFrom == NULL)
        {
            retVal = -1;
            retVal |= IssUtils_mbxFreeMsg(pMsg, BIOS_WAIT_FOREVER);
            return retVal;
        }

        retVal = IssUtils_quePut(&pMsg->pFrom->ackQue, pMsg, BIOS_WAIT_FOREVER);
    }
    else
    {
        /* ACK flag is not set */

        /* free message */
        retVal = IssUtils_mbxFreeMsg(pMsg, BIOS_WAIT_FOREVER);
    }

    return retVal;
}

/* ===================================================================
 *  @func     IssUtils_mbxWaitCmd                                               
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
Int32 IssUtils_mbxWaitCmd(IssUtils_MbxHndl * pMbxHndl, IssUtils_MsgHndl ** pMsg,
                          UInt32 waitCmd)
{
    Int32 status = 0;

    IssUtils_MsgHndl *pRcvMsg;

    while (1)
    {
        /* wait for message */
        status = IssUtils_mbxRecvMsg(pMbxHndl, &pRcvMsg, BIOS_WAIT_FOREVER);
        if (status != 0)
            return status;

        /* is message command ID same as expected command ID */
        if (IssUtils_msgGetCmd(pRcvMsg) == waitCmd)
            break;                                         /* yes, exit loop */

        /* no, ACK or free received message */
        status = IssUtils_mbxAckOrFreeMsg(pRcvMsg, 0);
        if (status != 0)
            return status;
    }

    if (pMsg == NULL)
    {
        /* user does not want to examine the message, so free it here */
        status = IssUtils_mbxAckOrFreeMsg(pRcvMsg, 0);
    }
    else
    {
        /* user wants to examine the message to return it to user */
        *pMsg = pRcvMsg;
    }

    return status;
}
