/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "captureLink_priv.h"

char *gCaptureLink_portName[] =
{
    "VIP0 PortA", "VIP0 PortB", "VIP1 PortA", "VIP1 PortB",
};

char *gCaptureLink_ifName[] =
{
    " 8-bit", "16-bit", "24-bit",
};

char *gCaptureLink_modeName[] =
{
    "Non-mux Embedded Sync",
    "Line-mux Embedded Sync", "Pixel-mux Embedded Sync",
    "Non-mux Discrete Sync (HSYNC/VBLK)", "Non-mux Discrete Sync (HSYNC/VSYNC)",
    "Non-mux Discrete Sync (ACTVID/VBLK)", "Non-mux Discrete Sync (ACTVID/VSYNC)",
    "Split line Embedded Sync",
};


/* driver callback */
Int32 CaptureLink_drvCallback ( FVID2_Handle handle, Ptr appData, Ptr reserved )
{
    CaptureLink_Obj *pObj = (CaptureLink_Obj *)appData;

    if(pObj->cbCount && (pObj->cbCount%CAPTURE_LINK_TSK_TRIGGER_COUNT) == 0)
    {
        VpsUtils_tskSendCmd(&pObj->tsk, SYSTEM_CMD_NEW_DATA);
    }
    pObj->cbCount++;

    return FVID2_SOK;
}

/* select input source in simulator environment */
Int32 CaptureLink_drvSimVideoSourceSelect(
                    UInt32 instId,
                    UInt32 captureMode,
                    UInt32 videoIfMode
    )
{
    UInt32 fileId, pixelClk;

    /* select input source file,

       Assumes that the simulator VIP super file contents are like below

        1 <user path>\output_bt656_QCIF.bin             # 8 -bit YUV422 single CH input
        2 <user path>\output_bt1120_QCIF.bin            # 16-bit YUV422 single CH input
        3 <user path>\output_bt1120_QCIF_RGB.bin        # 24-bit RGB888 single CH input
        4 <user path>\output_tvp5158_8CH_bt656_QCIF.bin # 8 -bit YUV422 multi  CH input
    */

    pixelClk = 80*1000000/2;

    switch(captureMode)
    {
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC:
            switch(videoIfMode)
            {
                case VPS_CAPT_VIDEO_IF_MODE_8BIT:
                    fileId = 5;
                    break;
                case VPS_CAPT_VIDEO_IF_MODE_16BIT:
                    fileId = 6;
                    break;
                case VPS_CAPT_VIDEO_IF_MODE_24BIT:
                    fileId = 7;
                    break;

            }
            break;

        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC:
            fileId = 8;
            pixelClk = 40*1000000;
            break;


    }

    Vps_platformSimVideoInputSelect(instId, fileId, pixelClk);

    return FVID2_SOK;
}

/* Create video decoder, TVP5158 or HDMI RX */
Int32 CaptureLink_drvCreateVideoDecoder(CaptureLink_Obj *pObj, UInt16 instId)
{
    CaptureLink_VipInstParams *pInstPrm;
    CaptureLink_InstObj *pInst;
    Vps_CaptCreateParams *pVipCreateArgs;

    Vps_VideoDecoderChipIdParams vidDecChipIdArgs;
    Vps_VideoDecoderChipIdStatus vidDecChipIdStatus;

    Int32 status;

    pInstPrm = &pObj->createArgs.vipInst[instId];
    pInst = &pObj->instObj[instId];

    pVipCreateArgs = &pInst->createArgs;

    pInst->vidDecCreateArgs.deviceI2cInstId = Vps_platformGetI2cInstId();
    pInst->vidDecCreateArgs.numDevicesAtPort = 1;
    pInst->vidDecCreateArgs.deviceI2cAddr[0]
            = Vps_platformGetVidDecI2cAddr(pInstPrm->videoDecoderId, pInstPrm->vipInstId);
    pInst->vidDecCreateArgs.deviceResetGpio[0] = VPS_VIDEO_DECODER_GPIO_NONE;

    if((pInstPrm->videoDecoderId==FVID2_VPS_VID_DEC_TVP7002_DRV ||
        pInstPrm->videoDecoderId==FVID2_VPS_VID_DEC_SII9135_DRV)
        &&
        (pInstPrm->vipInstId == VPS_CAPT_INST_VIP0_PORTA ||
        pInstPrm->vipInstId == VPS_CAPT_INST_VIP1_PORTA))
    {
        Vps_platformSelectVideoDecoder(pInstPrm->videoDecoderId, pInstPrm->vipInstId);
    }

    pInst->videoDecoderHandle = FVID2_create(
                                    pInstPrm->videoDecoderId,
                                    0,
                                    &pInst->vidDecCreateArgs,
                                    &pInst->vidDecCreateStatus,
                                    NULL
                                    );

    GT_assert( GT_DEFAULT_MASK, pInst->videoDecoderHandle!=NULL);

    vidDecChipIdArgs.deviceNum = 0;

    status = FVID2_control(
        pInst->videoDecoderHandle,
        IOCTL_VPS_VIDEO_DECODER_GET_CHIP_ID,
        &vidDecChipIdArgs,
        &vidDecChipIdStatus
        );

    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pInst->vidDecVideoModeArgs.videoIfMode = pVipCreateArgs->videoIfMode;
    pInst->vidDecVideoModeArgs.videoDataFormat = pVipCreateArgs->inDataFormat;
    pInst->vidDecVideoModeArgs.standard = pInstPrm->standard;
    pInst->vidDecVideoModeArgs.videoCaptureMode = pVipCreateArgs->videoCaptureMode;
    /* TVP7002 supports only ACTVID/VSYNC mode of operation in single channel
       discrete sync mode */
    if ((pInstPrm->videoDecoderId == FVID2_VPS_VID_DEC_TVP7002_DRV) &&
        (pVipCreateArgs->videoCaptureMode != VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC))
    {
        pInst->vidDecVideoModeArgs.videoCaptureMode =
            VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC;
    }
    pInst->vidDecVideoModeArgs.videoSystem = VPS_VIDEO_DECODER_VIDEO_SYSTEM_AUTO_DETECT;
    pInst->vidDecVideoModeArgs.videoAutoDetectTimeout = BIOS_WAIT_FOREVER;
    pInst->vidDecVideoModeArgs.videoCropEnable = FALSE;

    if(pInstPrm->videoDecoderId==FVID2_VPS_VID_DEC_TVP5158_DRV)
    {
        /* need to be in NTSC or PAL mode specifically in order for
            cable disconnect/connect to work reliably with TVP5158
         */
        if(pObj->isPalMode)
        {
            pInst->vidDecVideoModeArgs.videoSystem = VPS_VIDEO_DECODER_VIDEO_SYSTEM_PAL;
        }
        else
        {
            pInst->vidDecVideoModeArgs.videoSystem = VPS_VIDEO_DECODER_VIDEO_SYSTEM_NTSC;
        }

    }

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: VIP %d: VID DEC %d (0x%02x): %04x:%04x:%04x\n",
        Clock_getTicks(),
        pInstPrm->vipInstId,
        pInstPrm->videoDecoderId,
        pInst->vidDecCreateArgs.deviceI2cAddr[0],
        vidDecChipIdStatus.chipId,
        vidDecChipIdStatus.chipRevision,
        vidDecChipIdStatus.firmwareVersion
      );
    #endif

    return status;
}

Int32 CaptureLink_drvInstSetFrameSkip(CaptureLink_Obj *pObj, UInt16 instId, UInt32 frameSkipMask)
{
    Vps_CaptFrameSkip frameSkip;
    UInt16 outId, chId;
    Int32 status = FVID2_SOK;
    CaptureLink_InstObj *pInst;
    Vps_CaptCreateParams *pVipCreateArgs;

    pInst = &pObj->instObj[instId];
    pVipCreateArgs = &pInst->createArgs;

    /*
     * set frame skip using a IOCTL if enabled
     */
    for ( outId = 0; outId < pVipCreateArgs->numStream;
          outId++ )
    {
        for ( chId = 0; chId < pVipCreateArgs->numCh; chId++ )
        {

            frameSkip.channelNum =
                pVipCreateArgs->channelNumMap[outId][chId];

            frameSkip.frameSkipMask = frameSkipMask;

            status = FVID2_control ( pInst->captureVipHandle,
                                     IOCTL_VPS_CAPT_SET_FRAME_SKIP,
                                     &frameSkip, NULL );
            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
        }
    }

    return status;
}

/* Create capture driver */
Int32 CaptureLink_drvCreateInst(CaptureLink_Obj *pObj, UInt16 instId)
{
    CaptureLink_VipInstParams *pInstPrm;
    CaptureLink_InstObj *pInst;
    Vps_CaptCreateParams *pVipCreateArgs;
    Vps_CaptStorageParams storagePrms;
    Vps_CaptOutInfo *pVipOutPrm;
    CaptureLink_OutParams *pOutPrm;
    System_LinkChInfo *pQueChInfo;
    UInt16 queId, queChId, outId, chId, inWidth, inHeight;
    FVID2_ScanFormat inScanFormat;
    Int32 status;
    UInt32 platform, cpuRev;
    Vps_CaptVipCropParams vipCropPrms;
    Vps_VipPortConfig *vipPortCfg;

    platform = Vps_platformGetId();
    if ((platform <= VPS_PLATFORM_ID_UNKNOWN) ||
        (platform >= VPS_PLATFORM_ID_MAX))
    {
        GT_assert(GT_DEFAULT_MASK, FALSE);
    }

    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_UNKNOWN)
    {
        GT_assert(GT_DEFAULT_MASK,  FALSE);
    }

    pInstPrm = &pObj->createArgs.vipInst[instId];
    pInst = &pObj->instObj[instId];
    pVipCreateArgs = &pInst->createArgs;
    vipPortCfg = &pInst->vipPortCfg;
    memset ( vipPortCfg, 0, sizeof ( *vipPortCfg ) );

    pInst->instId = pInstPrm->vipInstId;

    CaptureLink_drvInitCreateArgs(pVipCreateArgs);

    pVipCreateArgs->inDataFormat = pInstPrm->inDataFormat;
    pObj->outSizeSwitchFrameCnt = 0u;

    if(pInstPrm->videoDecoderId==FVID2_VPS_VID_DEC_TVP5158_DRV)
    {
        if ((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
            (cpuRev == VPS_PLATFORM_CPU_REV_2_0))
        {
            /* Configure clock edge polarity to falling edge for TI816x 2.0 */
            vipPortCfg->ctrlChanSel = VPS_VIP_CTRL_CHAN_SEL_7_0;
            vipPortCfg->ancChSel8b = VPS_VIP_ANC_CH_SEL_8B_LUMA_SIDE;
            vipPortCfg->pixClkEdgePol = VPS_VIP_PIX_CLK_EDGE_POL_FALLING;

            vipPortCfg->invertFidPol = FALSE;
            vipPortCfg->embConfig.errCorrEnable = FALSE;
            vipPortCfg->embConfig.srcNumPos = VPS_VIP_SRC_NUM_POS_LS_NIBBLE_OF_CODEWORD;
            vipPortCfg->embConfig.isMaxChan3Bits = FALSE;

            pVipCreateArgs->vipParserPortConfig = vipPortCfg;
        }


        inScanFormat = FVID2_SF_INTERLACED;

        if(instId==0)
        {
            pObj->maxWidth = 720;
            if(pObj->isPalMode)
                pObj->maxHeight = 288;
            else
                pObj->maxHeight = 240;
        }
        inWidth = 720;
        if(pObj->isPalMode)
            inHeight = 288;
        else
            inHeight = 240;

        if(Vps_platformIsEvm())
        {
            #ifndef PLATFORM_ZEBU
            pVipCreateArgs->videoCaptureMode = VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC;
            #else
            pVipCreateArgs->videoCaptureMode = VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC;
            #endif
        }
        else
        {
            pVipCreateArgs->videoCaptureMode = VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC;
        }
        pVipCreateArgs->videoIfMode = VPS_CAPT_VIDEO_IF_MODE_8BIT;

        switch(pInstPrm->standard)
        {
            case FVID2_STD_CIF:
            case FVID2_STD_HALF_D1:
            case FVID2_STD_D1:
                pVipCreateArgs->numCh = 1;
                pVipCreateArgs->videoCaptureMode = VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC;
                break;

            case FVID2_STD_MUX_2CH_D1:
                pVipCreateArgs->numCh = 2;
                break;

            default:
            case FVID2_STD_MUX_4CH_CIF:
            case FVID2_STD_MUX_4CH_HALF_D1:
            case FVID2_STD_MUX_4CH_D1:
                pVipCreateArgs->numCh = 4;
                break;
        }
    }
    else
    {
        switch(pInstPrm->standard)
        {
            case FVID2_STD_1080I_60:
                inScanFormat = FVID2_SF_INTERLACED;
                if(pVipCreateArgs->inDataFormat==FVID2_DF_RGB24_888)
                    inScanFormat = FVID2_SF_PROGRESSIVE;  // since in 24-bit discrete sync mode FID signal is not connected in board
                if(instId==0)
                {
                    pObj->maxWidth = 1920;
                    pObj->maxHeight = 540;
                }
                inWidth = 1920;
                inHeight = 540;
                vipCropPrms.vipCropCfg.cropStartX = 0u;
                vipCropPrms.vipCropCfg.cropStartY = 0u;
                vipCropPrms.vipCropCfg.cropWidth = inWidth;
                vipCropPrms.vipCropCfg.cropHeight = inHeight;
                break;

            case FVID2_STD_720P_60:
                inScanFormat = FVID2_SF_PROGRESSIVE;
                if(instId==0)
                {
                    pObj->maxWidth = 1280;
                    pObj->maxHeight = 720;
                }
                inWidth = 1280;
                inHeight = 720;
                vipCropPrms.vipCropCfg.cropStartX = 0u;
                vipCropPrms.vipCropCfg.cropStartY = 0u;
                vipCropPrms.vipCropCfg.cropWidth = inWidth;
                vipCropPrms.vipCropCfg.cropHeight = inHeight;
                break;

            case FVID2_STD_480I:
                inScanFormat = FVID2_SF_INTERLACED;
                if(instId==0)
                {
                    pObj->maxWidth = 720;
                    if(pObj->isPalMode)
                        pObj->maxHeight = 288;
                    else
                        pObj->maxHeight = 240;
                }
                inWidth = 720;
                if(pObj->isPalMode)
                    inHeight = 288;
                else
                    inHeight = 240;
                vipCropPrms.vipCropCfg.cropStartX = 0u;
                vipCropPrms.vipCropCfg.cropStartY = 0u;
                vipCropPrms.vipCropCfg.cropWidth = inWidth;
                vipCropPrms.vipCropCfg.cropHeight = inHeight;
                break;

            default:
                inScanFormat = FVID2_SF_PROGRESSIVE;
                if(instId==0)
                {
                    pObj->maxWidth = 1920;
                    pObj->maxHeight = 1080;
                }
                inWidth = 1920;
                inHeight = 1080;
                vipCropPrms.vipCropCfg.cropStartX = 0u;
                vipCropPrms.vipCropCfg.cropStartY = 0u;
                vipCropPrms.vipCropCfg.cropWidth = inWidth;
                vipCropPrms.vipCropCfg.cropHeight = inHeight;
                break;
        }

        if(pVipCreateArgs->inDataFormat==FVID2_DF_RGB24_888)
        {
            pVipCreateArgs->videoCaptureMode =
                VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK;
            /* For TI814x/TI816x select VSYNC mode of operation for ES2.0 or
             * above */
            if (((platform == VPS_PLATFORM_ID_EVM_TI814x) &&
                 (cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
                ((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
                 (cpuRev >= VPS_PLATFORM_CPU_REV_2_0)) ||
                (platform == VPS_PLATFORM_ID_EVM_TI8107))
            {
                pVipCreateArgs->videoCaptureMode =
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC;
            }
            pVipCreateArgs->videoIfMode = VPS_CAPT_VIDEO_IF_MODE_24BIT;
        }
        else
        {
            pVipCreateArgs->videoCaptureMode = VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC;
            pVipCreateArgs->videoIfMode = VPS_CAPT_VIDEO_IF_MODE_16BIT;
        }
        pVipCreateArgs->numCh = 1;
    }

    Vps_printf(" %d: CAPTURE: %s capture mode is [%s, %s] !!! \n",
        Clock_getTicks(),
        gCaptureLink_portName[pInstPrm->vipInstId],
        gCaptureLink_ifName[pVipCreateArgs->videoIfMode],
        gCaptureLink_modeName[pVipCreateArgs->videoCaptureMode]
        );

    pVipCreateArgs->inScanFormat = inScanFormat;
    pVipCreateArgs->periodicCallbackEnable = TRUE;

    pVipCreateArgs->numStream = pInstPrm->numOutput;

    for(outId=0; outId<pVipCreateArgs->numStream; outId++)
    {
        pVipOutPrm = &pVipCreateArgs->outStreamInfo[outId];
        pOutPrm = &pInstPrm->outParams[outId];

        pVipOutPrm->dataFormat = pOutPrm->dataFormat;

        pVipOutPrm->memType = VPS_VPDMA_MT_NONTILEDMEM;

        if(pObj->createArgs.tilerEnable &&
            (pOutPrm->dataFormat == FVID2_DF_YUV420SP_UV ||
              pOutPrm->dataFormat == FVID2_DF_YUV422SP_UV )
          )
        {
            pVipOutPrm->memType = VPS_VPDMA_MT_TILEDMEM;
        }

        pVipOutPrm->pitch[0] = VpsUtils_align(pObj->maxWidth, VPS_BUFFER_ALIGNMENT*2);
        if(pVipOutPrm->dataFormat==FVID2_DF_YUV422I_YUYV)
            pVipOutPrm->pitch[0] *= 2;
        if(pVipOutPrm->dataFormat==FVID2_DF_RGB24_888)
            pVipOutPrm->pitch[0] *= 3;

        pVipOutPrm->pitch[1] = pVipOutPrm->pitch[0];

        if(CaptureLink_drvIsDataFormatTiled(pVipCreateArgs, outId))
        {
            pVipOutPrm->pitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
            pVipOutPrm->pitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
        }

        pVipOutPrm->pitch[2] = 0;

        pVipOutPrm->scEnable = pOutPrm->scEnable;

        if (pObj->maxWidth <= 352)
        {
            pVipOutPrm->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_352_PIXELS;
        }
        else if (pObj->maxWidth <= 768)
        {
            pVipOutPrm->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_768_PIXELS;
        }
        else if (pObj->maxWidth <= 1280)
        {
            pVipOutPrm->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_1280_PIXELS;
        }
        else if (pObj->maxWidth <= 1920)
        {
            pVipOutPrm->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_1920_PIXELS;
        }
        else
        {
            pVipOutPrm->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_UNLIMITED;
        }

        if (pObj->maxHeight<=288)
        {
            pVipOutPrm->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_288_LINES;
        }
        else if (pObj->maxHeight<=576)
        {
            pVipOutPrm->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_576_LINES;
        }
        else if(pObj->maxHeight<=720)
        {
            pVipOutPrm->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_720_LINES;
        }
        else if (pObj->maxHeight<=1080)
        {
            pVipOutPrm->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_1080_LINES;
        }
        else
        {
            pVipOutPrm->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;
        }

        if(pVipOutPrm->scEnable)
        {
            Vps_CaptScParams *pScParams;

            pScParams = &pVipCreateArgs->scParams;

            pScParams->inScanFormat = FVID2_SF_PROGRESSIVE; // NOT USED
            pScParams->inWidth = inWidth;
            pScParams->inHeight = inHeight;
            pScParams->inCropCfg.cropStartX = 0;
            pScParams->inCropCfg.cropStartY = 0;
            pScParams->inCropCfg.cropWidth = (pScParams->inWidth & 0xFFFE);
            pScParams->inCropCfg.cropHeight = (pScParams->inHeight & 0xFFFE);
            pScParams->outWidth = pOutPrm->scOutWidth;
            if(pObj->isPalMode)
            {
                /* input source is PAL mode
                    scOutHeight is setup assuming NTSC so convert
                    it to 288 lines for PAL mode
                */
                if(pOutPrm->scOutHeight==240)
                    pOutPrm->scOutHeight = 288;
            }
            pScParams->outHeight = pOutPrm->scOutHeight;
            pScParams->scConfig = NULL;
            pScParams->scCoeffConfig = NULL;
            pScParams->enableCoeffLoad = TRUE;
        }

        for(chId=0; chId<pVipCreateArgs->numCh; chId++)
        {
            queId = pOutPrm->outQueId;
            queChId = pObj->info.queInfo[queId].numCh;

            pQueChInfo = &pObj->info.queInfo[queId].chInfo[queChId];

            pQueChInfo->dataFormat = (FVID2_DataFormat)pVipOutPrm->dataFormat;
            pQueChInfo->memType    = (Vps_VpdmaMemoryType)pVipOutPrm->memType;

            if(pVipOutPrm->scEnable)
            {
                pQueChInfo->width      = pOutPrm->scOutWidth;
                pQueChInfo->height     = pOutPrm->scOutHeight;
            }
            else
            {
                pQueChInfo->width      = pObj->maxWidth;
                pQueChInfo->height     = pObj->maxHeight;
            }

            pQueChInfo->pitch[0]   = pVipOutPrm->pitch[0];
            pQueChInfo->pitch[1]   = pVipOutPrm->pitch[1];
            pQueChInfo->pitch[2]   = pVipOutPrm->pitch[2];
            pQueChInfo->scanFormat = inScanFormat;
            if (TRUE == pInstPrm->frameCaptureMode)
            {
                pQueChInfo->bufferFmt = FVID2_BUF_FMT_FRAME;
                pQueChInfo->height     = (UInt16)pObj->maxHeight * 2;
            }
            else
            {
                pQueChInfo->bufferFmt = FVID2_BUF_FMT_FIELD;
            }

            pObj->info.queInfo[queId].numCh++;

            pVipCreateArgs->channelNumMap[outId][chId] =
                CaptureLink_makeChannelNum(queId, queChId, pObj->numChPerQue);
        }
        pVipOutPrm->subFrameModeEnable = pOutPrm->subFrameEnable;
    }

    memset(&pInst->cbPrm, 0, sizeof(pInst->cbPrm));

    pInst->cbPrm.appData = pObj;

    if(instId==0)
        pInst->cbPrm.cbFxn = CaptureLink_drvCallback;

    if (Vps_platformIsSim())
    {
        CaptureLink_drvSimVideoSourceSelect(
            pInst->instId,
            pVipCreateArgs->videoCaptureMode,
            pVipCreateArgs->videoIfMode);
    }

    pObj->enableCheckOverflowDetect = TRUE;

    pInst->captureVipHandle = FVID2_create(
                                FVID2_VPS_CAPT_VIP_DRV,
                                pInst->instId,
                                pVipCreateArgs,
                                &pInst->createStatus,
                                &pInst->cbPrm
                                );

    GT_assert( GT_DEFAULT_MASK, pInst->captureVipHandle!=NULL);
    if (TRUE == pInstPrm->frameCaptureMode)
    {
        storagePrms.channelNum = 0;
        storagePrms.bufferFmt = FVID2_BUF_FMT_FRAME;
        if (TRUE == pInstPrm->fieldsMerged)
        {
            storagePrms.fieldMerged = TRUE;
        }
        else
        {
            storagePrms.fieldMerged = FALSE;
        }
        status = FVID2_control (pInst->captureVipHandle,
                                IOCTL_VPS_CAPT_SET_STORAGE_FMT,
                                &storagePrms, NULL  );
        GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK );
    }

    /* Use VIP trimmer for discrete sync mode for TI816x PG2.0 */
    if ((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
        (cpuRev >= VPS_PLATFORM_CPU_REV_2_0) &&
        (pInstPrm->videoDecoderId != FVID2_VPS_VID_DEC_TVP5158_DRV))
    {
        vipCropPrms.channelNum = 0u;
        /* Enable VIP trimmer in discrete sync mode */
        if ((pVipCreateArgs->videoCaptureMode ==
                VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK) ||
            (pVipCreateArgs->videoCaptureMode ==
                VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC))
        {

            vipCropPrms.vipCropEnable = TRUE;
        }
        else
        {
            vipCropPrms.vipCropEnable = FALSE;
        }
        status = FVID2_control(
                     pInst->captureVipHandle,
                     IOCTL_VPS_CAPT_SET_VIP_CROP_CFG,
                     &vipCropPrms, NULL);
        GT_assert(GT_DEFAULT_MASK, (status == FVID2_SOK));
    }

    CaptureLink_drvAllocAndQueueFrames(pObj, pInst,pInstPrm->frameCaptureMode);

    CaptureLink_drvInstSetFrameSkip(pObj, instId, 0);

    pInst->videoDecoderHandle = NULL;

    #ifdef SYSTEM_USE_VIDEO_DECODER
    CaptureLink_drvCreateVideoDecoder(pObj, instId);
    #endif

    return FVID2_SOK;
}

/* Create capture link

    This creates
    - capture driver
    - video decoder driver
    - allocate and queues frames to the capture driver
    - DOES NOT start the capture ONLY make it ready for operation
*/
Int32 CaptureLink_drvCreate(CaptureLink_Obj *pObj, CaptureLink_CreateParams *pPrm)
{
    Int32 status;
    UInt32 queId, instId;
    UInt32 platform, cpuRev;

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Create in progress !!!\n", Clock_getTicks());
    #endif

    memcpy(&pObj->createArgs, pPrm, sizeof(*pPrm));

    pObj->captureDequeuedFrameCount = 0;
    pObj->captureQueuedFrameCount   = 0;
    pObj->cbCount                   = 0;
    pObj->cbCountServicedCount      = 0;
    pObj->prevFrameCount            = 0;
    pObj->totalCpuLoad              = 0;
    pObj->resetCount                = 0;
    pObj->resetTotalTime            = 0;
    pObj->prevResetTime             = 0;
    pObj->isPalMode                 = FALSE;

    pObj->brightness = 0x1c; /* TUNED for specific scene's, to make black blacker */
    pObj->contrast = 0x89; /* TUNED for specific scene's, to make black blacker */
    pObj->saturation = 128; /* default */

    /* The number of channels of each queue is equal to all the queues, for the
       moment */
    pObj->numChPerQue =
        CAPTURE_LINK_MAX_CH_PER_OUT_QUE / pObj->createArgs.numOutQue;

    memset(pObj->captureFrameCount, 0, sizeof(pObj->captureFrameCount));

    pObj->info.numQue = CAPTURE_LINK_MAX_OUT_QUE;

    #ifdef SYSTEM_USE_VIDEO_DECODER
    CaptureLink_drvDetectVideoStandard(pObj);
    #endif

    platform = Vps_platformGetId();
    if ((platform <= VPS_PLATFORM_ID_UNKNOWN) ||
        (platform >= VPS_PLATFORM_ID_MAX))
    {
        GT_assert(GT_DEFAULT_MASK, FALSE);
    }

    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_UNKNOWN)
    {
        GT_assert(GT_DEFAULT_MASK,  FALSE);
    }

    for(queId=0; queId<CAPTURE_LINK_MAX_OUT_QUE ; queId++)
    {
        status = VpsUtils_bufCreate(&pObj->bufQue[queId], FALSE, FALSE);
        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        pObj->info.queInfo[queId].numCh = 0;
    }

    /*
     * Create global VIP capture handle, used for dequeue,
     * queue from all active captures
     */
    pObj->fvidHandleVipAll = FVID2_create(
                                        FVID2_VPS_CAPT_VIP_DRV,
                                        VPS_CAPT_INST_VIP_ALL,
                                        NULL,
                                        NULL,
                                        NULL
                                      );
    GT_assert( GT_DEFAULT_MASK,  pObj->fvidHandleVipAll != NULL );

    if(CAPTURE_LINK_TMP_BUF_SIZE)
    {
       pObj->tmpBufAddr = VpsUtils_memAlloc(CAPTURE_LINK_TMP_BUF_SIZE, 32);
    }

    #if 0
    status = FVID2_control( pObj->fvidHandleVipAll,
                   IOCTL_VPS_CAPT_DROP_DATA_BUFFER,
                   pObj->tmpBufAddr, NULL
                 );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
    #endif

    status = FVID2_control( pObj->fvidHandleVipAll,
                   IOCTL_VPS_CAPT_RESET_VIP0,
                   NULL, NULL
                 );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    status = FVID2_control( pObj->fvidHandleVipAll,
                   IOCTL_VPS_CAPT_RESET_VIP1,
                   NULL, NULL
                 );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    System_clearVipResetFlag(SYSTEM_VIP_0);
    System_clearVipResetFlag(SYSTEM_VIP_1);

    for(instId = 0; instId < pPrm->numVipInst; instId++)
    {
        CaptureLink_drvCreateInst(pObj, instId);
    }

    #if 0
    /* set user defined values at TVP5158 for brightness/contrast/saturation */
    CaptureLink_drvSetColor(pObj, 0, 0, 0);
    #endif

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Create Done !!!\n", Clock_getTicks());
    #endif

    return status;
}

Int32 CaptureLink_drvDetectVideo(CaptureLink_Obj *pObj, UInt32 timeout)
{
    Int32 status;
    UInt32 instId;

    CaptureLink_InstObj *pInst;

    #ifdef SYSTEM_DO_VIDEO_DETECT
    Vps_VideoDecoderVideoStatusParams videoStatusArgs;
    Vps_VideoDecoderVideoStatus videoStatus;
    UInt32 chId, repeatCnt;
    #endif

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Detect video in progress !!!\n", Clock_getTicks());
    #endif

    for(instId=0; instId < pObj->createArgs.numVipInst; instId++ )
    {
        pInst = &pObj->instObj[instId];

        if(pInst->videoDecoderHandle==NULL)
            return FVID2_SOK;

        pInst->vidDecVideoModeArgs.videoAutoDetectTimeout = timeout;

        status = FVID2_control(
            pInst->videoDecoderHandle,
            IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_MODE,
            &pInst->vidDecVideoModeArgs,
            NULL
            );

        GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

        #ifdef SYSTEM_DO_VIDEO_DETECT
        for(chId=0; chId<pInst->createArgs.numCh; chId++)
        {
            repeatCnt = 5;
            while (--repeatCnt != 0)
            {
                videoStatusArgs.channelNum = chId;
                status = FVID2_control(
                             pInst->videoDecoderHandle,
                             IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS,
                             &videoStatusArgs,
                             &videoStatus);
                GT_assert(GT_DEFAULT_MASK, (status == FVID2_SOK));
                if (videoStatus.isVideoDetect)
                {
                    #ifdef SYSTEM_DEBUG_CAPTURE
                    Vps_printf(" %d: CAPTURE: Detected video at CH%d (%dx%d@%dHz, %d)!!!\n",
                        Clock_getTicks(),
                        chId,
                        videoStatus.frameWidth,
                        videoStatus.frameHeight,
                        1000000/videoStatus.frameInterval,
                        videoStatus.isInterlaced);
                    #endif

                    #ifdef SYSTEM_USE_VIDEO_DETECT_INFO
                    {
                        UInt32 outId, queId, queChId;
                        Vps_CaptOutInfo *pVipOutPrm;
                        System_LinkChInfo *pQueChInfo;
                        Vps_CaptCreateParams *pVipCreateArgs;

                        pVipCreateArgs = &pInst->createArgs;

                        for(outId=0; outId<pInst->createArgs.numStream; outId++)
                        {
                            pVipOutPrm = &pVipCreateArgs->outStreamInfo[outId];

                            if(pVipOutPrm->scEnable)
                            {
                                Vps_CaptScParams *pScParams;

                                pScParams = &pVipCreateArgs->scParams;

                                pScParams->inWidth = videoStatus.frameWidth;
                                pScParams->inHeight = videoStatus.frameHeight;
                                pScParams->inCropCfg.cropWidth =
                                    pScParams->inWidth;
                                pScParams->inCropCfg.cropHeight =
                                    pScParams->inHeight;
                                pScParams->enableCoeffLoad = TRUE;

                                status = FVID2_control(
                                             pInst->captureVipHandle,
                                             IOCTL_VPS_CAPT_SET_SC_PARAMS,
                                             pScParams,
                                             NULL);
                                GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
                            }

                            queId = CaptureLink_getQueId(pVipCreateArgs->channelNumMap[outId][chId]);
                            queChId = CaptureLink_getQueChId(pVipCreateArgs->channelNumMap[outId][chId]);

                            GT_assert( GT_DEFAULT_MASK, queId<pObj->info.numQue);
                            GT_assert( GT_DEFAULT_MASK,
                                queChId<pObj->info.queInfo[queId].numCh);

                            pQueChInfo =
                                &pObj->info.queInfo[queId].chInfo[queChId];

                            if(videoStatus.isInterlaced)
                                pQueChInfo->scanFormat = FVID2_SF_INTERLACED;
                            else
                                pQueChInfo->scanFormat = FVID2_SF_PROGRESSIVE;

                            if(!pVipOutPrm->scEnable)
                            {
                                pQueChInfo->width = videoStatus.frameWidth;
                                pQueChInfo->height = videoStatus.frameHeight;
                            }
                        }
                    }
                    #endif
                    break;
                }
                Task_sleep(100);
            }
        }
        #endif
    }

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Detect video Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}



Int32 CaptureLink_drvOverflowDetectAndReset(CaptureLink_Obj *pObj, Bool doForceReset)
{
    Vps_CaptOverFlowStatus  overFlowStatus;
    UInt32 curTime = 0u;

    #if 0
    {
        UInt32 elaspedTime;

        elaspedTime = Clock_getTicks() - pObj->startTime ;

        if( (elaspedTime - pObj->prevResetTime) > 5*1000)
        {
            pObj->prevResetTime = elaspedTime;
            doForceReset = TRUE;
        }
    }
    #endif

    FVID2_control( pObj->fvidHandleVipAll,
               IOCTL_VPS_CAPT_CHECK_OVERFLOW,
               NULL, &overFlowStatus
             );

    if(doForceReset)
    {
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA] = TRUE;
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB] = TRUE;
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA] = TRUE;
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB] = TRUE;
    }

    if(overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB]
    )
    {
        //System_haltExecution();

        curTime = Clock_getTicks();

        pObj->resetCount++;
    }

    if(overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB]
    )
    {
        System_lockVip(SYSTEM_VIP_0);

        Vps_rprintf(" %d: CAPTURE: Overflow detected on VIP0, Total Resets = %d\n", Clock_getTicks(), pObj->resetCount);
    }
    if(overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB]
    )
    {
        System_lockVip(SYSTEM_VIP_1);

        Vps_rprintf(" %d: CAPTURE: Overflow detected on VIP1, Total Resets = %d\n", Clock_getTicks(), pObj->resetCount);
    }

    FVID2_control( pObj->fvidHandleVipAll,
               IOCTL_VPS_CAPT_RESET_AND_RESTART,
               &overFlowStatus, NULL
             );

    if(overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB]
    )
    {
        System_setVipResetFlag(SYSTEM_VIP_0);
        System_unlockVip(SYSTEM_VIP_0);
    }
    if(overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB]
    )
    {
        System_setVipResetFlag(SYSTEM_VIP_1);
        System_unlockVip(SYSTEM_VIP_1);
    }
    if(overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA]
        ||
        overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB]
    )
    {
        curTime = Clock_getTicks() - curTime;
        pObj->resetTotalTime += curTime;
    }

    return FVID2_SOK;
}

Int32 CaptureLink_drvPrintRtStatus(CaptureLink_Obj *pObj, UInt32 frameCount, UInt32 elaspedTime)
{
    UInt32 fps = (frameCount*100)/(elaspedTime/10);

    Vps_rprintf(" %d: CAPTURE: Fields = %d (fps = %d), Total Resets = %d (Avg %d ms per reset)\r\n",
        Clock_getTicks(),
        frameCount,
        fps,
        pObj->resetCount,
        pObj->resetTotalTime/pObj->resetCount
    );

    System_displayUnderflowPrint(TRUE, FALSE);

    return 0;
}

Int32 CaptureLink_drvProcessData(CaptureLink_Obj *pObj)
{
    Int scEnable = FALSE;
    UInt32 frameId, queId, streamId, queChId, elaspedTime, instId;
    FVID2_FrameList frameList;
    FVID2_Frame *pFrame;
    volatile UInt32 sendMsgToTsk=0, tmpValue;
    Int32 status;
    CaptureLink_InstObj *pInst;
    Vps_CaptScParams *pScParams;
    Vps_CaptCreateParams *pVipCreateArgs;

    pObj->cbCountServicedCount++;

    System_displayUnderflowCheck(FALSE);

    for ( streamId = 0; streamId < CAPTURE_LINK_MAX_OUTPUT_PER_INST;
          streamId++ )
    {
        /*
         * Deque frames for all active handles
         */
        FVID2_dequeue ( pObj->fvidHandleVipAll,
                        &frameList, streamId, BIOS_NO_WAIT );

        if ( frameList.numFrames )
        {
            for(frameId=0; frameId<frameList.numFrames; frameId++)
            {
                pFrame = frameList.frames[frameId];

                queId   = CaptureLink_getQueId(
                            pFrame->channelNum,
                            pObj->numChPerQue);
                queChId = CaptureLink_getQueChId(
                            pFrame->channelNum,
                            pObj->numChPerQue);

                GT_assert( GT_DEFAULT_MASK, queId<CAPTURE_LINK_MAX_OUT_QUE);
                GT_assert( GT_DEFAULT_MASK, queChId<CAPTURE_LINK_MAX_CH_PER_OUT_QUE);

                pObj->captureDequeuedFrameCount++;
                pObj->captureFrameCount[queId][queChId]++;

                tmpValue = (UInt32)pFrame->reserved;
                if(tmpValue>0)
                {
                    Vps_printf(" %d: CAPTURE: Dequeued frame more than once (%d,%d, %08x) \n", Clock_getTicks(), queId, queChId, pFrame->addr[0][0]);
                }
                tmpValue++;
                pFrame->reserved = (Ptr)tmpValue;


                pFrame->perFrameCfg = NULL;
                pFrame->channelNum = queChId;

                sendMsgToTsk |= (1<<queId);

                status = VpsUtils_bufPutFullFrame(&pObj->bufQue[queId], pFrame);
                GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);
            }

            #ifdef SYSTEM_DEBUG_CAPTURE_RT
            Vps_printf(" %d: CAPTURE: Dequeued %d frames !!!\n", Clock_getTicks(), frameList.numFrames);
            #endif
        }
    }

    if (TRUE == pObj->createArgs.captureScSwitchEnable)
    {
        /* Change VIP scaler config every CAPTURE_LINK_OUT_SIZE_SWITCH_FREQ
         * frames.
         */
        if ( ((pObj->captureDequeuedFrameCount %
                CAPTURE_LINK_OUT_SIZE_SWITCH_FREQ) == 0u) &&
             (pObj->outSizeSwitchFrameCnt != pObj->captureDequeuedFrameCount))
        {
            for(instId = 0; instId < pObj->createArgs.numVipInst; instId++)
            {
                pInst = &pObj->instObj[instId];
                pVipCreateArgs = &pInst->createArgs;
                for (streamId = 0;
                     streamId < CAPTURE_LINK_MAX_OUTPUT_PER_INST;
                     streamId++ )
                {
                    if (TRUE == pVipCreateArgs->outStreamInfo[streamId].scEnable)
                    {
                        scEnable = TRUE;
                        break;
                    }
                }

                if (TRUE == scEnable)
                {
                    pScParams = &(pVipCreateArgs->scParams);
                    /* Switch the output sizes down & up randomly */
                    if ((rand() % 2) == 0u)
                    {
                        pScParams->outWidth =
                            VpsUtils_align((pScParams->outWidth / 2),
                                           VPS_BUFFER_ALIGNMENT);
                        /* Minimum horizontal scaling ratio is 1/16 */
                        if (pScParams->outWidth <
                                (pScParams->inCropCfg.cropWidth / 16u))
                        {
                            pScParams->outWidth =
                                pScParams->inCropCfg.cropWidth / 16u;
                        }

                        pScParams->outHeight =
                            VpsUtils_align((pScParams->outHeight / 2),
                                           VPS_BUFFER_ALIGNMENT);
                    }
                    else
                    {
                        /* Increase the outWidth subject to max. limit of
                         * cropWidth since up-scaling is not permitted in VIP
                         * inline scaler.
                         */
                        pScParams->outWidth = pScParams->outWidth * 2;
                        pScParams->outWidth =
                            pScParams->outWidth > pScParams->inCropCfg.cropWidth ?
                            pScParams->inCropCfg.cropWidth : pScParams->outWidth;

                        /* Increase the outHeight subject to max. limit of
                         * cropHeight since up-scaling is not permitted in VIP
                         * inline scaler.
                         */
                        pScParams->outHeight = pScParams->outHeight * 2;
                        pScParams->outHeight =
                            pScParams->outHeight > pScParams->inCropCfg.cropHeight ?
                            pScParams->inCropCfg.cropHeight : pScParams->outHeight;
                    }

                    /* Enable scaler coefficient load. */
                    pScParams->enableCoeffLoad = TRUE;

                    pObj->outSizeSwitchFrameCnt = pObj->captureDequeuedFrameCount;
                    status = FVID2_control(pInst->captureVipHandle,
                                           IOCTL_VPS_CAPT_SET_SC_PARAMS,
                                           pScParams,
                                           NULL);
                    GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
                }
            }
        }
    }

    elaspedTime = Clock_getTicks() - pObj->startTime;

    if((elaspedTime - pObj->prevTime) > 30*1000)
    {
        CaptureLink_getCpuLoad();

        CaptureLink_drvPrintRtStatus(pObj,
            pObj->captureDequeuedFrameCount - pObj->prevFrameCount,
            elaspedTime - pObj->prevTime
            );

        pObj->prevFrameCount = pObj->captureDequeuedFrameCount;
        pObj->prevTime = elaspedTime;
    }

    for(queId=0; queId<CAPTURE_LINK_MAX_OUT_QUE; queId++)
    {
        if(sendMsgToTsk & 0x1)
        {
            /* send data available message to next tsk  */
            System_sendLinkCmd(
                    pObj->createArgs.outQueParams[queId].nextLink,
                    SYSTEM_CMD_NEW_DATA
                );
        }

        sendMsgToTsk >>= 1;
        if(sendMsgToTsk==0)
            break;
    }

    if(pObj->enableCheckOverflowDetect)
    {
        CaptureLink_drvOverflowDetectAndReset(pObj, FALSE);
    }

    pObj->exeTime = Clock_getTicks() - pObj->startTime;

    return FVID2_SOK;
}

Int32 CaptureLink_drvPutEmptyFrames(CaptureLink_Obj *pObj, FVID2_FrameList *pFrameList)
{
    UInt32 frameId;
    FVID2_Frame *pFrame;
    System_FrameInfo *pFrameInfo;
    volatile UInt32 tmpValue;

    if(pFrameList->numFrames)
    {
        for(frameId=0; frameId<pFrameList->numFrames; frameId++)
        {
            pFrame = pFrameList->frames[frameId];

            tmpValue = (UInt32)pFrame->reserved;
            tmpValue--;
            pFrame->reserved = (Ptr)tmpValue;

            pFrameInfo = (System_FrameInfo*)pFrame->appData;
            GT_assert( GT_DEFAULT_MASK, pFrameInfo!=NULL);

            pFrame->perFrameCfg = &pFrameInfo->captureRtParams;
            pFrame->channelNum = pFrameInfo->captureChannelNum;
        }

        #ifdef SYSTEM_DEBUG_CAPTURE_RT
        Vps_printf(" %d: CAPTURE: Queued back %d frames !!!\n", Clock_getTicks(), pFrameList->numFrames);
        #endif

        pObj->captureQueuedFrameCount += pFrameList->numFrames;

        FVID2_queue ( pObj->fvidHandleVipAll,
                      pFrameList,
                      VPS_CAPT_STREAM_ID_ANY
                    );
    }

    return FVID2_SOK;
}

Int32 CaptureApp_cfgTimeStamp (FVID2_Handle handle)
{
    Int32 status;
    Vps_CaptFrameTimeStampParms tsParams;

    tsParams.enableBestEffort = TRUE;
    tsParams.timeStamper = NULL;
    tsParams.expectedFps = 60;

    status = FVID2_control(
        handle,
        IOCTL_VPS_CAPT_CFG_TIME_STAMPING_FRAMES,
        &tsParams,
        NULL
        );

    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    return status;

}


Int32 CaptureLink_drvStart(CaptureLink_Obj *pObj)
{
    UInt32 instId;
    CaptureLink_InstObj *pInstObj;

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Start in progress !!!\n", Clock_getTicks());
    #endif

    for(instId=0; instId<pObj->createArgs.numVipInst; instId++)
    {
        pInstObj = &pObj->instObj[instId];

        /* video decoder */
        if(pInstObj->videoDecoderHandle)
            FVID2_start(pInstObj->videoDecoderHandle, NULL);

    }

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Start Done !!!\n", Clock_getTicks());
    #endif

    #ifdef PLATFORM_ZEBU
    Vps_printf(" %d: CAPTURE: Start Done !!!\n", Clock_getTicks());
    #endif

    #ifdef PLATFORM_EVM_SI
#ifndef PLATFORM_ZEBU
    Task_sleep(1000);
#endif
    #endif

    pObj->prevTime = pObj->startTime = Clock_getTicks();

    for(instId=0; instId<pObj->createArgs.numVipInst; instId++)
    {
        pInstObj = &pObj->instObj[instId];

        CaptureApp_cfgTimeStamp(pInstObj->captureVipHandle);
        /* VIP capture */
        FVID2_start(pInstObj->captureVipHandle, NULL);
    }

    return FVID2_SOK;
}

Int32 CaptureLink_drvFlush(FVID2_Handle captureVipHandle, char *portName)
{
    Int32 status;
    FVID2_FrameList frameList;

    do
    {
        status = FVID2_control(
            captureVipHandle,
            IOCTL_VPS_CAPT_FLUSH,
            NULL,
            &frameList
            );

        #ifdef SYSTEM_DEBUG_CAPTURE
        Vps_rprintf(" %d: CAPTURE: %s: Flushed %d frames.\n",
            Clock_getTicks(),
            portName,
            frameList.numFrames
        );
        #endif

    } while(frameList.numFrames!=0 && status == FVID2_SOK );

    if(status!=FVID2_SOK)
    {
        #ifdef SYSTEM_DEBUG_CAPTURE
        Vps_rprintf(" %d: CAPTURE: %s: Flushing ... ERROR !!!\n",
            Clock_getTicks(),
            portName
        );
        #endif
    }

    return FVID2_SOK;
}

Int32 CaptureLink_drvStop(CaptureLink_Obj *pObj)
{
    UInt32 instId;
    CaptureLink_InstObj *pInstObj;


    for(instId=0; instId<pObj->createArgs.numVipInst; instId++)
    {
        pInstObj = &pObj->instObj[instId];

        /* VIP capture */
        FVID2_stop(pInstObj->captureVipHandle, NULL);

        CaptureLink_drvFlush(pInstObj->captureVipHandle, gCaptureLink_portName[pInstObj->instId]);
    }

    pObj->exeTime = Clock_getTicks() - pObj->startTime;

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Stop in progress !!!\n", Clock_getTicks());
    #endif

    for(instId=0; instId<pObj->createArgs.numVipInst; instId++)
    {
        pInstObj = &pObj->instObj[instId];

        /* video decoder */
        if(pInstObj->videoDecoderHandle)
            FVID2_stop(pInstObj->videoDecoderHandle, NULL);
    }

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Stop Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}

Int32 CaptureLink_drvDelete(CaptureLink_Obj *pObj)
{
    UInt32 instId;
    UInt32 queId;
    CaptureLink_InstObj *pInstObj;

    #ifdef SYSTEM_DEBUG_CAPTURE
    CaptureLink_drvPrintStatus(pObj);
    #endif

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Delete in progress !!!\n", Clock_getTicks());
    #endif

    if(CAPTURE_LINK_TMP_BUF_SIZE)
    {
       VpsUtils_memFree(pObj->tmpBufAddr, CAPTURE_LINK_TMP_BUF_SIZE);
    }

    for(instId=0; instId<pObj->createArgs.numVipInst; instId++)
    {
        pInstObj = &pObj->instObj[instId];

        /* VIP capture */
        FVID2_delete(pInstObj->captureVipHandle, NULL);

        if(pInstObj->videoDecoderHandle)
        {
            /* video decoder */
            FVID2_delete(pInstObj->videoDecoderHandle, NULL);
        }

        CaptureLink_drvFreeFrames(pObj, pInstObj,
                                  pObj->createArgs.vipInst[instId].frameCaptureMode);
    }

    FVID2_delete ( pObj->fvidHandleVipAll, NULL );

    for(queId=0; queId<CAPTURE_LINK_MAX_OUT_QUE; queId++)
    {
        VpsUtils_bufDelete(&pObj->bufQue[queId]);
    }

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: Delete Done !!!\n", Clock_getTicks());
    #endif

    return FVID2_SOK;
}

/*
  Allocate and queue frames to driver

  pDrvObj - capture driver information
*/
Int32 CaptureLink_drvAllocAndQueueFrames( CaptureLink_Obj *pObj,
                                          CaptureLink_InstObj * pDrvObj,
                                          UInt32 frameCaptureMode)
{
    Int32 status;
    UInt16 streamId, chId, frameId, idx;
    Vps_CaptOutInfo *pOutInfo;
    FVID2_Frame *frames;
    System_FrameInfo *pFrameInfo;
    FVID2_FrameList frameList;
    FVID2_Format format;
    UInt32       yField1Offset, cbCrField0Offset, cbCrField1Offset;

    /*
     * init frameList for list of frames that are queued per CH to driver
     */
    frameList.perListCfg = NULL;
    frameList.reserved = NULL;

    /*
     * for every stream and channel in a capture handle
     */
    for ( streamId = 0; streamId < pDrvObj->createArgs.numStream; streamId++ )
    {
        for ( chId = 0; chId < pDrvObj->createArgs.numCh; chId++ )
        {

            pOutInfo = &pDrvObj->createArgs.outStreamInfo[streamId];

            /*
             * base index for pDrvObj->frames[] and pDrvObj->frameInfo[]
             */
            idx =
                VPS_CAPT_CH_PER_PORT_MAX * CAPTURE_LINK_FRAMES_PER_CH *
                streamId + CAPTURE_LINK_FRAMES_PER_CH * chId;
            if (idx >= CAPTURE_LINK_MAX_FRAMES_PER_HANDLE)
            {
                idx = 0u;
            }

            pFrameInfo = &pDrvObj->frameInfo[idx];
            frames = &pDrvObj->frames[idx];

            /* fill format with channel specific values  */
            format.channelNum = pDrvObj->createArgs.channelNumMap[streamId][chId];
            format.width = pObj->maxWidth;
            format.height = pObj->maxHeight+CAPTURE_LINK_HEIGHT_PAD_LINES;
            /* For frame capture mode we will be capturing both the fields
             * So height should be double
             */
            if (TRUE == frameCaptureMode)
            {
                format.height *= 2;
            }
            format.pitch[0] = pOutInfo->pitch[0];
            format.pitch[1] = pOutInfo->pitch[1];
            format.pitch[2] = pOutInfo->pitch[2];
            format.fieldMerged[0] = FALSE;
            format.fieldMerged[1] = FALSE;
            format.fieldMerged[2] = FALSE;
            format.dataFormat = pOutInfo->dataFormat;
            format.scanFormat = FVID2_SF_PROGRESSIVE;
            format.bpp = FVID2_BPP_BITS8; /* ignored */

            if(format.dataFormat==FVID2_DF_RAW_VBI)
            {
                format.height = CAPTURE_LINK_RAW_VBI_LINES;
            }

            /*
             * alloc memory based on 'format'
             * Allocated frame info is put in frames[]
             * CAPTURE_LINK_APP_FRAMES_PER_CH is the number of buffers per channel to
             * allocate
             */
            if ( CaptureLink_drvIsDataFormatTiled
                 ( &pDrvObj->createArgs, streamId ) )
            {
                VpsUtils_tilerFrameAlloc ( &format, frames,
                                           CAPTURE_LINK_FRAMES_PER_CH );
            }
            else
            {
                VpsUtils_memFrameAlloc ( &format, frames,
                                         CAPTURE_LINK_FRAMES_PER_CH );
            }

            /*
             * Set rtParams for every frame in perFrameCfg
             */
            for ( frameId = 0; frameId < CAPTURE_LINK_FRAMES_PER_CH; frameId++ )
            {
                frames[frameId].perFrameCfg = &pFrameInfo[frameId].captureRtParams;
                frames[frameId].subFrameInfo   = NULL;
                frames[frameId].appData = &pFrameInfo[frameId];
                frames[frameId].reserved = NULL;

                pFrameInfo[frameId].captureChannelNum = frames[frameId].channelNum;

                frameList.frames[frameId] = &frames[frameId];
                if (TRUE == frameCaptureMode)
                {
                    /* Since VpsUtils_memFrameAlloc is setting the address for only
                     * even field set addresses for odd fields.
                     */
                    if (FVID2_DF_YUV422I_YUYV == format.dataFormat)
                    {
                        yField1Offset = (UInt32)frames[frameId].addr[0][0] + format.pitch[0];
                        frames[frameId].addr[1][0] = (Ptr)yField1Offset;
                    }
                    if (FVID2_DF_YUV420SP_UV == format.dataFormat)
                    {
                        yField1Offset = (UInt32)frames[frameId].addr[0][0] + format.pitch[0];
                        cbCrField0Offset =(UInt32)((UInt32)frames[frameId].addr[0][0] +
                            (format.pitch[0] * pObj->maxHeight * 2));
                        cbCrField1Offset =(UInt32)(cbCrField0Offset + format.pitch[0]);
                        frames[frameId].addr[0][1] = (Ptr)cbCrField0Offset;
                        frames[frameId].addr[1][0] = (Ptr)yField1Offset;
                        frames[frameId].addr[1][1] = (Ptr)cbCrField1Offset;
                    }
                }

                #ifdef SYSTEM_VERBOSE_PRINTS
                if(pDrvObj->instId==0 && streamId==0 && chId==0)
                {
                    Vps_rprintf(" %d: CAPTURE: %d: 0x%08x, %d x %d, 0x%08x B\n",
                        Clock_getTicks(),
                        frameId, frames[frameId].addr[0][0],
                        format.pitch[0]/2, format.height, format.height*format.pitch[0]);
                }
                #endif
            }
            #ifdef SYSTEM_VERBOSE_PRINTS
            if(pDrvObj->instId==0 && streamId==0 && chId==0)
            {
                Vps_rprintf(" %d: CAPTURE: 0x%08x %08x B\n",
                    Clock_getTicks(),
                    frames[0].addr[0][0],
                    format.height*format.pitch[0]*frameId);
            }
            #endif

            /*
             * Set number of frame in frame list
             */
            frameList.numFrames = CAPTURE_LINK_FRAMES_PER_CH;

            /*
             * queue the frames in frameList
             * All allocate frames are queued here as an example.
             * In general atleast 2 frames per channel need to queued
             * before starting capture,
             * else frame will get dropped until frames are queued
             */
            status = FVID2_queue ( pDrvObj->captureVipHandle, &frameList, streamId );
            GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
        }
    }

    return FVID2_SOK;
}

/*
  Free allocated frames

  pDrvObj - capture driver information
*/
Int32 CaptureLink_drvFreeFrames( CaptureLink_Obj *pObj,
                                 CaptureLink_InstObj * pDrvObj,
                                 UInt32 frameCaptureMode)
{
    UInt32 idx;
    UInt16 streamId, chId;
    FVID2_Format format;
    FVID2_Frame *pFrames;
    Vps_CaptOutInfo *pOutInfo;
    UInt32 tilerUsed = FALSE;

    for ( streamId = 0; streamId < pDrvObj->createArgs.numStream; streamId++ )
    {
        for ( chId = 0; chId < pDrvObj->createArgs.numCh; chId++ )
        {
            pOutInfo = &pDrvObj->createArgs.outStreamInfo[streamId];

            idx = VPS_CAPT_CH_PER_PORT_MAX *
                  CAPTURE_LINK_FRAMES_PER_CH * streamId +
                  CAPTURE_LINK_FRAMES_PER_CH * chId;

            if (idx >= CAPTURE_LINK_MAX_FRAMES_PER_HANDLE)
            {
                idx = 0u;
            }

            pFrames = &pDrvObj->frames[idx];


            /* fill format with channel specific values  */
            format.channelNum = pDrvObj->createArgs.channelNumMap[streamId][chId];
            format.width = pObj->maxWidth;
            format.height = pObj->maxHeight+CAPTURE_LINK_HEIGHT_PAD_LINES;
            /* For frame capture mode we will be capturing both the fields
             * So height should be double
             */
            if (TRUE == frameCaptureMode)
            {
                format.height *= 2;
            }
            format.pitch[0] = pOutInfo->pitch[0];
            format.pitch[1] = pOutInfo->pitch[1];
            format.pitch[2] = pOutInfo->pitch[2];
            format.fieldMerged[0] = FALSE;
            format.fieldMerged[1] = FALSE;
            format.fieldMerged[2] = FALSE;
            format.dataFormat = pOutInfo->dataFormat;
            format.scanFormat = FVID2_SF_PROGRESSIVE;
            format.bpp = FVID2_BPP_BITS8; /* ignored */

            if(format.dataFormat==FVID2_DF_RAW_VBI)
            {
                format.height = CAPTURE_LINK_RAW_VBI_LINES;
            }

            if ( CaptureLink_drvIsDataFormatTiled
                 ( &pDrvObj->createArgs, streamId ) )
            {
                /*
                 * cannot free tiled frames
                 */
                tilerUsed = TRUE;
            }
            else
            {
                /*
                 * free frames for this channel, based on pFormat
                 */
                VpsUtils_memFrameFree ( &format, pFrames,
                                        CAPTURE_LINK_FRAMES_PER_CH );
            }
        }
    }

    if ( tilerUsed )
    {
        VpsUtils_tilerFreeAll (  );
    }

    return FVID2_SOK;
}

UInt32 CaptureLink_drvIsDataFormatTiled ( Vps_CaptCreateParams * createArgs,
                                    UInt16 streamId )
{
    Vps_CaptOutInfo *pOutInfo;

    pOutInfo = &createArgs->outStreamInfo[streamId];

    if ( ( pOutInfo->dataFormat == FVID2_DF_YUV420SP_UV ||
           pOutInfo->dataFormat == FVID2_DF_YUV422SP_UV )
         && pOutInfo->memType == VPS_VPDMA_MT_TILEDMEM )
    {
        return TRUE;
    }

    return FALSE;
}

/*
  Init create arguments to default values

  createArgs - create arguments
*/
Int32 CaptureLink_drvInitCreateArgs ( Vps_CaptCreateParams * createArgs )
{
    UInt16 chId, streamId;
    Vps_CaptOutInfo *pOutInfo;
    Vps_CaptScParams *pScParams;

    memset ( createArgs, 0, sizeof ( *createArgs ) );

    createArgs->videoCaptureMode =
        VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC;

    createArgs->videoIfMode = VPS_CAPT_VIDEO_IF_MODE_8BIT;

    createArgs->inDataFormat = FVID2_DF_YUV422P;
    createArgs->periodicCallbackEnable = FALSE;
    createArgs->numCh = 1;
    createArgs->numStream = 1;

    createArgs->vipParserInstConfig = NULL;
    createArgs->vipParserPortConfig = NULL;
    createArgs->cscConfig = NULL;
    createArgs->muxModeStartChId = 0u;

    pScParams = &createArgs->scParams;

    pScParams->inScanFormat = FVID2_SF_PROGRESSIVE;
    pScParams->inWidth = 360;
    pScParams->inHeight = 240;
    pScParams->inCropCfg.cropStartX = 0;
    pScParams->inCropCfg.cropStartY = 0;
    pScParams->inCropCfg.cropWidth = pScParams->inWidth;
    pScParams->inCropCfg.cropHeight = pScParams->inHeight;
    pScParams->outWidth = pScParams->inWidth;
    pScParams->outHeight = pScParams->inHeight;
    pScParams->scConfig = NULL;
    pScParams->scCoeffConfig = NULL;
    pScParams->enableCoeffLoad = TRUE;

    for ( streamId = 0; streamId < VPS_CAPT_STREAM_ID_MAX; streamId++ )
    {
        pOutInfo = &createArgs->outStreamInfo[streamId];

        pOutInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;
        pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_UNLIMITED;
        pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;

        pOutInfo->dataFormat = FVID2_DF_INVALID;

        pOutInfo->scEnable = FALSE;
        pOutInfo->subFrameModeEnable = FALSE;
        pOutInfo->numLinesInSubFrame = 0;
        pOutInfo->subFrameCb = NULL;

        if ( streamId == 0 )
        {
            pOutInfo->dataFormat = FVID2_DF_YUV422I_YUYV;
        }

        for ( chId = 0; chId < VPS_CAPT_CH_PER_PORT_MAX; chId++ )
        {
            createArgs->channelNumMap[streamId][chId] =
                Vps_captMakeChannelNum ( 0, streamId, chId );
        }
    }

    return 0;
}

Int32 CaptureLink_getCpuLoad()
{
    gCaptureLink_obj.totalCpuLoad += Load_getCPULoad();
    gCaptureLink_obj.cpuLoadCount++;

    return 0;
}

Int32 CaptureLink_drvPrintStatus(CaptureLink_Obj *pObj)
{
    UInt32 fps;

    FVID2_control( pObj->fvidHandleVipAll,
                   IOCTL_VPS_CAPT_PRINT_ADV_STATISTICS,
                   (Ptr)pObj->exeTime,
                   NULL
                 );

    fps = (pObj->captureDequeuedFrameCount*100)/(pObj->exeTime/10);

    Vps_printf(" %d: CAPTURE: Fields = %d (fps = %d, CPU Load = %d)\r\n",
        Clock_getTicks(),
        pObj->captureDequeuedFrameCount,
        fps,
        pObj->totalCpuLoad/pObj->cpuLoadCount
    );

    Vps_printf(" %d: CAPTURE: Num Resets = %d (Avg %d ms per reset)\r\n",
        Clock_getTicks(),
        pObj->resetCount,
        pObj->resetTotalTime/pObj->resetCount
    );

    return 0;
}

Int32 CaptureLink_drvDetectVideoStandard(CaptureLink_Obj *pObj)
{
    CaptureLink_VipInstParams *pInstPrm;

    Vps_VideoDecoderVideoStatusParams videoStatusArgs;
    Vps_VideoDecoderVideoStatus videoStatus;
    Vps_VideoDecoderCreateParams vidDecCreateArgs;
    Vps_VideoDecoderCreateStatus vidDecCreateStatus;
    FVID2_Handle videoDecoderHandle;
    Int32 status, instId;
    UInt32 repeatCnt;

    instId = 0;

    pObj->isPalMode = FALSE;

    pInstPrm = &pObj->createArgs.vipInst[instId];

    if(pInstPrm->videoDecoderId!=FVID2_VPS_VID_DEC_TVP5158_DRV )
    {
        /* auto-detect only supported for TVP5158 as of now */
        return FVID2_SOK;
    }

    /* detecting standard at VIP0 - Port A - CH0 ONLY - assuming same is used for all CHs */

    vidDecCreateArgs.deviceI2cInstId = Vps_platformGetI2cInstId();
    vidDecCreateArgs.numDevicesAtPort = 1;
    vidDecCreateArgs.deviceI2cAddr[0]
            = Vps_platformGetVidDecI2cAddr(pInstPrm->videoDecoderId, pInstPrm->vipInstId);
    vidDecCreateArgs.deviceResetGpio[0] = VPS_VIDEO_DECODER_GPIO_NONE;

    videoDecoderHandle = FVID2_create(
                                    pInstPrm->videoDecoderId,
                                    0,
                                    &vidDecCreateArgs,
                                    &vidDecCreateStatus,
                                    NULL
                                    );

    GT_assert( GT_DEFAULT_MASK, videoDecoderHandle!=NULL);

    #ifdef SYSTEM_DEBUG_CAPTURE
    Vps_printf(" %d: CAPTURE: VIP %d: VID DEC %d (0x%02x): Video Standard Detect in Progress !!!\n",
        Clock_getTicks(),
        pInstPrm->vipInstId,
        pInstPrm->videoDecoderId,
        vidDecCreateArgs.deviceI2cAddr[0]
      );
    #endif

    repeatCnt = 5;
    while (--repeatCnt != 0)
    {
        videoStatusArgs.channelNum = 0;
        status = FVID2_control(
                     videoDecoderHandle,
                     IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS,
                     &videoStatusArgs,
                     &videoStatus);
        GT_assert(GT_DEFAULT_MASK, (status == FVID2_SOK));

        if (videoStatus.isVideoDetect)
        {
            #ifdef SYSTEM_DEBUG_CAPTURE
            Vps_printf(" %d: CAPTURE: Detected video (%dx%d@%dHz, %d)!!!\n",
                Clock_getTicks(),
                videoStatus.frameWidth,
                videoStatus.frameHeight,
                1000000/videoStatus.frameInterval,
                videoStatus.isInterlaced);
            #endif

            if (videoStatus.frameHeight == 288)
                pObj->isPalMode = TRUE;

            break;
        }
        Task_sleep(100);
    }

    FVID2_delete(videoDecoderHandle, NULL);

    return FVID2_SOK;
}

Int32 CaptureLink_drvSetColor(CaptureLink_Obj *pObj, Int32 contrast, Int32 brightness, Int32 saturation)
{
    CaptureLink_VipInstParams *pInstPrm;
    CaptureLink_InstObj *pInst;
    Vps_VideoDecoderColorParams colorPrm;
    Int32 instId, chId, status;

    pObj->brightness += brightness;
    pObj->contrast += contrast;
    pObj->saturation += saturation;

    if(pObj->brightness<0)
        pObj->brightness = 0;
    if(pObj->brightness>255)
        pObj->brightness = 255;

    if(pObj->contrast<0)
        pObj->contrast = 0;
    if(pObj->contrast>255)
        pObj->contrast = 255;

    if(pObj->saturation<0)
        pObj->saturation = 0;
    if(pObj->saturation>255)
        pObj->saturation = 255;

    for(instId = 0; instId < pObj->createArgs.numVipInst; instId++)
    {
        pInstPrm = &pObj->createArgs.vipInst[instId];
        pInst = &pObj->instObj[instId];

        if(pInstPrm->videoDecoderId==FVID2_VPS_VID_DEC_TVP5158_DRV)
        {
            for(chId=0; chId<4; chId++)
            {
                colorPrm.channelNum = chId;
                colorPrm.videoBrightness = pObj->brightness;
                colorPrm.videoContrast = pObj->contrast;
                colorPrm.videoSaturation = pObj->saturation;
                colorPrm.videoSharpness = VPS_VIDEO_DECODER_NO_CHANGE;
                colorPrm.videoHue = VPS_VIDEO_DECODER_NO_CHANGE;

                status = FVID2_control(
                    pInst->videoDecoderHandle,
                    IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_COLOR,
                    &colorPrm,
                    NULL
                    );

                if(status==FVID2_SOK)
                {
                    Vps_rprintf(" %d: CAPTURE: %d: %d: Color parameter setting successful !!!\n",
                            Clock_getTicks(),
                            instId,
                            chId
                    );
                }
                else
                {
                    Vps_rprintf(" %d: CAPTURE: %d: %d: Color parameter setting ERROR !!!\n",
                            Clock_getTicks(),
                            instId,
                            chId
                    );
                }
            }
        }
    }

    return FVID2_SOK;
}
