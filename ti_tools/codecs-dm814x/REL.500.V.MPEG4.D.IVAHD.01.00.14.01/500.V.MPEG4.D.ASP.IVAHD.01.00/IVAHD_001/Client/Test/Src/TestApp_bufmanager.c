/*
*******************************************************************************
* HDVICP2.0 Based MPEG4 ASP Decoder
*
* "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
*  HDVICP2 based SOCs. This module is capable of decode a 4:2:0 Raw
*  video stream of Advanced/Simple profile and also H.263 bit-stream.
*  Based on  ISO/IEC 14496-2:2003."
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
*******************************************************************************
*/

/**
*******************************************************************************
* @file <TestApp_bufmanager.c>
*
* @brief  This File contains function definitions which implements a sample
*         buffer management algorithm for integrating with XDM compliant
*         video decoder
*
* @author:  Ashish Singh
*
* @version 0.0 (August 2011)  : Initial version
*
*
******************************************************************************
*/
/******************************************************************************
*  INCLUDE FILES
******************************************************************************/
#include <xdc/std.h>
#include <TestApp_constants.h>
#include <TestAppDecoder.h>
#include <tilerBuf.h>

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
BUFFMGR_buffEle buffArray[MAX_BUFF_ELEMENTS];
XDAS_UInt8 *globalBufferHandle[XDM_MAX_IO_BUFFERS];
XDAS_UInt32 globalBufferSizes[XDM_MAX_IO_BUFFERS];
XDAS_UInt8 *LumaBufPtr, *ChromaBufPtr, *NextBufPtr;
XDAS_UInt32 LumaBufWidth, ChromaBufWidth;

/*--------------------------------------------------------------------------*/
/* Externally declared variables, accessed in this file                     */
/*--------------------------------------------------------------------------*/
extern IMPEG4VDEC_Params    params;
extern sTilerParams       tilerParams;
extern sAppControlParams  appControlPrms;

extern IMPEG4VDEC_DynamicParams   dynamicParams;
extern XDAS_UInt32              bufferFreed;

extern XDAS_UInt8 metaDataIndex[IVIDEO_MAX_NUM_METADATA_PLANES];
extern XDAS_UInt8 metaDataEnable[IVIDEO_MAX_NUM_METADATA_PLANES];

/**
********************************************************************************
 *  @fn     BUFFMGR_Init
 *  @brief  The BUFFMGR_Init function is called by the test application to
 *          initialise the global buffer element array to default and to
 *          allocate required number of memory data for reference and output
 *          buffers.
 *          The maximum required dpb size is defined by the supported profile &
 *          level
 *
 *  @param[in] numBufs   : Number of buffers to be allocated.
 *  @param[in] bufSizes  : Address of array containing the buffer sizes of the
 *                         numbufs.
 *  @param[in] memType   : Pointer to the memory type to tell whether it is
 *                         Tiled memory or not
 *
 *  @return Success(0)/failure(-1) in allocating and initialising
********************************************************************************
*/
XDAS_Int32 BUFFMGR_Init(XDAS_Int32    numBufs,
                        XDM2_BufSize *bufSizes,
                        XDAS_Int32   *memType
                       )
{
  XDAS_UInt32 tmpCnt, Size;
  XDAS_UInt8  idx;
  XDAS_UInt8 *BufPtr;

  /*--------------------------------------------------------------------------*/
  /* Initialise the elements in the global buffer array                       */
  /*--------------------------------------------------------------------------*/
  for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
  {
    buffArray[tmpCnt].bufId      = tmpCnt + 1;
    buffArray[tmpCnt].bufStatus  = BUFFMGR_BUFFER_FREE;

    for (idx = 0; idx < MAX_BUFS_IN_FRAME; idx++)
    {
      buffArray[tmpCnt].bufSize[idx].bytes = 0;
      buffArray[tmpCnt].buf[idx]           = NULL;
    }
  }

  if(!dynamicParams.viddec3DynamicParams.decodeHeader)
  {
    /*-----------------------------------------------------------------------*/
    /* Allocate memory from the tiled space                                  */
    /*-----------------------------------------------------------------------*/
    if((appControlPrms.tilerEnable) && (memType[0] != XDM_MEMTYPE_ROW))
    {
      buffArray[0].buf[0]  =
                  (volatile XDAS_UInt8 *)(tilerParams.tiledBufferAddr[0]);

      buffArray[0].bufSize[0].tileMem.width  = bufSizes[0].tileMem.width;
      buffArray[0].bufSize[0].tileMem.height = bufSizes[0].tileMem.height;

      buffArray[0].buf[1] =
                (volatile XDAS_UInt8 *)(tilerParams.tiledBufferAddr[1]);

      buffArray[0].bufSize[1].tileMem.width  = bufSizes[1].tileMem.width;
      buffArray[0].bufSize[1].tileMem.height = bufSizes[1].tileMem.height;
    }
    else
    {
      memType[0] = XDM_MEMTYPE_ROW;
      memType[1] = XDM_MEMTYPE_ROW;

      buffArray[0].bufSize[0].bytes = (bufSizes[0].tileMem.width) *
                                      (bufSizes[0].tileMem.height);

      buffArray[0].bufSize[1].bytes = (bufSizes[1].tileMem.width) *
                                      (bufSizes[1].tileMem.height);

      bufSizes[0].bytes             = buffArray[0].bufSize[0].bytes;
      bufSizes[1].bytes             = buffArray[0].bufSize[1].bytes;

      for (idx = 0; idx < 2; idx++)
      {
        /*--------------------------------------------------------------------*/
        /*  NON-CONTIGUOUS BUFFERS:                                           */
        /*  Separate buffers for Luma & chroma.                               */
        /*--------------------------------------------------------------------*/
        Size   = bufSizes[idx].bytes;
        BufPtr = my_Memalign (128, Size * sizeof(XDAS_UInt8));

        /*--------------------------------------------------------------------*/
        /* Check if allocation took place properly or not                     */
        /*--------------------------------------------------------------------*/
        if(BufPtr == NULL)
        {
          return -1;
        }

        memset(BufPtr, 0x80,  Size);

        buffArray[0].buf[idx]             = BufPtr;
        buffArray[0].bufSize[idx].bytes   = Size;
      }
    }
 }

  /*--------------------------------------------------------------------------*/
  /* Meta Data Buffer allocation & initialization                             */
  /*--------------------------------------------------------------------------*/
  if(TestAppMetaDataBuf(bufSizes))
  {
    return -1;
  }

   return 0;
}

/**
********************************************************************************
 *  @fn     BUFFMGR_ReInit
 *  @brief  The BUFFMGR_ReInit function allocates global luma and chroma buffers
 *          and allocates entire space to first element. This element will be
 *          used in first frame decode. After the picture's height and width and
 *          its luma and chroma buffer requirements are obtained the global luma
 *          and chroma buffers are re-initialised to other elements in that
 *          buffer array.
 *
 *  @param[in] numRefBufs : Number of Buffers that will work as reference
 *                          buffers for decoding of a frame.
 *  @param[in] numOutBufs : pointer to SeqParams.
 *  @param[in] *bufSizes  : Pointer to array containing the buffer sizes.
 *  @param[in] *memType   : Pointer to the arry containing memory type
 *                          (Tiler/Non-Tiler)
 *
 *  @return Success(0)/failure(-1) in allocating and initialising
********************************************************************************
*/
XDAS_Int32 BUFFMGR_ReInit( XDAS_Int32 numRefBufs,
                           XDAS_Int32 numOutBufs,
                           XDM2_BufSize *bufSizes,
                           XDAS_Int32 *memType,
                           XDAS_Int32  startBufIdx
                         )
{
  XDAS_Int32  idx, size;
  XDAS_Int32  startIdx = startBufIdx;

  if(dynamicParams.viddec3DynamicParams.decodeHeader)
  {
    startIdx = 0;
  }

  /*--------------------------------------------------------------------------*/
  /*  Freeing up meta data Buffers                                            */
  /*--------------------------------------------------------------------------*/
  if(metaDataEnable[APP_MB_INFO])
  {
    free((void *)buffArray[0].buf[2 + metaDataIndex[APP_MB_INFO]]);
  }

  /*--------------------------------------------------------------------------*/
  /* As the application would have already provided one buffer                */
  /* allocate memory for 1 minus the actual number of reference buffers.      */
  /*--------------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------*/
  /* Allocate from Tiled space                                                */
  /*--------------------------------------------------------------------------*/
  if((appControlPrms.tilerEnable) && (memType[0] != XDM_MEMTYPE_ROW))
  {
    for(idx = startIdx; idx < MAX_BUFF_ELEMENTS; idx++)
    {
      buffArray[idx].buf[0]  =
                  (volatile XDAS_UInt8 *)tilerParams.tiledBufferAddr[idx*2];

      buffArray[idx].bufSize[0].tileMem.width  = bufSizes[0].tileMem.width;
      buffArray[idx].bufSize[0].tileMem.height = bufSizes[0].tileMem.height;

      buffArray[idx].buf[1] =
                (volatile XDAS_UInt8 *)tilerParams.tiledBufferAddr[idx*2+1];

      buffArray[idx].bufSize[1].tileMem.width  = bufSizes[1].tileMem.width;
      buffArray[idx].bufSize[1].tileMem.height = bufSizes[1].tileMem.height;

    }
  }
  else
  {
      memType[0] = XDM_MEMTYPE_ROW;
      memType[1] = XDM_MEMTYPE_ROW;

      for(idx = startIdx; idx < MAX_BUFF_ELEMENTS; idx++)
      {
        buffArray[idx].bufSize[0].bytes = (bufSizes[0].tileMem.width) *
                                          (bufSizes[0].tileMem.height);

        buffArray[idx].bufSize[1].bytes = (bufSizes[1].tileMem.width) *
                                          (bufSizes[1].tileMem.height);
      }

      bufSizes[0].bytes = buffArray[idx-1].bufSize[0].bytes;
      bufSizes[1].bytes = buffArray[idx-1].bufSize[1].bytes;

      for(idx = startIdx; idx < MAX_BUFF_ELEMENTS; idx++)
      {
        /*--------------------------------------------------------------------*/
        /* Allocation for Luma. Check if allocation happened or not           */
        /*--------------------------------------------------------------------*/
        size       = bufSizes[0].bytes;
        LumaBufPtr = my_Memalign (128, size * sizeof(XDAS_UInt8));

        if(LumaBufPtr == NULL)
        {
          return -1;
        }
        /*--------------------------------------------------------------------*/
        /* Fill the Buffer with gray value.                                   */
        /*--------------------------------------------------------------------*/
        memset(LumaBufPtr, 0x80, size);

        /*--------------------------------------------------------------------*/
        /* Allocation for Chroma. Check if allocation happened or not         */
        /*--------------------------------------------------------------------*/
        size         = bufSizes[1].bytes;
        ChromaBufPtr = my_Memalign (128, size * sizeof(XDAS_UInt8));

        if(ChromaBufPtr == NULL)
        {
          free(LumaBufPtr);
          return -1;
        }
        /*--------------------------------------------------------------------*/
        /* Fill the Buffer with gray value.                                   */
        /*--------------------------------------------------------------------*/
        memset(ChromaBufPtr, 0x80, size);

        buffArray[idx].buf[0]           = LumaBufPtr;
        buffArray[idx].bufSize[0].bytes = bufSizes[0].bytes;
        buffArray[idx].buf[1]           = ChromaBufPtr;
        buffArray[idx].bufSize[1].bytes = bufSizes[1].bytes;
      }
    }

  /*--------------------------------------------------------------------------*/
  /* Meta Data Buffer allocation & initialization                             */
  /*--------------------------------------------------------------------------*/
  if(TestAppMetaDataBuf(bufSizes))
  {
    return -1;
  }

  return 0;
}

/**
********************************************************************************
 *  @fn     TestAppMetaDataBuf
 *  @brief  The TestAppMetaDataBuf function is called by the test application to
 *          initialise the Meta data buffers allocation and corresponding
 *          initializations of global buffer arrays.
 *
 *  @param[in] bufSizes  : Address of array containing the buffer sizes of the
 *                         numbufs.
 *
 *  @return Success(0)/failure(-1) in allocating and initialising
********************************************************************************
*/
XDAS_Int32 TestAppMetaDataBuf(XDM2_BufSize *bufSizes)
{
  XDAS_UInt32 Size;
  XDAS_UInt8 *BufPtr, metaBufIdx;

  /*------------------------------------------------------------------------*/
  /* MB-Info Meta Data buffer Allocation                                    */
  /*------------------------------------------------------------------------*/
  metaBufIdx  = 2 + metaDataIndex[APP_MB_INFO];
  Size        = bufSizes[metaBufIdx].bytes;

  if(Size && metaDataEnable[APP_MB_INFO])
  {
    BufPtr = my_Memalign (128, Size * sizeof(XDAS_UInt8));

    if(BufPtr == NULL)
    {
      return -1;
    }

    buffArray[0].buf[metaBufIdx]           = BufPtr;
    buffArray[0].bufSize[metaBufIdx].bytes = Size;
  }

  return 0;
}

/**
********************************************************************************
 *  @fn     BUFFMGR_GetFreeBuffer
 *
 *  @brief  The BUFFMGR_GetFreeBuffer function searches for a free buffer in the
 *          global buffer array and returns the address of that element. Incase
 *          if none of the elements are free then it returns NULL
 *
 *  @return Valid buffer element address or NULL incase if no buffers are empty
********************************************************************************
*/
BUFFMGR_buffEleHandle BUFFMGR_GetFreeBuffer()
{
  XDAS_UInt32 tmpCnt;

  for(tmpCnt = 0; tmpCnt < MAX_BUFF_ELEMENTS; tmpCnt++)
  {
    /*------------------------------------------------------------------------*/
    /* Check for first empty buffer in the array and return its address       */
    /*------------------------------------------------------------------------*/
    if(buffArray[tmpCnt].bufStatus == BUFFMGR_BUFFER_FREE)
    {
      /*----------------------------------------------------------------------*/
      /* The staus of the buffer may be free, but allocation might not have   */
      /* happened really                                                      */
      /*----------------------------------------------------------------------*/
      if((buffArray[tmpCnt].buf[0] == NULL) ||
         (buffArray[tmpCnt].buf[1] == NULL))
      {
        printf("Exceeded the number of allocated buffers...\n");
        return NULL;
      }
      else
      {
        buffArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_USED;
        return (&buffArray[tmpCnt]);
      }
    }
  }

  /*--------------------------------------------------------------------------*/
  /* Incase if no elements in the array are free then return NULL             */
  /*--------------------------------------------------------------------------*/
    return NULL;
}

/**
********************************************************************************
 *  @fn     BUFFMGR_ReleaseBuffer
 *
 *  @brief  The BUFFMGR_ReleaseBuffer function takes an array of buffer-ids
 *          which are released by the test-app. "0" is not a valid buffer Id
 *          hence this function keeps moving until it encounters a buffer Id
 *          as zero or it hits the MAX_BUFF_ELEMENTS
 *
 *  @param[in] bufffId : Buffer ID for releasing the buffer.
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_ReleaseBuffer(XDAS_UInt32 bufffId[])
{
  XDAS_UInt32 tmpCnt, tmpId;

  for(tmpCnt = 0; (tmpCnt < MAX_BUFF_ELEMENTS); tmpCnt++)
  {
    tmpId = bufffId[tmpCnt];

    /*------------------------------------------------------------------------*/
    /* Check if the buffer Id = 0 condition has reached. zero is not a valid  */
    /* buffer Id hence that value is used to identify the end of buffer array */
    /*------------------------------------------------------------------------*/
    if(tmpId == 0)
    {
      break;
    }

    /*------------------------------------------------------------------------*/
    /* convert the buffer-Id to its corresponding index in the global array   */
    /*------------------------------------------------------------------------*/
    tmpId--;

    /*------------------------------------------------------------------------*/
    /* Set the status of the buffer to FREE                                   */
    /*------------------------------------------------------------------------*/
    buffArray[tmpId].bufStatus = BUFFMGR_BUFFER_FREE;
    bufferFreed++;
  }

  return;
}

/**
********************************************************************************
 *  @fn     BUFFMGR_ReleaseAllBuffers
 *
 *  @brief  The BUFFMGR_ReleaseAllBuffers function will set the status of all
 *          buffer elements in the buffArray[] to free. This is called for cases
 *          when application does not set freebufId of all buffers typically
 *          when stream has not finished but app wants to end decoding.
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_ReleaseAllBuffers()
{
  XDAS_UInt32 tmpCnt;

  for(tmpCnt = 0; (tmpCnt < MAX_BUFF_ELEMENTS); tmpCnt++)
  {
    /*------------------------------------------------------------------------*/
    /* Set the status of the buffer to FREE                                   */
    /*------------------------------------------------------------------------*/
    buffArray[tmpCnt].bufStatus = BUFFMGR_BUFFER_FREE;
  }
    return;
}

/**
********************************************************************************
 *  @fn     BUFFMGR_DeInit
 *
 *  @brief  The BUFFMGR_DeInit function releases all memory allocated by buffer
 *          manager.
 *
 *  @param[in] numRefBufs : Number of buffers to be de-allocated.
 *  @param[in] numOutBufs : Number of buffers to be de-allocated.
 *
 *  @return None
********************************************************************************
*/
void BUFFMGR_DeInit(XDAS_Int32 numRefBufs, XDAS_Int32 numOutBufs)
{
  XDAS_Int32  idx;

  /*--------------------------------------------------------------------------*/
  /*  Freeing up all the Buffers                                              */
  /*--------------------------------------------------------------------------*/
  for(idx = 0; idx < MAX_BUFF_ELEMENTS; idx++)
  {
    free((void *)buffArray[idx].buf[0]);
    free((void *)buffArray[idx].buf[1]);
  }

  /*--------------------------------------------------------------------------*/
  /*  Freeing up meta data Buffers                                            */
  /*--------------------------------------------------------------------------*/
  if(metaDataEnable[APP_MB_INFO])
  {
    free((void *)buffArray[0].buf[2 + metaDataIndex[APP_MB_INFO]]);
  }

  return;
}

/**
********************************************************************************
*  @fn     SetPATViewMapBase
*
*  @brief  This function provides the base address, i.e the physical address
*          in external memory to be used as tiler memory.
*
*  @return None
********************************************************************************
*/
void SetPATViewMapBase()
{
  /*--------------------------------------------------------------------------*/
  /* For Netra Simulator PAT register programming is required for TILER view  */
  /* mapping. For EVM, GEL files already has done the same mapping            */
  /*--------------------------------------------------------------------------*/
#ifdef SIMULATOR_RUN
#ifdef NETRA
  *(int*)0x4E000460 = 0xA0000000;
#endif /* NETRA*/
#endif /* SIMULATOR_RUN*/

#ifndef NETRA
  /*--------------------------------------------------------------------------*/
  /* For OMAP below register programming will enable the TILER view and       */
  /* maps 0x90000000 as TILER base address                                    */
  /*--------------------------------------------------------------------------*/
  *(int *)(0x4E000440) = 0x05040302;
  *(int *)(0x4E000444) = 0x05040302;
  *(int *)(0x4E000448) = 0x05040302;
  *(int *)(0x4E00044C) = 0x05040302;
  *(int *)(0x4E000460) = 0x80000000;
  *(int *)(0x4E000480) = 0x00000003;
  *(int *)(0x4E0004C0) = 0x0000000B;
  *(int *)(0x4E000504) = 0x3FFF20E0;
  *(int *)(0x4E00050C) = 0x8510F010;
#endif /* NETRA*/
}

/**
********************************************************************************
 *  @fn     testAppTilerInit
 *
 *  @brief  The testAppTilerInit function initializes & chunks the TILER
 *          memory into required number with required 2D block sizes.
 *
 *  @param[in] bufSizes  : Address of array containing the buffer sizes of the
 *                         numbufs.
 *
 *  @return None
********************************************************************************
*/
XDAS_Void testAppTilerInit(XDM2_BufSize *bufSizes)
{
  XDAS_Int8    ret_val;
  /*-----------------------------------------------------------------------*/
  /*  Get the buffers in TILER space                                       */
  /*-----------------------------------------------------------------------*/
  if(appControlPrms.tilerEnable)
  {
    /*---------------------------------------------------------------------*/
    /* Get the buffers in TILER space                                      */
    /* Do the allocation for maximum supported resoultion and keep to some */
    /* aligned boundary. Also this allocation assumes that tiled8 and      */
    /* tiled16 are mapped to same physical space                           */
    /*---------------------------------------------------------------------*/
    sTilerParams *pTilerParams     = &tilerParams;

    SetPATViewMapBase();

    pTilerParams->tilerSpace[0]    = CONTAINER_8BITS;
    pTilerParams->tilerSpace[1]    = CONTAINER_16BITS;
    if(appControlPrms.chromaTilerMode)
    {
      pTilerParams->tilerSpace[1]  = CONTAINER_8BITS;
    }

    pTilerParams->imageDim[0]      = bufSizes[0].tileMem.width;
    pTilerParams->imageDim[1]      = bufSizes[0].tileMem.height;
    pTilerParams->totalSizeLuma    = 0x4800000;

    pTilerParams->memoryOffset[0]  = 0;
    pTilerParams->memoryOffset[1]  = pTilerParams->totalSizeLuma;

    ret_val = tiler_init(pTilerParams);
    if(ret_val == -1)
    {
      /*-------------------------------------------------------------------*/
      /* If tiler allocation is not successful then Force both the buffer  */
      /* to be in raw region                                               */
      /*-------------------------------------------------------------------*/
      appControlPrms.tilerEnable = 0 ;
    }
  }
}

