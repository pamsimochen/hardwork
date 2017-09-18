/** ==================================================================
 *  @file   issutils_buf.c                                                  
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
#include <ti/psp/platforms/utility/issutils_buf.h>

/* ===================================================================
 *  @func     IssUtils_bufCreate                                               
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
Int32 IssUtils_bufCreate(IssUtils_BufHndl * pHndl, Bool blockOnGet,
                         Bool blockOnPut)
{
    Int32 status;

    UInt32 flags;

    flags = ISSUTILS_QUE_FLAG_NO_BLOCK_QUE;

    if (blockOnGet)
        flags |= ISSUTILS_QUE_FLAG_BLOCK_QUE_GET;
    if (blockOnPut)
        flags |= ISSUTILS_QUE_FLAG_BLOCK_QUE_PUT;

    status = IssUtils_queCreate(&pHndl->emptyQue,
                                ISSUTILS_BUF_MAX_QUE_SIZE,
                                pHndl->emptyQueMem, flags);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    status = IssUtils_queCreate(&pHndl->fullQue,
                                ISSUTILS_BUF_MAX_QUE_SIZE,
                                pHndl->fullQueMem, flags);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_bufDelete                                               
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
Int32 IssUtils_bufDelete(IssUtils_BufHndl * pHndl)
{
    IssUtils_queDelete(&pHndl->emptyQue);
    IssUtils_queDelete(&pHndl->fullQue);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IssUtils_bufGetEmpty                                               
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
Int32 IssUtils_bufGetEmpty(IssUtils_BufHndl * pHndl,
                           FVID2_FrameList * pFrameList, UInt32 timeout)
{
    UInt32 idx, maxFrames;

    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);
    GT_assert(GT_DEFAULT_MASK, pFrameList != NULL);

    if (timeout == BIOS_NO_WAIT)
        maxFrames = FVID2_MAX_FVID_FRAME_PTR;
    else
        maxFrames = pFrameList->numFrames;

    GT_assert(GT_DEFAULT_MASK, maxFrames <= FVID2_MAX_FVID_FRAME_PTR);

    for (idx = 0; idx < maxFrames; idx++)
    {
        status =
            IssUtils_queGet(&pHndl->emptyQue, (Ptr *) & pFrameList->frames[idx],
                            1, timeout);
        if (status != FVID2_SOK)
            break;
    }

    pFrameList->numFrames = idx;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IssUtils_bufGetEmptyFrame                                               
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
Int32 IssUtils_bufGetEmptyFrame(IssUtils_BufHndl * pHndl,
                                FVID2_Frame ** pFrame, UInt32 timeout)
{
    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);
    GT_assert(GT_DEFAULT_MASK, pFrame != NULL);

    *pFrame = NULL;

    status = IssUtils_queGet(&pHndl->emptyQue, (Ptr *) pFrame, 1, timeout);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_bufPutEmpty                                               
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
Int32 IssUtils_bufPutEmpty(IssUtils_BufHndl * pHndl,
                           FVID2_FrameList * pFrameList)
{
    UInt32 idx;

    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);
    GT_assert(GT_DEFAULT_MASK, pFrameList != NULL);
    GT_assert(GT_DEFAULT_MASK,
              pFrameList->numFrames <= FVID2_MAX_FVID_FRAME_PTR);

    for (idx = 0; idx < pFrameList->numFrames; idx++)
    {
        status =
            IssUtils_quePut(&pHndl->emptyQue, pFrameList->frames[idx],
                            BIOS_NO_WAIT);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IssUtils_bufPutEmptyFrame                                               
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
Int32 IssUtils_bufPutEmptyFrame(IssUtils_BufHndl * pHndl, FVID2_Frame * pFrame)
{
    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);

    status = IssUtils_quePut(&pHndl->emptyQue, pFrame, BIOS_NO_WAIT);
    GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IssUtils_bufGetFull                                               
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
Int32 IssUtils_bufGetFull(IssUtils_BufHndl * pHndl,
                          FVID2_FrameList * pFrameList, UInt32 timeout)
{
    UInt32 idx, maxFrames;

    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);
    GT_assert(GT_DEFAULT_MASK, pFrameList != NULL);

    if (timeout == BIOS_NO_WAIT)
        maxFrames = FVID2_MAX_FVID_FRAME_PTR;
    else
        maxFrames = pFrameList->numFrames;

    GT_assert(GT_DEFAULT_MASK, maxFrames <= FVID2_MAX_FVID_FRAME_PTR);

    for (idx = 0; idx < maxFrames; idx++)
    {
        status =
            IssUtils_queGet(&pHndl->fullQue, (Ptr *) & pFrameList->frames[idx],
                            1, timeout);
        if (status != FVID2_SOK)
            break;
    }

    pFrameList->numFrames = idx;

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IssUtils_bufGetFullFrame                                               
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
Int32 IssUtils_bufGetFullFrame(IssUtils_BufHndl * pHndl,
                               FVID2_Frame ** pFrame, UInt32 timeout)
{
    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);
    GT_assert(GT_DEFAULT_MASK, pFrame != NULL);

    *pFrame = NULL;

    status = IssUtils_queGet(&pHndl->fullQue, (Ptr *) pFrame, 1, timeout);

    return status;
}

/* ===================================================================
 *  @func     IssUtils_bufPutFull                                               
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
Int32 IssUtils_bufPutFull(IssUtils_BufHndl * pHndl,
                          FVID2_FrameList * pFrameList)
{
    UInt32 idx;

    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);
    GT_assert(GT_DEFAULT_MASK, pFrameList != NULL);
    GT_assert(GT_DEFAULT_MASK,
              pFrameList->numFrames <= FVID2_MAX_FVID_FRAME_PTR);

    for (idx = 0; idx < pFrameList->numFrames; idx++)
    {
        status =
            IssUtils_quePut(&pHndl->fullQue, pFrameList->frames[idx],
                            BIOS_NO_WAIT);
        GT_assert(GT_DEFAULT_MASK, status == FVID2_SOK);
    }

    return FVID2_SOK;
}

/* ===================================================================
 *  @func     IssUtils_bufPutFullFrame                                               
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
Int32 IssUtils_bufPutFullFrame(IssUtils_BufHndl * pHndl, FVID2_Frame * pFrame)
{
    Int32 status;

    GT_assert(GT_DEFAULT_MASK, pHndl != NULL);

    status = IssUtils_quePut(&pHndl->fullQue, pFrame, BIOS_NO_WAIT);
    if (status != FVID2_SOK)
    {
#if 0
        Iss_rprintf
            ("%d: ERROR: In IssUtils_bufPutFullFrame(), IssUtils_quePut() failed !!!\n",
             Clock_getTicks());
#endif
    }

    return status;
}
