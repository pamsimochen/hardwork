/** ==================================================================
 *  @file   system_linkApi.c                                                  
 *                                                                    
 *  @path   /ti/psp/examples/common/iss/chains/links/system/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/

#include <ti/psp/examples/common/iss/chains/links/system/system_priv.h>

/* ===================================================================
 *  @func     System_linkCreate                                               
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
Int32 System_linkCreate(UInt32 linkId, Ptr createArgs)
{
    return System_linkControl(linkId, SYSTEM_CMD_CREATE, createArgs, TRUE);
}

/* ===================================================================
 *  @func     System_linkStart                                               
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
Int32 System_linkStart(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_START, NULL, TRUE);
}

/* ===================================================================
 *  @func     System_linkStop                                               
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
Int32 System_linkStop(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_STOP, NULL, TRUE);
}

/* ===================================================================
 *  @func     System_linkDelete                                               
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
Int32 System_linkDelete(UInt32 linkId)
{
    return System_linkControl(linkId, SYSTEM_CMD_DELETE, NULL, TRUE);
}

/* ===================================================================
 *  @func     System_linkControl                                               
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
Int32 System_linkControl(UInt32 linkId, UInt32 cmd, Void * pPrm, Bool waitAck)
{
    Int32 status;

    VpsUtils_MbxHndl *pToMbx;

    UInt32 flags = 0;

    GT_assert(GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pToMbx = &gSystem_obj.linkObj[linkId].pTsk->mbx;

    if (waitAck)
        flags = VPSUTILS_MBX_FLAG_WAIT_ACK;

    status = VpsUtils_mbxSendMsg(&gSystem_obj.mbx, pToMbx, cmd, pPrm, flags);

    return status;
}

/* ===================================================================
 *  @func     System_sendLinkCmd                                               
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
Int32 System_sendLinkCmd(UInt32 linkId, UInt32 cmd)
{
    VpsUtils_MbxHndl *pToMbx;

    GT_assert(GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pToMbx = &gSystem_obj.linkObj[linkId].pTsk->mbx;

    return VpsUtils_mbxSendCmd(pToMbx, cmd);
}

/* ===================================================================
 *  @func     System_getLinkInfo                                               
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
System_LinkInfo *System_getLinkInfo(UInt32 linkId)
{
    System_LinkObj *pTsk;

    GT_assert(GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_obj.linkObj[linkId];

    if (pTsk->getLinkInfo != NULL)
        return pTsk->getLinkInfo(pTsk->pTsk);

    return NULL;
}

/* ===================================================================
 *  @func     System_getLinksFullFrames                                               
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
Int32 System_getLinksFullFrames(UInt32 linkId, UInt16 queId,
                                FVID2_FrameList * pFrameList)
{
    System_LinkObj *pTsk;

    GT_assert(GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_obj.linkObj[linkId];

    if (pTsk->linkGetFullFrames != NULL)
        return pTsk->linkGetFullFrames(pTsk->pTsk, queId, pFrameList);

    return FVID2_EFAIL;
}

/* ===================================================================
 *  @func     System_putLinksEmptyFrames                                               
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
Int32 System_putLinksEmptyFrames(UInt32 linkId, UInt16 queId,
                                 FVID2_FrameList * pFrameList)
{
    System_LinkObj *pTsk;

    GT_assert(GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    pTsk = &gSystem_obj.linkObj[linkId];

    if (pTsk->linkPutEmptyFrames != NULL)
        return pTsk->linkPutEmptyFrames(pTsk->pTsk, queId, pFrameList);

    return FVID2_EFAIL;
}

/* ===================================================================
 *  @func     System_registerLink                                               
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
Int32 System_registerLink(UInt32 linkId, System_LinkObj * pTskObj)
{
    GT_assert(GT_DEFAULT_MASK, linkId < SYSTEM_LINK_ID_MAX);

    memcpy(&gSystem_obj.linkObj[linkId], pTskObj, sizeof(*pTskObj));

    return FVID2_SOK;
}
