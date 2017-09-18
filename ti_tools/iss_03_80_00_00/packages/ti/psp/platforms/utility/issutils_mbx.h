/** ==================================================================
 *  @file   issutils_mbx.h                                                  
 *                                                                    
 *  @path   /ti/psp/platforms/utility/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

/**
    \ingroup ISSUTILS_API
    \defgroup ISSUTILS_MBX_API Message exchange API

    APIs from this file are used to exchange messages between two tasks in the
    links and chains examples.

    A message consists of a 32-bit command and optional 32-bit parameter value.

    The 32-bit command is defined by the user.

    The 32-bit parameter could inturn point to a bigger data structure as defined
    by user.

    The APIs allow a user to send a message and wait for ACK before proceeding
    further.

    Internally message passing is implemented using queue's

    @{
*/

/**
    \file issutils_mbx.h
    \brief Message exchange API
*/

#ifndef _ISSUTILS_MBX_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSUTILS_MBX_H_

#include <stdlib.h>
#include <ti/psp/iss/common/issutils_que.h>

/** \brief Maximum receive que length */
#define ISSUTILS_MBX_RECV_QUE_LEN_MAX (1024)

/** \brief Maximum acknowledgement que length */
#define ISSUTILS_MBX_ACK_QUE_LEN_MAX  (2)

/** \brief Message flag: wait for ACK when this flag is set */
#define ISSUTILS_MBX_FLAG_WAIT_ACK    (0x1)

/** \brief Get 32-bit command from message pointer */
#define IssUtils_msgGetCmd(pMsg)      ((pMsg)->cmd)

/** \brief Get 32-bit parameter pointer from message pointer */
#define IssUtils_msgGetPrm(pMsg)      ((pMsg)->pPrm)

/**
  \brief Mailbox handle
*/
typedef struct {
    IssUtils_QueHandle recvQue;
  /**< message receive queue  */

    IssUtils_QueHandle ackQue;
  /**< ack message receive queue  */

    Ptr memRecvQue[ISSUTILS_MBX_RECV_QUE_LEN_MAX];
  /**< memory for receive queue */

    Ptr memAckQue[ISSUTILS_MBX_ACK_QUE_LEN_MAX];
  /**< memory for ack queue */

} IssUtils_MbxHndl;

/**
  \brief Message structure
*/
typedef struct {
    IssUtils_MbxHndl *pFrom;
  /**< sender mailbox */

    Int32 result;
  /**< result to be sent as part of ACK */

    Void *pPrm;
  /**< parameters sent by sender  */

    UInt32 cmd;
  /**< command sent by sender */

    UInt32 flags;
  /**< message flags set by sender */

} IssUtils_MsgHndl;

/**
  \brief One-time system init for mailbox subsystem
*/
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
Int32 IssUtils_mbxInit();

/**
  \brief Free's resources allocated during IssUtils_mbxInit()
*/
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
Int32 IssUtils_mbxDeInit();

/**
  \brief Create's a message box and related resources

  \param pHandle    [OUT] Created handle

  \return FVID2_SOK on success else failure
*/
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
Int32 IssUtils_mbxCreate(IssUtils_MbxHndl * pHandle);

/**
  \brief Free's resources allocated during IssUtils_mbxCreate()

  \param pHandle    [IN] Mail box handle

  \return FVID2_SOK on success else failure
*/
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
Int32 IssUtils_mbxDelete(IssUtils_MbxHndl * pHandle);

/**
  \brief Send message from one mailbox to another mailbox

  When 'msgFlags' is ISSUTILS_MBX_FLAG_WAIT_ACK,
    the function waits until an ack is received

  When 'msgFlags' is 0,
    the function returns after message is sent to the receiver

  User can use 'pPrm' to pass a parameter pointer to the receiver.
  Its upto user to manage the memory for this parameter pointer.

  'cmd' can be any 32-bit value that is sent to the receiver

  When ISSUTILS_MBX_FLAG_WAIT_ACK is set,
    return value is the value sent via 'result' by the
    receiver when the receiver calls IssUtils_mbxAckOrFreeMsg()

  When ISSUTILS_MBX_FLAG_WAIT_ACK is not set,
    return value is message send status

  \param pFrom    [IN] Sender Mail box handle
  \param pTo      [IN] Receiver mail box handle
  \param cmd      [IN] 32-bit command
  \param pPrm     [IN] 32-bit parameter pointer
  \param msgFlags [IN] ISSUTILS_MBX_FLAG_xxxx

  \return FVID2_SOK on success else failure

*/
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
                          UInt32 cmd, Void * pPrm, Uint32 msgFlags);

/**
  \brief Send command to another mail box

  Same as IssUtils_mbxSendMsg() except,
  - it can be called from interrupt context as well as task, SWI context
  - ACK message cannot received

  \param pTo      [IN] Receiver mail box handle
  \param cmd      [IN] Command to be sent

  \return FVID2_EFAIL in case message could not be sent
*/
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
Int32 IssUtils_mbxSendCmd(IssUtils_MbxHndl * pTo, UInt32 cmd);

/**
  \brief Waits for a message to arrive

  When 'timeout' is BIOS_WAIT_FOREVER, it waits until atleast one message arrives

  When 'timeout' is BIOS_NO_WAIT, it just checks for any available message.
    if no message is received then it return's with error

  User MUST call IssUtils_mbxAckOrFreeMsg() for every received message,
  else message will not be free'ed and there will be memory leak.

  \param pMbx       [IN] Receiver mail box
  \param pMsg       [OUT] received message
  \param timeout    [IN] BIOS_WAIT_FOREVER or BIOS_NO_WAIT

  \return FVID2_SOK on success else failure
*/
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
Int32 IssUtils_mbxRecvMsg(IssUtils_MbxHndl * pMbx, IssUtils_MsgHndl ** pMsg,
                          UInt32 timeout);

/**
  \brief Acks or frees a message depending on flags set in the message

  if ISSUTILS_MBX_FLAG_WAIT_ACK is set,
    then an ack message is sent to the sender

  if ISSUTILS_MBX_FLAG_WAIT_ACK is not set,
    then it frees the memory associated with this message

  User MUST call this API for every message received using
    IssUtils_mbxRecvMsg() or IssUtils_mbxWaitCmd()
  else message will not be free'ed and there will be memory leak.

  \param pMsg       [IN] Message to ACK'ed or free'ed
  \param result     [IN] return code that is sent to the sender if an ack message
                         is sent to the sender

  \return FVID2_SOK on success else failure
*/
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
Int32 IssUtils_mbxAckOrFreeMsg(IssUtils_MsgHndl * pMsg, Int32 result);

/**
  \brief Waits until command of value 'cmdToWait' is received

  if 'pMsg' is NULL it frees the msg internally when it is received and returns

  if 'pMsg' is NOT NULL then it returns the received message to user. User
    needs to free the received message using IssUtils_mbxAckOrFreeMsg()

  \param pMbx       [IN] Receiver mail box
  \param pMsg       [OUT] received message
  \param cmdToWait  [IN] command to wait for

  \return FVID2_SOK on success else failure
*/
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
Int32 IssUtils_mbxWaitCmd(IssUtils_MbxHndl * pMbx, IssUtils_MsgHndl ** pMsg,
                          UInt32 cmdToWait);

/**
  \brief Returns the number of free messages in the mailbox.

  \return Number of free messages in the mailbox on success.
*/
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
UInt32 IssUtils_mbxGetFreeMsgCount();

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
