/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_deiCommon.c
 *
 *  \brief VPS DEI Path Core internal file containing functiosn for allocation
 *  and freeing of handle/channel and other objects.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/core/src/vpscore_deiPriv.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *  the instances. Each of the channels in an instance is assumed to be
 *  shared across all handles of that instance.
 *  All the handles will allocate channel objects from this pool only.
 *  Instead of allocating VCORE_DEI_MAX_HANDLES * VCORE_DEI_MAX_CHANNELS
 *  objects per instance, only the below number of objects are allocated
 *  to save memory.
 */
#define VCORE_DEI_MAX_CHANNEL_OBJECTS   (VCORE_DEI_MAX_CHANNELS *              \
                                         VCORE_DEI_INST_MAX)

/**
 *  \brief Maximum number of mosaic object to allocate which is used in
 *  dynamic mosaic layout change.
 */
#define VCORE_DEI_MAX_MOSAIC_MEM_POOL   (VCORE_DEI_MAX_MULTIWIN_SETTINGS *     \
                                         VCORE_DEI_INST_MAX)

/** \brief Time out to be used in sem pend. */
#define VCORE_DEI_SEM_TIMEOUT           (BIOS_WAIT_FOREVER)

/** \brief Default width used for initializing format structure. */
#define VCORE_DEI_DEFAULT_WIDTH         (720u)
/** \brief Default height used for initializing format structure. */
#define VCORE_DEI_DEFAULT_HEIGHT        (480u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_DeiPoolObj
 *  Memory pool object containing all the statically allocated
 *  objects - used structure to avoid multiple global variables.
 */
typedef struct
{
    Vcore_DeiChObj          chMemPool[VCORE_DEI_MAX_CHANNEL_OBJECTS];
    /**< Memory pool for the channel objects. */
    UInt32                  chMemFlag[VCORE_DEI_MAX_CHANNEL_OBJECTS];
    /**< The flag variable represents whether a channel memory is allocated
         or not. */
    VpsUtils_PoolParams     chPoolPrm;
    /**< Pool params for descriptor pool memory. */

    Vcore_DeiMosaicInfo     mInfoMemPool[VCORE_DEI_MAX_MOSAIC_MEM_POOL];
    /**< Memory pool for the mosaic info objects. */
    UInt32                  mInfoMemFlag[VCORE_DEI_MAX_MOSAIC_MEM_POOL];
    /**< The flag variable represents whether a mosaic object is allocated
         or not. */
    VpsUtils_PoolParams     mInfoPoolPrm;
    /**< Pool params for mosaic pool memory. */

    Semaphore_Handle        lockSem;
    /**< Semaphore for protecting allocation and freeing of memory pool
         objects at open/close time. */
} Vcore_DeiPoolObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static UInt32 vcoreDeiIsValidLayoutId(UInt32 layoutId);

static Vcore_DeiMosaicInfo *vcoreDeiAllocMosaicObj(Vcore_DeiChObj *chObj,
                                                   UInt32 *layoutId);
static Int32 vcoreDeiFreeMosaicObj(Vcore_DeiInstObj *instObj,
                                   Vcore_DeiChObj *chObj,
                                   UInt32 layoutId);
static Int32 vcoreDeiAllocFreeCh(Vcore_DeiInstObj *instObj, UInt32 numFreeCh);
static Int32 vcoreDeiFreeFreeCh(Vcore_DeiInstObj *instObj,
                                Vcore_DeiMosaicInfo *mInfo);

static void vcoreDeiSetDefChInfo(Vcore_DeiChObj *chObj, UInt32 chNum);
static void vcoreDeiSetDefMosaicInfo(Vcore_DeiMosaicInfo *mInfo);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief DEI path instance objects. */
static Vcore_DeiInstObj DeiInstObjects[VCORE_DEI_INST_MAX];

/**
 *  \brief Pool objects used for storing pool memories, pool flags and pool
 *  handles.
 */
static Vcore_DeiPoolObj VcoreDeiPoolObjs;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vcoreDeiInit
 *  \brief Initializes DEI core objects, allocates memory etc.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vcoreDeiInit(UInt32 numInst, const Vcore_DeiInitParams *initPrms)
{
    Int32               retVal = VPS_SOK;
    UInt32              instCnt, hCnt, chCnt;
    Vcore_DeiInstObj   *instObj;
    Semaphore_Params    semParams;

    /* Check for errors */
    GT_assert(DeiCoreTrace, (numInst <= VCORE_DEI_INST_MAX));
    GT_assert(DeiCoreTrace, (NULL != initPrms));

    /* Init pool objects */
    VpsUtils_initPool(
        &VcoreDeiPoolObjs.chPoolPrm,
        (Void *) VcoreDeiPoolObjs.chMemPool,
        VCORE_DEI_MAX_CHANNEL_OBJECTS,
        sizeof (Vcore_DeiChObj),
        VcoreDeiPoolObjs.chMemFlag,
        DeiCoreTrace);
    VpsUtils_initPool(
        &VcoreDeiPoolObjs.mInfoPoolPrm,
        (Void *) VcoreDeiPoolObjs.mInfoMemPool,
        VCORE_DEI_MAX_MOSAIC_MEM_POOL,
        sizeof (Vcore_DeiMosaicInfo),
        VcoreDeiPoolObjs.mInfoMemFlag,
        DeiCoreTrace);

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    VcoreDeiPoolObjs.lockSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == VcoreDeiPoolObjs.lockSem)
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        for (instCnt = 0u; instCnt < numInst; instCnt++)
        {
            /* Initialize instance object members */
            instObj = &DeiInstObjects[instCnt];
            instObj->instId = initPrms[instCnt].instId;
            instObj->numHandle = initPrms[instCnt].maxHandle;
            instObj->isHqDei = initPrms[instCnt].isHqDei;
            instObj->vpdmaPath = initPrms[instCnt].vpdmaPath;
            /* Get the VPDMA channels */
            for (chCnt = 0u; chCnt < VCORE_DEI_MAX_VPDMA_CH; chCnt++)
            {
                instObj->vpdmaCh[chCnt] = initPrms[instCnt].vpdmaCh[chCnt];
            }

            instObj->openCnt = 0u;
            instObj->numDescPerWindow = VCORE_DEI_NUM_DESC_PER_WINDOW;
            instObj->curMode = VCORE_OPMODE_INVALID;
            instObj->numFreeCh = 0u;
            VpsUtils_memset(
                instObj->freeChList,
                0u,
                sizeof(instObj->freeChList));

            /* Mark all handles as free */
            for (hCnt = 0u; hCnt < VCORE_DEI_MAX_HANDLES; hCnt++)
            {
                instObj->hObjs[hCnt].isUsed = FALSE;
            }

            /* Assign HAL parmaters of instance object */
            vcoreDeiInitHalParams(instObj, &initPrms[instCnt]);
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiDeInit
 *  \brief Deallocates memory allocated by init function.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vcoreDeiDeInit(void)
{
    UInt32              instCnt;
    UInt32              halCnt;
    Vcore_DeiInstObj   *instObj;

    for (instCnt = 0u; instCnt < VCORE_DEI_INST_MAX; instCnt++)
    {
        instObj = &DeiInstObjects[instCnt];
        instObj->instId = 0u;
        instObj->curMode = VCORE_OPMODE_INVALID;
        instObj->numHandle = 0u;
        for (halCnt = 0u; halCnt < VCORE_DEI_MAX_HAL; halCnt++)
        {
            instObj->halHandle[halCnt] = NULL;
            instObj->halShadowOvlySize[halCnt] = 0u;
            instObj->ovlyOffset[halCnt] = 0u;
        }
    }

    /* Delete the Semaphore created for pool objects */
    if (NULL != VcoreDeiPoolObjs.lockSem)
    {
        Semaphore_delete(&VcoreDeiPoolObjs.lockSem);
        VcoreDeiPoolObjs.lockSem = NULL;
    }

    return (VPS_SOK);
}



/**
 *  vcoreDeiCreateLayout
 *  \brief Creates a layout according to the multi-window parameter and assigns
 *  a valid layout ID to layoutId parameter on success.
 */
Int32 vcoreDeiCreateLayout(Vcore_DeiInstObj *instObj,
                           Vcore_DeiChObj *chObj,
                           const Vps_MultiWinParams *multiWinPrms,
                           UInt32 *layoutId)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      numFreeCh;
    UInt32                      winCnt, bufOffset;
    UInt32                      numFirstRowDesc, numMultiDesc;
    Vcore_DeiMosaicInfo        *mInfo = NULL;
    const Vps_WinFormat        *winFmt;
    VpsHal_VpdmaMosaicWinFmt   *mulWinFmt;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
    UInt32                      numAbortDesc;
    Bool                        isStartOfRow;
#endif

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != multiWinPrms));
    GT_assert(DeiCoreTrace, (NULL != layoutId));

    /* Allocate a mosaic object */
    mInfo = vcoreDeiAllocMosaicObj(chObj, layoutId);
    if (NULL == mInfo)
    {
        GT_0trace(DeiCoreTrace, GT_ERR,
            "Mosaic object memory alloc failed!!\n");
        retVal = VPS_EALLOC;
    }

    if (VPS_SOK == retVal)
    {
        /* Copy the application configuration to core object */
        mInfo->appMulWinPrms.channelNum = multiWinPrms->channelNum;
        mInfo->appMulWinPrms.numWindows = multiWinPrms->numWindows;
        mInfo->appMulWinPrms.winFmt = &mInfo->appMulWinFmt[0u];
        GT_assert(DeiCoreTrace,
            (multiWinPrms->numWindows < VCORE_DEI_MAX_MOSAIC_WINDOW));
        VpsUtils_memcpy(
            &mInfo->appMulWinFmt[0u],
            multiWinPrms->winFmt,
            (sizeof(Vps_WinFormat) * multiWinPrms->numWindows));

        /* Initialize VPDMA mosaic params */
        mInfo->mulWinPrms.frameWidth = chObj->corePrms.frameWidth;
        mInfo->mulWinPrms.frameHeight = chObj->corePrms.frameHeight;
        mInfo->mulWinPrms.splitWindows = &mInfo->mulWinFmt[0u];
        mInfo->mulWinPrms.maxNumSplitWindows = VCORE_DEI_MAX_MOSAIC_WINDOW;

        /* Setup mosaic configuration using VPDMA HAL */
        mInfo->mulWinPrms.getNoWinsPerRow = FALSE;
        mInfo->mulWinPrms.numWindowsPerRow = NULL;
        retVal = VpsHal_vpdmaSetupMosaicCfg(
                     &mInfo->appMulWinPrms,
                     &mInfo->mulWinPrms);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "VPDMA HAL set mosaic configuration failed!!\n");
        }
    }

    if (VPS_SOK == retVal)
    {
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        numAbortDesc = 0u;
        isStartOfRow = TRUE;
#endif
        for (winCnt = 0u; winCnt < mInfo->mulWinPrms.numSplitWindows; winCnt++)
        {
            mulWinFmt = &mInfo->mulWinFmt[winCnt];
            winFmt = mulWinFmt->ipWindowFmt;
            GT_assert(DeiCoreTrace, (NULL != winFmt));

            /* Calculate buffer offset based on pitch and format */
            if (FVID2_DF_YUV422I_YUYV == chObj->corePrms.fmt.dataFormat)
            {
                /* YUYV422 interleaved format */
                bufOffset = mulWinFmt->bufferOffsetX * 2u;
            }
            else
            {
                /* YUV422/YUV420 semi planar format */
                bufOffset = mulWinFmt->bufferOffsetX;
            }
            if ((bufOffset & (VPSHAL_VPDMA_BUF_ADDR_ALIGN - 1u)) != 0u)
            {
                GT_3trace(DeiCoreTrace, GT_ERR,
                    "Buffer offset %d of split window %d not aligned to %d\n",
                    bufOffset,
                    winCnt,
                    VPSHAL_VPDMA_BUF_ADDR_ALIGN);
                retVal = VPS_EINVALID_PARAMS;
                break;
            }

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
            /* Calculate number of abort descriptors needed on row's last free
             * channel window */
            if ((TRUE == isStartOfRow) && (FALSE == mulWinFmt->isActualChanReq))
            {
                numAbortDesc++;
                isStartOfRow = FALSE;
            }
            /* We have an actual channel, so next row is a new row */
            if (TRUE == mulWinFmt->isActualChanReq)
            {
                isStartOfRow = TRUE;
            }
#endif
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Check if number of free channel is more than allocated array size */
        numFreeCh = mInfo->mulWinPrms.numFreeChans * instObj->numDescPerWindow;
        if (numFreeCh > VCORE_DEI_MAX_VPDMA_FREE_CH)
        {
            GT_2trace(DeiCoreTrace, GT_ERR,
                "Number of VPDMA free channels %d is more than supported %d\n",
                numFreeCh,
                VCORE_DEI_MAX_VPDMA_FREE_CH);
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Check if number of rows is more than what is supported */
        if (mInfo->mulWinPrms.numRows > VCORE_DEI_MAX_MOSAIC_ROW)
        {
            GT_2trace(DeiCoreTrace, GT_ERR,
                "Number of rows %d is more than supported %d\n",
                mInfo->mulWinPrms.numRows,
                VCORE_DEI_MAX_MOSAIC_ROW);
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Allocate free channels */
        retVal = vcoreDeiAllocFreeCh(instObj, numFreeCh);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Alloc free channels failed!!\n");
        }
        else
        {
            mInfo->numFreeCh = numFreeCh;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Dummy data descriptor is not needed if frame size and last window
         * bottom-right coordinates match */
        mInfo->numExtraDesc = 0u;
        if (TRUE == mInfo->mulWinPrms.isDummyNeeded)
        {
            mInfo->numExtraDesc += instObj->numDescPerWindow;
        }
        /* Add data descriptor for SOCH and abort of actual channel
         * in display mode */
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            mInfo->numExtraDesc += instObj->numDescPerWindow;
        }

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        /* Add the abort descriptors required for each row's last window
         * free channel */
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            /* Account for luma and chroma abort descriptors */
            numAbortDesc *= instObj->numDescPerWindow;

            /* Since data descriptor size is twice that of control descriptor,
             * divide by 2 (Add 1 for odd values cases). */
            mInfo->numExtraDesc += ((numAbortDesc + 1u) / 2u);

            GT_2trace(DeiCoreTrace, GT_DEBUG,
                "Number of free channels abort desc: %d (Extra Desc: %d)\n",
                numAbortDesc,
                (numAbortDesc + 1u) / 2u);
        }
#endif

        /* Set the number of data descriptors needed for mosaic mode */
        GT_assert(DeiCoreTrace,
            (mInfo->mulWinPrms.numWindowsFirstRow
                <= mInfo->mulWinPrms.numSplitWindows));
        numFirstRowDesc = (mInfo->mulWinPrms.numWindowsFirstRow *
                           instObj->numDescPerWindow);
        numMultiDesc =
            mInfo->numExtraDesc + ((mInfo->mulWinPrms.numSplitWindows -
                                        mInfo->mulWinPrms.numWindowsFirstRow) *
                                    instObj->numDescPerWindow);
        vcoreDeiSetDescInfo(
            instObj,
            chObj,
            &mInfo->descInfo,
            numFirstRowDesc,
            numMultiDesc);
    }

    /* If error, free-up the already allocated objects */
    if ((VPS_SOK != retVal) && (NULL != mInfo))
    {
        vcoreDeiFreeMosaicObj(instObj, chObj, *layoutId);
        *layoutId = VCORE_INVALID_LAYOUT_ID;
    }

    return (retVal);
}



/**
 *  vcoreDeiDeleteLayout
 *  \brief Free-up the mosaic layout object corresponding to the layout ID.
 *  Also frees-up any alocated VPDMA free channels.
 */
Int32 vcoreDeiDeleteLayout(Vcore_DeiInstObj *instObj,
                           Vcore_DeiChObj *chObj,
                           UInt32 layoutId)
{
    Int32           retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Free-up the allocated mosaic object */
    retVal = vcoreDeiFreeMosaicObj(instObj, chObj, layoutId);

    return (retVal);
}



/**
 *  vcoreDeiGetLayoutObj
 *  \brief Returns the layout object pointer.
 */
Vcore_DeiMosaicInfo *vcoreDeiGetLayoutObj(const Vcore_DeiChObj *chObj,
                                          UInt32 layoutId)
{
    UInt32                  layoutIdx;
    Vcore_DeiMosaicInfo    *mInfo = NULL;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Check for valid layout ID */
    if (vcoreDeiIsValidLayoutId(layoutId))
    {
        /* Get the mosaic object index */
        GT_assert(DeiCoreTrace, (layoutId >= VCORE_START_VALID_LAYOUT_ID));
        layoutIdx = layoutId - VCORE_START_VALID_LAYOUT_ID;
        mInfo = chObj->mInfo[layoutIdx];
    }

    return (mInfo);
}



/**
 *  vcoreDeiAllocHandleObj
 *  \brief Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
Vcore_DeiHandleObj *vcoreDeiAllocHandleObj(UInt32 instId, Vcore_OpMode mode)
{
    UInt32              instCnt, hCnt;
    Vcore_DeiInstObj   *instObj = NULL;
    Vcore_DeiHandleObj *hObj = NULL;

    /* Find out the instance to which this handle belongs to */
    for (instCnt = 0u; instCnt < VCORE_DEI_INST_MAX; instCnt++)
    {
        if (DeiInstObjects[instCnt].instId == instId)
        {
            instObj = &DeiInstObjects[instCnt];
            break;
        }
    }

    Semaphore_pend(VcoreDeiPoolObjs.lockSem, VCORE_DEI_SEM_TIMEOUT);

    if (NULL != instObj)
    {
        /* Check if the required mode is proper */
        if (VCORE_OPMODE_INVALID > instObj->curMode)
        {
            /* Core already opend for some mode. For display mode, only one
             * handle should be allowed. It doesn't make sense isn't it? */
            GT_assert(DeiCoreTrace, (VCORE_OPMODE_DISPLAY != instObj->curMode));
            GT_assert(DeiCoreTrace,
                !((VCORE_OPMODE_MEMORY == instObj->curMode)
                    && (VCORE_OPMODE_MEMORY != mode)));
        }
    }

    if (NULL != instObj)
    {
        /* Use a free handle object */
        for (hCnt = 0u; hCnt < instObj->numHandle; hCnt++)
        {
            if (FALSE == instObj->hObjs[hCnt].isUsed)
            {
                hObj = &instObj->hObjs[hCnt];
                break;
            }
        }
    }

    if (NULL != hObj)
    {
        /* Update the state and return the handle handle */
        hObj->isUsed = TRUE;
        hObj->instObj = instObj;
        instObj->openCnt++;
        instObj->curMode = mode;
    }

    Semaphore_post(VcoreDeiPoolObjs.lockSem);

    return (hObj);
}



/**
 *  vcoreDeiFreeHandleObj
 *  \brief Frees-up the handle object and resets the variables.
 */
Int32 vcoreDeiFreeHandleObj(Vcore_DeiHandleObj *hObj)
{
    Int32               retVal = VPS_EFAIL;
    UInt32              hCnt;
    Vcore_DeiInstObj   *instObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(DeiCoreTrace, (NULL != instObj));

    Semaphore_pend(VcoreDeiPoolObjs.lockSem, VCORE_DEI_SEM_TIMEOUT);

    /* Free the handle object */
    for (hCnt = 0u; hCnt < instObj->numHandle; hCnt++)
    {
        if (hObj == &instObj->hObjs[hCnt])
        {
            /* Check if the handle is already allocated */
            GT_assert(DeiCoreTrace, (TRUE == instObj->hObjs[hCnt].isUsed));

            /* Reset the variables */
            hObj->isUsed = FALSE;
            hObj->instObj = NULL;
            hObj->perChCfg = FALSE;
            hObj->numCh = 0u;

            /* If last handle of the instance, then set mode to invalid */
            if (instObj->openCnt > 0u)
            {
                instObj->openCnt--;
            }
            if (0u == instObj->openCnt)
            {
                instObj->curMode = VCORE_OPMODE_INVALID;
            }

            retVal = VPS_SOK;
            break;
        }
    }

    Semaphore_post(VcoreDeiPoolObjs.lockSem);

    return (retVal);
}



/**
 *  vcoreDeiAllocChObjs
 *  \brief Allocates channel objects.
 *  Returns error if allocation failed.
 */
Int32 vcoreDeiAllocChObjs(Vcore_DeiHandleObj *hObj, UInt32 numCh)
{
    Int32       retVal = VPS_SOK;
    UInt32      chCnt;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Allocate channel objects one at a time */
        hObj->chObjs[chCnt] = (Vcore_DeiChObj *)
            VpsUtils_alloc(
                &VcoreDeiPoolObjs.chPoolPrm,
                sizeof (Vcore_DeiChObj),
                DeiCoreTrace);
        if (NULL == hObj->chObjs[chCnt])
        {
            GT_1trace(DeiCoreTrace, GT_ERR,
                "Channel Object memory alloc failed for channel %d!!\n",
                chCnt);
            retVal = VPS_EALLOC;
            break;
        }

        /* Set default values for the channel object */
        vcoreDeiSetDefChInfo(hObj->chObjs[chCnt], chCnt);
    }

    /* If error, free-up the already allocated objects */
    if (VPS_SOK != retVal)
    {
        vcoreDeiFreeChObjs(hObj, chCnt);
    }

    return (retVal);
}



/**
 *  vcoreDeiFreeChObjs
 *  \brief Frees-up the channel objects.
 */
Int32 vcoreDeiFreeChObjs(Vcore_DeiHandleObj *hObj, UInt32 numCh)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  chCnt, mCnt;
    Vcore_DeiChObj         *chObj;
    Vcore_DeiMosaicInfo    *mInfo;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != hObj));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        GT_assert(DeiCoreTrace, (NULL != chObj));

        /* Free all the allocated mosaic objects */
        for (mCnt = 0u; mCnt < VCORE_DEI_MAX_LAYOUT_PER_CH; mCnt++)
        {
            mInfo = chObj->mInfo[mCnt];
            if (NULL != mInfo)
            {
                /* Free the VPDMA free channels */
                if (mInfo->numFreeCh > 0u)
                {
                    retVal = vcoreDeiFreeFreeCh(hObj->instObj, mInfo);
                    mInfo->numFreeCh = 0u;
                }

                /* Free the mosaic object */
                retVal |= VpsUtils_free(
                              &VcoreDeiPoolObjs.mInfoPoolPrm,
                              mInfo,
                              DeiCoreTrace);
                chObj->mInfo[mCnt] = NULL;
            }
        }

        /* Memset the channel object before freeing */
        VpsUtils_memset(hObj->chObjs[chCnt], 0u, sizeof(Vcore_DeiChObj));
        retVal |= VpsUtils_free(
                      &VcoreDeiPoolObjs.chPoolPrm,
                      chObj,
                      DeiCoreTrace);
        hObj->chObjs[chCnt] = NULL;
    }

    return (retVal);
}



/**
 *  vcoreDeiCheckParams
 *  \brief Checks whether a given parameters are valid or not.
 *  Returns VPS_SOK if everything is fine else returns error value.
 */
Int32 vcoreDeiCheckParams(const Vcore_DeiInstObj *instObj,
                          const Vcore_DeiParams *corePrms)
{
    Int32       retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != corePrms));

    /* Check whether data format is supported or not */
    if (!((FVID2_DF_YUV422I_YUYV == corePrms->fmt.dataFormat)
       || (FVID2_DF_YUV422I_UYVY == corePrms->fmt.dataFormat)	
       || (FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat)
       || (FVID2_DF_YUV422SP_UV == corePrms->fmt.dataFormat)))
    {
        GT_1trace(DeiCoreTrace, GT_ERR,
            "Data format (%d) not supported!!\n",
            corePrms->fmt.dataFormat);
        retVal = VPS_EINVALID_PARAMS;
    }

    if ((FVID2_DF_YUV422I_YUYV == corePrms->fmt.dataFormat) 
		|| (FVID2_DF_YUV422I_UYVY == corePrms->fmt.dataFormat))
    {
        /* Check whether the pitch is valid */
        if (corePrms->fmt.pitch[FVID2_YUV_INT_ADDR_IDX]
            < (corePrms->fmt.width * 2u))
        {
            GT_2trace(DeiCoreTrace, GT_ERR,
                "Pitch (%d) less than Width (%d) in bytes!!\n",
                corePrms->fmt.pitch[FVID2_YUV_INT_ADDR_IDX],
                (corePrms->fmt.width * 2u));
            retVal = VPS_EINVALID_PARAMS;
        }
    }
    else if ((FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat) ||
             (FVID2_DF_YUV422SP_UV == corePrms->fmt.dataFormat))
    {
        /* Check whether the pitch is valid */
        if (corePrms->fmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX]
            < corePrms->fmt.width)
        {
            GT_2trace(DeiCoreTrace, GT_ERR,
                "Y Pitch (%d) less than Width (%d) in bytes!!\n",
                corePrms->fmt.pitch[FVID2_YUV_SP_Y_ADDR_IDX],
                corePrms->fmt.width);
            retVal = VPS_EINVALID_PARAMS;
        }
        if (corePrms->fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX]
            < corePrms->fmt.width)
        {
            GT_2trace(DeiCoreTrace, GT_ERR,
                "CbCr Pitch (%d) less than Width (%d) in bytes!!\n",
                corePrms->fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX],
                corePrms->fmt.width);
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    /* Check whether window width/startX is even */
    if ((corePrms->fmt.width & 0x01u) || (corePrms->startX & 0x01u))
    {
        GT_2trace(DeiCoreTrace, GT_ERR,
            "Width(%d)/StartX(%d) can't be odd!!\n",
            corePrms->fmt.width,
            corePrms->startX);
        retVal = VPS_EINVALID_PARAMS;
    }

    if (instObj->halHandle[VCORE_DEI_DEI_IDX] == NULL)
    {
        if ((FVID2_SF_INTERLACED == corePrms->fmt.scanFormat) &&
            (FVID2_SF_PROGRESSIVE == corePrms->secScanFmt))
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Interlaced to progressive is not supported for this instance\n");
            retVal = VPS_EINVALID_PARAMS;
        }
        if ((FVID2_SF_PROGRESSIVE == corePrms->fmt.scanFormat) &&
            (FVID2_SF_INTERLACED == corePrms->secScanFmt))
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "Progressive to interlaced is not supported for this instance\n");
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    if (FVID2_DF_YUV420SP_UV == corePrms->fmt.dataFormat)
    {
        if ((FVID2_SF_INTERLACED == corePrms->fmt.scanFormat) &&
            (VCORE_OPMODE_DISPLAY == instObj->curMode))
        {
            /* Check whether window height/startY is multiple of 4 for YUV420
             * format in interlaced display mode */
            if ((corePrms->fmt.height & 0x03u) || (corePrms->startY & 0x03u))
            {
                GT_2trace(DeiCoreTrace, GT_ERR,
                    "Height(%d)/StartY(%d) should be multiple of 4 for YUV420 "
                    "format in display mode!!\n",
                    corePrms->fmt.height,
                    corePrms->startY);
                retVal = VPS_EINVALID_PARAMS;
            }
        }
        else
        {
            /* Check whether window height/startY is even for YUV420 format */
            if ((corePrms->fmt.height & 0x01u) || (corePrms->startY & 0x01u))
            {
                GT_2trace(DeiCoreTrace, GT_ERR,
                    "Height(%d)/StartY(%d) can't be odd for YUV420 format!!\n",
                    corePrms->fmt.height,
                    corePrms->startY);
                retVal = VPS_EINVALID_PARAMS;
            }
        }
    }

    /* Check whether the active window is within frame limit */
    if (corePrms->frameWidth < (corePrms->startX + corePrms->fmt.width))
    {
        GT_2trace(DeiCoreTrace, GT_ERR,
            "Frame Width (%d) less than Width + HOffset (%d)!!\n",
            corePrms->frameWidth,
            (corePrms->startX + corePrms->fmt.width));
        retVal = VPS_EINVALID_PARAMS;
    }
    if (corePrms->frameHeight < (corePrms->startY + corePrms->fmt.height))
    {
        GT_2trace(DeiCoreTrace, GT_ERR,
            "Frame Height (%d) less than Height + VOffset (%d)!!\n",
            corePrms->frameHeight,
            (corePrms->startY + corePrms->fmt.height));
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check N-1 field parameters */
    if (VPS_DEIHQ_CTXMODE_APP_N_1 == corePrms->deiHqCtxMode)
    {
        /* Check the width and height with input format */
        if ((corePrms->inFmtFldN_1.width != corePrms->fmt.width)
            || (corePrms->inFmtFldN_1.height != corePrms->fmt.height))
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "N-1 field width/height not same as in width/height!!\n");
            retVal = VPS_EINVALID_PARAMS;
        }

        /* Check whether the data format is supported or not */
        if (!((FVID2_DF_YUV422I_YUYV == corePrms->inFmtFldN_1.dataFormat)
			|| (FVID2_DF_YUV422I_UYVY == corePrms->inFmtFldN_1.dataFormat)
           || (FVID2_DF_YUV422SP_UV == corePrms->inFmtFldN_1.dataFormat)))
        {
            GT_1trace(DeiCoreTrace, GT_ERR,
                "Data format (%d) not supported for N-1 field buffer!!\n",
                corePrms->inFmtFldN_1.dataFormat);
            retVal = VPS_EINVALID_PARAMS;
        }

        if ((FVID2_DF_YUV422I_YUYV == corePrms->inFmtFldN_1.dataFormat) 
			|| (FVID2_DF_YUV422I_UYVY == corePrms->inFmtFldN_1.dataFormat))
        {
            /* Check whether the pitch is valid */
            if (corePrms->inFmtFldN_1.pitch[FVID2_YUV_INT_ADDR_IDX]
                < (corePrms->fmt.width * 2u))
            {
                GT_2trace(DeiCoreTrace, GT_ERR,
                    "Pitch (%d) less than Width (%d) in bytes!!\n",
                    corePrms->inFmtFldN_1.pitch[FVID2_YUV_INT_ADDR_IDX],
                    (corePrms->fmt.width * 2u));
                retVal = VPS_EINVALID_PARAMS;
            }
        }
        else if (FVID2_DF_YUV422SP_UV == corePrms->inFmtFldN_1.dataFormat)
        {
            /* Check whether the pitch is valid */
            if (corePrms->inFmtFldN_1.pitch[FVID2_YUV_SP_Y_ADDR_IDX]
                < corePrms->fmt.width)
            {
                GT_2trace(DeiCoreTrace, GT_ERR,
                    "Y Pitch (%d) less than Width (%d) in bytes!!\n",
                    corePrms->inFmtFldN_1.pitch[FVID2_YUV_SP_Y_ADDR_IDX],
                    corePrms->fmt.width);
                retVal = VPS_EINVALID_PARAMS;
            }
            if (corePrms->inFmtFldN_1.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX]
                < corePrms->fmt.width)
            {
                GT_2trace(DeiCoreTrace, GT_ERR,
                    "CbCr Pitch (%d) less than Width (%d) in bytes!!\n",
                    corePrms->inFmtFldN_1.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX],
                    corePrms->fmt.width);
                retVal = VPS_EINVALID_PARAMS;
            }
        }

        /* Check scan format */
        if (FVID2_SF_INTERLACED != corePrms->inFmtFldN_1.scanFormat)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "N-1 field should be interlaced scan format!!\n");
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    /* Can't enable compressor in progressive TNR mode - not supported */
    if ((TRUE == instObj->isHqDei) &&
        (TRUE == corePrms->deiHqCfg.bypass) &&
        (TRUE == corePrms->deiHqCfg.tnrEnable) &&
        (FVID2_SF_PROGRESSIVE == corePrms->fmt.scanFormat) &&
        (TRUE == corePrms->comprEnable[0u]))
    {
        GT_0trace(DeiCoreTrace, GT_ERR,
            "Enabling of compression not supported in progressive TNR mode\n");
        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        retVal = VpsHal_vpdmaIsValidSize(
                    instObj->vpdmaPath,
                    corePrms->fmt.dataFormat,
                    (VpsHal_VpdmaMemoryType) corePrms->memType,
                    corePrms->fmt.width,
                    corePrms->fmt.height);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR, "Wrong Input Size!!\n");
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiCheckMosaicCfg
 *  \brief Checks whether a given mosaic config is valid or not.
 *  Returns VPS_SOK if everything is fine else returns error value.
 */
Int32 vcoreDeiCheckMosaicCfg(const Vps_MultiWinParams *multiWinPrms)
{
    Int32       retVal = VPS_SOK;

    /* Check for valid number of windows */
    if (multiWinPrms->numWindows > VCORE_DEI_MAX_MOSAIC_WINDOW)
    {
        GT_1trace(DeiCoreTrace, GT_ERR,
            "Number of windows more than what could be supported (%d)\n",
            VCORE_DEI_MAX_MOSAIC_WINDOW);
        retVal = VPS_EOUT_OF_RANGE;
    }

    return (retVal);
}



/**
 *  vcoreDeiIsValidLayoutId
 *  \brief Checks whether a layout ID is valid or not.
 */
static UInt32 vcoreDeiIsValidLayoutId(UInt32 layoutId)
{
    UInt32      isValid = FALSE;

    if ((layoutId >= VCORE_START_VALID_LAYOUT_ID) &&
        (layoutId < (VCORE_DEI_MAX_LAYOUT_PER_CH +
                     VCORE_START_VALID_LAYOUT_ID)))
    {
        isValid = TRUE;
    }

    return (isValid);
}



/**
 *  vcoreDeiAllocMosaicObj
 *  \brief Allocates a mosaic object from the memory pool and assigns it to a
 *  free mosaic info pointer in channel object.
 */
static Vcore_DeiMosaicInfo *vcoreDeiAllocMosaicObj(Vcore_DeiChObj *chObj,
                                                   UInt32 *layoutId)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  mCnt;
    Vcore_DeiMosaicInfo    *mInfo;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != layoutId));

    /* Allocate a free mosaic object */
    mInfo =(Vcore_DeiMosaicInfo *)
        VpsUtils_alloc(
            &VcoreDeiPoolObjs.mInfoPoolPrm,
            sizeof (Vcore_DeiMosaicInfo),
            DeiCoreTrace);
    if (NULL == mInfo)
    {
        GT_0trace(DeiCoreTrace, GT_ERR,
            "Mosaic object memory alloc failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        /* Set default mosaic info */
        vcoreDeiSetDefMosaicInfo(mInfo);

        /* Find a free space in the channel mosaic list and store the
         * allocated object */
        retVal = VPS_EALLOC;
        for (mCnt = 0u; mCnt < VCORE_DEI_MAX_LAYOUT_PER_CH; mCnt++)
        {
            if (NULL == chObj->mInfo[mCnt])
            {
                chObj->mInfo[mCnt] = mInfo;
                /* Index + start valid ID is the layout ID as 0 is used as
                 * non-mosaic layout ID. */
                *layoutId = mCnt + VCORE_START_VALID_LAYOUT_ID;
                retVal = VPS_SOK;
                break;
            }
        }
    }

    /* Free mosaic object if error occurs */
    if ((VPS_SOK != retVal) && (NULL != mInfo))
    {
        VpsUtils_free(&VcoreDeiPoolObjs.mInfoPoolPrm, mInfo, DeiCoreTrace);
        *layoutId = VCORE_INVALID_LAYOUT_ID;
    }

    return (mInfo);
}



/**
 *  vcoreDeiFreeMosaicObj
 *  \brief Frees the mosaic object and frees any allocated VPDMA free channels
 *  if already allocated.
 */
static Int32 vcoreDeiFreeMosaicObj(Vcore_DeiInstObj *instObj,
                                   Vcore_DeiChObj *chObj,
                                   UInt32 layoutId)
{
    Int32                   retVal = VPS_SOK;
    UInt32                  layoutIdx;
    Vcore_DeiMosaicInfo    *mInfo;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != chObj));
    GT_assert(DeiCoreTrace, (NULL != layoutId));

    /* Check for valid layout ID */
    if (!vcoreDeiIsValidLayoutId(layoutId))
    {
        GT_0trace(DeiCoreTrace, GT_ERR, "Invalid layout ID!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }

    if (VPS_SOK == retVal)
    {
        /* Get the mosaic object index */
        GT_assert(DeiCoreTrace, (layoutId >= VCORE_START_VALID_LAYOUT_ID));
        layoutIdx = layoutId - VCORE_START_VALID_LAYOUT_ID;

        /* Free the mosaic object */
        mInfo = chObj->mInfo[layoutIdx];
        if (NULL != mInfo)
        {
            /* Free the VPDMA free channels */
            if (mInfo->numFreeCh > 0u)
            {
                retVal = vcoreDeiFreeFreeCh(instObj, mInfo);
                mInfo->numFreeCh = 0u;
            }

            /* Free the mosaic object */
            retVal |= VpsUtils_free(
                          &VcoreDeiPoolObjs.mInfoPoolPrm,
                          mInfo,
                          DeiCoreTrace);
            chObj->mInfo[layoutIdx] = NULL;
        }
    }

    return (retVal);
}



/**
 *  vcoreDeiAllocFreeCh
 *  \brief Allocates VPDMA free channels if the new layout number of
 *  free channels is more than the already allocated free channels for an
 *  instance.
 */
static Int32 vcoreDeiAllocFreeCh(Vcore_DeiInstObj *instObj, UInt32 numFreeCh)
{
    Int32       retVal = VPS_SOK;
    UInt32      freeChToAllocate;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));

    Semaphore_pend(VcoreDeiPoolObjs.lockSem, VCORE_DEI_SEM_TIMEOUT);

    /* Allocate free channels only if the new request is more than already
     * allocated free channels. */
    if (numFreeCh > instObj->numFreeCh)
    {
        /* Allocate only the difference in count */
        freeChToAllocate = numFreeCh - instObj->numFreeCh;

        /* Allocate free channels - Y and C need different free channel */
        retVal = Vrm_allocFreeChannel(
                     &instObj->freeChList[instObj->numFreeCh],
                     freeChToAllocate);
        if (VPS_SOK != retVal)
        {
            GT_0trace(DeiCoreTrace, GT_ERR,
                "VPDMA free channel allocation failed!!\n");
        }
        else
        {
            GT_3trace(DeiCoreTrace, GT_DEBUG,
                "FreeCh Alloc: %d, Prev Num FreeCh: %d, Cur Num FreeCh: %d\n",
                freeChToAllocate, instObj->numFreeCh, numFreeCh);

            instObj->numFreeCh = numFreeCh;
        }
    }

    Semaphore_post(VcoreDeiPoolObjs.lockSem);

    return (retVal);
}



/**
 *  vcoreDeiFreeFreeCh
 *  \brief Frees the VPDMA free channels if not used by other layouts in an
 *  instance.
 */
static Int32 vcoreDeiFreeFreeCh(Vcore_DeiInstObj *instObj,
                                Vcore_DeiMosaicInfo *mInfo)
{
    Int32               retVal = VPS_SOK;
    UInt32              hCnt, chCnt, mCnt;
    UInt32              freeChToFree, maxNumFreeCh = 0u;
    Vcore_DeiChObj     *chObj;
    Vcore_DeiHandleObj *hObj;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != instObj));
    GT_assert(DeiCoreTrace, (NULL != mInfo));

    Semaphore_pend(VcoreDeiPoolObjs.lockSem, VCORE_DEI_SEM_TIMEOUT);

    /* Find the max free channel in an instance object excluding this layout */
    for (hCnt = 0u; hCnt < VCORE_DEI_MAX_HANDLES; hCnt++)
    {
        hObj = &instObj->hObjs[hCnt];
        if (TRUE == hObj->isUsed)
        {
            for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
            {
                chObj = hObj->chObjs[chCnt];
                /* NULL pointer check */
                GT_assert(DeiCoreTrace, (NULL != chObj));

                for (mCnt = 0u; mCnt < VCORE_DEI_MAX_LAYOUT_PER_CH; mCnt++)
                {
                    if ((NULL != chObj->mInfo[mCnt]) &&
                        (mInfo != chObj->mInfo[mCnt]) &&
                        (chObj->mInfo[mCnt]->numFreeCh > maxNumFreeCh))
                    {
                        maxNumFreeCh = chObj->mInfo[mCnt]->numFreeCh;
                    }
                }
            }
        }
    }

    /* Free the excess free channels */
    if (mInfo->numFreeCh > maxNumFreeCh)
    {
        freeChToFree = mInfo->numFreeCh - maxNumFreeCh;
        retVal = Vrm_releaseFreeChannel(
                     &instObj->freeChList[maxNumFreeCh],
                     freeChToFree);

        GT_3trace(DeiCoreTrace, GT_DEBUG,
            "FreeCh Freed: %d, Prev Num FreeCh: %d, Cur Num FreeCh: %d\n",
            freeChToFree, instObj->numFreeCh, maxNumFreeCh);

        instObj->numFreeCh = maxNumFreeCh;
    }

    Semaphore_post(VcoreDeiPoolObjs.lockSem);

    return (retVal);
}



/**
 *  vcoreDeiSetDefChInfo
 *  \brief Resets the channel information with default values.
 */
static void vcoreDeiSetDefChInfo(Vcore_DeiChObj *chObj, UInt32 chNum)
{
    UInt32      chCnt, cnt, mCnt;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != chObj));

    /* Set proper states */
    chObj->state.isFormatSet = FALSE;
    chObj->state.isParamsSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    /* Initialize core parameters - Assumes YUV422 720x480 progressive */
    chObj->corePrms.frameWidth = VCORE_DEI_DEFAULT_WIDTH;
    chObj->corePrms.frameHeight = VCORE_DEI_DEFAULT_HEIGHT;
    chObj->corePrms.startX = 0u;
    chObj->corePrms.startY = 0u;
    chObj->corePrms.secScanFmt = FVID2_SF_PROGRESSIVE;
    chObj->corePrms.memType =
        (UInt32) VPSHAL_VPDMA_MT_NONTILEDMEM;

    chObj->corePrms.fmt.channelNum = chNum;
    chObj->corePrms.fmt.width = VCORE_DEI_DEFAULT_WIDTH;
    chObj->corePrms.fmt.height = VCORE_DEI_DEFAULT_HEIGHT;
    chObj->corePrms.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
        VCORE_DEI_DEFAULT_WIDTH * 2u;
    chObj->corePrms.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chObj->corePrms.fmt.dataFormat = FVID2_DF_YUV422I_YUYV;
    chObj->corePrms.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
    chObj->corePrms.fmt.bpp = FVID2_BPP_BITS16;
    chObj->corePrms.fmt.reserved = NULL;

    chObj->corePrms.inFmtFldN_1.channelNum = chNum;
    chObj->corePrms.inFmtFldN_1.width = VCORE_DEI_DEFAULT_WIDTH;
    chObj->corePrms.inFmtFldN_1.height = VCORE_DEI_DEFAULT_HEIGHT;
    chObj->corePrms.inFmtFldN_1.pitch[FVID2_YUV_INT_ADDR_IDX] =
        VCORE_DEI_DEFAULT_WIDTH * 2u;
    chObj->corePrms.inFmtFldN_1.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chObj->corePrms.inFmtFldN_1.dataFormat = FVID2_DF_YUV422I_YUYV;
    chObj->corePrms.inFmtFldN_1.scanFormat = FVID2_SF_PROGRESSIVE;
    chObj->corePrms.inFmtFldN_1.bpp = FVID2_BPP_BITS16;
    chObj->corePrms.inFmtFldN_1.reserved = NULL;
    chObj->corePrms.deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;

    /* Initialize DRN HAL parameters */
    chObj->corePrms.drnEnable = FALSE;

    /* Initialize DEI HQ HAL parameters */
    chObj->corePrms.deiHqCfg.bypass = TRUE;
    chObj->corePrms.deiHqCfg.inpMode = VPS_DEIHQ_EDIMODE_EDI_LARGE_WINDOW;
    chObj->corePrms.deiHqCfg.tempInpEnable = TRUE;
    chObj->corePrms.deiHqCfg.tempInpChromaEnable = TRUE;
    chObj->corePrms.deiHqCfg.spatMaxBypass = FALSE;
    chObj->corePrms.deiHqCfg.tempMaxBypass = FALSE;
    chObj->corePrms.deiHqCfg.fldMode = VPS_DEIHQ_FLDMODE_5FLD;
    chObj->corePrms.deiHqCfg.lcModeEnable = TRUE;
    chObj->corePrms.deiHqCfg.mvstmEnable = TRUE;
    chObj->corePrms.deiHqCfg.tnrEnable = FALSE;
    chObj->corePrms.deiHqCfg.snrEnable = TRUE;
    chObj->corePrms.deiHqCfg.sktEnable = FALSE;
    chObj->corePrms.deiHqCfg.chromaEdiEnable = TRUE;

    /* Initialize DEI HAL parameters */
    chObj->corePrms.deiCfg.bypass = TRUE;
    chObj->corePrms.deiCfg.spatMaxBypass = FALSE;
    chObj->corePrms.deiCfg.tempMaxBypass = FALSE;
    chObj->corePrms.deiCfg.inpMode = VPS_DEI_EDIMODE_LUMA_CHROMA;
    chObj->corePrms.deiCfg.tempInpEnable = TRUE;
    chObj->corePrms.deiCfg.tempInpChromaEnable = TRUE;

    /* Initialize SC parameters */
    chObj->corePrms.scCfg.bypass = TRUE;
    chObj->corePrms.cropCfg.cropStartX = 0u;
    chObj->corePrms.cropCfg.cropStartY = 0u;
    chObj->corePrms.cropCfg.cropWidth = VCORE_DEI_DEFAULT_WIDTH;
    chObj->corePrms.cropCfg.cropHeight = VCORE_DEI_DEFAULT_HEIGHT;
    chObj->corePrms.scCfg.nonLinear = FALSE;
    chObj->corePrms.scCfg.stripSize = 0u;
    chObj->corePrms.scCfg.vsType = VPS_SC_VST_POLYPHASE;
    chObj->corePrms.scCfg.hsType = VPS_SC_HST_AUTO;
    chObj->corePrms.tarWidth = VCORE_DEI_DEFAULT_WIDTH;
    chObj->corePrms.tarHeight = VCORE_DEI_DEFAULT_HEIGHT;

    /* Initialize channel descriptor info structure */
    VpsUtils_memset(&chObj->descInfo, 0u, sizeof(Vcore_DescInfo));
    VpsUtils_memset(chObj->descOffset, 0u, sizeof(chObj->descOffset));
    chObj->numExtraDesc = 0u;
    chObj->isDummyNeeded = FALSE;
    chObj->fsEvent = VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE;

    /* Initialize mosaic info */
    for (mCnt = 0u; mCnt < VCORE_DEI_MAX_LAYOUT_PER_CH; mCnt++)
    {
        chObj->mInfo[mCnt] = NULL;
    }

    /* Initialize DEI state machine */
    chObj->ctxInfo.isDeinterlacing = FALSE;
    chObj->ctxInfo.isTnrMode = FALSE;
    chObj->ctxInfo.numPrevFld = 0u;
    chObj->ctxInfo.numMvIn = 0u;
    chObj->ctxInfo.numMvOut = 0u;
    chObj->ctxInfo.numMvstmIn = 0u;
    chObj->ctxInfo.numMvstmOut = 0u;
    chObj->ctxInfo.numCurFldOut = 0u;
    for (chCnt = 0u; chCnt < VCORE_DEI_MAX_VPDMA_CH; chCnt++)
    {
        chObj->isDescReq[chCnt] = FALSE;
        chObj->isDescDummy[chCnt] = FALSE;
    }
    for (cnt = 0u; cnt < VCORE_DEI_NUM_DESC_PER_WINDOW; cnt++)
    {
        chObj->bufIndex[cnt] = FVID2_YUV_INT_ADDR_IDX;
    }

    return;
}



/**
 *  vcoreDeiSetDefMosaicInfo
 *  \brief Resets the mosaic information with default values.
 */
static void vcoreDeiSetDefMosaicInfo(Vcore_DeiMosaicInfo *mInfo)
{
    UInt32          cnt, pitchCnt;

    /* NULL pointer check */
    GT_assert(DeiCoreTrace, (NULL != mInfo));

    /* Initialize mosaic info structure */
    mInfo->numExtraDesc = 0u;
    VpsUtils_memset(&mInfo->descInfo, 0u, sizeof(Vcore_DescInfo));
    mInfo->numFreeCh = 0u;

    /* Initialize application params */
    mInfo->appMulWinPrms.channelNum = 0u;
    mInfo->appMulWinPrms.numWindows = 0u;
    mInfo->appMulWinPrms.winFmt = NULL;

    /* Initialize window format structure */
    for (cnt = 0u; cnt < VCORE_DEI_MAX_MOSAIC_WINDOW; cnt++)
    {
        mInfo->appMulWinFmt[cnt].winStartX = 0u;
        mInfo->appMulWinFmt[cnt].winStartY = 0u;
        mInfo->appMulWinFmt[cnt].winWidth = 0u;
        mInfo->appMulWinFmt[cnt].winHeight = 0u;
        for (pitchCnt = 0u; pitchCnt < FVID2_MAX_PLANES; pitchCnt++)
        {
            mInfo->appMulWinFmt[cnt].pitch[pitchCnt] = 0u;
        }
        mInfo->appMulWinFmt[cnt].dataFormat = FVID2_DF_YUV422I_YUYV;
        mInfo->appMulWinFmt[cnt].bpp = FVID2_BPP_BITS16;
        mInfo->appMulWinFmt[cnt].priority = 0u;
    }

    /* Initialize VPDMA mosaic params */
    mInfo->mulWinPrms.splitWindows = NULL;
    mInfo->mulWinPrms.maxNumSplitWindows = 0u;
    mInfo->mulWinPrms.frameWidth = VCORE_DEI_DEFAULT_WIDTH;
    mInfo->mulWinPrms.frameHeight = VCORE_DEI_DEFAULT_HEIGHT;

    return;
}
