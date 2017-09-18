/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2010 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mSlice.c
 *
 *  \brief VPS M2M SubFrame level processing functions used in all m2m drivers
 *           which supports sub-frame level processing .
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mDei.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/drivers/m2m/vps_m2mInt.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSlice.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Scalar vertical filter RAV max init value: used for subframe height calc */
#define VPSMDRV_SC_VERT_RAV_INIT_MAX                    (1024u)

#define VSPMDRV_SC_HORZ_SHIFT                           (24u)

#define VPSMDRV_SUBFRM_SC_MAX_LONG_LONG                 (0xFFFFFFFFFFFFFFFF)



/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsMdrv_deiInit
 *  \brief DEI M2M driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of M2M driver API.
 *
 *  \param numInst      Number of instance objects to be initialized.
 *  \param initPrms     Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */


/**
 * \brief Initializes subframe instance structure of the channel.
 * Called when first subframe of a frame is processed.
 * Initializes buffer offsets and calculates subframe height and scalar
 * phase information based on chosen configuration.
 * Source/target frame height, filter type and dataformat should
 * have been initialised before calling this function which is done at Create time
 * TODO: Also takes care of
 * updating subframe instance based on frame runtime parameters.
 */
void vpsMdrvScSubFrameInitInfo(VpsMdrv_SubFrameChInst *slcData)
{
    slcData->subFrameNum        = 0;
    slcData->srcBufOffset  = 0;
    slcData->tarBufOffset  = 0;
    slcData->rowOffset     = 0;

    /* Since Scalar hardware use multi tap filter, it needs few lines of image from
           previous and next subFrame to make scaled output equal to frame level processing.
           This Line memory varies depending on Scalar type (polyPhase/ RAV) and
           input type (420/422) as CHR_US line memory comes into picture */
    if (slcData->dataFmt == FVID2_DF_YUV420SP_UV)
    {
        if (slcData->filtType == VPS_SC_VST_POLYPHASE)
        {
            slcData->chrUsLnMemSlcTop = 0;
            /*< 0 for first subframe and all other subFrame 2: 2 for chr_us*/
            slcData->chrUsLnMemSlcBtm = 4;
            /*< 0 for Last subframe and all other subFrame 4: 4 for chr_us */
            slcData->scCropLnsTop     = 0;
            /*< 0 for first subframe and all other subFrame 2*/
            slcData->scCropLnsBtm     = 4;
            /*< 0 for Last subframe and all other subFrame 4*/
        }
        else if (slcData->filtType == VPS_SC_VST_RAV)
        {
            slcData->chrUsLnMemSlcTop = 0;
            /* 0 for first subframe and all other subFrame 2+1: 2 for chr_us and +1 for Sc*/
            slcData->chrUsLnMemSlcBtm = 4;
            /*0 for Last subframe and all other subframes 4:0 for Sc  and 4 for chr_us */
            slcData->scCropLnsTop     = 0;
            /*< 0 for first subframe and all other subframes 3 :(2+1)*/
            slcData->scCropLnsBtm     = 4;
            /*< 0 for Last subframe and all other subframes 4*/
        }
    }
    else /* 422 input case */
    {
        slcData->chrUsLnMemSlcTop = 0;
        slcData->chrUsLnMemSlcBtm = 0;
        slcData->scCropLnsTop     = 0;
        slcData->scCropLnsBtm     = 0;
    }

    /*Scalar Phase info calc for PolyPh and RAV filter  */
    /* TODO: assumes progressinv in/out. Need update to support interlaced in/out */
    if (slcData->filtType == VPS_SC_VST_POLYPHASE)
    {
        slcData->rowAccIncr    = ((slcData->srcFrmH - 1u) <<16) /
                                       (slcData->tarFrmH- 1u);;
        slcData->rowAccOffset  = 0;
        slcData->rowAccOffsetB = 0;
        slcData->rowAcc        = 0;
    } else
    if (slcData->filtType == VPS_SC_VST_RAV)
    {
        /*Scalar Phase info calc for RAV filter  */
        slcData->ravMaxSrcHtPerSlc= (slcData->slcSz);
        slcData->ravScFactor   = (UInt16)((slcData->tarFrmH << 10)/
                                    slcData->srcFrmH);
        slcData->ravRowAccInit =  slcData->ravScFactor  +
                                ((1u + slcData->ravScFactor) >> 1u);
        if (slcData->ravRowAccInit >= VPSMDRV_SC_VERT_RAV_INIT_MAX)
        {
            slcData->ravRowAccInit -= VPSMDRV_SC_VERT_RAV_INIT_MAX;
        }
        slcData->ravRowAccInitB = slcData->ravRowAccInit +
                                  (1u + (slcData->ravRowAccInit >> 1u)) -
                                  (VPSMDRV_SC_VERT_RAV_INIT_MAX >> 1u);
        if (slcData->ravRowAccInitB < 0)
        {
            slcData->ravRowAccInitB += slcData->ravRowAccInit;
            slcData->ravRowAccInit += slcData->ravRowAccInit;
        }
    }

}

void vpsMdrvScSubFrameHorzInitInfo(VpsMdrv_SubFrameChInst *slcData)
{
    VpsMDrv_HorzSubFrmCfg  *slcCfg;

    slcCfg = &slcData->hSubFrmInfo.slcCfg[0u];

    slcCfg->colAccOffset = 0;

    /* Calculate the linear Accumulator increament */
    slcData->hSubFrmInfo.linAccIncr =
        ((unsigned long long)slcData->srcFrmW << VSPMDRV_SC_HORZ_SHIFT) /
            slcData->tarFrmW;
    slcData->hSubFrmInfo.colAcc = 0;

    /* Calculate the post processing information */
    if (slcData->dataFmt == FVID2_DF_YUV420SP_UV)
    {
        slcCfg->chrUsPixMemSlcLft = 0;
        /*< 0 for first subframe and all other subFrame 2: 2 for chr_us*/
        slcCfg->chrUsPixMemSlcRgt = 4;
        /*< 0 for Last subframe and all other subFrame 4: 4 for chr_us */
        slcCfg->scCropPixLft     = 0;
        /*< 0 for first subframe and all other subFrame 2*/
        slcCfg->scCropPixRgt     = 4;
        /*< 0 for Last subframe and all other subFrame 4*/
    }
    else
    {
        slcCfg->chrUsPixMemSlcLft = 0;
        slcCfg->chrUsPixMemSlcRgt = 0;
        slcCfg->scCropPixLft      = 0;
        slcCfg->scCropPixRgt      = 0;
    }

    if (FALSE == slcData->hSubFrmInfo.isEnable)
    {
        slcData->hSubFrmInfo.noOfSlcs = 0u;
        VpsUtils_memset(slcCfg,
                        0,
                        sizeof(VpsMDrv_HorzSubFrmCfg));
        slcCfg->srcW = slcData->srcFrmW;
        slcCfg->tarW = slcData->tarFrmW;
    }
}

void vpsMdrvScSubFrameCalcInfo(VpsMdrv_SubFrameChInst *slcData)
{
    if (FALSE == slcData->isEnable)
    {
        slcData->srcH = slcData->slcSz;
        slcData->tarH = slcData->tarFrmH;
    }
    else
    {
        if (slcData->filtType == VPS_SC_VST_POLYPHASE)
        {
            /* source height*/
            slcData->srcH = slcData->slcSz+4;
            if (slcData->subFrameNum == slcData->noOfSlcs-1)
            {
                if (0u != (slcData->srcFrmH % slcData->slcSz))
                {
                    slcData->srcH = (slcData->srcFrmH % slcData->slcSz) + 2u;
                }
                else
                {
                    slcData->srcH = slcData->srcH-2;
                }
            }

            /* target height*/
            if (slcData->subFrameNum ==0)   // the first subframe
                slcData->tarH = (slcData->srcH-4-1+0.5 - slcData->rowAccOffset/65536.0)*
                                (slcData->tarFrmH-1)/(slcData->srcFrmH-1)+1;
            else if (slcData->subFrameNum == slcData->noOfSlcs-1)    // the last subframe
                slcData->tarH = (slcData->srcH-2-1+0.5- slcData->rowAccOffset/65536.0+2)*
                                (slcData->tarFrmH-1)/(slcData->srcFrmH-1)+1;
            else
                slcData->tarH = (slcData->srcH-4-1+0.5- slcData->rowAccOffset/65536.0+2)*
                                (slcData->tarFrmH-1)/(slcData->srcFrmH-1)+1;

         }
         else if(slcData->filtType == VPS_SC_VST_RAV)
         {
            /* Calculate the last RAV filter reset line with in the given max Source height lines
                       Also calculate the target height */
            UInt16 tarRow, srcRow, slcSrcH=0;
            UInt32 rowAcc=slcData->ravRowAccInit;
            UInt32 rowAccEndOfLstSlc=slcData->ravRowAccInit;
            tarRow=0;
            for (srcRow=0; srcRow < slcData->ravMaxSrcHtPerSlc; srcRow++) {
                if (rowAcc>=VPSMDRV_SC_VERT_RAV_INIT_MAX) {
                    rowAcc-=VPSMDRV_SC_VERT_RAV_INIT_MAX;
                    tarRow++;
                    slcSrcH = srcRow;
                    rowAccEndOfLstSlc = rowAcc;
                }
                rowAcc+=slcData->ravScFactor;
            }

            slcData->srcH = slcSrcH + 1;
            slcData->tarH = tarRow;
            slcData->ravRowAccInit = rowAccEndOfLstSlc;
        }
    }
}


/* update post processing subframe data */
void vpsMdrvScSubFrameCalcPostProcInfo(VpsMdrv_SubFrameChInst *slcData)
{
    int i;

    if (slcData->dataFmt == FVID2_DF_YUV420SP_UV)
    {
        if (slcData->filtType == VPS_SC_VST_POLYPHASE)
        {
            slcData->chrUsLnMemSlcTop = 2;
            /*< 0 for first subframe and all other subframes 2: for chr_us*/
            slcData->scCropLnsTop     = 2;
            /*< 0 for first subframe and all other subframes 2*/
            if (slcData->subFrameNum == slcData->noOfSlcs-1)
            {
                slcData->chrUsLnMemSlcBtm = 0;
                /*< 0 for Last subframe and all other subframes 4: 4 for chr_us */
                slcData->scCropLnsBtm     = 0;
                /*< 0 for Last subframe and all other subframes 4*/
            } else
            {
                slcData->chrUsLnMemSlcBtm = 4;
                /*< 0 for Last subframe and all other subframes 4: 4 for chr_us */
                slcData->scCropLnsBtm     = 4;
                /*< 0 for Last subframe and all other subframes 4*/
            }

            for (i = 0; i < slcData->tarH; i++){
                   slcData->rowAcc += slcData->rowAccIncr;   /* phase accumulation*/
            }
            /* at subframe boundary, need to read two more lines for overlapped processing */
            slcData->rowAccOffset = slcData->rowAcc -
                                  ((slcData->srcH-4)*(slcData->subFrameNum+1)<<16)+(2<<16);

            /* update row_offset which is to determine slice_row_offset for each subframe input
                         vertical index.*/
            slcData->rowOffset    += (slcData->srcH-4);
            slcData->tarBufOffset += (slcData->tarH);

            /* source buf offset*/
            slcData->srcBufOffset = (slcData->rowOffset -2 - slcData->chrUsLnMemSlcTop); //-2 for scalar top line memory
        } else
        if (slcData->filtType == VPS_SC_VST_RAV)
        {
            slcData->chrUsLnMemSlcTop = 2+1;
            /* 0 for first subframe and all other subframes 2+2: 2 for Sc and 2 for chr_us*/
            slcData->scCropLnsTop     = 3;
            /*< 0 for first subframe and all other subframes 3 :(2+1)*/

            if (slcData->subFrameNum == slcData->noOfSlcs-1)
            {
                slcData->chrUsLnMemSlcBtm = 0;
                /*0 for Last subframe and all other subframes 4:0 for Sc  and 4 for chr_us */
                slcData->scCropLnsBtm     = 0;
                /*< 0 for Last subframe and all other subframes 4*/
            } else
            {
                slcData->chrUsLnMemSlcBtm = 4;
                /*0 for Last subframe and all other subframes 4:0 for Sc  and 4 for chr_us */
                slcData->scCropLnsBtm     = 4;
                /*< 0 for Last subframe and all other subframes 4*/
            }
            slcData->rowOffset        +=  (slcData->srcH-1); /* one line of memory for RAV from prev subframe */
            slcData->tarBufOffset     +=  slcData->tarH;

            /* max process remaining lines from previous subframe and the lines in next subframe */
            slcData->ravMaxSrcHtPerSlc = (slcData->ravMaxSrcHtPerSlc
                                          - (slcData->srcH-1)
                                          + slcData->slcSz);
            /* Process all remaining lines with Last subframe */
            if ((slcData->srcBufOffset + slcData->ravMaxSrcHtPerSlc) >
                 slcData->srcFrmH)
               slcData->ravMaxSrcHtPerSlc = (slcData->srcFrmH - slcData->srcBufOffset);

            slcData->srcBufOffset    = (slcData->rowOffset - 1
                                        - slcData->scCropLnsTop);
            /* -1 for RAV Ln Mem on top*/
        }
    }else
    {
        slcData->chrUsLnMemSlcTop = 0;
        slcData->chrUsLnMemSlcBtm = 0;
        slcData->scCropLnsTop     = 0;
        slcData->scCropLnsBtm     = 0;

        if (slcData->filtType == VPS_SC_VST_POLYPHASE)
        {

            for (i = 0; i < slcData->tarH; i++){
                   slcData->rowAcc += slcData->rowAccIncr;
            }
            /* at subframe boundary, need to read two more lines for overlapped processing */
            slcData->rowAccOffset = slcData->rowAcc -
                                  ((slcData->srcH-4)*(slcData->subFrameNum+1)<<16)+(2<<16);

            /* update row_offset which is to determine slice_row_offset for each subframe
                          input vertical index.*/
            slcData->rowOffset    += (slcData->srcH-4);
            slcData->tarBufOffset += (slcData->tarH);

            /* source buf offset : at subframe boundary, need to read two more */
            slcData->srcBufOffset = slcData->rowOffset-2;
        }
        else if(slcData->filtType == VPS_SC_VST_RAV)
        {
            slcData->srcBufOffset     +=  (slcData->srcH-1);
            /* one line of memory for RAV from prev subframe */
            slcData->tarBufOffset     +=  slcData->tarH;

            /* max process remaining lines from previous subframe and the lines in next subframe */
            slcData->ravMaxSrcHtPerSlc = (slcData->ravMaxSrcHtPerSlc -
                                         (slcData->srcH-1)+
                                          slcData->slcSz);
            /* Process all remaining lines with Last subframe */
            if ((slcData->srcBufOffset + slcData->ravMaxSrcHtPerSlc) >
                 slcData->srcFrmH)
              slcData->ravMaxSrcHtPerSlc = (slcData->srcFrmH - slcData->srcBufOffset);
         }
    }

}

void printHsubFrameInfo(VpsMdrv_HorzSubFrmInfo *slcInfo)
{
	VpsMDrv_HorzSubFrmCfg *slcCfg;
	UInt32 index;

	GT_0trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "MP Scaling - Calculated slice configurations are...\n");

	GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Slice size - slcSz = %d\n", slcInfo->slcSz);
	GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Number of slices - noOfSlcs = %d\n", slcInfo->noOfSlcs);
	GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Linear Accumulators value - linAccIncr = %ld\n", slcInfo->linAccIncr);
	GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Column Accumulators value - colAcc = %ld\n", slcInfo->colAcc);

	for (index = 0; index < slcInfo->noOfSlcs; index++)
	{
		slcCfg = &slcInfo->slcCfg[index];
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Source Slice width - srcW = %d", slcCfg->srcW);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Source First Slice width - fstSrcW = %d", slcCfg->fstSrcW);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Target Slice width - tarW = %d", slcCfg->tarW);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Target First Slice width - fstTarW = %d", slcCfg->fstTarW);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Source buffer offset - srcBufOffset = %d", slcCfg->srcBufOffset);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Target buffer offset - tarBufOffset = %d", slcCfg->tarBufOffset);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Left Num pixels extra for chr us - chrUsPixMemSlcLft = %d", slcCfg->chrUsPixMemSlcLft);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Right Num pixels extra for chr us - chrUsPixMemSlcRgt = %d", slcCfg->chrUsPixMemSlcRgt);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Left SC Crop - scCropPixLft = %d", slcCfg->scCropPixLft);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Right SC Crop - scCropPixRgt = %d", slcCfg->scCropPixRgt);
		GT_1trace((GT_TraceState_Enable | GT_DEBUG), GT_DEBUG, "Column offset for this slice - colAccOffset = %ld", slcCfg->colAccOffset);
	}
}


Int32 vpsMdrvScSubFrameCalcHorzInfo(VpsMdrv_SubFrameChInst *slcData)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  subFrmCnt = 0u, colCnt, fstIter = 0;
    UInt32                  startPix, endPix;
    VpsMDrv_HorzSubFrmCfg  *slcCfg;
    VpsMdrv_HorzSubFrmInfo *slcInfo;
    unsigned long long tempColAcc, orgColAcc;

    /* Calculate the subframe information for the 0th subframe,
       many of this information will be reused for all other subframes */
    slcInfo = &slcData->hSubFrmInfo;
    slcCfg = &slcInfo->slcCfg[0u];

    /* Initialized Variables */
    slcCfg->colAccOffset      = 0u;
    slcCfg->chrUsPixMemSlcLft = 0u;
    slcCfg->chrUsPixMemSlcRgt = 0u;
    slcCfg->scCropPixLft      = 0u;
    slcCfg->scCropPixRgt      = 0u;
    slcCfg->srcBufOffset      = 0u;

    /* Initialize horizontal Sub Frame information,
       this will initialize all variables to 0 and
       calculate linear accumulator increment value */
    vpsMdrvScSubFrameHorzInitInfo(slcData);

    /* Initialize column accumulator value to 0 */
    slcInfo->colAcc = 0;
    for (subFrmCnt = 0u; subFrmCnt < slcInfo->noOfSlcs; subFrmCnt ++)
    {
        slcCfg = &slcInfo->slcCfg[subFrmCnt];

        /* Initialized Variables */
        slcCfg->colAccOffset      = 0u;
        slcCfg->chrUsPixMemSlcLft = 0u;
        slcCfg->chrUsPixMemSlcRgt = 0u;
        slcCfg->scCropPixLft      = 0u;
        slcCfg->scCropPixRgt      = 0u;
        slcCfg->srcBufOffset      = 0u;
        slcCfg->tarBufOffset      = 0u;

        if (slcData->dataFmt == FVID2_DF_YUV420SP_UV)
        {
            if (0u == subFrmCnt)
            {
                slcCfg->chrUsPixMemSlcLft = 0;
                /*< 0 for first subframe and all other subFrame 2: 2 for chr_us*/
                slcCfg->chrUsPixMemSlcRgt = 4;
                /*< 0 for Last subframe and all other subFrame 4: 4 for chr_us */
                slcCfg->scCropPixLft     = 0;
                /*< 0 for first subframe and all other subFrame 2*/
                slcCfg->scCropPixRgt     = 4;
                /*< 0 for Last subframe and all other subFrame 4*/
            }
            else if ((slcInfo->noOfSlcs - 1u) == subFrmCnt)
            {
                slcCfg->chrUsPixMemSlcLft = 2;
                /*< 0 for first subframe and all other subframes 2: for chr_us*/
                slcCfg->scCropPixLft     = 2;
                /*< 0 for first subframe and all other subframes 2*/
                slcCfg->chrUsPixMemSlcRgt = 0;
                /*< 0 for Last subframe and all other subframes 4: 4 for chr_us */
                slcCfg->scCropPixRgt     = 0;
                /*< 0 for Last subframe and all other subframes 4*/
            }
            else
            {
                slcCfg->chrUsPixMemSlcLft = 2;
                /*< 0 for first subframe and all other subframes 2: for chr_us*/
                slcCfg->scCropPixLft     = 2;
                /*< 0 for first subframe and all other subframes 2*/
                slcCfg->chrUsPixMemSlcRgt = 4;
                /*< 0 for Last subframe and all other subframes 4: 4 for chr_us */
                slcCfg->scCropPixRgt     = 4;
                /*< 0 for Last subframe and all other subframes 4*/
            }
        }

        slcCfg->tarW = slcInfo->slcSz;

        if ((slcData->tarFrmW % slcInfo->slcSz) &&
            (subFrmCnt == (slcInfo->noOfSlcs - 1)))
        {
            slcCfg->tarW = slcData->tarFrmW % slcInfo->slcSz;
        }

        fstIter = 0;
        do
        {
            tempColAcc = slcInfo->colAcc;
            for (colCnt = 0; colCnt < slcCfg->tarW; colCnt ++)
            {
                tempColAcc += slcInfo->linAccIncr;

                if (colCnt > 0)
                {
                    continue;
                }

                slcCfg->colAccOffset = tempColAcc & ((1 << 24) - 1);
                startPix = (tempColAcc &
                            (VPSMDRV_SUBFRM_SC_MAX_LONG_LONG << 24) ) >> 24;

                if (startPix >= 3)
                {
                    startPix = startPix - 3;
                }
                else
                {
                    startPix = 0;
                }
            }

            endPix = (tempColAcc &
                        (VPSMDRV_SUBFRM_SC_MAX_LONG_LONG << 24)) >> 24;

            if (endPix < (slcData->srcFrmW - 1))
            {
                endPix += 1;
            }
            else
            {
                endPix = slcData->srcFrmW - 1;
            }

            /*if (endPix < (slcData->srcFrmW - 1))
            {
                endPix += 1;
            }*/

            slcCfg->srcW = endPix - startPix + 1;

            if (0 == fstIter)
            {
                /* Store the Column accumulator of the first iteration.
                   There could be multiple iterations of this loop to make
                   source slice size to be even. But for the next slice size,
                   this original accumulator value will be used. */
                orgColAcc = tempColAcc;
            }

            if ((subFrmCnt == (slcInfo->noOfSlcs - 1)) &&
                (0 != (slcCfg->srcW & 0x1)))
            {
                startPix = startPix & (~0x1);
                slcCfg->srcW = slcCfg->srcW & (~0x1);
                break;
            }

            if (0 == (slcCfg->srcW & 0x1))
            {
                break;
            }
            else
            {
                slcCfg->tarW += 2;
                fstIter ++;
            }

            if (slcCfg->tarW >=
               (slcData->tarFrmW - slcInfo->slcSz * subFrmCnt))
            {
                retVal = FVID2_EINVALID_PARAMS;
                break;
            }

        } while (1u);

        slcInfo->colAcc = orgColAcc;

        if (subFrmCnt > 0)
        {
            slcCfg->tarBufOffset = slcInfo->slcSz * subFrmCnt;
            slcCfg->srcBufOffset = startPix;
        }

        if (slcCfg->srcBufOffset & 0x1)
        {
            slcCfg->srcBufOffset &= (~0x1u);
        }

        if (slcCfg->srcBufOffset & 0xF)
        {
            slcCfg->scCropPixLft = slcCfg->srcBufOffset & 0xF;
            slcCfg->srcBufOffset -= slcCfg->scCropPixLft;
        }
    }

/* 	printHsubFrameInfo(slcInfo); */

    return (retVal);
}



