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
 *****************************************************************************
 * @file  mpeg4enc_ti_config.c
 *
 * @brief This File contains function definitions for a standard
 *        implementation of a test configuration file parser.
 *        These functiosn parses the input  configuration files and
 *        assigns user provided values to global instance structures
 *
 *
 * @author: Radhesh Bhat
 *
 * @version 0.0 (Mar 2010) : Base version created
 *                           [Radhesh Bhat]
 *****************************************************************************
*/

/* -------------------- compilation control switches -------------------------*/
/* -------------------- system and platform files ----------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/*--------------------- program files ----------------------------------------*/
#include <TestAppComDataType.h>
#include <ti/xdais/dm/ividenc2.h>
#include <mpeg4enc.h>
#include <mpeg4enc_ti.h>
#include "buffermanager.h"
#include <MPEG4SPEnc_rman_config.h>
#ifdef USE_DSS_SCRIPTING
#include "pci.h"
#endif
#include <mpeg4enc_ti_config.h>
#include <mpeg4enc_ti_test.h>

#ifdef A9_HOST_FILE_IO
#include <mpeg4enc_ti_file_io.h>
#include "mpeg4enc_ti_init_ivahd_clk.h"
#endif


/*----------------------------------------------------------------------------*/
/*Instance of the config structure                                            */
/*----------------------------------------------------------------------------*/
MPEG4EncoderConfig  gConfig;

/*----------------------------------------------------------------------------*/
/* Instance creation Parameter - XDM                                          */
/*----------------------------------------------------------------------------*/
MPEG4ENC_Params gParams;

/*----------------------------------------------------------------------------*/
/* Dynamic Parameter struct- XDM                                              */
/*----------------------------------------------------------------------------*/
MPEG4ENC_DynamicParams gDynamicParams;

/*----------------------------------------------------------------------------*/
/* Pointer for storing the last address given to codec for getBuffer().       */
/*----------------------------------------------------------------------------*/
S32 *baseAddrGetBuffer;

extern U08 *output_buffer;
extern U08 dataSyncOutputBuffer[];
extern S08 datasync_bitstreamname[];
extern U32 *datasynchNumBlocks;

/*
 * Maximum number of blocks that the app can give to codec.
 */
#define MAX_SLICE 15

sProfileLog gProfileLog;

static sTokenMapping sTokenMap[100] ;
/*----------------------------------------------------------------------------*/
/*parsing of the file fills this buffer                                       */
/*----------------------------------------------------------------------------*/
static XDAS_Int8 buf[20000];

/*--------------------------------------*/
/* bit-stream data sync global varialbe */
/*--------------------------------------*/
U32 gInitialTime = 0;
U32 *gSlicesBase[NUMBER_OF_SLICES];
U32 gSlicesBaseIndex = 0;
U32 gSlicesSize[NUMBER_OF_SLICES];
U32 gSlicesSizeIndex = 0;

U32 gSliceBaseStitch;
U32 numberOfBitsStitched = 0;
S16 stitchBitPtr;
S16 stitchBufPtr = 0;
U08 *srcDataBuf;
U08 *dstDataBuf;
U32 lastPaddedBits = 0;


/*----------------------------------------------------------------------------*/
/* Error strings which are mapped to codec errors                             */
/* Please refer User guide for more details on error strings                  */
/*----------------------------------------------------------------------------*/
static sErrorMapping gErrorStrings[32] = 
{
  (XDAS_Int8 *)"IMPEG4ENC_LEVEL_INCOMPLAINT_PARAMETER , 0, \0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 1,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 2,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 3,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 4,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 5,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 6,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 7,\0",
  (XDAS_Int8 *)"XDM_PARAMSCHANGE = 8,\0",
  (XDAS_Int8 *)"XDM_APPLIEDCONCEALMENT = 9,\0",
  (XDAS_Int8 *)"XDM_INSUFFICIENTDATA = 10,\0",
  (XDAS_Int8 *)"XDM_CORRUPTEDDATA = 11,\0",
  (XDAS_Int8 *)"XDM_CORRUPTEDHEADER = 12,\0",
  (XDAS_Int8 *)"XDM_UNSUPPORTEDINPUT = 13,\0",
  (XDAS_Int8 *)"XDM_UNSUPPORTEDPARAM = 14,\0",
  (XDAS_Int8 *)"XDM_FATALERROR = 15\0",
  (XDAS_Int8 *)"IMPEG4ENC_IMPROPER_HDVICP2_STATE = 16\0",
  (XDAS_Int8 *)"IMPEG4ENC_WARNING_H263_PLUS_CUSTOM_SOURCE_FORMAT = 17\0",
  (XDAS_Int8 *)"IMPEG4ENC_ERROR_BITSTREAM_MEMORY_INSUFFICIENT = 18,\0",
  (XDAS_Int8 *)"IMPEG4ENC_IMPROPER_DATASYNC_SETTING = 19,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_VIDENC2PARAMS = 20,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_RATECONTROLPARAMS = 21,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_INTERCODINGPARAMS = 22,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_INTRACODINGPARAMS = 23,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 24,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_SLICECODINGPARAMS = 25,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 26,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 27,\0",
  (XDAS_Int8 *)"IMPEG4ENC_RESERVED_ERROR_BIT = 28,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_MPEG4ENCPARAMS = 29,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_VIDENC2DYNAMICPARAMS = 30,\0",
  (XDAS_Int8 *)"IMPEG4ENC_UNSUPPORTED_MPEG4ENCDYNAMICPARAMS = 31, \0"
};



/**
********************************************************************************
 *  @func     MPEG4E_TI_PerformStitching
 *  @brief  Incase of H.263 with H.241, performs the stitching of bitstreams
 *          between two packets which are seperated by GSTUFF bits..
 *
 *  @param[in]  dataSyncDesc : Pointer to the data sync descriptor
 *
 *  @param[in]  blockSizes :   Size of the packet in bits
 *  @return None
********************************************************************************
*/
XDAS_Void MPEG4E_TI_PerformStitching(XDM_DataSyncDesc *dataSyncDescrSliceMode,
  U32 blockSizes)
{
  U32 actualBits;
  U32 postPaddedBits;
  U32 i;
  U32 numberOfBytes;


/*---------------------------------------------------------------------------*/
/*           GSTUFF bits                  GSTUFF bits                        */
/*             |                              |                              */
/*             |                              |                              */
/*             v                              v                              */
/*         <-------->                     <-------->                         */
/*            +---------------  packet boundary                              */
/*            |                                                              */
/*            |      Actual bits encoded                                     */
/*            |      within standard syntax                                  */
/*            v     <-------------------->                                   */
/*            +=================================+====================+       */
/*         ***|*****|                    |******|**|          |******|       */
/*            |     |                    |      |  |          |      |       */
/*            +=================================+====================+       */
/*                         PACKET#N                  PACKET#N+1              */
/*                                                                           */
/*         <-><-------------------------><------>                            */
/*          ^             ^                 ^                                */
/*          |             |                 |                                */
/*          |             |                 |                                */
/*          |             |                 |                                */
/*          |             |              postPaddedBits                      */
/*          |          No.of bits given                                      */
/*          |            by codec                                            */
/*   lastPaddedBits                                                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*  The actual bits can be extracted by lastPaddedBits and postPaddedBits    */
/*  The postPaddedBits can be calculated by the blockSize given by the codec */
/*  by looking whether block size is multiple of 8 or not.                   */
/*                                                                           */
/*  For the PACKET #N+1, lastPaddedBits is nothing but the postPaddedBits of */
/*  PACKET #N                                                                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
  {
    actualBits =  blockSizes;
    postPaddedBits = (((blockSizes + 7) >> 3) << 3) - blockSizes;
    if(lastPaddedBits)
      actualBits -= (8 - lastPaddedBits);

    numberOfBytes = (blockSizes + postPaddedBits) >> 3;

    /*-----------------------------------------------------------------------*/
    /* Get the destination base address where stitched bitstream             */
    /* needs to be put                                                       */
    /*-----------------------------------------------------------------------*/
    if(gSlicesBaseIndex == 0)
    {
      dstDataBuf = (U08 *)gSlicesBase[gSlicesBaseIndex];
    }

    /*-----------------------------------------------------------------------*/
    /* Get the base address of the bitstream                                 */
    /*-----------------------------------------------------------------------*/
    srcDataBuf = (U08 *)gSlicesBase[gSlicesBaseIndex];

    if(gSlicesBaseIndex !=0)
    {
      if(lastPaddedBits)
      {
        dstDataBuf[stitchBufPtr++] |=
          (srcDataBuf[0] << lastPaddedBits) >> lastPaddedBits;

        for(i=1;i<(numberOfBytes - 1);i++)
        {
          dstDataBuf[stitchBufPtr++] = srcDataBuf[i];
        }
        dstDataBuf[stitchBufPtr] = srcDataBuf[i];
        if(!postPaddedBits)
        {
          stitchBufPtr++;
        }
      }
      else
      {
        for(i=0;i<(numberOfBytes - 1);i++)
        {
          dstDataBuf[stitchBufPtr++] = srcDataBuf[i];
        }
        dstDataBuf[stitchBufPtr] = srcDataBuf[i];
        if(!postPaddedBits)
        {
          stitchBufPtr++;
        }
      }

    }
    else
    {
      stitchBufPtr = blockSizes >> 3;
    }
     /*-----------------------------------------------------------------------*/
    /* Assign the post padded bits of previous packet to be the              */
     /* last padded bits for the current packet                               */
    /*-----------------------------------------------------------------------*/
    lastPaddedBits = postPaddedBits;

  }
}

/**
********************************************************************************
 *  @func     StitchTwoPackets
 *  @brief    Function to stitch two H.263 stuffed packets
 *
 *  @param[in]  dst     : Pointer to the destination buffer where the stitched
 *                        packets are to be stored
 * 
 *  @param[in]  src     : Pointer to the source buffer(Second packet base addr)
 * 
 *  @param[in]  dstBits : Number of valid bits in the first packet
 * 
 *  @param[in]  srcBits : Number of valid bits in the second packet. This bits
 *                        include the initial stuff bits too.
 *
 *  @return   Valid bits in the bitstream after stitching
********************************************************************************
*/
int StitchTwoPackets(unsigned char *dst,
                     unsigned char *src,
                     int  dstBits,
                     int  srcBits)
{
  int dstBytePos, stuffedBitsInDst, numBytes, srcBytePos;
  
/*---------------------------------------------------------------------------*/
/*           GSTUFF bits                  GSTUFF bits                        */
/*             |                              |                              */
/*             |                              |                              */
/*             v                              v                              */
/*         <-------->                     <-------->                         */
/*            +---------------  packet boundary                              */
/*            |                                                              */
/*            |      Actual bits encoded                                     */
/*            |      within standard syntax                                  */
/*            v     <-------------------->                                   */
/*            +=================================+====================+       */
/*         ***|*****|                    |******|**|          |******|       */
/*            |     |                    |      |  |          |      |       */
/*            +=================================+====================+       */
/*                         PACKET#N                  PACKET#N+1              */
/*                                                                           */
/*         <-><-------------------------><------>                            */
/*          ^             ^                 ^                                */
/*          |             |                 |                                */
/*          |             |                 |                                */
/*          |             |                 |                                */
/*          |             |              postPaddedBits                      */
/*          |          No.of bits given                                      */
/*          |            by codec                                            */
/*   lastPaddedBits                                                          */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*  The actual bits can be extracted by lastPaddedBits and postPaddedBits    */
/*  The postPaddedBits can be calculated by the blockSize given by the codec */
/*  by looking whether block size is multiple of 8 or not.                   */
/*                                                                           */
/*  For the PACKET #N+1, lastPaddedBits is nothing but the postPaddedBits of */
/*  PACKET #N                                                                */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
  
  /*-------------------------------------------------------------------------*/
  /* Calculate the destination memory location where we need to stitch.      */
  /* Also calculate the number of stuffed bits.                              */
  /*-------------------------------------------------------------------------*/
  srcBytePos = 0;
  dstBytePos = dstBits >> 3;
  stuffedBitsInDst = (((dstBytePos + 1) << 3) - dstBits) & 0x7;
  
  if(stuffedBitsInDst)
  {
    unsigned char data;
    
    data = *src & ((1 << stuffedBitsInDst) - 1);
    dst[dstBytePos] |= data;
    ++dstBytePos;
    ++srcBytePos;
  }
  
  numBytes = ((srcBits + 0x7) >> 3) - (stuffedBitsInDst != 0);
  memcpy((char*)((U32)dst + dstBytePos),
         (char*)((U32)src + srcBytePos),
         numBytes);
  
  return((srcBits + dstBits) - (dstBits & 0x7));
}

/**
********************************************************************************
 *  @func     StitchAllPackets
 *  @brief    Function to stitch all the H.263 stuffed packets
 *
 *  @param[in]  None
 *
 *  @return   Valid bits in the bitstream after stitching
********************************************************************************
*/
XDAS_Int32 StitchAllPackets()
{
  short block;
  U32 *src;
  unsigned char *dst;
  int dstBits, srcBits;
  
  dst = (unsigned char*)&dataSyncOutputBuffer[0];
  srcBits = dstBits = 0;
  
  for(block = 0; block < gSlicesBaseIndex; block++)
  {
    srcBits = gSlicesSize[block];
    src = (U32*)gSlicesBase[block];
    dstBits = StitchTwoPackets(dst, (unsigned char*)src, dstBits, srcBits);
  }
  
  return((dstBits + 0x7) >> 3);
}

/**
********************************************************************************
 *  @func     MPEG4E_TI_Update_getDataCallBack
 *  @brief  Append one or more number of input rows into the frame buffer and
 *          update the numBlocks equal to the total number of rows has been
 *          filled into the frame buffer.
 *          codec will do a call back as soon as it completed encoding the
 *          number of rows available in frame buffer.
 *          Codec assumes atleast one row appended per a call back.

 *
 *  @param[in]  ialg_handle  : Pointer to the IALG_Handle structure
 *
 *  @param[in]  dataSyncDesc : Pointer to the data sync descriptor
 *
 *  @return None
********************************************************************************
*/
XDAS_Void MPEG4E_TI_Update_getDataCallBack(
  IALG_Handle ialg_handle,
  XDM_DataSyncDesc *dataSyncDescrRowLevel
)
{
  static int count = 0;
  if (gParams.videnc2Params.inputDataMode == IVIDEO_NUMROWS)
  {
    /*----------------------------------------------------------*/
    /* get the input to frame buffer and update the numBlocks.  */
    /* numBlocks should be total number of input rows available */
    /* from the start of the frame, codec internally knows the  */
    /* number of rows it has encoded already.                   */
    /* codec assumes atleast one row appended per a call back.  */
    /*----------------------------------------------------------*/
    if(gConfig.datasynchFilename[0] == '\0')
    {
      dataSyncDescrRowLevel->numBlocks = 1;
    }
    else
    {
      dataSyncDescrRowLevel->numBlocks = datasynchNumBlocks[count++];
      count = count % 8;
    }
  }

  return;
}

/**
********************************************************************************
 *  @func     MPEG4E_TI_Update_getBufferCallBack
 *  @brief  Encoder will do a CallBack when ever there is no buffers available
 *          for further encode operation.
 *          Encoder assumes the buffer sizes will be in multiples of 2048 Bytes.
 *          Encoder expects atleast one buffer per CallBack.
 *
 *  @param[in]  ialg_handle  : Pointer to the IALG_Handle structure
 *
 *  @param[in]  dataSyncDesc : Pointer to the data sync descriptor
 *
 *  @return     Zero if memory is provided to codec, -1 if memory is not
 *              available
********************************************************************************
*/
XDAS_Int32 MPEG4E_TI_Update_getBufferCallBack(
  IALG_Handle ialg_handle,
  XDM_DataSyncDesc *dataSyncDescrGetBuffer
)
{
  /*-------------------------------------------------------------------------*/
  /* Flag to tell whether the memory to give to the codec is continuous or   */
  /* non-continuous. If set to 1, it means continuous memory.                */
  /*-------------------------------------------------------------------------*/
  U32 isContinuous = 1;
  /*-------------------------------------------------------------------------*/
  /* Number of blocks to give to the codec. For sliceMode, the blockSizes    */
  /* will be the multiple of sliceSizes. For others, the blockSizes will be  */
  /* multiple of 1024.                                                       */
  /*-------------------------------------------------------------------------*/
  static U32 numBlocks = 8;
  /*-------------------------------------------------------------------------*/
  /* Number of pages in each block. For sliceMode, the blockSizes will be the*/
  /* multiple of sliceSizes. For others, the blockSizes will be multiple of  */
  /* 1024.                                                                   */
  /*-------------------------------------------------------------------------*/
  U32 numPagesEachBlock = 1;
  /*-------------------------------------------------------------------------*/
  /* Array of pointers for storing the base address and block sizes of each  */
  /* data unit/block.                                                        */
  /*-------------------------------------------------------------------------*/
  S32 *baseAddr[MAX_SLICE];
  S32 blockSizes[MAX_SLICE];
  U32 blockSize;
  U32 nonContiguousSpace = 500;
  U32 initBufferSize;
  
  /*-------------------------------------------------------------------------*/
  /* Initialize the block size according to the data sync.                   */
  /*-------------------------------------------------------------------------*/
  if((gParams.sliceCodingParams.sliceMode == IMPEG4_SLICEMODE_BITS) &&
     (gParams.videnc2Params.outputDataMode == IVIDEO_SLICEMODE))
  {
    blockSize = gParams.sliceCodingParams.sliceUnitSize >> 3;
    /*-----------------------------------------------------------------------*/
    /* At the frame start, the first packet size would include the Sequence  */
    /* headers(VO, VOS, VOL). Therefore the first packet size would be       */
    /* greater than the slice unit size. So to avoid bitstream corruption    */
    /* we are adding some extra bytes.                                       */
    /*-----------------------------------------------------------------------*/
    initBufferSize = blockSize + 100;
  }
  else
  {
    blockSize = 1024;
    initBufferSize = INIT_BUFFER_SIZE;
  }
  
  /*-------------------------------------------------------------------------*/
  /* Force to continuous memory in case of entire frame mode or fixed MB     */
  /* slice mode data sync                                                    */
  /*-------------------------------------------------------------------------*/
  if((gParams.videnc2Params.outputDataMode == IVIDEO_ENTIREFRAME) ||
     ((gParams.sliceCodingParams.sliceMode == IMPEG4_SLICEMODE_BITS) &&
     (gParams.videnc2Params.outputDataMode == IVIDEO_SLICEMODE)))
  {
    isContinuous = 1;
    initBufferSize = INIT_BUFFER_SIZE;
  }
  
  if(isContinuous)
  {
    // Giving continuous memory to the codec in each getBuffer call
    if(gInitialTime == 0)
    {
      /*---------------------------------------------------------------------*/
      /* At the frame start we have given INIT_BUFFER_SIZE amount of memory. */
      /* So increment base address by INIT_BUFFER_SIZE for the second block. */
      /*---------------------------------------------------------------------*/
      gInitialTime = 1;
      baseAddrGetBuffer = (S32*) ((U32)baseAddrGetBuffer + initBufferSize);
      baseAddr[0] = (S32*) baseAddrGetBuffer;
      blockSizes[0] = numBlocks * blockSize;
      dataSyncDescrGetBuffer->numBlocks = 1;
      dataSyncDescrGetBuffer->scatteredBlocksFlag = 0;
      dataSyncDescrGetBuffer->varBlockSizesFlag = 0;
    }
    else
    {
      /*---------------------------------------------------------------------*/
      /* Each call to this function would give different kind of data sync   */
      /* descriptors to the codec.                                           */
      /*---------------------------------------------------------------------*/
      baseAddrGetBuffer = (S32*) ((U32)baseAddrGetBuffer +
                                  (numBlocks * blockSize));
      if((gInitialTime & 0x3) == 0x3)
      {
        // One baseAddr and one blockSize
        baseAddr[0] = (S32*) baseAddrGetBuffer;
        blockSizes[0] = numBlocks * blockSize;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 0;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 0;
        dataSyncDescrGetBuffer->numBlocks = 1;
      }
      else if((gInitialTime & 0x2) == 0x2)
      {
        U32 i;
          
        // One baseAddr and multiple blockSizes
        baseAddr[0] = (S32*) baseAddrGetBuffer;
        for(i = 0; i < (numBlocks / numPagesEachBlock); i++)
        {
          blockSizes[i] = numPagesEachBlock * blockSize;
        }
        dataSyncDescrGetBuffer->numBlocks = i;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 0;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 1;
      }
      else if((gInitialTime & 0x1) == 0x1)
      {
        U32 i;
          
        // One blockSizes and multiple baseAddr case
        blockSizes[0] = numPagesEachBlock * blockSize;
        for(i = 0; i < (numBlocks / numPagesEachBlock); i++)
        {
          baseAddr[i] = (S32*)baseAddrGetBuffer;
          baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer +
             (U32)(numPagesEachBlock * blockSize));
        }
        baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer -
          (U32)(numBlocks * blockSize));
        dataSyncDescrGetBuffer->numBlocks = i;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 1;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 0;
      }
      else
      {
        U32 i;
          
        // Multiple baseAddr and multiple blockSizes case
        for(i = 0; i < (numBlocks / numPagesEachBlock); i++)
        {
          baseAddr[i] = (S32*)baseAddrGetBuffer;
          blockSizes[i] = numPagesEachBlock * blockSize;
          baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer +
                                      (U32) blockSizes[i]);
        }
        baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer -
                                    (U32) (numBlocks * blockSize));
        dataSyncDescrGetBuffer->numBlocks = i;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 1;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 1;
      }
      /*---------------------------------------------------------------------*/
      /* Increment the getBuffer call counter. This is just for testing the  */
      /* codec behavior. This counter is just for giving different types of  */
      /* data sync descriptors to the codec.                                 */
      /*---------------------------------------------------------------------*/
      ++gInitialTime;
    }
  }
  else
  {
    // Giving non-contiguous memory to the codec in each getBuffer call
    if(gInitialTime == 0)
    {
      /*---------------------------------------------------------------------*/
      /* At the frame start we have given INIT_BUFFER_SIZE amount of memory. */
      /* So increment base address by INIT_BUFFER_SIZE for the second block. */
      /*---------------------------------------------------------------------*/
      gInitialTime = 1;
      baseAddrGetBuffer = (S32*) ((U32)baseAddrGetBuffer + initBufferSize +
                                       nonContiguousSpace);
      baseAddr[0] = (S32*) baseAddrGetBuffer;
      blockSizes[0] = numBlocks * blockSize;
      dataSyncDescrGetBuffer->numBlocks = 1;
      dataSyncDescrGetBuffer->scatteredBlocksFlag = 0;
      dataSyncDescrGetBuffer->varBlockSizesFlag = 0;
    }
    else
    {
      /*---------------------------------------------------------------------*/
      /* Each call to this function would give different kind of data sync   */
      /* descriptors to the codec.                                           */
      /*---------------------------------------------------------------------*/
      baseAddrGetBuffer = (S32*) ((U32)baseAddrGetBuffer +
                                  (numBlocks * blockSize) +
                                  nonContiguousSpace);
      if((gInitialTime & 0x3) == 0x3)
      {
        // One baseAddr and one blockSize
        baseAddr[0] = (S32*) baseAddrGetBuffer;
        blockSizes[0] = numBlocks * blockSize;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 0;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 0;
        dataSyncDescrGetBuffer->numBlocks = 1;
      }
      else if((gInitialTime & 0x2) == 0x2)
      {
        U32 i;
          
        // One baseAddr and multiple blockSizes
        baseAddr[0] = (S32*) baseAddrGetBuffer;
        for(i = 0; i < (numBlocks / numPagesEachBlock); i++)
        {
          blockSizes[i] = numPagesEachBlock * blockSize;
        }
        dataSyncDescrGetBuffer->numBlocks = i;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 0;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 1;
      }
      else if((gInitialTime & 0x1) == 0x1)
      {
        U32 i;
          
        // One blockSizes and multiple baseAddr case
        blockSizes[0] = numPagesEachBlock * blockSize;
        for(i = 0; i < (numBlocks / numPagesEachBlock); i++)
        {
          baseAddr[i] = (S32*)baseAddrGetBuffer;
          baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer +
             (U32)(numPagesEachBlock * blockSize));
        }
        baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer -
          (U32)(numBlocks * blockSize));
        dataSyncDescrGetBuffer->numBlocks = i;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 1;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 0;
      }
      else
      {
        U32 i;
          
        // Multiple baseAddr and multiple blockSizes case
        for(i = 0; i < (numBlocks / numPagesEachBlock); i++)
        {
          baseAddr[i] = (S32*)baseAddrGetBuffer;
          blockSizes[i] = numPagesEachBlock * blockSize;
          baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer +
                                      (U32) blockSizes[i]);
        }
        baseAddrGetBuffer = (S32*) ((U32) baseAddrGetBuffer -
                                    (U32) (numBlocks * blockSize));
        dataSyncDescrGetBuffer->numBlocks = i;
        dataSyncDescrGetBuffer->scatteredBlocksFlag = 1;
        dataSyncDescrGetBuffer->varBlockSizesFlag = 1;
      }
      /*---------------------------------------------------------------------*/
      /* Increment the getBuffer call counter. This is just for testing the  */
      /* codec behavior. This counter is just for giving different types of  */
      /* data sync descriptors to the codec.                                 */
      /*---------------------------------------------------------------------*/
      ++gInitialTime;
    }
  }
    
  /*-------------------------------------------------------------------------*/
  /* Assign the blockSizes pointer of the data sync descriptor to blockSizes */
  /* array.                                                                  */
  /*-------------------------------------------------------------------------*/
  dataSyncDescrGetBuffer->blockSizes = (XDAS_Int32*) blockSizes;
  
  /*-------------------------------------------------------------------------*/
  /* If scattered is 0, dataSyncDescrGetBuffer->baseAddr will point to the   */
  /* base address of the buffer else it's an array of pointers and each      */
  /* pointer will hold the starting address of the scattered buffer.         */
  /*-------------------------------------------------------------------------*/
  if(dataSyncDescrGetBuffer->scatteredBlocksFlag)
  {
    dataSyncDescrGetBuffer->baseAddr = (XDAS_Int32*) baseAddr;
  }
  else
  {
    dataSyncDescrGetBuffer->baseAddr = (XDAS_Int32*) baseAddr[0];
  }
  

  return 0;
}


/**
********************************************************************************
 *  @func     MPEG4E_TI_Update_putDataCallBack
 *  @brief  Accept the no.of slices and slice sizes from the encoder and store
 *          them in to an array.
 *
 *  @param[in]  ialg_handle  : Pointer to the IALG_Handle structure
 *
 *  @param[in]  dataSyncDesc : Pointer to the data sync descriptor
 *
 *  @return None
********************************************************************************
*/
MY_FILE *fout2;
XDAS_Int32 MPEG4E_TI_Update_putDataCallBack(
  IALG_Handle ialg_handle,
  XDM_DataSyncDesc *dataSyncDescrSliceMode
)
{
  {
    char* baseAddr;
    U32* blockSizes;
    U32 numBlocks, i, blockSize = 0, isScattered;
  

  /*-------------------------------------------------------------------------*/  
  /* Dump Other information given out for Mode B Packetization is a log file */
  /*-------------------------------------------------------------------------*/  
	{
		FILE *ftemp;
        IMPEG4ENC_DataSyncDesc *dsPtr =
			(IMPEG4ENC_DataSyncDesc*)dataSyncDescrSliceMode;

		if((ftemp = fopen("Log.txt", "a")) == NULL)
		{
			printf("Error: Opening log file.\n");
		}

		for(i = 0; i < dataSyncDescrSliceMode->numBlocks; ++i)
		{
      fprintf(ftemp, "%d, %d, %d, (%d, %d)\n",
		    dsPtr->mbAddr[i],
		   	dsPtr->gobNumber[i],
			  dsPtr->quantScale[i],
				(S16)(dsPtr->mv[i] & 0xFFFF),
				(S16)((dsPtr->mv[i] & 0xFFFF0000) >> 16));
		}

		fclose(ftemp);
	}
    blockSizes = (U32*)&dataSyncDescrSliceMode->blockSizes[0];
    numBlocks = dataSyncDescrSliceMode->numBlocks;
    isScattered = dataSyncDescrSliceMode->scatteredBlocksFlag;
    /*************************************************************************/
    /* Fix for SDOCM00077402                                                 */
    /* Data sync output file is not created if data sync is enabled for      */
    /* certain test cases in case of MPEG-4.                                 */
    /*************************************************************************/
    /*
    if(count == 0)
    {
      if((fout2 = MY_FOPEN(datasync_bitstreamname, "wb")) == NULL)
      {
      printf("Error: Opening temp m4v file.\n");
      return -1;
      }
      count = 1;
    }*/
    
    /*-----------------------------------------------------------------------*/
    /* Data sync descriptors can be of 4 types as described below.           */
    /* 1) One base address and one block size                                */
    /* 2) Multiple base addresses and one block size                         */
    /* 3) One base address and multiple block sizes                          */
    /* 4) Multiple base addresses and multiple block sizes                   */
    /* The above cases are based on 2 flags in the data sync descriptors.    */
    /* Data sync descriptors shall have multiple base addresses if the       */
    /* scatteredBlocksFlag is set to TRUE otherwise one base address.        */
    /* Data sync descriptor shall have multiple block sizes if the           */
    /* varBlockSizesFlag is set to TRUE.                                     */
    /* If scatteredBlocksFlag is set to FALSE, then there shall be only one  */
    /* base address else the number of base address is equal to numBlocks.   */
    /* If varBlockSizesFlag is set to FALSE, then there shall be only one    */
    /* blockSizes else the number of blockSizes is equal to numBlocks.       */
    /*-----------------------------------------------------------------------*/
    if(isScattered)
    {
      S32 *baseAddr;
      
      baseAddr = (S32*)&dataSyncDescrSliceMode->baseAddr[0];
      if(dataSyncDescrSliceMode->varBlockSizesFlag)
      {
        /*-------------------------------------------------------------------*/
        /* Multiple base address and multiple block sizes case               */
        /*-------------------------------------------------------------------*/
        for(i = 0; i < numBlocks; i++)
        {
          MY_FWRITE((void*)baseAddr[i], 1, blockSizes[i], fout2);
          gSlicesBase[gSlicesBaseIndex] = (U32*)baseAddr[i];
          gSlicesSize[gSlicesSizeIndex] = blockSizes[i];
          ++gSlicesBaseIndex;
          ++gSlicesSizeIndex;
        }
      }
      else
      {
        /*-------------------------------------------------------------------*/
        /* Multiple base address and one block sizes case                    */
        /*-------------------------------------------------------------------*/
        for(i = 0; i < numBlocks; i++)
        {
          MY_FWRITE((void*)baseAddr[i], 1, blockSizes[0], fout2);
          gSlicesBase[gSlicesBaseIndex] = (U32*)baseAddr[i];
          gSlicesSize[gSlicesSizeIndex] = blockSizes[0];
          ++gSlicesBaseIndex;
          ++gSlicesSizeIndex;
        }
      }
    }
    else // Not scattered
    {
      baseAddr = (char*)dataSyncDescrSliceMode->baseAddr;
      if(dataSyncDescrSliceMode->varBlockSizesFlag)
      {
        /*-------------------------------------------------------------------*/
        /* One base address and multiple block sizes case                    */
        /*-------------------------------------------------------------------*/
        for(i = 0; i < numBlocks; i++)
        {
          blockSize += blockSizes[i];
        }
        MY_FWRITE(baseAddr, 1, blockSize, fout2);
        gSlicesBase[gSlicesBaseIndex] = (U32*)baseAddr;
        gSlicesSize[gSlicesSizeIndex] = blockSize;
        ++gSlicesBaseIndex;
        ++gSlicesSizeIndex;
      }
      else
      {
        /*-------------------------------------------------------------------*/
        /* One base address and one block sizes case                         */
        /*-------------------------------------------------------------------*/
        blockSize = numBlocks * blockSizes[0];
        MY_FWRITE(baseAddr, 1, blockSize, fout2);
        gSlicesBase[gSlicesBaseIndex] = (U32*)baseAddr;
        gSlicesSize[gSlicesSizeIndex] = blockSize;
        ++gSlicesBaseIndex;
        ++gSlicesSizeIndex;
      }
    }
  }
  
  return 0;
}
/** 
********************************************************************************
 *  @fn     MPEG4ENC_TI_Scratch_Contaminate
 *  @brief  This function will currupt scratch data in IVAHD buffers
 *          
 *          This function will currupt DTCM and other SL2 memory. This function
 *          does not contaminate const and stack area of ICONTs
 *          NOTE: Here the addresses depend on ICONT memory map, so when ever
 *          we change ICONT command file we need to change the hot 
 *          coded address
 *  
 *
 *  @return     None
********************************************************************************
*/

void MPEG4ENC_TI_Scratch_Contaminate(XDAS_Int32 hdvicp_status)
{

   XDAS_Int8 pattren[5] = {0x0,0x55,0xAA,0x99, 0xFF};
   static XDAS_Int8 index = 0;
  /*--------------------------------------------------------------------------*/
  /* Check for the scratch contamination flag at compile time                 */
  /*--------------------------------------------------------------------------*/  
  /*--------------------------------------------------------------------------*/
  /* Contaminate the total IVAHD module memory when FULL_CONTAMINATE flag is  */
  /* flag is enabled. Here the resource mode ID must be DIFFERENTCODEC so that*/
  /* every process call will update entire IVAHD persistant memorys           */
  /*--------------------------------------------------------------------------*/
  if(hdvicp_status == 2)
  {
    /*------------------------------------------------------------------------*/
    /* Currupt SL2 memory, it is of 256 KB                                    */
    /*------------------------------------------------------------------------*/
    memset ((Void*)(MEM_BASE), pattren[index], 256*1024);
    /*------------------------------------------------------------------------*/
    /* Currupt DTCM of both ICONTs                                            */
    /*------------------------------------------------------------------------*/
    memset ((Void*)(REG_BASE + ICONT1_DTCM_OFFSET), pattren[index],
                                                                     16 * 1024);
    memset ((Void*)(REG_BASE + ICONT2_DTCM_OFFSET), pattren[index],
                                                                     16 * 1024);
    /*------------------------------------------------------------------------*/
    /* Currupt ITCM of both ICONTs                                            */
    /*------------------------------------------------------------------------*/
    memset ((Void*)(REG_BASE + ICONT1_ITCM_OFFSET), pattren[index],
                                                                     32 * 1024);
    memset ((Void*)(REG_BASE + ICONT2_ITCM_OFFSET), pattren[index],
                                                                     32 * 1024);
  }
  else if((hdvicp_status == 0) || (hdvicp_status == 1))
  {
    /*------------------------------------------------------------------------*/
    /* If resource mode is not DIFFERENTCODEC currupt only part of DTCM all   */
    /* others being persistant memorys.                                       */
    /* Please note that we can do selective part of SL2 memory curruption, as */
    /* now we are not doing that                                              */
    /*------------------------------------------------------------------------*/
    /*------------------------------------------------------------------------*/
    /*    DTCM section in ICONT1_DTCM                                         */
    /*------------------------------------------------------------------------*/  
    memset ((Void*)(REG_BASE + ICONT1_DTCM_OFFSET + 0x1000),
                                                pattren[index], 0x00003000);  
    /*------------------------------------------------------------------------*/
    /*    DTCM section in ICONT2_DTCM                                         */
    /*------------------------------------------------------------------------*/  
    memset ((Void*)(REG_BASE + ICONT2_DTCM_OFFSET + 0x2900),
                                                pattren[index], 0x00001700);  
    
  }
  /*--------------------------------------------------------------------------*/  
  /* When the resource mode is samecodectype we can currupt total SL2 memory  */
  /*--------------------------------------------------------------------------*/  
  if(hdvicp_status == 1)
  {
    memset ((Void*)(MEM_BASE), pattren[index], 256*1024);
  }
  index++ ;
  if(index == 5)
  {
    index = 0;
  }

}
/**
********************************************************************************
 *  @func     TestApp_SetInitParams
 *  @brief  Init MPEG4 encoder parameters
 *
 *  @param[in]  params : Pointer to the MPEG4ENC_Params structure
 *
 *  @param[in]  config : Pointer to MPEG4EncoderConfig structure
 *
 *  @return None
********************************************************************************
*/
XDAS_Void TestApp_SetInitParams(
  MPEG4ENC_Params *params,
  MPEG4EncoderConfig *config
)
{
  /*-----------------------------------------------------------------------*/
  /* Initialize extended class structure size instead of base class / user */
  /* defined class structure size.                                         */
  /*-----------------------------------------------------------------------*/
  params->videnc2Params.size = sizeof(MPEG4ENC_Params);

  /*-----------------------------------------------------------------------*/
  /*  numInputDataUnits :  Number of input rows                            */
  /*-----------------------------------------------------------------------*/
  params->videnc2Params.profile = MPEG4_SIMPLE_PROFILE_IDC;/* Simple profile */
  params->videnc2Params.maxInterFrameInterval = NULL; /* No B frames */

  params->useVOS = 1;
  params->enableMONA = 0;
  params->enableAnalyticinfo = IVIDEO_METADATAPLANE_NONE;
  params->rateControlParams.vbvBufferSize = 0;
  params->rateControlParams.initialBufferLevel = 0;
  params->rateControlParams.qpMinIntra = 0;
  
  config->tilerSpace[0]= 0 ;
  config->tilerSpace[1] = 0 ;
  config->ivahdid = 0;
  config->forceSKIPPeriod = 0xFFFFFFFF;
  config->controlFilename[0] = '\0';
  config->datasynchFilename[0] = '\0';
  config->runTimeChangeFilename[0] = '\0';
  config->frameNumber = -1;

  return;

}


/**
********************************************************************************
 *  @func     TestApp_SetDynamicParams
 *  @brief  Init MPEG4 encoder Dynamic parameters
 *
 *  @param[in]  dynamicParams : Pointer to the MPEG4ENC_DynamicParams structure
 *
 *  @return None
********************************************************************************
*/
XDAS_Void TestApp_SetDynamicParams(MPEG4ENC_DynamicParams *dynamicParams)
{
  /*-----------------------------------------------------------------------*/
  /*  Set IVIDENC2 Dynamic parameters                                      */
  /*-----------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------*/
  /*  Size: sizeof(H264ENC_DynamicParams) incase of extended class  of     */
  /*  paramaters.                                                          */
  /*-----------------------------------------------------------------------*/
  dynamicParams->videnc2DynamicParams.size  = sizeof(MPEG4ENC_DynamicParams);

  /*-----------------------------------------------------------------------*/
  /*  Set Extended Parameters in IMPEG4VENC parameters                     */
  /*-----------------------------------------------------------------------*/
  memcpy (&((dynamicParams)->rateControlParams), &((gParams).rateControlParams),
    sizeof (IMPEG4ENC_RateControlParams));
  memcpy (&((dynamicParams)->interCodingParams), &((gParams).interCodingParams),
    sizeof (IMPEG4ENC_InterCodingParams));
  memcpy (&((dynamicParams)->sliceCodingParams), &((gParams).sliceCodingParams),
    sizeof (IMPEG4ENC_sliceCodingParams));

  dynamicParams->videnc2DynamicParams.interFrameInterval = 0; /* No B frames */

  /*-----------------------------------------------------------------------*/
  /* assign dataSynch callBack function pointers to encoder dynamicParams  */
  /*-----------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------*/
  /* NUM_ROWS                                                              */
  /*-----------------------------------------------------------------------*/
  dynamicParams->videnc2DynamicParams.getDataFxn =
    (XDM_DataSyncGetFxn)MPEG4E_TI_Update_getDataCallBack;

  dynamicParams->videnc2DynamicParams.getDataHandle = NULL;

  /*-----------------------------------------------------------------------*/
  /* FIXEDLENGTH                                                           */
  /*-----------------------------------------------------------------------*/
  dynamicParams->videnc2DynamicParams.getBufferFxn =
    (XDM_DataSyncGetFxn)MPEG4E_TI_Update_getBufferCallBack;

  dynamicParams->videnc2DynamicParams.getBufferHandle = NULL;

  /*-----------------------------------------------------------------------*/
  /* SLICEMODE                                                             */
  /*-----------------------------------------------------------------------*/
  dynamicParams->videnc2DynamicParams.putDataFxn =
    (XDM_DataSyncPutFxn)MPEG4E_TI_Update_putDataCallBack;

  dynamicParams->videnc2DynamicParams.putDataHandle = NULL;

  return;
}


/**
********************************************************************************
 *  @func     readparamfile
 *  @brief  Top Level function to read the parameter file.
 *
 *  @param[in]  fname : Pointer to the configuration file
 *
 *  @return     status ( PASS/ FAIL)
********************************************************************************
*/
XDAS_Int32 readparamfile(FILE * fname)
{
  XDAS_Int8 *FileBuffer = NULL ;
  XDAS_Int32 retVal ;

  /*----------------------------*/
  /*read the content in a buffer*/
  /*----------------------------*/
  FileBuffer = GetConfigFileContent(fname);

  if(FileBuffer)
  {
    retVal  = ParseContent(FileBuffer,strlen(FileBuffer));
    return retVal ;
  }
  else
  {
    return -1;
  }
}/* readparamfile */


/** 
********************************************************************************
 *  @fn     MPEG4ENC_TI_Report_Error
 *  @brief  This function will print error messages
 *          
 *          This function will check for codec errors which are mapped to 
 *          extended errors in videnc2status structure and prints them in cosole
 *          Returns XDM_EFAIL in case of fatal error
 *
 *  @param[in]  uiErrorMsg  : Extended error message
 *
 *
 *  @return     XDM_EOK -  when there is no fatal error
 *              XDM_EFAIL - when it is fatal error
********************************************************************************
*/

XDAS_Int32 MPEG4ENC_TI_Report_Error(XDAS_Int32 uiErrorMsg)
{
  int i;
  if(uiErrorMsg)
  {
    /*------------------------------------------------------------------------*/
    /* Loop through all the bits in error message and map to the glibal       */
    /* error string                                                           */
    /*------------------------------------------------------------------------*/
    for (i = 1; i < 32; i ++)
    {
      if (uiErrorMsg & (1 << i))
      {
        printf("ERROR: %s \n",  gErrorStrings[i].errorName);           
      }
    }
  }
  if (XDM_ISFATALERROR(uiErrorMsg))
  {
    return XDM_EFAIL;
  }
  else
  {
    return XDM_EOK;
  }
}

/**
********************************************************************************
 *  @func     GetConfigFileContent
 *  @brief  Reads the configuration file content in a buffer and returns the
 *          address of the buffer.
 *
 *  @param[in]  fname : Pointer to the configuration file
 *
 *  @return    FAIL if file error else address of buffer which contains parsed
 *             data of configuration file
********************************************************************************
*/
XDAS_Int8 *GetConfigFileContent (FILE *fname)
{
  XDAS_Int32 FileSize;

  if (0 != fseek (fname, 0, SEEK_END))
  {
    return 0;
  }

  FileSize = ftell (fname);
  if (FileSize < 0 || FileSize > 20000)
  {
    return 0;

  }
  if (0 != fseek (fname, 0, SEEK_SET))
  {
    return 0;
  }

  /*-------------------------------------------------------------------------*/
  /* Note that ftell() gives us the file size as the file system sees it.    */
  /* The actual file size, as reported by fread() below will be often smaller*/
  /* due to CR/LF to CR conversion and/or control characters after the dos   */
  /* EOF marker in the file.                                                 */
  /*-------------------------------------------------------------------------*/
  FileSize = fread (buf, 1, FileSize, fname);
  buf[FileSize] = '\0';
  fclose (fname);
  return buf;

}/* GetConfigFileContent */


/**
********************************************************************************
 *  @func     ParseContent
 *  @brief  Parses the character array buf and writes global variable input.
 *          This is necessary to facilitate the addition of new parameters
 *          through the sTokenMap[] mechanism.Need compiler-generated addresses
 *          in sTokenMap.
 *
 *  @param[in]  buf     : Pointer to the buffer to be parsed
 *
 *  @param[in]  bufsize : size of buffer
 *
 *  @return status ( PASS/ FAIL)
********************************************************************************
*/
XDAS_Int32 ParseContent (XDAS_Int8 *buf, XDAS_Int32 bufsize)
{
  XDAS_Int8 *items[MAX_ITEMS_TO_PARSE];
  XDAS_Int32 MapIdx;
  XDAS_Int32 item = 0;
  XDAS_Int32 InString = 0, InItem = 0;
  XDAS_Int8 *p = buf;
  XDAS_Int8 *bufend = &buf[bufsize];
  XDAS_Int32 IntContent;
  double DoubleContent;
  XDAS_Int32 i;
  FILE *fpErr = stderr;

  /*-------------------------------------------------------------------------*/
  /*                                 STAGE ONE                               */
  /*-------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------*/
  /* Generate an argc/argv-type list in items[], without comments and        */
  /* whitespace. This is context insensitive and could be done most easily   */
  /* with lex(1).                                                            */
  /*-------------------------------------------------------------------------*/
  while (p < bufend)
  {
    switch (*p)
    {
      case 13:
        p++;
        break;
      case '#':     /* Found comment */
        *p = '\0';  /* Replace '#' with '\0' in case of comment */
                    /* immediately following integer or string  */
        /* Skip till EOL or EOF, whichever comes first */
        while (*p != '\n' && p < bufend)
          p++;
        InString = 0;
        InItem = 0;
        break;
      case '\n':
        InItem = 0;
        InString = 0;
        *p++='\0';
        break;
      case ' ':
      case '\t':            /* Skip whitespace, leave state unchanged */
        if (InString)
          p++;
        else
        {                   /* Terminate non-strings once whitespace is found */
          *p++ = '\0';
          InItem = 0;
        }
        break;

      case '"':             /* Begin/End of String */
        *p++ = '\0';
        if (!InString)
        {
          items[item++] = p;
          InItem = ~InItem;
        }
        else
          InItem = 0;
        InString = ~InString; /* Toggle */
        break;

      default:
        if (!InItem)
        {
          items[item++] = p;
          InItem = ~InItem;
        }
        p++;
    }
  }

  item--;

  for (i=0; i<item; i+= 3)
  {
    if (0 > (MapIdx = ParameterNameToMapIndex (items[i])))
    {
      fprintf(fpErr, " \nParameter Name '%s' not recognized.. ", items[i]);
      return -1 ;

    }
    if (strcmp ("=", items[i+1]))
    {
      fprintf(fpErr, "\nfile: '=' expected as the second token in each line.");
      return -1 ;
    }
    if(sTokenMap[MapIdx].bType == 0)
    {
      strcpy((XDAS_Int8 *)sTokenMap[MapIdx].place, items[i+2]);
    }
    else if(sTokenMap[MapIdx].bType == 4)
    {
        sscanf (items[i+2], "%lf", &DoubleContent) ;
      * ((XDAS_Int32 *) (sTokenMap[MapIdx].place)) = DoubleContent;
    }
    else if(sTokenMap[MapIdx].bType == 2)
    {
        sscanf (items[i+2], "%lf", &DoubleContent) ;
      * ((XDAS_Int16 *) (sTokenMap[MapIdx].place)) = DoubleContent;
    }
    else
    {
      sscanf (items[i+2], "%d", &IntContent) ;
      * ((XDAS_Int8 *) (sTokenMap[MapIdx].place)) = IntContent;
    }
  }
  return 0 ;
}/* ParseContent */


/**
********************************************************************************
 *  @func     ParameterNameToMapIndex
 *  @brief  Returns the index number from sTokenMap[] for a given parameter name
 *
 *  @param[in]  s : parameter name string
 *
 *  @return  The index number if the string is a valid parameter name,
 *          -1 for error
********************************************************************************
*/
XDAS_Int32 ParameterNameToMapIndex (XDAS_Int8 *s)
{
  XDAS_Int32 i = 0;

  while (sTokenMap[i].tokenName != NULL)
  {
    if (0==strcmp (sTokenMap[i].tokenName, s))
      return i;
    else
      i++;
  }
  return -1;
}/* ParameterNameToMapIndex */

/** 
********************************************************************************
 *  @fn     MPEG4ENC_TI_Init_Profile
 *  @brief  This function will initialize status printing module
 *          
 *          This function, this prints log like number of frames encoded,
 *          bits consumed, cycles consumed etc
 *
 *  @param[in]  None
 *
 *  @return     none
********************************************************************************
*/

Void MPEG4ENC_TI_Init_Profile()
{
  /*--------------------------------------------------------------------------*/
  /* Initialize the parameters                                                */
  /*--------------------------------------------------------------------------*/
  gProfileLog.AverageCycles = 0;
  gProfileLog.PeakCycles = 0;
  gProfileLog.AverageCyclesM3 = 0;
  gProfileLog.PeakCyclesM3 = 0;

#ifdef PROFILE_HOST
  ICEC_CNT_CTRL = 0x40000000;
  while(!(ICEC_CNT_CTRL & (1 << 28)))  /* is owner Application */
  {
    /* claim resource */
    ICEC_CNT_CTRL = 0x40000000;
  }
  /* enable resource */
  ICEC_CNT_CTRL = 0x80000000;
  ICEC_CNT_CTRL = 0x80000000 |  (1<<19) | (1<<16);
  /* load enable counter mode1 */
  ICEC_CNT_CTRL = 0x80000000 |  (1<<19) | (1<<16) | (1<<12);

  /*--------------------------------------------------------------------------*/
  /* Cortex-M3 counter register initialization.                               */
  /* This is a decrementing counter.                                          */
  /*--------------------------------------------------------------------------*/
  SYSTICKREG = 0x4;
  SYSTICKREL = SYSTICKRELVAL;
  SYSTICKREG = 0x5;
#endif
}

/** 
********************************************************************************
 *  @fn     MPEG4ENC_TI_Capture_time
 *  @brief  This function will capture time instance 
 *          
 *          This function captures time and updates start or end time in log
 *          instance,
 *
 *  @param[in]  uiCapturePoint : tells start time or end time
 *                               0 - start time
 *                               1 - end time
 *
 *  @return     none
********************************************************************************
*/

void MPEG4ENC_TI_Capture_time(XDAS_Int32 uiCapturePoint)
{
#ifdef PROFILE_HOST

  if(uiCapturePoint == 0)
  {
    gProfileLog.startTime  = ICEC_CNT_VAL;
    gProfileLog.startTimeM3  = SYSTICKVAL;
  }
  else if(uiCapturePoint == 1)
  {

     gProfileLog.endTimeM3  = SYSTICKVAL;
     gProfileLog.endTime  = ICEC_CNT_VAL;

     gProfileLog.AverageCycles += (gProfileLog.endTime - gProfileLog.startTime);
     if((gProfileLog.endTime - gProfileLog.startTime) > gProfileLog.PeakCycles)
       gProfileLog.PeakCycles = gProfileLog.endTime - gProfileLog.startTime;

     if((XDAS_Int32)(gProfileLog.startTimeM3 - gProfileLog.endTimeM3)<0)
     {
       gProfileLog.startTimeM3 += SYSTICKRELVAL;
     }

     gProfileLog.AverageCyclesM3 += ( gProfileLog.startTimeM3 - 
       gProfileLog.endTimeM3);
     if((gProfileLog.startTimeM3 - gProfileLog.endTimeM3) > 
       gProfileLog.PeakCyclesM3)
     {
       gProfileLog.PeakCyclesM3 = gProfileLog.startTimeM3 - 
         gProfileLog.endTimeM3;
     }
  }

#endif
}

/** 
********************************************************************************
 *  @fn     MPEG4ENC_TI_PrintProfileInfo
 *  @brief  This function will capture time instance 
 *          
 *          This function captures time and updates start or end time in log
 *          instance,
 *
 *  @param[in]  uiCapturePoint : tells start time or end time
 *                               0 - start time
 *                               1 - end time
 *
 *  @return     none
********************************************************************************
*/
Void MPEG4ENC_TI_PrintProfileInfo(XDAS_UInt32 printInstance, 
  XDAS_Int32 bytesGenerated, XDAS_Int32 frame_nr)
{

#if 1//def PROFILE_HOST
  if(printInstance == PRINT_EACH_FRAME)
  {
#if 1
    /*Cycles(ProcessCall) */
    printf("\t%10.2f",
      (((gProfileLog.startTimeM3 - gProfileLog.endTimeM3)*30 * IVAHD_MHZ)/ \
      M3_HZ)); /* MHz(ProcessCall) */
#else
    printf("   Across Process Call (Cycles) : %8d\n",
      (gProfileLog.endTime - gProfileLog.startTime)); /*Cycles(ProcessCall) */
    /* MHz(ProcessCall) */
    printf("   Across Process Call (MHz)    : %3.2f\n",
      ((float)((gProfileLog.endTime -gProfileLog.startTime)*30)/1000000)); 
#endif
    printf("\t\t%3.2f",
      ((((bytesGenerated ) * 8)*30)/1000000.0));
  }
  else if(printInstance == PRINT_END_FRAME)
  {
#if 1
  printf("Peak Cycles    = %8d \n", gProfileLog.PeakCyclesM3);
  printf("Peak MHz       = %8.2f \n", 
    ((gProfileLog.PeakCyclesM3 * 30 * IVAHD_MHZ)/M3_HZ));
  printf("Average Cycles = %8d \n", gProfileLog.AverageCyclesM3/frame_nr);
  printf("Average MHz    = %8.2f \n", 
    (((gProfileLog.AverageCyclesM3/frame_nr)*30 * IVAHD_MHZ )/M3_HZ));

#else
  printf("\n\n Peak Cycles = %d \n", gProfileLog.PeakCycles);
  printf("\n Average Cycles = %d \n", gProfileLog.AverageCycles/frame_nr);
  printf("\n Average MHz = %3.2f \n", 
    (gProfileLog.AverageCycles/frame_nr)*30.0/1000000);
#endif
  }
  else
  {
    printf("\n No information to print. Check the print instance.");
  }
#endif //PROFILE_HOST
}
/**
********************************************************************************
 *  @func     Initialize_map
 *  @brief    Maps the address the variables to be parsed to the token map
 *
 *  @return None
********************************************************************************
*/
U32 Initialize_map()
{
  U32 uiTokenCtr = 0;

  sTokenMap[uiTokenCtr].tokenName = "InputFile";
  sTokenMap[uiTokenCtr].bType     = 0 ;
  sTokenMap[uiTokenCtr++].place   =  (gConfig.inputFile);
  
  sTokenMap[uiTokenCtr].tokenName = "CONTROL_FILE";
  sTokenMap[uiTokenCtr].bType     = 0 ;
  sTokenMap[uiTokenCtr++].place   =  (gConfig.controlFilename);
  
  sTokenMap[uiTokenCtr].tokenName = "DATASYNCH_FILE";
  sTokenMap[uiTokenCtr].bType     = 0 ;
  sTokenMap[uiTokenCtr++].place   =  (gConfig.datasynchFilename);
  
  sTokenMap[uiTokenCtr].tokenName = "RUNTIMECHANGE_FILE";
  sTokenMap[uiTokenCtr].bType     = 0 ;
  sTokenMap[uiTokenCtr++].place   =  (gConfig.runTimeChangeFilename);

  sTokenMap[uiTokenCtr].tokenName = "ReconFile";
  sTokenMap[uiTokenCtr].bType     = 0 ;
  sTokenMap[uiTokenCtr++].place   =  &(gConfig.reconFile);

  sTokenMap[uiTokenCtr].tokenName = "EncodedFile";
  sTokenMap[uiTokenCtr].bType     = 0 ;
  sTokenMap[uiTokenCtr++].place   =  &gConfig.bitstreamname;

  sTokenMap[uiTokenCtr].tokenName = "RefEncFile";
  sTokenMap[uiTokenCtr].bType     = 0 ;
  sTokenMap[uiTokenCtr++].place   =  &gConfig.refEncbitstreamname;




  sTokenMap[uiTokenCtr].tokenName = "FramesToBeEncoded";
  sTokenMap[uiTokenCtr].bType     = sizeof(gConfig.numFrames) ;
  sTokenMap[uiTokenCtr++].place   =  &(gConfig.numFrames);
  
  sTokenMap[uiTokenCtr].tokenName = "FrameNumber";
  sTokenMap[uiTokenCtr].bType     = sizeof(gConfig.frameNumber) ;
  sTokenMap[uiTokenCtr++].place   =  &(gConfig.frameNumber);

  sTokenMap[uiTokenCtr].tokenName = "NumInputUnits";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.videnc2Params.numInputDataUnits) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.videnc2Params.numInputDataUnits);

  sTokenMap[uiTokenCtr].tokenName = "EncodingPreset";
  sTokenMap[uiTokenCtr].bType    = sizeof(gParams.videnc2Params.encodingPreset);
  sTokenMap[uiTokenCtr++].place   =  &(gParams.videnc2Params.encodingPreset);

  sTokenMap[uiTokenCtr].tokenName = "RateControlPreset";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.videnc2Params.rateControlPreset) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.videnc2Params.rateControlPreset);

  sTokenMap[uiTokenCtr].tokenName = "ReferenceFrameRate";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.refFrameRate) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.refFrameRate);

  sTokenMap[uiTokenCtr].tokenName = "targetFrameRate";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.targetFrameRate) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.targetFrameRate);

  sTokenMap[uiTokenCtr].tokenName = "targetBitRate";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.targetBitRate) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.targetBitRate);

  sTokenMap[uiTokenCtr].tokenName = "MaxInterFrameInterval";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.videnc2Params.maxInterFrameInterval) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.videnc2Params.maxInterFrameInterval);

  sTokenMap[uiTokenCtr].tokenName = "inputWidth";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.inputWidth) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.inputWidth);

  sTokenMap[uiTokenCtr].tokenName = "inputHeight";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.inputHeight) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.inputHeight);

  sTokenMap[uiTokenCtr].tokenName = "Profile";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.videnc2Params.profile) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.videnc2Params.profile);

  sTokenMap[uiTokenCtr].tokenName = "Level";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.videnc2Params.level) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.videnc2Params.level);

  sTokenMap[uiTokenCtr].tokenName = "MaxWidth";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.videnc2Params.maxWidth) ;
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.maxWidth;

  sTokenMap[uiTokenCtr].tokenName = "MaxHeight";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.videnc2Params.maxHeight) ;
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.maxHeight;

  sTokenMap[uiTokenCtr].tokenName = "DataEndianess";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.videnc2Params.dataEndianness) ;
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.dataEndianness;

  sTokenMap[uiTokenCtr].tokenName = "InputChromaFormat";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.videnc2Params.inputChromaFormat) ;
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.inputChromaFormat;

  sTokenMap[uiTokenCtr].tokenName = "InputContentType";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.videnc2Params.inputContentType) ;
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.inputContentType;

  sTokenMap[uiTokenCtr].tokenName = "OperatingMode";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.videnc2Params.operatingMode);
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.operatingMode;

  sTokenMap[uiTokenCtr].tokenName = "InputDataMode";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.videnc2Params.inputDataMode);
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.inputDataMode;

  sTokenMap[uiTokenCtr].tokenName = "OutputDataMode";
  sTokenMap[uiTokenCtr].bType    = sizeof(gParams.videnc2Params.outputDataMode);
  sTokenMap[uiTokenCtr++].place   =  &gParams.videnc2Params.outputDataMode;

  sTokenMap[uiTokenCtr].tokenName = "NumOutputUnits";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.videnc2Params.numOutputDataUnits) ;
  sTokenMap[uiTokenCtr++].place   = &(gParams.videnc2Params.numOutputDataUnits);

  sTokenMap[uiTokenCtr].tokenName = "GenerateHeader";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.generateHeader) ;
  sTokenMap[uiTokenCtr++].place   =
    &gDynamicParams.videnc2DynamicParams.generateHeader;

  sTokenMap[uiTokenCtr].tokenName = "intraFrameInterval";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.intraFrameInterval) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.intraFrameInterval);

  sTokenMap[uiTokenCtr].tokenName = "interFrameInterval";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.interFrameInterval) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.interFrameInterval);

  sTokenMap[uiTokenCtr].tokenName = "CaptureWidth";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.captureWidth) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gDynamicParams.videnc2DynamicParams.captureWidth);

  sTokenMap[uiTokenCtr].tokenName = "CaptureHeight";
  sTokenMap[uiTokenCtr].bType     = sizeof(gConfig.captureHeight) ;
  sTokenMap[uiTokenCtr++].place   =  &(gConfig.captureHeight);

  sTokenMap[uiTokenCtr].tokenName = "ForceFrameType";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.forceFrame) ;
  sTokenMap[uiTokenCtr++].place   =
    &gDynamicParams.videnc2DynamicParams.forceFrame;


  sTokenMap[uiTokenCtr].tokenName = "forceSKIPPeriod";
  sTokenMap[uiTokenCtr].bType     = sizeof(gConfig.forceSKIPPeriod) ;
  sTokenMap[uiTokenCtr++].place   = &gConfig.forceSKIPPeriod;

  sTokenMap[uiTokenCtr].tokenName = "MotionVectorAccuracy";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.mvAccuracy) ;
  sTokenMap[uiTokenCtr++].place   =
    &gDynamicParams.videnc2DynamicParams.mvAccuracy;

  sTokenMap[uiTokenCtr].tokenName = "SampleAspectRatioHeight";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.sampleAspectRatioHeight) ;
  sTokenMap[uiTokenCtr++].place   =
    &gDynamicParams.videnc2DynamicParams.sampleAspectRatioHeight;

  sTokenMap[uiTokenCtr].tokenName = "SampelAspectRatioWidth";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.sampleAspectRatioWidth) ;
  sTokenMap[uiTokenCtr++].place   =
    &gDynamicParams.videnc2DynamicParams.sampleAspectRatioWidth;

  sTokenMap[uiTokenCtr].tokenName = "IgnoreOutBufSizeFlag";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gDynamicParams.videnc2DynamicParams.ignoreOutbufSizeFlag) ;
  sTokenMap[uiTokenCtr++].place   =
    &gDynamicParams.videnc2DynamicParams.ignoreOutbufSizeFlag;

  sTokenMap[uiTokenCtr].tokenName = "UseDataPartitioning";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.useDataPartitioning) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.useDataPartitioning);

  sTokenMap[uiTokenCtr].tokenName = "UseRvlc";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.useRvlc) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.useRvlc);

  sTokenMap[uiTokenCtr].tokenName = "ShortVideoHeader";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.useShortVideoHeader) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.useShortVideoHeader);

  /*-------------------------------------------------------------------------*/
  /* Rate control                                                            */
  /*-------------------------------------------------------------------------*/
  sTokenMap[uiTokenCtr].tokenName = "rateControlParamPreset";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.rateControlParams.rateControlParamsPreset) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.rateControlParams.rateControlParamsPreset);

  sTokenMap[uiTokenCtr].tokenName = "rcAlgo";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.rateControlParams.rcAlgo) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.rcAlgo);

  sTokenMap[uiTokenCtr].tokenName = "qpI";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.rateControlParams.qpI) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.qpI);

  sTokenMap[uiTokenCtr].tokenName = "qpP";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.rateControlParams.qpP) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.qpP);

  sTokenMap[uiTokenCtr].tokenName = "PerceptualQuant";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.rateControlParams.enablePerceptualQuantMode) ;
  sTokenMap[uiTokenCtr++].place   =  
    &(gParams.rateControlParams.enablePerceptualQuantMode);

  sTokenMap[uiTokenCtr].tokenName = "qpMax";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.rateControlParams.qpMax) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.qpMax);

  sTokenMap[uiTokenCtr].tokenName = "qpMin";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.rateControlParams.qpMin) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.qpMin);
  
  sTokenMap[uiTokenCtr].tokenName = "qpMinIntra";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.rateControlParams.qpMinIntra) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.qpMinIntra);

  sTokenMap[uiTokenCtr].tokenName = "rcFrameSkipEnable";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.rateControlParams.allowFrameSkip) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.rateControlParams.allowFrameSkip);


  sTokenMap[uiTokenCtr].tokenName = "seIntialQP";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.rateControlParams.seIntialQP) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.seIntialQP);
  
  sTokenMap[uiTokenCtr].tokenName = "VBVSize";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.rateControlParams.vbvBufferSize) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.rateControlParams.vbvBufferSize);

  sTokenMap[uiTokenCtr].tokenName = "initialBufferLevel";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.rateControlParams.initialBufferLevel) ;
  sTokenMap[uiTokenCtr++].place   =  
    &(gParams.rateControlParams.initialBufferLevel);
  

  /*-------------------------------------------------------------------------*/
  /* InterCoding Params                                                      */
  /*-------------------------------------------------------------------------*/
  sTokenMap[uiTokenCtr].tokenName = "interCodingPreset";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.interCodingParams.interCodingPreset) ;
  sTokenMap[uiTokenCtr++].place   =
    &gParams.interCodingParams.interCodingPreset;

  sTokenMap[uiTokenCtr].tokenName = "searchRangeHorP";
  sTokenMap[uiTokenCtr].bType     =
    sizeof(gParams.interCodingParams.searchRangeHorP) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.interCodingParams.searchRangeHorP);

  sTokenMap[uiTokenCtr].tokenName = "searchRangeVerP";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.interCodingParams.searchRangeVerP) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.interCodingParams.searchRangeVerP);

  sTokenMap[uiTokenCtr].tokenName = "GlobalOffsetME";
  sTokenMap[uiTokenCtr].bType     =
    sizeof(gParams.interCodingParams.globalOffsetME) ;
  sTokenMap[uiTokenCtr++].place   = &(gParams.interCodingParams.globalOffsetME);

  sTokenMap[uiTokenCtr].tokenName = "EnableRoundingControl";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.interCodingParams.enableRoundingControl) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.interCodingParams.enableRoundingControl);

  sTokenMap[uiTokenCtr].tokenName = "InterSearch8x8";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.interCodingParams.minBlockSizeP) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.interCodingParams.minBlockSizeP);

  sTokenMap[uiTokenCtr].tokenName = "EarlySkipThreshold";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.interCodingParams.earlySkipThreshold) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.interCodingParams.earlySkipThreshold);

  sTokenMap[uiTokenCtr].tokenName = "ThresholdingCost";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.interCodingParams.enableThresholdingMethod) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.interCodingParams.enableThresholdingMethod);

  /*-------------------------------------------------------------------------*/
  /* Intra coding params                                                     */
  /*-------------------------------------------------------------------------*/
  sTokenMap[uiTokenCtr].tokenName = "intraCodingPreset";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.intraCodingParams.intraCodingPreset) ;
  sTokenMap[uiTokenCtr++].place   =
    &gParams.intraCodingParams.intraCodingPreset ;

  sTokenMap[uiTokenCtr].tokenName = "airMethod";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.intraCodingParams.intraRefreshMethod) ;
  sTokenMap[uiTokenCtr++].place   =  
    &(gParams.intraCodingParams.intraRefreshMethod);

  sTokenMap[uiTokenCtr].tokenName = "airParam";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.intraCodingParams.intraRefreshRate) ;
  sTokenMap[uiTokenCtr++].place   =  
    &(gParams.intraCodingParams.intraRefreshRate);

  sTokenMap[uiTokenCtr].tokenName = "AcPredEnable";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.intraCodingParams.acpredEnable) ;
  sTokenMap[uiTokenCtr++].place   =  
    &(gParams.intraCodingParams.acpredEnable);

  sTokenMap[uiTokenCtr].tokenName = "insertGOVHdrBeforeIframe";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.intraCodingParams.insertGOVHdrBeforeIframe) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.intraCodingParams.insertGOVHdrBeforeIframe);

  sTokenMap[uiTokenCtr].tokenName = "EnableDriftControl";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.intraCodingParams.enableDriftControl) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.intraCodingParams.enableDriftControl);

  /*-------------------------------------------------------------------------*/
  /* Slice coding params                                                     */
  /*-------------------------------------------------------------------------*/
  sTokenMap[uiTokenCtr].tokenName = "sliceCodingPreset";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.sliceCodingParams.sliceCodingPreset) ;
  sTokenMap[uiTokenCtr++].place   =
    &(gParams.sliceCodingParams.sliceCodingPreset);

  sTokenMap[uiTokenCtr].tokenName = "GobInterval";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.sliceCodingParams.gobInterval) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.sliceCodingParams.gobInterval);

  sTokenMap[uiTokenCtr].tokenName = "sliceMode";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.sliceCodingParams.sliceMode);
  sTokenMap[uiTokenCtr++].place   =  &(gParams.sliceCodingParams.sliceMode);

  sTokenMap[uiTokenCtr].tokenName = "sliceUnitSize";
  sTokenMap[uiTokenCtr].bType     = 
    sizeof(gParams.sliceCodingParams.sliceUnitSize) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.sliceCodingParams.sliceUnitSize);

  sTokenMap[uiTokenCtr].tokenName = "UseHec";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.sliceCodingParams.useHec) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.sliceCodingParams.useHec);

  sTokenMap[uiTokenCtr].tokenName = "AspectRatioInfo";
  sTokenMap[uiTokenCtr].bType     = sizeof(gDynamicParams.aspectRatioIdc ) ;
  sTokenMap[uiTokenCtr++].place   =  &(gDynamicParams.aspectRatioIdc );

  sTokenMap[uiTokenCtr].tokenName = "VopTimeIncrementResolution";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.vopTimeIncrementResolution) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.vopTimeIncrementResolution);

  sTokenMap[uiTokenCtr].tokenName = "NonMultipleOf16RefPadding";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.nonMultiple16RefPadMethod) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.nonMultiple16RefPadMethod);

  sTokenMap[uiTokenCtr].tokenName = "PixelRange";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.pixelRange) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.pixelRange);
  
  sTokenMap[uiTokenCtr].tokenName = "EnableSceneChangeAlgo";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.enableSceneChangeAlgo) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.enableSceneChangeAlgo);

  sTokenMap[uiTokenCtr].tokenName = "ivahdId";
  sTokenMap[uiTokenCtr].bType     = sizeof(gConfig.ivahdid) ;
  sTokenMap[uiTokenCtr++].place   =  &(gConfig.ivahdid);
  
  sTokenMap[uiTokenCtr].tokenName = "lumaTilerSpace";
  sTokenMap[uiTokenCtr].bType     = sizeof(gConfig.tilerSpace[0]) ;
  sTokenMap[uiTokenCtr++].place   =  &(gConfig.tilerSpace[0]);
  
  sTokenMap[uiTokenCtr].tokenName = "chromaTilerSpace";
  sTokenMap[uiTokenCtr].bType     = sizeof(gConfig.tilerSpace[1]) ;
  sTokenMap[uiTokenCtr++].place   =  &(gConfig.tilerSpace[1]);
  
  sTokenMap[uiTokenCtr].tokenName = "EnableAnalyticInfoDump";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.enableAnalyticinfo) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.enableAnalyticinfo);

  

  sTokenMap[uiTokenCtr].tokenName = "DebugTraceLevel";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.debugTraceLevel) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.debugTraceLevel);
  
  sTokenMap[uiTokenCtr].tokenName = "LastNFramesToLog";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.lastNFramesToLog) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.lastNFramesToLog);
  
  sTokenMap[uiTokenCtr].tokenName = "useVOS";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.useVOS) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.useVOS);

  sTokenMap[uiTokenCtr].tokenName = "enableMONA";
  sTokenMap[uiTokenCtr].bType     = sizeof(gParams.enableMONA) ;
  sTokenMap[uiTokenCtr++].place   =  &(gParams.enableMONA);

  sTokenMap[uiTokenCtr].tokenName = "\0";
  sTokenMap[uiTokenCtr].bType     = -1 ;
  sTokenMap[uiTokenCtr++].place   = NULL;

  return 1;
}
