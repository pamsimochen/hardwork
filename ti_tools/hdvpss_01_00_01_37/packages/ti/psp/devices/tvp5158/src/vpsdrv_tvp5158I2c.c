/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/tvp5158/src/vpsdrv_tvp5158Priv.h>

/* TVP5158 patch code */
static const UInt8 gVps_tvp5158Patch[] = {
#include "vpsdrv_tvp5158Patch_v2_03_02.h"
};

static const UInt32 gVps_tvp5158VbusAddrValueSet[] =
{
    /* VBUS Register Address (24-bit), VBUS Register value (8-bit) */
    #include "vpsdrv_tvp5158VbusAddrValueSet.h"

    (UInt32)-1, 0   /* Last entry */
};

/* Set TVP5158 mode based on

  - mux mode - line or pixel or no-mux
  - number of channels
  - resolution
  - 8/16-bit data width
  - NTSC/PAL standard
  - cascade mode of operation
*/
Int32 Vps_tvp5158SetVideoMode ( Vps_Tvp5158Obj * pObj,
                                Vps_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    Int32 devId;
    UInt8 regAddr[9];
    UInt8 regValue[9];
    UInt8 numRegs;
    UInt32 interleaveMode, chMuxNum, vidResSel, outputType;
    UInt32 cascadeEnable, chIdEnable, vdetEnable, videoCropEnable;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm == NULL )
        return FVID2_EBADARGS;

    #ifdef VPS_TVP5158_ENABLE_FIRMWARE_PATCHES
    status = Vps_tvp5158Reset(pObj);
    if ( status != FVID2_SOK )
        return status;
    #endif

    #ifdef VPS_TVP5158_ENABLE_NF
    status = Vps_tvp5158NfEnableAll(pObj, TRUE);
    if ( status != FVID2_SOK )
        return status;
    #endif

    memcpy(&pObj->videoModeParams, pPrm, sizeof(*pPrm));

    /*
     * for each TVP5158 device starting from cascade slave device do ...
     */
    for ( devId = pCreateArgs->numDevicesAtPort - 1; devId >= 0; devId-- )
    {
        /*
         * Set default settings
         */
        chIdEnable = FALSE;
        vdetEnable = FALSE;
        cascadeEnable = FALSE;

        if (TRUE == pPrm->videoCropEnable)
            videoCropEnable = TRUE;
        else
            videoCropEnable = FALSE;

        interleaveMode = VPS_TVP5158_LINE_INTERLEAVED_MODE;
        chMuxNum = VPS_TVP5158_4CH_MUX;
        vidResSel = VPS_TVP5158_RES_D1;
        outputType = VPS_TVP5158_OUT_TYPE_8BIT;

        /*
         * select mux mode, vdet mode based on 'videoCaptureMode'
         */
        switch ( pPrm->videoCaptureMode )
        {
            case VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_EMBEDDED_SYNC:
                interleaveMode = VPS_TVP5158_NON_INTERLEAVED_MODE;
                vdetEnable = TRUE;
                break;

            case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_PIXEL_MUX_EMBEDDED_SYNC:
                interleaveMode = VPS_TVP5158_PIXEL_INTERLEAVED_MODE;
                chIdEnable = TRUE;
                vdetEnable = TRUE;
                break;

            case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_EMBEDDED_SYNC:
                interleaveMode = VPS_TVP5158_LINE_INTERLEAVED_MODE;
                break;

            case VPS_CAPT_VIDEO_CAPTURE_MODE_MULTI_CH_LINE_MUX_SPLIT_LINE_EMBEDDED_SYNC:
                interleaveMode = VPS_TVP5158_LINE_INTERLEAVED_HYBRID_MODE;
                break;

            default:
                status = FVID2_EBADARGS;
                break;
        }

        /*
         * set number of channels to mux and video resolution based on
         * 'standard'
         */
        switch ( pPrm->standard )
        {
            case FVID2_STD_MUX_2CH_D1:
                chMuxNum = VPS_TVP5158_2CH_MUX;
                vidResSel = VPS_TVP5158_RES_D1;
                break;

            case FVID2_STD_MUX_4CH_D1:
                chMuxNum = VPS_TVP5158_4CH_MUX;
                vidResSel = VPS_TVP5158_RES_D1;
                break;

            case FVID2_STD_MUX_4CH_CIF:
                chMuxNum = VPS_TVP5158_4CH_MUX;
                vidResSel = VPS_TVP5158_RES_CIF;
                break;

            case FVID2_STD_MUX_4CH_HALF_D1:
                chMuxNum = VPS_TVP5158_4CH_MUX;
                vidResSel = VPS_TVP5158_RES_HALF_D1;
                break;

            case FVID2_STD_MUX_8CH_CIF:
                chMuxNum = VPS_TVP5158_8CH_MUX;
                vidResSel = VPS_TVP5158_RES_CIF;

                /*
                 * if cascade device set cascade mode to TRUE
                 */
                if ( devId > 0 )
                    cascadeEnable = TRUE;

                break;

            case FVID2_STD_MUX_8CH_HALF_D1:
                chMuxNum = VPS_TVP5158_8CH_MUX;
                vidResSel = VPS_TVP5158_RES_HALF_D1;

                /*
                 * if cascade device set cascade mode to TRUE
                 */
                if ( devId > 0 )
                    cascadeEnable = TRUE;

                break;

            case FVID2_STD_CIF:
                chMuxNum = VPS_TVP5158_1CH_MUX;
                vidResSel = VPS_TVP5158_RES_CIF;
                break;

            case FVID2_STD_HALF_D1:
                chMuxNum = VPS_TVP5158_1CH_MUX;
                vidResSel = VPS_TVP5158_RES_HALF_D1;
                break;

            case FVID2_STD_D1:
                chMuxNum = VPS_TVP5158_1CH_MUX;
                vidResSel = VPS_TVP5158_RES_D1;
                break;

            default:
                status = FVID2_EBADARGS;
                break;

        }

        /*
         * set 8/16-bit mode
         */
        switch ( pPrm->videoIfMode )
        {
            case VPS_CAPT_VIDEO_IF_MODE_8BIT:
                outputType = VPS_TVP5158_OUT_TYPE_8BIT;
                break;

            case VPS_CAPT_VIDEO_IF_MODE_16BIT:
                outputType = VPS_TVP5158_OUT_TYPE_16BIT;
                break;

            default:
                status = FVID2_EBADARGS;
                break;
        }

        /*
         * validate if setup combination's are valid
         */
        #if 0
        /*
         * cascade ON but number of device < 2
         */
        if ( cascadeEnable && pCreateArgs->numDevicesAtPort == 1 )
            status = FVID2_EBADARGS;

        /*
         * only YUV422 can be the video format
         */
        if ( pPrm->videoDataFormat != FVID2_DF_YUV422P )
            status = FVID2_EBADARGS;

        if ( status != FVID2_SOK )
            return status;

        /*
         * check video setup combinations are valid or not
         */
        status = Vps_tvp5158CheckVideoSettings ( interleaveMode,
                                                 chMuxNum, vidResSel,
                                                 outputType );
        if ( status != FVID2_SOK )
            return status;
        #endif

        /*
         * fill the register addr/value pairs
         */

        numRegs = 0;

        regAddr[numRegs] = VPS_TVP5158_REG_AVD_OUT_CTRL_1;
        regValue[numRegs] =
            ( interleaveMode << 6 )
            | ( chMuxNum << 4 )
            | ( outputType << 3 ) | ( cascadeEnable << 2 ) | ( vidResSel << 0 );
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_AVD_OUT_CTRL_2;
        regValue[numRegs] = ( ( videoCropEnable & 0x1 ) << 6 ) | ( 1 << 4 ) /* 1: dithering enable */
            | ( chIdEnable << 2 ) | ( chIdEnable << 1 ) | ( vdetEnable << 0 );
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_OFM_MODE_CTRL;
        regValue[numRegs] = ( 1 << 5 );  /* 1: OSC out enabled  */
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_AUTO_SW_MASK;
        regValue[numRegs] = 0xFF;  /* No mask for autoswitch  */
        numRegs++;

        /*
         * select decoder cores to which the selectings should apply
         */
        status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        /*
         * apply the settings
         */
        status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                    pCreateArgs->deviceI2cAddr[devId],
                                    regAddr, regValue, numRegs );

        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;


    }

    #ifdef VPS_TVP5158_ENABLE_FIRMWARE_PATCHES
    /* Extended settings should be configured only in case of force mode */
    if (VPS_VIDEO_DECODER_VIDEO_SYSTEM_AUTO_DETECT !=
            pObj->videoModeParams.videoSystem)
    {
        Vps_tvp5158SetExtendedSettings(pObj);
    }
    #endif

    Task_sleep ( 20 );

    #ifdef VPS_TVP5158_ENABLE_FIRMWARE_PATCHES
    Vps_tvp5158OfmReset(pObj);
    #endif

    return status;
}

/*
  check if the video settings combinations are valid
*/
Int32 Vps_tvp5158CheckVideoSettings ( UInt32 interleaveMode,
                                      UInt32 chMuxNum, UInt32 vidResSel,
                                      UInt32 outputType )
{
    Int32 status = FVID2_SOK;

    switch ( interleaveMode )
    {
        case VPS_TVP5158_NON_INTERLEAVED_MODE:
            if ( chMuxNum != VPS_TVP5158_1CH_MUX )
                status = FVID2_EBADARGS;
            if ( outputType != VPS_TVP5158_OUT_TYPE_8BIT )
                status = FVID2_EBADARGS;
            if ( vidResSel != VPS_TVP5158_RES_D1 )
                status = FVID2_EBADARGS;
            break;

        case VPS_TVP5158_PIXEL_INTERLEAVED_MODE:
            if ( chMuxNum != VPS_TVP5158_2CH_MUX
                 || chMuxNum != VPS_TVP5158_4CH_MUX )
            {
                status = FVID2_EBADARGS;
            }
            if ( outputType != VPS_TVP5158_OUT_TYPE_8BIT )
                status = FVID2_EBADARGS;
            if ( vidResSel != VPS_TVP5158_RES_D1 )
                status = FVID2_EBADARGS;
            break;

        case VPS_TVP5158_LINE_INTERLEAVED_MODE:
            if ( chMuxNum != VPS_TVP5158_2CH_MUX
                 || chMuxNum != VPS_TVP5158_4CH_MUX
                 || chMuxNum != VPS_TVP5158_8CH_MUX )
            {
                status = FVID2_EBADARGS;
            }
            if ( chMuxNum == VPS_TVP5158_4CH_MUX )
            {
                if ( vidResSel == VPS_TVP5158_RES_CIF
                     && outputType == VPS_TVP5158_OUT_TYPE_16BIT )
                {
                    status = FVID2_EBADARGS;
                }
            }
            else
            {
                if ( outputType != VPS_TVP5158_OUT_TYPE_8BIT )
                    status = FVID2_EBADARGS;
            }
            break;

        case VPS_TVP5158_LINE_INTERLEAVED_HYBRID_MODE:
            if ( chMuxNum != VPS_TVP5158_4CH_MUX
                 || chMuxNum != VPS_TVP5158_8CH_MUX )
            {
                status = FVID2_EBADARGS;
            }
            if ( vidResSel != VPS_TVP5158_RES_HALF_D1
                 || vidResSel != VPS_TVP5158_RES_CIF )
            {
                status = FVID2_EBADARGS;
            }
            if ( outputType != VPS_TVP5158_OUT_TYPE_8BIT )
                status = FVID2_EBADARGS;
            if ( chMuxNum == VPS_TVP5158_8CH_MUX
                 && vidResSel == VPS_TVP5158_RES_HALF_D1 )
            {
                status = FVID2_EBADARGS;
            }
            break;

        default:
            status = FVID2_EBADARGS;
            break;
    }

    return status;
}

/* reset TVP5158 OFM logic  */
Int32 Vps_tvp5158Reset ( Vps_Tvp5158Obj * pObj )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    Int32 devId;

    pCreateArgs = &pObj->createArgs;

    /*
     * calling reset for every device connected to the port starting
     * from cascade device
     */
    for ( devId = pCreateArgs->numDevicesAtPort - 1; devId >= 0; devId-- )
    {
        status = Vps_tvp5158PatchDownload(pObj, devId);
        if ( status != FVID2_SOK )
            return status;
    }

    return status;
}

/*
  Soft-reset of TVP5158
*/
Int32 Vps_tvp5158OfmReset ( Vps_Tvp5158Obj * pObj)
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    Int32 devId;

    pCreateArgs = &pObj->createArgs;

    for ( devId = pCreateArgs->numDevicesAtPort - 1; devId >= 0; devId-- )
    {
        /*
         * select all TVP5158 cores
         */
        status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
        if ( status != FVID2_SOK )
            return status;

        /*
         * do OFM reset
         */
        numRegs = 0;

        regAddr[numRegs] = VPS_TVP5158_REG_MISC_OFM_CTRL;
        regValue[numRegs] = 0x01;
        numRegs++;

        status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                    pCreateArgs->deviceI2cAddr[devId],
                                    regAddr, regValue, numRegs );

        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        /*
         * wait for reset to be effective
         */
        Task_sleep ( 20 );
    }

    return status;
}

Int32 Vps_tvp5158NfEnableAll(Vps_Tvp5158Obj * pObj, Bool enable)
{
    Vps_Tvp5158VideoNfParams nfParams;
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    Int32 devId, chId;

    devId = 0;

    pCreateArgs = &pObj->createArgs;

    nfParams.nfEnable = enable;
    nfParams.colorKillerEnable = enable;
    nfParams.maxNoise = 0;

    /*
     * for all devices starting from cascade device do ...
     */
    for ( devId = pCreateArgs->numDevicesAtPort - 1; devId >= 0; devId-- )
    {
        for(chId=0; chId<VPS_TVP5158_CH_PER_DEVICE_MAX; chId++)
        {
            nfParams.channelNum = chId;

            /* Max noise: 0..63 */
            if(chId==0)
                nfParams.maxNoise = 0;
            if(chId==1)
                nfParams.maxNoise = 16;
            if(chId==2)
                nfParams.maxNoise = 32;
            if(chId==3)
                nfParams.maxNoise = 63;

            status = Vps_tvp5158SetVideoNf(pObj, &nfParams);

            if ( status != FVID2_SOK )
                return FVID2_ETIMEOUT;
        }
    }

    return status;
}

Int32 Vps_tvp5158SetExtendedSettings( Vps_Tvp5158Obj * pObj)
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    Vps_VideoDecoderVideoModeParams *pVidModePrm;
    Int32 devId;
    UInt8 regAddr[32];
    UInt8 regValue[32];
    UInt8 numRegs, autoSwitchMode;

    devId = 0;

    pCreateArgs = &pObj->createArgs;

    pVidModePrm = &pObj->videoModeParams;

    /* 0: Auto switch mode, 1: force NTSC mode, 2: force PAL mode */

    autoSwitchMode = 0x00; /* default */
    if(pVidModePrm->videoSystem==VPS_VIDEO_DECODER_VIDEO_SYSTEM_NTSC)
    {
        autoSwitchMode = 0x01;
    }
    else
    if(pVidModePrm->videoSystem==VPS_VIDEO_DECODER_VIDEO_SYSTEM_PAL)
    {
        autoSwitchMode = 0x02;
    }

    #ifdef VPS_TVP5158_ENABLE_COMB_FILTER_SETUP
    Vps_tvp5158SetIndirectRegisters(pObj);
    #endif

    /*
     * for all devices starting from cascade device do ...
     */
    for ( devId = pCreateArgs->numDevicesAtPort - 1; devId >= 0; devId-- )
    {
        status = Vps_tvp5158SelectWrite(pObj, 0, VPS_TVP5158_CORE_ALL);
        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        numRegs = 0;

        regAddr[numRegs] = VPS_TVP5158_REG_VID_STD_SELECT;
        regValue[numRegs] = autoSwitchMode;
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_OP_MODE_CTRL;
        regValue[numRegs] = 0x08; /* New V-Bit control algorithm (number of
                                     active lines per frame is constant as
                                     total LPF varies) */
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_FV_DEC_CTRL;
        regValue[numRegs] = 0x03; /* Adaptive BOP/EOP control for TI816x enabled
                                     and F and V bits decoded from line count */
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_FV_CTRL;
        regValue[numRegs] = 0x06; /* V-PLL fast-lock disabled and windowed
                                     VSYNC pipe disabled */
        numRegs++;

        regAddr[numRegs]  = VPS_TVP5158_REG_FBIT_DURATION;
        regValue[numRegs] = 0x60; /* Maximum field duration set to 788 for NTSC
                                     and 938 for PAL */
        numRegs++;

        regAddr[numRegs]  = VPS_TVP5158_REG_ESYNC_OFFSET_1;
        regValue[numRegs] = 0x02;   /* Default V-bit position relative to F-bit
                                       Moving line position of Embedded
                                       F and V bit signals to offset
                                       from 656 standard to patch for DVD
                                       player offset issue */
        numRegs++;

        regAddr[numRegs]  = VPS_TVP5158_REG_ESYNC_OFFSET_2;
        regValue[numRegs] = 0x00;   /* Default V-bit position relative to F-bit */
        numRegs++;

        regAddr[numRegs]  = VPS_TVP5158_REG_MIN_F1_ACT;
        regValue[numRegs] = 0x08;   /* Minimum F1_to_active set to 8 lines */
        numRegs++;

        regAddr[numRegs]  = VPS_TVP5158_REG_Y_CTRL_2;
        regValue[numRegs] = 0x01;   /* Peaking Gain 0: default, 1: 0.5, 2:1, 3:2  */
        #ifdef VPS_TVP5158_ENABLE_COMB_FILTER_SETUP
        regValue[numRegs] |= 0x20;  /* Apply Comb filter settings */
        #endif

        numRegs++;

        #if 0
        regAddr[numRegs]  = VPS_TVP5158_REG_Y_BRIGHTNESS;
        regValue[numRegs] = 0x1c;   /* decrease brightness */
        numRegs++;

        regAddr[numRegs]  = VPS_TVP5158_REG_Y_CONTRAST;
        regValue[numRegs] = 0x89;   /* increase contrast */
        numRegs++;
        #endif

        status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                    pCreateArgs->deviceI2cAddr[devId],
                                    regAddr, regValue, numRegs );

        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        numRegs = 0;

        regAddr[numRegs] = VPS_TVP5158_REG_Y_CTRL_1;
        regValue[numRegs] = 0;
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_Y_CTRL_2;
        regValue[numRegs] = 0;
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_C_CTRL_1;
        regValue[numRegs] = 0;
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_C_CTRL_2;
        regValue[numRegs] = 0;
        numRegs++;

        status = Vps_deviceRead8 ( pCreateArgs->deviceI2cInstId,
                                    pCreateArgs->deviceI2cAddr[devId],
                                    regAddr, regValue, numRegs );

        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        #ifdef VPS_TVP5158_VERBOSE_DEBUG
        Vps_printf
            ( " TVP5158: 0x%02x: Y Procesing Control 1 = 0x%02x\n",
                pCreateArgs->deviceI2cAddr[devId], regValue[0]
            );
        Vps_printf
            ( " TVP5158: 0x%02x: Y Procesing Control 2 = 0x%02x\n",
                pCreateArgs->deviceI2cAddr[devId], regValue[1]
            );
        Vps_printf
            ( " TVP5158: 0x%02x: C Procesing Control 1 = 0x%02x\n",
                pCreateArgs->deviceI2cAddr[devId], regValue[2]
            );
        Vps_printf
            ( " TVP5158: 0x%02x: C Procesing Control 2 = 0x%02x\n",
                pCreateArgs->deviceI2cAddr[devId], regValue[3]
            );
        #endif
    }

    return status;
}


/*
  Enable TVP5158 output port
*/
Int32 Vps_tvp5158OutputEnable ( Vps_Tvp5158Obj * pObj, UInt32 enable )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    Int32 devId;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;

    pCreateArgs = &pObj->createArgs;

    /*
     * for all devices starting from cascade device do ...
     */
    for ( devId = pCreateArgs->numDevicesAtPort - 1; devId >= 0; devId-- )
    {
        /*
         * select all TVP5158 cores
         */
        status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
        if ( status != FVID2_SOK )
            return status;

        /*
         * read register for output enable
         */
        numRegs = 0;
        regAddr[numRegs] = VPS_TVP5158_REG_OFM_MODE_CTRL;
        regValue[numRegs] = 0;
        numRegs++;

        status = Vps_deviceRead8 ( pCreateArgs->deviceI2cInstId,
                                   pCreateArgs->deviceI2cAddr[devId],
                                   regAddr, regValue, numRegs );

        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        /*
         * set bit or reset bit to enable or disable output
         */
        if ( enable )
            regValue[0] |= VPS_TVP5158_OUT_ENABLE;
        else
            regValue[0] &= ~VPS_TVP5158_OUT_ENABLE;

        /*
         * write register
         */
        status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                    pCreateArgs->deviceI2cAddr[devId],
                                    regAddr, regValue, numRegs );

        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;
    }

    return status;
}

/* start/enable output  */
Int32 Vps_tvp5158Start ( Vps_Tvp5158Obj * pObj )
{
    Int32 status = FVID2_SOK;

    status = Vps_tvp5158OutputEnable ( pObj, TRUE );

    return status;
}

/* stop/disable output  */
Int32 Vps_tvp5158Stop ( Vps_Tvp5158Obj * pObj )
{
    Int32 status = FVID2_SOK;

    status = Vps_tvp5158OutputEnable ( pObj, FALSE );

    return status;
}

/*
  Select TVP5158 core

  devId - device for which to apply this
  value - 0x1: Core0, 0x2: Core1, 0x4: Core2, 0x8: Core3,
          VPS_TVP5158_CORE_ALL: All Core's
*/
Int32 Vps_tvp5158SelectWrite ( Vps_Tvp5158Obj * pObj, UInt32 devId,
                               UInt32 coreId )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;

    pCreateArgs = &pObj->createArgs;

    numRegs = 0;

    regAddr[numRegs] = VPS_TVP5158_REG_DEC_WR_EN;
    regValue[numRegs] = coreId;
    numRegs++;

    status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[devId],
                                regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/* read from TVP5158 VBUS */
Int32 Vps_tvp5158VbusRead ( Vps_Tvp5158Obj * pObj,
                            UInt32 devId, UInt32 vbusAddr, UInt8 * val )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;

    pCreateArgs = &pObj->createArgs;

    /*
     * set VBUS address
     */
    status = Vps_tvp5158VbusWrite ( pObj, devId, vbusAddr, 0, 0 );
    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    /*
     * read VBUS address value
     */
    numRegs = 0;

    regAddr[numRegs] = 0xE0;
    regValue[numRegs] = 0;
    numRegs++;

    status = Vps_deviceRead8 ( pCreateArgs->deviceI2cInstId,
                               pCreateArgs->deviceI2cAddr[devId],
                               regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return status;

    *val = regValue[0];

    return status;
}

/*
  Write to VBUS
*/
Int32 Vps_tvp5158VbusWrite ( Vps_Tvp5158Obj * pObj,
                             UInt32 devId, UInt32 vbusAddr, UInt8 val,
                             UInt32 len )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;

    pCreateArgs = &pObj->createArgs;

    numRegs = 0;

    /*
     * set VBUS address
     */
    regAddr[numRegs] = 0xE8;
    regValue[numRegs] = ( UInt8 ) ( ( vbusAddr >> 0 ) & 0xFF );
    numRegs++;

    regAddr[numRegs] = 0xE9;
    regValue[numRegs] = ( UInt8 ) ( ( vbusAddr >> 8 ) & 0xFF );
    numRegs++;

    regAddr[numRegs] = 0xEA;
    regValue[numRegs] = ( UInt8 ) ( ( vbusAddr >> 16 ) & 0xFF );
    numRegs++;

    if ( len )
    {
        /*
         * set VBUS address value, if required
         */
        regAddr[numRegs] = 0xE0;
        regValue[numRegs] = val;
        numRegs++;
    }

    status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[devId],
                                regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/* Download patch to TVP5158  */
Int32 Vps_tvp5158PatchDownload ( Vps_Tvp5158Obj * pObj, UInt32 devId )
{
    Int32 status = FVID2_SOK;
    static UInt8 regAddr[256];
    UInt8 vbusStatus;
    UInt32 wrSize;
    UInt8 *patchAddr;
    UInt32 patchSize;
    Vps_VideoDecoderCreateParams *pCreateArgs;

    pCreateArgs = &pObj->createArgs;

    patchAddr = ( UInt8 * ) gVps_tvp5158Patch;
    patchSize = sizeof ( gVps_tvp5158Patch );

    /*
     * select all TVP5158 core's
     */
    status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
    if ( status != FVID2_SOK )
        return status;

    status = Vps_tvp5158VbusRead ( pObj, devId, 0xB00060, &vbusStatus );
    if ( status != FVID2_SOK )
        return status;

    /*
     * if no patch download then return from here
     */
#ifdef VPS_TVP5158_NO_PATCH_DOWNLOAD
    {
        #ifdef VPS_TVP5158_DEBUG
        Vps_printf
            ( " TVP5158: 0x%02x: NO Patch downloaded, using ROM firmware.\n",
                pCreateArgs->deviceI2cAddr[devId]
            );
        Vps_tvp5158PrintChipId(pObj);
        #endif
        return status;
    }
#endif

    /*
     * if force patch download then then skip checking of patch load status
     */
#ifndef VPS_TVP5158_FORCE_PATCH_DOWNLOAD
    if ( vbusStatus & 0x2 )
    {
        #ifdef VPS_TVP5158_DEBUG
        Vps_printf ( " TVP5158: 0x%02x: Patch is already running.\n",
            pCreateArgs->deviceI2cAddr[devId]
        );
        Vps_tvp5158PrintChipId(pObj);
        #endif
        return status;  // patch already running
    }
#endif

    #ifdef VPS_TVP5158_DEBUG
    Vps_printf
        ( " TVP5158: 0x%02x: Downloading patch ... \n",
            pCreateArgs->deviceI2cAddr[devId]
        );
    #endif

    /*
     * select all TVP5158 core's
     */
    status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
    if ( status != FVID2_SOK )
        return status;

    /*
     * keep firmware in reset state
     */
    vbusStatus |= 0x1;

    status = Vps_tvp5158VbusWrite ( pObj, devId, 0xB00060, vbusStatus, 1 );
    if ( status != FVID2_SOK )
        return status;

    /*
     * download patch
     */
    status = Vps_tvp5158VbusWrite ( pObj, devId, 0x400000, 0, 0 );
    if ( status != FVID2_SOK )
        return status;

    memset ( regAddr, 0xE1, sizeof ( regAddr ) );

    while ( patchSize )
    {

        if ( patchSize < sizeof ( regAddr ) )
            wrSize = patchSize;
        else
            wrSize = sizeof ( regAddr );

        status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                    pCreateArgs->deviceI2cAddr[devId],
                                    regAddr, patchAddr, wrSize );

        if ( status != FVID2_SOK )
            return status;

        patchAddr += wrSize;
        patchSize -= wrSize;
    }

    /*
     * keep in reset and apply patch
     */
    vbusStatus |= 0x3;

    status = Vps_tvp5158VbusWrite ( pObj, devId, 0xB00060, vbusStatus, 1 );
    if ( status != FVID2_SOK )
        return status;

    /*
     * release from reset
     */
    vbusStatus &= ~( 0x1 );

    status = Vps_tvp5158VbusWrite ( pObj, devId, 0xB00060, vbusStatus, 1 );
    if ( status != FVID2_SOK )
        return status;

    /*
     * wait for patch to get applied
     */
    Task_sleep ( 300 );

    #ifdef VPS_TVP5158_DEBUG
    Vps_printf
        ( " TVP5158: 0x%02x: Downloading patch ... DONE !!!\n",
            pCreateArgs->deviceI2cAddr[devId]
        );

    Vps_tvp5158PrintChipId(pObj);
    #endif


    return status;
}

Int32 Vps_tvp5158PrintChipId ( Vps_Tvp5158Obj * pObj)
{
    Vps_VideoDecoderChipIdParams vidDecChipIdArgs;
    Vps_VideoDecoderChipIdStatus vidDecChipIdStatus;
    Int32 status;

    vidDecChipIdArgs.deviceNum = 0;

    status = Vps_tvp5158GetChipId(
        pObj,
        &vidDecChipIdArgs,
        &vidDecChipIdStatus
        );

    Vps_printf(" TVP5158: 0x%02x: %04x:%04x:%04x\n",
        pObj->createArgs.deviceI2cAddr[0],
        vidDecChipIdStatus.chipId,
        vidDecChipIdStatus.chipRevision,
        vidDecChipIdStatus.firmwareVersion
      );

    return status;
}

/*
  Get TP5158 chip ID, revision ID and firmware patch ID
*/
Int32 Vps_tvp5158GetChipId ( Vps_Tvp5158Obj * pObj,
                             Vps_VideoDecoderChipIdParams * pPrm,
                             Vps_VideoDecoderChipIdStatus * pStatus )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;

    if ( pStatus == NULL || pPrm == NULL )
        return FVID2_EBADARGS;

    memset ( pStatus, 0, sizeof ( *pStatus ) );

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum >= pCreateArgs->numDevicesAtPort )
        return FVID2_EBADARGS;

    numRegs = 0;

    regAddr[numRegs] = VPS_TVP5158_REG_CHIP_ID_MSB;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_CHIP_ID_LSB;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_ROM_VERSION;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_RAM_VERSION_0;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_RAM_VERSION_1;
    regValue[numRegs] = 0;
    numRegs++;

    status = Vps_deviceRead8 ( pCreateArgs->deviceI2cInstId,
                               pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                               regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    pStatus->chipId = ( ( UInt32 ) regValue[0] << 8 ) | regValue[1];
    pStatus->chipRevision = regValue[2];
    pStatus->firmwareVersion = (regValue[3] << 8)| (regValue[4]);

    return status;
}

/*
  Get TVP5158 detect vide standard status

  Can be called for each channel
*/
Int32 Vps_tvp5158GetVideoStatus ( Vps_Tvp5158Obj * pObj,
                                  Vps_VideoDecoderVideoStatusParams * pPrm,
                                  Vps_VideoDecoderVideoStatus * pStatus )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt32 devId, chId, std;

    if ( pStatus == NULL || pPrm == NULL )
        return FVID2_EBADARGS;

    memset ( pStatus, 0, sizeof ( *pStatus ) );

    pCreateArgs = &pObj->createArgs;

    /*
     * Identify channel/core number and device number from channelNum
     */
    devId = pPrm->channelNum / VPS_TVP5158_CH_PER_DEVICE_MAX;
    chId = pPrm->channelNum % VPS_TVP5158_CH_PER_DEVICE_MAX;

    /* There can be at max 2 TVP5158 devices per port */
    if ( devId >= VPS_TVP5158_DEV_PER_PORT_MAX )
        return FVID2_EBADARGS;

    numRegs = 0;

    /*
     * select appropiate core
     */
    regAddr[numRegs] = VPS_TVP5158_REG_DEC_RD_EN;
    regValue[numRegs] = 1 << chId;
    numRegs++;

    status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[devId],
                                regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    /*
     * read status
     */

    numRegs = 0;

    regAddr[numRegs] = VPS_TVP5158_REG_STATUS_1;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_STATUS_2;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_VID_STD_STATUS;
    regValue[numRegs] = 0;
    numRegs++;

    status = Vps_deviceRead8 ( pCreateArgs->deviceI2cInstId,
                               pCreateArgs->deviceI2cAddr[devId],
                               regAddr, regValue, numRegs );
    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    if ( ( regValue[0] & VPS_TVP5158_HSYNC_LOCKED )
         && ( regValue[0] & VPS_TVP5158_VSYNC_LOCKED )
         && ( regValue[1] & VPS_TVP5158_SIGNAL_DETECT ) )
    {
        pStatus->isVideoDetect = TRUE;
    }

    if ( pStatus->isVideoDetect )
    {
        /*
         * since input to TVP5158 is always interlaced
         */
        pStatus->isInterlaced = TRUE;

        /*
         * 60Hz, i.e 16.667msec per field
         */
        pStatus->frameInterval = 16667;

        if ( ( regValue[0] & VPS_TVP5158_SIGNAL_60HZ ) )    /* is 50Hz or 60Hz ? */
        {
            /*
             * 50Hz, i.e 20msec per field
             */
            pStatus->frameInterval = 20000;
        }

        /*
         * frame width is always 720 pixels
         */
        pStatus->frameWidth = VPS_TVP5158_NTSC_PAL_WIDTH;
        pStatus->frameHeight = 0;

        std = ( regValue[2] & VPS_TVP5158_VID_STD_MASK );   /* video standard */

        if ( std == VPS_TVP5158_VID_STD_PAL_BDGHIN  /* PAL (B,D,G,H,I,N) */
             || std == VPS_TVP5158_VID_STD_PAL_M    /* PAL (M) */
             || std == VPS_TVP5158_VID_STD_PAL_COMB_N   /* PAL (Combination-N) */
             || std == VPS_TVP5158_VID_STD_PAL_60   /* PAL 60  */
             )
        {
            /*
             * PAL standard
             */
            pStatus->frameHeight = VPS_TVP5158_PAL_HEIGHT;
        }
        if ( std == VPS_TVP5158_VID_STD_NTSC_MJ /* NTSC (M,J) */
             || std == VPS_TVP5158_VID_STD_NTSC_4_43    /* NTSC 4.43 */
             )
        {
            /*
             * NTSC standard
             */
            pStatus->frameHeight = VPS_TVP5158_NTSC_HEIGHT;
        }
    }

    return status;
}

/*
  Set video color related parameters
*/
Int32 Vps_tvp5158SetVideoColor ( Vps_Tvp5158Obj * pObj,
                                 Vps_VideoDecoderColorParams * pPrm )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt32 devId, chId;

    if ( pPrm == NULL )
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    devId = pPrm->channelNum / VPS_TVP5158_CH_PER_DEVICE_MAX;
    chId = pPrm->channelNum % VPS_TVP5158_CH_PER_DEVICE_MAX;
    chId = 1 << chId;

    /* There can be at max 2 TVP5158 devices per port */
    if ( devId >= VPS_TVP5158_DEV_PER_PORT_MAX )
        return FVID2_EBADARGS;

    numRegs = 0;

    /*
     * set core ID
     */
    regAddr[numRegs] = VPS_TVP5158_REG_DEC_WR_EN;
    regValue[numRegs] = chId;
    numRegs++;

    /*
     * set video processing parameters
     */
    regAddr[numRegs] = VPS_TVP5158_REG_Y_BRIGHTNESS;

    if ( pPrm->videoBrightness == VPS_VIDEO_DECODER_DEFAULT )
    {
        regValue[numRegs] = 128;
        numRegs++;
    }
    else if ( pPrm->videoBrightness == VPS_VIDEO_DECODER_NO_CHANGE )
    {

    }
    else
    {
        regValue[numRegs] = pPrm->videoBrightness;
        numRegs++;
    }

    regAddr[numRegs] = VPS_TVP5158_REG_Y_CONTRAST;

    if ( pPrm->videoContrast == VPS_VIDEO_DECODER_DEFAULT )
    {
        regValue[numRegs] = 128;
        numRegs++;
    }
    else if ( pPrm->videoContrast == VPS_VIDEO_DECODER_NO_CHANGE )
    {

    }
    else
    {
        regValue[numRegs] = pPrm->videoContrast;
        numRegs++;
    }

    regAddr[numRegs] = VPS_TVP5158_REG_C_SATURATION;

    if ( pPrm->videoSaturation == VPS_VIDEO_DECODER_DEFAULT )
    {
        regValue[numRegs] = 128;
        numRegs++;
    }
    else if ( pPrm->videoSaturation == VPS_VIDEO_DECODER_NO_CHANGE )
    {

    }
    else
    {
        regValue[numRegs] = pPrm->videoSaturation;
        numRegs++;
    }

    regAddr[numRegs] = VPS_TVP5158_REG_C_HUE;

    if ( pPrm->videoHue == VPS_VIDEO_DECODER_DEFAULT )
    {
        regValue[numRegs] = 128;
        numRegs++;
    }
    else if ( pPrm->videoHue == VPS_VIDEO_DECODER_NO_CHANGE )
    {

    }
    else
    {
        regValue[numRegs] = pPrm->videoHue;
        numRegs++;
    }

    /*
     * write to TVP5158
     */
    status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[devId],
                                regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  Set video noise filter related parameters
*/
Int32 Vps_tvp5158SetVideoNf ( Vps_Tvp5158Obj * pObj,
                              Vps_Tvp5158VideoNfParams * pPrm )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt32 devId, chId;

    if ( pPrm == NULL )
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    devId = pPrm->channelNum / VPS_TVP5158_CH_PER_DEVICE_MAX;
    chId = pPrm->channelNum % VPS_TVP5158_CH_PER_DEVICE_MAX;
    chId = 1 << chId;

    /* There can be at max 2 TVP5158 devices per port */
    if ( devId >= VPS_TVP5158_DEV_PER_PORT_MAX )
        return FVID2_EBADARGS;

    numRegs = 0;

    /*
     * set core ID
     */
    regAddr[numRegs] = VPS_TVP5158_REG_DEC_WR_EN;
    regValue[numRegs] = chId;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_NR_MAX_NOISE;
    regValue[numRegs] = pPrm->maxNoise;
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_NR_CTRL;
    regValue[numRegs] =
        ( ( !pPrm->nfEnable & 0x1 ) << 0 )
        | ( ( pPrm->colorKillerEnable & 0x1 ) << 4 )
        | (1<<3) /* Block width UV, 0: 128pixels, 1: 256pixels */
        | (0<<2) /* Block width  Y, 0: 256pixels, 1: 512pixels */
        ;
    numRegs++;

    status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[devId],
                                regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/* write to I2C registers */
Int32 Vps_tvp5158RegWrite ( Vps_Tvp5158Obj * pObj,
                            Vps_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return FVID2_EBADARGS;

    status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                                pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}

/* read from I2C registers */
Int32 Vps_tvp5158RegRead ( Vps_Tvp5158Obj * pObj,
                           Vps_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return FVID2_EBADARGS;

    status = Vps_deviceRead8 ( pCreateArgs->deviceI2cInstId,
                               pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                               pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}

/*
  Set audio related parameters
*/
Int32 Vps_tvp5158SetAudioMode ( Vps_Tvp5158Obj * pObj,
                                Vps_Tvp5158AudioModeParams * pPrm )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt32 devId;

    pCreateArgs = &pObj->createArgs;

    devId = pPrm->deviceNum;

    if ( devId >= pCreateArgs->numDevicesAtPort )
        return FVID2_EBADARGS;

    /*
     * select all core's
     */
    status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    numRegs = 0;

    regAddr[numRegs] = VPS_TVP5158_REG_AUDIO_SAMPLE_HZ;
    regValue[numRegs] = VPS_TVP5158_AUDIO_16KHZ;
    if ( pPrm->samplingHz == 8000 )
        regValue[numRegs] = VPS_TVP5158_AUDIO_8KHZ;
    if ( pPrm->samplingHz == 16000 )
        regValue[numRegs] = VPS_TVP5158_AUDIO_16KHZ;

    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_AUDIO_MIXER;
    switch(pPrm->tdmChannelNum)
    {
        case 0:
            regValue[numRegs] = VPS_TVP5158_AUDIO_TDM_2CH;
            break;
        default:
        case 1:
            regValue[numRegs] = VPS_TVP5158_AUDIO_TDM_4CH;
            break;
        case 2:
            regValue[numRegs] = VPS_TVP5158_AUDIO_TDM_8CH;
            break;
        case 3:
            regValue[numRegs] = VPS_TVP5158_AUDIO_TDM_12CH;
            break;
        case 4:
            regValue[numRegs] = VPS_TVP5158_AUDIO_TDM_16CH;
            break;
    }
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_AUDIO_CASCADE;
    regValue[numRegs] = ( pPrm->cascadeStage & 0x3 );
    numRegs++;

    regAddr[numRegs] = VPS_TVP5158_REG_AUDIO_CTRL;
    /* Added check for tdmChannelNum also, along with cascade stage */
    if (pPrm->cascadeStage || pPrm->tdmChannelNum > VPS_TVP5158_AUDIO_TDM_4CH)
    {
        regValue[numRegs] = 0x00
            | ( 0 << 7 )    /* 0: SD_M disable, 1: SD_M enable */
            | ( 1 << 6 )    /* 0: SD_R disable, 1: SD_R enable */
            | ( ( pPrm->masterModeEnable & 0x1 ) << 5 )
            /*
             * 0: Slave mode, 1: Master mode
             */
            | ( ( pPrm->dspModeEnable & 0x1 ) << 4 )
            /*
             * 0: I2S mode, 1: DSP mode
             */
            | ( 0 << 3 )        /* 0: 256fs, 1: 64fs */
            | ( ( pPrm->ulawEnable & 0x1 ) << 2 )   /* 0: 16-bit PCM,
                                                     * 1: 8-bit ulaw, 2:8-bit Alaw */
            | ( 0 << 0 )    /* 0: SD_R only, 1: SD_R + SD_M  */
            ;
    }
    else
    {
        regValue[numRegs] = 0x00
            | ( 0 << 7 )    /* 0: SD_M disable, 1: SD_M enable */
            | ( 1 << 6 )    /* 0: SD_R disable, 1: SD_R enable */
            | ( ( pPrm->masterModeEnable & 0x1 ) << 5 )
            /*
             * 0: Slave mode, 1: Master mode
             */
            | ( ( pPrm->dspModeEnable & 0x1 ) << 4 )
            /*
             * 0: I2S mode, 1: DSP mode
             */
            | ( 1 << 3 )        /* 0: 256fs, 1: 64fs */
            | ( ( pPrm->ulawEnable & 0x1 ) << 2 )   /* 0: 16-bit PCM,
                                                     * 1: 8-bit ulaw, 2:8-bit Alaw */
            | ( 0 << 0 )    /* 0: SD_R only, 1: SD_R + SD_M  */
            ;
    }
    numRegs++;

    status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[devId],
                                regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    status = Vps_tvp5158SetAudioVolume ( pObj,
                                         VPS_TVP5158_CORE_ALL,
                                         pPrm->audioVolume );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  Set audio volume

  when coreId is VPS_TVP5158_CORE_ALL, same volume is set for all channels
*/
Int32 Vps_tvp5158SetAudioVolume ( Vps_Tvp5158Obj * pObj,
                                  UInt32 coreId, UInt32 audioVolume )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt32 devId, chId, numDev, vol;

    pCreateArgs = &pObj->createArgs;

    if ( coreId == VPS_TVP5158_CORE_ALL )
    {
        /*
         * setup all channels for all devices
         */
        devId = 0;

        chId = VPS_TVP5158_CORE_ALL;

        numDev = pCreateArgs->numDevicesAtPort;
    }
    else
    {
        devId = coreId / VPS_TVP5158_CH_PER_DEVICE_MAX;

        chId = coreId % VPS_TVP5158_CH_PER_DEVICE_MAX;

        numDev = 1;
    }

    if ( devId > VPS_TVP5158_DEV_PER_PORT_MAX )
        return FVID2_EBADARGS;

    vol = audioVolume & 0xF;

    while ( numDev-- )
    {
        status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        numRegs = 0;

        regAddr[numRegs] = VPS_TVP5158_REG_AUDIO_GAIN_1;
        regValue[numRegs] = 0;
        numRegs++;

        regAddr[numRegs] = VPS_TVP5158_REG_AUDIO_GAIN_2;
        regValue[numRegs] = 0;
        numRegs++;

        status = Vps_deviceRead8 ( pCreateArgs->deviceI2cInstId,
                                   pCreateArgs->deviceI2cAddr[devId],
                                   regAddr, regValue, numRegs );

        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        if ( chId == VPS_TVP5158_CORE_ALL )
        {
            regValue[0] = regValue[1] = ( vol << 4 ) | ( vol << 0 );
        }
        else if ( chId == 0 )
        {
            regValue[0] &= 0xF0;
            regValue[0] |= ( vol << 0 );
        }
        else if ( chId == 1 )
        {
            regValue[0] &= 0x0F;
            regValue[0] |= ( vol << 4 );
        }
        else if ( chId == 2 )
        {
            regValue[1] &= 0xF0;
            regValue[1] |= ( vol << 0 );
        }
        else if ( chId == 3 )
        {
            regValue[1] &= 0x0F;
            regValue[1] |= ( vol << 4 );
        }

        status = Vps_deviceWrite8 ( pCreateArgs->deviceI2cInstId,
                                    pCreateArgs->deviceI2cAddr[devId],
                                    regAddr, regValue, numRegs );

        devId++;
    }

    return status;
}


Int32 Vps_tvp5158SetIndirectRegisters(Vps_Tvp5158Obj * pObj)
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt32 devId, numRegs, i;
    volatile UInt32 vbusAddr;
    volatile UInt8 vbusVal8;

    pCreateArgs = &pObj->createArgs;

    for(devId=0; devId<pCreateArgs->numDevicesAtPort; devId++)
    {
        status = Vps_tvp5158SelectWrite ( pObj, devId, VPS_TVP5158_CORE_ALL );
        if ( status != FVID2_SOK )
            return FVID2_ETIMEOUT;

        numRegs
            = (sizeof(gVps_tvp5158VbusAddrValueSet)/sizeof(gVps_tvp5158VbusAddrValueSet[0]))/2;

        for(i=0; i<numRegs; i++)
        {
            /* 24-bit Vbus Addr */
            vbusAddr = (gVps_tvp5158VbusAddrValueSet[2*i] & 0xFFFFFF);

            if(vbusAddr==0xFFFFFF)
                break;

            /* 8-bit Vbus value */
            vbusVal8 = (gVps_tvp5158VbusAddrValueSet[2*i+1] & 0xFF);

            status = Vps_tvp5158VbusWrite(pObj, devId, vbusAddr, vbusVal8, 1);
            if ( status != FVID2_SOK )
                return FVID2_ETIMEOUT;

            #ifdef VPS_TVP5158_VERBOSE_DEBUG
            /* read back and print */
            vbusVal8 = 0;

            status = Vps_tvp5158VbusRead(pObj, devId, vbusAddr, (UInt8*)&vbusVal8);
            if ( status != FVID2_SOK )
                return FVID2_ETIMEOUT;

            Vps_printf
                ( " TVP5158: 0x%02x: VBUS 0x%08x = 0x%02x\n",
                    pCreateArgs->deviceI2cAddr[devId], vbusAddr, vbusVal8
                );
            #endif
        }
    }

    return FVID2_SOK;
}
