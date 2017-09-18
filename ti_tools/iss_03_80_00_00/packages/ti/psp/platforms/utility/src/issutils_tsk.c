/** ==================================================================
 *  @file   issutils_tsk.c                                                  
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

#include <ti/psp/iss/common/trace.h>
#include <ti/psp/platforms/utility/issutils_tsk.h>
#include <ti/psp/platforms/utility/issutils_prf.h>

#define ISSUTILS_TSK_CMD_EXIT   (0xFFFFFFFF)

/* ===================================================================
 *  @func     IssUtils_tskMain                                               
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
Void IssUtils_tskMain(UArg arg0, UArg arg1)
{
    IssUtils_TskHndl *pHndl = (IssUtils_TskHndl *) arg0;

    IssUtils_MsgHndl *pMsg;

    Int32 status;

    UInt32 cmd;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);

    while (1)
    {
        status = IssUtils_mbxRecvMsg(&pHndl->mbx, &pMsg, BIOS_WAIT_FOREVER);
        if (status != FVID2_SOK)
            break;

        cmd = IssUtils_msgGetCmd(pMsg);
        if (cmd == ISSUTILS_TSK_CMD_EXIT)
        {
            IssUtils_tskAckOrFreeMsg(pMsg, FVID2_SOK);
            break;
        }

        if (pHndl->funcMain)
            pHndl->funcMain(pHndl, pMsg);
    }
}

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
                         UInt32 stackSize, Ptr appData, char *tskName)
{
    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);
    GT_assert(GT_DEFAULT_MASK, funcMain != NULL);

    pHndl->funcMain = funcMain;
    pHndl->stackSize = stackSize;
    pHndl->stackAddr = stackAddr;
    pHndl->tskPri = tskPri;
    pHndl->appData = appData;

    status = IssUtils_mbxCreate(&pHndl->mbx);

    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    Task_Params_init(&pHndl->tskParams);

    pHndl->tskParams.arg0 = (UArg) pHndl;
    pHndl->tskParams.arg1 = (UArg) pHndl;
    pHndl->tskParams.priority = pHndl->tskPri;
    pHndl->tskParams.stack = pHndl->stackAddr;
    pHndl->tskParams.stackSize = pHndl->stackSize;

    pHndl->tsk = Task_create(IssUtils_tskMain, &pHndl->tskParams, NULL);

    GT_assert(GT_DEFAULT_MASK, pHndl->tsk != NULL);

    IssUtils_prfLoadRegister(pHndl->tsk, tskName);

    return status;
}

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
Int32 IssUtils_tskDelete(IssUtils_TskHndl * pHndl)
{
    UInt32 sleepTime = 8;                                  /* in OS ticks */

    IssUtils_tskSendCmd(pHndl, ISSUTILS_TSK_CMD_EXIT);

    /* wait for command to be received and task to be exited */

    Task_sleep(1);

    while (Task_Mode_TERMINATED != Task_getMode(pHndl->tsk))
    {

        Task_sleep(sleepTime);

        sleepTime >>= 1;

        if (sleepTime == 0)
        {
            GT_assert(GT_DEFAULT_MASK, 0);
        }
    }

    IssUtils_prfLoadUnRegister(pHndl->tsk);

    Task_delete(&pHndl->tsk);
    IssUtils_mbxDelete(&pHndl->mbx);

    return FVID2_SOK;
}
