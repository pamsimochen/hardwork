/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include <ti/psp/vps/common/trace.h>
#include <ti/psp/examples/utility/vpsutils_tsk.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>

#define VPSUTILS_TSK_CMD_EXIT   (0xFFFFFFFF)

Void VpsUtils_tskMain ( UArg arg0, UArg arg1 )
{
    VpsUtils_TskHndl *pHndl = (VpsUtils_TskHndl *)arg0;
    VpsUtils_MsgHndl *pMsg;
    Int32 status;
    UInt32 cmd;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);

    while(1)
    {
        status = VpsUtils_mbxRecvMsg(&pHndl->mbx, &pMsg, BIOS_WAIT_FOREVER);
        if(status!=FVID2_SOK)
            break;

        cmd = VpsUtils_msgGetCmd(pMsg);
        if(cmd==VPSUTILS_TSK_CMD_EXIT)
        {
            VpsUtils_tskAckOrFreeMsg(pMsg, FVID2_SOK);
            break;
        }

        if(pHndl->funcMain)
            pHndl->funcMain(pHndl, pMsg);
    }
}

Int32 VpsUtils_tskCreate(
                    VpsUtils_TskHndl *pHndl,
                    VpsUtils_TskFuncMain funcMain,
                    UInt32 tskPri,
                    UInt8 *stackAddr,
                    UInt32 stackSize,
                    Ptr appData,
                    char *tskName
                    )
{
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);
    GT_assert( GT_DEFAULT_MASK, funcMain!=NULL);

    pHndl->funcMain = funcMain;
    pHndl->stackSize = stackSize;
    pHndl->stackAddr = stackAddr;
    pHndl->tskPri = tskPri;
    pHndl->appData = appData;

    status = VpsUtils_mbxCreate(&pHndl->mbx);

    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    Task_Params_init ( &pHndl->tskParams );

    pHndl->tskParams.arg0 = (UArg)pHndl;
    pHndl->tskParams.arg1 = (UArg)pHndl;
    pHndl->tskParams.priority = pHndl->tskPri;
    pHndl->tskParams.stack = pHndl->stackAddr;
    pHndl->tskParams.stackSize = pHndl->stackSize;

    pHndl->tsk = Task_create ( VpsUtils_tskMain, &pHndl->tskParams, NULL );

    GT_assert( GT_DEFAULT_MASK, pHndl->tsk!=NULL);

    VpsUtils_prfLoadRegister(pHndl->tsk, tskName);

    return status;
}

Int32 VpsUtils_tskDelete(VpsUtils_TskHndl *pHndl)
{
    UInt32 sleepTime = 8; /* in OS ticks */

    VpsUtils_tskSendCmd(pHndl, VPSUTILS_TSK_CMD_EXIT);

    /* wait for command to be received
       and task to be exited */

    Task_sleep(1);

    while ( Task_Mode_TERMINATED != Task_getMode ( pHndl->tsk) ) {

      Task_sleep ( sleepTime );

      sleepTime >>= 1;

      if ( sleepTime == 0 ) {
        GT_assert( GT_DEFAULT_MASK, 0);
      }
    }

    VpsUtils_prfLoadUnRegister(pHndl->tsk);

    Task_delete ( &pHndl->tsk);
    VpsUtils_mbxDelete(&pHndl->mbx);

    return FVID2_SOK;
}
