/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2012 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_proxyServerSc.h
 *
 *  \brief This file deal with declaration global funtions used by Proxy Server
 *         for scalar api calls.
 *
 *         This file declares the scalar APIs, that will be used by
 *         proxyserver for supporting YUV420 upsampling and scaling for both
 *         420 and 422 formats.
 *
 */


#ifndef _VPS_PROXY_SERVER_SCALAR_H
#define _VPS_PROXY_SERVER_SCALAR_H

#include <string.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mSc.h>
#include <ti/psp/proxyServer/vps_proxyServer.h>

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================================================== */
/*                             Local Macros                                   */
/* ========================================================================== */

/* Driver instance to test. */
#define DRIVER_INSTANCE                 (VPS_M2M_INST_SEC0_SC5_WB2)
/* Number of channels to be supported per handle */
#define CHANNELS_PER_HANDLE             (1u)
/* Total Number of requests to be processed */
#define TOTAL_NUM_OF_REQUESTS           (1u)
/* Total Number of requests to be processed */
#define IN_NUM_BUFFERS                  (1u * TOTAL_NUM_OF_REQUESTS)
#define IN_NUM_FRAMELIST                (1u)
/* Number of frames per framelist. */
#define IN_NUM_FRAMES_PER_LIST          (1u * CHANNELS_PER_HANDLE)
/* Total number of buffers to allocate.
 * This should be the number of frames that should be loaded in to the
 * input buffer. */
#define IN_TOTAL_NUM_BUFFERS             (IN_NUM_FRAMES_PER_LIST * \
                                            IN_NUM_BUFFERS)
#define OUTPUT_WIDTH                     1920
#define OUTPUT_HEIGHT                    1080
#define OUT_BUFFER_SIZE                 (OUTPUT_WIDTH * OUTPUT_HEIGHT * 2u)
#define OUT_DATA_FORMAT                 (FVID2_DF_YUV422I_YUYV)
//#define OUT_SCAN_FORMAT                 (FVID2_SF_PROGRESSIVE)
#define OUT_NUM_BUFFERS                 (1u * TOTAL_NUM_OF_REQUESTS)
#define OUT_NUM_FRAMELIST               (1u)
/* Number of frames per framelist. */
#define OUT_NUM_FRAMES_PER_LIST         (1u * CHANNELS_PER_HANDLE)
/* Total number of buffers to allocate.
 * This should be the number of frames that should be loaded in to the
 * input buffer. */
#define OUT_TOTAL_NUM_BUFFERS           (OUT_NUM_FRAMES_PER_LIST * \
                                            OUT_NUM_BUFFERS)
/* Scalar params */
#define SC_NON_LINEAR_SCAL      FALSE /*Poly phase filter */
#define SC_STRIP_SIZE           0
#define SC_VER_SCAL_TYPE        VPS_SC_VST_POLYPHASE
#define SC_BYPASS               FALSE /* Dont bypass scalar */

/* In case of YUV 420 semiplanar data we will have two planes one for Y
 * an another for CB and CR
 */
#define MAX_PLANES              2u



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 * @brief  M2M scalar in/out frame and driver config
 *
 *         Strucutre is used to configure the m2m driver for different
 *         configurations
 */

typedef struct
{
UInt32 outw;
UInt32 outh;
UInt32 cropx;
UInt32 cropy;
UInt32 inFrmWidth;
UInt32 inFrmHeight;
UInt32 scEnable;
UInt32 scCount;
} M2mScCh_Cfg;

/**
 * @brief  structure holding scalar config objects
 *
 *         This structure contains fvid2 and vps structures that are used for
 *         initialization and configuration of m2m scalar with default configs.
 *
 */

typedef struct
{
    Semaphore_Params semParams;
    /* Mem2Mem Driver handle */
    FVID2_Handle scwbHandle;
    /* Process list for submitting request */
    FVID2_ProcessList scProcessList[TOTAL_NUM_OF_REQUESTS];
    /* Input Framelist */
    FVID2_FrameList scInFrameList
           [IN_NUM_FRAMELIST * TOTAL_NUM_OF_REQUESTS];
    /* Output FrameList */
    FVID2_FrameList scOutFrameList
           [OUT_NUM_FRAMELIST * TOTAL_NUM_OF_REQUESTS];
    /* In FrameBuffer*/
    FVID2_Frame scInFrames
           [IN_NUM_FRAMES_PER_LIST * TOTAL_NUM_OF_REQUESTS];
    /* Out FrameBuffer Pointers */
    FVID2_Frame scOutFrames
           [OUT_NUM_FRAMES_PER_LIST * TOTAL_NUM_OF_REQUESTS];
    /* Error processList */
    FVID2_ProcessList scErrProcessList;
    /* In FrameBuffer pointers for Scalar */
    UInt8 *scInput420Buffer
              [IN_TOTAL_NUM_BUFFERS * TOTAL_NUM_OF_REQUESTS][MAX_PLANES];
    /* Stream complete semaphoreused to sync between callback and task. */
    Semaphore_Handle semReqProcessed;
    /* Fvid2 handle for graphics display*/
    Ptr grpxFvid2Handle;
    /* Per handle parameters for each driver */
    Vps_M2mScCreateParams scCreateParams;
    /* Configuration parameters for each channel*/
    Vps_M2mScChParams scChParams[CHANNELS_PER_HANDLE];
    /* Parameters for setting coefficients */
    Vps_ScCoeffParams scCoeffParams;
    /* Runtime configuration parameters */
    Vps_M2mScRtParams scRtParams;
    /* Runtime scalar output configuration parameters */
    Vps_FrameParams scOutFrmPrms;
    /* Runtime scalar inputframe configuration parameters */
    Vps_FrameParams scInFrmPrms;
    /* Runtime scalar crop configuration parameters */
    Vps_CropConfig scSrcCropCfg;
    /* Instance of the output format for each channel */
    Vps_ScConfig chScCfg[CHANNELS_PER_HANDLE];
    /* Instance of the channel scalar config */
    Vps_CropConfig chSrcCropCfg[CHANNELS_PER_HANDLE];
    /* in/out subframe info for in/out framelist */
    FVID2_SubFrameInfo scSubFrameInfo[2];
    /* Queue element for temporarily keeping input buffer pointer from V4L2 */
    VpsUtils_QElem qElem;
    /* Queue for keeping input buffer pointer from V4L2 */
    VpsUtils_QHandle inbufQHandle;
    M2mScCh_Cfg curCfg;
} M2mScMCh_Obj;


/* ========================================================================== */
/*                       SCALAR APIs Declarations                             */
/* ========================================================================== */

 /**
 * \brief psScale
 *
 *          This function is called by the proxy server when scaling is enabled.
 *          Does upsampling of nv12 frame and scaling of input frames.
 *
 * \param   qParams   [IN] Pointer of type VPS_PSrvFvid2QueueParams
 *
 * \return  VPS_SOK if successful, else suitable error code
 */
Int32 psScale( VPS_PSrvFvid2QueueParams *qParams);

/**
 * \brief psScalarDequeue
 *
 *          This function is called by proxy server to additional dequeuing
 *          steps when scalar is enabled.
 *
 * \param   dQParams   [IN] Pointer of type VPS_PSrvFvid2DequeueParams
 *
 * \return  void       Returns nothing
 */
void  psScalarDequeue(VPS_PSrvFvid2DequeueParams *dQParams);

/**
 * \brief psScalarControl
 *
 *          This function is called by proxy server to do additional control
 *          settings for fid2 display driver and scalar config settings.
 *
 * \param   ctrlParams   [IN] Pointer of type VPS_PSrvFvid2ControlParams
 *
 * \return  void         Returns nothing
 */
void  psScalarControl(VPS_PSrvFvid2ControlParams *ctrlParams);

/**
 * \brief psScalarDelete
 *
 *          This function is called by proxy server to reset/deinitialize
 *          scalar handle and and other scalar config params.
 *
 * \param   delParams   [IN] Pointer of type VPS_PSrvFvid2DeleteParams
 *
 * \return  FVID2_SOK if successful, else suitable error code
 */
Int32 psScalarDelete(VPS_PSrvFvid2DeleteParams *delParams);

#ifdef __cplusplus
}
#endif

#endif /* _VPS_PROXY_SERVER_SCALAR_H */
