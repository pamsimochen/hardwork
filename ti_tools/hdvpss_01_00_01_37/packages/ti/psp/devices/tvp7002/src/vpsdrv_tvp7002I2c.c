/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_tvp7002I2c.c
 *
 *  \brief TVP7002 video encoder FVID2 driver implementation file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/devices/tvp7002/src/vpsdrv_tvp7002Priv.h>
#include <ti/psp/devices/tvp7002/src/vpsdrv_tvp7002Settings.h>
#include <ti/psp/platforms/vps_platform.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */






/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/**
 *  \brief Setup video modes.
 */
Int32 Vps_tvp7002SetupVideo(Vps_Tvp7002HandleObj *pObj,
                                   Vps_VideoDecoderVideoModeParams *pPrm)
{
    Int32           retVal = FVID2_SOK;
    UInt8           numRegs;
    UInt8           devAddr;
    UInt32          i2cInstId;
    UInt8           *regAddrList;
    static UInt8    regValList[sizeof(gTvp7002ModeAddrList)];
    Vps_PlatformCpuRev cpuRev;
    Vps_PlatformId platform;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    GT_assert(VpsDeviceTrace, (NULL != pPrm));

    /* Determine Pltform and CPU version */
    platform = Vps_platformGetId();
    cpuRev   = Vps_platformGetCpuRev();

    if (((platform <= VPS_PLATFORM_ID_UNKNOWN) ||
         (platform >= VPS_PLATFORM_ID_MAX)) ||
        (cpuRev >= VPS_PLATFORM_CPU_REV_UNKNOWN))
    {
        GT_assert(VpsDeviceTrace, (FALSE) );
    }
    /* Enable THS filter */
    retVal = Vps_platformEnableTvp7002Filter((FVID2_Standard) pPrm->standard);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "THS filter programming failed\n");
    }

    if (FVID2_SOK == retVal)
    {
        devAddr = pObj->createArgs.deviceI2cAddr[0u];
        i2cInstId = pObj->createArgs.deviceI2cInstId;

        /* Check for valid array sizes */
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002DefAddrList) == sizeof(gTvp7002DefValueList));
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002ModeAddrList) ==
                sizeof(gTvp7002Value1080p60EmbSync));
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002ModeAddrList) ==
                sizeof(gTvp7002Value1080i60EmbSync));
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002ModeAddrList) ==
                sizeof(gTvp7002Value720p60EmbSync));
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002ModeAddrList) ==
                sizeof(gTvp7002ValueSxga60));
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002ModeAddrList) ==
                sizeof(gTvp7002Value1080p60DiscSync));
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002ModeAddrList) ==
                sizeof(gTvp7002Value1080i60DiscSync));
        GT_assert(VpsDeviceTrace,
            sizeof(gTvp7002ModeAddrList) ==
                sizeof(gTvp7002Value720p60DiscSync));

        regAddrList = (UInt8*)&gTvp7002ModeAddrList[0];
        numRegs = sizeof(gTvp7002ModeAddrList);

        /* Select the register values depending on standard */
        switch (pPrm->standard)
        {
            case FVID2_STD_1080P_60:
                if (pPrm->videoCaptureMode ==
                        VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC)
                {
                    memcpy(regValList, gTvp7002Value1080p60DiscSync, numRegs);
                }
                else
                {
                    memcpy(regValList, gTvp7002Value1080p60EmbSync, numRegs);
                }
                break;

            case FVID2_STD_1080I_60:
            case FVID2_STD_AUTO_DETECT:
                if (pPrm->videoCaptureMode ==
                        VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC)
                {
                    memcpy(regValList, gTvp7002Value1080i60DiscSync, numRegs);
                }
                else
                {
                    memcpy(regValList, gTvp7002Value1080i60EmbSync, numRegs);
                }
                break;

            case FVID2_STD_720P_60:
                if (pPrm->videoCaptureMode ==
                        VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC)
                {
                    memcpy(regValList, gTvp7002Value720p60DiscSync, numRegs);
                }
                else
                {
                    memcpy(regValList, gTvp7002Value720p60EmbSync, numRegs);
                }
                break;

            case FVID2_STD_SXGA_60:
                memcpy(regValList, gTvp7002ValueSxga60, numRegs);
                break;

            default:
                retVal = FVID2_EINVALID_PARAMS;
                GT_0trace(VpsDeviceTrace, GT_ERR, "Invalid standard\n");
                break;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Write the common settings for all the modes */
        retVal |= Vps_tvp7002I2cWrite8(
                      pObj,
                      i2cInstId,
                      devAddr,
                      gTvp7002DefAddrList,
                      gTvp7002DefValueList,
                      sizeof(gTvp7002DefAddrList));

        if (pPrm->videoCaptureMode==VPS_CAPT_VIDEO_CAPTURE_MODE_SINGLE_CH_NON_MUX_DISCRETE_SYNC_ACTVID_VSYNC)
        {
            UInt8  idx;

            /* discrete sync mode, adjust AVID START, AVID END, VBLK START, VBLK DURATION */
            for(idx = 0; idx < numRegs; idx++)
            {
                if (!(((platform == VPS_PLATFORM_ID_EVM_TI814x) &&
                        (cpuRev >= VPS_PLATFORM_CPU_REV_2_1)) ||
                      ((platform == VPS_PLATFORM_ID_EVM_TI816x) &&
                        (cpuRev >= VPS_PLATFORM_CPU_REV_2_0))) ||
                       (platform == VPS_PLATFORM_ID_EVM_TI8107))
                {
                    if (regAddrList[idx]==VPS_TVP7002_REG_VBLK_FLD0_START_OFFSET ||
                        regAddrList[idx]==VPS_TVP7002_REG_VBLK_FLD1_START_OFFSET)
                    {
                        regValList[idx] = 0x0;
                    }
                    else if (regAddrList[idx]==VPS_TVP7002_REG_VBLK_FLD0_DURATION ||
                             regAddrList[idx]==VPS_TVP7002_REG_VBLK_FLD1_DURATION)
                    {
                        regValList[idx] = 0x1;
                    }
                }
            }
        }

        /* Write the mode specific settings */
        retVal |= Vps_tvp7002I2cWrite8(
                      pObj,
                      i2cInstId,
                      devAddr,
                      regAddrList,
                      regValList,
                      numRegs);
    }

    if (FVID2_SOK == retVal)
    {
        /* Set the output format */
        retVal = Vps_tvp7002SetOutputFormat(pObj, pPrm);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDeviceTrace, GT_ERR, "Set output format failed\n");
        }
    }

    return (retVal);
}



/**
 *  \brief Setup output format.
 */
Int32 Vps_tvp7002SetOutputFormat(Vps_Tvp7002HandleObj *pObj,
                                        Vps_VideoDecoderVideoModeParams *pPrm)
{
    Int32           retVal = FVID2_SOK;
    UInt8           regAddr;
    UInt8           regVal;
    UInt8           numRegs;
    UInt8           devAddr;
    UInt32          i2cInstId;
    UInt32          order;
    UInt32          outFmt;
    UInt32          cscEnable;
    UInt32          isRgbInput;
    UInt32          codingRange;
    UInt32          embeddedSync;
    Bool            cscY2R;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    GT_assert(VpsDeviceTrace, (NULL != pPrm));

    devAddr = pObj->createArgs.deviceI2cAddr[0u];
    i2cInstId = pObj->createArgs.deviceI2cInstId;

    order = VPS_TVP7002_CRCB_ORDER;
    if ((FVID2_DF_YUV422I_YVYU == pPrm->videoDataFormat) ||
        (FVID2_DF_YUV422I_VYUY == pPrm->videoDataFormat))
    {
        order = VPS_TVP7002_CBCR_ORDER;
    }

    if ((pPrm->standard >= FVID2_STD_VGA_60) &&
        (pPrm->standard <= FVID2_STD_UXGA_60))
    {
        /* VESA input */
        isRgbInput = TRUE;
    }
    else
    {
        /* YPbPr input */
        isRgbInput = FALSE;
    }

    outFmt = VPS_TVP7002_OUT_FORMAT_20BIT;

    cscEnable = FALSE;
    if (TRUE == isRgbInput)
        cscEnable = TRUE;

    codingRange = VPS_TVP7002_RANGE_BT601;
    embeddedSync = TRUE;
    cscY2R       = FALSE;

    switch (pPrm->videoDataFormat)
    {
        case FVID2_DF_YUV422I_UYVY:
        case FVID2_DF_YUV422I_YUYV:
        case FVID2_DF_YUV422I_YVYU:
        case FVID2_DF_YUV422I_VYUY:
        case FVID2_DF_YUV422P:
        case FVID2_DF_YUV422SP_UV:
        case FVID2_DF_YUV422SP_VU:
            break;

        case FVID2_DF_YUV444P:
        case FVID2_DF_YUV444I:
            outFmt = VPS_TVP7002_OUT_FORMAT_30BIT;
            embeddedSync = FALSE;
            break;

        case FVID2_DF_RGB24_888:
            outFmt = VPS_TVP7002_OUT_FORMAT_30BIT;
            cscEnable = FALSE;
            if (FALSE == isRgbInput)
            {
                cscEnable = TRUE;
                cscY2R = TRUE;
            }
            codingRange = VPS_TVP7002_RANGE_RGB;
            embeddedSync = FALSE;
            break;

        default:
            GT_0trace(VpsDeviceTrace, GT_ERR, "Invalid data format\n");
            retVal = FVID2_EINVALID_PARAMS;
            break;
    }

    if (FVID2_SOK == retVal)
    {
        numRegs = 1u;
        regAddr = VPS_TVP7002_REG_OUT_FMT;

        regVal = ((codingRange << VPS_TVP7002_OUT_FMT_RANGE_SHIFT) |
                 (order << VPS_TVP7002_OUT_FMT_ORDER_SHIFT) |
                 (outFmt << VPS_TVP7002_OUT_FMT_DECIMATION_SHIFT) |
                 (embeddedSync << VPS_TVP7002_OUT_FMT_SYNC_ENABLE_SHIFT));

        retVal = Vps_tvp7002I2cWrite8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     &regAddr,
                     &regVal,
                     numRegs);
    }

    if (FVID2_SOK == retVal)
    {
        /* Read the out formatter register */
        numRegs = 1u;
        regAddr = VPS_TVP7002_REG_MISC_CTRL3;
        regVal = 0u;
        retVal = Vps_tvp7002I2cRead8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     &regAddr,
                     &regVal,
                     numRegs);
        if (FVID2_SOK == retVal)
        {
            regVal &= ~(VPS_TVP7002_MISC_CTRL3_CSC_ENABLE_MASK);
            regVal |= (cscEnable << VPS_TVP7002_MISC_CTRL3_CSC_ENABLE_SHIFT);

            /* Write the modified value */
            retVal = Vps_tvp7002I2cWrite8(
                         pObj,
                         i2cInstId,
                         devAddr,
                         &regAddr,
                         &regVal,
                         numRegs);

            if(cscEnable)
            {
                #if 1
                if(cscY2R)
                {
                    /* Apply YUV444 to RGB888 CSC here */
                    Vps_tvp7002ApplyCsc(pObj, gTvp7002CscCoeffYuv2Rgb);
                }
                else
                {
                    /* Apply RGB888 to YUV444 CSC here
                       Apply default values in TVP7002
                    */
                    Vps_tvp7002ApplyCsc(pObj, gTvp7002CscCoeffRgb2Yuv);
                }
                #endif
            }
        }
    }

    return (retVal);
}



Int32 Vps_tvp7002ApplyCsc(Vps_Tvp7002HandleObj *pObj,
                                    UInt16 *cscCoeffs)
{
    Int32           retVal = FVID2_SOK;
    UInt8           regAddr[VPS_TVP7002_NUM_CSC_COEFF_REGS];
    UInt8           regVal[VPS_TVP7002_NUM_CSC_COEFF_REGS];
    UInt8           numRegs;
    UInt8           devAddr;
    UInt32          i2cInstId, regId;


    devAddr = pObj->createArgs.deviceI2cAddr[0u];
    i2cInstId = pObj->createArgs.deviceI2cInstId;

    for(regId=0; regId<VPS_TVP7002_NUM_CSC_COEFF_REGS; regId++)
    {
        regAddr[regId] = VPS_TVP7002_REG_CSC_COEFF0_LSB + regId;
        if((regId%2)==0)
        {
            /* LSB */
            regVal[regId] = (cscCoeffs[regId/2] >> 0 ) & 0xFF;
        }
        else
        {
            /* MSB */
            regVal[regId] = (cscCoeffs[regId/2] >> 8 ) & 0xFF;
        }
    }
    numRegs = VPS_TVP7002_NUM_CSC_COEFF_REGS;

    retVal = Vps_tvp7002I2cWrite8(
                 pObj,
                 i2cInstId,
                 devAddr,
                 regAddr,
                 regVal,
                 numRegs);

    return retVal;
}

/**
 *  \brief Gets the video status of the detected video.
 */
Int32 Vps_tvp7002GetVideoStatusIoctl(Vps_Tvp7002HandleObj *pObj,
                                     Vps_VideoDecoderVideoStatusParams *pPrm,
                                     Vps_VideoDecoderVideoStatus *pStatus)
{
    Int32           retVal = FVID2_SOK;
    UInt8           regAddr[4u];
    UInt8           regVal[4u];
    UInt8           numRegs;
    UInt8           devAddr;
    UInt32          i2cInstId;
    UInt32          clocksPerLine;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    if ((NULL == pStatus) || (NULL == pPrm))
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer\n");
        retVal = FVID2_EBADARGS;
    }

    Task_sleep(100);

    if (FVID2_SOK == retVal)
    {
        VpsUtils_memset(pStatus, 0u, sizeof (*pStatus));

        devAddr = pObj->createArgs.deviceI2cAddr[0u];
        i2cInstId = pObj->createArgs.deviceI2cInstId;

        numRegs = 1;
        regAddr[0u] = VPS_TVP7002_REG_SYNC_DETECT_STATUS;
        regVal[0u] = 0;
        retVal = Vps_tvp7002I2cRead8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     regAddr,
                     regVal,
                     numRegs);
    }

    if (FVID2_SOK == retVal)
    {
        /* Check for video detect */
        if ((regVal[0u] & 0x02u) ||
           ((regVal[0u] & 0x80u) && (regVal[0u] & 0x10u)))
        {
            pStatus->isVideoDetect = TRUE;
        }
        else
        {
            pStatus->isVideoDetect = FALSE;
        }

        /* Read the other status registers */
        numRegs = 0u;
        regAddr[numRegs] = VPS_TVP7002_REG_LINES_PER_FRAME_STATUS_LSB;
        regVal[numRegs] = 0u;
        numRegs++;
        regAddr[numRegs] = VPS_TVP7002_REG_LINES_PER_FRAME_STATUS_MSB;
        regVal[numRegs] = 0u;
        numRegs++;
        regAddr[numRegs] = VPS_TVP7002_REG_CLOCK_PER_LINE_STATUS_LSB;
        regVal[numRegs] = 0u;
        numRegs++;
        regAddr[numRegs] = VPS_TVP7002_REG_CLOCK_PER_LINE_STATUS_MSB;
        regVal[numRegs] = 0u;
        numRegs++;
        retVal = Vps_tvp7002I2cRead8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     regAddr,
                     regVal,
                     numRegs);
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the status information */
        pStatus->frameHeight =
            (regVal[0u] | (((UInt32) regVal[1u] & 0x0Fu) << 8u));
        pStatus->isInterlaced = !(regVal[1u] & 0x20u);
        clocksPerLine = (regVal[2u] | (((UInt32) regVal[3u] & 0x0Fu) << 8u));
        pStatus->frameWidth = clocksPerLine; //TODO: How to derive width?
        /* Assuming 27MHz clock input */
        pStatus->frameInterval = (clocksPerLine * pStatus->frameHeight) / 27;
    }

    return (retVal);
}

/**
 *  \brief Sets the required video standard and output formats depending
 *  on requested parameters.
 */
Int32 Vps_tvp7002SetVideoModeIoctl(Vps_Tvp7002HandleObj *pObj,
                                   Vps_VideoDecoderVideoModeParams *pPrm)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    if (NULL == pPrm)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer\n");
        retVal = FVID2_EBADARGS;
    }

    /* Reset device */
    retVal = Vps_tvp7002ResetIoctl(pObj);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Device reset failed\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Setup video processing path based on request */
        retVal = Vps_tvp7002SetupVideo(pObj, pPrm);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDeviceTrace, GT_ERR, "Setup video failed\n");
        }
    }

    return (retVal);
}

/**
 *  \brief Gets TVP7002 Chip ID and revision ID.
 */
Int32 Vps_tvp7002GetChipIdIoctl(Vps_Tvp7002HandleObj *pObj,
                                Vps_VideoDecoderChipIdParams *pPrm,
                                Vps_VideoDecoderChipIdStatus *pStatus)
{
    Int32           retVal = FVID2_SOK;
    UInt8           regAddr;
    UInt8           regVal;
    UInt8           numRegs;
    UInt8           devAddr;
    UInt32          i2cInstId;

    /* Check for errors */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    if ((NULL == pStatus) ||
        (NULL == pPrm) ||
        (pPrm->deviceNum >= pObj->createArgs.numDevicesAtPort))
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        VpsUtils_memset(pStatus, 0u, sizeof (*pStatus));

        devAddr = pObj->createArgs.deviceI2cAddr[0u];
        i2cInstId = pObj->createArgs.deviceI2cInstId;

        /* Read teh chip revision register */
        numRegs = 1u;
        regAddr = VPS_TVP7002_REG_CHIP_REVISION;
        regVal = 0u;
        retVal = Vps_tvp7002I2cRead8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     &regAddr,
                     &regVal,
                     numRegs);
    }

    if (FVID2_SOK == retVal)
    {
        /* Update status */
        pStatus->chipId = 0x7002u;
        pStatus->chipRevision = regVal;
        pStatus->firmwareVersion = 0u;
    }

    return (retVal);
}


/**
 *  \brief Resets the TVP7002.
 *
 *  This API
 *      - Disable output
 *      - Power-ON of all module
 *      - Manual reset of TVP7002 and then setup in auto reset mode
 */
Int32 Vps_tvp7002ResetIoctl(Vps_Tvp7002HandleObj *pObj)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    /* Disable outputs */
    retVal = Vps_tvp7002StopIoctl(pObj);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Device stop failed\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Normal operation */
        retVal = Vps_tvp7002PowerDown(pObj, FALSE);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDeviceTrace, GT_ERR, "Device power-up failed\n");
        }
    }

    return (retVal);
}

/**
 *  \brief Starts TVP7002.
 */
Int32 Vps_tvp7002StartIoctl(Vps_Tvp7002HandleObj *pObj)
{
    Int32       retVal;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    /* Enable output */
    retVal = Vps_tvp7002OutputEnable(pObj, TRUE);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Output enable failed\n");
    }

    return (retVal);
}



/**
 *  \brief Stops TVP7002.
 */
Int32 Vps_tvp7002StopIoctl(Vps_Tvp7002HandleObj *pObj)
{
    Int32       retVal;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    /* Disable output */
    retVal = Vps_tvp7002OutputEnable(pObj, FALSE);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Output disable failed\n");
    }

    return (retVal);
}

/**
 *  \brief Power ON/OFF TVP7002.
 */
Int32 Vps_tvp7002PowerDown(Vps_Tvp7002HandleObj *pObj, UInt32 powerDown)
{
    Int32           retVal = FVID2_SOK;
    UInt8           regAddr;
    UInt8           regVal;
    UInt8           numRegs;
    UInt8           devAddr;
    UInt32          i2cInstId;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    devAddr = pObj->createArgs.deviceI2cAddr[0u];
    i2cInstId = pObj->createArgs.deviceI2cInstId;

    numRegs = 1u;
    regAddr = VPS_TVP7002_REG_HPLL_CLAMP_CTRL;
    regVal = 0u;
    retVal = Vps_tvp7002I2cRead8(
                 pObj,
                 i2cInstId,
                 devAddr,
                 &regAddr,
                 &regVal,
                 numRegs);
    if (FVID2_SOK == retVal)
    {
        if (TRUE == powerDown)
        {
            regVal &= ~0x02;    /* Power down */
        }
        else
        {
            regVal |= 0x02;     /* Normal operation */
        }

        retVal = Vps_tvp7002I2cWrite8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     &regAddr,
                     &regVal,
                     numRegs);
    }

    return (retVal);
}



/**
 *  \brief Enable output port.
 */
Int32 Vps_tvp7002OutputEnable(Vps_Tvp7002HandleObj *pObj, UInt32 enable)
{
    Int32           retVal = FVID2_SOK;
    UInt8           regAddr;
    UInt8           regVal;
    UInt8           numRegs;
    UInt8           devAddr;
    UInt32          i2cInstId;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));

    devAddr = pObj->createArgs.deviceI2cAddr[0u];
    i2cInstId = pObj->createArgs.deviceI2cInstId;

    numRegs = 1u;
    regAddr = VPS_TVP7002_REG_MISC_CTRL2;
    regVal = 0u;
    retVal = Vps_tvp7002I2cRead8(
                 pObj,
                 i2cInstId,
                 devAddr,
                 &regAddr,
                 &regVal,
                 numRegs);
    if (FVID2_SOK == retVal)
    {
        if (TRUE == enable)
        {
            regVal &= ~0x01;    /* Enable output */
        }
        else
        {
            regVal |= 0x01;     /* Disable output */
        }

        retVal = Vps_tvp7002I2cWrite8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     &regAddr,
                     &regVal,
                     numRegs);
    }

    return (retVal);
}



/**
 *  \brief I2C read wrapper function.
 */
Int32 Vps_tvp7002I2cRead8(Vps_Tvp7002HandleObj *pObj,
                                 UInt32 i2cInstId,
                                 UInt32 i2cDevAddr,
                                 const UInt8 *regAddr,
                                 UInt8 *regVal,
                                 UInt32 numRegs)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    GT_assert(VpsDeviceTrace, (NULL != regAddr));
    GT_assert(VpsDeviceTrace, (NULL != regVal));

    retVal = Vps_deviceRead8(i2cInstId, i2cDevAddr, regAddr, regVal, numRegs);
    if (FVID2_SOK == retVal)
    {

    }
    else
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "I2C read failed\n");
    }

    return (retVal);
}



/**
 *  \brief I2C write wrapper function.
 */
Int32 Vps_tvp7002I2cWrite8(Vps_Tvp7002HandleObj *pObj,
                                  UInt32 i2cInstId,
                                  UInt32 i2cDevAddr,
                                  const UInt8 *regAddr,
                                  UInt8 *regVal,
                                  UInt32 numRegs)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    GT_assert(VpsDeviceTrace, (NULL != regAddr));
    GT_assert(VpsDeviceTrace, (NULL != regVal));

    retVal = Vps_deviceWrite8(i2cInstId, i2cDevAddr, regAddr, regVal, numRegs);
    if (FVID2_SOK == retVal)
    {

    }
    else
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "I2C write failed\n");
    }

    return (retVal);
}


/**
 *  \brief Writes to device registers.
 */
Int32 Vps_tvp7002RegWriteIoctl(Vps_Tvp7002HandleObj *pObj,
                               Vps_VideoDecoderRegRdWrParams *pPrm)
{
    Int32           retVal = FVID2_SOK;
    UInt8           devAddr;
    UInt32          i2cInstId;

    /* Check for errors */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    if ((NULL == pPrm) ||
        (NULL == pPrm->regAddr) ||
        (NULL == pPrm->regValue8) ||
        (0u == pPrm->numRegs) ||
        (pPrm->deviceNum >= pObj->createArgs.numDevicesAtPort))
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer/Invalid arguments\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        devAddr = pObj->createArgs.deviceI2cAddr[0u];
        i2cInstId = pObj->createArgs.deviceI2cInstId;
        retVal = Vps_tvp7002I2cWrite8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     pPrm->regAddr,
                     pPrm->regValue8,
                     pPrm->numRegs);
    }

    return (retVal);
}



/**
 *  \brief Reads from device registers.
 */
Int32 Vps_tvp7002RegReadIoctl(Vps_Tvp7002HandleObj *pObj,
                              Vps_VideoDecoderRegRdWrParams *pPrm)
{
    Int32           retVal = FVID2_SOK;
    UInt8           devAddr;
    UInt32          i2cInstId;

    /* Check for errors */
    GT_assert(VpsDeviceTrace, (NULL != pObj));
    if ((NULL == pPrm) ||
        (NULL == pPrm->regAddr) ||
        (NULL == pPrm->regValue8) ||
        (0u == pPrm->numRegs) ||
        (pPrm->deviceNum >= pObj->createArgs.numDevicesAtPort))
    {
        GT_0trace(VpsDeviceTrace, GT_ERR, "Null pointer/Invalid arguments\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        devAddr = pObj->createArgs.deviceI2cAddr[0u];
        i2cInstId = pObj->createArgs.deviceI2cInstId;

        VpsUtils_memset(pPrm->regValue8, 0u, pPrm->numRegs);
        retVal = Vps_tvp7002I2cRead8(
                     pObj,
                     i2cInstId,
                     devAddr,
                     pPrm->regAddr,
                     pPrm->regValue8,
                     pPrm->numRegs);
    }

    return (retVal);
}

