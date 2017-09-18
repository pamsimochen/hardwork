/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vps_platform.c
 *
 *  \brief Interface file to the platform specific functions abstraction APIs.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */


#include <string.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Semaphore.h>
#ifdef PROFILE
#include <ti/sysbios/knl/Clock.h>
#endif

#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/platforms/vps_platform.h>
#include <ti/psp/devices/vps_videoDecoder.h>
#ifdef TI_816X_BUILD
#include <ti/psp/platforms/ti816x/vps_platformTI816x.h>
#endif
#ifdef TI_814X_BUILD
#include <ti/psp/platforms/ti814x/vps_platformTI814x.h>
#endif
#ifdef TI_8107_BUILD
#include <ti/psp/platforms/ti8107/vps_platformTI8107.h>
#endif
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

//#define PLATFORM_ZEBU

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#ifndef PLATFORM_ZEBU
#define VPS_PLATFORM_RESET_VIDEO_DECODER
#endif

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

typedef struct
{
    FVID2_DrvOps                fvidDrvOps;
    /**< FVID2 driver ops */
    Semaphore_Handle            glock;
    /**< global level lock */
    Vps_SystemVPllClk           vpllCfgVencD, vpllCfgVencA, vpllCfgRf;
    /**< current pixel clock settings */
    UInt32                      openCnt;
    /**< open count */
    Vps_PlatformId              platformId;
    /**< Platform ID. For valid values see #Vps_PlatformId. */
    UInt32                      isPlatformEvm;
    /**< Flag to determine whether platform is EVM or Simulator. */
    Vps_PlatformBoardId         platBoardId;
    /**< Platform Board ID. For valid values see #Vps_PlatformBoardId. */
    Vps_PlatformCpuRev          cpuRev;
    /**< CPU revision. */
    Vps_PlatformBoardRev        baseBoardRev;
    /**< Base Board revision. */
    Vps_PlatformBoardRev        dcBoardRev;
    /**< Daughter card board revision. */
} Vps_platformCommonObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Fdrv_Handle Vps_platformCreate(UInt32 drvId,
                                      UInt32 instanceId,
                                      Ptr createArgs,
                                      Ptr createStatusArgs,
                                      const FVID2_DrvCbParams * fdmCbParams);
static Int32 Vps_platformDelete(Fdrv_Handle handle, Ptr deleteArgs);
static Int32 Vps_platformControl(Fdrv_Handle handle,
                                 UInt32 cmd,
                                 Ptr cmdArgs,
                                 Ptr cmdStatusArgs);
static Int32 Vps_platformSetVencPixClk(Vps_SystemVPllClk *vpllCfg);
static Int32 vpsPlatformDrvLock(void);
static Int32 vpsPlatformDrvUnLock(void);
static inline UInt32 vpsPlatformVpllChangeReq(Vps_SystemVPllClk *vpllCfg);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

Vps_platformCommonObj gPlatformCommonObj;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

Int32 Vps_platformInit(Vps_PlatformInitParams *initParams)
{
    Int32 retVal = FVID2_SOK;
#ifdef PLATFORM_EVM_SI
    Vps_PlatformInitParams localInitParams;
#endif /* ifdef PLATFORM_EVM_SI */

    memset(&gPlatformCommonObj, 0, sizeof(gPlatformCommonObj));

    gPlatformCommonObj.platformId = VPS_PLATFORM_ID_UNKNOWN;
    gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_MAX;

#ifdef PLATFORM_EVM_SI
    /* Platform is EVM */
    gPlatformCommonObj.isPlatformEvm = TRUE;
#ifdef TI_816X_BUILD
    gPlatformCommonObj.platformId = VPS_PLATFORM_ID_EVM_TI816x;
    /* If NULL is passed pinMux will be initialized by default */
    if (NULL == initParams)
    {
       initParams =  &localInitParams;
       initParams->isPinMuxSettingReq = TRUE;
    }
    retVal = Vps_platformTI816xInit(initParams);

    /* Get the CPU and board revisions */
    gPlatformCommonObj.cpuRev = Vps_platformTI816xGetCpuRev();
#endif

#ifdef TI_814X_BUILD
    gPlatformCommonObj.platformId = VPS_PLATFORM_ID_EVM_TI814x;
    /* If NULL is passed pinMux will be initialized by default */
    if (NULL == initParams)
    {
       initParams =  &localInitParams;
       initParams->isPinMuxSettingReq = TRUE;
    }
    retVal = Vps_platformTI814xInit(initParams);

    /* Get the CPU and board revisions */
    gPlatformCommonObj.cpuRev = Vps_platformTI814xGetCpuRev();
#endif

#ifdef TI_8107_BUILD
    gPlatformCommonObj.platformId = VPS_PLATFORM_ID_EVM_TI8107;
    /* If NULL is passed pinMux will be initialized by default */
    if (NULL == initParams)
    {
       initParams =  &localInitParams;
       initParams->isPinMuxSettingReq = TRUE;
    }
    retVal = Vps_platformTI8107Init(initParams);

    /* Get the CPU and board revisions */
    gPlatformCommonObj.cpuRev = Vps_platformTI8107GetCpuRev();
#endif
    gPlatformCommonObj.baseBoardRev = VPS_PLATFORM_BOARD_REV_MAX;
    gPlatformCommonObj.dcBoardRev = VPS_PLATFORM_BOARD_REV_MAX;

#else
    /* Platform is Simulator */
    gPlatformCommonObj.isPlatformEvm = FALSE;
#ifdef TI_816X_BUILD
    gPlatformCommonObj.platformId = VPS_PLATFORM_ID_SIM_TI816x;
    /* Simulator - use latest revision */
    gPlatformCommonObj.cpuRev = VPS_PLATFORM_CPU_REV_2_0;
#endif

#ifdef TI_814X_BUILD
    gPlatformCommonObj.platformId = VPS_PLATFORM_ID_SIM_TI814x;
    /* Simulator - use latest revision */
    gPlatformCommonObj.cpuRev = VPS_PLATFORM_CPU_REV_2_1;
#endif

    gPlatformCommonObj.baseBoardRev = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    gPlatformCommonObj.dcBoardRev = VPS_PLATFORM_BOARD_REV_UNKNOWN;
#endif  /* PLATFORM_EVM_SI */

    return (retVal);
}

Int32 Vps_platformDeInit(void)
{
    Int32 retVal = FVID2_SOK;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    retVal = Vps_platformTI816xDeInit();
#endif

#ifdef TI_814X_BUILD
    retVal = Vps_platformTI814xDeInit();
#endif

#ifdef TI_8107_BUILD
    retVal = Vps_platformTI8107DeInit();
#endif
#endif

    return (retVal);
}

/* Init EVM related sub-systems like I2C instance */
Int32 Vps_platformDeviceInit(Vps_PlatformDeviceInitParams *initPrms)
{
    Int32               retVal = FVID2_SOK;
    Semaphore_Params    semParams;
    Vps_SystemVPllClk  pllClk;
    Vps_PlatformDeviceInitParams localInitPrms;


    /* Create global 9002A lock  */
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    gPlatformCommonObj.glock = Semaphore_create(1u, &semParams, NULL);

    if(NULL == gPlatformCommonObj.glock)
        retVal = FVID2_EALLOC;

#ifdef PROFILE
    Vps_printf(" %d: System Drv Create >>>\r\n", Clock_getTicks());
#endif
    if (FVID2_SOK == retVal)
    {
        gPlatformCommonObj.fvidDrvOps.create = Vps_platformCreate;
        gPlatformCommonObj.fvidDrvOps.delete = Vps_platformDelete;
        gPlatformCommonObj.fvidDrvOps.control = Vps_platformControl;
        gPlatformCommonObj.fvidDrvOps.drvId = FVID2_VPS_VID_SYSTEM_DRV;
        gPlatformCommonObj.openCnt = 0u;

        retVal = FVID2_registerDriver(&gPlatformCommonObj.fvidDrvOps);
        if(FVID2_SOK != retVal)
        {
            /*Error - free acquired resources  */
            Semaphore_delete(&gPlatformCommonObj.glock);
        }
        else
        {
#ifdef TI_816X_BUILD
            /*set analog PLL*/
            pllClk.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_A;
            pllClk.outputClk = 148500u;

            retVal |= Vps_platformControl(
                        &gPlatformCommonObj,
                        IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                        &pllClk,
                        NULL);
#endif /*TI_816X_BUILD */
           /*set digital PLL*/
            pllClk.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_D;
            pllClk.outputClk = 297000u;

            retVal |= Vps_platformControl(
                        &gPlatformCommonObj,
                        IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                        &pllClk,
                        NULL);

            /*set RF PLL*/
            pllClk.outputVenc = VPS_SYSTEM_VPLL_OUTPUT_VENC_RF;
#ifdef TI_816X_BUILD
            pllClk.outputClk = 216000u;
#endif /* TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
            pllClk.outputClk = 54000u;
#endif /* TI_814X_BUILD || TI_8107_BUILD */
            retVal |= Vps_platformControl(
                        &gPlatformCommonObj,
                        IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL,
                        &pllClk,
                        NULL);
        }
    }
#ifdef PROFILE
    Vps_printf(" %d: System Drv Create <<<\r\n", Clock_getTicks());
#endif
    if (NULL == initPrms)
    {
        initPrms = &localInitPrms;
        localInitPrms.isI2cInitReq = TRUE;
        localInitPrms.isI2cProbingReq = TRUE;
    }
#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    retVal |= Vps_platformTI816xDeviceInit(initPrms);
#endif

#ifdef TI_814X_BUILD
    retVal |= Vps_platformTI814xDeviceInit(initPrms);
#endif

#ifdef TI_8107_BUILD
    retVal |= Vps_platformTI8107DeviceInit(initPrms);
#endif
#endif

    return (retVal);
}

/* De-Init EVM related sub-systems */
Int32 Vps_platformDeviceDeInit(void)
{
    Int32 retVal = FVID2_SOK;

    /* Unregister FVID2 driver   */
    FVID2_unRegisterDriver(&gPlatformCommonObj.fvidDrvOps);
    /* Delete semaphore's  */
    Semaphore_delete( &gPlatformCommonObj.glock);

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    retVal = Vps_platformTI816xDeviceDeInit();
#endif

#ifdef TI_814X_BUILD
    retVal = Vps_platformTI814xDeviceDeInit();
#endif

#ifdef TI_8107_BUILD
    retVal |= Vps_platformTI8107DeviceDeInit();
#endif
#endif

    return (retVal);
}


UInt32 Vps_platformGetI2cInstId(void)
{
    UInt32 i2cInst = 0u;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    i2cInst = Vps_platformTI816xGetI2cInstId();
#endif

#ifdef TI_814X_BUILD
    i2cInst = Vps_platformTI814xGetI2cInstId();
#endif

#ifdef TI_8107_BUILD
    i2cInst = Vps_platformTI8107GetI2cInstId();
#endif
#endif

    return (i2cInst);
}

UInt8 Vps_platformGetVidDecI2cAddr(UInt32 vidDecId, UInt32 vipInstId)
{
    UInt8 devAddr = 0u;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    devAddr = Vps_platformTI816xGetVidDecI2cAddr(vidDecId, vipInstId);
#endif

#ifdef TI_814X_BUILD
    devAddr = Vps_platformTI814xGetVidDecI2cAddr(vidDecId, vipInstId);
#endif

#ifdef TI_8107_BUILD
    devAddr = Vps_platformTI8107GetVidDecI2cAddr(vidDecId, vipInstId);
#endif
#endif

    return (devAddr);
}

UInt8 Vps_platformGetVidEncI2cAddr(UInt32 vidEncId)
{
    UInt8 devAddr = 0u;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    devAddr = Vps_platformTI816xGetVidEncI2cAddr(vidEncId);
#endif

#ifdef TI_814X_BUILD
    devAddr = Vps_platformTI814xGetVidEncI2cAddr(vidEncId);
#endif

#ifdef TI_8107_BUILD
    devAddr = Vps_platformTI8107GetVidEncI2cAddr(vidEncId);
#endif
#endif

    return (devAddr);
}

Int32 Vps_platformSimVideoInputSelect(UInt32 vipInstId,
                                      UInt32 fileId,
                                      UInt32 pixelClk)
{
    Int32 status = FVID2_SOK;

#ifdef PLATFORM_SIM
#ifdef TI_816X_BUILD
    status = Vps_platformTI816xSimVideoInputSelect(vipInstId, fileId, pixelClk);
#endif

#ifdef TI_814X_BUILD
    status = Vps_platformTI814xSimVideoInputSelect(vipInstId, fileId, pixelClk);
#endif

#ifdef TI_8107_BUILD
    status = Vps_platformTI814xSimVideoInputSelect(vipInstId, fileId, pixelClk);
#endif
#endif

    return (status);
}

Int32 Vps_platformVideoResetVideoDevices()
{
    Int32 status = FVID2_SOK;

#ifdef VPS_PLATFORM_RESET_VIDEO_DECODER

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    status = Vps_platformTI816xResetVideoDevices();
#endif

#ifdef TI_814X_BUILD
    status = Vps_platformTI814xResetVideoDevices();
#endif /* TI_814X_BUILD */

#ifdef TI_8107_BUILD
    status = Vps_platformTI8107ResetVideoDevices();
#endif /* TI_8107_BUILD */
#endif /* PLATFORM_EVM_SI */

#endif /* VPS_PLATFORM_RESET_VIDEO_DECODER */

    return status;
}

Vps_PlatformCpuRev Vps_platformGetCpuRev(void)
{
    return (gPlatformCommonObj.cpuRev);
}

Vps_PlatformBoardRev Vps_platformGetBaseBoardRev(void)
{
#ifdef PLATFORM_ZEBU
    gPlatformCommonObj.baseBoardRev = VPS_PLATFORM_BOARD_REV_A;
    return (gPlatformCommonObj.baseBoardRev);
#else
    if (VPS_PLATFORM_BOARD_REV_MAX == gPlatformCommonObj.baseBoardRev)
    {
#ifdef TI_816X_BUILD
        gPlatformCommonObj.baseBoardRev = Vps_platformTI816xGetBaseBoardRev();
#endif  /* #ifdef TI_816X_BUILD */

#ifdef TI_814X_BUILD
        gPlatformCommonObj.baseBoardRev = Vps_platformTI814xGetBaseBoardRev();
#endif  /* #ifdef TI_814X_BUILD */

#ifdef TI_8107_BUILD
        gPlatformCommonObj.baseBoardRev = Vps_platformTI8107GetBaseBoardRev();
#endif  /* #ifdef TI_8107_BUILD */
    }

    return (gPlatformCommonObj.baseBoardRev);
#endif
}

Vps_PlatformBoardRev Vps_platformGetDcBoardRev(void)
{
    if (VPS_PLATFORM_BOARD_REV_MAX == gPlatformCommonObj.dcBoardRev)
    {
#ifdef TI_816X_BUILD
        gPlatformCommonObj.dcBoardRev = Vps_platformTI816xGetDcBoardRev();
#endif  /* #ifdef TI_816X_BUILD */

#ifdef TI_814X_BUILD
        gPlatformCommonObj.dcBoardRev = Vps_platformTI814xGetDcBoardRev();
#endif  /* #ifdef TI_814X_BUILD */

#ifdef TI_8107_BUILD
        gPlatformCommonObj.dcBoardRev = Vps_platformTI8107GetDcBoardRev();
#endif  /* #ifdef TI_8107_BUILD */
    }

    return (gPlatformCommonObj.dcBoardRev);
}

static Fdrv_Handle Vps_platformCreate(
                        UInt32                    drvId,
                        UInt32                    instanceId,
                        Ptr                       createArgs,
                        Ptr                       createStatusArgs,
                        const FVID2_DrvCbParams  *fdmCbParams)
{
    vpsPlatformDrvLock();
    gPlatformCommonObj.openCnt ++;
    vpsPlatformDrvUnLock();

    return (Fdrv_Handle)(&gPlatformCommonObj);
}

static Int32 Vps_platformDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    vpsPlatformDrvLock();
    gPlatformCommonObj.openCnt --;
    vpsPlatformDrvUnLock();

    return (FVID2_SOK);
}

static Int32 Vps_platformControl(
                        Fdrv_Handle             handle,
                        UInt32                  cmd,
                        Ptr                     cmdArgs,
                        Ptr                     cmdStatusArgs)
{
    Int32                    retVal = FVID2_SOK;
    Vps_platformCommonObj   *platformObj = (Vps_platformCommonObj *)handle;
    Vps_SystemVPllClk       *vpllCfg = NULL;
    UInt32                   vpllCfgChReq = TRUE;

    vpsPlatformDrvLock();
    if ((NULL == cmdArgs) || (NULL == platformObj))
    {
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        switch (cmd)
        {
            case IOCTL_VPS_VID_SYSTEM_SET_VIDEO_PLL:
                vpllCfg = (Vps_SystemVPllClk *)cmdArgs;

                vpllCfgChReq = vpsPlatformVpllChangeReq(vpllCfg);

                if (TRUE == vpllCfgChReq)
                {
                    retVal = Vps_platformSetVencPixClk(vpllCfg);
                }

                if ((TRUE == vpllCfgChReq) &&  (FVID2_SOK == retVal))
                {
                    if (VPS_SYSTEM_VPLL_OUTPUT_VENC_RF == vpllCfg->outputVenc)
                    {
                        platformObj->vpllCfgRf.outputClk = vpllCfg->outputClk;
                    }
                    else if ((VPS_SYSTEM_VPLL_OUTPUT_VENC_A == vpllCfg->outputVenc) ||
                            (VPS_SYSTEM_VPLL_OUTPUT_HDMI == vpllCfg->outputVenc))
                    {
                        platformObj->vpllCfgVencA.outputClk =
                            vpllCfg->outputClk;
                    }
                    else if (VPS_SYSTEM_VPLL_OUTPUT_VENC_D == vpllCfg->outputVenc)
                    {
                        platformObj->vpllCfgVencD.outputClk =
                            vpllCfg->outputClk;
                    }
                    else
                    {
                        retVal = FVID2_EINVALID_PARAMS;
                    }
                }
                break;

            case IOCTL_VPS_VID_SYSTEM_GET_VIDEO_PLL:
                vpllCfg = (Vps_SystemVPllClk *)cmdArgs;
                if (VPS_SYSTEM_VPLL_OUTPUT_VENC_RF == vpllCfg->outputVenc)
                {
                    vpllCfg->outputClk = platformObj->vpllCfgRf.outputClk;
                }
                else if ((VPS_SYSTEM_VPLL_OUTPUT_VENC_A == vpllCfg->outputVenc) ||
                         (VPS_SYSTEM_VPLL_OUTPUT_HDMI == vpllCfg->outputVenc))
                {
                    vpllCfg->outputClk = platformObj->vpllCfgVencA.outputClk;
                }
                else if (VPS_SYSTEM_VPLL_OUTPUT_VENC_D == vpllCfg->outputVenc)
                {
                    vpllCfg->outputClk = platformObj->vpllCfgVencD.outputClk;
                }
                else
                {
                    retVal = FVID2_EINVALID_PARAMS;
                }
                break;

            case IOCTL_VPS_VID_SYSTEM_GET_PLATFORM_ID:
                *(UInt32*)cmdArgs = Vps_platformGetId();
                break;

            default:
                retVal = FVID2_EUNSUPPORTED_CMD;
                break;
        }
    }
    vpsPlatformDrvUnLock();

    return (retVal);
}

static Int32 Vps_platformSetVencPixClk(Vps_SystemVPllClk *vpllCfg)
{
    Int32 status = FVID2_SOK;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    status = Vps_platformTI816xSetVencPixClk(vpllCfg);
#endif

#ifdef TI_814X_BUILD
    status = Vps_platformTI814xSetVencPixClk(vpllCfg);
#endif

#ifdef TI_8107_BUILD
    status = Vps_platformTI8107SetVencPixClk(vpllCfg);
#endif
#endif

    return (status);
}

Vps_PlatformId Vps_platformGetId(void)
{
    return (gPlatformCommonObj.platformId);
}

UInt32 Vps_platformIsEvm(void)
{
    return (gPlatformCommonObj.isPlatformEvm);
}

UInt32 Vps_platformIsSim(void)
{
    return (gPlatformCommonObj.isPlatformEvm ? FALSE : TRUE);
}

Int32 Vps_platformSelectVideoDecoder(UInt32 vidDecId, UInt32 vipInstId)
{
    Int32 status = FVID2_SOK;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    status = Vps_platformTI816xSelectVideoDecoder(vidDecId, vipInstId);
#endif

#ifdef TI_814X_BUILD
    status = Vps_platformTI814xSelectVideoDecoder(vidDecId, vipInstId);
#endif

#ifdef TI_8107_BUILD
    status = Vps_platformTI8107SelectVideoDecoder(vidDecId, vipInstId);
#endif
#endif

    return status;
}

Vps_PlatformBoardId Vps_platformGetBoardId(void)
{
#ifdef PLATFORM_ZEBU
    gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_VC;
    return (gPlatformCommonObj.platBoardId);
#else
#ifdef CUSTOM_BOARD_NETCAM
    gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_NETCAM;
    return (gPlatformCommonObj.platBoardId);
#else
    UInt8 videoDeviceI2cAddr;
    UInt32 i2cInst;
    UInt32 vipInst = VPS_CAPT_INST_VIP0_PORTA;
    Int32 status;
    Vps_PlatformBoardId boardId = VPS_PLATFORM_BOARD_UNKNOWN;

    i2cInst = Vps_platformGetI2cInstId();
    if (VPS_PLATFORM_BOARD_MAX == gPlatformCommonObj.platBoardId)
    {
        /* Board ID not yet detected, find and save it */
        videoDeviceI2cAddr = Vps_platformGetVidDecI2cAddr(
                                 FVID2_VPS_VID_DEC_TVP5158_DRV,
                                 vipInst);

        status = Vps_deviceI2cProbeDevice(i2cInst, videoDeviceI2cAddr);
        if (FVID2_SOK ==  status)
        {
            gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_VS;
            return (VPS_PLATFORM_BOARD_VS);
        }

        /* Since we have sii9135 on both 814X and 816X VC cards on instance 1
           of VIP, probing that
           On 814X VIP instance 0 is used only for TVP7002 */
        vipInst = VPS_CAPT_INST_VIP1_PORTA;
        videoDeviceI2cAddr = Vps_platformGetVidDecI2cAddr(
                                 FVID2_VPS_VID_DEC_SII9135_DRV,
                                 vipInst);

        status = Vps_deviceI2cProbeDevice(i2cInst, videoDeviceI2cAddr);
        if (FVID2_SOK == status)
        {
            gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_VC;
            return (VPS_PLATFORM_BOARD_VC);
        }

        /* TI816x Catalog Board */
        videoDeviceI2cAddr = Vps_platformGetVidDecI2cAddr(
                                 FVID2_VPS_VID_DEC_TVP7002_DRV,
                                 vipInst);
        status = Vps_deviceI2cProbeDevice(i2cInst, videoDeviceI2cAddr);

        if (FVID2_SOK == status)
        {
            gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_CATALOG;
            return (VPS_PLATFORM_BOARD_CATALOG);
        }
        /* TI814x Catalog board
            TVP7002 I2C2 0x5D   -   VIN 0
            SiI1161 I2C2 0X76   -   VIN 0 TVP7002 and siI1161 are mutually
                                            exclusive
            PCF8575 I2C2 0X21   -   IO Expander to reset / select TVP7002
            TVP5147 I2C2 0XB8 */
        vipInst = VPS_CAPT_INST_VIP0_PORTA;
        videoDeviceI2cAddr = Vps_platformGetVidDecI2cAddr(
                                 FVID2_VPS_VID_DEC_TVP7002_DRV,
                                 vipInst);
        status = Vps_deviceI2cProbeDevice(i2cInst, videoDeviceI2cAddr);

        if (FVID2_SOK == status)
        {
            gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_CATALOG;
            return (VPS_PLATFORM_BOARD_CATALOG);
        }

        /* Unknown board */
        gPlatformCommonObj.platBoardId = VPS_PLATFORM_BOARD_UNKNOWN;
    }
    else
    {
        boardId = gPlatformCommonObj.platBoardId;
    }

    return (boardId);
#endif
#endif
}

Char *gPlatformName[] =
{
    "UNKNOWN",
    "TI816x EVM",
    "TI816x SIM",
    "TI814x EVM",
    "TI814x SIM",
    "TI8107 EVM",
};

Char *gPlatformBoardName[] =
{
    "UNKNOWN",
    "4x TVP5158 VS",
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    "1x SII9135, 1x TVP7002 VC",
    "1x TVP7002 Catalog",
#else
    "2x SII9135, 1x TVP7002 VC",
    "2x SIL1161A, 2x TVP7002 Catalog",
#endif /* TI_814X_BUILD || TI_8107_BUILD */
    "Custom",
    "Netcam"
};

Char *gPlatformCpuName[] =
{
    "ES1.0",
    "ES1.1",
    "ES2.0",
    "ES2.1",
    "UNKNOWN",
};

Char *gPlatformBoardRev[] =
{
    "UNKNOWN",
    "REV A",
    "REV B",
    "REV C",
};

const Char *Vps_platformGetString(void)
{
    Vps_PlatformId          platformId;

    /* Check for out of bound access */
    GT_assert(GT_DEFAULT_MASK,
        (VPS_PLATFORM_ID_MAX == ((sizeof(gPlatformName)) / (sizeof(Char *)))));

    platformId = Vps_platformGetId();
    if (platformId >= VPS_PLATFORM_ID_MAX)
    {
        platformId = VPS_PLATFORM_ID_UNKNOWN;
    }

    return (gPlatformName[platformId]);
}

const Char *Vps_platformGetBoardString(void)
{
    Vps_PlatformBoardId     boardId;

    /* Check for out of bound access */
    GT_assert(GT_DEFAULT_MASK,
        (VPS_PLATFORM_BOARD_MAX ==
            ((sizeof(gPlatformBoardName)) / (sizeof(Char *)))));

    boardId = Vps_platformGetBoardId();
    if (boardId >= VPS_PLATFORM_BOARD_MAX)
    {
        boardId = VPS_PLATFORM_BOARD_UNKNOWN;
    }

    return (gPlatformBoardName[boardId]);
}

const Char *Vps_platformGetCpuRevString(void)
{
    Vps_PlatformCpuRev      cpuRev;

    /* Check for out of bound access */
    GT_assert(GT_DEFAULT_MASK,
        (VPS_PLATFORM_CPU_REV_MAX ==
            ((sizeof(gPlatformCpuName)) / (sizeof(Char *)))));

    cpuRev = Vps_platformGetCpuRev();
    if (cpuRev >= VPS_PLATFORM_CPU_REV_MAX)
    {
        cpuRev = VPS_PLATFORM_CPU_REV_UNKNOWN;
    }

    return (gPlatformCpuName[cpuRev]);
}

const Char *Vps_platformGetBaseBoardRevString(void)
{
    Vps_PlatformBoardRev    boardRev;

    /* Check for out of bound access */
    GT_assert(GT_DEFAULT_MASK,
        (VPS_PLATFORM_BOARD_REV_MAX ==
            ((sizeof(gPlatformBoardRev)) / (sizeof(Char *)))));

    boardRev = Vps_platformGetBaseBoardRev();
    if (boardRev >= VPS_PLATFORM_BOARD_REV_MAX)
    {
        boardRev = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    }

    return (gPlatformBoardRev[boardRev]);
}

const Char *Vps_platformGetDcRevString(void)
{
    Vps_PlatformBoardRev    boardRev;

    /* Check for out of bound access */
    GT_assert(GT_DEFAULT_MASK,
        (VPS_PLATFORM_BOARD_REV_MAX ==
            ((sizeof(gPlatformBoardRev)) / (sizeof(Char *)))));

    boardRev = Vps_platformGetDcBoardRev();
    if (boardRev >= VPS_PLATFORM_BOARD_REV_MAX)
    {
        boardRev = VPS_PLATFORM_BOARD_REV_UNKNOWN;
    }

    return (gPlatformBoardRev[boardRev]);
}

Void Vps_platformPrintInfo(void)
{
    Vps_printf(" PLATFORM: [%s] !!!\r\n", Vps_platformGetString());

    Vps_printf(" PLATFORM: CPU Revision [%s] !!!\r\n",
        Vps_platformGetCpuRevString());

    Vps_printf(" PLATFORM: Detected [%s] Board !!!\r\n",
        Vps_platformGetBoardString());

    Vps_printf(" PLATFORM: Base Board Revision [%s] !!!\r\n",
        Vps_platformGetBaseBoardRevString());

    Vps_printf(" PLATFORM: Daughter Card Revision [%s] !!!\r\n",
        Vps_platformGetDcRevString());

    return;
}

Int32 Vps_platformEnableTvp7002Filter(FVID2_Standard standard)
{
    Int32 status = FVID2_SOK;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    status = Vps_platformTI816xEnableTvp7002Filter(standard);
#endif

#ifdef TI_814X_BUILD
    status = Vps_platformTI814xEnableTvp7002Filter(standard);
#endif

#ifdef TI_8107_BUILD
    status = Vps_platformTI8107EnableTvp7002Filter(standard);
#endif
#endif

    return (status);
}

Int32 Vps_platformEnableLcd(UInt32 isEnable)
{
    Int32 status = FVID2_SOK;

#ifdef PLATFORM_EVM_SI
#ifdef TI_8107_BUILD
    status = Vps_platformTI8107EnableLcd(isEnable);
#endif
#endif

    return (status);
}

Int32 Vps_platformEnableLcdBackLight(UInt32 isEnable)
{
    Int32 status = FVID2_SOK;

#ifdef PLATFORM_EVM_SI
#ifdef TI_8107_BUILD
    status = Vps_platformTI8107EnableLcdBackLight(isEnable);
#endif
#endif

    return (status);
}

Int32 Vps_platformSetLcdBackLight(UInt32 value)
{
    Int32 status = FVID2_SOK;

#ifdef PLATFORM_EVM_SI
#ifdef TI_8107_BUILD
    status = Vps_platformTI8107SetLcdBackLight(value);
#endif
#endif

    return (status);
}

Int32 Vps_platformEnableThs7353(FVID2_Standard standard,
                                UInt32 i2cInstId,
                                UInt32 i2cDevAddr)
{
    Int32       status = FVID2_SOK;
    UInt8       regAddr, regValue;

    switch (standard)
    {
        case FVID2_STD_1080P_60:
        case FVID2_STD_1080P_50:
        case FVID2_STD_SXGA_60:
        case FVID2_STD_SXGA_75:
        case FVID2_STD_SXGAP_60:
        case FVID2_STD_SXGAP_75:
        case FVID2_STD_UXGA_60:
            regValue = 0x9Cu;
            break;

        case FVID2_STD_1080I_60:
        case FVID2_STD_1080I_50:
        case FVID2_STD_1080P_24:
        case FVID2_STD_1080P_30:
        case FVID2_STD_720P_60:
        case FVID2_STD_720P_50:
        case FVID2_STD_SVGA_60:
        case FVID2_STD_SVGA_72:
        case FVID2_STD_SVGA_75:
        case FVID2_STD_SVGA_85:
        case FVID2_STD_XGA_60:
        case FVID2_STD_XGA_70:
        case FVID2_STD_XGA_75:
        case FVID2_STD_XGA_85:
        case FVID2_STD_WXGA_60:
        case FVID2_STD_WXGA_75:
        case FVID2_STD_WXGA_85:
            regValue = 0x94u;
            break;

        case FVID2_STD_480P:
        case FVID2_STD_576P:
        case FVID2_STD_VGA_60:
        case FVID2_STD_VGA_72:
        case FVID2_STD_VGA_75:
        case FVID2_STD_VGA_85:
            regValue = 0x4Cu;
            break;

        case FVID2_STD_NTSC:
        case FVID2_STD_PAL:
        case FVID2_STD_480I:
        case FVID2_STD_576I:
        case FVID2_STD_D1:
            regValue = 0x04u;
            break;

        default:
            regValue = 0x94u;
            break;
    }

    regAddr = 0x01u;
    status |= Vps_deviceWrite8(i2cInstId, i2cDevAddr, &regAddr, &regValue, 1);
    regAddr = 0x02u;
    status |= Vps_deviceWrite8(i2cInstId, i2cDevAddr, &regAddr, &regValue, 1);
    regAddr = 0x03u;
    status |= Vps_deviceWrite8(i2cInstId, i2cDevAddr, &regAddr, &regValue, 1);

    return (status);
}

Int32 Vps_platformSelectHdCompSyncSource(Vps_PlatformHdCompSyncSource syncSrc,
                                         UInt32 enable)
{
    Int32 status = FVID2_EFAIL;

#ifdef PLATFORM_EVM_SI
#ifdef TI_816X_BUILD
    status = Vps_platformTI816xSelectHdCompSyncSource(syncSrc, enable);
#endif
#endif

    return (status);
}

Int32 Vps_platformSelectHdCompClkSrc(Vps_VPllOutputClk clkSrc)
{
    Int32 status = FVID2_EFAIL;

#ifdef PLATFORM_EVM_SI
#ifdef TI_8107_BUILD
    status = Vps_platformTI816xSelectHdCompClkSrc(clkSrc);
#endif
#endif

    return (status);
}


/*
  Global driver level lock
*/
static Int32 vpsPlatformDrvLock(void)
{
    Semaphore_pend (gPlatformCommonObj.glock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Global driver level unlock
*/
static Int32 vpsPlatformDrvUnLock(void)
{
    Semaphore_post(gPlatformCommonObj.glock);

    return FVID2_SOK;
}

static inline UInt32 vpsPlatformVpllChangeReq(Vps_SystemVPllClk *vpllCfg)
{
    UInt32 isChReq = FALSE;
    GT_assert( GT_DEFAULT_MASK, NULL != vpllCfg);

    if (VPS_SYSTEM_VPLL_OUTPUT_VENC_RF == vpllCfg->outputVenc)
    {
        if (gPlatformCommonObj.vpllCfgRf.outputClk != vpllCfg->outputClk)
        {
            isChReq = TRUE;
        }
    }
    else if (VPS_SYSTEM_VPLL_OUTPUT_VENC_A == vpllCfg->outputVenc)
    {
        if (gPlatformCommonObj.vpllCfgVencA.outputClk != vpllCfg->outputClk)
        {
            isChReq = TRUE;
        }
    }
    else if (VPS_SYSTEM_VPLL_OUTPUT_VENC_D == vpllCfg->outputVenc)
    {
        if (gPlatformCommonObj.vpllCfgVencD.outputClk != vpllCfg->outputClk)
        {
            isChReq = TRUE;
        }
    } else if (VPS_SYSTEM_VPLL_OUTPUT_HDMI == vpllCfg->outputVenc)
    {
        isChReq=TRUE;
    }

    return (isChReq);
}

