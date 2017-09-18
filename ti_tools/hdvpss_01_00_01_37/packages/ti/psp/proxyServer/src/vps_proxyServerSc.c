/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_proxyServerSc.c
 *
 *  \brief This file implements the functions / tasks that constitute
 *         Scaling and chroma upsampling
 *
 *         Scalar provides scaling or/and chroma upsampling for YUV422 and 420
 *         frames passed from A8 drivers before giving it to display.
 */

#define SYSLINKS_IPC_NOTIFY_INCLUDED
#define VPS_PS_TASK_4_CB


/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */



#include <ti/psp/proxyServer/src/vps_proxyServerSc.h>
/* ========================================================================== */
/*                             DEBUG Macros                                   */
/* ========================================================================== */
#define PSTrace (GT_TraceState_Enable | GT_ERR )

/* ========================================================================== */
/*                             Local Macros                                   */
/* ========================================================================== */
#define VPS_PSRV_IPC_LINE_ID_USED   (0x0u)




/* ========================================================================== */
/*                    Local Functions prototype                               */
/* ========================================================================== */

static void initScalar();
/**< Function for initialization of m2m scalar */
static void scalarInitVariables(void);
/**< Function for initialization of m2m scalar parameters */
static void scalarSetConfigParams(void);
/**< Function for initial configuraton of m2m scalar creation parameters */
static void scalar_Fvid2_Create(void) ;
/**< Funtion used to create handle for FVID2 scalar */
static Int32 scalarCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved);
/**< Scalar callback funtion */
static Int32 scalarErrCbFxn(FVID2_Handle handle,
                Ptr appData,
                Void *errList,
                Ptr reserved);
/**< Scalar error callback funtion */
static void setScalarRtParams(Vps_FrameParams *inFrmptr);
/**< Function for runtime configuration of Scalar params */
static Int32  initQueue(void);
/**<  create input queue to hold input buffers */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
static M2mScMCh_Obj M2mScChObj;

/**
 * \brief initScalar
 *        This function will be called to by FVID2  Queue call for display
 *         from host
 *
 *        This function would initialize scalar and make it ready for
 *        Scaling and upsampling of input frames
 *
 * \param   scFrmPrm    [IN] pointer to Scalar frame params passed from host.
 * \param   inFrmptr   [IN]  pointer to input frame params passed from host.
 *
 * \return  None
 */
static void initScalar()
{
    /* Initialize the frame pointers */
    scalarInitVariables();
    /* Set the configuration parameters for each channel */
    scalarSetConfigParams();

    M2mScChObj.scChParams[0].inFmt.width = M2mScChObj.curCfg.inFrmWidth;
    M2mScChObj.scChParams[0].inFmt.height = M2mScChObj.curCfg.inFrmHeight;
    M2mScChObj.scChParams[0].srcCropCfg->cropWidth =
                                            M2mScChObj.curCfg.inFrmWidth;
    M2mScChObj.scChParams[0].srcCropCfg->cropHeight =
                                            M2mScChObj.curCfg.inFrmHeight;
    if (M2mScChObj.scChParams[0].inFmt.dataFormat == FVID2_DF_YUV422I_YUYV)
    {
        M2mScChObj.scChParams[0].inFmt.pitch[FVID2_YUV_INT_ADDR_IDX]
            = M2mScChObj.curCfg.inFrmWidth * 2u;
    }
    else
    {
        M2mScChObj.scChParams[0].inFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX]
            = (M2mScChObj.curCfg.inFrmWidth + (2 * 32) + 127) & 0xFFFFFF80;
        M2mScChObj.scChParams[0].inFmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX]
            = (M2mScChObj.curCfg.inFrmWidth + (2 * 32) + 127) & 0xFFFFFF80;
    }
    scalar_Fvid2_Create();
}


 /**
 * \brief scalarInitVariables
 *        Initialize the global variables and frame pointers used for scalar.
 *
 *        This function would initialize global varibales and in/out frame-list
 *        pointers for scalar.
 *
 * \param   None
 *
 * \return  None
 */
static Void scalarInitVariables(void)
{
    UInt32              frmListCnt, reqCnt, frmCnt, chCnt;
    FVID2_FrameList     *inFrameListPtr;
    FVID2_FrameList     *outFrameListPtr;

    for (chCnt = 0;  chCnt < CHANNELS_PER_HANDLE; chCnt++)
    {
        M2mScChObj.scChParams[chCnt].scCfg = &M2mScChObj.chScCfg[chCnt];
        M2mScChObj.scChParams[chCnt].srcCropCfg =
                                        &M2mScChObj.chSrcCropCfg[chCnt];
    }

    for (reqCnt = 0; reqCnt < TOTAL_NUM_OF_REQUESTS; reqCnt++)
    {
        M2mScChObj.scProcessList[reqCnt].inFrameList[0] =
            &M2mScChObj.scInFrameList[reqCnt];
        M2mScChObj.scProcessList[reqCnt].outFrameList[0] =
            &M2mScChObj.scOutFrameList[reqCnt];
        M2mScChObj.scProcessList[reqCnt].numInLists = 1;
        M2mScChObj.scProcessList[reqCnt].numOutLists = 1;
        M2mScChObj.scProcessList[reqCnt].reserved = NULL;
        for (frmListCnt = 0u; frmListCnt < IN_NUM_FRAMELIST; frmListCnt++)
        {
            inFrameListPtr = M2mScChObj.scProcessList[reqCnt].inFrameList[0];
            inFrameListPtr->numFrames = IN_NUM_FRAMES_PER_LIST;
            inFrameListPtr->perListCfg = NULL;
            inFrameListPtr->reserved = NULL;
            for (frmCnt = 0; frmCnt < IN_NUM_FRAMES_PER_LIST; frmCnt++)
            {
                inFrameListPtr->frames[frmCnt] =
                    &M2mScChObj.scInFrames[(reqCnt * TOTAL_NUM_OF_REQUESTS) +
                                                                        frmCnt];
                inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_INT_ADDR_IDX] = M2mScChObj.scInput420Buffer
                    [(reqCnt * TOTAL_NUM_OF_REQUESTS) + frmCnt][0];
                inFrameListPtr->frames[frmCnt]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_SP_CBCR_ADDR_IDX] = M2mScChObj.scInput420Buffer
                    [(reqCnt * TOTAL_NUM_OF_REQUESTS) + frmCnt][1];
                inFrameListPtr->frames[frmCnt]->fid = FVID2_FID_FRAME;
                inFrameListPtr->frames[frmCnt]->channelNum = frmCnt;
                inFrameListPtr->frames[frmCnt]->appData = NULL;
                inFrameListPtr->frames[frmCnt]->perFrameCfg = NULL;
                inFrameListPtr->frames[frmCnt]->reserved = NULL;
                inFrameListPtr->frames[frmCnt]->subFrameInfo =
                    &M2mScChObj.scSubFrameInfo[0];

                memset(&M2mScChObj.scSubFrameInfo[0],
                       0,
                       sizeof (FVID2_SubFrameInfo));
            }

        }


        for (frmListCnt = 0u; frmListCnt < OUT_NUM_FRAMELIST; frmListCnt++)
        {
            outFrameListPtr = M2mScChObj.scProcessList[reqCnt].outFrameList[0];
            outFrameListPtr->numFrames = OUT_NUM_FRAMES_PER_LIST;
            outFrameListPtr->perListCfg = NULL;
            outFrameListPtr->reserved = NULL;
            for (frmCnt = 0; frmCnt < OUT_NUM_FRAMES_PER_LIST; frmCnt++)
            {
                outFrameListPtr->frames[frmCnt] =
                    &M2mScChObj.scOutFrames[(reqCnt * TOTAL_NUM_OF_REQUESTS) +
                                                                        frmCnt];
                outFrameListPtr->frames[frmCnt]->fid = FVID2_FID_FRAME;
                outFrameListPtr->frames[frmCnt]->channelNum = frmCnt;
                outFrameListPtr->frames[frmCnt]->appData = NULL;
                outFrameListPtr->frames[frmCnt]->perFrameCfg = NULL;
                outFrameListPtr->frames[frmCnt]->reserved = NULL;
                outFrameListPtr->frames[frmCnt]->subFrameInfo =
                                                &M2mScChObj.scSubFrameInfo[1];
                memset(&M2mScChObj.scSubFrameInfo[1],
                       0,
                       sizeof (FVID2_SubFrameInfo));
             }
        }
    }

    /* Init error process list */
    memset(&M2mScChObj.scErrProcessList, 0u,
           sizeof (M2mScChObj.scErrProcessList));
 }

/**
 * \brief scalarSetConfigParams
 *        Initialize the global channel configuraions used for scalar.
 *
 *        This function would initialize global channel config params
 *        for scalar initialization.
 *
 * \param   None
 *
 * \return  None
 */
static void scalarSetConfigParams(void)
{
    /* Configuration per each channel */
    M2mScChObj.scCreateParams.mode = VPS_M2M_CONFIG_PER_CHANNEL;
    /* Number of channels per handle */
    M2mScChObj.scCreateParams.numChannels = CHANNELS_PER_HANDLE;

    M2mScChObj.scChParams[0].inFmt.channelNum = 0;
    /* If the input is from tiled space, for the Y data  is going to
     *  8-bit container for that pitch is always 16K
     */
#ifdef SC_APP_TEST_TILER
    M2mScChObj.scChParams[0].inFmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX] =
        VPSUTILS_TILER_CNT_8BIT_PITCH;
    /* If the input is from tiled space, for the CbCr data  is going to
     *  16-bit container for that pitch is always 32K
     */
    M2mScChObj.scChParams[0].inFmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] =
        VPSUTILS_TILER_CNT_16BIT_PITCH;
#else

#endif

    M2mScChObj.scChParams[0].outFmt.channelNum = 0;
    M2mScChObj.scChParams[0].outFmt.width = OUTPUT_WIDTH;
    M2mScChObj.scChParams[0].outFmt.height = OUTPUT_HEIGHT;
    M2mScChObj.scChParams[0].outFmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
        OUTPUT_WIDTH * 2u;
    M2mScChObj.scChParams[0].outFmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    M2mScChObj.scChParams[0].outFmt.dataFormat = OUT_DATA_FORMAT;
    M2mScChObj.scChParams[0].outFmt.scanFormat = FVID2_SF_PROGRESSIVE;

    M2mScChObj.scChParams[0].srcCropCfg->cropStartY =  0;
    M2mScChObj.scChParams[0].srcCropCfg->cropStartX = 0;
    M2mScChObj.scChParams[0].srcCropCfg->cropWidth = OUTPUT_WIDTH;
    M2mScChObj.scChParams[0].srcCropCfg->cropHeight = OUTPUT_HEIGHT;

    M2mScChObj.scChParams[0].scCfg->nonLinear = SC_NON_LINEAR_SCAL;
    M2mScChObj.scChParams[0].scCfg->stripSize = SC_STRIP_SIZE;
    M2mScChObj.scChParams[0].scCfg->vsType = SC_VER_SCAL_TYPE;
    M2mScChObj.scChParams[0].scCfg->bypass = SC_BYPASS;

    M2mScChObj.scChParams[0].subFrameParams= NULL;

    /* Input is from tiled or non tiled space */
#ifdef SC_APP_TEST_TILER
    M2mScChObj.scChParams[0].inMemType = VPS_VPDMA_MT_TILEDMEM;
#else
    M2mScChObj.scChParams[0].inMemType = VPS_VPDMA_MT_NONTILEDMEM;
#endif
    M2mScChObj.scChParams[0].outMemType = VPS_VPDMA_MT_NONTILEDMEM;
    M2mScChObj.scCreateParams.chParams = M2mScChObj.scChParams;
}

/**
 * \brief scalar_Fvid2_Create
 *        Function for Mem2mem FVID2 scalar driver handle creation.
 *
 *        This function would create FVID2 handle for scalar driver and set
 *        scalar coefficient for initialization of scalar.
 *
 * \param   None
 *
 * \return  None
 */

static void scalar_Fvid2_Create(void)
{


    Int32                   retVal = FVID2_SOK;
    Vps_M2mScCreateStatus status;
    FVID2_CbParams          cbParams;

    Semaphore_Params_init(&M2mScChObj.semParams);

    M2mScChObj.semReqProcessed = Semaphore_create(0, &M2mScChObj.semParams,
                                                  NULL);
      /* Open the driver */
       cbParams.cbFxn = scalarCbFxn;
       cbParams.errCbFxn = scalarErrCbFxn;
       cbParams.errList = &M2mScChObj.scErrProcessList;
       cbParams.appData = NULL;
       cbParams.reserved = NULL;
     /* Open the driver */
     M2mScChObj.scwbHandle = FVID2_create(
                        FVID2_VPS_M2M_SC_DRV,
                        VPS_M2M_INST_SEC0_SC5_WB2,
                        &M2mScChObj.scCreateParams,
                        &status,
                        &cbParams);
    if (NULL == M2mScChObj.scwbHandle)
    {
         Vps_printf(
            "%s: Error %d @ line %d\n", __FUNCTION__, status.retVal, __LINE__);
        Vps_printf("Maximum handles supported %d\n", status.maxHandles);
        Vps_printf("Maximum channels supported %d\n",
            status.maxChannelsPerHandle);
        Vps_printf("Maximum number of same channels that can be"
            "submitted in single request %d\n",
            status.maxSameChPerRequest);
        Vps_printf("Maxumim number of requests that can be queued %d\n",
            status.maxReqInQueue);
    }
    Vps_printf("FVID2 scalar created\n");
     /* Set the coefficients.  Coefficients programming is allowed only once
    before submitting the first request */
    /* Currently doing upsampling so set coefficients for the up sampling. */
    /* Set the horizontal scaling set */
    M2mScChObj.scCoeffParams.hScalingSet  = VPS_SC_US_SET;
    /* Set the vertical scaling set */
    M2mScChObj.scCoeffParams.vScalingSet = VPS_SC_US_SET;
    /* Set to null  if user is not providing the coefficients */
    M2mScChObj.scCoeffParams.coeffPtr = NULL;
    M2mScChObj.scCoeffParams.scalarId = VPS_M2M_SC_SCALAR_ID_DEFAULT;
    retVal = FVID2_control(M2mScChObj.scwbHandle, IOCTL_VPS_SET_COEFFS,
        &M2mScChObj.scCoeffParams, NULL);
    if (0 != retVal)
    {
        Vps_printf("Setting of the coefficients failed\n");
    }
    Vps_printf("M2mScUpScale: Completed Programming Coeffs\n");
}

/**
 *  Driver callback function.
 */

 /**
 * \brief scalarCbFxn
 *        Driver callback function for scalar processing completion.
 *
 *        This function would handle the scalar processing
 *        completion event for particular a input frame.
 *
 * \param   handle   [IN] FVID2_Handle
 * \param   appData
 * \param   reserved
 *
 * \return  FVID2_SOK if successful
 */

static Int32 scalarCbFxn(FVID2_Handle handle, Ptr appData, Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    /* Request completed processing */

    Semaphore_post(M2mScChObj.semReqProcessed);

    return (retVal);
}

/**
 * \brief scalarErrCbFxn
 *        Driver callback function for scalar processing error.
 *
 *        This function would handle the scalar processing
 *        error event for particular a input frame.
 *
 * \param   handle   [IN] FVID2_Handle
 * \param   appData
 * \param   errList
 * \param   reserved
 *
 * \return  FVID2_SOK if successful
 */

static Int32 scalarErrCbFxn(FVID2_Handle handle,
                            Ptr appData,
                            Void *errList,
                            Ptr reserved)
{
    Int32           retVal = FVID2_SOK;

    Vps_printf("Error occurred!!\n");

    return (retVal);
}

/**
* Set runtime scalar params
*/
static void setScalarRtParams(Vps_FrameParams *inFrmptr)
{
    Int32 cropStartx,cropStarty;

    cropStartx = inFrmptr ? (M2mScChObj.curCfg.inFrmWidth - inFrmptr->width):
                             M2mScChObj.curCfg.cropx;
    cropStarty = inFrmptr ? (M2mScChObj.curCfg.inFrmHeight - inFrmptr->height):
                             M2mScChObj.curCfg.cropy;
    M2mScChObj.scRtParams.outFrmPrms = &M2mScChObj.scOutFrmPrms;
    M2mScChObj.scOutFrmPrms.width = M2mScChObj.curCfg.outw;
    M2mScChObj.scOutFrmPrms.height = M2mScChObj.curCfg.outh;
    M2mScChObj.scOutFrmPrms.pitch[FVID2_YUV_INT_ADDR_IDX] =
                                           M2mScChObj.curCfg.outw * 2u;

    M2mScChObj.scRtParams.inFrmPrms = &M2mScChObj.scInFrmPrms;
    M2mScChObj.scInFrmPrms.width = M2mScChObj.curCfg.inFrmWidth;
    M2mScChObj.scInFrmPrms.height = M2mScChObj.curCfg.inFrmHeight;
    if (M2mScChObj.scChParams[0].inFmt.dataFormat == FVID2_DF_YUV422I_YUYV)
    {
      M2mScChObj.scInFrmPrms.pitch[FVID2_YUV_INT_ADDR_IDX] =
                                           M2mScChObj.curCfg.inFrmWidth * 2u;
    }
    else
    {
      M2mScChObj.scInFrmPrms.pitch[FVID2_YUV_SP_Y_ADDR_IDX] =
                   (M2mScChObj.curCfg.inFrmWidth + (2 * 32) + 127) & 0xFFFFFF80;
      M2mScChObj.scInFrmPrms.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] =
                   (M2mScChObj.curCfg.inFrmWidth + (2 * 32) + 127) & 0xFFFFFF80;
    }
    M2mScChObj.scRtParams.srcCropCfg = &M2mScChObj.scSrcCropCfg;
    M2mScChObj.scSrcCropCfg.cropStartX = cropStartx;
    M2mScChObj.scSrcCropCfg.cropStartY = cropStarty;
    M2mScChObj.scSrcCropCfg.cropWidth = M2mScChObj.curCfg.inFrmWidth -
                                                                cropStartx;
    M2mScChObj.scSrcCropCfg.cropHeight = M2mScChObj.curCfg.inFrmHeight -
                                                                cropStarty;
    M2mScChObj.scRtParams.scCfg = NULL;
    M2mScChObj.scInFrmPrms.dataFormat = inFrmptr ? inFrmptr->dataFormat :
                                M2mScChObj.scChParams[0].inFmt.dataFormat;
}

/**
 * \brief initQueue
 *
 *          This function is called by fvid2queue functionality just after
 *          initializing scalar to hold the input buffers temporarily,
 *          while posting scalar o/p for display.
 *
 * \param   void   [IN] No input
 *
 * \return  VPS_SOK if successful, else suitable error code
 */

Int32  initQueue(void)
{
    Int32 qInitRtnVal   = VPS_SOK;
    M2mScChObj.inbufQHandle = VpsUtils_createQ();
    if (NULL == M2mScChObj.inbufQHandle)
    {
        GT_0trace(PSTrace, GT_ERR, "Queue creation failed!!\n");
        qInitRtnVal  =    VPS_EFAIL;
    }
    GT_0trace(PSTrace, GT_LEAVE, "PS: <<<<<VPS_PSrvInit\n");
    return (qInitRtnVal);
}

/* ========================================================================== */
/*                             Local Functions                                */
/* ========================================================================== */


/**
 * \brief psCallFvid2Api
 *        This function will be called to convert an Proxy Server command to an
 *        FVID2 call and make FVID2 call.
 *
 *        This primary requirement of this function is to be re-entrant. This
 *        function could be called by any of the active Proxy Server tasks.
 *
 * \pre This function expects that simplexCommand points to one of the simplex
 *      command. This function cannot handle composite commands.
 *
 * \param   simplexCommand    [IN]  Pointer to a Proxy Server command.
 * \param   pSrvTsk           [IN]  Pointer to task descriptor.
 * \param   procId            [IN]  Id of the processor that command was
 *                                      received on.
 * \param   pSrvTsk           [IN]  Event number on which the command was
 *                                      received.
 *
 * \return  None
 */
Int32 psScale(VPS_PSrvFvid2QueueParams *qParams)
{
    Int32   fivd2RtnValue   =   VPS_EFAIL;
    Int32 retVal = 0;
    static FVID2_FrameList     *InFrmListPtr;
    static FVID2_FrameList     *OutFrmListPtr;
    UInt8 *inputBuffPtr;
    Vps_DispRtParams *dispRtPrms;
    static UInt8 * outBuffer;
    Vps_FrameParams *inFrmptr;

    GT_0trace(PSTrace, GT_ENTER, "PS: >>>>psCallFvid2Api\n");
    if (qParams == NULL)
    {
        /* Breaking the multiple return RULE, in this function there are 2
           returns, one here and other at the last of this function
           Another level of indentation was not justified to meet this rule */
        GT_0trace(PSTrace, GT_INFO, "PS : Received an NULL command\n");
        return fivd2RtnValue;
    }

    /*
     * Following steps are performed
     * 1. Scaling and chroma upsampling is done here
    */
    GT_0trace(PSTrace, GT_INFO, "PS : FVID2 - Queuing up\n");
    /* Scalar Path is used to perform 420 to 422 Conversion & scale*/
    dispRtPrms = (Vps_DispRtParams *)qParams->frameList->perListCfg;
    if ( qParams->fvid2Handle != M2mScChObj.grpxFvid2Handle)
    {
        if(M2mScChObj.curCfg.scEnable)
        {
            if(dispRtPrms != NULL)
            {
                inFrmptr = (Vps_FrameParams *)dispRtPrms->inFrmPrms;
                if(M2mScChObj.curCfg.scCount == 0)
                {
                    GT_0trace(PSTrace, GT_INFO, "SETTING scRtParams \n");
                    initScalar();
                    retVal = initQueue();
                    if (VPS_SOK != retVal)
                    {
                        fivd2RtnValue = VPS_EFAIL;
                        return (fivd2RtnValue);
                    }
                    M2mScChObj.curCfg.scCount ++;
                }
                InFrmListPtr = M2mScChObj.scProcessList[0].inFrameList[0];
                OutFrmListPtr = M2mScChObj.scProcessList[0].outFrameList[0];
                setScalarRtParams(inFrmptr);
                InFrmListPtr->frames[0]->perFrameCfg = &M2mScChObj.scRtParams;
                if(inFrmptr != NULL){
                    inFrmptr->pitch[FVID2_YUV_INT_ADDR_IDX] =
                                          M2mScChObj.curCfg.outw * 2u;
                    inFrmptr->width = M2mScChObj.curCfg.outw;
                    inFrmptr->height = M2mScChObj.curCfg.outh;
                }
            }
            outBuffer = (UInt8 *)qParams->frameList->frames[0]->addr[0][2];
            OutFrmListPtr->frames[0]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_INT_ADDR_IDX] = (UInt8 *) outBuffer;
            inputBuffPtr = (UInt8 *)qParams->frameList->frames[0]->addr[0][0];

            if (M2mScChObj.scChParams[0].inFmt.dataFormat !=
                                               FVID2_DF_YUV422I_YUYV)
            {
                /*
                 * Start address of the active region in the Decoder
                 * Output buffer for Luma and Chroma components
                 */
                M2mScChObj.scInput420Buffer[0][0] =
                            (UInt8 *)qParams->frameList->frames[0]->addr[0][0];
                M2mScChObj.scInput420Buffer[0][1] =
                            (UInt8 *)qParams->frameList->frames[0]->addr[0][1];

                /* Assign Y buffer */
                InFrmListPtr->frames[0]->addr[FVID2_FRAME_ADDR_IDX]
                [FVID2_YUV_INT_ADDR_IDX] = M2mScChObj.scInput420Buffer[0][0];
                /* Assign CbCr buffer */
                InFrmListPtr->frames[0]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_SP_CBCR_ADDR_IDX] =
                        M2mScChObj.scInput420Buffer[0][1];
            }
            else
            {
                InFrmListPtr->frames[0]->addr[FVID2_FRAME_ADDR_IDX]
                    [FVID2_YUV_INT_ADDR_IDX] =
                    ( unsigned char *) (inputBuffPtr);
            }
            /* FRAME based processing */
            /* Enables the scaling and chroma conversion */
            retVal = FVID2_processFrames(M2mScChObj.scwbHandle,
                                        &M2mScChObj.scProcessList[0]);
            if (0 != retVal)
            {
                 Vps_printf("Processing the frame failed\n\n");
            }
            Vps_printf("Processing of the frame done\n\n");
            /* Wait till frames gets processed */
            Semaphore_pend(M2mScChObj.semReqProcessed, BIOS_WAIT_FOREVER);
            /* Clear the processlist before getting it from driver */
            memset(&M2mScChObj.scProcessList[0], 0,
                   sizeof(M2mScChObj.scProcessList[0]));

            /* Get the processed request */
            fivd2RtnValue = FVID2_getProcessedFrames(M2mScChObj.scwbHandle,
                            &M2mScChObj.scProcessList[0], 0);
            if (0 != fivd2RtnValue)
            {
                Vps_printf("Scalar:Getting processed frames Failed\n\n");
            }

            OutFrmListPtr =M2mScChObj.scProcessList[0].outFrameList[0];

            /* Queue the queue object to input buffer Q */
            VpsUtils_queue(M2mScChObj.inbufQHandle, &M2mScChObj.qElem,
                           qParams->frameList->frames[0]->addr[0][0]);
            qParams->frameList->frames[0]->addr[0][0] =
                           OutFrmListPtr->frames[0]->addr[0][0];
        }
        else
        {
           fivd2RtnValue = VPS_SOK;
        }
    }
    else
    {
        fivd2RtnValue = VPS_SOK;
    }

    GT_1trace(PSTrace, GT_INFO, "PS : [Done] - rtn val %d\n", fivd2RtnValue);
    return (fivd2RtnValue);

}

void psScalarDequeue(VPS_PSrvFvid2DequeueParams *dQParams)
{
    void * queuePtr;

    GT_0trace(PSTrace, GT_INFO, "PS : psScalarDequeue - De-Queuing\n");
  if(M2mScChObj.curCfg.scEnable){
    queuePtr = VpsUtils_dequeue(M2mScChObj.inbufQHandle);
    if(queuePtr != NULL)
    {
        dQParams->frameList->frames[0]->addr[0][0] = queuePtr;
    }
    dQParams->frameList->perListCfg = NULL;
   }
   GT_0trace(PSTrace, GT_INFO, "PS : [Done] \n");
}

void psScalarControl( VPS_PSrvFvid2ControlParams *ctrlParams)
{
    FVID2_Format * setFmt;
    Vps_DeiDispParams *deiScFmt;
    Int32 pitch;
    GT_0trace(PSTrace,GT_INFO,"PS : FVID2 - Control\n");

     if(ctrlParams->cmd == IOCTL_VPS_SET_GRPX_PARAMS)
    {
        M2mScChObj.grpxFvid2Handle = ctrlParams->fvid2Handle;
    }
    else if(ctrlParams->cmd == FVID2_SET_FORMAT)
    {
        setFmt = (FVID2_Format *)(ctrlParams->cmdArgs);
        if ((setFmt->dataFormat == FVID2_DF_YUV422I_YUYV) ||
            (setFmt->dataFormat == FVID2_DF_YUV420SP_UV))
        {
            M2mScChObj.scChParams[0].inFmt.channelNum = setFmt->channelNum;
            M2mScChObj.scChParams[0].inFmt.dataFormat = setFmt->dataFormat;
            M2mScChObj.scChParams[0].inFmt.scanFormat = setFmt->scanFormat;

            if (M2mScChObj.scChParams[0].inFmt.dataFormat ==
                                               FVID2_DF_YUV422I_YUYV)
            {
                M2mScChObj.scChParams[0].inFmt.fieldMerged
                                  [FVID2_YUV_INT_ADDR_IDX] = FALSE;
            }
            else
            {
                M2mScChObj.scChParams[0].inFmt.fieldMerged
                                              [FVID2_YUV_SP_Y_ADDR_IDX] = FALSE;
                M2mScChObj.scChParams[0].inFmt.fieldMerged
                                           [FVID2_YUV_SP_CBCR_ADDR_IDX] = FALSE;
            }
            setFmt->dataFormat = FVID2_DF_YUV422I_YUYV;
            pitch = setFmt->width * 2u;
            if (pitch & 0xF)
            {
                pitch += 16 - (pitch & 0xF);
            }
            setFmt->pitch[FVID2_YUV_INT_ADDR_IDX] = pitch;
        }
    }
    else if(ctrlParams->cmd == IOCTL_VPS_DEI_DISP_SET_PARAMS)
    {

        deiScFmt = (Vps_DeiDispParams *)(ctrlParams->cmdArgs);
        M2mScChObj.curCfg.scEnable =  !deiScFmt->scCfg.bypass;
        M2mScChObj.curCfg.outw = deiScFmt->scTarWidth;
        M2mScChObj.curCfg.outh = deiScFmt->scTarHeight;
        if (NULL != deiScFmt->deiScCropCfg)
        {
            M2mScChObj.curCfg.cropx = deiScFmt->deiScCropCfg->cropStartX;
            M2mScChObj.curCfg.cropy = deiScFmt->deiScCropCfg->cropStartY;
        }
        else
        {
            M2mScChObj.curCfg.cropx = 0u;
            M2mScChObj.curCfg.cropy = 0u;
        }
        M2mScChObj.curCfg.inFrmWidth = deiScFmt->fmt.width;
        M2mScChObj.curCfg.inFrmHeight =deiScFmt->fmt.height;
    }

    GT_0trace(PSTrace, GT_INFO, "PS : [Done] \n");
}

Int32 psScalarDelete(VPS_PSrvFvid2DeleteParams *delParams)
{
    /*
     * execute scalar delete reset variables
     * Update the return value
     */
    Int32   fivd2RtnValue   =   VPS_EBADARGS;
    GT_0trace(PSTrace,GT_INFO,"PS : Deleting FVID2 stream\n");

    if (delParams->fvid2Handle != M2mScChObj.grpxFvid2Handle)
    {
        if(M2mScChObj.curCfg.scEnable)
        {
            fivd2RtnValue = FVID2_delete(M2mScChObj.scwbHandle, NULL);

            if (fivd2RtnValue == FVID2_SOK)
            {
                GT_0trace(PSTrace, GT_INFO,"SUCCESS::Scalar handle deleted");
            }
            Semaphore_delete(&M2mScChObj.semReqProcessed);
        }
        M2mScChObj.curCfg.scEnable = 0;
    }

    if (delParams->fvid2Handle != M2mScChObj.grpxFvid2Handle)
    {
        M2mScChObj.curCfg.scCount = 0;
        if (NULL != M2mScChObj.inbufQHandle)
        {
           M2mScChObj.inbufQHandle = NULL;
        }
    }
    if (delParams->fvid2Handle == M2mScChObj.grpxFvid2Handle)
    {
        M2mScChObj.grpxFvid2Handle = NULL;
        fivd2RtnValue = FVID2_SOK;
    }
    GT_0trace(PSTrace, GT_INFO, "PS :Scalar handle delete [Done] \n");
    return (fivd2RtnValue);
}
