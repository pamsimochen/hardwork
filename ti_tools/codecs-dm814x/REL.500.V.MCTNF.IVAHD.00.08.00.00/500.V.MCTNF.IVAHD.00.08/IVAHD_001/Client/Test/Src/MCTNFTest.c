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
********************************************************************************
 * @file MCTNFTest.c
 *
 * @brief This is top level client file for IVAHD MCTNF
 *        MCTNF process call using IVIDNF1 XDM Interface
 *
 * @author: Shyam Jagannathan (shyam.jagannathan@ti.com)
 *          Deepak Kumar Poddar (deepak.poddar@ti.com)
 *
 ********************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <xdc/std.h>
#include <mctnf_ti.h>
#include "ividnf1.h"
#include <ti/xdais/xdas.h>
#include "buffermanager.h"
#include <mctnf.h>
#include <mctnf_ti_test.h>
#include <mctnf_ti_config.h>
#include "mctnf_ti_rman_config.h"
#include <mctnf_ti_file_io.h>
#include "tilerBuf.h"

FILE *fin, *fout;
/*-----------------------------------------------------------------------------*/
/*   Lenth of Boot code                                                        */
/*-----------------------------------------------------------------------------*/
#define LENGTH_BOOT_CODE  14

/**
 *  Macro to set sizes of Base Class Objects
*/
//#define BASE_CLASS

/**
 *  Macro to have the pattern to fill the extended class objects while testing
 *  base class objects
*/
#define PATTERN_FILL_EXT    0xFF

/*------------------------------------------------------------------------------*/
/*   Hex code to set for Stack setting, Interrupt vector setting                */
/*   and instruction to put ICONT in WFI mode.                                  */
/*   This shall be placed at TCM_BASE_ADDRESS of given IVAHD, which is          */
/*   0x0000 locally after reset.                                                */
/*------------------------------------------------------------------------------*/

const unsigned int IVAHD_memory_wfi[LENGTH_BOOT_CODE] =
{
    0xEA000006,
    0xEAFFFFFE,
    0xEAFFFFFE,
    0xEAFFFFFE,
    0xEAFFFFFE,
    0xEAFFFFFE,
    0xEAFFFFFE,
    0xEAFFFFFE,
    0xE3A00000,
    0xEE070F9A,
    0xEE070F90,
    0xE3A00000,
    0xEAFFFFFE,
    0xEAFFFFF1
};

XDAS_Int32 gAlgExtendedError;

#ifdef NALSTREAM_TESTING
/*----------------------------------------------------------------------------*/
/*  Output Bitstream Buffer                                                   */
/*----------------------------------------------------------------------------*/
#pragma DATA_SECTION(gOutputData, ".bitStream_sect");
volatile XDAS_UInt8 gOutputDataOrg[OUTPUT_BUFFER_SIZE];
volatile XDAS_UInt8 *gOutputData;
#else
/*----------------------------------------------------------------------------*/
/*  Output Bitstream Buffer                                                   */
/*----------------------------------------------------------------------------*/
#pragma DATA_SECTION(gOutputData, ".bitStream_sect");
volatile XDAS_UInt8 gOutputData[OUTPUT_BUFFER_SIZE];
#endif

/*----------------------------------------------------------------------------*/
/* Output Blocks sizes provided through putDataFxn()                          */
/*----------------------------------------------------------------------------*/
#pragma DATA_SECTION(gBlockSizes, ".bitStream_sect");
XDAS_Int32 gBlockSizes[TOT_BLOCKS_IN_PROCESS];
/*----------------------------------------------------------------------------*/
/* Output Blocks izes provided through putDataFxn()                          */
/*----------------------------------------------------------------------------*/
#pragma DATA_SECTION(gBlockAddr, ".bitStream_sect");
XDAS_Int32 gBlockAddr[TOT_BLOCKS_IN_PROCESS];
/*----------------------------------------------------------------------------*/
/* Total number of blocks recieved in each process call through putDataFxn()  */
/*----------------------------------------------------------------------------*/
XDAS_Int32 gOutputBlocksRecieved;

XDAS_Int32 gInitialDataUnit;
XDAS_Int32 gDataSynchUnit;
/*----------------------------------------------------------------------------*/
/*  Gap between two data synch units                                          */
/*----------------------------------------------------------------------------*/
XDAS_Int32 gDataSyncUnitGap;

/*----------------------------------------------------------------------------*/
/*  Analytic info output buffer                                               */
/*----------------------------------------------------------------------------*/
#pragma DATA_SECTION(gAnalyticInfoOutputData, ".analyticinfo_sect");
XDAS_UInt8 gAnalyticInfoOutputData[ANALYTICINFO_OUTPUT_BUFF_SIZE];

/*----------------------------------------------------------------------------*/
/* Varible holding data fed to the algorithm, helps for DATA sync APIs        */
/*----------------------------------------------------------------------------*/
XDAS_UInt32 gOutputDataProvided;
XDAS_Int32  numFramesFiltered;

/*----------------------------------------------------------------------------*/
/* Varible used to change dynamic parameter at runtime                        */
/*----------------------------------------------------------------------------*/
XDAS_Int32 stopPos;
/*---------------------------------------------------------------------------*/
/* Counter for setting MCTNF                                                 */
/*---------------------------------------------------------------------------*/
extern Uint32 FrameNum;
/*----------------------------------------------------------------------------*/
/* Input data handles, we define 2 just to check some robust ness             */
/*----------------------------------------------------------------------------*/
extern XDM_DataSyncHandle fGetInpDHandle1;
extern XDM_DataSyncHandle fGetInpDHandle2;

sTilerParams TilerParams;
extern sProfileData profileData;

/*----------------------------------------------------------------------------*/
/* Global instance parameters                                                 */
/*----------------------------------------------------------------------------*/
extern MCTNFConfig            gConfig;
extern MCTNF_Params           gParams;
extern MCTNF_DynamicParams    gDynamicParams;
extern IVIDEO2_BufDesc        gInputBufDesc;
extern IVIDEO2_BufDesc        gOutputBufDesc;

void MCTNF_TI_initInputBufDesc
(
    BUFFMGR_buffEleHandle inBufEle,
    MCTNF_Status *status,
    XDAS_Int32 uiCaptureWidth
);
void MCTNF_TI_initOutputBufDesc
(
   BUFFMGR_buffEleHandle outBufEle,
   MCTNF_Status *status
);
XDAS_Int16 TestApp_ReadInputYUVData
(
    IVIDEO2_BufDesc *inputBuf,
    XDAS_Int32       frameCount,
    FILE            *fin,
    XDAS_UInt8       lumaTilerSpace,
    XDAS_UInt8       chromaTilerSpace
);
void TestApp_WriteOutputData(FILE *pfOutYUVFile);

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
#define BITSTREAM_SUBFRAME_SYNC
XDAS_Int32 IVAHD_Standby_power_on_uboot()
{

  unsigned int length =0;
  /*--------------------------------------------------------------------------*/
  /* Assigment of pointers                                                    */
  /* A generic code shall take all address as input parameters                */
  /*--------------------------------------------------------------------------*/
  volatile unsigned int *prcm_ivahd_icont_rst_cntl_addr =
    (unsigned int *)RST_CNTL_BASE;
  volatile unsigned int *icont1_itcm_base_addr =
    (unsigned int *)(REG_BASE + ICONT1_ITCM_OFFSET);
  volatile unsigned int *icont2_itcm_base_addr =
    (unsigned int *)(REG_BASE + ICONT2_ITCM_OFFSET);
  /*--------------------------------------------------------------------------*/
  /* Set IVAHD in reset mode to enable downloading of boot code               */
  /* Please note that this state can be SKIPPED if IVAHD is alredy in reset   */
  /* state during uboot and reset is not de-asserted                          */
  /* Set bit0 to 1 to put ICONT1 in reset state                               */
  /* Set bit1 to 1 to put ICONT2 in reset state                               */
  /*--------------------------------------------------------------------------*/
  *prcm_ivahd_icont_rst_cntl_addr |=  0x00000003;

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
  *prcm_ivahd_icont_rst_cntl_addr &=  0xFFFFFFFC;

  /*--------------------------------------------------------------------------*/
  /* As ICONT goes in WFI and there are no pending VDMA transction            */
  /* entire IVAHD will be go in standby mode and PRCM will fully control      */
  /* further managment of IVAHD power state                                   */
  /*--------------------------------------------------------------------------*/

  return (1);
}

#pragma DATA_SECTION(RawLuma,   ".RawInput");
#pragma DATA_SECTION(RawChroma, ".RawInput");
XDAS_UInt8 RawLuma[2048*1256];
XDAS_UInt8 RawChroma[2048*(1256/2)];

extern const IMCTNF_Params MCTNF_TI_PARAMS;
extern const IMCTNF_DynamicParams MCTNF_TI_DYNAMICPARAMS;

/**
********************************************************************************
 *  @fn     main
 *  @brief  Sample main program implementation which uses IVIDNF1 APIs to create
 *          MCTNF instance and filter multiple input files. It supports dumping
 *          of output filtered data. Uses single instance of MCTNF.
 *
 *  @param  none
 *
 *  @return     0 - when there is no error while filtering
 *              0 - when there is error while filtering
********************************************************************************
*/
XDAS_Int32 main()  
{

  /*--------------------------------------------------------------------------*/
  /*  File I/O variables                                                      */
  /*--------------------------------------------------------------------------*/
  FILE      *foutFile, *finFile;
  FILE      *fTrace_file;
  FILE      *fTestCasesFile;
  FILE      *fProfileTrace;
  FILE      *fDynamicParamChgFile;

  /*--------------------------------------------------------------------------*/
  /* String arrays to hold the file names along with path                     */
  /*--------------------------------------------------------------------------*/
  XDM2_BufSize  inBufSizes[IVIDEO2_MAX_IO_BUFFERS];
  XDM2_BufSize  outBufSizes[IVIDEO2_MAX_IO_BUFFERS];

  XDAS_Int8     inFile[FILE_NAME_SIZE];
  XDAS_Int8     outFile[FILE_NAME_SIZE];
  XDAS_Int8     configFile[FILE_NAME_SIZE];
  XDAS_Int8     testCasesFile[FILE_NAME_SIZE];
  XDAS_Int8     scTraceFile[FILE_NAME_SIZE];
  XDAS_Int8     scProfileTrace[FILE_NAME_SIZE];
  XDAS_Int8     version[FILE_NAME_SIZE];
  XDAS_Int8     dynamicparamChgFile[FILE_NAME_SIZE];
  XDAS_UInt32   uiCaptureWidth,uiCaptureHeight,uiSize,uiSize1;
  XDAS_UInt32   uiInputWidth, uiInputHeight;
  XDAS_UInt32   testCaseCounter = 0;
  XDAS_UInt32   testCaseControl;
  XDAS_Int32    numInBufSet;
  XDAS_Int32    numOutBufSet;
  XDAS_Int32    ret, i;

  /*--------------------------------------------------------------------------*/
  /*  Declaring Algorithm specific handle                                     */
  /*--------------------------------------------------------------------------*/
  MCTNF_Handle  handle;

  /*--------------------------------------------------------------------------*/
  /* XDM extended class structre declarations                                 */
  /*--------------------------------------------------------------------------*/
  MCTNF_InArgs  inArgs;
  MCTNF_OutArgs outArgs;
  MCTNF_Status  status;

  /*--------------------------------------------------------------------------*/
  /*  Handle to a buffer element allocated by the buffer manager module       */
  /*--------------------------------------------------------------------------*/
  BUFFMGR_buffEleHandle inBufEle;
  BUFFMGR_buffEleHandle outBufEle;
  /*--------------------------------------------------------------------------*/
  /*  Other local variables                                                   */
  /*--------------------------------------------------------------------------*/
  XDAS_Int32 retValue;
  XDAS_Int32 frameNum = 0;

  numFramesFiltered = 0;
  /*--------------------------------------------------------------------------*/
  /* Initialize file IO                                                       */
  /*--------------------------------------------------------------------------*/
  init_file_io();

  /*--------------------------------------------------------------------------*/
  /* Configaration file with path used for controling MCTNF                   */
  /*--------------------------------------------------------------------------*/
  strcpy((char *)testCasesFile,"..\\..\\..\\Test\\TestVecs\\Config\\testcases.txt");

  /*--------------------------------------------------------------------------*/
  /*  Open Test Config File                                                   */
  /*--------------------------------------------------------------------------*/
  fTestCasesFile = fopen((const char *)testCasesFile,"r");
  if(fTestCasesFile == NULL)
  {
    printf("\n Unable to open TestCases List File(TestCases.txt) %s\n", testCasesFile);
    return(XDM_EFAIL);
  }

  while(!feof(fTestCasesFile))
  {
     testCaseCounter++;
     /*--------------------------------------------------------------------------*/
     /* Set Initial default values for all the parameters                        */
     /*--------------------------------------------------------------------------*/
     //memcpy(&gParams,&(MCTNF_TI_PARAMS),sizeof(IMCTNF_Params));
     TestApp_SetInitParams(&gParams,&gDynamicParams);

     /*--------------------------------------------------------------------------*/
     /* Configaration file with path used for controling MCTNF                   */
     /*--------------------------------------------------------------------------*/
     fscanf(fTestCasesFile, "%d",&testCaseControl);
     fscanf(fTestCasesFile, "%s",configFile);
     if(testCaseControl == 2)
     {
        printf("\n\n\n ******** Request to END MCTNF ******** \n");
        abort();
     }
     else if(testCaseControl == 1)
     {
        printf("\n***********************************************\n");
        printf("Request to SKIP Test Case Number : %d \n",testCaseCounter);
        printf("Config File   : %s",configFile);
        printf("\n***********************************************\n");
        continue;
     }

     printf("\n-----------------------------------------------\n");
     printf("Test Case Number : %d",testCaseCounter);
     printf("\n-----------------------------------------------\n");

     /*--------------------------------------------------------------------------*/
     /* Read Test Config Parameter File                                          */
     /*--------------------------------------------------------------------------*/
     gConfig.startFrame     = 0;
     if(readparamfile(configFile) != 0)
     {
        printf("Unable to read Config Parameter File %s\n", configFile);
        return(XDM_EFAIL);
     }
     /*--------------------------------------------------------------------------*/
     /*  Get the IVAHD Config, SL2, PRCM base address based on IVAHD ID          */
     /*--------------------------------------------------------------------------*/
     Init_IVAHDAddrSpace();

     IVAHD_Standby_power_on_uboot();
     /*--------------------------------------------------------------------------*/
     /* Below code enables hrd paramters always.                                 */
     /*--------------------------------------------------------------------------*/
     //memcpy(&gDynamicParams,&(MCTNF_TI_DYNAMICPARAMS),sizeof(IMCTNF_DynamicParams));

     MCTNF_TI_InitDynamicParams(&gParams,&gDynamicParams);

     /*--------------------------------------------------------------------------*/
     /*    Trace file to write the encoding details                              */
     /*--------------------------------------------------------------------------*/
     strcpy((char *)scTraceFile, (const char *)gConfig.outputFile);
     strcat((char *)scTraceFile, ".txt");
     fTrace_file = fopen((const char *)scTraceFile, "w");
     strcpy((char *)scProfileTrace, (const char *)gConfig.outputFile);
     strcat((char *)scProfileTrace, "_profile.bin");
     fProfileTrace = fopen((const char *)scProfileTrace, "wb");

     if((fTrace_file || fProfileTrace)== NULL)
     {
       printf("\n Unable to open Trace file to write the encoding details %s\n", scTraceFile);
       return(XDM_EFAIL);
     }

     printf("Config File   : %s\n",configFile);
     fprintf(fTrace_file, "Config File   : %s\n",configFile);
     printf("Input YUV     : %s\n",gConfig.inputFile);
     fprintf(fTrace_file, "Input YUV     : %s\n",gConfig.inputFile);
     printf("Output YUV    : %s\n\n",gConfig.outputFile);
     fprintf(fTrace_file, "Output YUV    : %s\n",gConfig.outputFile);

     /*--------------------------------------------------------------------------*/
     /* Make sure output file is proper and empty the file if it already exists  */
     /* By open and close the file and reopen the file                           */
     /*--------------------------------------------------------------------------*/
     strcpy((char *)outFile,(const char *)gConfig.outputFile);
     foutFile = fopen((const char *)outFile, "wb");
     if(foutFile == NULL)
     {
        printf("\n Unable to open Output YUV File %s\n", outFile);
        return(XDM_EFAIL);
     }
     fclose(foutFile);
     foutFile = my_fopen((const char *)outFile, "wb");

     /*--------------------------------------------------------------------------*/
     /* Open Input YUV Sequence file.                                            */
     /*--------------------------------------------------------------------------*/
     strcpy((char *)inFile, (const char *)gConfig.inputFile);
     finFile = my_fopen((const char *)inFile, "rb");
     if(finFile == NULL)
     {
        printf("\n Unable to open Input YUV Sequence File %s\n", inFile);
        my_fclose(foutFile);
        return(XDM_EFAIL);
     }

     /*--------------------------------------------------------------------------*/
     /*  Setting the sizes of Base Class Objects                                 */
     /*--------------------------------------------------------------------------*/
#ifdef BASE_CLASS
     status.vidnf1Status.size                 = sizeof(IVIDNF1_Status);
     inArgs.vidnf1InArgs.size                 = sizeof(IVIDNF1_InArgs);
     outArgs.vidnf1OutArgs.size               = sizeof(IVIDNF1_OutArgs);
     gParams.vidnf1Params.size                = sizeof(IVIDNF1_Params);
     gDynamicParams.vidnf1DynamicParams.size  = sizeof(IVIDNF1_DynamicParams);

     memset((XDAS_Int8*)&status + sizeof(IVIDNF1_Status), PATTERN_FILL_EXT,
          sizeof(MCTNF_Status) - sizeof(IVIDNF1_Status));
     memset((XDAS_Int8*)&gParams + sizeof(IVIDNF1_Params), PATTERN_FILL_EXT,
          sizeof(MCTNF_Params) - sizeof(IVIDNF1_Params));
     memset((XDAS_Int8*)&gDynamicParams + sizeof(IVIDNF1_DynamicParams), PATTERN_FILL_EXT,
          sizeof(MCTNF_DynamicParams) - sizeof(IVIDNF1_DynamicParams));
#else
     status.vidnf1Status.size                 = sizeof(MCTNF_Status);
     inArgs.vidnf1InArgs.size                 = sizeof(MCTNF_InArgs);
     outArgs.vidnf1OutArgs.size               = sizeof(MCTNF_OutArgs);
     gParams.vidnf1Params.size                = sizeof(MCTNF_Params);
     gDynamicParams.vidnf1DynamicParams.size  = sizeof(MCTNF_DynamicParams);
#endif

  /*--------------------------------------------------------------------------*/
  /* Reset the previous extended error status to avoid unnecessary failures   */
  /*--------------------------------------------------------------------------*/
  status.vidnf1Status.extendedError = 0;
  gParams.reservedParams[1] = 0x3;

  /*--------------------------------------------------------------------------*/
  /*  Algorithm create call. This function will allocate memory for the main  */
  /*  algorithm handle algorithm and initializes accordingly. The alg handle  */
  /*  is returnd.                                                             */
  /*--------------------------------------------------------------------------*/
  handle = MCTNF_create((MCTNF_Fxns *)&MCTNF_TI_IMCTNF, &gParams);

  /*--------------------------------------------------------------------------*/
  /*  Initialize the Handle to the MB processing function that is             */
  /*  required to be called by the ISR.                                       */
  /*--------------------------------------------------------------------------*/
  g_handle[0] = (IALG_Handle)handle;

  /*--------------------------------------------------------------------------*/
  /*  Check for errors in algorithm handle creation                           */
  /*--------------------------------------------------------------------------*/
  if(handle == NULL)
  {
    fprintf(stdout, "Creation Failed.\n");
    fprintf(stdout, "Check the Create-time static parameter settings.\n");
    goto DELETE_INSTANCE;
  }

  /*--------------------------------------------------------------------------*/
  /*  Get the buffers in TILER space                                          */
  /*--------------------------------------------------------------------------*/
  {
    /*------------------------------------------------------------------------*/
    /*  Get the buffers in TILER space                                        */
    /*  Do the allocation for maximum supported resoultion and keep to some   */
    /* aligned boundary. Also this allocation assumes that tiled8 and tiled16 */
    /* are mapped to same physical space                                      */
    /*------------------------------------------------------------------------*/
    sTilerParams *pTilerParams     = &TilerParams;
    pTilerParams->tilerSpace[0]    = gConfig.tilerSpace[0];
    pTilerParams->tilerSpace[1]    = gConfig.tilerSpace[1];
    pTilerParams->memoryOffset[0]  = 0;
    pTilerParams->memoryOffset[1]  = 0x4800000;
    pTilerParams->totalSizeLuma    = 0x4800000;
    pTilerParams->imageDim[0]      = 2048;
    pTilerParams->imageDim[1]      = 1536;
    ret = tiler_init(pTilerParams);
    if(ret == -1)
    {
      /*------------------------------------------------------------------------*/
      /* If tiler allocation is not successful then Force both the buffer to be */
      /* in raw region                                                          */
      /*------------------------------------------------------------------------*/
      gConfig.tilerSpace[0]=  gConfig.tilerSpace[1] = 0;
    }
    else
    {
      /*----------------------------------------------------------------------*/
      /* Library also overrides few requests so take to the application       */
      /* variables to keep the flow correct                                   */
      /*----------------------------------------------------------------------*/
      gConfig.tilerSpace[0]  = pTilerParams->tilerSpace[0];
      gConfig.tilerSpace[1]  = pTilerParams->tilerSpace[1];
    }
  }

  /*--------------------------------------------------------------------------*/
  /* Counter for setting MCTNF                                                */
  /*--------------------------------------------------------------------------*/
  FrameNum = 0;

  /*--------------------------------------------------------------------------*/
  /*  Call to RMAN Assign Resources (to verify the ires-rman functions.       */
  /*--------------------------------------------------------------------------*/
  RMAN_AssignResources((IALG_Handle)handle, gConfig.tilerSpace[0],
  gConfig.tilerSpace[1] );

  /*--------------------------------------------------------------------------*/
  /* Get the version of MCTNF and print it                                    */
  /*--------------------------------------------------------------------------*/
  status.vidnf1Status.data.buf = (XDAS_Int8*)&version[0];
  status.vidnf1Status.data.bufSize = FILE_NAME_SIZE;
  retValue = MCTNF_control
             (
                 handle,
                 XDM_GETVERSION,
                 (MCTNF_DynamicParams *)&gDynamicParams,
                 (MCTNF_Status *)&status
             );
  printf("\nAlgorithm Instance  Creation for the Module %s Done...\n", version);
  status.vidnf1Status.data.bufSize = 0;

  /*--------------------------------------------------------------------------*/
  /* Check for the create time errors, and print them in log file             */
  /* Controll commands will enabled only after RMAN_AssignResources           */
  /* First we will print MCTNF version when before printig any error          */
  /*--------------------------------------------------------------------------*/
  if(MCTNF_TI_Report_Error(fTrace_file,status.vidnf1Status.extendedError))
  {
    printf("Creation Failed.\n");
    fprintf(fTrace_file, "Creation Failed.\n");
    goto DELETE_INSTANCE;
  }

  /*--------------------------------------------------------------------------*/
  /*  XDM_SETPARAMS is called to pass the Dynamic Parameters to the algorithm */
  /*  The Control function shall validate the parameters. If an error         */
  /*  is found then the Control function returns failure.                     */
  /*--------------------------------------------------------------------------*/
  retValue = MCTNF_control
             (
                 handle,
                 XDM_SETPARAMS,
                 (MCTNF_DynamicParams *)&gDynamicParams,
                 (MCTNF_Status *)&status
             );
  if(retValue == XDM_EFAIL)
  {
    MCTNF_TI_Report_Error(fTrace_file,status.vidnf1Status.extendedError);
    printf("Failed in dynamicParams setiing.\n");
    printf("Check the dynamicParams settings.\n");

    fprintf(fTrace_file, "Failed in dynamicParams setiing.\n");
    fprintf(fTrace_file, "Check the dynamicParams settings.\n");

    /* Delete only if it is a FATAL Error else continue */
    if(XDM_ISFATALERROR(status.vidnf1Status.extendedError))
    {
      goto DELETE_INSTANCE;
    }
  }

  /*--------------------------------------------------------------------------*/
  /*  XDM_SETLATEACQUIREARG is called to set the channel ID to the algorithm  */
  /*--------------------------------------------------------------------------*/
  gDynamicParams.vidnf1DynamicParams.lateAcquireArg = CHANNEL_ID ;
  retValue = MCTNF_control
             (
                 handle,
                 XDM_SETLATEACQUIREARG,
                 (MCTNF_DynamicParams *)&gDynamicParams,
                 (MCTNF_Status *)&status
             );
  if(retValue == XDM_EFAIL)
  {
    MCTNF_TI_Report_Error(fTrace_file,status.vidnf1Status.extendedError);
    printf("Failed in dynamicParams setiing.\n");
    printf("Check the dynamicParams settings.\n");

    fprintf(fTrace_file, "Failed in dynamicParams setiing.\n");
    fprintf(fTrace_file, "Check the dynamicParams settings.\n");

    /* Delete only if it is a FATAL Error else continue */
    if(XDM_ISFATALERROR(status.vidnf1Status.extendedError))
    {
      goto DELETE_INSTANCE;
    }
  }

  /*--------------------------------------------------------------------------*/
  /* Get buffer with XDM_GETBUFINFO command and allocate buffers accordingly  */
  /* with Buffer manager                                                      */
  /*--------------------------------------------------------------------------*/
  MCTNF_control
  (
      handle,
      XDM_GETBUFINFO,
      (MCTNF_DynamicParams *)&gDynamicParams,
      (MCTNF_Status *)&status
  );
  uiCaptureWidth  = (gDynamicParams.vidnf1DynamicParams.captureWidth + 15 ) & ~15;

  /*------------------------------------------------------------------------*/
  /* uiCaptureWidth ==0  means same as pitch == width                       */
  /* in test application code uiCaptureWidth variable should be used and to */
  /* MCTNF interface level gDynamicParams.vidnf1DynamicParams.captureWidth  */
  /*------------------------------------------------------------------------*/
  if(uiCaptureWidth == 0)
  {
    uiCaptureWidth = (gDynamicParams.vidnf1DynamicParams.inputWidth + 15) & ~15 ;
  }

  uiCaptureHeight = (gConfig.captureHeight+ 15 ) & ~15;
  if(gParams.vidnf1Params.inputContentType == IVIDEO_INTERLACED)
  {
    uiCaptureHeight <<= 1;
  }

  /*--------------------------------------------------------------------------*/
  /* Loop through the number of buffers and calulate the size required        */
  /* TODO: Need to get some clarification why control will not give size      */
  /*--------------------------------------------------------------------------*/
  for(i = 0; i < status.vidnf1Status.bufInfo.minNumInBufs; i++)
  {

    if(!gConfig.enableRunTimeTest)
    {
      if(status.vidnf1Status.bufInfo.inBufMemoryType[i] != XDM_MEMTYPE_ROW)
      {
        uiSize1 = status.vidnf1Status.bufInfo.minInBufSize[i].tileMem.width *
                  status.vidnf1Status.bufInfo.minInBufSize[i].tileMem.height;
      }
      else
      {
        uiSize1 = status.vidnf1Status.bufInfo.minInBufSize[i].bytes;
      }
    }
    else
    {
      /*------------------------------------------------------------------------*/
      /* Here for input buffer size calculation maxWidth & maxHeight are used   */
      /* This is done for testing dynamic resolution chage                      */
      /*------------------------------------------------------------------------*/
      if(i == 0)
      {
        uiSize1 = gParams.vidnf1Params.maxWidth * gParams.vidnf1Params.maxHeight;
      }
      else
      {
        uiSize1 = gParams.vidnf1Params.maxWidth * (gParams.vidnf1Params.maxHeight >> 1);
      }
    }

    if((i & 1) == 0)
    {
      uiSize = uiCaptureWidth * uiCaptureHeight;
    }
    else
    {
      uiSize = uiCaptureWidth * (uiCaptureHeight >> 1);
    }

    uiSize = uiSize > (uiSize1) ? uiSize : uiSize1;
    inBufSizes[i].bytes = uiSize;
  }
  numInBufSet = i >> 1;

  for(i = 0; i < status.vidnf1Status.bufInfo.minNumOutBufs; i++)
  {

    if(!gConfig.enableRunTimeTest)
    {
      if(status.vidnf1Status.bufInfo.outBufMemoryType[i] != XDM_MEMTYPE_ROW)
      {
        uiSize1 = status.vidnf1Status.bufInfo.minOutBufSize[i].tileMem.width *
                  status.vidnf1Status.bufInfo.minOutBufSize[i].tileMem.height;
      }
      else
      {
        uiSize1 = status.vidnf1Status.bufInfo.minOutBufSize[i].bytes;
      }
    }
    else
    {
      /*------------------------------------------------------------------------*/
      /* Here for input buffer size calculation maxWidth & maxHeight are used   */
      /* This is done for testing dynamic resolution chage                      */
      /*------------------------------------------------------------------------*/
      if(i == 0)
      {
        uiSize1 = gParams.vidnf1Params.maxWidth * gParams.vidnf1Params.maxHeight;
      }
      else
      {
        uiSize1 = gParams.vidnf1Params.maxWidth * (gParams.vidnf1Params.maxHeight >> 1);
      }
    }

    if((i & 1) == 0)
    {
      uiSize = uiCaptureWidth * uiCaptureHeight;
    }
    else
    {
      uiSize = uiCaptureWidth * (uiCaptureHeight >> 1);
    }

    uiSize = uiSize > (uiSize1) ? uiSize : uiSize1;
    outBufSizes[i].bytes = uiSize;
  }
  numOutBufSet = i >> 1;

  retValue = BUFFMGR_Init
             (
                numInBufSet,
                inBufSizes,
                numOutBufSet,
                outBufSizes,
                gConfig.tilerSpace[0],
                gConfig.tilerSpace[1]
             );

  if (retValue)
  {
    printf ("\nMemory could not get allocated for output buffers\n");
    fprintf (fTrace_file, "\nMemory could not get allocated for output buffers\n");
    goto DELETE_INSTANCE;
  }
  numFramesFiltered = 0;
  frameNum = gConfig.startFrame;


  /*------------------------------------------------------------------------*/
  /*  Check for debug trace parameters             .                        */
  /*------------------------------------------------------------------------*/
  retValue = MCTNF_control
             (
                 handle,
                 XDM_GETSTATUS,
                 (MCTNF_DynamicParams *)&gDynamicParams,
                 (MCTNF_Status *)&status
             );

  printf("Debug Trace (@ Level %d) Buffer in DDR @ 0x%X of size %d bytes\n",
            status.debugTraceLevel,
            status.extMemoryDebugTraceAddr,
            status.extMemoryDebugTraceSize);

  /*------------------------------------------------------------------------*/
  /* Check for Status base class working                                    */
  /* when working in base class, the pattern filled at extended part        */
  /* should be same after XDM_GETSTATUS call                                */
  /*------------------------------------------------------------------------*/
#ifdef BASE_CLASS
  {
    XDAS_Int8 *src = (XDAS_Int8*)&status + sizeof(IVIDNF1_Status);
    XDAS_Int32 size = sizeof(MCTNF_Status) - sizeof(IVIDNF1_Status);
    for(i = 0; i < size; i++)
    {
      if(*src != (XDAS_Int8)PATTERN_FILL_EXT)
      {
        printf("\n Failed in testing Status Base class mode\n");
        goto DELETE_INSTANCE;
      }
      src++;
    }
  }
#endif
  /*------------------------------------------------------------------------*/
  /*  Dynamic parameter change testing                                      */
  /*------------------------------------------------------------------------*/
  if(gConfig.enableRunTimeTest)
  {
    XDAS_Int8 tempDynamicFramePos[40];

    stopPos = 1;
    gConfig.DynamicFramePos = 0;
    strcpy((char *)dynamicparamChgFile,
           "..\\..\\..\\Test\\TestVecs\\Config\\dynamicparamchg.txt");

    /*------------------------------------------------------------------------*/
    /*  Open dynamic parameter change Config File                             */
    /*------------------------------------------------------------------------*/
    fDynamicParamChgFile = fopen((const char *)dynamicparamChgFile,"rb");

    if(fDynamicParamChgFile == NULL)
    {
      printf("\n Unable to open dynamic parameter change config File %s\n",
                         dynamicparamChgFile);
      return(XDM_EFAIL);
    }
    /*------------------------------------------------------------------------*/
    /* Read first dynamic parameter change frame number                       */
    /* In the dynamic param change config file, frame number at which the     */
    /* dynamic parameters to be changed is the first parameter and is of      */
    /* format DynamicFramePos = <frameno>. Here first 2 fscanf will read      */
    /* "DynamicFramePos" & "=" and the next fscanf will read the frame number */
    /*------------------------------------------------------------------------*/
    fscanf(fDynamicParamChgFile, "%s", tempDynamicFramePos);
    fscanf(fDynamicParamChgFile, "%s", tempDynamicFramePos);
    fscanf(fDynamicParamChgFile, "%d", &gConfig.DynamicFramePos);
    fclose(fDynamicParamChgFile);
  }
  gDataSyncUnitGap = DATA_SYNCH_UNIT_GAP;
  /*--------------------------------------------------------------------------*/
  /*  Do-While Loop for MCTNF Process Call                                    */
  /*--------------------------------------------------------------------------*/
  do
  {

    if(gParams.vidnf1Params.outputDataMode == IVIDEO_SLICEMODE)
    {
      if((testCaseCounter & 0x1))
      {
       gInitialDataUnit = OUTPUT_BUFFER_SIZE;
       gDataSynchUnit   = DATA_SYNCH_UNIT;
      }
      else
      {
       gInitialDataUnit = ((numFramesFiltered + 0x2) << 2);
       gDataSynchUnit   = ((((numFramesFiltered & 0x3) + 1))* DATA_SYNCH_UNIT);
      }
    }
    else if(gParams.vidnf1Params.outputDataMode == IVIDEO_FIXEDLENGTH)
    {
       gInitialDataUnit = ((numFramesFiltered + 0x2) << 2);
       gDataSynchUnit   = ((((numFramesFiltered & 0x3) + 1))* DATA_SYNCH_UNIT);
    }
    else
    {
       gInitialDataUnit = OUTPUT_BUFFER_SIZE;
       gDataSynchUnit   = ((((numFramesFiltered & 0x3) + 1))* DATA_SYNCH_UNIT);
    }

    /*------------------------------------------------------------------------*/
    /*  Call the getfreebuffer() function of buffermanager to get the         */
    /*  buffer element to be supplied to algorithm for the next process       */
    /*  call.                                                                 */
    /*------------------------------------------------------------------------*/
      inBufEle = BUFFMGR_GetFreeBuffer(BUFFMGR_IN_BUF);
      if (inBufEle == NULL)
      {
        fprintf (stdout,"\nNo free Input buffer available\n");
        break;
      }

      outBufEle = BUFFMGR_GetFreeBuffer(BUFFMGR_OUT_BUF);
      if (outBufEle == NULL)
      {
        fprintf (stdout,"\nNo free Output buffer available\n");
        break;
      }

    /*----------------------------------------------------------------------*/
    /* Dynamic parameter change test                                        */
    /*----------------------------------------------------------------------*/
    if(gConfig.enableRunTimeTest)
    {
      if(numFramesFiltered == gConfig.DynamicFramePos)
      {

        uiInputWidth = gDynamicParams.vidnf1DynamicParams.inputWidth;
        uiInputHeight = gDynamicParams.vidnf1DynamicParams.inputHeight;
        /*------------------------------------------------------------------*/
        /* Read dynamic parameter change config file                        */
        /*------------------------------------------------------------------*/
        if(readparamfile(dynamicparamChgFile) != 0)
        {
          printf("Unable to read runtime parameter change file %s\n",
                          dynamicparamChgFile);
          return(XDM_EFAIL);
        }
        printf("\nRequested dynamic parameter change at Frame #%-5d .... ",
                  (numFramesFiltered));
        fprintf(fTrace_file,
               "\nRequested dynamic parameter change at Frame #%-5d .... ",
                    (numFramesFiltered));
        MCTNF_TI_InitDynamicParams(&gParams,&gDynamicParams);
        /*--------------------------------------------------------------------*/
        /* XDM_SETPARAMS is called to pass the Dynamic Parameters to the      */
        /* algorithm. The Control function shall validate the parameters.     */
        /* If an error is found then the Control function returns failure.    */
        /*--------------------------------------------------------------------*/
        retValue = MCTNF_control
                   (
                       handle,
                       XDM_SETPARAMS,
                       (MCTNF_DynamicParams *)&gDynamicParams,
                       (MCTNF_Status *)&status
                   );

        if(retValue == XDM_EFAIL)
        {
          MCTNF_TI_Report_Error(fTrace_file,status.vidnf1Status.extendedError);
          printf("Failed in dynamicParams setting.\n");
          printf("Check the dynamicParams settings.\n");

          fprintf(fTrace_file, "Failed in dynamicParams setting.\n");
          fprintf(fTrace_file, "Check the dynamicParams settings.\n");

          /* Delete only if it is a FATAL Error else continue */
          if(XDM_ISFATALERROR(status.vidnf1Status.extendedError))
          {
            goto DELETE_INSTANCE;
          }
        }
        /*-------------------------------------------------------------------*/
        /* Read new yuv if there is a change in resolution                   */
        /*-------------------------------------------------------------------*/
        if((uiInputWidth != gDynamicParams.vidnf1DynamicParams.inputWidth) ||
           (uiInputHeight != gDynamicParams.vidnf1DynamicParams.inputHeight))
        {
          /*-----------------------------------------------------------------*/
          /* Open Input YUV Sequence file.                                   */
          /*-----------------------------------------------------------------*/
          my_fclose(finFile);
          strcpy((char *)inFile, (const char *)gConfig.inputFile);
          finFile = my_fopen((const char *)inFile, "rb");
          if(finFile == NULL)
          {
            printf("\n Unable to open Input YUV Sequence File %s\n", inFile);
            return(XDM_EFAIL);
          }
          frameNum = 0;
        }

        uiCaptureWidth  =
        (gDynamicParams.vidnf1DynamicParams.captureWidth + 15 ) & ~15;

        /*--------------------------------------------------------------------*/
        /* uiCaptureWidth ==0  means same as pitch == width                   */
        /* in test application code uiCaptureWidth variable should be used    */
        /* and to MCTNF interface level                                       */
        /* gDynamicParams.vidnf1DynamicParams.captureWidth                    */
        /*--------------------------------------------------------------------*/
        if(uiCaptureWidth == 0)
        {
          uiCaptureWidth =
              (gDynamicParams.vidnf1DynamicParams.inputWidth + 15) & ~15 ;
        }

        printf("\nRequest Dynamic parameter change sucessful  .... \n");
        fprintf(fTrace_file, "\nRequest Dynamic parameter change sucessful  .... \n");
      }
    }

    MCTNF_TI_initInputBufDesc
    (
        inBufEle,
        &status,
        uiCaptureWidth
    );

    MCTNF_TI_initOutputBufDesc
    (
        outBufEle,
        &status
    );

    /*------------------------------------------------------------------------*/
    /*  Initialize the input ID in input arguments to the bufferid of the     */
    /*  buffer element returned from getfreebuffer() function.                */
    /*------------------------------------------------------------------------*/
    inArgs.vidnf1InArgs.inBufID   = inBufEle->bufId;
    inArgs.vidnf1InArgs.outBufID  = outBufEle->bufId;

    /*------------------------------------------------------------------------*/
    /* Come out of the loop when given number of frames are filterd           */
    /*------------------------------------------------------------------------*/
    if(numFramesFiltered >= gConfig.numInputDataUnits)
    {
      fwrite(&profileData, 1, sizeof(sProfileData), fProfileTrace);
      break;
    }

    /*------------------------------------------------------------------------*/
    /*  Read Input YUV Sequence data.                                         */
    /*------------------------------------------------------------------------*/

    TestApp_ReadInputYUVData
    (
        &gInputBufDesc,
        frameNum,
        finFile,
        gConfig.tilerSpace[0],
        gConfig.tilerSpace[1]
    );

    /*------------------------------------------------------------------------*/
    /*  Start the MCTNF process for one frame/field by calling the            */
    /*  MCTNF_processFrame() function.                                        */
    /*------------------------------------------------------------------------*/
    RMAN_activateAllResources ((IALG_Handle)handle);
    /*--------------------------------------------------------------------------*/
    /* Init the log generation logics                                           */
    /*--------------------------------------------------------------------------*/
    MCTNF_TI_Init_Log(fTrace_file);
    /*------------------------------------------------------------------------*/
    /* Capture process start time when profile is enabled                     */
    /*------------------------------------------------------------------------*/
    fflush(stdout);
    printf("Filtering Frame %d...\n", numFramesFiltered);

    MCTNF_TI_Capture_time(PROCESS_START);

    retValue = MCTNF_processFrame
               (
                   handle,
                   &gInputBufDesc,
                   &gOutputBufDesc,
                   &inArgs,
                   &outArgs
               );
    /*------------------------------------------------------------------------*/
    /* Capture process end time when profile is enabled                       */
    /*------------------------------------------------------------------------*/
    MCTNF_TI_Capture_time(PROCESS_END);

    MCTNF_TI_Print_Log(fTrace_file);

    RMAN_deactivateAllResources((IALG_Handle)handle);

    /*------------------------------------------------------------------------*/
    /*  Check for failure message in the return value.                        */
    /*------------------------------------------------------------------------*/
    if(retValue != IVIDNF1_EOK)
    {
      printf("Frame filtering failed.\n");
      fprintf(fTrace_file,"Frame filtering failed.\n");
    }
#ifdef MB_LEVEL_PROFILE
    if(frameNum == 2)
    {
      short* dtcm1_profileData = (short*)(int)0x5A002c70;
      short* dtcm2_profileData = (short*)(int)0x5A010fa0;
        int MbNum = 0;
        int mbLevelCycle =0;
        for(MbNum = 0;MbNum<111;MbNum++)
        {
          printf("\nMb No :: %d, ModeDecision :: %d, WeightCalc :: %d, ime3 :: %d, mc3 :: %d, ipe3 :: %d",\
          MbNum,dtcm1_profileData[MbNum], dtcm2_profileData[MbNum], (dtcm1_profileData[112+MbNum] - dtcm1_profileData[112+MbNum -1]),\
          (dtcm1_profileData[2*112+MbNum] - dtcm1_profileData[2*112+MbNum -1]),\
          (dtcm2_profileData[112+MbNum] - dtcm2_profileData[112+MbNum -1]));
        }
    }
#endif
    /*------------------------------------------------------------------------*/
    /*  Always release buffers - which are released from the algorithm        */
    /*  side   -back to the buffer manager. The freebufID array of outargs    */
    /*  contains the sequence of bufferIds which need to be freed. This       */
    /*  gets populated by the algorithm. The following function will do       */
    /*  the job of freeing up the buffers.                                    */
    /*------------------------------------------------------------------------*/
    BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.vidnf1OutArgs.freeInBufID, BUFFMGR_IN_BUF);
    BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.vidnf1OutArgs.freeOutBufID, BUFFMGR_OUT_BUF);

    /*------------------------------------------------------------------------*/
    /*  Check for extended errors while encoding     .                        */
    /*------------------------------------------------------------------------*/
    retValue = MCTNF_control
               (
                   handle,
                   XDM_GETSTATUS,
                   (MCTNF_DynamicParams *)&gDynamicParams,
                   (MCTNF_Status *)&status
               );
    /*------------------------------------------------------------------------*/
    /* For each and every 10 frames change the handle and Get Input data API  */
    /* to check codec functionality                                           */
    /*------------------------------------------------------------------------*/
    if(!(numFramesFiltered % 10))
    {
      if(gDynamicParams.vidnf1DynamicParams.getDataHandle == fGetInpDHandle1)
      {
        gDynamicParams.vidnf1DynamicParams.getDataHandle = fGetInpDHandle2;
        gDynamicParams.vidnf1DynamicParams.getDataFxn    = MCTNFTest_DataSyncGetDataFxn2;
      }
      else
      {
        gDynamicParams.vidnf1DynamicParams.getDataHandle = fGetInpDHandle1;
        gDynamicParams.vidnf1DynamicParams.getDataFxn    = MCTNFTest_DataSyncGetDataFxn1;
      }

      MCTNF_control
      (
         handle,
         XDM_SETPARAMS,
        (MCTNF_DynamicParams *)&gDynamicParams,
        (MCTNF_Status *)&status
      );
    }

    if(MCTNF_TI_Report_Error(fTrace_file,outArgs.vidnf1OutArgs.extendedError))
    {
      printf("MCTNF Failed.\n");
      fprintf(fTrace_file, "MCTNF Failed.\n");
      goto DELETE_INSTANCE;
    }
    /*------------------------------------------------------------------------*/
    /*  Write the Output Bitstream to a file.                                 */
    /*------------------------------------------------------------------------*/
    if(outArgs.vidnf1OutArgs.displayID[0] != 0)
    {
        TestApp_WriteOutputData(foutFile);
    }

    /*------------------------------------------------------------------------*/
    /*  Increment the frames filtered count.                                  */
    /*------------------------------------------------------------------------*/
    numFramesFiltered++;
    frameNum ++;

  } while(1);

  /*--------------------------------------------------------------------------*/
  /*  Flush the algorithm                                                     */
  /*  Call XDM flush to inform algorithm that we are going to flush all input */
  /*  frames given                                                            */
  /*--------------------------------------------------------------------------*/
  MCTNF_control
  (
      handle,
      XDM_FLUSH,
      (MCTNF_DynamicParams *)&gDynamicParams,
      (MCTNF_Status *)&status
  );

  do
  {
    gOutputBufDesc.numPlanes        = 0;
    gInputBufDesc.numPlanes         = 0;
    gOutputBufDesc.planeDesc[0].buf = 0;
    gInputBufDesc.planeDesc[0].buf  = 0;
    inArgs.vidnf1InArgs.inBufID    = 0;
    inArgs.vidnf1InArgs.outBufID   = 0;

    RMAN_activateAllResources ((IALG_Handle)handle);
    /*------------------------------------------------------------------------*/
    /* Reseting of blocks recieved in previous process call. This variable is */
    /* used only when oututDataMode is not FULL_FRAME_MODE                    */
    /*------------------------------------------------------------------------*/
    gOutputBlocksRecieved = 0                                                 ;
    retValue =  MCTNF_processFrame
                (
                    handle,
                    &gInputBufDesc,
                    &gOutputBufDesc,
                    &inArgs,
                    &outArgs
                );

    RMAN_deactivateAllResources((IALG_Handle)handle);

    if (XDM_ISINSUFFICIENTDATA (outArgs.vidnf1OutArgs.extendedError))
    {
      break;
    }

    /*------------------------------------------------------------------------*/
    /*  Always release buffers - which are released from the algorithm        */
    /*  side   -back to the buffer manager. The freebufID array of outargs    */
    /*  contains the sequence of bufferIds which need to be freed. This       */
    /*  gets populated by the algorithm. The following function will do       */
    /*  the job of freeing up the buffers.                                    */
    /*------------------------------------------------------------------------*/
    BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.vidnf1OutArgs.freeOutBufID, BUFFMGR_OUT_BUF);
    /*------------------------------------------------------------------------*/
    /* Write the out put into buffer                                          */
    /*------------------------------------------------------------------------*/
    if(outArgs.vidnf1OutArgs.displayID[0] != 0)
    {
        //printf(" D_Out: %d", outArgs.vidnf1OutArgs.displayID[0]);
        TestApp_WriteOutputData(foutFile);
    }

  }while(1);

  /*--------------------------------------------------------------------------*/
  /*  DO XDM call of Reset                                                    */
  /*--------------------------------------------------------------------------*/
  MCTNF_control
  (
      handle,
      XDM_RESET,
      (MCTNF_DynamicParams *)&gDynamicParams,
      (MCTNF_Status *)&status
  );

DELETE_INSTANCE:
    if(isAcquireCallMade != 0)
    {
      fprintf(stdout,"HDVICP is not in released state while exiting..\n");
    }
    my_fclose(foutFile);
    my_fclose(finFile);
    fclose(fTrace_file);
    fclose(fProfileTrace);
    if(status.debugTraceLevel)
    {
      strcpy((char *)scTraceFile, (const char *)gConfig.outputFile);
      strcat((char *)scTraceFile, "debugTrace.bin");
      fTrace_file = fopen((const char *)scTraceFile, "wb");
      if(NULL == fTrace_file)
      {
        fprintf(stdout,"Could not open the debug trace file \n");
      }
      else
      {
        fwrite(status.extMemoryDebugTraceAddr,1,
        status.extMemoryDebugTraceSize,fTrace_file);
        fclose(fTrace_file);
      }
    }
     /*--------------------------------------------------------------------------*/
     /*  The algorithm currently does not guarantee to set freebufid of          */
     /*  all buffers in use.                                                     */
     /*  This is managed for now by an application function. Note that           */
     /*  doing this has significance only when running multiple streams.         */
     /*--------------------------------------------------------------------------*/
     BUFFMGR_ReleaseAllBuffers();

     /*--------------------------------------------------------------------------*/
     /*  Free the buffers allocated for through IRES-RMAN.                       */
     /*--------------------------------------------------------------------------*/
     RMAN_FreeResources((IALG_Handle)handle);

     /*--------------------------------------------------------------------------*/
     /*  Delete the MCTNF Instance.                                              */
     /*--------------------------------------------------------------------------*/
     MCTNF_delete(handle);
  }
  fclose(fTestCasesFile);
  /*--------------------------------------------------------------------------*/
  /* use abort function to go to loader_exit() without doing any thing        */
  /* This helps for PCI driver to reset and run again from the start          */
  /*--------------------------------------------------------------------------*/
  abort();
}

void MCTNF_TI_initInputBufDesc
(
    BUFFMGR_buffEleHandle inBufEle,
    MCTNF_Status       *status,
    XDAS_Int32            uiCaptureWidth
)
{
    XDAS_Int32 i;
    /*------------------------------------------------------------------------*/
    /*  Initialize the input buffer properties as required by algorithm       */
    /*  based on info received by preceding GETBUFINFO call.                  */
    /*------------------------------------------------------------------------*/
    gInputBufDesc.numPlanes = 2;
    gInputBufDesc.numMetaPlanes = 0;

    /*------------------------------------------------------------------------*/
    /* Set entire Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    gInputBufDesc.imageRegion.topLeft.x = 0;
    gInputBufDesc.imageRegion.topLeft.y = 0;
    gInputBufDesc.imageRegion.bottomRight.x = uiCaptureWidth;
    gInputBufDesc.imageRegion.bottomRight.y = gConfig.captureHeight;

    /*------------------------------------------------------------------------*/
    /* Set proper Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    gInputBufDesc.activeFrameRegion.topLeft.x = gConfig.captureTopLeftx;
    gInputBufDesc.activeFrameRegion.topLeft.y = gConfig.captureTopLefty;
    gInputBufDesc.activeFrameRegion.bottomRight.x = gConfig.captureTopLeftx + \
                                gDynamicParams.vidnf1DynamicParams.inputWidth;
    gInputBufDesc.activeFrameRegion.bottomRight.y = gConfig.captureTopLefty + \
                               gDynamicParams.vidnf1DynamicParams.inputHeight;
    /*------------------------------------------------------------------------*/
    /* Image pitch is capture width                                           */
    /*------------------------------------------------------------------------*/
    gInputBufDesc.imagePitch[0] = gDynamicParams.vidnf1DynamicParams.captureWidth;
    gInputBufDesc.imagePitch[1] = gDynamicParams.vidnf1DynamicParams.captureWidth;

    /*------------------------------------------------------------------------*/
    /* Set Content type and chroma format from MCTNF parameters               */
    /*------------------------------------------------------------------------*/
    gInputBufDesc.contentType = gParams.vidnf1Params.inputContentType;
    gInputBufDesc.chromaFormat = gParams.vidnf1Params.inputChromaFormat;

    /*------------------------------------------------------------------------*/
    /* Assign memory pointers adn sizes for the all the input buffers         */
    /*------------------------------------------------------------------------*/
    for(i = 0; i < status->vidnf1Status.bufInfo.minNumInBufs; i++)
    {
      gInputBufDesc.planeDesc[i].bufSize.bytes  = (XDAS_Int32)inBufEle->bufSize[i];
      /*----------------------------------------------------------------------*/
      /* Make the input buffer start address to 1 byte aligned , This is      */
      /* to make sure support of unaligned input buffer base address working  */
      /* This is just for test purpose. in system until unless required, it   */
      /* should be aligned to 16 byte boundary..                              */
      /* Here the same line of code for TILED16 & else part is kept intensely */
      /* For testing unaligned input buffer base address support, the         */
      /* allignment should be minimum 2 byte in case of TILED16 & 1 byte for  */
      /* other cases                                                          */
      /*----------------------------------------------------------------------*/
      if(gConfig.tilerSpace[i] == XDM_MEMTYPE_TILED16)
      {
        gInputBufDesc.planeDesc[i].buf = (XDAS_Int8 *)((((XDAS_UInt32)inBufEle->buf[i] + 15) & (~15)) + 0);
      }
      else
      {
        gInputBufDesc.planeDesc[i].buf = (XDAS_Int8 *)((((XDAS_UInt32)inBufEle->buf[i] + 15) & (~15)) + 0);
      }
      gInputBufDesc.planeDesc[i].memType =  status->vidnf1Status.bufInfo.inBufMemoryType[0];

      /*--------------------------------------------------------------------*/
      /* Overwrite the memory request from config file                      */
      /*--------------------------------------------------------------------*/
      gInputBufDesc.planeDesc[i].memType = gConfig.tilerSpace[i] ;
  }

  /*------------------------------------------------------------------------*/
  /* Set second field offset width and height according to input data       */
  /* When second field of the input data starts at 0 it represents 2 fields */
  /* are seperated and provided at 2 diff process calls (60 process call)   */
  /*------------------------------------------------------------------------*/
  if(((gInputBufDesc.dataLayout == IVIDEO_FIELD_SEPARATED) &&
      (gConfig.numProcessCall == 60)) &&
      (gParams.vidnf1Params.inputContentType == IVIDEO_INTERLACED))
  {
    gInputBufDesc.secondFieldOffsetHeight[0] = 0;
    gInputBufDesc.secondFieldOffsetHeight[1] = 0;
    gInputBufDesc.secondFieldOffsetHeight[2] = 0;
  }
  else
  {
    gInputBufDesc.secondFieldOffsetHeight[0] = gDynamicParams.vidnf1DynamicParams.inputHeight;
    gInputBufDesc.secondFieldOffsetHeight[1] = (gDynamicParams.vidnf1DynamicParams.inputHeight >> 1);
    gInputBufDesc.secondFieldOffsetHeight[2] = (gDynamicParams.vidnf1DynamicParams.inputHeight >> 1);
  }
  gInputBufDesc.secondFieldOffsetWidth[0]  = 0;
  gInputBufDesc.secondFieldOffsetWidth[1]  = 0;
  gInputBufDesc.secondFieldOffsetWidth[2]  = 0;

}

void MCTNF_TI_initOutputBufDesc
(
    BUFFMGR_buffEleHandle outBufEle,
    MCTNF_Status *status
)
{
    XDAS_Int32 i;
    /*------------------------------------------------------------------------*/
    /*  Initialize the output buffer properties as required by algorithm      */
    /*  based on info received by preceding GETBUFINFO call.                  */
    /*------------------------------------------------------------------------*/
    gOutputBufDesc.numPlanes = 2;
    gOutputBufDesc.numMetaPlanes = 0;

    /*------------------------------------------------------------------------*/
    /* Set entire Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    gOutputBufDesc.imageRegion.topLeft.x = 0;
    gOutputBufDesc.imageRegion.topLeft.y = 0;
    gOutputBufDesc.imageRegion.bottomRight.x = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;
    gOutputBufDesc.imageRegion.bottomRight.y = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.height;

    /*------------------------------------------------------------------------*/
    /* Set proper Image region in the buffer  by using config parameters      */
    /*------------------------------------------------------------------------*/
    gOutputBufDesc.activeFrameRegion.topLeft.x = 0;
    gOutputBufDesc.activeFrameRegion.topLeft.y = 0;
    gOutputBufDesc.activeFrameRegion.bottomRight.x = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;
    gOutputBufDesc.activeFrameRegion.bottomRight.y = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.height;

    /*------------------------------------------------------------------------*/
    /* Image pitch is capture width                                           */
    /*------------------------------------------------------------------------*/
    gOutputBufDesc.imagePitch[0] = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;
    gOutputBufDesc.imagePitch[1] = status->vidnf1Status.bufInfo.minOutBufSize[0].tileMem.width;

    /*------------------------------------------------------------------------*/
    /* Set Content type and chroma format from MCTNF parameters               */
    /*------------------------------------------------------------------------*/
    gOutputBufDesc.contentType = gParams.vidnf1Params.inputContentType;
    gOutputBufDesc.chromaFormat = gParams.vidnf1Params.inputChromaFormat;

    /*------------------------------------------------------------------------*/
    /* Assign memory pointers adn sizes for the all the input buffers         */
    /*------------------------------------------------------------------------*/
    for(i = 0; i < status->vidnf1Status.bufInfo.minNumOutBufs; i++)
    {
      gOutputBufDesc.planeDesc[i].bufSize.bytes  = (XDAS_Int32)outBufEle->bufSize[i];
      /*----------------------------------------------------------------------*/
      /* Make the input buffer start address to 1 byte aligned , This is      */
      /* to make sure support of unaligned input buffer base address working  */
      /* This is just for test purpose. in system until unless required, it   */
      /* should be aligned to 16 byte boundary..                              */
      /* Here the same line of code for TILED16 & else part is kept intensely */
      /* For testing unaligned input buffer base address support, the         */
      /* allignment should be minimum 2 byte in case of TILED16 & 1 byte for  */
      /* other cases                                                          */
      /*----------------------------------------------------------------------*/
      if(gConfig.tilerSpace[i] == XDM_MEMTYPE_TILED16)
      {
        gOutputBufDesc.planeDesc[i].buf = (XDAS_Int8 *)((((XDAS_UInt32)outBufEle->buf[i] + 15) & (~15)) + 0);
      }
      else
      {
        gOutputBufDesc.planeDesc[i].buf = (XDAS_Int8 *)((((XDAS_UInt32)outBufEle->buf[i] + 15) & (~15)) + 0);
      }
      gOutputBufDesc.planeDesc[i].memType = status->vidnf1Status.bufInfo.inBufMemoryType[0];

      /*--------------------------------------------------------------------*/
      /* Overwrite the memory request from config file                      */
      /*--------------------------------------------------------------------*/
      gOutputBufDesc.planeDesc[i].memType = gConfig.tilerSpace[i] ;
    }

    /*------------------------------------------------------------------------*/
    /* Set second field offset width and height according to input data       */
    /* When second field of the input data starts at 0 it represents 2 fields */
    /* are seperated and provided at 2 diff process calls (60 process call)   */
    /*------------------------------------------------------------------------*/
    if(((gOutputBufDesc.dataLayout == IVIDEO_FIELD_SEPARATED) &&
        (gConfig.numProcessCall == 60)) &&
        (gParams.vidnf1Params.inputContentType == IVIDEO_INTERLACED))
    {
      gOutputBufDesc.secondFieldOffsetHeight[0] = 0;
      gOutputBufDesc.secondFieldOffsetHeight[1] = 0;
      gOutputBufDesc.secondFieldOffsetHeight[2] = 0;
    }
    else
    {
      gOutputBufDesc.secondFieldOffsetHeight[0] =  gDynamicParams.vidnf1DynamicParams.inputHeight;
      gOutputBufDesc.secondFieldOffsetHeight[1] = (gDynamicParams.vidnf1DynamicParams.inputHeight >> 1);
      gOutputBufDesc.secondFieldOffsetHeight[2] = (gDynamicParams.vidnf1DynamicParams.inputHeight >> 1);
    }
    gOutputBufDesc.secondFieldOffsetWidth[0]  = 0;
    gOutputBufDesc.secondFieldOffsetWidth[1]  = 0;
    gOutputBufDesc.secondFieldOffsetWidth[2]  = 0;
}

/**
********************************************************************************
 *  @fn     TestApp_ReadInputYUVData
 *  @brief  Reads the entire frame/field data which is needed for one process
 *          call
 *
 *  @param[in]  inputBuf : Pointer to input buffer discriptors
 *
 *  @param[in]  frameCount : Total number of frames filtered till now
 *                           Used to calculate offset in input file
 *
 *  @param[in]  fin : File pointer to the input file
 *
 *  @return     -1 in case of error
 *               0 in case when there is not error
********************************************************************************
*/

XDAS_Int16 TestApp_ReadInputYUVData
(
    IVIDEO2_BufDesc *inputBuf,
    XDAS_Int32       frameCount,
    FILE            *fin,
    XDAS_UInt8       lumaTilerSpace,
    XDAS_UInt8       chromaTilerSpace
)
{

  XDAS_Int32 pic_size;
  XDAS_UInt8 *dst,botFieldFirst;
  XDAS_Int8 fieldOffset = 0;
  XDAS_Int16 frmWidth,frmHeight;
  XDAS_UInt16 dataLayout;
  XDAS_UInt32 fileSize;
  XDAS_UInt32 NumframesInfile;
  XDAS_UInt32 numFields, fieldNo ;
  XDAS_UInt32 fieldOffsetLuma   ;
   XDAS_UInt32 fieldOffsetChroma ;

  XDAS_UInt8 *pLuma    ;
  XDAS_UInt8 *pChroma  ;


  frmWidth = inputBuf->imageRegion.bottomRight.x;
  frmHeight = inputBuf->imageRegion.bottomRight.y;
  dataLayout = inputBuf->dataLayout;
  botFieldFirst = !inputBuf->topFieldFirstFlag;
  botFieldFirst = (!inputBuf->topFieldFirstFlag) &&
                  (inputBuf->contentType == IVIDEO_INTERLACED);

  /*----------------------------------------------------------------------*/
  /* Calculating the Num frames in the YUV file and over writing on the   */
  /* gConfig.numInputDataUnits to avoid any file IO isses                 */
  /*----------------------------------------------------------------------*/
  if(frameCount == 0)
  {
    fileSize        = TestApp_FileLength(fin);
    NumframesInfile = (fileSize / ((3 * frmWidth * frmHeight) >> 1));
    if(inputBuf->contentType == IVIDEO_INTERLACED)
    {
      NumframesInfile >>= 1;
    }
    if(gConfig.numInputDataUnits > NumframesInfile)
    {
      gConfig.numInputDataUnits = NumframesInfile;
    }
    if(gConfig.numProcessCall == 60)
    {
      gConfig.numInputDataUnits *= 2;
    }
  }

  /*--------------------------------------------------------*/
  /* For tiled buffer first read into some temporary buffer */
  /*--------------------------------------------------------*/
  if(lumaTilerSpace)
  {
    pLuma   = RawLuma   ;
  }
  else
  {
    pLuma   = (XDAS_UInt8 *) inputBuf->planeDesc[0].buf ;
  }

  if(chromaTilerSpace)
  {
    pChroma = RawChroma ;
  }
  else
  {
    pChroma = (XDAS_UInt8 *) inputBuf->planeDesc[1].buf ;
  }

 /*---------------------------------------------------------------------------*/
 /*   In case of field interleaved format we need to consider twice the height*/
 /*   of the buffer as "frmHeight" represents height of the field             */
 /*---------------------------------------------------------------------------*/
   if((inputBuf->contentType == IVIDEO_INTERLACED) &&
    (dataLayout == IVIDEO_FIELD_INTERLEAVED))
  {
     frmHeight <<= 1;
   }
 /*---------------------------------------------------------------------------*/
 /*                         Size of the picture to be read                    */
 /*---------------------------------------------------------------------------*/
  pic_size = (3 * frmWidth * frmHeight) >> 1;
 /*---------------------------------------------------------------------------*/
 /* In the case of the bottom field first and field seperated YUV we need to  */
 /* capture first the bottom filed and than top field. In this situation we   */
 /* can provide the second field offset or not. If the second field offset is */
 /* provided than it will be 30 process call scenario, and both the field will*/
 /* captured in single process call. So inputBufdsc will point to the bottom  */
 /* field data and second field offset will help in getting second field (top */
 /* field) data.                                                              */
 /* If it is 60 process call scenario ( when second field offset is not set by*/
 /* the user) than in each process call we will be providing one field of data*/
 /* In odd number process call we will have to provide bottom field data and  */
 /* in even numbered process call we will have to provide top field data.     */
 /*---------------------------------------------------------------------------*/
   if(botFieldFirst && (dataLayout == IVIDEO_FIELD_SEPARATED)&&
    (inputBuf->contentType == IVIDEO_INTERLACED) )
   {
     if(((frameCount & 0x1) == 0) || (gConfig.numProcessCall == 30))
     {
      fieldOffset = 1;
     }
      else
     {
      fieldOffset = -1;
     }
   }

  if(gConfig.numProcessCall != 60 && (dataLayout == IVIDEO_FIELD_SEPARATED) &&
      (inputBuf->contentType == IVIDEO_INTERLACED)
  )
  {
    /* Not 60 process call and data layout = 1 and interlaced */
    numFields         =  2 ;
    fieldOffsetLuma   =
      inputBuf->secondFieldOffsetHeight[0] * frmWidth +
      inputBuf->secondFieldOffsetWidth[0] ;
    fieldOffsetChroma =
      inputBuf->secondFieldOffsetHeight[1] * frmWidth +
      inputBuf->secondFieldOffsetWidth[1] ;
   /*---------------------------------------------------------------------------*/
   /* Seek to the excact offset in the file                                     */
   /*---------------------------------------------------------------------------*/
    //my_fseek(fin, 2*pic_size * (frameCount), SEEK_SET);
  }
  else
  {
   /*---------------------------------------------------------------------------*/
   /* Seek to the excact offset in the file                                     */
   /*---------------------------------------------------------------------------*/
    //my_fseek(fin, pic_size * (frameCount + fieldOffset), SEEK_SET);
    numFields       = 1 ;
  }

  //numFields = 0;
  for(fieldNo = 0 ; fieldNo < numFields ; fieldNo++)
  {
   my_fseek(fin, pic_size * (numFields * frameCount + fieldOffset), SEEK_SET);
   if(botFieldFirst)
   {
     fieldOffset = 0;
   }
   else
   {
     fieldOffset = 1;
   }

   /*---------------------------------------------------------------------------*/
   /*   Update pointer to luma input buffer read data                           */
   /*---------------------------------------------------------------------------*/
    dst = (XDAS_UInt8 *) pLuma + fieldNo * fieldOffsetLuma;
    my_fread((char *)dst, sizeof(XDAS_UInt8), (frmWidth*frmHeight),fin);
   /*---------------------------------------------------------------------------*/
   /*   Update pointer to chroma input buffer and read data                     */
   /*---------------------------------------------------------------------------*/
    dst = (XDAS_UInt8*) pChroma + fieldNo * fieldOffsetChroma;
    my_fread((char*)dst, sizeof(XDAS_UInt8), (frmWidth*frmHeight>>1),fin);
  }

  if(lumaTilerSpace)
  {
  Tiler_DataMove((char *)RawLuma,(char *)gInputBufDesc.planeDesc[0].buf,frmWidth,
    frmHeight*numFields, lumaTilerSpace);
  }

  if(chromaTilerSpace)
  {
  Tiler_DataMove((char *)RawChroma,(char *)gInputBufDesc.planeDesc[1].buf,frmWidth,
    (frmHeight/2)*numFields, chromaTilerSpace);
  }

  return 0;
}
/*===========================================================================*/
/**
*@brief This function converts the frame/field data to the raster-scan format.
*
*@param  pubYFrameData
*        Pointer to the Y component of the raster-scan frame.
*
*@param  pubUFrameData
*        Pointer to the U component of the raster-scan frame.
*
*@param  pubVFrameData
*        Pointer to the V component of the raster-scan frame.
*
*@param  pubPictY[2]
*        Pointer to the Luminance (Y) component of the picture for both
*        fields. Populate only the first element for frame data
*
*@param  pubPictUV[2]
*        Pointer to the Chrominance (UV) component of the picture for both
*        fields. populate only the first element for frame data
*
*@param  uiPicWidth
*        Width of the input picture.
*
*@param  uiPicHeight
*        Heigth of the input picture.
*
*@param  uiReconPitch,
*        Pitch of the reconstructed image from encoder
*
*@param  ubIsField
*        Flag to indicate wherther to separate the data into two different
*        fields or not (0 : Frame data / 1 : Field).
*
*@return None
*
*@see    None
*
*@note   None
*/
/*===========================================================================*/

void ConvertFrame_Field_Out
(
    XDAS_UInt8  *pubYFrameData,
    XDAS_UInt8  *pubUFrameData,
    XDAS_UInt8  *pubVFrameData,
    XDAS_UInt8  *pubPictY[2],
    XDAS_UInt8  *pubPictUV[2],
    XDAS_UInt32  uiPictWidth,
    XDAS_UInt32  uiPictHeight,
    XDAS_UInt32  uiReconPitch,
    XDAS_UInt8   ubIsField
)
{
  // Variables declared here
  unsigned short j,k;
  unsigned char *pubSrc[2],*PubSrc_field;
  unsigned char *pubDstY, *pubDstU, *pubDstV;
  unsigned char ubIdx = 0;
  unsigned int uiRecWidth;

  // Code starts here
  /*-------------------------------------------------------------------------*/
  /*                           CONVERT LUMINANCE DATA                        */
  /*-------------------------------------------------------------------------*/
  pubSrc[0] = 32 + pubPictY[0] + (32 * uiReconPitch);


  uiRecWidth = uiReconPitch;

  ubIdx = 0;

  for(j = 0; j < uiPictHeight; j++)
  {

    pubDstY = pubYFrameData + j * (uiPictWidth << ubIsField );

    memcpy(pubDstY, pubSrc[ubIdx], uiPictWidth);
    if(ubIsField)
    {
      PubSrc_field = pubSrc[ubIdx]+ uiReconPitch *(uiPictHeight + 64);
      pubDstY = pubDstY + uiPictWidth;
      memcpy(pubDstY, PubSrc_field, uiPictWidth);
    }

    // Update the destination pointer
    pubSrc[ubIdx] += uiRecWidth;
  }// 0 - uiPictHeight

  /*-------------------------------------------------------------------------*/
  /*                           CONVERT CHROMINANCE DATA                      */
  /*-------------------------------------------------------------------------*/
  /* Subtracting 4 from luma horizontal pitch since currently the            */
  /* luma/chroma pitches are not same                                        */
  /*-------------------------------------------------------------------------*/
  //uiRecWidth = uiReconPitch-4;
  uiPictHeight >>= 1;
  uiPictWidth >>= 1;

  pubSrc[0] = 32 + pubPictUV[0] + (16 * uiReconPitch);


  ubIdx = 0;

  for(j = 0; j < uiPictHeight; j++)
  {
    unsigned char *pubCurSrc;


    pubDstU = pubUFrameData + j * (uiPictWidth << ubIsField );
    pubDstV = pubVFrameData + j * (uiPictWidth <<ubIsField );

    pubCurSrc = pubSrc[ubIdx];

    for(k = 0; k < uiPictWidth; k++)
    {
      *pubDstU++  = *pubCurSrc++;
      *pubDstV++  = *pubCurSrc++;
    }
    if (ubIsField)
    {

      pubCurSrc = pubSrc[ubIdx] + uiRecWidth*(uiPictHeight + 32);

    for(k = 0; k < uiPictWidth; k++)
    {
      *pubDstU++  = *pubCurSrc++;
      *pubDstV++  = *pubCurSrc++;
    }
    }

    pubSrc[ubIdx] += uiRecWidth;
  }

}

/*===========================================================================*/
/**
*@func   WriteReconOut()
*
*@brief  Write out the Reconstructed frame
*
*@param  pubReconY
*        Pointer to buffer containing Luma data
*
*@param  pubReconU
*        Pointer to buffer containing Chroma Cb data
*
*@param  pubReconV
*        Pointer to buffer containing Chroma Cr data
*
*@param  uiWidth
*        Picture Width
*
*@param  uiHeight
*        Picture Height
*
*@param  pfOutYUVFile
*        Output file pointer
*
*@return Status
*
*@note
*/
/*===========================================================================*/
XDAS_Int32 WriteReconOut
(
   XDAS_UInt8 *pubReconY,
   XDAS_UInt8 *pubReconU,
   XDAS_UInt8 *pubReconV,
   XDAS_Int32  uiWidth,
   XDAS_Int32  uiHeight,
   FILE       *pfOutYUVFile
)
{

  XDAS_Int32 uiSize = uiWidth * uiHeight;

  /*-------------------------------------------------------------------------*/
  /*                        WRITE THE BITSTREAM                              */
  /*-------------------------------------------------------------------------*/
  my_fwrite(pubReconY, 1, uiSize, pfOutYUVFile);
  my_fwrite(pubReconU, 1, (uiSize >> 2), pfOutYUVFile);
  my_fwrite(pubReconV, 1, (uiSize >> 2), pfOutYUVFile);

  return(0);
}

/**
********************************************************************************
 *  @fn     TestApp_WriteOutputData
 *  @brief  This function writes filtered frame data into the output file
 *
 *  @param[in]  outputBuf : Pointer to output buffer
 *
 *  @param[in]  uiBytesToWrite : Total number of bytes to be written
 *
 *  @param[in]  fout : File pointer to the output file
 *
 *  @return     -1 in case of error
 *               0 in case when there is no error
********************************************************************************
*/
void TestApp_WriteOutputData(FILE *pfOutYUVFile)
{
    /*-------------------------------------------------------------------------*/
    /* Buffer Pointers to the reconstructed frame                               */
    /*-------------------------------------------------------------------------*/
    XDAS_UInt8 *pubYRecon;
    XDAS_UInt8 *pubURecon;
    XDAS_UInt8 *pubVRecon;

    XDAS_UInt8 *pubInBufY[2];
    XDAS_UInt8 *pubInBufUV[2];
    unsigned char ubIsField;
    XDAS_UInt32 uiSize;
    /*-------------------------------------------------------------------------*/
    /* Recon Dump of interlace type in progressive format.                     */
    /*-------------------------------------------------------------------------*/

    ubIsField = gParams.vidnf1Params.inputContentType ;
    uiSize = (gDynamicParams.vidnf1DynamicParams.inputHeight << ubIsField)
            * gDynamicParams.vidnf1DynamicParams.inputWidth;
    pubYRecon = malloc(uiSize * sizeof(XDAS_Int8));
    pubURecon = malloc((uiSize >> 2) * sizeof(XDAS_Int8));
    pubVRecon = malloc((uiSize >> 2) * sizeof(XDAS_Int8));

    pubInBufY[0]  = (XDAS_UInt8 *)gOutputBufDesc.planeDesc[0].buf;
    pubInBufUV[0] = (XDAS_UInt8 *)gOutputBufDesc.planeDesc[1].buf;

    ConvertFrame_Field_Out
    (
        pubYRecon,
        pubURecon,
        pubVRecon,
        pubInBufY,
        pubInBufUV,
        gDynamicParams.vidnf1DynamicParams.inputWidth,
        gDynamicParams.vidnf1DynamicParams.inputHeight,
        gOutputBufDesc.imagePitch[0],
        ubIsField
    );

    WriteReconOut
    (
        pubYRecon,
        pubURecon,
        pubVRecon,
        gDynamicParams.vidnf1DynamicParams.inputWidth,
        (gDynamicParams.vidnf1DynamicParams.inputHeight<<ubIsField),
        pfOutYUVFile
    );

    free(pubYRecon);
    free(pubURecon);
    free(pubVRecon);
}

/**
********************************************************************************
 *  @fn     TestApp_CompareOutputData
 *  @brief  This function will compare the output generated data to ref data
 *
 *  @param[in]  fRefFile : File pointer to the ref data
 *
 *  @param[in]  outputBufDesc : Output buffer discriptors containing filtered
 *                              data buffer pointers
 *
 *  @param[in]  size : Size of the filtered YUV data
 *
 *  @return     -1 in case of error
 *               0 in case when there is no error
********************************************************************************
*/

XDAS_Int32 TestApp_CompareOutputData
(
    FILE         *fRefFile,
    XDM2_BufDesc *outputBufDesc,
    XDAS_Int32    size
)
{
  XDAS_Int32 i,retVal;
  XDAS_UInt8 *refData;
  retVal = IVIDNF1_EOK;

  /*--------------------------------------------------------------------------*/
  /*   Allocate memory for the refdata with the size given                    */
  /*--------------------------------------------------------------------------*/
  refData = malloc(size + 32);

  /*--------------------------------------------------------------------------*/
  /* Compare all the output Buffers with the ref File                         */
  /*--------------------------------------------------------------------------*/
  for(i=0; i < outputBufDesc->numBufs ; i++)
  {
    fread(refData, 1, size, fRefFile);
    if(memcmp(refData, outputBufDesc->descs[i].buf, size))
    {
      retVal = IVIDNF1_EFAIL;
    }
    break ;
  }

  free(refData);
  return retVal;
}

/* ======================================================================== */
/*  End of file:  MCTNFTest.c                                               */
/* ------------------------------------------------------------------------ */
/*            Copyright (c) 2013 Texas Instruments, Incorporated.           */
/*                           All Rights Reserved.                           */
/* ======================================================================== */
