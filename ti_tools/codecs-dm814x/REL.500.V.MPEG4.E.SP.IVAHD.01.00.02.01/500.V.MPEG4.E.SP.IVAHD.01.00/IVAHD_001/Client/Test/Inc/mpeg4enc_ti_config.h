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
 * @file <mpeg4enc_ti_config.c>
 *
 * @brief This File contains function definitions for a standard
 *        implementation of a test configuration file parser.
 *        These functiosn parses the input  configuration files and
 *        assigns user provided values to global instance structures.
 *        Also includes implementation of datsynch APIs.
 *
 * @author: Madhukar Budagavi
 *
 * @version 0.0 (Jul 2007) : Intial version
 *                           [Madhukar Budagavi]
 *
 * @version 0.1 (Feb 2010) : Total cleanup for the target Code
 *                           [Radhesh Bhat]
 *
 *******************************************************************************
*/

#ifndef _MPEG4_ENC_TI_CONFIG_H_
#define _MPEG4_ENC_TI_CONFIG_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <mpeg4enc_ti_test.h>
#include "mpeg4enc_ti_config.h"

/* -------------------- compilation control switches -------------------------*/
#define DEFAULTCONFIGFILENAME "..\\..\\..\\Test\\TestVecs\\Config\\encoder.cfg"

/**
* Enable this incase to profile on OMAP4 SDC of FPGA. Disable this macro 
* for Simulator                                                          
*/ 
//#define PROFILE_HOST

#ifdef PROFILE_HOST

/** 
 *  Macro defining address of TIMER init module 
*/
#define ICEC_CNT_CTRL (*(volatile int*)(REG_BASE + 0x40840))
/** 
 *  Macro defining address of capture time register in ICE CRUSHER
*/
#define ICEC_CNT_VAL  (*(volatile int*)(REG_BASE + 0x40844))

/** 
 *  Macro defining address of M3 TIMER init module 
*/
#define SYSTICKREG        (*(volatile int*)0xE000E010)
#define SYSTICKREL        (*(volatile int*)0xE000E014)
#define SYSTICKVAL        (*(volatile int*)0xE000E018)
#define SYSTICKRELVAL     0x00FFFFFF

#endif//PROFILE_HOST

#ifdef NETRA_SIM
#define M3_HZ     (250000000)
#define IVAHD_MHZ (533.0)

#elif defined HOSTCORTEXM3_OMAP4
#define M3_HZ     (166000000)
#define IVAHD_MHZ (266.0)
#endif

/** 
 *  Macro defining ICONT1 DTCM offset from the base address
*/
#define ICONT1_DTCM_OFFSET 0x00000000
/** 
 *  Macro defining ICONT2 DTCM offset from the base address
*/
#define ICONT2_DTCM_OFFSET 0x00010000
/** 
 *  Macro defining ICONT1 ITCM offset from the base address
*/
#define ICONT1_ITCM_OFFSET 0x00008000
/** 
 *  Macro defining ICONT2 ITCM offset from the base address
*/
#define ICONT2_ITCM_OFFSET 0x00018000

typedef struct sErrorMapping{
  XDAS_Int8 *errorName;
}sErrorMapping;

/**
*******************************************************************************
*  @struct MPEG4EncConfig_t
*  @brief  Configuration structure to parse some of parameters in the config
*          file
*
*  @param  inputFile           : Input YUV file name
*  @param  bitstreamname       : Output bitstream file name
*  @param  reconFile           : Reconsrtructed file name
*  @param  refEncbitstreamname : Reference bitstream name to be compared with
*                                platform generated bitstream
*  @param  numFrames           : Number of frames to be encoded
*  @param  captureHeight       : Capture height
*  @param  ivahdid             : Parameter to get Ivahd ID
*  @param  frameNumber         : Frame number at which the run time changes
*                                has to happen.
*******************************************************************************
*/
typedef struct MPEG4EncConfig_t {
  S08 inputFile[FILE_NAME_SIZE];
  S08 bitstreamname[FILE_NAME_SIZE];
  S08 reconFile[FILE_NAME_SIZE];
  S08 refEncbitstreamname[FILE_NAME_SIZE];
  S08 controlFilename[FILE_NAME_SIZE];
  S08 datasynchFilename[FILE_NAME_SIZE];
  S08 runTimeChangeFilename[FILE_NAME_SIZE];
  U32 numFrames;
  U32 captureHeight;
  XDAS_Int8  ivahdid;
  XDAS_Int8  tilerSpace[2];
  XDAS_Int32 forceSKIPPeriod;
  XDAS_Int32 frameNumber;
} MPEG4EncoderConfig;


/*----------------------------------------------------------------------------*/
/* Array of elements of type sTokenMapping for parsing and holding the tokens */
/* from the input configuration file.                                         */
/*----------------------------------------------------------------------------*/
typedef struct _sTokenMapping
{
  XDAS_Int8 *tokenName;
  XDAS_Void *place;
  XDAS_Int8  bType;
}sTokenMapping;

/**
 *******************************************************************************
 *  @struct sProfileLog
 *  @brief  Structure to hold log data, like frame start time instance,
 *          frame end time istance.
 *
 *  @param  startTime     : 
 *  @param  endTime       : 
 *      AverageCycles :
 *      PeakCycles    :
 *      startTimeM3   :
 *      endTimeM3     :
 *      AverageCyclesM3:
 *      PeakCyclesM3  :
 * 
 *******************************************************************************
*/
typedef struct sProfileLog{
  XDAS_UInt32    startTime;
  XDAS_UInt32    endTime;
  XDAS_UInt32    AverageCycles;
  XDAS_UInt32    PeakCycles;
  XDAS_UInt32    startTimeM3;
  XDAS_UInt32    endTimeM3;
  XDAS_UInt32    AverageCyclesM3;
  XDAS_UInt32   PeakCyclesM3;

} sProfileLog;

typedef enum {
PRINT_EACH_FRAME = 0,
PRINT_END_FRAME  = 1
}printInstance;

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
XDAS_Int32 readparamfile(FILE *fname);

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
*/XDAS_Int8* GetConfigFileContent(FILE *fname);

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
XDAS_Int32 ParseContent(XDAS_Int8 *buf, XDAS_Int32 bufsize);

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
XDAS_Int32 ParameterNameToMapIndex (XDAS_Int8 *s);

/**
********************************************************************************
 *  @func     Initialize_map
 *  @brief    Maps the address the variables to be parsed to the token map
 *
 *  @return None
********************************************************************************
*/
U32 Initialize_map();

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
XDAS_Void TestApp_SetInitParams(MPEG4ENC_Params *params,
  MPEG4EncoderConfig *config);

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
XDAS_Void TestApp_SetDynamicParams(MPEG4ENC_DynamicParams *dynamicParams);

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

XDAS_Int32 MPEG4ENC_TI_Report_Error(XDAS_Int32 uiErrorMsg);

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
XDAS_Int32 StitchAllPackets();

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

Void MPEG4ENC_TI_Init_Profile();

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

void MPEG4ENC_TI_Capture_time(XDAS_Int32 uiCapturePoint);

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
  XDAS_Int32 bytesGenerated, XDAS_Int32 frame_nr);

#endif /*_MPEG4_ENC_TI_CONFIG_H_*/
