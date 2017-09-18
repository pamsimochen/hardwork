/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_hdmi9022a.c
 *
 *  \brief HDMI9022a driver
 *  This file implements functionality for the HDMI.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/psp/devices/sii9022a/src/vpsdrv_sii9022aPriv.h>
#include <ti/sysbios/knl/Semaphore.h>



/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
#define TPI_INPUTBUS_PIXEL_REP_BIT4_MASK                (0x10u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    FVID2_Standard    standard;
    UInt32            modeCode;
    UInt32            pixClk;
    UInt32            vFreq;
    UInt32            pixels;
    UInt32            lines;
    struct {
        UInt32            hBitToHSync;
        UInt32            field2Offset;
        UInt32            hWidth;
        UInt32            vBitToVSync;
        UInt32            vWidth;
    } embSyncPrms;
    struct {
        UInt32            deDelay;
        /**< Width of the area to the left of the active display */
        UInt32            deTop;
        /**< Height of the area to the top of the active display */
        UInt32            deCnt;
        /**< Width of the active display */
        UInt32            deLine;
        /**< Height of the active display */
    } extSyncPrms;
} VpsDrv_SiI9022AModeInfo;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static Vps_Sii9022aObj* vpsdrvSii9022AllocObj(void);

static Int32 vpsdrvSii9022aUnlock(void);

static Int32 vpsdrvSii9022aLock(void);

static Int32 vpsdrvSii9022aUnLockObj(Vps_Sii9022aObj * pSiiObj);

static Int32 vpsdrvSii9022aLockObj(Vps_Sii9022aObj * pSiiObj);

static Int32 vpsdrvSii9022DeAllocObj(Vps_Sii9022aObj* pObj);

static Int32 sii9022aGetHdmiChipId(Vps_Sii9022aObj* psiiObj,
                                   Vps_HdmiChipId *hdmichipId);

static Int32 vpsSii9022aGetDetailedChipId(Fdrv_Handle handle,
                                          Ptr cmdArgs,
                                          Ptr cmdStatusArgs);

static Int32 vpsSii9022aSetMode(Fdrv_Handle handle,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs);

static Int32 vpsSii9022aStart(Fdrv_Handle handle,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs);
static Int32 vpsSii9022aStop(Fdrv_Handle handle,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs);

static Int32 vpsSii9022aGetHpd(Fdrv_Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);

static Int32 vpsSii9022aSetPrms(Fdrv_Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);
static Int32 vpsSii9022aGetPrms(Fdrv_Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs);
static Int32 sii9022aDeviceInit(Vps_Sii9022aObj* pObj);

static Int32 sii9022aReset(Vps_Sii9022aObj* pSiiObj);
static Int32 sii9022aEnable(Vps_Sii9022aObj* pSiiObj);
static Int32 sii9022aPowerUpTxm(Vps_Sii9022aObj* pSiiObj);
static Int32 sii9022aCfgInBus(Vps_Sii9022aObj* pSiiObj);
static Int32 sii9022aCfgYcMode(Vps_Sii9022aObj* pSiiObj);
static Int32 sii9022aCfgSyncMode(Vps_Sii9022aObj* pSiiObj);
static Int32 vpsSii9022aPrgmEmbSyncTimingInfo(
                Vps_Sii9022aObj *siiObj,
                VpsDrv_SiI9022AModeInfo *siModeInfo);
static Int32 vpsSii9022aPrgmExtSyncTimingInfo(
                Vps_Sii9022aObj *siiObj,
                VpsDrv_SiI9022AModeInfo *siModeInfo);
static Int32 vpsSii9022aPrgmAvInfoFrame(
                Vps_Sii9022aObj *siiObj,
                VpsDrv_SiI9022AModeInfo *modeInfo);
static Int32 vpsSii9022aPrgmMdResetRegs(
                Vps_Sii9022aObj *siiObj,
                VpsDrv_SiI9022AModeInfo *siModeInfo);
static Int32 vpsSii9022aCalcCRC(UInt8 *regAddr, UInt8 *regValue, UInt32 *regCnt);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* Global object storing all information related to all HDMI driver handles */
Vps_Sii9022aCommonObj gVpsSii9022aCommonObj;

VpsDrv_SiI9022AModeInfo SiI9022ModeInfo[] =
{
    {FVID2_STD_480P, 3, 2700, 60, 858, 525,
        {16, 0, 62, 9, 6}, {122, 36, 720, 480}},
    {FVID2_STD_576P, 18, 2700, 50, 864, 625,
        {12, 0, 64, 5, 5}, {132, 44, 720, 576}},
    {FVID2_STD_720P_60, 4, 7425, 60, 1650, 750,
        {110, 0, 40, 5, 5}, {260, 25, 1280, 720}},
    {FVID2_STD_720P_50, 19, 7425, 50, 1980, 750,
        {440, 0, 40, 5, 5}, {260, 25, 1280, 720}},
    {FVID2_STD_1080P_30, 34, 7425, 30, 2200, 1125,
        {88, 0, 44, 4, 5}, {192, 41, 1920, 1080}},
    {FVID2_STD_1080P_50, 16, 7425, 50, 2640, 1125,
        {528, 0, 44, 4, 5}, {192, 41, 1920, 1080}},
    {FVID2_STD_1080P_60, 16, 14850, 60, 2200, 1125,
        {88, 0, 44, 4, 5}, {192, 41, 1920, 1080}},
    {FVID2_STD_1080I_60, 5, 7425, 60, 2200, 1125,
        {88, 564, 44, 2, 5}, {192, 20, 1920, 540}},
    {FVID2_STD_1080I_50, 20, 7425, 50, 2640, 1125,
        {528, 564, 44, 2, 5}, {192, 20, 1920, 540}},
};

#define VPS_SII9022A_MAX_MODES  (sizeof(SiI9022ModeInfo) /                     \
                                 sizeof(VpsDrv_SiI9022AModeInfo))


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


Int32 Vps_hdmi9022aInit(void)
{

    Int32 status = FVID2_SOK;
    Semaphore_Params semParams;

    /* Set to 0's for global object, descriptor memory  */
    memset(&gVpsSii9022aCommonObj, 0, sizeof (gVpsSii9022aCommonObj));

    /* Create global 9002A lock  */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    gVpsSii9022aCommonObj.glock = Semaphore_create(1u, &semParams, NULL);

    if(gVpsSii9022aCommonObj.glock == NULL)
        status = FVID2_EALLOC;

    if ( status == FVID2_SOK )
    {
        gVpsSii9022aCommonObj.fvidDrvOps.create
            = ( FVID2_DrvCreate )Vps_hdmi9022aCreate;
        gVpsSii9022aCommonObj.fvidDrvOps.delete = Vps_hdmi9022aDelete;
        gVpsSii9022aCommonObj.fvidDrvOps.control = Vps_hdmi9022aControl;
        gVpsSii9022aCommonObj.fvidDrvOps.queue = NULL;
        gVpsSii9022aCommonObj.fvidDrvOps.dequeue = NULL;
        gVpsSii9022aCommonObj.fvidDrvOps.processFrames = NULL;
        gVpsSii9022aCommonObj.fvidDrvOps.getProcessedFrames = NULL;
        gVpsSii9022aCommonObj.fvidDrvOps.drvId = FVID2_VPS_VID_ENC_SII9022A_DRV;
        gVpsSii9022aCommonObj.deviceState = VPS_SII9022A_NOT_INITED;

        status = FVID2_registerDriver(&gVpsSii9022aCommonObj.fvidDrvOps );
        if(status != FVID2_SOK)
        {
            /*Error - free acquired resources  */
            Semaphore_delete (&gVpsSii9022aCommonObj.glock);
        }
    }

    if(status != FVID2_SOK)
    {
        Vps_printf ( " ERROR %s:%s:%d !!!\n", __FILE__, __FUNCTION__,
                        __LINE__ );
    }

    return status;
}

Int32 Vps_hdmi9022aDeInit( void )
{
    /* Unregister FVID2 driver   */
    FVID2_unRegisterDriver ( &gVpsSii9022aCommonObj.fvidDrvOps );
    /* Delete semaphore's  */
    Semaphore_delete ( &gVpsSii9022aCommonObj.glock );
    return 0;
}

Fdrv_Handle Vps_hdmi9022aCreate (UInt32 drvId,
                                 UInt32 instanceId,
                                 Ptr createArgs,
                                 Ptr createStatusArgs,
                                 const FVID2_DrvCbParams * fdmCbParams)
{

    Vps_Sii9022aObj*  pSiiObj;
    Int32            retVal = FVID2_SOK;
    Vps_VideoEncoderCreateParams* vidEncCreateArgs
        = (Vps_VideoEncoderCreateParams*) createArgs;
    Vps_VideoEncoderCreateStatus* vidEecCreateStatus
        = (Vps_VideoEncoderCreateStatus*) createStatusArgs;

    if(NULL == vidEecCreateStatus)
    {
        retVal= FVID2_EBADARGS;
    }

    if(FVID2_SOK == retVal)
    {
        if((drvId != FVID2_VPS_VID_ENC_SII9022A_DRV) ||
           (instanceId != 0) ||
           (vidEncCreateArgs == NULL ))
        {
           retVal = FVID2_EBADARGS;
        }
    }

    if ((FVID2_SOK == retVal) &&
        (vidEncCreateArgs->deviceI2cInstId > VPS_DEVICE_I2C_INST_ID_MAX))
    {
        retVal = FVID2_EINVALID_PARAMS;
    }


    if (FVID2_SOK == retVal)
    {
        pSiiObj = vpsdrvSii9022AllocObj( );
        if (pSiiObj == NULL)
        {
            retVal = FVID2_EALLOC;
        }
        else
        {
            memcpy(&pSiiObj->createArgs,
                   vidEncCreateArgs,
                   sizeof(*vidEncCreateArgs));
        }
    }

    if((FVID2_SOK == retVal) && (NULL != pSiiObj))
    {
        gVpsSii9022aCommonObj.prms.outputFormat = VPS_SII9022A_HDMI_RGB;
        if (VPS_VIDEO_ENCODER_EMBEDDED_SYNC == pSiiObj->createArgs.syncMode)
        {
            pSiiObj->syncCfgReg = 0x84;
            pSiiObj->inBusCfg = 0x60;
        }
        else /* Separate Sync Mode */
        {
            pSiiObj->syncCfgReg = 0x04;
            pSiiObj->inBusCfg = 0x70;
        }

        /* FALSE to latch data on falling clock edge. Rising edge otherwise */
        /* Bit 4 of of TPI Input bus and pixel repetation */
        if (pSiiObj->createArgs.clkEdge == FALSE)
        {
            pSiiObj->inBusCfg &= (~(TPI_INPUTBUS_PIXEL_REP_BIT4_MASK));
        }
        else
        {
            pSiiObj->inBusCfg |= TPI_INPUTBUS_PIXEL_REP_BIT4_MASK;
        }

        /* Enable DE in syncpolarity register at 0x63 */
        pSiiObj->syncPolarityReg = 0x40;
        sii9022aDeviceInit(pSiiObj);
        vidEecCreateStatus->retVal = FVID2_SOK;
        return (pSiiObj);
    }
    else
    {
        if (NULL != vidEecCreateStatus)
        {
            vidEecCreateStatus->retVal = retVal;
        }
        return (NULL);
    }
}


Int32 Vps_hdmi9022aDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Vps_Sii9022aObj* pObj= (Vps_Sii9022aObj*)handle;

    if ( pObj == NULL )
        return FVID2_EBADARGS;

    /* free driver handle object  */
    vpsdrvSii9022DeAllocObj(pObj);
    return FVID2_SOK;
}

Int32 Vps_hdmi9022aControl (Fdrv_Handle handle,
                            UInt32 cmd,
                            Ptr cmdArgs,
                            Ptr cmdStatusArgs)
{
    Vps_Sii9022aObj *pSiiObj = (Vps_Sii9022aObj *)handle;
    Int32 status;

    if ( handle == NULL )
        return FVID2_EBADARGS;

    /*lock handle    */
    vpsdrvSii9022aLockObj(pSiiObj);

    switch (cmd)
    {
        case FVID2_START:
            status = vpsSii9022aStart(handle, cmdArgs, cmdStatusArgs);
            break;

        case FVID2_STOP:
            status = vpsSii9022aStop(handle, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_VPS_VIDEO_ENCODER_GET_CHIP_ID:
            status = FVID2_EUNSUPPORTED_CMD;
            break;

        case IOCTL_VPS_VIDEO_ENCODER_SET_MODE:
            status = vpsSii9022aSetMode(handle, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_VPS_SII9022A_GET_DETAILED_CHIP_ID:
            status = vpsSii9022aGetDetailedChipId(handle,
                                                  cmdArgs,
                                                  cmdStatusArgs);
            break;

        case IOCTL_VPS_SII9022A_QUERY_HPD:
            status = vpsSii9022aGetHpd(handle, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_VPS_SII9022A_SET_PARAMS:
            status = vpsSii9022aSetPrms(handle, cmdArgs, cmdStatusArgs);
            break;

        case IOCTL_VPS_SII9022A_GET_PARAMS:
            status = vpsSii9022aGetPrms(handle, cmdArgs, cmdStatusArgs);
            break;

        default:
            status = FVID2_EUNSUPPORTED_CMD;
            break;
    }

    /* unlock handle   */
    vpsdrvSii9022aUnLockObj (pSiiObj);
    return status;
}

/*
  Get TP5158 chip ID, revision ID and firmware patch ID
*/
static Int32 vpsSii9022aGetDetailedChipId(Fdrv_Handle handle, Ptr cmdArgs,
                                  Ptr cmdStatusArgs)
{
    Int32 status = FVID2_SOK;
    Vps_HdmiChipId *hdmichipId = (Vps_HdmiChipId*)cmdArgs;
    Vps_Sii9022aObj *pSiiObj = (Vps_Sii9022aObj*)handle;

    if(VPS_SII9022A_INITED == gVpsSii9022aCommonObj.deviceState)
    {
        hdmichipId->deviceId = pSiiObj->hdmiChipid.deviceId;
        hdmichipId->deviceProdRevId = pSiiObj->hdmiChipid.deviceProdRevId;
        hdmichipId->tpiRevId = pSiiObj->hdmiChipid.tpiRevId;
        hdmichipId->hdcpRevTpi = pSiiObj->hdmiChipid.hdcpRevTpi;
        status = FVID2_SOK;
    }
    else
    {
        status = FVID2_EFAIL;
    }

    return (status);
}



/* Enable TMDS output */
static Int32 vpsSii9022aStart(Fdrv_Handle handle,
                              Ptr cmdArgs,
                              Ptr cmdStatusArgs)
{
    Int32                    retVal = FVID2_SOK;
    UInt8                    regValue = 0u, regAddr;
    Vps_Sii9022aObj         *pSiiObj = (Vps_Sii9022aObj*)handle;

    regAddr = 0x1A;
    retVal = Vps_deviceRead8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                &regAddr,
                &regValue,
                1u);
    if (FVID2_SOK == retVal)
    {
        /* Enable HDMI output */
        regValue |= 0x1;
        /* Enable Output TMDS */
        regValue &= 0xEF;

        retVal = Vps_deviceWrite8(
                    pSiiObj->createArgs.deviceI2cInstId,
                    pSiiObj->createArgs.deviceI2cAddr,
                    &regAddr,
                    &regValue,
                    1u);

        /* Configure Input Bus after starting */
        retVal |= sii9022aCfgInBus(pSiiObj);
    }

    return (retVal);
}



/* Disable TMDS output */
static Int32 vpsSii9022aStop(Fdrv_Handle handle,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs)
{
    Int32                    retVal = FVID2_SOK;
    UInt8                    regValue = 0u, regAddr;
    Vps_Sii9022aObj         *pSiiObj = (Vps_Sii9022aObj*)handle;

    regAddr = 0x1A;
    retVal = Vps_deviceRead8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                &regAddr,
                &regValue,
                1u);
    if (FVID2_SOK == retVal)
    {
        /* Enable HDMI output */
        regValue |= 0x1;
        /* Power Down Output TMDS Clock */
        regValue |= 0x10;

        retVal = Vps_deviceWrite8(
                    pSiiObj->createArgs.deviceI2cInstId,
                    pSiiObj->createArgs.deviceI2cAddr,
                    &regAddr,
                    &regValue,
                    1u);
    }

    return (retVal);
}



/*
  Set Mode
*/
static Int32 vpsSii9022aSetMode(Fdrv_Handle handle,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs)
{
    Int32                    retVal = FVID2_SOK;
    UInt32                   modeCnt;
    Vps_Sii9022aObj         *pSiiObj = (Vps_Sii9022aObj*)handle;
    Vps_SiI9022aModeParams  *modeInfo = (Vps_SiI9022aModeParams *)cmdArgs;
    VpsDrv_SiI9022AModeInfo *siModeInfo = NULL;

    for (modeCnt = 0u; modeCnt < VPS_SII9022A_MAX_MODES; modeCnt ++)
    {
        siModeInfo = &SiI9022ModeInfo[modeCnt];
        if (modeInfo->standard == siModeInfo->standard)
        {
            break;
        }
    }

    if (NULL == siModeInfo)
    {
        Vps_printf("Unsupported mode\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        if (VPS_VIDEO_ENCODER_EMBEDDED_SYNC == pSiiObj->createArgs.syncMode)
        {
            retVal |= vpsSii9022aPrgmEmbSyncTimingInfo(pSiiObj, siModeInfo);
        }
        else if (VPS_VIDEO_ENCODER_EXTERNAL_SYNC == pSiiObj->createArgs.syncMode)
        {
            retVal |= vpsSii9022aPrgmExtSyncTimingInfo(pSiiObj, siModeInfo);
        }
        else
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
        retVal |= vpsSii9022aPrgmAvInfoFrame(pSiiObj, siModeInfo);
        retVal |= vpsSii9022aPrgmMdResetRegs(pSiiObj, siModeInfo);
    }

    return (retVal);
}



static Int32 vpsSii9022aGetHpd(Fdrv_Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
    Int32                    retVal = FVID2_SOK;
    UInt8                    regValue = 0u, regAddr;
    Vps_Sii9022aObj         *pSiiObj = (Vps_Sii9022aObj *)handle;
    Vps_SiI9022aHpdPrms     *hpd = (Vps_SiI9022aHpdPrms *)cmdArgs;

    regAddr = 0x3D;
    retVal = Vps_deviceRead8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                &regAddr,
                &regValue,
                1u);

    if (FVID2_SOK == retVal)
    {
        hpd->hpdEvtPending = regValue & 0x01;
        hpd->busError = regValue & 0x02;
        hpd->hpdStatus = regValue & 0x04;
    }

    return (retVal);
}



/*
  Device Init
*/
static Int32 sii9022aDeviceInit(Vps_Sii9022aObj* psiiObj)
{
    Int32 status = FVID2_SOK;

    status = sii9022aReset(psiiObj);
    if(FVID2_SOK == status)
    {
        status = sii9022aGetHdmiChipId(psiiObj,&(psiiObj->hdmiChipid));
    }

    if (FVID2_SOK == status)
    {
        status = sii9022aPowerUpTxm(psiiObj);
    }

    if (FVID2_SOK == status)
    {
        status = sii9022aEnable(psiiObj);
    }

    if (FVID2_SOK == status)
    {
        status = sii9022aCfgInBus(psiiObj);
    }

    if (FVID2_SOK == status)
    {
        status = sii9022aCfgYcMode(psiiObj);
    }

    if (FVID2_SOK == status)
    {
        status = sii9022aCfgSyncMode(psiiObj);
    }

    if (FVID2_SOK == status)
    {
        gVpsSii9022aCommonObj.deviceState = VPS_SII9022A_INITED;
    }

    return status;
}

/*
  Device Res-set
*/
static Int32 sii9022aReset(Vps_Sii9022aObj* pSiiObj)
{
    Int32 status = FVID2_SOK;
    UInt8 regAddr[4];
    UInt8 regValue[4];
    UInt8 numRegs;
    UInt32 rptCnt = 0;

    numRegs = 0;
    regAddr[numRegs] = 0xc7;
    regValue[numRegs] = 0x00;
    numRegs++;

    /* TODO Reset is failing for the first time with chains on TI814x. To look
     * into
     */
    for (rptCnt = 0; rptCnt < 5; rptCnt++)
    {
        status = Vps_deviceWrite8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                regAddr,
                regValue,
                numRegs);
        if (FVID2_SOK == status)
        {
            break;
        }
   }
    if (FVID2_SOK != status)
    {
        status = FVID2_ETIMEOUT;
    }
    return status;
}

/*
  Device Enable
*/
static Int32 sii9022aEnable(Vps_Sii9022aObj* pSiiObj)
{
    Int32 status = FVID2_SOK;
    UInt8 regAddr[4];
    UInt8 regValue[4];
    UInt8 numRegs;

    numRegs = 0;
    regAddr[numRegs] = 0xBC;
    regValue[numRegs] = 0x01;
    numRegs++;

    regAddr[numRegs] = 0xBD;
    regValue[numRegs] = 0x82;
    numRegs++;

    status = Vps_deviceWrite8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                regAddr,
                regValue,
                numRegs);

    if (FVID2_SOK == status)
    {
        numRegs = 0;
        regAddr[numRegs] = 0xBE;
        regValue[numRegs] = 0x01;
        numRegs++;

        status = Vps_deviceRead8(
                    pSiiObj->createArgs.deviceI2cInstId,
                    pSiiObj->createArgs.deviceI2cAddr,
                    regAddr,
                    regValue,
                    numRegs);
        if (FVID2_SOK == status)
        {
            regValue[0u] |= 0x01;
            status = Vps_deviceWrite8(
                        pSiiObj->createArgs.deviceI2cInstId,
                        pSiiObj->createArgs.deviceI2cAddr,
                        regAddr,
                        regValue,
                        numRegs);
        }
    }

    return status;
}


/*
  Device Power up Transmitter
*/
static Int32 sii9022aPowerUpTxm(Vps_Sii9022aObj* pSiiObj)
{
    Int32 status = FVID2_SOK;
    UInt8 regAddr;
    UInt8 regValue = 0u;

    regAddr = 0x1E;
    status = Vps_deviceRead8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                &regAddr,
                &regValue,
                1);

    if (FVID2_SOK == status)
    {
        regValue &= 0xFC;
        status = Vps_deviceWrite8(
                    pSiiObj->createArgs.deviceI2cInstId,
                    pSiiObj->createArgs.deviceI2cAddr,
                    &regAddr,
                    &regValue,
                    1);
    }

    return status;
}


/*
  Configure Input Bus and pixel repetation
*/
static Int32 sii9022aCfgInBus(Vps_Sii9022aObj* pSiiObj)
{
    Int32 status = FVID2_SOK;
    UInt8 regAddr;
    UInt8 regValue;

    regAddr = 0x08;
    regValue = pSiiObj->inBusCfg;

    status = Vps_deviceWrite8(
                    pSiiObj->createArgs.deviceI2cInstId,
                    pSiiObj->createArgs.deviceI2cAddr,
                    &regAddr,
                    &regValue,
                    1);

    return status;
}


/*
  Configure YC Mode
*/
static Int32 sii9022aCfgYcMode(Vps_Sii9022aObj* pSiiObj)
{
    Int32 status = FVID2_SOK;
    UInt8 regAddr;
    UInt8 regValue;

    regAddr = 0x0B;
    regValue = 0x00;

    status = Vps_deviceWrite8(
                    pSiiObj->createArgs.deviceI2cInstId,
                    pSiiObj->createArgs.deviceI2cAddr,
                    &regAddr,
                    &regValue,
                    1);

    return status;
}


/*
  Configure Sync Mode
*/
static Int32 sii9022aCfgSyncMode(Vps_Sii9022aObj* pSiiObj)
{
    Int32 status = FVID2_SOK;
    UInt8 regAddr;
    UInt8 regValue;

    regAddr = 0x60;
    regValue = pSiiObj->syncCfgReg;
    status = Vps_deviceWrite8(
                    pSiiObj->createArgs.deviceI2cInstId,
                    pSiiObj->createArgs.deviceI2cAddr,
                    &regAddr,
                    &regValue,
                    1);
    return status;
}


/*
  Get TP5158 chip ID, revision ID and firmware patch ID
*/
static Int32 sii9022aGetHdmiChipId(Vps_Sii9022aObj* pSiiObj,
                                   Vps_HdmiChipId *hdmichipId)
{
    Int32 status = FVID2_SOK;
    UInt8 regAddr[8];
    UInt8 regValue[8];
    UInt8 numRegs;

    numRegs = 0;
    regAddr[numRegs] = 0x1b;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = 0x1c;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = 0x1d;
    regValue[numRegs] = 0;
    numRegs++;

    regAddr[numRegs] = 0x30;
    regValue[numRegs] = 0;
    numRegs++;

    status = Vps_deviceRead8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                regAddr,
                regValue,
                numRegs);

    if (FVID2_SOK != status)
    {
        status = FVID2_ETIMEOUT;
    }

    if(FVID2_SOK == status)
    {
        hdmichipId->deviceId = regValue[0];
        hdmichipId->deviceProdRevId = regValue[1];
        hdmichipId->tpiRevId = regValue[2];
        hdmichipId->hdcpRevTpi = regValue[3];
    }

    return (status);
}

/*
  Handle level lock
*/
static Int32 vpsdrvSii9022aLockObj(Vps_Sii9022aObj * pSiiObj)
{
    Semaphore_pend(pSiiObj->lock, BIOS_WAIT_FOREVER );
    return FVID2_SOK;
}

/*
  Handle level unlock
*/
static Int32 vpsdrvSii9022aUnLockObj(Vps_Sii9022aObj * pSiiObj)
{
    Semaphore_post(pSiiObj->lock);
    return FVID2_SOK;
}

/*
  Global driver level lock
*/
Int32 vpsdrvSii9022aLock(void)
{
    Semaphore_pend (gVpsSii9022aCommonObj.glock, BIOS_WAIT_FOREVER );
    return FVID2_SOK;
}

/*
  Global driver level unlock
*/
static Int32 vpsdrvSii9022aUnlock(void)
{
    Semaphore_post(gVpsSii9022aCommonObj.glock);
    return FVID2_SOK;
}

static Vps_Sii9022aObj* vpsdrvSii9022AllocObj(void)
{
    UInt32 handleId;
    Vps_Sii9022aObj *pSiiObj;
    Semaphore_Params semParams;
    UInt32 found = FALSE;

    /* Take global lock to avoid race condition */
    vpsdrvSii9022aLock();

    /* find a unallocated object in pool */
    for(handleId = 0; handleId < VPS_DEVICE_MAX_HANDLES;handleId++)
    {

        pSiiObj = &gVpsSii9022aCommonObj.sii9022aObj[handleId];

        if (pSiiObj->state == VPS_SII9022A_OBJ_STATE_UNUSED)
        {
            /* free object found   */

            memset(pSiiObj, 0, sizeof(*pSiiObj));

            /* init state and handle ID */
            pSiiObj->state = VPS_SII9022A_OBJ_STATE_USED;
            pSiiObj->handleId = handleId;

            /* create driver object specific semaphore lock */
            Semaphore_Params_init(&semParams);
            semParams.mode = Semaphore_Mode_BINARY;
            pSiiObj->lock = Semaphore_create(1u, &semParams, NULL);

            found = TRUE;
            if ( pSiiObj->lock == NULL )
            {
                /* Error - release object   */
                found = FALSE;
                pSiiObj->state = VPS_SII9022A_OBJ_STATE_UNUSED;
            }
            break;
        }
    }

    vpsdrvSii9022aUnlock();
    if(found)
    {
        return (pSiiObj);
    }
    else
    {
        return (NULL);
    }
}

/*
  De-Allocate driver object

  Marks handle as 'NOT IN USE'
  Also delete's handle level semaphore lock
*/
static Int32 vpsdrvSii9022DeAllocObj(Vps_Sii9022aObj* pObj )
{
    /* take global lock  */
    vpsdrvSii9022aLock( );

    if ( pObj->state != VPS_SII9022A_OBJ_STATE_UNUSED )
    {
        /* mark state as unused  */
        pObj->state = VPS_SII9022A_OBJ_STATE_UNUSED;

        /* delete object locking semaphore  */
        Semaphore_delete ( &pObj->lock );
    }

    /* release global lock   */
    vpsdrvSii9022aUnlock( );

    return FVID2_SOK;
}

/*
 * Program Timing information for the separate sync input
 */
static Int32 vpsSii9022aPrgmExtSyncTimingInfo(
            Vps_Sii9022aObj *pSiiObj,
            VpsDrv_SiI9022AModeInfo *siModeInfo)
{
    Int32 retVal;
    UInt8 regValue[20], regAddr[20];
    UInt32 numRegs;

    numRegs = 0;
    regAddr[numRegs] = 0x62;
    regValue[numRegs] = siModeInfo->extSyncPrms.deDelay & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x63;
    pSiiObj->syncPolarityReg &= ~(0x03);
    pSiiObj->syncPolarityReg |=
        ((siModeInfo->extSyncPrms.deDelay & 0x300) >> 8);
    regValue[numRegs] = pSiiObj->syncPolarityReg & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x64;
    regValue[numRegs] = siModeInfo->extSyncPrms.deTop & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x66;
    regValue[numRegs] = siModeInfo->extSyncPrms.deCnt & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x67;
    regValue[numRegs] = (siModeInfo->extSyncPrms.deCnt & 0xF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x68;
    regValue[numRegs] = siModeInfo->extSyncPrms.deLine & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x69;
    regValue[numRegs] = (siModeInfo->extSyncPrms.deLine & 0x700) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x00;
    regValue[numRegs] = siModeInfo->pixClk & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x01;
    regValue[numRegs] = (siModeInfo->pixClk & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x02;
    regValue[numRegs] = siModeInfo->vFreq & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x03;
    regValue[numRegs] = (siModeInfo->vFreq & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x04;
    regValue[numRegs] = siModeInfo->pixels & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x05;
    regValue[numRegs] = (siModeInfo->pixels & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x06;
    regValue[numRegs] = siModeInfo->lines & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x07;
    regValue[numRegs] = (siModeInfo->lines & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x08;
    regValue[numRegs] = pSiiObj->inBusCfg;
    numRegs ++;

    regAddr[numRegs] = 0x09;
    regValue[numRegs] = 0x00;
    numRegs ++;

    regAddr[numRegs] = 0x0A;
    switch (gVpsSii9022aCommonObj.prms.outputFormat)
    {
        case VPS_SII9022A_HDMI_RGB:
            regValue[numRegs] = 0x10;
            pSiiObj->isRgbOutput = 1;
            break;

        case VPS_SII9022A_HDMI_YUV444:
            regValue[numRegs] = 0x11;
            pSiiObj->isRgbOutput = 0;
            break;

        case VPS_SII9022A_HDMI_YUV422:
            regValue[numRegs] = 0x12;
            pSiiObj->isRgbOutput = 0;
            break;

        case VPS_SII9022A_DVI_RGB:
            regValue[numRegs] = 0x13;
            pSiiObj->isRgbOutput = 1;
            break;
        default:
            break;
    }
    numRegs ++;

    retVal = Vps_deviceWrite8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                regAddr,
                regValue,
                numRegs);

    return (retVal);
}


static Int32 vpsSii9022aPrgmEmbSyncTimingInfo(
            Vps_Sii9022aObj *pSiiObj,
            VpsDrv_SiI9022AModeInfo *siModeInfo)
{
    Int32 retVal;
    UInt8 regValue[20], regAddr[20];
    UInt32 numRegs;

    numRegs = 0;
    regAddr[numRegs] = 0x62;
    regValue[numRegs] = siModeInfo->embSyncPrms.hBitToHSync & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x63;
    pSiiObj->syncPolarityReg &= ~(0x03);
    pSiiObj->syncPolarityReg |=
        ((siModeInfo->embSyncPrms.hBitToHSync & 0x300) >> 8);
    regValue[numRegs] = pSiiObj->syncPolarityReg & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x64;
    regValue[numRegs] = siModeInfo->embSyncPrms.field2Offset & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x65;
    regValue[numRegs] = (siModeInfo->embSyncPrms.field2Offset & 0xF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x66;
    regValue[numRegs] = siModeInfo->embSyncPrms.hWidth & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x67;
    regValue[numRegs] = (siModeInfo->embSyncPrms.hWidth & 0x300) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x68;
    regValue[numRegs] = siModeInfo->embSyncPrms.vBitToVSync & 0x3F;
    numRegs ++;

    regAddr[numRegs] = 0x69;
    regValue[numRegs] = siModeInfo->embSyncPrms.vWidth & 0x3F;
    numRegs ++;

    regAddr[numRegs] = 0x00;
    regValue[numRegs] = siModeInfo->pixClk & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x01;
    regValue[numRegs] = (siModeInfo->pixClk & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x02;
    regValue[numRegs] = siModeInfo->vFreq & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x03;
    regValue[numRegs] = (siModeInfo->vFreq & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x04;
    regValue[numRegs] = siModeInfo->pixels & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x05;
    regValue[numRegs] = (siModeInfo->pixels & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x06;
    regValue[numRegs] = siModeInfo->lines & 0xFF;
    numRegs ++;

    regAddr[numRegs] = 0x07;
    regValue[numRegs] = (siModeInfo->lines & 0xFF00) >> 8;
    numRegs ++;

    regAddr[numRegs] = 0x08;
    regValue[numRegs] = pSiiObj->inBusCfg;
    numRegs ++;

    regAddr[numRegs] = 0x09;
    regValue[numRegs] = 0x02;
    numRegs ++;

    regAddr[numRegs] = 0x0A;
    switch (gVpsSii9022aCommonObj.prms.outputFormat)
    {
        case VPS_SII9022A_HDMI_RGB:
            regValue[numRegs] = 0x10;
            pSiiObj->isRgbOutput = 1;
            break;

        case VPS_SII9022A_HDMI_YUV444:
            regValue[numRegs] = 0x11;
            pSiiObj->isRgbOutput = 0;
            break;

        case VPS_SII9022A_HDMI_YUV422:
            regValue[numRegs] = 0x12;
            pSiiObj->isRgbOutput = 0;
            break;

        case VPS_SII9022A_DVI_RGB:
            regValue[numRegs] = 0x13;
            pSiiObj->isRgbOutput = 1;
            break;
        default:
            break;
    }
    numRegs ++;

    retVal = Vps_deviceWrite8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                regAddr,
                regValue,
                numRegs);

    return (retVal);
}

/* Program AVInfo Frame */
static Int32 vpsSii9022aPrgmAvInfoFrame(Vps_Sii9022aObj *pSiiObj,
                                        VpsDrv_SiI9022AModeInfo *modeInfo)
{
    Int32 retVal = FVID2_SOK;
    UInt8 regValue[15], regAddr[15];
    UInt32 regCnt;

    regCnt = 0;

    regAddr[regCnt] = 0x0D;
    if (pSiiObj->isRgbOutput)
    {
        regValue[regCnt] = 0x01;
    }
    else
    {
        regValue[regCnt] = 0x21;
    }
    regCnt ++;

    regAddr[regCnt] = 0x0E;
    regValue[regCnt] = 0xA0;
    regCnt ++;

    regAddr[regCnt] = 0x0F;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x10;
    regValue[regCnt] = modeInfo->modeCode & 0x7F;
    regCnt ++;

    regAddr[regCnt] = 0x11;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x12;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x13;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x14;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x15;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x16;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x17;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x18;
    regValue[regCnt] = 0x00;
    regCnt ++;

    regAddr[regCnt] = 0x19;
    regValue[regCnt] = 0x00;
    regCnt ++;

    if (FVID2_SOK == retVal)
    {
        retVal = vpsSii9022aCalcCRC(regAddr, regValue, &regCnt);

        if (FVID2_SOK == retVal)
        {
            if (VPS_SII9022A_DVI_RGB == gVpsSii9022aCommonObj.prms.outputFormat)
            {
                memset(regValue, 0x0, sizeof(regValue));
            }

            retVal = Vps_deviceWrite8(
                        pSiiObj->createArgs.deviceI2cInstId,
                        pSiiObj->createArgs.deviceI2cAddr,
                        regAddr,
                        regValue,
                        regCnt);
        }
    }

    regAddr[0] = 0x19;
    regValue[0] = 0x00;
    retVal |= Vps_deviceWrite8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                regAddr,
                regValue,
                1);

    return (retVal);
}

static Int32 vpsSii9022aPrgmMdResetRegs(Vps_Sii9022aObj *pSiiObj,
                                        VpsDrv_SiI9022AModeInfo *siModeInfo)
{
    Int32 retVal = FVID2_SOK;
    UInt8 regAddr, regValue;

    regAddr = 0x63;
    regValue = pSiiObj->syncPolarityReg;
    retVal = Vps_deviceWrite8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                &regAddr,
                &regValue,
                1);

    regAddr = 0x60;
    regValue = pSiiObj->syncCfgReg;
    retVal |= Vps_deviceWrite8(
                pSiiObj->createArgs.deviceI2cInstId,
                pSiiObj->createArgs.deviceI2cAddr,
                &regAddr,
                &regValue,
                1);

    if (FVID2_SOK == retVal)
    {
        Task_sleep(5);
        regAddr = 0x61;
        retVal = Vps_deviceRead8(
                        pSiiObj->createArgs.deviceI2cInstId,
                        pSiiObj->createArgs.deviceI2cAddr,
                        &regAddr,
                        &regValue,
                        1);
        if (FVID2_SOK == retVal)
        {
            /* Set the same sync polarity in 0x63 register */
            pSiiObj->syncPolarityReg &= ~(0x30);
            pSiiObj->syncPolarityReg |= ((regValue & 0x03) << 4);

            regAddr = 0x63;
            regValue = pSiiObj->syncPolarityReg;
            retVal = Vps_deviceWrite8(
                        pSiiObj->createArgs.deviceI2cInstId,
                        pSiiObj->createArgs.deviceI2cAddr,
                        &regAddr,
                        &regValue,
                        1);
        }
    }

    return (retVal);
}

static Int32 vpsSii9022aCalcCRC(UInt8 *regAddr, UInt8 *regValue, UInt32 *regCnt)
{
    Int32 retVal = FVID2_SOK;
    UInt32 cnt, sum = 0;

    for (cnt = 0u; cnt < *regCnt; cnt ++)
    {
        sum += regValue[cnt];
    }

    if (FVID2_SOK == retVal)
    {
        sum += 0x82 + 0x02 + 13;
        sum &= 0xFF;
        regValue[*regCnt] = 0x100 - sum;
        regAddr[*regCnt] = 0x0C;
        (*regCnt) ++;
    }

    return (retVal);
}

static Int32 vpsSii9022aSetPrms(Fdrv_Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
    Int32 retVal = FVID2_SOK;
    Vps_SiI9022aPrms *prms = (Vps_SiI9022aPrms *)cmdArgs;

    if (NULL == prms)
    {
        System_printf("Sii9022A: Null Pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        if (VPS_SII9022A_MAX_FORMAT <= prms->outputFormat)
        {
            System_printf("Sii9022A: Wrong output Type\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        gVpsSii9022aCommonObj.prms.outputFormat = prms->outputFormat;
    }

    return (retVal);
}

static Int32 vpsSii9022aGetPrms(Fdrv_Handle handle,
                               Ptr cmdArgs,
                               Ptr cmdStatusArgs)
{
    Int32 retVal = FVID2_SOK;
    Vps_SiI9022aPrms *prms = (Vps_SiI9022aPrms *)cmdArgs;

    if (NULL == prms)
    {
        System_printf("Sii9022A: Null Pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        prms->outputFormat = gVpsSii9022aCommonObj.prms.outputFormat;
    }

    return (retVal);
}
