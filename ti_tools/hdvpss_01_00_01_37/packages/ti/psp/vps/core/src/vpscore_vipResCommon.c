/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/core/src/vpscore_vipResPriv.h>
#include <ti/psp/vps/drivers/capture/src/vpsdrv_capturePriv.h>
#include <ti/psp/vps/common/vps_utils.h>

/* VIP resource allocator info  */
Vcore_VipResCommonObj gVcore_vipResCommonObj;

/* Below names are used for more verbose log's during API execution */

/* Resource names */
static char *gVcore_vipResName[VCORE_VIP_RES_MAX] = {
    "PORT_A    ",
    "PORT_A_ANC",
    "PORT_B    ",
    "PORT_B_ANC",
    "SECONDARY ",
    "COMP      ",
    "CSC       ",
    "SC        ",
    "CHR_DS_0  ",
    "CHR_DS_1  ",
    "Y_UP      ",
    "UV_UP     ",
    "Y_LOW     ",
    "UV_LOW    ",
};

/* Chorma DS source names */
static char *gVcore_vipResMuxChrDsSrcName[] = {
    "DISABLED (0)",
    "SC (1)",
    "CSC (2)",
    "PORTA_422 (3)",
    "PORTB_422 (4)",
    "SECONDARY (5)",
};

/* RGB source names */
static char *gVcore_vipResMuxRgbSrcName[] = {
    "COMP (0)",
    "CSC (1)",
};

/* CSC source names */
static char *gVcore_vipResMuxCscSrcName[] = {
    "DISABLED (0)",
    "PORTA_422 (1)",
    "PORTB_422 (2)",
    "SECONDARY (3)",
    "PORTA_RGB (4)",
    "COMP (5)",
};

/* SC source names  */
static char *gVcore_vipResMuxScSrcName[] = {
    "DISABLED (0)",
    "CSC (1)",
    "PORTA_422 (2)",
    "PORTB_422 (3)",
    "SECONDARY (4)",
};

/*
  Initialize VIP resource manager

  - create semaphore locks
  - open HAL handles for VIP, CSC, and SC
*/
Int32 Vcore_vipResInit (  )
{
    Semaphore_Params semParams;
    Int32 retVal = FVID2_SOK;
    UInt16 instId, portId;

    memset ( &gVcore_vipResCommonObj, 0, sizeof ( gVcore_vipResCommonObj ) );

    gVcore_vipResCommonObj.enableDebugLog = FALSE;

#ifdef VCORE_VIP_RES_DEBUG
    gVcore_vipResCommonObj.enableDebugLog = TRUE;
#endif

    /*
     * create semaphore lock
     */
    Semaphore_Params_init ( &semParams );

    semParams.mode = Semaphore_Mode_BINARY;

    gVcore_vipResCommonObj.lock = Semaphore_create ( 1u, &semParams, NULL );

    if ( gVcore_vipResCommonObj.lock == NULL )
    {
        Vps_printf ( " VIPRES:%s:%d: Semaphore_create() failed !!!\n",
                        __FUNCTION__, __LINE__ );
        retVal = FVID2_EALLOC;
    }

    Vcore_vipResetMux (  );

    /*
     * for all VIP instances
     */
    for ( instId = 0; instId < VPSHAL_VIP_INST_MAX; instId++ )
    {

        /*
         * mark all VIP resources as available
         */
        Vcore_vipResResetAll ( gVcore_vipResCommonObj.globalResTable[instId] );

        /*
         * open CSC, SC handles
         */

        gVcore_vipResCommonObj.scHandle[instId]
            = VpsHal_scOpen ( VPSHAL_SC_INST_3 + instId );
        GT_assert( GT_DEFAULT_MASK,  gVcore_vipResCommonObj.scHandle[instId] != NULL );

        gVcore_vipResCommonObj.cscHandle[instId]
            = VpsHal_cscOpen ( VPSHAL_CSC_INST_VIP0 + instId );
        GT_assert( GT_DEFAULT_MASK,  gVcore_vipResCommonObj.cscHandle[instId] != NULL );

        /*
         * for all VIP port
         */
        for ( portId = 0; portId < VPSHAL_VIP_PORT_MAX; portId++ )
        {
            /*
             * open port handles
             */
            gVcore_vipResCommonObj.vipHandle[instId][portId]
                = VpsHal_vipOpen ( ( VpsHal_VipInst )
                                   ( VPSHAL_VIP_INST_0 + instId ),
                                   ( VpsHal_VipPort ) ( VPSHAL_VIP_PORT_A +
                                                        portId ) );
            GT_assert( GT_DEFAULT_MASK,  gVcore_vipResCommonObj.vipHandle[instId][portId] != NULL );
        }
    }

    return retVal;
}

/*
  De-init resource manager
*/
Int32 Vcore_vipResDeInit (  )
{
    UInt16 instId, portId;

    if ( gVcore_vipResCommonObj.enableDebugLog )
    {
        Vcore_vipResPrintAllStatus (  );
    }

    Semaphore_delete ( &gVcore_vipResCommonObj.lock );

    /*
     * for all VIP instances
     */
    for ( instId = 0; instId < VPSHAL_VIP_INST_MAX; instId++ )
    {
        /*
         * close SC , CSC handles
         */
        VpsHal_scClose ( gVcore_vipResCommonObj.scHandle[instId] );
        VpsHal_cscClose ( gVcore_vipResCommonObj.cscHandle[instId] );

        /*
         * for all VIP ports
         */
        for ( portId = 0; portId < VPSHAL_VIP_PORT_MAX; portId++ )
        {
            /*
             * close VIP ports
             */
            VpsHal_vipClose ( gVcore_vipResCommonObj.
                              vipHandle[instId][portId] );
        }
    }

    return 0;
}

Int32 Vcore_vipResAllocPath ( Vcore_VipResObj * pObj,
                              Vcore_VipResAllocParams * pParams )
{
    Int32 status = -1;

    /*
     * parameter check
     */
    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  pParams != NULL );

    Vcore_vipResLock (  );

    if ( gVcore_vipResCommonObj.enableDebugLog )
    {
        Vps_printf
            ( " VIP %d RES: DRV [%s, 0x%08x] path alloc in progress ... \n",
              pParams->vipInstId, pParams->drvName, pParams->drvId );
    }

    /*
     * reset path object
     */
    Vcore_vipResResetObj ( pObj );

    /*
     * copy user params inside object
     */
    pObj->resParams = *pParams;

    if ( Vcore_vipResIsInSrcMultiChYuv422 ( pObj->resParams.inSrc ) )
    {
        /*
         * input source is multi-channel
         */
        status = Vcore_vipResAllocMultiChPath ( pObj );
    }
    if ( Vcore_vipResIsInSrcSingleChYuv422 ( pObj->resParams.inSrc ) )
    {
        /*
         * input source is single-channel YUV422
         */
        status = Vcore_vipResAllocSingleChYuv422Path ( pObj );
    }
    if ( Vcore_vipResIsInSrcSingleChRgb ( pObj->resParams.inSrc ) )
    {
        /*
         * input source is single-channel RGB
         */
        status = Vcore_vipResAllocSingleChRgb888Path ( pObj );
    }
    if ( Vcore_vipResIsInSrcSingleChYuv444 ( pObj->resParams.inSrc ) )
    {
        /*
         * input source is single-channel RGB - NOT SUPPORTED AS OF NOW
         */
        status = FVID2_EFAIL;
    }

    /*
     * in case of error in path allocation,
     * free all partially acquired resources
     */
    if ( status != FVID2_SOK )
    {
        Vcore_vipResFreeAll ( pObj );

        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf
                ( " VIP %d RES: DRV [%s, 0x%08x] path alloc ERROR !!! \n",
                  pParams->vipInstId, pParams->drvName, pParams->drvId );
        }
    }
    else
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf
                ( " VIP %d RES: DRV [%s, 0x%08x] path alloc is DONE !!! \n",
                  pParams->vipInstId, pParams->drvName, pParams->drvId );
        }
    }

    Vcore_vipResUnlock (  );

    return status;
}

Int32 Vcore_vipResFreePath ( Vcore_VipResObj * pObj )
{
    Vcore_vipResLock (  );

    if ( gVcore_vipResCommonObj.enableDebugLog )
    {
        Vps_printf
            ( " VIP %d RES: DRV [%s, 0x%08x] path free in progress ... \n",
              pObj->resParams.vipInstId, pObj->resParams.drvName,
              pObj->resParams.drvId );
    }

    /*
     * Restore muxes to originl setting
     */
    Vcore_vipRestoreMux ( pObj );

    /*
     * free all resource
     */
    Vcore_vipResFreeAll ( pObj );

    if ( gVcore_vipResCommonObj.enableDebugLog )
    {
        Vps_printf
            ( " VIP %d RES: DRV [%s, 0x%08x] path free is DONE !!! \n",
              pObj->resParams.vipInstId, pObj->resParams.drvName,
              pObj->resParams.drvId );
    }

    Vcore_vipResUnlock (  );

    return 0;
}

Int32 Vcore_vipCreateConfigOvly(Vcore_VipResObj *pObj, Ptr configOvlyPtr)
{
    Int32 retVal = 0;
    VpsHal_VpsVipInst vipInstId;

    GT_assert( GT_DEFAULT_MASK, NULL != pObj);
    GT_assert( GT_DEFAULT_MASK, NULL != configOvlyPtr);
    vipInstId = (VpsHal_VpsVipInst)pObj->resParams.vipInstId;

    /* only muxes that are in the path are setup, other muxes are not modified  */
    if (pObj->muxInfo.cscSrc != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: CSC SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxCscSrcName[pObj->muxInfo.cscSrc]);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.scSrc != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: SC SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxScSrcName[pObj->muxInfo.scSrc]);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.rgbSrc != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: RGB SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxRgbSrcName[pObj->muxInfo.rgbSrc]);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.chrDsSrc[0] != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 0 SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxChrDsSrcName[pObj->muxInfo.chrDsSrc[0]]);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.chrDsSrc[1] != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 1 SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxChrDsSrcName[pObj->muxInfo.chrDsSrc[1]]);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.chrDsBypass[0] != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 0 BYPASS = %d\n",
                vipInstId,
                pObj->muxInfo.chrDsBypass[0]);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.chrDsBypass[1] != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 1 BYPASS = %d\n",
                vipInstId,
                pObj->muxInfo.chrDsBypass[1]);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.multiChSel != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: MULTI CH = %d\n",
                vipInstId,
                pObj->muxInfo.multiChSel);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.rgbOutHiSel != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: RGB HI   = %d\n",
                vipInstId,
                pObj->muxInfo.rgbOutHiSel);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    if ((pObj->muxInfo.rgbOutLoSel != -1) && (0 == retVal))
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: RGB LO   = %d\n",
                vipInstId,
                pObj->muxInfo.rgbOutLoSel);
        }
        retVal = VpsHal_vpsVipCreateConfigOvly(vipInstId, configOvlyPtr);
        configOvlyPtr = (Ptr)((UInt32)configOvlyPtr +
                              VpsHal_vpsVipGetConfigOvlySize(vipInstId));
    }

    return (retVal);
}

/*
 * Get the Configuration overlay size for the multiplexers
 */
UInt32 Vcore_vipGetConfigOvlySize(Vcore_VipResObj *pObj)
{
    /* get VIP inst ID  */
    UInt32 configOvlySize = 0u;
    VpsHal_VpsVipInst vipInstId = (VpsHal_VpsVipInst)pObj->resParams.vipInstId;

    /* only muxes that are in the path are setup, other muxes are not modified  */

    if(pObj->muxInfo.cscSrc != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: CSC SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxCscSrcName[pObj->muxInfo.cscSrc]);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.scSrc != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: SC SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxScSrcName[pObj->muxInfo.scSrc]);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }


    if(pObj->muxInfo.rgbSrc != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: RGB SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxRgbSrcName[pObj->muxInfo.rgbSrc]);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.chrDsSrc[0] != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 0 SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxChrDsSrcName[pObj->muxInfo.chrDsSrc[0]]);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.chrDsSrc[1] != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 1 SRC = %s\n",
                vipInstId,
                gVcore_vipResMuxChrDsSrcName[pObj->muxInfo.chrDsSrc[1]]);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.chrDsBypass[0] != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 0 BYPASS = %d\n",
                vipInstId,
                pObj->muxInfo.chrDsBypass[0]);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.chrDsBypass[1] != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: CHR DS 1 BYPASS = %d\n",
                vipInstId,
                pObj->muxInfo.chrDsBypass[1]);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.multiChSel != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: MULTI CH = %d\n",
                vipInstId,
                pObj->muxInfo.multiChSel);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.rgbOutHiSel != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                "VIP %d MUX: RGB HI   = %d\n",
                vipInstId,
                pObj->muxInfo.rgbOutHiSel);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    if(pObj->muxInfo.rgbOutLoSel != -1)
    {
        if(gVcore_vipResCommonObj.enableDebugLog)
        {
            Vps_printf(
                " VIP %d MUX: RGB LO   = %d\n",
                vipInstId,
                pObj->muxInfo.rgbOutLoSel);
        }
        configOvlySize += VpsHal_vpsVipGetConfigOvlySize(vipInstId);
    }

    return (configOvlySize);
}

/*
  Set muxes for muxes in the allocated path object
*/
Int32 Vcore_vipSetMux ( Vcore_VipResObj * pObj, Ptr cfgOvlyPtr )
{
    /*
     * get VIP inst ID
     */
    VpsHal_VpsVipInst vipInstId =
        ( VpsHal_VpsVipInst ) pObj->resParams.vipInstId;

    /*
     * only muxes that are in the path are setup, other muxes are not modified
     */

    if ( pObj->muxInfo.cscSrc != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CSC SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxCscSrcName[pObj->muxInfo.cscSrc] );
        }
        VpsHal_vpsVipCscMuxSrcSelect ( vipInstId,
                                       ( VpsHal_VpsVipCscMuxSrc ) pObj->muxInfo.
                                       cscSrc,
                                       cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt8 *)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.scSrc != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: SC SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxScSrcName[pObj->muxInfo.scSrc] );
        }
        VpsHal_vpsVipScMuxSrcSelect ( vipInstId,
                                      ( VpsHal_VpsVipScMuxSrc ) pObj->muxInfo.
                                      scSrc,
                                      cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.rgbSrc != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: RGB SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxRgbSrcName[pObj->muxInfo.rgbSrc] );
        }
        VpsHal_vpsVipRgbMuxSrcSelect ( vipInstId,
                                       ( VpsHal_VpsVipRgbMuxSrc ) pObj->muxInfo.
                                       rgbSrc,
                                       cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.chrDsSrc[0] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 0 SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxChrDsSrcName[pObj->muxInfo.
                                                         chrDsSrc[0]] );
        }

        VpsHal_vpsVipChrdsMuxSrcSelect
            ( vipInstId,
              VPSHAL_VPS_CHRDS_INST_0,
              ( VpsHal_VpsVipChrdsMuxSrc ) pObj->muxInfo.chrDsSrc[0],
              cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.chrDsSrc[1] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 1 SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxChrDsSrcName[pObj->muxInfo.
                                                         chrDsSrc[1]] );
        }

        VpsHal_vpsVipChrdsMuxSrcSelect
            ( vipInstId,
              VPSHAL_VPS_CHRDS_INST_1,
              ( VpsHal_VpsVipChrdsMuxSrc ) pObj->muxInfo.chrDsSrc[1],
              cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.chrDsBypass[0] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 0 BYPASS = %d\n",
                            vipInstId, pObj->muxInfo.chrDsBypass[0] );
        }

        VpsHal_vpsVipChrDsBypass
            ( vipInstId, VPSHAL_VPS_CHRDS_INST_0,
              pObj->muxInfo.chrDsBypass[0],
              cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.chrDsBypass[1] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 1 BYPASS = %d\n",
                            vipInstId, pObj->muxInfo.chrDsBypass[1] );
        }

        VpsHal_vpsVipChrDsBypass
            ( vipInstId, VPSHAL_VPS_CHRDS_INST_1,
              pObj->muxInfo.chrDsBypass[1],
              cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.multiChSel != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: MULTI CH = %d\n",
                            vipInstId, pObj->muxInfo.multiChSel );
        }

        VpsHal_vpsVipMultiChEnable ( vipInstId,
                                     pObj->muxInfo.multiChSel,
                                     cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.rgbOutHiSel != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: RGB HI   = %d\n",
                            vipInstId, pObj->muxInfo.rgbOutHiSel );
        }

        VpsHal_vpsVipRgbHighEnable ( vipInstId,
                                     pObj->muxInfo.rgbOutHiSel,
                                     cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    if ( pObj->muxInfo.rgbOutLoSel != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: RGB LO   = %d\n",
                            vipInstId, pObj->muxInfo.rgbOutLoSel );
        }

        VpsHal_vpsVipRgbLowEnable ( vipInstId,
                                    pObj->muxInfo.rgbOutLoSel,
                                    cfgOvlyPtr );
        if (NULL != cfgOvlyPtr)
        {
            cfgOvlyPtr = (Ptr) ((UInt32)cfgOvlyPtr +
                                    VpsHal_vpsVipGetConfigOvlySize(vipInstId));
        }
    }

    return FVID2_SOK;
}

Int32 Vcore_vipSetParserParams ( Vcore_VipResObj * pObj,
                                 VpsHal_VipConfig * vipConfig,
                                 VpsHal_VipPortConfig * portConfig )
{
    Int32 status = FVID2_SOK;

    GT_assert( GT_DEFAULT_MASK,  pObj != NULL );
    GT_assert( GT_DEFAULT_MASK,  vipConfig != NULL );
    GT_assert( GT_DEFAULT_MASK,  portConfig != NULL );

    if ( pObj->resInfo.vipHalHandle != NULL )
    {

        /*
         * handle is assigned, outId.e module is in the path so set it up
         */

        status |= VpsHal_vipSetVipConfig ( ( VpsHal_VipInst ) pObj->resParams.
                                           vipInstId, vipConfig );

        status |=
            VpsHal_vipSetPortConfig ( pObj->resInfo.vipHalHandle, portConfig );
    }

    return status;
}

Int32 Vcore_vipSetCscParams ( Vcore_VipResObj * pObj,
                              Vps_CscConfig * cscConfig )
{
    Int32 status = FVID2_SOK;

    if ( pObj->resInfo.cscHalHandle != NULL )
    {
        /*
         * handle is assigned, outId.e module is in the path so set it up
         */

        /*
         * user supplied NULL config so use auto-calculated config
         */
        if ( cscConfig == NULL )
            cscConfig = &pObj->cscConfig;

        status =
            VpsHal_cscSetConfig ( pObj->resInfo.cscHalHandle, cscConfig, NULL );
    }

    return status;
}

Int32 Vcore_vipSetScParams ( Vcore_VipResObj * pObj,
                             VpsHal_ScConfig * scConfig )
{
    Int32 status = FVID2_SOK;

    GT_assert( GT_DEFAULT_MASK,  scConfig != NULL );

    if ( pObj->resInfo.scHalHandle != NULL )
    {
        /*
         * handle is assigned, outId.e module is in the path so set it up
         */

        status =
            VpsHal_scSetConfig ( pObj->resInfo.scHalHandle, scConfig, NULL );
    }

    return status;
}

Int32 Vcore_vipSetVipActCropCfg(Vcore_VipResObj *pObj,
                                const VpsHal_VipCropConfig *vipHalCropCfg)
{
    Int32 status = FVID2_SOK;

    GT_assert(GT_DEFAULT_MASK, (pObj != NULL));
    GT_assert(GT_DEFAULT_MASK, (vipHalCropCfg != NULL));

    if (pObj->resInfo.vipHalHandle != NULL)
    {
        status = VpsHal_vipSetVipActCropCfg(
                     pObj->resInfo.vipHalHandle,
                     vipHalCropCfg);
    }

    return (status);
}

Int32 Vcore_vipGetVipActCropCfg(Vcore_VipResObj *pObj,
                                VpsHal_VipCropConfig *vipHalCropCfg)
{
    Int32 status = FVID2_SOK;

    GT_assert(GT_DEFAULT_MASK, (pObj != NULL));
    GT_assert(GT_DEFAULT_MASK, (vipHalCropCfg != NULL));

    if (pObj->resInfo.vipHalHandle != NULL)
    {
        status = VpsHal_vipGetVipActCropCfg(
                     pObj->resInfo.vipHalHandle,
                     vipHalCropCfg);
    }

    return (status);
}

/* Create coeff in coeff memory
 * Coeffs created only if SC is in the allocated path
 * This function is called from the Capture driver for setting the scalar
 * coefficients into provided coefficient memory.
 */
Int32 Vcore_vipSetScCoeff (Vcore_VipResObj * pObj,
                           Vps_ScCoeffParams * scCoeff,
                           Ptr scCoeffMem)
{
    Int32 status = FVID2_SOK;
    VpsHal_ScCoeffConfig scCoeffConfig;
    Ptr horzCoeffMemPtr = NULL;
    Ptr vertCoeffMemPtr = NULL;
    Ptr vertBilinearCoeffMemPtr = NULL;

    GT_assert(GT_DEFAULT_MASK, (scCoeff != NULL));

    if (pObj->resInfo.scHalHandle != NULL)
    {
        /*
         * handle is assigned, outId.e module is in the path so set it up
         */
        if (NULL != scCoeff->coeffPtr)
        {
            status = VpsHal_scCreateUserCoeffOvly(
                         pObj->resInfo.scHalHandle,
                         scCoeff->coeffPtr,
                         &horzCoeffMemPtr,
                         &vertCoeffMemPtr,
                         &vertBilinearCoeffMemPtr);
        }
        else
        {
            scCoeffConfig.hScalingSet = scCoeff->hScalingSet;
            scCoeffConfig.vScalingSet = scCoeff->vScalingSet;

            status = VpsHal_scCreateCoeffOvly(pObj->resInfo.scHalHandle,
                                     &scCoeffConfig,
                                     &horzCoeffMemPtr,
                                     &vertCoeffMemPtr,
                                     &vertBilinearCoeffMemPtr);
        }

        if (FVID2_SOK == status)
        {
            /* Now copy the horizontal and vertical coefficients into user
             * provided buffer. Bilinear coeffs are not applicable since it
             * is not a HQ scalar.
             */
            GT_assert(GT_DEFAULT_MASK, (NULL != horzCoeffMemPtr));
            VpsUtils_memcpy(scCoeffMem,
                            horzCoeffMemPtr,
                            (VPS_CAPT_VIP_SC_COEFF_SIZE / 2u));
            GT_assert(GT_DEFAULT_MASK, (NULL != vertCoeffMemPtr));
            VpsUtils_memcpy(
                (Ptr) ((UInt32) scCoeffMem + (VPS_CAPT_VIP_SC_COEFF_SIZE / 2u)),
                vertCoeffMemPtr,
                (VPS_CAPT_VIP_SC_COEFF_SIZE / 2u));

            /* Bilinear coeffs should be returned as NULL since this is not a
             * HQ scalar. Assert for that.
             */
            GT_assert(GT_DEFAULT_MASK,
                      (NULL == vertBilinearCoeffMemPtr));
        }
    }

    return status;
}

/**
 *  Returns information about the scaling factor configuration for a
 *  given channel. Also updates the registr overlay based on the calculated
 *  scaling factor.
 */
Int32 Vcore_vipGetAndUpdScFactorConfig(Vcore_VipResObj * pObj,
                                       VpsHal_ScConfig * scConfig,
                                       VpsHal_ScFactorConfig *scFactorConfig)
{
    Int32 retVal = VPS_SOK;

    /* Check for errors */
    GT_assert(VipCoreTrace, (NULL != pObj));
    GT_assert(VipCoreTrace, (NULL != scConfig));
    GT_assert(VipCoreTrace, (NULL != scFactorConfig));

    if (pObj->resInfo.scHalHandle != NULL)
    {
        /* Call the HAL function to get the scaling factor config. */
        retVal = VpsHal_scGetScFactorConfig(pObj->resInfo.scHalHandle,
                    scConfig,
                    scFactorConfig,
                    NULL);
    }

    return (retVal);
}

UInt32 Vcore_vipIsScAlloc ( Vcore_VipResObj * pObj )
{
    if ( pObj->resInfo.scHalHandle != NULL )
        return TRUE;

    return FALSE;
}

Int32 Vcore_vipEnablePort ( Vcore_VipResObj * pObj, UInt32 enable )
{
    Int32 status = FVID2_SOK;

    if ( pObj->resInfo.vipHalHandle != NULL )
    {

        /*
         * handle is assigned, outId.e module is in the path so set it up
         */

        /*
         * enable/disable port
         */
        status = VpsHal_vipPortEnable ( pObj->resInfo.vipHalHandle, enable );
    }

    return status;
}


Int32 Vcore_vipResetPort ( Vcore_VipResObj * pObj, UInt32 enable )
{
    Int32 status = FVID2_SOK;

    if ( pObj->resInfo.vipHalHandle != NULL )
    {
        status = VpsHal_vipPortReset ( pObj->resInfo.vipHalHandle, enable );
    }

    return status;
}

Int32 Vcore_vipResetCsc ( Vcore_VipResObj * pObj, VpsHal_VpsVipInst vipInst,
                          UInt32 enable )
{
    Int32 status = FVID2_SOK;

    if ( pObj->resInfo.cscHalHandle != NULL )
    {
        status = VpsHal_vpsVipCscReset(vipInst, enable);
    }

    return status;
}

Int32 Vcore_vipResetSc ( Vcore_VipResObj * pObj, VpsHal_VpsVipInst vipInst,
                          UInt32 enable )
{
    Int32 status = FVID2_SOK;

    if ( pObj->resInfo.scHalHandle != NULL )
    {
        status = VpsHal_vpsVipScReset(vipInst, enable);
    }

    return status;
}


Int32 Vcore_vipResetChrds ( Vcore_VipResObj * pObj, VpsHal_VpsVipInst vipInst,
                            UInt32 enable )
{
    Int32 status = FVID2_SOK;

    if ( pObj->muxInfo.chrDsSrc[0] != -1 )
    {
        status = VpsHal_vpsVipChrdsReset(vipInst,
                    VPSHAL_VPS_CHRDS_INST_0, enable);
    }

    if ( pObj->muxInfo.chrDsSrc[1] != -1 )
    {
        status = VpsHal_vpsVipChrdsReset(vipInst,
                    VPSHAL_VPS_CHRDS_INST_1, enable);
    }
    return status;
}

Int32 Vcore_vipResetModules(Vcore_VipResObj *pObj,
                            UInt32 enable)
{
    Int32 status = FVID2_SOK;
    UInt32 numModules = 0;
    VpsHal_VpsClkcModule modules[10];
    VpsHal_VipInst vipInst;

    vipInst = (VpsHal_VipInst)pObj->resParams.vipInstId;

    if (NULL != pObj->resInfo.cscHalHandle)
    {
        if (VPSHAL_VIP_INST_0 == vipInst)
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP0_CSC;
        }
        else
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP1_CSC;
        }

        numModules ++;
    }

    if (NULL != pObj->resInfo.scHalHandle)
    {
        if (VPSHAL_VIP_INST_0 == vipInst)
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP0_SC;
        }
        else
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP1_SC;
        }

        numModules ++;
    }

    if (-1 != pObj->muxInfo.chrDsSrc[0])
    {
        if (VPSHAL_VIP_INST_0 == vipInst)
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP0_CHR_DS0;
        }
        else
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP1_CHR_DS0;
        }

        numModules ++;
    }

    if (-1 != pObj->muxInfo.chrDsSrc[1])
    {
        if (VPSHAL_VIP_INST_0 == vipInst)
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP0_CHR_DS1;
        }
        else
        {
            modules[numModules] = VPSHAL_VPS_CLKC_VIP1_CHR_DS1;
        }

        numModules ++;
    }

    status = VpsHal_vpsClkcResetModules(modules,
                                        numModules,
                                        enable);
    return (status);
}


/* Print all resource status  */
Int32 Vcore_vipResPrintAllStatus (  )
{
    UInt16 resId, instId;

    Vcore_vipResLock (  );

    for ( instId = 0; instId < VPSHAL_VIP_INST_MAX; instId++ )
    {
        /*
         * free resources associated with this object
         */
        for ( resId = 0; resId < VCORE_VIP_RES_MAX; resId++ )
        {
            Vcore_vipResPrintStatus ( instId, resId );
        }
    }

    Vcore_vipResUnlock (  );

    return 0;
}

int Vcore_vipResDebugLogEnable ( int enable )
{
    /*
     * enable/disable debug log
     */
    gVcore_vipResCommonObj.enableDebugLog = enable;

    return 0;
}

/*
  Alloc a Chroma DS

  when chrDsId is -1, it will try to allocate CHR DS0 first, if this alloc fails
  then it will try to allocate CRH DS 1

  when chrDsId is 0 or 1, it will try to allocate only that specific CHR DS

  when allocYcMux = FALSE, it will try to allocate only CHR DS
  when allocYcMux = TRUE, it will try to allocate Y_UP, C_UP for CHR DS 0
  and Y_LOW, C_LOW for CHR DS 1
*/
Int32 Vcore_vipResAllocChrDs ( Vcore_VipResObj * pObj,
                               Int32 chrDsId, UInt32 allocYcMux )
{
    Int32 status = 0, curChrDsId;

    curChrDsId = chrDsId;

    if ( curChrDsId < 0 )
    {
        /*
         * if any CHR DS is requested, then start with CHR DS 0
         */
        curChrDsId = 0;
    }

    if ( curChrDsId == 0 )
    {

        status = 0;

        /*
         * try to alloc CHR DS
         */
        status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_CHR_DS_0 );

        if ( status == FVID2_SOK )
        {
            /*
             * success
             */

            if ( allocYcMux )
            {
                /*
                 * Y/C mux required try to allocate it
                 */

                status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_UP );
                if ( status == FVID2_SOK )
                {
                    /*
                     * able to allocate Y mux, try C mux
                     */
                    status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_UP );
                    if ( status != FVID2_SOK )
                    {
                        /*
                         * C mux not available, free previous allocate modules
                         */
                        Vcore_vipResFree ( pObj, VCORE_VIP_RES_CHR_DS_0 );
                        Vcore_vipResFree ( pObj, VCORE_VIP_RES_Y_UP );
                    }
                }
                else
                {
                    /*
                     * Y mux not available, free previous allocate modules
                     */
                    Vcore_vipResFree ( pObj, VCORE_VIP_RES_CHR_DS_0 );
                }
            }
        }

        /*
         * if success then exit CHR DS ID as return
         */
        if ( status == 0 )
            return curChrDsId;

        /*
         * could not get CHR DS 0
         */
        curChrDsId = -1;

        if ( chrDsId < 0 )
        {
            /*
             * since user request any CHR DS, try to get CHR DS 1
             */
            curChrDsId = 1;
        }
    }
    if ( curChrDsId == 1 )
    {

        status = 0;

        /*
         * try to alloc CHR DS
         */

        status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_CHR_DS_1 );

        if ( status == FVID2_SOK )
        {
            /*
             * sucess
             */
            if ( allocYcMux )
            {
                /*
                 * try to allocate Y mux
                 */
                status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_LOW );
                if ( status == FVID2_SOK )
                {
                    /*
                     * try to allocate C mux
                     */
                    status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_LOW );
                    if ( status != FVID2_SOK )
                    {
                        /*
                         * not able to allocate C mux, free previous allocated modules
                         */
                        Vcore_vipResFree ( pObj, VCORE_VIP_RES_CHR_DS_1 );
                        Vcore_vipResFree ( pObj, VCORE_VIP_RES_Y_LOW );
                    }
                }
                else
                {
                    /*
                     * not able to allocate Y mux, free previous allocate modules
                     */
                    Vcore_vipResFree ( pObj, VCORE_VIP_RES_CHR_DS_1 );
                }
            }
        }

        /*
         * able to allocate CHR DS 1, return ID
         */
        if ( status == 0 )
            return curChrDsId;
    }

    /*
     * unable to allocate CHR DS return -1
     */
    return -1;
}

/*
  Path allocation when input is multi-ch
*/
Int32 Vcore_vipResAllocMultiChPath ( Vcore_VipResObj * pObj )
{
    Int32 status = 0, portId, vipInstId, outId;
    Vcore_VipInSrc inSrc;
    Vcore_VipOutParams *pOutParams;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT_MULTI_CH )
    {
        /*
         * for 8-bit allocate one Port
         */
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A );

        portId = VPSHAL_VIP_PORT_A;
    }
    else if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT_MULTI_CH )
    {
        /*
         * for 16-bit allocate both ports
         */
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A );
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_B );

        portId = VPSHAL_VIP_PORT_A;
    }
    else if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT_MULTI_CH )
    {
        /*
         * for 8-bit allocate one Port
         */
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_B );

        portId = VPSHAL_VIP_PORT_B;
    }
    else
    {
        /*
         * invalid input source
         */
        return -1;
    }

    /*
     * assign port handle
     */
    pObj->resInfo.vipHalHandle
        = gVcore_vipResCommonObj.vipHandle[vipInstId][portId];

    /*
     * set mux settings
     */
    pObj->muxInfo.rgbOutLoSel = FALSE;
    pObj->muxInfo.multiChSel = TRUE;

    /*
     * depending on output format allocate rest of the path
     */
    for ( outId = 0; outId < pObj->resParams.numOutParams; outId++ )
    {
        pOutParams = &pObj->resParams.outParams[outId];

        if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV422 )
        {
            /*
             * YUV422 output
             */
            if ( pOutParams->scEnable )
            {
                /*
                 * scaling not supported for YUV422 path for multi-channel
                 */
                return -1;
            }

            if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
            {
                /*
                 * if port, allocate Y_LOW mux
                 */
                status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_LOW );
            }
            else
            {
                /*
                 * else allocate UV_LOW mux
                 */
                status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_LOW );
            }

            /*
             * assign VPDMA channel info
             */
            pObj->vpdmaInfo[outId].isMultiCh = TRUE;
            pObj->vpdmaInfo[outId].vpdmaChNum = 1;

            pObj->vpdmaInfo[outId].vpdmaChId[0]
                = VCORE_VIP_VCH_VIP_MULT ( vipInstId, portId, 0 );
            /* Add the VPDMA channel offset to start channel mapping from a
             * required channel */
            pObj->vpdmaInfo[outId].vpdmaChId[0] +=
                pObj->resParams.muxModeStartChId;

            pObj->vpdmaInfo[outId].vpdmaChDataType[0] =
                VPSHAL_VPDMA_CHANDT_YC422;
        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_ANC )
        {
            /*
             * ancillary data output
             */
            if ( pOutParams->scEnable )
            {
                /*
                 * scaling not supported for ancillary path
                 */
                return -1;
            }

            /*
             * allocate path
             */
            if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
            {
                status |=
                    Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A_ANC );
            }
            else
            {
                status |=
                    Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_B_ANC );
            }

            /*
             * assign VPDMA channel info
             */
            pObj->vpdmaInfo[outId].isMultiCh = TRUE;
            pObj->vpdmaInfo[outId].vpdmaChNum = 1;

            pObj->vpdmaInfo[outId].vpdmaChId[0]
                = VCORE_VIP_VCH_VIP_MULT_ANC ( vipInstId, portId, 0 );
            /* Add the VPDMA channel offset to start channel mapping from a
             * required channel */
            pObj->vpdmaInfo[outId].vpdmaChId[0] +=
                pObj->resParams.muxModeStartChId;

            pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_ANC;
        }
        else
        {
            /*
             * no other output format supported for multi-channel
             */
            return -1;
        }
    }

    return status;
}

/*
  Reset object to init state
*/
Int32 Vcore_vipResResetObj ( Vcore_VipResObj * pObj )
{
    UInt16 vChId;

    /*
     * reset object resource table
     */
    Vcore_vipResResetAll ( pObj->resTable );

    /*
     * mark handles as NULL
     */
    pObj->resInfo.vipHalHandle = NULL;
    pObj->resInfo.scHalHandle = NULL;
    pObj->resInfo.cscHalHandle = NULL;

    /*
     * mark muxes as unused
     */
    pObj->muxInfo.cscSrc = -1;
    pObj->muxInfo.scSrc = -1;
    pObj->muxInfo.chrDsSrc[0] = -1;
    pObj->muxInfo.chrDsSrc[1] = -1;
    pObj->muxInfo.rgbSrc = -1;
    pObj->muxInfo.chrDsBypass[0] = -1;
    pObj->muxInfo.chrDsBypass[1] = -1;
    pObj->muxInfo.rgbOutHiSel = -1;
    pObj->muxInfo.rgbOutLoSel = -1;
    pObj->muxInfo.multiChSel = -1;

    /*
     * mark out params as 0
     */
    for ( vChId = 0; vChId < VCORE_VIP_MAX_OUT_PARAMS; vChId++ )
    {
        pObj->vpdmaInfo[vChId].isMultiCh = FALSE;
        pObj->vpdmaInfo[vChId].vpdmaChNum = 0;
    }

    /*
     * reset auto-calc CSC params
     */
    pObj->cscConfig.bypass = TRUE;
    pObj->cscConfig.mode = VPS_CSC_MODE_NONE;
    pObj->cscConfig.coeff = NULL;

    return 0;
}

Int32 Vcore_vipResIsInSrcPortA ( Vcore_VipInSrc inSrc )
{
    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT_MULTI_CH
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT_MULTI_CH
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_RGB888_24_BIT
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV444_24_BIT )
    {
        return TRUE;
    }

    return FALSE;
}

Int32 Vcore_vipResIsInSrcMultiChYuv422 ( Vcore_VipInSrc inSrc )
{
    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT_MULTI_CH
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT_MULTI_CH
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT_MULTI_CH )
    {
        return TRUE;
    }

    return FALSE;
}

Int32 Vcore_vipResIsInSrcSingleChYuv422 ( Vcore_VipInSrc inSrc )
{
    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT
         || inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
    {
        return TRUE;
    }

    return FALSE;
}

Int32 Vcore_vipResIsInSrcSingleChRgb ( Vcore_VipInSrc inSrc )
{
    if ( inSrc == VCORE_VIP_IN_SRC_COMP_RGB888_24_BIT
         || inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_RGB888_24_BIT )
    {
        return TRUE;
    }

    return FALSE;
}

Int32 Vcore_vipResIsInSrcSingleChYuv444 ( Vcore_VipInSrc inSrc )
{
    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV444_24_BIT )
    {
        return TRUE;
    }

    return FALSE;
}

/*
  Allocate resource
*/
Int32 Vcore_vipResAlloc ( Vcore_VipResObj * pObj, UInt16 resId )
{
    Vcore_VipResEntry *pGlobalResObj, *pResObj;
    Int32 status;

    /*
     * get resource object from table
     */
    pResObj = &pObj->resTable[resId];

    /*
     * make sure pResObj->resId matches, this should never fail
     */
    GT_assert( GT_DEFAULT_MASK,  pResObj->resId == resId );

    /*
     * get global resource object
     */
    pGlobalResObj = &gVcore_vipResCommonObj.globalResTable
        [pObj->resParams.vipInstId][pResObj->resId];

    /*
     * make sure pResObj->resId matches, this should never fail
     */
    GT_assert( GT_DEFAULT_MASK,  pGlobalResObj->resId == pResObj->resId );

    if ( pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_EXCLUSIVE )
    {
        /*
         * open mode is exclusive
         */
        if ( !pGlobalResObj->isAlloc )
        {
            /*
             * alloc resource in global resource manager as well
             */
            status =
                Vcore_vipResVrmSetState ( pObj->resParams.vipInstId, resId,
                                          TRUE );
            if ( status != FVID2_SOK )
            {
                /*
                 * cannot allocate resource from global resource manager
                 */
                return status;
            }

            /*
             * resource is not allocated, allocate it in exclusive mode
             */
            pResObj->drvName = pGlobalResObj->drvName = pObj->resParams.drvName;
            pResObj->isAlloc = pGlobalResObj->isAlloc = TRUE;
            pResObj->isShared = pGlobalResObj->isShared = FALSE;
            pResObj->allocCnt = pGlobalResObj->allocCnt = 1;
            pResObj->drvId = pGlobalResObj->drvId = pObj->resParams.drvId;

            if ( gVcore_vipResCommonObj.enableDebugLog )
            {
                Vps_printf
                    ( " VIP %d RES: [%s] is allocated in EXCLUSIVE mode \n",
                      pObj->resParams.vipInstId, gVcore_vipResName[resId] );
            }

            /*
             * done
             */
            return FVID2_SOK;
        }

        /*
         * resource already allocated in exclusive or shared  mode
         */

        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d RES: [%s] is ALREADY allocated "
                            "(ALLOC COUNT = %d, DRV ID = 0x%08x)\n",
                            pObj->resParams.vipInstId,
                            gVcore_vipResName[resId],
                            pGlobalResObj->allocCnt, pGlobalResObj->drvId );
        }

        return FVID2_EFAIL;
    }
    if ( pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_SHARED )
    {
        /*
         * open mode is shared
         */
        if ( !pGlobalResObj->isAlloc )
        {
            /*
             * alloc resource in global resource manager as well
             */
            status =
                Vcore_vipResVrmSetState ( pObj->resParams.vipInstId, resId,
                                          TRUE );
            if ( status != FVID2_SOK )
            {
                /*
                 * cannot allocate resource from global resource manager
                 */
                return status;
            }

            /*
             * not allocated, so allocate it
             */
            pResObj->drvName = pGlobalResObj->drvName = pObj->resParams.drvName;
            pResObj->isAlloc = pGlobalResObj->isAlloc = TRUE;
            pResObj->isShared = pGlobalResObj->isShared = TRUE;
            pResObj->allocCnt = pGlobalResObj->allocCnt = 1;
            pResObj->drvId = pGlobalResObj->drvId = pObj->resParams.drvId;

            if ( gVcore_vipResCommonObj.enableDebugLog )
            {
                Vps_printf ( " VIP %d RES: [%s] is allocated in SHARED mode "
                                "(ALLOC COUNT = %d, DRV ID = 0x%08x) \n",
                                pObj->resParams.vipInstId,
                                gVcore_vipResName[resId],
                                pGlobalResObj->allocCnt, pGlobalResObj->drvId );
            }

            /*
             * done
             */
            return FVID2_SOK;
        }
        if ( pGlobalResObj->isShared
             && pGlobalResObj->drvId == pObj->resParams.drvId )
        {
            /*
             * allocated, but in shared mode and allocated by same driver ID
             * so allocate it
             */
            pResObj->isAlloc = TRUE;
            pResObj->isShared = TRUE;
            pResObj->drvId = pGlobalResObj->drvId;

            /*
             * increment alloc count
             */
            pGlobalResObj->allocCnt++;
            pResObj->allocCnt = pGlobalResObj->allocCnt;

            if ( gVcore_vipResCommonObj.enableDebugLog )
            {
                Vps_printf ( " VIP %d RES: [%s] is allocated in SHARED mode "
                                "(ALLOC COUNT = %d, DRV ID = 0x%08x) \n",
                                pObj->resParams.vipInstId,
                                gVcore_vipResName[resId],
                                pGlobalResObj->allocCnt, pGlobalResObj->drvId );
            }

            /*
             * done
             */
            return FVID2_SOK;
        }

        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d RES: [%s] is ALREADY allocated "
                            "(ALLOC COUNT = %d, DRV ID = 0x%08x)\n",
                            pObj->resParams.vipInstId,
                            gVcore_vipResName[resId],
                            pGlobalResObj->allocCnt, pGlobalResObj->drvId );
        }

        /*
         * allocated in exclusive mode OR
         * allocated in shared mode but by on a different driver ID hence cannot
         * allocate in shared mode
         */
        return FVID2_EFAIL;
    }

    if ( gVcore_vipResCommonObj.enableDebugLog )
    {
        Vps_printf ( " VIP %d RES: [%s] Invalid allocation mode "
                        "(ALLOC MODE = %d)\n",
                        pObj->resParams.vipInstId,
                        gVcore_vipResName[resId], pObj->resParams.openMode );
    }

    /*
     * invalid allocation mode
     */
    return FVID2_EFAIL;
}

/* Free resource associated with a resource object */
Int32 Vcore_vipResFree ( Vcore_VipResObj * pObj, UInt16 resId )
{
    Vcore_VipResEntry *pGlobalResObj, *pResObj;

    /*
     * get resource from object
     */
    pResObj = &pObj->resTable[resId];

    /*
     * make sure pResObj->resId matches, this should never fail
     */
    GT_assert( GT_DEFAULT_MASK,  pResObj->resId == resId );

    /*
     * get global resource object
     */
    pGlobalResObj = &gVcore_vipResCommonObj.globalResTable
        [pObj->resParams.vipInstId][pResObj->resId];

    /*
     * make sure pResObj->resId matches, this should never fail
     */
    GT_assert( GT_DEFAULT_MASK,  pGlobalResObj->resId == pResObj->resId );

    /*
     * if object has allocated this resource
     */
    if ( pResObj->isAlloc )
    {

        /*
         * yes, object has allocated this resource, free it from the object
         */

        if ( pResObj->isShared )
        {

            /*
             * allocation is in shared mode
             */

            /*
             * hence decrement allocation count
             */
            if ( pGlobalResObj->allocCnt > 0 )
                pGlobalResObj->allocCnt--;

            if ( gVcore_vipResCommonObj.enableDebugLog )
            {
                Vps_printf
                    ( " VIP %d RES: [%s] is free-ed by a SHARED client "
                      "(ALLOC COUNT = %d, DRV ID = 0x%08x)\n",
                      pObj->resParams.vipInstId, gVcore_vipResName[resId],
                      pGlobalResObj->allocCnt, pGlobalResObj->drvId );
            }

            if ( pGlobalResObj->allocCnt == 0 )
            {

                /*
                 * allcoation count has become zero
                 */
                if ( gVcore_vipResCommonObj.enableDebugLog )
                {
                    Vps_printf ( " VIP %d RES: [%s] is FREE \n",
                                    pObj->resParams.vipInstId,
                                    gVcore_vipResName[resId] );
                }

                /*
                 * mark resource as fully free
                 */
                Vcore_vipResReset ( pGlobalResObj, pGlobalResObj->resId );
                Vcore_vipResReset ( pResObj, pGlobalResObj->resId );

                /*
                 * free resource in global resource manager as well
                 */

                Vcore_vipResVrmSetState ( pObj->resParams.vipInstId, resId,
                                          FALSE );
            }

        }
        else
        {
            if ( gVcore_vipResCommonObj.enableDebugLog )
            {
                Vps_printf ( " VIP %d RES: [%s] is FREE \n",
                                pObj->resParams.vipInstId,
                                gVcore_vipResName[resId] );
            }

            /*
             * allocation was in exclusive mode, so mark it as free
             */
            Vcore_vipResReset ( pGlobalResObj, pGlobalResObj->resId );
            Vcore_vipResReset ( pResObj, pGlobalResObj->resId );

            /*
             * free resource in global resource manager as well
             */
            Vcore_vipResVrmSetState ( pObj->resParams.vipInstId, resId, FALSE );
        }
    }

    return FVID2_SOK;
}

/* mark as Free or allocated in global resoruce manager module  */
Int32 Vcore_vipResVrmSetState ( UInt16 instId, UInt16 resId, UInt32 alloc )
{
    Vrm_Resource vrmResId;
    Int32 status;

    if ( instId == 0 )
    {
        vrmResId = ( Vrm_Resource ) ( VRM_RESOURCE_VIP0_PARSER_PORT_A + resId );
    }
    else
    {
        vrmResId = ( Vrm_Resource ) ( VRM_RESOURCE_VIP1_PARSER_PORT_A + resId );
    }

    if ( alloc )
    {
        status = Vrm_allocResource ( vrmResId );
    }
    else
    {
#if 0
        if(resId == VCORE_VIP_RES_SC)
        {
            VpsHal_vpsVipScMuxSrcSelect ( (VpsHal_VpsVipInst) instId,
            VPSHAL_VPS_VIP_SC_MUX_SRC_DISABLED,
            NULL );
        }
        else if(resId == VCORE_VIP_RES_CSC)
        {
            VpsHal_vpsVipCscMuxSrcSelect( (VpsHal_VpsVipInst) instId,
                VPSHAL_VPS_VIP_CSC_MUX_SRC_DISABLED,
                NULL);
        }
        else if(resId == VCORE_VIP_RES_CHR_DS_0)
        {
            VpsHal_vpsVipChrdsMuxSrcSelect((VpsHal_VpsVipInst) instId,
                VPSHAL_VPS_CHRDS_INST_0,
                VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED,
                NULL );
        }
        else if(resId == VCORE_VIP_RES_CHR_DS_1)
        {
            VpsHal_vpsVipChrdsMuxSrcSelect((VpsHal_VpsVipInst) instId,
                VPSHAL_VPS_CHRDS_INST_1,
                VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED,
                NULL );
        }
#endif

        status = Vrm_releaseResource ( vrmResId );
    }

    return status;
}

/* Reset resource and mark as free  */
Int32 Vcore_vipResReset ( Vcore_VipResEntry * pResObj, UInt16 resId )
{
    static char vipResNoneDrvName[] = "NONE";

    pResObj->drvName = vipResNoneDrvName;
    pResObj->resId = resId;
    pResObj->isAlloc = FALSE;
    pResObj->isShared = FALSE;
    pResObj->allocCnt = 0;
    pResObj->drvId = 0xFFFFFFFF;

    return 0;
}

/* Reset all resources and mark as free */
Int32 Vcore_vipResResetAll ( Vcore_VipResEntry * pResObj )
{
    UInt16 resId;

    for ( resId = 0; resId < VCORE_VIP_RES_MAX; resId++ )
    {
        Vcore_vipResReset ( &pResObj[resId], resId );
    }

    return 0;
}

/* Mark all resource in a object as free  */
Int32 Vcore_vipResFreeAll ( Vcore_VipResObj * pObj )
{
    UInt16 resId;

    /*
     * free resources associated with this object
     */
    for ( resId = 0; resId < VCORE_VIP_RES_MAX; resId++ )
    {
        Vcore_vipResFree ( pObj, resId );
    }

    /*
     * Mark object as free
     */
    Vcore_vipResResetObj ( pObj );

    return 0;
}

/*
  Lock resource manager
*/
Int32 Vcore_vipResLock (  )
{
    Semaphore_pend ( gVcore_vipResCommonObj.lock, BIOS_WAIT_FOREVER );

    return FVID2_SOK;
}

/*
  Unlock resource manager
*/
Int32 Vcore_vipResUnlock (  )
{
    Semaphore_post ( gVcore_vipResCommonObj.lock );

    return FVID2_SOK;
}

/*
  Restore muxes for paths allocated by object to original state
*/
Int32 Vcore_vipRestoreMux ( Vcore_VipResObj * pObj )
{
    Vcore_VipResEntry *pGlobalResObj;

    /*
     * get VIP instance
     */
    VpsHal_VpsVipInst vipInstId =
        ( VpsHal_VpsVipInst ) pObj->resParams.vipInstId;

    /*
     * only revert to original if this mux was setup by this path object
     */

    if ( pObj->muxInfo.cscSrc != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CSC SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxCscSrcName
                            [VPSHAL_VPS_VIP_CSC_MUX_SRC_DISABLED] );
        }

        pGlobalResObj = &gVcore_vipResCommonObj.globalResTable
            [vipInstId][VCORE_VIP_RES_CSC];

        if (1 == pGlobalResObj->allocCnt)
        {
            VpsHal_vpsVipCscMuxSrcSelect(
                vipInstId,
                VPSHAL_VPS_VIP_CSC_MUX_SRC_DISABLED,
                NULL);
        }
    }

    if ( pObj->muxInfo.scSrc != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: SC SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxScSrcName
                            [VPSHAL_VPS_VIP_SC_MUX_SRC_DISABLED] );
        }
        pGlobalResObj = &gVcore_vipResCommonObj.globalResTable
            [vipInstId][VCORE_VIP_RES_SC];

        if (1 == pGlobalResObj->allocCnt)
        {
            VpsHal_vpsVipScMuxSrcSelect(
                vipInstId,
                VPSHAL_VPS_VIP_SC_MUX_SRC_DISABLED,
                NULL);
        }
    }

    if ( pObj->muxInfo.rgbSrc != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: RGB SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxRgbSrcName
                            [VPSHAL_VPS_VIP_RGB_MUX_SRC_COMP] );
        }

        if (pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_EXCLUSIVE)
        {
            VpsHal_vpsVipRgbMuxSrcSelect(
                vipInstId,
                VPSHAL_VPS_VIP_RGB_MUX_SRC_COMP,
                NULL);
        }
    }

    if ( pObj->muxInfo.chrDsSrc[0] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 0 SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxChrDsSrcName
                            [VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED] );
        }

        if (pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_EXCLUSIVE)
        {
            VpsHal_vpsVipChrdsMuxSrcSelect(
                vipInstId,
                VPSHAL_VPS_CHRDS_INST_0,
                VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED,
                NULL);
        }
    }

    if ( pObj->muxInfo.chrDsSrc[1] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 1 SRC = %s\n",
                            vipInstId,
                            gVcore_vipResMuxChrDsSrcName
                            [VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED] );
        }

        pGlobalResObj = &gVcore_vipResCommonObj.globalResTable
            [vipInstId][VCORE_VIP_RES_CHR_DS_0];

        if (1 == pGlobalResObj->allocCnt)
        {
            VpsHal_vpsVipChrdsMuxSrcSelect(
                vipInstId,
                VPSHAL_VPS_CHRDS_INST_1,
                VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED,
                NULL);
        }
    }

    if ( pObj->muxInfo.chrDsBypass[0] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 0 BYPASS = %d\n",
                            vipInstId, FALSE );
        }

        pGlobalResObj = &gVcore_vipResCommonObj.globalResTable
            [vipInstId][VCORE_VIP_RES_CHR_DS_1];

        if (1 == pGlobalResObj->allocCnt)
        {
            VpsHal_vpsVipChrDsBypass(
                vipInstId,
                VPSHAL_VPS_CHRDS_INST_0,
                FALSE,
                NULL );
        }
    }

    if ( pObj->muxInfo.chrDsBypass[1] != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: CHR DS 1 BYPASS = %d\n",
                            vipInstId, FALSE );
        }

        if (pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_EXCLUSIVE)
        {
            VpsHal_vpsVipChrDsBypass(
                vipInstId,
                VPSHAL_VPS_CHRDS_INST_1,
                FALSE,
                NULL);
        }
    }

    if ( pObj->muxInfo.multiChSel != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: MULTI CH = %d\n", vipInstId, FALSE );
        }

        if (pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_EXCLUSIVE)
        {
            VpsHal_vpsVipMultiChEnable(vipInstId, FALSE, NULL);
        }
    }

    if ( pObj->muxInfo.rgbOutHiSel != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: RGB HI   = %d\n", vipInstId, FALSE );
        }

        if (pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_EXCLUSIVE)
        {
            VpsHal_vpsVipRgbHighEnable(vipInstId, FALSE, NULL);
        }
    }

    if ( pObj->muxInfo.rgbOutLoSel != -1 )
    {
        if ( gVcore_vipResCommonObj.enableDebugLog )
        {
            Vps_printf ( " VIP %d MUX: RGB LO   = %d\n", vipInstId, FALSE );
        }

        if (pObj->resParams.openMode == VCORE_VIP_RES_ALLOC_EXCLUSIVE)
        {
            VpsHal_vpsVipRgbLowEnable(vipInstId, FALSE, NULL);
        }
    }

    return FVID2_SOK;
}

/*
  Reset all muxes to init state
*/
Int32 Vcore_vipResetMux (  )
{
    UInt16 vipInstId;

    /*
     * for VIP instances ...
     */
    for ( vipInstId = 0; vipInstId < VPSHAL_VPS_VIP_INST_MAX; vipInstId++ )
    {

        VpsHal_vpsVipScMuxSrcSelect
            ( ( VpsHal_VpsVipInst ) vipInstId,
              VPSHAL_VPS_VIP_SC_MUX_SRC_DISABLED,
              NULL );

        VpsHal_vpsVipCscMuxSrcSelect
            ( ( VpsHal_VpsVipInst ) vipInstId,
              VPSHAL_VPS_VIP_CSC_MUX_SRC_DISABLED,
              NULL );

        VpsHal_vpsVipRgbMuxSrcSelect
            ( ( VpsHal_VpsVipInst ) vipInstId,
              VPSHAL_VPS_VIP_RGB_MUX_SRC_COMP,
              NULL );

        VpsHal_vpsVipChrdsMuxSrcSelect
            ( ( VpsHal_VpsVipInst ) vipInstId,
              VPSHAL_VPS_CHRDS_INST_0,
              VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED,
              NULL );

        VpsHal_vpsVipChrdsMuxSrcSelect
            ( ( VpsHal_VpsVipInst ) vipInstId,
              VPSHAL_VPS_CHRDS_INST_1,
              VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED,
              NULL );

        VpsHal_vpsVipChrDsBypass
            ( ( VpsHal_VpsVipInst ) vipInstId,
                VPSHAL_VPS_CHRDS_INST_0,
                FALSE,
                NULL );

        VpsHal_vpsVipChrDsBypass
            ( ( VpsHal_VpsVipInst ) vipInstId,
                VPSHAL_VPS_CHRDS_INST_1,
                FALSE,
                NULL );

        VpsHal_vpsVipMultiChEnable ( ( VpsHal_VpsVipInst ) vipInstId,
                                       FALSE,
                                       NULL );

        VpsHal_vpsVipRgbHighEnable ( ( VpsHal_VpsVipInst ) vipInstId,
                                       FALSE,
                                       NULL );

        VpsHal_vpsVipRgbLowEnable ( ( VpsHal_VpsVipInst ) vipInstId,
                                      FALSE,
                                      NULL );
    }

    return 0;
}

UInt32 Vcore_vipGetPortCompleteStatus(Vcore_VipResObj *pObj)
{
    UInt32 status = FVID2_SOK;

    if (pObj->resInfo.vipHalHandle != NULL)
    {
        status = VpsHal_vipPortGetCompleteStatus(pObj->resInfo.vipHalHandle);
    }

    return status;
}

Void Vcore_vipClearPortCompleteStatus(Vcore_VipResObj *pObj)
{
    if (pObj->resInfo.vipHalHandle != NULL)
    {
        VpsHal_vipPortClearCompleteStatus(pObj->resInfo.vipHalHandle);
    }
}

Bool Vcore_vipResIsSwapReq(Vcore_VipResAllocParams * resParams)
{

    Bool swap = FALSE;
    Vcore_VipOutParams *pOutParamsfir;
    Vcore_VipOutParams *pOutParamssec;

    /*swap makes sense only if there are more than 1 outparams*/
    if (resParams->numOutParams == 2)
    {
        pOutParamsfir = &resParams->outParams[0];
        pOutParamssec = &resParams->outParams[1];
        if (Vcore_vipResIsInSrcSingleChRgb (resParams->inSrc ))
        {
            if ((pOutParamsfir->format == VCORE_VIP_OUT_FORMAT_YUV420) &&
                (pOutParamssec->format == VCORE_VIP_OUT_FORMAT_RGB888))
            {
                swap = TRUE;
            }
        }
    }
    return swap;
}
/* Print resource status  */
Int32 Vcore_vipResPrintStatus ( UInt16 instId, UInt16 resId )
{
    Vcore_VipResEntry *pGlobalResObj;

    /*
     * get global resource object
     */
    pGlobalResObj = &gVcore_vipResCommonObj.globalResTable[instId][resId];

    Vps_printf ( " VIP %d RES %2d: NAME = [%s], ALLOC = %d, "
                    "DRV = [%s, 0x%08x], SHARED = %d, ALLOC COUNT = %d \n",
                    instId,
                    resId,
                    gVcore_vipResName[resId],
                    pGlobalResObj->isAlloc,
                    pGlobalResObj->drvName,
                    pGlobalResObj->drvId,
                    pGlobalResObj->isShared, pGlobalResObj->allocCnt );

    return 0;
}
