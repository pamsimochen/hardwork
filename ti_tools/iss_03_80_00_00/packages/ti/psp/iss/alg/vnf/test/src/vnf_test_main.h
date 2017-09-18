/* ===========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file vnf_test_main.h
 *
 * This File contains declarations of structures and functions used 
 * in the MSP VNF component's test code; targeted at MONICA/OMAP4. 
 * 
 *
 * @path  $(DUCATIVOB)\alg\vnf\test\src\
 *
 * @rev  1.0
 */
/* ---------------------------------------------------------------------------- 
 *! 
 *! Revision History 
 *! ===================================
 *! 28-07-2009 Venkat Peddigari: Initial Release
 *!
 *!Revisions appear in reverse chronological order; 
 *!that is, newest first.  The date format is dd-mm-yyyy.  
 * =========================================================================== */
#ifndef _MSP_VNF_TEST_H
#define _MSP_VNF_TEST_H

#ifdef __cplusplus

extern "C" {
#endif

/* User code goes here */
/* ------compilation control switches -------------------------*/
/****************************************************************
*  INCLUDE FILES
****************************************************************/
/* ----- system and platform files ----------------------------*/
/*-------program files ----------------------------------------*/
#include "ti/psp/iss/alg/vnf/msp_vnf.h"
#include "ti/psp/iss/alg/rm/inc/rm.h"


/****************************************************************
*  EXTERNAL REFERENCES NOTE : only use if not found in header file
****************************************************************/
/****************************************************************
*  PUBLIC DECLARATIONS Defined here, used elsewhere
****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/


/* ========================================================================== */
/**
* @struct VnfTestInputType
* This structure is used for providing input to the test-framework of MSP VNF component.
*
* @param input_fname           : Input file name
* @param output_fname          : Output file name
* @param previous_fname        : Previous frame file name
* @param first_frm_fname       : First frame output filename
* @param ulInputWidth          : Input width of the input to LDC in pixels
* @param ulInputHeight         : Input height of the input to LDC in pixels
* @param ulComputeWidth        : Compute width used for processing data
* @param ulComputeHeight       : Compute height used for processing data
* @param ulInputStride         : Input stride of the input to LDC in bytes for YUV422 Interleaved or luma in case of YUV420 NV12 input
* @param ulInputStrideChroma  : Input stride of the input to LDC in bytes for chroma in case of YUV420 NV12 input
* @param ulOutputStrideLuma    : Output stride for the luma component in pixels
* @param ulOutputStrideChroma  : Output stride for the chroma component in pixels
* @param eOperateMode          : Operate mode for VNF component
* @param eInputFormat          : Input format
* @param eOutputFormat         : Output format
* @param eInterpolationLuma    : Interpolation method to be used for Y data by LDC module
* @param ptAffineParams        : Pointer to affine transform parameters
* @param ptTnfParams           : Pointer to temporal noise filter parameters
* @param tStartX               : LDC start X coordinate
* @param tStartY               : LDC start Y coordinate
* @param bLumaEn               : Enable luma filtering within NSF module
* @param bChromaEn             : Enable chroma filtering within NSF module
* @param eSmoothVal            : Smoothness parameter for NSF
* @param bSmoothLumaEn         : Enable smoothing operation of luma component
* @param bSmoothChromaEn       : Enable smoothing operation of chroma component
* @param eFilterParam          : Enable filtering feature of NSF
* @param eShdParam             : Enable shading correction feature of NSF
* @param eEdgeParam            : Enable edge enhancement feature of NSF
* @param eDesatParam           : Enable desaturation feature of NSF
* @param bDisablePipeDown    : Enable/Disable Pipe Down
* 
*/
/* ========================================================================== */
typedef struct
{
    char                              input_fname[100];
    char                              output_fname[100];
    char                              previous_fname[100];
	  char                              first_frm_fname[100];
    int                               NumFrames;
    MSP_U32                           ulInputWidth;
    MSP_U32                           ulInputHeight;
    MSP_U32                           ulComputeWidth;
    MSP_U32                           ulComputeHeight;
    MSP_U32                           ulInputStride;
    MSP_U32                           ulInputStrideChroma;	
    MSP_U32                           ulOutputStrideLuma;
    MSP_U32                           ulOutputStrideChroma;
    MSP_VNF_OPERATION_MODE            eOperateMode;
    MSP_VNF_DATA_FORMAT               eInputFormat;
    MSP_VNF_DATA_FORMAT               eOutputFormat;
    MSP_VNF_Y_INTERPOLATE_METHOD      eInterpolationLuma;
    MSP_VNF_AFFINE_PARAMS            *ptAffineParams;
    MSP_VNF_TNF_PARAMS               *ptTnfParams;
    MSP_U32                           tStartX;
    MSP_U32                           tStartY;
    MSP_BOOL                          bLumaEn;
    MSP_BOOL                          bChromaEn;
    MSP_VNF_SMOOTH_PARAM              eSmoothVal;
    MSP_BOOL                          bSmoothLumaEn;
    MSP_BOOL                          bSmoothChromaEn;
    MSP_VNF_PARAM_SET                 eFilterParam;
    MSP_VNF_PARAM_SET                 eShdParam;
    MSP_VNF_PARAM_SET                 eEdgeParam;
    MSP_VNF_PARAM_SET                 eDesatParam;
    MSP_BOOL                          bDisablePipeDown;
	MSP_VNF_NSF_SET                   eNsfSet;
	MSP_VNF_PARAM_SET                 eTnf3Preset;
}VnfTestInputType;
#ifdef RM_CODE_ENABLE
typedef struct{

            RM_HANDLE        pRMHandle;
            RM_RESOURCE_HANDLE        IssRMhndl;
            RM_RESOURCE_STATUS stIssStatus; 

}TEST_VNF_CLIENT_HANDLETYPE;
#endif
/****************************************************************
*  PRIVATE DECLARATIONS Defined here, used only here
****************************************************************/
/*--------data declarations -----------------------------------*/
/* Pre-defined affine parameters structures including default */
MSP_VNF_AFFINE_PARAMS  vnf_affine_default_params          = {4096, 0, 0, 0, 4096, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_1_params                = {4218, 0, 0, 0, 4218, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_2_params                = {3978, 0, 0, 0, 3978, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_3_params                = {3921, 0, 0, 0, 3921, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_4_params                = {3840, 0, 0, 0, 3840, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_horizontal_shift_params = {4096, 0, 100, 0, 4096, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_vertical_shift_params   = {4096, 0, 0, 0, 4096, 100};
MSP_VNF_AFFINE_PARAMS  vnf_affine_rotation_1_params       = {0, 4096, 0, 4096, 0, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_rotation_2_params       = {4096, 0, 0, 0, 4096, 0};
MSP_VNF_AFFINE_PARAMS  vnf_affine_rotation_3_params       = {4096, 0, 0, 0, 4096, 0};

/* Pre-defined temporal noise filter parameters structures including default */
MSP_VNF_TNF_PARAMS     tnf_default_params             = {16, 16, 16, 255, 13,1,0,MSP_VNF_PARAM_USER,13, MSP_VNF_PARAM_DEFAULT};
MSP_VNF_TNF_PARAMS     tnf_1_params                   = {16, 10, 10, 255, 15,1,0,MSP_VNF_PARAM_USER,13, MSP_VNF_PARAM_DEFAULT};

/* Pre-defined LDC start coordinates parameter structures including default */
MSP_VNF_START_LDC      ldc_startxy_default_params     = {0, 0};
MSP_VNF_START_LDC      ldc_startxy_1_params           = {10, 12};

/* Test-structure which holds parameters corresponding to the test-cases */
VnfTestInputType VNF_TestStructure[] =
{  
  //{"ipipe_320x240_420sp.yuv", "ipipe_320x240_420sp_output.yuv", "frame6.yuv", "first.nv12",   80, 320, 240, 288, 208, 320, 320, 288, 288, MODE_3DNF, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YINTERPOLATION_BICUBIC, &vnf_affine_default_params, &tnf_default_params,  0, 0, MSP_TRUE, MSP_TRUE, MSP_VNF_SMOOTH_1, MSP_TRUE, MSP_TRUE, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_FALSE,MSP_VNF_NSF_LUMA_CHROMA, MSP_VNF_PARAM_DEFAULT},
  //{"ipipe_640x480_420sp.yuv", "ipipe_640x480_420sp_output.yuv", "frame6.yuv", "first.nv12",    80, 640, 480, 608, 448, 640, 640, 608, 608, MODE_3DNF, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YINTERPOLATION_BICUBIC, &vnf_affine_default_params, &tnf_default_params,  0, 0, MSP_TRUE, MSP_TRUE, MSP_VNF_SMOOTH_1, MSP_TRUE, MSP_TRUE, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_FALSE,MSP_VNF_NSF_LUMA_CHROMA, MSP_VNF_PARAM_DEFAULT},
  //{"ipipe_960x768_420sp.yuv", "ipipe_960x768_420sp_output.yuv", "frame6.yuv", "first.nv12",    80, 960, 768, (960-32), (768-32), 960, 960, (960-32), (960-32), MODE_3DNF, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YINTERPOLATION_BICUBIC, &vnf_affine_default_params, &tnf_default_params,  0, 0, MSP_TRUE, MSP_TRUE, MSP_VNF_SMOOTH_1, MSP_TRUE, MSP_TRUE, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_FALSE,MSP_VNF_NSF_LUMA_CHROMA, MSP_VNF_PARAM_DEFAULT},
  //{"ipipe_320x240_420sp.yuv", "ipipe_320x240_420sp_output.yuv", "frame6.yuv", "first.nv12",    80, 320, 240, (320-32), (240-32), 320, 320, (320-32), (320-32), MODE_3DNF, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YINTERPOLATION_BICUBIC, &vnf_affine_default_params, &tnf_default_params,  0, 0, MSP_TRUE, MSP_TRUE, MSP_VNF_SMOOTH_1, MSP_TRUE, MSP_TRUE, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_FALSE, MSP_VNF_NSF_LUMA_CHROMA, MSP_VNF_PARAM_DEFAULT},
  {"ipipe_320x240_420sp.yuv", "ipipe_320x240_420sp_output.yuv", "frame6.yuv", "first.nv12",    80, 320, 240, (320-32), (240-32), 320, 320, (320-32), (320-32), MODE_TNF3, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YINTERPOLATION_BICUBIC, &vnf_affine_default_params, &tnf_default_params,  0, 0, MSP_TRUE, MSP_TRUE, MSP_VNF_SMOOTH_1, MSP_TRUE, MSP_TRUE, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_FALSE, MSP_VNF_NSF_LUMA_CHROMA, MSP_VNF_PARAM_DEFAULT},
  //{"dataset1_1280x960_420sp_original.yuv", "dataset1_1280x960_420sp_output.yuv", "frame6.yuv", "first.nv12",   80, 1280, 960, (1280-32), (960-32), 1280, 1280, (1280-32), (1280-32), MODE_3DNF, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YUV_FORMAT_YCBCR420, MSP_VNF_YINTERPOLATION_BICUBIC, &vnf_affine_default_params, &tnf_default_params,  0, 0, MSP_TRUE, MSP_TRUE, MSP_VNF_SMOOTH_1, MSP_TRUE, MSP_TRUE, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_VNF_PARAM_DEFAULT, MSP_FALSE, MSP_VNF_NSF_LUMA_CHROMA, MSP_VNF_PARAM_DEFAULT},
};


/* ======================================================================= */
/**
 * @def Input and output test vectors relative path related macros
 */
/* ======================================================================= */
#define INPUT_PATH          "/opt/ipnc/"
#define PREVIOUS_PATH       "/opt/ipnc/"
#define OUTPUT_PATH         "/opt/ipnc/"


/* ======================================================================= */
/**
 * @def MAX_VNF_TEST_NUM 
 * Maximum number of test cases allowed for VNF component
 */
/* ======================================================================= */
#define MAX_VNF_TEST_NUM 100


/* ======================================================================= */
/**
 * @def MSP_VNF_EXIT_IF 
 * Macro to check whether given condition fails or satisfies;
 * If fails, exit with file-name, function-name and line number.
 */
/* ======================================================================= */
#define MSP_VNF_EXIT_IF(_Cond,_ErrorCode) { \
if ((_Cond)) { \
  /* status = _ErrorCode;*/ \
  MSP_PRINT("Error :: %s : %s : %d :: Exiting because : %s\n", \
      __FILE__, __FUNCTION__, __LINE__, #_Cond); \
  goto EXIT; \
} \
}

/* ===========================================================================*/
/**
 * Update_TestParams_VNF()
 * Updates the create time parameter structure as per the inputs for the
 * given test case
 *
 * @param  pApp_params   : Handle to the create time parameters structure of 
 *                         MSP VNF component.
 * @param  testnum       : Test case number for the MSP VNF component
 *
 * @return none
 */
/* ===========================================================================*/
MSP_ERROR_TYPE Update_TestParams_VNF (MSP_VNF_CREATE_PARAMS* pApp_params, MSP_U8 testnum);


/* ================================================================*/
/**
 * MSP_VNF_Callback()
 * This is MSP VNF component callback implementation
 *
 * @param  hMSP          : Handle to MSP VNF component
 * @param  pAppData      : Handle to MSP VNF application data
 * @param  tEvent        : Handle to event structure
 * @param  nEventData1   : Event data type 1
 * @param  nEventData2   : Event data type 2
 *
 * @return MSP_ERROR_NONE    = success \n
 *         Other             = error
 */
/* ================================================================*/
static MSP_ERROR_TYPE MSP_VNF_Callback(MSP_PTR hMSP, MSP_PTR  pAppData, MSP_EVENT_TYPE tEvent, MSP_OPAQUE nEventData1, MSP_OPAQUE nEventData2);


/* ================================================================*/
/**
 * vnf_test_main()
 * This is the main function for the test bench of MSP VNF component
 *
 * @param   none
 *
 * @return  none
 */
/* ================================================================*/
void vnf_test_main();

#ifndef MSP_MEMORY
extern TIMM_OSAL_PTR TIMM_OSAL_MallocExtn(TIMM_OSAL_U32 size, 
                                   TIMM_OSAL_BOOL bBlockContiguous, 
                                   TIMM_OSAL_U32 unBlockAlignment, 
                                   TIMM_OSAL_U32 tMemSegId,
							       TIMM_OSAL_PTR hHeap);
extern void TIMM_OSAL_Free (TIMM_OSAL_PTR pData);
extern TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memset (TIMM_OSAL_PTR pBuffer, 
                                      TIMM_OSAL_U8 uValue, TIMM_OSAL_U32 uSize);
extern TIMM_OSAL_ERRORTYPE TIMM_OSAL_Memcpy (TIMM_OSAL_PTR pBufDst, 
										  TIMM_OSAL_PTR pBufSrc, 
										  TIMM_OSAL_U32 uSize);
#define MSP_VNF_Malloc(SIZE, BYTEALIGNMENT)      TIMM_OSAL_MallocExtn(SIZE, TIMM_OSAL_TRUE, BYTEALIGNMENT, 0, NULL)
#define MSP_VNF_Free(PTR)                        if(PTR != NULL) TIMM_OSAL_Free(PTR)
#define MSP_VNF_Memset(PTR, SIZE, VAL)           TIMM_OSAL_Memset(PTR, SIZE, VAL)
#define MSP_VNF_Memcpy(PTR_Dst, PTR_Src, USIZE)  TIMM_OSAL_Memcpy(PTR_Dst, PTR_Src, USIZE)
#else
#define MSP_VNF_Malloc(SIZE, BYTEALIGNMENT)      MSP_MallocExtn(SIZE, TIMM_OSAL_TRUE, BYTEALIGNMENT, 0, NULL)
#define MSP_VNF_Free(PTR)                        if(PTR != NULL) MSP_Free(PTR)
#define MSP_VNF_Memset(PTR, SIZE, VAL)           MSP_Memset(PTR, SIZE, VAL)
#define MSP_VNF_Memcpy(PTR_Dst, PTR_Src, USIZE)  MSP_Memcpy(PTR_Dst, PTR_Src, USIZE)
#endif

Ptr Utils_memAlloc(UInt32 size, UInt32 align);
Int32 Utils_memFree(Ptr addr, UInt32 size);

#define MSP_VNF_Malloc_Frame(SIZE, BYTEALIGNMENT)      Utils_memAlloc(SIZE, BYTEALIGNMENT)
#define MSP_VNF_Free_Frame(PTR, SIZE)                        if(PTR != NULL) Utils_memFree(PTR, SIZE)

#ifdef __cplusplus
}
#endif

#endif

