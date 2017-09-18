/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/*
  this file has path allocation logic when input is YUV422 8/16-bit
*/

#include <ti/psp/vps/core/src/vpscore_vipResPriv.h>

/*
  Allocate path with YUV422 as input

  Input could be from Port A, Port B or secondary path
*/
Int32 Vcore_vipResAllocSingleChYuv422Path ( Vcore_VipResObj * pObj )
{
    Int32 status = 0, portId, vipInstId, outId;
    Vcore_VipOutParams *pOutParams;
    Vcore_VipInSrc inSrc;
    Bool swap = FALSE;
    Int32 swapOutId;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    /*
     * allocate input source module
     */
    if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_8_BIT )
    {
        /*
         * for 8-bit allocate one port
         */
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A );

        portId = VPSHAL_VIP_PORT_A;

    }
    else if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_A_YUV422_16_BIT )
    {
        /*
         * for 16-bit allocate both ports
         */
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A );
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_B );

        portId = VPSHAL_VIP_PORT_A;

    }
    else if ( inSrc == VCORE_VIP_IN_SRC_PARSER_PORT_B_YUV422_8_BIT )
    {
        /*
         * for 8-bit allocate one port
         */
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_B );

        portId = VPSHAL_VIP_PORT_B;

    }
    else if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
    {
        status |=
            Vcore_vipResAlloc ( pObj,
                                VCORE_VIP_RES_SEC_422 );

        /*
         * port ID is invalid for secondary path
         */
        portId = -1;

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
         * assign VIP parser handle if valid port ID
         */
        pObj->resInfo.vipHalHandle
            = gVcore_vipResCommonObj.vipHandle[vipInstId][portId];
    }

        /*
     * Check if swap of allocating resource is required
     */
     swap = Vcore_vipResIsSwapReq(&pObj->resParams);

    /*
     * for each output allocate path
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
             * allocate for YUV422 output
             */
            status = Vcore_vipResAllocSingleChYuv422PathYuv422Out
                ( pObj, swapOutId, portId );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_ANC )
        {
            /*
             * allocate for vertical ancillary data output
             */
            status = Vcore_vipResAllocSingleChYuv422PathAncOut
                ( pObj, swapOutId, portId );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV420 )
        {
            /*
             * allocate for vertical YUV420 data output
             */
            status = Vcore_vipResAllocSingleChYuv422PathYuv420Out
                ( pObj, swapOutId );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_RGB888 ||
                  pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV444 )
        {
            /*
             * allocate for vertical RGB888 or YUV444 24-bit data output
             */
            status = Vcore_vipResAllocSingleChYuv422PathRgb888Yuv444Out
                ( pObj, swapOutId );

        }
        else if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV422SP_UV )
        {
            status = Vcore_vipResAllocSingleChYuv422PathYuv422spOut
                ( pObj, swapOutId );
        }
        else
        {
            /*
             * invalid output format
             */
            return -1;
        }

        if ( status != FVID2_SOK )
            return status;
    }

    return status;
}

Int32 Vcore_vipResAllocSingleChYuv422PathYuv422Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId,
                                                     UInt16 portId )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId, chrDsId, chrDsSrc;
    Vcore_VipInSrc inSrc;
    UInt32 isScAlloc;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    if ( pObj->resInfo.scHalHandle != NULL )
        isScAlloc = TRUE;
    else
        isScAlloc = FALSE;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];

    if ( pOutParams->scEnable
         || inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
    {
        // scaling not supported for YUV422 path

        chrDsId = Vcore_vipResAllocChrDs ( pObj, -1, FALSE );
        if ( chrDsId < 0 )
        {
            // no chroma downsampler is available
            return -1;
        }

        if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
        {
            chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SEC;
        }
        else if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
        {
            chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTA;
        }
        else
        {
            chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTB;
        }

        if ( pOutParams->scEnable )
        {
            if ( !isScAlloc )
            {
                status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_SC );

                pObj->resInfo.scHalHandle =
                    gVcore_vipResCommonObj.scHandle[vipInstId];

                if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
                {
                    pObj->muxInfo.scSrc =
                        VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_SEC;
                }
                else if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
                {
                    pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTA;
                }
                else
                {
                    pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTB;
                }
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

    }
    else
    {

        if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
        {
            status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_LOW );
        }
        else
        {
            status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_LOW );
        }

        if (FVID2_SOK == status)
        {
            pObj->muxInfo.rgbOutLoSel = FALSE;
            pObj->muxInfo.multiChSel = TRUE;

            pVpdmaInfo->isMultiCh = FALSE;
            pObj->vpdmaInfo[outId].vpdmaChNum = 1;
            pObj->vpdmaInfo[outId].vpdmaChId[0]
                = VCORE_VIP_VCH_VIP_MULT ( vipInstId, portId, 0 );
            pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_YC422;
        }
        else
        {
            /* Try to allocate Y_UP/UV_UP */
            if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
            {
                chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTA;
            }
            else
            {
                chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTB;
            }

            /* If Y_UP is available, then CHR_DS0 will alse be available */
            status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_Y_UP);
            if (FVID2_SOK == status)
            {
                status = Vcore_vipResAllocChrDs ( pObj, 0, FALSE );

                if (status < 0)
                {
                    return status;
                }
                else
                {

                    pObj->muxInfo.rgbOutHiSel = FALSE;
                    pObj->muxInfo.chrDsBypass[0] = TRUE;

                    pObj->muxInfo.chrDsSrc[0] = chrDsSrc;

                    pObj->vpdmaInfo[outId].vpdmaChNum = 1;
                    pObj->vpdmaInfo[outId].vpdmaChId[0]
                        = VCORE_VIP_VCH_VIP_LUMA ( vipInstId, 0 );
                    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_YC422;
                }
            }
            else
            {
                status = Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_UV_UP);
                if (FVID2_SOK != status)
                    return status;

                status = Vcore_vipResAllocChrDs ( pObj, 0, FALSE );

                if (status < 0)
                {
                    return status;
                }
                else
                {
                    pObj->muxInfo.rgbOutHiSel = FALSE;
                    pObj->muxInfo.chrDsBypass[0] = TRUE;
                    pObj->muxInfo.chrDsBypass[1] = TRUE;

                    pObj->muxInfo.chrDsSrc[1] = chrDsSrc;

                    pObj->vpdmaInfo[outId].vpdmaChNum = 1;
                    pObj->vpdmaInfo[outId].vpdmaChId[0]
                        = VCORE_VIP_VCH_VIP_CHROMA ( vipInstId, 0 );
                    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_YC422;
                }
            }
        }
    }

    return status;
}

Int32 Vcore_vipResAllocSingleChYuv422PathYuv420Out ( Vcore_VipResObj * pObj,
                                                     UInt16 outId )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId, chrDsId, chrDsSrc;
    Vcore_VipInSrc inSrc;
    UInt32 isScAlloc;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];

    if ( pObj->resInfo.scHalHandle != NULL )
        isScAlloc = TRUE;
    else
        isScAlloc = FALSE;

    /* For VIP input and 420 output, perfer to use CHR_DS instance 1. As there
       are cases where in secondary input requires to support 422I output
       which cannot be supported on CHR_DS instance 1. */
    if ( inSrc != VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
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

    if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
    {
        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SEC;
    }
    else if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
    {
        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTA;
    }
    else
    {
        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTB;
    }

    if ( pOutParams->scEnable )
    {
        if ( !isScAlloc )
        {
            status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_SC );

            pObj->resInfo.scHalHandle =
                gVcore_vipResCommonObj.scHandle[vipInstId];

            if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
            {
                pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_SEC;
            }
            else if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
            {
                pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTA;
            }
            else
            {
                pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTB;
            }
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

Int32 Vcore_vipResAllocSingleChYuv422PathRgb888Yuv444Out ( Vcore_VipResObj *
                                                           pObj, UInt16 outId )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId, chrDsId;
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

    pObj->resInfo.cscHalHandle = gVcore_vipResCommonObj.cscHandle[vipInstId];

    if ( pOutParams->format == VCORE_VIP_OUT_FORMAT_YUV444 )
    {
        pObj->cscConfig.bypass = TRUE;
        pObj->cscConfig.mode = VPS_CSC_MODE_NONE;
        pObj->cscConfig.coeff = NULL;
    }
    else
    {
        pObj->cscConfig.bypass = FALSE;
        pObj->cscConfig.mode = VPS_CSC_MODE_HDTV_GRAPHICS_Y2R;
        pObj->cscConfig.coeff = NULL;
    }

    if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
    {
        pObj->muxInfo.cscSrc = VPSHAL_VPS_VIP_CSC_MUX_SRC_SEC;
    }
    else if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
    {
        pObj->muxInfo.cscSrc = VPSHAL_VPS_VIP_CSC_MUX_SRC_VIP_PORTA_422;
    }
    else
    {
        pObj->muxInfo.cscSrc = VPSHAL_VPS_VIP_CSC_MUX_SRC_VIP_PORTB_422;
    }

    status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_CSC );

    chrDsId = Vcore_vipResAllocChrDs ( pObj, -1, TRUE );
    if ( chrDsId < 0 )
        return -1;

    pObj->muxInfo.chrDsSrc[chrDsId] = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_DISABLED;
    pObj->muxInfo.chrDsBypass[chrDsId] = FALSE;

    if ( chrDsId == 1 )
    {

        pObj->muxInfo.rgbOutLoSel = TRUE;
        pObj->muxInfo.multiChSel = FALSE;

    }
    else
    {

        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_COMP_RGB );

        pObj->muxInfo.rgbSrc = VPSHAL_VPS_VIP_RGB_MUX_SRC_CSC;

        pObj->muxInfo.chrDsBypass[1] = FALSE;

        pObj->muxInfo.rgbOutHiSel = TRUE;
    }

    pVpdmaInfo->isMultiCh = FALSE;
    pObj->vpdmaInfo[outId].vpdmaChNum = 1;
    pObj->vpdmaInfo[outId].vpdmaChId[0]
        = VCORE_VIP_VCH_VIP_RGB ( vipInstId, chrDsId );
    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_RGB888;

    return status;
}

Int32 Vcore_vipResAllocSingleChYuv422PathAncOut ( Vcore_VipResObj * pObj,
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

    if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
    {
        // if input source is secondary 422 then output cannot be ancillary data
        return -1;
    }

    if ( pOutParams->scEnable )
    {
        // scaling not supported for ancillary path
        return -1;
    }

    if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
    {
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_A_ANC );
    }
    else
    {
        status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_PARSER_PORT_B_ANC );
    }

    pVpdmaInfo->isMultiCh = FALSE;
    pObj->vpdmaInfo[outId].vpdmaChNum = 1;
    pObj->vpdmaInfo[outId].vpdmaChId[0]
        = VCORE_VIP_VCH_VIP_MULT_ANC ( vipInstId, portId, 0 );
    pObj->vpdmaInfo[outId].vpdmaChDataType[0] = VPSHAL_VPDMA_CHANDT_ANC;

    return status;
}


Int32 Vcore_vipResAllocSingleChYuv422PathYuv422spOut ( Vcore_VipResObj * pObj,
                                                     UInt16 outId )
{
    Vcore_VipOutParams *pOutParams;
    Vcore_VipVpdmaInfo *pVpdmaInfo;
    Int32 status = 0, vipInstId, chrDsId, chrDsSrc;
    Vcore_VipInSrc inSrc;
    UInt32 isScAlloc;

    vipInstId = pObj->resParams.vipInstId;
    inSrc = pObj->resParams.inSrc;

    pOutParams = &pObj->resParams.outParams[outId];
    pVpdmaInfo = &pObj->vpdmaInfo[outId];

    if ( pObj->resInfo.scHalHandle != NULL )
    {
        isScAlloc = TRUE;
    }
    else
    {
        isScAlloc = FALSE;
    }

    /* We cannot suppport 422SP on instance 0 of CHR_DS due to mux, UV_UP/Y_UP
       mux requires to be set to take input from CHR_DS, which causes mux
       chr_ds_1_bypass to select 422 as input for Y_UP. Hence we are using only
       instance 1 of CHR_DS.
       Require to by-pass down sampling but use Y UV seperation. Which requires
       CHR_DS to be opertated in by-pass mode. */
    chrDsId = Vcore_vipResAllocChrDs ( pObj, 1, TRUE );
    if ( chrDsId != 1 )
    {
        /* required chroma downsampler is not available */
        return -1;
    }

    if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
    {
        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SEC;
    }
    else if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
    {
        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTA;
    }
    else
    {
        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_VIP_PORTB;
    }

    if ( pOutParams->scEnable )
    {
        if ( !isScAlloc )
        {
            status |= Vcore_vipResAlloc ( pObj, VCORE_VIP_RES_SC );

            pObj->resInfo.scHalHandle =
                gVcore_vipResCommonObj.scHandle[vipInstId];

            if ( inSrc == VCORE_VIP_IN_SRC_SEC_YUV422_16_BIT )
            {
                pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_SEC;
            }
            else if ( Vcore_vipResIsInSrcPortA ( inSrc ) )
            {
                pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTA;
            }
            else
            {
                pObj->muxInfo.scSrc = VPSHAL_VPS_VIP_SC_MUX_SRC_VIP_PORTB;
            }
        }
        chrDsSrc = VPSHAL_VPS_VIP_CHRDS_MUX_SRC_SC;
    }

    pObj->muxInfo.chrDsSrc[chrDsId] = chrDsSrc;
    pObj->muxInfo.chrDsBypass[chrDsId] = TRUE;

    pObj->muxInfo.rgbOutLoSel = FALSE;
    pObj->muxInfo.multiChSel = FALSE;

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
