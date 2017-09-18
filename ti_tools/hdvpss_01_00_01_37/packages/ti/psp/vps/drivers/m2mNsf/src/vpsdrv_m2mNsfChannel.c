/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <ti/psp/vps/drivers/m2mNsf/src/vpsdrv_m2mNsfPriv.h>

/*
The layout od descriptor memory in NSF driver is shown below

Descriptor Layout of a request in a channel
===========================================
Size in bytes
       16   Config Desc
       32   Out Data Desc - Output - Y420
       32   Out Data Desc - Output - C420
       32   In Data Desc - Previous Output - Y420
       32   In Data Desc - Previous Output - C420
       32   In Data Desc - Input - YC422
       16   SOC - NF_LUMA_WRITE
       16   RLD - XXX
       96   NF Config Overlay

"Config Desc" payload points to "NF Config Overlay"
which has HW register settings for this request

Descriptor Layout for a channel (VPS_NSF_MAX_DESC_SET_PER_CH requests)
===============================
  Descriptor Set 0
  Descriptor Set 1
  Descriptor Set 2
  ...
  Descriptor Set VPS_NSF_MAX_DESC_SET_PER_CH - 1

Descriptor set for a channel is allocated from a common memory pool
which is used across all channels
*/

/*
  Init and create channel specific resources
*/
Int32 Vps_nsfChannelCreate ( Vps_NsfObj * pObj, UInt32 curCh )
{
    Int32 status;
    UInt32 descSetId, queFlags;
    Vps_NsfChObj *chObj;
    UInt8 *descSetMemBaseAddr;

    GT_assert( GT_DEFAULT_MASK,  curCh < pObj->createArgs.numCh );

    /* TI814x PG1.0 has a hardware bug becuase of which NSF can't work in
     * TNF bypass and SNF bypass + TNF bypass modes
     */
    if ((VPS_PLATFORM_ID_EVM_TI814x == Vps_platformGetId()) &&
        (VPS_PLATFORM_CPU_REV_1_0 == Vps_platformGetCpuRev()))
    {
        if (VPS_NSF_DISABLE_TNF == pObj->processingCfg[curCh].bypassMode ||
            VPS_NSF_DISABLE_SNF_TNF ==
                 pObj->processingCfg[curCh].bypassMode)
        {
            return FVID2_EINVALID_PARAMS;
        }

    }

    /*
     * Nsf only supports 422I input format
     */
#ifdef _IPNC_HW_PLATFORM_
    if ((FVID2_DF_YUV422I_YUYV != pObj->createArgs.dataFormat[curCh].inDataFormat) &&
   	    (FVID2_DF_YUV422I_VYUY != pObj->createArgs.dataFormat[curCh].inDataFormat) &&
		(FVID2_DF_YUV422I_UYVY != pObj->createArgs.dataFormat[curCh].inDataFormat))
#else
    if (FVID2_DF_YUV422I_YUYV != pObj->createArgs.dataFormat[curCh].inDataFormat)
#endif
    {
        return FVID2_EINVALID_PARAMS;
    }
    if ( FVID2_DF_YUV420SP_UV != pObj->createArgs.dataFormat[curCh].outDataFormat)
    {
        return FVID2_EINVALID_PARAMS;
    }

    /*
     * determine queue empty blocking condition
     */

    /*
     * non-blocking by default
     */
    queFlags = VPSUTILS_QUE_FLAG_NO_BLOCK_QUE;
    pObj->chDescSetFreeTimeout = BIOS_NO_WAIT;

    /*
     * get channel specific object
     */
    chObj = &pObj->chObj[curCh];

    /*
     * allocate HW frame noise index for this channel
     * 'frameNoiseAutoCalcEnable ' determines if frame noise index
     * is allocated in exclusive mode (TRUE or auto mode)
     * or shared mode (FALSE or static mode)
     *
     * maximum VPS_NSF_MAX_FRAME_NOISE_IDX are available
     */
    chObj->hwFrameNoiseIdx =
        Vps_nsfHwFrameIdxAlloc ( pObj->createArgs.processingCfg[curCh].
                                 frameNoiseAutoCalcEnable );

    if ( chObj->hwFrameNoiseIdx < 0 )
        return FVID2_EALLOC;    /* Error - could not allocate HW frame noise index  */

    /*
     * create free que
     */
    status = VpsUtils_queCreate ( &chObj->descSetFreeQue,
                                  VPS_NSF_MAX_DESC_SET_PER_CH,
                                  chObj->descSetFreeQueMem, queFlags );

    if ( status != FVID2_SOK )
        return FVID2_EALLOC;    /* Error - que allocation  */

    /*
     * Allocate/assign descriptor memory address
     */
    descSetMemBaseAddr = ( UInt8 * ) Vps_nsfChDescSetAlloc (  );
    if ( descSetMemBaseAddr == NULL )
        return FVID2_EALLOC;

    /*
     * for each descriptor set in the channel ...
     */
    for ( descSetId = 0; descSetId < VPS_NSF_MAX_DESC_SET_PER_CH; descSetId++ )
    {
        /*
         * set descriptor set base address
         */
        chObj->descSetMem[descSetId] =
            descSetMemBaseAddr + descSetId * VPS_NSF_MAX_DESC_SET_SIZE;

        /*
         * add descriptor set to free que
         */
        status = VpsUtils_quePut ( &chObj->descSetFreeQue,
                                   chObj->descSetMem[descSetId], BIOS_NO_WAIT );

        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );
    }

    /*
     * set descriptor set size
     */
    pObj->descSetSize = VPS_NSF_MAX_DESC_SET_SIZE;

    /*
     * set channel data format
     */
    Vps_nsfChannelSetFormat ( pObj, &pObj->createArgs.dataFormat[curCh] );

    /*
     * set channel processing parameters
     */
    Vps_nsfChannelSetProcessingCfg ( pObj,
                                     &pObj->createArgs.processingCfg[curCh] );

    /*
     * makr channel as created
     */
    chObj->isCreate = TRUE;

    return status;
}

/*
  Free channel specific resources
*/
Int32 Vps_nsfChannelDelete ( Vps_NsfObj * pObj, UInt32 curCh )
{
    Int32 status = FVID2_SOK;
    Vps_NsfChObj *chObj;

    GT_assert( GT_DEFAULT_MASK,  curCh < pObj->createArgs.numCh );

    chObj = &pObj->chObj[curCh];

    /*
     * free HW frame noise index
     */
    Vps_nsfHwFrameIdxFree ( chObj->hwFrameNoiseIdx );

    /*
     * free descriptor set memory
     */
    Vps_nsfChDescSetFree ( chObj->descSetMem[0] );

    if ( TRUE == chObj->isCreate )
    {
        /*
         * delete queue
         */
        status = VpsUtils_queDelete ( &chObj->descSetFreeQue );

        GT_assert( GT_DEFAULT_MASK,  status == FVID2_SOK );

        chObj->isCreate = FALSE;
    }

    return status;
}

void vpsMdrvNsfSubFrameUpdtBufAddr ( VpsMdrv_NsfSubFrameChInst *slcNsfdata,
    FVID2_Frame *inFrame,
    FVID2_Frame *prevOutFrame,
    FVID2_Frame *outFrame,
    Vps_NsfDataFormat    *dataformat)
{


    inFrame->addr[FVID2_FRAME_ADDR_IDX]
       [FVID2_YUV_INT_ADDR_IDX] =
    (char *)inFrame->addr[FVID2_FRAME_ADDR_IDX]
        [FVID2_YUV_INT_ADDR_IDX]
        + (slcNsfdata->subFrameNum*slcNsfdata->slcSz*dataformat->inPitch);



    outFrame->addr[FVID2_FRAME_ADDR_IDX]
    [FVID2_YUV_SP_Y_ADDR_IDX] =
        (char *)outFrame->addr[FVID2_FRAME_ADDR_IDX]
               [FVID2_YUV_SP_Y_ADDR_IDX]
        + (slcNsfdata->subFrameNum*slcNsfdata->slcSz*dataformat->outPitch[0]);

    outFrame->addr[FVID2_FRAME_ADDR_IDX]
    [FVID2_YUV_SP_CBCR_ADDR_IDX] =
        (char *)outFrame->addr[FVID2_FRAME_ADDR_IDX]
        [FVID2_YUV_SP_CBCR_ADDR_IDX]
        + (slcNsfdata->subFrameNum*slcNsfdata->slcSz*((dataformat->outPitch[1])>>1));



    prevOutFrame->addr[FVID2_FRAME_ADDR_IDX]
    [FVID2_YUV_SP_Y_ADDR_IDX] =
        (char *)prevOutFrame->addr[FVID2_FRAME_ADDR_IDX]
               [FVID2_YUV_SP_Y_ADDR_IDX]
        + (slcNsfdata->subFrameNum*slcNsfdata->slcSz*dataformat->outPitch[0]);

    prevOutFrame->addr[FVID2_FRAME_ADDR_IDX]
    [FVID2_YUV_SP_CBCR_ADDR_IDX] =
        (char *)prevOutFrame->addr[FVID2_FRAME_ADDR_IDX]
        [FVID2_YUV_SP_CBCR_ADDR_IDX]
        + (slcNsfdata->subFrameNum*slcNsfdata->slcSz*((dataformat->outPitch[1])>>1));


    /*
     * Update the input frame height to size of the single slice
     */
    if(slcNsfdata->subFrameNum == slcNsfdata->numOfSlcs)
    {

        dataformat->inFrameHeight =
            dataformat->inFrameHeight -(slcNsfdata->subFrameNum * slcNsfdata->slcSz);


    }
    else
    {

        dataformat->inFrameHeight        = slcNsfdata->slcSz;


    }

}


Int32 Vps_nsfChannelGetAdvanceCfg( Vps_NsfObj * pObj,
                Vps_NsfAdvConfig * params)
{

    Vps_NsfChObj *chObj;

    if(params->channelNum > pObj->createArgs.numCh)
    {
        return FVID2_EINVALID_PARAMS;
    }
    /*
    * get channel object
    */
    chObj = &pObj->chObj[params->channelNum];

    /*
    * Fill the user structure of advanced config params
    * with the driver configuration values
    */

    /*
    * copy from user parameters to driver data structure
    */
    memcpy ( &params->dataFormat,
                &pObj->createArgs.dataFormat[params->channelNum],
                    sizeof ( params->dataFormat ) );


    params->processingCfg.spatialStrengthLow[0] =
            chObj->nfHwConfig.spatialStrengthLow[0];
    params->processingCfg.spatialStrengthLow[1] =
            chObj->nfHwConfig.spatialStrengthLow[1];
    params->processingCfg.spatialStrengthLow[2] =
            chObj->nfHwConfig.spatialStrengthLow[2];


    params->processingCfg.spatialStrengthHigh[0] =
                chObj->nfHwConfig.spatialStrengthHigh[0];
    params->processingCfg.spatialStrengthHigh[1] =
                chObj->nfHwConfig.spatialStrengthHigh[1];
    params->processingCfg.spatialStrengthHigh[2] =
                chObj->nfHwConfig.spatialStrengthHigh[2];


    params->processingCfg.temporalStrength =
            chObj->nfHwConfig.temporalStrength;

    params->processingCfg.temporalTriggerNoise =
            chObj->nfHwConfig.temporalTriggerNoise;

    params->processingCfg.noiseIirCoeff =
            chObj->nfHwConfig.noiseIirCoeff;

    params->processingCfg.maxNoise =
            chObj->nfHwConfig.maxNoise;

    params->processingCfg.pureBlackThres =
            chObj->nfHwConfig.pureBlackThres;

    params->processingCfg.pureWhiteThres =
            chObj->nfHwConfig.pureWhiteThres;

    params->processingCfg.staticFrameNoise[0] =
        chObj->nfHwConfig.frameNoise[0];
    params->processingCfg.staticFrameNoise[1] =
        chObj->nfHwConfig.frameNoise[1];
    params->processingCfg.staticFrameNoise[2] =
        chObj->nfHwConfig.frameNoise[2];


    if(chObj->nfHwConfig.bypassSpatial == FALSE)
    {
        if(chObj->nfHwConfig.bypassTemporal == FALSE)
            params->processingCfg.bypassMode =
                VPS_NSF_DISABLE_NONE;
        else
            params->processingCfg.bypassMode =
                VPS_NSF_DISABLE_TNF;
    }
    else
    {
        if(chObj->nfHwConfig.bypassTemporal == FALSE)
            params->processingCfg.bypassMode =
                VPS_NSF_DISABLE_SNF;
        else
            params->processingCfg.bypassMode =
                VPS_NSF_DISABLE_SNF_TNF;
    }

     if((chObj->nfHwConfig.initFrameNoise == TRUE) &&
            (chObj->nfHwConfig.updateFrameNoise == FALSE))
    {
        params->processingCfg.frameNoiseAutoCalcEnable   = FALSE;
    }
    else
    {
        params->processingCfg.frameNoiseAutoCalcEnable   = TRUE;
    }

    params->videoSrcIndex = chObj->nfHwConfig.videoSrcIndex;

    params->enable = chObj->nfHwConfig.enable;

    params->refFrameConfig = chObj->nfHwConfig.refFrameConfig;

    params->processingCfg.frameNoiseCalcReset =
        pObj->createArgs.processingCfg[params->channelNum].frameNoiseCalcReset;

    return FVID2_SOK;

}

/*
  Set channel data format parameters like width x height
  Format will get updated on the next channel request made to the driver
*/
Int32 Vps_nsfChannelSetFormat ( Vps_NsfObj * pObj, Vps_NsfDataFormat * format )
{
    Vps_NsfChObj *chObj;

    if(format->channelNum > pObj->createArgs.numCh)
    {
            return FVID2_EINVALID_PARAMS;
    }

    /*
     * get channel object
     */
    chObj = &pObj->chObj[format->channelNum];

    /*
     * copy from user parameters to driver data structure
     */
    memcpy ( &pObj->createArgs.dataFormat[format->channelNum], format,
             sizeof ( *format ) );

    /*
     * update HW config parameter structure, HW registers
     * or config descriptor itself not updated here
     */
    chObj->nfHwConfig.width = format->inFrameWidth;
    chObj->nfHwConfig.height = format->inFrameHeight;

    /*
     * mark how many descriptor set's to update
     * it should be all descriptor set's i.e, VPS_NSF_MAX_DESC_SET_PER_CH
     */
    chObj->updateDescSet = VPS_NSF_MAX_DESC_SET_PER_CH;

    return FVID2_SOK;
}

/*
  Set channel processing parameter like strength, filter type

  HW registers or config descriptor are not updated here.
  Processing parameters will get updated on the next channel request made to the driver.
*/
Int32 Vps_nsfChannelSetProcessingCfg ( Vps_NsfObj * pObj,
                                       Vps_NsfProcessingCfg * params )
{
    Vps_NsfChObj *chObj;
    VpsHal_NfConfig nfHwDefault;

    if(params->channelNum > pObj->createArgs.numCh)
    {
            return FVID2_EINVALID_PARAMS;
    }

    /*
     * get channel object
     */
    chObj = &pObj->chObj[params->channelNum];

    /*
     * copy from user parameters to driver data structure
     */
    memcpy ( &pObj->createArgs.processingCfg[params->channelNum],
             params, sizeof ( Vps_NsfProcessingCfg ) );

    /*
     * get HW default config in local data structure
     */
    VpsHal_nfGetDefaultConfig ( &nfHwDefault );

    /*
     * update driver config data structure to user
     * supplied value or default depending on the value
     *
     * when 'params' value is VPS_NSF_PROCESSING_CFG_DEFAULT,
     * value to be set is taken from 'nfHwDefault'
     * else it is taken from 'params'
     */

    if ( params->spatialStrengthLow[0] != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.spatialStrengthLow[0] = params->spatialStrengthLow[0];
    else
        chObj->nfHwConfig.spatialStrengthLow[0] =
            nfHwDefault.spatialStrengthLow[0];

    if ( params->spatialStrengthLow[1] != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.spatialStrengthLow[1] = params->spatialStrengthLow[1];
    else
        chObj->nfHwConfig.spatialStrengthLow[1] =
            nfHwDefault.spatialStrengthLow[1];

    if ( params->spatialStrengthLow[2] != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.spatialStrengthLow[2] = params->spatialStrengthLow[2];
    else
        chObj->nfHwConfig.spatialStrengthLow[2] =
            nfHwDefault.spatialStrengthLow[2];

    if ( params->spatialStrengthHigh[0] != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.spatialStrengthHigh[0] =
            params->spatialStrengthHigh[0];
    else
        chObj->nfHwConfig.spatialStrengthHigh[0] =
            nfHwDefault.spatialStrengthHigh[0];

    if ( params->spatialStrengthHigh[1] != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.spatialStrengthHigh[1] =
            params->spatialStrengthHigh[1];
    else
        chObj->nfHwConfig.spatialStrengthHigh[1] =
            nfHwDefault.spatialStrengthHigh[1];

    if ( params->spatialStrengthHigh[2] != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.spatialStrengthHigh[2] =
            params->spatialStrengthHigh[2];
    else
        chObj->nfHwConfig.spatialStrengthHigh[2] =
            nfHwDefault.spatialStrengthHigh[2];

    if ( params->temporalStrength != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.temporalStrength = params->temporalStrength;
    else
        chObj->nfHwConfig.temporalStrength = nfHwDefault.temporalStrength;

    if ( params->temporalTriggerNoise != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.temporalTriggerNoise = params->temporalTriggerNoise;
    else
        chObj->nfHwConfig.temporalTriggerNoise =
            nfHwDefault.temporalTriggerNoise;

    if ( params->noiseIirCoeff != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.noiseIirCoeff = params->noiseIirCoeff;
    else
        chObj->nfHwConfig.noiseIirCoeff = nfHwDefault.noiseIirCoeff;

    if ( params->maxNoise != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.maxNoise = params->maxNoise;
    else
        chObj->nfHwConfig.maxNoise = nfHwDefault.maxNoise;

    if ( params->pureBlackThres != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.pureBlackThres = params->pureBlackThres;
    else
        chObj->nfHwConfig.pureBlackThres = nfHwDefault.pureBlackThres;

    if ( params->pureWhiteThres != VPS_NSF_PROCESSING_CFG_DEFAULT )
        chObj->nfHwConfig.pureWhiteThres = params->pureWhiteThres;
    else
        chObj->nfHwConfig.pureWhiteThres = nfHwDefault.pureWhiteThres;

    chObj->nfHwConfig.videoSrcIndex = chObj->hwFrameNoiseIdx;

    chObj->nfHwConfig.enable = TRUE;
    chObj->nfHwConfig.refFrameConfig = VPS_NSF_REF_FRAME_ENABLED;
    chObj->nfHwConfig.initFrameNoise = FALSE;
    chObj->nfHwConfig.updateFrameNoise = TRUE;

    chObj->nfHwConfig.frameNoise[0] = params->staticFrameNoise[0];
    chObj->nfHwConfig.frameNoise[1] = params->staticFrameNoise[1];
    chObj->nfHwConfig.frameNoise[2] = params->staticFrameNoise[2];

    if ( params->frameNoiseAutoCalcEnable == FALSE )
    {
        /*
         * when frame noise calculation is disabled
         * do not update frame noise calculation in hardware registers
         * and init frame noise from static frame noise registers
         */
        chObj->nfHwConfig.initFrameNoise = TRUE;
        chObj->nfHwConfig.updateFrameNoise = FALSE;
    }

    switch (params->bypassMode)
    {
        case VPS_NSF_DISABLE_NONE:
            chObj->nfHwConfig.bypassSpatial = FALSE;
            chObj->nfHwConfig.bypassTemporal = FALSE;
            break;
        case VPS_NSF_DISABLE_SNF:
            chObj->nfHwConfig.bypassSpatial = TRUE;
            chObj->nfHwConfig.bypassTemporal = FALSE;
            break;
        case VPS_NSF_DISABLE_TNF:
            chObj->nfHwConfig.bypassSpatial = FALSE;
            chObj->nfHwConfig.bypassTemporal = TRUE;
            break;
        case VPS_NSF_DISABLE_SNF_TNF:
            chObj->nfHwConfig.bypassSpatial = TRUE;
            chObj->nfHwConfig.bypassTemporal = TRUE;
            break;
        default:
            chObj->nfHwConfig.bypassSpatial = FALSE;
            chObj->nfHwConfig.bypassTemporal = FALSE;
    }


    /* Noise filter in noise filtering mode is not working on TI816x 1.0 */
    if ((VPS_PLATFORM_ID_EVM_TI816x == Vps_platformGetId()) &&
        (VPS_PLATFORM_CPU_REV_1_0 == Vps_platformGetCpuRev()))
    {
        params->bypassMode = VPS_NSF_DISABLE_SNF_TNF;
    }


    if ( params->bypassMode == VPS_NSF_DISABLE_SNF )
    {
        /*
         * when SNF is bypass, all SNF strengths are forced to 0
         */
        chObj->nfHwConfig.spatialStrengthLow[0] = 0;
        chObj->nfHwConfig.spatialStrengthLow[1] = 0;
        chObj->nfHwConfig.spatialStrengthLow[2] = 0;
        chObj->nfHwConfig.spatialStrengthHigh[0] = 0;
        chObj->nfHwConfig.spatialStrengthHigh[1] = 0;
        chObj->nfHwConfig.spatialStrengthHigh[2] = 0;
    }
    if ( TRUE == params->frameNoiseCalcReset )
        chObj->frameNoiseCalcReset = VPS_NSF_RESET_FRAME_NOISE_RESET;

    if ( params->bypassMode == VPS_NSF_DISABLE_TNF )
    {
        /*
         * when TNF is bypass, all TNF strengths are forced to 0
         * and reference frame read is disabled to save bandwidth
         */
        chObj->nfHwConfig.temporalStrength = 0;

        chObj->nfHwConfig.refFrameConfig = VPS_NSF_REF_FRAME_DISABLED;
    }

    if ( params->bypassMode == VPS_NSF_DISABLE_SNF_TNF )
    {
        /*
         * when SNF+TNF is bypass, all SNF+TNF strengths are forced to 0
         * and reference frame read is disabled to save bandwidth
         */

        chObj->nfHwConfig.bypassSpatial = TRUE;
        chObj->nfHwConfig.spatialStrengthLow[0] = 0;
        chObj->nfHwConfig.spatialStrengthLow[1] = 0;
        chObj->nfHwConfig.spatialStrengthLow[2] = 0;
        chObj->nfHwConfig.spatialStrengthHigh[0] = 0;
        chObj->nfHwConfig.spatialStrengthHigh[1] = 0;
        chObj->nfHwConfig.spatialStrengthHigh[2] = 0;
        chObj->nfHwConfig.temporalStrength = 0;
        chObj->nfHwConfig.refFrameConfig = VPS_NSF_REF_FRAME_DISABLED;
    }

    /*
     * mark how many descriptor set's to update
     * it should be all descriptor set's i.e, VPS_NSF_MAX_DESC_SET_PER_CH
     */
    chObj->updateDescSet = VPS_NSF_MAX_DESC_SET_PER_CH;

    return FVID2_SOK;
}

/*
  Update channel descriptor set based on latest format and processing paramters
*/
Int32 Vps_nsfChannelUpdateDescSet ( Vps_NsfObj * pObj,
                                    Vps_NsfChObj * chObj,
                                    Vps_NsfDataFormat * format,
                                    UInt8 * descSetMem )
{
    VpsHal_VpdmaInDescParams inDescInfo;
    VpsHal_VpdmaOutDescParams outDescInfo;
    UInt8 *curDescPtr;
    VpsHal_NfConfigOverlay *pNfConfigOverlay;
    UInt32 frameAlign;

    frameAlign = VPS_NSF_FRAME_SIZE_ALIGN;

    /*
     * descriptor set base address
     */
    curDescPtr = descSetMem;

    /*
     * init in/out descriptor structures
     */
    inDescInfo.lineSkip = VPSHAL_VPDMA_LS_1;
    inDescInfo.notify = FALSE;
    inDescInfo.priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD_NSF;
    inDescInfo.is1DMem = FALSE;

    outDescInfo.lineSkip = VPSHAL_VPDMA_LS_1;
    outDescInfo.notify = FALSE;
    outDescInfo.priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR_NSF;
    outDescInfo.memType = ( VpsHal_VpdmaMemoryType ) format->outMemType;
    outDescInfo.nextChannel = ( VpsHal_VpdmaChannel ) 0;
    outDescInfo.is1DMem = FALSE;
    outDescInfo.maxWidth = VPSHAL_VPDMA_OUTBOUND_MAX_WIDTH_UNLIMITED;
    outDescInfo.maxHeight = VPSHAL_VPDMA_OUTBOUND_MAX_HEIGHT_UNLIMITED;

    /*
     * get config overlay memory address
     */
    pNfConfigOverlay = Vps_nsfChannelGetNfConfigOverlayAddr ( descSetMem );

    if ( chObj->frameNoiseCalcReset == VPS_NSF_RESET_FRAME_NOISE_RESET )
    {
        /*
         * frame noise calc is to be reseted
         */

        /*
         * keep a copy of 'steady-state' config value
         */
        chObj->initFrameNoiseOrg = chObj->nfHwConfig.initFrameNoise;
        chObj->refFrameConfigOrg = chObj->nfHwConfig.refFrameConfig;

        /*
         * set config values to reset frame noise
         */
        chObj->nfHwConfig.initFrameNoise = TRUE;

        chObj->nfHwConfig.refFrameConfig = VPS_NSF_REF_FRAME_DISABLED;

        /*
         * change state so that next time, steady-state values are setup
         */
        chObj->frameNoiseCalcReset = VPS_NSF_RESET_FRAME_NOISE_RESET_DONE;
    }
    else if ( chObj->frameNoiseCalcReset ==
              VPS_NSF_RESET_FRAME_NOISE_RESET_DONE )
    {
        /*
         * next state as programmed earlier, set config to 'steady-state' values
         */
        chObj->nfHwConfig.initFrameNoise = chObj->initFrameNoiseOrg;
        chObj->nfHwConfig.refFrameConfig = chObj->refFrameConfigOrg;

        /*
         * mark as parameter update done so no need to further update parameters
         */
        chObj->frameNoiseCalcReset = VPS_NSF_RESET_FRAME_NOISE_IDLE;
    }

    /*
     * set NF HW params in overlay memory
     * chObj->nfHwConfig points to latest NF config
     *
     * this API is locked at upper level to ensure, format, processing
     * parameter change API and this update is mutually exclusive
     */
    VpsHal_nfSetConfig ( gVps_nsfCommonObj.nsfHalHandle,
                         &chObj->nfHwConfig, pNfConfigOverlay );

    /*
     * create config descriptor with payload as pNfConfigOverlay
     */
    VpsHal_vpdmaCreateConfigDesc ( curDescPtr,
                                   VPSHAL_VPDMA_CONFIG_DEST_MMR,
                                   VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                                   VPSHAL_VPDMA_CCT_INDIRECT,
                                   VPS_NSF_CONFIG_PAYLOAD_SIZE,
                                   ( UInt8 * ) pNfConfigOverlay, NULL, 0 );
    curDescPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

    /*
     * create output - Y descriptor
     */
    outDescInfo.channel = VPSHAL_VPDMA_CHANNEL_NF_WR_LUMA;
    outDescInfo.dataType = VPSHAL_VPDMA_CHANDT_Y420;
    outDescInfo.lineStride = format->outPitch[FVID2_YUV_SP_Y_ADDR_IDX];
    outDescInfo.nextChannel = outDescInfo.channel;

    VpsHal_vpdmaCreateOutBoundDataDesc ( curDescPtr, &outDescInfo );

    #ifdef VPS_NSF_ENABLE_WR_DESC
    VpsHal_vpdmaSetDescWriteAddr(
        curDescPtr,
        descSetMem
            + VPS_NSF_WR_DESC_OFFSET
            + (VPSHAL_VPDMA_DATA_DESC_SIZE
              * FVID2_YUV_SP_Y_ADDR_IDX)
        );
    VpsHal_vpdmaSetWriteDesc(curDescPtr, TRUE);
    #endif

    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

    /*
     * create output - C descriptor
     */
    outDescInfo.channel = VPSHAL_VPDMA_CHANNEL_NF_WR_CHROMA;
    outDescInfo.dataType = VPSHAL_VPDMA_CHANDT_C420;
    outDescInfo.lineStride = format->outPitch[FVID2_YUV_SP_CBCR_ADDR_IDX];
    outDescInfo.nextChannel = outDescInfo.channel;

    VpsHal_vpdmaCreateOutBoundDataDesc ( curDescPtr, &outDescInfo );

    #ifdef VPS_NSF_ENABLE_WR_DESC
    VpsHal_vpdmaSetDescWriteAddr(
        curDescPtr,
        descSetMem
            + VPS_NSF_WR_DESC_OFFSET
            + (VPSHAL_VPDMA_DATA_DESC_SIZE
              * FVID2_YUV_SP_CBCR_ADDR_IDX)
        );
    VpsHal_vpdmaSetWriteDesc(curDescPtr, TRUE);
    #endif

    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

    /*
     * create previous output - Y descriptor
     */
    inDescInfo.transWidth =
        VpsUtils_align ( format->inFrameWidth, frameAlign );
    inDescInfo.transHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );
    inDescInfo.frameWidth =
        VpsUtils_align ( format->inFrameWidth, frameAlign );
    inDescInfo.frameHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );
    inDescInfo.startX = 0;
    inDescInfo.startY = 0;
    inDescInfo.channel = VPSHAL_VPDMA_CHANNEL_NF_PREV_LUMA;
    inDescInfo.nextChannel = inDescInfo.channel;
    inDescInfo.dataType = VPSHAL_VPDMA_CHANDT_Y420;
    inDescInfo.lineStride = format->outPitch[FVID2_YUV_SP_Y_ADDR_IDX];
    inDescInfo.memType = ( VpsHal_VpdmaMemoryType ) format->outMemType;

    if ( chObj->nfHwConfig.refFrameConfig == VPS_NSF_REF_FRAME_DISABLED )
    {
        /*
         * previous output is disabled, then program dummy descriptor in its place
         */
        VpsHal_vpdmaCreateDummyDesc ( curDescPtr );
        VpsHal_vpdmaCreateDummyDesc ( curDescPtr +
                                      VPSHAL_VPDMA_CONFIG_DESC_SIZE );
    }
    else
    {
        /*
         * previous output is not disabled, program descriptor
         */
        VpsHal_vpdmaCreateInBoundDataDesc ( curDescPtr, &inDescInfo );
    }
    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

    /*
     * create previous output - C descriptor
     */
    inDescInfo.transHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign ) / 2;
    inDescInfo.frameHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign ) / 2;
    inDescInfo.channel = VPSHAL_VPDMA_CHANNEL_NF_PREV_CHROMA;
    inDescInfo.nextChannel = inDescInfo.channel;
    inDescInfo.dataType = VPSHAL_VPDMA_CHANDT_C420;
    inDescInfo.lineStride = format->outPitch[FVID2_YUV_SP_CBCR_ADDR_IDX];
    inDescInfo.memType = ( VpsHal_VpdmaMemoryType ) format->outMemType;

    if ( chObj->nfHwConfig.refFrameConfig == VPS_NSF_REF_FRAME_DISABLED )
    {
        /*
         * previous output is disabled, then program dummy descriptor in its place
         */
        VpsHal_vpdmaCreateDummyDesc ( curDescPtr );
        VpsHal_vpdmaCreateDummyDesc ( curDescPtr +
                                      VPSHAL_VPDMA_CONFIG_DESC_SIZE );
    }
    else
    {
        /*
         * previous output is not disabled, program descriptor
         */
        VpsHal_vpdmaCreateInBoundDataDesc ( curDescPtr, &inDescInfo );
    }
    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

    /*
     * create input YUV422 descriptor
     * - this must be programmed after output descriptors
     */
    inDescInfo.transWidth =
        VpsUtils_align ( format->inFrameWidth, frameAlign );
    inDescInfo.transHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );
    inDescInfo.frameWidth = VpsUtils_align ( format->inFrameWidth, frameAlign );
    inDescInfo.frameHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );
    inDescInfo.startX = 0;
    inDescInfo.startY = 0;
    inDescInfo.channel = VPSHAL_VPDMA_CHANNEL_NF_RD;
    inDescInfo.nextChannel = inDescInfo.channel;
    inDescInfo.dataType = VPSHAL_VPDMA_CHANDT_YC422;

#ifdef _IPNC_HW_PLATFORM_
	if ((format->inDataFormat == FVID2_DF_YUV422I_VYUY) ||
	    (format->inDataFormat == FVID2_DF_YUV422I_UYVY))
    	inDescInfo.dataType = VPSHAL_VPDMA_CHANDT_CY422;
#endif

    inDescInfo.lineStride = format->inPitch;
    inDescInfo.memType = ( VpsHal_VpdmaMemoryType ) format->inMemType;

    VpsHal_vpdmaCreateInBoundDataDesc ( curDescPtr, &inDescInfo );
    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

#if(VPS_NSF_NUM_SOC_DESC>0)
    /*
     * program sync on client descriptor in order to wait
     * until output starts arriving
     * once output starts arrivingnew set of descriptors can
     * be programmed for next request
     * without waiting for all of output to complete since all
     * NF registers are shadowed
     */
    VpsHal_vpdmaCreateSOCCtrlDesc ( curDescPtr,
                                    VPSHAL_VPDMA_CHANNEL_NF_WR_LUMA,
                                    VPSHAL_VPDMA_SOC_SOF, 0, 0 );
    curDescPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
#endif

#if(VPS_NSF_NUM_SOCH_DESC>0)
    /*
     * for debug purposes program SOC on output channel not-active, i.e all output completed
     */
    VpsHal_vpdmaCreateSOCHCtrlDesc ( curDescPtr,
                                     VPSHAL_VPDMA_CHANNEL_NF_WR_LUMA );
    curDescPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
#endif

#if(VPS_NSF_NUM_SOCH_DESC>1)
    /*
     * for debug purposes program SOC on output channel not-active, i.e all output completed
     */
    VpsHal_vpdmaCreateSOCHCtrlDesc ( curDescPtr,
                                     VPSHAL_VPDMA_CHANNEL_NF_WR_CHROMA );
    curDescPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
#endif

#if(VPS_NSF_NUM_SOCH_DESC>2)
    /*
     * for debug purposes program SOC on output channel not-active, i.e all output completed
     */
    VpsHal_vpdmaCreateSOCHCtrlDesc ( curDescPtr,
                                     VPSHAL_VPDMA_CHANNEL_NF_RD );
    curDescPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
#endif

#if(VPS_NSF_NUM_SOCH_DESC>3)
    /*
     * for debug purposes program SOC on output channel not-active, i.e all output completed
     */
    VpsHal_vpdmaCreateSOCHCtrlDesc ( curDescPtr,
                                     VPSHAL_VPDMA_CHANNEL_NF_PREV_LUMA );
    curDescPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
#endif

#if(VPS_NSF_NUM_SOCH_DESC>4)
    /*
     * for debug purposes program SOC on output channel not-active, i.e all output completed
     */
    VpsHal_vpdmaCreateSOCHCtrlDesc ( curDescPtr,
                                     VPSHAL_VPDMA_CHANNEL_NF_PREV_CHROMA );
    curDescPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
#endif

    /*
     * program reload descriptor with next load address as NULL
     * and size as 0
     * linking itself is done inside Vps_nsfReqSubmit()
     */
    VpsHal_vpdmaCreateRLCtrlDesc ( curDescPtr, 0, 0 );
    curDescPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;

    return FVID2_SOK;
}

/*
 * Update Height and width of the frame in overlay registers with the new values
 * Also Updates the height of the last slice(usually it will be diff from normal
 * height)
 * In descriptors
 */
Int32 Vps_nsfChannelUpdateHeight(Vps_NsfObj * pObj,
                                    Vps_NsfChObj * chObj,
                                    Vps_NsfDataFormat * format,
                                    UInt8 * descSetMem )
{

    VpsHal_NfConfigOverlay *pNfConfigOverlay;

    VpsHal_VpdmaInDescParams inDescInfo;
    UInt8 *curDescPtr;
    UInt32 frameAlign;

    frameAlign = VPS_NSF_FRAME_SIZE_ALIGN;


    /*
    * descriptor set base address
    */
    curDescPtr = descSetMem;


    /*
    * get config overlay memory address
    */
    pNfConfigOverlay = Vps_nsfChannelGetNfConfigOverlayAddr ( descSetMem );


    VpsHal_nfSetWidthHeight ( gVps_nsfCommonObj.nsfHalHandle,
                        chObj->nfHwConfig.width,chObj->nfHwConfig.height,
                        pNfConfigOverlay );


    /* Desc is such that
        0. Config desc
        1. output Y
        2. output C
        3. Prev Output Y
        4. Prev output C
        5. Input Y
    */

    /** Point curDescPtr to Previous o/p Y descriptor **/
    curDescPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;


    /** Modify height in previous o/p Y descriptor **/
    inDescInfo.transHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );

    inDescInfo.frameHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );

    VpsHal_vpdmaSetDescHeight ( curDescPtr, &inDescInfo );


    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

    /** Modify height in previous o/p C descriptor **/
    inDescInfo.transHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign )/2;

    inDescInfo.frameHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign )/2;

    VpsHal_vpdmaSetDescHeight ( curDescPtr, &inDescInfo );

    curDescPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

    /** Modifying height in the  I/p Y descriptor**/
    inDescInfo.transHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );

    inDescInfo.frameHeight =
        VpsUtils_align ( format->inFrameHeight, frameAlign );

    VpsHal_vpdmaSetDescHeight ( curDescPtr, &inDescInfo );


    return FVID2_SOK;
}

/*
  Make channel descriptor set

  All dewscriptor set entires are not filled always.
  In steady state only input/output addresses will be updated

  Unless there is a change in data format or processing parameters in which
  case config overlay area and/or all descriptor fields will get updated
*/
Int32 Vps_nsfChannelMakeDescSet ( Vps_NsfObj * pObj,
                                  Vps_NsfReq * reqObj,
                                  FVID2_ProcessList * processList,
                                  UInt32 reqNum )
{
    Vps_NsfChObj                   *chObj;
    Int32                          status;
    VpsHal_VpdmaInDataDescOverlay  *dataDescOverlay;
    FVID2_Frame                    *inFrame, *prevOutFrame, *outFrame;
    Ptr                            inAddrY, outAddrY, outAddrC;
    Ptr                            prevOutAddrY,prevOutAddrC;
    Vps_NsfDataFormat              *dataformat;
    Int32                          actInHeight = 0;
    Vps_M2mNsfRtParams             *rtParams;


    /*
     * get pointer to input, previous output, output frames information
     */
    inFrame =
        processList->inFrameList[VPS_NSF_IN_FRAME_LIST_IDX]->frames[reqNum];

    prevOutFrame =
        processList->inFrameList[VPS_NSF_PREV_OUT_FRAME_LIST_IDX]->
        frames[reqNum];

    outFrame =
        processList->outFrameList[VPS_NSF_OUT_FRAME_LIST_IDX]->frames[reqNum];


    rtParams = (Vps_M2mNsfRtParams *)inFrame->perFrameCfg;

    /* copy FID from input to output frame  */
    outFrame->fid = inFrame->fid;

    /*
     * set channel num in request object
     */
    reqObj->chId = inFrame->channelNum;

    /*
     * check if channel num is valid
     */
    if ( reqObj->chId > pObj->createArgs.numCh )
        return FVID2_EFAIL;

    /*
     * get channel object
     */
    chObj = &pObj->chObj[reqObj->chId];



    /*Updating run time parameters*/
    if (rtParams != NULL )
    {

        if (rtParams->dataFormat != NULL)
        {
            Vps_nsfChannelSetFormat(pObj,rtParams->dataFormat);
        }

        if (rtParams->processingCfg != NULL)
        {
            Vps_nsfChannelSetProcessingCfg(pObj,rtParams->processingCfg);
        }
    }


    if (chObj->subFrameChPrms.subFrameMdEnable == TRUE)
    {


        dataformat  = &pObj->createArgs.dataFormat[reqObj->
                          chId];

        /*
         * store orignal input height
         */
        actInHeight = dataformat->inFrameHeight;


        chObj->subFrameChPrms.slcSz =
            pObj->createArgs.processingCfg[reqObj->chId].numLinesPerSubFrame;

        chObj->subFrameChPrms.subFrameNum = inFrame->subFrameInfo->subFrameNum;


        chObj->subFrameChPrms.numOfSlcs =
            (dataformat->inFrameHeight/chObj->subFrameChPrms.slcSz);

        if (chObj->subFrameChPrms.subFrameNum > chObj->subFrameChPrms.numOfSlcs)
            return FVID2_EINVALID_PARAMS;

        if ( chObj->subFrameChPrms.subFrameNum == 0)
        {
            outFrame->subFrameInfo->numOutLines = 0;
        }

        if ( dataformat->inPitch % 32 != 0)
            return FVID2_EINVALID_PARAMS;


        inAddrY      =
            inFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX];
        outAddrY     =
            outFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX];
        outAddrC     =
            outFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX];
        prevOutAddrY =
            prevOutFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX];
        prevOutAddrC =
            prevOutFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX];


        vpsMdrvNsfSubFrameUpdtBufAddr(&chObj->subFrameChPrms,
                               inFrame,
                               prevOutFrame,
                               outFrame,
                               dataformat);

        chObj->nfHwConfig.height = dataformat->inFrameHeight;

    }
    /*
     * get free descriptor set for this channel
     */
    status = VpsUtils_queGet ( &chObj->descSetFreeQue,
                               &reqObj->descSetMem,
                               1, pObj->chDescSetFreeTimeout );

    if ( status != FVID2_SOK )
        return status;  /* Error - no free descriptor set */


    if ( 0 != chObj->updateDescSet )
    {
        /*
         * descriptor set update is needed
         */

        /*
         * update descriptor set
         */
        Vps_nsfChannelUpdateDescSet ( pObj,
                                      chObj,
                                      &pObj->createArgs.dataFormat[reqObj->
                                                                   chId],
                                      reqObj->descSetMem );

        /*
         * decrement descriptor set update count,
         * when all descriptor sets for a channel are updated
         * no further descriptor set updates are done
         */
        chObj->updateDescSet--;
    }

    else if (chObj->subFrameChPrms.subFrameMdEnable == TRUE)
    {
        Vps_nsfChannelUpdateHeight ( pObj,
                                     chObj,
                                     &pObj->createArgs.dataFormat[reqObj->
                                     chId],
                                     reqObj->descSetMem );
    }

    /*
     * point to start of data descriptors
     */
    dataDescOverlay =
        ( VpsHal_VpdmaInDataDescOverlay * )
        ( ( UInt32 ) reqObj->descSetMem + VPSHAL_VPDMA_CONFIG_DESC_SIZE );

    /*
     * update output Y address
     */
    dataDescOverlay->startAddr = VpsHal_vpdmaVirtToPhy(
        outFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX]);

    dataDescOverlay++;  /* goto next data descriptor */

    /*
     * update output C address
     */
    dataDescOverlay->startAddr = VpsHal_vpdmaVirtToPhy(
        outFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX]);

    dataDescOverlay++;  /* goto next data descriptor */

    if ( chObj->nfHwConfig.refFrameConfig != VPS_NSF_REF_FRAME_DISABLED )
    {
        /*
         * reference frame input enabled
         */

        /*
         * update reference frame Y address
         */
        dataDescOverlay->startAddr = VpsHal_vpdmaVirtToPhy(
            prevOutFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX]);

        dataDescOverlay++;  /* goto next data descriptor */

        /*
         * update reference frame C address
         */
        dataDescOverlay->startAddr = VpsHal_vpdmaVirtToPhy(
            prevOutFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX]);

        dataDescOverlay++;  /* goto next data descriptor */

    }
    else
    {
        /*
         * reference frame input disabled
         */

        dataDescOverlay++;  /* skip to next data descriptor */
        dataDescOverlay++;  /* skip to next data descriptor */
    }

    /*
     * update input YUV422 address
     */
    dataDescOverlay->startAddr = VpsHal_vpdmaVirtToPhy(
        inFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX]);


    dataDescOverlay++;  /* goto next data descriptor */

    if(chObj->subFrameChPrms.subFrameMdEnable == TRUE)
    {

        outFrame->subFrameInfo->subFrameNum = chObj->subFrameChPrms.subFrameNum;
        outFrame->subFrameInfo->numOutLines += chObj->subFrameChPrms.slcSz;


        /* restore inFrm and outFrm addresses to pass processlist
        intact to application */
        inFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX]         =
            inAddrY;

        outFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_INT_ADDR_IDX]         =
            outAddrY;

        outFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX]     =
            outAddrC;

        prevOutFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_Y_ADDR_IDX]    =
            prevOutAddrY ;

        prevOutFrame->addr[FVID2_FRAME_ADDR_IDX][FVID2_YUV_SP_CBCR_ADDR_IDX] =
            prevOutAddrC;

        /** Restore the actual input height **/
        dataformat->inFrameHeight        = actInHeight;

    }

    return status;
}
