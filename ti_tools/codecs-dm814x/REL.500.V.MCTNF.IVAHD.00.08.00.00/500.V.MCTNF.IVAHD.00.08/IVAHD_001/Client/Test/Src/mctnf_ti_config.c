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
 * @file mctnf_ti_config.c
 *
 * @brief This File contains function definitions for a standard
 *        implementation of a test configuration file parser.
 *        These functiosn parses the input  configuration files and
 *        assigns user provided values to global instance structures
 *
 *
 * @author: Rama Mohana Reddy (rama.mr@ti.com)
 *
 * @version 0.0 (Jan 2008) : Base version created
 *                           [Pramod]
 * @version 0.1 (Jan 2013) : Cleaned up and added parameters for MCTNF.
 *                           [Shyam Jagannathan]
 *****************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include <mctnf_ti_config.h>
#include <mctnf_ti_test.h>
#include <mctnf_ti_rman_config.h>


/*----------------------------------------------------------------------------*/
/* Global instance parameters                                                 */
/*----------------------------------------------------------------------------*/
MCTNFConfig            gConfig;
MCTNF_Params             gParams;
MCTNF_DynamicParams      gDynamicParams;
IVIDEO2_BufDesc            gInputBufDesc;
IVIDEO2_BufDesc            gOutputBufDesc;


#ifdef SCRATCH_CONTAMINATE
#include <..\..\..\algsrc\inc\mctnf_ti_object_size.h>
#include <..\..\..\algsrc\inc\dataFormatMCTNF.h>
extern XDAS_Int8 index;
extern XDAS_Int8 pattren[5];
#endif
/*----------------------------------------------------------------------------*/
/* Global instance log generation                                             */
/*----------------------------------------------------------------------------*/
sLogGenerate gLogMessge;

/*----------------------------------------------------------------------------*/
/*  Global Structure variables for profile info                               */
/*----------------------------------------------------------------------------*/

sProfileTimeStamps profileTimeStamps;
sProfileData profileData;

extern XDM_DataSyncHandle fGetInpDHandle1;
extern XDM_DataSyncHandle fGetInpDHandle2;
extern XDM_DataSyncDesc   ip_datasyncDesc1;
extern XDM_DataSyncDesc   ip_datasyncDesc2;
extern XDAS_Int32  numFramesFiltered;
extern XDAS_Int32 stopPos;

/*----------------------------------------------------------------------------*/
/* Error strings which are mapped to MCTNF errors                             */
/* Please refer User guide for more details on error strings                  */
/*----------------------------------------------------------------------------*/
static sErrorMapping gErrorStrings[32] =
{
  (XDAS_Int8 *)"IMCTNF_IMPROPER_HDVICP2_STATE = 16\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_DEVICE = 18,\0",
  (XDAS_Int8 *)"IMCTNF_IMPROPER_DATASYNC_SETTING = 19,\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_VIDNF1PARAMS = 20,\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_MOTIONSEARCHPARAMS = 21,\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_FIELDSELECTIONTYPE = 23,\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_NOISEFILTERPARAMS = 26,\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_MCTNFPARAMS = 29,\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_VIDNF1DYNAMICPARAMS = 30,\0",
  (XDAS_Int8 *)"IMCTNF_UNSUPPORTED_MCTNFDYNAMICPARAMS = 31, \0"
};

/*----------------------------------------------------------------------------*/
/* Array of elements of type sTokenMapping for parsing and holding the tokens */
/* from the input configuration file.                                         */
/*----------------------------------------------------------------------------*/
static sTokenMapping sTokenMap[MAX_ITEMS_TO_PARSE] =
{
  /*--------------------------------------------------------------------------*/
  /* Input file name along with path and type of element is string            */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"InputFile",&gConfig.inputFile,0},
  /*--------------------------------------------------------------------------*/
  /* Input file name along with path and type of element is string            */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"OutputFile",&gConfig.outputFile,0},
  /*--------------------------------------------------------------------------*/
  /* Reference file to bitmatch with Output File                              */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"ReferenceFile",&gConfig.refFile,0},
  /*--------------------------------------------------------------------------*/
  /* Preset to control MCTNF quality.                                         */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"FilterPreset",&gParams.vidnf1Params.filterPreset,
                                  sizeof(gParams.vidnf1Params.filterPreset)},

  /*--------------------------------------------------------------------------*/
  /* Max height of the input frame supported, used at create time to init     */
  /* MCTNF instance.                                                          */
  /* Supported values are : 80 to 1088 in case of progressive                 */
  /*                        80 to 544 in case of interlaced                   */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"MaxHeight",&gParams.vidnf1Params.maxHeight,
                                       sizeof(gParams.vidnf1Params.maxHeight)},
  /*--------------------------------------------------------------------------*/
  /* Max Width of the input frame supported, used at create time to init      */
  /* MCTNF instance.                                                          */
  /* Supported values are : 96 to 1920                                        */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"MaxWidth",&gParams.vidnf1Params.maxWidth,
                                        sizeof(gParams.vidnf1Params.maxWidth)},
  /*--------------------------------------------------------------------------*/
  /* Endianness of data supports only big endian format                       */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"DataEndianess", &gParams.vidnf1Params.dataEndianness,
                                  sizeof(gParams.vidnf1Params.dataEndianness)},
  /*--------------------------------------------------------------------------*/
  /* Input data chroma format, supports only semi planar chroma format        */
  /* XDM_YUV_420SP                                                            */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"InputChromaFormat",&gParams.vidnf1Params.inputChromaFormat,
                               sizeof(gParams.vidnf1Params.inputChromaFormat)},
  /*--------------------------------------------------------------------------*/
  /* Input buffer content type : progresive(0) interlaced (1)                 */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"InputContentType",&gParams.vidnf1Params.inputContentType,
                                sizeof(gParams.vidnf1Params.inputContentType)},
  /*--------------------------------------------------------------------------*/
  /* Video buffer layout, field interleaved or field separated                */
  /* Only IVIDEO_FIELD_INTERLEAVED and IVIDEO_FIELD_SEPARATED are supported   */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"dataLayout",&gInputBufDesc.dataLayout,
                                              sizeof(gInputBufDesc.dataLayout)},
  /*--------------------------------------------------------------------------*/
  /* Parameter to filter in 30 process call and 60 process call mode          */
  /*   0 - Filter in 30 process call mode                                     */
  /*   1 - Filter in 60 process call mode                                     */
  /*       When data layout is 0 this parameter is ignored                    */
  /*   Default - 0                                                            */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"numProcessCall",&gConfig.numProcessCall,
                                                sizeof(gConfig.numProcessCall)},
  /*--------------------------------------------------------------------------*/
  /* Method of feeding input YUV to MCTNF                                     */
  /*    Supports IVIDEO_FIXEDLENGTH, and IVIDEO_ENTIREFRAME modes             */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"InputDataMode",&gParams.vidnf1Params.inputDataMode,
                                   sizeof(gParams.vidnf1Params.inputDataMode)},
  /*--------------------------------------------------------------------------*/
  /* Method of providing filtered YUV to the appication                       */
  /* Supports IVIDEO_ENTIREFRAME,IVIDEO_NUMROWS only                          */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"OutputDataMode",&gParams.vidnf1Params.outputDataMode,
                                  sizeof(gParams.vidnf1Params.outputDataMode)},
  /*--------------------------------------------------------------------------*/
  /* Total number of frames to filter                                         */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"NumInputUnits",&gConfig.numInputDataUnits,
                                             sizeof(gConfig.numInputDataUnits)},
  /*--------------------------------------------------------------------------*/
  /* Size of the input data buffer in each chunk                              */
  /* This value to be provided in terms of no of rows in frame                */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"NumInputBufferUnits",&gParams.vidnf1Params.numInputDataUnits,
                              sizeof(gParams.vidnf1Params.numInputDataUnits)},
  /*--------------------------------------------------------------------------*/
  /* Number of data units per frame                                           */
  /* Supports only 1 data unit per frame                                      */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"NumOutputUnits",&gParams.vidnf1Params.numOutputDataUnits,
                              sizeof(gParams.vidnf1Params.numOutputDataUnits)},
  /*--------------------------------------------------------------------------*/
  /* Width of input YUV                                                       */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"inputWidth",&gDynamicParams.vidnf1DynamicParams.inputWidth,
                        sizeof(gDynamicParams.vidnf1DynamicParams.inputWidth)},
  /*--------------------------------------------------------------------------*/
  /* Height of input YUV                                                      */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"inputHeight",&gDynamicParams.vidnf1DynamicParams.inputHeight,
                       sizeof(gDynamicParams.vidnf1DynamicParams.inputHeight)},

  /*--------------------------------------------------------------------------*/
  /* Number of past filtered frames for reference.                            */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"numPastRef",
                &gDynamicParams.vidnf1DynamicParams.numPastRef,
                sizeof(gDynamicParams.vidnf1DynamicParams.numPastRef)},
  /*--------------------------------------------------------------------------*/
  /* Number of future unfiltered frames for reference.                        */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"numFutureRef",
                &gDynamicParams.vidnf1DynamicParams.numFutureRef,
                sizeof(gDynamicParams.vidnf1DynamicParams.numFutureRef)},

  /*--------------------------------------------------------------------------*/
  /* Image capture width                                                      */
  /* If the field is set to:                                                  */
  /*  0 - Input image width is used as pitch.                                 */
  /*  Any non-zero value, capture width is used as pitch (if capture width    */
  /*  is greater than image width).                                           */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"captureWidth",
                              &gDynamicParams.vidnf1DynamicParams.captureWidth,
                      sizeof(gDynamicParams.vidnf1DynamicParams.captureWidth)},
  /*--------------------------------------------------------------------------*/
  /* Image capture height                                                     */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"captureHeight",
                          &gConfig.captureHeight,sizeof(gConfig.captureHeight)},
  /*--------------------------------------------------------------------------*/
  /* Exact source position of the pixel to filter in input buffer X direction */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"captureTopLeftx",
                      &gConfig.captureTopLeftx,sizeof(gConfig.captureTopLeftx)},
  /*--------------------------------------------------------------------------*/
  /* Exact source position of the pixel to filter in input buffer Y direction */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"captureTopLefty",
                      &gConfig.captureTopLefty,sizeof(gConfig.captureTopLefty)},
  /*motion search params*/
  /*--------------------------------------------------------------------------*/
  /* Motion search preset                                                     */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"motionSearchPreset",
                                   &gParams.motionSearchParams.motionSearchPreset,
                           sizeof(gParams.motionSearchParams.motionSearchPreset)},
  /*--------------------------------------------------------------------------*/
  /* Horizontal Search Range                                                  */
  /* Supported values are                                                     */
  /*    16 to 144                                                             */
  /*    Default value is 144                                                  */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"searchRangeHor",&gParams.motionSearchParams.searchRangeHor,
                             sizeof(gParams.motionSearchParams.searchRangeHor)},
  /*--------------------------------------------------------------------------*/
  /* Vertical Search Range                                                    */
  /* Supported values are                                                     */
  /*    16 to 32                                                              */
  /*    Default value is 32                                                   */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"searchRangeVer",&gParams.motionSearchParams.searchRangeVer,
                             sizeof(gParams.motionSearchParams.searchRangeVer)},
  /*--------------------------------------------------------------------------*/
  /* Nosie Filter Params                              */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"meEffectivenessTh",&gDynamicParams.noiseFilterParams.meEffectivenessTh,
                                         sizeof(gDynamicParams.noiseFilterParams.meEffectivenessTh)},
  {(XDAS_Int8 *)"blendingFactorQ4",&gDynamicParams.noiseFilterParams.blendingFactorQ4,
                                         sizeof(gDynamicParams.noiseFilterParams.blendingFactorQ4)},
  {(XDAS_Int8 *)"minBlendQ4",&gDynamicParams.noiseFilterParams.minBlendQ4,
                                         sizeof(gDynamicParams.noiseFilterParams.minBlendQ4)},
  {(XDAS_Int8 *)"maxBlendQ4",&gDynamicParams.noiseFilterParams.maxBlendQ4,
                                         sizeof(gDynamicParams.noiseFilterParams.maxBlendQ4)},
  {(XDAS_Int8 *)"meLambdaType",&gDynamicParams.noiseFilterParams.meLambdaType,
                                         sizeof(gDynamicParams.noiseFilterParams.meLambdaType)},
  {(XDAS_Int8 *)"meLambdaFactorQ2",&gDynamicParams.noiseFilterParams.meLambdaFactorQ2,
                                           sizeof(gDynamicParams.noiseFilterParams.meLambdaFactorQ2)},
  {(XDAS_Int8 *)"mvXThForStaticLamda",&gDynamicParams.noiseFilterParams.mvXThForStaticLamda,
                                           sizeof(gDynamicParams.noiseFilterParams.mvXThForStaticLamda)},
  {(XDAS_Int8 *)"mvYThForStaticLamda",&gDynamicParams.noiseFilterParams.mvYThForStaticLamda,
                                           sizeof(gDynamicParams.noiseFilterParams.mvYThForStaticLamda)},
  {(XDAS_Int8 *)"maxLambdaQ2",&gDynamicParams.noiseFilterParams.maxLambdaQ2,
                                           sizeof(gDynamicParams.noiseFilterParams.maxLambdaQ2)},
  {(XDAS_Int8 *)"sadForMinLambda",&gDynamicParams.noiseFilterParams.sadForMinLambda,
                                         sizeof(gDynamicParams.noiseFilterParams.sadForMinLambda)},
  {(XDAS_Int8 *)"fixWtForCurQ8",&gDynamicParams.noiseFilterParams.fixWtForCurQ8,
                                         sizeof(gDynamicParams.noiseFilterParams.fixWtForCurQ8)},
  {(XDAS_Int8 *)"minWtForCurQ8",&gDynamicParams.noiseFilterParams.minWtForCurQ8,
                                         sizeof(gDynamicParams.noiseFilterParams.minWtForCurQ8)},
  {(XDAS_Int8 *)"biasZeroMotion",&gDynamicParams.noiseFilterParams.biasZeroMotion,
                                         sizeof(gDynamicParams.noiseFilterParams.biasZeroMotion)},
  {(XDAS_Int8 *)"staticMBThZeroMV",&gDynamicParams.noiseFilterParams.staticMBThZeroMV,
                                         sizeof(gDynamicParams.noiseFilterParams.staticMBThZeroMV)},
  {(XDAS_Int8 *)"staticMBThNonZeroMV",&gDynamicParams.noiseFilterParams.staticMBThNonZeroMV,
                                         sizeof(gDynamicParams.noiseFilterParams.staticMBThNonZeroMV)},
  {(XDAS_Int8 *)"blockinessRemFactor",&gDynamicParams.noiseFilterParams.blockinessRemFactor,
                                         sizeof(gDynamicParams.noiseFilterParams.blockinessRemFactor)},
  {(XDAS_Int8 *)"sadForMaxStrength",&gDynamicParams.noiseFilterParams.sadForMaxStrength,
                                         sizeof(gDynamicParams.noiseFilterParams.sadForMaxStrength)},

  /*--------------------------------------------------------------------------*/
  /* Search center for Motion estimation i.e global motion vector in X dir    */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"searchCenter_x",&gDynamicParams.searchCenter.x,
                                         sizeof(gDynamicParams.searchCenter.x)},
  /*--------------------------------------------------------------------------*/
  /* Search center for Motion estimation i.e global motion vector in X dir    */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"searchCenter_y",&gDynamicParams.searchCenter.y,
                                         sizeof(gDynamicParams.searchCenter.y)},

  /*--------------------------------------------------------------------------*/
  /* Parameter to get tiler space for luma and chorma buffers                 */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"lumaTilerSpace",&gConfig.tilerSpace[0],
                                                sizeof(gConfig.tilerSpace[0])},

  {(XDAS_Int8 *)"chromaTilerSpace",&gConfig.tilerSpace[1],
                                                sizeof(gConfig.tilerSpace[1])},

  /*--------------------------------------------------------------------------*/
  /* Parameter to get Ivahd ID                                                */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"ivahdid",&gConfig.ivahdid, sizeof(gConfig.ivahdid)},
  /*--------------------------------------------------------------------------*/
  /* Offset in input sequence for the encoding to start                       */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"startFrame",&gConfig.startFrame,4},

  /*--------------------------------------------------------------------------*/
  /* Parameter to enable enableAnalyticinfo                                   */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"enableAnalyticinfo", &gParams.enableAnalyticinfo,
                                           sizeof(gParams.enableAnalyticinfo)},

  /*--------------------------------------------------------------------------*/
  /* Flag to enable testing of runtime parameter change                       */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"enableRunTimeTest", &gConfig.enableRunTimeTest,
                sizeof(gConfig.enableRunTimeTest)},

  /*--------------------------------------------------------------------------*/
  /* Parameter to have frame number at which the dynamic parameters to change */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"DynamicFramePos", &gConfig.DynamicFramePos,
                sizeof(gConfig.DynamicFramePos)},

  /*--------------------------------------------------------------------------*/
  /* Number of blocks to be given as input duringeach DataSyncGetData call    */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"numBlocks0", &gConfig.numBlocks[0],
                sizeof(gConfig.numBlocks[0])},

  {(XDAS_Int8 *)"numBlocks1", &gConfig.numBlocks[1],
                sizeof(gConfig.numBlocks[1])},

  {(XDAS_Int8 *)"numBlocks2", &gConfig.numBlocks[2],
                sizeof(gConfig.numBlocks[2])},

  {(XDAS_Int8 *)"numBlocks3", &gConfig.numBlocks[3],
                sizeof(gConfig.numBlocks[3])},

  {(XDAS_Int8 *)"numBlocks4", &gConfig.numBlocks[4],
                sizeof(gConfig.numBlocks[4])},

  {(XDAS_Int8 *)"numBlocks5", &gConfig.numBlocks[5],
                sizeof(gConfig.numBlocks[5])},

  {(XDAS_Int8 *)"numBlocks6", &gConfig.numBlocks[6],
                sizeof(gConfig.numBlocks[6])},

  {(XDAS_Int8 *)"numBlocks7", &gConfig.numBlocks[7],
                sizeof(gConfig.numBlocks[7])},

  /*--------------------------------------------------------------------------*/
  /* ALWAYS the last element in the map                                       */
  /*--------------------------------------------------------------------------*/
  {(XDAS_Int8 *)"\0",NULL,0}
};

/**
********************************************************************************
 *  @fn     MCTNF_TI_Scratch_Contaminate
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

void MCTNF_TI_Scratch_Contaminate(XDAS_Int32 hdvicp_status)
{
#ifdef SCRATCH_CONTAMINATE

   static XDAS_Int8 index = 0;;
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
    /*    DTCM1_SHARED section in ICONT1 DTCM                                 */
    /*------------------------------------------------------------------------*/
    memset ((Void*)(REG_BASE + ICONT1_DTCM_OFFSET),
                                                pattren[index], 0x000001D0);
    /*------------------------------------------------------------------------*/
    /*    DTCM section in ICONT1_DTCM                                         */
    /*------------------------------------------------------------------------*/
    memset ((Void*)(REG_BASE + ICONT1_DTCM_OFFSET + 0x1D0),
                                                pattren[index], 0x00002FA0);
    /*------------------------------------------------------------------------*/
    /*    DTCM section in ICONT2_DTCM                                         */
    /*------------------------------------------------------------------------*/
    memset ((Void*)(REG_BASE + ICONT2_DTCM_OFFSET + 0x400),
                                                pattren[index], 0x00000630);
    /*------------------------------------------------------------------------*/
    /*    DTCMSCRATCH section in ICONT2_DTCM                                  */
    /*------------------------------------------------------------------------*/
    memset ((Void*)(REG_BASE + ICONT2_DTCM_OFFSET + 0xa30),
                                                pattren[index], 0x00000F90);
  }
  /*--------------------------------------------------------------------------*/
  /* When the resource mode is samecodectype we can currupt total SL2 memory  */
  /*--------------------------------------------------------------------------*/
  if(hdvicp_status == 1)
  {
    memset ((Void*)(MEM_BASE), pattren[index], 256*1024);
  }
  else if(hdvicp_status == 0)
  {
    memset ((Void*)(MEM_BASE), pattren[index], SIZEOF_SSL2INFO_T);
  }
  index++ ;
  if(index == 5)
  {
    index = 0;
  }
#endif
}
/**
********************************************************************************
 *  @fn     MCTNF_TI_Init_Log
 *  @brief  This function will initialize status printing module
 *
 *          This function, this prints log like number of frames encoded,
 *          bits consumed, cycles consumed etc
 *
 *  @param[in]  fTrace_file : File pointer to the trace log file
 *
 *  @return     XDM_EOK -  when there is no fatal error
 *              XDM_EFAIL - when it is fatal error
********************************************************************************
*/

XDAS_Int32 MCTNF_TI_Init_Log(FILE * fTrace_file)
{
  /*--------------------------------------------------------------------------*/
  /* Initialize the parameters                                                */
  /*--------------------------------------------------------------------------*/
  gLogMessge.uiProcessStart = 0;
  gLogMessge.uiProcessEnd   = 0;

  profileTimeStamps.processStartTimeStamp[0] =
    profileTimeStamps.processStartTimeStamp[1] = 0;
  profileTimeStamps.processEndTimeStamp[0] =
    profileTimeStamps.processEndTimeStamp[1] = 0;
  profileTimeStamps.hdvicpWaitTimeStamp[0] =
    profileTimeStamps.hdvicpWaitTimeStamp[1] = 0;
  profileTimeStamps.hdvicpAcquireTimeStamp[0] =
    profileTimeStamps.hdvicpAcquireTimeStamp[1] = 0;
  profileTimeStamps.hdvicpDoneTimeStamp[0] =
    profileTimeStamps.hdvicpDoneTimeStamp[1] = 0;
  profileTimeStamps.hdvicpReleaseTimeStamp[0] =
    profileTimeStamps.hdvicpReleaseTimeStamp[1] = 0;

#ifdef GENERATE_PROFILE_NUMBER
  /*--------------------------------------------------------------------------*/
  /* Enable and Initialise the ICE CRUSHER TIMER                              */
  /*--------------------------------------------------------------------------*/
  //while(ICEC_CNT_CTRL != 0x40000000)
    ICEC_CNT_CTRL = 0x40000000;
    ICEC_CNT_CTRL = 0x40000000;
    ICEC_CNT_CTRL = 0x40000000;
    ICEC_CNT_CTRL = 0x40000000;
    /*------------------------------------------------------------------------*/
    /* Enable Resource                                                        */
    /*------------------------------------------------------------------------*/
     ICEC_CNT_CTRL = 0x80000000;
     ICEC_CNT_CTRL = 0x80000000 |  (1<<19) | (1<<16) ;
     ICEC_CNT_CTRL = 0x80000000 |  (1<<19) | (1<<16) | (1<<12);
#endif
#ifdef PROFILE_M3
  SYSTICKREG = 0x4;
  SYSTICKREL = SYSTICKRELVAL;
  SYSTICKREG = 0x5;
#endif
  return XDM_EOK;
}

/**
********************************************************************************
 *  @fn     MCTNF_TI_Capture_time
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

void MCTNF_TI_Capture_time(XDAS_Int32 uiCapturePoint)
{
  XDAS_UInt32 startTime;
  XDAS_UInt32 idx = numFramesFiltered % NO_OF_PROFILE_INSTANCES;

  sProfileCycles *profileCycles =
  (sProfileCycles *)&profileData.profileCycles[idx];

  switch(uiCapturePoint)
  {
    case PROCESS_START:
    {
      profileTimeStamps.processStartTimeStamp[0] = ICEC_CNT_VAL;
      profileTimeStamps.processStartTimeStamp[1] = SYSTICKVAL;
    }
    break;


    case HDVICP_ACQUIRE:
    {
      profileTimeStamps.hdvicpAcquireTimeStamp[0] = ICEC_CNT_VAL;
      profileTimeStamps.hdvicpAcquireTimeStamp[1] = SYSTICKVAL;

      startTime = profileTimeStamps.processStartTimeStamp[1];

      if((XDAS_Int32)(startTime - profileTimeStamps.hdvicpAcquireTimeStamp[1])< 0)
      {
        startTime += SYSTICKRELVAL;
      }
      profileCycles->procStart_hdvicpAcquire[1] =
      startTime - profileTimeStamps.hdvicpAcquireTimeStamp[1];
      profileCycles->procStart_hdvicpAcquire[0] =
      profileTimeStamps.hdvicpAcquireTimeStamp[0] -
        profileTimeStamps.processStartTimeStamp[0];
    }
    break;

    case HDVICP_WAIT:
    {
      profileTimeStamps.hdvicpWaitTimeStamp[0] = ICEC_CNT_VAL;
      profileTimeStamps.hdvicpWaitTimeStamp[1] = SYSTICKVAL;

      startTime = profileTimeStamps.hdvicpAcquireTimeStamp[1];

      if((XDAS_Int32)(startTime - profileTimeStamps.hdvicpWaitTimeStamp[1]) < 0)
      {
        startTime += SYSTICKRELVAL;
      }
      profileCycles->hdvicpAcquire_hdvicpWait[1] =
      startTime - profileTimeStamps.hdvicpWaitTimeStamp[1];
      profileCycles->hdvicpAcquire_hdvicpWait[0] =
      profileTimeStamps.hdvicpWaitTimeStamp[0] -
      profileTimeStamps.hdvicpAcquireTimeStamp[0];
   }
    break;

    case HDVICP_DONE:
    {
      profileTimeStamps.hdvicpDoneTimeStamp[0] = ICEC_CNT_VAL;
      profileTimeStamps.hdvicpDoneTimeStamp[1] = SYSTICKVAL;

      startTime = profileTimeStamps.hdvicpWaitTimeStamp[1];

      if((XDAS_Int32)(startTime - profileTimeStamps.hdvicpDoneTimeStamp[1]) < 0)
      {
        startTime += SYSTICKRELVAL;
      }
      profileCycles->hdvicpWait_hdvicpDone[1] =
      startTime - profileTimeStamps.hdvicpDoneTimeStamp[1];
      profileCycles->hdvicpWait_hdvicpDone[0] =
      profileTimeStamps.hdvicpDoneTimeStamp[0] -
        profileTimeStamps.hdvicpWaitTimeStamp[0];
    }
    break;
    case HDVICP_RELEASE:
    {
      profileTimeStamps.hdvicpReleaseTimeStamp[0] = ICEC_CNT_VAL;
      profileTimeStamps.hdvicpReleaseTimeStamp[1] = SYSTICKVAL;

      startTime = profileTimeStamps.hdvicpDoneTimeStamp[1];

      if((XDAS_Int32)(startTime - profileTimeStamps.hdvicpReleaseTimeStamp[1])< 0)
      {
        startTime += SYSTICKRELVAL;
      }
      profileCycles->hdvicpDone_hdvicpRelease[1] =
      startTime - profileTimeStamps.hdvicpReleaseTimeStamp[1];
      profileCycles->hdvicpDone_hdvicpRelease[0] =
      profileTimeStamps.hdvicpReleaseTimeStamp[0] -
        profileTimeStamps.hdvicpDoneTimeStamp[0];
    }
    break;

   case PROCESS_END:
   {
      profileTimeStamps.processEndTimeStamp[0] = ICEC_CNT_VAL;
      profileTimeStamps.processEndTimeStamp[1] = SYSTICKVAL;

      startTime = profileTimeStamps.hdvicpReleaseTimeStamp[1];

      if((XDAS_Int32)(startTime - profileTimeStamps.processEndTimeStamp[1]) < 0)
      {
      startTime += SYSTICKRELVAL;
      }
      profileCycles->hdvicpRelease_procEnd[1] =
      startTime - profileTimeStamps.processEndTimeStamp[1];
      profileCycles->hdvicpRelease_procEnd[0] =
      profileTimeStamps.processEndTimeStamp[0] -
      profileTimeStamps.hdvicpReleaseTimeStamp[0];
    }
    break;
  }
}

/**
********************************************************************************
 *  @fn     MCTNF_TI_Print_Log
 *  @brief  This function will print log encoding of the frame
 *          It will print frame type, number of frames filtered and cycles
 *          consumed for each file
 *  @param[in]  fTrace_file : File pointer to the trace log file
 *
 *  @return     XDM_EOK -  when there is no fatal error
 *              XDM_EFAIL - when it is fatal error
********************************************************************************
*/

XDAS_Int32 MCTNF_TI_Print_Log(FILE * fTrace_file)
{
  XDAS_UInt32 m3PreCycles , m3PostCycles, iVACycles, m3Cycles ;
  XDAS_UInt32 idx = numFramesFiltered % NO_OF_PROFILE_INSTANCES;
  sProfileCycles *profileCycles =
  (sProfileCycles *)&profileData.profileCycles[idx];

  /* Below all cycles are in M3 clock */
  m3PreCycles  = profileCycles->procStart_hdvicpAcquire[1] +
                 profileCycles->hdvicpAcquire_hdvicpWait[1] ;
  m3PostCycles = profileCycles->hdvicpDone_hdvicpRelease[1] +
                 profileCycles->hdvicpRelease_procEnd[1]  ;

  m3Cycles     = m3PreCycles + m3PostCycles ;
  iVACycles    = profileCycles->hdvicpAcquire_hdvicpWait[1] +
                 profileCycles->hdvicpWait_hdvicpDone[1] +
                 profileCycles->hdvicpDone_hdvicpRelease[1];

  /* Print of M3 MHZ in M3 unit and IVA in IVA unit */
  printf(":M3 Usage: %3.2f MHz(M3), IVA Usage : %3.2f MHz(IVA-systick)",
             ((m3Cycles*30.0)/1000000), (iVACycles*30*IVAHD_MHZ/M3_HZ));


  iVACycles    = profileCycles->hdvicpAcquire_hdvicpWait[0] +
                 profileCycles->hdvicpWait_hdvicpDone[0] +
                 profileCycles->hdvicpDone_hdvicpRelease[0];

  printf("IVA Usage : %3.2f MHz(ICE-CRUSH)",(iVACycles*30.0)/1000000);

#ifdef GENERATE_PROFILE_NUMBER
{
  XDAS_UInt32 PicInitCycles;

  XDAS_UInt32 uiField0Cycles;
//  XDAS_UInt32 uiField1Cycles;
  /*--------------------------------------------------------------------------*/
  /* Print MB total MB loop cycles here                                       */
  /* MB loop cycles are stored in last 16 bites of SL2 memory                 */
  /*--------------------------------------------------------------------------*/
  uiField0Cycles = MB_LOOP_FIELD_0;
//  uiField1Cycles = MB_LOOP_FIELD_1;

  {
    PicInitCycles = profileCycles->hdvicpWait_hdvicpDone[0] - uiField0Cycles;
    printf(" : PicInit: %3.2f MHz", (((PicInitCycles)*30.0)/1000000));
    //fprintf(fTrace_file, " : PicInit: %3d MHz",(((PicInitCycles)*30)/1000000));

    printf(" : MB Loop : %3.2f MHz",(((uiField0Cycles)*30.0)/1000000));
    //fprintf(fTrace_file, " : MB Loop : %3d MHz",
    //                                     (((uiField0Cycles)*30)/1000000));
  }
}
#endif /*#ifdef GENERATE_PROFILE_NUMBER*/

  printf("\n");
  return XDM_EOK;
}
/**
********************************************************************************
 *  @fn     MCTNF_TI_Report_Error
 *  @brief  This function will print error messages
 *
 *          This function will check for MCTNF errors which are mapped to
 *          extended errors in vidnf1status structure and prints them in console
 *          Returns XDM_EFAIL in case of fatal error
 *
 *  @param[in]  uiErrorMsg  : Extended error message
 *
 *  @param[in]  fTrace_file : File pointer to the trace log file
 *
 *  @return     XDM_EOK -  when there is no fatal error
 *              XDM_EFAIL - when it is fatal error
********************************************************************************
*/

XDAS_Int32 MCTNF_TI_Report_Error(FILE * fTrace_file,XDAS_Int32 uiErrorMsg)
{
  int i;
  if(uiErrorMsg)
  {
    /*------------------------------------------------------------------------*/
    /* Loop through all the bits in error message and map to the glibal       */
    /* error string                                                           */
    /*------------------------------------------------------------------------*/
    for (i = 0; i < 32; i ++)
    {
      if (uiErrorMsg & (1 << i))
      {
        printf("ERROR: %s \n",  gErrorStrings[i].errorName);
        //fprintf(fTrace_file, "ERROR: %s \n",  gErrorStrings[i].errorName);
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
 *  @fn     GetConfigFileContent
 *  @brief  Function to read contents of the configuration file
 *
 *          This function just checks size of the given file and copies to the
 *          Buffer created with mallocs and returns the buffer pointer.
 *          Allocated buffer will be freed out side the function once parsing of
 *          config parameters is done
 *
 *  @param[in]  fname  : File pointer to be read.
 *
 *  @return bufConfig  : Pointer to the config file data
********************************************************************************
*/
static XDAS_Int8 *GetConfigFileContent (FILE *fname)
{
  XDAS_Int32 FileSize;
  XDAS_Int8 *bufConfig;
  /*--------------------------------------------------------------------------*/
  /* Check size of the file and return in case of error                       */
  /*--------------------------------------------------------------------------*/
  if (0 != fseek (fname, 0, SEEK_END))
  {
    return 0;
  }

  FileSize = ftell (fname);
  if (FileSize < 0 || FileSize > 30000)
  {
    return 0;
  }
  /*--------------------------------------------------------------------------*/
  /* Go to the start of the file to read content                              */
  /*--------------------------------------------------------------------------*/
  if (0 != fseek (fname, 0, SEEK_SET))
  {
    return 0;
  }
  /*--------------------------------------------------------------------------*/
  /* Allocate the buffer with size as file size                               */
  /* One byte extra allocated to insert end of string delimeter               */
  /*--------------------------------------------------------------------------*/
  if ((bufConfig = malloc(FileSize + 1)) == NULL)
  {
    fprintf(stdout, "No Enough memory for Config Buffer");
  }
  FileSize = fread (bufConfig, 1, FileSize, fname);
  /*--------------------------------------------------------------------------*/
  /* Put end of string marker                                                 */
  /*--------------------------------------------------------------------------*/
  bufConfig[FileSize] = '\0';

  return bufConfig;
}/* GetConfigFileContent */

/**
********************************************************************************
 *  @fn     ParameterNameToMapIndex
 *  @brief  Returns the index number from sTokenMap for a given parameter name.
 *
 *
 *  @param[in]  s  : pointer to the TokenMap string to be searched
 *
 *  @return i  : Index number if it finds the sring in token map structure
 *               -1 in case it does not find string in tokem map structure
********************************************************************************
*/

static XDAS_Int32 ParameterNameToMapIndex (XDAS_Int8 *s)
{
  XDAS_Int32 i = 0;

  while (sTokenMap[i].tokenName != NULL)
  {
    if (0==strcmp((const char *)sTokenMap[i].tokenName, (const char *)s))
      return i;
    else
      i++;
  }

  return -1;
}/* ParameterNameToMapIndex */

/**
********************************************************************************
 *  @fn     ParseContent
 *  @brief  Parses the character array buf and writes global variable input,
 *          which is defined above. This hack will continue to be
 *          necessary to facilitate the addition of new parameters through the
 *          sTokenMap[] mechanism-Need compiler-generated addresses in sTokenMap
 *
 *
 *  @param[in]  buf : buffer to be parsed
 *
 *  @param[in]  bufsize : size of the buffer
 *
 *  @return     0 - if successfully parsed all the elements in param file and
 *                  their  values read into the memory addresses given in
 *                  token mappign array.
 *             -1 - For any file read operation related errors or if
 *                  unknown parameter names are entered or if the parameter
 *                  file syntax is not in compliance with the below
 *                  implementation.
********************************************************************************
*/

static XDAS_Int32 ParseContent (XDAS_Int8 *buf, XDAS_Int32 bufsize)
{
  /*--------------------------------------------------------------------------*/
  /* Total number of parameter initialisation lines in the file this          */
  /* excludes comment lines and blank lines if any                            */
  /*--------------------------------------------------------------------------*/
  XDAS_Int8 *items[MAX_ITEMS_TO_PARSE];
  XDAS_Int32 item = 0;
  /*--------------------------------------------------------------------------*/
  /* Index into the token map array - into which the corresponding            */
  /* parameter values needs to be initialised  from the param file            */
  /*--------------------------------------------------------------------------*/
  XDAS_Int32 MapIdx;
  /*--------------------------------------------------------------------------*/
  /* Flags which indicate the current state of the state machine              */
  /* InString - indicates that buffer pointer is currently in between         */
  /* a valid string, which inturn can contain multiple items.                 */
  /* InItem - indicates that buffer pointer is within a valid item            */
  /*--------------------------------------------------------------------------*/
  XDAS_Int32 InString = 0, InItem = 0;
  /*--------------------------------------------------------------------------*/
  /* Dynamic pointer movign allong the param file buffer byte-by-byte         */
  /*--------------------------------------------------------------------------*/
  XDAS_Int8 *p = buf;
  /*--------------------------------------------------------------------------*/
  /* end of buffer - used for terminating the parse loop                      */
  /*--------------------------------------------------------------------------*/
  XDAS_Int8 *bufend = &buf[bufsize];
  /*--------------------------------------------------------------------------*/
  /* Parameter value read from the file and loop counter                      */
  /*--------------------------------------------------------------------------*/
  XDAS_Int32 IntContent;
  XDAS_Int32 i;
  XDAS_Int32 pos = 0;
  /*--------------------------------------------------------------------------*/
  /*                                STAGE ONE                                 */
  /* Generate an argc/argv-type list in items[], without comments and         */
  /* whitespace. This is context insensitive and could be done most easily    */
  /* with lex(1).                                                             */
  /*--------------------------------------------------------------------------*/
  while (p < bufend)
  {
    switch (*p)
    {
      /*----------------------------------------------------------------------*/
      /* blank space - skip the character and go to next                      */
      /*----------------------------------------------------------------------*/
      case 13:
        p++;
      break;
      /*----------------------------------------------------------------------*/
      /* Found a comment line skip all characters until end-of-line is found  */
      /*----------------------------------------------------------------------*/
      case '#':
        /*--------------------------------------------------------------------*/
        /* Replace '#' with '\0' in case of comment immediately following     */
        /* integer or string Skip till EOL or EOF, whichever comes first      */
        /*--------------------------------------------------------------------*/
        *p = '\0';
        while (*p != '\n' && p < bufend)
          p++;
        InString = 0;
        InItem = 0;
      break;
      /*----------------------------------------------------------------------*/
      /* end of line - skip the character and go to next,reset the InItem flag*/
      /* to indicate that we are not in any valid item                        */
      /*----------------------------------------------------------------------*/
      case '\n':
        InItem = 0;
        InString = 0;
        *p++='\0';
      break;
      /*----------------------------------------------------------------------*/
      /* Whitespaces and tabs indicate end of an item                         */
      /*----------------------------------------------------------------------*/
      case ' ':
      case '\t':
        /*--------------------------------------------------------------------*/
        /* if the state machine is within a string Skip whitespace,           */
        /* leave state unchanged                                              */
        /* else Terminate non-strings once whitespace is found                */
        /*--------------------------------------------------------------------*/
        if (InString)
          p++;
        else
        {
          *p++ = '\0';
          InItem = 0;
        }
      break;
      /*----------------------------------------------------------------------*/
      /* begining or end of string - toggle the string indication flag        */
      /*----------------------------------------------------------------------*/
      case '"':
        *p++ = '\0';
        if (!InString)
        {
          items[item++] = p;
          InItem = ~InItem;
        }
        else
          InItem = 0;
        InString = ~InString;
      break;
      /*----------------------------------------------------------------------*/
      /* Any othe character is taken into the item provided the state machine */
      /* is within a valid Item                                               */
      /*----------------------------------------------------------------------*/
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
  /*--------------------------------------------------------------------------*/
  /* for all the items found - check if they correspond to any valid parameter*/
  /* names specified by the user through token map array,                     */
  /* Note: Contigous three items are grouped into triplets. Thsi oredered     */
  /* triplet is used to identify the valid parameter entry in the Token map   */
  /* array.                                                                   */
  /* First item in the triplet has to eb a parameter name.                    */
  /* Second item has to be a "=" symbol                                       */
  /* Third item has to be a integer value.                                    */
  /* Any violation of the above order in the triplets found would lead        */
  /* to error condition.                                                      */
  /*--------------------------------------------------------------------------*/
  for (i=0; i<item; i+= 3)
  {
    /*------------------------------------------------------------------------*/
    /* Get the map index into the token map array - corresponding to the      */
    /* first item - in a item-triplet                                         */
    /*------------------------------------------------------------------------*/
    if (0 <= (MapIdx = ParameterNameToMapIndex (items[i])))
    {
      /*----------------------------------------------------------------------*/
      /* Assert if the second item is indeed "=" symbol                       */
      /*----------------------------------------------------------------------*/
      if (strcmp ((const char *)"=", (const char *)items[i+1]))
      {
        printf(
          "\nfile: '=' expected as the second token in each line.");
        return -1;
      }
      /*----------------------------------------------------------------------*/
      /* depending upon the type of content convert them and populate the     */
      /* corresponding entry in the token map array with this value           */
      /*----------------------------------------------------------------------*/
      if(sTokenMap[MapIdx].type == 0)
      {
        strcpy((char *)sTokenMap[MapIdx].place, (char *)items[i+2]);
      }
      else if (sTokenMap[MapIdx].type == 4)
      {
        sscanf ((const char *)items[i+2], "%d", (&IntContent));
        * ((XDAS_Int32 *) (sTokenMap[MapIdx].place)) = IntContent;
      }
      else if (sTokenMap[MapIdx].type == 2)
      {
        sscanf ((const char *)items[i+2], "%d", &IntContent);
        * ((XDAS_Int16 *) (sTokenMap[MapIdx].place)) = IntContent;
      }
      else
      {
        sscanf ((const char *)items[i+2], "%d", &IntContent);
        * ((XDAS_Int8 *) (sTokenMap[MapIdx].place)) = IntContent;
      }

      /*---------------------------------------------------------------------*/
      /* This part of code is used in reading dynamic parameter change file  */
      /* The parameter to be changed for a particular frame that will present*/
      /* between the token "DynamicFramePos = #frameno" in dynamic parameter */
      /* change file. So for reading changed parameters for a frame,         */
      /* it should read from first "DynamicFramePos = #frameno" till the next*/
      /* "DynamicFramePos = #frameno". Here stopPos is global variable       */
      /* initialized with 1 at the start of sequence and pos is local        */
      /* variable initialized with 0. After reading the token DynamicFramePos*/
      /* for 2 times, it means parameter change for a particular frame is    */
      /* read.                                                               */
      /*---------------------------------------------------------------------*/
      if(gConfig.enableRunTimeTest)
      {
        if (0==strcmp((const char *)sTokenMap[MapIdx].tokenName,
            "DynamicFramePos"))
        {
          if(stopPos == pos)
          {
            stopPos++;
            return 0;
          }
          pos++;
        }
      }
    }
    else
    {
      printf("\nParameter Name '%s' not recognized...  \n", items[i]);
    }
  }
  return 0 ;
}/* ParseContent */

/**
********************************************************************************
 *  @fn     readparamfile
 *  @brief  Reads the entire param file contents into a global buffer,which is
 *          used for parsing and updates the params to given addresses.
 *
 *  @param[in]  file name : Name of the configuration file with path
 *
 *  @return     0 - if successfully parsed all the elements in param file and
 *                  their  values read into the memory addresses given in
 *                  token mappign array.
 *             XDM_EFAIL - For any file read operation related errors or if
 *                  unknown parameter names are entered or if the parameter
 *                  file syntax is not in compliance with the below
 *                  implementation.
********************************************************************************
*/

XDAS_Int32 readparamfile(XDAS_Int8 *configFile)
{
  XDAS_Int8 *FileBuffer = NULL;
  XDAS_Int32  retVal ;
  FILE        *fConfigFile;
  /*--------------------------------------------------------------------------*/
  /*  Open Test Config File                                                   */
  /*--------------------------------------------------------------------------*/
  fConfigFile = fopen((const char *)configFile,"r");
  /*--------------------------------------------------------------------------*/
  /*  Perform file open error check.                                          */
  /*--------------------------------------------------------------------------*/
  if (!fConfigFile)
  {
    printf("Couldn't open Parameter Config file %s.\n",configFile);
    return XDM_EFAIL;
  }
  /*--------------------------------------------------------------------------*/
  /* read the content in a buffer                                             */
  /*--------------------------------------------------------------------------*/
  FileBuffer = GetConfigFileContent(fConfigFile);

  /*--------------------------------------------------------------------------*/
  /*    Close Config Parameter File.                                          */
  /*--------------------------------------------------------------------------*/
  fclose(fConfigFile);
  /*--------------------------------------------------------------------------*/
  /* if the buffer address is NULL then return error                          */
  /*--------------------------------------------------------------------------*/
  if(FileBuffer)
  {
    /*------------------------------------------------------------------------*/
    /* Parse every string into items and group them into triplets.            */
    /* Decode these ordered triplets into correspondign indices in the global */
    /* Token Map arrray provided by the user.                                 */
    /*------------------------------------------------------------------------*/
    retVal  = ParseContent(FileBuffer,strlen((const char *)FileBuffer));
    /*------------------------------------------------------------------------*/
    /* Free the buffer allocated in "GetConfigFileContent" function           */
    /*------------------------------------------------------------------------*/
    free(FileBuffer) ;
    return retVal;
  }
  else
  {
    return XDM_EFAIL;
  }
}/* readparamfile */

/**
********************************************************************************
 *  @fn     MCTNF_TI_InitDynamicParams
 *  @brief  Function to init dynamic params with MCTNF params
 *
 *          This fucntion will update the extended dynamic params with the
 *          extended params in MCTNF params structure.
 *          This function should be called as soon as cofig params are parsed
 *          and read from the file. This is due to all the extended params will
 *          be updated in MCTNF params structure
 *
 *  @param[in] params : pointer to the XDM params interface structure
 *
 *  @param[in] dynamicParams : Pointer to t he XDM dynamicparams structure
 *
 *  @return    None
********************************************************************************
*/

XDAS_Void MCTNF_TI_InitDynamicParams(MCTNF_Params *params,
                                           MCTNF_DynamicParams *dynamicParams)
{

  dynamicParams->vidnf1DynamicParams.size  = sizeof(MCTNF_DynamicParams);

  memcpy(&((dynamicParams)->motionSearchParams),&(params->motionSearchParams),
                                           sizeof (IMCTNF_MotionSearchParams));

  gDynamicParams.vidnf1DynamicParams.getBufferFxn = MCTNFTest_DataSyncGetBufFxn;
  gDynamicParams.vidnf1DynamicParams.putDataFxn   = MCTNFTest_DataSyncPutDataFxn;
  /*--------------------------------------------------------------------------*/
  /* Set "getDataFxn" pointer to the call back function in application        */
  /*--------------------------------------------------------------------------*/
  fGetInpDHandle1 = (XDM_DataSyncHandle)&ip_datasyncDesc1;
  fGetInpDHandle2 = (XDM_DataSyncHandle)&ip_datasyncDesc2;
  gDynamicParams.vidnf1DynamicParams.getDataFxn    =  MCTNFTest_DataSyncGetDataFxn1;
  gDynamicParams.vidnf1DynamicParams.getDataHandle =  fGetInpDHandle1;

  dynamicParams->reservedDynParams[0]               = 0;
  dynamicParams->reservedDynParams[1]               = 0;
  dynamicParams->reservedDynParams[2]               = 0;
  dynamicParams->reservedDynParams[3]               = 0;

}
/**
********************************************************************************
 *  @fn     TestApp_SetInitParams
 *  @brief  Function to set default parameters
 *          This function will set default params for params function and
 *          dynamicparams function. This must be called before parsing config
 *          file so that default values are set when config file does not
 *          specify any value
 *
 *  @param[in] params : pointer to the XDM params interface structure
 *
 *  @param[in] dynamicParams : Pointer to t he XDM dynamicparams structure
 *
 *  @return    None
********************************************************************************
*/
XDAS_Void TestApp_SetInitParams(MCTNF_Params *params,
                                           MCTNF_DynamicParams *dynamicParams)
{
  /*--------------------------------------------------------------------------*/
  /* First set default values to base class XDM params structure              */
  /* Please refer User guide Table 4.5 for further details on values          */
  /*--------------------------------------------------------------------------*/
  params->vidnf1Params.filterPreset                = XDM_DEFAULT;
  params->vidnf1Params.maxHeight                     = 1088;
  params->vidnf1Params.maxWidth                      = 1920;
  params->vidnf1Params.dataEndianness                = XDM_BYTE;
  params->vidnf1Params.inputChromaFormat             = XDM_YUV_420SP ;
  params->vidnf1Params.inputContentType              = IVIDEO_PROGRESSIVE;
  params->vidnf1Params.inputDataMode                 = IVIDEO_ENTIREFRAME;
  params->vidnf1Params.outputDataMode                = 0;
  params->vidnf1Params.numInputDataUnits             = 1;
  params->vidnf1Params.numOutputDataUnits            = 2;

  /*--------------------------------------------------------------------------*/
  /* Set Inter coding params in extended XDM params structure                 */
  /* Please refer User guide Table 4.8 for further details on values          */
  /*--------------------------------------------------------------------------*/
  params->motionSearchParams.motionSearchPreset        = 0;
  params->motionSearchParams.searchRangeHor            = 144;
  params->motionSearchParams.searchRangeVer            = 32;

  params->pConstantMemory                           = 0;
  params->reservedParams[0]                         = 0;
  params->reservedParams[1]                         = 0;
  params->reservedParams[2]                         = 0;
  params->debugTraceLevel                           = 1;
  params->lastNFramesToLog                          = 0;

  params->enableAnalyticinfo = 0;
  /*--------------------------------------------------------------------------*/
  /* Set dynamic parameters                                                   */
  /* Most of the default parameters can be copied from Endoer params          */
  /*--------------------------------------------------------------------------*/
  memcpy(&((dynamicParams)->motionSearchParams),&(params->motionSearchParams),
                                           sizeof (IMCTNF_MotionSearchParams));
  /*--------------------------------------------------------------------------*/
  /* Set all the XDM dynamic values to default values                         */
  /*--------------------------------------------------------------------------*/
  dynamicParams->vidnf1DynamicParams.captureWidth = 1920;
  dynamicParams->vidnf1DynamicParams.inputHeight = 1088;
  dynamicParams->vidnf1DynamicParams.inputWidth  = 1920;
  dynamicParams->vidnf1DynamicParams.numPastRef = 1;
  dynamicParams->vidnf1DynamicParams.putDataFxn = NULL;
  dynamicParams->vidnf1DynamicParams.putDataHandle = 0;

  dynamicParams->searchCenter.x                     = 0x7FFF;
  dynamicParams->searchCenter.y                     = 0x7FFF;
  dynamicParams->reservedDynParams[0]               = 0;
  dynamicParams->reservedDynParams[1]               = 0;
  dynamicParams->reservedDynParams[2]               = 0;
  dynamicParams->reservedDynParams[3]               = 0;

  dynamicParams->noiseFilterParams.staticMBThNonZeroMV = 10;
  dynamicParams->noiseFilterParams.staticMBThZeroMV = 8;

  /*--------------------------------------------------------------------------*/
  /* Set Main gConfig structure parameter values with default values          */
  /*--------------------------------------------------------------------------*/
  gConfig.captureTopLeftx = 0;
  gConfig.captureTopLefty = 0;

  /*--------------------------------------------------------------------------*/
  /* Irrespective of content type we always use 30 process call method of     */
  /* encoding for interlced content                                           */
  /*--------------------------------------------------------------------------*/
  gConfig.numProcessCall  = 0;
  gInputBufDesc.topFieldFirstFlag = 0;
  gConfig.tilerSpace[0] =  0 ;
  gConfig.tilerSpace[1] =  0 ;

  /*--------------------------------------------------------------------------*/
  /* Set IVAHD0 as default                                                    */
  /*--------------------------------------------------------------------------*/
  gConfig.ivahdid = 0;

  /*--------------------------------------------------------------------------*/
  /* Flag to enable testing of runtime parameter change                       */
  /*--------------------------------------------------------------------------*/
  gConfig.enableRunTimeTest = 0;
  gConfig.DynamicFramePos = 0;

  /*--------------------------------------------------------------------------*/
  /* Number of blocks to be given as input duringeach DataSyncGetData call    */
  /*--------------------------------------------------------------------------*/
  gConfig.numBlocks[0] = 1;
  gConfig.numBlocks[1] = 1;
  gConfig.numBlocks[2] = 1;
  gConfig.numBlocks[3] = 1;
  gConfig.numBlocks[4] = 1;
  gConfig.numBlocks[5] = 1;
  gConfig.numBlocks[6] = 1;
  gConfig.numBlocks[7] = 1;

}

