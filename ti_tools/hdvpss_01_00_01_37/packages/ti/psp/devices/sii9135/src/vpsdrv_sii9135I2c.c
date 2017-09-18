/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/devices/sii9135/src/vpsdrv_sii9135Priv.h>

Int32 Vps_sii9135I2cRead8 ( Vps_Sii9135Obj * pObj,
                        UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        UInt8 * regValue, UInt32 numRegs )
{
    Int32 status = FVID2_SOK;

    #ifdef VPS_SII9135_USE_REG_CACHE
    UInt32 reg, port;

    if(pObj->createArgs.deviceI2cAddr[0]==i2cDevAddr)
        port = 0;
    else
        port = 1;

    for(reg=0; reg<numRegs; reg++)
    {
        regValue[reg] = pObj->regCache[port][regAddr[reg]];
    }
    #else
    status = Vps_deviceRead8(
                    i2cInstId, i2cDevAddr,
                    regAddr, regValue,
                    numRegs
                    );

    if(status!=FVID2_SOK)
        return status;
    #endif

    return status;
}

Int32 Vps_sii9135I2cWrite8 ( Vps_Sii9135Obj * pObj,
                        UInt32 i2cInstId, UInt32 i2cDevAddr,
                        const UInt8 * regAddr,
                        UInt8 * regValue, UInt32 numRegs )
{
    Int32 status = FVID2_SOK;

    #ifdef VPS_SII9135_USE_REG_CACHE
    UInt32 reg, port;

    if(pObj->createArgs.deviceI2cAddr[0]==i2cDevAddr)
        port = 0;
    else
        port = 1;

    for(reg=0; reg<numRegs; reg++)
    {
        pObj->regCache[port][regAddr[reg]] = regValue[reg];
    }
    #endif

    status = Vps_deviceWrite8(
                    i2cInstId, i2cDevAddr,
                    regAddr, regValue,
                    numRegs
                    );

    if(status!=FVID2_SOK)
        return status;

    return status;
}

Int32 Vps_sii9135ResetRegCache(Vps_Sii9135Obj * pObj)
{
    #ifdef VPS_SII9135_USE_REG_CACHE

    memset(pObj->regCache, 0, sizeof(pObj->regCache));

    // for now hard code default values in registers 0x0 to 0x7
    pObj->regCache[0][0] = 0x01;
    pObj->regCache[0][1] = 0x00;
    pObj->regCache[0][2] = 0x35;
    pObj->regCache[0][3] = 0x91;
    pObj->regCache[0][4] = 0x03;
    pObj->regCache[0][5] = 0x00;
    pObj->regCache[0][6] = 0x1F; // assume signal is detected
    pObj->regCache[0][7] = 0x00;

    #endif

    return FVID2_SOK;
}

Int32 Vps_sii9135UpdateRegCache(Vps_Sii9135Obj * pObj)
{
    Int32 status = FVID2_SOK;

    #ifdef VPS_SII9135_USE_REG_CACHE

    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 numRegs = 0xf8;
    UInt8 devAddr[2];
    UInt8 regAddr[8];
    UInt8 regValue[8];

    pCreateArgs = &pObj->createArgs;

    devAddr[0] = pCreateArgs->deviceI2cAddr[0];
    devAddr[1] = pCreateArgs->deviceI2cAddr[0] + VPS_SII9135_I2C_PORT1_OFFSET;

    Vps_sii9135ResetRegCache(pObj);

    regAddr[0] = 0;
    regAddr[1] = 1;

    // dummy read to make next read io work
    status = Vps_deviceRead8(
                pCreateArgs->deviceI2cInstId,
                devAddr[0],
                regAddr,
                regValue,
                2
                );
    if(status!=FVID2_SOK)
        return status;

    status = Vps_deviceRawRead8(
                pCreateArgs->deviceI2cInstId,
                devAddr[0],
                &pObj->regCache[0][8],
                numRegs
                );
    if(status!=FVID2_SOK)
        return status;

    status = Vps_deviceRawRead8(
                pCreateArgs->deviceI2cInstId,
                devAddr[1],
                &pObj->regCache[1][8],
                numRegs
                );

    if(status!=FVID2_SOK)
        return status;

    #endif

    return status;
}

/*
  For SII9135 below parameters in Vps_VideoDecoderVideoModeParams are ignored
  and any value set by user for these parameters is not effective.

  videoIfMode
  standard
  videoCaptureMode
  videoSystem
  videoCropEnable

  Depending on video data format SII9135 is configured as below

  videoDataFormat =
   FVID2_DF_YUV422P   : 16-bit YUV422 single CH embedded sync auto-detect mode
   FVID2_DF_YUV444P   : 24-bit YUV444 single CH discrete sync auto-detect mode
   FVID2_DF_RGB24_888 : 24-bit RGB24  single CH discrete sync auto-detect mode
*/
Int32 Vps_sii9135SetVideoMode ( Vps_Sii9135Obj * pObj,
                                Vps_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = FVID2_SOK;
    Vps_Sii9135AviInfoFrame aviInfo;

    /*
     * reset device
     */
    status = Vps_sii9135Reset ( pObj );
    if ( status != FVID2_SOK )
        return status;

    /*
     * detect video source properties
     */
    status = Vps_sii9135DetectVideo ( pObj,
                                      &aviInfo, pPrm->videoAutoDetectTimeout );

    if ( status != FVID2_SOK )
        return status;

    if(pPrm->standard==FVID2_STD_480I
        ||
        pPrm->standard==FVID2_STD_576I
        ||
        pPrm->standard==FVID2_STD_D1
        ||
        pPrm->standard==FVID2_STD_NTSC
        ||
        pPrm->standard==FVID2_STD_PAL
    )
    {
        aviInfo.pixelRepeat = VPS_SII9135_AVI_INFO_PIXREP_2X;
    }

    /*
     * setup video processing path based on detected source
     */
    status = Vps_sii9135SetupVideo ( pObj, &aviInfo, pPrm );
    if ( status != FVID2_SOK )
        return status;

    return status;
}

/*
  Setup video processing path based on detected source

  pAviInfoFrame - input source properties
  pPrm - user required parameters
*/
Int32 Vps_sii9135SetupVideo ( Vps_Sii9135Obj * pObj,
                              Vps_Sii9135AviInfoFrame * pAviInfo,
                              Vps_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[32];
    UInt8 regValue[32];
    UInt8 numRegs;
    UInt8 devAddr;
    UInt32 insSavEav, cscR2Y, upSmp, downSmp, chMap;
    UInt32 cscY2R, outColorSpace, isBT709, inPixRep;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];

    outColorSpace = VPS_SII9135_COLOR_FORMAT_YUV;
    if ( pPrm->videoDataFormat == FVID2_DF_RGB24_888 )
        outColorSpace = VPS_SII9135_COLOR_FORMAT_RGB;

    inPixRep = pAviInfo->pixelRepeat;
    if ( inPixRep > VPS_SII9135_AVI_INFO_PIXREP_4X )
    {
        inPixRep = 0;
        status = FVID2_EFAIL;
    }

    isBT709 = 1;
    if ( pAviInfo->colorImetric == VPS_SII9135_AVI_INFO_CMETRIC_ITU601 )
        isBT709 = 0;

    cscR2Y = 0;
    upSmp = 0;
    downSmp = 0;
    cscY2R = 0;
    insSavEav = 0;

    switch ( pAviInfo->colorSpace )
    {
        case VPS_SII9135_AVI_INFO_COLOR_RGB444:
            switch ( pPrm->videoDataFormat )
            {
                case FVID2_DF_YUV422P:
                    cscR2Y = 1;
                    downSmp = 1;
                    insSavEav = 1;
                    break;

                case FVID2_DF_YUV444P:
                    cscR2Y = 1;
                    break;

                case FVID2_DF_RGB24_888:
                    break;

                default:
                    status = FVID2_EBADARGS;
                    break;
            }
            break;

        case VPS_SII9135_AVI_INFO_COLOR_YUV444:
            switch ( pPrm->videoDataFormat )
            {
                case FVID2_DF_YUV422P:
                    downSmp = 1;
                    insSavEav = 1;
                    break;

                case FVID2_DF_YUV444P:
                    break;

                case FVID2_DF_RGB24_888:
                    cscY2R = 1;
                    break;

                default:
                    status = FVID2_EBADARGS;
                    break;
            }
            break;

        case VPS_SII9135_AVI_INFO_COLOR_YUV422:
            switch ( pPrm->videoDataFormat )
            {
                case FVID2_DF_YUV422P:
                    insSavEav = 1;
                    break;

                case FVID2_DF_YUV444P:
                    upSmp = 1;
                    break;

                case FVID2_DF_RGB24_888:
                    upSmp = 1;
                    cscY2R = 1;
                    break;

                default:
                    status = FVID2_EBADARGS;
                    break;
            }
            break;

        default:
            status = FVID2_EFAIL;
            break;
    }

    numRegs = 0;

    regAddr[numRegs] = VPS_SII9135_REG_VID_MODE_1;
    regValue[numRegs] = ( insSavEav << 7 )  /* 1: SAV2EAV enable   , 0: disable */
        | ( 0 << 6 )    /* 1: Mux Y/C          , 0: No MUX  */
        | ( 1 << 5 )    /* 1: Dither enable    , 0: disable */
        | ( 1 << 4 )    /* 1: R2Y compress     , 0: bypass  */
        | ( cscR2Y << 3 )   /* 1: Enable R2Y CSC   , 0: bypass  */
        | ( upSmp << 2 )    /* 1: YUV422 to YUV444 , 0: bypass  */
        | ( downSmp << 1 )  /* 1: YUV444 to YUV422 , 0: bypass  */
        ;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_VID_MODE_2;
    regValue[numRegs] = ( 0 << 6 )  /* 0: Dither to 8bits, 1: 10bits, 2: 12bits  */
        | ( 0 << 5 )    /* 0: EVNODD LOW if field 0 is even, 1: HIGH */
        | ( 1 << 3 )    /* 1: Y2R compress     , 0: bypass           */
        | ( cscY2R << 2 )   /* 1: Y2R CSC          , 0: bypass           */
        | ( outColorSpace << 1 )    /* 0: Output format RGB, 1: YUV           */
        | ( 1 << 0 )    /* 1: Range clip enable, 0: disable          */
        ;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_VID_CTRL;
    regValue[numRegs] = ( 0 << 7 )  /* 0: Do not invert VSYNC, 1: invert */
        | ( 0 << 6 )    /* 0: Do not invert HSYNC, 1: invert */
        | ( isBT709 << 2 )  /* 0: Y2R BT601          , 1: BT709  */
        | ( 0 << 1 )    /* 0: 8bits RGB or YUV   , 1: YUV422 > 8 bits */
        | ( isBT709 << 0 )  /* 0: R2Y BT601          , 1: BT709  */
        ;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_SYS_CTRL_1;
    regValue[numRegs] = ( inPixRep << 6 )  /* 0: Output pixel clock divided 1x, 1: 1/2x, 3: 1/4x */
        | ( inPixRep << 4 ) /* 0: Input pixel replicate 1x, 1:2x, 3:4x            */
        | ( 1 << 2 )    /* 0: 12-bit mode         , 1: 24-bit mode      */
        | ( 0 << 1 )    /* 0: Normal output clock , 1: Invert clock     */
        | ( 1 << 0 )    /* 0: Power down          , 1: Normal operation */
        ;
    numRegs++;

    if((devAddr & 0x1)==0)
    {
        /* this is device connected to VIP1 and is connected in
            16-bit mode */
        chMap = 0x0;
    }
    else
    {
        /* this is device connected to VIP0 and is connected in
            24-bit mode */
        chMap = 0x5;
    }

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)

    /* On VC Card
        Q4-Q10  =>  D0 D7   ->  Read / CbCr
        Q16-Q23 =>  D8 D16  ->  Green / Y
        Q28-Q35 =>  D17 D23 ->  Blue */
    chMap = 0x05;
#endif /* TI_814X_BUILD || TI_8107_BUILD */
    /*     Q[23:16]  Q[15:8]  Q[7:0]
        0:     R        G       B
        1:     R        B       G
        2:     G        R       B
        3:     G        B       R
        4:     B        R       G
        5:     B        G       R
    */
    regAddr[numRegs] = VPS_SII9135_REG_VID_CH_MAP;
    regValue[numRegs] = chMap;
        ;
    numRegs++;

    status = Vps_sii9135I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );
    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  Read AVI packet info and parse information from it

  pAviInfo - parsed information returned by this API
*/
Int32 Vps_sii9135ReadAviInfo ( Vps_Sii9135Obj * pObj,
                               Vps_Sii9135AviInfoFrame * pAviInfo )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[32];
    UInt8 regValue[32];
    UInt8 numRegs;
    UInt8 devAddr;
    UInt8 aviAddr;
    UInt8 *aviData;

    /*
     * read AVI Info Frame
     */

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0] + VPS_SII9135_I2C_PORT1_OFFSET;

    numRegs = 0;

    for ( aviAddr = VPS_SII9135_REG_AVI_TYPE;
          aviAddr <= VPS_SII9135_REG_AVI_DBYTE15; aviAddr++ )
    {
        regAddr[numRegs] = aviAddr;
        regValue[numRegs] = 0;
        numRegs++;
    }

    status = Vps_sii9135I2cRead8 ( pObj,
                               pCreateArgs->deviceI2cInstId,
                               devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    status = FVID2_EFAIL;

    if ( regValue[0] == VPS_SII9135_AVI_INFO_PACKET_CODE
         && regValue[1] == VPS_SII9135_AVI_INFO_VERSION_CODE
         && regValue[2] == VPS_SII9135_AVI_INFO_PACKET_LENGTH )
    {
        /*
         * Valid AVI packet recevied
         */

        status = FVID2_SOK;

        aviData = &regValue[3]; /* point to start of AVI data checksum */

        /*
         * parse information
         */
        pAviInfo->colorSpace = ( ( aviData[1] >> 5 ) & 0x3 );
        pAviInfo->colorImetric = ( ( aviData[2] >> 6 ) & 0x3 );
        pAviInfo->pixelRepeat = ( ( aviData[5] >> 0 ) & 0xF );
    }

    return status;
}

/*
  Wait until video is detected and the get video properties
*/
Int32 Vps_sii9135DetectVideo ( Vps_Sii9135Obj * pObj,
                               Vps_Sii9135AviInfoFrame * pAviInfo,
                               UInt32 timeout )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderVideoStatus vidStatus;
    Vps_VideoDecoderVideoStatusParams vidStatusPrm;
    UInt32 loopTimeout, curTimeout = 0;

    vidStatusPrm.channelNum = 0;

    timeout = 1000; /* hard code for now */
    loopTimeout = 250;   /* check video status every 50 OS ticks */

    Task_sleep(100);

    while ( 1 )
    {
        status = Vps_sii9135GetVideoStatus ( pObj, &vidStatusPrm, &vidStatus );
        if ( status != FVID2_SOK )
            return status;

        if ( vidStatus.isVideoDetect )
        {
            break;
        }
        Task_sleep ( loopTimeout );

        if ( timeout != FVID2_TIMEOUT_FOREVER )
        {
            curTimeout += loopTimeout;

            if ( curTimeout >= timeout )
            {
                status = FVID2_ETIMEOUT;
                break;
            }
        }
    }

    /*
     * video detected, read AVI info
     */
    status |= Vps_sii9135ReadAviInfo ( pObj, pAviInfo );

    #ifndef VPS_SII9135_USE_AVI_FRAME_INFO
    // Hardcode AVI frame info
    pAviInfo->colorSpace = VPS_SII9135_AVI_INFO_COLOR_RGB444;
    pAviInfo->colorImetric = VPS_SII9135_AVI_INFO_CMETRIC_ITU709;
    pAviInfo->pixelRepeat = VPS_SII9135_AVI_INFO_PIXREP_NONE;
    status = FVID2_SOK;
    #endif

    return status;
}

/*
  Select TMDS core
*/
Int32 Vps_sii9135SelectTmdsCore ( Vps_Sii9135Obj * pObj, UInt32 coreId )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt8 devAddr;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];

    numRegs = 0;

    regAddr[numRegs] = VPS_SII9135_REG_SYS_SWTCHC;
    regValue[numRegs] = 0x80;    /* DDCDLY_EN = 1, default value */
    if ( coreId == 0 )
        regValue[numRegs] |= VPS_SII9135_SEL_TMDS_CORE_0;    /* select core 0 */
    else
        regValue[numRegs] |= VPS_SII9135_SEL_TMDS_CORE_1;    /* select core 1 */
    numRegs++;

    status = Vps_sii9135I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  This API
  - Disable output
  - Power-ON of all module
  - Manual reset of SII9135 and then setup in auto reset mode
  - Select default TMDS core
*/
Int32 Vps_sii9135Reset ( Vps_Sii9135Obj * pObj )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt8 devAddr;

    Vps_sii9135ResetRegCache(pObj);

    /*
     * disable outputs
     */
    status = Vps_sii9135Stop ( pObj );
    if ( status != FVID2_SOK )
        return status;

    /*
     * Normal operation
     */
    status = Vps_sii9135PowerDown ( pObj, FALSE );
    if ( status != FVID2_SOK )
        return status;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];

    numRegs = 0;

    regAddr[numRegs] = VPS_SII9135_REG_SW_RST_0;
    regValue[numRegs] = 0x2F;   /* reset AAC, HDCP, ACR, audio FIFO, SW */
    numRegs++;

    status = Vps_sii9135I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    /*
     * wait for reset to be effective
     */
    Task_sleep ( 50 );

    /*
     * remove reset and setup in auto-reset mode
     */
    numRegs = 0;

    regAddr[numRegs] = VPS_SII9135_REG_SW_RST_0;
    regValue[numRegs] = 0x10;   /* auto-reset SW */
    numRegs++;

    status = Vps_sii9135I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    /*
     * select TDMS core
     */
    status = Vps_sii9135SelectTmdsCore ( pObj, VPS_SII9135_CORE_ID_DEFAULT );
    if ( status != FVID2_SOK )
        return status;

    status = Vps_sii9135UpdateRegCache(pObj);
    if ( status != FVID2_SOK )
        return status;

    return status;
}

/*
  Power-ON All modules
*/
Int32 Vps_sii9135PowerDown ( Vps_Sii9135Obj * pObj, UInt32 powerDown )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt8 devAddr;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0];

    numRegs = 0;
    regAddr[numRegs] = VPS_SII9135_REG_SYS_CTRL_1;
    regValue[numRegs] = 0;
    numRegs++;

    status = Vps_sii9135I2cRead8 ( pObj,
                                pCreateArgs->deviceI2cInstId,
                               devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    if ( powerDown )
        regValue[0] = 0x04;  /* power down  */
    else
        regValue[0] = 0x05;  /* normal operation */

    status = Vps_sii9135I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  Enable output port
*/
Int32 Vps_sii9135OutputEnable ( Vps_Sii9135Obj * pObj, UInt32 enable )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;
    UInt8 devAddr;

    pCreateArgs = &pObj->createArgs;

    devAddr = pCreateArgs->deviceI2cAddr[0] + VPS_SII9135_I2C_PORT1_OFFSET;

    numRegs = 0;
    regAddr[numRegs] = VPS_SII9135_REG_SYS_PWR_DWN_2;
    regValue[numRegs] = 0xC3;   /* Enable all expect outputs  */
    if ( enable )
        regValue[numRegs] |= ( 1 << 2 );    /* Enable outputs  */

    regAddr[numRegs] = VPS_SII9135_REG_SYS_PWR_DWN;
    regValue[numRegs] = 0xAD;   /* Enable all expect outputs  */
    if ( enable )
        regValue[numRegs] |= ( 1 << 6 );    /* Enable outputs  */

    numRegs++;

    status = Vps_sii9135I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                devAddr, regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    return status;
}

/*
  Stop output port
*/
Int32 Vps_sii9135Stop ( Vps_Sii9135Obj * pObj )
{
    Int32 status = FVID2_SOK;

    status = Vps_sii9135OutputEnable ( pObj, FALSE );

    return status;
}

/*
  Start output port
*/
Int32 Vps_sii9135Start ( Vps_Sii9135Obj * pObj )
{
    Int32 status = FVID2_SOK;

    status = Vps_sii9135OutputEnable ( pObj, TRUE );

    return status;
}

/*
  Get Chip ID and revision ID
*/
Int32 Vps_sii9135GetChipId ( Vps_Sii9135Obj * pObj,
                             Vps_VideoDecoderChipIdParams * pPrm,
                             Vps_VideoDecoderChipIdStatus * pStatus )
{
    Int32 status = FVID2_SOK;
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

    regAddr[numRegs] = VPS_SII9135_REG_VND_IDL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_VND_IDH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_DEV_IDL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_DEV_IDH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_DEV_REV;
    regValue[numRegs] = 0;
    numRegs++;

    status = Vps_sii9135I2cRead8 (
                               pObj,
                               pCreateArgs->deviceI2cInstId,
                               pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                               regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    pStatus->chipId = ( ( UInt32 ) regValue[3] << 8 ) | regValue[2];
    pStatus->chipRevision = regValue[4];
    pStatus->firmwareVersion = ( ( UInt32 ) regValue[1] << 8 ) | regValue[0];

    return status;
}

/*
  Get and detect video status
*/
Int32 Vps_sii9135GetVideoStatus ( Vps_Sii9135Obj * pObj,
                                  Vps_VideoDecoderVideoStatusParams * pPrm,
                                  Vps_VideoDecoderVideoStatus * pStatus )
{
    Int32 status = FVID2_SOK;
    Vps_VideoDecoderCreateParams *pCreateArgs;
    UInt8 regAddr[16];
    UInt8 regValue[16];
    UInt8 numRegs;
    UInt32 hRes, vRes, dePix, deLin, xclkInPclk;
    UInt32 sysStat, vidStat;
    UInt32 t_line;

    if ( pStatus == NULL || pPrm == NULL )
        return FVID2_EBADARGS;

    memset ( pStatus, 0, sizeof ( *pStatus ) );

    pCreateArgs = &pObj->createArgs;

    numRegs = 0;

    regAddr[numRegs] = VPS_SII9135_REG_H_RESL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_H_RESH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_V_RESL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_V_RESH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_DE_PIXL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_DE_PIXH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_DE_LINL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_DE_LINH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_VID_XPCNTL;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_VID_XPCNTH;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_VID_STAT;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = VPS_SII9135_REG_STATE;
    regValue[numRegs] = 0;
    numRegs++;

    status = Vps_sii9135I2cRead8 (
                               pObj,
                               pCreateArgs->deviceI2cInstId,
                               pCreateArgs->deviceI2cAddr[0],
                               regAddr, regValue, numRegs );

    if ( status != FVID2_SOK )
        return FVID2_ETIMEOUT;

    /*
     * horizontal resolution
     */
    hRes = ( ( UInt32 ) regValue[1] << 8 ) | regValue[0];

    /*
     * vertical resolution
     */
    vRes = ( ( UInt32 ) regValue[3] << 8 ) | regValue[2];

    /*
     * horizontal active data resolution
     */
    dePix = ( ( UInt32 ) regValue[5] << 8 ) | regValue[4];

    /*
     * vertical active data resolution
     */
    deLin = ( ( UInt32 ) regValue[7] << 8 ) | regValue[6];

    /*
     * number of xclks per 2048 video clocks
     */
    xclkInPclk = ( ( UInt32 ) regValue[9] << 8 ) | regValue[8];

    /*
     * video status
     */
    vidStat = regValue[10];

    /*
     * system status
     */
    sysStat = regValue[11];

    if ( sysStat & VPS_SII9135_VID_DETECT )
    {
        pStatus->isVideoDetect = TRUE;

        if ( vidStat & VPS_SII9135_VID_INTERLACE )
        {
            pStatus->isInterlaced = TRUE;
        }

        pStatus->frameWidth = dePix;
        pStatus->frameHeight = deLin;

        /*
         * time interval in usecs for each line
         */
        t_line = ( UInt32 ) ( ( ( UInt32 ) hRes * xclkInPclk * 1000 ) / ( VPS_SII9135_FXTAL_KHZ * 2048 ) ); /* in usecs */

        /*
         * time interval in usecs for each frame/field
         */
        pStatus->frameInterval = t_line * vRes;

        #if 0
        Vps_printf(" SII9135: %dx%d@%dHz, %d\n",
                pStatus->frameWidth,
                pStatus->frameHeight,
                1000000/pStatus->frameInterval,
                pStatus->isInterlaced
            );
        #endif
    }

    return status;
}

/*
  write to device registers
*/
Int32 Vps_sii9135RegWrite ( Vps_Sii9135Obj * pObj,
                            Vps_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return FVID2_EBADARGS;

    status = Vps_sii9135I2cWrite8 ( pObj, pCreateArgs->deviceI2cInstId,
                                pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                                pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}

/*
  read from device registers
*/
Int32 Vps_sii9135RegRead ( Vps_Sii9135Obj * pObj,
                           Vps_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status;
    Vps_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return FVID2_EBADARGS;

    if ( pPrm->regAddr == NULL || pPrm->regValue8 == NULL
         || pPrm->numRegs == 0 )
        return FVID2_EBADARGS;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return FVID2_EBADARGS;

    memset ( pPrm->regValue8, 0, pPrm->numRegs );

    status = Vps_sii9135I2cRead8 ( pObj,
                               pCreateArgs->deviceI2cInstId,
                               pCreateArgs->deviceI2cAddr[pPrm->deviceNum],
                               pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}
