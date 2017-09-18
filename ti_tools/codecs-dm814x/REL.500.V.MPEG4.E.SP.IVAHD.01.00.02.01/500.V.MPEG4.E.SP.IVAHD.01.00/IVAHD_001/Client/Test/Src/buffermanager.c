/*
********************************************************************************
 * HDVICP2.0 Based MPEG4 SP Encoder
 *
 * "HDVICP2.0 Based MPEG4 SP Encoder" is software module developed on TI's
 *  HDVICP2 based SOCs. This module is capable of compressing a 4:2:0 Raw
 *  video into a simple profile bit-stream. Based on ISO/IEC 14496-2."
 *
 * Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 * ALL RIGHTS RESERVED
********************************************************************************
*/
/**
********************************************************************************
 * @file <BufferManager.c>
 *
 * @brief This is source module for buffer manager functionality
 *        (Simple Profile) Video Encoder Call using XDM Interface
 *
 * @author: Venugopala Krishna (venugopala@ti.com)
 *
 * @version 0.0 (May 2007) : Base version for IVAHD developement
 *                           [Venugopala]
 *
 *******************************************************************************
*/


/* -------------------- compilation control switches -------------------------*/

/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
/* -------------------- system and platform files ----------------------------*/
#include <stdio.h>
#include <stdlib.h>

/*--------------------- program files ----------------------------------------*/
#include <TestAppComDataType.h>
#include <ti/xdais/xdas.h>
#include <ti/xdais/dm/xdm.h>
#include "buffermanager.h"
#include "tilerBuf.h"

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/
#pragma DATA_SECTION(InBuffers, ".inputbuffer");

#define BUFF_ELEMENT_0    0
#define BUFF_ELEMENT_1    1
#define BUFF_ELEMENT_2    2
#define BUFF_ELEMENT_3    3
#define BUFF_ELEMENT_4    4

/**
 * Used for Buffer management. Stores the luma and chroma sizes for each frame
 * Used by the application to find out the buffers locked by the codec and the
 * free buffers
 */
BUFFMGR_buffEle buffArray[MAX_BUFF_ELEMENTS];
/**
 * Buffer used for storing the input frame(YUV), input to the MPEG4 Encoder
 */
XDAS_UInt8 InBuffers[(1920*1088 + 1920*544)*2];

extern sTilerParams TilerParams;
extern U32 frameCaptureWidth;
extern U32 frameCaptureHeight;

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/


/**
********************************************************************************
 * @func BUFFMGR_Init()
 * @brief  TII's (Texas Instrument India) implementation of buffer manager 
 *        initialization module
 *
 *         The BUFFMGR_Init function is called by the test application to
 *         initialise the global buffer element array to default and to allocate
 *         required number of memory data for reference and output buffers.
 *         The maximum required dpb size is defined by the supported profile & 
 *         level.
 *
 * @param  numRefBufs
 *         Number of buffers to be allocated
 *
 * @param  bufSizes
 *         Address of array containing the buffer sizes of the numbufs
 *
 * @return Success(0)/failure(-1) in allocating and initialising
 *
********************************************************************************
*/
XDAS_Int32 BUFFMGR_Init(XDAS_Int32 numRefBufs, XDAS_Int32 numBufs, 
                        XDM2_BufSize *bufSizes,
                        XDAS_Int8 lumaTilerSpace, XDAS_Int8 chromaTilerSpace)
{
  XDAS_UInt32 tmpCnt, size;
  XDAS_UInt8  idx;
  XDAS_UInt8 *tmpBufPtr;
  XDAS_UInt8 *InbufPtr;
  InbufPtr = (XDAS_UInt8 *)InBuffers;

  /*----------------------------------------------------*/
  /* Initialise the elements in the global buffer array */
  /*----------------------------------------------------*/
  for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
  {
    buffArray[tmpCnt].bufId = tmpCnt + 1;
    buffArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_FREE;

    for (idx = 0; idx < MAX_BUFS_IN_FRAME; idx++)
    {
      buffArray[tmpCnt].bufSize[idx] = 0;
      buffArray[tmpCnt].buf[idx] = NULL;
    }
  }

  for(idx = 0; idx < numRefBufs; idx++)
  {
#if 0 // Karthick
    size = bufSizes[0].tileMem.height * bufSizes[0].tileMem.width 
           + bufSizes[1].tileMem.height * bufSizes[1].tileMem.width  + 32;
#else
    size = frameCaptureHeight * frameCaptureWidth +
           frameCaptureWidth * (frameCaptureHeight >> 1) + 32;
#endif
    tmpBufPtr = (InbufPtr + size * idx + 15);
    tmpBufPtr = (XDAS_UInt8*)((U32)tmpBufPtr >> 4);
    tmpBufPtr = (XDAS_UInt8*)((U32)tmpBufPtr << 4);

    /*----------------------------------------------------*/
    /* Check if allocation took place properly or not     */
    /*----------------------------------------------------*/
    if (tmpBufPtr == NULL)
    {
      return -1;
    }
   
    if(lumaTilerSpace)
    {
      buffArray[idx].buf[0]     = 
        (volatile XDAS_UInt8 *)TilerParams.tiledBufferAddr[2*idx];;
    buffArray[idx].bufSize[0] = bufSizes[0].bytes;
    }
    else
    {
      buffArray[idx].buf[0]     = tmpBufPtr;
    buffArray[idx].bufSize[0] =
#if 0 // Karthick 
      bufSizes[0].tileMem.height * bufSizes[0].tileMem.width;
#else
      frameCaptureHeight * frameCaptureWidth;
#endif
    }    
   
    if(chromaTilerSpace)
    {
      buffArray[idx].buf[1]     = 
        (volatile XDAS_UInt8 *)TilerParams.tiledBufferAddr[2*idx+1];
      buffArray[idx].bufSize[1] = bufSizes[1].bytes; 
        
    }
    else
    {
      buffArray[idx].buf[1]     = 
#if 0 // Karthick
        tmpBufPtr + bufSizes[0].tileMem.height * bufSizes[0].tileMem.width;
#else
        tmpBufPtr + frameCaptureWidth * frameCaptureHeight;
#endif

      buffArray[idx].bufSize[1] = 
#if 0 // Karthick
        bufSizes[1].tileMem.height * bufSizes[1].tileMem.width;
#else
        frameCaptureWidth * (frameCaptureHeight >> 1);
#endif
    }    
  }
  return 0;
}


/**
********************************************************************************
 *  @func     BUFFMGR_ReInit
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          re-initialization module
 *
 *          The BUFFMGR_ReInit function allocates global luma and chroma buffers
 *          and allocates entire space to first element. This element will be
 *          used in first frame decode. After the picture's height and width and
 *          its luma and chroma buffer requirements are obtained the global luma
 *          and chroma buffers are re-initialised to other elements in the
 *          buffer array.
 *
 *  @param[in]  numRefBufs : Number of reference buffers to be allocated
 *
 *  @param[in]  numOutBufs : Number of output buffers
 *
 *  @param[in]  bufSizes   : Pointer to the XDM2_BufSize structure
 *
 *  @return Success(0)/failure(-1) in allocating and initialising
********************************************************************************
*/
XDAS_Int32 BUFFMGR_ReInit(
  XDAS_Int32 numRefBufs,
  XDAS_Int32 numOutBufs,
  XDM2_BufSize *bufSizes
)
{
  XDAS_UInt8 *tmpBufPtr;
  XDAS_Int32  idx, ctr, size;

  /*--------------------------------------------------------------*/
  /* As the application would have already provided one buffer    */
  /* allocate memory for 1 minus the actual number of reference   */
  /* buffers.                                                     */
  /*--------------------------------------------------------------*/
  for(idx = 1; idx < numRefBufs; idx++)
  {
    ctr = 0;
    size = bufSizes[ctr].bytes + bufSizes[ctr+1].bytes + 32;
    tmpBufPtr = malloc (size * sizeof(XDAS_UInt8));
    memset(tmpBufPtr,0,size);

    if(tmpBufPtr == NULL)
    {
      free(tmpBufPtr);
      return -1;
    }

    buffArray[idx].buf[ctr]     = tmpBufPtr;
    buffArray[idx].bufSize[ctr] = bufSizes[ctr].bytes;
    buffArray[idx].buf[ctr + 1]     = tmpBufPtr + bufSizes[ctr].bytes;
    buffArray[idx].bufSize[ctr + 1] = bufSizes[ctr + 1].bytes;
  }

  return 0;
}


/**
********************************************************************************
 *  @func     BUFFMGR_Init
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          BUFFMGR_GetFreeBuffer module
 *
 *          The BUFFMGR_GetFreeBuffer function searches for a free buffer in the
 *          global buffer array and returns the address of that element. Incase
 *          if none of the elements are free then it returns NULL
 *
 *  @return Address of the first empty Buffer Array if success else NULL
********************************************************************************
*/
BUFFMGR_buffEleHandle BUFFMGR_GetFreeBuffer()
{
  XDAS_UInt32 tmpCnt;
  for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
  {
    /*------------------------------------------------------------------*/
    /* Check for first empty buffer in the array and return its address */
    /*------------------------------------------------------------------*/
    if(buffArray[tmpCnt].bufStatus == BUFFMGR_BUFFER_FREE)
    {
       buffArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_USED;
       return (&buffArray[tmpCnt]);
    }
  }
  /*------------------------------------------------------------------*/
  /* Incase if no elements in the array are free then return NULL     */
  /*------------------------------------------------------------------*/
  return NULL;
}


/**
********************************************************************************
 *  @func     BUFFMGR_ReleaseBuffer
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          BUFFMGR_ReleaseBuffer module
 *
 *          The BUFFMGR_ReleaseBuffer function takes an array of buffer-ids
 *          which are released by the test-app. "0" is not a valid buffer Id
 *          hence this function keeps moving until it encounters a buffer Id as
 *          zero or it hits the MAX_BUFF_ELEMENTS
 *
 *  @param  bufffId    Array of buffer ID's holds the index of the available
 *                     buffers, includes both the buffers locked by the codec
 *                     as well the ones that are free
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[])
{
  XDAS_UInt32 tmpCnt, tmpId;

  for(tmpCnt = 0;
    (tmpCnt < MAX_BUFF_ELEMENTS);
    tmpCnt++)
  {
    tmpId = bufffId[tmpCnt];
    /*-----------------------------------------------------------------*/
    /* Check if the buffer Id = 0 condition has reached. zero is not a */
    /* valid buffer Id hence that value is used to identify the end of */
    /* buffer array.                                                   */
    /*-----------------------------------------------------------------*/
    if(tmpId == 0)
    {
       break;
    }
    /*-----------------------------------------------------------------*/
    /* convert the buffer-Id to its corresponding index in the global  */
    /* array.                                                          */
    /*-----------------------------------------------------------------*/
    tmpId--;

    /*-----------------------------------------------------------------*/
    /* Set the status of the buffer to FREE                            */
    /*-----------------------------------------------------------------*/
    buffArray[tmpId].bufStatus = BUFFMGR_BUFFER_FREE;
  }

  return;
}


/**
********************************************************************************
 *  @func     BUFFMGR_ReleaseAllBuffers
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          BUFFMGR_ReleaseAllBuffers module
 *
 *          The BUFFMGR_ReleaseAllBuffers function will set the status of
 *          all buffer elements in the buffArray[] to free. This is called
 *          for cases when application does not set freebufid of all buffers
 *          typically when stream has not finished but app wants to end decoding
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_ReleaseAllBuffers()
{
  XDAS_UInt32 tmpCnt;
  for(tmpCnt = 0; (tmpCnt < MAX_BUFF_ELEMENTS); tmpCnt++)
  {
    /*--------------------------------------------------------------*/
    /* Set the status of the buffer to FREE                         */
    /*--------------------------------------------------------------*/
    buffArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_FREE;
  }
  return;
}

/**
********************************************************************************
 *  @func     BUFFMGR_DeInit
 *  @brief  TII's (Texas Instrument India) implementation of buffer manager
 *          BUFFMGR_DeInit module
 *
 *          The BUFFMGR_DeInit function releases all memory allocated by buffer
 *          manager.
 *
 *  @param[in]  numRefBufs : Number of reference buffers to be allocated
 *
 *  @param[in]  numOutBufs : Number of output buffers
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_DeInit
(
  XDAS_Int32 numRefBufs,
  XDAS_Int32 numOutBufs
)
{
  XDAS_Int32  idx, ctr;

  /*--------------------------------------------------------------*/
  /* As the application would have already provided one buffer    */
  /* allocate memory for 1 minus the actual number of reference   */
  /* buffers.                                                     */
  /*--------------------------------------------------------------*/
  for(idx = 0; idx < numRefBufs; idx++)
  {
    ctr = 0;
    free((void *)buffArray[idx].buf[ctr]);
  }
  return;
}
