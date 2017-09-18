/* ======================================================================= *
 * Texas Instruments OMAP(TM) Platform Software * (c) Copyright 2008 Texas
 * Instruments Incorporated. All Rights Reserved. * * Use of this software is 
 * controlled by the terms and conditions found * in the license agreement
 * under which this software has been supplied. *
 * ======================================================================== */
/**
* @file Jpeg_test.c
*
* This file contains test wrapper api's for Jpeg Encode on SIMCOP in OMAP4/Monica
*
* @path code\test
*
* @rev 1.0
*/
/*========================================================================
*!
*! Revision History
*! ===================================
*! 23-Sep 2008 Sowmya Priya: Initial Release
*!
*========================================================================= */
#include "jpeg_test_dec.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ti/iss/drivers/csl/iss/simcop/common/jpeg_tables.h>

#include <ti/iss/alg/jpeg_dec/msp_jpeg_dec.h>
#include <ti/iss/framework/msp/msp.h>

#include <ti/timmosal/timm_osal_interfaces.h>
#include <ti/timmosal/timm_osal_trace.h>
// #include <ti/sysbios/hal/unicache/Cache.h>

#define __INTERRUPT_CHANGES__
// #define __RM_CHANGES__

#ifdef __PROFILING_CHANGES__
#include <xdc/runtime/Timestamp.h>
#endif

#ifdef __INTERRUPT_CHANGES__
#include <ti/iss/drivers/csl/iss/iss_common/iss_common.h>
#include <ti/iss/drivers/csl/iss/simcop/common/simcop_irq.h>
#endif

// #ifdef __RM_CHANGES__
#include <ti/iss/framework/resource_manager/rm.h>
// #endif

extern MSP_TEST_CASE_ENTRY MSP_JPEGDTestCaseTable[];

extern long THC_GetTestNum();

/* ===================================================================
 *  @func     Test_MemStatPrint                                               
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
TIMM_OSAL_U32 Test_MemStatPrint(void);

/* ===================================================================
 *  @func     msp_jpegd_dummy_callback                                               
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
MSP_ERROR_TYPE msp_jpegd_dummy_callback(MSP_PTR hMSP,
                                        MSP_PTR pAppData,
                                        MSP_EVENT_TYPE tEvent,
                                        MSP_OPAQUE nEventData1,
                                        MSP_OPAQUE nEventData2)
{
    return MSP_ERROR_NONE;
}

/* ===================================================================
 *  @func     rearrange                                               
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
void rearrange(Uint16 nMcus, unsigned char *data_in, unsigned char *dataout,
               Uint16 * iwt, Uint16 * iht)
{
    Uint16 i, j, k, l, m, iwp, ihp;

    unsigned char *temp;

    iwp = *iwt;
    ihp = *iht;
    temp = data_in;

    for (i = 0; i < ihp; i += 8)                           /* process for
                                                            * each MCU row */
    {
        for (j = 0; j < iwp; j += 8 * nMcus)               /* process for
                                                            * each MCU */
        {
            for (k = 0; k < nMcus; k++)
            {
                for (l = 0; l < 8; l++)
                {
                    for (m = 0; m < 8; m++)
                    {
                        dataout[i * iwp + j + iwp * l + m + 8 * k]
                            = temp[k * 64 + l * 8 + m];
                    }
                }
            }
            temp += (64 * nMcus);
        }
    }
}

/* ===================================================================
 *  @func     Jpegd_MemStatPrint                                               
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
TIMM_OSAL_U32 Jpegd_MemStatPrint(void)
{

    Memory_Stats stats;

    TIMM_OSAL_U32 mem_count = 0;

    TIMM_OSAL_U32 mem_size = 0;

    // Get Memory status before running the test case
    mem_count = TIMM_OSAL_GetMemCounter();
    mem_size = TIMM_OSAL_GetMemUsage();

    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "Value from GetMemCounter = %d", mem_count);
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Value from GetMemUsage = %d",
                       mem_size);

    Memory_getStats(NULL, &stats);
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "TotalSize = %d; TotalFreeSize = %d; LargetFreeSize = %d",
                       stats.totalSize, stats.totalFreeSize,
                       stats.largestFreeSize);

    return mem_size;
}

/* ===================================================================
 *  @func     Jpeg_test_dec                                               
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
void Jpeg_test_dec()
{
    FILE *finp, *foup;

    char fin_name[100], fout_name[100];

    MSP_TEST_CASE_ENTRY *testcaseEntry;

    MSP_APPCBPARAM_TYPE pCbptr;

    MSP_HANDLE hMSP;

    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    MSP_U32 *pISSRegs = (MSP_U32 *) ISS_BASE_ADDRESS;

    MSP_U16 iw, ih, yuv_mode, iw_temp, ih_temp;

    MSP_U32 bssize, slice_idx, no_of_slices;

    MSP_S32 slice_mode, convert_420to422;

    char slice_ext[20];

    MSP_JPEG_DEC_CREATE_PARAM pQueryParam;

    MSP_JPEG_DEC_SLICE_PARAM pSliceParam;

    MSP_JPEGD_OUTPUT_TYPE pOutFormat;

    MSP_BUFHEADER_TYPE inBuff, outBuff;

    UINT8 *FinalOp, *y_ptr, *cb_ptr, *cr_ptr, *data_in;

    MSP_U32 test_case;

    MSP_S32 test_case_start, test_case_end;

    MSP_U32 mem_size_start = 0;

    MSP_U32 mem_size_end = 0;

#ifdef __PROFILING_CHANGES__
    volatile MSP_U32 overhead = 0;

    MSP_U32 start, CycleCount;

    MSP_U32 TraceGrp;

    TraceGrp = TIMM_OSAL_TRACEGRP_SYSTEM;                  // Disable all
                                                           // traces other
                                                           // than System
                                                           // level ones
    TIMM_OSAL_SetTraceGrp(TraceGrp);

    start = Timestamp_get32();
    overhead = (Timestamp_get32() - start) - overhead;
#endif

    // Get Memory status before running the test case
    mem_size_start = Jpegd_MemStatPrint();

    /* SWITCH ON THE SIMCOP CLK IN ISS REGISTER */
    *(pISSRegs + (0x84 >> 2)) |= 0x1;

#ifdef __INTERRUPT_CHANGES__
    iss_init();
    status = simcop_common_init();
    if (status != CSL_SOK)
    {
        TIMM_OSAL_TraceFunction
            ("Error occured while trying to initialize the interrupt manager");
        exit(-1);
    }
#endif

    // #ifdef __RM_CHANGES__
    RM_Init();
    // #endif

#ifdef SCANF_WORKAROUND
    test_case_start = THC_GetTestNum();
    if (test_case_start == -1)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Error in input!! exit");
        System_exit(0);
    }
    TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Test Start Number : %d",
                      test_case_start);
#else                                                      /* SCANF_WORKAROUND 
                                                            */
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "\r\n Select test case start ID (1-45):");
    scanf("%d", &test_case_start);
#endif

#ifdef SCANF_WORKAROUND
    test_case_end = THC_GetTestNum();
    if (test_case_end == -1)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Error in input!! exit");
        goto EXIT;
    }
    TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Test Start Number : %d",
                      test_case_end);
#else                                                      /* SCANF_WORKAROUND 
                                                            */
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "Select test case end ID (1-45):");
    scanf("%d", &test_case_end);
#endif

    if (test_case_end < test_case_start || test_case_start < 1 ||
        test_case_end > 45)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "Invalid test ID range selection.");
        goto EXIT;
    }

#ifdef SCANF_WORKAROUND
    slice_mode = THC_GetTestNum();
    if (slice_mode == -1)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Error in input!! exit");
        goto EXIT;
    }
    TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Slice Mode? : %d",
                      slice_mode);
#else                                                      /* SCANF_WORKAROUND 
                                                            */
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "Decode in Slice mode? (0: Frame and 1: Slice)");
    scanf("%d", &slice_mode);
#endif

#ifdef SCANF_WORKAROUND
    convert_420to422 = THC_GetTestNum();
    if (convert_420to422 == -1)
    {
        TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SYSTEM, "Error in input!! exit");
        goto EXIT;
    }
    TIMM_OSAL_InfoExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                      "YUV 420 to 422 Convertion? : %d", convert_420to422);
#else                                                      /* SCANF_WORKAROUND 
                                                            */
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "YUV 420 to 422 Conversion? (0: No and 1: Yes)");
    scanf("%d", &convert_420to422);
#endif

    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "-----------------------------------------------");
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "JPEG Decoder Test case begin");

    for (test_case = (test_case_start - 1); test_case < test_case_end;
         test_case++)
    {
        testcaseEntry = &MSP_JPEGDTestCaseTable[test_case];
        strcpy(fin_name, INPUT_PATH);
        strcat(fin_name, testcaseEntry->pInputImage);
        // strcat(fin_name, INPUT_IMAGE);

        finp = fopen(fin_name, "rb");
        if (finp == NULL)
        {
            TIMM_OSAL_TraceFunction("error opening i/p file");
            status = MSP_ERROR_NOTENOUGHRESOURCES;
            goto EXIT;
        }

#ifdef __PROFILING_CHANGES__
        start = Timestamp_get32();
#endif

        fseek(finp, 0, SEEK_END);
        bssize = ftell(finp);
        fseek(finp, 0, SEEK_SET);

        inBuff.pBuf[0] = (MSP_U8 *) TIMM_OSAL_MallocExtn(bssize, TIMM_OSAL_TRUE,
                                                         16,
                                                         TIMMOSAL_MEM_SEGMENT_EXT,
                                                         NULL);
        JPEG_EXIT_IF(NULL == inBuff.pBuf[0], MSP_ERROR_FAIL);
        inBuff.unBufSize[0] = bssize;
        inBuff.nPortIndex = 0;

        fread(inBuff.pBuf[0], 1, bssize, finp);
        // Cache_wb(inBuff.pBuf[0], bssize, Cache_Type_ALL, TRUE);
        fclose(finp);

#ifdef __PROFILING_CHANGES__
        CycleCount = (Timestamp_get32() - start) - overhead;
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "Input File read: Execution time in cycles consumed is %d",
                           CycleCount);
#endif

#ifdef __PROFILING_CHANGES__
        start = Timestamp_get32();
#endif

        pCbptr.MSP_callback = msp_jpegd_dummy_callback;
        MSP_init(&hMSP, "MSP.JPEGDEC", MSP_PROFILE_REMOTE, &pCbptr);
        status = MSP_open(hMSP, (void *) &inBuff);
        JPEG_EXIT_IF((status != MSP_ERROR_NONE) ||
                     (hMSP == NULL), MSP_ERROR_FAIL);
        status =
            MSP_query(hMSP,
                      (MSP_INDEXTYPE) MSP_JPEG_DEC_INDEXTYPE_CREATE_PARAMS,
                      (MSP_PTR) & pQueryParam);
        JPEG_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        iw = pQueryParam.ulImageWidth;
        ih = pQueryParam.ulImageHeight;
        yuv_mode = pQueryParam.Format;

        if (slice_mode == 1)
        {
            pSliceParam.eUnComprsdOpMode = MSP_JPEG_DEC_SLICE_MODE;
            if (yuv_mode == 420)
                pSliceParam.ulSliceHeight = 16;
            else
                pSliceParam.ulSliceHeight = 8;
            no_of_slices = ih / pSliceParam.ulSliceHeight;
            ih = pSliceParam.ulSliceHeight;                // Use buffers of
                                                           // height = slice
                                                           // height for
                                                           // slice mode
        }
        else
        {
            pSliceParam.eUnComprsdOpMode = MSP_JPEG_DEC_FRAME_MODE;
            pSliceParam.ulSliceHeight = 0;
            no_of_slices = 1;
        }
        status =
            MSP_config(hMSP,
                       (MSP_INDEXTYPE) MSP_JPEG_DEC_INDEXTYPE_SLICE_PARAMS,
                       (MSP_PTR) & pSliceParam);
        JPEG_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        if (convert_420to422 == 1)
        {
            pOutFormat = MSP_JPEGD_TO_YUV422I_FORMAT;
            status =
                MSP_config(hMSP,
                           (MSP_INDEXTYPE)
                           MSP_JPEG_DEC_INDEXTYPE_SET_OUTPUT_FMT,
                           (MSP_PTR) & pOutFormat);
            JPEG_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
            yuv_mode = 422;
        }

#ifdef __PROFILING_CHANGES__
        CycleCount = (Timestamp_get32() - start) - overhead;
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "MSP init and open: Execution time in cycles consumed is %d",
                           CycleCount);
#endif

#ifdef __PROFILING_CHANGES__
        start = Timestamp_get32();
#endif

        /* Allocate output buffer */
        if ((yuv_mode == 420) && (pQueryParam.Isplanar == FALSE))
        {
            outBuff.pBuf[0] = (MSP_U8 *) TIMM_OSAL_MallocExtn(iw * ih,
                                                              TIMM_OSAL_TRUE,
                                                              16,
                                                              TIMMOSAL_MEM_SEGMENT_EXT,
                                                              NULL);
            outBuff.unMaxBufSize[0] = iw * ih;
            // Cache_inv(outBuff.pBuf[0], iw * ih, Cache_Type_ALL, TRUE);
            outBuff.pBuf[1] = (MSP_U8 *) TIMM_OSAL_MallocExtn(iw * ih * 0.5,
                                                              TIMM_OSAL_TRUE,
                                                              16,
                                                              TIMMOSAL_MEM_SEGMENT_EXT,
                                                              NULL);
            outBuff.unMaxBufSize[1] = iw * ih * 0.5;
            // Cache_inv(outBuff.pBuf[1], iw * ih * 0.5, Cache_Type_ALL,
            // TRUE);
        }
        else if ((yuv_mode == 420) && (pQueryParam.Isplanar == TRUE))
        {
            outBuff.pBuf[0] = (MSP_U8 *) TIMM_OSAL_MallocExtn(iw * ih * 1.5,
                                                              TIMM_OSAL_TRUE,
                                                              16,
                                                              TIMMOSAL_MEM_SEGMENT_EXT,
                                                              NULL);
            outBuff.unMaxBufSize[0] = iw * ih * 1.5;
            // Cache_inv(outBuff.pBuf[0], iw * ih * 1.5, Cache_Type_ALL,
            // TRUE);
        }
        else if (yuv_mode == 422)
        {
            outBuff.pBuf[0] = (MSP_U8 *) TIMM_OSAL_MallocExtn(iw * ih * 2, TIMM_OSAL_TRUE, 16, TIMMOSAL_MEM_SEGMENT_EXT, NULL); // OutBuf;
            outBuff.unMaxBufSize[0] = iw * ih * 2;
            // Cache_inv(outBuff.pBuf[0], iw * ih * 2, Cache_Type_ALL, TRUE);
        }
        else                                               // if(yuv_mode ==
                                                           // 444)
        {
            outBuff.pBuf[0] = (MSP_U8 *) TIMM_OSAL_MallocExtn(iw * ih * 3,
                                                              TIMM_OSAL_TRUE,
                                                              16,
                                                              TIMMOSAL_MEM_SEGMENT_EXT,
                                                              NULL);
            outBuff.unMaxBufSize[0] = iw * ih * 3;
            // Cache_inv(outBuff.pBuf[0], iw * ih * 3, Cache_Type_ALL, TRUE);
        }
        JPEG_EXIT_IF(NULL == outBuff.pBuf[0], MSP_ERROR_FAIL);

#ifdef __PROFILING_CHANGES__
        CycleCount = (Timestamp_get32() - start) - overhead;
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "Output buffer allocation: Execution time in cycles consumed is %d",
                           CycleCount);
#endif

        inBuff.nPortIndex = 0;
        status = MSP_process(hMSP, NULL, &inBuff);
        JPEG_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        // /////// REPEAT FOR EVERY SLICE
        for (slice_idx = 1; slice_idx <= no_of_slices; slice_idx++)
        {

#ifdef __PROFILING_CHANGES__
            start = Timestamp_get32();
#endif

            outBuff.nPortIndex = 1;
            status = MSP_process(hMSP, NULL, &outBuff);
            JPEG_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

#ifdef __PROFILING_CHANGES__
            CycleCount = (Timestamp_get32() - start) - overhead;
            TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                               "MSP Process: Execution time in cycles consumed is %d",
                               CycleCount);
#endif

            strcpy(fout_name, OUTPUT_PATH);
            strcat(fout_name, testcaseEntry->pInputImage);
            if (slice_mode == 1)
            {
                sprintf(slice_ext, "_%d.yuv", slice_idx);
                strcat(fout_name, slice_ext);
            }
            else
                strcat(fout_name, ".yuv");
            foup = fopen(fout_name, "wb");
            if (foup == NULL)
            {
                TIMM_OSAL_TraceFunction("error opening o/p file");
                status = MSP_ERROR_NOTENOUGHRESOURCES;
                goto EXIT;
            }

#ifndef __PROFILING_CHANGES__
            if (yuv_mode == 420 && pQueryParam.Isplanar == FALSE)
            {
                fwrite(outBuff.pBuf[0], sizeof(char), (iw * ih), foup);
                fwrite(outBuff.pBuf[1], sizeof(char), (iw * ih * 0.5), foup);
            }
            else if (yuv_mode == 422 && pQueryParam.Isplanar == FALSE)
            {
                fwrite(outBuff.pBuf[0], sizeof(char), (iw * ih * 2), foup);
            }
            else if (pQueryParam.Isplanar == TRUE)
            {
                /* perform query to know the number of MCUS */
                status =
                    MSP_query(hMSP,
                              (MSP_INDEXTYPE)
                              MSP_JPEG_DEC_INDEXTYPE_CREATE_PARAMS,
                              (MSP_PTR) & pQueryParam);
                JPEG_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

                if (yuv_mode == 420)
                {
                    FinalOp = (UINT8 *) TIMM_OSAL_MallocExtn(iw * ih * 3 / 2,
                                                             TIMM_OSAL_TRUE, 16,
                                                             TIMMOSAL_MEM_SEGMENT_EXT,
                                                             NULL);
                }
                else if (yuv_mode == 422)
                {
                    FinalOp = (UINT8 *) TIMM_OSAL_MallocExtn(iw * ih * 2,
                                                             TIMM_OSAL_TRUE, 16,
                                                             TIMMOSAL_MEM_SEGMENT_EXT,
                                                             NULL);
                }
                else                                       // if(yuv_mode ==
                                                           // 444)
                {
                    FinalOp = (UINT8 *) TIMM_OSAL_MallocExtn(iw * ih * 3,
                                                             TIMM_OSAL_TRUE, 16,
                                                             TIMMOSAL_MEM_SEGMENT_EXT,
                                                             NULL);
                }
                JPEG_EXIT_IF(FinalOp == NULL, MSP_ERROR_NULLPTR);
                y_ptr = FinalOp;
                data_in = outBuff.pBuf[0];
                cb_ptr = FinalOp + (iw * ih);
                iw_temp = iw;
                ih_temp = ih;
                if (yuv_mode == 420)
                {
                    cr_ptr = cb_ptr + (iw * ih / 4);
                }
                else if (yuv_mode == 422)
                {
                    cr_ptr = cb_ptr + (iw * ih / 2);
                }
                else
                {
                    cr_ptr = cb_ptr + (iw * ih);
                }
                rearrange(pQueryParam.ulNumMCUY, data_in, y_ptr, &iw_temp,
                          &ih_temp);
                data_in += (iw * ih);
                if (yuv_mode == 420)
                {
                    iw_temp = iw / 2;
                    ih_temp = ih / 2;
                }
                else if (yuv_mode == 422)
                {
                    iw_temp = iw / 2;
                    ih_temp = ih;
                }

                rearrange(pQueryParam.ulNumMCUCb, data_in, cb_ptr, &iw_temp,
                          &ih_temp);
                if (yuv_mode == 420)
                {
                    data_in += (iw * ih / 4);
                }
                else if (yuv_mode == 422)
                {
                    data_in += (iw * ih / 2);
                }
                else
                {
                    data_in += (iw * ih);
                }
                rearrange(pQueryParam.ulNumMCUCr, data_in, cr_ptr, &iw_temp,
                          &ih_temp);

                if (yuv_mode == 420)
                {
                    fwrite(FinalOp, sizeof(char), (iw * ih * 3) / 2, foup);
                    MSP_JPEG_DEC_FREE(FinalOp);
                }
                else if (yuv_mode == 422)
                {
                    fwrite(FinalOp, sizeof(char), (iw * ih * 2), foup);
                    MSP_JPEG_DEC_FREE(FinalOp);
                }
                else if (yuv_mode == 444)
                {
                    fwrite(FinalOp, sizeof(char), (iw * ih * 3), foup);
                    MSP_JPEG_DEC_FREE(FinalOp);
                }
            }
#endif

            fclose(foup);
        }
        // /////// REPEAT FOR EVERY SLICE

        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS, "Decode done\n");

        JPEG_EXIT_IF((MSP_close(hMSP)) != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        MSP_JPEG_DEC_FREE(inBuff.pBuf[0]);
        MSP_JPEG_DEC_FREE(outBuff.pBuf[0]);
        MSP_JPEG_DEC_FREE(outBuff.pBuf[1]);

        JPEG_EXIT_IF((MSP_deInit(hMSP)) != MSP_ERROR_NONE, MSP_ERROR_FAIL);
    }

  EXIT:
    TIMM_OSAL_TraceFunction("\nJpeg Decode %s\n", (status == 0) ?
                            "Test case passed" : "Failed");

    if (finp == NULL)
    {
        fclose(finp);
    }

    if (finp == NULL)
    {
        fclose(foup);
    }

    // #ifdef __RM_CHANGES__
    RM_DeInit();
    // #endif

    /* SWITCH OFF THE SIMCOP CLK IN ISS REGISTER */
    *(pISSRegs + (0x84 >> 2)) &= (~0x00000001);

    // Get Memory status after running the test case
    mem_size_end = Jpegd_MemStatPrint();
    if (mem_size_end != mem_size_start)
        TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                           "Memory leak detected. Bytes lost = %d",
                           (mem_size_end - mem_size_start));

    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM, "JPEG Decoder Test End");
    TIMM_OSAL_TraceExt(TIMM_OSAL_TRACEGRP_SYSTEM,
                       "-----------------------------------------------");

}
