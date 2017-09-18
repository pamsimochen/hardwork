/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file gbce_test_main.h
 *
 * This File contains declarations of structures and functions used 
 * in the MSP GBCE component's test code; targeted at MONICA/OMAP4. 
 * 
 * @path  $(DUCATIVOB)\alg\gbce\test\src\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 15-Nov-2009 Sanish Mahadik: Initial Release
 *!
 *!Revisions appear in reverse chronological order; 
 *! that is, newest first.  The date format is dd-Mon-yyyy.  
 * =========================================================================== */

#ifndef _MSP_GBCE_TEST_H
#define _MSP_GBCE_TEST_H

#ifdef __cplusplus

extern "C" {
#endif

    /* User code goes here */
    /* ------compilation control switches ------------------------- */
   /****************************************************************
    *  INCLUDE FILES                                                 
    ****************************************************************/
    /* ----- system and platform files ---------------------------- */
   /*-------program files ----------------------------------------*/
#include "../../msp_gbce.h"
#include "../../../platform/osal/timm_osal_memory.h"
#include "../../../platform/osal/timm_osal_trace.h"
   /****************************************************************
    *  EXTERNAL REFERENCES NOTE : only use if not found in header file
    ****************************************************************/
   /****************************************************************
    *  PUBLIC DECLARATIONS Defined here, used elsewhere
    ****************************************************************/
   /*--------data declarations -----------------------------------*/

    // Test platform structure. Contains all parameters for the test
    // platform.
    typedef struct {

        TIMM_OSAL_CHAR HistFileName[100];
        TIMM_OSAL_CHAR GBCEFileName[100];
        TIMM_OSAL_CHAR GammaFileName[100];
        MSP_S32 Exposure_Time;
        MSP_S16 AnalogGain;
        MSP_S16 DigitalGain;
        MSP_S16 Aperture;
        MSP_GBCE_TABLESIZE TableSize;

    } GbceTestInputType;

   /****************************************************************
    *  PRIVATE DECLARATIONS Defined here, used only here
    ****************************************************************/
   /*--------data declarations -----------------------------------*/

    /* Test Case Params */
    GbceTestInputType GBCE_TestStructure[] =               /* !<
                                                            * Test-structure
                                                            * which holds
                                                            * parameters
                                                            * corresponding
                                                            * to the
                                                            * test-cases */
    {

        {"090926_192152_XENA_histogram.txt", "Test01_GBCE.txt",
         "Test01_Gamma.txt", 13000, 256, 256, 860, MSP_GBCE_TABLESIZE_256},
        {"090928_214646_XENA_histogram.txt", "Test02_GBCE.txt",
         "Test02_Gamma.txt", 33000, 512, 256, 648, MSP_GBCE_TABLESIZE_256},
        {"090929_055246_XENA_histogram.txt", "Test03_GBCE.txt",
         "Test03_Gamma.txt", 70000, 2048, 256, 648, MSP_GBCE_TABLESIZE_256},

        {"090926_192152_XENA_histogram.txt", "Test04_GBCE.txt",
         "Test04_Gamma.txt", 13000, 256, 256, 860, MSP_GBCE_TABLESIZE_1024},
        {"090928_214646_XENA_histogram.txt", "Test05_GBCE.txt",
         "Test05_Gamma.txt", 33000, 512, 256, 648, MSP_GBCE_TABLESIZE_1024},
        {"090929_055246_XENA_histogram.txt", "Test06_GBCE.txt",
         "Test06_Gamma.txt", 70000, 2048, 256, 648, MSP_GBCE_TABLESIZE_1024}
    };

   /*--------function prototypes ---------------------------------*/

   /*--------macros ----------------------------------------------*/
    /* ! \def INPUT_PATH relative path of the input test-vector w.r.t.
     * base-image location */
#define INPUT_PATH      "..\\..\\..\\..\\..\\alg\\gbce\\test\\test_vectors\\input\\"
    // #define INPUT_PATH ".\\"

    /* ! \def OUTPUT_PATH relative path of the output test-vector w.r.t.
     * base-image location */
#define OUTPUT_PATH "..\\..\\..\\..\\..\\alg\\gbce\\test\\test_vectors\\output\\"

    /* ! \def MAX_NSF_TEST_NUM Maximum number of test-cases */
#define MAX_GBCE_TEST_NUM 6

#define MSP_GBCE_EXIT_IF(_Cond,_ErrorCode) { \
   if ((_Cond)) { \
      /* status = _ErrorCode;*/ \
      TIMM_OSAL_TraceFunction("Error :: %s : %s : %d :: Exiting because : %s\n", \
	      __FILE__, __FUNCTION__, __LINE__, #_Cond); \
      goto EXIT; \
   } \
}

    static void Update_TestParams_GBCE(MSP_GBCE_CREATE_PARAMS * pApp_params,
                                       Uint8 testnum);
    static void GBCE_Allocate_TestBuffers(Uint8 testnum,
                                          MSP_BUFHEADER_TYPE * inbuf,
                                          MSP_BUFHEADER_TYPE * outbuf,
                                          MSP_GBCE_CREATE_PARAMS * pParam);
    static MSP_ERROR_TYPE MSP_GBCE_Callback(MSP_PTR hMSP, MSP_PTR pAppData,
                                            MSP_EVENT_TYPE tEvent,
                                            MSP_OPAQUE nEventData1,
                                            MSP_OPAQUE nEventData2);

    void gbce_test_main();

#ifdef __cplusplus
}
#endif

#endif
