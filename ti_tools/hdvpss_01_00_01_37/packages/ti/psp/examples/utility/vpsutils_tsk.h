/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
    \ingroup VPSUTILS_API
    \defgroup VPSUTILS_TSK_API Task wrapper APIs

    APIs in this file couple a BIOS Task with a mailbox in order to allow
    application to implement a state machine kind of logic, where in state
    change happens based on received message's

    @{
*/

/**
    \file vpsutils_tsk.h
    \brief Task wrapper API
*/

#ifndef _VPSUTILS_TSK_H_
#define _VPSUTILS_TSK_H_


#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/psp/examples/utility/vpsutils_mbx.h>
#include <ti/psp/vps/vps.h>

struct VpsUtils_TskHndl;

/**
    \brief Task main function

    This function is called when a message is received by the
    task.

    \param pHndl [OUT] Task handle
    \param pMsg  [OUT] Received message

*/
typedef Void (*VpsUtils_TskFuncMain)(
                            struct VpsUtils_TskHndl *pHndl,
                            VpsUtils_MsgHndl *pMsg);


/**
    \brief Task handle
*/
typedef struct VpsUtils_TskHndl
{
    Task_Handle tsk;
    /**< BIOS Task handle */

    Task_Params tskParams;
    /**< BIOS Task create params */

    VpsUtils_MbxHndl mbx;
    /**< Mail box associated with this task */

    UInt8 *stackAddr;
    /**< Task stack address */

    UInt32 stackSize;
    /**< Task stack size */

    UInt32 tskPri;
    /**< Task priority as defined by BIOS */

    VpsUtils_TskFuncMain funcMain;
    /**< Task main,

        Note, this is different from BIOS Task, since this function
        is entered ONLY when a message is received.
    */

    Ptr appData;
    /**< Application specific data */
} VpsUtils_TskHndl;


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
Int32 VpsUtils_tskCreate(
                    VpsUtils_TskHndl *pHndl,
                    VpsUtils_TskFuncMain funcMain,
                    UInt32 tskPri,
                    UInt8 *stackAddr,
                    UInt32 stackSize,
                    Ptr appData,
                    char *tskName
                    );

/**
    \brief Delete a task

    \param pHndl        [OUT] Task handle

    \return FVID2_SOK on success else failure
*/
Int32 VpsUtils_tskDelete(VpsUtils_TskHndl *pHndl);

/**
    \brief Send message from one task to another task

    Refer to VpsUtils_mbxSendMsg() for details
*/
static inline Int32 VpsUtils_tskSendMsg(
                        VpsUtils_TskHndl *pFrom,
                        VpsUtils_TskHndl *pTo,
                        UInt32 cmd, Void *pPrm,
                        UInt32 msgFlags)
{
    return VpsUtils_mbxSendMsg(&pFrom->mbx, &pTo->mbx, cmd, pPrm, msgFlags);
}

/**
    \brief Send 32-bit command to another task

    Refer to VpsUtils_mbxSendCmd() for details
*/
static inline Int32 VpsUtils_tskSendCmd(
                        VpsUtils_TskHndl * pTo,
                        UInt32 cmd)
{
    return VpsUtils_mbxSendCmd(&pTo->mbx, cmd);
}

/**
    \brief Wait for a message to arrive

    Refer to VpsUtils_mbxRecvMsg() for details
*/
static inline Int32 VpsUtils_tskRecvMsg(
                        VpsUtils_TskHndl *pHndl,
                        VpsUtils_MsgHndl **pMsg,
                        UInt32 timeout)
{
    return VpsUtils_mbxRecvMsg(&pHndl->mbx, pMsg, timeout);
}

/**
    \brief ACK or free received message

    Refer to VpsUtils_mbxAckOrFreeMsg() for details
*/
static inline Int32 VpsUtils_tskAckOrFreeMsg(VpsUtils_MsgHndl *pMsg, Int32 result)
{
    return VpsUtils_mbxAckOrFreeMsg(pMsg, result);
}

/**
    \brief Wait until user specified command is received

    Refer to VpsUtils_mbxWaitCmd() for details
*/
static inline Int32 VpsUtils_tskWaitCmd(
                        VpsUtils_TskHndl *pHndl,
                        VpsUtils_MsgHndl **pMsg,
                        UInt32 cmdToWait)
{
    return VpsUtils_mbxWaitCmd(&pHndl->mbx, pMsg, cmdToWait);
}

#endif

/*@}*/
