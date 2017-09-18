/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/examples/common/vps/chains/links/system/system_priv.h>

Int32 System_linkCreate(UInt32 linkId, Ptr createArgs)
{
    return System_linkControl(linkId, SYSTEM_CMD_CREATE, createArgs, TRUE);
}

Int32 System_linkStart(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_START, NULL, TRUE);
}

Int32 System_linkStop(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_STOP, NULL, TRUE);
}

Int32 System_linkDelete(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_DELETE, NULL, TRUE);
}

Int32 System_linkControl(UInt32 linkId, UInt32 cmd, Void *pPrm, Bool waitAck)
{
    Int32 status;
    VpsUtils_MbxHndl *pToMbx;
    UInt32 flags=0;

    GT_assert( GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pToMbx = &gSystem_obj.linkObj[linkId].pTsk->mbx;

    if(waitAck)
        flags = VPSUTILS_MBX_FLAG_WAIT_ACK;

    status = VpsUtils_mbxSendMsg(&gSystem_obj.mbx, pToMbx, cmd, pPrm, flags);

    return status;
}

Int32 System_sendLinkCmd(UInt32 linkId, UInt32 cmd)
{
    VpsUtils_MbxHndl *pToMbx;

    GT_assert( GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pToMbx = &gSystem_obj.linkObj[linkId].pTsk->mbx;

    return VpsUtils_mbxSendCmd(pToMbx, cmd);
}

System_LinkInfo * System_getLinkInfo(UInt32 linkId)
{
    System_LinkObj *pTsk;

    GT_assert( GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_obj.linkObj[linkId];

    if(pTsk->getLinkInfo!=NULL)
        return pTsk->getLinkInfo(pTsk->pTsk);

    return NULL;
}

Int32 System_getLinksFullFrames(UInt32 linkId, UInt16 queId, FVID2_FrameList *pFrameList)
{
    System_LinkObj *pTsk;

    GT_assert( GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_obj.linkObj[linkId];

    if(pTsk->linkGetFullFrames!=NULL)
        return pTsk->linkGetFullFrames(pTsk->pTsk, queId, pFrameList);

    return FVID2_EFAIL;
}

Int32 System_putLinksEmptyFrames(UInt32 linkId, UInt16 queId, FVID2_FrameList *pFrameList)
{
    System_LinkObj *pTsk;

    GT_assert( GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_obj.linkObj[linkId];

    if(pTsk->linkPutEmptyFrames!=NULL)
        return pTsk->linkPutEmptyFrames(pTsk->pTsk, queId, pFrameList);

    return FVID2_EFAIL;
}


Int32 System_registerLink(UInt32 linkId, System_LinkObj *pTskObj)
{
    GT_assert( GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    memcpy(&gSystem_obj.linkObj[linkId], pTskObj, sizeof(*pTskObj));

    return FVID2_SOK;
}

