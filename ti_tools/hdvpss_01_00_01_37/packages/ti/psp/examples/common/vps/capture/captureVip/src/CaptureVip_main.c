/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/sysbios/BIOS.h>
#include <stdio.h>
#include "CaptureVip_main.h"

/*  test application stack  */
#pragma DATA_ALIGN(gCaptureApp_tskStackMain, 32)
#pragma DATA_SECTION(gCaptureApp_tskStackMain, ".bss:taskStackSection")
UInt8 gCaptureApp_tskStackMain[CAPTURE_APP_TSK_STACK_MAIN];

/* test application control structure */
CaptureApp_Ctrl gCaptureApp_ctrl;

/*
  Capture test cases
*/
CaptureApp_UtParams gCaptureApp_utParamsMultiCh[] = {
    /* Multi-CH use-cases   */
    {
     FVID2_VPS_VID_DEC_TVP5158_DRV,
     4, VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC, 4,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_8BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
};

CaptureApp_UtParams gCaptureApp_utParamsSingleCh[] = {
#ifdef TI_814X_BUILD
    /* Single channel use case - YUV422 in and YUV422 out, 16 bit embe sync  */
    {
     FVID2_VPS_VID_DEC_SII9135_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x10, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
    /* Single channel capture via TVP7002 -
       Tested 16 bit capture.
       24 bit captures not yet validated on TI814x platform */
    /* TVP7002 - YUV422 in - YUV422 out */
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x11, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
#endif /* TI_814X_BUILD */

    /* Single-CH use-cases   */
#if 0 /* SII9135 combos */
    /* SII9135 - YUV422 in - YUV422 out */
    {
     FVID2_VPS_VID_DEC_SII9135_DRV,
     2, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x12, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    /* SII9135 - YUV422 in - YUV422SP out */
    {
     FVID2_VPS_VID_DEC_SII9135_DRV,
     2, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422SP_UV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x13, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // SII9135 - YUV422 in - RGB out
    {
     FVID2_VPS_VID_DEC_SII9135_DRV,
     2, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_RGB24_888, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x14, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // SII9135 - YUV422 in - SC YUV420 out
    {
     FVID2_VPS_VID_DEC_SII9135_DRV,
     2, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV420SP_UV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x15, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // SII9135 - YUV422 in - SC YUV420 out, YUV422 out
    {
     FVID2_VPS_VID_DEC_SII9135_DRV,
     2, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     2, {FVID2_DF_YUV420SP_UV, FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x16, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
#endif

#if 1 /* TVP7002 combos */
    /* TVP7002 - YUV422 in - YUV422 out Framebased Caputre */
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x17, FIELD_MERGED_CAPTURE_ENABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
    /* TVP7002 - YUV422 in - YUV422 out Framebased Caputre */
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV420SP_UV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x18, FIELD_MERGED_CAPTURE_ENABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
    // TVP7002 - YUV422 in - YUV422 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x19, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
    // TVP7002 - RGB in - RGB out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_24BIT, FVID2_DF_RGB24_888,
     1, {FVID2_DF_RGB24_888, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x1A, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // TVP7002 - RGB in - YUV422 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_24BIT, FVID2_DF_RGB24_888,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x1B, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // TVP7002 - RGB in - SC YUV422 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_24BIT, FVID2_DF_RGB24_888,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x1C, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // TVP7002 - RGB in - SC YUV420 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_24BIT, FVID2_DF_RGB24_888,
     1, {FVID2_DF_YUV420SP_UV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x1D, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // TVP7002 - RGB in - RGB out, SC YUV420 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_24BIT, FVID2_DF_RGB24_888,
     2, {FVID2_DF_RGB24_888, FVID2_DF_YUV420SP_UV, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x1E, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // 16bit discrete sync TVP7002 - YUV in - YUV422 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, 0x1F, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // 16bit discrete sync TVP7002 - YUV in - SC YUV422 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x20, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // 16bit discrete sync TVP7002 - YUV in - SC YUV420 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV420SP_UV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x21, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    // TVP7002 - 16bit discrete sync YUV in - RGB out, SC YUV420 out
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     2, {FVID2_DF_YUV420SP_UV, FVID2_DF_RGB24_888, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_ENABLE_0, 0, 0x22, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
#endif /* TVP7002 combos */

};

CaptureApp_UtParams gCaptureApp_utParamsCatalogSingleCh[] = {
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},

    /* Please note for TI814x Catalog board, at this point, we support capture
       through TVP7002 - VIP 0 Port A only */
    {
     FVID2_VPS_VID_DEC_TVP7002_DRV,
#ifdef TI_814X_BUILD
     1, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
#else
     2, VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC, 1,
#endif /* #ifdef TI_814X_BUILD */
     CAPTURE_APP_RUN_COUNT,
     VPS_CAPT_VIDEO_IF_MODE_16BIT, FVID2_DF_YUV422P,
     1, {FVID2_DF_YUV422I_YUYV, FVID2_DF_INVALID, FVID2_DF_INVALID}
     ,
     CAPTURE_SC_DISABLE_ALL, 0, FIELD_MERGED_CAPTURE_DISABLE,
     FALSE, FALSE, FALSE,
     206,   206,   206},
};

Int32 CaptureApp_overflowDetect(void)
{
    Vps_CaptOverFlowStatus  overFlowStatus;

    FVID2_control( gCaptureApp_ctrl.fvidHandleVipAll,
               IOCTL_VPS_CAPT_CHECK_OVERFLOW,
               NULL, &overFlowStatus
             );

    if (overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTA])
    {
        gCaptureApp_ctrl.totalOverflowCount++;
        gCaptureApp_ctrl.overflowCount[VPS_CAPT_INST_VIP0_PORTA]++;
    }
    if (overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP0_PORTB])
    {
        gCaptureApp_ctrl.totalOverflowCount++;
        gCaptureApp_ctrl.overflowCount[VPS_CAPT_INST_VIP0_PORTB]++;
    }
    if (overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTA])
    {
        gCaptureApp_ctrl.totalOverflowCount++;
        gCaptureApp_ctrl.overflowCount[VPS_CAPT_INST_VIP1_PORTA]++;
    }
    if (overFlowStatus.isPortOverFlowed[VPS_CAPT_INST_VIP1_PORTB])
    {
        gCaptureApp_ctrl.totalOverflowCount++;
        gCaptureApp_ctrl.overflowCount[VPS_CAPT_INST_VIP1_PORTB]++;
    }

    return 0;
}

Int32 captureApp_subFrame_callback_422I (FVID2_Handle handle, FVID2_Frame *subFrame)
{
    gCaptureApp_ctrl.subFrame_YUV422I_counter++;
    if ( gCaptureApp_ctrl.subFrameIdx < CAPTURE_SUBFRAME_COUNT )
    {
        memcpy ( &gCaptureApp_ctrl.subFrames[gCaptureApp_ctrl.subFrameIdx],
                 subFrame, sizeof(FVID2_Frame) );
        memcpy ( &gCaptureApp_ctrl.subFrameInfo[gCaptureApp_ctrl.subFrameIdx],
                 subFrame->subFrameInfo, sizeof(FVID2_SubFrameInfo));
        gCaptureApp_ctrl.subFrames[gCaptureApp_ctrl.subFrameIdx].subFrameInfo =
                &gCaptureApp_ctrl.subFrameInfo[gCaptureApp_ctrl.subFrameIdx];

        gCaptureApp_ctrl.subFrameIdx++;
    }
    return 0x0;
}

Int32 captureApp_subFrame_callback_422SP (FVID2_Handle handle, FVID2_Frame *subFrame)
{
    gCaptureApp_ctrl.subFrame_YUV422SP_counter++;

    return 0x0;
}

Int32 captureApp_subFrame_callback_RGB (FVID2_Handle handle, FVID2_Frame *subFrame)
{
    gCaptureApp_ctrl.subFrame_RGB_counter++;

    return 0x0;
}


/*
  Capture driver callback called when frames are available
  and callback is enabled

  deque frames from all active VIP ports
  print captured frame information
  queue frames back to the driver

  timeout - blocking or non-blocking dequeue
*/
Int32 CaptureApp_callback ( FVID2_Handle handle, Ptr appData, Ptr reserved )
{
    UInt32 streamId;
    FVID2_FrameList frameList;

    CaptureApp_overflowDetect();

    /*
     * for each stream ...
     */
    for ( streamId = 0; streamId < gCaptureApp_ctrl.utParams.numOutputs;
          streamId++ )
    {

        /*
         * Deque frames for all active handles
         */
        FVID2_dequeue ( gCaptureApp_ctrl.fvidHandleVipAll,
                        &frameList, streamId, BIOS_NO_WAIT );

        if ( frameList.numFrames )
        {

            gCaptureApp_ctrl.totalFieldCount += frameList.numFrames;

            /*
             * Queue back the frames to the driver
             */
            FVID2_queue ( gCaptureApp_ctrl.fvidHandleVipAll, &frameList,
                          streamId );

        }
    }
    return 0;
}

Int32 CaptureApp_cfgTimeStamp (CaptureApp_DrvObj *pObj)
{
    Int32 status;
    Vps_CaptFrameTimeStampParms tsParams;

    tsParams.enableBestEffort = TRUE;
    tsParams.timeStamper = NULL;
    tsParams.expectedFps = 60;

    status = FVID2_control(
        pObj->fvidHandle,
        IOCTL_VPS_CAPT_CFG_TIME_STAMPING_FRAMES,
        &tsParams,
        NULL
        );

    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    return status;

}


Int32 CaptureApp_detectVideo(CaptureApp_DrvObj *pObj)
{
    Int32 status;
    UInt32 chId, repeatCnt;
    Vps_VideoDecoderVideoStatusParams videoStatusArgs;
    Vps_VideoDecoderVideoStatus videoStatus;

    Vps_printf(" CAPTUREAPP: Detect video in progress for inst %d !!!\n", pObj->instId);

    status = FVID2_control(
        pObj->videoDecoderHandle,
        IOCTL_VPS_VIDEO_DECODER_SET_VIDEO_MODE,
        &pObj->vidDecVideoModeArgs,
        NULL
        );

    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    for (chId = 0; chId < pObj->createArgs.numCh; chId++)
    {
        repeatCnt = 5;
        while (--repeatCnt != 0)
        {
            videoStatusArgs.channelNum = chId;
            status = FVID2_control(
                         pObj->videoDecoderHandle,
                         IOCTL_VPS_VIDEO_DECODER_GET_VIDEO_STATUS,
                         &videoStatusArgs,
                         &videoStatus);
            GT_assert(GT_DEFAULT_MASK, (status == FVID2_SOK));

            if (videoStatus.isVideoDetect)
            {
                Vps_printf(" CAPTUREAPP: Detected video at CH%d (%dx%d@%dHz, %d)!!!\n",
                    chId,
                    videoStatus.frameWidth,
                    videoStatus.frameHeight,
                    1000000/videoStatus.frameInterval,
                    videoStatus.isInterlaced);
                break;
            }
            Task_sleep(100);
        }
    }

    Vps_printf(" CAPTUREAPP: Detect video Done !!!\n");

    return FVID2_SOK;
}

Int32 CaptureApp_createVideoDecoder(
                CaptureApp_DrvObj *pObj
                )
{
    Vps_CaptCreateParams *pVipCreateArgs;

    Vps_VideoDecoderChipIdParams vidDecChipIdArgs;
    Vps_VideoDecoderChipIdStatus vidDecChipIdStatus;

    UInt32 standard, videoDecoderId;

    Int32 status;

    pVipCreateArgs = &pObj->createArgs;

    if(pVipCreateArgs->videoCaptureMode
        ==VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC
        ||
        pVipCreateArgs->videoCaptureMode
        ==VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC
    )
    {
        videoDecoderId = FVID2_VPS_VID_DEC_TVP5158_DRV;
        standard = FVID2_STD_MUX_4CH_D1;
    }
    else
    {
        standard = FVID2_STD_AUTO_DETECT;
        videoDecoderId = gCaptureApp_ctrl.utParams.videoDecoderId;
    }

    if((videoDecoderId==FVID2_VPS_VID_DEC_TVP7002_DRV ||
        videoDecoderId==FVID2_VPS_VID_DEC_SII9135_DRV) &&
        (
           pObj->instId == VPS_CAPT_INST_VIP0_PORTA ||
           pObj->instId == VPS_CAPT_INST_VIP1_PORTA)
      )
    {
        /* select video decoder at board level mux */
        Vps_platformSelectVideoDecoder(videoDecoderId, pObj->instId);
    }

    if(videoDecoderId==FVID2_VPS_VID_DEC_TVP7002_DRV)
    {
        standard = FVID2_STD_1080I_60;
    }

    pObj->vidDecCreateArgs.deviceI2cInstId = Vps_platformGetI2cInstId();
    pObj->vidDecCreateArgs.numDevicesAtPort = 1;
    pObj->vidDecCreateArgs.deviceI2cAddr[0]
            = Vps_platformGetVidDecI2cAddr(videoDecoderId, pObj->instId);
    pObj->vidDecCreateArgs.deviceResetGpio[0] = VPS_VIDEO_DECODER_GPIO_NONE;

    pObj->videoDecoderHandle = FVID2_create(
                                    videoDecoderId,
                                    0,
                                    &pObj->vidDecCreateArgs,
                                    &pObj->vidDecCreateStatus,
                                    NULL
                                    );

    GT_assert( GT_DEFAULT_MASK, pObj->videoDecoderHandle!=NULL);

    vidDecChipIdArgs.deviceNum = 0;

    status = FVID2_control(
        pObj->videoDecoderHandle,
        IOCTL_VPS_VIDEO_DECODER_GET_CHIP_ID,
        &vidDecChipIdArgs,
        &vidDecChipIdStatus
        );

    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    pObj->vidDecVideoModeArgs.videoIfMode = pVipCreateArgs->videoIfMode;
    pObj->vidDecVideoModeArgs.videoDataFormat = pVipCreateArgs->inDataFormat;
    pObj->vidDecVideoModeArgs.standard = standard;
    pObj->vidDecVideoModeArgs.videoCaptureMode = pVipCreateArgs->videoCaptureMode;
    /* TVP7002 supports only ACTVID/VSYNC mode of operation in single channel
       discrete sync mode */
    if ((videoDecoderId == FVID2_VPS_VID_DEC_TVP7002_DRV) &&
        (pVipCreateArgs->videoCaptureMode != VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC))
    {
        pObj->vidDecVideoModeArgs.videoCaptureMode =
            VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC;
    }
    pObj->vidDecVideoModeArgs.videoSystem = VPS_VIDEO_DECODER_VIDEO_SYSTEM_AUTO_DETECT;
    pObj->vidDecVideoModeArgs.videoAutoDetectTimeout = BIOS_WAIT_FOREVER;
    pObj->vidDecVideoModeArgs.videoCropEnable = FALSE;

    Vps_printf(" CAPTUREAPP: VIP %d: VID DEC %04x (0x%02x): %04x:%04x:%04x\n",
        pObj->instId,
        videoDecoderId,
        pObj->vidDecCreateArgs.deviceI2cAddr[0],
        vidDecChipIdStatus.chipId,
        vidDecChipIdStatus.chipRevision,
        vidDecChipIdStatus.firmwareVersion
      );

    CaptureApp_detectVideo(pObj);

    return status;
}

Int32 CaptureApp_selectSimVideoSource()
{
    UInt32 instId, fileId, pixelClk;

    /* select input source file,

       Assumes that the simulator VIP super file contents are like below

        1 <user path>\output_bt656_QCIF.bin             # 8 -bit YUV422 single CH input
        2 <user path>\output_bt1120_QCIF.bin            # 16-bit YUV422 single CH input
        3 <user path>\output_bt1120_QCIF_RGB.bin        # 24-bit RGB888 single CH input
        4 <user path>\output_tvp5158_8CH_bt656_QCIF.bin # 8 -bit YUV422 multi  CH input
    */

    pixelClk = 1300*1000;

    switch(gCaptureApp_ctrl.utParams.captureMode)
    {
        case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC:
            switch(gCaptureApp_ctrl.utParams.videoIfMode)
            {
                case VPS_CAPT_VIDEO_IF_MODE_8BIT:
                    fileId = 1;
                    break;
                case VPS_CAPT_VIDEO_IF_MODE_16BIT:
                    fileId = 2;
                    break;
                case VPS_CAPT_VIDEO_IF_MODE_24BIT:
                    fileId = 3;
                    break;

            }
            break;

        case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC:
            fileId = 4;
            pixelClk *= 8; /* assumes eight CHs in input file */
            break;


    }

    for(instId=0; instId<VPS_CAPT_INST_MAX; instId++)
    {
        Vps_platformSimVideoInputSelect(instId, fileId, pixelClk);
    }
    return 0;
}


/*
  Create driver
  Allocate frames
  Queue frames
*/
Int32 CaptureApp_create (  )
{
    int handleId, outId, status;
    CaptureApp_DrvObj *pDrvObj;
    UInt16 chId, streamId;
    Vps_CaptFrameSkip frameSkip;
    Vps_CaptOutInfo *pOutInfo;
    Vps_CaptScParams *pScParams;
    UInt32 isTilerMode;
    Vps_CaptStorageParams storagePrms;
    Vps_CaptVipCropParams vipCropPrms;

    if(gCaptureApp_ctrl.utParams.videoDecoderId==FVID2_VPS_VID_DEC_TVP7002_DRV)
    {
        gCaptureApp_ctrl.maxWidth  = 1920;
        gCaptureApp_ctrl.maxHeight =  540;
    }
    if(gCaptureApp_ctrl.utParams.videoDecoderId==FVID2_VPS_VID_DEC_SII9135_DRV)
    {
#ifdef TI_814X_BUILD
        gCaptureApp_ctrl.maxWidth  = 1920;
        gCaptureApp_ctrl.maxHeight = 1080;
#else
        gCaptureApp_ctrl.maxWidth  = 1280;
        gCaptureApp_ctrl.maxHeight = 720;
#endif
    }

    if (Vps_platformIsSim())
    {
        if(gCaptureApp_ctrl.utParams.captureMode==VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC)
        {
            /* sim supports only line mux mode in mux mode */
            gCaptureApp_ctrl.utParams.captureMode = VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC;
        }

        CaptureApp_selectSimVideoSource();
    }

    gCaptureApp_ctrl.errorCount = 0;

     if ((gCaptureApp_ctrl.platformId == VPS_PLATFORM_ID_EVM_TI814x) &&
         (gCaptureApp_ctrl.utParams.videoDecoderId ==
            FVID2_VPS_VID_DEC_SII9135_DRV))
    {
        /* Instance 0 is connected to TVP and instance 1 to SII9135, ensure
           use the right instances */
        gCaptureApp_ctrl.drvObj[0].instId = VPS_CAPT_INST_VIP1_PORTA;
        gCaptureApp_ctrl.drvObj[1].instId = VPS_CAPT_INST_VIP0_PORTA;
    }
    else
    {
        gCaptureApp_ctrl.drvObj[0].instId = VPS_CAPT_INST_VIP0_PORTA;
        gCaptureApp_ctrl.drvObj[1].instId = VPS_CAPT_INST_VIP1_PORTA;
    }
    gCaptureApp_ctrl.drvObj[2].instId = VPS_CAPT_INST_VIP0_PORTB;
    gCaptureApp_ctrl.drvObj[3].instId = VPS_CAPT_INST_VIP1_PORTB;

    status = FVID2_control( gCaptureApp_ctrl.fvidHandleVipAll,
                   IOCTL_VPS_CAPT_RESET_VIP0,
                   NULL, NULL
                 );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    status = FVID2_control( gCaptureApp_ctrl.fvidHandleVipAll,
                   IOCTL_VPS_CAPT_RESET_VIP1,
                   NULL, NULL
                 );
    GT_assert( GT_DEFAULT_MASK, status==FVID2_SOK);

    /*
     * for all required handles ...
     */
    for ( handleId = 0; handleId < gCaptureApp_ctrl.utParams.numHandles;
          handleId++ )
    {

        pDrvObj = &gCaptureApp_ctrl.drvObj[handleId];

        /*
         * init create args
         */
        CaptureApp_initCreateArgs(pDrvObj);

        /*
         * set create args based on current test case parameters
         */
        pDrvObj->createArgs.videoCaptureMode =
            gCaptureApp_ctrl.utParams.captureMode;
        /* For TI814x/TI816x select VSYNC mode of operation for ES2.0 or
         * above */
        if (((gCaptureApp_ctrl.platformId == VPS_PLATFORM_ID_EVM_TI814x) &&
                (gCaptureApp_ctrl.cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
            ((gCaptureApp_ctrl.platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
                (gCaptureApp_ctrl.cpuRev >= VPS_PLATFORM_CPU_REV_2_0)))
        {
            if (pDrvObj->createArgs.videoCaptureMode ==
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK)
            {
                pDrvObj->createArgs.videoCaptureMode =
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC;
            }
        }

        pDrvObj->createArgs.videoIfMode = gCaptureApp_ctrl.utParams.videoIfMode;

        pDrvObj->createArgs.inDataFormat =
            gCaptureApp_ctrl.utParams.inDataFormat;

        pDrvObj->createArgs.numCh = gCaptureApp_ctrl.utParams.numCh;
        pDrvObj->createArgs.numStream = gCaptureApp_ctrl.utParams.numOutputs;

        pScParams = &pDrvObj->createArgs.scParams;

        pScParams->inWidth = gCaptureApp_ctrl.maxWidth;
        pScParams->inHeight = gCaptureApp_ctrl.maxHeight+2; /* Just to make sure TRIMMER is enabled */

        pScParams->inCropCfg.cropStartX = 0;
        pScParams->inCropCfg.cropStartY = 0;
        pScParams->inCropCfg.cropWidth = VpsUtils_align(gCaptureApp_ctrl.maxWidth, 2);
        pScParams->inCropCfg.cropHeight = VpsUtils_align(gCaptureApp_ctrl.maxHeight, 2);

        /*
         * when scaler is enabled is path, set output wxh and input w/2 x h/2
         * this is just a example for testing
         */
        pScParams->outWidth = VpsUtils_align( pScParams->inCropCfg.cropWidth / 2, 2);
        pScParams->outHeight = VpsUtils_align( pScParams->inCropCfg.cropHeight / 2, 2);

        /* Enable scaler coefficient load when scaler params are changed. */
        pScParams->enableCoeffLoad = TRUE;

        for ( outId = 0; outId < pDrvObj->createArgs.numStream; outId++ )
        {
            pOutInfo = &pDrvObj->createArgs.outStreamInfo[outId];

            pOutInfo->dataFormat =
                gCaptureApp_ctrl.utParams.outFormat[outId];

            pOutInfo->scEnable = FALSE;

            if (gCaptureApp_ctrl.maxWidth <= 352)
            {
                pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_352_PIXELS;
            }
            else if (gCaptureApp_ctrl.maxWidth <= 768)
            {
                pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_768_PIXELS;
            }
            else if (gCaptureApp_ctrl.maxWidth <= 1280)
            {
                pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_1280_PIXELS;
            }
            else if (gCaptureApp_ctrl.maxWidth <= 1920)
            {
                pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_1920_PIXELS;
            }
            else
            {
                pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_UNLIMITED;
            }

            if(gCaptureApp_ctrl.maxHeight<=288)
            {
                pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_288_LINES;
            } else
            if(gCaptureApp_ctrl.maxHeight<=576)
            {
                pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_576_LINES;
            } else
            if(gCaptureApp_ctrl.maxHeight<=720)
            {
                pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_720_LINES;
            } else
            if(gCaptureApp_ctrl.maxHeight<=1080)
            {
                pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_1080_LINES;
            } else
            {
                pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;
            }

            pOutInfo->pitch[0] = 0;
            pOutInfo->pitch[1] = 0;
            pOutInfo->pitch[2] = 0;

            isTilerMode = CaptureApp_isDataFormatTiled(
                            &pDrvObj->createArgs, outId
                          );

            switch(pOutInfo->dataFormat)
            {
                case FVID2_DF_YUV422I_YUYV:
                    pOutInfo->pitch[0] = VpsUtils_align(
                                            gCaptureApp_ctrl.maxWidth*2,
                                            VPS_BUFFER_ALIGNMENT
                                            );
                    break;

                case FVID2_DF_YUV420SP_UV:
                case FVID2_DF_YUV422SP_UV:
                    pOutInfo->pitch[0] = VpsUtils_align(
                                            gCaptureApp_ctrl.maxWidth,
                                            VPS_BUFFER_ALIGNMENT
                                            );
                    pOutInfo->pitch[1] = pOutInfo->pitch[0];

                    if(isTilerMode)
                    {
                        pOutInfo->pitch[0] = VPSUTILS_TILER_CNT_8BIT_PITCH;
                        pOutInfo->pitch[1] = VPSUTILS_TILER_CNT_16BIT_PITCH;
                    }
                    break;

                case FVID2_DF_YUV444I:
                case FVID2_DF_RGB24_888:
                    pOutInfo->pitch[0] = VpsUtils_align(
                                            gCaptureApp_ctrl.maxWidth*3,
                                            VPS_BUFFER_ALIGNMENT
                                            );
                    break;
                case FVID2_DF_RAW_VBI:
                    pOutInfo->pitch[0] = VpsUtils_align(
                                            gCaptureApp_ctrl.maxWidth,
                                            VPS_BUFFER_ALIGNMENT
                                            );
                    break;

            }

            if (pDrvObj->createArgs.videoCaptureMode ==
                VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC)
            {
                pDrvObj->createArgs.inScanFormat = FVID2_SF_INTERLACED;
            }
            pDrvObj->createArgs.muxModeStartChId = 0u;

            /*
             * scaler is supported only for YUV 422 and 420 output
             * and single channel mode
             */
            if (((pDrvObj->createArgs.videoCaptureMode ==
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC) ||
                  (pDrvObj->createArgs.videoCaptureMode ==
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK) ||
                  (pDrvObj->createArgs.videoCaptureMode ==
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC)
                 )
                 &&
                 ( pOutInfo->dataFormat
                   == FVID2_DF_YUV420SP_UV
                   ||
                   pOutInfo->dataFormat
                   == FVID2_DF_YUV422I_YUYV ) )
            {
                if ( outId == 0 &&
                     ( gCaptureApp_ctrl.utParams.
                       scEnable & CAPTURE_SC_ENABLE_0 ) )
                {
                    pOutInfo->scEnable = TRUE;
                }
                if ( outId == 1 &&
                     ( gCaptureApp_ctrl.utParams.
                       scEnable & CAPTURE_SC_ENABLE_1 ) )
                {
                    pOutInfo->scEnable = TRUE;
                }
            }
            /* Enable sub frame here */
            {
                if (gCaptureApp_ctrl.utParams.subFrameCfg.enable[outId] == TRUE)
                {
                    pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_UNLIMITED;
                    pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;
                    pOutInfo->subFrameModeEnable = TRUE;
                    pOutInfo->numLinesInSubFrame =
                            gCaptureApp_ctrl.utParams.subFrameCfg.nLines[outId];
                    if (pOutInfo->dataFormat == FVID2_DF_YUV422I_YUYV)
                    {
                        pOutInfo->subFrameCb = captureApp_subFrame_callback_422I;
                    }

                    if (pOutInfo->dataFormat == FVID2_DF_YUV420SP_UV)
                    {
                        pOutInfo->subFrameCb = captureApp_subFrame_callback_422SP;
                    }

                    if (pOutInfo->dataFormat == FVID2_DF_RGB24_888)
                    {
                        pOutInfo->subFrameCb = captureApp_subFrame_callback_RGB;
                    }
                }
            }

        }

        /*
         * set channelNum Map using utility macro Vps_captMakeChannelNum()
         */
        for ( streamId = 0; streamId < pDrvObj->createArgs.numStream;
              streamId++ )
        {
            for ( chId = 0; chId < pDrvObj->createArgs.numCh; chId++ )
            {

                pDrvObj->createArgs.channelNumMap[streamId][chId] =
                    Vps_captMakeChannelNum ( pDrvObj->instId, streamId, chId );
            }
        }

        memset ( &pDrvObj->cbPrm, 0, sizeof ( pDrvObj->cbPrm ) );

        /*
         * attach callback
         */

        pDrvObj->cbPrm.cbFxn = CaptureApp_callback;

        /*
         * create the driver
         */
        pDrvObj->fvidHandle = FVID2_create ( FVID2_VPS_CAPT_VIP_DRV,
                                             pDrvObj->instId,
                                             &pDrvObj->createArgs,
                                             &pDrvObj->createStatus,
                                             &pDrvObj->cbPrm );

        GT_assert( GT_DEFAULT_MASK, pDrvObj->fvidHandle != NULL );
        /*
         * Set the frame capture mode for interlaced input
         */
        /* channelNum is ignored configuration applies for all channels/streams
         * of the handle
         */
        if (TRUE == gCaptureApp_ctrl.utParams.fieldMerged)
        {
            storagePrms.channelNum = 0;
            storagePrms.bufferFmt = FVID2_BUF_FMT_FRAME;
            storagePrms.fieldMerged = TRUE;
            status = FVID2_control (pDrvObj->fvidHandle,
                                    IOCTL_VPS_CAPT_SET_STORAGE_FMT,
                                    &storagePrms, NULL  );
             GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK );
        }

        /* Use VIP trimmer for discrete sync mode for TI816x PG2.0 */
        if ((gCaptureApp_ctrl.platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
            (gCaptureApp_ctrl.cpuRev >= VPS_PLATFORM_CPU_REV_2_0))
        {
            vipCropPrms.channelNum = 0u;
            vipCropPrms.vipCropCfg.cropStartX = 0u;
            vipCropPrms.vipCropCfg.cropStartY = 0u;
            vipCropPrms.vipCropCfg.cropWidth = 1920u;
            vipCropPrms.vipCropCfg.cropHeight = 540u;
            /* Enable VIP trimmer in discrete sync mode */
            if ((pDrvObj->createArgs.videoCaptureMode ==
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VBLK) ||
                (pDrvObj->createArgs.videoCaptureMode ==
                    VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC))
            {
                vipCropPrms.vipCropEnable = TRUE;
            }
            else
            {
                vipCropPrms.vipCropEnable = FALSE;
            }
            status = FVID2_control(
                         pDrvObj->fvidHandle,
                         IOCTL_VPS_CAPT_SET_VIP_CROP_CFG,
                         &vipCropPrms, NULL);
            GT_assert(GT_DEFAULT_MASK, (status == FVID2_SOK));
        }

        /*
         * allocate and queue frames to the driver
         */
        CaptureApp_allocAndQueueFrames ( pDrvObj,
            gCaptureApp_ctrl.utParams.fieldMerged );

        /*
         * set frame skip using a IOCTL if enabled
         */
        for ( streamId = 0; streamId < pDrvObj->createArgs.numStream;
              streamId++ )
        {
            for ( chId = 0; chId < pDrvObj->createArgs.numCh; chId++ )
            {

                frameSkip.channelNum =
                    pDrvObj->createArgs.channelNumMap[streamId][chId];

                frameSkip.frameSkipMask =
                    gCaptureApp_ctrl.utParams.frameSkipMask;

                status = FVID2_control ( pDrvObj->fvidHandle,
                                         IOCTL_VPS_CAPT_SET_FRAME_SKIP,
                                         &frameSkip, NULL );
                GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK );
            }
        }

        Vps_printf ( " CAPTUREAPP: %d: CaptureApp_create() - DONE !!!\n",
                        handleId );

        pDrvObj->videoDecoderHandle = NULL;

        if (Vps_platformIsEvm())
        {
            CaptureApp_createVideoDecoder(pDrvObj);
        }
    }

    return 0;
}

Int32 CaptureApp_getCpuLoad()
{
    gCaptureApp_ctrl.totalCpuLoad += Load_getCPULoad();
    gCaptureApp_ctrl.cpuLoadCount++;

    return 0;
}

Int32 CaptureApp_resetStatistics()
{
    gCaptureApp_ctrl.totalFieldCount = 0;
    gCaptureApp_ctrl.totalCpuLoad = 0;
    gCaptureApp_ctrl.cpuLoadCount = 0;

    return 0;
}

Int32 CaptureApp_printStatistics(UInt32 totalTimeInMsecs)
{
    Vps_printf(" \r\n");
    Vps_printf(" Execution Statistics \r\n");
    Vps_printf(" ==================== \r\n");
    Vps_printf(" \r\n");
    Vps_printf(" Execution time    : %d.%d s \r\n", totalTimeInMsecs/1000, totalTimeInMsecs%1000);
    Vps_printf(" Total field Count : %d (%d fields/sec) \r\n",
                        gCaptureApp_ctrl.totalFieldCount,
                        (gCaptureApp_ctrl.totalFieldCount*1000)/totalTimeInMsecs
                );
    Vps_printf(" Avg CPU Load      : %d %% \r\n", gCaptureApp_ctrl.totalCpuLoad/gCaptureApp_ctrl.cpuLoadCount);
    Vps_printf(" Overflow Count    : %d\r\n", gCaptureApp_ctrl.totalOverflowCount);

    FVID2_control( gCaptureApp_ctrl.fvidHandleVipAll,
                   IOCTL_VPS_CAPT_PRINT_ADV_STATISTICS,
                   (Ptr)totalTimeInMsecs,
                   NULL
                 );

    return 0;
}

/*
  Delete driver
  Free buffer memory
*/
Int32 CaptureApp_delete (  )
{
    int handleId;
    CaptureApp_DrvObj *pDrvObj;

    /*
     * for all handles
     */
    for ( handleId = 0; handleId < gCaptureApp_ctrl.utParams.numHandles;
          handleId++ )
    {
        pDrvObj = &gCaptureApp_ctrl.drvObj[handleId];

        /*
         * delete handle
         */
        FVID2_delete ( pDrvObj->fvidHandle, NULL );

        if(pDrvObj->videoDecoderHandle)
            FVID2_delete ( pDrvObj->videoDecoderHandle, NULL );

        /*
         * free frame memory
         */
        CaptureApp_freeFrames ( pDrvObj, gCaptureApp_ctrl.utParams.fieldMerged );

        Vps_printf ( " CAPTUREAPP: %d: CaptureApp_delete() - DONE !!!\n",
                        handleId );
    }

    return 0;
}

/*
  Do runtime scaler parameter change
*/
Int32 CaptureApp_changeScParams (  )
{
    CaptureApp_DrvObj *pDrvObj;
    int handleId;
    Vps_CaptScParams *pScParams;
    static UInt32 isDone = FALSE;

    /*
     * if already done then dont do again
     * this is just an example
     */
    if ( isDone )
        return 0;

    /*
     * change scaler parameters
     * actual effect will be seen only if scaler was enabled during create,
     * else this IOCTL has no effect
     */

    isDone = TRUE;

    for ( handleId = 0; handleId < gCaptureApp_ctrl.utParams.numHandles;
          handleId++ )
    {

        pDrvObj = &gCaptureApp_ctrl.drvObj[handleId];

        pScParams = &pDrvObj->createArgs.scParams;

        /*
         * setting output wxh to input w/4 x h/4
         * this is just a example
         */
        pScParams->outWidth = pScParams->inWidth / 4;
        pScParams->outHeight = pScParams->inHeight / 4;

        FVID2_control ( pDrvObj->fvidHandle,
                        IOCTL_VPS_CAPT_SET_SC_PARAMS, pScParams, NULL );
    }

    return 0;
}

/*
  Execute test case

  pUtParams - test case parameters
*/
Int32 CaptureApp_run ( CaptureApp_UtParams * pUtParams )
{
    int handleId;
    CaptureApp_DrvObj *pDrvObj;
    UInt32 captureTime, startTime, loopCount;
    char ch;

    UInt32 curTime, prevTime, prevFieldCount, curFieldCount, fieldCount;

    /* Clear out subframe counters */
    gCaptureApp_ctrl.subFrameIdx = 0x0;
    gCaptureApp_ctrl.subFrame_YUV422I_counter = 0x0;
    gCaptureApp_ctrl.subFrame_RGB_counter = 0x0;
    gCaptureApp_ctrl.subFrame_YUV422SP_counter = 0x0;

    memcpy ( &gCaptureApp_ctrl.utParams,
             pUtParams, sizeof ( gCaptureApp_ctrl.utParams ) );

    /*
     * Print test case information
     */
    Vps_printf ( " CAPTUREAPP: HANDLES %d: MODE %04x : CH %d: RUN COUNT %d: "
                    "OUTPUT:%d:%d !!!\n",
                    pUtParams->numHandles,
                    pUtParams->captureMode,
                    pUtParams->numCh,
                    pUtParams->runCount,
                    pUtParams->outFormat[0], pUtParams->outFormat[1] );

    /*
     * Create driver
     */
    CaptureApp_create (  );

    CaptureApp_resetStatistics();

    loopCount = 0;

    curTime = prevTime = curFieldCount = prevFieldCount = 0;

    Vps_printf ( " CAPTUREAPP: Starting capture ... !!!\r\n");

    /*
     * Start video decoder
     */
    for ( handleId = 0; handleId < gCaptureApp_ctrl.utParams.numHandles;
          handleId++ )
    {
        pDrvObj = &gCaptureApp_ctrl.drvObj[handleId];

        if(pDrvObj->videoDecoderHandle)
            FVID2_start ( pDrvObj->videoDecoderHandle, NULL );
    }

    Vps_printf ( " CAPTUREAPP: Starting capture ... DONE !!!\r\n");
    Vps_printf ( " CAPTUREAPP: Capture in progress ... DO NOT HALT !!!\r\n");

    startTime = Clock_getTicks();

    /* Start the load calculation */
    VpsUtils_prfLoadCalcStart();

    /*
     * Start driver
     */
    for ( handleId = 0; handleId < gCaptureApp_ctrl.utParams.numHandles;
          handleId++ )
    {
        pDrvObj = &gCaptureApp_ctrl.drvObj[handleId];

        CaptureApp_cfgTimeStamp(pDrvObj);

        FVID2_start ( pDrvObj->fvidHandle, NULL );
    }

    /*
     * Capture frames, check status, change run-time parameters
     */
    while ( gCaptureApp_ctrl.utParams.runCount >= 0 )
    {
        /*
         * check status every 5msec
         */
        if (Vps_platformIsEvm())
        {
            Task_sleep(1000);
        }
        else
        {
            Task_sleep(10);
        }

        gCaptureApp_ctrl.utParams.runCount--;

        CaptureApp_getCpuLoad();

        if(loopCount && loopCount%(5)==0)
        {
            curTime = (Clock_getTicks() - startTime);
            curFieldCount = gCaptureApp_ctrl.totalFieldCount;

            captureTime = curTime-prevTime;
            fieldCount = curFieldCount-prevFieldCount;

            prevTime = curTime;
            prevFieldCount = curFieldCount;

            Vps_rprintf
                ( " CAPTUREAPP: %5d.%3ds: Fields = %5d (%3d fps)\r\n",
                    curTime/1000, curTime%1000, fieldCount, (fieldCount*1000)/captureTime
                );
        }

        VpsUtils_getChar(&ch, BIOS_NO_WAIT);
        if(ch=='p')
        {
            curTime = (Clock_getTicks() - startTime);
            CaptureApp_printStatistics(curTime);
        }
        if(ch=='0')
        {
            break;
        }

        loopCount++;

        #if 0
        if ( gCaptureApp_ctrl.utParams.runCount < pUtParams->runCount / 2 )
        {
            /*
             * change run-time params
             */
            CaptureApp_changeScParams (  );
        }
        #endif
    }

    /*
     * Stop driver
     */
    for ( handleId = 0; handleId < gCaptureApp_ctrl.utParams.numHandles;
          handleId++ )
    {
        pDrvObj = &gCaptureApp_ctrl.drvObj[handleId];

        FVID2_stop ( pDrvObj->fvidHandle, NULL );
    }

    captureTime = (Clock_getTicks() - startTime);

    /* Stop the load calculation */
    VpsUtils_prfLoadCalcStop();

    Vps_printf ( " CAPTUREAPP: Stopping capture ... !!!\r\n");

    /*
     * Stop video decoder
     */
    for ( handleId = 0; handleId < gCaptureApp_ctrl.utParams.numHandles;
          handleId++ )
    {
        pDrvObj = &gCaptureApp_ctrl.drvObj[handleId];

        if(pDrvObj->videoDecoderHandle)
            FVID2_stop ( pDrvObj->videoDecoderHandle, NULL );
    }

    Vps_printf ( " CAPTUREAPP: Stopping capture ... DONE !!!\r\n");

    CaptureApp_printStatistics(captureTime);

    /*
     * Delete driver
     */
    CaptureApp_delete (  );

    /*
     * Print test case information
     */
    Vps_printf
        ( " CAPTUREAPP: ERROR COUNT %d: HANDLES %d: MODE %04x : CH %d: RUN COUNT %d:"
          " !!! - DONE\n", gCaptureApp_ctrl.errorCount,
          pUtParams->numHandles, pUtParams->captureMode, pUtParams->numCh,
          pUtParams->runCount );

     /* Print the load */
    VpsUtils_prfLoadPrintAll(TRUE);

    /* Reset the load */
    VpsUtils_prfLoadCalcReset();


    return 0;
}

/*
  Allocate and queue frames to driver

  pDrvObj - capture driver information
*/
Int32 CaptureApp_allocAndQueueFrames ( CaptureApp_DrvObj * pDrvObj,
                                        UInt32 fieldMerged)
{
    Int32 status;
    UInt16 streamId, chId, frameId, idx;
    Vps_CaptOutInfo *pOutInfo;
    FVID2_Frame *frames;
    Vps_CaptRtParams *rtParams;
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
             * base index for pDrvObj->frames[] and pDrvObj->rtParams[]
             */
            idx =
                VPS_CAPT_CH_PER_PORT_MAX * CAPTURE_APP_FRAMES_PER_CH *
                streamId + CAPTURE_APP_FRAMES_PER_CH * chId;

            if (idx >= CAPTURE_MAX_FRAMES_PER_HANDLE)
            {
                idx = 0u;
            }

            rtParams = &pDrvObj->rtParams[idx];
            frames = &pDrvObj->frames[idx];

            /* fill format with channel specific values  */
            format.channelNum = Vps_captMakeChannelNum(
                                    pDrvObj->instId,
                                    streamId,
                                    chId
                                 );
            format.width = gCaptureApp_ctrl.maxWidth;
            format.height = gCaptureApp_ctrl.maxHeight + CAPTURE_APP_PADDING_LINES ;
            if (fieldMerged)
            {
                format.height *= 2;
            }
            format.pitch[0] = pOutInfo->pitch[0];
            format.pitch[1] = pOutInfo->pitch[1];
            format.pitch[2] = pOutInfo->pitch[2];
            format.fieldMerged[0] = TRUE;
            format.fieldMerged[1] = TRUE;
            format.fieldMerged[2] = TRUE;
            format.dataFormat = pOutInfo->dataFormat;
            format.scanFormat = FVID2_SF_PROGRESSIVE;
            format.bpp = FVID2_BPP_BITS8; /* ignored */

            /*
             * alloc memory based on 'format'
             * Allocated frame info is put in frames[]
             * CAPTURE_APP_FRAMES_PER_CH is the number of buffers per channel to
             * allocate
             */
            if ( CaptureApp_isDataFormatTiled
                 ( &pDrvObj->createArgs, streamId ) )
            {
                VpsUtils_tilerFrameAlloc ( &format, frames,
                                           CAPTURE_APP_FRAMES_PER_CH );
            }
            else
            {
                VpsUtils_memFrameAlloc ( &format, frames,
                                         CAPTURE_APP_FRAMES_PER_CH );
            }

            /*
             * Set rtParams for every frame in perFrameCfg
             */
            for ( frameId = 0; frameId < CAPTURE_APP_FRAMES_PER_CH; frameId++ )
            {
                frames[frameId].perFrameCfg = &rtParams[frameId];
                frames[frameId].subFrameInfo = &pDrvObj->subFrameInfo[frameId];
                frameList.frames[frameId] = &frames[frameId];

                if (TRUE == fieldMerged)
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
                            (format.pitch[0] * gCaptureApp_ctrl.maxHeight * 2));
                        cbCrField1Offset =(UInt32)(cbCrField0Offset + format.pitch[0]);
                        frames[frameId].addr[0][1] = (Ptr)cbCrField0Offset;
                        frames[frameId].addr[1][0] = (Ptr)yField1Offset;
                        frames[frameId].addr[1][1] = (Ptr)cbCrField1Offset;
                    }
                }
            }

            /*
             * Set number of frame in frame list
             */
            frameList.numFrames = CAPTURE_APP_FRAMES_PER_CH;

            /*
             * queue the frames in frameList
             * All allocate frames are queued here as an example.
             * In general atleast 2 frames per channel need to queued
             * before starting capture,
             * else frame will get dropped until frames are queued
             */
            status = FVID2_queue ( pDrvObj->fvidHandle, &frameList, streamId );
            GT_assert( GT_DEFAULT_MASK, status == FVID2_SOK );
        }
    }

    return FVID2_SOK;
}

/*
  Free allocated frames

  pDrvObj - capture driver information
*/
Int32 CaptureApp_freeFrames ( CaptureApp_DrvObj * pDrvObj, UInt32 fieldMerged )
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
                  CAPTURE_APP_FRAMES_PER_CH * streamId +
                  CAPTURE_APP_FRAMES_PER_CH * chId;
            if (idx >= CAPTURE_MAX_FRAMES_PER_HANDLE)
            {
                idx = 0u;
            }

            pFrames =
                &pDrvObj->frames[idx];

            /* fill format with channel specific values  */
            format.channelNum = Vps_captMakeChannelNum(
                                    pDrvObj->instId,
                                    streamId,
                                    chId
                                 );
            format.width = gCaptureApp_ctrl.maxWidth;
            format.height = gCaptureApp_ctrl.maxHeight + CAPTURE_APP_PADDING_LINES;
            if (fieldMerged)
                format.height *= 2;
            format.pitch[0] = pOutInfo->pitch[0];
            format.pitch[1] = pOutInfo->pitch[1];
            format.pitch[2] = pOutInfo->pitch[2];
            format.fieldMerged[0] = FALSE;
            format.fieldMerged[1] = FALSE;
            format.fieldMerged[2] = FALSE;
            format.dataFormat = pOutInfo->dataFormat;
            format.scanFormat = FVID2_SF_PROGRESSIVE;
            format.bpp = FVID2_BPP_BITS8; /* ignored */

            if ( CaptureApp_isDataFormatTiled
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
                                        CAPTURE_APP_FRAMES_PER_CH );
            }
        }
    }

    if ( tilerUsed )
    {
        VpsUtils_tilerFreeAll (  );
    }

    return FVID2_SOK;
}

UInt32 CaptureApp_isDataFormatTiled ( Vps_CaptCreateParams * createArgs,
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
Int32 CaptureApp_initCreateArgs(CaptureApp_DrvObj *pDrvObj)
{
    UInt16 chId, streamId;
    Vps_CaptOutInfo *pOutInfo;
    Vps_CaptScParams *pScParams;
    Vps_CaptCreateParams *createArgs;
    Vps_VipPortConfig *vipPortCfg;

    createArgs = &pDrvObj->createArgs;
    vipPortCfg = &pDrvObj->vipPortCfg;
    memset ( createArgs, 0, sizeof ( *createArgs ) );
    memset ( vipPortCfg, 0, sizeof ( *vipPortCfg ) );

    createArgs->videoCaptureMode =
        VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC;

    createArgs->videoIfMode = VPS_CAPT_VIDEO_IF_MODE_8BIT;

    createArgs->inDataFormat = FVID2_DF_YUV422P;
    createArgs->periodicCallbackEnable = FALSE;
    createArgs->numCh = 1;
    createArgs->numStream = 1;

    createArgs->vipParserInstConfig = NULL;

    vipPortCfg->ctrlChanSel = VPS_VIP_CTRL_CHAN_SEL_7_0;
    vipPortCfg->ancChSel8b = VPS_VIP_ANC_CH_SEL_8B_LUMA_SIDE;
    vipPortCfg->pixClkEdgePol = VPS_VIP_PIX_CLK_EDGE_POL_FALLING;

    vipPortCfg->invertFidPol = FALSE;
    vipPortCfg->embConfig.errCorrEnable = FALSE;
    vipPortCfg->embConfig.srcNumPos = VPS_VIP_SRC_NUM_POS_LS_NIBBLE_OF_CODEWORD;
    vipPortCfg->embConfig.isMaxChan3Bits = FALSE;

    if ((gCaptureApp_ctrl.platformId == VPS_PLATFORM_ID_EVM_TI816x) &&
        (gCaptureApp_ctrl.cpuRev == VPS_PLATFORM_CPU_REV_2_0) &&
        (gCaptureApp_ctrl.boardId == VPS_PLATFORM_BOARD_VS))
    {
        /* Use Falling edge clock polarity for VS board on TI816X 2.0 */
        createArgs->vipParserPortConfig = vipPortCfg;
    }
    else
    {
        createArgs->vipParserPortConfig = NULL;
    }
    createArgs->cscConfig = NULL;

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


    for ( streamId = 0; streamId < VPS_CAPT_STREAM_ID_MAX; streamId++ )
    {
        pOutInfo = &createArgs->outStreamInfo[streamId];

        pOutInfo->memType = VPS_VPDMA_MT_NONTILEDMEM;

#ifdef CAPTUREAPP_TEST_TILER
        pOutInfo->memType = VPS_VPDMA_MT_TILEDMEM;
#endif

        pOutInfo->dataFormat = FVID2_DF_INVALID;

        pOutInfo->maxOutWidth = VPS_CAPT_MAX_OUT_WIDTH_UNLIMITED;
        pOutInfo->maxOutHeight = VPS_CAPT_MAX_OUT_HEIGHT_UNLIMITED;

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

Int32 CaptureApp_detectBoard()
{
    gCaptureApp_ctrl.boardId = Vps_platformGetBoardId();

    if(gCaptureApp_ctrl.boardId>=VPS_PLATFORM_BOARD_MAX)
        gCaptureApp_ctrl.boardId = VPS_PLATFORM_BOARD_UNKNOWN;

    /*
     * handle to instance VIP mapping
     */
    gCaptureApp_ctrl.platformId = Vps_platformGetId();
    if ((gCaptureApp_ctrl.platformId == VPS_PLATFORM_ID_UNKNOWN) ||
        (gCaptureApp_ctrl.platformId >= VPS_PLATFORM_ID_MAX))
    {
        Vps_printf("%s: Error Unrecognized platform @ line %d\n",
            __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }
    gCaptureApp_ctrl.cpuRev = Vps_platformGetCpuRev();
    if (gCaptureApp_ctrl.cpuRev >= VPS_PLATFORM_CPU_REV_UNKNOWN)
    {
        Vps_printf("%s: Error Unrecognized CPU Revision @ line %d\n",
            __FUNCTION__, __LINE__);
        return FVID2_EFAIL;
    }

    /* Print platform information */
    Vps_platformPrintInfo();

    return 0;
}

/*
  System init
*/
Int32 CaptureApp_init (  )
{
    Int32 status;
    FVID2_CbParams cbPrm;
    const Char     *versionStr;
    Vps_PlatformDeviceInitParams deviceInitPrms;
    Vps_PlatformInitParams platformInitPrms;

    /*
     * Init memory allocator
     */
    VpsUtils_memInit (  );

#ifdef CAPTUREAPP_TEST_TILER
    VpsUtils_tilerInit (  );
#endif

#ifdef CAPTURE_APP_DEBUG_LOG
    VpsUtils_tilerDebugLogEnable ( TRUE );
#endif

    #ifndef PLATFORM_ZEBU
    VpsUtils_memClearOnAlloc(TRUE);
    #endif
    platformInitPrms.isPinMuxSettingReq = TRUE;
    status = Vps_platformInit(&platformInitPrms);
    GT_assert( GT_DEFAULT_MASK, status == 0 );

    /*
     * Get the version string
     */
    versionStr = FVID2_getVersionString();
    Vps_printf(" CAPTUREAPP: HDVPSS Drivers Version: %s\n", versionStr);

    /*
     * Initialize the vpsUtils to get the load
     */
    VpsUtils_prfInit();
    /*
     * FVID2 system init
     */
    status = FVID2_init ( NULL );
    GT_assert( GT_DEFAULT_MASK, status == 0 );

    deviceInitPrms.isI2cInitReq = TRUE;
    deviceInitPrms.isI2cProbingReq = TRUE;
    status = Vps_platformDeviceInit(&deviceInitPrms);
    GT_assert( GT_DEFAULT_MASK, status == 0 );

    if (Vps_platformIsEvm())
    {
        CaptureApp_detectBoard();
    }

#ifdef CAPTURE_APP_DEBUG_LOG
    /*
     * enable logs from VIP resource allocator
     */
    Vcore_vipResDebugLogEnable ( TRUE );
#endif

    /*
     * must be NULL for VPS_CAPT_INST_VIP_ALL
     */
    memset ( &cbPrm, 0, sizeof ( cbPrm ) );

    /*
     * Create global VIP capture handle, used for dequeue,
     * queue from all active captures
     */
    gCaptureApp_ctrl.fvidHandleVipAll = FVID2_create ( FVID2_VPS_CAPT_VIP_DRV, VPS_CAPT_INST_VIP_ALL, NULL, /* must be NULL for VPS_CAPT_INST_VIP_ALL */
                                                       NULL,    /* must be NULL for VPS_CAPT_INST_VIP_ALL */
                                                       &cbPrm );

    GT_assert( GT_DEFAULT_MASK, gCaptureApp_ctrl.fvidHandleVipAll != NULL );

    Vps_printf ( " CAPTUREAPP: CaptureApp_init() - DONE !!!\n" );

    return status;
}

/*
  System de-init
*/
Int32 CaptureApp_deInit (  )
{
    Vps_platformDeInit();

    /*
     * Delete global VIP capture handle
     */
    FVID2_delete ( gCaptureApp_ctrl.fvidHandleVipAll, NULL );

    Vps_platformDeviceDeInit();

    /*
     * FVID2 system de-init
     */
    FVID2_deInit ( NULL );

    /*
     * De-Initialize the vpsUtils to get the load
     */
    VpsUtils_prfDeInit();

#ifdef CAPTUREAPP_TEST_TILER
    VpsUtils_tilerDeInit (  );
#endif

    /*
     * De-init memory allocator
     */
    VpsUtils_memDeInit (  );

    Vps_printf ( " CAPTUREAPP: CaptureApp_deInit() - DONE !!!\n" );

    return 0;
}

/*
  Test task main
*/
Void CaptureApp_tskMain ( UArg arg1, UArg arg2 )
{
    UInt16 testId, numParams;

    CaptureApp_UtParams *pUtParams;

    Vps_printf ( " CAPTUREAPP: Sample Application - STARTS !!!\n" );
    /*
     * Ssystem init
     */
    CaptureApp_init (  );

    gCaptureApp_ctrl.maxWidth  = 160;
    gCaptureApp_ctrl.maxHeight = 128;

    pUtParams = gCaptureApp_utParamsMultiCh;
    numParams = sizeof ( gCaptureApp_utParamsMultiCh ) / sizeof ( CaptureApp_UtParams);

    if(gCaptureApp_ctrl.boardId==VPS_PLATFORM_BOARD_VS)
    {
        pUtParams = gCaptureApp_utParamsMultiCh;
        numParams = sizeof ( gCaptureApp_utParamsMultiCh ) / sizeof ( CaptureApp_UtParams);

        gCaptureApp_ctrl.maxWidth  = 720;
        gCaptureApp_ctrl.maxHeight = 288;
    }
    else if(gCaptureApp_ctrl.boardId==VPS_PLATFORM_BOARD_VC)
    {
        pUtParams = gCaptureApp_utParamsSingleCh;
        numParams = sizeof ( gCaptureApp_utParamsSingleCh ) / sizeof ( CaptureApp_UtParams);

        /* max width, height also overridden setup in CaptureApp_create() later */
        gCaptureApp_ctrl.maxWidth  = 1920;
        gCaptureApp_ctrl.maxHeight = 1080;
    }
    else if (gCaptureApp_ctrl.boardId == VPS_PLATFORM_BOARD_CATALOG)
    {
        pUtParams = gCaptureApp_utParamsCatalogSingleCh;
        numParams = sizeof ( gCaptureApp_utParamsCatalogSingleCh ) / sizeof ( CaptureApp_UtParams);

        /* max width, height also overridden setup in CaptureApp_create() later */
        gCaptureApp_ctrl.maxWidth  = 1920;
        gCaptureApp_ctrl.maxHeight = 1080;
    }

    if(pUtParams)
    {
#ifdef CAPTUREAPP_TEST_ALL
        int loopCount = CAPTUREAPP_TEST_LOOP_COUNT;

        while(loopCount--)
        {
            Vps_printf ( " CAPTUREAPP: Loop %d of %d !!!\n",
                CAPTUREAPP_TEST_LOOP_COUNT-loopCount,
                CAPTUREAPP_TEST_LOOP_COUNT
             );

            /*
             * Run all test cases
             */
            for ( testId = 0; testId < numParams; testId++ )
            {
                System_printf(" TestCase 0x%x\n", pUtParams[testId].testNum);
                CaptureApp_run ( &pUtParams[testId] );
            }
        }
#else
    /*
     * Run 0-th test case
     */
    testId = 0;
    numParams = 0x01u;

    for ( testId = 0; testId < numParams; testId++ )
    {
        CaptureApp_run ( &pUtParams[testId] );
    }
#endif
    }

    /*
     * System de-init
     */
    CaptureApp_deInit (  );

    Vps_printf ( " CAPTUREAPP: Sample Application - DONE !!!\n" );
}

/* Create test task */
Void CaptureApp_createTsk (  )
{
    Task_Params tskParams;

    /*
     * Set to 0
     */
    memset ( &gCaptureApp_ctrl, 0, sizeof ( gCaptureApp_ctrl ) );

    /*
     * Create test task
     */
    Task_Params_init ( &tskParams );

    tskParams.priority = CAPTURE_APP_TSK_PRI_MAIN;
    tskParams.stack = gCaptureApp_tskStackMain;
    tskParams.stackSize = sizeof ( gCaptureApp_tskStackMain );

    gCaptureApp_ctrl.tskMain = Task_create ( CaptureApp_tskMain,
                                             &tskParams, NULL );

    GT_assert( GT_DEFAULT_MASK, gCaptureApp_ctrl.tskMain != NULL );

    /* Register the task to the load module for calculating the load */
    VpsUtils_prfLoadRegister(gCaptureApp_ctrl.tskMain, "CAPTUREAPP:");
}

/*
  Application main
*/
Int32 main ( void )
{
    /*
     * Create test task
     */
    CaptureApp_createTsk (  );

    /*
     * Start BIOS
     */
    BIOS_start (  );

    return (0);
}
