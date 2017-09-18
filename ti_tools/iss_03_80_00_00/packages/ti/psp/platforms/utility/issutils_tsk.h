/** ==================================================================
 *  @file   issutils_tsk.h                                                  
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
    \defgroup ISSUTILS_TSK_API Task wrapper APIs

    APIs in this file couple a BIOS Task with a mailbox in order to allow
    application to implement a state machine kind of logic, where in state
    change happens based on received message's

    @{
*/

/**
    \file issutils_tsk.h
    \brief Task wrapper API
*/

#ifndef _ISSUTILS_TSK_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#define _ISSUTILS_TSK_H_

#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/platforms/utility/issutils_mbx.h>
#include <ti/psp/iss/iss.h>

struct IssUtils_TskHndl;

/**
    \brief Task main function

    This function is called when a message is received by the
    task.

    \param pHndl [OUT] Task handle
    \param pMsg  [OUT] Received message

*/
typedef Void(*IssUtils_TskFuncMain) (struct IssUtils_TskHndl * pHndl,
                                     IssUtils_MsgHndl * pMsg);

/**
    \brief Task handle
*/
typedef struct IssUtils_TskHndl {
    Task_Handle tsk;
    /**< BIOS Task handle */

    Task_Params tskParams;
    /**< BIOS Task create params */

    IssUtils_MbxHndl mbx;
    /**< Mail box associated with this task */

    UInt8 *stackAddr;
    /**< Task stack address */

    UInt32 stackSize;
    /**< Task stack size */

    UInt32 tskPri;
    /**< Task priority as defined by BIOS */

    IssUtils_TskFuncMain funcMain;
    /**< Task main,

        Note, this is different from BIOS Task, since this function
        is entered ONLY when a message is received.
    */

    Ptr appData;
    /**< Application specific data */
} IssUtils_TskHndl;

/**
    \brief Create a task

    \param pHndl        [OUT] Task handle
    \param funcMain     [IN]  Task main,
                              Note, this is different from BIOS Task, since
                              this function
                              is entered ONLY when a message is received.
    \param tskPri       [IN]  Task priority as defined by BIOS
    \param stackAddr    [IN]  Task stack address
    \param stackSize    [IN]  Task stack size
    \param appData      [IN]  Application specific data
    \param tskName      [IN]  Task name

    \return FVID2_SOK on success else failure
*/
/* ===================================================================
 *  @func     IssUtils_tskCreate                                               
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
Int32 IssUtils_tskCreate(IssUtils_TskHndl * pHndl,
                         IssUtils_TskFuncMain funcMain,
                         UInt32 tskPri,
                         UInt8 * stackAddr,
                         UInt32 stackSize, Ptr appData, char *tskName);

/**
    \brief Delete a task

    \param pHndl        [OUT] Task handle

    \return FVID2_SOK on success else failure
*/
/* ===================================================================
 *  @func     IssUtils_tskDelete                                               
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
Int32 IssUtils_tskDelete(IssUtils_TskHndl * pHndl);

/**
    \brief Send message from one task to another task

    Refer to IssUtils_mbxSendMsg() for details
*/
/* ===================================================================
 *  @func     IssUtils_tskSendMsg                                               
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
static inline Int32 IssUtils_tskSendMsg(IssUtils_TskHndl * pFrom,
                                        IssUtils_TskHndl * pTo,
                                        UInt32 cmd, Void * pPrm,
                                        UInt32 msgFlags)
{
    return IssUtils_mbxSendMsg(&pFrom->mbx, &pTo->mbx, cmd, pPrm, msgFlags);
}

/**
    \brief Send 32-bit command to another task

    Refer to IssUtils_mbxSendCmd() for details
*/
/* ===================================================================
 *  @func     IssUtils_tskSendCmd                                               
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
static inline Int32 IssUtils_tskSendCmd(IssUtils_TskHndl * pTo, UInt32 cmd)
{
    return IssUtils_mbxSendCmd(&pTo->mbx, cmd);
}

/**
    \brief Wait for a message to arrive

    Refer to IssUtils_mbxRecvMsg() for details
*/
/* ===================================================================
 *  @func     IssUtils_tskRecvMsg                                               
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
static inline Int32 IssUtils_tskRecvMsg(IssUtils_TskHndl * pHndl,
                                        IssUtils_MsgHndl ** pMsg,
                                        UInt32 timeout)
{
    return IssUtils_mbxRecvMsg(&pHndl->mbx, pMsg, timeout);
}

/**
    \brief ACK or free received message

    Refer to IssUtils_mbxAckOrFreeMsg() for details
*/
/* ===================================================================
 *  @func     IssUtils_tskAckOrFreeMsg                                               
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
static inline Int32 IssUtils_tskAckOrFreeMsg(IssUtils_MsgHndl * pMsg,
                                             Int32 result)
{
    return IssUtils_mbxAckOrFreeMsg(pMsg, result);
}

/**
    \brief Wait until user specified command is received

    Refer to IssUtils_mbxWaitCmd() for details
*/
/* ===================================================================
 *  @func     IssUtils_tskWaitCmd                                               
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
static inline Int32 IssUtils_tskWaitCmd(IssUtils_TskHndl * pHndl,
                                        IssUtils_MsgHndl ** pMsg,
                                        UInt32 cmdToWait)
{
    return IssUtils_mbxWaitCmd(&pHndl->mbx, pMsg, cmdToWait);
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif

/* @} */
