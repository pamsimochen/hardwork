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
 * @file <TestAppDecoder.c>
 *
 * @brief  This file is a MPEG4 Decoder Sample test application
 *
 *         This file has MPEG4 Decoder sample test app functions to understand
 *         the usage of codec function calls during integration.
 *
* @author:  Ashish Singh
*
* @version 0.0 (August 2011)  : Initial version
*
* @version 0.1 (Mar 2012)     : Support for Enhanced deblocking feature added
*
* @version 0.2 (July 2012)    : Added support for only I frames decoding
*                               [Mahantesh]
******************************************************************************
*/
/******************************************************************************
*  INCLUDE FILES
******************************************************************************/
#include <xdc/std.h>
#include "TestAppDecoder.h"
#include "TestApp_rmanConfig.h"
#include "mpeg4vdec.h"
/* -------------------------------------------------------------------------- */
/*  Externally declared variables, but referred here.                         */
/* -------------------------------------------------------------------------- */
extern IMPEG4VDEC_InArgs    inArgs;
extern IMPEG4VDEC_OutArgs   outArgs;
extern IMPEG4VDEC_Handle    mpeg4decHandle;
extern IMPEG4VDEC_Status  status;
extern sAppFilePointers   filePtrs;
extern BUFFMGR_buffEleHandle buffEle;
extern XDAS_Int8  inputData[INPUT_BUFFER_SIZE];
extern XDAS_UInt32 decodeMode;
extern XDAS_UInt16 DataSyncIntrCntr;


extern XDAS_UInt32 streamEnd;
extern XDAS_UInt32 frameCount;

extern XDAS_UInt8  appIvahdId;
extern XDAS_Int8   configStatus;
extern XDAS_Int32  countConfigSet;
extern XDAS_Int8   algCreated;
extern XDAS_UInt32 bufferDisplayed;

extern IMPEG4VDEC_Params          params;
extern IMPEG4VDEC_DynamicParams   dynamicParams;
extern sAppControlParams        appControlPrms;
extern const XDAS_UInt32 IVAHD_memory_wfi[];
extern XDAS_Int32 putBufferCount;

/*----------------------------------------------------------------------------*/
/*  Token Map array for inputs from testparams.cfg file.                      */
/*----------------------------------------------------------------------------*/
sTokenMapping sTokenMap[] =
{
  {(XDAS_Int8 *)"maxHeight",          &params.viddec3Params.maxHeight, 0},
  {(XDAS_Int8 *)"maxWidth",           &params.viddec3Params.maxWidth, 0},
  {(XDAS_Int8 *)"maxFrameRate",       &params.viddec3Params.maxFrameRate, 0},
  {(XDAS_Int8 *)"maxBitRate",         &params.viddec3Params.maxBitRate, 0},
  {(XDAS_Int8 *)"dataEndianness",     &params.viddec3Params.dataEndianness, 0},
  {(XDAS_Int8 *)"forceChromaFormat",  &params.viddec3Params.forceChromaFormat, 0},
  {(XDAS_Int8 *)"operatingMode",      &params.viddec3Params.operatingMode, 0},
  {(XDAS_Int8 *)"displayDelay",       &params.viddec3Params.displayDelay, 0},
  {(XDAS_Int8 *)"inputDataMode",      &params.viddec3Params.inputDataMode, 0},
  {(XDAS_Int8 *)"outputDataMode",     &params.viddec3Params.outputDataMode, 0},
  {(XDAS_Int8 *)"numInputDataUnits",  &params.viddec3Params.numInputDataUnits, 0},
  {(XDAS_Int8 *)"numOutputDataUnits", &params.viddec3Params.numOutputDataUnits, 0},
  {(XDAS_Int8 *)"errorInfoMode",      &params.viddec3Params.errorInfoMode, 0},
  {(XDAS_Int8 *)"displayBufsMode",    &params.viddec3Params.displayBufsMode, 0},
  {(XDAS_Int8 *)"metadataType_0",     &params.viddec3Params.metadataType[0], 0},
  {(XDAS_Int8 *)"metadataType_1",     &params.viddec3Params.metadataType[1], 0},
  {(XDAS_Int8 *)"metadataType_2",     &params.viddec3Params.metadataType[2], 0},
  {(XDAS_Int8 *)"outloopDeBlocking",  &params.outloopDeBlocking, 0},
  {(XDAS_Int8 *)"enhancedDeBlockingQp",&params.enhancedDeBlockingQp,0},
  {(XDAS_Int8 *)"errorConcealmentEnable", &params.errorConcealmentEnable, 0},
  {(XDAS_Int8 *)"sorensonSparkStream",&params.sorensonSparkStream,0},
  {(XDAS_Int8 *)"debugTraceLevel"    ,&params.debugTraceLevel,0},
  {(XDAS_Int8 *)"lastNFramesToLog"   ,&params.lastNFramesToLog,0},
  {(XDAS_Int8 *)"paddingType"        ,&params.paddingMode,0},
  {(XDAS_Int8 *)"decodeOnlyIntraFrames",&params.decodeOnlyIntraFrames,0},
  {(XDAS_Int8 *)"Rsvd2"              ,&params.reserved,0},
  {(XDAS_Int8 *)"decodeHeader"       ,&dynamicParams.viddec3DynamicParams.decodeHeader, 0},
  {(XDAS_Int8 *)"displayWidth"       ,&dynamicParams.viddec3DynamicParams.displayWidth, 0},
  {(XDAS_Int8 *)"frameSkipMode"      ,&dynamicParams.viddec3DynamicParams.frameSkipMode, 0},
  {(XDAS_Int8 *)"newFrameFlag"       ,&dynamicParams.viddec3DynamicParams.newFrameFlag, 0},
  {(XDAS_Int8 *)"lateAcquireArg"     ,&dynamicParams.viddec3DynamicParams.lateAcquireArg, 0},
  {(XDAS_Int8 *)"DynRsvd0"           ,&dynamicParams.reserved[0], 0},
  {(XDAS_Int8 *)"DynRsvd1"           ,&dynamicParams.reserved[1], 0},
  {(XDAS_Int8 *)"DynRsvd2"           ,&dynamicParams.reserved[2], 0},
  {(XDAS_Int8 *)"MbInfoWriteMode"    ,&appControlPrms.mbInfoWriteMode, 0},
  {(XDAS_Int8 *)"TilerEnable",       &appControlPrms.tilerEnable, 0},
  {(XDAS_Int8 *)"ChromaTilerMode",   &appControlPrms.chromaTilerMode, 0},
  {(XDAS_Int8 *)"BitStreamMode",     &appControlPrms.bitStreamMode, 0},
  {(XDAS_Int8 *)"ReadHeaderData",    &appControlPrms.readHeaderdata, 0},
  {(XDAS_Int8 *)"NumFramesToDecode", &appControlPrms.framesToDecode, 0},
  {(XDAS_Int8 *)"parBoundCheck",     &appControlPrms.parBoundCheck, 0},
  {(XDAS_Int8 *)"parExpectedStatus", &appControlPrms.parExpectedStatus, 0},
  {(XDAS_Int8 *)"exitLevel",         &appControlPrms.exitLevel, 0},
  {(XDAS_Int8 *)"xdmReset",          &appControlPrms.xdmReset, 0},
  {(XDAS_Int8 *)"DumpFrom",          &appControlPrms.dumpFrom, 0},
  {(XDAS_Int8 *)"CRCEnable",         &appControlPrms.crcNyuv, 0},
  {(XDAS_Int8 *)"ProfileEnable",     &appControlPrms.profileEnable, 0},
  {(XDAS_Int8 *)"BaseClassOnly",     &appControlPrms.baseClassOnly, 0},
  {(XDAS_Int8 *)"TestCompliance",    &appControlPrms.testCompliance, 0},
  {(XDAS_Int8 *)"ivahdID",           &appControlPrms.ivahdId, 0},
  {(XDAS_Int8 *)"inputBitStream",    &appControlPrms.inFile, 1},
  {(XDAS_Int8 *)"outputYUV",         &appControlPrms.outFile, 1},
  {(XDAS_Int8 *)"referenceYUV",      &appControlPrms.refFile, 1},
  {(XDAS_Int8 *)"frameSizeFile",     &appControlPrms.sizeFile, 1},
  {(XDAS_Int8 *)"AppRsvd0",          &appControlPrms.reserved[0], 0},
  {(XDAS_Int8 *)"AppRsvd1",          &appControlPrms.reserved[1], 0},
  {(XDAS_Int8 *)"AppRsvd2",          &appControlPrms.reserved[2], 0},
  {NULL,             NULL}
};
/*------------------------------------------------------------------------*/
/* Arrays to capture the metadata buffer indicies: 3 types of metadata    */
/* are supported by the decoder: SEI, VUI and MBInfo                      */
/*------------------------------------------------------------------------*/
XDAS_UInt8 metaDataIndex[IVIDEO_MAX_NUM_METADATA_PLANES];
XDAS_UInt8 metaDataEnable[IVIDEO_MAX_NUM_METADATA_PLANES];

/**
********************************************************************************
 *  @fn     TestApp_updateIObufDesc
 *  @brief  Updates the Input & Output buffer descriptors for every decode call.
 *
 *  @param[in]  inputBufDesc : Pointer to the input Buffer Descriptor.
 *  @param[in]  outputBufDesc : Pointer to the output Buffer descriptor
 *  @param[in]  status : Pointer to the status structure.
 *  @param[in]  bytesConsumed : Indicates the bytesConsumed from the input
 *                              buffer till the point.
 *  @param[in]  validBytes : Indicates valid Bytes yet to be consumed by
 *                           codec & available in input buffer.
 *
 *  @return    None
********************************************************************************
*/
XDAS_Void TestApp_updateIObufDesc(XDM2_BufDesc     *inputBufDesc,
                             XDM2_BufDesc     *outputBufDesc,
                             IVIDDEC3_Status  *status,
                             XDAS_UInt32      bytesConsumed,
                             XDAS_Int32       validBytes
                            )
{
  XDAS_Int8   tag;
  XDAS_UInt8  metaBufIdx, Idx;

  if(appControlPrms.bitStreamMode != BS_FRAME_MODE)
  {
    /*------------------------------------------------------------------------*/
    /*  Reinitialize the input data buf ptr with the right offset from the    */
    /*  the start of inputData with the help of current value of              */
    /*  bytesConsumed which tells the number of bytes used up for the         */
    /*  current config stream.                                                */
    /*------------------------------------------------------------------------*/
    inputBufDesc->descs[0].buf = (XDAS_Int8 *)((XDAS_Int32) inputData +
                                   bytesConsumed);
  }
  else
  {
    inputBufDesc->descs[0].buf = (XDAS_Int8 *)((XDAS_Int32) inputData);
  }

  /*--------------------------------------------------------------------------*/
  /* Do the update of output buffer descriptor only when the decode mode is   */
  /* set to XDM_DECODE_AU                                                      */
  /*--------------------------------------------------------------------------*/
  if(decodeMode == XDM_DECODE_AU)
  {
    /*------------------------------------------------------------------------*/
    /*  NON_CONTIGUOUS BUFFERS:                                               */
    /*  Separate Buffers for Luma & Chroma.                                   */
    /*  Copy the output buffer size values and pointers from the buffer       */
    /*  element returned by getfreebuffer() function.                         */
    /* The output buffer memType can be TILED or NON-TILED based on the input */
    /* configuration chosen.                                                  */
    /*------------------------------------------------------------------------*/
    for(tag = 0 ; tag < 2 ; tag++)
    {
      outputBufDesc->descs[tag].buf = (XDAS_Int8 *) buffEle->buf[tag];

      if(appControlPrms.tilerEnable)
      {
        outputBufDesc->descs[tag].memType =
                                         status->bufInfo.outBufMemoryType[tag];

        if(appControlPrms.chromaTilerMode && tag)
        {
          outputBufDesc->descs[tag].memType = XDM_MEMTYPE_TILED8;
        }

        outputBufDesc->descs[tag].bufSize.tileMem.width  =
                               (XDAS_Int32) buffEle->bufSize[tag].tileMem.width;

        outputBufDesc->descs[tag].bufSize.tileMem.height =
                              (XDAS_Int32) buffEle->bufSize[tag].tileMem.height;
      }
      else
      {
        outputBufDesc->descs[tag].bufSize.bytes  =
                                     (XDAS_Int32) buffEle->bufSize[tag].bytes;

        outputBufDesc->descs[tag].memType = XDM_MEMTYPE_ROW;
      }
    }
  }

  /*----------------------------------------------------------------------*/
  /* Meta Data (SEI, VUI & MB-Info) buffer addresses update.              */
  /*----------------------------------------------------------------------*/
  for(Idx=0; Idx<IVIDEO_MAX_NUM_METADATA_PLANES; Idx++)
  {
    if(metaDataEnable[Idx])
    {
      metaBufIdx = 2 + metaDataIndex[Idx];

      outputBufDesc->descs[metaBufIdx].bufSize.bytes  =
                       (XDAS_Int32) buffArray[0].bufSize[metaBufIdx].bytes;
      outputBufDesc->descs[metaBufIdx].buf     =
                                (XDAS_Int8 *) buffArray[0].buf[metaBufIdx];
      outputBufDesc->descs[metaBufIdx].memType =
                              status->bufInfo.outBufMemoryType[metaBufIdx];
    }
    else
    {
      outputBufDesc->descs[Idx+2].bufSize.bytes  = 0;
      outputBufDesc->descs[Idx+2].buf            = NULL;
      outputBufDesc->descs[Idx+2].memType        = XDM_MEMTYPE_ROW;
    }
  }
}

/**
********************************************************************************
 *  @fn     TestApp_SetInitParams
 *  @brief  The function is an Application function to set init params
 *
 *  @param[in] params  : Pointer to static params structure
 *
 *  @return  None
********************************************************************************
*/
XDAS_Void TestApp_SetInitParams(IMPEG4VDEC_Params *params)
{
  XDAS_UInt8  cntr;
  /*-----------------------------------------------------------------------*/
  /*  Set IVIDDEC3_Params parameters                                       */
  /*-----------------------------------------------------------------------*/
  /*--------------------------------------------------------------------------*/
  /* Initializing the Metadata arrays.                                        */
  /*--------------------------------------------------------------------------*/
  for(cntr = 0; cntr < IVIDEO_MAX_NUM_METADATA_PLANES; cntr++)
  {
    metaDataIndex[cntr]   = cntr;
    metaDataEnable[cntr]  = 0;
  }

  /*--------------------------------------------------------------------------*/
  /* Check metadata types                                                     */
  /* Based on the metadata types requested, fill the metadataType[] array     */
  /*--------------------------------------------------------------------------*/
  for(cntr = 0; cntr < IVIDEO_MAX_NUM_METADATA_PLANES; cntr++)
  {
    XDAS_Int32 metaType = params->viddec3Params.metadataType[cntr];

    if((metaType > IVIDEO_METADATAPLANE_NONE) &&
       (metaType < IVIDEO_MAX_NUM_METADATA_PLANES))
    {
      metaDataIndex[metaType]  = cntr;
      metaDataEnable[metaType] = 1;
      if(metaType == APP_MB_INFO)
      {
        params->viddec3Params.metadataType[cntr] = IVIDEO_METADATAPLANE_MBINFO;
      }
    }
  }
}


/**
********************************************************************************
 *  @fn     TestApp_SetDynamicParams
 *  @brief  The function is an Application function to set dynamic params
 *
 *  @param[in] dynamicParams  : Pointer to dynamic params structure
 *
 *  @return  None
********************************************************************************
*/
XDAS_Void TestApp_SetDynamicParams(IVIDDEC3_DynamicParams *dynamicParams)
{
  /*--------------------------------------------------------------------------*/
  /* Set IVIDDEC3 Run time parameters                                         */
  /*--------------------------------------------------------------------------*/
  if(!appControlPrms.parBoundCheck)
  {
    dynamicParams->decodeHeader  = decodeMode;
  }
  /*------------------------------------------------------------------------*/
  /* Populate the function pointer for putBuffer function.                  */
  /* Note: the putBufferHandle and getDataHandle can be used by the app     */
  /* to indicate any additional info related to the putBuffer call          */
  /* Here we do not use this handle for any further processing hence setting*/
  /* them to NULL                                                           */
  /*------------------------------------------------------------------------*/
  dynamicParams->putDataFxn    = NULL;
  dynamicParams->putDataHandle = NULL;
  dynamicParams->getDataFxn    = NULL;
  /*------------------------------------------------------------------------*/
  /* Populate the function pointer for putBuffer function.                  */
  /* Note: the putBufferHandle and getDataHandle can be used by the app     */
  /* to indicate any additional info related to the putBuffer call          */
  /* Here we do not use this handle for any further processing hence setting*/
  /* them to NULL                                                           */
  /*------------------------------------------------------------------------*/
  dynamicParams->putBufferFxn    = NULL;
  dynamicParams->putBufferHandle = NULL;
  dynamicParams->getDataHandle   = NULL;

  dynamicParams->frameSkipMode   = IVIDEO_NO_SKIP;
  dynamicParams->newFrameFlag    = XDAS_TRUE;
}

/**
********************************************************************************
 *  @fn     MEMUTILS_getPhysicalAddr
 *
 *  @brief  Sample utility function that need to be implemented by the
 *          applicationor framework components for address translation
 *           (virtual to physical). This function is SOC/APP specific.
 *
 *  @param[in] Addr  : Address to be translated.
 *
 *  @return  translated address
********************************************************************************
*/
XDAS_Void *MEMUTILS_getPhysicalAddr(Ptr Addr)
{
  if(NULL == Addr)
  {
    printf("NULL address sent for V2P conversion: %d\n", Addr);
  }
#ifdef USE_HOST_FILE_IO
  {
    Uint32 temp1;
  if(((Uint32)Addr<(0x0+0x4000))){
   temp1= ((Uint32)Addr+0x9d000000);
  }else if(((Uint32)Addr>=0x4000)&&((Uint32)Addr<(0x4000+0x1fc000))){
   temp1= ((Uint32)Addr - 0x4000 + 0x9d004000);
  }else if(((Uint32)Addr>=0x200000)&&((Uint32)Addr<(0x200000+0x600000))){
   temp1= ((Uint32)Addr - 0x200000 + 0x9d200000);
  }else if(((Uint32)Addr>=0x800000)&&((Uint32)Addr<(0x800000+0x200000))){
   temp1= ((Uint32)Addr - 0x800000 + 0x9d800000);
  }else if(((Uint32)Addr>=0xa00000)&&((Uint32)Addr<(0xa00000+0x600000))){
   temp1= ((Uint32)Addr - 0xa00000 + 0x9da00000);

  }else if(((Uint32)Addr>=0x80000000)&&((Uint32)Addr<(0x80000000+0x100000))){
   temp1= ((Uint32)Addr - 0x80000000 +0x9e000000);
  }else if(((Uint32)Addr>=0x80100000)&&((Uint32)Addr<(0x80100000+0x100000))){
   temp1= ((Uint32)Addr - 0x80100000 + 0x9e100000);

  }else if(((Uint32)Addr>=0x80200000)&&((Uint32)Addr<(0x80200000+0x100000))){
   temp1= ((Uint32)Addr - 0x80200000 +0x9e200000);

  }else if(((Uint32)Addr>=0x80300000)&&((Uint32)Addr<(0x80300000+0x1000000))){
   temp1= ((Uint32)Addr - 0x80300000 +0x9e300000);

  }else if(((Uint32)Addr>=0x81300000)&&((Uint32)Addr<(0x81300000+0xc00000))){
   temp1= ((Uint32)Addr - 0x81300000 +0x9f300000);

  }else if(((Uint32)Addr>=0x81f00000)&&((Uint32)Addr<(0x81f00000+0x100000))){
   temp1= ((Uint32)Addr - 0x81f00000 +0x9ff00000);

  }else if(((Uint32)Addr>=0xa0000000)&&((Uint32)Addr<(0xa0000000+0x55000))){
   temp1= ((Uint32)Addr - 0xa0000000 +0x9cf00000);

  }else if(((Uint32)Addr>=0xa0055000)&&((Uint32)Addr<(0xa0055000+0x55000))){
   temp1= ((Uint32)Addr - 0xa0055000 +0x9cf55000);

  }else{
   temp1= ((Uint32)Addr & 0xFFFFFFFF);
  }
    return (Void *)temp1;
  }
#else
  /*------------------------------------------------------------------------*/
  /* Address translation based on the SOC/APP                               */
  /*------------------------------------------------------------------------*/
#if defined(HOST_ARM9)
  return ((void *)((unsigned int)Addr & VDMAVIEW_EXTMEM));
#else
  return ((void*)Addr);
#endif
#endif
}

/**
********************************************************************************
 *  @fn     paramBoundaryCheck
 *  @brief  main function of Host TestAppDecoder
 *
 *  @param[in] retVal       : Indicates the value returned by create call or
 *                            control call.
 *  @param[in] checkLevel   : Indicates the check & exit points during param
 *                            boundary checks validation.
 *
 *  @return IALG_EFAIL/APP_NEXT_CONFIG/APP_SAME_CONFIG
********************************************************************************
*/
XDAS_Int8 paramBoundaryCheck(XDAS_Int8 status, XDAS_Int8 checkLevel)
{
  XDAS_UInt8 freeMem = 0;
  XDAS_Int8  retVal  = APP_SAME_CONFIG;
  /*------------------------------------------------------------------------*/
  /* Check if parameter Boundary Check validation is enabled.               */
  /*------------------------------------------------------------------------*/
  if(appControlPrms.parBoundCheck)
  {
    if(appControlPrms.exitLevel == checkLevel)
    {
      if(status != appControlPrms.parExpectedStatus)
      {
        configStatus = IALG_EFAIL;
      }
      freeMem = 1;
      retVal  = APP_NEXT_CONFIG;
      /*--------------------------------------------------------------------*/
      /* Print each config status in a Log file to check the failures if any*/
      /*--------------------------------------------------------------------*/
      updateConfigStatusFile(configStatus);
    }

    if(retVal == APP_NEXT_CONFIG)
    {
      countConfigSet++;
      my_fclose(filePtrs.fpInFile, ARG_FILEIO_TYPE);
      my_fclose(filePtrs.fpOutFile, ARG_FILEIO_TYPE);
      fclose(filePtrs.fpRefFile);
      algCreated = XDAS_FALSE;
    }

    /*----------------------------------------------------------------------*/
    /* Check if it is time to free up the memory blocks.                    */
    /*----------------------------------------------------------------------*/
    if(freeMem && (mpeg4decHandle != NULL))
    {
      RMAN_FreeResources((IALG_Handle)mpeg4decHandle);
      MPEG4VDEC_delete(mpeg4decHandle);
    }
  }
  else if(mpeg4decHandle == NULL)
  {
    algCreated = XDAS_FALSE;
    retVal  = APP_NEXT_CONFIG;
  }

  return retVal;
}

/**
********************************************************************************
 *  @fn     TestApp_checkErr
 *  @brief  Checks whether the errors resulted in process call force application
 *          application to initiate flush operation.
 *
 *  @param[in] errMsg : Error message returned by process call.
 *
 *  @return XDAS_TRUE or XDAS_FALSE
********************************************************************************
*/
XDAS_Bool TestApp_flushCheck(XDAS_Int32 errMsg)
{
  /*----------------------------------------------------------------------*/
  /* Under certain error conditions, the application need to stop decoding*/
  /* the current stream and do an XDM_FLUSH which enables the codec to    */
  /* flush (display and free up) the frames locked by it. The following   */
  /* error conditions fall in this category.                              */
  /*----------------------------------------------------------------------*/
    if(((TestApp_checkErr(errMsg, XDM_INSUFFICIENTDATA) &&
        (BS_FRAME_MODE != appControlPrms.bitStreamMode))) ||
     (TestApp_checkErr(errMsg, IMPEG4D_ERR_STREAM_END)) ||
     (TestApp_checkErr(errMsg, IMPEG4D_ERR_PICSIZECHANGE)) ||
     (TestApp_checkErr(errMsg, IMPEG4D_ERR_UNSUPPRESOLUTION)) ||
     (TestApp_checkErr(errMsg, IMPEG4D_ERR_INVALID_MBOX_MESSAGE))||
     (TestApp_checkErr(errMsg, XDM_FATALERROR)))
   {
     return XDAS_TRUE;
   }
   else
   {
     return XDAS_FALSE;
   }
}


/**
********************************************************************************
 *  @fn     testAppFlushFrames
 *  @brief  Flush implementation : First the application needs to call control
 *          API with XDM_FLUSH command. Then the buffers provided by the decoder
 *          need to be displayed and freed up. This function implements this
 *          flush opearation.
 *
 *  @param[in] inputBufDesc : Pointer to the input Buffer descriptor
 *  @param[in] outputBufDesc: Pointer to the output Buffer descriptor
 *  @param[in] displayBufs  : Pointer to the display Buffers descriptor
 *
 *  @return IVIDDEC3_EOK/IVIDDEC3_EFAIL
********************************************************************************
*/
XDAS_Int8 testAppFlushFrames(XDM2_BufDesc *inputBufDesc,
                              XDM2_BufDesc *outputBufDesc,
                              IVIDEO2_BufDesc *displayBufs)
{
  XDAS_Int8  testVal = APP_EOK;
  XDAS_Int16 idx = 0;
  XDAS_Int32 retVal;
  /*--------------------------------------------------------------------*/
  /* Put the decoder in FLUSH mode through the control() call and       */
  /* XDM_FLUSH command.                                                 */
  /*--------------------------------------------------------------------*/
  MPEG4VDEC_control(mpeg4decHandle, XDM_FLUSH, &dynamicParams, &status);
  /*--------------------------------------------------------------------*/
  /* The following loop displays and frees up the buffers locked by the */
  /* decoder. After the XDM_FLUSH command, the app needs to call the    */
  /* process function untill the process returns IVIDDEC3_EOK. The      */
  /* frame ids provided in the OutArgs need to be displayed/freed up.   */
  /* The loop breaks once process call returns failure (IVIDDEC3_EFAIL) */
  /*--------------------------------------------------------------------*/
    do
    {
#ifdef MEM_CORRUPTION_TEST
      /*----------------------------------------------------------------------*/
      /* Do scratch corruption of SL2, ITCM, DTCM and IP memories             */
      /* If memory corruption test is enabled                                 */
      /*----------------------------------------------------------------------*/
      MPEG4DEC_ScratchMemoryCorruption();
#endif
      /*----------------------------------------------------------------------*/
      /* Call process function. This is to flush out the frames and not for   */
      /* stream decoding. The deocder is alredy put in flush mode through     */
      /* the control call XDM_FLUSH                                           */
      /*----------------------------------------------------------------------*/
      retVal = MPEG4VDEC_decodeFrame(mpeg4decHandle, inputBufDesc,
                                    outputBufDesc, &inArgs, &outArgs);
#ifdef MEM_CORRUPTION_TEST
      /*----------------------------------------------------------------------*/
      /* Deactivate all resources                                             */
      /*----------------------------------------------------------------------*/
      RMAN_DeactivateAllResources((IALG_Handle)mpeg4decHandle);
#endif
      /*----------------------------------------------------------------------*/
      /* If a frame is available for display, then write it to file           */
      /*----------------------------------------------------------------------*/
      if (retVal == IVIDDEC3_EOK && displayBufs->numPlanes)
      {
        idx++;
        if(frameCount > appControlPrms.dumpFrom)
        {
          testVal = TestApp_outputDataXfr(filePtrs.fpOutFile, filePtrs.fpRefFile,
                                          outArgs, appControlPrms.testCompliance,
                                          appControlPrms.crcNyuv);
        }
      }

      /*-------------------------------------------------------------------*/
      /*  Check for Break off condition of in test compliance              */
      /*-------------------------------------------------------------------*/
      if(testVal == APP_EFAIL)
      {
        break;
      }

      /*------------------------------------------------------------------*/
      /* Release(FREE UP) buffers - which are released from the algorithm   */
      /* side  -back to the buffer manager. The freebufID array of outargs*/
      /* contains the sequence of bufferIds which need to be freed. This  */
      /*  gets populated by the algorithm. The following function will do */
      /*  the job of freeing up the buffers.                              */
      /*------------------------------------------------------------------*/
      BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddec3OutArgs.freeBufID);

    } while (retVal == IVIDDEC3_EOK);

    printf("Flushed %d pictures\n", idx);
    bufferDisplayed += idx;

  return testVal;
}

#ifdef MEM_CORRUPTION_TEST
/**
********************************************************************************
 *  @fn            MPEG4DEC_ScratchMemoryCorruption
 *  @brief         This function does scratch corruption of SL2, ITCM, DTCM
 *                 and IP Memories
 *  @param[in] void : None
 *
 *  @return         : None
********************************************************************************
*/
Void MPEG4DEC_ScratchMemoryCorruption()
{
  /*---------------------------------------------------------------------*/
  /*  Fill whole of SL2,ITCM,DTCM and IP memories with some data, to     */
  /*  check that codec doesnt depend on SL2,ITCM,DTCM and IP memories    */
  /*  content of previous process call.                                  */
  /*---------------------------------------------------------------------*/
  XDAS_Int8 word_pattern[4] = {0x00, 0xAA, 0x55, 0x99};
  XDAS_Int8 word = word_pattern[frameCount & 0x3];

  XDAS_UInt32 regBaseAddr, memBaseAddr;

  regBaseAddr = regBaseAdderss[appIvahdId];
  memBaseAddr = memBaseAddress[appIvahdId];
  /*---------------------------------------------------------------------*/
  /* For IP corruption, turn on clocks                                   */
  /*---------------------------------------------------------------------*/
  TestApp_turnOnClocks();

  /*------------------------------------------------------------------------*/
  /* Fill SL2 memory with some data                                         */
  /*------------------------------------------------------------------------*/
  memset ((Void*)(memBaseAddr + 0x000000000), word, (256*1024));

  /*------------------------------------------------------------------------*/
  /* Fill ITCM memory with some data                                        */
  /*------------------------------------------------------------------------*/
  memset ((Void*)(regBaseAddr + ICONT1_DTCM_OFFSET), word, 16*1024);
  memset ((Void*)(regBaseAddr + ICONT2_DTCM_OFFSET), word, 16*1024);

  /*------------------------------------------------------------------------*/
  /* Fill DTCM memory with some data                                        */
  /*------------------------------------------------------------------------*/
  memset ((Void*)(regBaseAddr + ICONT1_ITCM_OFFSET), word, 32*1024);
  memset ((Void*)(regBaseAddr + ICONT2_ITCM_OFFSET), word, 32*1024);


  /*------------------------------------------------------------------------*/
  /* Fill all ECD3 buffers with some data                                   */
  /*------------------------------------------------------------------------*/
  /*------------------------------------------------------------------------*/
  /* Set all buffers to full view mode                                      */
  /*------------------------------------------------------------------------*/
  *(XDAS_UInt32 *)(regBaseAddr + ECD3_CFG_OFFSET + 0x200)  = 0;

  /*------------------------------------------------------------------------*/
  /* Get owner ship of all buffers                                          */
  /*------------------------------------------------------------------------*/
  *(XDAS_UInt32 *)(regBaseAddr + ECD3_CFG_OFFSET + 0x200 + 0x4)  = 0;
  *(XDAS_UInt32 *)(regBaseAddr + ECD3_CFG_OFFSET + 0x200 + 0x8)  = 0;

  /*------------------------------------------------------------------------*/
  /* Fill buffers with pattern                                              */
  /*------------------------------------------------------------------------*/
  memset ((Void*)(regBaseAddr + ECD3_MEM_OFFSET), word, 0x1000);
  memset ((Void*)(regBaseAddr + ECD3_MEM_OFFSET + 0x2000), word, 0x1800);
  memset ((Void*)(regBaseAddr + ECD3_MEM_OFFSET + 0x6000), word, 0x1000);

  /*------------------------------------------------------------------------*/
  /* Fill all MC3 buffers with some data                                   */
  /*------------------------------------------------------------------------*/
  /* Set all buffers to full view mode                                      */
  /*------------------------------------------------------------------------*/
  *(XDAS_UInt32*)(regBaseAddr + MC3_CFG_OFFSET + 0x200)   = 0;
  /*------------------------------------------------------------------------*/
  /* Get owner ship of all buffers                                          */
  /*------------------------------------------------------------------------*/
  *(XDAS_UInt32*)(regBaseAddr + MC3_CFG_OFFSET + 0x200 + 4)   = 0;
  *(XDAS_UInt32*)(regBaseAddr + MC3_CFG_OFFSET + 0x200 + 8)   = 0;
  /*------------------------------------------------------------------------*/
  /* Fill buffers with pattern                                              */
  /*------------------------------------------------------------------------*/
  memset ((Void*)(regBaseAddr + MC3_MEM_OFFSET), word, 10*1024);
  memset ((Void*)(regBaseAddr + MC3_MEM_OFFSET + 0x5000), word, 2*1024);

  /*------------------------------------------------------------------------*/
  /* Fill all CALC3 buffers with some data                                   */
  /*------------------------------------------------------------------------*/
  /* Set all buffers to full view mode                                      */
  /*------------------------------------------------------------------------*/
  *(XDAS_UInt32*)(regBaseAddr + CALC3_CFG_OFFSET + 0x200) = 0;
  /*------------------------------------------------------------------------*/
  /* Get owner ship of all buffers                                          */
  /*------------------------------------------------------------------------*/
  *(XDAS_UInt32*)(regBaseAddr + CALC3_CFG_OFFSET + 0x200 + 4)   = 0;
  *(XDAS_UInt32*)(regBaseAddr + CALC3_CFG_OFFSET + 0x200 + 8)   = 0;
  /*------------------------------------------------------------------------*/
  /* Fill buffers with pattern                                              */
  /*------------------------------------------------------------------------*/
  memset ((Void*)(regBaseAddr + CALC3_MEM_OFFSET), word, 8*1024);
  memset ((Void*)(regBaseAddr + CALC3_MEM_OFFSET + 0x2000), word,
                                                                 4*1024);
  memset ((Void*)(regBaseAddr + CALC3_MEM_OFFSET + 0x3000), word,
                                                                   4*1024);

  /*---------------------------------------------------------------------*/
  /* after IP corruption Turn off clocks                                 */
  /*---------------------------------------------------------------------*/
  TestApp_turnOffClocks();

}
/**
 *******************************************************************************
 *  @fn    TestApp_turnOnClocks
 *
 *  @brief This function turns on the clocks for all the required IPs for
 *         decoding. Note that here we dont check whether the clock is already
 *         enabled or not.
 *
 *  @param[in] : none
 *
 *  @return : APP_EOK/APP_EFAIL
 *******************************************************************************
*/
XDAS_UInt32 TestApp_turnOnClocks()
{
  XDAS_UInt32 regBaseAddr = (XDAS_UInt32)regBaseAdderss[appIvahdId];

  /*--------------------------------------------------------------------------*/
  /* Enable interrupts for all the required IPs. Here are the explanantion    */
  /* for the bit fields that we write into the clock control reg              */
  /*  31:11  Reserved  Reserved                                               */
  /*  10  SMSET  Clock control of SMSET                                       */
  /*    0: Exit idle state and start SMSET clock                              */
  /*    1: Request SMSET to go to idle state and stop SMSET clock             */
  /*        Note: Shutting-down SMSET clock may hang system                   */
  /*        if SW performs SW instrumentation and/or access                   */
  /*        to its configuration port.                                        */
  /*  9  MSGIF  Clock control of MSGIF                                        */
  /*    0: Exit idle state and start MSGIF clock                              */
  /*    1: Request MSGIF to go to idle state and stop MSGIF clock             */
  /*  8  ECD3  Clock control of ECD3                                          */
  /*    0: Exit idle state and start ECD3 clock                               */
  /*    1: Request ECD3 to go to idle state and stop ECD3 clock               */
  /*  7  MC3  Clock control of MC3                                            */
  /*    0: Exit idle state and start MC3 clock                                */
  /*    1: Request MC3 to go to idle state and stop MC3 clock                 */
  /*  6  IPE3  Clock control of IPE3                                          */
  /*    0: Exit idle state and start IPE3 clock                               */
  /*    1: Request IME3 to go to idle state and stop IPE3 clock               */
  /*  5  CALC3  Clock control of CALC3                                        */
  /*    0: Exit idle state and start CALC3 clock                              */
  /*    1: Request CALC3 to go to idle state and stop CALC3 clock             */
  /*  4  ILF3  Clock control of ILF3                                          */
  /*    0: Exit idle state and start ILF3 clock                               */
  /*    1: Request ILF3 to go to idle state and stop ILF3 clock               */
  /*  3  IME3  Clock control of IME3                                          */
  /*    0: Exit idle state and start IME3 clock                               */
  /*    1: Request IME3 to go to idle state and stop IME3 clock               */
  /*  2  VDMA  Clock control of VDMA                                          */
  /*    0: Exit idle state and start VDMA clock                               */
  /*    1: Request VDMA to go to idle state and stop VDMA clock               */
  /*  1  ICONT2  Clock control of ICONT2                                      */
  /*    0: Exit idle state and start ICONT2 clock                             */
  /*    1: Request ICONT2 to go to idle state and stop ICONT2 clock           */
  /*  0  ICONT1  Clock control of ICONT1                                      */
  /*    0: Exit idle state and start ICONT1 clock                             */
  /*    1: Request ICONT1 to go to idle state and stop ICONT1 clock           */
  /*                                                                          */
  /*--------------------------------------------------------------------------*/

  /*--------------------------------------------------------------------------*/
  /* Here we DONT enable for following IPs                                    */
  /* IME3 (3), IPE3(6)                                                        */
  /* If SMSET trace is needed for debugging we need to make the bit 10 to 0   */
  /*--------------------------------------------------------------------------*/
  volatile int status;
  XDAS_UInt32 clock_cntl = 0x00000048; /*See bit defs above*/
  XDAS_UInt32 timeOutCounter = 1000;

  *((XDAS_UInt32 *)(regBaseAddr + SYS_CLKCTRL_OFFSET)) = clock_cntl;

  status = *((XDAS_UInt32 *)(regBaseAddr + SYS_CLKSTATUS_OFFSET));

  while((((~(status)) & 0x7FF) != clock_cntl) &&
       (timeOutCounter > 0))
  {
    timeOutCounter--;
  }

  if(timeOutCounter == 0)
  {
    return APP_EFAIL;
  }

  return APP_EOK;
}

/**
 *******************************************************************************
 *  @fn     TestApp_turnOffClocks
 *
 *  @brief This function turns off the clocks for all the required IPs for
 *         decoding. Note that here we dont check whether the clock is already
 *         disabled or not.
 *
 *         In ordert o completly disable the clock, we need to make sure some
 *         of these things.
 *          -# All IPs interrupts need to be cleared. This mostly happens in
 *             LSE IPs. So clear all the interrupts before turing off the clock
 *          -# For ECD in error case we need to do reset. This currently we are
 *             doing even for normal case
 *
 *  @param[in] : none
 *
 *  @return : APP_EOK/APP_EFAIL
 *******************************************************************************
*/
XDAS_UInt32 TestApp_turnOffClocks()
{
  XDAS_UInt32 regBaseAddr = (XDAS_UInt32)regBaseAdderss[appIvahdId];

  volatile int status;
  XDAS_UInt32 timeOutCounter = 1000;
  XDAS_UInt32 clockCntl;

  clockCntl                   = 0x000003F8;

  *((XDAS_UInt32 *)(regBaseAddr + SYS_CLKCTRL_OFFSET)) = clockCntl;

  do{
      status = *((XDAS_UInt32 *)(regBaseAddr + SYS_CLKSTATUS_OFFSET));
      timeOutCounter --;
    }while ((status !=  ((~clockCntl) & 0x7FF) ) && (timeOutCounter > 0));

  if(timeOutCounter == 0)
  {
    return APP_EFAIL;
  }

  return APP_EOK;
}
#endif

/**
********************************************************************************
 *  @func   IVAHD_Standby_power_on_uboot
 *
 *  @brief  This function puts given IVAHD in standby after power on during
 *          booting process of given SoC
 *
 *  @param  None
 *
 *  @return TRUE/FALSE
 *
 *  @note   Please make to define address as per given SoC, IVAHD# and MMU
 *          Setting
********************************************************************************
*/
XDAS_Void IVAHD_Standby_power_on_uboot(XDAS_UInt32 IVAHD_CONFIG_REG_BASE,
                                      XDAS_UInt32 PRCM_IVAHD_ICONT_RST_CNTL_ADDR
                                       )
{
  unsigned int length =0;

  /*--------------------------------------------------------------------------*/
  /* Assigment of pointers                                                    */
  /* A generic code shall take all address as input parameters                */
  /*--------------------------------------------------------------------------*/
  /*volatile unsigned int *ivahd_config_base_addr =
              (unsigned int *)IVAHD_CONFIG_REG_BASE;*/
  volatile unsigned int *prcm_ivahd_icont_rst_cntl_addr =
              (unsigned int *)PRCM_IVAHD_ICONT_RST_CNTL_ADDR;
  volatile unsigned int *icont1_itcm_base_addr =
              (unsigned int *)(IVAHD_CONFIG_REG_BASE + ICONT1_ITCM_OFFSET);
  volatile unsigned int *icont2_itcm_base_addr =
              (unsigned int *)(IVAHD_CONFIG_REG_BASE + ICONT2_ITCM_OFFSET);
  /*--------------------------------------------------------------------------*/
  /* Set IVAHD in reset mode to enable downloading of boot code               */
  /* Please note that this state can be SKIPPED if IVAHD is alredy in reset   */
  /* state during uboot and reset is not de-asserted                          */
  /* Set bit0 to 1 to put ICONT1 in reset state                               */
  /* Set bit1 to 1 to put ICONT2 in reset state                               */
  /*--------------------------------------------------------------------------*/
#if defined(HOST_ARM9)
  *prcm_ivahd_icont_rst_cntl_addr &=  0xFFFFFFF3;
#else
  *prcm_ivahd_icont_rst_cntl_addr |=  0x00000003;
#endif

  /*--------------------------------------------------------------------------*/
  /* Copy boot code to ICONT1 & INCOT2 memory                                 */
  /*--------------------------------------------------------------------------*/
  for (length=0; length<LENGTH_BOOT_CODE; length++) {
    *icont1_itcm_base_addr++ = IVAHD_memory_wfi[length];
    *icont2_itcm_base_addr++ = IVAHD_memory_wfi[length];
  }
  /*--------------------------------------------------------------------------*/
  /* Take IVAHD out of reset mode.                                            */
  /* Set bit0 to 0 to take ICONT1 out of reset state                          */
  /* Set bit1 to 0 to take ICONT1 out of reset state                          */
  /* This implies ICONT inside IVAHD will exectute WFI                        */
  /*--------------------------------------------------------------------------*/
#if defined(HOST_ARM9)
  *prcm_ivahd_icont_rst_cntl_addr |=  0x0000000C;
#else
  *prcm_ivahd_icont_rst_cntl_addr &=  0xFFFFFFFC;
#endif

  /*--------------------------------------------------------------------------*/
  /* As ICONT goes in WFI and there are no pending VDMA transction            */
  /* entire IVAHD will be go in standby mode and PRCM will fully control      */
  /* further managment of IVAHD power state                                   */
  /*--------------------------------------------------------------------------*/

  return;
}

