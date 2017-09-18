/*
********************************************************************************
 * HDVICP2.0 Based Motion Compensated Temporal Noise Filter(MCTNF)
 *
 * "HDVICP2.0 Based MCTNF" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of filtering noise from a
 *  4:2:0 semi planar Raw data.
 *
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/

/**
 *****************************************************************************
 * @file buffermanager.c
 *
 * @brief This is source module for buffer manager functionality
 *
 * @author: MMCODECS
 *
 * @version 0.0 (Jan 2008) : Base version created
 *                           [MMCODECS]
 * @version 0.1 (Jan 2013) : Modified to supply input buf and output buf
 *                           [Shyam Jagannathan]
 *****************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>

#include <xdc/std.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include "buffermanager.h"
#include <mctnf.h>
#include <tilerBuf.h>

#define MAX_DPB_LUMA_SIZE   (10705920)
#define MAX_DPB_CHROMA_SIZE (5352960)

BUFFMGR_buffEle inBufArray[MAX_BUFF_ELEMENTS];
XDAS_UInt8 *gInBufferHandle[XDM_MAX_IO_BUFFERS];
XDAS_UInt32 gInBufferSizes[XDM_MAX_IO_BUFFERS];

#pragma DATA_SECTION(InBuffers, ".inputbuffer_mem");
XDAS_UInt8 InBuffers[(2048*2048 + 2048*1024 + 32 )*4];

BUFFMGR_buffEle outBufArray[MAX_BUFF_ELEMENTS];
XDAS_UInt8 *gOutBufferHandle[XDM_MAX_IO_BUFFERS];
XDAS_UInt32 gOutBufferSizes[XDM_MAX_IO_BUFFERS];

#pragma DATA_SECTION(OutBuffers, ".inputbuffer_mem");
XDAS_UInt8 OutBuffers[(2048*2048 + 2048*1024 + 32 )*5];

extern sTilerParams TilerParams;
extern MCTNF_Params     gParams;
extern MCTNF_DynamicParams  gDynamicParams;


/*****************************************************************************/
/**
*@func BUFFMGR_Init()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       initialization module
*
*        The BUFFMGR_Init function is called by the test application to
*        initialise the global buffer element array to default and to allocate
*        required number of memory data for reference and output buffers.
*        The maximum required dpb size is defined by the supported profile &
*        level.
*
*@param  numBufs
*        Number of buffers to be allocated
*
*@param  bufSizes
*        Address of array containing the buffer sizes of the numbufs
*
*@return Success(0)/failure(-1) in allocating and initialising
*
*/
/*****************************************************************************/
XDAS_Int32 BUFFMGR_Init
(
    XDAS_Int32    numInBufs,
    XDM2_BufSize *inBufSizes,
    XDAS_Int32    numOutBufs,
    XDM2_BufSize *outBufSizes,
    XDAS_Int8     lumaTilerSpace,
    XDAS_Int8     chromaTilerSpace
)
{
    XDAS_UInt32 tmpCnt, size;
    XDAS_UInt8  idx;

    XDAS_UInt8 *tmpBufPtr;
    XDAS_UInt8 *bufPtr;

    for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
    {
        inBufArray[tmpCnt].bufId      = tmpCnt+1;
        inBufArray[tmpCnt].bufStatus  = BUFFMGR_BUFFER_FREE;

        for (idx = 0; idx < MAX_BUFS_IN_FRAME; idx++)
        {
            inBufArray[tmpCnt].bufSize[idx] = 0;
            inBufArray[tmpCnt].buf[idx]     = NULL;
        }
    }

    bufPtr = (XDAS_UInt8 *)InBuffers;
    /* Initialise the elements in the global buffer array */
    for(idx = 0; idx < numInBufs; idx++)
    {
        size = inBufSizes[0].bytes + inBufSizes[1].bytes + 32;
        tmpBufPtr = (bufPtr + (size*idx) + 15);
        tmpBufPtr = (XDAS_UInt8*)((XDAS_UInt32)tmpBufPtr >> 4);
        tmpBufPtr = (XDAS_UInt8*)((XDAS_UInt32)tmpBufPtr << 4);

        /* Check if allocation took place properly or not */
        if (tmpBufPtr == NULL)
        {
           return -1;
        }

        if(lumaTilerSpace)
        {
           inBufArray[idx].buf[0]     = (volatile XDAS_UInt8 *)TilerParams.tiledBufferAddr[2*idx];
        }
        else
        {
           inBufArray[idx].buf[0]     = tmpBufPtr;
        }
        inBufArray[idx].bufSize[0] = inBufSizes[0].bytes;

        if(chromaTilerSpace)
        {
           inBufArray[idx].buf[1]     = (volatile XDAS_UInt8 *)TilerParams.tiledBufferAddr[2*idx+1];
        }
        else
        {
           inBufArray[idx].buf[1]     = tmpBufPtr + inBufSizes[0].bytes;
        }
        inBufArray[idx].bufSize[1] = inBufSizes[1].bytes;
    }

    for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
    {
        outBufArray[tmpCnt].bufId      = tmpCnt+1;
        outBufArray[tmpCnt].bufStatus  = BUFFMGR_BUFFER_FREE;

        for (idx = 0; idx < MAX_BUFS_IN_FRAME; idx++)
        {
            outBufArray[tmpCnt].bufSize[idx] = 0;
            outBufArray[tmpCnt].buf[idx]     = NULL;
        }
    }

    bufPtr = (XDAS_UInt8 *)OutBuffers;
    /* Initialise the elements in the global buffer array */
    for(idx = 0; idx < numOutBufs; idx++)
    {
        size = outBufSizes[0].bytes + outBufSizes[1].bytes + 32;
        tmpBufPtr = (bufPtr + (size*idx) + 15);
        tmpBufPtr = (XDAS_UInt8*)((XDAS_UInt32)tmpBufPtr >> 4);
        tmpBufPtr = (XDAS_UInt8*)((XDAS_UInt32)tmpBufPtr << 4);

        /* Check if allocation took place properly or not */
        if (tmpBufPtr == NULL)
        {
           return -1;
        }

        if(lumaTilerSpace)
        {
           outBufArray[idx].buf[0]     = (volatile XDAS_UInt8 *)TilerParams.tiledBufferAddr[2*idx];
        }
        else
        {
           outBufArray[idx].buf[0]     = tmpBufPtr;
        }
        outBufArray[idx].bufSize[0] = outBufSizes[0].bytes;

        if(chromaTilerSpace)
        {
           outBufArray[idx].buf[1]     = (volatile XDAS_UInt8 *)TilerParams.tiledBufferAddr[2*idx+1];
        }
        else
        {
           outBufArray[idx].buf[1]     = tmpBufPtr + outBufSizes[0].bytes;
        }
        outBufArray[idx].bufSize[1] = outBufSizes[1].bytes;
    }
     return 0;
}

/*****************************************************************************/
/**
*@func BUFFMGR_GetFreeBuffer()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*        The BUFFMGR_GetFreeBuffer function searches for a free buffer in the
*        global buffer array and returns the address of that element. Incase
*        if none of the elements are free then it returns NULL
*
*
*@return Valid buffer element address or NULL incase if no buffers are empty
*
*/
/*****************************************************************************/
BUFFMGR_buffEleHandle BUFFMGR_GetFreeBuffer(XDAS_UInt32 In_nOut)
{
    XDAS_UInt32 tmpCnt;

    if(In_nOut) {
        for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
        {
            /* Check for first empty buffer in the array and return its address */
            if(inBufArray[tmpCnt].bufStatus == BUFFMGR_BUFFER_FREE)
            {
                inBufArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_USED;
                //printf(" G_In: %d", (tmpCnt + 1));
                return (&inBufArray[tmpCnt]);
            }
        }
    }
    else {
        for(tmpCnt = 0;
            tmpCnt < MAX_BUFF_ELEMENTS;
            tmpCnt++)
        {
            /* Check for first empty buffer in the array and return its address */
            if(outBufArray[tmpCnt].bufStatus == BUFFMGR_BUFFER_FREE)
            {
                outBufArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_USED;
                //printf(" G_Out: %d", (tmpCnt + 1));
                return (&outBufArray[tmpCnt]);
            }
        }
    }

    /* Incase if no elements in the array are free then return NULL */
    return NULL;
}


/*****************************************************************************/
/**
*@func BUFFMGR_ReleaseBuffer(buffId)
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*        The BUFFMGR_ReleaseBuffer function takes an array of buffer-ids
*        which are released by the test-app. "0" is not a valid buffer Id
*        hence this function keeps moving until it encounters a buffer Id
*        as zero or it hits the MAX_BUFF_ELEMENTS
*
*
*@return None
*
*/
/*****************************************************************************/
void BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[], XDAS_UInt32 In_nOut)
{
    XDAS_UInt32 tmpCnt, tmpId;
    if(In_nOut) {
        for(tmpCnt = 0; (tmpCnt < MAX_BUFF_ELEMENTS);  tmpCnt++)
        {
            tmpId = bufffId[tmpCnt];
            /*
             * Check if the buffer Id = 0 condition has reached. zero is not a
             * valid buffer Id hence that value is used to identify the end of
             * buffer array
             */
            if(tmpId == 0)
            {
               break;
            }
            /*
             * convert the buffer-Id to its corresponding index in the global
             * array
             */
            //printf(" F_In: %d", tmpId);
            tmpId--;
            /* Set the status of the buffer to FREE */
            inBufArray[tmpId].bufStatus = BUFFMGR_BUFFER_FREE;
        }
    }
    else {
        for(tmpCnt = 0;
            (tmpCnt < MAX_BUFF_ELEMENTS);
            tmpCnt++)
        {
            tmpId = bufffId[tmpCnt];
            /*
             * Check if the buffer Id = 0 condition has reached. zero is not a
             * valid buffer Id hence that value is used to identify the end of
             * buffer array
             */
            if(tmpId == 0)
            {
               break;
            }
            /*
             * convert the buffer-Id to its corresponding index in the global
             * array
             */
            //printf(" F_Out: %d", tmpId);
            tmpId--;
            /* Set the status of the buffer to FREE */
            outBufArray[tmpId].bufStatus = BUFFMGR_BUFFER_FREE;
        }
    }

    return;
}

/****************************************************************************/
/**
*@func BUFFMGR_ReleaseAllBuffers()
*@brief  TII's (Texas Instrument India) implementation of buffer manager
*       re-initialization module
*
*       The BUFFMGR_ReleaseAllBuffers function will set the status of
*       all buffer elements in the buffArray[] to free. This is called
*       for cases when application does not set freebufid of all buffers
*       typically when stream has not finished but app wants to end decoding.
*
*@return None
*/
/****************************************************************************/
void BUFFMGR_ReleaseAllBuffers()
{
    XDAS_UInt32 tmpCnt;
    for(tmpCnt = 0; (tmpCnt < MAX_BUFF_ELEMENTS); tmpCnt++)
    {
        /* Set the status of the buffer to FREE */
        inBufArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_FREE;
        outBufArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_FREE;
    }
    return;
}

