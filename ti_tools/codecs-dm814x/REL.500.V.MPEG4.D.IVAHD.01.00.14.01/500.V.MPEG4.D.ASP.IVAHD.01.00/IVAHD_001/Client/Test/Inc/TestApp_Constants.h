/*
********************************************************************************
* HDVICP2.0 Based MPEG4 ASP Decoder
*
* "HDVICP2.0 Based MPEG4 ASP Decoder" is software module developed on TI's
*  HDVICP2 based SOCs. This module is capable of decode a 4:2:0 Raw
*  video stream of Advanced/Simple profile and also H.263 bit-stream.
*  Based on  ISO/IEC 14496-2:2003."
* Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
* ALL RIGHTS RESERVED
********************************************************************************
*/



/**
*******************************************************************************
 * @file TestApp_Constants.h
 *
 * @brief  This File contains macros & enums that will be used by
 *         Sample Test application.
 *
 * @author:  Ashish Singh
 *
 * @version 0.1 (August 2011) : Base version Created [Ashish]
 *
 ******************************************************************************
*/
#ifndef _TESTAPP_CONSTANTS_H_
#define _TESTAPP_CONSTANTS_H_

/******************************************************************************
*  INCLUDE FILES
******************************************************************************/
#include <stdio.h>

/******************************************************************************
*  MACROS
******************************************************************************/

/**
 *  Macro to enable/disable the run on OMAP4 SDC
*/
#define SIMULATOR_RUN
#undef  SIMULATOR_RUN

/**
 *  Macro to enable/disable the run on OMAP4 SDC
*/
#define DUMP_PROFILE_NUMS
#undef  DUMP_PROFILE_NUMS

/**
 *  Macro that works as control switch to enable/disable PCI file io
 *  mode.
*/
#define USE_PCI
#undef  USE_PCI

/**
 *  Macro to enable fast file IO from A9 in Linux environment
 *
*/
#define USE_HOST_FILE_IO
#undef  USE_HOST_FILE_IO

/**
 *  Macro to control the way application want to write out the yuv data
 *  in output file.
*/
#define DUMP_WITHPADDING
#undef  DUMP_WITHPADDING

/**
 *  Macro to control Seek operation.
*/
#define TEST_SEEK_OP
#undef  TEST_SEEK_OP

/**
 *  Macro to enable memory corruption test
*/
#define MEM_CORRUPTION_TEST
#undef  MEM_CORRUPTION_TEST


#define WR_MEM_32(addr, data)  *(volatile unsigned int*)(addr)=(unsigned int)(data)
#define RD_MEM_32(addr)        *(volatile unsigned int*)(addr)

/**
 *  Macro for width of the Display buffer
*/
#define IMAGE_WIDTH            2048

/**
 *  Macro for Height of the Display buffer
*/
#define IMAGE_HEIGHT           2048

#define D1_MBINFO_BUFSIZE      (45*36*208)
#define HD_MBINFO_BUFSIZE      (80*45*208)
#define FULLHD_MBINFO_BUFSIZE  (120*68*208)

/**
 *  Macro to indicate input Buffer size.
*/
#define INPUT_BUFFER_SIZE      0x000400000
#define INFLIE_READ_THRESHOLD  0x000200000

/**
 *  Macro to indicate input Buffer size.
 *  In Full file read case, allocate bigger input buffer.
*/
#define DISPLAY_YUV_SIZE       (IMAGE_WIDTH)*(IMAGE_HEIGHT )

#ifdef _QT_
  #define BIT_STREAM_SIZE        26540
#endif

/**
 *  Maximum size of the string to hold file name along with path
*/
#define STRING_SIZE            512

/**
 *  In case of TILER Memory, the stride is constant & below macro defines
 *  the pitch value.
*/
#define APP_TILER_PITCH      (0x4000)
#define CHROMA_CONTAINER_8BITS   1
#define CHROMA_CONTAINER_16BITS  2

#if defined(HOST_ARM9)
  #define VDMAVIEW_EXTMEM      (0x07FFFFFF)
#elif defined(HOST_M3)
  #define VDMAVIEW_EXTMEM      (0xFFFFFFFF)
#else
  #define VDMAVIEW_EXTMEM      (0x07FFFFFF)
#endif

#define   PADX  16
#define   PADY  16


#define FULL_IMAGE_X (IMAGE_WIDTH + 2*PADX)
#define FULL_IMAGE_Y (IMAGE_HEIGHT + 2*PADY)

#define ICEC_CNT_CTRL   (0x40050)
#define ICEC_CNT_VAL    (0x40054)
#define SYSTICKREG      (*(volatile int*)0xE000E010)
#define SYSTICKREL      (*(volatile int*)0xE000E014)
#define SYSTICKVAL      (*(volatile int*)0xE000E018)
#define SYSTICKRELVAL   0x00FFFFFF

#ifdef MEM_CORRUPTION_TEST
#define MC3_MEM_OFFSET     0x00030000
#define CALC3_MEM_OFFSET   0x00028000
#define ECD3_MEM_OFFSET    0x00020000

#define MC3_CFG_OFFSET     0x00059000
#define CALC3_CFG_OFFSET   0x00058000
#define ECD3_CFG_OFFSET    0x00059800

#define SYSCTRL_OFFSET            0x0005A400
#define SYS_CLKCTRL_OFFSET        (SYSCTRL_OFFSET + 0x50)
#define SYS_CLKSTATUS_OFFSET      (SYSCTRL_OFFSET + 0x54)
#endif /* MEM_CORRUPTION_TEST */

/**
 ******************************************************************************
 *  @enum       AppOOBchk
 *  @brief      This tells application function's return status.
 *  @details
 ******************************************************************************
*/
typedef enum _AppOOBchk
{
  APP_SAME_CONFIG = 100,
    /**<
    * This indicates to stick with same config & continue
    */
  APP_NEXT_CONFIG = 101
    /**<
    * This indicates to jump to next config by exiting current config.
    */
}AppOOBchk;

/**
 ******************************************************************************
 *  @enum       AppRetStatus
 *  @brief      This tells application function's return status.
 *  @details
 ******************************************************************************
*/
typedef enum _AppRetStatus
{
  APP_EOK,
    /**<
    * This indicates the function return status as OK
    */
  APP_EFAIL,
    /**<
    * This indicates the function return status as FAIL, but application
    * can proceed further with this status.
    */
  APP_EXIT
    /**<
    * This indicates the application has exit as some critical error occured.
    */
}AppRetStatus;

/**
 ******************************************************************************
 *  @enum       BsReadMode
 *  @brief      This tells the reading mode of input bit stream.
 *  @details
 ******************************************************************************
*/
typedef enum _BsReadMode
{
  BS_BUFFER_FULL,
    /**<
    * This indicates that read bit stream until it fills input buffer full.
    * New fill happens only when the bytes consumption by codec crosses a
    * predefined threshold.
    */
  BS_FRAME_MODE
    /**<
    * This indicates that input bit stream is read at frame basis. We read
    * exactly the bytes needed for one frame decoding at a time.
    */
}BsReadMode;

/**
 ******************************************************************************
 *  @enum       timeCapture
 *  @brief      This tells the reading mode of input bit stream.
 *  @details
 ******************************************************************************
*/
typedef enum _timeCapture
{
  START_TIME,
    /**<
    * This indicates the starting position from which we want to do profiling.
    */
  END_TIME
    /**<
    * This indicates the end position to which we want to do profiling.
    */
}timeCapture;

/**
 ******************************************************************************
 *  @enum       InputDataSyncMode
 *  @brief      Enumerations for the InputDatasync Modes
 *  @details
 ******************************************************************************
*/
typedef enum _InputDataSyncMode
{
    APP_INPUTDS_OFF = 0,
      /**< Datasync is off
      */
    APP_INPUTDS_FIXEDLENGTH = 1,
      /**< Datasync: Interms of multiples of 2K
      */
    APP_INPUTDS_SLICEMODE = 2,
      /**< Datasync: Slice Mode
      */
    APP_INPUTDS_NALMODE = 3
      /**< Datasync: NAL unit Mode without Start code
      */
}InputDataSyncMode;

/**
 ******************************************************************************
 *  @enum       metaDataMode
 *  @brief      Enumerations for the InputDatasync Modes
 *  @details
 ******************************************************************************
*/
typedef enum _appMetaDataMode
{
    APP_NO_MB_INFO = -1,
      /**< No Mb  Info required
      */
    APP_MB_INFO = 0
      /**< Macro Block header data
      */
}metaDataMode;

/**
 ******************************************************************************
 *  @enum       parBoundaryExitLevel
 *  @brief      Enumerations for exit levels during parameter boudary testing.
 *  @details
 ******************************************************************************
*/
typedef enum _parBoundaryExitLevel
{
    CREATE_TIME_LEVEL = 1,
      /**< Exit at algorithm creation time
      */
    CONTROL_CALL_LEVEL = 2
      /**< Exit at control call level
      */
}parBoundaryExitLevel;

#endif
