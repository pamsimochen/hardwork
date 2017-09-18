/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/drivers/capture/src/vpsdrv_capturePriv.h>
#include <ti/psp/vps/common/vps_evtMgr.h>

/**< Value to indicate that subframe object is FREE and can be allocated */
#define VPS_CAPT_SUBFRAME_FREE  (FALSE)
/**< Value to indicate that subframe object is allocated */
#define VPS_CAPT_SUBFRAME_ALLOC (0xFFFFFFFF)

#define VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
#define VPS_CAPT_SUBFRAME_CHECK_REQFLOW

/**< Q objects that would used to track the current frame under reception */
Vps_CaptSubFrameQObj gVps_captSubFrameQObj[VPS_CAPT_STREAM_ID_MAX][VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX];
/**< Index indicating the next free object in the array */
UInt32 gVps_captSubFrameQIndex = 0x0;

#ifdef VPS_CAPTURE_TS_DEBUG
extern UInt32 gVps_CaptResetTsStats;
#endif /* VPS_CAPTURE_TS_DEBUG */

#ifdef VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
typedef struct {
    UInt32 eofCounter;
    UInt32 xLineCounter;
    UInt32 fidMismatch;
    UInt32 maxHeight;
    UInt32 minHeight;
    UInt32 maxWidth;
    UInt32 minWidth;
    UInt32 shortFrames;
    UInt32 minSubFrames;
    UInt32 maxSubFrames;
    UInt32 shortFrameTrigFlag;
} VPS_CaptSubFrameDebugCounters;
/**< Index indicating the next free object in the array */
VPS_CaptSubFrameDebugCounters gCaptSubFrameDebug;
#endif /* VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS */


/* Forward Declaration */
Void Vps_captVipSubFrameXlinePIsr ( const UInt32 *event,
                                    UInt32 numEvents,
                                    Ptr arg);
Void Vps_captVipSubFrameEOFIsr(const UInt32 *event,
                                  UInt32 numEvents,
                                  Ptr arg);
#ifdef VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL
Void Vps_captVipSubFrameXlineSIsr ( const UInt32 *event,
                                    UInt32 numEvents,
                                   Ptr arg);
#endif /* VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL */

/* Local Functions */
static Vps_CaptSubFrameQObj * Vps_captVipSubFrameAllocQObj ( UInt32 streamId );
static Int32 Vps_captVipSubFrameFreeQObj ( Vps_CaptSubFrameQObj *obj );

/*
  Make VIP parser config based on create parameters

  The HW register are not updated in this API
  only config structure is initialized.
*/
Int32 Vps_captVipParserConfigSetup ( Vps_CaptObj * pObj,
                                     VpsHal_VipConfig * vipInstConfig,
                                     VpsHal_VipPortConfig * vipPortConfig )
{
    Int32 status = FVID2_SOK;
    Vps_VipConfig *pUserInstConfig;
    Vps_VipPortConfig *pUserPortConfig;

    /*
     * setup VIP parser parameters
     */
    vipInstConfig->vipConfig.clipActive = FALSE;
    vipInstConfig->vipConfig.clipBlank = FALSE;

    vipPortConfig->ancChSel8b = VPS_VIP_ANC_CH_SEL_8B_LUMA_SIDE;
    vipPortConfig->pixClkEdgePol = VPS_VIP_PIX_CLK_EDGE_POL_RISING;
    vipPortConfig->disConfig.fidSkewPostCnt = 0;
    vipPortConfig->disConfig.fidSkewPreCnt = 0;
    vipPortConfig->disConfig.lineCaptureStyle =
        VPS_VIP_LINE_CAPTURE_STYLE_ACTVID;
    if ((pObj->createArgs.videoCaptureMode ==
        VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VBLK) ||
        (pObj->createArgs.videoCaptureMode ==
        VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC))
    {
        vipPortConfig->disConfig.lineCaptureStyle =
            VPS_VIP_LINE_CAPTURE_STYLE_HSYNC;
    }
    vipPortConfig->disConfig.fidDetectMode = VPS_VIP_FID_DETECT_MODE_PIN;
    vipPortConfig->disConfig.actvidPol = VPS_VIP_POLARITY_HIGH;
    vipPortConfig->disConfig.vsyncPol = VPS_VIP_POLARITY_LOW;
    vipPortConfig->disConfig.hsyncPol = VPS_VIP_POLARITY_LOW;

    vipPortConfig->embConfig.srcNumPos
        = VPS_VIP_SRC_NUM_POS_LS_NIBBLE_OF_CODEWORD;

    vipPortConfig->embConfig.isMaxChan3Bits = FALSE;

    vipPortConfig->invertFidPol = FALSE;
    vipPortConfig->enablePort = FALSE;
    vipPortConfig->clrAsyncFifoRd = FALSE;
    vipPortConfig->clrAsyncFifoWr = FALSE;

    vipPortConfig->ctrlChanSel = VPS_VIP_CTRL_CHAN_SEL_7_0;

    /*
     * setup 8/16/24 interface based on user parameters
     */
    if ( pObj->createArgs.videoIfMode == VPS_CAPT_VIDEO_IF_MODE_8BIT )
    {
        vipInstConfig->intfMode = VPSHAL_VIP_INTF_MODE_8B;
    }
    else if ( pObj->createArgs.videoIfMode == VPS_CAPT_VIDEO_IF_MODE_16BIT )
    {
        vipInstConfig->intfMode = VPSHAL_VIP_INTF_MODE_16B;
        vipPortConfig->ctrlChanSel = VPS_VIP_CTRL_CHAN_SEL_15_8;

        /*
         * control channel needs to be Luma in sim for 16-bit interface
         */
        vipPortConfig->ctrlChanSel = VPS_VIP_CTRL_CHAN_SEL_15_8;
    }
    else if ( pObj->createArgs.videoIfMode == VPS_CAPT_VIDEO_IF_MODE_24BIT )
    {
        vipInstConfig->intfMode = VPSHAL_VIP_INTF_MODE_24B;
    }
    else
    {
        vipInstConfig->intfMode = VPSHAL_VIP_INTF_MODE_8B;
    }

    vipPortConfig->embConfig.errCorrEnable = FALSE;
    vipPortConfig->syncType = VPSHAL_VIP_SYNC_TYPE_EMB_SINGLE_422_YUV;

    /*
     * based on capture mode setup VIP parser syncType
     */
    switch ( pObj->createArgs.videoCaptureMode )
    {
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC:

            if ( pObj->createArgs.inDataFormat == FVID2_DF_YUV422P )
                vipPortConfig->syncType =
                    VPSHAL_VIP_SYNC_TYPE_EMB_SINGLE_422_YUV;
            else
                vipPortConfig->syncType =
                    VPSHAL_VIP_SYNC_TYPE_EMB_SINGLE_RGB_OR_444_YUV;

            break;

        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_SPLIT_LINE_EMBEDDED_SYNC:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC:

            if ( pObj->createArgs.inDataFormat == FVID2_DF_YUV422P )
            {
                vipPortConfig->syncType = VPSHAL_VIP_SYNC_TYPE_EMB_LINE_YUV;
            }
            break;

        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC:

            if ( pObj->createArgs.inDataFormat == FVID2_DF_YUV422P )
            {
                if ( pObj->createArgs.numCh == 2 )
                    vipPortConfig->syncType =
                        VPSHAL_VIP_SYNC_TYPE_EMB_2X_422_YUV;
                else
                    vipPortConfig->syncType =
                        VPSHAL_VIP_SYNC_TYPE_EMB_4X_422_YUV;
            }
            break;

        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VBLK:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK:
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC:

            if ( pObj->createArgs.videoIfMode == VPS_CAPT_VIDEO_IF_MODE_24BIT )
            {
                vipPortConfig->syncType =
                    VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_24B_RGB;
            }
            else
            {
                vipPortConfig->syncType =
                    VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_YUV;
            }

            vipPortConfig->discreteBasicMode = FALSE;
            if ((pObj->createArgs.videoCaptureMode ==
                VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_HSYNC_VSYNC) ||
                (pObj->createArgs.videoCaptureMode ==
                VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC))
            {
                vipPortConfig->discreteBasicMode = TRUE;
            }

#if 0
            if ( pObj->createArgs.inDataFormat == FVID2_DF_YUV422P )
            {
                vipPortConfig->syncType =
                    VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_422_YUV;
            }
            else
            {

                if ( pObj->createArgs.videoIfMode ==
                     VPS_CAPT_VIDEO_IF_MODE_24BIT )
                    vipPortConfig->syncType =
                        VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_24B_RGB;
                else
                    vipPortConfig->syncType =
                        VPSHAL_VIP_SYNC_TYPE_DIS_SINGLE_8B_RGB;
            }
#endif
            break;
    }

    /*
     * over ride with user supplied extended params if required
     */
    pUserPortConfig = pObj->createArgs.vipParserPortConfig;
    pUserInstConfig = pObj->createArgs.vipParserInstConfig;

    if ( pUserInstConfig != NULL )
    {
        if ( pUserInstConfig->clipActive !=
             ( UInt32 ) VPS_VIP_VALUE_DONT_CARE )
        {
            vipInstConfig->vipConfig.clipActive =
                pUserInstConfig->clipActive;
        }
        if ( pUserInstConfig->clipBlank !=
             ( UInt32 ) VPS_VIP_VALUE_DONT_CARE )
        {
            vipInstConfig->vipConfig.clipBlank = pUserInstConfig->clipBlank;
        }
    }
    if ( pUserPortConfig != NULL )
    {
        if ( pUserPortConfig->ctrlChanSel
             != ( UInt32 ) VPS_VIP_CTRL_CHAN_DONT_CARE )
        {
            vipPortConfig->ctrlChanSel = pUserPortConfig->ctrlChanSel;
        }
        if ( pUserPortConfig->ancChSel8b
             != ( UInt32 ) VPS_VIP_ANC_CH_SEL_DONT_CARE )
        {
            vipPortConfig->ancChSel8b = pUserPortConfig->ancChSel8b;
        }
        if ( pUserPortConfig->pixClkEdgePol !=
             ( UInt32 ) VPS_VIP_PIX_CLK_EDGE_POL_DONT_CARE )
        {
            vipPortConfig->pixClkEdgePol = pUserPortConfig->pixClkEdgePol;
        }
        if ( pUserPortConfig->invertFidPol !=
             ( UInt32 ) VPS_VIP_VALUE_DONT_CARE )
        {
            vipPortConfig->invertFidPol = pUserPortConfig->invertFidPol;
        }

        if ( pUserPortConfig->embConfig.errCorrEnable
             != ( UInt32 ) VPS_VIP_VALUE_DONT_CARE )
        {
            vipPortConfig->embConfig.errCorrEnable
                = pUserPortConfig->embConfig.errCorrEnable;
        }
        if ( pUserPortConfig->embConfig.srcNumPos !=
             ( UInt32 ) VPS_VIP_SRC_NUM_POS_DONT_CARE )
        {
            vipPortConfig->embConfig.srcNumPos
                = pUserPortConfig->embConfig.srcNumPos;
        }
        if ( pUserPortConfig->embConfig.isMaxChan3Bits !=
             ( UInt32 ) VPS_VIP_VALUE_DONT_CARE )
        {
            vipPortConfig->embConfig.isMaxChan3Bits
                = pUserPortConfig->embConfig.isMaxChan3Bits;
        }

        if ( pUserPortConfig->disConfig.fidSkewPostCnt !=
             ( UInt32 ) VPS_VIP_VALUE_DONT_CARE )
        {
            vipPortConfig->disConfig.fidSkewPostCnt
                = pUserPortConfig->disConfig.fidSkewPostCnt;
        }
        if ( pUserPortConfig->disConfig.fidSkewPreCnt !=
             ( UInt32 ) VPS_VIP_VALUE_DONT_CARE )
        {
            vipPortConfig->disConfig.fidSkewPreCnt
                = pUserPortConfig->disConfig.fidSkewPreCnt;
        }
        if ( pUserPortConfig->disConfig.lineCaptureStyle
             != ( UInt32 )  VPS_VIP_LINE_CAPTURE_STYLE_DONT_CARE )
        {
            vipPortConfig->disConfig.lineCaptureStyle
                = pUserPortConfig->disConfig.lineCaptureStyle;
        }
        if ( pUserPortConfig->disConfig.fidDetectMode !=
             ( UInt32 ) VPS_VIP_FID_DETECT_MODE_DONT_CARE )
        {
            vipPortConfig->disConfig.fidDetectMode
                = pUserPortConfig->disConfig.fidDetectMode;
        }
        if ( pUserPortConfig->disConfig.actvidPol !=
             ( UInt32 ) VPS_VIP_POLARITY_DONT_CARE )
        {
            vipPortConfig->disConfig.actvidPol
                = pUserPortConfig->disConfig.actvidPol;
        }
        if ( pUserPortConfig->disConfig.vsyncPol !=
             ( UInt32 ) VPS_VIP_POLARITY_DONT_CARE )
        {
            vipPortConfig->disConfig.vsyncPol
                = pUserPortConfig->disConfig.vsyncPol;
        }
        if ( pUserPortConfig->disConfig.hsyncPol !=
             ( UInt32 ) VPS_VIP_POLARITY_DONT_CARE )
        {
            vipPortConfig->disConfig.hsyncPol
                = pUserPortConfig->disConfig.hsyncPol;
        }
    }

    return status;
}

/*
  Make scaler config based on create parameters

  The HW register are not updated in this API
  only config structure is initialized.
*/
Int32 Vps_captVipScConfigSetup ( Vps_CaptObj * pObj,
                                 VpsHal_ScConfig * scConfig,
                                 Vps_ScCoeffParams * scCoeffConfig )
{
    Int32 status = FVID2_SOK;
    Vps_CaptScParams *pScParams;
    Vps_ScConfig *pUserConfig;
    Vps_ScCoeffParams *pUserCoeffConfig;

    pScParams = &pObj->createArgs.scParams;

    if ((pScParams->outWidth > pScParams->inCropCfg.cropWidth) ||
        (pScParams->outHeight > pScParams->inCropCfg.cropHeight))
    {
        status = FVID2_EFAIL;
    }

    scConfig->tarWidth = pScParams->outWidth;
    scConfig->tarHeight = pScParams->outHeight;
    scConfig->srcWidth = pScParams->inWidth;
    scConfig->srcHeight = pScParams->inHeight;
    scConfig->cropStartX = pScParams->inCropCfg.cropStartX;
    scConfig->cropStartY = pScParams->inCropCfg.cropStartY;
    scConfig->cropWidth = pScParams->inCropCfg.cropWidth;
    scConfig->cropHeight = pScParams->inCropCfg.cropHeight;
    scConfig->inFrameMode
        = ( Vps_ScanFormat )
            FVID2_SF_PROGRESSIVE;
    scConfig->outFrameMode
        = ( Vps_ScanFormat )
            FVID2_SF_PROGRESSIVE;
    scConfig->hsType = VPS_SC_HST_AUTO;
    scConfig->nonLinear = FALSE;
    scConfig->stripSize = 0;
    scConfig->vsType = VPS_SC_VST_POLYPHASE;
    scConfig->hsType = VPS_SC_HST_AUTO;
    scConfig->fidPol = VPS_FIDPOL_NORMAL;
    scConfig->selfGenFid = VPS_SC_SELF_GEN_FID_DISABLE;
    scConfig->defConfFactor = 0;
    scConfig->biLinIntpType = VPS_SC_BINTP_ORIGINAL;
    scConfig->enableEdgeDetect = FALSE;
    scConfig->hPolyBypass = TRUE;
    /* For downscaling enable pre-peaking filter */
    if (scConfig->tarWidth < scConfig->cropWidth)
    {
        scConfig->enablePeaking = TRUE;
    }
    else
    {
        scConfig->enablePeaking = FALSE;
    }
    scConfig->phInfoMode = VPS_SC_SET_PHASE_INFO_DEFAULT;
    scConfig->bypass = FALSE;
    scConfig->arg = NULL;

    #if 0 // if user has said enable SC then always enable, sicne for YUV420 output user may always want SC
    /*
     * bypass scaler if not required
     */
    if ( pScParams->outWidth    /* input = output and no crop */
         == pScParams->inWidth
        &&
        pScParams->inCropCfg.cropWidth
            == pScParams->inWidth
        &&
        pScParams->outHeight
         == pScParams->inHeight
        &&
        pScParams->inCropCfg.cropHeight
            == pScParams->inHeight
       )
    {
        scConfig->bypass = TRUE;
    }
    #endif

    /*
     * Check if user has provided SC coeff config
     */
    pUserCoeffConfig = pObj->createArgs.scParams.scCoeffConfig;
    if (NULL != pUserCoeffConfig)
    {
        /* Use user-provided values. */
        scCoeffConfig->hScalingSet = pUserCoeffConfig->hScalingSet;
        scCoeffConfig->vScalingSet = pUserCoeffConfig->vScalingSet;
        scCoeffConfig->coeffPtr = pUserCoeffConfig->coeffPtr;
        scCoeffConfig->scalarId = pUserCoeffConfig->scalarId;
    }
    else
    {
        /* Fill with defaults. */
        scCoeffConfig->hScalingSet = VPS_SC_DS_SET_8_16;
        scCoeffConfig->vScalingSet = VPS_SC_DS_SET_8_16;
        scCoeffConfig->coeffPtr = NULL;
        scCoeffConfig->scalarId = VPS_CAPT_SCALAR_ID_DEFAULT;
    }

    /*
     * over ride with user supplied extended params if required
     */
    pUserConfig = pObj->createArgs.scParams.scConfig;

    if ( pUserConfig != NULL )
    {
        scConfig->bypass = pUserConfig->bypass;
        scConfig->nonLinear = pUserConfig->nonLinear;
        scConfig->stripSize = pUserConfig->stripSize;
        scConfig->vsType = pUserConfig->vsType;
        scConfig->enablePeaking = pUserConfig->enablePeaking;
        scConfig->enableEdgeDetect = pUserConfig->enableEdgeDetect;
    }

    return status;
}

/*
  Setup path in VIP HW

  Setup all modules in the path
  Setup all muxes to enable the path

  The VIP port itself is not started at this point
*/
Int32 Vps_captCreateVip ( Vps_CaptObj * pObj )
{
    Bool scEnable = FALSE;
    VpsHal_ScConfig scConfig;
    VpsHal_VipConfig vipInstConfig;
    VpsHal_VipPortConfig vipPortConfig;
    Vps_CscConfig *pUserCscConfig;
    Vps_ScCoeffParams scCoeffConfig;

    Int32 status = FVID2_SOK;
    UInt32 streamId;

    /*
     * disable port, just in case it is enabled
     */
    Vcore_vipEnablePort ( &pObj->resObj, FALSE );

    /*
     * setup HW modules
     */

    /*
     * setup VIP parser
     */
    Vps_captVipParserConfigSetup ( pObj, &vipInstConfig, &vipPortConfig );

    /*
     * setup scaler if required
     */
    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        if ( pObj->createArgs.outStreamInfo[streamId].scEnable )
        {
            scEnable = TRUE;
            status = Vps_captVipScConfigSetup ( pObj, &scConfig,
                                       &scCoeffConfig );
            /* Needs to be done only once for this instance. */
            break;
        }
    }

    if (FVID2_SOK == status)
    {
        /*
         * override with user params if set by user
         */
        pUserCscConfig = pObj->createArgs.cscConfig;

        /*
         * setup CSC, internally the API does no setting if CSC is not in this path
         */
        Vcore_vipSetCscParams ( &pObj->resObj, pUserCscConfig );

        if (TRUE == scEnable)
        {
            /*
             * setup SC.
             */
            Vcore_vipSetScParams(&pObj->resObj, &scConfig);

            /* Setup the current scaling factor information. */
            pObj->curScFactor.hScalingSet = scCoeffConfig.hScalingSet;
            pObj->curScFactor.vScalingSet = scCoeffConfig.vScalingSet;
            pObj->curScFactor.hsType = scConfig.hsType;
            pObj->curScFactor.vsType = scConfig.vsType;

            /* If scaler is not in bypass and if user has not provided specific
             * scaler coefficients, then calculate the best scaling factor
             * internally. If scaler is in bypass, use the default coefficient
             * sets configured. */
            if ((FALSE == scConfig.bypass) &&
                (NULL == pObj->createArgs.scParams.scCoeffConfig))
            {
                /*
                 * Get current scaling factor
                 */
                Vcore_vipGetAndUpdScFactorConfig(&pObj->resObj,
                    &scConfig,
                    &pObj->curScFactor);

                scCoeffConfig.hScalingSet = pObj->curScFactor.hScalingSet;
                scCoeffConfig.vScalingSet = pObj->curScFactor.vScalingSet;
            }

            /* Update the scaler config. */
            Vcore_vipSetScCoeff(&pObj->resObj,
                &scCoeffConfig,
                Vps_captGetScCoeffMem(pObj->vipInstId));
        }

        /*
         * setup VIP parser, internally the API
         * sets up the correct VIP for this path
         */
        Vcore_vipSetParserParams ( &pObj->resObj, &vipInstConfig, &vipPortConfig );

        /*
         * setup muxes for this path
         */
        Vcore_vipSetMux ( &pObj->resObj, NULL );

        /*
         * set frame start events for associated VPDMA channels
         */
        Vps_captSetFrameStartEvent ( pObj );
    }

    return status;
}

/*
  Set frame start event as channel active for all associated VPDMA channels
*/
Int32 Vps_captSetFrameStartEvent ( Vps_CaptObj * pObj )
{
    UInt16 chId, streamId, vChId;
    Vps_CaptChObj *pChObj;

    /*
     * for all channels in the driver instance
     */
    for ( chId = 0; chId < pObj->numCh; chId++ )
    {
        /*
         * for all streams
         */
        for ( streamId = 0; streamId < pObj->numStream; streamId++ )
        {

            pChObj = &pObj->chObj[streamId][chId];

            /*
             * for associated VPDMA channels
             */
            for ( vChId = 0; vChId < pChObj->vChannelNum; vChId++ )
            {
                /*
                 * set frame start event as channel active
                 */
                VpsHal_vpdmaSetFrameStartEvent ( pChObj->vChannelId[vChId], VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE, VPSHAL_VPDMA_LM_0, /* NOT USED  */
                                                 0, NULL, 0u );
            }
        }
    }

    return FVID2_SOK;
}

/*
  Start VIP HW port
*/
Int32 Vps_captStartVip ( Vps_CaptObj * pObj )
{
    /*
     * enable port
     */
    Vcore_vipEnablePort ( &pObj->resObj, TRUE );

    return FVID2_SOK;
}

/*
  Stop VIP HW port
*/
Int32 Vps_captStopVip ( Vps_CaptObj * pObj )
{
    /*
     * disable port
     */
    Vcore_vipEnablePort ( &pObj->resObj, FALSE );

    return FVID2_SOK;
}

/*
  Delete VIP
*/
Int32 Vps_captDeleteVip ( Vps_CaptObj * pObj )
{
    /*
     * stop VIP HW port
     */
    Vps_captStopVip ( pObj );

    return FVID2_SOK;
}

/*
  Set scaler frame info
*/
Int32 Vps_captVipSetScParams(Vps_CaptObj *pObj,
                             Vps_CaptScParams *scParams)
{
    Int32 status = FVID2_EFAIL, outId;
    Ptr userCoeffPtr = NULL;
    VpsHal_ScConfig scConfig;
    Vps_ScCoeffParams scCoeffConfig;
    VpsHal_ScFactorConfig scFactorConfig;
    Vps_CaptOutInfo *outInfo;
    UInt32 cookie;

    /*
     * parameter checking
     */
    if ((pObj == (Vps_CaptObj *)VPS_CAPT_INST_VIP_ALL) ||
        (pObj == NULL) ||
        (scParams == NULL))
    {
        return FVID2_EFAIL;
    }

    /*
     * for all capture stream
     */
    for ( outId = 0; outId < pObj->numStream; outId++ )
    {

        outInfo = &pObj->createArgs.outStreamInfo[outId];

        /*
         * if scaler is enabled
         */
        if ( outInfo->scEnable )
        {
            /*
             * copy current scaler frame info to driver frame info
             */
            pObj->createArgs.scParams = *scParams;

            /*
             * make scaler config
             */
            status = Vps_captVipScConfigSetup (pObj, &scConfig, &scCoeffConfig);

            if (FVID2_SOK == status)
            {
                /*
                 * disable interrupt
                 */
                cookie = Hwi_disable (  );

                /*
                 * set scaler HW
                 */
                Vcore_vipSetScParams ( &pObj->resObj, &scConfig );

                /*
                 * restore interrupt
                 */
                Hwi_restore ( cookie );

                /* The scaler coeff load must be done only if scaler is not in
                 * bypass and enableCoeffLoad is TRUE. */
                if ((FALSE == scConfig.bypass) &&
                    (TRUE == scParams->enableCoeffLoad))
                {
                    /* If user has not provided specific scaler coefficients,
                     * then calculate the best scaling factor internally. */
                    if (NULL == pObj->createArgs.scParams.scCoeffConfig)
                    {
                        /*
                         * Get new scaling factor
                         */
                        Vcore_vipGetAndUpdScFactorConfig (&pObj->resObj,
                            &scConfig,
                            &scFactorConfig);
                    }
                    else
                    {
                        if (NULL != scCoeffConfig.coeffPtr)
                        {
                            userCoeffPtr = scCoeffConfig.coeffPtr;
                        }
                        /* Otherwise set scFactorConfig to user-provided scaler
                         * coeff config. */
                        scFactorConfig.hScalingSet = scCoeffConfig.hScalingSet;
                        scFactorConfig.vScalingSet = scCoeffConfig.vScalingSet;
                        scFactorConfig.hsType = scConfig.hsType;
                        scFactorConfig.vsType = scConfig.vsType;
                    }

                    /* If the user has provided coefficient pointer, OR
                     * if the coefficient set has changed from current, then
                     * need to stop, reset and restart the VIP instance so that
                     * the new scaler coefficients can be configured.
                     * This is the case when either:
                     * 1. Horizontal polyphase scaling set has changed
                     * 2. Vertical filter type is Polyphase and the scaling set
                     *    has changed.
                     * The curScFactor must reflect the current scaling factor
                     * for which scaler coefficients have been loaded.
                     */
                    if (   (NULL != userCoeffPtr)
                        || (pObj->curScFactor.hScalingSet !=
                                scFactorConfig.hScalingSet)
                        || ( (scFactorConfig.vsType == VPS_SC_VST_POLYPHASE)
                            && (pObj->curScFactor.vScalingSet !=
                                   scFactorConfig.vScalingSet)))
                    {
                        /* Update the scaler config with latest
                         * scaling factor. The vsType is set internally
                         * in the Vcore_vipGetAndUpdScFactorConfig
                         * function. */
                        scCoeffConfig.hScalingSet = scFactorConfig.hScalingSet;
                        scCoeffConfig.vScalingSet = scFactorConfig.vScalingSet;

                        /* Change curScFactor to the new one. */
                        pObj->curScFactor.hScalingSet =
                            scFactorConfig.hScalingSet;
                        pObj->curScFactor.vScalingSet =
                            scFactorConfig.vScalingSet;
                        pObj->curScFactor.hsType = scFactorConfig.hsType;
                        pObj->curScFactor.vsType = scFactorConfig.vsType;

                        /* Stop the VIP instance. */
                        pObj->resetStatistics = FALSE;
                        Vps_captStop(pObj, 1u);

                        Vcore_vipSetScCoeff (&pObj->resObj,
                            &scCoeffConfig,
                            Vps_captGetScCoeffMem (pObj->vipInstId));

                        /* Reset the VIP instance */
                        if (((pObj->platform == VPS_PLATFORM_ID_EVM_TI816x) &&
                             (pObj->cpuRev <= VPS_PLATFORM_CPU_REV_1_1)) ||
                            ((pObj->platform == VPS_PLATFORM_ID_EVM_TI814x) &&
                             (pObj->cpuRev <= VPS_PLATFORM_CPU_REV_2_1)) ||
                            (VPS_PLATFORM_ID_EVM_TI8107 == pObj->platform))
                        {
                            Vps_captVipBlockReset(pObj);
                        }

                        /* Start the VIP instance. When it is started, the new
                         * coefficients are loaded. */
                        Vps_captStart(pObj);
                    }
                }

                /*
                 * mark status as OK
                 */
                status = FVID2_SOK;
            }

            /*
             * break since there will be atmost only one stream with scaler enabled
             */
            break;
        }
    }

    return status;
}

Int32 Vps_captVipPortAndModulesReset(Vps_CaptObj *pObj)
{
    UInt32 delayMsec = 1;

    /* Reset VIP Port */
    Vcore_vipResetPort(&pObj->resObj, TRUE);

    /* Reset CSC, SC, core will ensure if CSC, SC is being used in the path,
       it would reset otherwise skip resetting it */
    Vcore_vipResetModules(&pObj->resObj, TRUE);

    Task_sleep(delayMsec);

    /* Program Abort descriptors and wait for list to get completed */
    Vps_captPostAbortList(pObj);

    /* clear VIP overflow FIQ */
    VpsHal_vipInstPortClearFIQ((VpsHal_VipInst)pObj->vipInstId);

    Task_sleep(delayMsec);

    Vcore_vipResetModules(&pObj->resObj, FALSE);

    /* Clear VIP port */
    Vcore_vipResetPort(&pObj->resObj, FALSE);

    return (FVID2_SOK);
}


//#define VPS_CAPT_DEBUG_LOG_VIP_RESET
Int32 Vps_captVipBlockReset(Vps_CaptObj *pObj)
{
    UInt32                  delayMsec = 1;
    VpsHal_VpsClkcModule    vipAllClkcModule;

    GT_assert( GT_DEFAULT_MASK, (pObj != NULL));

    if (pObj->vipInstId == VPSHAL_VIP_INST_0)
    {
        vipAllClkcModule = VPSHAL_VPS_CLKC_VIP0;
    }
    else if (pObj->vipInstId == VPSHAL_VIP_INST_1)
    {
        vipAllClkcModule = VPSHAL_VPS_CLKC_VIP1;
    }
    else
    {
        return FVID2_EFAIL;
    }

    /*
     * take CLM lock
     */
    Vps_captLmLock (  );

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] VIP Reset Starting !!!\n", pObj->vipInstId );
    #endif

    Task_sleep(delayMsec);

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] Disabling Parser !!!\n", pObj->vipInstId);
    #endif

    /* disable VIP Parser */
    VpsHal_vipInstDisable((VpsHal_VipInst)pObj->vipInstId);

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] Asserting Reset !!!\n", pObj->vipInstId);
    #endif

    /* Reset the whole VIP block */
    VpsHal_vpsClkcModuleReset(vipAllClkcModule, TRUE);

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] Asserting Clear FIFO !!!\n", pObj->vipInstId);
    #endif

    /* assert clear async FIFO */
    VpsHal_vipInstResetFIFO((VpsHal_VipInst)pObj->vipInstId, TRUE);

    Task_sleep(delayMsec);

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] Posting Abort List !!!\n", pObj->vipInstId);
    #endif

    /* clear VIP overflow FIQ */
    VpsHal_vipInstPortClearFIQ((VpsHal_VipInst)pObj->vipInstId);

    /* post abort desc on all VIP VPDMA CHs */
    Vps_captPostAllAbortList((VpsHal_VipInst)pObj->vipInstId);

    Task_sleep(delayMsec);

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] De-asserting Reset !!!\n", pObj->vipInstId);
    #endif

    /* Release the VIP block reset */
    VpsHal_vpsClkcModuleReset(vipAllClkcModule, FALSE);

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] De-asserting Clear FIFO !!!\n", pObj->vipInstId);
    #endif

    /* Release FIFO Reset */
    VpsHal_vipInstResetFIFO((VpsHal_VipInst)pObj->vipInstId, FALSE);

    Task_sleep(delayMsec);

    #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
    Vps_rprintf(" [VIP%d] VIP Reset Done !!!\n", pObj->vipInstId);
    #endif

    /*
     * unlock CLM
     */
    Vps_captLmUnlock (  );

    return 0;
}


Void Vps_captVipWaitForComplete(Vps_CaptObj *pObj)
{
    UInt32 delay = 1u;
    volatile UInt32 waitCnt = 40;

    while (0u == Vcore_vipGetPortCompleteStatus(&pObj->resObj))
    {
        Task_sleep(delay);

        waitCnt --;

        if (!waitCnt)
        {

            #ifdef VPS_CAPT_DEBUG_LOG_VIP_RESET
            Vps_rprintf("[VIP%d]Cannot get Complete Status!!!\n",
                pObj->instanceId);
            #endif
            break;
        }
    }

    Vcore_vipClearPortCompleteStatus(&pObj->resObj);
}


/**
 *  Vps_captSubFrameIsEnabled
 *  \brief This function checks if sub frame capturing is enabled for any of the
 *         output streams are not. The create arguments supplied by application
 *         is used to determine if subframe based capture is enabled.
 *
 *  \param pObj   Pointer to instance object.
 *
 *  \return       Returns TRUE is sub frame based capture is enabled, FALSE
 *                otherwise.
 */

Bool Vps_captSubFrameIsEnabled ( Vps_CaptObj *pObj )
{
    Bool rtnValue = FALSE;
    UInt32 numStream;

    for ( numStream = 0x0; numStream < pObj->createArgs.numStream; numStream++ )
    {
        if ( pObj->createArgs.outStreamInfo[numStream].subFrameModeEnable
                == TRUE )
        {
            rtnValue = TRUE;
            break;
        }
    }

    return (rtnValue);
}


/**
 *  Vps_captVipSubFrameRegisterIsr
 *  \brief This function register the ISR for X line interrupt and frame
 *         reception complete interrupts.
 *
 *  \param pObj   Instance object pointer.
 *
 *  \pre This function assumes that subFrameQ for each channel is created.
 *
 *  \return   Returns 0 on success, negative value otherwise.
 */

Int32 Vps_captVipSubFrameRegisterIsr ( Vps_CaptObj * pObj )
{
    Int32 rtnValue = FVID2_SOK;
    UInt32 streamId, chId, subFrameEn;
    Int32 clientNo[VPS_CAPT_VCH_PER_LCH_MAX];
    UInt32 vpdmaChanNo;
    Vps_CaptChObj *pChObj;
    Vps_CaptSubFrameChObj *pChSfObj;

    #ifdef VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
    gCaptSubFrameDebug.eofCounter = 0x0;
    gCaptSubFrameDebug.xLineCounter = 0x0;
    gCaptSubFrameDebug.fidMismatch = 0x0;
    gCaptSubFrameDebug.maxHeight = 0x0;
    gCaptSubFrameDebug.minHeight = 0xFFFF;
    gCaptSubFrameDebug.maxWidth = 0x0;
    gCaptSubFrameDebug.minWidth = 0xFFFF;
    gCaptSubFrameDebug.shortFrames = 0x0;
    gCaptSubFrameDebug.minSubFrames = 0xFFFF;
    gCaptSubFrameDebug.maxSubFrames = 0x0;
    gCaptSubFrameDebug.shortFrameTrigFlag = 0x0;
    #endif /* VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS */

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        subFrameEn =
            pObj->createArgs.outStreamInfo[streamId].subFrameModeEnable;
        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            pChObj = &pObj->chObj[streamId][chId];
            if (subFrameEn == TRUE)
            {
                pChSfObj = &pChObj->subFrameObj;

                GT_assert( GT_DEFAULT_MASK, (chId == 0x0));
                GT_assert( GT_DEFAULT_MASK, (pChSfObj != NULL));
                /* Primarily we will registering N line callback for
                 * Luma / Green. In case we have to look at secondary
                 * (Chroma / Blue) channel also, register the secondary
                 * channel ISRs
                 */
                clientNo[0x0u] =
                    VpsHal_vpdmaGetClientNo ( pChObj->vChannelId[0] );
                GT_assert( GT_DEFAULT_MASK,
                    ( clientNo[0x0u] != VPSHAL_VPDMA_CLIENT_INVALID ));

                pChSfObj->subFrameClientInt[0x0u] =  Vem_register (
                                        VEM_EG_CLIENT,
                                        (UInt32 *) clientNo,
                                        1,
                                        VEM_PRIORITY1,
                                        Vps_captVipSubFrameXlinePIsr,
                                        (Void *) (&pChSfObj->subFrameQ) );

                if ( pChSfObj->subFrameClientInt[0x0u] == NULL )
                {
                    rtnValue = FVID2_EFAIL;
                }

                #ifdef VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL
                if ( (pChObj->vChannelNum > 0x0u) && (rtnValue == FVID2_SOK) )
                {
                     clientNo[0x1u] =
                        VpsHal_vpdmaGetClientNo ( pChObj->vChannelId[1] );
                    GT_assert( GT_DEFAULT_MASK,
                        ( clientNo[0x0u] != VPSHAL_VPDMA_CLIENT_INVALID ));

                    pChSfObj->subFrameClientInt[0x1u] = Vem_register (
                                        VEM_EG_CLIENT,
                                        (UInt32 *) &(clientNo[1]),
                                        1,
                                        VEM_PRIORITY1,
                                        Vps_captVipSubFrameXlineSIsr,
                                        (Void *) (&pChSfObj->subFrameQ) );

                    if ( pChSfObj->subFrameClientInt[0x1u] == NULL )
                    {
                        rtnValue = FVID2_EFAIL;
                    }
                }
                #endif /* VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL */

                /* Register Channel ISR
                 * End Of Frame Reception ISR
                 */
                if ( rtnValue == FVID2_SOK )
                {
                    vpdmaChanNo = pChObj->vChannelId[0];
                    pChSfObj->subFrameChannelInt  =   Vem_register (
                                            VEM_EG_CHANNEL,
                                            (UInt32 *) &vpdmaChanNo,
                                            1,
                                            VEM_PRIORITY0,
                                            Vps_captVipSubFrameEOFIsr,
                                            (Void *)(&pChSfObj->subFrameQ));

                    if ( pChSfObj->subFrameChannelInt == NULL )
                    {
                        rtnValue = FVID2_EFAIL;
                    }
                }
                /* Assumption, end of frame is assumed when we get end of frame
                 * for the primary channel. Hence not registering CB for
                 * secondary channel
                 */
            }
        }
    }

    return (rtnValue);
}

/**
 *  Vps_captVipSubFrameUnRegisterIsr
 *  \brief This function un-register the ISR for X line interrupt and frame
 *         reception complete interrupt.
 *
 *  \param pObj   Instance object pointer.
 *
 *  \return   Returns 0 on success, negative value otherwise.
 */

Int32 Vps_captVipSubFrameUnRegisterIsr ( Vps_CaptObj * pObj )
{
    Int32 rtnValue = FVID2_SOK;
    UInt32 streamId, chId, subFrameEn;
    Vps_CaptChObj *pChObj;
    Vps_CaptSubFrameChObj *pChSfObj;

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        subFrameEn =
            pObj->createArgs.outStreamInfo[streamId].subFrameModeEnable;
        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            GT_assert( GT_DEFAULT_MASK, (chId == 0x0));

            pChObj = &pObj->chObj[streamId][chId];
            if (subFrameEn == TRUE)
            {
                pChSfObj = &pChObj->subFrameObj;

                GT_assert( GT_DEFAULT_MASK,
                    ( pChSfObj->subFrameClientInt[0x0u] != NULL ) );
                rtnValue =
                    Vem_unRegister ( pChSfObj->subFrameClientInt[0x0u] );
                GT_assert( GT_DEFAULT_MASK, ( rtnValue == FVID2_SOK ) );

                if (pChObj->vChannelNum > 0x0u)
                {
                    #ifdef VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL
                    GT_assert( GT_DEFAULT_MASK,
                        ( pChSfObj->subFrameClientInt[0x1u] != NULL ) );

                    rtnValue =
                        Vem_unRegister (pChSfObj->subFrameClientInt[0x1u]);

                    GT_assert( GT_DEFAULT_MASK, ( rtnValue == FVID2_SOK ) );
                    #endif /* VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL */
                }
                GT_assert( GT_DEFAULT_MASK,
                    ( pChSfObj->subFrameChannelInt != NULL ) );

                rtnValue = Vem_unRegister ( pChSfObj->subFrameChannelInt );
                GT_assert( GT_DEFAULT_MASK, ( rtnValue == FVID2_SOK ) );
            }
        }
    }
    return (rtnValue);
}


/**
 *  Vps_captVipSubFrameAddQObj
 *  \brief This function, adds the given frame into channels sub frame Q.
 *
 *  \param pChObj       Pointer to channel handle
 *  \param pFrame       Pointer to frame that would be saved
 *
 *  \return   Returns pointer to Vps_CaptSubFrameQObj, which was added into
 *            subFrame Q. NULL otherwise.
 */

Vps_CaptSubFrameQObj * Vps_captVipSubFrameAddQObj( Vps_CaptChObj *pChObj,
                                                   FVID2_Frame *pFrame)
{
    Int32 rtnValue;
    Vps_CaptSubFrameQObj *pQObj = NULL;

    GT_assert( GT_DEFAULT_MASK, (pFrame != NULL));
    GT_assert( GT_DEFAULT_MASK, (pChObj != NULL));
    GT_assert( GT_DEFAULT_MASK, (pChObj->isSubFrameEnabled == TRUE));

    pQObj = Vps_captVipSubFrameAllocQObj( pChObj->subFrameObj.streamId );

    if (pQObj != NULL)
    {
        pQObj->pFrame = pFrame;
        rtnValue = VpsUtils_quePut ( &pChObj->subFrameObj.subFrameQ,
                                     (Ptr) pQObj, BIOS_NO_WAIT );
        if ( rtnValue != FVID2_SOK )
        {
            Vps_captVipSubFrameFreeQObj ( pQObj );
            pQObj = NULL;
        }
    }

    return (pQObj);
}

/**
 *  Vps_captVipSubFrameAllocQObj
 *  \brief This function allocates a subframe Q object.
 *
 *  \param streamId     Stream identifier that of a given channel
 *
 *  \return   Returns pointer to Vps_CaptSubFrameQObj, NULL otherwise.
 */

static Vps_CaptSubFrameQObj * Vps_captVipSubFrameAllocQObj( UInt32 streamId )
{
    Vps_CaptSubFrameQObj *allocObj = NULL;
    UInt32 cookie, index, exitCondition, startIndex, stopIndex;

    exitCondition = FALSE;
    startIndex = gVps_captSubFrameQIndex;
    stopIndex = VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;

    if (streamId >= VPS_CAPT_STREAM_ID_MAX)
    {
        return (allocObj);
    }
    while (TRUE)
    {
        cookie = Hwi_disable (  );
        for (index = startIndex; index < stopIndex; index++)
        {
            if (gVps_captSubFrameQObj[streamId][index].allocStatus ==
                    VPS_CAPT_SUBFRAME_FREE)
            {
                gVps_captSubFrameQObj[streamId][index].allocStatus =
                    VPS_CAPT_SUBFRAME_ALLOC;

                allocObj = &gVps_captSubFrameQObj[streamId][index];
                gVps_captSubFrameQIndex = index;
                gVps_captSubFrameQIndex++;
                if (gVps_captSubFrameQIndex == VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX)
                {
                    gVps_captSubFrameQIndex = 0x0;
                }
                break;
            }
        }

        if (allocObj == NULL)
        {
            if (exitCondition == TRUE)
            {
                /* We have checked all entries no objects available */
                /* Allocation error */
                break;
            }
            startIndex = 0x0;
            stopIndex = VPS_CAPT_FRAME_QUE_LEN_PER_CH_MAX;
            exitCondition = TRUE;
        }
        else
        {
            /* We have a object */
            break;
        }
        Hwi_restore ( cookie );
    }
    return (allocObj);
}


/**
 *  Vps_captVipSubFrameFreeQObj
 *  \brief Releases a previously allocated sub frame Q object.
 *
 *  \param obj      Pointer to previously allocated sub frame Q object.
 *
 *  \return   Returns FVID2_SOK on success, a negative value otherwise.
 *
 *  CAUTION : Do not clear following elements of Q object
 *      1. obj->pChObj
 *      2. obj->handle
 *      3. obj->maxFrameHeight
 */

static Int32 Vps_captVipSubFrameFreeQObj ( Vps_CaptSubFrameQObj *obj )
{
    UInt32 cookie;

    GT_assert( GT_DEFAULT_MASK, obj);

    cookie = Hwi_disable (  );

    obj->status                 = FVID2_SOK;
    obj->pFrame                 = NULL;
    obj->totalNoOfLinesPChan    = 0x0;
    obj->totalNoOfLinesSChan    = 0x0;
    obj->allocStatus            = VPS_CAPT_SUBFRAME_FREE;
    obj->subFrameNo             = 0x0;

    Hwi_restore ( cookie );

    return (FVID2_SOK);
}

/**
 *  Vps_captVipSubFrameXlinePIsr
 *  \brief When sub frame based processing is enabled, this function would be
 *         called once every X lines are captured for the primary channel, Y
 *         case of YUV data type and G incase of RGB. This function would
 *         1. For very first X lines of a field, this function would READ and
 *            update the FID.
 *         2. Frame and issue application callbacks indicating completion of
 *            X lines reception.
 *
 *  \param event        Event that caused this interrupt. Ignored
 *  \param numEvents    Number of events that happened. Ignored
 *  \param arg          Pointer to channels subFrame queue.
 *
 *  \return   Returns 0 on success, negative value otherwise.
 */

Void Vps_captVipSubFrameXlinePIsr( const UInt32 *event,
                                   UInt32 numEvents,
                                   Ptr arg )
{
    Vps_CaptSubFrameQObj *currField = NULL;
    VpsUtils_QueHandle *qHndl = (VpsUtils_QueHandle *)arg;
    Int32 rtnValue = FVID2_SOK;
    Vps_CaptSubFrameChObj *pChSfObj = NULL;

    /* arg Will point to the channels Q Handle */
    /* 1. Get the address of the first element */
    rtnValue = VpsUtils_quePeek(qHndl, (Ptr *) &currField);

    if ((rtnValue == FVID2_SOK) && (currField != NULL))
    {
        GT_assert( GT_DEFAULT_MASK, currField->pFrame);
        /* 2. Ensure the frame under capture is valid */
        if ((currField->status == FVID2_SOK) &&
            (currField->pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID))
        {
            pChSfObj = &currField->pChObj->subFrameObj;

            /* 2.1. Check if current field is still under capture */
            if (pChSfObj->currCaptField == currField)
            {
                /* 3. Increment the primary slice counters */
                currField->totalNoOfLinesPChan += pChSfObj->nLineCount;

                #ifdef VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL
                /* 3.1 if N counter > 0x2 and S counters == 0x0
                       We did not get chroma flag an error to apps */
                if (currField->subFrameNo == 0x2)
                {
                    if (currField->totalNoOfLinesSChan == 0x0)
                    {
                        /* Did not see any callback on secondary channel */

                    }
                }
                #endif /* VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL */

                /* 3.2 Read the FID, width and height from VIP registers */
                if (currField->subFrameNo == 0x0)
                {
                    /* Read FID for the very first time */
                    currField->pFrame->fid =
                        (((*pChSfObj->fidReg) & pChSfObj->maskFid)
                            >> pChSfObj->shiftFid);

                    #ifdef VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
                    /* Track FID here SUBFRAME_TBD */
                    #endif /* VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS */

                }
                /* 3.3 Update the return value */
                if (currField->pFrame->subFrameInfo != NULL)
                {
                    currField->pFrame->subFrameInfo->subFrameNum =
                        currField->subFrameNo;
                    currField->pFrame->subFrameInfo->numOutLines =
                        currField->totalNoOfLinesPChan;
                }
                currField->subFrameNo++;

                /* 4. Issue the X line callback */
                pChSfObj->subFrameCb ( currField->handle, currField->pFrame );
            }
        }
    }
    #ifdef VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
    gCaptSubFrameDebug.xLineCounter++;
    #endif /* VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS */
    return;
}

/**
 *  Vps_captVipSubFrameXlineSIsr
 *  \brief When sub frame based processing is enabled, this function would be
 *         called once every X lines are captured on secondary channel, UV
 *         in case of YUV data type. This function would update the secondary
 *         line counter.
 *         Primarily used for DEBUG.
 *
 *  \param event        Event that caused this interrupt. Ignored
 *  \param numEvents    Number of events that happened. Ignored
 *  \param arg          Pointer to channels subFrame queue.
 *
 *  \return   Returns 0 on success, negative value otherwise.
 */
#ifdef VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL
Void Vps_captVipSubFrameXlineSIsr(const UInt32 *event,
                                  UInt32 numEvents,
                                  Ptr arg)
{
    Vps_CaptSubFrameQObj *currField = NULL;
    Int32 rtnValue = FVID2_SOK;
    VpsUtils_QueHandle *qHndl = (VpsUtils_QueHandle *)arg;

    /* arg Will point to the channels Q Handle */
    /* 1. Get the address of the first element - */
    rtnValue = VpsUtils_quePeek( qHndl, (Ptr *) &currField );
    if ( (rtnValue == FVID2_SOK) && (currField != NULL) )
    {
        GT_assert( GT_DEFAULT_MASK, currField->pFrame );
        /* 2. Ensure the frame under capture is valid */
        if ( (currField->status == FVID2_SOK) &&
             (currField->pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID) )
        {
            if ( currField->pChObj->subFrameObj->currCaptField == currField)
            {
                /* 3. Increment the secondary slice counters */
                currField->totalNoOfLinesSChan++;
            }
        }
    }

    return;
}
#endif /* VPS_CAPT_SUBFRAME_CHECK_SEC_CHANNEL */


/**
 *  Vps_captVipSubFrameEOFIsr
 *  \brief When sub frame based processing is enabled and capture of a field
 *         completes. This function would perform following steps
 *          1. Get the first element from the sub frame Q
 *              Require exclusive access here
 *          1.1. Get the handle for next frame, update channel var
 *               with next frame pointer value
 *              Exit exclusive access
 *          2. Ensure current frame is NOT dummy frame and
 *              can be issue callback to application
 *          3. Check for short frames and long frames - Debug AID
 *          4. Frame and Issue callback - Read Height and Width
 *          5. Release sub frame Q object
 *
 *  \param event        Event that caused this interrupt. Ignored
 *  \param numEvents    Number of events that happened. Ignored
 *  \param arg          Pointer to channels subFrame queue.
 *
 *  \return   Returns 0 on success, negative value otherwise.
 */

Void Vps_captVipSubFrameEOFIsr(const UInt32 *event,
                                  UInt32 numEvents,
                                  Ptr arg)
{
    /* Note :   In situation where in N line ISR has low pirority when compared
        with frame completion ISR, N line ISR could be preempted. To ensure that
        consistency between N line CB and end of frame CB
        1. Have a global var in channel handle that will hold current frame
            under reception.
        2. Frame complete CB will update this as a first step in ISR
        3. In N lines ISR compare this var with pFrame value if its not same
           do not issue N line CB.
    */

    UInt32 cookie, widthAtEof, heightAtEof;
    VpsUtils_QueHandle *qHndl = (VpsUtils_QueHandle *)arg;
    Vps_CaptSubFrameQObj *compField, *currField;
    Vps_CaptSubFrameChObj *pChSfObj = NULL;
    Vps_CaptRtParams *pRtParams = NULL;

    /* arg Will point to the channels Q Handle */
    /* 1. Get the first element - First frame captured */

    VpsUtils_queGet( qHndl, (Ptr *) &compField, 1, BIOS_NO_WAIT );

    if ( compField != NULL )
    {
        /* 1.1. Update the pFrame with current frame under reception */
        cookie = Hwi_disable (  );

        VpsUtils_quePeek( qHndl, (Ptr *) &currField );

        if ( currField != NULL )
        {
            currField->pChObj->subFrameObj.currCaptField = currField;
        }
        Hwi_restore ( cookie );

        GT_assert( GT_DEFAULT_MASK, compField->pFrame );
        /* 2. Ensure the frame under capture is valid */
        if ( compField->pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID )
        {
            pChSfObj = &compField->pChObj->subFrameObj;
            /* Read the width and Height */
            widthAtEof = (((*pChSfObj->srcSizeWidthReg) & pChSfObj->maskWidth)
                            >> pChSfObj->shiftWidth);
            heightAtEof = (((*pChSfObj->srcSizeHeightReg) & pChSfObj->maskHeight)
                            >> pChSfObj->shiftHeight);

            /* Check if we can update sub frame info */
            if ( compField->pFrame->subFrameInfo != NULL )
            {
                /* 3. Check short / long frames - debug trackers */
                #ifdef VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
                if (pChSfObj->expectSFCount !=
                        VPS_CAPT_SUBFRAME_HEIGHT_UNLIM_EXPECTED_SF_COUNT)
                {
                    if ( compField->subFrameNo != (pChSfObj->expectSFCount - 0x1u) )
                    {
                        /* Did not receive expected number of sub frames.
                           Due to connect / disconnect ?
                           Nothing much could be done, tell the apps frame
                           is completed, increment debug counters */

                        gCaptSubFrameDebug.shortFrames++;
                    }
                }
                else
                {
                    /* Track the minimum sub frames and maximum sub frames */
                    if (gCaptSubFrameDebug.minSubFrames > compField->subFrameNo)
                    {
                        gCaptSubFrameDebug.minSubFrames = compField->subFrameNo;
                        if (gCaptSubFrameDebug.shortFrameTrigFlag == 0x0)
                        {
                            gCaptSubFrameDebug.shortFrameTrigFlag = 0x01;
                        }
                        else
                        {
                            gCaptSubFrameDebug.shortFrames++;
                        }
                    }
                    if (gCaptSubFrameDebug.maxSubFrames < compField->subFrameNo)
                    {
                        gCaptSubFrameDebug.maxSubFrames = compField->subFrameNo;
                    }
                }
                #endif /* VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS */

                /* End Of Frame, its expected to receive full buffer, if few
                   lines were missing, nothing much can be done. Tell the app
                   we captured complete frame */
                compField->pFrame->subFrameInfo->subFrameNum =
                    compField->subFrameNo;
                compField->pFrame->subFrameInfo->numOutLines = heightAtEof;
            }

            /* 4.1 Update the height and width */
            if (NULL != compField->pFrame->perFrameCfg)
            {
                pRtParams = (Vps_CaptRtParams *)compField->pFrame->perFrameCfg;

                /* udpate width x height in run time per frame config */
                pRtParams->captureOutWidth = widthAtEof;
                pRtParams->captureOutHeight = heightAtEof;

            }
            #ifdef VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
            if (gCaptSubFrameDebug.maxHeight < heightAtEof)
            {
                gCaptSubFrameDebug.maxHeight = heightAtEof;
            }
            if (gCaptSubFrameDebug.minHeight > heightAtEof)
            {
                gCaptSubFrameDebug.minHeight = heightAtEof;
            }
            if (gCaptSubFrameDebug.maxWidth < widthAtEof)
            {
                gCaptSubFrameDebug.maxWidth = widthAtEof;
            }
            if (gCaptSubFrameDebug.minWidth > widthAtEof)
            {
                gCaptSubFrameDebug.minWidth = widthAtEof;
            }
            #endif /* VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS */
            /* 4.2 Issue the callback */
            pChSfObj->subFrameCb ( compField->handle, compField->pFrame );
        }
        /* 5. Release sub frame Q */
        Vps_captVipSubFrameFreeQObj ( compField );

    }

    #ifdef VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS
    gCaptSubFrameDebug.eofCounter++;
    #endif /* VPS_CAPT_SUBFRAME_DEBUG_CB_COUNTERS */
    return;
}

/**
 *  Vps_captVipTSFrameEofIsr
 *  \brief When the capture driver is configured to time stamp frames at the end
 *          of a frame, this ISR would be called. This ISR would
 *          1. Get the first element from channel tmpQue
 *          2. Check for NULL - This ISR will not check for dummy frames.
 *          3. Time stamp the frame.
 *
 *  \param event        Event that caused this interrupt. Ignored
 *  \param numEvents    Number of events that happened. Ignored
 *  \param arg          Pointer to channel object.
 *
 *  \return   None
 */

Void Vps_captVipTSFrameEofIsr(const UInt32 *event,
                                  UInt32 numEvents,
                                  Ptr arg)
{
    Int32 status;
    Vps_CaptChObj *pChObj = (Vps_CaptChObj *) arg;
    volatile FVID2_Frame *pFrame = NULL;

    GT_assert( GT_DEFAULT_MASK, (pChObj != NULL));
    GT_assert( GT_DEFAULT_MASK,  pChObj->getCurrTime != NULL);

    status = VpsUtils_queGet(&pChObj->tsFrameQue, 
                        (Ptr *) &pFrame, 0x1, BIOS_NO_WAIT);
    if (status == 0x0)
    {
        if (pFrame == NULL)
        {
            /* No frames queued for capture, must be multiple short frames, that
                consumed the VPDMA desc programmed by 8 msec ticks */
            return;
        }

        /* The de-queued frame is discarded, as the timestamp would be updated
            below */
        if (pFrame->timeStamp == 0)
        {
            pFrame->timeStamp = pChObj->getCurrTime((Ptr)pFrame);
        }
#ifndef VPS_CAPT_PRINT_TS_STATS

    }

#else
        else
        {
            if (pFrame->channelNum != VPS_CAPT_DROP_FRAME_CH_ID)
            {
                /* Ideally this should not occur, if so increment the counter */
                pChObj->tsMultiStampFrameCnt++;
            }
        }
    }
    else
    {
        pChObj->tsErr++;
    }
#endif /* VPS_CAPT_PRINT_TS_STATS */
    /* Could not de-queue from the que, could occur, when multiple short frames
        occur with in 8 msec */
        
        
    /* Debug code - tracks the raw time stamps of the End of Frame ISR */
#ifdef VPS_CAPTURE_TS_DEBUG
    {
        volatile UInt32 temp;

        temp = pChObj->getCurrTime(NULL);

        if ((temp - pChObj->tsDbgLastTs) < pChObj->tsExpectTime)
        {
            pChObj->tsIsrDbg[pChObj->tsIsrDbgIndex][0] = temp - pChObj->tsDbgLastTs;
            pChObj->tsIsrDbg[pChObj->tsIsrDbgIndex][1] = temp;
            if (pChObj->tsIsrDbgIndex < VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH)
            {
                pChObj->tsIsrDbgIndex++;
            }
        }
        
        if ((temp - pChObj->tsDbgLastTs) > (pChObj->tsExpectTime + 1))
        {
            pChObj->tsIsrHDbg[pChObj->tsIsrHDbgIndex][0] = temp - pChObj->tsDbgLastTs;
            pChObj->tsIsrHDbg[pChObj->tsIsrHDbgIndex][1] = temp;
            if (pChObj->tsIsrHDbgIndex < VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH)
            {
                pChObj->tsIsrHDbgIndex++;
            }
        }

        pChObj->tsDbgLastTs = temp;
    }
#endif /* VPS_CAPTURE_TS_DEBUG */

    return;
}

/**
 *  Vps_captTimeStampAddElem
 *  \brief This function adds the provided the frame pointer, into time stamp
 *          queue.
 *
 *  \param pChObj   Pointer to the channel object.
 *  \param pFrame   Pointer to a valid FVID2_Frame.
 *
 *  \return  Returns 0 on success else a negative number.
 */

Int32 Vps_captTimeStampAddElem( Vps_CaptChObj * pChObj, FVID2_Frame *pFrame )
{
    Int32 rtnValue;

    GT_assert( GT_DEFAULT_MASK, (pChObj != NULL));
    GT_assert( GT_DEFAULT_MASK, (pFrame != NULL));
    
    rtnValue = VpsUtils_quePut ( &pChObj->tsFrameQue,
                                 (Ptr) pFrame, BIOS_NO_WAIT );
    if (rtnValue != FVID2_SOK)
    {
        GT_assert( GT_DEFAULT_MASK, FALSE);
    }

    return rtnValue;
}


/**
 *  Vps_captTimeStampFlushQue
 *  \brief This function clear all the added elements.
 *
 *  \param pChObj   Pointer to the channel object.
 *
 *  \return  Returns 0 on success else a negative number.
 */

Int32 Vps_captTimeStampFlushQue( Vps_CaptChObj * pChObj )
{
    UInt32 cookie;
    FVID2_Frame *pFrame;

    /* Just to be sure, no else queues while flushing */
    cookie = Hwi_disable (  );

    GT_assert( GT_DEFAULT_MASK, (pChObj != NULL));

    while (VpsUtils_queIsEmpty( &pChObj->tsFrameQue) == FALSE)
    {
        VpsUtils_queGet(&pChObj->tsFrameQue, 
                        (Ptr *) &pFrame, 0x1, BIOS_NO_WAIT);
    }

    Hwi_restore ( cookie );
    return FVID2_SOK;
}

/**
 *  Vps_captTimeStampCfgEofInt
 *  \brief This function either enables or disables the end of frame interrupt
 *          (channel interrupt), for all channels in this instance.
 *
 *  \param pObj         Pointer to instance object.
 *  \param enable       Either to enable / disable the interrupt
 *
 *  \return  Returns 0 on success else a negative number.
 */


Int32 Vps_captTimeStampCfgEofInt( Vps_CaptObj * pObj, UInt32 enable )
{
    Int32 retVal = FVID2_SOK;
    UInt32 streamId, chId, vpdmaChNo;
    Vps_CaptChObj *pChObj;

    for ( streamId = 0; streamId < pObj->numStream; streamId++ )
    {
        for ( chId = 0; chId < pObj->numCh; chId++ )
        {
            pChObj = &pObj->chObj[streamId][chId];
            vpdmaChNo = pChObj->vChannelId[0];

            if (enable == TRUE)
            {
                pChObj->tsEofIsrHndl = Vem_register (
                                            VEM_EG_CHANNEL,
                                            (UInt32 *) &vpdmaChNo,
                                            1,
                                            VEM_PRIORITY0,
                                            &Vps_captVipTSFrameEofIsr,
                                            (Void *)pChObj);

                if ( pChObj->tsEofIsrHndl == NULL )
                {
                    retVal = FVID2_EFAIL;
                    break;
                }
            }
            else
            {
                if (pChObj->tsEofIsrHndl != NULL)
                {
                    retVal = Vem_unRegister ( pChObj->tsEofIsrHndl );
                    GT_assert( GT_DEFAULT_MASK, ( retVal == FVID2_SOK ) );
                    pChObj->tsEofIsrHndl = NULL;
                }
            }
        }
    }

    return retVal;
}


/**
 *  Vps_captTimeStampStatsUpdate
 *  \brief This function updates the minimum time / maximum time period between
 *          two consecutive frames.
 *
 *  \param pChObj       Pointer to channel instance.
 *  \param currTs       Time Stamp of the current frame.
 *
 *  \return  Returns 0
 */
Int32 Vps_captTimeStampStatsUpdate( Vps_CaptChObj * pChObj, UInt32 currTs )
{
#ifdef VPS_CAPT_PRINT_TS_STATS
    Int32 cTsDif;

    GT_assert( GT_DEFAULT_MASK, (pChObj != NULL));

    if (currTs == 0)
    {
        /* This frame was not time stampped */
        pChObj->tsMissedFrameCnt++;
        if (pChObj->tsLast != 0)
        {
            pChObj->tsLast += pChObj->tsExpectTime;
        }
        return FVID2_SOK;
    }
    if (pChObj->tsLast == 0)
    {
        pChObj->tsLast = currTs;

#ifdef VPS_CAPTURE_TS_DEBUG
        pChObj->tsDbgLastTs = currTs;
#endif /* VPS_CAPTURE_TS_DEBUG */

        return FVID2_SOK;
    }
    cTsDif = currTs - pChObj->tsLast;
    pChObj->tsLast = currTs;

#ifdef VPS_CAPTURE_TS_DEBUG
    if ((gVps_CaptResetTsStats & (0x1 << pChObj->channelNum)) != 0)
    {
        gVps_CaptResetTsStats &= ~(0x1 << pChObj->channelNum);
        pChObj->tsDrvDbgIndex = 0;
        pChObj->tsMinDiff = 0xFFFFFFF;
        pChObj->tsDrvHDbgIndex = 0;
        pChObj->tsMaxDiff = 0;
        
        pChObj->tsCummulative = 0;

    }
#endif /* VPS_CAPTURE_TS_DEBUG */

    if (cTsDif > 0)
    {
        pChObj->tsCummulative += cTsDif;

        if (pChObj->tsMinDiff > cTsDif)
        {
            pChObj->tsMinDiff = cTsDif;

#ifdef VPS_CAPTURE_TS_DEBUG
            if (cTsDif < pChObj->tsExpectTime)
            {
                pChObj->tsDrvDbg[pChObj->tsDrvDbgIndex][0] = cTsDif;
                pChObj->tsDrvDbg[pChObj->tsDrvDbgIndex][1] = pChObj->tsLast;
                if (pChObj->tsDrvDbgIndex < VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH)
                {
                    pChObj->tsDrvDbgIndex++;
                }
            }
#endif /* VPS_CAPTURE_TS_DEBUG */

        }
        if (pChObj->tsMaxDiff < cTsDif)
        {
            pChObj->tsMaxDiff = cTsDif;

#ifdef VPS_CAPTURE_TS_DEBUG
            if (cTsDif > (pChObj->tsExpectTime + 1))
            {
                pChObj->tsDrvHDbg[pChObj->tsDrvHDbgIndex][0] = cTsDif;
                pChObj->tsDrvHDbg[pChObj->tsDrvHDbgIndex][1] = pChObj->tsLast;
                if (pChObj->tsDrvHDbgIndex < VPS_CAPTURE_TS_UNIQUE_VALUE_DEPTH)
                {
                    pChObj->tsDrvHDbgIndex++;
                }
            }
#endif /* VPS_CAPTURE_TS_DEBUG */
        }
    }
    else
    {
        pChObj->tsErr++;
    }
#endif /* VPS_CAPT_PRINT_TS_STATS */

    return FVID2_SOK;
}


