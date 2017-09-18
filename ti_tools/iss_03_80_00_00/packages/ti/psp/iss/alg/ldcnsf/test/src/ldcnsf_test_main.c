/* ============================================================================== 
 * Texas Instruments OMAP(TM) Platform Software (c) Copyright 2009, Texas
 * Instruments Incorporated.  All Rights Reserved.  Use of this software is
 * controlled by the terms and conditions found in the license agreement
 * under which this software has been supplied.
 * =========================================================================== 
 */
/**
 * @file ldcnsf_test_main.c
 *
 * This File is test-bench for MSP LDCNSF component; targeted at OMAP4.
 *
 * @path  $(DUCATIVOB)\alg\ldcnsf\test\src\
 *
 * @rev  1.0
 */
/* ----------------------------------------------------------------------------
 * ! ! Revision History ! =================================== ! 25-July-2009
 * Sanish Mahadik: Initial Release ! !Revisions appear in reverse chronological
 * order; !that is, newest first.  The date format is dd-Mon-yyyy.
 * =========================================================================== */

/* ! \mainpage Documentation for MSP NSF \section Introduction This is the
 * documentation for MSP NSF component. It does not include documentation for 
 * the dependencies involved. */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/ipc/Semaphore.h>
#include <xdc/runtime/Timestamp.h>

// #define ENABLE_PRINTF
#define __PROFILE_LDCNSF__
// #define GENERATE_CMODEL_FILES

#ifdef GENERATE_CMODEL_FILES
#include "../../inc/msp_ldcnsf_internal.h"
#endif

#include "../../../../drivers/csl/iss/simcop/common/simcop.h"
#include "../../../../framework/msp/msp.h"
#include "../../../../framework/resource_manager/rm.h"
#include  "../../../../drivers/csl/iss/iss_common/iss_common.h"
#include "../../../../drivers/csl/iss/simcop/common/simcop_irq.h"
#include "ldcnsf_test_main.h"
#include "../../../ip_run/inc/ip_run.h"

static Semaphore_Handle gSemHandleLdcNsf;

extern const MSP_U16 ldcnsf_ldc_lut_0[];

extern const MSP_U16 ldcnsf_ldc_lut_1[];

extern const MSP_U16 ldcnsf_ldc_lut_2[];

extern const MSP_U16 ldcnsf_ldc_lut_3[];

extern const MSP_U16 ldcnsf_ldc_lut_4[];

extern const MSP_U16 ldcnsf_ldc_lut_5[];

extern const MSP_U16 ldcnsf_ldc_lut_6[];

extern const MSP_U16 ldcnsf_ldc_lut_7[];

extern const MSP_U16 ldcnsf_ldc_lut_8[];

extern MSP_U16 ldcnsf_default_ldckhl;

extern MSP_U16 ldcnsf_default_ldckhr;

extern MSP_U16 ldcnsf_default_ldckvl;

extern MSP_U16 ldcnsf_default_ldckvu;

extern MSP_U16 ldcnsf_default_Rth;

extern MSP_U16 ldcnsf_default_rightShiftBits;

extern MSP_S16 ldcnsf_user_thrconfig[];

extern MSP_S16 ldcnsf_user_shdconfig[];

extern MSP_S16 ldcnsf_user_edgeconfig[];

extern MSP_S16 ldcnsf_user_desatconfig[];

extern MSP_S16 ldcnsf_default_thrconfig[];

extern MSP_S16 ldcnsf_default_shdconfig[];

extern MSP_S16 ldcnsf_default_edgeconfig[];

extern MSP_S16 ldcnsf_default_desatconfig[];

static MSP_U32 ulDataOffset_Y = 0;

static MSP_U32 ulDataOffset_UV = 0;

static float mult_input = 2.0;

static MSP_U32 img_width_input = 0;

static MSP_U32 img_height_input = 0;

static long long CycleCount = 0;

#ifdef DBG_SERVER_SCRIPTING

char sFileInpNameLDCNSF[200], sFileOutNameLDCNSF[200],
    sFileOutNameLDCNSFUV[200];
char *Y_op_ptr = NULL;

char *UV_op_ptr = NULL;

MSP_U32 Y_size, UV_size;

MSP_U8 *InpBuf_LDCNSF = NULL;

Dummy_function()
{
}
#endif

/* ===================================================================
 *  @func     Update_TestParams_LDCNSF                                               
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
void Update_TestParams_LDCNSF(MSP_LDCNSF_CREATE_PARAMS * pApp_params,
                              MSP_U8 testnum)
{

    MSP_LDCNSF_LDC_PARAMS *ldc_params = &pApp_params->tLdcParams;

    MSP_LDCNSF_LUT_PARAMS *lutParams =
        ldc_params->ptLensDistortionParams->ptLutParams;
    MSP_LDCNSF_NSF_PARAM *nsf_params = &pApp_params->tNsfParams;

    pApp_params->ulComputeWidth = LDCNSF_TestStructure[testnum].ulComputeWidth;
    pApp_params->ulComputeHeight =
        LDCNSF_TestStructure[testnum].ulComputeHeight;
    pApp_params->ulTilBuffWidth = LDCNSF_TestStructure[testnum].ulComputeWidth;
    pApp_params->ulTilBuffHeight =
        LDCNSF_TestStructure[testnum].ulComputeHeight;
    pApp_params->eMspRotation = 0;

    pApp_params->ulInputStride = pApp_params->ulInputStrideChroma =
        LDCNSF_TestStructure[testnum].ulInputStride;
    pApp_params->ulOutputStride = pApp_params->ulOutputStrideChroma =
        LDCNSF_TestStructure[testnum].ulOutputStride;

    pApp_params->eOperateMode = LDCNSF_TestStructure[testnum].eOperateMode;
    pApp_params->eInputFormat = LDCNSF_TestStructure[testnum].eInputFormat;
    pApp_params->eOutputFormat = LDCNSF_TestStructure[testnum].eOutputFormat;

    ldc_params->eBayerInitColor = LDCNSF_TestStructure[testnum].eBayerInitColor;
    ldc_params->unPixelPad =
        (LDCNSF_TestStructure[testnum].eInputFormat ==
         MSP_LDCNSF_YUV_FORMAT_YCBCR422 ||
         LDCNSF_TestStructure[testnum].eInputFormat ==
         MSP_LDCNSF_YUV_FORMAT_YCBCR420) ? 6 : 3;

    ldc_params->tStartXY.unStartX = LDCNSF_TestStructure[testnum].tStartX;
    ldc_params->tStartXY.unStartY = LDCNSF_TestStructure[testnum].tStartY;

    ldc_params->ptLensDistortionParams->unLensCentreX = 40;
    ldc_params->ptLensDistortionParams->unLensCentreY = 32;

    if (testnum > 50 && testnum < 86)
    {
        ldc_params->ptLensDistortionParams->unLensCentreX =
            LDCNSF_TestStructure[testnum].ulInputWidth / 2;
        ldc_params->ptLensDistortionParams->unLensCentreY =
            LDCNSF_TestStructure[testnum].ulInputHeight / 2;
    }
    ldc_params->eInterpolationLuma =
        LDCNSF_TestStructure[testnum].eInterpolationLuma;

    lutParams->unLdcKhl = ldcnsf_default_ldckhl;
    lutParams->unLdcKhr = ldcnsf_default_ldckhr;
    lutParams->unLdcKvl = ldcnsf_default_ldckvl;
    lutParams->unLdcKvu = ldcnsf_default_ldckvu;
    lutParams->unLdcRth = ldcnsf_default_Rth;
    lutParams->ucRightShiftBits = ldcnsf_default_rightShiftBits;

    switch (pApp_params->eInputFormat)
    {
        case MSP_LDCNSF_YUV_FORMAT_YCBCR420:
        case MSP_LDCNSF_YUV_FORMAT_YCBCR422:
            lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_0;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_ALAW8:
            lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_5;
            if ((testnum == 15) | (testnum == 68))
                lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_8;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_PACK8:
            lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_5;
            if ((testnum == 14) || (testnum == 65))
                lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_8;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_PACK12:
            lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_5;
            if ((testnum == 16) | (testnum == 71))
                lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_8;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_UNPACK12:
            lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_8;
            if (testnum == 5)
                lutParams->punLdcLutTable = (MSP_U16 *) ldcnsf_ldc_lut_2;
            break;                                         // ### changed to
                                                           // 8 from 2

    }

    ldc_params->ptAffineParams->unAffineA =
        LDCNSF_TestStructure[testnum].ptAffineParams->unAffineA;
    ldc_params->ptAffineParams->unAffineB =
        LDCNSF_TestStructure[testnum].ptAffineParams->unAffineB;
    ldc_params->ptAffineParams->unAffineC =
        LDCNSF_TestStructure[testnum].ptAffineParams->unAffineC;
    ldc_params->ptAffineParams->unAffineD =
        LDCNSF_TestStructure[testnum].ptAffineParams->unAffineD;
    ldc_params->ptAffineParams->unAffineE =
        LDCNSF_TestStructure[testnum].ptAffineParams->unAffineE;
    ldc_params->ptAffineParams->unAffineF =
        LDCNSF_TestStructure[testnum].ptAffineParams->unAffineF;

    nsf_params->bChromaEnable = LDCNSF_TestStructure[testnum].bChromaEn;
    nsf_params->bLumaEnable = LDCNSF_TestStructure[testnum].bLumaEn;
    nsf_params->bSmoothChroma = LDCNSF_TestStructure[testnum].bSmoothChromaEn;
    nsf_params->bSmoothLuma = LDCNSF_TestStructure[testnum].bSmoothLumaEn;
    nsf_params->eSmoothVal = LDCNSF_TestStructure[testnum].eSmoothVal;

    nsf_params->eDesatParam = LDCNSF_TestStructure[testnum].eDesatParam;
    nsf_params->eEdgeParam = LDCNSF_TestStructure[testnum].eEdgeParam;
    nsf_params->eShdParam = LDCNSF_TestStructure[testnum].eShdParam;
    nsf_params->eFilterParam = LDCNSF_TestStructure[testnum].eFilterParam;

    if (LDCNSF_TestStructure[testnum].eDesatParam == MSP_LDCNSF_PARAM_USER)
    {
        nsf_params->rgnDesatVal = (MSP_S16 *) ldcnsf_user_desatconfig;
    }
    else
    {
        nsf_params->rgnDesatVal = (MSP_S16 *) ldcnsf_default_desatconfig;
    }
    if (LDCNSF_TestStructure[testnum].eEdgeParam == MSP_LDCNSF_PARAM_USER)
    {
        nsf_params->rgnEdgeVal = (MSP_S16 *) ldcnsf_user_edgeconfig;
    }
    else
    {
        nsf_params->rgnEdgeVal = (MSP_S16 *) ldcnsf_default_edgeconfig;
    }

    if (LDCNSF_TestStructure[testnum].eShdParam == MSP_LDCNSF_PARAM_USER)
    {
        nsf_params->rgnShdVal = (MSP_S16 *) ldcnsf_user_shdconfig;
    }
    else
    {
        nsf_params->rgnShdVal = (MSP_S16 *) ldcnsf_default_shdconfig;
    }

    if (LDCNSF_TestStructure[testnum].eFilterParam == MSP_LDCNSF_PARAM_USER)
    {
        nsf_params->rgnFilterVal = (MSP_S16 *) ldcnsf_user_thrconfig;
    }
    else
    {
        nsf_params->rgnFilterVal = (MSP_S16 *) ldcnsf_default_thrconfig;
    }

}

/* ===================================================================
 *  @func     LDCNSF_Allocate_TestBuffers                                               
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
void LDCNSF_Allocate_TestBuffers(MSP_U8 testnum, MSP_BUFHEADER_TYPE * inbuf,
                                 MSP_BUFHEADER_TYPE * outbuf,
                                 MSP_LDCNSF_CREATE_PARAMS * pParam)
{
    float multiplier_ip;

    MSP_U8 unNumCompBuffersIn = 1, unNumCompBuffersOut = 1;

    MSP_U32 ulInputStride = 0, ulOutputStride = 0, ulInputStrideChroma =
        0, ulOutputStrideChroma = 0;
    MSP_U32 ulInputHeight = 0, ulOutputHeight = 0;

    MSP_LDCNSF_DATAFORMAT inp_dataformat =
        LDCNSF_TestStructure[testnum].eInputFormat;
    MSP_LDCNSF_DATAFORMAT op_dataformat =
        LDCNSF_TestStructure[testnum].eOutputFormat;

    inbuf->unBufSize[0] = inbuf->unBufSize[1] = inbuf->unBufSize[2] = 0;
    outbuf->unBufSize[0] = outbuf->unBufSize[1] = outbuf->unBufSize[2] = 0;
    switch (pParam->eOperateMode)
    {
        case MODE_LDC_AFFINE:
        case MODE_CAC_AFFINE:
        case MODE_AFFINE_ONLY:

            multiplier_ip =
                (inp_dataformat ==
                 MSP_LDCNSF_YUV_FORMAT_YCBCR422) ? 2.0 : ((inp_dataformat ==
                                                           MSP_LDCNSF_YUV_FORMAT_YCBCR420
                                                           || inp_dataformat ==
                                                           MSP_LDCNSF_BAYER_FORMAT_ALAW8
                                                           || inp_dataformat ==
                                                           MSP_LDCNSF_BAYER_FORMAT_PACK8)
                                                          ? 1.0
                                                          : (inp_dataformat ==
                                                             MSP_LDCNSF_BAYER_FORMAT_PACK12
                                                             ? 1.5 : 2));
            if (inp_dataformat == MSP_LDCNSF_YUV_FORMAT_YCBCR420)
                unNumCompBuffersOut = unNumCompBuffersIn = 2;
            else
                unNumCompBuffersOut = unNumCompBuffersOut = 1;

            ulOutputHeight = pParam->ulComputeHeight;

            break;

        case MODE_LDC_AFFINE_NSF:
        case MODE_AFFINE_NSF:
        case MODE_NSF_ONLY:
        case MODE_FAST_NSF:

            multiplier_ip =
                (inp_dataformat == MSP_LDCNSF_YUV_FORMAT_YCBCR422) ? 2.0 : 1.0;

            unNumCompBuffersIn =
                (inp_dataformat == MSP_LDCNSF_YUV_FORMAT_YCBCR420) ? 2 : 1;
            unNumCompBuffersOut =
                (op_dataformat == MSP_LDCNSF_YUV_FORMAT_YCBCR420) ? 2 : 1;

            ulOutputHeight = pParam->ulComputeHeight + 32;

            break;

    }

    ulInputHeight = img_height_input =
        LDCNSF_TestStructure[testnum].ulInputHeight;
    img_width_input = LDCNSF_TestStructure[testnum].ulInputWidth;

    ulInputStride = LDCNSF_TestStructure[testnum].ulInputStride;
    ulOutputStride = LDCNSF_TestStructure[testnum].ulOutputStride;
    ulOutputStrideChroma = LDCNSF_TestStructure[testnum].ulOutputStride;
    mult_input = multiplier_ip;

    /* Allocate the output buffer */
    outbuf->nPortIndex = 1;
    outbuf->unNumOfCompBufs = unNumCompBuffersOut;
    outbuf->unBufStride[0] = ulOutputStride;
    outbuf->unBufSize[0] = (MSP_U32) ((ulOutputStride) * (ulOutputHeight));
    if (testnum == 46 || testnum == 47 || testnum == 48 || testnum == 49)
        outbuf->unBufSize[0] =
            (MSP_U32) ((ulInputStride) * (ulInputHeight + 96));
    if (pParam->eOperateMode == MODE_FAST_NSF)
        outbuf->unBufSize[0] += 32;
    outbuf->pBuf[0] =
        (MSP_U8 *) TIMM_OSAL_MallocExtn(outbuf->unBufSize[0], TIMM_OSAL_TRUE,
                                        THIRTYTWO_BYTE_ALIGN,
                                        TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_LDCNSF_EXIT_IF(outbuf->pBuf[0] == NULL, MSP_ERROR_NULLPTR);

    if (unNumCompBuffersOut > 1)
    {
        outbuf->unBufSize[1] =
            (MSP_U32) ((ulOutputStrideChroma) * (ulOutputHeight / 2));
        if (testnum == 46 || testnum == 47 || testnum == 48 || testnum == 49)
            outbuf->unBufSize[1] =
                (MSP_U32) ((ulOutputStrideChroma) * (ulOutputHeight / 2 + 96));
        outbuf->unBufStride[1] = ulOutputStrideChroma;
        if (pParam->eOperateMode == MODE_FAST_NSF)
            outbuf->unBufSize[1] += outbuf->unBufStride[1] * 8;
        outbuf->pBuf[1] =
            (MSP_U8 *) TIMM_OSAL_MallocExtn(outbuf->unBufSize[1],
                                            TIMM_OSAL_TRUE,
                                            THIRTYTWO_BYTE_ALIGN,
                                            TIMMOSAL_MEM_SEGMENT_EXT, NULL);
        MSP_LDCNSF_EXIT_IF(outbuf->pBuf[1] == NULL, MSP_ERROR_NULLPTR);
    }

    /* Allocate input buffers */
    inbuf->nPortIndex = 0;
    inbuf->unNumOfCompBufs = unNumCompBuffersIn;
    inbuf->unBufSize[0] = (MSP_U32) ((ulInputStride) * (ulInputHeight));

    if (testnum == 44 || testnum == 45)
    {
        inbuf->pBuf[0] = outbuf->pBuf[0];
        inbuf->pBuf[1] = outbuf->pBuf[1];
        inbuf->unBufSize[0] = outbuf->unBufSize[0];
        inbuf->unBufSize[1] = outbuf->unBufSize[1];
        inbuf->unBufStride[0] = ulInputStride;
        inbuf->unBufStride[1] = ulInputStride;
    }

    else if (testnum == 46 || testnum == 47 || testnum == 48)
    {
        inbuf->pBuf[0] = outbuf->pBuf[0] + 96 * ulOutputStride;
        inbuf->pBuf[1] = outbuf->pBuf[1] + 96 * ulOutputStride;
        inbuf->unBufSize[0] = outbuf->unBufSize[0];
        inbuf->unBufSize[1] = outbuf->unBufSize[1];
        inbuf->unBufStride[0] = ulInputStride;
        inbuf->unBufStride[1] = ulInputStride;
    }
    else if (testnum == 49)
    {
        inbuf->pBuf[0] = outbuf->pBuf[0] + 96 * ulOutputStride + 224 * 2;
        inbuf->unBufSize[0] = outbuf->unBufSize[0];
        inbuf->unBufSize[1] = 0;
        inbuf->unBufStride[0] = ulInputStride;
        inbuf->unBufStride[1] = ulInputStride;
    }

    else
    {
        if (unNumCompBuffersIn > 1)
        {
            inbuf->unBufSize[1] =
                (MSP_U32) ((ulInputStride) * (ulInputHeight / 2));
            inbuf->unBufStride[0] = inbuf->unBufStride[1] = ulInputStride;
            inbuf->pBuf[0] =
                (MSP_U8 *) TIMM_OSAL_MallocExtn(inbuf->unBufSize[0] +
                                                inbuf->unBufSize[1],
                                                TIMM_OSAL_TRUE,
                                                THIRTYTWO_BYTE_ALIGN,
                                                TIMMOSAL_MEM_SEGMENT_EXT, NULL);
            MSP_LDCNSF_EXIT_IF(inbuf->pBuf[0] == NULL, MSP_ERROR_NULLPTR);
            inbuf->pBuf[1] = inbuf->pBuf[0] + inbuf->unBufSize[0];
        }
        else
        {
            inbuf->pBuf[0] =
                (MSP_U8 *) TIMM_OSAL_MallocExtn(inbuf->unBufSize[0],
                                                TIMM_OSAL_TRUE,
                                                THIRTYTWO_BYTE_ALIGN,
                                                TIMMOSAL_MEM_SEGMENT_EXT, NULL);
            MSP_LDCNSF_EXIT_IF(inbuf->pBuf[0] == NULL, MSP_ERROR_NULLPTR);
            inbuf->unBufStride[0] = ulInputStride;
            inbuf->unBufSize[1] = 0;
        }

    }
    return;
  EXIT:

    TIMM_OSAL_TraceFunction("\n Buffer allocation in test-case failed\n");
}

/* ===================================================================
 *  @func     MSP_LDCNSF_Callback                                               
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
static MSP_ERROR_TYPE MSP_LDCNSF_Callback(MSP_PTR hMSP,
                                          MSP_PTR pAppData,
                                          MSP_EVENT_TYPE tEvent,
                                          MSP_OPAQUE nEventData1,
                                          MSP_OPAQUE nEventData2)
{

    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    MSP_BUFHEADER_TYPE *pBufHeader;

    /* Cast the Event data2 as bufferheader */
    pBufHeader = (MSP_BUFHEADER_TYPE *) nEventData2;
    switch (tEvent)
    {
        case MSP_OPEN_EVENT:
        case MSP_CLOSE_EVENT:
        case MSP_PROFILE_EVENT:
        case MSP_CTRLCMD_EVENT:
        case MSP_ERROR_EVENT:
            break;

        case MSP_DATA_EVENT:
            if (nEventData1 == MSP_LDCNSF_INPUT_PORT)
            {
                break;
            }

            if (nEventData1 == MSP_LDCNSF_OUTPUT_PORT)
            {
                ulDataOffset_Y = pBufHeader->unBufOffset[0];
                ulDataOffset_UV = pBufHeader->unBufOffset[1];

                /* post the semaphore + write data to file */
                Semaphore_post(gSemHandleLdcNsf);
                break;
            }

        default:
            status = MSP_ERROR_INVALIDCMD;
            break;

    }
    return status;
}

#ifdef GENERATE_CMODEL_FILES
/* ===================================================================
 *  @func     WriteOut_NSF_file                                               
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
void WriteOut_NSF_file(MSP_U16 testnum,
                       MSP_LDCNSF_CREATE_PARAMS * pLdcNsfQueryParam)
{
    FILE *fout = NULL;

    char *filename = NULL;

    MSP_LDCNSF_FilterConfig *pFilterParams = NULL;

    MSP_LDCNSF_ShdConfig *pShdParams = NULL;

    MSP_LDCNSF_EdgeConfig *pEdgeParams = NULL;

    MSP_LDCNSF_DesatConfig *pDesatParams = NULL;

    MSP_LDCNSF_EXIT_IF((pLdcNsfQueryParam == NULL), MSP_ERROR_NULLPTR);
    strcpy(filename, OUTPUT_PATH);
    strcat(filename, "NSF_Cmodel_");
    strcat(filename, LDCNSF_TestStructure[testnum].output_fname);
    strcat(filename, ".txt");

    fout = fopen(filename, "w");
    MSP_LDCNSF_EXIT_IF(fout == NULL, MSP_ERROR_NULLPTR);

    pFilterParams =
        (MSP_LDCNSF_FilterConfig *) (pLdcNsfQueryParam->tNsfParams.
                                     rgnFilterVal);
    pShdParams =
        (MSP_LDCNSF_ShdConfig *) pLdcNsfQueryParam->tNsfParams.rgnShdVal;
    pEdgeParams =
        (MSP_LDCNSF_EdgeConfig *) pLdcNsfQueryParam->tNsfParams.rgnEdgeVal;
    pDesatParams =
        (MSP_LDCNSF_DesatConfig *) pLdcNsfQueryParam->tNsfParams.rgnDesatVal;

    fprintf(fout, "%d \n", pLdcNsfQueryParam->ulComputeWidth + 32);
    fprintf(fout, "%d \n", pLdcNsfQueryParam->ulComputeHeight + 32);
    fprintf(fout, "0 \n 0 \n 1\n 0\n 1\n");
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->eOutputFormat ==
             MSP_LDCNSF_YUV_FORMAT_YCBCR422 ? 1 : 0));
    fprintf(fout, "1 \n");

    // ### Hard-code the values of luma and chroma enable to '1' as C-model
    // has issues in Y_ONLY and UV_ONLY modes
    fprintf(fout, "1 \n");
    fprintf(fout, "1 \n");
    // fprintf(fout,"%d \n", (pLdcNsfQueryParam->tNsfParams.bLumaEnable ==
    // MSP_TRUE? 1 : 0));
    // fprintf(fout,"%d \n", (pLdcNsfQueryParam->tNsfParams.bChromaEnable ==
    // MSP_TRUE? 1 : 0));

    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tNsfParams.eShdParam ==
             MSP_LDCNSF_PARAM_DISABLE ? 0 : 1));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tNsfParams.bSmoothLuma == MSP_TRUE ? 1 : 0));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tNsfParams.bSmoothChroma == MSP_TRUE ? 1 : 0));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tNsfParams.eEdgeParam ==
             MSP_LDCNSF_PARAM_DISABLE ? 0 : 1));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tNsfParams.eDesatParam ==
             MSP_LDCNSF_PARAM_DISABLE ? 0 : 1));
    fprintf(fout, "0 \n 0 \n");
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->ulComputeWidth + 32 >=
             LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 ?
             LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 : pLdcNsfQueryParam->
             ulComputeWidth + 32));
    fprintf(fout, "8 \n 0 \n");
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->eOutputFormat ==
             MSP_LDCNSF_YUV_FORMAT_YCBCR422 ? 0 : 1) *
            ((pLdcNsfQueryParam->ulComputeWidth + 32 >=
              LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 ?
              LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 : pLdcNsfQueryParam->
              ulComputeWidth + 32)) * (8));
    fprintf(fout, "%d \n 0 \n",
            (pLdcNsfQueryParam->eOutputFormat ==
             MSP_LDCNSF_YUV_FORMAT_YCBCR422 ? 2 : 1) *
            ((pLdcNsfQueryParam->ulComputeWidth + 32 >=
              LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 ?
              LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 : pLdcNsfQueryParam->
              ulComputeWidth + 32)));
    fprintf(fout, "%d \n",
            ((pLdcNsfQueryParam->ulComputeWidth + 32 >=
              LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 ?
              LDCNSF_MAX_INPUTBLOCKWIDTH_YUV422 : pLdcNsfQueryParam->
              ulComputeWidth + 32)));

    /* NSF_THR_XX registers */
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr00,
            pFilterParams->nsf_thr01);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr02,
            pFilterParams->nsf_thr03);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr04,
            pFilterParams->nsf_thr05);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr10,
            pFilterParams->nsf_thr11);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr12,
            pFilterParams->nsf_thr13);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr14,
            pFilterParams->nsf_thr15);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr20,
            pFilterParams->nsf_thr21);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr22,
            pFilterParams->nsf_thr23);
    fprintf(fout, "%d   %d \n", pFilterParams->nsf_thr24,
            pFilterParams->nsf_thr25);

    /* NSF_SHD_ registers */
    fprintf(fout, "%d \n", pShdParams->nsf_shd_xst);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_yst);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_ha1);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_ha2);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_va1);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_va2);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_hs_hsc);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_vs_vcs);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_hs_s0);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_hs_hs1);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_hs_hs2);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_vs_vs1);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_vs_vs2);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_gadj);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_oadj);
    fprintf(fout, "%d \n", pShdParams->nsf_shd_maxg);

    /* NSF_EE_XX registers */
    fprintf(fout, "%d \n", pLdcNsfQueryParam->tNsfParams.eSmoothVal);

    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l1_slope);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l1_thr1);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l1_thr2);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l1_ofst2);

    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l2_slope);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l2_thr1);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l2_thr2);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l2_ofst2);

    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l3_slope);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l3_thr1);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l3_thr2);
    fprintf(fout, "%d \n", pEdgeParams->nsf_ee_l3_ofst2);

    /* NSF_DS_ registers */
    // fprintf(fout,"%d \n", pDesatParams->nsf_ds_thr2);
    fprintf(fout, "%d \n", pDesatParams->nsf_ds_thr1);
    fprintf(fout, "%d \n", pDesatParams->nsf_ds_slope1);
    fprintf(fout, "%d\n", pDesatParams->nsf_ds_slope2);

    fclose(fout);

    return;

  EXIT:
    TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "Could not generate C-model file for NSF\n ");

}

/* ===================================================================
 *  @func     WriteOut_LDC_file                                               
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
void WriteOut_LDC_file(MSP_U16 testnum,
                       MSP_LDCNSF_CREATE_PARAMS * pLdcNsfQueryParam)
{
    FILE *fout = NULL;

    char *filename = NULL;

    float multiplier = 2.0;

    unsigned int width = 0, height = 0, obw = 16, i;

    MSP_U16 *pLutPtr = NULL;

    MSP_LDCNSF_EXIT_IF((pLdcNsfQueryParam == NULL), MSP_ERROR_NULLPTR);
    MSP_LDCNSF_EXIT_IF((pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->
                        ptLutParams->punLdcLutTable == NULL),
                       MSP_ERROR_NULLPTR);
    strcpy(filename, OUTPUT_PATH);
    strcat(filename, "LDC_Cmodel_");
    strcat(filename, LDCNSF_TestStructure[testnum].output_fname);
    strcat(filename, ".txt");

    fout = fopen(filename, "w");
    MSP_LDCNSF_EXIT_IF(fout == NULL, MSP_ERROR_NULLPTR);

    pLutPtr =
        pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams->
        punLdcLutTable;

    if (pLdcNsfQueryParam->eOperateMode == MODE_CAC_AFFINE ||
        pLdcNsfQueryParam->eOperateMode == MODE_LDC_AFFINE ||
        pLdcNsfQueryParam->eOperateMode == MODE_AFFINE_ONLY)
    {
        fprintf(fout, "%d \n 1 \n", pLdcNsfQueryParam->ulComputeHeight);
        height = pLdcNsfQueryParam->ulComputeHeight;
        width = pLdcNsfQueryParam->ulComputeWidth;
    }
    else
    {
        fprintf(fout, "%d \n 1 \n", pLdcNsfQueryParam->ulComputeHeight + 32);
        height = pLdcNsfQueryParam->ulComputeHeight + 32;
        width = pLdcNsfQueryParam->ulComputeWidth + 32;
    }
    switch (pLdcNsfQueryParam->eInputFormat)
    {
        case MSP_LDCNSF_YUV_FORMAT_YCBCR422:
            fprintf(fout, "0 \n");
            fprintf(fout, "0 \n");
            multiplier = 2.0;
            obw = 16;
            break;

        case MSP_LDCNSF_YUV_FORMAT_YCBCR420:
            fprintf(fout, "2 \n");
            fprintf(fout, "0 \n");
            multiplier = 1.0;
            obw = 32;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_ALAW8:
            fprintf(fout, "1 \n");
            fprintf(fout, "3 \n");
            multiplier = 1.0;
            obw = 32;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_PACK8:
            fprintf(fout, "1 \n");
            fprintf(fout, "2 \n");
            multiplier = 1.0;
            obw = 32;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_PACK12:
            fprintf(fout, "1 \n");
            fprintf(fout, "1 \n");
            multiplier = 1.5;
            obw = 64;
            break;

        case MSP_LDCNSF_BAYER_FORMAT_UNPACK12:
            fprintf(fout, "1 \n");
            fprintf(fout, "0 \n");
            multiplier = 2.0;
            obw = 16;
            break;
    }

    if (pLdcNsfQueryParam->eOperateMode == MODE_AFFINE_ONLY)
        fprintf(fout, "0 \n");
    else
        fprintf(fout, "1 \n");

    fprintf(fout, "%d \n", (unsigned int) (width * multiplier));
    fprintf(fout, "%d \n", (height));
    fprintf(fout, "%d \n", (width));
    fprintf(fout, "%d \n", (unsigned int) (width * multiplier));

    fprintf(fout, "%d \n", (pLdcNsfQueryParam->tLdcParams.tStartXY.unStartY));
    fprintf(fout, "%d \n", (pLdcNsfQueryParam->tLdcParams.tStartXY.unStartX));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams->
             unLdcRth));
    fprintf(fout, "%d \n", (pLdcNsfQueryParam->tLdcParams.eInterpolationLuma));
    fprintf(fout, "%d \n",
            ((pLdcNsfQueryParam->tLdcParams.eBayerInitColor) % 4));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams->
             ucRightShiftBits));

    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->
             unLensCentreY));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->
             unLensCentreX));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams->
             unLdcKvl));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams->
             unLdcKvu));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams->
             unLdcKhr));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams->
             unLdcKhl));
    fprintf(fout, "%d \n", (pLdcNsfQueryParam->tLdcParams.unPixelPad));
    fprintf(fout, "8 \n");
    fprintf(fout, "%d \n", (obw));

    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptAffineParams->unAffineA));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptAffineParams->unAffineB));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptAffineParams->unAffineC));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptAffineParams->unAffineD));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptAffineParams->unAffineE));
    fprintf(fout, "%d \n",
            (pLdcNsfQueryParam->tLdcParams.ptAffineParams->unAffineF));

    for (i = 0; i < 256; i++)
    {
        fprintf(fout, "%d \n", (pLutPtr[i]));
    }

    fclose(fout);

    return;

  EXIT:
    TIMM_OSAL_ErrorExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                       "Could not generate C-model file for LDC\n ");

}
#endif
/* ===================================================================
 *  @func     ldcnsf_prepare_test                                               
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
void ldcnsf_prepare_test(MSP_U8 testnum,
                         MSP_LDCNSF_CREATE_PARAMS * pLdcNsfQueryParam,
                         MSP_BUFHEADER_TYPE * pInBuffLdcNsf,
                         MSP_BUFHEADER_TYPE * pOutBuffLdcNsf)
{
    char fin_name[200];

    char fout_name[200];

    MSP_U32 input_filesize;

#ifndef DBG_SERVER_SCRIPTING
    MSP_U8 *tmp_buffer;

    MSP_U8 *ptr_out;

    MSP_U8 *ptr_in;
#endif
    MSP_U32 i;

    FILE *fp_inp;

    /* Open the file for input */
    strcpy(fin_name, INPUT_PATH);
    strcat(fin_name, LDCNSF_TestStructure[testnum].input_fname);

    strcpy(fout_name, OUTPUT_PATH);
    strcat(fout_name, LDCNSF_TestStructure[testnum].output_fname);

#ifdef DBG_SERVER_SCRIPTING
    strcpy(sFileInpNameLDCNSF, fin_name);
    strcpy(sFileOutNameLDCNSF, fout_name);
    strcpy(sFileOutNameLDCNSFUV, fout_name);
    strcat(sFileOutNameLDCNSFUV, ".UV");
#endif
    /* Update the pLdcNsfQueryParam structure with test-paramaters */
    Update_TestParams_LDCNSF(pLdcNsfQueryParam, testnum);
#ifndef GENERATE_CMODEL_FILES
    LDCNSF_Allocate_TestBuffers(testnum, pInBuffLdcNsf, pOutBuffLdcNsf,
                                pLdcNsfQueryParam);
#endif

    if (LDCNSF_TestStructure[testnum].eInputFormat ==
        MSP_LDCNSF_YUV_FORMAT_YCBCR420)
        input_filesize =
            (MSP_U32) ((img_width_input) * (img_height_input) * 1.5);
    else
        input_filesize = (img_width_input) * (img_height_input) * mult_input;

    if (testnum == 0 || testnum == 40 || testnum == 45 || testnum == 46 ||
        testnum == 49)
    {
#ifndef DBG_SERVER_SCRIPTING

        tmp_buffer =
            (MSP_U8 *) TIMM_OSAL_MallocExtn(input_filesize, TIMM_OSAL_TRUE,
                                            DEFAULT_BYTE_ALIGN,
                                            TIMMOSAL_MEM_SEGMENT_EXT, NULL);
        MSP_LDCNSF_EXIT_IF(tmp_buffer == NULL, MSP_ERROR_NULLPTR);

        fp_inp = fopen(fin_name, "rb");
        if (fp_inp == NULL)
        {
            TIMM_OSAL_TraceFunction("Error opening input file:  %s\n",
                                    fin_name);
            MSP_LDCNSF_EXIT_IF(fp_inp == NULL, MSP_ERROR_FAIL);
        }
        /* Read the input image into INPUT buffer */
        fread((void *) tmp_buffer, sizeof(unsigned char), input_filesize,
              fp_inp);
        fflush(fp_inp);
        fclose(fp_inp);

        // Copy this into the input buffer allocated
        for (i = 0; i < img_height_input; i++)
        {
            ptr_out =
                pInBuffLdcNsf->pBuf[0] + i * pLdcNsfQueryParam->ulInputStride;
            ptr_in =
                tmp_buffer + (MSP_U32) (mult_input * i * (img_width_input));
            TIMM_OSAL_Memcpy(ptr_out, ptr_in, (img_width_input) * mult_input);
        }

        if (LDCNSF_TestStructure[testnum].eInputFormat ==
            MSP_LDCNSF_YUV_FORMAT_YCBCR420)
        {
            for (i = 0; i < img_height_input / 2; i++)
            {
                ptr_out =
                    pInBuffLdcNsf->pBuf[1] +
                    i * pLdcNsfQueryParam->ulInputStrideChroma;
                ptr_in =
                    (tmp_buffer + img_width_input * img_height_input) +
                    (MSP_U32) (mult_input * i * (img_width_input));
                TIMM_OSAL_Memcpy(ptr_out, ptr_in,
                                 (img_width_input) * mult_input);
            }
            TIMM_OSAL_Free(tmp_buffer);
        }
#endif
    }
    else
    {
#ifndef DBG_SERVER_SCRIPTING
        fp_inp = fopen(fin_name, "rb");
        if (fp_inp == NULL)
        {
            TIMM_OSAL_TraceFunction("Error opening input file:  %s\n",
                                    fin_name);
            MSP_LDCNSF_EXIT_IF(fp_inp == NULL, MSP_ERROR_FAIL);
        }
        fread((void *) pInBuffLdcNsf->pBuf[0], sizeof(unsigned char),
              input_filesize, fp_inp);
        fflush(fp_inp);
        fclose(fp_inp);
#else
        InpBuf_LDCNSF = (MSP_U8 *) (pInBuffLdcNsf->pBuf[0]);
        Dummy_function();
#endif
    }

    return;
  EXIT:
    TIMM_OSAL_TraceFunction("Something went wrong!!\n");
    return;

}

/* ===================================================================
 *  @func     ldcnsf_run_test                                               
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
void ldcnsf_run_test(MSP_HANDLE hNsfMSP, MSP_U8 testnum,
                     MSP_LDCNSF_CREATE_PARAMS * pLdcNsfQueryParam,
                     MSP_BUFHEADER_TYPE * pInBuffLdcNsf,
                     MSP_BUFHEADER_TYPE * pOutBuffLdcNsf)
{

    FILE *fp_inp, *fp_outp, *fp_outp_40;

    MSP_U32 i = 0;

#ifdef __PROFILE_LDCNSF__
    volatile long long overhead = 0;

    long long start = 0;

    // CycleCount;
#endif

    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    // MSP_APPCBPARAM_TYPE pLdcNsf_appcbptr;
    MSP_U32 input_filesize;

    char fout_name[200];

    char fin_name[200];

    strcpy(fout_name, OUTPUT_PATH);
    strcat(fout_name, LDCNSF_TestStructure[testnum].output_fname);

    if (LDCNSF_TestStructure[testnum].eInputFormat ==
        MSP_LDCNSF_YUV_FORMAT_YCBCR420)
        input_filesize =
            (MSP_U32) ((img_width_input) * (img_height_input) * 1.5);
    else
        input_filesize = (img_width_input) * (img_height_input) * mult_input;

    status = MSP_open(hNsfMSP, (MSP_PTR *) pLdcNsfQueryParam);
    MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    if (testnum == 34)
    {
        status = MSP_config(hNsfMSP, CONFIG_LDC_AFFINE, &affine_default_params);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        status =
            MSP_config(hNsfMSP, CONFIG_LDC_START_POSITION, &ldc_start_params);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        status =
            MSP_config(hNsfMSP, CONFIG_NSF_NOISE_FILTER,
                       (void *) ldcnsf_default_thrconfig);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        status =
            MSP_config(hNsfMSP, CONFIG_NSF_SHADING_CORRECTION,
                       (void *) ldcnsf_default_shdconfig);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        status =
            MSP_config(hNsfMSP, CONFIG_NSF_EDGE,
                       (void *) ldcnsf_default_edgeconfig);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        status =
            MSP_config(hNsfMSP, CONFIG_NSF_DESATURATION,
                       (void *) ldcnsf_default_desatconfig);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
    }

    if (testnum == 36)
    {
        pInBuffLdcNsf->pBuf[0] = pInBuffLdcNsf->pBuf[0] + 3;
        pOutBuffLdcNsf->pBuf[0] = pOutBuffLdcNsf->pBuf[0] + 5;
    }

    if (testnum == 41)
    {
        pInBuffLdcNsf->unBufSize[0] -= 500;                /* reduce the
                                                            * buffer size */
    }

    /* Generate the C_model output */
#ifdef GENERATE_CMODEL_FILES
    if (LDCNSF_TestStructure[testnum].eOperateMode == MODE_NSF_ONLY)
    {
        WriteOut_NSF_file(testnum, pLdcNsfQueryParam);
    }
    else if (LDCNSF_TestStructure[testnum].eOperateMode == MODE_CAC_AFFINE ||
             LDCNSF_TestStructure[testnum].eOperateMode == MODE_LDC_AFFINE ||
             LDCNSF_TestStructure[testnum].eOperateMode == MODE_AFFINE_ONLY)
    {
        WriteOut_LDC_file(testnum, pLdcNsfQueryParam);
    }
    else
    {
        WriteOut_NSF_file(testnum, pLdcNsfQueryParam);
        WriteOut_LDC_file(testnum, pLdcNsfQueryParam);
    }

#endif

#ifndef GENERATE_CMODEL_FILES

    status = MSP_process(hNsfMSP, NULL, pInBuffLdcNsf);
    if (testnum == 41 && status != MSP_ERROR_NONE)
    {
        TIMM_OSAL_TraceFunction("\n Smaller buffer than required is passed \n");
    }
    else
    {
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
    }

    if (testnum == 41)
    {
        pInBuffLdcNsf->unBufSize[0] += 500;
        status = MSP_process(hNsfMSP, NULL, pInBuffLdcNsf);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
    }

    if (testnum == 37)
    {
        status = MSP_process(hNsfMSP, NULL, pInBuffLdcNsf);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
    }

#ifdef __PROFILE_LDCNSF__
    start = Timestamp_get32();
    overhead = (Timestamp_get32() - start) - overhead;
    start = Timestamp_get32();
#endif

    status = MSP_process(hNsfMSP, NULL, pOutBuffLdcNsf);
    MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

    /* Wait for the task-thread to complete its job */
    Semaphore_pend(gSemHandleLdcNsf, BIOS_WAIT_FOREVER);
#ifdef ENABLE_PRINTF
    TIMM_OSAL_TraceFunction("\n LDC + NSF Processing done \n");
#endif

#ifdef __PROFILE_LDCNSF__
    CycleCount = (Timestamp_get32() - start) - overhead;
    TIMM_OSAL_MenuExt(TIMM_OSAL_TRACEGRP_SIMCOPALGOS,
                      "Exectuion time in cycles consumed is %d\n", CycleCount);
    System_flush();
#endif

    if (testnum == 39)
    {
        fp_outp_40 = fopen(fout_name, "wb");
        if (fp_outp_40 == NULL)
        {
            TIMM_OSAL_TraceFunction("error opening output file  file %s\n",
                                    fout_name);
            MSP_LDCNSF_EXIT_IF(fp_outp_40 == NULL, MSP_ERROR_FAIL);
        }
        /* Write-out the output of first image */
        fwrite((void *) (pOutBuffLdcNsf->pBuf[0] + ulDataOffset_Y),
               sizeof(MSP_U8),
               (MSP_U32) (LDCNSF_TestStructure[testnum].ulComputeHeight *
                          LDCNSF_TestStructure[testnum].ulOutputStride),
               fp_outp_40);
        fflush(fp_outp_40);
        fclose(fp_outp_40);

        /* Open another file for input */
        strcpy(fin_name, INPUT_PATH);
        strcat(fin_name, "176x144_test40.uyvy");

        fp_inp = fopen(fin_name, "rb");
        if (fp_inp == NULL)
        {
            TIMM_OSAL_TraceFunction("error opening output file  file %s\n",
                                    fin_name);
            MSP_LDCNSF_EXIT_IF(fp_inp == NULL, MSP_ERROR_FAIL);
        }

        fread((void *) pInBuffLdcNsf->pBuf[0], sizeof(unsigned char),
              input_filesize, fp_inp);
        fclose(fp_inp);

        /* Open another file for input */
        strcpy(fout_name, OUTPUT_PATH);
        strcat(fout_name, "176x144_test40_op.uyvy");

        for (i = 0; i < LDCNSF_STRESS_TESTING; i++)
        {
            status = MSP_process(hNsfMSP, NULL, pInBuffLdcNsf);
            MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

            status = MSP_process(hNsfMSP, NULL, pOutBuffLdcNsf);
            MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

            /* Wait for the task-thread to complete its job */
            Semaphore_pend(gSemHandleLdcNsf, BIOS_WAIT_FOREVER);
        }
        TIMM_OSAL_TraceFunction("\n LDC + NSF Processing done \n");
    }

#endif
    MSP_LDCNSF_EXIT_IF((MSP_close(hNsfMSP)) != MSP_ERROR_NONE, MSP_ERROR_FAIL);

#ifndef DBG_SERVER_SCRIPTING
    /* Write out the output in file */
    fp_outp = fopen(fout_name, "wb");
    if (fp_outp == NULL)
    {
        TIMM_OSAL_TraceFunction("error opening output file  file %s\n",
                                fout_name);
        MSP_LDCNSF_EXIT_IF(fp_outp == NULL, MSP_ERROR_FAIL);
    }
    fwrite((void *) (pOutBuffLdcNsf->pBuf[0] + ulDataOffset_Y), sizeof(MSP_U8),
           (MSP_U32) (LDCNSF_TestStructure[testnum].ulComputeHeight *
                      LDCNSF_TestStructure[testnum].ulOutputStride), fp_outp);
    fflush(fp_outp);

    if (LDCNSF_TestStructure[testnum].eOutputFormat ==
        MSP_LDCNSF_YUV_FORMAT_YCBCR420)
    {
        fwrite((void *) (pOutBuffLdcNsf->pBuf[1] + ulDataOffset_UV),
               sizeof(MSP_U8),
               (MSP_U32) (LDCNSF_TestStructure[testnum].ulComputeHeight / 2 *
                          LDCNSF_TestStructure[testnum].ulOutputStride),
               fp_outp);
        fflush(fp_outp);
    }
    fclose(fp_outp);
#else
    Y_op_ptr = (char *) (pOutBuffLdcNsf->pBuf[0] + ulDataOffset_Y);
    UV_op_ptr = (char *) (pOutBuffLdcNsf->pBuf[1] + ulDataOffset_UV);
    Y_size =
        LDCNSF_TestStructure[testnum].ulComputeHeight *
        LDCNSF_TestStructure[testnum].ulOutputStride;
    if (LDCNSF_TestStructure[testnum].eOutputFormat ==
        MSP_LDCNSF_YUV_FORMAT_YCBCR420)
        UV_size =
            LDCNSF_TestStructure[testnum].ulComputeHeight / 2 *
            LDCNSF_TestStructure[testnum].ulOutputStride;
    else
        UV_size = 0;
    Dummy_function();
#endif

    if (testnum > 43)
    {
        TIMM_OSAL_Free(pOutBuffLdcNsf->pBuf[0]);
        if (pLdcNsfQueryParam->eOutputFormat == MSP_LDCNSF_YUV_FORMAT_YCBCR420)
        {
            TIMM_OSAL_Free(pOutBuffLdcNsf->pBuf[1]);
        }
    }

    else
    {
        TIMM_OSAL_Free(pOutBuffLdcNsf->pBuf[0]);
        TIMM_OSAL_Free(pInBuffLdcNsf->pBuf[0]);

        if (pLdcNsfQueryParam->eOutputFormat == MSP_LDCNSF_YUV_FORMAT_YCBCR420)
        {
            TIMM_OSAL_Free(pOutBuffLdcNsf->pBuf[1]);
        }
    }

    return;

  EXIT:
    TIMM_OSAL_TraceFunction("Something went wrong!!\n");
    return;
}

/* ===================================================================
 *  @func     MSP_LDCNSF_RM_Callback                                               
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
static RM_STATUS MSP_LDCNSF_RM_Callback(RM_CALLBACK_CONDITION
                                        eCallbackCondition,
                                        RM_CLIENT_HANDLE pClientHandle,
                                        RM_RESOURCE_HANDLE hResourceHandle,
                                        void *pAppData,
                                        RM_RESOURCE_STATUS * pResourceStatus)
{

    return (RM_STATUS_SUCCESS);

}

#ifdef DBG_SERVER_SCRIPTING
/* ===================================================================
 *  @func     ldcnsf_test_main                                               
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
void ldcnsf_test_main(int testnum)
#else
/* ===================================================================
 *  @func     ldcnsf_test_main                                               
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
void ldcnsf_test_main()
#endif
{
    MSP_ERROR_TYPE status = MSP_ERROR_NONE;

    MSP_APPCBPARAM_TYPE pLdcNsf_appcbptr;

    MSP_HANDLE hNsfMSP, hLdcNsfMSP2;

    MSP_U16 i, j;

    MSP_LDCNSF_CREATE_PARAMS *pLdcNsfQueryParam;

    MSP_BUFHEADER_TYPE inBuffLdcNsf, outBuffLdcNsf;

    Semaphore_Params SemParams;

    MSP_U32 mem_count_start = 0;

    MSP_U32 mem_size_start = 0;

    MSP_U32 mem_count_end = 0;

    MSP_U32 mem_size_end = 0;

    Memory_Stats stats;

    FILE *fp_log;

#ifndef DBG_SERVER_SCRIPTING
    MSP_S8 testnum = 1;
#endif

#ifdef __PROFILE_LDCNSF__
    long long start;
#endif
    TEST_LDCNSF_CLIENT_HANDLETYPE *pTestLDCNSFHandle;

    RM_CLIENT_PARAMS stRMClientParams;

    RM_STATUS eRMStatus = RM_STATUS_SUCCESS;

    RM_CLIENT_CONFIG stRMClientConfig;

    RM_RESOURCE_PARAMS stRMResourceParams;

    Semaphore_Params_init(&SemParams);
    gSemHandleLdcNsf = Semaphore_create(0, &SemParams, NULL);

    Memory_getStats(NULL, &stats);
#ifdef  ENABLE_PRINTF
    TIMM_OSAL_TraceFunction("\n Total size = %d", stats.totalSize);
    TIMM_OSAL_TraceFunction("\n Total free size = %d", stats.totalFreeSize);
    TIMM_OSAL_TraceFunction("\n Largest Free size = %d", stats.largestFreeSize);
    TIMM_OSAL_TraceFunction("\n");
#endif
    mem_count_start = TIMM_OSAL_GetMemCounter();
    mem_size_start = TIMM_OSAL_GetMemUsage();
#if ENABLE_PRINTF
    TIMM_OSAL_TraceFunction("\n Value from GetMemCounter = %d",
                            mem_count_start);
    TIMM_OSAL_TraceFunction("\n Value from GetMemUsage = %d", mem_size_start);

    TIMM_OSAL_TraceFunction
        ("\n--------At the start of test---------------------------------------");
    TIMM_OSAL_TraceFunction("\n");
#endif

    /* Allocate the structure for MSP OPEN call configuration */
    pLdcNsfQueryParam =
        (MSP_LDCNSF_CREATE_PARAMS *)
        TIMM_OSAL_MallocExtn(sizeof(MSP_LDCNSF_CREATE_PARAMS), TIMM_OSAL_TRUE,
                             DEFAULT_BYTE_ALIGN, TIMMOSAL_MEM_SEGMENT_EXT,
                             NULL);
    MSP_LDCNSF_EXIT_IF(pLdcNsfQueryParam == NULL, MSP_ERROR_NULLPTR);
    pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams =
        (MSP_LDCNSF_LENS_DISTORTION_PARAMS *)
        TIMM_OSAL_MallocExtn(sizeof(MSP_LDCNSF_LENS_DISTORTION_PARAMS),
                             TIMM_OSAL_TRUE, DEFAULT_BYTE_ALIGN,
                             TIMMOSAL_MEM_SEGMENT_EXT, NULL);
    MSP_LDCNSF_EXIT_IF(pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams ==
                       NULL, MSP_ERROR_NULLPTR);
    pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->ptLutParams =
        (MSP_LDCNSF_LUT_PARAMS *)
        TIMM_OSAL_MallocExtn(sizeof(MSP_LDCNSF_LUT_PARAMS), TIMM_OSAL_TRUE,
                             DEFAULT_BYTE_ALIGN, TIMMOSAL_MEM_SEGMENT_EXT,
                             NULL);
    MSP_LDCNSF_EXIT_IF(pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->
                       ptLutParams == NULL, MSP_ERROR_NULLPTR);
    pLdcNsfQueryParam->tLdcParams.ptAffineParams =
        (MSP_LDCNSF_AFFINE_PARAMS *)
        TIMM_OSAL_MallocExtn(sizeof(MSP_LDCNSF_AFFINE_PARAMS), TIMM_OSAL_TRUE,
                             DEFAULT_BYTE_ALIGN, TIMMOSAL_MEM_SEGMENT_EXT,
                             NULL);
    MSP_LDCNSF_EXIT_IF(pLdcNsfQueryParam->tLdcParams.ptAffineParams == NULL,
                       MSP_ERROR_NULLPTR);

#ifndef DBG_SERVER_SCRIPTING
    TIMM_OSAL_TraceFunction("\n Enter the test number of LDCNSF \n");
    scanf("%d", &testnum);

    if (testnum <= 0 || testnum > MAX_LDCNSF_TEST_NUM)
    {
        TIMM_OSAL_TraceFunction
            ("\n This is an invalid test number. Enter a valid test number\n");
        status = MSP_ERROR_FATAL;
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FATAL);
    }
#endif
    testnum = testnum - 1;

    /* Initialize the Resource Manager */
    // RM_Init();

    pTestLDCNSFHandle =
        (TEST_LDCNSF_CLIENT_HANDLETYPE *)
        TIMM_OSAL_MallocExtn(sizeof(TEST_LDCNSF_CLIENT_HANDLETYPE),
                             TIMM_OSAL_TRUE, DEFAULT_BYTE_ALIGN,
                             TIMMOSAL_MEM_SEGMENT_EXT, NULL);

    stRMClientParams.nSize = sizeof(RM_CLIENT_PARAMS);
    stRMClientParams.pClientHandle = (RM_CLIENT_HANDLE) pTestLDCNSFHandle;
    stRMClientParams.nGroupID = 0;                         // pTestJpegHandle->nGroupID;
    stRMClientParams.bAutoDeQueue = TIMM_OSAL_FALSE;
    stRMClientParams.bAutoReturnNotify = TIMM_OSAL_FALSE;

    /* Get Resource Manager Handle independent of resources And register
     * Client */

    eRMStatus =
        RM_RegisterClient(&stRMClientParams, &pTestLDCNSFHandle->pRMHandle);
    MSP_LDCNSF_EXIT_IF(eRMStatus != RM_STATUS_SUCCESS,
                       MSP_ERROR_NOTENOUGHRESOURCES);

    stRMClientConfig.nSize = sizeof(RM_CLIENT_CONFIG);
    stRMClientConfig.bBlocking = TIMM_OSAL_TRUE;
    stRMClientConfig.pAppData = TIMM_OSAL_NULL;
    stRMClientConfig.eCallingCtx = RM_CLIENT_CALLINGCONTEXT_TASK;
    stRMClientConfig.nPriority = 0;
    stRMClientConfig.RM_callback = MSP_LDCNSF_RM_Callback;

    stRMResourceParams.eResourceName = RM_RESOURCE_NAME_ISS;
    stRMResourceParams.nSize = sizeof(RM_RESOURCE_PARAMS);
    stRMResourceParams.nResourceProtocolVersion = 0;
    stRMResourceParams.pExtResourceParams = TIMM_OSAL_NULL;

    /* Resource Handle for Simcop */

    eRMStatus = RM_GetResourceHandle(pTestLDCNSFHandle->pRMHandle,
                                     &stRMClientConfig,
                                     &stRMResourceParams,
                                     (RM_RESOURCE_HANDLE) (&pTestLDCNSFHandle->
                                                           IssRMhndl));

    MSP_LDCNSF_EXIT_IF(eRMStatus != RM_STATUS_SUCCESS,
                       MSP_ERROR_NOTENOUGHRESOURCES);
    eRMStatus =
        RM_AcquireResource(pTestLDCNSFHandle->pRMHandle,
                           pTestLDCNSFHandle->IssRMhndl,
                           &(pTestLDCNSFHandle->stIssStatus));

    /* Initialize the interrupt frameworks */
    iss_init();
    iss_module_clk_ctrl(ISS_SIMCOP_CLK, ISS_CLK_ENABLE);
    status = simcop_common_init();
    MSP_LDCNSF_EXIT_IF(status != CSL_SOK, MSP_ERROR_FAIL);

    /* Initialize the IP_RUN by creating the task and semaphores */
    IP_Run_Init();

    pLdcNsf_appcbptr.MSP_callback = MSP_LDCNSF_Callback;

    if (testnum == 42)
    {
        /* MSP_init for another instance */
        status =
            MSP_init(&hLdcNsfMSP2, "MSP.LDCNSF", MSP_PROFILE_REMOTE,
                     &pLdcNsf_appcbptr);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
    }

    if (testnum == 38)
    {
        status =
            MSP_init(&hNsfMSP, "MSP.LDCNSF", MSP_PROFILE_REMOTE,
                     &pLdcNsf_appcbptr);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
        for (i = 0; i < 34; i++)
        {
            j = i % 34;

            TIMM_OSAL_TraceFunction("Current Test No. = %d\n", j);
            ldcnsf_prepare_test(j, pLdcNsfQueryParam, &inBuffLdcNsf,
                                &outBuffLdcNsf);
            ldcnsf_run_test(hNsfMSP, j, pLdcNsfQueryParam, &inBuffLdcNsf,
                            &outBuffLdcNsf);

        }
        MSP_LDCNSF_EXIT_IF((MSP_deInit(hNsfMSP)) != MSP_ERROR_NONE,
                           MSP_ERROR_FAIL);
    }
    else if (testnum == 43)
    {
        fp_log = fopen("Log_LDCNSF.txt", "w");
        MSP_LDCNSF_EXIT_IF(fp_log == NULL, MSP_ERROR_FAIL);

        status =
            MSP_init(&hNsfMSP, "MSP.LDCNSF", MSP_PROFILE_REMOTE,
                     &pLdcNsf_appcbptr);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        for (i = 0; i < LDCNSF_STRESS_TESTING; i++)
        {
            start = Timestamp_get32();
            j = (start / 2) % 34;

            TIMM_OSAL_TraceFunction("Current Test No. = %d\n", j + 1);

            ldcnsf_prepare_test(j, pLdcNsfQueryParam, &inBuffLdcNsf,
                                &outBuffLdcNsf);
            ldcnsf_run_test(hNsfMSP, j, pLdcNsfQueryParam, &inBuffLdcNsf,
                            &outBuffLdcNsf);

            fprintf(fp_log, "Test No.= %d   Cycles = %d \n", j + 1, CycleCount);
        }
        fclose(fp_log);
        MSP_LDCNSF_EXIT_IF((MSP_deInit(hNsfMSP)) != MSP_ERROR_NONE,
                           MSP_ERROR_FAIL);

    }
    else
    {

        status =
            MSP_init(&hNsfMSP, "MSP.LDCNSF", MSP_PROFILE_REMOTE,
                     &pLdcNsf_appcbptr);
        MSP_LDCNSF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

        ldcnsf_prepare_test(testnum, pLdcNsfQueryParam, &inBuffLdcNsf,
                            &outBuffLdcNsf);
        ldcnsf_run_test(hNsfMSP, testnum, pLdcNsfQueryParam, &inBuffLdcNsf,
                        &outBuffLdcNsf);

        MSP_LDCNSF_EXIT_IF((MSP_deInit(hNsfMSP)) != MSP_ERROR_NONE,
                           MSP_ERROR_FAIL);
    }

    if (testnum == 42)
    {
        ldcnsf_prepare_test(1, pLdcNsfQueryParam, &inBuffLdcNsf,
                            &outBuffLdcNsf);
        ldcnsf_run_test(hLdcNsfMSP2, 1, pLdcNsfQueryParam, &inBuffLdcNsf,
                        &outBuffLdcNsf);

        MSP_LDCNSF_EXIT_IF((MSP_deInit(hLdcNsfMSP2)) != MSP_ERROR_NONE,
                           MSP_ERROR_FAIL);
    }

    TIMM_OSAL_TraceFunction
        ("\n MSP LDCNSF component test finished successfully...exiting...\n");

  EXIT:

    // RM_DeInit();
    /* De-initialize the IP_RUN framework */
    IP_Run_Deinit();

    TIMM_OSAL_Free(pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams->
                   ptLutParams);
    TIMM_OSAL_Free(pLdcNsfQueryParam->tLdcParams.ptLensDistortionParams);
    TIMM_OSAL_Free(pLdcNsfQueryParam->tLdcParams.ptAffineParams);
    TIMM_OSAL_Free(pLdcNsfQueryParam);

    // Memory_getStats(NULL, &stats);
#ifdef ENABLE_PRINTF
    TIMM_OSAL_TraceFunction("\n Total size = %d", stats.totalSize);
    TIMM_OSAL_TraceFunction("\n Total free size = %d", stats.totalFreeSize);
    TIMM_OSAL_TraceFunction("\n Largest Free size = %d", stats.largestFreeSize);
    TIMM_OSAL_TraceFunction("\n");
#endif
    mem_count_end = TIMM_OSAL_GetMemCounter();
    mem_size_end = TIMM_OSAL_GetMemUsage();
#if ENABLE_PRINTF
    TIMM_OSAL_TraceFunction("\n Value from GetMemCounter = %d", mem_count_end);
    TIMM_OSAL_TraceFunction("\n Value from GetMemUsage = %d", mem_size_end);
#endif
    if (mem_count_start != mem_count_end)
    {
        TIMM_OSAL_TraceFunction("\n Memory leak detected. Bytes lost = %d",
                                (mem_size_end - mem_size_start));
    }
    else
    {
        TIMM_OSAL_TraceFunction("\n NO MEMORY LEAKS..!");
    }
#if ENABLE_PRINTF
    TIMM_OSAL_TraceFunction
        ("\n-----------------------------------------------");
    TIMM_OSAL_TraceFunction("\n");
#endif

    return;
}
