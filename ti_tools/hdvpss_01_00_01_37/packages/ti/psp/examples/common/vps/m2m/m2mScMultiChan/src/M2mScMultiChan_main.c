/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file M2mScMultiChan_main.c
 *
 * \brief   Multi-channel Scalar application demonstrating scalar for
 *          memory to memory driver for all available scalar paths.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mSc.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/examples/utility/vpsutils.h>
#include <ti/psp/examples/utility/vpsutils_tiler.h>
#include <ti/psp/examples/utility/vpsutils_prf.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* Output width for each successive channel for the multichannel will decrease
 * by this value for the upscaling case.  While for the down scaling case
 * it will decrease by OUTPUT_WIDTH_CHANGE/10.  So for upscaling from
 * 720X480 --> 1920X1080 first channel width will be 1080, second will be
 * 980 and so on. Pitch will remain same for all the cases.
 */
#define OUTPUT_WIDTH_CHANGE             (100u)
#define IN_NUM_FRAMELIST                (1u)
#define OUT_NUM_FRAMELIST               (1u)
#define MAX_IN_BUFFERS                  (1)
#define MAX_OUT_BUFFERS                 (VPS_M2M_SC_MAX_CH*MAX_IN_BUFFERS)


/* In case of YUV 420 semiplanar data we will have two planes one for Y
 * an another for CB and CR
 */
#define MAX_PLANES              2u

/* Test application stack size. */
#define APP_TSK_STACK_SIZE              (200u * 1024u)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* Strucutre is used to configure the driver for different configurations */
typedef struct
{
    Char    testDesc[200];
    UInt32  driverId;
    UInt32  cfgId;
    UInt32  loopCount;
    UInt32  chPerHndl;
    UInt32  inputWidth;
    UInt32  inputHeight;
    UInt32  outputWidth;
    UInt32  outputHeight;
    UInt32  inputDataFmt;
    UInt32  outputDataFmt;
    UInt32  vsType;
    UInt32  cropStartX;
    UInt32  cropStartY;
    UInt32  cropWidth;
    UInt32  cropHeight;
    UInt32  nonlinearScaling;
    UInt32  stripsize;
    UInt32  scBypass;
    UInt32  fps;
    UInt32  subFrameProcEn;
    UInt32  NumSubFrames;
}AppM2mScMCh_Cfg;

/**
 *  \brief SC M2M application object.
 */
typedef struct
{
    FVID2_Handle            fvid2Handle;
    /**< Mem2Mem Driver handle */
    AppM2mScMCh_Cfg        *curCfg;
    /**< Pointer to the currently selected configuration */
    FVID2_ProcessList       processList;
    /**< Process list for submitting request */
    FVID2_FrameList         inFrameList;
    /**< Input Framelist */
    FVID2_FrameList         outFrameList;
    /**< Output Framelist */
    FVID2_Frame             inFrames[VPS_M2M_SC_MAX_CH];
    /**< In FrameBuffer Pointers */
    FVID2_Frame             outFrames[VPS_M2M_SC_MAX_CH];
    /**< Out FrameBuffer Pointers */
    UInt8                  *inputBuffer[MAX_IN_BUFFERS][MAX_PLANES];
    /**< Pointers to input buffers */
    UInt8                  *outputBuffer[MAX_OUT_BUFFERS][MAX_PLANES];
    /**< Pointers to output buffers */
    UInt32                  inputBufferSize;
    /**< Input buffer size */
    UInt32                  outputBufferSize;
    /**< Output buffer size */
    FVID2_ProcessList       errProcessList;
    /**< Error ProcessList */
    Semaphore_Handle        reqProcessed;
    /**< Stream complete semaphore used to sync between callback and task */
    Semaphore_Handle        tskComplete;
    /**< Task complete semaphore */
    Vps_M2mScCreateParams   createParams;
    /**< Per handle parameters for each driver */
    Vps_M2mScChParams       chParams[VPS_M2M_SC_MAX_CH];
    /**< Configuration parameters for each channel */
    Vps_ScCoeffParams       coeffParams;
    /**< Parameters for setting coefficients */
    Vps_ScConfig            chScCfg[VPS_M2M_SC_MAX_CH];
    /**< Instance of the channel scalar config */
    Vps_CropConfig          chSrcCropCfg[VPS_M2M_SC_MAX_CH];
    /**< Instance of the crop config */
    Vps_SubFrameParams      subFrameParams[VPS_M2M_SC_MAX_CH];
    /**< Sub-frame level processing params */
    FVID2_SubFrameInfo      subFrameInfo[VPS_M2M_SC_MAX_CH];
    /**< Sub-frame level processing info */
    VpsUtils_PrfTsHndl     *prfTsHandle;
    /**< Handle to get the Timestamp */
    UInt32                  inputBufIndex;
    /**< Buffer index to be updated for each request */
    UInt32                  outputBufIndex;
    /**< Buffer index to be updated for each channel */

} AppM2mScMCh_Obj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Void vpsScMChTestTask(void);
void vpsScMChSetConfigParams();
static Int32 vpsScMChTest(UInt32 driverInst);
static Int32 vpsScMChCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
static Int32 vpsScMChErrCbFxn(FVID2_Handle handle,
                            Ptr appData,
                            Void *errList,
                            Ptr reserved);
static Void vpsScMChInitVariables(void);
static Void vpsScMChFreeInOutBufs();
static Int32 vpsScMChAllocOutBufs();
static Int32 vpsScMChAllocInBufs();
static Int32 vpsScMChRunTestCases(UInt32 cfgCnt);
static Int32 vpsScMChGetTstId();
static UInt32 get16ByteAlignedPitch(UInt32 width, UInt32  dataFmt);
static Int32 vpsScMchUpdateBuffers();


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/*  M2M application object. */
static AppM2mScMCh_Obj AppM2mScMChObj;

/* String to print the instance */
static char *AppM2mScInstName[] =
{
    "VpsM2mInstSec0Sc5Wb2",  /* VPS_M2M_INST_SEC0_SC5_WB2 */
    "VpsM2mInstBp0Sc5Wb2",   /* VPS_M2M_INST_BP0_SC5_WB2 */
    "VpsM2mInstBp1Sc5Wb2",   /* VPS_M2M_INST_BP1_SC5_WB2 */
    "VpsM2mInstSec0Sc3Vip0", /* VPS_M2M_INST_SEC0_SC3_VIP0 */
    "VpsM2mInstSec1Sc4Vip1"  /* VPS_M2M_INST_SEC1_SC4_VIP1 */
};

/* String to print the format */
static char *AppM2mScFmtName[] =
{
    "FormatNotSupported", /* FVID2_DF_YUV422I_UYVY */
    "yuyv422",            /* FVID2_DF_YUV422I_YUYV */
    "FormatNotSupported", /* FVID2_DF_YUV422I_YVYU */
    "FormatNotSupported", /* FVID2_DF_YUV422I_VYUY */
    "nv16",               /* FVID2_DF_YUV422SP_UV */
    "FormatNotSupported", /* FVID2_DF_YUV422SP_VU */
    "FormatNotSupported", /* FVID2_DF_YUV422P */
    "nv12"                /* FVID2_DF_YUV420SP_UV */
};


/* Configuration for various test cases */
AppM2mScMCh_Cfg AppM2mScMchCfg[] =
{
    /* 1 channel YUV420 SD ---> YUV420 HD;  */
    {
        "1  CH YUV420 SD ---> YUV420 HD, Driver: VPS_M2M_INST_SEC1_SC4_VIP1",
        VPS_M2M_INST_SEC1_SC4_VIP1, 0,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV420SP_UV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 SD ---> YUV422 HD */
    {
        "1  CH YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC0_SC5_WB2",
        VPS_M2M_INST_SEC0_SC5_WB2, 1,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV422SP SD ---> YUV422I SD;  */
    {
        "1  CH YUV422SP SD ---> YUV422I SD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 3,
        1600, 1, 720, 480, 720, 480, FVID2_DF_YUV422SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV422 SD ---> YUV422I HD BP1*/
    {
        "1  CH YUV422I SD ---> YUV422I HD, Driver: VPS_M2M_INST_BP1_SC5_WB2",
        VPS_M2M_INST_BP1_SC5_WB2, 6,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV422I_YUYV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },

    /* 1 channel YUV420 SD ---> YUV422I HD */
    {
        "1  CH YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC1_SC4_VIP1",
        VPS_M2M_INST_SEC1_SC4_VIP1, 7,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 SD ---> YUV422I HD */
    {
        "1  CH YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 8,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 8 channel YUV420 SD ---> YUV420 HD */
    {
        "8  CH YUV420 SD ---> YUV420 HD, Driver: VPS_M2M_INST_SEC1_SC4_VIP1",
        VPS_M2M_INST_SEC1_SC4_VIP1, 9,
        200, 8, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV420SP_UV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 8 channel YUV420 SD ---> YUV422I HD */
    {
        "8  CH YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC1_SC4_VIP1",
        VPS_M2M_INST_SEC1_SC4_VIP1, 10,
        200, 8, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 8 channel YUV420 SD ---> YUV420 HD;  */
    {
        "8  CH YUV420 SD ---> YUV420 HD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 11,
        200, 8, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV420SP_UV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 8 channel YUV420 SD ---> YUV422I */
    {
        "8  CH YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 12,
        200, 8, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 8 channel YUV420 SD ---> YUV422I HD */
    {
        "8  CH YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC0_SC5_WB2",
        VPS_M2M_INST_SEC0_SC5_WB2, 13,
        200, 8, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 16 channel YUV420 SD ---> YUV422I HD */
    {
        "16 CH YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC0_SC5_WB2",
        VPS_M2M_INST_SEC0_SC5_WB2, 14,
        100, 16, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },

    /* 1 channel YUV420 HD ---> YUV422I SD 10000 frames. */
    {
        "1  CH YUV420 HD ---> YUV422I SD, Driver: VPS_M2M_INST_SEC0_SC5_WB2",
        VPS_M2M_INST_SEC0_SC5_WB2, 15,
        1600, 1, 1920, 1080, 720, 480, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 1920, 1080, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 4 channel YUV420 HD ---> YUV422I SD 10000 frames. */
    {
        "4  CH YUV420 HD ---> YUV422I SD, Driver: VPS_M2M_INST_SEC1_SC4_VIP1",
        VPS_M2M_INST_SEC1_SC4_VIP1, 16,
        400, 4, 1920, 1080, 720, 480, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 8 channel YUV420 HD ---> YUV422I SD 10000 frames. */
    {
        "8  CH YUV420 HD ---> YUV422I SD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 17,
        200, 8, 1920, 1080, 720, 480, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 16 channel YUV420 HD ---> YUV422I SD 10000 frames. */
    {
        "16 CH YUV420 HD ---> YUV422I SD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
         VPS_M2M_INST_SEC0_SC3_VIP0, 18,
        100, 16, 1920, 1080, 720, 480, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 VGA ---> YUV422I HD Non Linear Scaling*/
    {
        "1  CH YUV420 SD->VGA(Crop) ---> YUV422I HD Non Linear 4:3 ---> 16:9, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
          VPS_M2M_INST_SEC0_SC3_VIP0, 19,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 640, 480, TRUE, 420, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 SD ---> YUV422I 360X240 Running Average Vertical Filter*/
    {
        "1 CH YUV420 SD ---> YUV422I 360X240 Running Average Vertical Filter, Driver : VPS_M2M_INST_SEC0_SC3_VIP0",
     VPS_M2M_INST_SEC0_SC3_VIP0, 20,
        1600, 1, 720, 480, 360, 240, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_RAV, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 HD ---> YUV422I HD with scalar in bypass mode. */
    {
        "1  CH YUV420 SD ---> YUV422I SD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
         VPS_M2M_INST_SEC0_SC3_VIP0, 21,
        1600, 1, 720, 480, 720, 480, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, TRUE, 0, FALSE, 1
    },
    /* 1 channel YUV420 SD ---> YUV422 128X80 */
    {
        "1  CH YUV420 SD ---> YUV422I 128X80, Driver: VPS_M2M_INST_SEC0_SC5_WB2",
        VPS_M2M_INST_SEC0_SC5_WB2, 1,
        1600, 1, 720, 480, 128, 80, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 SD ---> YUV422 80X48 */
    {
        "1  CH YUV420 SD ---> YUV422I 80X48, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 1,
        1600, 1, 720, 480, 80, 48, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 SD ---> YUV422 48X32 */
    {
        "1  CH YUV420 SD ---> YUV422I 48X32, Driver: VPS_M2M_INST_SEC1_SC4_VIP1",
        VPS_M2M_INST_SEC1_SC4_VIP1, 1,
        1600, 1, 720, 480, 48, 32, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 34X18 ---> YUV422I 176X144;  */
    {
        "1  CH YUV420 34X18 ---> YUV422I 176X144, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 2,
        1600, 1, 34, 18, 176, 144, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 34, 18, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /* 1 channel YUV420 34X18 ---> YUV420 176X144;;  */
    {
        "1  CH YUV420 34X18 ---> YUV420 176X144, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 4,
        1600, 1, 34, 18, 176, 144, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV420SP_UV,
        VPS_SC_VST_POLYPHASE, 0, 0, 34, 18, FALSE, 0, FALSE, 0, FALSE, 1
    },
    /**** Below five entries are for SubFrame based processign verification ****/
#if 0 /*sub-frame processing not yet suported in SC3/4, this code can be enabled after supporting.*/
    /* 1 channel YUV420 SD ---> YUV420;  */
    {
        "1 CH SUB-FRAME PROCESSING YUV420 SD ---> YUV420 HD, Driver: VPS_M2M_INST_SEC0_SC3_VIP0",
        VPS_M2M_INST_SEC0_SC3_VIP0, 22,
        2, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV420SP_UV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0,
        TRUE, 4
    },

    /* 1 channel YUV420 SD ---> YUV420;  */
    {
        "1 CH SUB-FRAME PROCESSING YUV420 SD ---> YUV420 HD, Driver: VPS_M2M_INST_SEC1_SC4_VIP1",
        VPS_M2M_INST_SEC1_SC4_VIP1, 23,
        2, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV420SP_UV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0,
        TRUE, 4
    },
#endif
    /* 1 channel YUV420 SD ---> YUV422I HD sub-frame */
    {
        "1 CH SUB-FRAME PROCESSING YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC0_SC5_WB2",
        VPS_M2M_INST_SEC0_SC5_WB2, 24,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0,
        TRUE, 4
    },
    /* 1 channel YUV422I SD ---> YUV422I BP0 sub-frame*/
    {
        "1 CH SUB-FRAME PROCESSING YUV422I SD ---> YUV422I HD, Driver: VPS_M2M_INST_BP0_SC5_WB2",
        VPS_M2M_INST_BP0_SC5_WB2, 25,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV422I_YUYV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0,
        TRUE, 4
    },
    /* 1 channel YUV422 SD ---> YUV422 BP1 sub-frame */
    {
        "1 CH SUB-FRAME PROCESSING YUV422I SD ---> YUV422I HD, Driver: VPS_M2M_INST_BP1_SC5_WB2",
        VPS_M2M_INST_BP1_SC5_WB2, 26,
        1600, 1, 720, 480, 1920, 1080, FVID2_DF_YUV422I_YUYV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0,
        TRUE, 4
    },
    /* 1 channel YUV420 SD ---> YUV422I HD sub-frame */
    {
        "1 CH SUB-FRAME PROCESSING YUV420 SD ---> YUV422I HD, Driver: VPS_M2M_INST_SEC0_SC5_WB2",
        VPS_M2M_INST_SEC0_SC5_WB2, 27,
        200, 1, 720, 480, 320, 180, FVID2_DF_YUV420SP_UV,
        FVID2_DF_YUV422I_YUYV,
        VPS_SC_VST_POLYPHASE, 0, 0, 720, 480, FALSE, 0, FALSE, 0,
        TRUE, 4
    },
};

/* Test application stack. */
#pragma DATA_ALIGN(App_mainTskStack, 32)
#pragma DATA_SECTION(App_mainTskStack, ".bss:taskStackSection")
static UInt8 App_mainTskStack[APP_TSK_STACK_SIZE];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Application main function.
 */
Int32 main(void)
{
    Task_Params             tskParams;
    Semaphore_Params        semParams;
    Int32                   retVal = FVID2_SOK;
    Task_Handle             tskHandle;

    Semaphore_Params_init(&semParams);
    AppM2mScMChObj.tskComplete = Semaphore_create(0, &semParams, NULL);
    if (NULL == AppM2mScMChObj.tskComplete)
    {
        Vps_printf("M2mScMultiChan:%s: Error %d @ line %d\n",
            __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EFAIL;
    }
    if (FVID2_SOK == retVal)
    {
        VpsUtils_prfInit();
        AppM2mScMChObj.prfTsHandle = VpsUtils_prfTsCreate("M2mScMultiChan");
        if (NULL == AppM2mScMChObj.prfTsHandle)
        {
            retVal = FVID2_EFAIL;
            Vps_printf("M2mScMultiChan:%s: Error %d @ line %d\n",
                __FUNCTION__, retVal, __LINE__);
        }
        Task_Params_init(&tskParams);
        tskParams.stack = App_mainTskStack;
        tskParams.stackSize = APP_TSK_STACK_SIZE;
        /* Start the Testing Task */
        tskHandle = Task_create((Task_FuncPtr) vpsScMChTestTask,
                        &tskParams,
                        NULL);
        if (NULL == tskHandle)
        {
            Vps_printf("M2mScMultiChan:%s: Error %d @ line %d\n",
                __FUNCTION__, retVal, __LINE__);
            retVal = FVID2_EFAIL;
        }
        VpsUtils_prfLoadRegister(tskHandle, "M2mScMultiChan:");
        BIOS_start();
    }
    if (FVID2_SOK == retVal)
    {
        /* Wait for task to complete */
        Semaphore_pend(AppM2mScMChObj.tskComplete, BIOS_WAIT_FOREVER);
        VpsUtils_prfTsDelete(AppM2mScMChObj.prfTsHandle);
        Semaphore_delete(&AppM2mScMChObj.tskComplete);
        VpsUtils_prfLoadUnRegister(tskHandle);
        VpsUtils_prfDeInit();
        Task_delete(&tskHandle);
    }

    return (retVal);
}

/**
 *  Application test task.
 */
static Void vpsScMChTestTask(void)
{
    const Char  *versionStr;
    Int32       retVal;
    Int32       tstCnt, testId;
    Vps_PlatformInitParams       platformInitPrms;

    /* Initialize the platform */
    platformInitPrms.isPinMuxSettingReq = TRUE;
    retVal = Vps_platformInit(&platformInitPrms);
    /* Init memory allocator */
    VpsUtils_memInit();

    /*
     * Get the version string
     */
    versionStr = FVID2_getVersionString();
    Vps_printf("HDVPSS Drivers Version: %s\n", versionStr);
    /* FVID2 Init */
    retVal = FVID2_init(NULL);

    /* Run the test cases */
    do {
        /* Get the TestId */
        testId = vpsScMChGetTstId();
        if ((testId >= 0) &&
            (testId < sizeof(AppM2mScMchCfg)/sizeof(AppM2mScMCh_Cfg)))
        {
            retVal = vpsScMChRunTestCases(testId);
        }
        else if (testId == 100)
        {
            break;
        }
        else if (testId == sizeof(AppM2mScMchCfg)/sizeof(AppM2mScMCh_Cfg))
        {
            for (tstCnt = 0;
                 tstCnt < sizeof(AppM2mScMchCfg)/sizeof(AppM2mScMCh_Cfg);
                 tstCnt++)
            {
                retVal = vpsScMChRunTestCases(tstCnt);
                /* Manual tests will always run with 0th configuration which is
                 * user selected */
            }
        }
        else
        {
            Vps_printf("M2mScMultiChan:Invalid TestID Enter Again\n");
        }
    }while (testId != 100);

    retVal = FVID2_deInit(NULL);
    if (FVID2_SOK != retVal)
    {
        Vps_printf("M2mScMultiChan:%s: Error %d @ line %d\n",
            __FUNCTION__, retVal, __LINE__);
        return;
    }
     /* De-Init memory allocator */
    VpsUtils_memDeInit();
    /* De-Initialize the platform */
    Vps_platformDeInit();

    Vps_printf("M2mScMultiChan:Test Successfull!!\n");
    Semaphore_post(AppM2mScMChObj.tskComplete);
}

static Int32 vpsScMChRunTestCases(UInt32 cfgCnt)
{
    char    ch;
    Int32   retVal = 0;
    UInt32  outputPitch, outputWidth;

    AppM2mScMChObj.curCfg = &AppM2mScMchCfg[cfgCnt];
    /* Loop count is always 1 for simulator as it's very slow */
    if (Vps_platformIsSim())
    {
        AppM2mScMChObj.curCfg->loopCount = 1;
    }
#ifdef PLATFORM_ZEBU
    AppM2mScMChObj.curCfg->loopCount = 1;
#endif /* PLATFORM_ZEBU */

    retVal = vpsScMChAllocInBufs();
    retVal += vpsScMChAllocOutBufs();
    if (retVal != FVID2_SOK)
    {
        Vps_printf(
            "M2mScMultiChan:Allocation of Buffers Failed...\n");
        retVal = FVID2_EALLOC;
        return retVal;
    }
    Vps_printf("Enter any key after loading...\n");
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    retVal = vpsScMChTest(AppM2mScMChObj.curCfg->driverId);

    if (FVID2_SOK != retVal)
    {
        Vps_printf("M2mScMultiChan:%s: Error %d @ line %d\n",
            __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EALLOC;
        return retVal;
    }
    else
    {
        Vps_printf("M2mScMultiChan:Frames completed processing successfully\n");
    }

    Vps_printf("M2mScMultiChan:Output Buffers Start Address 0x%p\n",
               AppM2mScMChObj.outputBuffer[0][0]);
    outputPitch = get16ByteAlignedPitch(AppM2mScMChObj.curCfg->outputWidth,
                        AppM2mScMChObj.curCfg->outputDataFmt);
    switch (AppM2mScMChObj.curCfg->outputDataFmt)
    {
        case FVID2_DF_YUV420SP_UV:
        case FVID2_DF_YUV422SP_UV:
        {
            outputWidth = outputPitch;
            break;
        }
        case FVID2_DF_YUV422I_YUYV:
            outputWidth = outputPitch >> 1;
            break;
        default:
            outputWidth = outputPitch;
    }

    Vps_printf(
        "M2mScMultiChan:Save output file with command:saveRaw(0, 0x%.8x, "
        "\"C:\\\\sc%sCh%dWbOut_%s_prog_packed_%d_%d.tigf\", "
        "%d, 32, true);\n",
        AppM2mScMChObj.outputBuffer[0][0],
        AppM2mScInstName[AppM2mScMChObj.curCfg->driverId],
        AppM2mScMChObj.curCfg->chPerHndl,
        AppM2mScFmtName[AppM2mScMChObj.curCfg->outputDataFmt],
        outputWidth,
        AppM2mScMChObj.curCfg->outputHeight,
        (AppM2mScMChObj.outputBufferSize * AppM2mScMChObj.curCfg->chPerHndl *
         MAX_IN_BUFFERS)/4);

    Vps_printf("Enter any key after saving the output file ...\n");
    VpsUtils_getChar(&ch, BIOS_WAIT_FOREVER);

    /* Free the input and output buffers */
    vpsScMChFreeInOutBufs();

    return retVal;
}

void vpsScMChSetConfigParams()
{
    UInt32 configCnt, inputPitch, outputPitch;
    /* Configuration per each channel */
    AppM2mScMChObj.createParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    /* Number of channels per handle */
    AppM2mScMChObj.createParams.numChannels = AppM2mScMChObj.curCfg->chPerHndl;

    AppM2mScMChObj.chParams[0].inFmt.channelNum = 0;
    AppM2mScMChObj.chParams[0].inFmt.width = AppM2mScMChObj.curCfg->inputWidth;
    AppM2mScMChObj.chParams[0].inFmt.height =
        AppM2mScMChObj.curCfg->inputHeight;
    inputPitch = get16ByteAlignedPitch(AppM2mScMChObj.chParams[0].inFmt.width,
                        AppM2mScMChObj.curCfg->inputDataFmt);

    if (FVID2_DF_YUV420SP_UV == AppM2mScMChObj.curCfg->inputDataFmt ||
            (FVID2_DF_YUV422SP_UV == AppM2mScMChObj.curCfg->inputDataFmt))
    {
        AppM2mScMChObj.chParams[0].inFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] =
            inputPitch;
        AppM2mScMChObj.chParams[0].inFmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] =
            inputPitch;
    }
    else
    {
        AppM2mScMChObj.chParams[0].inFmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
            inputPitch;
    }

    AppM2mScMChObj.chParams[0].inFmt.fieldMerged[FVID2_YUV_SP_Y_ADDR_IDX] =
        FALSE;
    AppM2mScMChObj.chParams[0].inFmt.fieldMerged[FVID2_YUV_SP_CBCR_ADDR_IDX] =
        FALSE;
    AppM2mScMChObj.chParams[0].inFmt.dataFormat =
        AppM2mScMChObj.curCfg->inputDataFmt;
    AppM2mScMChObj.chParams[0].inFmt.scanFormat = FVID2_SF_PROGRESSIVE;

    AppM2mScMChObj.chParams[0].outFmt.channelNum = 0;
    AppM2mScMChObj.chParams[0].outFmt.width =
        AppM2mScMChObj.curCfg->outputWidth;
    AppM2mScMChObj.chParams[0].outFmt.height =
        AppM2mScMChObj.curCfg->outputHeight;
    outputPitch = get16ByteAlignedPitch(AppM2mScMChObj.chParams[0].outFmt.width,
                        AppM2mScMChObj.curCfg->outputDataFmt);
    if (FVID2_DF_YUV420SP_UV == AppM2mScMChObj.curCfg->outputDataFmt ||
            (FVID2_DF_YUV422SP_UV == AppM2mScMChObj.curCfg->outputDataFmt))
    {
        AppM2mScMChObj.chParams[0].outFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] =
            outputPitch;
        AppM2mScMChObj.chParams[0].outFmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] =
            outputPitch;
    }
    else
    {
        AppM2mScMChObj.chParams[0].outFmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
            outputPitch;
    }

    AppM2mScMChObj.chParams[0].outFmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] =
        FALSE;
    AppM2mScMChObj.chParams[0].outFmt.dataFormat =
        AppM2mScMChObj.curCfg->outputDataFmt;
    AppM2mScMChObj.chParams[0].outFmt.scanFormat = FVID2_SF_PROGRESSIVE;
    AppM2mScMChObj.chParams[0].srcCropCfg->cropStartX =
        AppM2mScMChObj.curCfg->cropStartX;
    AppM2mScMChObj.chParams[0].srcCropCfg->cropStartY =
        AppM2mScMChObj.curCfg->cropStartY;
    AppM2mScMChObj.chParams[0].srcCropCfg->cropWidth =
        AppM2mScMChObj.curCfg->cropWidth;
    AppM2mScMChObj.chParams[0].srcCropCfg->cropHeight =
        AppM2mScMChObj.curCfg->cropHeight;
    AppM2mScMChObj.chParams[0].scCfg->nonLinear =
        AppM2mScMChObj.curCfg->nonlinearScaling;
    AppM2mScMChObj.chParams[0].scCfg->stripSize =
        AppM2mScMChObj.curCfg->stripsize;
    AppM2mScMChObj.chParams[0].scCfg->vsType = AppM2mScMChObj.curCfg->vsType;
    AppM2mScMChObj.chParams[0].scCfg->bypass = AppM2mScMChObj.curCfg->scBypass;
    /* Edge detect is enabled for the SC_H and is dont care for all other
     * scalars. This application does not support driver involving SC_H
     */
    AppM2mScMChObj.chParams[0].scCfg->enableEdgeDetect = FALSE;

    AppM2mScMChObj.chParams[0].scCfg->enablePeaking = TRUE;

    AppM2mScMChObj.chParams[0].outMemType = VPS_VPDMA_MT_NONTILEDMEM;
    AppM2mScMChObj.chParams[0].inMemType = VPS_VPDMA_MT_NONTILEDMEM;

    if (AppM2mScMChObj.curCfg->subFrameProcEn)
    {
        AppM2mScMChObj.chParams[0].subFrameParams->subFrameModeEnable =
            AppM2mScMChObj.curCfg->subFrameProcEn;
        AppM2mScMChObj.chParams[0].subFrameParams->numLinesPerSubFrame =
            (AppM2mScMChObj.curCfg->inputHeight/AppM2mScMChObj.curCfg->NumSubFrames);
    }
    else {
        AppM2mScMChObj.chParams[0].subFrameParams = NULL;
    }

    AppM2mScMChObj.createParams.chParams = AppM2mScMChObj.chParams;
    /* Copy same configuration parameters for all channel.  Each channel
     * can have different configuraton parameters also.
     */
    for (configCnt = 1; configCnt < AppM2mScMChObj.curCfg->chPerHndl; configCnt++)
    {
        memcpy(&AppM2mScMChObj.chParams[configCnt].inFmt,
            &AppM2mScMChObj.chParams[0].inFmt,
            sizeof(FVID2_Format));
        memcpy(&AppM2mScMChObj.chParams[configCnt].outFmt,
            &AppM2mScMChObj.chParams[0].outFmt,
            sizeof(FVID2_Format));
        memcpy(AppM2mScMChObj.chParams[configCnt].scCfg,
            AppM2mScMChObj.chParams[0].scCfg,
            sizeof(Vps_ScConfig));
        memcpy(AppM2mScMChObj.chParams[configCnt].srcCropCfg,
            AppM2mScMChObj.chParams[0].srcCropCfg,
            sizeof(Vps_CropConfig));
        /* Output width of each channel is reducing by OUTPUT_WIDTH_CHANGE
           for upscaling, For downScaling its reducing by
           OUTPUT_WIDTH_CHANGE/10 */
#if 0
        if (AppM2mScMChObj.curCfg->cfgId < 6)
        {
            AppM2mScMChObj.chParams[configCnt].outFmt.width =
                    AppM2mScMChObj.chParams[configCnt - 1].outFmt.width -
                                OUTPUT_WIDTH_CHANGE;
        }
        else
        {
             AppM2mScMChObj.chParams[configCnt].outFmt.width =
                    AppM2mScMChObj.chParams[configCnt - 1].outFmt.width -
                                (OUTPUT_WIDTH_CHANGE / 10);
        }
#endif
        /* Copy the input and output mem type for each channel */
        AppM2mScMChObj.chParams[configCnt].inMemType =
            AppM2mScMChObj.chParams[0].inMemType;
        AppM2mScMChObj.chParams[configCnt].outMemType =
            AppM2mScMChObj.chParams[0].outMemType;
        if (AppM2mScMChObj.curCfg->subFrameProcEn){
            memcpy(AppM2mScMChObj.chParams[configCnt].subFrameParams,
                AppM2mScMChObj.chParams[0].subFrameParams,
                sizeof(Vps_SubFrameParams));
        }
        else
        {
            AppM2mScMChObj.chParams[configCnt].subFrameParams = NULL;
        }
    }
}

/**
 *  Memory driver test function.
 */
static Int32 vpsScMChTest(UInt32 driverInst)
{
    Int32                   retVal = FVID2_SOK;
    Vps_M2mScCreateStatus   status;
    UInt32                  frmCnt, reqCnt, subFrameNum, horzVal, vertVal;
    Semaphore_Params        semParams;
    FVID2_CbParams          cbParams;
    Vps_ScLazyLoadingParams lazyLoadingParams;
    Vps_M2mScRtParams      *rtParamsptr[VPS_M2M_SC_MAX_CH];
    Vps_M2mScRtParams       rtParams[VPS_M2M_SC_MAX_CH];
    Vps_FrameParams         outFrmParams[VPS_M2M_SC_MAX_CH];

    Semaphore_Params_init(&semParams);
    AppM2mScMChObj.reqProcessed = Semaphore_create(0, &semParams, NULL);

    /* Initialize the frame pointers */
    vpsScMChInitVariables();

    /* Set the configuration parameters for each channel */
    vpsScMChSetConfigParams();
    /* Open the driver */
    cbParams.cbFxn = vpsScMChCbFxn;
    cbParams.errCbFxn = vpsScMChErrCbFxn;
    cbParams.errList = &AppM2mScMChObj.errProcessList;
    cbParams.appData = NULL;
    cbParams.reserved = NULL;
    /* Open the driver */
    AppM2mScMChObj.fvid2Handle = FVID2_create(FVID2_VPS_M2M_SC_DRV,
                                    driverInst,
                                    &AppM2mScMChObj.createParams,
                                    &status,
                                    &cbParams);
    if (NULL == AppM2mScMChObj.fvid2Handle)
    {
        Vps_printf("M2mScMultiChan:%s: Error %d @ line %d\n",
            __FUNCTION__, status.retVal, __LINE__);
        Vps_printf("M2mScMultiChan:Maximum handles supported %d\n",
            status.maxHandles);
        Vps_printf("M2mScMultiChan:Maximum channels supported %d\n",
            status.maxChannelsPerHandle);
        Vps_printf("M2mScMultiChan:Maximum number of same channels that can be"
            "submitted in single request %d\n",
            status.maxSameChPerRequest);
        Vps_printf("M2mScMultiChan:Maxumim number of requests that can be"
            " queued %d\n",
            status.maxReqInQueue);
        return (status.retVal);
    }
    /* Set the coefficients.  Coefficients programming is allowed only once
    before submitting the first request */
    /* Currently doing upsampling so set coefficients for the up sampling. */
    /* Set the horizontal scaling set */
    AppM2mScMChObj.coeffParams.hScalingSet  = VPS_SC_US_SET;
    /* Set the vertical scaling set */
    AppM2mScMChObj.coeffParams.vScalingSet = VPS_SC_US_SET;
    /* Set to null  if user is not providing the coefficients */
    AppM2mScMChObj.coeffParams.coeffPtr = NULL;
    AppM2mScMChObj.coeffParams.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;
    retVal = FVID2_control(AppM2mScMChObj.fvid2Handle, IOCTL_VPS_SET_COEFFS,
        &AppM2mScMChObj.coeffParams, NULL);
    if (0 != retVal)
    {
        Vps_printf("M2mScMultiChan:Setting of the coefficients failed\n");
    }

    lazyLoadingParams.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;
    lazyLoadingParams.enableLazyLoading = TRUE;
    lazyLoadingParams.enableFilterSelect = TRUE;
    retVal = FVID2_control(AppM2mScMChObj.fvid2Handle,
        IOCTL_VPS_SC_SET_LAZY_LOADING,
        &lazyLoadingParams,
        NULL);
    if (0 != retVal)
    {
        Vps_printf("M2mScMultiChan:Enabling Lazy Loading failed\n");
    }

    VpsUtils_prfLoadCalcStart();
    VpsUtils_prfTsBegin(AppM2mScMChObj.prfTsHandle);
    for (reqCnt = 0; reqCnt < AppM2mScMChObj.curCfg->loopCount; reqCnt++)
    {
        if (!AppM2mScMChObj.curCfg->subFrameProcEn)
        {   /* FRAME based processing */
            for (frmCnt = 0; frmCnt < AppM2mScMChObj.curCfg->chPerHndl; frmCnt++)
            {
                if (0 == reqCnt)
                {
                    rtParamsptr[frmCnt]               = &rtParams[frmCnt];
                    rtParamsptr[frmCnt]->outFrmPrms   = &outFrmParams[frmCnt];
                    rtParamsptr[frmCnt]->inFrmPrms    = NULL;
                    rtParamsptr[frmCnt]->srcCropCfg   = NULL;
                    rtParamsptr[frmCnt]->scCfg        = NULL;
                    horzVal = (   AppM2mScMChObj.chParams[frmCnt].outFmt.width
                               /  AppM2mScMChObj.curCfg->chPerHndl);
                    vertVal = (   AppM2mScMChObj.chParams[frmCnt].outFmt.height
                               /  AppM2mScMChObj.curCfg->chPerHndl);
                    rtParamsptr[frmCnt]->outFrmPrms->width = VpsUtils_align((
                            AppM2mScMChObj.chParams[frmCnt].outFmt.width
                        -   (frmCnt * horzVal)), VPS_BUFFER_ALIGNMENT);
                    /* Minimum horizontal scaling ratio is 1/16 */
                    if (   rtParamsptr[frmCnt]->outFrmPrms->width
                        <  (AppM2mScMChObj.chParams[frmCnt].outFmt.width / 16u))
                    {
                        rtParamsptr[frmCnt]->outFrmPrms->width =
                            AppM2mScMChObj.chParams[frmCnt].outFmt.width / 16u;
                    }
                    rtParamsptr[frmCnt]->outFrmPrms->height = VpsUtils_align((
                            AppM2mScMChObj.chParams[frmCnt].outFmt.height
                        -   (frmCnt * vertVal)), VPS_BUFFER_ALIGNMENT);
                    if (   (    FVID2_DF_YUV420SP_UV
                            ==  AppM2mScMChObj.curCfg->outputDataFmt)
                        || (    FVID2_DF_YUV422SP_UV
                            ==  AppM2mScMChObj.curCfg->outputDataFmt))
                    {
                        rtParamsptr[frmCnt]->outFrmPrms->pitch
                            [FVID2_YUV_SP_Y_ADDR_IDX] =
                                AppM2mScMChObj.chParams[frmCnt].outFmt.pitch
                                    [FVID2_YUV_SP_Y_ADDR_IDX];
                        rtParamsptr[frmCnt]->outFrmPrms->pitch
                            [FVID2_YUV_SP_CBCR_ADDR_IDX] =
                                AppM2mScMChObj.chParams[frmCnt].outFmt.pitch
                                    [FVID2_YUV_SP_CBCR_ADDR_IDX];
                    }
                    else
                    {
                        rtParamsptr[frmCnt]->outFrmPrms->pitch
                            [FVID2_YUV_INT_ADDR_IDX] =
                                AppM2mScMChObj.chParams[frmCnt].outFmt.pitch
                                    [FVID2_YUV_INT_ADDR_IDX];
                    }
                    rtParamsptr[frmCnt]->outFrmPrms->memType =
                        AppM2mScMChObj.chParams[frmCnt].outMemType;
                    rtParamsptr[frmCnt]->outFrmPrms->dataFormat =
                        AppM2mScMChObj.curCfg->outputDataFmt;
                    AppM2mScMChObj.processList.inFrameList[0]->frames[frmCnt]->
                        perFrameCfg = rtParamsptr[frmCnt];
                }
                else
                {
                    AppM2mScMChObj.processList.inFrameList[0]->frames[frmCnt]->
                        perFrameCfg = NULL;
                }
            }
            retVal = FVID2_processFrames(AppM2mScMChObj.fvid2Handle,
                        &AppM2mScMChObj.processList);
            if (0 != retVal)
            {
                Vps_printf("M2mScMultiChan:Processing of the frame failed\n\n");
            }
            /* Wait till frames gets processed */
            Semaphore_pend(AppM2mScMChObj.reqProcessed, BIOS_WAIT_FOREVER);
            /* Clear the processlist before getting it from driver */
            memset(&AppM2mScMChObj.processList,
                0,
                sizeof(AppM2mScMChObj.processList));
            /* Get the processed request */
            retVal = FVID2_getProcessedFrames(AppM2mScMChObj.fvid2Handle,
                        &AppM2mScMChObj.processList,
                        0);
            if (0 != retVal)
            {
                Vps_printf("M2mScMultiChan:Getting of the process frames"
                    " function Failed\n\n");
            }
            /* Update the frame with new buffer addresses */
            vpsScMchUpdateBuffers();
        }
        else
        {
            /* SUB-FRAME based processing */
            for (subFrameNum = 0;
                 subFrameNum < AppM2mScMChObj.curCfg->NumSubFrames;
                 subFrameNum++)
            {
                FVID2_Frame *inFrm;

                /* update subframe info in process list */
                inFrm  = AppM2mScMChObj.processList.inFrameList[0]->frames[0];
                inFrm->subFrameInfo->subFrameNum  = subFrameNum;
                inFrm->subFrameInfo->numInLines =
                    (subFrameNum+1)*(AppM2mScMChObj.curCfg->inputHeight/
                    AppM2mScMChObj.curCfg->NumSubFrames);
                /* do subframe processing */
                retVal = FVID2_processFrames(AppM2mScMChObj.fvid2Handle,
                    &AppM2mScMChObj.processList);
                if (0 != retVal)
                {
                    Vps_printf("M2mScMultiChan:Processing of the frame failed\n\n");
                }
                /* Wait till frames gets processed */
                Semaphore_pend(AppM2mScMChObj.reqProcessed, BIOS_WAIT_FOREVER);
                /* Clear the processlist before getting it from driver */
                memset(&AppM2mScMChObj.processList,
                    0,
                    sizeof(AppM2mScMChObj.processList));
                /* Get the processed request */
                retVal = FVID2_getProcessedFrames(AppM2mScMChObj.fvid2Handle,
                            &AppM2mScMChObj.processList, 0);
                if (0 != retVal)
                {
                    Vps_printf("M2mScMultiChan:Getting of the process frames"
                        " function Failed\n\n");
                }
            }
        }

        if ((reqCnt % 250) == 0)
        {
            Vps_printf("Loop Count = %d\n", reqCnt);
        }
    }
    VpsUtils_prfLoadCalcStop();
    VpsUtils_prfTsEnd(AppM2mScMChObj.prfTsHandle,
        AppM2mScMChObj.curCfg->loopCount * AppM2mScMChObj.curCfg->chPerHndl);
    VpsUtils_prfLoadPrintAll(TRUE);
    VpsUtils_prfTsPrint(AppM2mScMChObj.prfTsHandle, TRUE);
    VpsUtils_prfLoadCalcReset();
    if (0 == retVal)
    {
        Vps_printf("M2mScMultiChan:Processing of the frames passed\n");
    }
    retVal = FVID2_delete(AppM2mScMChObj.fvid2Handle, NULL);
    Semaphore_delete(&AppM2mScMChObj.reqProcessed);
    return retVal;
}



/**
 *  Driver callback function.
 */
static Int32 vpsScMChCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32 retVal = FVID2_SOK;

    /* Request completed processing */
    Semaphore_post(AppM2mScMChObj.reqProcessed);

    return (retVal);
}



/**
 *  Driver error callback function.
 */
static Int32 vpsScMChErrCbFxn(FVID2_Handle handle,
                            Ptr appData,
                            Void *errList,
                            Ptr reserved)
{
    Int32 retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}


/**
 *  Initialize the global variables and frame pointers.
 */
static Void vpsScMChInitVariables(void)
{
    UInt32              frmCnt, chCnt;
    FVID2_FrameList     *inFrameListPtr;
    FVID2_FrameList     *outFrameListPtr;
    UInt32              inBufIndex, outBufIndex;
    UInt32              chPerHndl;

    AppM2mScMChObj.inputBufIndex = 0;
    AppM2mScMChObj.outputBufIndex = 0;
    inBufIndex = AppM2mScMChObj.inputBufIndex;
    outBufIndex = AppM2mScMChObj.outputBufIndex;
    chPerHndl = AppM2mScMChObj.curCfg->chPerHndl;
    for (chCnt = 0;  chCnt < chPerHndl; chCnt++)
    {
        AppM2mScMChObj.chParams[chCnt].scCfg = &AppM2mScMChObj.chScCfg[chCnt];
        AppM2mScMChObj.chParams[chCnt].srcCropCfg =
            &AppM2mScMChObj.chSrcCropCfg[chCnt];
        if (AppM2mScMChObj.curCfg->subFrameProcEn){
            AppM2mScMChObj.chParams[chCnt].subFrameParams =
                &AppM2mScMChObj.subFrameParams[chCnt];
        }
        else
        {
            AppM2mScMChObj.chParams[chCnt].subFrameParams = NULL;
        }
    }
    AppM2mScMChObj.processList.inFrameList[0] = &AppM2mScMChObj.inFrameList;
    AppM2mScMChObj.processList.outFrameList[0] = &AppM2mScMChObj.outFrameList;
    AppM2mScMChObj.processList.numInLists = IN_NUM_FRAMELIST;
    AppM2mScMChObj.processList.numOutLists = OUT_NUM_FRAMELIST;
    AppM2mScMChObj.processList.reserved = NULL;

    inFrameListPtr = AppM2mScMChObj.processList.inFrameList[0];
    inFrameListPtr->numFrames = AppM2mScMChObj.curCfg->chPerHndl;
    inFrameListPtr->perListCfg = NULL;
    inFrameListPtr->reserved = NULL;
    for (frmCnt = 0; frmCnt < chPerHndl; frmCnt++)
    {
        inFrameListPtr->frames[frmCnt] = &AppM2mScMChObj.inFrames[frmCnt];
        inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_SP_Y_ADDR_IDX] =
            AppM2mScMChObj.inputBuffer[inBufIndex][0];
        inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_SP_CBCR_ADDR_IDX] =
            AppM2mScMChObj.inputBuffer[inBufIndex][1];
        inFrameListPtr->frames[frmCnt]->fid = FVID2_FID_FRAME;
        inFrameListPtr->frames[frmCnt]->channelNum = frmCnt;
        inFrameListPtr->frames[frmCnt]->appData = NULL;
        inFrameListPtr->frames[frmCnt]->perFrameCfg = NULL;
        if (AppM2mScMChObj.curCfg->subFrameProcEn){
             inFrameListPtr->frames[frmCnt]->subFrameInfo =
                &AppM2mScMChObj.subFrameInfo[chCnt];
        }
        else
        {
            inFrameListPtr->frames[frmCnt]->subFrameInfo = NULL;
        }
        inFrameListPtr->frames[frmCnt]->reserved = NULL;
    }
    /* Since we have same input buffer for all channels. Go to next buffer
     * after all frames of channels are udpated with same input buffer
     * address
     */
    AppM2mScMChObj.inputBufIndex++;
    if (AppM2mScMChObj.inputBufIndex == MAX_IN_BUFFERS)
    {
        AppM2mScMChObj.inputBufIndex = 0;
    }

    outFrameListPtr = AppM2mScMChObj.processList.outFrameList[0];
    outFrameListPtr->numFrames = AppM2mScMChObj.curCfg->chPerHndl;
    outFrameListPtr->perListCfg = NULL;
    outFrameListPtr->reserved = NULL;
    for (frmCnt = 0; frmCnt < chPerHndl; frmCnt++)
    {
        outFrameListPtr->frames[frmCnt] =
            &AppM2mScMChObj.outFrames[frmCnt];;
        outFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_INT_ADDR_IDX] =
            AppM2mScMChObj.outputBuffer[outBufIndex][0];
        outFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_SP_CBCR_ADDR_IDX] =
            AppM2mScMChObj.outputBuffer[outBufIndex][1];
        outFrameListPtr->frames[frmCnt]->channelNum = frmCnt;
        outFrameListPtr->frames[frmCnt]->appData = NULL;
        outFrameListPtr->frames[frmCnt]->perFrameCfg = NULL;
        outFrameListPtr->frames[frmCnt]->subFrameInfo = NULL;
        outFrameListPtr->frames[frmCnt]->reserved = NULL;
        AppM2mScMChObj.outputBufIndex++;
        if (AppM2mScMChObj.outputBufIndex == (MAX_IN_BUFFERS * chPerHndl))
        {
            AppM2mScMChObj.outputBufIndex = 0;
        }
    }

    /* Init error process list */
    memset(&AppM2mScMChObj.errProcessList,
        0u,
        sizeof (AppM2mScMChObj.errProcessList));
}

static Int32 vpsScMchUpdateBuffers()
{
    UInt32              frmCnt;
    FVID2_FrameList     *inFrameListPtr;
    FVID2_FrameList     *outFrameListPtr;
    UInt32              inBufIndex, outBufIndex;
    UInt32              chPerHndl;

    inBufIndex = AppM2mScMChObj.inputBufIndex;
    outBufIndex = AppM2mScMChObj.outputBufIndex;
    inFrameListPtr = AppM2mScMChObj.processList.inFrameList[0];
    outFrameListPtr = AppM2mScMChObj.processList.outFrameList[0];
    chPerHndl = AppM2mScMChObj.curCfg->chPerHndl;
    for (frmCnt = 0; frmCnt < chPerHndl; frmCnt++)
    {
        inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_SP_Y_ADDR_IDX] =
            AppM2mScMChObj.inputBuffer[inBufIndex][0];
        inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_SP_CBCR_ADDR_IDX] =
            AppM2mScMChObj.inputBuffer[inBufIndex][1];
        outFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_INT_ADDR_IDX] =
            AppM2mScMChObj.outputBuffer[outBufIndex][0];
        outFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
            [FVID2_YUV_SP_CBCR_ADDR_IDX] =
            AppM2mScMChObj.outputBuffer[outBufIndex][1];
        outBufIndex++;
        if (outBufIndex == (MAX_IN_BUFFERS * chPerHndl))
        {
            outBufIndex = 0;
        }
    }
    /* Since we have same input buffer for all channels. Go to next buffer
     * after all frames of channels are udpated with same input buffer
     * address
     */
    AppM2mScMChObj.inputBufIndex++;
    if (AppM2mScMChObj.inputBufIndex == MAX_IN_BUFFERS)
    {
        AppM2mScMChObj.inputBufIndex = 0;
    }
    /* Store back temporary outputBuf Index to appObj*/
    AppM2mScMChObj.outputBufIndex = outBufIndex;
    return 0;

}

static Int32 vpsScMChAllocInBufs()
{
    UInt32      inputPitch;
    UInt32      numInBufs;
    Int32       retVal = 0;
    UInt8       *inputTempbuffer;
    /* Allocate temporary buffer for loading data through CCS scripting console
     */
    inputPitch = get16ByteAlignedPitch(AppM2mScMChObj.curCfg->inputWidth,
                        AppM2mScMChObj.curCfg->inputDataFmt);
    if (AppM2mScMChObj.curCfg->inputDataFmt == FVID2_DF_YUV420SP_UV)
    {
        /* pitch = width, but bits per pixel is 12 for YUV420SP */
        AppM2mScMChObj.inputBufferSize =
            (inputPitch * AppM2mScMChObj.curCfg->inputHeight * 3) >> 1;
    }
    else if (AppM2mScMChObj.curCfg->inputDataFmt == FVID2_DF_YUV422SP_UV)
    {
        /* pitch = width, but bits per pixel is 16 for YUV422SP */
        AppM2mScMChObj.inputBufferSize =
            inputPitch * AppM2mScMChObj.curCfg->inputHeight * 2;
    }
    else
    {
        /* pitch = (width * 2), and bits per pixel is 16 for YUV422I */
        AppM2mScMChObj.inputBufferSize =
            inputPitch * AppM2mScMChObj.curCfg->inputHeight;
    }
    inputTempbuffer = (UInt8 *)VpsUtils_memAlloc(
                                            MAX_IN_BUFFERS *
                                            AppM2mScMChObj.inputBufferSize,
                                            VPS_BUFFER_ALIGNMENT);
    if (NULL != inputTempbuffer)
    {
        Vps_printf("M2mScMultiChan:Load %d %s buffers of size %dx%d with pitch as %d bytes to location: 0x%p\n",
            MAX_IN_BUFFERS,
            AppM2mScFmtName[AppM2mScMChObj.curCfg->inputDataFmt],
            AppM2mScMChObj.curCfg->inputWidth,
            AppM2mScMChObj.curCfg->inputHeight,
            inputPitch,
            inputTempbuffer);
        Vps_printf(
            "M2mScMultiChan:Load input buffer with command:loadRaw(0x%.8x, 0, "
            "\"<filePath>_%s_prog_packed_%d_%d.tigf\", 32, false);\n",
            inputTempbuffer,
            AppM2mScFmtName[AppM2mScMChObj.curCfg->inputDataFmt],
            AppM2mScMChObj.curCfg->inputWidth,
            AppM2mScMChObj.curCfg->inputHeight);
    }
    else
    {
        Vps_printf(
        "M2mScMultiChan: %s: Error %d @ line %d\n", __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EALLOC;
        return retVal;
    }
    /* If input is from non tiled space only update the buffer pointers.
     * No need to copy data
     */
    for (numInBufs = 0; numInBufs < MAX_IN_BUFFERS; numInBufs++)
    {
        AppM2mScMChObj.inputBuffer[numInBufs][0] =
            &inputTempbuffer[numInBufs * AppM2mScMChObj.inputBufferSize];

        /* For YUV420 SP UV plane Don't care for YUV422I  format */
        AppM2mScMChObj.inputBuffer[numInBufs][1] =
            &inputTempbuffer[(numInBufs * AppM2mScMChObj.inputBufferSize) +
                (inputPitch * AppM2mScMChObj.curCfg->inputHeight)];
    }
    return retVal;
}

static Int32  vpsScMChAllocOutBufs()
{
    UInt32      outputPitch;
    UInt32      numOutBufs;
    Int32       retVal = 0;
    UInt8       *outTempBuffer;

    outputPitch = get16ByteAlignedPitch(AppM2mScMChObj.curCfg->outputWidth,
                        AppM2mScMChObj.curCfg->outputDataFmt);
    /* Allocate temporary buffer for loading data through CCS scripting console
     */
    if (AppM2mScMChObj.curCfg->outputDataFmt == FVID2_DF_YUV420SP_UV)
    {
        /* pitch = width, but bits per pixel is 12 for YUV420SP */
        AppM2mScMChObj.outputBufferSize =
            (outputPitch * AppM2mScMChObj.curCfg->outputHeight * 3) >> 1;
    }
    else if (AppM2mScMChObj.curCfg->outputDataFmt == FVID2_DF_YUV422SP_UV)
    {
        /* pitch = width, but bits per pixel is 16 for YUV422SP */
        AppM2mScMChObj.outputBufferSize =
            outputPitch * AppM2mScMChObj.curCfg->outputHeight * 2;
    }
    else
    {
        /* pitch = (width * 2), and bits per pixel is 16 for YUV422I */
        AppM2mScMChObj.outputBufferSize =
            outputPitch * AppM2mScMChObj.curCfg->outputHeight;
    }

    outTempBuffer = (UInt8 *)VpsUtils_memAlloc(
                                (AppM2mScMChObj.outputBufferSize *
                                    AppM2mScMChObj.curCfg->chPerHndl *
                                    MAX_IN_BUFFERS),
                                VPS_BUFFER_ALIGNMENT);
    if (NULL == outTempBuffer)
    {
        Vps_printf("M2mScMultiChan: %s: Error %d @ line %d\n",
            __FUNCTION__, retVal, __LINE__);
        retVal = FVID2_EALLOC;
        return retVal;
    }

    for (numOutBufs = 0;
        numOutBufs < AppM2mScMChObj.curCfg->chPerHndl * MAX_IN_BUFFERS;
        numOutBufs++)
    {
        AppM2mScMChObj.outputBuffer[numOutBufs][0] =
            &outTempBuffer[numOutBufs * AppM2mScMChObj.outputBufferSize];

        /* For YUV420 SP UV plane Don't care for YUV422I  format */
        AppM2mScMChObj.outputBuffer[numOutBufs][1] =
            &outTempBuffer[(numOutBufs * AppM2mScMChObj.outputBufferSize) +
                (outputPitch * AppM2mScMChObj.curCfg->outputHeight)];
    }
    return retVal;
}

static Void vpsScMChFreeInOutBufs()
{
    VpsUtils_memFree(AppM2mScMChObj.outputBuffer[0][0],
        AppM2mScMChObj.outputBufferSize * MAX_IN_BUFFERS *
            AppM2mScMChObj.curCfg->chPerHndl);
    VpsUtils_memFree(AppM2mScMChObj.inputBuffer[0][0],
            AppM2mScMChObj.inputBufferSize * MAX_IN_BUFFERS);
}

static Int32 vpsScMChGetTstId()
{
    Int32       testId;
    UInt32      tstCnt;
    char        inputStr[10];

    Vps_printf("Enter TestCase ID:\n");
    for (tstCnt = 0;
         tstCnt < sizeof(AppM2mScMchCfg)/sizeof(AppM2mScMCh_Cfg);
         tstCnt++)
    {
        Vps_printf("%d:\t%s\n", tstCnt, AppM2mScMchCfg[tstCnt].testDesc);
    }
    Vps_printf("%d:\t%s\n",
        sizeof(AppM2mScMchCfg)/sizeof(AppM2mScMCh_Cfg),"Auto Run\n");
    Vps_printf("%d:\t%s\n", 100,"EXIT\n");
    VpsUtils_getString(inputStr, BIOS_WAIT_FOREVER);
    testId = atoi(inputStr);

    return (testId);
}

static UInt32 get16ByteAlignedPitch(UInt32 width, UInt32  dataFmt)
{
    UInt32 pitch;

    switch (dataFmt)
    {
        case FVID2_DF_YUV420SP_UV:
        case FVID2_DF_YUV422SP_UV:
        {
            pitch = width;
            break;
        }
        case FVID2_DF_YUV422I_YUYV:
            pitch = width * 2u;
            break;
        default:
            pitch = width;
    }

    pitch = VpsUtils_align (pitch, VPS_BUFFER_ALIGNMENT);
    return pitch;
}
