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
 * @file <MPEG4SPEncTest.c>
 *
 * @brief This File is a test app calling MPEG4 SP Encoder.
 *
 * @author: Madhukar Budagavi
 *
 * @version 0.0 (Jul 2007) : Intial version
 *                           [Madhukar Budagavi]
 *
 * @version 0.1 (Apr 2008) : Cleanup and Modified for IVA-HD
 *                           [Venugopala Krishna]
 *
 * @version 0.2 (Mar 2009) : Total cleanup for the target Code
 *                           [Venugopala Krishna]
 *
 *******************************************************************************
*/

/* -------------------- compilation control switches -------------------------*/
/*******************************************************************************
*                             INCLUDE FILES
*******************************************************************************/
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
#include "tilerBuf.h"
#include <mpeg4enc_ti_config.h>
#include <mpeg4enc_ti_test.h>

#ifdef A9_HOST_FILE_IO
#include <mpeg4enc_ti_file_io.h>
#include "mpeg4enc_ti_init_ivahd_clk.h"
#endif

/*******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/
#pragma DATA_SECTION(output_buffer, ".outputbuffer")
#pragma DATA_SECTION(dataSyncOutputBuffer, ".outputbuffer")
#pragma DATA_SECTION(RawLuma, ".RawInput");
#pragma DATA_SECTION(RawChroma, ".RawInput");

/*----------------------------------------------------------------------------*/
/*  Analytic info output buffer                                               */
/*----------------------------------------------------------------------------*/
#pragma DATA_SECTION(gAnalyticInfoOutputData, ".analyticinfo_sect");
volatile XDAS_UInt8 gAnalyticInfoOutputData[ANALYTICINFO_OUTPUT_BUFF_SIZE];

U08 output_buffer[OUTPUT_BUFFER_SIZE];
U08 dataSyncOutputBuffer[OUTPUT_BUFFER_SIZE];
sTilerParams TilerParams;
U32 frameCaptureWidth;
U32 frameCaptureHeight;

#ifdef A9_HOST_FILE_IO
#define LUMA_TILER_RAW    4
#define CHROMA_TILER_RAW  4
#else
#define LUMA_TILER_RAW  2048*1256
#define CHROMA_TILER_RAW 2048*(1256/2)
#endif
U08 RawLuma[LUMA_TILER_RAW];
U08 RawChroma[CHROMA_TILER_RAW];
extern U32 gInitialTime;
extern U32 gSlicesBaseIndex;
extern U32 gSlicesSizeIndex;
extern S32 *baseAddrGetBuffer;
extern U32 *gSlicesBase;
extern U32 *gSlicesSize;

U32 datasynchNumBlocks[8];


char version[100];
extern MY_FILE *fout2;

/*-----------------------------------------------------------------------------*/
/*   Lenth of Boot code                                                        */
/*-----------------------------------------------------------------------------*/
#define LENGTH_BOOT_CODE  14

/*------------------------------------------------------------------------------*/
/*   Hex code to set for Stack setting, Interrupt vector setting                */
/*   and instruction to put ICONT in WFI mode.                                  */
/*   This shall be placed at TCM_BASE_ADDRESS of given IVAHD, which is          */
/*   0x0000 locally after reset.                                                */
/*------------------------------------------------------------------------------*/

const unsigned int IVAHD_memory_wfi[LENGTH_BOOT_CODE] = {
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

/*----------------------------------------------------------------------------*/
/*  Structure to hold */
/*----------------------------------------------------------------------------*/
static XDAS_Int8 *pFrameType[3] = {
    (XDAS_Int8 *)"IVIDEO_I_FRAME",
    (XDAS_Int8 *)"IVIDEO_P_FRAME",
    (XDAS_Int8 *)"FRAME_SKIPPED",
  };

/*-------------------------------------------------------------------------*/
/* Handle to the Encoder instance - XDAIS */
/*-------------------------------------------------------------------------*/
MPEG4ENC_Handle mpeg4encHandle ;


/*----------------------------------------------------------------------------*/
/*Instance of the config structure                                             */
/*----------------------------------------------------------------------------*/
extern MPEG4EncoderConfig  gConfig;
/*----------------------------------------------------------------------------*/
/* Instance creation Parameter - XDM                                          */
/*----------------------------------------------------------------------------*/
extern MPEG4ENC_Params gParams;

/*----------------------------------------------------------------------------*/
/* Dynamic Parameter struct- XDM                                              */
/*----------------------------------------------------------------------------*/
extern MPEG4ENC_DynamicParams gDynamicParams;


U32 frameNumber;
S16 sliceNumber;

/*
 * Output bitstream filename for data sync
 */
S08 datasync_bitstreamname[FILE_NAME_SIZE];

/*
 * Output bitstream filename for stitched stream
 */
S08 stitched_bitstreamname[FILE_NAME_SIZE];

/*----------------------------------------------------------------------------*/
/*  Global flag for non-blocking call.                                        */
/*----------------------------------------------------------------------------*/
volatile XDAS_Int8    gMPEG4EIvahdDoneFlag = 1;


/**
* Defining this macro will enable to compare the platform generated output
* with reference generated output. This is usefull for sanity testing the 
* platform code
*/
// #define REF_COMPARE_TEST
// #define ENABLE_RECON_DUMP 1
#undef ENABLE_RECON_DUMP

Void WriteOutFrameYUV420nv12(
  MPEG4ENC_OutArgs * outArgs,
  MY_FILE *fout,
  S16 frameWidth,
  S16 frameHeight,
  S16 refWidth
);

/*******************************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
*******************************************************************************/
/*---------------------- data declarations -----------------------------------*/

/*---------------------- function prototypes ---------------------------------*/
/*---------------------- function prototypes ---------------------------------*/

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


/**
********************************************************************************
 *  @func     TestApp_ReadInputtoRawMem
 *  @brief    This function reads input yuv 
 *
 *  @param[in]  Luma          : Luma data poitner
 *  @param[in]  Chroma        : Chroma pointer
 *  @param[in]  frmWidth      : Width of input frame
 *  @param[in]  frmHeight     : Height of input frame
 *  @param[in]  dataLayout    : Video buffer layout.
 *  @param[in]  frameCount    : Pointer to frame count
 *  @param[in]  contentType   : Video content type
 *  @param[in]  fin           : Pointer to input file
 * 
 *  @return     none
********************************************************************************
*/

XDAS_Void TestApp_ReadInputtoRawMem(
  pU8 Luma,
  pU8 Chroma,
  S16 frmWidth,
  S16 frmHeight,
  U16 dataLayout,
  S32* frameCount,
  U16 contentType,
  MY_FILE * fin)
{
  S32 pic_size;
  U08 *dst;
  S08 fieldOffset = 0;
  S32 currFrameNum = (*frameCount) + 0;
  if((contentType== IVIDEO_INTERLACED) &&
    (dataLayout == IVIDEO_FIELD_INTERLEAVED))
  {
    frmHeight <<= 1;
  }
  pic_size = (3 * frmWidth * frmHeight) >> 1;

  MY_FSEEK(fin, pic_size * (currFrameNum + fieldOffset), SEEK_SET);

  dst = Luma;
  MY_FREAD((char *)dst, sizeof(U08), (frmWidth*frmHeight), fin);

  /* Read Chroma buffer */
  dst = Chroma;
  MY_FREAD((char*)dst, sizeof(U08), (frmWidth*frmHeight>>1), fin);

  if(dataLayout == IVIDEO_FIELD_SEPARATED)
  {
    if((contentType == IVIDEO_INTERLACED) && (dataLayout == IVIDEO_FIELD_SEPARATED))
    {

      dst = (U08 *) (Luma + (frmWidth*frmHeight));
      MY_FREAD((char *)dst, sizeof(U08), (frmWidth*frmHeight), fin);

      /* Read Chroma buffer */
      dst = (U08 *)(Chroma + (frmWidth * (frmHeight >> 1)));
      MY_FREAD((char *)dst, sizeof(U08), (frmWidth*frmHeight>>1), fin);
      /*(*frameCount)++;*/
    }
  }
  return;
}

/**
********************************************************************************
 *  @func     TestApp_CompareOutputData
 *  @brief  This function will compare output data with reference file data
 *
 *  @param[in]  fRefFile      : reference file pointer
 *
 *  @param[in]  outputBufDesc : Poniter to the output buffer descriptor
 *
 *  @param[in]  outArgs       : Poniter to the output arguments
 *
 *  @return     IVIDENC2_EOK or IVIDENC2_EFAIL
********************************************************************************
*/
XDAS_Int32 TestApp_CompareOutputData(FILE *fRefFile,
                                     XDM2_BufDesc *outputBufDesc,
                                     MPEG4ENC_OutArgs *outArgs)
{
  XDAS_Int32 i, retVal;
  XDAS_UInt8 *refData;

  retVal = IVIDENC2_EOK;

  refData = malloc(outArgs->videnc2OutArgs.bytesGenerated + 32);

  /*--------------------------------------------------*/
  /* Compare all the output Buffers with the ref File */
  /*--------------------------------------------------*/
  for(i=0; i < outputBufDesc->numBufs ; i++)
  {
    fread(refData, 1, outArgs->videnc2OutArgs.bytesGenerated, fRefFile);
    if (memcmp(refData, outputBufDesc->descs[i].buf,
          outArgs->videnc2OutArgs.bytesGenerated))
    {
      retVal = IVIDENC2_EFAIL;
    }
    break ;
  }

  free(refData);
  return retVal;
}

/**
********************************************************************************
 *  @func     WriteOutBitstream
 *  @brief    write output bitstream to file
 *
 *  @param[in]  Addr : Buffer address
 *
 *  @param[in]  cnt  : Number of bytes to be written
 *
 *  @param[in]  fout : File pointer of the file where bitstream content to be
 *                     dumped
 *
 *  @return  None
********************************************************************************
*/
Void WriteOutBitstream(U32 Addr, U32 cnt, MY_FILE * fout)
{
  U08 *dst;
  dst = (U08 *) Addr;
  MY_FWRITE((char*) dst, 1, cnt, fout);
  return;
}


/**
********************************************************************************
 *  @func     ReadInYUV420Picture
 *  @brief    Read input YUV data from the file and dump into an buffer
 *
 *  @param[in]  inputBuf  : Pointer to IVIDEO2_BufDesc structure
 *
 *  @param[in]  frame     : Number of frames read till now
 * 
 *  @param[in]  end_frame : indicates last frame in the file
 *
 *  @param[in]  Fskip     : Flag to skip the frame reading
 *
 *  @param[in]  width     : Width of input YUV frame
 *
 *  @param[in]  height    : Height of input YUV frame
 * 
 *  @param[in]  fin       : File pointer to input YUV file
 *
 *  @return  0 if success
 *           1 if end of frames
********************************************************************************
*/
S16 ReadInYUV420Picture(
  IVIDEO2_BufDesc *inputBuf,
  S16 frame,
  S16 end_frame,
  S16 Fskip,
  S16 width,
  S16 pitch,
  S16 height,
  MY_FILE *fin,
  XDAS_UInt8 lumaTilerSpace, XDAS_UInt8 chromaTilerSpace
)
{
  U32 pic_size;    /* picture size */
  U08 *dst;

  /*----------------------------------------------------------------------*/
  /* check if the number of current frame is larger than the given end    */
  /* frame. If true, encoding is finished                                 */
  /*----------------------------------------------------------------------*/
  if (frame > end_frame)
    return 1;

  /*--------------------------------------------------------*/
  /* For tiled buffer first read into some temporary buffer */
  /*--------------------------------------------------------*/
  if(lumaTilerSpace)
  { 
    dst   = RawLuma   ;
  }
  else
  {
    dst   = (XDAS_UInt8 *) inputBuf->planeDesc[0].buf ; 
  }
  
  /*----------------------------------------------------------------------*/
  /* get the picture size                                                 */
  /*----------------------------------------------------------------------*/
  pic_size = (3 * pitch * height) >> 1;
  MY_FSEEK(fin, pic_size * frame, SEEK_SET);

  /*----------------------------------------------------------------------*/
  /* Read Luma buffer                                                     */
  /*----------------------------------------------------------------------*/
#if 0 // Karthick
  if(width != pitch)
  {
    MY_FREAD((char*) dst, sizeof(U08), pitch * height, fin);
  }
  else
  {
    MY_FREAD((char*) dst, sizeof(U08), (width * height), fin);
  }
#endif

  if(pitch & 0xF)
  {
  	U32 line;
  	// If capture width  is not a multiple of 16
  	for(line = 0; line < height; line++)
  	{
  	  MY_FREAD((char*) dst, sizeof(U08), pitch, fin);
  	  dst = (U08*)((U32)dst + ((pitch + 0xF) & ~0xF));
  	}
  }
  else
  {
  	MY_FREAD((char*) dst, sizeof(U08), pitch * height, fin);
  }

    /*--------------------------------------------------------*/
  /* For tiled buffer first read into some temporary buffer */
  /*--------------------------------------------------------*/
  if(chromaTilerSpace)
  { 
    dst   = RawChroma   ;
  }
  else
  {
    dst   = (XDAS_UInt8 *) inputBuf->planeDesc[1].buf ; 
  }
  /*----------------------------------------------------------------------*/
  /* Read Chroma buffer                                                   */
  /*----------------------------------------------------------------------*/
#if 0 // Karthick
  if(width != pitch)
  {
   MY_FREAD((char*) dst, sizeof(U08), pitch * (height >> 1), fin);
  }
  else
  {
    MY_FREAD((char*) dst, sizeof(U08), (width * height>>1), fin);
  }
#endif
  if(pitch & 0xF)
  {
  	U32 line;
  	// If capture width is not a multiple of 16
  	for(line = 0; line < (height >> 1); line++)
  	{
  	  MY_FREAD((char*) dst, sizeof(U08), pitch, fin);
  	  dst = (U08*)((U32)dst + ((pitch + 0xF) & ~0xF));
  	}
  }
  else
  {
  	MY_FREAD((char*) dst, sizeof(U08), pitch * (height >> 1), fin);
  }
  
  
  if(lumaTilerSpace)
  {
    Tiler_DataMove((char *)RawLuma,(char *)inputBuf->planeDesc[0].buf,pitch, 
      height, lumaTilerSpace);
  }

  if(chromaTilerSpace)
  {
    Tiler_DataMove((char *)RawChroma,(char *)inputBuf->planeDesc[1].buf,pitch, 
      (height/2), chromaTilerSpace);
  }

  return 0;
}

/**
********************************************************************************
 *  @func     main
 *  @brief  Main function for MPEG4 SP encoder
 *
 *  @param[in]  argc   : Number of arguments passed to main
 *
 *  @param[in]  argv[] : Pointer to the arguments array
 *
 *  @return None
********************************************************************************
*/
int main(int argc, char *argv[])
{
  /*-------------------------------------------------------------------------*/
  /*  Declaring Algorithm specific handle                                    */
  /*-------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------*/
  /*  Base Class Structures for status, input and output arguments.          */
  /*-------------------------------------------------------------------------*/
  MPEG4ENC_Status status;   /* Stores the status of process call   - XDM     */
  MPEG4ENC_InArgs inArgs;   /* Input Parameter to the process call - XDM     */
  MPEG4ENC_OutArgs outArgs; /* Ouput parameters from process call  - XDM     */


  /*-------------------------------------------------------------------------*/
  /*  Input/Output Buffer Descriptors                                        */
  /*-------------------------------------------------------------------------*/
  IVIDEO2_BufDesc inputBufDesc; /*Input Buffer description array     - XDM   */
  XDM2_BufDesc outputBufDesc;   /*Output Buffer description array    - XDM   */


  /*-------------------------------------------------------------------------*/
  /*  Handle to a buffer element allocated by the buffer manager module      */
  /*-------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------*/
  /*  Handle to a buffer element allocated by the buffer manager module      */
  /*-------------------------------------------------------------------------*/
  BUFFMGR_buffEleHandle buffEle;
#ifdef RESET_TEST
  XDAS_Int8     outFile[FILE_NAME_SIZE];
  XDAS_UInt32   uiReset_Test = 0;
#endif
  XDAS_UInt32 BitstreamAddr;
  XDAS_Int32  frame_nr = 0;
  XDAS_Int32  retValue;
  XDAS_Int32  end_frame;
  XDAS_Int32  currFrame;
  XDAS_Int32   ret;
#ifndef A9_HOST_FILE_IO
  XDAS_Int32  cnt;
#endif
  XDAS_Int16  frameSkip = 0;
  XDAS_Int16  frameStart = 0, frameCoded ;
  XDAS_Int16  frameWidth ;
  XDAS_Int16  framePitch ;
#ifndef A9_HOST_FILE_IO
  XDAS_Int16  frameHeight ;
#endif
  XDAS_Int16  captureWidth;
  XDAS_Int16  last_frame;
  XDAS_UInt8 *MEM_SDRAM;
  char  *filename = DEFAULTCONFIGFILENAME;

  MY_FILE *fp = NULL;
  MY_FILE *fpbits = NULL;
  MY_FILE *fout = NULL;
#ifdef REF_COMPARE_TEST
  FILE *frefbits;
#endif
  FILE *pfConfigParamsFile;

  FILE *ftestcases;
  char *testcases_configfilename = "../../../Test/TestVecs/Config/configs.cfg";
  char testcasefilename[200];
  U32 iterationCount = 0;

#ifdef A9_HOST_FILE_IO  
  /*--------------------------------------------------------------------------*/
  /*  Enable the clock to IVAHD module. This is required only in case of      */
  /*  HW where GEL files are not used for enabling clock for IVAHD            */
  /*--------------------------------------------------------------------------*/
  Init_IVAHD_Clks();

  /*--------------------------------------------------------------------------*/
  /*  Initializa the file I/O methos specific to the paltform                 */
  /*  PCI based or Linux on A9 or jtag based                                  */
  /*--------------------------------------------------------------------------*/
  init_file_io();
#endif  
  

  if((ftestcases = fopen(testcases_configfilename, "r")) == NULL)
  {
    printf("Error: Opening test cases file.\n");
    return -1;
  }

  while(!feof(ftestcases))
  {
    printf("******************************\n");
    printf("Test case iteration count = %d\n", ++iterationCount);
    printf("******************************\n");
    fscanf(ftestcases, "%s", testcasefilename);
    filename = testcasefilename;
    
    // Initialization for every configuration. Used while closing the file ptr.
    fp = fout = fpbits = fout2 = NULL;
    
  /*-------------------------------------------------------------------------*/
  /*  MPEG4 SP encoder one-time Initialization                               */
  /*-------------------------------------------------------------------------*/
  
  /*-------------------------------------------------------------------------*/
  /*  Initialization of static parameters is done by this function call      */
  /*-------------------------------------------------------------------------*/
  TestApp_SetInitParams(&gParams, &gConfig);
  
  /*-------------------------------------------------------------------------*/
  /* Memory pointer to the output bitstream                                  */
  /*-------------------------------------------------------------------------*/
  MEM_SDRAM = (U08 *) output_buffer;
  BitstreamAddr = (U32) MEM_SDRAM;
  /*-------------------------------------------------------------------------*/
  /* Make it aligned to 32 bytes                                             */
  /*-------------------------------------------------------------------------*/
  BitstreamAddr = ((BitstreamAddr + 31) >> 5) << 5;
  baseAddrGetBuffer = (S32*) BitstreamAddr;
  /*-------------------------------------------------------------------------*/
  /*  Read input parameters                                                  */
  /*-------------------------------------------------------------------------*/
   pfConfigParamsFile = fopen(filename,"r");
  /*-------------------------------------------------------------------------*/
  /*  Perform file open error check.                                         */
  /*-------------------------------------------------------------------------*/
  if (!pfConfigParamsFile)
  {
    printf("Couldn't open Parameter Config file %s.\n",filename);
    return XDM_EFAIL;
  }
  Initialize_map();
  readparamfile(pfConfigParamsFile);
  fclose(pfConfigParamsFile);

  /*--------------------------------------------------------------------------*/
  /*  Get the IVAHD Config, SL2, PRCM base address based on IVAHD ID          */
  /*--------------------------------------------------------------------------*/
  Init_IVAHDAddrSpace();


  IVAHD_Standby_power_on_uboot();
  
  /*-------------------------------------------------------------------------*/
  /*  Initialize the profiling info related logic                            */
  /*-------------------------------------------------------------------------*/
  MPEG4ENC_TI_Init_Profile();


  /*-------------------------------------------------------------------------*/
  /*  Setting the sizes of Base Class Objects                                */
  /*-------------------------------------------------------------------------*/
  status.videnc2Status.size               = sizeof(MPEG4ENC_Status);
  inArgs.videnc2InArgs.size               = sizeof(MPEG4ENC_InArgs);
  outArgs.videnc2OutArgs.size             = sizeof(MPEG4ENC_OutArgs);
  status.videnc2Status.extendedError      = 0;

  /*-------------------------------------------------------------------------*/
  /*  Create MPEG4 context                                                   */
  /*-------------------------------------------------------------------------*/
  mpeg4encHandle = 
    MPEG4ENC_create((MPEG4ENC_Fxns *) & MPEG4ENC_TI_IMPEG4ENC, &gParams);

  if(mpeg4encHandle == NULL)
  {
    printf("\nEncoder creation failed... Exiting");
    printf("Check the Create-time static parameter settings.\n");
    exit(0);
  }

  /*-----------------------------------------------------------------------*/
  /*  Print success message if handle creation is proper.                  */
  /*-----------------------------------------------------------------------*/
  printf("\nAlgorithm Instance Creation Done...\n");
  
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
    sTilerParams *pTilerParams = &TilerParams;
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
      /*----------------------------------------------------------------------*/
      /* If tiler allocation is not successful then Force both the buffer to  */
      /* be in raw region                                                     */
      /*----------------------------------------------------------------------*/
      gConfig.tilerSpace[0]=  gConfig.tilerSpace[1] = 0 ;
    }
    else
    {
      /*----------------------------------------------------------------------*/
      /* Library also overrides few requests so take to the application       */
      /* variables to keep the flow correct                                   */
      /*----------------------------------------------------------------------*/
      gConfig.tilerSpace[0]  = pTilerParams->tilerSpace[0]  ;
      gConfig.tilerSpace[1]  = pTilerParams->tilerSpace[1]  ;
    }
    
#ifdef HOSTCORTEXM3_OMAP4
    /* progrma the DMM PAT registers incased of OMAP4 SDC */
    if(gConfig.tilerSpace[0])
    {
      *(int *)(0x4E000460) = 0x80000000; 
      *(int *)(0x4E000480) = 0x00000003; 
      *(int *)(0x4E0004C0) = 0x0000000B; 
      *(int *)(0x4E000504) = 0x3FFF20E0;
      *(int *)(0x4E00050C) = 0x8510F010;
    }  
#endif    
    
    
  }

  /*-----------------------------------------------------------------------*/
  /*  Call to RMAN Assign Resources (to verify the ires-rman functions.    */
  /*-----------------------------------------------------------------------*/

  /* Place holder for RMAN Call */
  RMAN_AssignResources((IALG_Handle)mpeg4encHandle,gConfig.tilerSpace[0], 
    gConfig.tilerSpace[1] );

  /*--------------------------------------------------------------------------*/
  /* Get the version of codec and print it                                    */
  /*--------------------------------------------------------------------------*/
  status.videnc2Status.data.buf = (XDAS_Int8*)&version[0];
  status.videnc2Status.data.bufSize = FILE_NAME_SIZE;

  retValue = MPEG4ENC_control(mpeg4encHandle,XDM_GETVERSION,
          (MPEG4ENC_DynamicParams *)&gDynamicParams,(MPEG4ENC_Status *)&status);
  
  printf("\nAlgorithm Instance  Creation for the Module %s Done...\n", version);
  status.videnc2Status.data.bufSize = 0;
  
  /*--------------------------------------------------------------------------*/
  /* Check for the create time errors, and print them in log file             */
  /* Controll commands will enabled only after RMAN_AssignResources           */
  /* First we will print codec version when before printig any error          */
  /*--------------------------------------------------------------------------*/
  if(MPEG4ENC_TI_Report_Error(status.videnc2Status.extendedError))
  {
    printf("Creation Failed.\n");
    goto DELETE_INSTANCE;
  }
  /*--------------------------------------------------------------------------*/
  /* Check for the create time errors, and print them in log file             */
  /* Controll commands will enabled only after RMAN_AssignResources           */
  /* First we will print codec version when before printig any error          */
  /*--------------------------------------------------------------------------*/
  
  /*--------------------------------------------------------------------------*/
  /* Fix for SDOCM00077409                                                    */
  /* If captureWidth is zero, test application should take inputWidth as      */ 
  /* captureWidth and has to allocate the buffer and read input frame         */
  /* accordingly.                                                             */
  /*--------------------------------------------------------------------------*/
  captureWidth = (gDynamicParams.videnc2DynamicParams.captureWidth == 0) ?
                  gDynamicParams.videnc2DynamicParams.inputWidth :
                  gDynamicParams.videnc2DynamicParams.captureWidth;
  gDynamicParams.videnc2DynamicParams.captureWidth =
    (gDynamicParams.videnc2DynamicParams.captureWidth + 0xF) & ~0xF;
  
  /*--------------------------------------------------------------------------*/
  /* Fix for SDOCM00077409                                                    */
  /* If captureWidth is zero, test application should take inputWidth as      */ 
  /* captureWidth and has to allocate the buffer and read input frame         */
  /* accordingly.                                                             */
  /*--------------------------------------------------------------------------*/
  frameCaptureWidth = (gDynamicParams.videnc2DynamicParams.captureWidth == 0) ?
                  gDynamicParams.videnc2DynamicParams.inputWidth :
                  gDynamicParams.videnc2DynamicParams.captureWidth;
  frameCaptureHeight = gConfig.captureHeight;
  
  /*-----------------------------------------------------------------------*/
  /*  The following function is used to initialize the dynamic params of   */
  /*  the IVIDENC2 handle.                                                 */
  /*-----------------------------------------------------------------------*/
  TestApp_SetDynamicParams(&gDynamicParams);

  frameCoded = gConfig.numFrames;
  frameWidth = (S16) (gDynamicParams.videnc2DynamicParams.inputWidth);
  
  /*--------------------------------------------------------------------------*/
  /* Fix for SDOCM00077409                                                    */
  /* If captureWidth is zero, test application should take inputWidth as      */ 
  /* captureWidth and has to allocate the buffer and read input frame         */
  /* accordingly.                                                             */
  /*--------------------------------------------------------------------------*/
  framePitch = (S16) (gDynamicParams.videnc2DynamicParams.captureWidth == 0) ?
                  gDynamicParams.videnc2DynamicParams.inputWidth :
                  gDynamicParams.videnc2DynamicParams.captureWidth;
  if(framePitch <= frameWidth)
    framePitch = frameWidth;
#ifndef A9_HOST_FILE_IO
  frameHeight = (S16) (gDynamicParams.videnc2DynamicParams.inputHeight);
#endif

  /*-----------------------------------------------------------------------*/
  /*  Initializing the output buffer descriptors.                          */
  /*-----------------------------------------------------------------------*/
  outputBufDesc.numBufs = 1; /*Only Bitstream buffer*/
  outputBufDesc.descs[0].buf =
    (XDAS_Int8 *)(( (unsigned int)BitstreamAddr + 15 ) & 0xFFFFFFF0)  ;
  outputBufDesc.descs[0].bufSize.bytes =
    status.videnc2Status.bufInfo.minOutBufSize[0].bytes;
  outputBufDesc.descs[0].memType =
    status.videnc2Status.bufInfo.outBufMemoryType[0];

  /*---------------------------------------------------------------------- */
  /*                   OPEN INPUT & OUTPUT FILEs                           */
  /*-----------------------------------------------------------------------*/

  strcpy(datasync_bitstreamname, gConfig.bitstreamname);
  strcat(datasync_bitstreamname, "_ds.m4v");
  strcpy(stitched_bitstreamname, gConfig.bitstreamname);
  strcat(stitched_bitstreamname, "_st.m4v");
  
  /*---------------------------------------------------------------------- */
  /*  open the input sequence file                                         */
  /*---------------------------------------------------------------------- */
  fp = MY_FOPEN(gConfig.inputFile, "rb");

  printf("Opening the YUV file = %s\n", gConfig.inputFile);
  if (fp == NULL) // || fp1 == NULL)
  {
    printf("cannot open the original sequence %s\n", gConfig.inputFile);
    exit(-1);
  }
  
#if defined (A9_HOST_FILE_IO) || defined (USE_DSS_SCRIPTING)
  end_frame = 5000;
#else
  fseek(fp, 0, SEEK_END);
  cnt = ftell(fp);
  end_frame = cnt / (frameWidth * frameHeight * 3 >> 1);
#endif

  printf("total %d frames in the current YUV sequence\n", end_frame);

  MY_FSEEK(fp, 0, SEEK_SET);

  if (end_frame > frameStart + (frameSkip + 1) * frameCoded)
  {
    end_frame = frameStart + (frameSkip + 1) * frameCoded;
  }

  /*---------------------------------------------------------------------- */
  /*  open bitstream file                                                  */
  /*---------------------------------------------------------------------- */
#ifdef RESET_TEST  
  strcpy((char *)outFile,(const char *)gConfig.bitstreamname);
#endif
  fpbits = MY_FOPEN(gConfig.bitstreamname, "wb");
  
  if (fpbits == NULL)
  {
    printf("cannot open the bitstream file %s\n", gConfig.bitstreamname);
    exit(-1);
  }

  /*---------------------------------------------------------------------- */
  /*  open reconstructed sequence file                                     */
  /*---------------------------------------------------------------------- */
  fout = MY_FOPEN(gConfig.reconFile, "wb");
  if (fout == NULL)
  {
    printf("cannot open the reconstructed sequence %s\n", gConfig.reconFile);
    exit(-1);
  }

#ifdef REF_COMPARE_TEST
  /*---------------------------------------------------------------------- */
  /*  open reference encoded bitstream file                                */
  /*---------------------------------------------------------------------- */
  frefbits = fopen(gConfig.refEncbitstreamname, "rb");
  if (frefbits == NULL)
  {
    printf("cannot open the ref encoded bitstream file %s\n",
      gConfig.refEncbitstreamname);
    exit(-1);
  }
#endif /*#ifdef REF_COMPARE_TEST*/

  /*-----------------------------------------------------------------------*/
  /*  XDM_SETPARAMS is called to pass the Dynamic Parameters to the codec. */
  /*  The Control function shall validate the parameters. If an error      */
  /*  is found then the Control function returns failure.                  */
  /*-----------------------------------------------------------------------*/
  retValue = MPEG4ENC_control(
    mpeg4encHandle,
    XDM_SETPARAMS,
    &gDynamicParams,
    &status
    );

  /*-----------------------------------------------------------------------*/
  /*  Check for errors during Control function call to XDM_SETPARAMS.      */
  /*-----------------------------------------------------------------------*/
  if(retValue == XDM_EFAIL)
  {
    MPEG4ENC_TI_Report_Error(status.videnc2Status.extendedError);
    printf("\nError in Dynamic Parameter Settings.");
    printf("\nCheck the Dynamic parameter settings.\n");

    /* Delete only if it is a FATAL Error else continue */
    if(XDM_ISFATALERROR(status.videnc2Status.extendedError))
    {
      goto DELETE_INSTANCE;
    }
  }
  
  /*-----------------------------------------------------------------------*/
  /*              GET MP4 ENCODER INPUT/OUTPUT BUFFER INFORMATION          */
  /*-----------------------------------------------------------------------*/

  /*-----------------------------------------------------------------------*/
  /* The No. of Input output buffers and their sizes are algorithm specific*/
  /* For a genric codec independent application like this, the buffer      */
  /* information for a codec can be obatined by making a call to the XDAIS */
  /* control fn-MP4VENC_control() with the command \b XDM_GETBUFINFO\b     */
  /* The function will return back the number of input and output arrays & */
  /* the size of each array.                                               */
  /*-----------------------------------------------------------------------*/
  /*-----------------------------------------------------------------------*/
  /*  The GETBUFINFO call gives information for number of input and        */
  /*  output buffers. For the first call however,                          */
  /*-----------------------------------------------------------------------*/

  MPEG4ENC_control(mpeg4encHandle,
    XDM_GETBUFINFO,
    &gDynamicParams,
    &status
    );

  /*-----------------------------------------------------------------------*/
  /* Initialize the input and output buffer properties as required by the  */
  /* algorithm based on info received by preceding GETBUFINFO call.        */
  /*-----------------------------------------------------------------------*/
  retValue = BUFFMGR_Init(MAX_BUFF_ELEMENTS,
    status.videnc2Status.bufInfo.minNumInBufs,
    status.videnc2Status.bufInfo.minInBufSize,
    gConfig.tilerSpace[0], gConfig.tilerSpace[1]
    );

  if(retValue)
  {
    printf ("\n[BUFFMGR]Could not get the memory for I/O buffers\n");
    exit(0);
  }

  /*-----------------------------------------------------------------------*/
  /* The Encoder requires the I/O pointers to be 32-bit aligned.           */
  /*-----------------------------------------------------------------------*/
  outputBufDesc.descs[0].buf =
    (XDAS_Int8 *)(((Uint32)outputBufDesc.descs[0].buf + 31)  & (~31));

  frame_nr = 0;
  
  /*------------------------------------------------------------------------*/
  /*  The GETSTATUS call here is specifically done to get information       */
  /*  about the configured debug trace level for the codec and debug        */
  /*  trace dump addresses in SL2 & External memory                         */
  /*------------------------------------------------------------------------*/
  MPEG4ENC_control
  (
    mpeg4encHandle,
    XDM_GETSTATUS,
    (MPEG4ENC_DynamicParams *)&gDynamicParams,
    (MPEG4ENC_Status *)&status
  );
  /*------------------------------------------------------------------------*/
  /*  Print information about the codec's debug information based on the    */
  /*  result of the GETSTATUS query                                         */
  /*------------------------------------------------------------------------*/
  if(status.debugTraceLevel)
  {
    /*----------------------------------------------------------------------*/
    /* If debug trace is enabled then,                                      */
    /*  - dump the extMemoryDebugTraceSize amount of bytes from             */
    /*    extMemoryDebugTraceAddr in the CCS memory window                  */
    /*  - Save it as *.dat file                                             */
    /*  - Use the PC based utility to generate the log file                 */
    /*----------------------------------------------------------------------*/ 
    printf("Debug Trace ON, Level %d\n", status.debugTraceLevel);
    printf("Debug trace history configured for previous %d frames\n",
                                                      status.lastNFramesToLog);
    printf("Debug trace dump address in External memory: 0x%x\n",
                                            status.extMemoryDebugTraceAddr);
    printf("Debug trace dump size in External memory: %d bytes\n",
                                              status.extMemoryDebugTraceSize);
  }
  else
    printf("Debug Trace OFF\n", status.debugTraceLevel);

#ifdef REF_COMPARE_TEST
  printf("******************************************************************************");
  printf("**************************");
  printf("\nFrame#    Frame Type      FrameBits    Across Process(MHz)  Frame Based Mbps");
  printf("    Frame Compliance Test");
  printf("\n*****************************************************************************");
  printf("**************************\n");
#else
  printf("******************************************************************************");
  printf("\nFrame#    Frame Type      FrameBits    Across Process(MHz)  Frame Based Mbps");
  printf("\n*****************************************************************************\n");
#endif
  
  if(gConfig.datasynchFilename[0] != '\0')
  {
  	FILE *fdatasynch;
  	fdatasynch = fopen(gConfig.datasynchFilename, "r");
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[0]);
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[1]);
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[2]);
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[3]);
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[4]);
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[5]);
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[6]);
  	fscanf(fdatasynch, "%d", &datasynchNumBlocks[7]);
  	fclose(fdatasynch);
  }
  
  /*************************************************************************/
  /* Fix for SDOCM00077402                                                 */
  /* Data sync output file is not created if data sync is enabled for      */
  /* certain test cases in case of MPEG-4.                                 */
  /*************************************************************************/
  if(gParams.videnc2Params.outputDataMode != IVIDEO_ENTIREFRAME)
  {
  	if((fout2 = MY_FOPEN(datasync_bitstreamname, "wb")) == NULL)
  	{
  	  printf("Error: Opening data sync file.\n");
  	  goto FRAME_ENCODE_FAIL;
  	}
  }

  /*-----------------------------------------------------------------------*/
  /*  main loop: encode sequence frame by frame                            */
  /*-----------------------------------------------------------------------*/
  do
  {
     gInitialTime = 0;
     baseAddrGetBuffer = (S32*) BitstreamAddr;
     currFrame = frameStart + (frameSkip + 1) * frame_nr;

     /*-------------------------------------------------------------------*/
     /*  Call the getfreebuffer() function of buffermanager to get the    */
     /*  buffer element to be supplied to algorithm for the next encode   */
     /*  call.                                                            */
     /*-------------------------------------------------------------------*/
     buffEle = BUFFMGR_GetFreeBuffer();
     if (buffEle == NULL)
     {
       printf("\nNo free buffer available from BUFFER MANAGER\n");
       exit(0);
     }

     /*-------------------------------------------------------------------*/
     /*  Initialize the input buffer properties as required by algorithm  */
     /*  based on info received by preceding GETBUFINFO call.             */
     /*-------------------------------------------------------------------*/
     inputBufDesc.numPlanes = status.videnc2Status.bufInfo.minNumInBufs;

     /*-------------------------------------------------------------------*/
     /*  Copy the input buffer size values and pointers from the buffer   */
     /*  element returned by getfreebuffer() function.                    */
     /*-------------------------------------------------------------------*/
     inputBufDesc.planeDesc[0].bufSize.bytes = 
       (XDAS_Int32) buffEle->bufSize[0];
     inputBufDesc.planeDesc[0].buf =
       (XDAS_Int8 *)(((XDAS_UInt32)buffEle->buf[0] + 31) & (~31));

     inputBufDesc.planeDesc[1].bufSize.bytes = 
       (XDAS_Int32) buffEle->bufSize[1];
     inputBufDesc.planeDesc[1].buf =
       (XDAS_Int8 *)(((XDAS_UInt32)buffEle->buf[1] + 31) & (~31));

     inputBufDesc.planeDesc[0].memType = XDM_MEMTYPE_ROW;
     inputBufDesc.planeDesc[1].memType = XDM_MEMTYPE_ROW;

     inputBufDesc.planeDesc[0].memType = gConfig.tilerSpace[0];
     inputBufDesc.planeDesc[1].memType = gConfig.tilerSpace[1];


     inputBufDesc.imageRegion.topLeft.x = 0;
     inputBufDesc.imageRegion.topLeft.y = 0;
     inputBufDesc.imageRegion.bottomRight.x = gDynamicParams.videnc2DynamicParams.inputWidth;
     inputBufDesc.imageRegion.bottomRight.y = gDynamicParams.videnc2DynamicParams.inputHeight;
     inputBufDesc.activeFrameRegion.topLeft.x = 0;
     inputBufDesc.activeFrameRegion.topLeft.y = 0;
     inputBufDesc.activeFrameRegion.bottomRight.x = gDynamicParams.videnc2DynamicParams.inputWidth;
     inputBufDesc.activeFrameRegion.bottomRight.y = gDynamicParams.videnc2DynamicParams.inputHeight;
     inputBufDesc.imagePitch[0] = framePitch;
     inputBufDesc.imagePitch[1] = framePitch;
     inputBufDesc.contentType = IVIDEO_PROGRESSIVE;
     inputBufDesc.chromaFormat = XDM_YUV_420SP;

     /*-------------------------------------------------------------------*/
     /*  Initializing the output buffer descriptors.                      */
     /*-------------------------------------------------------------------*/
     outputBufDesc.numBufs = 1;
     outputBufDesc.descs[0].buf =
       (XDAS_Int8 *)(((unsigned int)BitstreamAddr + 15 ) & 0xFFFFFFF0);
     
     outputBufDesc.descs[0].bufSize.bytes =
       status.videnc2Status.bufInfo.minOutBufSize[0].bytes;
     
     /*----------------------------------------------------------------------*/
     /* If ignoreOutBufSizeFlag is enabled, then give minimum buffer to the  */
     /* encoder and let the encoder call the getBuffer() callback function   */
     /* for getting more buffer from the application after exhausting that.  */
     /*----------------------------------------------------------------------*/
     if((gParams.sliceCodingParams.sliceMode == IMPEG4_SLICEMODE_BITS) &&
       (gParams.videnc2Params.outputDataMode == IVIDEO_SLICEMODE))
     {
       if(gDynamicParams.videnc2DynamicParams.ignoreOutbufSizeFlag)
       {
         outputBufDesc.descs[0].bufSize.bytes =
           gParams.sliceCodingParams.sliceUnitSize >> 3;
       }
     }
     else
     {
       if(gDynamicParams.videnc2DynamicParams.ignoreOutbufSizeFlag)
       {
         outputBufDesc.descs[0].bufSize.bytes = INIT_BUFFER_SIZE;
       }
     }
     outputBufDesc.descs[0].memType =
       status.videnc2Status.bufInfo.outBufMemoryType[0];

     

    /*-----------------------------------------------------------------------*/
    /* Set proper buffer addresses for MV & SAD data                         */
    /*-----------------------------------------------------------------------*/
    if(gParams.enableAnalyticinfo == IVIDEO_METADATAPLANE_MBINFO &&
       gParams.videnc2Params.encodingPreset == XDM_USER_DEFINED) 
    {
      outputBufDesc.descs[outputBufDesc.numBufs].buf = 
                                           (XDAS_Int8*)gAnalyticInfoOutputData;
      outputBufDesc.descs[outputBufDesc.numBufs].memType = 
          status.videnc2Status.bufInfo.outBufMemoryType[outputBufDesc.numBufs];
      outputBufDesc.descs[outputBufDesc.numBufs].bufSize.bytes = status. 
              videnc2Status.bufInfo.minOutBufSize[outputBufDesc.numBufs].bytes;
      outputBufDesc.numBufs++;
    }


     /*-------------------------------------------------------------------*/
     /*  Initialize the input ID in input arguments to the bufferid of the*/
     /*  buffer element returned from getfreebuffer() function.           */
     /*-------------------------------------------------------------------*/
     inArgs.videnc2InArgs.inputID  = buffEle->bufId;
     
    if((!(frame_nr % (XDAS_UInt32)gConfig.forceSKIPPeriod)) && (frame_nr))
    {
      printf("\nRequested a force SKIP at Frame #%-5d ...\n",(frame_nr));
      inArgs.videnc2InArgs.control            = IVIDENC2_CTRL_FORCESKIP; 
    }
    else
    {
      inArgs.videnc2InArgs.control            = IVIDENC2_CTRL_DEFAULT;
    }

     if (currFrame >= end_frame)
         last_frame = 1;
     else
         last_frame = 0;

     if (last_frame == 0)
     {
        /*-------------------------------------------------------------------*/
        /*  Read input YUV from the file.                                    */
        /*-------------------------------------------------------------------*/
        ReadInYUV420Picture(
          &inputBufDesc,
          currFrame,
          end_frame,
          frameSkip,
          frameWidth,
          captureWidth,
          frameCaptureHeight,
          fp,
          gConfig.tilerSpace[0], gConfig.tilerSpace[1]
          );

        /*-------------------------------------------------------------------*/
        /*  Used for DataSync Slice Mode Operation. Reset the Indices for h  */
        /*  each frame.                                                      */
        /*-------------------------------------------------------------------*/
         gSlicesSizeIndex = 0;
         gSlicesBaseIndex = 0;

#ifdef TEST_FORCE_I_FRAME
         if(currFrame == 3)
         {
           gDynamicParams.videnc2DynamicParams.forceFrame = IVIDEO_I_FRAME;
           /*----------------------------------------------------------------*/
           /*XDM_SETPARAMS is called to pass the Dynamic Params to the codec */
           /*The Control function shall validate the parameters. If an error */
           /*is found then the Control function returns failure.             */
           /*----------------------------------------------------------------*/
           retValue = MPEG4ENC_control(
             mpeg4encHandle,
             XDM_SETPARAMS,
             &gDynamicParams,
             &status
             );
          }
#endif /* TEST_FORCE_I_FRAME */

#if 0 // Karthick
         if(currFrame == 2)
         {
         	
         	//gDynamicParams.videnc2DynamicParams.targetFrameRate = 5000;
         	//gDynamicParams.videnc2DynamicParams.intraFrameInterval = 15;
         	gDynamicParams.videnc2DynamicParams.generateHeader = 0;
         	
         	retValue = MPEG4ENC_control(
             mpeg4encHandle,
             XDM_SETPARAMS,
             &gDynamicParams,
             &status
             );
         	inArgs.videnc2InArgs.control            = IVIDENC2_CTRL_FORCESKIP;
         	/*
         	retValue = MPEG4ENC_control(
             mpeg4encHandle,
             XDM_SETPARAMS,
             &gDynamicParams,
             &status
             );*/
         }
         if(currFrame == 3)
         {
         	inArgs.videnc2InArgs.control            = IVIDENC2_CTRL_DEFAULT;
         }
#endif

#if 0 // Karthick
       if(currFrame == 0)
       {
       	 if(gDynamicParams.videnc2DynamicParams.generateHeader == 1)
       	 {
       	 	retValue = MPEG4ENC_encodeFrame(
              mpeg4encHandle,
              &inputBufDesc,
              &outputBufDesc,
              &inArgs,
              &outArgs
              );
            
            gDynamicParams.videnc2DynamicParams.generateHeader  = 0;
            retValue = MPEG4ENC_control(
             mpeg4encHandle,
             XDM_SETPARAMS,
             &gDynamicParams,
             &status
             );
       	 }
       }
#endif

#undef CONTROL_API_CHECK
#ifdef CONTROL_API_CHECK
  if(currFrame == 2)
  {
    if(gConfig.controlFilename[0] != '\0')
    {
  	  FILE *fctrl;
  	  fctrl = fopen(gConfig.controlFilename, "r");
  	  readparamfile(fctrl);
  	  fclose(fctrl);
    }

    /*-----------------------------------------------------------------------*/
    /*  XDM_SETPARAMS is called to pass the Dynamic Parameters to the codec. */
    /*  The Control function shall validate the parameters. If an error      */
    /*  is found then the Control function returns failure.                  */
    /*-----------------------------------------------------------------------*/
    retValue = MPEG4ENC_control(
      mpeg4encHandle,
      XDM_SETPARAMS,
      &gDynamicParams,
      &status
      );

    /*-----------------------------------------------------------------------*/
    /*  Check for errors during Control function call to XDM_SETPARAMS.      */
    /*-----------------------------------------------------------------------*/
    if(retValue == XDM_EFAIL)
    {
      MPEG4ENC_TI_Report_Error(status.videnc2Status.extendedError);
      printf("\nError in Dynamic Parameter Settings.");
      printf("\nCheck the Dynamic parameter settings.\n");

      /* Delete only if it is a FATAL Error else continue */
      if(XDM_ISFATALERROR(status.videnc2Status.extendedError))
      {
        goto DELETE_INSTANCE;
      }
    }
  }
#endif

#define RUNTIMECHANGE_CHECK
#ifdef RUNTIMECHANGE_CHECK
  if(currFrame == gConfig.frameNumber)
  {
    if(gConfig.runTimeChangeFilename[0] != '\0')
    {
  	  FILE *fctrl;
  	  fctrl = fopen(gConfig.runTimeChangeFilename, "r");
  	  readparamfile(fctrl);
  	  fclose(fctrl);
    }
    /* Update in the dynamic parameters */
    memcpy (&((gDynamicParams).rateControlParams), &((gParams).rateControlParams),
      sizeof (IMPEG4ENC_RateControlParams));
    memcpy (&((gDynamicParams).interCodingParams), &((gParams).interCodingParams),
      sizeof (IMPEG4ENC_InterCodingParams));
    memcpy (&((gDynamicParams).sliceCodingParams), &((gParams).sliceCodingParams),
      sizeof (IMPEG4ENC_sliceCodingParams));

    /*-----------------------------------------------------------------------*/
    /*  XDM_SETPARAMS is called to pass the Dynamic Parameters to the codec. */
    /*  The Control function shall validate the parameters. If an error      */
    /*  is found then the Control function returns failure.                  */
    /*-----------------------------------------------------------------------*/
    retValue = MPEG4ENC_control(
      mpeg4encHandle,
      XDM_SETPARAMS,
      &gDynamicParams,
      &status
      );
      
    inputBufDesc.imageRegion.bottomRight.x = gDynamicParams.videnc2DynamicParams.inputWidth;
    inputBufDesc.imageRegion.bottomRight.y = gDynamicParams.videnc2DynamicParams.inputHeight;
    inputBufDesc.activeFrameRegion.bottomRight.x = gDynamicParams.videnc2DynamicParams.inputWidth;
    inputBufDesc.activeFrameRegion.bottomRight.y = gDynamicParams.videnc2DynamicParams.inputHeight;
      

    /*-----------------------------------------------------------------------*/
    /*  Check for errors during Control function call to XDM_SETPARAMS.      */
    /*-----------------------------------------------------------------------*/
    if(retValue == XDM_EFAIL)
    {
      MPEG4ENC_TI_Report_Error(status.videnc2Status.extendedError);
      printf("\nError in Dynamic Parameter Settings.");
      printf("\nCheck the Dynamic parameter settings.\n");

      /* Delete only if it is a FATAL Error else continue */
      if(XDM_ISFATALERROR(status.videnc2Status.extendedError))
      {
        goto DELETE_INSTANCE;
      }
    }
  }
#endif

         MPEG4ENC_TI_Capture_time(0);

       /*--------------------------------------------------------------------*/
       /*  Start the encode process for one frame/field by calling the       */
       /*  MPEG4ENC_encodeFrame() function.                                  */
       /*--------------------------------------------------------------------*/
       retValue = MPEG4ENC_encodeFrame(
         mpeg4encHandle,
         &inputBufDesc,
         &outputBufDesc,
         &inArgs,
         &outArgs
         );

       MPEG4ENC_TI_Capture_time(1);

       /*--------------------------------------------------------------------*/
       /*  Check for failure message in the return value.                    */
       /*--------------------------------------------------------------------*/
       if(retValue != IVIDENC2_EOK)
       {
           MPEG4ENC_TI_Report_Error(outArgs.videnc2OutArgs.extendedError);
        /* Frame encoded unsuccessfully */
          printf("Frame encoding failed.\n");
          goto FRAME_ENCODE_FAIL;
          // exit(0);
       }

       /*--------------------------------------------------------------------*/
       /*  Always release buffers - which are released from the algorithm    */
       /*  side   -back to the buffer manager. The freebufID array of outargs*/
       /*  contains the sequence of bufferIds which need to be freed. This   */
       /*  gets populated by the algorithm. The following function will do   */
       /*  the job of freeing up the buffers.                                */
       /*--------------------------------------------------------------------*/
       BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.videnc2OutArgs.freeBufID);

       if (outArgs.videnc2OutArgs.inputFrameSkip == IVIDEO_FRAME_SKIPPED)
       {
          /*-----------------------------------------------------------------*/
          /* To be added: Need to output statistics of skipped frames        */
          /*-----------------------------------------------------------------*/
          if(!outArgs.videnc2OutArgs.bytesGenerated)
          {
            printf("%4d %18s %8d\n",
              frame_nr, pFrameType[2],
              outArgs.videnc2OutArgs.bytesGenerated * 8);
            frame_nr++;
            continue;
          }
       }

       /*--------------------------------------------------------------------*/
       /* Write out bits generated for picture                               */
       /*--------------------------------------------------------------------*/
       WriteOutBitstream(
         (U32) outputBufDesc.descs[0].buf,
         outArgs.videnc2OutArgs.bytesGenerated,
         fpbits
         );
#ifndef A9_HOST_FILE_IO      
     MY_FFLUSH(fpbits);
#endif


#if 0  // Karthick
     if(gDynamicParams.videnc2DynamicParams.generateHeader == 1)
     {
     	retValue = MPEG4ENC_encodeFrame(
          mpeg4encHandle,
          &inputBufDesc,
          &outputBufDesc,
          &inArgs,
          &outArgs
          );
            
        gDynamicParams.videnc2DynamicParams.generateHeader  = 0;
        retValue = MPEG4ENC_control(
          mpeg4encHandle,
          XDM_SETPARAMS,
          &gDynamicParams,
          &status
          );
      }
#endif
     /*---------------------------------------------------------------------*/
     /* Stitching routine for H263+H241+Datasync                            */
     /*---------------------------------------------------------------------*/
     if((gParams.sliceCodingParams.sliceMode == IMPEG4_SLICEMODE_BITS) &&
       (gParams.videnc2Params.outputDataMode == IVIDEO_SLICEMODE) &&
       gParams.useShortVideoHeader)
     {
       int bytesEncoded;
       MY_FILE *fDataSyncH263;
       char mode[] = "wb";
       
       if(frame_nr == 0)
         mode[0] = 'w';
       else
         mode[0] = 'a';
       if((fDataSyncH263 = MY_FOPEN(stitched_bitstreamname, mode)) ==
           NULL)
       {
          printf("Error: Opening stitching m4v file.\n");
       }
       
       bytesEncoded = StitchAllPackets();
       if(fDataSyncH263 != NULL)
       {
          MY_FWRITE(dataSyncOutputBuffer, 1, bytesEncoded, fDataSyncH263);
          MY_FCLOSE(fDataSyncH263);
       }
     }

     /*---------------------------------------------------------------------*/
     /* Write out Analytic Info dumped by codec                             */
     /*---------------------------------------------------------------------*/
#ifdef ENABLE_DUMP_ANALYTIC_INOF     
     if(gParams.enableAnalyticinfo == IVIDEO_METADATAPLANE_MBINFO)
     {
       MY_FILE *fAnalytic;
       if(frame_nr == 0)
         fAnalytic = MY_FOPEN("AnalyticInfo.txt", "wb");
       else
         fAnalytic = MY_FOPEN("AnalyticInfo.txt", "ab");

       MY_FWRITE((char *)gAnalyticInfoOutputData, 1, 
         outputBufDesc.descs[1].bufSize.bytes, fAnalytic);

       MY_FCLOSE(fAnalytic);
     }
#endif     

     /* Encoder report - frame output */
     printf("%4d %18s %8d",
       frame_nr, pFrameType[outArgs.videnc2OutArgs.encodedFrameType],
       outArgs.videnc2OutArgs.bytesGenerated * 8);

     MPEG4ENC_TI_PrintProfileInfo(PRINT_EACH_FRAME, 
       outArgs.videnc2OutArgs.bytesGenerated, frame_nr);


#ifdef REF_COMPARE_TEST
     /*---------------------------------------------------------------------*/
     /*  Compare the output frames with the Reference File.                 */
     /*---------------------------------------------------------------------*/
     retValue = TestApp_CompareOutputData(
       frefbits,
       &outputBufDesc,
       &outArgs
       );
     if (retValue != IVIDENC2_EOK)
     {
       /*-------------------------------------------------------------------*/
       /* Test Compliance Failed... Breaking...                             */
       /*-------------------------------------------------------------------*/
       fprintf(stdout, "                FAIL\n.");
       break;
     }
     else
     {
       fprintf(stdout, "                PASS.");
     }

#endif     
     printf("\n");

     /* ------------------------------------------------------------------ */
     /* In IVAHD encoder only UV interleaved mode is supported for         */
     /* reconstructed. This is due to a limitation in VDMA                 */
     /* (don't support UV interleaving for SL2=>DDR)                       */
     /* -------------------------------------------------------------------*/
#if ENABLE_RECON_DUMP && !defined(A9_HOST_FILE_IO)

     WriteOutFrameYUV420nv12(
       &outArgs,
       fout,
       frameWidth,
       frameHeight,
       0 //refWidth
       );

     // MY_FFLUSH(fout);
#endif


     frame_nr++;

     }
#ifdef RESET_TEST
     /*------------------------------------------------------------------------*/
    /* whether XDM reset test is required, when it is required                */
    /*  - Check whether total number of frames to be encoded is completed     */
    /*  - when completed reopen the output and log files Make get encoder     */
    /*    state to reset position and continue loop                           */
    /*------------------------------------------------------------------------*/
    if(uiReset_Test)
    {
      if(last_frame == 1)
      {
        /*--------------------------------------------------------------------*/
        /* Reset then encoder with XDM_RESET controll command                 */
        /*--------------------------------------------------------------------*/
        MPEG4ENC_control(mpeg4encHandle,XDM_RESET,
            (MPEG4ENC_DynamicParams *)&gDynamicParams,(MPEG4ENC_Status *)&status);
        /*--------------------------------------------------------------------*/
        /* Close the output file open new files appending ".reset"            */
        /*--------------------------------------------------------------------*/
        MY_FCLOSE(fpbits);
        strcpy((char *)outFile,(const char *)gConfig.bitstreamname);
        strcat((char *)outFile, "_reset.m4v");
        fpbits = fopen((const char *)outFile, "wb");
        if(fpbits == NULL)
        {
          printf("\n Unable to open Output Bitstream File %s\n", outFile);
          return(XDM_EFAIL);
        }
        printf("Testing: %s\nwith config %s \n",gConfig.inputFile,
                                                            gConfig.bitstreamname);
        /*--------------------------------------------------------------------*/
        /* Make number of frames encoded to zero so that input YUV reading    */
        /* starts from the start and encodes same number of frames            */
        /*--------------------------------------------------------------------*/
        frame_nr = 0;
        /*--------------------------------------------------------------------*/
        /* Set the reset flag to zero to come of dead lock situation          */
        /*--------------------------------------------------------------------*/
        uiReset_Test = 0;
        last_frame   = 0;
      }
    }
#endif
  } while (!last_frame);

  /* Free input frame buffer memory*/
  /* free(inBufs.bufDesc[0].buf);*/
FRAME_ENCODE_FAIL:


#ifdef REF_COMPARE_TEST
  fclose(frefbits);
#endif

  MPEG4ENC_TI_PrintProfileInfo(PRINT_END_FRAME,
    outArgs.videnc2OutArgs.bytesGenerated, frame_nr);

  printf (" Done with the running of the encoder\n");

DELETE_INSTANCE:

  if(fp)    MY_FCLOSE(fp);
  if(fout)  MY_FCLOSE(fout);
  if(fpbits)MY_FCLOSE(fpbits);
  if(gParams.videnc2Params.outputDataMode != IVIDEO_ENTIREFRAME)
  {
    if(fout2)MY_FCLOSE(fout2);
  }


  /*----------------------------------------------------------------------*/
  /*  Delete MPEG4 context                                                */
  /*----------------------------------------------------------------------*/

  RMAN_FreeResources((IALG_Handle)mpeg4encHandle,
    gConfig.tilerSpace[0],gConfig.tilerSpace[1] );

  if (mpeg4encHandle)
    MPEG4ENC_delete(mpeg4encHandle);
  }
  fclose(ftestcases);
  exit(1);
  return 1;
}



#if ENABLE_RECON_DUMP
/**
********************************************************************************
 *  @func     WriteOutFrameYUV420nv12
 *  @brief    Routine for writing reconstructed YUV sequence
 *
 *  @param[in]  outArgs     : Pointer to the MPEG4ENC_OutArgs structure
 *
 *  @param[in]  fout        : Pointer to recon file
 *
 *  @param[in]  frameWidth  : output frame width
 *
 *  @param[in]  frameHeight : output frame height
 *
 *  @param[in]  refWidth    : reference frame width
 *
 *  @return None
********************************************************************************
*/
Void WriteOutFrameYUV420nv12(
  MPEG4ENC_OutArgs * outArgs,
  MY_FILE *fout,
  S16 frameWidth,
  S16 frameHeight,
  S16 refWidth
)
{
  U32 width;       /* horizontal frame size */
  U32 height;      /* vertical frame size */
  U32 ref_width;   /* horizontal frame size after padding */
  U08 *frame;      /* field[0] = frame luma  or top-field address */
  U08 *frame_uv;   /* UV interleaved adreess */
  S16 i;
  /*U32 width_uv;*/
  U32 height_uv, ref_width_uv;
  U08 *srcY, *srcUV;
#ifdef A9_HOST_FILE_IO
  U08 localBuf[2048];
#endif

  width = frameWidth;
  height = frameHeight;
  height = ((height + 15) >> 4 ) << 4;

  ref_width = outArgs->videnc2OutArgs.reconBufs.imagePitch[0];

  /*width_uv = (S16) ((width + 1) >> 1);*/
  height_uv = (S16) ((height + 1) >> 1);

  frame = (U08 *)(outArgs->videnc2OutArgs.reconBufs.planeDesc[0].buf +
    outArgs->videnc2OutArgs.reconBufs.imageRegion.topLeft.y * (ref_width) +
    outArgs->videnc2OutArgs.reconBufs.imageRegion.topLeft.x);
  frame_uv = (U08 *)((U08 *)outArgs->videnc2OutArgs.reconBufs.planeDesc[1].buf +
    outArgs->videnc2OutArgs.reconBufs.imageRegion.topLeft.y * (ref_width) +
    outArgs->videnc2OutArgs.reconBufs.imageRegion.topLeft.x);

  ref_width_uv = ref_width;
  /*-------------*/
  /* luma output */
  /*-------------*/
  srcY = (U08 *) (frame);
  for (i = 0; i < height; i++) {
#ifdef A9_HOST_FILE_IO
  	memcpy(localBuf, srcY, width);
#endif
    MY_FWRITE((char*)localBuf, 1, width, fout);
    srcY += ref_width;
  }

  /*-------------*/
  /* UV output   */
  /*-------------*/
  srcUV = (U08 *) (frame_uv);
  for (i = 0; i < height_uv; i++) {
#ifdef A9_HOST_FILE_IO
    memcpy(localBuf, srcUV, width);
#endif
    MY_FWRITE((char*)localBuf, 1, width, fout);
    srcUV += ref_width_uv;
  }

  return;
}
#endif





