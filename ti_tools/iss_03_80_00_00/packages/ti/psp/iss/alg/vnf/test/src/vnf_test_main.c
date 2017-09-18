/* ===========================================================================
 *             Texas Instruments OMAP(TM) Platform Software
 *  (c) Copyright 2009, Texas Instruments Incorporated.  All Rights Reserved.
 *
 *  Use of this software is controlled by the terms and conditions found 
 *  in the license agreement under which this software has been supplied.
 * =========================================================================== */
/**
 * @file vnf_test_main.c
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

/* User code goes here */
/* ------compilation control switches -------------------------*/
/****************************************************************
 *  INCLUDE FILES                                                 
 ****************************************************************/
/* ----- system and platform files ----------------------------*/
/*-------program files ----------------------------------------*/
#define _4MACS

#define _SIMCOP


#ifdef CCS_PROJECT

#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/ipc/Semaphore.h>

#include "simcop.h"
#include "vnf_test_main.h"
#include "../../../../framework/msp/msp.h"
#include "../../../../framework/resource_manager/rm.h"
#include  "../../../../drivers/csl/iss/iss_common/iss_common.h"
#include "../../../../drivers/csl/iss/simcop/common/simcop_irq.h"

#else

#include<stdio.h> 
#include<stdlib.h>
#include<string.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/knl/Task.h>

//#define _REMOTE_FILE_IO

#ifdef _REMOTE_FILE_IO
#include "Rfile.h"
#endif
#include "vnf_test_main.h"
#include "ti/psp/iss/alg/jpeg_enc/inc/msp.h"
#include "ti/psp/iss/alg/rm/inc/rm.h"
#include "ti/psp/iss/hal/iss/simcop/common/simcop.h"
#include "ti/psp/iss/hal/iss/iss_common/iss_common.h"
#include "ti/psp/iss/hal/iss/simcop/common/simcop_irq.h"
#include "ti/psp/iss/alg/ip_run/inc/ip_run.h"
#include "ti/psp/iss/drivers/iss_init.h"

#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/platforms/iss_platform.h>

#endif
#include <ti/sysbios/knl/Clock.h> 
#include "ti/psp/iss/alg/evf/inc/cpisCore.h"
#include "ti/psp/iss/alg/evf/inc/cpisImgproc.h"


#ifndef _REMOTE_FILE_IO
#define Rfile_printf	printf
#define Rfile_Handle    FILE *
#define Rfile_alloc		MSP_VNF_Malloc
#define Rfile_open		fopen
#define Rfile_readLine  fgets
#define Rfile_close     fclose
#define Rfile_read      fread
#define Rfile_write		fwrite
#define Rfile_free		MSP_VNF_Free
#endif

volatile int vnf_ccs_wait = 0;

#ifdef MODE_3DNF_CHANGES
#define SPATIAL_FILTER_DIABLE
#endif
/****************************************************************
 * EXTERNAL REFERENCES NOTE : only use if not found in header file
 ****************************************************************/
/*--------data declarations -----------------------------------*/
/*--------function prototypes ---------------------------------*/
/*--------macros ----------------------------------------------*/
#define FILE_IO_TEST
//#define __PROFILE_VNF__
#define ENABLE_VNF_PRINTF
//#define VNF_MEMORY_LEAK
#define WRITE_VNF_FIRST_FRM
#define ZEBU_TESTING
MSP_U32 STRESSTESTNUM = 1000;
MSP_U32 regtestnum    =   14;

#pragma DATA_ALIGN(gCaptureApp_tskStackMain, 32)
#pragma DATA_SECTION(gCaptureApp_tskStackMain, ".bss:taskStackSection")
UInt8 gCaptureApp_tskStackMain[10*1024];

#define WIDTH (640)
#define HEIGHT (256)
UInt8 *inputBuffer;
UInt8 *coeffBuffer;
UInt8 *outputBuffer;



/****************************************************************
 *  PUBLIC DECLARATIONS Defined here, used elsewhere
 ****************************************************************/
/*--------data declarations -----------------------------------*/
static Semaphore_Handle       gSemHandleVnf;    

#ifdef MODE_3DNF_CHANGES  
static MSP_U8  spatial_filter_enable_stagea;
static MSP_U8  stageAPassthrough;
static MSP_U32  offset;
static MSP_U8* dummyptr;
#endif

extern const MSP_U16          vnf_ldc_lut_0[];

extern MSP_U8                 vnf_default_ldckhl;
extern MSP_U8                 vnf_default_ldckhr ;
extern MSP_U8                 vnf_default_ldckvl ;
extern MSP_U8                 vnf_default_ldckvu ;
extern MSP_U16                vnf_default_Rth ;
extern MSP_U8                 vnf_default_rightShiftBits;

extern MSP_S16                vnf_user_thrconfig[]; 
#ifdef MODE_3DNF_CHANGES
extern MSP_S16                vnf_user_Quad_thrconfig[];
extern MSP_S16                vnf_user_Half_thrconfig[];
#endif
extern MSP_S16                vnf_user_shdconfig[];
extern MSP_S16                vnf_user_edgeconfig[];
extern MSP_S16                vnf_user_desatconfig[];

#ifdef FILE_IO_TEST
static MSP_U32                ulVnfDataOffset_Y  = 0; 
static MSP_U32                ulVnfDataOffset_UV = 0; 
#endif

#if 1
MSP_S16 shift_factor_for_3dnf_diff ; 
MSP_S16 scale_factor_for_3dnf_diff ; 
extern MSP_S16 vnf_default_thrconfig[] ;
extern MSP_S16 vnf_default_Half_thrconfig[];
extern MSP_S16 vnf_default_Quad_thrconfig[];
extern MSP_S16  vnf_user_alpha_lookup[];

MSP_S16 test_shift_factor_for_3dnf_diff   = 0; //MODE_3DNF_DIFF_SHIFT_FACTOR  2
MSP_S16 test_scale_factor_for_3dnf_diff   = 1; //MODE_3DNF_DIFF_SCALE_FACTOR  12
MSP_S16 test_vnf_default_thrconfig[]      = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // For Stage A
MSP_S16 test_vnf_default_Half_thrconfig[] = { 40, 0, 20, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
MSP_S16 test_vnf_default_Quad_thrconfig[] = { 10, 0, 6, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
MSP_S16 test_vnf_user_alpha_lookup[]     = {
        204, 204, 204, 204, 204, 204, 204, 204,  
        163,  122,  81,  40,  0, 0, 0, 0,   
        0,   0,   0,   0,   0,  0,  0,  0,
        0,   0,   0,   0,   0,  0,  0,  0
};


#endif





#define BUF_SIZE    (1024*16)



// Taken from gel file
#define OSC_0                    20
#define PLL_BASE_ADDRESS         0x481C5000
#define ISS_PLL_BASE            (PLL_BASE_ADDRESS+0x140)
#define M2NDIV                  0x10
#define MN2DIV                  0x14
#define	M2N_PLL                 (volatile Uint32*)(ISS_PLL_BASE+M2NDIV)
#define	M_PLL                   (volatile Uint32*)(ISS_PLL_BASE+MN2DIV)

// The timer is driven by clock at OSC_0 Mhz
#define TIOCP_CFG   ((volatile Uint32*)0x48042010)
#define TCLR        ((volatile Uint32*)0x48042038)
#define TCRR        ((volatile Uint32*)0x4804203C)
static Uint16 armScale;
static unsigned long long freq;
static Uint32 ratioVicpArm;
static Uint16 armMhz, vicpMhz;

Uint16 getARMmhz()
{

    Uint16 M,N,M2;

    M= *M_PLL;
    N= *M2N_PLL  & 0xFFFF;
    M2= (*M2N_PLL >> 16)  & 0xFFFF;
    return (((OSC_0*M)/(N+1))/(2*M2));
}

Uint16 getVICPmhz(){

    Uint16 M,N,M2;

    M= *M_PLL;
    N= *M2N_PLL  & 0xFFFF;
    M2= (*M2N_PLL >> 16)  & 0xFFFF;
    return(((OSC_0*M)/(N+1))/M2);

}

/* Initialize timer and start it */
Int32 timerInit(){

	Types_FreqHz temp;
    Timestamp_getFreq(&temp);
    freq = temp.hi;
    freq = (freq<<32)|temp.lo;

    freq = freq/1000;

    armMhz=getARMmhz();
    vicpMhz=getVICPmhz();

    ratioVicpArm= (vicpMhz<<3)/armMhz;

    return 0;
}

#if 1
/* Set counter to 0 and start timer */
Uint32 timerReadStart(){
	Types_Timestamp64 temp;
	long long temp64;
	
    Timestamp_get64(&temp);

    temp64= (long long)8*temp.lo;
    return ( (Uint32)(temp64 / ratioVicpArm));
}

/* Stop timer and returns counter at end of benchmark region, scaled at ARM frequency */
Uint32 timerReadEnd(){

	Types_Timestamp64 temp;
	long long temp64;

    Timestamp_get64(&temp);

    temp64= (long long)8*temp.lo;
    return ( (Uint32)(temp64 / ratioVicpArm));
}
#else
/* Set counter to 0 and start timer */
Uint32 timerReadStart(){
    return ( (((Uint32)Timestamp_get32()*8) / ratioVicpArm));
}

/* Stop timer and returns counter at end of benchmark region, scaled at ARM frequency */
Uint32 timerReadEnd(){

    return ( (((Uint32)Timestamp_get32()*8) / ratioVicpArm));
}
#endif


UInt64 Utils_getCurTimeInUsec()
 {
 static UInt32 cpuMhz = 500; // default
     static Bool isInit = FALSE;

     Types_Timestamp64 ts64;
     UInt64 curTs;

     if(!isInit)
     {
         /* do this only once */

         Types_FreqHz cpuHz;

         isInit = TRUE;

         Timestamp_getFreq(&cpuHz);

         cpuMhz = cpuHz.lo / (1000*1000); /* convert to Mhz */

         Vps_printf(" \n");
         Vps_printf(" *** UTILS: CPU MHz = %d Mhz ***\n", cpuMhz);
         Vps_printf(" \n");


     }

     Timestamp_get64(&ts64);

     curTs = ((UInt64) ts64.hi << 32) | ts64.lo;

     return (curTs/cpuMhz)*100;
 }
 
 UInt32 Utils_setCpuFrequency (UInt32 freq)
 {
     UInt cookie;
     Types_FreqHz cpuHz;
     Types_FreqHz OldCpuHz;

     BIOS_getCpuFreq(&OldCpuHz);

     cookie = Hwi_disable();
     cpuHz.lo = freq;
     cpuHz.hi = 0;
     ti_sysbios_BIOS_setCpuFreq(&cpuHz);
     Clock_tickStop();
     Clock_tickReconfig();
     Clock_tickStart();
     Hwi_restore(cookie);

     BIOS_getCpuFreq(&cpuHz);
     Vps_printf("***** SYSTEM  : Frequency <ORG> - %d, <NEW> - %d\n", OldCpuHz.lo, cpuHz.lo);
     return 0;
 }





void MSP_GLBCE_init (void)
{
    return;
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
MSP_ERROR_TYPE Update_TestParams_VNF1 (MSP_VNF_CREATE_PARAMS* pApp_params, MSP_U8 testnum)
{
    MSP_ERROR_TYPE       status      = MSP_ERROR_NONE;
    MSP_VNF_LDC_PARAMS                 *ldc_params             = pApp_params->ptLdcParams;
    MSP_VNF_LENS_DISTORTION_PARAMS     *lens_distortion_params = ldc_params->ptLensDistortionParams;
    MSP_VNF_LUT_PARAMS                 *lutParams              = lens_distortion_params->ptLutParams;
    MSP_VNF_NSF_PARAMS                 *nsf_params             = pApp_params->ptNsfParams;
    MSP_VNF_TNF_PARAMS                 *tnf_params             = pApp_params->ptTnfParams;

    int i;
    pApp_params->ulComputeWidth  = VNF_TestStructure[testnum].ulComputeWidth;
    pApp_params->ulComputeHeight = VNF_TestStructure[testnum].ulComputeHeight;
    pApp_params->eOperateMode    = VNF_TestStructure[testnum].eOperateMode;
    pApp_params->eInputFormat    = VNF_TestStructure[testnum].eInputFormat;
    pApp_params->eOutputFormat   = VNF_TestStructure[testnum].eOutputFormat;
    pApp_params->bDisablePipeDown   = VNF_TestStructure[testnum].bDisablePipeDown;

#ifdef MODE_3DNF_CHANGES    
    //pApp_params->ptNsfParams->bSpatialFilterEnableForStageA = spatial_filter_enable_stagea; //Need to get this from test structure
    //    spatial_filter_enable_stagea =  pApp_params->ptNsfParams->bSpatialFilterEnableForStageA;
#endif
    pApp_params->ulInputStride = VNF_TestStructure[testnum].ulInputStride;
    pApp_params->ulInputStrideChroma = VNF_TestStructure[testnum].ulInputStrideChroma;

    if (VNF_TestStructure[testnum].eOutputFormat == MSP_VNF_YUV_FORMAT_YCBCR420)
    {
        pApp_params->ulOutputStrideLuma    = (MSP_U32)(VNF_TestStructure[testnum].ulOutputStrideLuma);
        pApp_params->ulOutputStrideChroma  = (MSP_U32)(VNF_TestStructure[testnum].ulOutputStrideChroma);
    }
    else
    {
        MSP_VNF_EXIT_IF(1, MSP_ERROR_INVALIDCONFIG); 
    }

    ldc_params->unPixelPad = 6;

    ldc_params->tStartXY.unStartX                     = VNF_TestStructure[testnum].tStartX;
    ldc_params->tStartXY.unStartY                     = VNF_TestStructure[testnum].tStartY;
    ldc_params->ptLensDistortionParams->unLensCentreX = 40;
    ldc_params->ptLensDistortionParams->unLensCentreY = 32;
    ldc_params->eYInterpolationMethod                 = VNF_TestStructure[testnum].eInterpolationLuma;

    lutParams->unLdcKhl         = vnf_default_ldckhl;
    lutParams->unLdcKhr         = vnf_default_ldckhr;
    lutParams->unLdcKvl         = vnf_default_ldckvl;
    lutParams->unLdcKvu         = vnf_default_ldckvu;
    lutParams->unLdcRth         = vnf_default_Rth;
    lutParams->ucRightShiftBits = vnf_default_rightShiftBits;
    lutParams->punLdcLutTable   = (MSP_U16 *)vnf_ldc_lut_0;

    ldc_params->ptAffineParams->unAffineA = VNF_TestStructure[testnum].ptAffineParams->unAffineA;
    ldc_params->ptAffineParams->unAffineB = VNF_TestStructure[testnum].ptAffineParams->unAffineB;
    ldc_params->ptAffineParams->unAffineC = VNF_TestStructure[testnum].ptAffineParams->unAffineC;
    ldc_params->ptAffineParams->unAffineD = VNF_TestStructure[testnum].ptAffineParams->unAffineD;
    ldc_params->ptAffineParams->unAffineE = VNF_TestStructure[testnum].ptAffineParams->unAffineE;
    ldc_params->ptAffineParams->unAffineF = VNF_TestStructure[testnum].ptAffineParams->unAffineF;

    nsf_params->bChromaEnable  = VNF_TestStructure[testnum].bChromaEn;
    nsf_params->bLumaEnable    = VNF_TestStructure[testnum].bLumaEn;
    nsf_params->bSmoothChroma  = VNF_TestStructure[testnum].bSmoothChromaEn;
    nsf_params->bSmoothLuma    = VNF_TestStructure[testnum].bSmoothLumaEn;
    nsf_params->eSmoothVal     = VNF_TestStructure[testnum].eSmoothVal;
	nsf_params->eNsfSet        = VNF_TestStructure[testnum].eNsfSet;
    if(stageAPassthrough)
    {   
        nsf_params->eDesatParam    = MSP_VNF_PARAM_DISABLE;//VNF_TestStructure[testnum].eDesatParam;
        nsf_params->eEdgeParam     = MSP_VNF_PARAM_DISABLE;//VNF_TestStructure[testnum].eEdgeParam;
        nsf_params->eShdParam      = MSP_VNF_PARAM_DISABLE;//VNF_TestStructure[testnum].eShdParam;
        nsf_params->eFilterParam   = MSP_VNF_PARAM_DISABLE;//VNF_TestStructure[testnum].eFilterParam;
    }
    else
    {
        nsf_params->eDesatParam    = VNF_TestStructure[testnum].eDesatParam;
        nsf_params->eEdgeParam     = VNF_TestStructure[testnum].eEdgeParam;
        nsf_params->eShdParam      = VNF_TestStructure[testnum].eShdParam;
        nsf_params->eFilterParam   = VNF_TestStructure[testnum].eFilterParam;
    }
    nsf_params->eHalfFilterParam = MSP_VNF_PARAM_DEFAULT;
    nsf_params->eQuadFilterParam = MSP_VNF_PARAM_DEFAULT; 

    tnf_params->unYComponentWt      = VNF_TestStructure[testnum].ptTnfParams->unYComponentWt;
    tnf_params->unUComponentWt      = VNF_TestStructure[testnum].ptTnfParams->unUComponentWt;
    tnf_params->unVComponentWt      = VNF_TestStructure[testnum].ptTnfParams->unVComponentWt;
    tnf_params->unMaxBlendingFactor = VNF_TestStructure[testnum].ptTnfParams->unMaxBlendingFactor;
    tnf_params->unMotionThreshold   = VNF_TestStructure[testnum].ptTnfParams->unMotionThreshold;
    tnf_params->unDiffScaleFactor   = test_scale_factor_for_3dnf_diff;// VNF_TestStructure[testnum].ptTnfParams->unMotionThreshold;
    tnf_params->unDiffShiftFactor   = test_shift_factor_for_3dnf_diff;//VNF_TestStructure[testnum].ptTnfParams->unMotionThreshold;        
    tnf_params->eTnfAlphaLUTParam   = VNF_TestStructure[testnum].ptTnfParams->eTnfAlphaLUTParam;
	tnf_params->eTnf3Preset         = VNF_TestStructure[testnum].ptTnfParams->eTnf3Preset;

    if(VNF_TestStructure[testnum].eDesatParam == MSP_VNF_PARAM_USER)
    {
        nsf_params->rgnDesatVal = (MSP_S16 *)vnf_user_desatconfig;
    }

    if(VNF_TestStructure[testnum].eEdgeParam == MSP_VNF_PARAM_USER)
    {
        nsf_params->rgnEdgeVal = (MSP_S16 *)vnf_user_edgeconfig;
    }

    if(VNF_TestStructure[testnum].eShdParam == MSP_VNF_PARAM_USER)
    {
        nsf_params->rgnShdVal = (MSP_S16 *)vnf_user_shdconfig;
    }

    if(VNF_TestStructure[testnum].eFilterParam == MSP_VNF_PARAM_USER)
    {
        nsf_params->rgnFilterVal     = (MSP_S16 *)vnf_user_thrconfig;
#ifdef MODE_3DNF_CHANGES        
        nsf_params->rgnHalfFilterVal = (MSP_S16 *)vnf_user_Half_thrconfig;
        nsf_params->rgnQuadFilterVal = (MSP_S16 *)vnf_user_Quad_thrconfig;
#endif        
    }
#if 1
    shift_factor_for_3dnf_diff = test_shift_factor_for_3dnf_diff;
    scale_factor_for_3dnf_diff = test_scale_factor_for_3dnf_diff;
    memcpy(vnf_default_thrconfig,test_vnf_default_thrconfig,sizeof(MSP_VNF_FilterConfig));
    memcpy(vnf_default_Half_thrconfig,test_vnf_default_Half_thrconfig,sizeof(MSP_VNF_FilterConfig));
    memcpy(vnf_default_Quad_thrconfig,test_vnf_default_Quad_thrconfig,sizeof(MSP_VNF_FilterConfig));
    memcpy(vnf_user_alpha_lookup,test_vnf_user_alpha_lookup,sizeof(MSP_S16)*32);
    Rfile_printf("shift_factor_for_3dnf_diff = %d \n",shift_factor_for_3dnf_diff);
    Rfile_printf("scale_factor_for_3dnf_diff = %d \n",scale_factor_for_3dnf_diff);
    for(i = 0; i < 18; i++)
    {    
        Rfile_printf("vnf_default_thrconfig[%2d]      = %d \n",i,vnf_default_thrconfig[i]);
        Rfile_printf("vnf_default_Half_thrconfig[%2d] = %d \n",i,vnf_default_Half_thrconfig[i]);
        Rfile_printf("vnf_default_Quad_thrconfig[%2d] = %d \n",i,vnf_default_Quad_thrconfig[i]);
    }
    for(i = 0; i < 32; i++)
    {    
        Rfile_printf("test_vnf_user_alpha_lookup[%2d]      = %d \n",i,vnf_user_alpha_lookup[i]);
    }

#endif
    EXIT:
    return status;

}


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
 * @return  MSP_ERROR_NONE    = success \n
 *          Other             = error
 */
/* ================================================================*/
static MSP_ERROR_TYPE MSP_VNF_Callback(MSP_PTR hMSP, MSP_PTR  pAppData, MSP_EVENT_TYPE tEvent, MSP_OPAQUE nEventData1, MSP_OPAQUE nEventData2)
{
    MSP_ERROR_TYPE       status      = MSP_ERROR_NONE;

    switch(tEvent)
    {
    case MSP_OPEN_EVENT:
    case MSP_CLOSE_EVENT:
    case MSP_PROFILE_EVENT:
    case MSP_CTRLCMD_EVENT:
    case MSP_ERROR_EVENT: 
        break;

    case MSP_DATA_EVENT:
        if(nEventData1 == MSP_VNF_INPUT_PORT)
        {
            break;
        }

        if(nEventData1 == MSP_VNF_OUTPUT_PORT)
        {
#ifdef FILE_IO_TEST
/* Type cast the nEventData2 as buffer header */
            MSP_BUFHEADER_TYPE  *pBufHeader = (MSP_BUFHEADER_TYPE *)nEventData2;

            ulVnfDataOffset_Y  = pBufHeader->unBufOffset[0];
            ulVnfDataOffset_UV = pBufHeader->unBufOffset[1];    
#endif

            /* Post the semaphore and write output data to file */
            Semaphore_post(gSemHandleVnf);
            break;
        }

    default:
        status = MSP_ERROR_INVALIDCMD;
        break;
    }

    return status;
}


/* ================================================================*/
/**
 * vnf_test_main()
 * This is the main function for the test bench of MSP VNF component
 *
 * @param   none
 *
 * @return  MSP_ERROR_NONE    = success \n
 *          Other             = error
 */
/* ================================================================*/
#ifdef CCS_PROJECT

int main()

#else

void vnf_main()

#endif  
{
#ifdef FILE_IO_TEST
    // FILE                    *fp_inp, *fp_outp;
#endif
    MSP_VNF_CREATE_PARAMS   *pVnfQueryParam;
    MSP_APPCBPARAM_TYPE      pVnf_appcbptr;
    MSP_HANDLE               hVnfMSP;
    MSP_BUFHEADER_TYPE       inBuffVnf, outBuffVnf, currOutBuffVnf,prevInBuffVnf;
    char                     fin_name[200];
    char                     fprev_name[200];
    char                     fout_name[200];
    char                     fprev_out_name[200];
    unsigned char * tempPrevPtr0;
    unsigned char * tempPrevPtr1;
    int frameNum;
    Semaphore_Params         SemParams;
    MSP_S8                   testnum       = 1;
    MSP_ERROR_TYPE           status        = MSP_ERROR_NONE;
    MSP_U32                  i             = 0;
    MSP_U8 offset ;
#ifdef MODE_3DNF_CHANGES    
    MSP_U32                  j            = 0;
    MSP_U32                  k           = 0;
#endif    
#ifndef FILE_IO_TEST
    Bool debug0 = 1, debug1 = 1, debug2 = 1;
#endif
    Ptr              srcBuf = NULL;
    Ptr              dstBuf = NULL;
    Rfile_Handle     fp_inp = NULL;
    Rfile_Handle     fp_outp = NULL;
    Rfile_Handle     fp_config = NULL;
    unsigned char configLine[300]; 
    unsigned char test_name[300];
    int           test_value;

#ifdef __PROFILE_VNF__
    volatile MSP_U32     overhead      = 0;
    MSP_U32              start;
    MSP_U32              CycleCountVnfMain;
#endif

#ifdef VNF_MEMORY_LEAK
    Memory_Stats stats;
    MSP_U32      mem_count_start = 0;
    MSP_U32      mem_size_start  = 0;
    MSP_U32      mem_count_end   = 0;
    MSP_U32      mem_size_end    = 0;



    Memory_getStats(NULL, &stats);
    Rfile_printf("\n");
    Rfile_printf("Total Size = %d \n", stats.totalSize);
    Rfile_printf("Total FreeSize = %d \n", stats.totalFreeSize);
    Rfile_printf("Largest Free Size = %d \n", stats.largestFreeSize);
    Rfile_printf("\n");

    /* Get memory status before running the test case */
    mem_count_start = TIMM_OSAL_GetMemCounter();
    mem_size_start  = TIMM_OSAL_GetMemUsage();

    Rfile_printf("--------At the start of test--------");
    Rfile_printf("\n");
    Rfile_printf("Value from GetMemCounter = %d \n", mem_count_start);
    Rfile_printf("Value from GetMemUsage   = %d \n", mem_size_start);

#endif


     
    

    Semaphore_Params_init(&SemParams);
    gSemHandleVnf = Semaphore_create(0, &SemParams, NULL);
	
	{
	 unsigned int startTime, stopTime, totalTime;
	    startTime = Utils_getCurTimeInUsec();
	        Semaphore_pend(gSemHandleVnf, 1);
		stopTime  = Utils_getCurTimeInUsec();   
		
		printf("Total Time taken is %d microsec\n", (stopTime - startTime)/100);
	
	}

    /* Allocate the memory for the create time parameters struture */
    pVnfQueryParam              = (MSP_VNF_CREATE_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_CREATE_PARAMS), DEFAULT_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(pVnfQueryParam == NULL, MSP_ERROR_NULLPTR);
    pVnfQueryParam->ptLdcParams = (MSP_VNF_LDC_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_LDC_PARAMS), DEFAULT_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(pVnfQueryParam->ptLdcParams == NULL, MSP_ERROR_NULLPTR);
    pVnfQueryParam->ptNsfParams = (MSP_VNF_NSF_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_NSF_PARAMS), DEFAULT_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(pVnfQueryParam->ptNsfParams == NULL, MSP_ERROR_NULLPTR);
    pVnfQueryParam->ptTnfParams = (MSP_VNF_TNF_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_TNF_PARAMS), DEFAULT_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(pVnfQueryParam->ptTnfParams == NULL, MSP_ERROR_NULLPTR);

    pVnfQueryParam->ptLdcParams->ptLensDistortionParams              = (MSP_VNF_LENS_DISTORTION_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_LENS_DISTORTION_PARAMS), DEFAULT_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(pVnfQueryParam->ptLdcParams->ptLensDistortionParams == NULL, MSP_ERROR_NULLPTR);
    pVnfQueryParam->ptLdcParams->ptAffineParams                      = (MSP_VNF_AFFINE_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_AFFINE_PARAMS), DEFAULT_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(pVnfQueryParam->ptLdcParams->ptAffineParams == NULL, MSP_ERROR_NULLPTR);
    pVnfQueryParam->ptLdcParams->ptLensDistortionParams->ptLutParams = (MSP_VNF_LUT_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_LUT_PARAMS), DEFAULT_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(pVnfQueryParam->ptLdcParams->ptLensDistortionParams->ptLutParams == NULL, MSP_ERROR_NULLPTR);


    testnum = 1;
    if(testnum <= 0 || testnum > MAX_VNF_TEST_NUM)
    {
        Rfile_printf("\n This is an invalid test number. Enter a valid test number \n");
        status = MSP_ERROR_FATAL;
        MSP_VNF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FATAL);
    }

    testnum = testnum -1;

    srcBuf = Rfile_alloc(BUF_SIZE, 16);
    Rfile_printf("srcBuf is %x",  srcBuf);

    dstBuf = Rfile_alloc(BUF_SIZE, 16);
    Rfile_printf("dstBuf is %x",  dstBuf);

    Rfile_printf("Opening Config file \n");
    fp_config  = Rfile_open("config.txt", "r");



    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%s", VNF_TestStructure[testnum].input_fname);
    Rfile_printf("%s \n",VNF_TestStructure[testnum].input_fname);

    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%s", VNF_TestStructure[testnum].output_fname);
    Rfile_printf("%s \n",VNF_TestStructure[testnum].output_fname);

    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%d", &(VNF_TestStructure[testnum].ulInputWidth));
    Rfile_printf("%d \n",VNF_TestStructure[testnum].ulInputWidth);

    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%d", &(VNF_TestStructure[testnum].ulInputHeight));
    Rfile_printf("%d \n",VNF_TestStructure[testnum].ulInputHeight);

    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%d", &(VNF_TestStructure[testnum].NumFrames));
    Rfile_printf("%d \n",VNF_TestStructure[testnum].NumFrames);

    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%d", &(spatial_filter_enable_stagea));
    Rfile_printf("%d \n",spatial_filter_enable_stagea);

    //Rfile_readLine(configLine,300,fp_config);
    //sscanf(configLine, "%d", &(VNF_TestStructure[testnum].eNsfSet));
    //Rfile_printf("%d \n",VNF_TestStructure[testnum].eNsfSet);


    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%d", &(stageAPassthrough));
    Rfile_printf("%d \n",stageAPassthrough);

    if(spatial_filter_enable_stagea)
    {
        VNF_TestStructure[testnum].ulComputeHeight           = VNF_TestStructure[testnum].ulInputHeight-32;
        VNF_TestStructure[testnum].ulComputeWidth            = VNF_TestStructure[testnum].ulInputWidth-32;

        VNF_TestStructure[testnum].ulInputStride             = VNF_TestStructure[testnum].ulInputWidth;
        VNF_TestStructure[testnum].ulInputStrideChroma       = VNF_TestStructure[testnum].ulInputWidth;
        VNF_TestStructure[testnum].ulOutputStrideLuma        = VNF_TestStructure[testnum].ulInputWidth/*-32*/;
        VNF_TestStructure[testnum].ulOutputStrideChroma      = VNF_TestStructure[testnum].ulInputWidth/*-32*/;
    }
    else
    {

        VNF_TestStructure[testnum].ulComputeHeight           = VNF_TestStructure[testnum].ulInputHeight;
        VNF_TestStructure[testnum].ulComputeWidth            = VNF_TestStructure[testnum].ulInputWidth;

        VNF_TestStructure[testnum].ulInputStride             = VNF_TestStructure[testnum].ulInputWidth;
        VNF_TestStructure[testnum].ulInputStrideChroma       = VNF_TestStructure[testnum].ulInputWidth;
        VNF_TestStructure[testnum].ulOutputStrideLuma        = VNF_TestStructure[testnum].ulInputWidth;
        VNF_TestStructure[testnum].ulOutputStrideChroma      = VNF_TestStructure[testnum].ulInputWidth;
    }

    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%hd", &(test_scale_factor_for_3dnf_diff));
    Rfile_printf("%d \n",test_scale_factor_for_3dnf_diff);

    Rfile_readLine(configLine,300,fp_config);
    sscanf(configLine, "%hd", &(test_shift_factor_for_3dnf_diff));
    Rfile_printf("%d \n",test_shift_factor_for_3dnf_diff);

    for(i = 0; i < 18; i++)
    {    
        Rfile_readLine(configLine,300,fp_config);
        sscanf(configLine, "%hd", &(test_vnf_default_thrconfig[i]));
        Rfile_printf("%d \n",test_vnf_default_thrconfig[i]);
    }

    for(i = 0; i < 6; i++)
    {    
        Rfile_readLine(configLine,300,fp_config);
        sscanf(configLine, "%hd", &(test_vnf_default_Half_thrconfig[i]));
        Rfile_printf("%d \n",test_vnf_default_Half_thrconfig[i]);
    }

    for(i = 0; i < 6; i++)
    {    
        Rfile_readLine(configLine,300,fp_config);
        sscanf(configLine, "%hd", &(test_vnf_default_Quad_thrconfig[i]));
        Rfile_printf("%d \n",test_vnf_default_Quad_thrconfig[i]);
    }

    for(i = 0; i < 32; i++)
    {    
        Rfile_readLine(configLine,300,fp_config);
        sscanf(configLine, "%hd", &(test_vnf_user_alpha_lookup[i]));
        Rfile_printf("%d \n",test_vnf_user_alpha_lookup[i]);
    }

    Rfile_close(fp_config);


#if 0
    /* Update the path of the second frame for the given test case */
    strcpy(fin_name, INPUT_PATH);
    if(testnum >= 0 && testnum < MAX_VNF_TEST_NUM)
        strcat(fin_name, VNF_TestStructure[testnum].input_fname);

    /* Update the path of the second frame output for the given test case */
    strcpy(fout_name, OUTPUT_PATH);
    if(testnum >= 0 && testnum < MAX_VNF_TEST_NUM)
        strcat(fout_name, VNF_TestStructure[testnum].output_fname);

    /* Update the path of the first frame for the given test case */
    strcpy(fprev_name, PREVIOUS_PATH);
    if(testnum >= 0 && testnum < MAX_VNF_TEST_NUM)
        strcat(fprev_name, VNF_TestStructure[testnum].previous_fname);

    strcpy(fprev_out_name, OUTPUT_PATH);
    if(testnum >= 0 && testnum < MAX_VNF_TEST_NUM)
        strcat(fprev_out_name, VNF_TestStructure[testnum].first_frm_fname);
#endif
    strcpy(fin_name, VNF_TestStructure[testnum].input_fname);
    strcpy(fout_name, VNF_TestStructure[testnum].output_fname);


    /* Update the pVnfQueryParam structure based on the test case parameter inputs */
    status = Update_TestParams_VNF1(pVnfQueryParam, testnum);
    MSP_VNF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_INVALIDCONFIG);

    /* Allocate the input buffer that would be used to load the first and subsequent frames */


    inBuffVnf.unNumOfCompBufs = (pVnfQueryParam->eInputFormat == MSP_VNF_YUV_FORMAT_YCBCR422) ? 1 : 2;
    inBuffVnf.nPortIndex      = 0;
    inBuffVnf.unBufSize[0]    = (MSP_U32)((pVnfQueryParam->ulInputStride) * (VNF_TestStructure[testnum].ulInputHeight));
    inBuffVnf.unBufStride[0]  = pVnfQueryParam->ulInputStride;

    /*For previous frame buffer header*/
    prevInBuffVnf.unNumOfCompBufs = 2;
    prevInBuffVnf.nPortIndex  = 0;

#ifdef MODE_3DNF_CHANGES        

    if(pVnfQueryParam->eOperateMode == MODE_3DNF || pVnfQueryParam->eOperateMode == MODE_TNF3 )
    {
        if(spatial_filter_enable_stagea)
        {
            inBuffVnf.unBufSize[0]    = (MSP_U32)((pVnfQueryParam->ulInputStride) * (VNF_TestStructure[testnum].ulInputHeight));
            inBuffVnf.unBufStride[0]  = pVnfQueryParam->ulInputStride;                
        }

    }

#endif


    if(inBuffVnf.unNumOfCompBufs == 2)
    {
        inBuffVnf.unBufSize[1]    = (MSP_U32)((pVnfQueryParam->ulInputStrideChroma) * ((VNF_TestStructure[testnum].ulInputHeight)/2));
        inBuffVnf.unBufStride[1]  = pVnfQueryParam->ulInputStrideChroma;


#ifdef MODE_3DNF_CHANGES        

        if(pVnfQueryParam->eOperateMode == MODE_3DNF || pVnfQueryParam->eOperateMode == MODE_TNF3)
        {
            if(spatial_filter_enable_stagea)
            {
                inBuffVnf.unBufSize[1]    = (MSP_U32)((pVnfQueryParam->ulInputStrideChroma) * ((VNF_TestStructure[testnum].ulInputHeight )/2));
                inBuffVnf.unBufStride[1]  = pVnfQueryParam->ulInputStrideChroma;

            }
        }

#endif    

        inBuffVnf.pBuf[0] = (MSP_U8 *)MSP_VNF_Malloc(inBuffVnf.unBufSize[0] + inBuffVnf.unBufSize[1], THIRTYTWO_BYTE_ALIGN);
        MSP_VNF_EXIT_IF(inBuffVnf.pBuf[0] == NULL, MSP_ERROR_NULLPTR);
        inBuffVnf.pBuf[1] = inBuffVnf.pBuf[0] + inBuffVnf.unBufSize[0];

        /* Initialize the size of the prior output frame corresponding to the first frame */

#ifdef MODE_3DNF_CHANGES        

        if(pVnfQueryParam->eOperateMode == MODE_3DNF || pVnfQueryParam->eOperateMode == MODE_TNF3)
        {
            if(spatial_filter_enable_stagea)
            {
                prevInBuffVnf.unBufSize[0]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideLuma /*+ 32*/ )* (pVnfQueryParam->ulComputeHeight + 32));

                prevInBuffVnf.unBufSize[1]    = (MSP_U32)((((pVnfQueryParam->ulOutputStrideChroma /*+32*/) * (pVnfQueryParam->ulComputeHeight + 32))/2));
            }
            else
            {
                prevInBuffVnf.unBufSize[0]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideLuma ) * (pVnfQueryParam->ulComputeHeight));
                prevInBuffVnf.unBufSize[1]    = (MSP_U32)(((pVnfQueryParam->ulOutputStrideChroma ) * (pVnfQueryParam->ulComputeHeight ))/2);
            }            
        }
        else if(pVnfQueryParam->eOperateMode != MODE_TNF)
#else
            if(pVnfQueryParam->eOperateMode != MODE_TNF)
#endif        
            {
                prevInBuffVnf.unBufSize[0]    = (MSP_U32)(pVnfQueryParam->ulOutputStrideLuma * (pVnfQueryParam->ulComputeHeight + 32));
                prevInBuffVnf.unBufSize[1]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideChroma * (pVnfQueryParam->ulComputeHeight + 32))/2);
            }
            else if(pVnfQueryParam->eOperateMode == MODE_TNF)
            {
                prevInBuffVnf.unBufSize[0]  = (MSP_U32)(pVnfQueryParam->ulOutputStrideLuma * (pVnfQueryParam->ulComputeHeight));
                prevInBuffVnf.unBufSize[1]  = (MSP_U32)((pVnfQueryParam->ulOutputStrideChroma * (pVnfQueryParam->ulComputeHeight))/2);        
            }

        if(spatial_filter_enable_stagea)
        {
            prevInBuffVnf.unBufStride[0]  = pVnfQueryParam->ulOutputStrideLuma ;
            prevInBuffVnf.unBufStride[1]  = pVnfQueryParam->ulOutputStrideChroma ;
        }
        else
        {
            prevInBuffVnf.unBufStride[0]  = pVnfQueryParam->ulOutputStrideLuma;
            prevInBuffVnf.unBufStride[1]  = pVnfQueryParam->ulOutputStrideChroma;           
        }


        /* Initialize the prior output frame address as NULL to indicate that it is a first frame */
        /* Within the process call it would be assigned a valid address and configures TNF motion threshold as zero to enable pass through functionality of TNF */
#ifdef MODE_3DNF_CHANGES
        if(pVnfQueryParam->eOperateMode == MODE_3DNF || pVnfQueryParam->eOperateMode == MODE_TNF3)
        {
            prevInBuffVnf.pBuf[0] = (MSP_U8 *)MSP_VNF_Malloc(prevInBuffVnf.unBufSize[0] + prevInBuffVnf.unBufSize[1], THIRTYTWO_BYTE_ALIGN);
            MSP_VNF_EXIT_IF(prevInBuffVnf.pBuf[0] == NULL, MSP_ERROR_NULLPTR);
            prevInBuffVnf.pBuf[1] = prevInBuffVnf.pBuf[0] + prevInBuffVnf.unBufSize[0];

            dummyptr = (MSP_U8 *)MSP_VNF_Malloc(14*(pVnfQueryParam->ulOutputStrideLuma /*+ 32*/), THIRTYTWO_BYTE_ALIGN);
        }
        else if(pVnfQueryParam->eOperateMode != MODE_TNF)
#else
            if(pVnfQueryParam->eOperateMode != MODE_TNF)
#endif        
            {    
                prevInBuffVnf.pBuf[0] = NULL;
                prevInBuffVnf.pBuf[1] = NULL;
            }
            else if(pVnfQueryParam->eOperateMode == MODE_TNF)
            {
                prevInBuffVnf.pBuf[0] = (MSP_U8 *)MSP_VNF_Malloc(prevInBuffVnf.unBufSize[0] + prevInBuffVnf.unBufSize[1], THIRTYTWO_BYTE_ALIGN);
                MSP_VNF_EXIT_IF(prevInBuffVnf.pBuf[0] == NULL, MSP_ERROR_NULLPTR);
                prevInBuffVnf.pBuf[1] = prevInBuffVnf.pBuf[0] + prevInBuffVnf.unBufSize[0];
            }


    }
    else
    {

        //inBuffVnf.pBuf[0] = (MSP_U8 *)MSP_VNF_Malloc_Frame(inBuffVnf.unBufSize[0], THIRTYTWO_BYTE_ALIGN);
        inBuffVnf.pBuf[0] = (MSP_U8 *)MSP_VNF_Malloc(inBuffVnf.unBufSize[0], THIRTYTWO_BYTE_ALIGN);
        MSP_VNF_EXIT_IF(inBuffVnf.pBuf[0] == NULL, MSP_ERROR_NULLPTR);        

        /* Initialize the size of the prior output frame corresponding to the first frame */
        prevInBuffVnf.unBufSize[0]    = (MSP_U32)(pVnfQueryParam->ulOutputStrideLuma * (pVnfQueryParam->ulComputeHeight + 32));
        prevInBuffVnf.unBufSize[1]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideChroma * (pVnfQueryParam->ulComputeHeight + 32))/2);
        prevInBuffVnf.unBufStride[0]  = pVnfQueryParam->ulOutputStrideLuma;
        prevInBuffVnf.unBufStride[1]  = pVnfQueryParam->ulOutputStrideChroma;

        /* Initialize the prior output frame address as NULL to indicate that it is a first frame */
        /* Within the process call it would be assigned a valid address and configures TNF motion threshold as zero to enable pass through functionality of TNF */
        prevInBuffVnf.pBuf[0] = NULL;
        prevInBuffVnf.pBuf[1] = NULL;

    }



    /* Allocate the output buffer for the previous frame or first frame */

    outBuffVnf.unNumOfCompBufs = 2;
    outBuffVnf.nPortIndex      = 1;

#ifdef MODE_3DNF_CHANGES    
    if(pVnfQueryParam->eOperateMode == MODE_3DNF || pVnfQueryParam->eOperateMode == MODE_TNF3)
    {      
        if(spatial_filter_enable_stagea)
        {
            outBuffVnf.unBufSize[0]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideLuma /*+ 32*/) * (pVnfQueryParam->ulComputeHeight + 32));
            outBuffVnf.unBufSize[1]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideChroma /*+ 32*/) * (pVnfQueryParam->ulComputeHeight + 32))/2;

        }
        else
        {
            outBuffVnf.unBufSize[0]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideLuma ) * (pVnfQueryParam->ulComputeHeight ));
            outBuffVnf.unBufSize[1]    = (MSP_U32)(((pVnfQueryParam->ulOutputStrideChroma ) * (pVnfQueryParam->ulComputeHeight))/2);
        }

    }
    else if(pVnfQueryParam->eOperateMode != MODE_TNF)
#else
    if(pVnfQueryParam->eOperateMode != MODE_TNF)
#endif    
    {    
        outBuffVnf.unBufSize[0]    = (MSP_U32)(pVnfQueryParam->ulOutputStrideLuma * (pVnfQueryParam->ulComputeHeight + 32));
        outBuffVnf.unBufSize[1]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideChroma * (pVnfQueryParam->ulComputeHeight + 32))/2);
    }
    else if(pVnfQueryParam->eOperateMode == MODE_TNF)
    {
        outBuffVnf.unBufSize[0]    = (MSP_U32)(pVnfQueryParam->ulOutputStrideLuma * (pVnfQueryParam->ulComputeHeight));
        outBuffVnf.unBufSize[1]    = (MSP_U32)((pVnfQueryParam->ulOutputStrideChroma * (pVnfQueryParam->ulComputeHeight))/2);    
    }

    if(spatial_filter_enable_stagea)
    {
        outBuffVnf.unBufStride[0]  = pVnfQueryParam->ulOutputStrideLuma ;
        outBuffVnf.unBufStride[1]  = pVnfQueryParam->ulOutputStrideChroma;

    }
    else
    {
        outBuffVnf.unBufStride[0]  = pVnfQueryParam->ulOutputStrideLuma;
        outBuffVnf.unBufStride[1]  = pVnfQueryParam->ulOutputStrideChroma;

    }

    //outBuffVnf.pBuf[0]         = (MSP_U8 *)MSP_VNF_Malloc_Frame(outBuffVnf.unBufSize[0] + outBuffVnf.unBufSize[1], THIRTYTWO_BYTE_ALIGN);
    outBuffVnf.pBuf[0]         = (MSP_U8 *)MSP_VNF_Malloc(outBuffVnf.unBufSize[0] + outBuffVnf.unBufSize[1], THIRTYTWO_BYTE_ALIGN);
    MSP_VNF_EXIT_IF(outBuffVnf.pBuf[0] == NULL, MSP_ERROR_NULLPTR);
    outBuffVnf.pBuf[1]         = (MSP_U8 *)(outBuffVnf.pBuf[0]) + outBuffVnf.unBufSize[0];

    {

        volatile Int32               retVal;
        Vps_PlatformDeviceInitParams vpsInitPrms;
        Iss_PlatformDeviceInitParams issInitPrms;
        Iss_PlatformInitParams       issPlatInitPrms;
        Vps_PlatformInitParams       vpsPlatInitPrms; 

        //vpsPlatInitPrms.isPinMuxSettingReq = TRUE;
        //retVal = Vps_platformInit(&vpsPlatInitPrms);
        //Rfile_printf("\Return value is %d at line: %5d : in file %22s, of function : %s \n",retVal,__LINE__, __FILE__,__FUNCTION__); 

        //issPlatInitPrms.isPinMuxSettingReq = TRUE;
        //retVal = Iss_platformInit(&issPlatInitPrms);
        //Rfile_printf("\Return value is %d at line: %5d : in file %22s, of function : %s \n",retVal,__LINE__, __FILE__,__FUNCTION__); 

        retVal = FVID2_init(NULL);
        Rfile_printf("\Return value is %d at line: %5d : in file %22s, of function : %s \n",retVal,__LINE__, __FILE__,__FUNCTION__); 


        retVal = Iss_init(NULL);
        Rfile_printf("\Return value is %d at line: %5d : in file %22s, of function : %s \n",retVal,__LINE__, __FILE__,__FUNCTION__); 

        //vpsInitPrms.isI2cInitReq = TRUE;
        //retVal = Vps_platformDeviceInit(&vpsInitPrms);
        //Rfile_printf("\Return value is %d at line: %5d : in file %22s, of function : %s \n",retVal,__LINE__, __FILE__,__FUNCTION__); 

        //issInitPrms.isI2cInitReq = TRUE;
        //retVal = Iss_platformDeviceInit(&issInitPrms);
        //Rfile_printf("\Return value is %d at line: %5d : in file %22s, of function : %s \n",retVal,__LINE__, __FILE__,__FUNCTION__); 

#ifdef _REMOTE_FILE_IO	  
        Utils_memInit();
        Utils_memClearOnAlloc(TRUE);
#endif
#if 0 	
	{
	      CPIS_Init vicpInit;
          CPIS_BaseParms base;
          CPIS_BaseParms baseRef;
          CPIS_ArrayOpParms params;
          CPIS_Handle handle;
	 unsigned int startTime, stopTime, totalTime;
	 unsigned char *ptr = (MSP_VNF_CREATE_PARAMS *)MSP_VNF_Malloc(sizeof(MSP_VNF_CREATE_PARAMS), DEFAULT_BYTE_ALIGN);
	 
	 ptr = (unsigned char *) ((unsigned int) ptr & 0xFFFFFFF0);
	 inputBuffer = ptr;
	 coeffBuffer = inputBuffer + WIDTH*HEIGHT;
	 outputBuffer = inputBuffer + WIDTH*HEIGHT*2;
	 
		  
		  
        base.srcBuf[0].ptr= (Uint8*)inputBuffer;
        base.srcBuf[1].ptr= (Uint8*)coeffBuffer;
        base.srcBuf[0].stride= WIDTH;
        base.srcBuf[1].stride= WIDTH;

        base.dstBuf[0].ptr= (Uint8*)outputBuffer;
        base.dstBuf[0].stride= WIDTH;

        base.roiSize.width= WIDTH;
        base.roiSize.height= HEIGHT;
        base.procBlockSize.width= 0;//BLOCK_WIDTH; // if 0, let function computes optimum BLOCK_WIDTH
        base.procBlockSize.height= 0;//BLOCK_HEIGHT; // if 0, let function computes optimum BLOCK_WIDTH
		
		base.srcFormat[0] = CPIS_U8BIT;
		base.srcFormat[1] = CPIS_U8BIT;
		base.dstFormat[0] = CPIS_8BIT;
		
	params.qShift = 0;
    params.operation = CPIS_OP_MPY;
    params.sat_high = 255;
    params.sat_high_set = 255;
    params.sat_low = 0;
    params.sat_low_set = 0;
	
	
	    startTime = timerReadStart();//Utils_getCurTimeInUsec();
       if (CPIS_arrayOp(
            &handle,
            &base,
            &params,
            CPIS_SYNC
            )== -1) {
            printf("\nCPIS_ArrayOp() error %d\n", CPIS_errno);
            exit(-1);
        };
 		stopTime  = timerReadEnd();//Utils_getCurTimeInUsec();   
		
		//printf("Total Time taken is %d microsec\n", (stopTime - startTime)/100);
		printf("Total M3 cycles taken is %d \n", (stopTime - startTime));
		printf("Number of points is %d\n ", WIDTH*HEIGHT);
		printf("Exec coproc cycles taken per point is %f \n", (float)(ratioVicpArm*(stopTime - startTime))/(8*WIDTH*HEIGHT));
		exit(0);  
		  
	}
#endif	
	
        Rfile_printf("Calling IP_Run_Init \n");   
        /* Initialize the IP_RUN by creating the task and semaphores */
        IP_Run_Init();


    }

	
	
	
	
#ifndef ZEBU_TESTING
    /* Reset the image buffers */
    ptr = (char *)(CSL_IMGBUF_A);
    MSP_VNF_Memset(ptr, 0x00, 0x4000);
    ptr = (char *)(CSL_IMGBUF_E);
    MSP_VNF_Memset(ptr, 0x00, 0x4000);
#endif
    while(vnf_ccs_wait);

    pVnf_appcbptr.MSP_callback = MSP_VNF_Callback;
    Rfile_printf("Calling MSP_init \n");   

    /* Initialize the MSP VNF component */
#ifdef __PROFILE_VNF__
    start    = Utils_getCurTimeInUsec();
    overhead = (Utils_getCurTimeInUsec() - start) - overhead;
    start    = Utils_getCurTimeInUsec();
#endif
    status = MSP_init(&hVnfMSP, "MSP.VNF", MSP_PROFILE_REMOTE, &pVnf_appcbptr);
    MSP_VNF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
#ifdef __PROFILE_VNF__
    CycleCountVnfMain = (Utils_getCurTimeInUsec() - start) - overhead;
    Rfile_printf("System Init time is %d \n",CycleCountVnfMain);
#endif

    /* Open the MSP VNF component */
	Rfile_printf("Calling MSP_open \n");   
#ifdef __PROFILE_VNF__
    start    = Utils_getCurTimeInUsec();
    overhead = (Utils_getCurTimeInUsec() - start) - overhead;
    start    = Utils_getCurTimeInUsec();
#endif
    status = MSP_VNF_open(hVnfMSP, (MSP_PTR *)pVnfQueryParam);
    MSP_VNF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);
#ifdef __PROFILE_VNF__
    CycleCountVnfMain = (Utils_getCurTimeInUsec() - start) - overhead;
    Rfile_printf("System open time is %d \n",CycleCountVnfMain);
#endif


    Rfile_printf("Opening Input File %s :)\n", fin_name);
    fp_inp  = Rfile_open(fin_name, "rb");
    Rfile_printf("Opening Output File %s :)\n", fout_name);
    fp_outp  = Rfile_open(fout_name, "wb");


    tempPrevPtr0 = prevInBuffVnf.pBuf[0];
    tempPrevPtr1 = prevInBuffVnf.pBuf[1];
    prevInBuffVnf.pBuf[0] = NULL;
    prevInBuffVnf.pBuf[1] = NULL;

    for(frameNum  = 0; frameNum < VNF_TestStructure[testnum].NumFrames; frameNum++)
    {
        Rfile_printf("Reading Input frames num %d \n", frameNum);
        for(i = 0; i < VNF_TestStructure[testnum].ulInputHeight; i++)
        {   //testing
            Rfile_read(srcBuf, sizeof(unsigned char), VNF_TestStructure[testnum].ulInputWidth, fp_inp);
            memcpy((void*)(inBuffVnf.pBuf[0] + ((i * (pVnfQueryParam->ulInputStride )))), srcBuf, VNF_TestStructure[testnum].ulInputWidth);
        }
        for(i = 0; i < VNF_TestStructure[testnum].ulInputHeight/2; i++)
        {   //testing 
            Rfile_read(srcBuf, sizeof(unsigned char), VNF_TestStructure[testnum].ulInputWidth, fp_inp);
            memcpy((void*)(inBuffVnf.pBuf[1] + ((i)*(pVnfQueryParam->ulInputStrideChroma))), srcBuf, VNF_TestStructure[testnum].ulInputWidth);       
        }



#ifdef __PROFILE_VNF__
        start    = Utils_getCurTimeInUsec();
#endif

        status = MSP_process(hVnfMSP, &prevInBuffVnf, &inBuffVnf);
        MSP_VNF_EXIT_IF(status != MSP_ERROR_NONE, MSP_ERROR_FAIL);

#ifdef __PROFILE_VNF__
        CycleCountVnfMain = (Utils_getCurTimeInUsec() - start) - overhead;
        Rfile_printf("First frame InBuff process time in cycles is %d \n",CycleCountVnfMain);
#endif

#ifdef __PROFILE_VNF__
        start    = Utils_getCurTimeInUsec();
#endif
        status = MSP_process(hVnfMSP, NULL, &outBuffVnf);
        MSP_VNF_EXIT_IF(status != MSP_ERROR_NONE,MSP_ERROR_FAIL);

        /* Wait for the task thread to complete its job of performing LDC + VNF on previous frame or first frame */
        Semaphore_pend(gSemHandleVnf, BIOS_WAIT_FOREVER);

#ifdef __PROFILE_VNF__
        CycleCountVnfMain = (Utils_getCurTimeInUsec() - start) - overhead;
        Rfile_printf("frame execution time in cycles in cycles is %d \n",CycleCountVnfMain);
#endif

        Rfile_printf("Completed temporal noise filtering & Writing to file\n");

        /* Write luma component of first frame output to file */
        if(spatial_filter_enable_stagea)
        {

            offset = 32;
        }
        else
        {

            offset =0;
        }

        for(i = 0; i < VNF_TestStructure[testnum].ulComputeHeight+offset ; i++)
        {

            memcpy(dstBuf, (void*)(outBuffVnf.pBuf[0] + (i * (pVnfQueryParam->ulOutputStrideLuma /*+ offset*/ ))), (MSP_U32)(pVnfQueryParam->ulComputeWidth  +offset ));
            Rfile_write(dstBuf, sizeof(MSP_U8), (MSP_U32)(pVnfQueryParam->ulComputeWidth  +offset ), fp_outp);
        }     

        /* Write chroma component of first frame output to file */
        for(i = 0; i < ((VNF_TestStructure[testnum].ulComputeHeight + offset )/2); i++)
        {
            memcpy(dstBuf, (void*)(outBuffVnf.pBuf[1] + (i * (pVnfQueryParam->ulOutputStrideChroma /*+ offset*/ ))), (MSP_U32)(pVnfQueryParam->ulComputeWidth  +offset ));
            Rfile_write(dstBuf, sizeof(MSP_U8), (MSP_U32)(pVnfQueryParam->ulComputeWidth  +offset ), fp_outp);
        }  


        if(frameNum == 0)
        {
            prevInBuffVnf.pBuf[0] = tempPrevPtr0;
            prevInBuffVnf.pBuf[1] = tempPrevPtr1;    
        }


        tempPrevPtr0          = prevInBuffVnf.pBuf[0];
        tempPrevPtr1          = prevInBuffVnf.pBuf[1]; 
        prevInBuffVnf.pBuf[0] = outBuffVnf.pBuf[0];
        prevInBuffVnf.pBuf[1] = outBuffVnf.pBuf[1];  
        outBuffVnf.pBuf[0]    = tempPrevPtr0;
        outBuffVnf.pBuf[1]    = tempPrevPtr1;  
    }
    Rfile_close(fp_inp);
    Rfile_close(fp_outp);

#ifdef _REMOTE_FILE_IO	
    Rfile_free(srcBuf, BUF_SIZE);
    Rfile_free(dstBuf, BUF_SIZE);
#else
    Rfile_free(srcBuf);
    Rfile_free(dstBuf);
#endif   
    /* Close the MSP VNF component */
    MSP_VNF_EXIT_IF((MSP_close(hVnfMSP))!=MSP_ERROR_NONE,MSP_ERROR_FAIL);

    /* De-initialize the MSP VNF component */
    MSP_VNF_EXIT_IF((MSP_deInit(hVnfMSP))!=MSP_ERROR_NONE,MSP_ERROR_FAIL);

    /* De-initialize the resource manager */
    RM_SIMCOP_DeInit();

    /* De-initialize the IP Run */
    IP_Run_Deinit();

    MSP_VNF_Free(pVnfQueryParam->ptLdcParams->ptLensDistortionParams->ptLutParams);
    MSP_VNF_Free(pVnfQueryParam->ptLdcParams->ptLensDistortionParams);
    MSP_VNF_Free(pVnfQueryParam->ptLdcParams->ptAffineParams);
    MSP_VNF_Free(pVnfQueryParam->ptLdcParams);
    MSP_VNF_Free(pVnfQueryParam->ptNsfParams);
    MSP_VNF_Free(pVnfQueryParam->ptTnfParams);
    MSP_VNF_Free(pVnfQueryParam);

    if(pVnfQueryParam->eOperateMode != MODE_TNF 
#ifdef MODE_3DNF_CHANGES
            && pVnfQueryParam->eOperateMode != MODE_3DNF
			&&  pVnfQueryParam->eOperateMode != MODE_TNF3
#endif    
    )
    {
        MSP_VNF_Free(currOutBuffVnf.pBuf[0]);
    }

    MSP_VNF_Free(outBuffVnf.pBuf[0]);
    MSP_VNF_Free(inBuffVnf.pBuf[0]);
    if(pVnfQueryParam->eOperateMode == MODE_TNF)
    {
        MSP_VNF_Free(prevInBuffVnf.pBuf[0]);        
    }

    Rfile_printf("\n VNF test app finished ... Exiting ... \n");

#ifdef VNF_MEMORY_LEAK
    /* Get memory status after running the test case */
    mem_count_end = TIMM_OSAL_GetMemCounter();
    mem_size_end  = TIMM_OSAL_GetMemUsage();
    Rfile_printf("--------At the end of test--------");
    Rfile_printf("Value from GetMemCounter = %d \n", mem_count_end);
    Rfile_printf("Value from GetMemUsage   = %d \n", mem_size_end);

    if(mem_count_start != mem_count_end)
    {
        Rfile_printf("Memory leak detected. Bytes lost = %d \n", (mem_size_end - mem_size_start));
    }
    else
    {
        Rfile_printf("No memory leaks detected \n");
    }
    Rfile_printf(" ----------------------------------------------- ");
    Rfile_printf("\n");
#endif
    BIOS_exit (0);
    return;

    EXIT:
    Rfile_printf("Something went wrong!! \n");
    return;
}
#ifndef _REMOTE_FILE_IO
/* Create test task */
Void VNF_createTsk (  )
{
    Task_Params tskParams;
    Task_Handle tskMain;


    /*
     * Create test task
     */
    Task_Params_init ( &tskParams );

    tskParams.priority = 10;
    tskParams.stack = gCaptureApp_tskStackMain;
    tskParams.stackSize = sizeof ( gCaptureApp_tskStackMain );

    tskMain = Task_create ( vnf_main, &tskParams, NULL );

    //GT_assert( GT_DEFAULT_MASK, tskMain != NULL );

    /* Register the task to the load module for calculating the load */
    //IssUtils_prfLoadRegister(tskMain, "VNFAPP:");
}

/*
  Application main
 */
Int32 main ( void )
{
    timerInit();
    printf("\n----------------------------------------\n");
    printf("ARM9: %d Mhz, VICP: %d Mhz\n", armMhz, vicpMhz);
    printf("\n----------------------------------------\n");
	Utils_setCpuFrequency(armMhz * 1000000);
    /*
     * Create test task
     */
    VNF_createTsk (  );

    /*
     * Start BIOS
     */
    BIOS_start (  );

    return (0);
}
#endif


Ptr Utils_memAlloc(UInt32 size, UInt32 align)
{
   return(MSP_MallocExtn(size, TIMM_OSAL_TRUE, align, 0, NULL));
}

Int32 Utils_memFree(Ptr addr, UInt32 size)
{
   MSP_Free(addr);
   return 0;
}


