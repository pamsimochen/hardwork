/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_grpxPriv.c
 *
 *  \brief VPS graphics Core Private file.
 *  This file implements the core layer for graphics paths
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_grpx.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_graphics.h>
#include <ti/psp/vps/core/src/vpscore_grpxPriv.h>
/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/** Graphics path objects */
static Vcore_GrpxInstObj      GrpxInstObjects[VCORE_GRPX_NUM_INST];

/**
 *  \brief Memory pool for the sub-context objects.
 *  This memory is allocated statically and is fixed for each of the
 *  sub-context objects.
 */
static Vcore_GrpxChObj      GrpxChannelMemPool[VCORE_GRPX_MAX_CHANNEL_OBJECTS];

/**
 *  \brief The flag variable represents whether a given context object is
 *  allocated or not
 */
static UInt32                GrpxChannelMemFlag[VCORE_GRPX_MAX_CHANNEL_OBJECTS];

/** \brief Pool parameters for GRPX channel pool memory */
static VpsUtils_PoolParams  GrpxChanPoolParams;

/** Semaphore for protecting allocation and freeing of memory pool objects */
static Semaphore_Handle      GrpxPoolSem;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  vcoreGrpxPrivInit
 *  Initializes GRPX core objects, allocates memory etc.
 *  Returns VPS_SOK on success else returns error value.
 */

Int vcoreGrpxPrivInit(UInt32 numInst, const Vcore_GrpxInitParams *initParams)
{

    Int32                    instCnt;
    UInt32                   event;
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxInstObj        *instObj;
    Semaphore_Params         semParams;

    /* error sanity check*/
    GT_assert(GrpxCoreTrace, (numInst <= VCORE_GRPX_NUM_INST));
    GT_assert(GrpxCoreTrace, (NULL != initParams));


    /*init channel pool*/
    VpsUtils_initPool(&GrpxChanPoolParams,
                      (Void *)GrpxChannelMemPool,
                      VCORE_GRPX_MAX_CHANNEL_OBJECTS,
                      sizeof(Vcore_GrpxChObj),
                      GrpxChannelMemFlag,
                      GrpxCoreTrace);

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    GrpxPoolSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == GrpxPoolSem)
    {
        GT_0trace(GrpxCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        for (instCnt = 0u; instCnt < numInst; instCnt++)
        {
            GT_assert(GrpxCoreTrace,
                (initParams[instCnt].maxHandle <= VCORE_GRPX_MAX_HANDLES));
            /* Initialize instance object members */
            instObj = &GrpxInstObjects[instCnt];
            instObj->instId = initParams[instCnt].instId;
            instObj->chGrpx = initParams[instCnt].chGrpx;
            instObj->openCnt = 0;
            /*stenciling channel and CLUT channel are in according with
             * graphics channel*/
            instObj->chSten = initParams[instCnt].chSten;
            instObj->chClut = initParams[instCnt].chClut;
            /*VPDMA path*/
            instObj->ptGrpx = initParams[instCnt].ptGrpx;
            instObj->ptSten = initParams[instCnt].ptSten;
            instObj->curMode = VCORE_OPMODE_INVALID;
            instObj->grpxHandle = initParams[instCnt].grpxHandle;
            instObj->grpxReset = FALSE;
            instObj->numDescPerReg = VCORE_GRPX_DESCS_PER_REGION;
            instObj->numHandle = initParams[instCnt].maxHandle;
            /* init instance Handle object pool */
            VpsUtils_initPool(&instObj->hObjsPoolParams,
                              (Void *)instObj->hObjs,
                              VCORE_GRPX_MAX_HANDLES,
                              sizeof(Vcore_GrpxHandleObj),
                              instObj->hObjsFlag,
                              GrpxCoreTrace);
            /*set the descriptor desctination*/
            if (VCORE_GRPX_INST_0 == instObj->instId)
            {
                instObj->configDest = VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX0;
            }
            else if (VCORE_GRPX_INST_1 == instObj->instId)
            {
                instObj->configDest = VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX1;
            }
            else
            {
                instObj->configDest = VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX2;
            }

            event = initParams[instCnt].event;
            instObj->demHandle =
                        Vem_register(VEM_EG_ERROR,
                                     &event,
                                     1u,
                                     VEM_PRIORITY0,
                                     vcoreGrpxErrCallback,
                                     (Ptr)instObj);
            if (NULL == instObj->demHandle)
            {
                GT_1trace(GrpxCoreTrace, GT_ERR,
                        "Event Register failed for %d!! \n",instCnt);
                retVal = VPS_EALLOC;
                break;
            }
        }
    }
    return (retVal);
}

/**
 *  vcoreGrpxPrivDeinit
 *  Deallocates memory allocated by init function.
 *  Returns VPS_SOK on success else returns error value.
 */

Int vcoreGrpxPrivDeinit(void)
{
    UInt32                   instCnt;
    Vcore_GrpxInstObj        *instObj;

    /* Free-up channel objects for each instance */
    for (instCnt = 0u; instCnt < VCORE_GRPX_NUM_INST; instCnt++)
    {
        instObj = &GrpxInstObjects[instCnt];
        instObj->instId = VCORE_GRPX_NUM_INST;
        instObj->chGrpx= VPSHAL_VPDMA_CHANNEL_INVALID;
        instObj->chSten = VPSHAL_VPDMA_CHANNEL_INVALID;
        instObj->chClut = VPSHAL_VPDMA_CHANNEL_INVALID;
        instObj->curMode = VCORE_OPMODE_INVALID;
        instObj->grpxHandle = NULL;
        instObj->numHandle = 0u;

        if (instObj->demHandle)
        {
            Vem_unRegister(instObj->demHandle);
            instObj->demHandle = NULL;
        }
    }

    /* Delete the Semaphore created for Pool objects */
    if (GrpxPoolSem)
    {
        Semaphore_delete(&GrpxPoolSem);
        GrpxPoolSem = NULL;
    }

    return (VPS_SOK);
}
/**
 *  vcoreGrpxAllocHandleObj
 *  Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
Vcore_GrpxHandleObj *vcoreGrpxAllocHandleObj(UInt32 instId,
                                                    Vcore_OpMode mode)
{
    UInt32                   instCnt;
    Vcore_GrpxInstObj        *instObj = NULL;
    Vcore_GrpxHandleObj      *hObj = NULL;

    /* Find out the instance to which this handle belongs to. */
    for (instCnt = 0u; instCnt < VCORE_GRPX_NUM_INST; instCnt++)
    {
        if (GrpxInstObjects[instCnt].instId == instId)
        {
            instObj = &GrpxInstObjects[instCnt];
            break;
        }
    }

    Semaphore_pend(GrpxPoolSem, VCORE_GRPX_SEM_TIMEOUT);

    if (NULL != instObj)
    {
        /* Check if the required mode is proper */
        if (VCORE_OPMODE_INVALID > instObj->curMode)
        {
            /* Core already opend for some mode. For display mode, only one
               handle should be allowed. It doesn't make sense isn't it? */
            if ((VCORE_OPMODE_DISPLAY == instObj->curMode)
                    || ((VCORE_OPMODE_MEMORY == instObj->curMode)
                        && (VCORE_OPMODE_MEMORY != mode)))
            {
                /* Already opened mode and requested mode are not matching */
                GT_0trace(GrpxCoreTrace, GT_ERR,
                     "Already opened mode and requested mode not matching!!\n");
                instObj = NULL;
            }
        }
    }


    if (NULL != instObj)
    {
        /* Use a free channel object */
        hObj = VpsUtils_alloc(&instObj->hObjsPoolParams,
                            sizeof(Vcore_GrpxHandleObj),
                            GrpxCoreTrace);
    }

    if (NULL != hObj)
    {
        /* Update the state and return the handle */
        hObj->instObj = instObj;
        instObj->openCnt++;
        instObj->curMode = mode;
    }

    Semaphore_post(GrpxPoolSem);

    return (hObj);
}




/**
 *  vcoreGrpxFreeHandleObj
 *  Frees-up the handle object and resets the variables.
 *  return 0 on success else return error values
 */
Int32 vcoreGrpxFreeHandleObj(Vcore_GrpxHandleObj *hObj)
{
    Int32                    retVal = VPS_EFAIL;
    Vcore_GrpxInstObj        *instObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    Semaphore_pend(GrpxPoolSem, VCORE_GRPX_SEM_TIMEOUT);

    /* Free the handle object */
    retVal = VpsUtils_free(&instObj->hObjsPoolParams,
                  (const Void *)hObj,
                  GrpxCoreTrace);
    /*init the handle object*/
    if (FVID2_SOK == retVal)
    {
        hObj->instObj = NULL;
        hObj->perChCfg = FALSE;
        hObj->numCh = 0u;

        vcoreGrpxSetDefaultCh(&hObj->handleContext, 0u);
        /* If last handle of the instance, then set mode to invalid */
        instObj->openCnt--;
        if (0u == instObj->openCnt)
        {
            instObj->curMode = VCORE_OPMODE_INVALID;
        }
    }
    else
    {
        GT_0trace(GrpxCoreTrace, GT_ERR,
            "failed to free handle object.\n");
    }

    Semaphore_post(GrpxPoolSem);

    return (retVal);
}


/**
 *  voreGrpxAllocChObj
 *  Allocates channel objects.
 *  Returns error if allocation failed.
 */
Int32 voreGrpxAllocChObj(Vcore_GrpxChObj **chObjs,
                         UInt32 numCh)
{
    UInt32                   chCnt;
    Int32                    retVal = VPS_SOK;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != chObjs));

    Semaphore_pend(GrpxPoolSem, VCORE_GRPX_SEM_TIMEOUT);

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Allocate channel objects one at a time */
        chObjs[chCnt] = (Vcore_GrpxChObj *)VpsUtils_alloc(
                                        &GrpxChanPoolParams,
                                        sizeof(Vcore_GrpxChObj),
                                        GrpxCoreTrace);
        if (NULL == chObjs[chCnt])
        {
            GT_1trace(GrpxCoreTrace, GT_ERR,
                      "Channel Object memory alloc failed for context %d!!\n",
                      chCnt);
            retVal = VPS_EALLOC;
            break;
        }

        /* Set default values for the channel object */
        vcoreGrpxSetDefaultCh(chObjs[chCnt], chCnt);
    }

    Semaphore_post(GrpxPoolSem);
    /* If error, free-up the already allocated objects */
    if (VPS_SOK != retVal)
    {
        vcoreGrpxFreeChObj(chObjs, chCnt);
    }
    return (retVal);
}


/**
 *  vcoreGrpxFreeChObj
 *  Frees-up the channel objects.
 *  return VPS_SOK on success else return error value
 */
Int32 vcoreGrpxFreeChObj(Vcore_GrpxChObj **chObjs,
                                UInt32 numCh)
{
    UInt32                   chCnt;
    Int32                    retVal = VPS_SOK;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != chObjs));
    Semaphore_pend(GrpxPoolSem, VCORE_GRPX_SEM_TIMEOUT);

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Memset the context object before freeing */
        VpsUtils_memset(chObjs[chCnt], 0u, sizeof(Vcore_GrpxChObj));
        retVal += VpsUtils_free(&GrpxChanPoolParams,
                                chObjs[chCnt],
                                GrpxCoreTrace);
        chObjs[chCnt] = NULL;
    }
    Semaphore_post(GrpxPoolSem);
    return (retVal);
}

/**
 *  vcoreGrpxCheckMultRegFormat
 *  Check multiple region format
 *  return VPS_SOK on success else return error value
 */

Int32 vcoreGrpxCheckMultRegFormat(const Vcore_GrpxHandleObj *hObj,
                                  const Vps_MultiWinParams *mReg,
                                  Vcore_Format *coreFmt)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxInstObj       *instObj;

    GT_assert(GrpxCoreTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    if ( VCORE_GRPX_MAX_REGIONS < mReg->numWindows)
    {
        GT_1trace(GrpxCoreTrace, GT_ERR,
                "Number of Regions more than what could be supported (%d)!!.\n"
                ,VCORE_GRPX_MAX_REGIONS);
        retVal = VPS_EOUT_OF_RANGE;
    }

    return (retVal);
}

/**
 * vcoreGrpxCheckFormat
 * check for the format
 * return VPS_SOK on success else return error values
 */
Int32 vcoreGrpxCheckFormat(const Vcore_GrpxHandleObj *hObj,
                           const Vcore_Format * coreFmt)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxInstObj        *instObj;

    GT_assert(GrpxCoreTrace, (NULL != hObj));
    GT_assert(GrpxCoreTrace, (NULL != coreFmt));


    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    if (vcoreGrpxCheckDataFormat(coreFmt->fmt.dataFormat))
    {
        GT_1trace(GrpxCoreTrace, GT_ERR,
                "Data format (%d) not supported!!\n",
                coreFmt->fmt.dataFormat);
        retVal = VPS_EINVALID_PARAMS;
    }

    if ((coreFmt->fmt.pitch[FVID2_RGB_ADDR_IDX] & 0xF) != 0u)
    {
        GT_1trace(GrpxCoreTrace,GT_ERR,
            "grpx data pitch(%d) is not on OCP word boundry.\n",
               coreFmt->fmt.pitch[FVID2_RGB_ADDR_IDX]);
        retVal = VPS_EINVALID_PARAMS;
    }

    if (coreFmt->fmt.pitch[FVID2_RGB_ADDR_IDX]< (coreFmt->fmt.width *
                vcoreGrpxGetNumBitsPerPixel(coreFmt->fmt.dataFormat) >> 3u))
    {
        GT_2trace(GrpxCoreTrace, GT_ERR,
                "Pitch (%d) less than Width (%d) in bytes!!\n",
                coreFmt->fmt.pitch[FVID2_RGB_ADDR_IDX],
                coreFmt->fmt.width );
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check if the scan format are matching Graphics progressive input can
     * used on both progressive and interlaced VENC output. but Graphics
     * interlaced input can only used on the interlaced VENC output.
    */
    if ((FVID2_SF_INTERLACED== coreFmt->fmt.scanFormat) &&
        (FVID2_SF_PROGRESSIVE== coreFmt->secScanFmt))
    {
        GT_0trace(GrpxCoreTrace, GT_ERR,
                "Interlaced Source does not provide progressive Output!!\n");

        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        retVal = VpsHal_vpdmaIsValidSize(
                        instObj->ptGrpx,
                        coreFmt->fmt.dataFormat,
                        (VpsHal_VpdmaMemoryType)coreFmt->memType,
                        coreFmt->fmt.width,
                        coreFmt->fmt.height);

        if (FVID2_SOK != retVal)
        {
            GT_0trace(GrpxCoreTrace, GT_ERR,
                "Size over VPMDA line size\n");
        }
    }
    return (retVal);
}

/**
 * vcoreGrpxCheckPitchDataFormat
 * check for the region pitch and data format
 * return VPS_SOK on success else return error values
 */

Int32 vcoreGrpxCheckPitchDataFormat(const Vcore_GrpxHandleObj *hObj,
                                    Vcore_GrpxChObj *chObj,
                                    Vps_GrpxRtParams *rtParams)
{

    Vps_GrpxRegionParams     *regParams;
    Vcore_GrpxInstObj        *instObj;

    GT_assert(GrpxCoreTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    regParams = &rtParams->regParams;

    /*region dimension check*/
    if ((regParams->regionWidth == 0) || (regParams->regionHeight == 0))
    {
        GT_0trace(GrpxCoreTrace, GT_ERR,
            "region dimension can not be zero.\n");
        return (VPS_EINVALID_PARAMS);
    }

    if (vcoreGrpxCheckDataFormat(rtParams->format))
    {
        GT_1trace(GrpxCoreTrace, GT_ERR,
                "Data format (%d) not supported!!\n",
                rtParams->format);
        return (VPS_EINVALID_PARAMS);
    }

    /*data pitch check*/
    if ((rtParams->pitch[FVID2_RGB_ADDR_IDX] & 0xF) != 0u)
    {
        GT_1trace(GrpxCoreTrace,GT_ERR,
            "grpx data pitch(%d) is not on OCP word boundry.\n",
               rtParams->pitch[FVID2_RGB_ADDR_IDX]);
        return (VPS_EINVALID_PARAMS);

    }

    if (rtParams->pitch[FVID2_RGB_ADDR_IDX] < (regParams->regionWidth *
                vcoreGrpxGetNumBitsPerPixel(rtParams->format) >> 3u))
    {
        GT_2trace(GrpxCoreTrace, GT_ERR,
                "Data Pitch (%d) less than Width (%d) in bytes!!\n",
                rtParams->pitch[FVID2_RGB_ADDR_IDX],
                regParams->regionWidth);
        return (VPS_EINVALID_PARAMS);
    }
    /*stenciling pitch check*/
    if (regParams->stencilingEnable == TRUE)
    {
        if ((rtParams->stenPitch & 0xF) != 0u)
        {
            GT_1trace(GrpxCoreTrace,GT_ERR,
                "stenciling pitch(%d) is not on OCP word boundry.\n",
                   rtParams->stenPitch);
            return (VPS_EINVALID_PARAMS);
        }

        if (rtParams->stenPitch < (regParams->regionWidth >> 3u))
        {
            GT_2trace(GrpxCoreTrace, GT_ERR,
                "Stenciling pitch (%d) less than width(%d) in bytes!!\n",
                rtParams->stenPitch,
                regParams->regionWidth);
            return (VPS_EINVALID_PARAMS);
        }
    }

    if ((TRUE == regParams->scEnable) &&
        (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat))
    {
        GT_0trace(GrpxCoreTrace, GT_ERR,
                "Scaling is not support on the interlaced input!!\n");
        return (VPS_EINVALID_PARAMS);

    }

    if (VpsHal_vpdmaIsValidSize(
                instObj->ptGrpx,
                rtParams->format,
                (VpsHal_VpdmaMemoryType)chObj->coreFmt.memType,
                regParams->regionWidth,
                regParams->regionHeight))
    {
        GT_0trace(GrpxCoreTrace, GT_ERR,
            "region size over VPDMA line size\n");
        return (VPS_EINVALID_PARAMS);
    }
    return (VPS_SOK);
}
/**
 *  vcoreGrpxSetDefaultCh
 *  Init the context
 */
Void vcoreGrpxSetDefaultCh(Vcore_GrpxChObj *chObj,
                           UInt32 chNum)
{
    GT_assert(GrpxCoreTrace, (NULL != chObj));

    /*init state*/
    chObj->state.isFormatSet = FALSE;
    chObj->state.isParamsSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    /*init corefmt*/
    chObj->coreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.frameHeight = VCORE_GRPX_DEFAULT_HEIGHT;
    chObj->coreFmt.frameWidth = VCORE_GRPX_DEFAULT_WIDTH;
    chObj->coreFmt.startX = 0u;
    chObj->coreFmt.startY = 0u;
    chObj->coreFmt.memType =
        (UInt32) VPSHAL_VPDMA_MT_NONTILEDMEM;
    /*init FVID2 format*/
    chObj->coreFmt.fmt.channelNum = chNum;
    chObj->coreFmt.fmt.width = VCORE_GRPX_DEFAULT_WIDTH;
    chObj->coreFmt.fmt.height = VCORE_GRPX_DEFAULT_HEIGHT;
    chObj->coreFmt.fmt.dataFormat = FVID2_DF_ARGB32_8888;
    chObj->coreFmt.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.fmt.bpp = FVID2_BPP_BITS32;
    chObj->coreFmt.fmt.fieldMerged[FVID2_RGB_ADDR_IDX] = FALSE;
    chObj->coreFmt.fmt.pitch[FVID2_RGB_ADDR_IDX] =
        VCORE_GRPX_DEFAULT_WIDTH *
        vcoreGrpxGetNumBitsPerPixel(chObj->coreFmt.fmt.dataFormat) >> 3;
    chObj->coreFmt.fmt.reserved = NULL;

    /*init descinfo*/
    chObj->descInfo.coeffConfigDest = VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX0;
    chObj->descInfo.horzCoeffOvlySize = 0u;
    chObj->descInfo.vertCoeffOvlySize = 0u;
    chObj->descInfo.vertBilinearCoeffOvlySize = 0u;
    chObj->descInfo.nonShadowOvlySize = 0u;
    chObj->descInfo.numInDataDesc = 0u;
    chObj->descInfo.numMultiWinDataDesc= 0u;
    chObj->descInfo.numOutDataDesc = 0u;
    chObj->descInfo.shadowOvlySize = 0u;

    /* flag clut and sten data*/
    chObj->clutData = NULL;
    chObj->stenData = NULL;

    VpsUtils_memset(&chObj->scParams, 0u, sizeof(chObj->scParams));
    VpsUtils_memset(&chObj->regParam, 0u, sizeof(chObj->regParam));

    /*set the some default value*/
    chObj->scParams.inHeight =
        chObj->scParams.outHeight =
            chObj->regParam.regionHeight =
                VCORE_GRPX_DEFAULT_HEIGHT;

    chObj->scParams.inWidth =
        chObj->scParams.outWidth =
                chObj->regParam.regionWidth =
                VCORE_GRPX_DEFAULT_WIDTH;

    chObj->regParam.firstRegion = chObj->regParam.lastRegion = TRUE;

    vcoreGrpxSetDefaultMuliRegInfo(&chObj->mInfo);
    return;

}



/**
 *  deiSetDefaultMosaicInfo
 *  Resets the channel information with default values.
 */
void vcoreGrpxSetDefaultMuliRegInfo(Vcore_GrpxMulRegionsInfo *mInfo)
{
    UInt32                   cnt, pitchCnt;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != mInfo));

    /* Initialize mosaic info structure */
    mInfo->numRegions = 0u;

    /* Initialize frame format structure */
    for (cnt = 0u; cnt < VCORE_GRPX_MAX_REGIONS; cnt++)
    {
        mInfo->regFmt[cnt].winStartX = 0u;
        mInfo->regFmt[cnt].winStartY = 0u;
        mInfo->regFmt[cnt].winWidth = 0u;
        mInfo->regFmt[cnt].winHeight = 0u;
        for (pitchCnt = 0u; pitchCnt < FVID2_MAX_PLANES; pitchCnt++)
        {
            mInfo->regFmt[cnt].pitch[pitchCnt] = 0u;
        }
        mInfo->regFmt[cnt].dataFormat = FVID2_DF_ARGB32_8888;
        mInfo->regFmt[cnt].bpp = FVID2_BPP_BITS32;

        mInfo->stenData[cnt] = NULL;
    }
    /* initialize the variable to zero*/
    VpsUtils_memset(mInfo->regParam,
            0u,
            sizeof(Vps_GrpxRegionParams) * VCORE_GRPX_MAX_REGIONS);

    VpsUtils_memset(mInfo->scParams,
            0u,
            sizeof(Vps_GrpxScParams) * VCORE_GRPX_MAX_REGIONS);
    return;
}

/**
 * vcoreGrpxErrCallback
 *  Error callback to get the error status
 */
void vcoreGrpxErrCallback(const UInt32 *event,
                          UInt32 numEvents,
                          Ptr arg)
{
    Vcore_GrpxInstObj        *instObj;
    VpsHal_GrpxIntStatus     grpxStatus;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != arg));

    instObj = (Vcore_GrpxInstObj *)arg;
    instObj->grpxReset = TRUE;
    GT_1trace(GrpxCoreTrace, GT_DEBUG, "GRPX%d got error ISR: \n",
                    instObj->instId);
    if (NULL != instObj->grpxHandle)
    {
        VpsHal_grpxGetStatus(instObj->grpxHandle,&grpxStatus);
        GT_4trace(GrpxCoreTrace, GT_ERR,"flush %d, cfgErr %d, eol %d, eof %d\n",
            grpxStatus.vpiDataFlushStatus, grpxStatus.frameCfgErrorInt,
            grpxStatus.eolMissingInt, grpxStatus.eofMissingInt);
        GT_4trace(GrpxCoreTrace, GT_ERR,"faultyPos %d, regID %d, w(%d)xh(%d)\n",
            grpxStatus.faultyPosInt,
            grpxStatus.faultyRegId, grpxStatus.hPosition, grpxStatus.vPosition);
    }
    /* TO DO how to pass this to driver, driver need issue a new reset case
     * then to A8*/


}

/**
 *  vcoreGrpxGetChannelObj
 *  Returns the channel object pointer depending on per channel configure is
 *  enabled or not.
 *  return VPS_SOK on success else return error
 */
Vcore_GrpxChObj *vcoreGrpxGetChannelObj(Vcore_GrpxHandleObj *hObj,
                                        UInt32 chNum)
{
    Vcore_GrpxChObj          *chObj;

    /* pointer sanity check */
    GT_assert(GrpxCoreTrace, (NULL != hObj));

    /* Check whether the formatting is common for all channel or
     * different for each of the channel for a given handle */
    if (TRUE == hObj->perChCfg)
    {
        chObj = hObj->chObjs[chNum];
    }
    else
    {
        /* Same format - so use handle context. Use each channel
         * objects to just store the data descriptor information */
        chObj = &hObj->handleContext;
    }

    return (chObj);
}

/**
 *  vcoreGrpxGetFrmAttr
 *  get the frame attribute
 *  return VPS_SOK on success else return error
 */
Int32 vcoreGrpxGetFrmAttr(Vcore_GrpxChObj *chObj,
                                 VpsHal_GrpxFrmDispAttr *frmAttr)
{
    /*get the frame attributes*/
    if (TRUE == chObj->state.isFormatSet)
    {

        frmAttr->frameHeight = chObj->coreFmt.frameHeight;
        frmAttr->frameWidth = chObj->coreFmt.frameWidth;
        if (chObj->coreFmt.fmt.scanFormat == FVID2_SF_PROGRESSIVE)
            frmAttr->interlacedSrc = FALSE;
        else
            frmAttr->interlacedSrc = TRUE;
    }
    else
    {
        return (VPS_EFAIL);
    }
    return (VPS_SOK);
}

/**
 *  vcoreGrpxCreateClutDataDesc
 *  create CLUT data descriptor
 *  return VPS_SOK on success else return error
 */
Int32 vcoreGrpxCreateClutDataDesc(const Vcore_GrpxHandleObj *hObj,
                                  Ptr descPtr,
                                  Ptr clutData)
{
    Int32                    retVal = VPS_SOK;
    VpsHal_VpdmaInDescParams clutDescParam;
    Vcore_GrpxInstObj        *instObj;

    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    clutDescParam.channel = instObj->chClut;
    clutDescParam.dataType = VPSHAL_VPDMA_CHANDT_CLUT;
    clutDescParam.memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
    clutDescParam.nextChannel = instObj->chClut;
    clutDescParam.notify = FALSE;
    clutDescParam.lineSkip = VPSHAL_VPDMA_LS_1;
    clutDescParam.transHeight = 1u;
    clutDescParam.transWidth = 1024u;
    clutDescParam.frameHeight = clutDescParam.frameWidth = 0u;
    clutDescParam.lineStride = 1024u;
    clutDescParam.startX = 0u;
    clutDescParam.startY = 0u;
    clutDescParam.is1DMem = FALSE;
    retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr,
                &clutDescParam);

    VpsHal_vpdmaSetAddress(descPtr, 0u, clutData);
    /* Print the created descriptor memory. */
    GT_1trace(GrpxCoreTrace, GT_DEBUG,
            "--------CLUT DATA at 0x%0.8x------------\n",(UInt32)descPtr);
    VpsHal_vpdmaPrintDesc(descPtr, GrpxCoreTrace);

    return (retVal);
}
/**
 *  vcoreGrpxCreateStenDataDesc
 *  create stenciling data desriptor
 *  return VPS_SOK on success else return error
 */

Int32 vcoreGrpxCreateStenDataDesc(const Vcore_GrpxHandleObj *hObj,
                                  Vcore_GrpxChObj *chObj,
                                  Vps_GrpxRegionParams *regParams,
                                  VpsHal_VpdmaInDescParams *stenDescParam,
                                  Ptr descPtr,
                                  UInt32 regId)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxInstObj        *instObj;

    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    stenDescParam->channel = instObj->chSten;
    stenDescParam->dataType = VPSHAL_VPDMA_CHANDT_STENCIL;
    stenDescParam->memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
    stenDescParam->is1DMem = FALSE;
    stenDescParam->nextChannel = instObj->chSten;
    stenDescParam->lineSkip = VPSHAL_VPDMA_LS_1;
    if (TRUE == chObj->state.isMultiWinMode)
    {
        stenDescParam->lineStride =
                    (UInt16)chObj->mInfo.regFmt[regId].pitch[2];
    }
    else
    {
        stenDescParam->lineStride = (UInt16)chObj->coreFmt.fmt.pitch[2];
    }
    stenDescParam->notify = FALSE;
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        stenDescParam->priority = VPS_CFG_DISP_VPDMA_PRIORITY;
    }
    else
    {
        stenDescParam->priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
    }
    stenDescParam->transWidth = stenDescParam->frameWidth =
                                          (UInt16)regParams->regionWidth;
    stenDescParam->transHeight = stenDescParam->frameHeight =
                                          (UInt16)regParams->regionHeight;
    stenDescParam->startX = 0u;
    stenDescParam->startY = 0u;

    /*create the descriptors*/
    VpsHal_vpdmaCreateInBoundDataDesc(descPtr, stenDescParam);

    /* Print the created descriptor memory. */
    GT_1trace(GrpxCoreTrace, GT_DEBUG,
           "---------STENCILING DATA at 0x%0.8x-------------\n",(UInt32)descPtr);
    VpsHal_vpdmaPrintDesc(descPtr, GrpxCoreTrace);
    return (retVal);
}

/**
 * vcoreGrpxCreateGrpxDataDesc
 *  function to create a graphics data descriptor
 */
Int32 vcoreGrpxCreateGrpxDataDesc(const Vcore_GrpxHandleObj *hObj,
                                  Vcore_GrpxChObj *chObj,
                                  Vps_GrpxRegionParams  *regParams,
                                  VpsHal_VpdmaRegionDataDescParams*regD,
                                  Ptr descPtr,
                                  UInt32 regId)
{
    Int32                    retVal = VPS_SOK;
    Vcore_GrpxInstObj        *instObj;

    instObj = hObj->instObj;
    GT_assert(GrpxCoreTrace, (NULL != instObj));

    regD->channel = instObj->chGrpx;
    regD->nextChannel = instObj->chGrpx;
    if (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat)
    {
        if (TRUE == chObj->coreFmt.fmt.fieldMerged[FVID2_RGB_ADDR_IDX])
        {
            regD->lineSkip = VPSHAL_VPDMA_LS_2;
        }
        else
        {
            regD->lineSkip = VPSHAL_VPDMA_LS_1;
        }
    }
    else
    {
        regD->lineSkip = VPSHAL_VPDMA_LS_1;
    }
    if (TRUE == chObj->state.isMultiWinMode)
    {
        regD->lineStride =
                (UInt16)chObj->mInfo.regFmt[regId].pitch[FVID2_RGB_ADDR_IDX];
        regD->dataType = vcoreGrpxGetVpdmaChanDataType(
                                    chObj->mInfo.regFmt[regId].dataFormat);
    }
    else
    {
        regD->dataType =
            vcoreGrpxGetVpdmaChanDataType(chObj->coreFmt.fmt.dataFormat);

        regD->lineStride =
                (UInt16)chObj->coreFmt.fmt.pitch[FVID2_RGB_ADDR_IDX];
    }
    regD->memType = (VpsHal_VpdmaMemoryType)chObj->coreFmt.memType;
    regD->notify = FALSE;
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        regD->priority = VPS_CFG_DISP_VPDMA_PRIORITY;
    }
    else
    {
        regD->priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
    }

    retVal = VpsHal_grpxRegionDispAttrSet(instObj->grpxHandle, regParams, regD);
    if (VPS_SOK == retVal)
    {
        retVal = VpsHal_vpdmaCreateRegionDataDesc(descPtr, regD);
        GT_1trace(GrpxCoreTrace, GT_DEBUG,
              "---------REGION DATA at 0x%0.8x-------------\n",(UInt32)descPtr);
        VpsHal_vpdmaPrintDesc(descPtr, GrpxCoreTrace);
    }
    return (retVal);
}


