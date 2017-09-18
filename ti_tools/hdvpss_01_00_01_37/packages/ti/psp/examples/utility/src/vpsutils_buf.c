/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/examples/utility/vpsutils_buf.h>


Int32 VpsUtils_bufCreate(VpsUtils_BufHndl *pHndl, Bool blockOnGet, Bool blockOnPut)
{
    Int32 status;
    UInt32 flags;

    flags = VPSUTILS_QUE_FLAG_NO_BLOCK_QUE;

    if(blockOnGet)
        flags |= VPSUTILS_QUE_FLAG_BLOCK_QUE_GET;
    if(blockOnPut)
        flags |= VPSUTILS_QUE_FLAG_BLOCK_QUE_PUT;

    status = VpsUtils_queCreate(
                &pHndl->emptyQue,
                VPSUTILS_BUF_MAX_QUE_SIZE,
                pHndl->emptyQueMem,
                flags
                );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    status = VpsUtils_queCreate(
                &pHndl->fullQue,
                VPSUTILS_BUF_MAX_QUE_SIZE,
                pHndl->fullQueMem,
                flags
                );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    return status;
}

Int32 VpsUtils_bufDelete(VpsUtils_BufHndl *pHndl)
{
    VpsUtils_queDelete(&pHndl->emptyQue);
    VpsUtils_queDelete(&pHndl->fullQue);

    return FVID2_SOK;
}


Int32 VpsUtils_bufGetEmpty(VpsUtils_BufHndl *pHndl, FVID2_FrameList *pFrameList, UInt32 timeout)
{
    UInt32 idx, maxFrames;
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrameList!=NULL);

    if(timeout==BIOS_NO_WAIT)
        maxFrames = FVID2_MAX_FVID_FRAME_PTR;
    else
        maxFrames = pFrameList->numFrames;

    GT_assert( GT_DEFAULT_MASK, maxFrames<=FVID2_MAX_FVID_FRAME_PTR);

    for(idx=0;idx<maxFrames; idx++)
    {
        status = VpsUtils_queGet(&pHndl->emptyQue, (Ptr*)&pFrameList->frames[idx], 1, timeout);
        if(status!=FVID2_SOK)
            break;
    }

    pFrameList->numFrames = idx;

    return FVID2_SOK;
}

Int32 VpsUtils_bufGetEmptyFrame(
                VpsUtils_BufHndl *pHndl,
                FVID2_Frame **pFrame,
                UInt32 timeout)
{
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrame!=NULL);

    *pFrame = NULL;

    status = VpsUtils_queGet(&pHndl->emptyQue, (Ptr*)pFrame, 1, timeout);

    return status;
}

Int32 VpsUtils_bufPutEmpty(VpsUtils_BufHndl *pHndl, FVID2_FrameList *pFrameList)
{
    UInt32 idx;
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrameList!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrameList->numFrames<=FVID2_MAX_FVID_FRAME_PTR);

    for(idx=0;idx<pFrameList->numFrames; idx++)
    {
        status = VpsUtils_quePut(&pHndl->emptyQue, pFrameList->frames[idx], BIOS_NO_WAIT);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    return FVID2_SOK;
}

Int32 VpsUtils_bufPutEmptyFrame(VpsUtils_BufHndl *pHndl, FVID2_Frame *pFrame)
{
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);

    status = VpsUtils_quePut(&pHndl->emptyQue, pFrame, BIOS_NO_WAIT);
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    return FVID2_SOK;
}


Int32 VpsUtils_bufGetFull(VpsUtils_BufHndl *pHndl, FVID2_FrameList *pFrameList, UInt32 timeout)
{
    UInt32 idx, maxFrames;
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrameList!=NULL);

    if(timeout==BIOS_NO_WAIT)
        maxFrames = FVID2_MAX_FVID_FRAME_PTR;
    else
        maxFrames = pFrameList->numFrames;

    GT_assert( GT_DEFAULT_MASK, maxFrames<=FVID2_MAX_FVID_FRAME_PTR);

    for(idx=0;idx<maxFrames; idx++)
    {
        status = VpsUtils_queGet(&pHndl->fullQue, (Ptr*)&pFrameList->frames[idx], 1, timeout);
        if(status!=FVID2_SOK)
            break;
    }

    pFrameList->numFrames = idx;

    return FVID2_SOK;
}

Int32 VpsUtils_bufGetFullFrame(
                VpsUtils_BufHndl *pHndl,
                FVID2_Frame **pFrame,
                UInt32 timeout)
{
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrame!=NULL);

    *pFrame = NULL;

    status = VpsUtils_queGet(&pHndl->fullQue, (Ptr*)pFrame, 1, timeout);

    return status;
}

Int32 VpsUtils_bufPutFull(VpsUtils_BufHndl *pHndl, FVID2_FrameList *pFrameList)
{
    UInt32 idx;
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrameList!=NULL);
    GT_assert( GT_DEFAULT_MASK, pFrameList->numFrames<=FVID2_MAX_FVID_FRAME_PTR);

    for(idx=0;idx<pFrameList->numFrames; idx++)
    {
        status = VpsUtils_quePut(&pHndl->fullQue, pFrameList->frames[idx], BIOS_NO_WAIT);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    }

    return FVID2_SOK;
}

Int32 VpsUtils_bufPutFullFrame(VpsUtils_BufHndl *pHndl, FVID2_Frame *pFrame)
{
    Int32 status;

    GT_assert( GT_DEFAULT_MASK, pHndl!=NULL);

    status = VpsUtils_quePut(&pHndl->fullQue, pFrame, BIOS_NO_WAIT);
    if(status!=FVID2_SOK)
    {
        #if 0
        Vps_rprintf( "%d: ERROR: In VpsUtils_bufPutFullFrame(), VpsUtils_quePut() failed !!!\n", Clock_getTicks());
        #endif
    }

    return status;
}
