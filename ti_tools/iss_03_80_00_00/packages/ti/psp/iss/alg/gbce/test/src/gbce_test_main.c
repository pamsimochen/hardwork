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
 * This File is the test-bench for testing MSP GBCE component.
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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/ipc/Semaphore.h>
#include "../../../../framework/msp/msp.h"
#include "gbce_test_main.h"

#define THIRTYTWO_BYTE_ALIGN 32
#define DEFAULT_BYTE_ALIGN 4
#define SIXTEEN_BYTE_ALIGN 16
#define __PROFILE_GBCE__

/* Function to update GBCE test parameters */
/* ===================================================================
 *  @func     Update_TestParams_GBCE                                               
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
void Update_TestParams_GBCE(MSP_GBCE_CREATE_PARAMS * pApp_params,
                            MSP_U8 testnum)
{

    pApp_params->OperateMode = MSP_GBCE_CAPTURE_MODE;
    pApp_params->TableSize = GBCE_TestStructure[testnum].TableSize;

    pApp_params->UseInputGammaTable = MSP_FALSE;
    // pApp_params->SetDefaultParams = MSP_TRUE;
    pApp_params->StregnthDark = MSP_GBCE_STRENGTH_1;
    pApp_params->StregnthIndoor = MSP_GBCE_STRENGTH_1;
    pApp_params->StregnthOutdoor = MSP_GBCE_STRENGTH_1;
}

/* Function to allocate memory for test buffers */
/* ===================================================================
 *  @func     GBCE_Allocate_TestBuffers                                               
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
void GBCE_Allocate_TestBuffers(MSP_U8 testnum, MSP_BUFHEADER_TYPE * inbuf,
                               MSP_BUFHEADER_TYPE * outbuf,
                               MSP_GBCE_CREATE_PARAMS * pParam)
{
    MSP_GBCE_RUNTIME_OUTPUT_PARAMS *ptr = NULL;

    inbuf->unBufSize[0] = inbuf->unBufSize[1] = inbuf->unBufSize[2] = 0;
    outbuf->unBufSize[0] = outbuf->unBufSize[1] = outbuf->unBufSize[2] = 0;

    /* Allocate for the default table being returned by the GBCE algo */
    pParam->InputGammaTable =
        (MSP_S32 *) TIMM_OSAL_MallocExtn(1024 * sizeof(MSP_S32), TIMM_OSAL_TRUE,
                                         SIXTEEN_BYTE_ALIGN,
                                         TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_GBCE_EXIT_IF(pParam->InputGammaTable == NULL, MSP_ERROR_NULLPTR);

    /* Allocate input buffer */
    inbuf->nPortIndex = 0;
    inbuf->unNumOfCompBufs = 1;
    inbuf->unBufSize[0] = (MSP_U32) (256 * sizeof(MSP_S32));
    inbuf->pBuf[0] =
        (MSP_U8 *) TIMM_OSAL_MallocExtn(inbuf->unBufSize[0], TIMM_OSAL_TRUE,
                                        THIRTYTWO_BYTE_ALIGN,
                                        TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_GBCE_EXIT_IF(inbuf->pBuf[0] == NULL, MSP_ERROR_NULLPTR);
    inbuf->unBufSize[1] = 0;
    inbuf->unBufSize[2] = 0;

    inbuf->pAppBufPrivate =
        (MSP_GBCE_RUNTIME_INPUT_PARAMS *)
        TIMM_OSAL_MallocExtn(sizeof(MSP_GBCE_RUNTIME_INPUT_PARAMS),
                             TIMM_OSAL_TRUE, DEFAULT_BYTE_ALIGN,
                             TIMMOSAL_MEM_SEGMENT_EXT, NULL);

    /* Allocate the output buffers */
    outbuf->nPortIndex = 1;
    outbuf->unNumOfCompBufs = 3;
    outbuf->unBufSize[0] = (MSP_U32) (1024 * sizeof(MSP_S32));
    outbuf->unBufSize[1] = (MSP_U32) (1024 * sizeof(MSP_S32));
    outbuf->unBufSize[2] = (MSP_U32) (1024 * sizeof(MSP_S32));

    /* GBCETable (combined Gamma and Tone Table) */
    outbuf->pBuf[0] =
        (MSP_U8 *) TIMM_OSAL_MallocExtn(outbuf->unBufSize[0], TIMM_OSAL_TRUE,
                                        SIXTEEN_BYTE_ALIGN,
                                        TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_GBCE_EXIT_IF(outbuf->pBuf[0] == NULL, MSP_ERROR_NULLPTR);

    /* GammaTable (separate Gamma Table) */
    outbuf->pBuf[1] =
        (MSP_U8 *) TIMM_OSAL_MallocExtn(outbuf->unBufSize[1], TIMM_OSAL_TRUE,
                                        SIXTEEN_BYTE_ALIGN,
                                        TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_GBCE_EXIT_IF(outbuf->pBuf[1] == NULL, MSP_ERROR_NULLPTR);

    /* Tobe Table (separate Tone Table) */
    outbuf->pBuf[2] =
        (MSP_U8 *) TIMM_OSAL_MallocExtn(outbuf->unBufSize[2], TIMM_OSAL_TRUE,
                                        SIXTEEN_BYTE_ALIGN,
                                        TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_GBCE_EXIT_IF(outbuf->pBuf[2] == NULL, MSP_ERROR_NULLPTR);

    ptr = outbuf->pAppBufPrivate =
        (MSP_GBCE_RUNTIME_OUTPUT_PARAMS *)
        TIMM_OSAL_MallocExtn(sizeof(MSP_GBCE_RUNTIME_OUTPUT_PARAMS),
                             TIMM_OSAL_TRUE, DEFAULT_BYTE_ALIGN,
                             TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_GBCE_EXIT_IF(ptr == NULL, MSP_ERROR_NULLPTR);

    /* Fill up these buffers with default values */

    TIMM_OSAL_Memset(pParam->InputGammaTable, 0xBF, 1024 * 4);

    TIMM_OSAL_Memset(inbuf->pBuf[0], 0xAB, 256 * 4);

    TIMM_OSAL_Memset(outbuf->pBuf[0], 0xCD, 1024 * 4);
    TIMM_OSAL_Memset(outbuf->pBuf[1], 0xEF, 1024 * 4);
    TIMM_OSAL_Memset(outbuf->pBuf[2], 0xBD, 1024 * 4);

    return;

  EXIT:
    TIMM_OSAL_TraceFunction("\n Buffer allocation in test-case failed\n");
}

/* ===================================================================
 *  @func     MSP_GBCE_Callback                                               
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
static MSP_ERROR_TYPE MSP_GBCE_Callback(MSP_PTR hMSP,
                                        MSP_PTR pAppData,
                                        MSP_EVENT_TYPE tEvent,
                                        MSP_OPAQUE nEventData1,
                                        MSP_OPAQUE nEventData2)
{

    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    switch (tEvent)
    {
        case MSP_OPEN_EVENT:
        case MSP_CLOSE_EVENT:
        case MSP_PROFILE_EVENT:
        case MSP_CTRLCMD_EVENT:
        case MSP_ERROR_EVENT:
            break;

        case MSP_DATA_EVENT:
            if (nEventData1 == MSP_GBCE_INPUT_PORT)
            {
                break;
            }

            if (nEventData1 == MSP_GBCE_OUTPUT_PORT)
            {
                break;
            }

        default:
            status = MSP_ERROR_INVALIDCMD;
            break;

    }
    return status;
}

#ifdef CCS_PROJECT

/* ===================================================================
 *  @func     main                                               
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
int main()
#else

/* ===================================================================
 *  @func     gbce_test_main                                               
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
void gbce_test_main()
#endif
{
    MSP_S32 i = 0;

    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    MSP_APPCBPARAM_TYPE pGbce_appcbptr;

    MSP_HANDLE hNsfMSP;

    MSP_GBCE_CREATE_PARAMS *pGbceQueryParam;

    MSP_BUFHEADER_TYPE inBuffGbce, outBuffGbce;

    MSP_GBCE_RUNTIME_OUTPUT_PARAMS *pOutputParam = NULL;

    MSP_GBCE_RUNTIME_INPUT_PARAMS *pInputParam = NULL;

    char fin_name[200];

    char fout_Gamma_name[200], fout_GBCE_name[200];

    FILE *fp_inp, *fp_outp_GBCE, *fp_outp_Gamma;

    MSP_S8 testnum = 1;

    MSP_U32 input_filesize = 0;

    MSP_U32 mem_count_start = 0;

    MSP_U32 mem_size_start = 0;

    MSP_U32 mem_count_end = 0;

    MSP_U32 mem_size_end = 0;

    Memory_Stats stats;

#ifdef __PROFILE_GBCE__
    unsigned volatile long long overhead = 0;

    unsigned long long start;

    unsigned long long CycleCount = 0;
#endif

    Memory_getStats(NULL, &stats);
    mem_count_start = TIMM_OSAL_GetMemCounter();
    mem_size_start = TIMM_OSAL_GetMemUsage();

    // Allocate the structure for MSP OPEN call configuration
    pGbceQueryParam =
        (MSP_GBCE_CREATE_PARAMS *)
        TIMM_OSAL_MallocExtn(sizeof(MSP_GBCE_CREATE_PARAMS), TIMM_OSAL_TRUE,
                             DEFAULT_BYTE_ALIGN, TIMMOSAL_MEM_SEGMENT_EXT,
                             NULL);
    MSP_GBCE_EXIT_IF(pGbceQueryParam == NULL, MSP_ERROR_NULLPTR);

    TIMM_OSAL_TraceFunction("\n Enter the test number of GBCE \n");
    scanf("%d", &testnum);

    if (testnum <= 0 || testnum > MAX_GBCE_TEST_NUM)
    {
        TIMM_OSAL_TraceFunction
            ("\n This is an invalid test number. Enter a valid test number\n");
        status = MSP_ERROR_FATAL;
        MSP_GBCE_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FATAL);

    }
    testnum = testnum - 1;

    /* Open the file for input */
    strcpy(fin_name, INPUT_PATH);
    strcat(fin_name, GBCE_TestStructure[testnum].HistFileName);

    strcpy(fout_GBCE_name, OUTPUT_PATH);
    strcat(fout_GBCE_name, GBCE_TestStructure[testnum].GBCEFileName);

    strcpy(fout_Gamma_name, OUTPUT_PATH);
    strcat(fout_Gamma_name, GBCE_TestStructure[testnum].GammaFileName);

    // Update the pGbceQueryParam structure with test-paramaters
    Update_TestParams_GBCE(pGbceQueryParam, testnum);

    fp_inp = fopen(fin_name, "rb");
    if (fp_inp == NULL)
    {
        TIMM_OSAL_TraceFunction("Error opening input file:  %s\n", fin_name);
        MSP_GBCE_EXIT_IF(fp_inp == NULL, MSP_ERROR_FAIL);
    }

    GBCE_Allocate_TestBuffers(testnum, &inBuffGbce, &outBuffGbce,
                              pGbceQueryParam);
    pOutputParam =
        (MSP_GBCE_RUNTIME_OUTPUT_PARAMS *) (outBuffGbce.pAppBufPrivate);
    pInputParam = (MSP_GBCE_RUNTIME_INPUT_PARAMS *) (inBuffGbce.pAppBufPrivate);
    input_filesize = 256;

    for (i = 0; i < input_filesize; i++)
    {
        fscanf(fp_inp, "%d", inBuffGbce.pBuf[0] + 4 * i);
    }

    fclose(fp_inp);

    /* Update the run-time parameters */
    pInputParam->AnalogGain = GBCE_TestStructure[testnum].AnalogGain;
    pInputParam->DigitalGain = GBCE_TestStructure[testnum].DigitalGain;
    pInputParam->ExposureTime = GBCE_TestStructure[testnum].Exposure_Time;
    pInputParam->Aperture = GBCE_TestStructure[testnum].Aperture;

    pGbce_appcbptr.MSP_callback = MSP_GBCE_Callback;

    /* Allocate the Input and Output Buffers */
    status =
        MSP_init(&hNsfMSP, "MSP.GBCE", MSP_PROFILE_REMOTE, &pGbce_appcbptr);
    MSP_GBCE_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    status = MSP_open(hNsfMSP, (MSP_PTR *) pGbceQueryParam);
    MSP_GBCE_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    /* Write out the default gamma table output in file */
    fp_outp_Gamma = fopen(fout_Gamma_name, "wb");
    if (fp_outp_Gamma == NULL)
    {
        TIMM_OSAL_TraceFunction("error opening output file  file %s\n",
                                fp_outp_Gamma);
        MSP_GBCE_EXIT_IF(fp_outp_Gamma == NULL, MSP_ERROR_FAIL);
    }

    for (i = 0; i < 1024; i++)
    {
        fprintf(fp_outp_Gamma, "%d\n", *(pGbceQueryParam->InputGammaTable + i));
        fflush(fp_outp_Gamma);
    }
    fclose(fp_outp_Gamma);

    status = MSP_process(hNsfMSP, NULL, &inBuffGbce);
    MSP_GBCE_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

#ifdef __PROFILE_GBCE__
    start = Timestamp_get32();
    overhead = (Timestamp_get32() - start) - overhead;
    start = Timestamp_get32();
#endif
    status = MSP_process(hNsfMSP, NULL, &outBuffGbce);
    MSP_GBCE_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

#ifdef __PROFILE_GBCE__
    CycleCount = (Timestamp_get32() - start) - overhead;
    TIMM_OSAL_TraceFunction("Exectuion time in cycles consumed is %llu \n",
                            CycleCount);
    System_flush();
#endif

    TIMM_OSAL_TraceFunction("\n GBCE Processing done \n");

    MSP_GBCE_EXIT_IF((MSP_close(hNsfMSP)) != MSP_ERROR_NONE, MSP_ERROR_FAIL);
    MSP_GBCE_EXIT_IF((MSP_deInit(hNsfMSP)) != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    /* Write out the output in file */
    fp_outp_GBCE = fopen(fout_GBCE_name, "wb");
    if (fp_outp_GBCE == NULL)
    {
        TIMM_OSAL_TraceFunction("error opening output file  file %s\n",
                                fout_GBCE_name);
        MSP_GBCE_EXIT_IF(fp_outp_GBCE == NULL, MSP_ERROR_FAIL);
    }

    for (i = 0; i < 1024; i++)
    {
        fprintf(fp_outp_GBCE, "%d\n", *((MSP_S32 *) (outBuffGbce.pBuf[0]) + i));
        fflush(fp_outp_GBCE);
    }
    fclose(fp_outp_GBCE);

    TIMM_OSAL_Free(outBuffGbce.pBuf[0]);
    TIMM_OSAL_Free(outBuffGbce.pBuf[1]);
    TIMM_OSAL_Free(outBuffGbce.pBuf[2]);
    TIMM_OSAL_Free(inBuffGbce.pBuf[0]);
    TIMM_OSAL_Free(inBuffGbce.pAppBufPrivate);
    TIMM_OSAL_Free(pOutputParam);
    TIMM_OSAL_Free(pGbceQueryParam->InputGammaTable);
    TIMM_OSAL_Free(pGbceQueryParam);

    Memory_getStats(NULL, &stats);
    mem_count_end = TIMM_OSAL_GetMemCounter();
    mem_size_end = TIMM_OSAL_GetMemUsage();

    if (mem_count_start != mem_count_end)
    {
        TIMM_OSAL_TraceFunction("\n Memory leak detected. Bytes lost = %d",
                                (mem_size_end - mem_size_start));
    }
    else
    {
        TIMM_OSAL_TraceFunction("\n NO MEMORY LEAKS..!");
    }

    TIMM_OSAL_TraceFunction("\n GBCE test finished\n");
    return;

  EXIT:
    TIMM_OSAL_TraceFunction("Something went wrong!!\n");
    return;
}
