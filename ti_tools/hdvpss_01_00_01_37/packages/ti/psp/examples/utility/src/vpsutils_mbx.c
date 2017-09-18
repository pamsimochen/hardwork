/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include <ti/psp/examples/utility/vpsutils_mbx.h>
#include <ti/sysbios/hal/Hwi.h>

#define VPSUTILS_MBX_MSG_POOL_MAX   (10240)

static VpsUtils_QueHandle gVpsUtils_mbxMsgPoolFreeQue;
static Ptr                gVpsUtils_mbxMsgPoolFreeQueMem[VPSUTILS_MBX_MSG_POOL_MAX];
static VpsUtils_MsgHndl   gVpsUtils_mbxMsgPool[VPSUTILS_MBX_MSG_POOL_MAX];

UInt32 VpsUtils_mbxGetFreeMsgCount()
{
    UInt32 cookie;
    UInt32 freeMsg;

    cookie = Hwi_disable();

    freeMsg = gVpsUtils_mbxMsgPoolFreeQue.count;

    Hwi_restore(cookie);

    return freeMsg;
}

VpsUtils_MsgHndl *VpsUtils_mbxAllocMsg(UInt32 timeout)
{
  VpsUtils_MsgHndl *pMsg = NULL;

  VpsUtils_queGet(&gVpsUtils_mbxMsgPoolFreeQue,
                    (Ptr*)&pMsg, 1, timeout
                  );

  return pMsg;
}

Int32 VpsUtils_mbxFreeMsg(VpsUtils_MsgHndl *pMsg, UInt32 timeout)
{
  Int32 status;

  status = VpsUtils_quePut(&gVpsUtils_mbxMsgPoolFreeQue,
                    pMsg, timeout
                  );

  return status;
}

Int32 VpsUtils_mbxInit()
{
  Int32 status, msgId;

  status = VpsUtils_queCreate(&gVpsUtils_mbxMsgPoolFreeQue,
                VPSUTILS_MBX_MSG_POOL_MAX,
                gVpsUtils_mbxMsgPoolFreeQueMem,
                VPSUTILS_QUE_FLAG_BLOCK_QUE
              );

  if(status!=0)
    return status;

  for(msgId=0; msgId<VPSUTILS_MBX_MSG_POOL_MAX; msgId++)
  {
    status = VpsUtils_mbxFreeMsg(&gVpsUtils_mbxMsgPool[msgId], BIOS_NO_WAIT);

    if(status!=0) {
      VpsUtils_mbxDeInit();
      break;
    }
  }

  return status;
}

Int32 VpsUtils_mbxDeInit()
{
  Int32 status;

  status = VpsUtils_queDelete(&gVpsUtils_mbxMsgPoolFreeQue);

  return status;
}

Int32 VpsUtils_mbxCreate(VpsUtils_MbxHndl *pMbx)
{
  Int32 status;

  /* create queues  */

  status = VpsUtils_queCreate(&pMbx->recvQue,
                VPSUTILS_MBX_RECV_QUE_LEN_MAX,
                pMbx->memRecvQue,
                VPSUTILS_QUE_FLAG_BLOCK_QUE
              );

  if(status!=0)
    return status;

  status = VpsUtils_queCreate(&pMbx->ackQue,
                VPSUTILS_MBX_ACK_QUE_LEN_MAX,
                pMbx->memAckQue,
                VPSUTILS_QUE_FLAG_BLOCK_QUE
              );

  if(status!=0) {
    VpsUtils_queDelete(&pMbx->recvQue);
  }

  return status;
}

Int32 VpsUtils_mbxDelete(VpsUtils_MbxHndl *pMbx)
{
  Int32 status=0;

  /* delete queues  */

  status |= VpsUtils_queDelete(&pMbx->recvQue);
  status |= VpsUtils_queDelete(&pMbx->ackQue);

  return status;
}

Int32 VpsUtils_mbxSendCmd(VpsUtils_MbxHndl * pTo, UInt32 cmd)
{
  VpsUtils_MsgHndl *pSentMsg;
  Int32 retVal=0;

  if(pTo==NULL)
    return -1;

  /* alloc message  */
  pSentMsg=VpsUtils_mbxAllocMsg(BIOS_NO_WAIT);
  if(pSentMsg==NULL)
    return -1;

  /* set message fields */
  pSentMsg->pFrom = NULL;
  pSentMsg->flags = 0;
  pSentMsg->cmd = cmd;
  pSentMsg->result = 0;
  pSentMsg->pPrm = NULL;

  /* send message */
  retVal = VpsUtils_quePut(&pTo->recvQue, pSentMsg, BIOS_NO_WAIT);

  if(retVal!=0)
  {
      retVal |= VpsUtils_mbxFreeMsg(pSentMsg, BIOS_NO_WAIT);
  }

  return retVal;
}

Int32 VpsUtils_mbxSendMsg(VpsUtils_MbxHndl * pFrom,
                 VpsUtils_MbxHndl * pTo,
                 UInt32 cmd,
                 Void * pPrm,
                 UInt32 msgFlags)
{
  VpsUtils_MsgHndl *pSentMsg, *pRcvMsg;
  Bool waitAck;
  Int32 retVal=0;
  VpsUtils_QueHandle *ackQue;

  if(pTo==NULL)
    return -1;

  /* set ACK que  */
  if(pFrom==NULL)
  {
    /* sender mailbox not specified by user */
    if(msgFlags & VPSUTILS_MBX_FLAG_WAIT_ACK)
    {
      /* ERROR: if sender mail box is NULL, then cannot wait for ACK */
      return -1;
    }
    ackQue = NULL;
  }
  else
  {
    /* sender mail box  */
    ackQue = &pFrom->ackQue;
  }

  /* alloc message  */
  pSentMsg=VpsUtils_mbxAllocMsg(BIOS_WAIT_FOREVER);
  if(pSentMsg==NULL)
    return -1;

  /* set message fields */
  pSentMsg->pFrom = pFrom;
  pSentMsg->flags = msgFlags;
  pSentMsg->cmd = cmd;
  pSentMsg->result = 0;
  pSentMsg->pPrm = pPrm;

  /* send message */
  retVal = VpsUtils_quePut(&pTo->recvQue, pSentMsg, BIOS_WAIT_FOREVER);

  if(retVal!=0)
    return retVal;

  if( (msgFlags & VPSUTILS_MBX_FLAG_WAIT_ACK) && ackQue != NULL )
  {
    /* need to wait for ACK */
    waitAck = TRUE;

    do
    {
      /* wait for ACK */
      retVal = VpsUtils_queGet(&pFrom->ackQue,
                    (Ptr*)&pRcvMsg, 1, BIOS_WAIT_FOREVER
                  );
      if(retVal!=0)
        return retVal;

      if(pRcvMsg==pSentMsg)
      {
        /* ACK received for sent MSG  */
        waitAck = FALSE;

        /* copy ACK status to return value  */
        retVal  = pRcvMsg->result;
      } /* else ACK received for some other message */

      /* free message */
      retVal |= VpsUtils_mbxFreeMsg(pRcvMsg, BIOS_WAIT_FOREVER);

    } while(waitAck);
  }

  return retVal;
}

Int32 VpsUtils_mbxRecvMsg(VpsUtils_MbxHndl *pMbxHndl, VpsUtils_MsgHndl **pMsg, UInt32 timeout)
{
  Int32 retVal;

  /* wait for message to arrive */
  retVal = VpsUtils_queGet(&pMbxHndl->recvQue, (Ptr*)pMsg, 1, timeout);

  return retVal;
}

Int32 VpsUtils_mbxAckOrFreeMsg(VpsUtils_MsgHndl *pMsg, Int32 ackRetVal)
{
  Int32 retVal=0;

  if(pMsg==NULL)
    return -1;

  /* check ACK flag */
  if(pMsg->flags & VPSUTILS_MBX_FLAG_WAIT_ACK)
  {
    /* ACK flag is set */

    /* Set ACK status */
    pMsg->result = ackRetVal;

    /* Send ACK to sender */
    if(pMsg->pFrom==NULL) {
      retVal = -1;
      retVal |= VpsUtils_mbxFreeMsg(pMsg, BIOS_WAIT_FOREVER);
      return retVal;
    }

    retVal = VpsUtils_quePut(&pMsg->pFrom->ackQue, pMsg, BIOS_WAIT_FOREVER);
  }
  else
  {
    /* ACK flag is not set  */

    /* free message */
    retVal = VpsUtils_mbxFreeMsg(pMsg, BIOS_WAIT_FOREVER);
  }

  return retVal;
}

Int32 VpsUtils_mbxWaitCmd(VpsUtils_MbxHndl *pMbxHndl, VpsUtils_MsgHndl **pMsg, UInt32 waitCmd)
{
  Int32 status=0;
  VpsUtils_MsgHndl *pRcvMsg;

  while(1)
  {
    /* wait for message */
    status = VpsUtils_mbxRecvMsg(pMbxHndl, &pRcvMsg, BIOS_WAIT_FOREVER);
    if(status!=0)
      return status;

    /* is message command ID same as expected command ID  */
    if(VpsUtils_msgGetCmd(pRcvMsg)==waitCmd)
      break; /* yes, exit loop  */

    /* no, ACK or free received message  */
    status = VpsUtils_mbxAckOrFreeMsg(pRcvMsg, 0);
    if(status!=0)
      return status;
  }

  if(pMsg==NULL)
  {
    /* user does not want to examine the message, so free it here */
    status = VpsUtils_mbxAckOrFreeMsg(pRcvMsg, 0);
  }
  else
  {
    /* user wants to examine the message to return it to user */
    *pMsg = pRcvMsg;
  }

  return status;
}
