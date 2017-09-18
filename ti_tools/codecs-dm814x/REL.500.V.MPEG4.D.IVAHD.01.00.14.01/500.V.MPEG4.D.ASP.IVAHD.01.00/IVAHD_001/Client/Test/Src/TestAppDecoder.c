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
* @version 0.1 (Sep 2011)  : move the tilerInit function to support allocation
*                            of the tiler memory in case of optinal deblock
*                            on.
*
*****************************************************************************
*/
/******************************************************************************
*  INCLUDE FILES
******************************************************************************/
#include <xdc/std.h>
#include "TestAppDecoder.h"
#include "TestApp_rmanConfig.h"
#include "tilerBuf.h"
#include "mpeg4vdec.h"


/******************************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
******************************************************************************/
#ifdef HOST_ARM9
void dummy(void);
#endif

XDAS_UInt32 bufferFreed;
XDAS_UInt32 bufferDisplayed;

FILE *fCycles;
#ifdef TEST_SEEK_OP
/**
 * Global variables needed for SEEK operation
*/
XDAS_UInt32 seekIdxCntr, SeekOpStart, SeekBackwd;
#endif
/*----------------------------------------------------------------------------*/
/*  Input buffer allocation                                                   */
/*----------------------------------------------------------------------------*/
#pragma DATA_SECTION(inputData, ".input");
XDAS_Int8  inputData[INPUT_BUFFER_SIZE];

/**
 *  Macro to indicate the buffer size needed to hold the version of codec
 */
#define VERSION_BUF_SIZE 100
#define PADDING_OFFSET 32
/*--------------------------------------------------------------------------*/
/*  Declaring Algorithm specific handle                                     */
/*--------------------------------------------------------------------------*/
IMPEG4VDEC_Handle mpeg4decHandle;
IMPEG4VDEC_Status           status;
/*----------------------------------------------------------------------------*/
/*  Global Structure variables for static and dynamic params.                 */
/*----------------------------------------------------------------------------*/
IMPEG4VDEC_Params          params;
IMPEG4VDEC_DynamicParams   dynamicParams;

/*----------------------------------------------------------------------------*/
/*  Global Structure variables for application control params                 */
/*----------------------------------------------------------------------------*/
sAppControlParams        appControlPrms;

/*----------------------------------------------------------------------------*/
/*  Global Structure variables for application file pointers                  */
/*----------------------------------------------------------------------------*/
sAppFilePointers         filePtrs;

/*----------------------------------------------------------------------------*/
/*  Global Structure variables for application file pointers                  */
/*----------------------------------------------------------------------------*/
sProfileParams           profilePrms;
sTilerParams             tilerParams;

/*----------------------------------------------------------------------------*/
/*  Global flag for non-blocking call.                                        */
/*----------------------------------------------------------------------------*/
XDAS_Int32          fatalErrorDetected;
XDAS_Int32          nonFatalErrorDetected;
XDAS_UInt32         frameCount;
XDAS_Int32          picture_count;
XDAS_UInt32         displayCount;
/*----------------------------------------------------------------------------*/
/* Scratch buffer for interleaving the data                                   */
/*----------------------------------------------------------------------------*/
XDAS_Int32          fieldBuf;
/*--------------------------------------------------------------------------*/
/*  Handle to a buffer element allocated by the buffer manager module       */
/*--------------------------------------------------------------------------*/
BUFFMGR_buffEleHandle buffEle;

/*--------------------------------------------------------------------------*/
/*  Counter for tracking the number of testvectors                          */
/*--------------------------------------------------------------------------*/
XDAS_Int32    countConfigSet;
XDAS_UInt8    appIvahdId;
/*--------------------------------------------------------------------------*/
/* BytesConsumed : Tracks Bytes consumed for stream                         */
/* streamEnd : Flag indicates that the stream has ended                     */
/* decodeMode : Flag indicating the decode mode (decodeHeader or decodeAU)  */
/*--------------------------------------------------------------------------*/
XDAS_UInt32 bytesConsumed;
XDAS_UInt32 streamEnd;
XDAS_UInt32 decodeMode;
/*--------------------------------------------------------------------------*/
/* Structure for Input/Ouput Arguments for the process call                 */
/*--------------------------------------------------------------------------*/
IMPEG4VDEC_InArgs    inArgs;
IMPEG4VDEC_OutArgs   outArgs;
/*--------------------------------------------------------------------------*/
/* File pointer for the file that logs status of each config.               */
/* And variables needed for staus logging                                   */
/*--------------------------------------------------------------------------*/
FILE *fLogFile;
XDAS_Int8  configStatus;
/*--------------------------------------------------------------------------*/
/* Flag to indicate if algorithm creation is done or not                    */
/*--------------------------------------------------------------------------*/
XDAS_Int8  algCreated;

/*--------------------------------------------------------------------------*/
/* Flag to check that codec has got input data in header mode               */
/*--------------------------------------------------------------------------*/
XDAS_Int8  ParsedHeader;


extern IRES_HDVICP2_Obj  IRES_HDVICP2_handle;
extern sTokenMapping     sTokenMap[] ;
extern FILE *fProfileNums;
extern XDAS_UInt32   ivaCodeLoadDone;
extern XDAS_UInt32  frameSizeOffset;


static sEnumToStringMapping gContentStrings[4] =
{
  (XDAS_Int8 *)"IVIDEO_PROGRESSIVE, \0",
  (XDAS_Int8 *)"IVIDEO_INTERLACED, \0",
  (XDAS_Int8 *)"IVIDEO_INTERLACED, \0",
  (XDAS_Int8 *)"IVIDEO_INTERLACED, \0"
};

static sEnumToStringMapping gFrameTypeStrings[3] =
{
  (XDAS_Int8 *)"IVIDEO_I_FRAME, \0",
  (XDAS_Int8 *)"IVIDEO_P_FRAME, \0",
  (XDAS_Int8 *)"IVIDEO_B_FRAME, \0"
};

/**
********************************************************************************
 *  @fn     main
 *  @brief  Sample main program implementation which uses MPEG4 Decoder APIs
 *          to create decoder instance and decode multiple input files.
 *
 *  @param[in] void          : None
 *
 *  @return XDM_EFAIL/APP_EFAIL/APP_EXIT
********************************************************************************
*/
XDAS_Int32 main(XDAS_Int32 argc, XDAS_Int8 * argv[])
{
  /*--------------------------------------------------------------------------*/
  /*  first_time: Flag used for reinitializing the buffers after the first    */
  /*              decode call. This need to be reset after buffer allocations */
  /*              are done based on the actual height,width are done.         */
  /* CodecExtError, CodecExtErrorFlush : Local variable to capture errors from*/
  /*                                     decode call.                         */
  /* testCompliance: If set to 1 indicates that the output need to be compared*/
  /*                 against the reference provided and pass/fail need to be  */
  /*                 given out. Otherwise (=0), the output is dumped to file. */
  /* numBuffersAllocated : The number of buffers needed for reference frame   */
  /*                       storage.                                           */
  /*--------------------------------------------------------------------------*/
  XDAS_Int32     first_time;
  XDAS_Int32    CodecExtError, CodecExtErrorFlush;
  XDAS_Int8     testCompliance,crcEnable;

  XDAS_Int32    numBuffersAllocated = 0;
  XDAS_Int32    count =0;
  XDAS_Int32    ReqWidth;
  XDAS_Int32   ReqHeight;
  /*--------------------------------------------------------------------------*/
  /*  File pointers for input/output/reference info needed.                   */
  /*--------------------------------------------------------------------------*/
  FILE *fsizeFile;
  FILE *fConfigFile, *frefFile;
  FILE *finFile, *foutFile;


#ifdef USE_HOST_FILE_IO
  /*--------------------------------------------------------------------------*/
  /*  Initializing the path of file containing the test vectors to test.      */
  /*--------------------------------------------------------------------------*/
  XDAS_Int8 fname[]  = "Testvecs.cfg";
  XDAS_Int8 fStatusFileName[] = "Log.txt";
#else
  /*--------------------------------------------------------------------------*/
  /*  Initializing the path of file containing the test vectors to test.      */
  /*--------------------------------------------------------------------------*/
  XDAS_Int8 fname[]  = "..\\..\\..\\Test\\TestVecs\\Config\\Testvecs.cfg";
  XDAS_Int8 fStatusFileName[] = "..\\..\\..\\Test\\TestVecs\\Output\\Log.txt";
#endif
  /*--------------------------------------------------------------------------*/
  /*  One Local variables to store the result of a process or control call,   */
  /*  and also for storing of comparison of results                           */
  /*--------------------------------------------------------------------------*/
  XDAS_Int8    ret_val;
  XDAS_Int8    testVal, chkVal;

  /*--------------------------------------------------------------------------*/
  /*  Base Class Structures for status, input and output arguments.           */
  /*--------------------------------------------------------------------------*/
  IVIDDEC3_Status           *BaseStatus = &status.viddec3Status;
  /*--------------------------------------------------------------------------*/
  /*  Display Buffer descriptor can be Array type or pointer type. We use     */
  /*  below array allocation when it is configured as embedded type.          */
  /*--------------------------------------------------------------------------*/
  IVIDEO2_BufDesc DisplayBuffers[1];
  IVIDEO2_BufDesc *displayBufs;

  /*--------------------------------------------------------------------------*/
  /*  Input/Output Buffer Descriptors                                         */
  /*--------------------------------------------------------------------------*/
  XDM2_BufDesc  inputBufDesc;
  XDM2_BufDesc  outputBufDesc;

  /*--------------------------------------------------------------------------*/
  /*  Input bitstream Buffer Management specific variables  like Bit stream   */
  /*  input file position for file read, variable to keep track of total bytes*/
  /*  consumed so far after every frame etc..                                 */
  /*--------------------------------------------------------------------------*/
  XDAS_UInt32 bsInFilePos;
  XDAS_Int32  validBytes;

  XDAS_Int32 contentType, frameType;
  XDAS_Int8  versionBuf[IMPEG4DEC_VERSION_LENGTH];

#ifdef HOST_ARM9
   dummy();
#endif

  /*--------------------------------------------------------------------------*/
  /*  Enable the clock to IVAHD module. This is required only in case of      */
  /*  HW where GEL files are not used for enabling clock for IVAHD            */
  /*--------------------------------------------------------------------------*/
  my_initFileIO(ARG_FILEIO_TYPE);

  /*--------------------------------------------------------------------------*/
  /* Put IVAHD in standby after power on during booting process of given SoC  */
  /*--------------------------------------------------------------------------*/
  IVAHD_Standby_power_on_uboot(IVAHD0_HOST_L2_BASE, IVA0_RESET_CONTROLLER);
#ifdef NETRA
  IVAHD_Standby_power_on_uboot(IVAHD1_HOST_L2_BASE, IVA1_RESET_CONTROLLER);
  IVAHD_Standby_power_on_uboot(IVAHD2_HOST_L2_BASE, IVA2_RESET_CONTROLLER);
#endif /* NETRA */

  /*--------------------------------------------------------------------------*/
  /*  Open Test Config File                                                   */
  /*--------------------------------------------------------------------------*/
  fConfigFile = fopen((const char *)fname, "r");

  /*--------------------------------------------------------------------------*/
  /* Open the Log file.                                                       */
  /*--------------------------------------------------------------------------*/
  fLogFile = fopen((const char *)fStatusFileName, "w");

  /*--------------------------------------------------------------------------*/
  /*  Perform file open error check.                                          */
  /*--------------------------------------------------------------------------*/
  if (!fConfigFile)
  {
    printf("Couldn't open parameter file %s", fname);
    return XDM_EFAIL;
  }

  /*--------------------------------------------------------------------------*/
  /*  countConfigSet counts the number of the current configuration set       */
  /*  provided in the testvecs.cfg file. Reset countConfigSet value to 1.     */
  /*--------------------------------------------------------------------------*/
  countConfigSet  = 1;

  /*--------------------------------------------------------------------------*/
  /* Initialization of global variables that indicate algorithm creation      */
  /* status and ivahd code load status                                        */
  /*--------------------------------------------------------------------------*/
  algCreated      = XDAS_FALSE;
  ivaCodeLoadDone = XDAS_FALSE;

  /*--------------------------------------------------------------------------*/
  /*  Read the Config File until it reaches the end of file                   */
  /*--------------------------------------------------------------------------*/
  while(!feof(fConfigFile))
  {
    /*------------------------------------------------------------------------*/
    /* Initializing the following variables to zero before first process call */
    /* for each test vector.                                                  */
    /*------------------------------------------------------------------------*/
    fatalErrorDetected    = 0;
    nonFatalErrorDetected = 0;
    frameCount            = 0;
    displayCount          = 0;
    picture_count         = 0;
    CodecExtError         = 0;
    bsInFilePos           = 0;
    bufferFreed           = 0;
    bufferDisplayed       = 0;
    frameType             = 0;
    contentType           = 0;
    ParsedHeader          = FALSE;
    frameSizeOffset       = 0;
#ifdef TEST_SEEK_OP
    seekIdxCntr           = 0;
    SeekOpStart           = 0;
    SeekBackwd            = 0;
#endif /* TEST_SEEK_OP */
    /*------------------------------------------------------------------------*/
    /* Initialize the function return values                                  */
    /*------------------------------------------------------------------------*/
    ret_val               = IVIDDEC3_EOK;
    testVal               = APP_EOK;
    configStatus          = IALG_EOK;
    chkVal                = APP_SAME_CONFIG;

    streamEnd = 0;

#ifndef USE_HOST_FILE_IO
    printf("\n*******************************************");
    printf("\n Read Configuration Set %d", countConfigSet );
    printf("\n*******************************************");
#endif /* USE_HOST_FILE_IO */

    /*------------------------------------------------------------------------*/
    /* Parse the test vecs & params config files                              */
    /*------------------------------------------------------------------------*/
    ret_val = TestApp_ioFilePtrInit(fConfigFile, &testCompliance, &crcEnable);

    /*------------------------------------------------------------------------*/
    /* modify the operating mode and metadata type based on mbInfoWriteMode   */
    /*------------------------------------------------------------------------*/
    if(appControlPrms.mbInfoWriteMode)
    {
      /*----------------------------------------------------------------------*/
      /* Set the operating mode as transcode                                  */
      /*----------------------------------------------------------------------*/
      params.viddec3Params.operatingMode = IVIDEO_TRANSCODE_FRAMELEVEL;

      for(count = 0; count < IVIDEO_MAX_NUM_METADATA_PLANES; count++)
      {
        /*--------------------------------------------------------------------*/
        /* set the available metadata for mb info dump                        */
        /*--------------------------------------------------------------------*/
        if(params.viddec3Params.metadataType[count] ==IVIDEO_METADATAPLANE_NONE)
        {
          params.viddec3Params.metadataType[count] = APP_MB_INFO;
          break;
        }
        else if(params.viddec3Params.metadataType[count] == APP_MB_INFO)
        {
          break;
        }
      }
      /*----------------------------------------------------------------------*/
      /* In case no index of metadata is set to MB-info, then there must be   */
      /* some repetition of some metadata type.                               */
      /*----------------------------------------------------------------------*/
      if(count == IVIDEO_MAX_NUM_METADATA_PLANES)
      {
        if(params.viddec3Params.metadataType[1] ==
                                         params.viddec3Params.metadataType[2])
        {
          params.viddec3Params.metadataType[2] = APP_MB_INFO;
        }
        else
        {
          params.viddec3Params.metadataType[0] = APP_MB_INFO;
        }
      }
    }
    else
    {
      params.viddec3Params.operatingMode = 0;
    }
    /*------------------------------------------------------------------------*/
    /* If the parser/file reader returns faliure, exit the current config     */
    /*------------------------------------------------------------------------*/
    if(ret_val == APP_EFAIL)
    {
      printf("Exiting for this configuration...\n");
      updateConfigStatusFile(IALG_EFAIL);
      continue;
    }
    else if(ret_val == APP_EXIT)
    {
      break;
    }

    /*------------------------------------------------------------------------*/
    /* Setting to default in case the user configured to wrong value.         */
    /*------------------------------------------------------------------------*/
    if(appControlPrms.ivahdId >= SOC_TOTAL_IVAHDS)
    {
      appIvahdId = 0;
    }
    else
    {
      appIvahdId = appControlPrms.ivahdId;
    }

    /*------------------------------------------------------------------------*/
    /* File pointer to capture profile info, if enabled                       */
    /*------------------------------------------------------------------------*/
    if(appControlPrms.profileEnable)
    {
      if(countConfigSet == 1)
      {
        fCycles = fopen("..\\..\\..\\Test\\TestVecs\\Output\\ProfileInfo.txt",
                        "w");
        fprintf(fCycles,
                  "\t\thostPreIva \tpreMbLoop \tinMbLoop \tpostMbLoop \thostPostIva \tivaTotalCycles \n");
        fprintf(fCycles,
                 "|====================================================|\n\n");
      }
      fprintf(fCycles, "%s\n\n",appControlPrms.inFile);
    }

    /*------------------------------------------------------------------------*/
    /* By default decoding mode is Full frame i.e, at Auxiliary Unit based    */
    /* If decodeHeader parameter is set to 1, then change the mode to         */
    /* XDM_PARSE_HEADER (header only decoding mode)                           */
    /*------------------------------------------------------------------------*/
    decodeMode =  dynamicParams.viddec3DynamicParams.decodeHeader;

    /*------------------------------------------------------------------------*/
    /* Initializing the File pointers                                         */
    /*------------------------------------------------------------------------*/
    finFile   = filePtrs.fpInFile;
    foutFile  = filePtrs.fpOutFile;
    frefFile  = filePtrs.fpRefFile;
    fsizeFile = filePtrs.fpSizeFile;

    /*------------------------------------------------------------------------*/
    /* Check whether Algorithm instance has been already created & still exist*/
    /*------------------------------------------------------------------------*/
    if(algCreated == XDAS_FALSE)
    {
      /*---------------------------------------------------------------------*/
      /*  Initialization of static parameters is done by this function call  */
      /*---------------------------------------------------------------------*/
      TestApp_SetInitParams(&params);

      /*---------------------------------------------------------------------*/
      /*  Setting the sizes based on baseClassOnly control paramater.        */
      /*  if "baseClassOnly" is true then set base class sizes, elase set the*/
      /*  extended class structure sizes                                     */
      /*---------------------------------------------------------------------*/
      if(appControlPrms.baseClassOnly)
      {
        params.viddec3Params.size              = sizeof(IVIDDEC3_Params);
        BaseStatus->size                       = sizeof(IVIDDEC3_Status);
        dynamicParams.viddec3DynamicParams.size= sizeof(IVIDDEC3_DynamicParams);
        inArgs.viddec3InArgs.size              = sizeof(IVIDDEC3_InArgs);
        outArgs.viddec3OutArgs.size            = sizeof(IVIDDEC3_OutArgs);
      }
      else
      {
        params.viddec3Params.size              = sizeof(IMPEG4VDEC_Params);
        BaseStatus->size                       = sizeof(IMPEG4VDEC_Status);
        dynamicParams.viddec3DynamicParams.size= sizeof(IMPEG4VDEC_DynamicParams);
        inArgs.viddec3InArgs.size              = sizeof(IMPEG4VDEC_InArgs);
        outArgs.viddec3OutArgs.size            = sizeof(IMPEG4VDEC_OutArgs);
      }

      /*---------------------------------------------------------------------*/
      /*  Create the Algorithm object (instance)                             */
      /*---------------------------------------------------------------------*/
      printf("\nCreating Algorithm Instance...");

      /*---------------------------------------------------------------------*/
      /*  Algorithm create call. This function will allocate memory for the  */
      /*  algorithm handle and for all the buffers (memtabs) required by the */
      /*  algorithm and initialize them accordingly. The alg handle is       */
      /*  returned.                                                          */
      /*---------------------------------------------------------------------*/
      mpeg4decHandle = (IMPEG4VDEC_Handle)MPEG4VDEC_create(
		  				(const IMPEG4VDEC_Fxns *)&MPEG4VDEC_TI_IMPEG4VDEC,
		  				&params);
      /*---------------------------------------------------------------------*/
      /*  Check for errors in algorithm handle creation                      */
      /*---------------------------------------------------------------------*/
      if (mpeg4decHandle == NULL)
      {
        fprintf(stdout,"Failed to Create Instance. Exiting this config..\n");
        ret_val = IALG_EFAIL;
      }
      else
      {
        /*--------------------------------------------------------------------*/
        /*  Print success message if handle creation is proper.               */
        /*--------------------------------------------------------------------*/
        printf("\nAlgorithm Instance Creation Done...\n");
		/*--------------------------------------------------------------------*/
		/* Initialize the TILER params & Get the buffers in TILER space       */
		/* below calulation is done for buffer allocation in tiler mode       */
		/* when optional filtering is on                                      */
		/* the same info app will get by quaering the codec post instance     */
		/* creation                                                           */
		/*--------------------------------------------------------------------*/
        ReqWidth  = ((params.viddec3Params.maxWidth  + PADDING_OFFSET + 127) & ~127);
        ReqHeight = (params.viddec3Params.maxHeight  + PADDING_OFFSET);
        /*----------------------------------------------------------------------*/
        /* Luma Parameter                                                       */
        /*----------------------------------------------------------------------*/
		BaseStatus->bufInfo.minOutBufSize[0].tileMem.width  = ReqWidth;
		BaseStatus->bufInfo.minOutBufSize[0].tileMem.height = ReqHeight;
		/*----------------------------------------------------------------------*/
		/* Chroma Parameter                                                     */
		/*----------------------------------------------------------------------*/
		BaseStatus->bufInfo.minOutBufSize[1].tileMem.width  = ReqWidth;
		BaseStatus->bufInfo.minOutBufSize[1].tileMem.height = (ReqHeight>>1);
		/*--------------------------------------------------------------------*/
		/* Call Tiler init function                                           */
		/*--------------------------------------------------------------------*/
		testAppTilerInit(BaseStatus->bufInfo.minOutBufSize);
		/*--------------------------------------------------------------------*/
		/* Call to RMAN Assign Resources (allocate ires-rman resources).      */
		/* For MPEG4 Decoder, Only the alg handle is requested through ialg    */
		/* interface, the rest of the buffers [memtab's] are requested through*/
		/* IRES interface. The function call allocates the resources requested*/
		/* trough IRES interface and initializes them.                        */
		/*--------------------------------------------------------------------*/
		ret_val = RMAN_AssignResources((IALG_Handle)mpeg4decHandle);
      }
      /*----------------------------------------------------------------------*/
      /*Set the variable to indicate that algorithm instance has been created */
      /*----------------------------------------------------------------------*/
      algCreated = XDAS_TRUE;
      /*----------------------------------------------------------------------*/
      /* Do the parameter Boundary check                                      */
      /*----------------------------------------------------------------------*/
      chkVal = paramBoundaryCheck(ret_val, CREATE_TIME_LEVEL);

      if(chkVal == APP_NEXT_CONFIG)
      {
        continue;
      }
    }

    /*----------------------------------------------------------------------*/
    /* Check the display Buffer Mode setting & do the corresponding init.   */
    /*----------------------------------------------------------------------*/
    if(params.viddec3Params.displayBufsMode == IVIDDEC3_DISPLAYBUFS_PTRS)
    {
      displayBufs = &DisplayBuffers[0];
      outArgs.viddec3OutArgs.displayBufs.pBufDesc[0] = displayBufs;
    }
    else
    {
      displayBufs = (IVIDEO2_BufDesc *)&outArgs.viddec3OutArgs.displayBufs.
                     bufDesc[0];
    }

    /*-------------------------------------------------------------------*/
    /* Populate the version specific elements of status structure.       */
    /*-------------------------------------------------------------------*/
    status.viddec3Status.data.buf     = versionBuf;
    status.viddec3Status.data.bufSize = IMPEG4DEC_VERSION_LENGTH;

    /*---------------------------------------------------------------------*/
    /* Make XDM_GETVERSION control call to figure out the library version  */
    /*---------------------------------------------------------------------*/
    ret_val = MPEG4VDEC_control
             (
               mpeg4decHandle,
               XDM_GETVERSION,
               (IMPEG4VDEC_DynamicParams *)&dynamicParams.viddec3DynamicParams,
               (IMPEG4VDEC_Status *)&status
             );

    /*------------------------------------------------------------------------*/
    /* Check if XDM_GETVERSION failed, if not print the version of library    */
    /*------------------------------------------------------------------------*/
    if(ret_val != XDM_EFAIL)
    {
      char *version = NULL;
      /*-------------------------------------------------------------------*/
      /* Print the version of the library                                  */
      /*-------------------------------------------------------------------*/
      version = (char *)&status.viddec3Status.data.buf[0];
      printf("Version of MPEG4 library : %s \n",version);
    }
    else
    {
      break;
    }

    /*------------------------------------------------------------------------*/
    /* Allocate  memory for a Temporary buffer used for interleaving the      */
    /* two fields to form the output frame (for interlaced cases).            */
    /* (memory allocated for worst case i.e. two fields).                     */
    /*------------------------------------------------------------------------*/
    fieldBuf = (XDAS_Int32)my_Memalign(128, (((params.viddec3Params.maxWidth +
               (2 * PADX)) * (params.viddec3Params.maxHeight +
               (4 * PADY)) * 3) >> 1));

    /*------------------------------------------------------------------------*/
    /*  The outBufsInUseFlag tells us whether the previous input buffer given */
    /*  by the application to the algorithm is still in use or not. Since     */
    /*  this is before the first decode call, assign this flag to 0. The      */
    /*  algorithm will take care to initialize this flag appropriately from   */
    /*  hereon for the current configuration.                                 */
    /*------------------------------------------------------------------------*/
    outArgs.viddec3OutArgs.outBufsInUseFlag     = 0;

    /*------------------------------------------------------------------------*/
    /*  Resetting bytesConsumed variable to 0. This variable will contain     */
    /*  the total number of bytes consumed by the algorithm till now for the  */
    /*  current configuration.                                                */
    /*------------------------------------------------------------------------*/
    validBytes                           = 0;
    bytesConsumed                        = 0;
    outArgs.viddec3OutArgs.bytesConsumed = 0;

    /*------------------------------------------------------------------------*/
    /*  The GETBUFINFO call gives information for number of input and         */
    /*  output buffers. For the first call however, since the buffmgr_init()  */
    /*  has already initialized the luma and chroma buffer handles, the       */
    /*  GETBUFINFO call will not return any sizes for outbufs.                */
    /*------------------------------------------------------------------------*/
    MPEG4VDEC_control
    (
      mpeg4decHandle,
      XDM_GETBUFINFO,
      (IMPEG4VDEC_DynamicParams *)&dynamicParams.viddec3DynamicParams,
      (IMPEG4VDEC_Status *)&status
    );
    /*------------------------------------------------------------------------*/
    /*  The buffermanager init function call allocates the memory for the     */
    /*  number of buffers of the corresponding sizes as passed in arguments   */
    /*  to it. This function initializes the output/reference bufs based on   */
    /*  the outBuf parameters returned from the GETBUFINFO call above. This   */
    /*  call will allocate each of the numOutbufs memory equal to (specified  */
    /*  bufsize * MAX_BUFF_ELEMENTS). MAX_BUFF_ELEMENTS refers to the max     */
    /*  num of ref bufs required by algorithm. The initialization of buff     */
    /*  ptr will take place only for the first ref buffer but allocation      */
    /*  will happen for MAX_BUFF_ELEMENTS so that after the first decode      */
    /*  call, when actual height and width of picture are known, remaining    */
    /*  ref bufs can be initialised in the buffmgr_reinit function.           */
    /*  Note that the allocation of output/ref buffers will happen only once  */
    /*  for the entire bunch of configuration sets provided in the testvecs   */
    /*  config file.                                                          */
    /*------------------------------------------------------------------------*/
    ret_val = BUFFMGR_Init( BaseStatus->bufInfo.minNumOutBufs,
                            BaseStatus->bufInfo.minOutBufSize,
                            BaseStatus->bufInfo.outBufMemoryType
                          );
    if (ret_val)
    {
      fprintf (stdout,"\nMemory couldn't get allocated for output buffers\n");
      break;
    }

    /*------------------------------------------------------------------------*/
    /* If decode mode is not the Parse header mode, number of buffers         */
    /* allocated will be 1.                                                   */
    /*------------------------------------------------------------------------*/
    if(decodeMode == XDM_DECODE_AU)
    {
      numBuffersAllocated = 1;
    }

    /*------------------------------------------------------------------------*/
    /*  Increment the countConfigSet value for next iteration.                */
    /*------------------------------------------------------------------------*/
    countConfigSet++;

    /*------------------------------------------------------------------------*/
    /*  Initialize the input buffer properties as required by algorithm       */
    /*  based on info received by preceding GETBUFINFO call. First init the   */
    /*  number of input bufs.                                                 */
    /*------------------------------------------------------------------------*/
    inputBufDesc.numBufs     = BaseStatus->bufInfo.minNumInBufs ;

    /*------------------------------------------------------------------------*/
    /*  For the num of input bufs, initialize the buffer pointer addresses    */
    /*  and buffer sizes.                                                     */
    /*------------------------------------------------------------------------*/
    inputBufDesc.descs[0].buf           = (XDAS_Int8 *)inputData;
    inputBufDesc.descs[0].bufSize.bytes = INPUT_BUFFER_SIZE;
    inputBufDesc.descs[0].memType     = BaseStatus->bufInfo.inBufMemoryType[0];

    /*------------------------------------------------------------------------*/
    /*  Initialize the output buffer properties as required by algorithm      */
    /*  based on info received by preceding GETBUFINFO call.                  */
    /*------------------------------------------------------------------------*/
    outputBufDesc.numBufs = BaseStatus->bufInfo.minNumOutBufs;

    /*------------------------------------------------------------------------*/
    /*  Initialize the first time flag to 1 which will get used for the       */
    /*  condition check for reinitializing the buffers after the first        */
    /*  decode call.                                                          */
    /*------------------------------------------------------------------------*/
    first_time = 1;
    /*------------------------------------------------------------------------*/
    /*  Read the input bit stream from the file                               */
    /*------------------------------------------------------------------------*/
    validBytes = TestApp_ReadByteStream(finFile, fsizeFile, bsInFilePos);

    printf("Valid bytes read = %d\n", validBytes);
    /*------------------------------------------------------------------------*/
    /*  Do-While Loop for Decode Calls                                        */
    /*  The loop will break out if:                                           */
    /*  - frameCount >= appControlPrms.framesToDecode                         */
    /*  - Errors encountered in the deocode call that indicates a need for    */
    /*    termination of decoding for the current stream being decoded        */
    /*  - Buffer allocation for recon buffers failed                          */
    /*------------------------------------------------------------------------*/
    do
    {
      /*----------------------------------------------------------------------*/
      /*  Read Input bit stram.                                               */
      /*  For frame mode of decoding(provide 1 frame worth of input data),    */
      /*  the fsizeFile will have the info on frame sizes, so at a time we    */
      /*  read the bytes sufficient for one frame decoding.                   */
      /*----------------------------------------------------------------------*/
      if((appControlPrms.bitStreamMode == BS_FRAME_MODE))
      {
        /*--------------------------------------------------------------------*/
        /*  if this not the first frame data then read the frame data from    */
        /*  file                                                              */
        /*  OR                                                                */
        /*  If codec has been set in parse header mode and also input data    */
        /*  provided as header data then residual data                        */
        /*  in above case app needs to read the residual data for next process*/
        /*  call from input size file as previous parse header mode had only  */
        /*  header data                                                       */
        /*--------------------------------------------------------------------*/
        if((picture_count) || (ParsedHeader))
        {
          validBytes = TestApp_ReadByteStream(finFile, fsizeFile, bsInFilePos);
	    }
      }
      else if(bytesConsumed > (INPUT_BUFFER_SIZE - INFLIE_READ_THRESHOLD))
      {
        /*--------------------------------------------------------------------*/
        /* Fill the buffer with more input data if bytesRemaining in the      */
        /* input buffer fall below a certain threshold                        */
        /*--------------------------------------------------------------------*/
        bsInFilePos    +=  bytesConsumed;
        bytesConsumed   = 0;
        printf("Reading another INPUT_BUFFER_SIZE bytes....\n");
        validBytes = TestApp_ReadByteStream(finFile, fsizeFile, bsInFilePos);
      }

      /*----------------------------------------------------------------------*/
      /*  Do the process call only if validBytes is greater than zero.        */
      /*----------------------------------------------------------------------*/
      if (validBytes > 0)
      {
        /*--------------------------------------------------------------------*/
        /*  Check for the value of the outBufsInUseFlag. If the previous      */
        /*  decode call was for the first field of an interlaced frame, the   */
        /*  previous outbuf supplied by algorithm may still be required for   */
        /*  decoding second field. In that case, the algorithm will not need  */
        /*  a new buffer from the application. If above is not the case, call */
        /*  the getfreebuffer() function of buffermanager to get the buffer   */
        /*  element to be supplied to algorithm for the next decode call.     */
        /*  Also if the codec is in parseHeader mode, then there is no need to*/
        /*  supply o/p buf                                                    */
        /*--------------------------------------------------------------------*/
        if((decodeMode == XDM_DECODE_AU) &&
           (!outArgs.viddec3OutArgs.outBufsInUseFlag))
        {
          buffEle = BUFFMGR_GetFreeBuffer();

          if (buffEle == NULL)
          {
            fprintf (stdout,"\nNo free buffer available\n");
            break;
          }
          /*------------------------------------------------------------------*/
          /*  Initialize the input ID in input arguments to the bufferid of   */
          /*  buffer element returned from getfreebuffer() function.          */
          /*------------------------------------------------------------------*/
          inArgs.viddec3InArgs.inputID         = buffEle->bufId;
        }
        /*--------------------------------------------------------------------*/
        /*  Give some arbit input ID as codec dont support ID as zero         */
        /*--------------------------------------------------------------------*/
        if(decodeMode == XDM_PARSE_HEADER)
        {
          inArgs.viddec3InArgs.inputID   = -1;
        }
        /*--------------------------------------------------------------------*/
        /* Update the Input & Output buffer descriptors.                      */
        /*--------------------------------------------------------------------*/
        TestApp_updateIObufDesc(&inputBufDesc, &outputBufDesc,
                                BaseStatus, bytesConsumed, validBytes);

        /*--------------------------------------------------------------------*/
        /* If codec has been set with parse header mode and also codec gets   */
        /* data header data seperatly and residual data seperatly             */
        /* then while decoding the first frame residula data validbytes needs */
        /* to update                                                          */
        /*--------------------------------------------------------------------*/
        if(ParsedHeader)
        {
          inArgs.viddec3InArgs.numBytes        = validBytes;
          //inputBufDesc.descs[0].buf   += outArgs.viddec3OutArgs.bytesConsumed;
          ParsedHeader = FALSE;
		}
		else
		{
          /*------------------------------------------------------------------*/
          /* Assign the number of bytes available                             */
          /* If application is configured the codec to run in parse-header and*/
          /* frame size bit-stream input mode, the update of inArgs numbytes  */
          /* and inputBufDesc buffer address should be done as if application */
          /* is providing bit-stream in buffer mode (not frame size mode).    */
          /* This   has to be taken care only after the first process call.   */
          /*------------------------------------------------------------------*/
        if((picture_count==0) &&(appControlPrms.bitStreamMode == BS_FRAME_MODE))
        {
          if(outArgs.viddec3OutArgs.bytesConsumed)
          {
            inArgs.viddec3InArgs.numBytes -=outArgs.viddec3OutArgs.bytesConsumed;
            inputBufDesc.descs[0].buf   += outArgs.viddec3OutArgs.bytesConsumed;
          }
          else
          {
            inArgs.viddec3InArgs.numBytes        = validBytes;
          }
        }
        else
        {
          inArgs.viddec3InArgs.numBytes        = validBytes;
        }
	    }
        /*--------------------------------------------------------------------*/
        /*  The following function is used to initialize the dynamic params   */
        /*  of the IVIDDEC1 handle.                                           */
        /*--------------------------------------------------------------------*/
        TestApp_SetDynamicParams(&dynamicParams.viddec3DynamicParams);

        /*--------------------------------------------------------------------*/
        /*  The SETPARAMS control call is used to set the codec's dynamic     */
        /*  parameters. Base or extended class is indicated by size field.    */
        /*--------------------------------------------------------------------*/
        ret_val = MPEG4VDEC_control
                 (
                   mpeg4decHandle,
                   XDM_SETPARAMS,
                  (IMPEG4VDEC_DynamicParams *)&dynamicParams.viddec3DynamicParams,
                  (IMPEG4VDEC_Status *)&status
                 );

        if(ret_val == IVIDDEC3_EUNSUPPORTED)
        {
          ret_val = IVIDDEC3_EFAIL;
        }

        /*--------------------------------------------------------------------*/
        /* Do the parameter Boundary check                                    */
        /*--------------------------------------------------------------------*/
        chkVal = paramBoundaryCheck(ret_val, CONTROL_CALL_LEVEL);
        if(chkVal == APP_NEXT_CONFIG)
        {
          /*------------------------------------------------------------------*/
          /* In Parameter Boundary check validation, some buffers might get   */
          /* allocated by this time. Freeing them up before proceeding to next*/
          /* test vector. Basically avoidng the memory leak during validation */
          /*------------------------------------------------------------------*/
          free((void *)fieldBuf);
          BUFFMGR_DeInit(1, 1);
          break;
        }
#ifdef MEM_CORRUPTION_TEST
      /*----------------------------------------------------------------------*/
      /* Do scratch corruption of SL2, ITCM, DTCM and IP memories             */
      /*----------------------------------------------------------------------*/
      MPEG4DEC_ScratchMemoryCorruption();
#endif
        /*--------------------------------------------------------------------*/
        /*  Configuring ICECRUSHER registers for profiling purpose            */
        /*--------------------------------------------------------------------*/
        ConfigureProfile();
        /*--------------------------------------------------------------------*/
        /* Capture the time just before the process call (for profiling).     */
        /*--------------------------------------------------------------------*/
        TestApp_captureTime(START_TIME);

        /*--------------------------------------------------------------------*/
        /*  Start the decode process for one frame/field by calling the       */
        /*  MPEG4VDEC_decode() function.                                       */
        /*  Note: MPEG4VDEC_decodeFrame() will decode one frame for progressive*/
        /*       streams and one field for interlaced streams.                */
        /*--------------------------------------------------------------------*/
        ret_val = MPEG4VDEC_decodeFrame(mpeg4decHandle, &inputBufDesc,
                                       &outputBufDesc, &inArgs, &outArgs);
        /*--------------------------------------------------------------------*/
        /*  Capture time immediately after the process call(for profiling).   */
        /*--------------------------------------------------------------------*/
        TestApp_captureTime(END_TIME);

#ifdef MEM_CORRUPTION_TEST
      /*----------------------------------------------------------------------*/
      /* Memory corruption TEST:                                              */
      /* Deactivate all resources: Since we corrupt the IVAHD memories,       */
      /* the persistant data need to be backed up through deactivate call     */
      /*----------------------------------------------------------------------*/
      RMAN_DeactivateAllResources((IALG_Handle)mpeg4decHandle);
#endif

        /*--------------------------------------------------------------------*/
        /*  Check for failure message in the return value.                    */
        /*--------------------------------------------------------------------*/
        if(ret_val != IVIDDEC3_EOK)
        {
          fprintf (stdout,"\nProcess Function returned Error\n");
        }

        /*--------------------------------------------------------------------*/
        /*  Update the bytesConsumed by adding the bytes consumed by the algo-*/
        /*  rithm in the last decode call. Similarly, reduce the valid bytes  */
        /*  remaining in the application input buffer by subtracting the      */
        /*  bytes consumed in the last decode call.                           */
        /*--------------------------------------------------------------------*/
        bytesConsumed          += outArgs.viddec3OutArgs.bytesConsumed;

        /*--------------------------------------------------------------------*/
        /* In case of frame Size mode , don't reduce the valid bytes by       */
        /* consumed in last process call. Because the validBytes doesn't      */
        /* get used to update the input buffer pointer as we always load      */
        /* one frame size data at the same address for all frames.            */
        /*--------------------------------------------------------------------*/
        if(appControlPrms.bitStreamMode != BS_FRAME_MODE)
        {
          validBytes             -= outArgs.viddec3OutArgs.bytesConsumed;
        }

        contentType = outArgs.viddec3OutArgs.decodedBufs.contentType;
        /*--------------------------------------------------------------------*/
        /* Read the frame type only in case of the decodeAU meode as frametype*/
        /* transmits with framedheader in case of MPEG4                       */
        /*--------------------------------------------------------------------*/
        if(decodeMode == XDM_DECODE_AU)
           frameType   = outArgs.viddec3OutArgs.decodedBufs.frameType;
        /*--------------------------------------------------------------------*/
        /* one frame is decoded successfully                                  */
        /*--------------------------------------------------------------------*/
#ifdef USE_HOST_FILE_IO
        printf("frameCount %6d\n", frameCount);
#else
        printf("#%6d, %6d \t%s \t%s", frameCount, picture_count,
               gFrameTypeStrings[frameType],gContentStrings[contentType]);
        printf("Decoded bytes=%d\n", outArgs.viddec3OutArgs.bytesConsumed);
#endif /* USE_HOST_FILE_IO */

        /*--------------------------------------------------------------------*/
        /*  Increment the local framecount variable when an entire frame or   */
        /*  both fields of an interlaced frame have got decoded.              */
        /*  The outBufsInUseFlag indicates(in case of interlaces streams), if */
        /*  both the fields of a frame are decoded or not.                    */
        /*  The increment should happen only if the process call has run in   */
        /*  XDM_DECODE_AU mode.                                               */
        /*--------------------------------------------------------------------*/
        if(decodeMode == XDM_DECODE_AU)
        {
          if (!outArgs.viddec3OutArgs.outBufsInUseFlag)
          {
            frameCount++;
          }
          picture_count++;
        }

        /*--------------------------------------------------------------------*/
        /* If the app needs any additional status info from the decoder,      */
        /* the GETSTATUS control call can be invoked to populate the status   */
        /* parameters(see IVIDDEC3_Status structure).                         */
        /*--------------------------------------------------------------------*/
        MPEG4VDEC_control
        (
           mpeg4decHandle,
           XDM_GETSTATUS,
          (IMPEG4VDEC_DynamicParams *)&dynamicParams,
          (IMPEG4VDEC_Status *)&status
        );

        if(params.lastNFramesToLog > 0)
        {
          /*------------------------------------------------------------------*/
          /* Print information about the codec's debug information based on   */
          /* the result of the GETSTATUS query                                */
          /*------------------------------------------------------------------*/
          printf("Debug trace history configured for previous %d frames\n",
                  status.lastNFramesToLog);
          printf("Debug trace dump address in External memory: 0x%x\n",
                  status.extMemoryDebugTraceAddr);
          printf("Debug trace dump size in External memory: %d bytes\n",
                  status.extMemoryDebugTraceSize);
        }

        /*--------------------------------------------------------------------*/
        /*  Prepare to Flush the display Frames and write into an output file */
        /*  Also,Check for Break off condition of bytes consumed >=  the bytes*/
        /*  in input buffer.                                                  */
        /*--------------------------------------------------------------------*/
        CodecExtError =  outArgs.viddec3OutArgs.decodedBufs.extendedError;

        /*--------------------------------------------------------------------*/
        /* Figure out the status of current test vector.                      */
        /*--------------------------------------------------------------------*/
        if(ret_val != IVIDDEC3_EOK)
        {
          XDAS_UInt32 maskEOS;

          /*------------------------------------------------------------------*/
          /* Mask the Stream end erro bit,then check to see if any other error*/
          /* got set.If any other error set means the current testVec failed. */
          /*------------------------------------------------------------------*/
          maskEOS = (~(1 << IMPEG4D_ERR_STREAM_END));
          if(CodecExtError & maskEOS)
          {
            /*----------------------------------------------------------------*/
            /* TODO: Keep fine tuning it to achieve the ideal status for each */
            /* config.                                                        */
            /*----------------------------------------------------------------*/
            if(outArgs.viddec3OutArgs.bytesConsumed > 0x5)
            {
              configStatus = IALG_EFAIL;
            }
          }
        }

        /*--------------------------------------------------------------------*/
        /* Decoding the error message & mapping to corresponding name.        */
        /*--------------------------------------------------------------------*/
        TestApp_errorReport(CodecExtError);
       /*--------------------------------------------------------------------*/
        /*  Request frame for display                                         */
        /*  Check for frame ready via display buffer pointers. If outputID of */
        /*  outargs is non-zero, only then a frame is required to be displayed*/
        /*  Call the display routines after performing the check.             */
        /*--------------------------------------------------------------------*/
        if((decodeMode == XDM_DECODE_AU) &&
           (outArgs.viddec3OutArgs.outputID[0] != 0) &&
           (displayBufs->numPlanes))
        {
          bufferDisplayed =  bufferDisplayed + 1;

          if(frameCount > appControlPrms.dumpFrom)
          {
            testVal = TestApp_outputDataXfr(foutFile, frefFile, outArgs,
                                            testCompliance, crcEnable);
          }
        }

        /*--------------------------------------------------------------------*/
        /*  Buffer free up checks: If the freeBufId in OutArgs is non-zero    */
        /*  then this indicated frame to be freed. Release the buffers which  */
        /*  are freed  side   -back to the buffer manager. The freebufID array*/
        /*  of outargs contains the sequence of bufferIds which need to be    */
        /*  freed. This gets populated by the algorithm. The following        */
        /*  function will do  the task of freeing up the buffers.             */
        /*  Note: The decoder can free up one or more buffers. The freeBufId  */
        /*        array need to be checked untill we find a zero buffer id    */
        /*        (zero indiates end of the array). Some process calls may not*/
        /*        have any buffers to be freed, in this case, the first       */
        /*        element of freeBufid will be zero.                          */
        /*--------------------------------------------------------------------*/
        if(decodeMode == XDM_DECODE_AU)
        {
          BUFFMGR_ReleaseBuffer((XDAS_UInt32 *)outArgs.viddec3OutArgs.freeBufID);
        }

        /*--------------------------------------------------------------------*/
        /* In the very first process call, Header parsing is done if decode   */
        /* header is set by dynamic params. Second process call onwards,      */
        /* the process call is to decode an entire AU.                        */
        /*--------------------------------------------------------------------*/
        decodeMode = XDM_DECODE_AU;
        /*--------------------------------------------------------------------*/
        /* first process call of codec has been provided only with header data*/
        /* and header has been decoded, now set the flag ParsedHeader to TRUE */
        /* now next data would be residual data for the same frame            */
        /*--------------------------------------------------------------------*/
        if(appControlPrms.readHeaderdata && (0 == picture_count))
        {
           ParsedHeader = TRUE;
	    }
        if (!outArgs.viddec3OutArgs.outBufsInUseFlag)
        {
          printf("|----------------------------------------------------|\n");
        }
        /*--------------------------------------------------------------------*/
        /*  Prepare to Flush the display Frames and write into an output file */
        /*  Also,Check for Break off condition of bytes consumed >=  the bytes*/
        /*  in input buffer.                                                  */
        /*--------------------------------------------------------------------*/
        CodecExtError =  outArgs.viddec3OutArgs.decodedBufs.extendedError;

      } /* if(validBytes > 0) */
      else
      {
        /*-------------------------------------------------------------------*/
        /*  Return if there is an error in reading the file                  */
        /*-------------------------------------------------------------------*/
        if (validBytes < 0)
        {
          validBytes = 0;
          fprintf(stdout,"Valid Bytes incorrect,Exiting this configuration\n");
        }
      }

      /*----------------------------------------------------------------------*/
      /* Do the flush if no more input bit-stream is available.               */
      /* Even some errors given out by the process call drive the application */
      /* to initiate flush operation.                                         */
      /*----------------------------------------------------------------------*/
      if((streamEnd) || (!validBytes) ||
         (TestApp_flushCheck(CodecExtError)))
      {
        testVal= testAppFlushFrames(&inputBufDesc, &outputBufDesc, displayBufs);
        first_time = 0;
      }

      CodecExtErrorFlush =  outArgs.viddec3OutArgs.decodedBufs.extendedError;
      printProfileNums();
      /*--------------------------------------------------------------------*/
      /*  If this is the first decode call in the stream, then reinitialize */
      /*  all the buffers based on the picture width and height.            */
      /*  If the picture resolution changes, then also the reinit of the    */
      /*  buffers need to be done based on the new height/width.            */
      /*--------------------------------------------------------------------*/
      if ((first_time) ||
          (TestApp_checkErr(CodecExtError, IMPEG4D_ERR_PICSIZECHANGE)))
      {
        /*------------------------------------------------------------------*/
        /* If there is a resolution change, the current buffers need to be  */
        /* freed and de-allocated and then new buffers need to be allocated */
        /* based on the changed resolution.                                 */
        /*------------------------------------------------------------------*/
        if(TestApp_checkErr(CodecExtError, IMPEG4D_ERR_PICSIZECHANGE))
        {
          /*----------------------------------------------------------------*/
          /* Mark all the buffers as free                                   */
          /*----------------------------------------------------------------*/
          BUFFMGR_ReleaseAllBuffers();
          /*----------------------------------------------------------------*/
          /*  Free the buffers allocated for output/reference               */
          /*  For picture size change, the buffers need to be reallocated   */
          /*----------------------------------------------------------------*/
          BUFFMGR_DeInit(numBuffersAllocated, outputBufDesc.numBufs);
       }
        /*------------------------------------------------------------------*/
        /* The GETBUFINFO call will give information for number of output   */
        /* buffers. This call is after the first decode call hence the      */
        /* output buffer sizes returned by this call is calculated based on */
        /* actual pic width and height. Hence, there is need for buffer re- */
        /* initializations which happens immediately after this call.       */
        /*------------------------------------------------------------------*/
        MPEG4VDEC_control
        (
          mpeg4decHandle,
          XDM_GETBUFINFO,
          (IMPEG4VDEC_DynamicParams *)&dynamicParams.viddec3DynamicParams,
          (IMPEG4VDEC_Status *)&status
        );

        /*------------------------------------------------------------------*/
        /*  Initialize the TILER params & Get the buffers in TILER space    */
        /*------------------------------------------------------------------*/
        testAppTilerInit(BaseStatus->bufInfo.minOutBufSize);
        /*------------------------------------------------------------------*/
        /* Reinitialize the buffers using the actual buffer sizes for luma  */
        /* and chroma. This call will initialize the buf ptrs for all       */
        /* ADD_DISPLAY_BUFS (In some cases, deocder may need double of the  */
        /* status.maxNumDisplayBufs) unlike the first                       */
        /* which had initialized only buffer pointers for first ref buffers.*/
        /*------------------------------------------------------------------*/
        numBuffersAllocated = (BaseStatus->maxNumDisplayBufs +
                               ADD_DISPLAY_BUFS);
        ret_val = BUFFMGR_ReInit
        (
          numBuffersAllocated,
          BaseStatus->bufInfo.minNumOutBufs,
          BaseStatus->bufInfo.minOutBufSize,
          BaseStatus->bufInfo.outBufMemoryType,
          first_time
        );

        if (ret_val)
        {
          fprintf(stdout,"\nMemory couldn't get allocated for O/P buffers\n");
          break;
        }
        /*------------------------------------------------------------------*/
        /*  Reset the flag first_time to 0 since the buffers are correctly  */
        /*  reinitialized.                                                  */
        /*------------------------------------------------------------------*/
        first_time = 0;
      }
      /*----------------------------------------------------------------------*/
      /*  Check for Break off condition of in case of test compliance mode    */
      /*----------------------------------------------------------------------*/
      if(testVal == APP_EFAIL)
      {
        break;
      }

      /*----------------------------------------------------------------------*/
      /*  Check for Break off condition of Error is due to Picture size Change*/
      /*  OR Buffer Under flow.                                               */
      /*  Check for Break off condition of End of Stream                      */
      /*----------------------------------------------------------------------*/
      if(((TestApp_checkErr(CodecExtError, XDM_INSUFFICIENTDATA)) &&
          (BS_FRAME_MODE != appControlPrms.bitStreamMode)) ||
         (TestApp_checkErr(CodecExtError, XDM_FATALERROR)) ||
         (TestApp_checkErr(CodecExtErrorFlush, IMPEG4D_ERR_STREAM_END)&&
         (!TestApp_checkErr(CodecExtError, IMPEG4D_ERR_PICSIZECHANGE))) )
      {
        /*--------------------------------------------------------------------*/
        /*  The algorithm currently does not guarantee to set freebufid of    */
        /*  all buffers in use when the framecount exceeds framestoDecode.    */
        /*  This is managed for now by an application function. Note that     */
        /*  doing this has significance only when running multiple streams.   */
        /*--------------------------------------------------------------------*/
        BUFFMGR_ReleaseAllBuffers();
        break;
      }

      /*----------------------------------------------------------------------*/
      /*  Update InArgs structure and the numBytes left to be decoded.        */
      /*----------------------------------------------------------------------*/
      if ((appControlPrms.bitStreamMode != BS_FRAME_MODE) &&
           (params.viddec3Params.inputDataMode == IVIDEO_FIXEDLENGTH)&&
          (CodecExtError != IMPEG4D_ERR_PICSIZECHANGE))
      {
        /*--------------------------------------------------------------------*/
        /* update BitsBuffer                                                  */
        /*--------------------------------------------------------------------*/
        inArgs.viddec3InArgs.inputID++;
        inArgs.viddec3InArgs.numBytes -= outArgs.viddec3OutArgs.bytesConsumed;
        inputBufDesc.descs[0].buf     += outArgs.viddec3OutArgs.bytesConsumed;
      }

#ifdef TEST_SEEK_OP
      /*--------------------------------------------------------------------*/
      /* The following code is to check the support for seek operation.     */
      /* Before performing the seek (forward, rewind) operation, the        */
      /* application should do a XDM flush by calling the control function. */
      /* MPEG4VDEC_control with XDM_FLUSH. This will flush out the frames    */
      /* currently locked by the decoder (display and then freed).          */
      /* Then the decoder has to latch on to the next frame (forward/rewind)*/
      /*--------------------------------------------------------------------*/
      if(frameCount == frameNumsToStop[seekIdxCntr])
      {
        /*--------------------------------------------------------------------*/
        /* flush Display queue                                                */
        /*--------------------------------------------------------------------*/
        XDAS_UInt32 seekVal;

        /*--------------------------------------------------------------------*/
        /* First flush the frames present in DPB, then seek to the requested  */
        /* or interested frame.                                               */
        /*--------------------------------------------------------------------*/
        testAppFlushFrames(&inputBufDesc, &outputBufDesc, displayBufs);

        printf("\nSeek Operation Started......\n");
        SeekOpStart = 1;
        SeekBackwd  = 0;

        /*--------------------------------------------------------------------*/
        /* Check if application want to do the seek opeation in backward      */
        /* direction.                                                         */
        /*--------------------------------------------------------------------*/
        if(frameCount > frameNumsToSeek[seekIdxCntr])
        {
          SeekBackwd = 1;
          frameCount = frameNumsToSeek[seekIdxCntr];
          printf("\n*** Seek In Backward Direction ***\n");
        }

        /*--------------------------------------------------------------------*/
        /* Seek the input file pointer to the right location in Bitstream file*/
        /* for the seeking frame number.                                      */
        /*--------------------------------------------------------------------*/
        do
        {
          seekVal = TestApp_ReadByteStream(finFile, fsizeFile, bsInFilePos);
          frameCount++;
          frameCount = frameCount - SeekBackwd;
          printf("\t Bytes Read: %d......\n", seekVal);

        }while(frameCount != frameNumsToSeek[seekIdxCntr]);

        printf("Seek is done to Frame: %d\n", frameCount);
        seekIdxCntr++;
        SeekOpStart = 0;
        SeekBackwd  = 0;
      }

#endif /* TEST_SEEK_OP */

      /*----------------------------------------------------------------------*/
      /* Check if we have decoded the specified number of frames: Break off   */
      /* the while loop if Specified number of Frames are Decoded             */
      /*----------------------------------------------------------------------*/

      if (frameCount >= appControlPrms.framesToDecode)
      {
        printf("\n Specified number of Frames Decoded...  ");
        break;
      }
      if((streamEnd) || (!validBytes))
      {
        printf("\n current stream decoded ...  \n");
        break;
      }
    } while (1);

    /*----------------------------------------------------------------------*/
    /* If the test status indicates decoding of next configuration, then    */
    /* skip current stream decoding and go to next config to be tested.     */
    /*----------------------------------------------------------------------*/
    if(chkVal == APP_NEXT_CONFIG)
    {
      continue;
    }
    /*----------------------------------------------------------------------*/
    /* Print each config status in a Log file to check the failures if any. */
    /*----------------------------------------------------------------------*/
    updateConfigStatusFile(configStatus);

    /*------------------------------------------------------------------------*/
    /* Free the temporary buffer                                              */
    /*------------------------------------------------------------------------*/
    free((void *)fieldBuf);

    /*------------------------------------------------------------------------*/
    /* Close the Input/output/reference files.                                */
    /*------------------------------------------------------------------------*/
    if(testCompliance)
    {
       if(frefFile)
       {
         fclose(frefFile);
       }
    }

    if((appControlPrms.bitStreamMode == BS_FRAME_MODE))
    {
      fclose(fsizeFile);
    }

    my_fclose(foutFile, ARG_FILEIO_TYPE);
    my_fclose(finFile, ARG_FILEIO_TYPE);

    /*------------------------------------------------------------------------*/
    /*  Free the buffers allocated for output/reference for all the streams   */
    /*  that have been executed.                                              */
    /*------------------------------------------------------------------------*/
    BUFFMGR_DeInit(numBuffersAllocated, outputBufDesc.numBufs);

#ifdef USE_HOST_FILE_IO
    printf("Decoding completed \n");
#else
    printf("Valid bytes remaining in the stream: %d\n", validBytes);
    /*---------------------------------------------------------------------*/
    /* make the next process call only if at least 4 bytes (needed for     */
    /* start code detection) are available.                                */
    /*---------------------------------------------------------------------*/
    if(validBytes > 4)
    {
      printf("Exiting due to error...\n");
    }
    printf("Decoding completed \n");
    printf("\n\n Frames Displayed - %d Frames Freed - %d\n",
           bufferDisplayed, bufferFreed);
#endif /* USE_HOST_FILE_IO*/
    /*---------------------------------------------------------------------*/
    /* If xdmReset flag is set, make the XDM_RESET control call instead of */
    /* deleting the current alg instance & re-creating.                    */
    /*---------------------------------------------------------------------*/
    if(appControlPrms.xdmReset)
    {
      /*---------------------------------------------------------------------*/
      /* Do an XDM_RESET call before decoding every bit stream, do not create*/
      /* the instance for each stream.                                       */
      /*---------------------------------------------------------------------*/
      MPEG4VDEC_control
      (
         mpeg4decHandle,
         XDM_RESET,
         (IMPEG4VDEC_DynamicParams *)&dynamicParams.viddec3DynamicParams,
         (IMPEG4VDEC_Status *)&status
      );

      algCreated = XDAS_TRUE;
    }
    else
    {
      /*----------------------------------------------------------------------*/
      /* Else of XDM_RESET: Delete the algo instance and free the resources   */
      /*----------------------------------------------------------------------*/
      RMAN_FreeResources((IALG_Handle)mpeg4decHandle);
      MPEG4VDEC_delete(mpeg4decHandle);
      algCreated = XDAS_FALSE;
    }
  }

  /*--------------------------------------------------------------------------*/
  /* Check whether the algorithm instance is still exists, delete if so.      */
  /*--------------------------------------------------------------------------*/
  if(algCreated == XDAS_TRUE)
  {
    RMAN_FreeResources((IALG_Handle)mpeg4decHandle);
    MPEG4VDEC_delete(mpeg4decHandle);
  }
  /*--------------------------------------------------------------------------*/
  /* Close the profile info file.                                             */
  /*--------------------------------------------------------------------------*/
  if(appControlPrms.profileEnable)
  {
    fclose(fCycles);
  }

#ifdef DUMP_PROFILE_NUMS
  fclose(fProfileNums);
#endif

  /*--------------------------------------------------------------------------*/
  /* Close the config and Log files                                           */
  /*--------------------------------------------------------------------------*/
  fclose(fConfigFile);
  fclose(fLogFile);

  return (0);

} /* main()*/

