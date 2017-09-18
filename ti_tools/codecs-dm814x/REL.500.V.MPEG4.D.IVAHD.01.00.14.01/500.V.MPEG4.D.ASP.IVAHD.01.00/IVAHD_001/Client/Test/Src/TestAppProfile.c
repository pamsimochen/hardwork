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
 * @file <TestApp_profile.c>
 *
 * @brief  This file is a MPEG4 Sample test application
 *
 *         This file has MPEG4 sample test app functions to understand
 *         the usage of codec function calls during integration.
 *
 * @author: Ashish
 *
 * @version 0.0 (August 2011)  : Base version created
 *
 ******************************************************************************
*/

/******************************************************************************
*  INCLUDE FILES
******************************************************************************/
#include <xdc/std.h>
#include <TestAppDecoder.h>
#include <TestApp_rmanConfig.h>

/******************************************************************************
*  MACRO DEFINITIONS
******************************************************************************/
/**
 *  Mega Hertz
 */
#define MEGA_HZ   (1000000.0)


/**
 *  M3 clock frequency in Hz
 */
#ifdef NETRA
  #define M3_CLOCK_FREQ   (250 * MEGA_HZ)
#else
  #define M3_CLOCK_FREQ   (166 * MEGA_HZ)
#endif

/**
 *  IVAHD clock frequency in MHz
 */
#ifdef NETRA
  #define IVA_CLOCK_FREQ  533.0
#else
  #define IVA_CLOCK_FREQ  266.0
#endif

extern IMPEG4VDEC_Params     params;
extern sAppControlParams   appControlPrms;
extern sProfileParams      profilePrms;

extern FILE *fCycles;
extern XDAS_UInt32         frameCount;
extern XDAS_UInt32         picture_count;
extern XDAS_UInt8          appIvahdId;
XDAS_Int32                 fileOpenDone = 0;

FILE *fProfileNums;

/**
********************************************************************************
 *  @fn     ConfigureProfile
 *  @brief  Need to configure the system registers for the profiling.
 *
 *  @param[in] void          : None
 *
 *  @return none
********************************************************************************
*/
XDAS_Void ConfigureProfile()
{
#ifdef DUMP_PROFILE_NUMS
  volatile int *iceCrusherCfgRegs = (volatile int *)(regBaseAdderss[appIvahdId]+
                                                     ICEC_CNT_CTRL);
  if(fileOpenDone == 0)
  {
    fProfileNums = fopen("..\\..\\..\\Test\\TestVecs\\Output\\ProfileNums.csv",
                         "w");
    fileOpenDone = 1;
  }
  if(picture_count == 0)
  {
    fprintf(fProfileNums, "%s\n\n", appControlPrms.inFile);
  }

  /*--------------------------------------------------------------------------*/
  /* Initialize the parameters                                                */
  /*--------------------------------------------------------------------------*/
  profilePrms.startTime = 0;
  profilePrms.endTime   = 0;

  /*--------------------------------------------------------------------------*/
  /* Enable and Initialise the ICE CRUSHER TIMER (IVAHD)                      */
  /* This is a incrementing counter.                                          */
  /*--------------------------------------------------------------------------*/
  *iceCrusherCfgRegs = 0x40000000;
  *iceCrusherCfgRegs = 0x40000000;
  *iceCrusherCfgRegs = 0x40000000;
  *iceCrusherCfgRegs = 0x40000000;

  while(!(*iceCrusherCfgRegs & (1 << 28)))
  {
    *iceCrusherCfgRegs = 0x40000000;
  }

  /*--------------------------------------------------------------------------*/
  /* Enable Resource                                                          */
  /*--------------------------------------------------------------------------*/
  *iceCrusherCfgRegs = 0x80000000;
  *iceCrusherCfgRegs = 0x80000000 |  (1<<19) | (1<<16) ;
  *iceCrusherCfgRegs = 0x80000000 |  (1<<19) | (1<<16) | (1<<12);

  /*--------------------------------------------------------------------------*/
  /* Cortex-M3 counter register initialization.                               */
  /* This is a decrementing counter.                                          */
  /*--------------------------------------------------------------------------*/
  SYSTICKREG = 0x4;
  SYSTICKREL = SYSTICKRELVAL;
  SYSTICKREG = 0x5;

#endif /* #ifdef DUMP_PROFILE_NUMS */

}

/**
********************************************************************************
 *  @fn     printProfileNums
 *  @brief  Printing profiling Numbers.
 *
 *  @param[in] void          : None
 *
 *  @return none
********************************************************************************
*/
XDAS_Void printProfileNums()
{

#ifdef DUMP_PROFILE_NUMS
  XDAS_Int32 diff;
  //XDAS_UInt32 diff2;
  diff = (XDAS_Int32)(profilePrms.startTimeM3 - profilePrms.endTimeM3);
  if(diff < 0)
  {
    profilePrms.startTimeM3 += SYSTICKRELVAL;
  }
  diff = (profilePrms.startTimeM3 - profilePrms.endTimeM3);
  printf("IVA MHz (SYS) per frame : %f\n",
                               (30 * diff * IVA_CLOCK_FREQ / (M3_CLOCK_FREQ)));
  //diff2 = (profilePrms.endTime - profilePrms.startTime);
  //printf("IVA MHz (ICE) per frame : %d\n", (30 * diff2));

  //fprintf(fProfileNums,"Frame:%d,\%f\n",frameCount,((30 * diff2)/MEGA_HZ));
  //fprintf(fCycles,"|----------------------------------------------------|\n");
#endif
}

/**
********************************************************************************
 *  @fn     TestApp_captureTime
 *  @brief  Capture the time stamp at the interested position.
 *
 *  @param[in] capLoc : Tells whether before or after the function that to be
 *                      profiled.
 *
 *  @return None.
********************************************************************************
*/
XDAS_Void TestApp_captureTime(XDAS_UInt32 capLoc)
{
#ifdef DUMP_PROFILE_NUMS
  volatile int *iceCrusherCtrRegs = (volatile int *)(regBaseAdderss[appIvahdId]+
                                                     ICEC_CNT_VAL);

  if(capLoc == START_TIME)
  {
    profilePrms.startTime     = (*iceCrusherCtrRegs);
    profilePrms.startTimeM3   = SYSTICKVAL;
    profilePrms.hostStartTime = SYSTICKVAL;
  }
  else
  {
    profilePrms.endTime      = (*iceCrusherCtrRegs);
    profilePrms.endTimeM3    = SYSTICKVAL;
    profilePrms.hostEndTime  = SYSTICKVAL;
    profilePrms.endTimeM3d   = SYSTICKVAL;
    profilePrms.endTimed     = (*iceCrusherCtrRegs);
  }
  return;

#endif /* #ifdef DUMP_PROFILE_NUMS */

}

