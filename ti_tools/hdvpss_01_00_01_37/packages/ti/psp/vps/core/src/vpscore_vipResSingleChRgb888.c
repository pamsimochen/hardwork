/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*
  this file has path allocation logic when input is RGB 24-bit
*/

#include <ti/psp/vps/core/src/vpscore_vipResPriv.h>

/*
  Allocate path with RGB888 as input

  Input could be from Port A, or COMP path
*/
Int32 Vcore_vipResAllocSingleChRgb888Path ( Vcore_VipResObj * pObj )
{
    Int32 status = 0, portId, vipInstId, outId;
    Vcore_VipOutParams *pOutParams;
    Vcore_VipInSrc inSrc;
    VpsHal_VpsVipCscMuxSrc cscSrc;
    Bool swap = FALSE;
    Int32 swapOutId;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_RGB888_24_BIT )
    {
        /*
         * for 24-bit input allocate both ports
         */
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A );
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_B );

        portId = VPSHAL_VIP_PORT_A;

        /*
         * in case CSC is used set the CSC source
         */
        cscSrc = VPSHAL_VPS_VIP_CSC_MUX_SRC_VIP_PORTA_RGB;

    }
    else if ( inSrc == VCORE_VIP_IN_SRC_COMP_RGB888_24_BIT )
    {
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_COMP_RGB );

        pObj->muxInfo.rgbSrc = VPSHAL_VPS_VIP_RGB_MUX_SRC_COMP;

        /*
         * port ID is not valid COMP input
         */
        portId = -1;

        /*
         * in case CSC is used set the CSC source
         */
        cscSrc = VPSHAL_VPS_VIP_CSC_MUX_SRC_COMP;

    }
    else
    {
        /*
         * invalid input source
         */
        return -1;
    }

    if ( portId >= 0 )
    {
        /*
         * assign VIP parser handle
         */
        pObj->resInfo.vipHalHandle
            = gVcore_vipResCommonObj.vipHandle[vipInstId][portId];
    }

    /*
     * Check if swap of allocating resource is required
     */
     swap = Vcore_vipResIsSwapReq(&pObj->resParams);
    /*
     * allocate output paths
     */
    for ( outId = 0; outId < pObj->resParams.numOutParams; outId++ )
    {
        /* if swap is true then the order of allocating resources
         * is reversed
         */
        if(TRUE == swap)
        {
            swapOutId = pObj->resParams.numOutParams - outId - 1;
        }
        else
        {
            swapOutId = outId;
        }

        pOutParams = &pObj->resParams.outParams[swapOutId];

        if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV422 )
        {
            /*
             * YUV422 output
             */
            status = Vcore_vipResAllocSingleChRgb888PathYuv422Out
                ( pObj, swapOutId, cscSrc );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_ANC )
        {
            /*
             * ancillary output
             */
            status = Vcore_vipResAllocSingleChRgb888PathAncOut
                ( pObj, swapOutId, portId );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV420 )
        {
            /*
             * YUV420 output
             */
            status = Vcore_vipResAllocSingleChRgb888PathYuv420Out
                ( pObj, swapOutId, cscSrc );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_RGB888 )
        {
            /*
             * RGB888 output
             */
            status = Vcore_vipResAllocSingleChRgb888PathRgb888Out
                ( pObj, swapOutId );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV422SP_UV )
        {
            status = Vcore_vipResAllocSingleChRgb888PathYuv422spOut
                ( pObj, swapOutId, cscSrc );
        }
        else
        {
            /*
             * other output formats not supported
             */
            return -1;
        }

        if ( status != FVID2_SOK )
            return status;
    }

    return status;
}

Int32 Vcore_vipResAllocSingleChRgb888PathYuv422Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     VpsHal_VpsVipCscMuxSrc
                                                     cscSrc )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId, chrDsId, chrDsSrc;
    UInt32 isCscAlloc, isScAlloc;

    vipInstId = pObj->resParams.vipInstId;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];

    if ( pObj->resInfo.cscHalHandle != NULL )
        isCscAlloc = TRUE;
    else
        isCscAlloc = FALSE;

    if ( pObj->resInfo.scHalHandle != NULL )
        isScAlloc = TRUE;
    else
        isScAlloc = FALSE;

    if ( !isCscAlloc )
    {
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_CSC );

        pObj->muxInfo.cscSrc = cscSrc;

        pObj->cscConfig.bypass = FALSE;
        pObj->cscConfig.mode = VPS_CSC_MODE_HDTV_GRAPHICS_R2Y;
        pObj->cscConfig.coeff = NULL;

        pObj->resInfo.cscHalHandle =
            gVcore_vipResCommonObj.cscHandle[vipInstId];
    }

    chrDsId = Vcore_vipResAllocChrDs ( pObj, -1, FALSE );
    if ( chrDsId < 0 )
    {
        // no chroma downsampler is available
        return -1;
    }

    chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_CSC;

    if ( pOutParams->scEnable )
    {
        if ( !isScAlloc )
        {
            status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_SC );

            pObj->resInfo.scHalHandle =
                gVcore_vipResCommonObj.scHandle[vipInstId];

            pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_CSC;
        }

        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SC;
    }

    pObj->muxInfo.chrDsSrc[chrDsId] = chrDsSrc;

    pObj->muxInfo.chrDsBypass[0] = TRUE;
    pObj->muxInfo.rgbOutHiSel = FALSE;

    if ( chrDsId == 0 )
    {
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_UP );

        pObj->vpdmaInfo[outId].vpdmaChId[0]
            = VCORE_VIP_VCH_VIP_LUMA ( vipInstId, 0 );

    }
    else
    {

        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_UP );

        pObj->muxInfo.chrDsBypass[1] = TRUE;
        pObj->muxInfo.rgbOutHiSel = FALSE;

        pObj->vpdmaInfo[outId].vpdmaChId[0]
            = VCORE_VIP_VCH_VIP_CHROMA ( vipInstId, 0 );
    }

    pVpdmaInfo->isMultiCh = FALSE;
    pObj->vpdmaInfo[outId].vpdmaChNum = 1;
    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_YC422;

    return status;
}

Int32 Vcore_vipResAllocSingleChRgb888PathYuv420Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     VpsHal_VpsVipCscMuxSrc
                                                     cscSrc )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId, chrDsId, chrDsSrc;
    UInt32 isCscAlloc, isScAlloc;
    Vcore_VipInSrc inSrc;

    vipInstId = pObj->resParams.vipInstId;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];
    inSrc = pObj->resParams.inSrc;

    if ( pObj->resInfo.cscHalHandle != NULL )
        isCscAlloc = TRUE;
    else
        isCscAlloc = FALSE;

    if ( pObj->resInfo.scHalHandle != NULL )
        isScAlloc = TRUE;
    else
        isScAlloc = FALSE;

    if ( !isCscAlloc )
    {

        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_CSC );

        pObj->muxInfo.cscSrc = cscSrc;

        pObj->cscConfig.bypass = FALSE;
        pObj->cscConfig.mode = VPS_CSC_MODE_HDTV_GRAPHICS_R2Y;
        pObj->cscConfig.coeff = NULL;

        pObj->resInfo.cscHalHandle =
            gVcore_vipResCommonObj.cscHandle[vipInstId];
    }

#if 0
    chrDsId = Vcore_vipResAllocChrDs ( pObj, -1, TRUE );
    if ( chrDsId < 0 )
    {
        // no chroma downsampler is available
        return -1;
    }
#endif
    /* For VIP input and 420 output, perfer to use CHR_DS instance 1. As there
       are cases where in secondary input requires to support 422I output
       which cannot be supported on CHR_DS instance 1. */
    if ( inSrc != VCORE_VIP_IN_SRC_COMP_RGB888_24_BIT )
    {
        chrDsId = Vcore_vipResAllocChrDs ( pObj, 1, TRUE );
        /* If we could not get CHR_DS instance 1, try with intance 0x0 */
        if ( chrDsId < 0 )
        {
            chrDsId = Vcore_vipResAllocChrDs ( pObj, 0, TRUE );
        }
    }
    else
    {
        chrDsId = Vcore_vipResAllocChrDs ( pObj, -1, TRUE );
    }

    if ( chrDsId < 0 )
    {
        // no chroma downsampler is available
        return -1;
    }

    chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_CSC;

    if ( pOutParams->scEnable )
    {
        if ( !isScAlloc )
        {
            status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_SC );

            pObj->resInfo.scHalHandle =
                gVcore_vipResCommonObj.scHandle[vipInstId];

            pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_CSC;
        }

        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SC;
    }

    pObj->muxInfo.chrDsSrc[chrDsId] = chrDsSrc;
    pObj->muxInfo.chrDsBypass[chrDsId] = FALSE;

    if ( chrDsId == 0 )
    {
        pObj->muxInfo.rgbOutHiSel = FALSE;
        pObj->muxInfo.chrDsBypass[1] = FALSE;
    }
    else
    {
        pObj->muxInfo.rgbOutLoSel = FALSE;
        pObj->muxInfo.multiChSel = FALSE;
    }

    pVpdmaInfo->isMultiCh = FALSE;
    pObj->vpdmaInfo[outId].vpdmaChNum = 2;

    pObj->vpdmaInfo[outId].vpdmaChId[0]
        = VCORE_VIP_VCH_VIP_LUMA ( vipInstId, chrDsId );

    pObj->vpdmaInfo[outId].vpdmaChId[1]
        = VCORE_VIP_VCH_VIP_CHROMA ( vipInstId, chrDsId );

    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_Y420;
    pObj->vpdmaInfo[outId].vpdmaChDataType[1] = VPSHAL_VPDMA_CHANDT_C420;

    return status;
}

Int32 Vcore_vipResAllocSingleChRgb888PathRgb888Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId;
    Vcore_VipInSrc inSrc;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];

    if ( pOutParams->scEnable )
    {
        // scaling not supported for RGB path
        return -1;
    }

    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_RGB888_24_BIT )
    {
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_LOW );
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_LOW );

        pObj->muxInfo.rgbOutLoSel = TRUE;
        pObj->muxInfo.multiChSel = TRUE;

        pObj->vpdmaInfo[outId].vpdmaChId[0] =
            VCORE_VIP_VCH_VIP_RGB ( vipInstId, 1 );
    }
    else
    {

        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_UP );
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_UP );
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_COMP_RGB );

        pObj->muxInfo.rgbSrc = VPSHAL_VPS_VIP_RGB_MUX_SRC_COMP;
        pObj->muxInfo.rgbOutHiSel = TRUE;
    }

    pVpdmaInfo->isMultiCh = FALSE;
    pObj->vpdmaInfo[outId].vpdmaChNum = 1;
    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_RGB888;

    return status;
}

Int32 Vcore_vipResAllocSingleChRgb888PathAncOut ( Vcore_VipResObj * pObj,
                                                  UInt16 outId, UInt16 portId )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId;
    Vcore_VipInSrc inSrc;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];

    if ( pOutParams->scEnable )
    {
        // scaling not supported for ancillary path
        return -1;
    }

    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_RGB888_24_BIT )
    {
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A_ANC );
    }
    else
    {
        // anc data not supported for COMP
        return -1;
    }

    pVpdmaInfo->isMultiCh = FALSE;
    pObj->vpdmaInfo[outId].vpdmaChNum = 1;

    pObj->vpdmaInfo[outId].vpdmaChId[0]
        = VCORE_VIP_VCH_VIP_MULT_ANC ( vipInstId, portId, 0 );

    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_ANC;

    return status;
}

Int32 Vcore_vipResAllocSingleChRgb888PathYuv422spOut ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     VpsHal_VpsVipCscMuxSrc
                                                     cscSrc )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId, chrDsId, chrDsSrc;
    UInt32 isCscAlloc, isScAlloc;

    vipInstId = pObj->resParams.vipInstId;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];

    if ( pObj->resInfo.cscHalHandle != NULL )
        isCscAlloc = TRUE;
    else
        isCscAlloc = FALSE;

    if ( pObj->resInfo.scHalHandle != NULL )
        isScAlloc = TRUE;
    else
        isScAlloc = FALSE;

    if ( !isCscAlloc )
    {

        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_CSC );

        pObj->muxInfo.cscSrc = cscSrc;

        pObj->cscConfig.bypass = FALSE;
        pObj->cscConfig.mode = VPS_CSC_MODE_HDTV_GRAPHICS_R2Y;
        pObj->cscConfig.coeff = NULL;

        pObj->resInfo.cscHalHandle =
            gVcore_vipResCommonObj.cscHandle[vipInstId];
    }

    /* We cannot suppport 422SP on instance 0 of CHR_DS due to mux, UV_UP/Y_UP
       mux requires to be set to take input from CHR_DS, which causes mux
       chr_ds_1_bypass to select 422 as input for Y_UP. Hence we are using only
       instance 1 of CHR_DS.
       Require to by-pass down sampling but use Y UV seperation. Which requires
       CHR_DS to be opertated in by-pass mode. */
    chrDsId = Vcore_vipResAllocChrDs ( pObj, 1, TRUE );
    if ( chrDsId < 0 )
    {
        /* required chroma downsampler is not available */
        return -1;
    }

    chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_CSC;

    if ( pOutParams->scEnable )
    {
        if ( !isScAlloc )
        {
            status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_SC );

            pObj->resInfo.scHalHandle =
                gVcore_vipResCommonObj.scHandle[vipInstId];

            pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_CSC;
        }

        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SC;
    }

    pObj->muxInfo.chrDsSrc[chrDsId] = chrDsSrc;
    pObj->muxInfo.chrDsBypass[chrDsId] = TRUE;

    if ( chrDsId == 0 )
    {
        pObj->muxInfo.rgbOutHiSel = FALSE;
        pObj->muxInfo.chrDsBypass[1] = FALSE;
    }
    else
    {
        pObj->muxInfo.rgbOutLoSel = FALSE;
        pObj->muxInfo.multiChSel = FALSE;
    }

    pVpdmaInfo->isMultiCh = FALSE;
    pObj->vpdmaInfo[outId].vpdmaChNum = 2;

    pObj->vpdmaInfo[outId].vpdmaChId[0]
        = VCORE_VIP_VCH_VIP_LUMA ( vipInstId, chrDsId );

    pObj->vpdmaInfo[outId].vpdmaChId[1]
        = VCORE_VIP_VCH_VIP_CHROMA ( vipInstId, chrDsId );

    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_Y422;
    pObj->vpdmaInfo[outId].vpdmaChDataType[1] = VPSHAL_VPDMA_CHANDT_C422;

    return status;
}
