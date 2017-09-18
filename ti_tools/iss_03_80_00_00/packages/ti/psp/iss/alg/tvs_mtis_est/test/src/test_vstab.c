/** ==================================================================
 *  @file   test_vstab.c                                                  
 *                                                                    
 *  @path   /ti/psp/iss/alg/tvs_mtis_est/test/src/                                                  
 *                                                                    
 *  @desc   This  File contains.                                      
 * ===================================================================
 *  Copyright (c) Texas Instruments Inc 2011, 2012                    
 *                                                                    
 *  Use of this software is controlled by the terms and conditions found
 *  in the license agreement under which this software has been supplied
 * ===================================================================*/
/*=======================================================================
 *
 *            Texas Instruments Internal Reference Software
 *
 *                           DSPS R&D Center
 *                     Video and Image Processing
 *
 *         Copyright (c) 2004 Texas Instruments, Incorporated.
 *                        All Rights Reserved.
 *
 *
 *          FOR TI INTERNAL USE ONLY. NOT TO BE REDISTRIBUTED.
 *
 *                    TI INTERNAL - TI PROPRIETARY
 *
 *
 *  Contact: Aziz Umit Batur     <batur@ti.com>
 *  Contact: Rajesh Narasimha    <rajeshn@ti.com>
 *
 *=======================================================================
 *
 *  File: main.c
 *
 *=======================================================================
 *
 *  Revision 4.5 (2-April-2009)
 *
 =======================================================================*/

#include "../inc/globalDefs.h"
// #include "TI_VidStab.h"
#include <WTSD_DucatiMMSW/alg/tvs_mtis_est/msp_tvs_mtis_est.h>
#include<WTSD_DucatiMMSW/platform/osal/timm_osal_trace.h>
#include <xdc/runtime/Timestamp.h>
#include "../inc/testPlatform.h"
#include "../../debugUtils.h"
#include <ti/sysbios/ipc/Semaphore.h>
#define DEFAULT_BYTE_ALIGN 4
Semaphore_Handle gSemHandleVstab;

/* ===================================================================
 *  @func     InitParams                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE InitParams(MSP_TVS_MTIS_CREATE_PARAM * vsCP, TPstruct * tp);

// void ProcessArgs(int argc, char *argv[], VS_CreationParamsStruct* vsCP,
// VS_FrameParamsStruct* vsFP, TPstruct* tp);

// GLOBAL MEMORY
// DEFINITIONS-----------------------------------------------------------------

// Frame buffer
// Byte frameBuffer1[DEF_FRAME_VSIZE*DEF_FRAME_HSIZE*3/2];
// Byte frameBuffer2[DEF_FRAME_VSIZE*DEF_FRAME_HSIZE*3/2];
Byte *frameBuffer1;

Byte *frameBuffer2;

// BSC buffer
// Byte bscBuffer1[1920*4];
// Byte bscBuffer2[1920*4];
Byte *bscBuffer1;

Byte *bscBuffer2;

/* 
 * //External data memory Byte externalDataMemory[DEF_EXT_DATA_MEMORY_SIZE];
 * 
 * //Internal persistent memory (holds VS params and data that should be
 * preserved between frames) Byte
 * internalPersistentMemory[DEF_INT_PERSISTENT_MEMORY_SIZE];
 * 
 * //Internal scratch memory (will be overwritten each time a frame is
 * processed) Byte internalScratchMemory[DEF_INT_SCRATCH_MEMORY_SIZE]; */
// Test platform data
TPstruct tpHandle;

MSP_TVS_MTIS_FRAME_OUT VS_FrameOut;

const VstabTestParamType VstabParam[] = {
    {"walkingGreenville_640x480_390.yuv", "out.yuv", 640, 480, 640, 480, 0, 4,
     64, 48, -1, -1},
    {"inp_vstab_176x144.yuv", "out.yuv", 176, 144, 176, 144, 0, 4, 18, 14, -1,
     -1},
    {"test_1920x1080.yuv", "out.yuv", 1920, 1080, 1920, 1080, 0, 4, 192, 108,
     -1, -1},
    {"walkingGreenville_640x480_390.yuv", "out.yuv", 640, 480, 640, 480, 0, 4,
     64, 48, 64, 48},
    {"inp_vstab_176x144.yuv", "out.yuv", 176, 144, 176, 144, 0, 4, 18, 14, 18,
     14},
    {"test_1920x1080.yuv", "out.yuv", 1920, 1080, 1920, 1080, 0, 4, 96, 54, -1,
     -1},
    {"test_1920x1080.yuv", "out.yuv", 1920, 1080, 1920, 1080, 0, 4, 192, 108,
     192, 108},
    {"test_2024x1140.yuv", "out.yuv", 2024, 1140, 1920, 1080, 0, 4, 404, 228,
     -1, -1},
    {"walkingGreenville_640x480_390.yuv", "out.yuv", 640, 480, 640, 480, 0, 4,
     64, 48, 364, 248},

    {"test_320x240.yuv", "out.yuv", 320, 240, 320, 240, 0, 4, 48, 36, 64, 48},
    {"test_320x240.yuv", "out.yuv", 320, 240, 320, 240, 0, 4, 32, 24, 96, 72},
    {"test_320x240.yuv", "out.yuv", 320, 240, 320, 240, 0, 4, 32, 24, 16, 12},
    {"test_854x480.yuv", "out.yuv", 854, 480, 854, 480, 0, 4, 85, 48, 128, 72},
    {"test_768x576.yuv", "out.yuv", 768, 576, 768, 576, 0, 4, 92, 69, -1, -1},
    {"test_800x600.yuv", "out.yuv", 800, 600, 800, 600, 0, 4, 79, 59, -1, -1},
    {"test_1280x720.yuv", "out.yuv", 1280, 720, 1280, 720, 0, 4, 128, 72, 256,
     144},
    {"test_1280x1024.yuv", "out.yuv", 1280, 1024, 1280, 1024, 0, 4, 128, 102,
     -1, -1}
    /* 
     * {"walkingGreenville_640x480_390.yuv", "out.yuv", 0, 50, 64, 48, 256,
     * 256}, {"walkingGreenville_640x480_390.yuv", "out.yuv", 0, 50, 64, 48,
     * 64, 48}, {"walkingGreenville_640x480_390.yuv", "out.yuv", 0, 50, 64,
     * 48, -1, -1}, {"walkingGreenville_640x480_390.yuv", "out.yuv", 0, 50,
     * 64, 48, 64, 48}, {"walkingGreenville_640x480_390.yuv", "out.yuv", 0,
     * 50, 64, 48, -1, -1}, {"walkingGreenville_640x480_390.yuv", "out.yuv",
     * 0, 50, 64, 48, 64, 48} */
};

Word32 bCurrBufferReceived = 0;

Word32 bPrevBufferReceived = 0;

/* ===================================================================
 *  @func     CallBackFromMSP2VstabApp                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE CallBackFromMSP2VstabApp(MSP_PTR hMSP,
                                        MSP_PTR pAppData,
                                        MSP_EVENT_TYPE tEvent,
                                        MSP_OPAQUE nEventData1,
                                        MSP_OPAQUE nEventData2)
{
    MSP_BUFHEADER_TYPE *pBufHdr;

    switch (tEvent)
    {
        case MSP_DATA_EVENT:
            if (nEventData1 == MSP_TVS_MTIS_INPUT_PREV)
            {
                bPrevBufferReceived = 1;
            }
            else if (nEventData1 == MSP_TVS_MTIS_INPUT_CURR)
            {
                bCurrBufferReceived = 1;
                pBufHdr = (MSP_BUFHEADER_TYPE *) nEventData2;
                TVS_MTIS_Memcpy(&VS_FrameOut.sROh, pBufHdr->pCodecData,
                                sizeof(MSP_TVS_MTIS_FRAME_OUT));
            }
            if (bPrevBufferReceived == 1 && bCurrBufferReceived == 1)
            {
                Semaphore_post(gSemHandleVstab);
                bCurrBufferReceived = 0;
                bPrevBufferReceived = 0;
            }
            break;
        case MSP_ERROR_EVENT:
            // gEncodeError = MSP_TRUE;
            break;
        case MSP_CTRLCMD_EVENT:
            /* if(nEventData1 == MSP_CTRLCMD_STOP) gEncodeError = MSP_TRUE; */
            break;
    }
    return (MSP_ERROR_NONE);
}

// ------------------------------------------------------------------------------------------

/* ===================================================================
 *  @func     VSTAB_Test                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
int VSTAB_Test(void)
{
    // VS_CONTEXT vsCon;
    Semaphore_Params SemParams;

    MSP_ERROR_TYPE tErr = MSP_ERROR_NONE;

    MSP_HANDLE hMSP;

    MSP_APPCBPARAM_TYPE *pAppCBParam;

    MSP_BUFHEADER_TYPE *pCurrBufHdr, *pPrevBufHdr;

    MSP_TVS_MTIS_CREATE_PARAM sVStabParam;

    TPstruct *tp = &tpHandle;

    // FILE *out_file;
    MSP_TVS_MTIS_MTISMOTIONPARAMS stMtisMVParams, *pMtisMVParams;

    MSP_TVS_MTIS_MTISOUTPARAMS stMtisOutParams, *pMtisOutParams;

    MSP_U32 ulQueryPtr;

    volatile MSP_U32 overhead = 0;

    MSP_U32 start, CycleCount;

    MSP_U32 ulTestNum;

    MSP_U32 mem_count_start = 0;

    MSP_U32 mem_size_start = 0;

    MSP_U32 mem_count_end = 0;

    MSP_U32 mem_size_end = 0;

    MSP_S32 np;

    MSP_TVS_MTIS_FRAMEPARAMS stFrameParams;

    // MSP_TVS_MTIS_MTISPREVIEWPARAMS stPrvOutParams;

    mem_count_start = TIMM_OSAL_GetMemCounter();
    mem_size_start = TIMM_OSAL_GetMemUsage();
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "\n At the start of testcase: Value from GetMemCounter = %d",
                       mem_count_start);
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "\n Value from GetMemUsage = %d", mem_size_start);

    /* Semaphore param intialization */
    Semaphore_Params_init(&SemParams);
    /* Create a semaphore */
    gSemHandleVstab = Semaphore_create(0, &SemParams, NULL);

    // Initialize various parameters
    tErr = InitParams(&sVStabParam, &tpHandle);
    TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);

    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "Enter a test case to be executed between 0 and 9: \n");
    scanf("%d", &ulTestNum);
    // if(ulTestNum >= 10) goto EXIT;

    pCurrBufHdr =
        (MSP_BUFHEADER_TYPE *) TVS_MTIS_Malloc(sizeof(MSP_BUFHEADER_TYPE),
                                               DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(pCurrBufHdr != NULL, MSP_ERROR_NOTENOUGHRESOURCES);
    pCurrBufHdr->nPortIndex = MSP_TVS_MTIS_INPUT_CURR;

    pPrevBufHdr =
        (MSP_BUFHEADER_TYPE *) TVS_MTIS_Malloc(sizeof(MSP_BUFHEADER_TYPE),
                                               DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(pPrevBufHdr != NULL, MSP_ERROR_NOTENOUGHRESOURCES);
    pPrevBufHdr->nPortIndex = MSP_TVS_MTIS_INPUT_PREV;

    pAppCBParam =
        (MSP_APPCBPARAM_TYPE *) TVS_MTIS_Malloc(sizeof(MSP_APPCBPARAM_TYPE),
                                                DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(pAppCBParam != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    pAppCBParam->MSP_callback = CallBackFromMSP2VstabApp;

    // Get the command line input arguments
    // ProcessArgs(argc, argv, &(vsCon.creationParams), &(vsCon.frameParams), 
    // &tpHandle);
    tpHandle.RO_fileExists = 0;
    tpHandle.SC_fileExists = 0;
    tpHandle.inSeqFile = NULL;
    strcpy(tpHandle.inSeqFileName, INPUT_PATH);
    strcat(tpHandle.inSeqFileName, VstabParam[ulTestNum].inputFile);
    strcpy(tpHandle.outSeqFileName, OUTPUT_PATH);
    strcat(tpHandle.outSeqFileName, VstabParam[ulTestNum].outputFile);
    tpHandle.numFirstFrame = VstabParam[ulTestNum].StartFrameNum;
    tpHandle.numLastFrame = VstabParam[ulTestNum].LastFrameNum;
    tpHandle.frameStep = 1;

    sVStabParam.sBounLines = VstabParam[ulTestNum].BoundaryPixelsV;
    sVStabParam.sBounPels = VstabParam[ulTestNum].BoundaryPixelsH;
    sVStabParam.sMaxSearchAmpV = VstabParam[ulTestNum].SearchRangePixelsV;
    sVStabParam.sMaxSearchAmpH = VstabParam[ulTestNum].SearchRangePixelsH;
    sVStabParam.sVertImgSizeBeforeBSC =
        VstabParam[ulTestNum].VertImgSizeBeforeBSC;
    sVStabParam.sHorzImgSizeBeforeBSC =
        VstabParam[ulTestNum].HorzImgSizeBeforeBSC;
    sVStabParam.sVertImgSizeAfterIPIPE =
        VstabParam[ulTestNum].VertImgSizeAfterIpipe;
    sVStabParam.sHorzImgSizeAfterIPIPE =
        VstabParam[ulTestNum].HorzImgSizeAfterIpipe;

    tErr = MSP_init(&hMSP, "MSP.TVS_MTIS_EST", MSP_PROFILE_HOST, pAppCBParam);
    TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);

    tErr = MSP_open(hMSP, &sVStabParam);
    TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);

    // Initialize the VS process
    // VS_algConfig(&vsCon);
    // VS_memEstimate(&vsCon);
    // VS_init(&vsCon);

    // Check if an error has occured
    /* if (vsCon.creationOutput.errorCode!=0) { #ifndef NO_PRINTF
     * fprintf(stderr,"\nAn error occured! Error code
     * %d\n",vsCon.creationOutput.errorCode); #endif exit(0); } */

    // Initialize the test platform
    InitTP(hMSP, &sVStabParam, &tpHandle);
#ifndef NO_PRINTF
    // out_file = fopen("SISO_MTIS.out", "w");
#endif

    tErr = MSP_control(hMSP, MSP_CTRLCMD_START, NULL);
    TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);

    // Main loop
    while (tpHandle.frameNo <= tpHandle.numLastFrame)
    {

        // Load a new frame to the current frame buffer
        getNewFrame(&tpHandle);

        // Run BSC simulator
        computeBoundarySignals(&tpHandle);

        start = Timestamp_get32();
        overhead = (Timestamp_get32() - start) - overhead;

        start = Timestamp_get32();

        // Motion estimation
        pCurrBufHdr->pBuf[0] = (MSP_U8 *) tpHandle.bscBufferCurr;
        pPrevBufHdr->pBuf[0] = (MSP_U8 *) tpHandle.bscBufferPrev;
        MSP_process(hMSP, NULL, pPrevBufHdr);
        MSP_process(hMSP, NULL, pCurrBufHdr);

        Semaphore_pend(gSemHandleVstab, 10000000);
        CycleCount = (Timestamp_get32() - start) - overhead;
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                           "Execution time in cycles consumed for this frame of motion estimation is %d\n",
                           CycleCount);
        // System_flush();
        // VS_estimateMotion(&vsCon);

        // Motion compensation
        VS_compensateMotion(&VS_FrameOut, &tpHandle);

#ifndef NO_PRINTF
        // fprintf(stdout,"\n-------------------------Frame =
        // %d---------------------------", tpHandle.frameNo);
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                           "\n-------------------------Frame = %d---------------------------\n",
                           tpHandle.frameNo);
        // System_flush();
#endif

        // Sleep(1000);
        // Note: to view all the outputs uncomment //#define DISP_DEBUG 1 in
        // decide.c and enable getchar(); in the following line

        // getchar();
        // if
        // ((vsCon.frameOutput.vertStatus==0)||(vsCon.frameOutput.horzStatus==0)) 
        // getchar();

        // SISO call
        // VS_MTIS_Apply(&vsCon);;

        tErr =
            MSP_query(hMSP, MSP_TVS_MTIS_QUERY_MTISPRVOUTPARAMS,
                      &stFrameParams.stPrvOutParams);
        TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);
        // pMtisOutParams = (MSP_TVS_MTIS_MTISOUTPARAMS*)ulQueryPtr;

        stFrameParams.sCurrentAG = 2048;
        stFrameParams.sCurrentDG = 512;
        stFrameParams.sCurrentExp = 3 * 256;

        tErr =
            MSP_config(hMSP, MSP_TVS_MTIS_INDEX_CURRENTFRAMEPARAMS,
                       (MSP_TVS_MTIS_FRAMEPARAMS *) & stFrameParams);
        TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);

        tErr = MSP_query(hMSP, MSP_TVS_MTIS_QUERY_MTISPARAMS, &stMtisOutParams);
        TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);
        pMtisOutParams = (MSP_TVS_MTIS_MTISOUTPARAMS *) ulQueryPtr;

        // Get Motion vector call (9 pairs)
        // VS_MTIS_GetMV(&vsCon);
        tErr =
            MSP_query(hMSP, MSP_TVS_MTIS_QUERY_MTISMOTIONPARAMS,
                      &stMtisMVParams);
        TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);
        pMtisMVParams = (MSP_TVS_MTIS_MTISMOTIONPARAMS *) ulQueryPtr;

#ifndef NO_PRINTF
        // fprintf(stderr,"\n%d %d",VS_FrameOut.sROh,VS_FrameOut.sROv);
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                           "\n(H,V) = (%d, %d) \n", VS_FrameOut.sROh,
                           VS_FrameOut.sROv);
        for (np = 0; np < 9; np++)
        {
            TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                               "MVv %f  MVh %f\n",
                               ((float) pMtisMVParams->allMVv[np]),
                               ((float) pMtisMVParams->allMVh[np]));
        }

        // System_flush();
#endif
        // Added on Nov 4,2008

#ifndef NO_PRINTF
        // fprintf(stdout,"\n-------------------------Frame =
        // %d---------------------------", tpHandle.frameNo);
        // printf("\nmaxMVv %f maxMVh %f",
        // ((float)vsCon.frameOutput.maxMVv)/(1<<8),
        // ((float)vsCon.frameOutput.maxMVh)/(1<<8));
        // for (np=0;np<9;np++)
        // {
        // printf("\nMVv %f MVh %f",
        // ((float)vsCon.frameOutput.allMVv[np])/(1<<8),
        // ((float)vsCon.frameOutput.allMVh[np])/(1<<8));
        // }
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                           "\nmaxMVv %f  maxMVh %f",
                           ((float) pMtisMVParams->maxMVv) / (1 << 8),
                           ((float) pMtisMVParams->maxMVh) / (1 << 8));
        // printf("\n SISO_Status %d SISO_AG %d SISO_DG %d SISO_Exp %d
        // SISO_Motion %d", (vsCon.frameOutput.SISO_Status),
        // (vsCon.frameOutput.SISO_AG), (vsCon.frameOutput.SISO_DG),
        // (vsCon.frameOutput.SISO_Exp), (vsCon.frameOutput.SISO_Motion));
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                           "\nSISO_Status %f  SISO_AG %f  SISO_DG %f  SISO_Exp %f SISO_Motion %f",
                           ((float) pMtisOutParams->SISO_Status),
                           ((float) pMtisOutParams->SISO_AG),
                           ((float) pMtisOutParams->SISO_DG),
                           ((float) pMtisOutParams->SISO_Exp),
                           ((float) pMtisOutParams->SISO_Motion));
        // getchar();
        // fprintf(out_file, "\nmaxMVv %f maxMVh %f MaxR %d SISO_Status %f
        // SISO_AG %f SISO_DG %f SISO_Exp %f SISO_Motion %f",
        // ((float)pMtisMVParams->maxMVv)/(1<<8),
        // ((float)pMtisMVParams->maxMVh)/(1<<8),
        // pMtisMVParams->Result_Motion,
        // ((float)pMtisOutParams->SISO_Status),
        // ((float)pMtisOutParams->SISO_AG),
        // ((float)pMtisOutParams->SISO_DG),
        // ((float)pMtisOutParams->SISO_Exp),
        // ((float)pMtisOutParams->SISO_Motion));

#endif

        tpHandle.frameNo += tpHandle.frameStep;
    }
    // fclose(out_file);
  EXIT:
    tErr = MSP_close(hMSP);
    // TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);

    tErr |= MSP_deInit(hMSP);
    // TVS_MTIS_ASSERT(tErr == MSP_ERROR_NONE, MSP_ERROR_FAIL);

    if (tpHandle.RO_fileExists == 1)
        writeRO(tpHandle.RO_FileName, tpHandle.ROyHist, tpHandle.ROxHist,
                tpHandle.numLastFrame);
    if (tpHandle.SC_fileExists == 1)
        writeSC(tpHandle.SC_FileName, tpHandle.MVscaleY, tpHandle.MVscaleX,
                tpHandle.numLastFrame);

    /* 
     * #ifdef IS_WINDOWS //Final display if ((tpHandle.inputType &
     * FILE_IO)&&(tpHandle.outputType & FINAL_DISP)) {
     * displayFinalStabSequence(&tpHandle); } #endif */

    // Close the test platform
    CloseTP(&tpHandle);
    TVS_MTIS_Free(tp->frmBuffer);
    TVS_MTIS_Free(tp->frmBufferPrev);
    TVS_MTIS_Free(tp->bscBufferCurr);
    TVS_MTIS_Free(tp->bscBufferPrev);

    TVS_MTIS_Free(tp->interlacedFrame);
    TVS_MTIS_Free(tp->stabFrm);

    TVS_MTIS_Free(tp->ROxHist);
    TVS_MTIS_Free(tp->ROyHist);
    TVS_MTIS_Free(tp->MVxHist);
    TVS_MTIS_Free(tp->MVyHist);
    TVS_MTIS_Free(tp->MVscaleY);
    TVS_MTIS_Free(tp->MVscaleX);
    TVS_MTIS_Free(tp->HPintFrame);

    TVS_MTIS_Free(tp->QPintFrame);
    TVS_MTIS_Free(pCurrBufHdr);
    TVS_MTIS_Free(pPrevBufHdr);
    TVS_MTIS_Free(pAppCBParam);
    mem_count_end = TIMM_OSAL_GetMemCounter();
    mem_size_end = TIMM_OSAL_GetMemUsage();
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "\n Value from GetMemCounter = %d", mem_count_end);
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "\n Value from GetMemUsage = %d", mem_size_end);

    if (mem_count_start != mem_count_end)
    {
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                           "\n Memory leak detected. Bytes lost = %d",
                           (mem_size_end - mem_size_start));
    }
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "\n-----------------------------------------------");
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS, "\n");
    return 0;
}

/*=======================================================================
 *
 * Name: InitParams()
 *
 * Description: Initializes various parameters related with VS input
 *              arguments and the test platform
 *
 * Input:
 *   vsCP: Pointer to the structure for VS input arguments
 *   tp:   Handle for the test platform
 *
 * Returns:
 *
 * Side effects:
 *
 * Notes: The creation parameters are used only once during system
 *        initialization. The frame level parameters can be changed for
 *        each frame.
 *
 =======================================================================*/

/* ===================================================================
 *  @func     InitParams                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
MSP_ERROR_TYPE InitParams(MSP_TVS_MTIS_CREATE_PARAM * vsCP, TPstruct * tp)
{
    MSP_ERROR_TYPE tErr = MSP_ERROR_NONE;

    // VS creation input parameters
    /* vsCP->p_internalScratchMemory = internalScratchMemory;
     * vsCP->internalScratchMemorySize = DEF_INT_SCRATCH_MEMORY_SIZE;
     * vsCP->p_internalPersistentMemory = internalPersistentMemory;
     * vsCP->internalPersistentMemorySize = DEF_INT_PERSISTENT_MEMORY_SIZE;
     * vsCP->p_externalDataMemory = externalDataMemory;
     * vsCP->externalDataMemorySize = DEF_EXT_DATA_MEMORY_SIZE; */
    vsCP->sBounPels = DEF_BOUN_PELS;
    vsCP->sBounLines = DEF_BOUN_LINES;
    vsCP->usKhMin = DEF_KMIN_H;
    vsCP->usKvMin = DEF_KMIN_V;
    vsCP->usKhMax = DEF_KMAX_H;
    vsCP->usKvMax = DEF_KMAX_V;
    vsCP->sMaxSearchAmpV = DEF_MAX_SEARCH_AMP_V;
    vsCP->sMaxSearchAmpH = DEF_MAX_SEARCH_AMP_H;

    vsCP->sVertImgSizeBeforeBSC = DEF_FRAME_VSIZE;
    vsCP->sHorzImgSizeBeforeBSC = DEF_FRAME_HSIZE;
    vsCP->sVertDownsampleRatioInImgSensor = 1;
    vsCP->sHorzDownSampleRatioInImgSensor = 1;
    vsCP->sVertImgSizeAfterIPIPE = DEF_FRAME_VSIZE;
    vsCP->sHorzImgSizeAfterIPIPE = DEF_FRAME_HSIZE;

    // VS frame level input parameters
    /* vsFP->maxMotionCompAmpV = DEF_MAX_MOTION_COMP_AMP_V;
     * vsFP->maxMotionCompAmpH = DEF_MAX_MOTION_COMP_AMP_H; vsFP->disableVS = 
     * 0; */

    // SISO Parameter Initialization
    // SISO Parameter Initialization
    // Creation Parameters (Set by user)
    vsCP->slMax_Blur = 3;                                  // 3 pixels
    vsCP->slMax_AG = 2048;
    vsCP->slMax_DG = 512;
    vsCP->slMin_Exp = 3 * 256;

    // Frame level Parameters
    /* vsFP->Current_AG = 256; vsFP->Current_DG = 256; vsFP->Current_Exp =
     * 51*256; vsFP->frame_period = (1000/30)*256; */
    // TP parameters
    tp->frameRate = DEF_FRAME_RATE;
    tp->inputType = DEF_INPUT_TYPE;
    tp->outputType = DEF_OUTPUT_TYPE;
    tp->numFirstFrame = 0;
    tp->frameStep = 1;
    tp->frmBuffer = frameBuffer1 =
        TVS_MTIS_Malloc(DEF_FRAME_VSIZE * DEF_FRAME_HSIZE * 3 / 2,
                        DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(frameBuffer1 != NULL, MSP_ERROR_NOTENOUGHRESOURCES);
    tp->frmBufferPrev = frameBuffer2 =
        TVS_MTIS_Malloc(DEF_FRAME_VSIZE * DEF_FRAME_HSIZE * 3 / 2,
                        DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(frameBuffer2 != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->bscBufferCurr = bscBuffer1 =
        TVS_MTIS_Malloc(1920 * 4, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(bscBuffer1 != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->bscBufferPrev = bscBuffer2 =
        TVS_MTIS_Malloc(1920 * 4, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(bscBuffer2 != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->interlacedFrame =
        TVS_MTIS_Malloc(MAX_FRAME_SIZE_FOR_DISPLAY_V *
                        MAX_FRAME_SIZE_FOR_DISPLAY_H * 3 / 2,
                        DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->interlacedFrame != NULL, MSP_ERROR_NOTENOUGHRESOURCES);
    tp->stabFrm =
        TVS_MTIS_Malloc(MAX_FRAME_SIZE_FOR_DISPLAY_V *
                        MAX_FRAME_SIZE_FOR_DISPLAY_H * 3 / 2,
                        DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->stabFrm != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->ROxHist = TVS_MTIS_Malloc(4000, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->ROxHist != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->ROyHist = TVS_MTIS_Malloc(4000, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->ROyHist != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->MVxHist = TVS_MTIS_Malloc(4000, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->MVxHist != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->MVyHist = TVS_MTIS_Malloc(4000, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->MVyHist != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->MVscaleY = TVS_MTIS_Malloc(4000, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->MVscaleY != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->MVscaleX = TVS_MTIS_Malloc(4000, DEFAULT_BYTE_ALIGN);
    TVS_MTIS_ASSERT(tp->MVscaleX != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->HPintFrame = TVS_MTIS_Malloc(1843200, DEFAULT_BYTE_ALIGN);  // [480*640*4*3/2];
    TVS_MTIS_ASSERT(tp->HPintFrame != NULL, MSP_ERROR_NOTENOUGHRESOURCES);

    tp->QPintFrame = TVS_MTIS_Malloc(7372800, DEFAULT_BYTE_ALIGN);  // [480*640*16*3/2];
    TVS_MTIS_ASSERT(tp->QPintFrame != NULL, MSP_ERROR_NOTENOUGHRESOURCES);
  EXIT:
    return tErr;
}

/*=======================================================================
 *
 * Name: ProcessArgs()
 *
 * Description: Gets the command line arguments
 *
 * Input:
 *   vsCP: Pointer to the structure for VS input arguments
 *   tp:   Handle for the test platform
 *
 * Returns:
 *
 * Side effects:
 *
 =======================================================================*/
#if 0
/* ===================================================================
 *  @func     ProcessArgs                                               
 *                                                                    
 *  @desc     Function does the following                             
 *                                                                    
 *  @modif    This function modifies the following structures         
 *                                                                    
 *  @inputs   This function takes the following inputs                
 *            <argument name>                                         
 *            Description of usage                                    
 *            <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @outputs  <argument name>                                         
 *            Description of usage                                    
 *                                                                    
 *  @return   Return value of this function if any                    
 *  ==================================================================
 */                                                                   
void ProcessArgs(int argc, char *argv[], VS_CreationParamsStruct * vsCP,
                 VS_FrameParamsStruct * vsFP, TPstruct * tp)
// Gets the command line input arguments
{
    Word32 i, requiredArgs[MAX_NUM_OF_INPUT_ARGS];

    // Choose which arguments are required input arguments
    for (i = 0; i < MAX_NUM_OF_INPUT_ARGS; i++)
        requiredArgs[i] = 0;
    requiredArgs[0] = 1;                                   // i
    requiredArgs[1] = 1;                                   // o
    requiredArgs[2] = 1;                                   // l
    tpHandle.RO_fileExists = 0;
    tpHandle.SC_fileExists = 0;

    for (i = 1; i < argc; i++)
    {
        if (*(argv[i]) == '-')
        {
            switch (*(++argv[i]))
            {
                case 'i':
                    strcpy(tpHandle.inSeqFileName, argv[++i]);
                    requiredArgs[0] = 0;
                    break;
                case 'o':
                    strcpy(tpHandle.outSeqFileName, argv[++i]);
                    requiredArgs[1] = 0;
                    break;
                case 'f':
                    tp->numFirstFrame = atoi(argv[++i]);
                    break;
                case 'l':
                    tp->numLastFrame = atoi(argv[++i]);
                    requiredArgs[2] = 0;
                    break;
                case 'S':
                    tp->frameStep = atoi(argv[++i]) + 1;
                    break;
                case 'b':
                    switch (*(argv[i] + 1))
                    {
                        case 'p':
                            vsCP->bounPels = atoi(argv[++i]);
                            break;
                        case 'l':
                            vsCP->bounLines = atoi(argv[++i]);
                            break;
                    }
                    break;
                case 'n':
                    switch (*(argv[i] + 1))
                    {
                        case 'R':
                            strcpy(tpHandle.RO_FileName, argv[++i]);
                            tpHandle.RO_fileExists = 1;
                            break;
                        case 'S':
                            strcpy(tpHandle.SC_FileName, argv[++i]);
                            tpHandle.SC_fileExists = 1;
                            break;
                    }
                    break;
            }
        }
    }

    // Determine whether any required argument is missing
    for (i = 0; i < MAX_NUM_OF_INPUT_ARGS; i++)
    {
        if (requiredArgs[i] != 0)
        {
#ifndef NO_PRINTF
            fprintf(stderr, "\nA required input argument is missing!");
#endif
            exit(-1);
        }
    }

    return;
}
#endif
