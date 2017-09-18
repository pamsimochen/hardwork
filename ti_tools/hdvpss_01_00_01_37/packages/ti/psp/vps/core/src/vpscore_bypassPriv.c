/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


/**
 *  \file vpscore_bypassPriv.c
 *
 *  \brief VPS Bypass Core internal file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vpdmaMosaic.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
#include <ti/psp/vps/core/src/vpscore_bypassPriv.h>

#include <ti/psp/platforms/vps_platform.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *  the instances. Each of the channels in an instance is assumed to be
 *  shared across all handles of that instance.
 *  All the handles will allocate channel objects from this pool only.
 *  Instead of allocating VCORE_BP_MAX_HANDLES * VCORE_BP_MAX_CHANNELS
 *  objects per instance, only the below number of objects are allocated
 *  to save memory.
 */
#define VCORE_BP_MAX_CHANNEL_OBJECTS    (VCORE_BP_MAX_CHANNELS *               \
                                         VCORE_BP_INST_MAX)

/**
 *  \brief Maximum number of mosaic object to allocate which is used in
 *  dynamic mosaic layout change.
 */
#define VCORE_BP_MAX_MOSAIC_MEM_POOL    (VCORE_BP_MAX_MULTIWIN_SETTINGS *      \
                                         VCORE_BP_INST_MAX)

/** \brief Time out to be used in sem pend. */
#define VCORE_BP_SEM_TIMEOUT            (BIOS_WAIT_FOREVER)

/** \brief Default width used for initializing format structure. */
#define VCORE_BP_DEFAULT_WIDTH          (720u)
/** \brief Default height used for initializing format structure. */
#define VCORE_BP_DEFAULT_HEIGHT         (480u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct Vcore_BpPoolObj
 *  Memory pool object containing all the statically allocated
 *  objects - used structure to avoid multiple global variables.
 */
typedef struct
{
    Vcore_BpChObj           chMemPool[VCORE_BP_MAX_CHANNEL_OBJECTS];
    /**< Memory pool for the channel objects. */
    UInt32                  chMemFlag[VCORE_BP_MAX_CHANNEL_OBJECTS];
    /**< The flag variable represents whether a channel memory is allocated
         or not. */
    VpsUtils_PoolParams     chPoolPrm;
    /**< Pool params for descriptor pool memory. */

    Vcore_BpMosaicInfo      mInfoMemPool[VCORE_BP_MAX_MOSAIC_MEM_POOL];
    /**< Memory pool for the mosaic info objects. */
    UInt32                  mInfoMemFlag[VCORE_BP_MAX_MOSAIC_MEM_POOL];
    /**< The flag variable represents whether a mosaic object is allocated
         or not. */
    VpsUtils_PoolParams     mInfoPoolPrm;
    /**< Pool params for mosaic pool memory. */

    Semaphore_Handle        lockSem;
    /**< Semaphore for protecting allocation and freeing of memory pool
         objects at open/close time. */
} Vcore_BpPoolObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static UInt32 vcoreBpIsValidLayoutId(UInt32 layoutId);

static Vcore_BpMosaicInfo *vcoreBpAllocMosaicObj(Vcore_BpChObj *chObj,
                                                 UInt32 *layoutId);
static Int32 vcoreBpFreeMosaicObj(Vcore_BpInstObj *instObj,
                                  Vcore_BpChObj *chObj,
                                  UInt32 layoutId);

static Int32 vcoreBpAllocFreeCh(Vcore_BpInstObj *instObj, UInt32 numFreeCh);
static Int32 vcoreBpFreeFreeCh(Vcore_BpInstObj *instObj,
                               Vcore_BpMosaicInfo *mInfo);

static void vcoreBpSetDefChInfo(Vcore_BpChObj *chObj, UInt32 chNum);
static void vcoreBpSetDefMosaicInfo(Vcore_BpMosaicInfo *mInfo);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief Bypass path instance objects. */
static Vcore_BpInstObj VcoreBpInstObjs[VCORE_BP_INST_MAX];

/**
 *  \brief Pool objects used for storing pool memories, pool flags and pool
 *  handles.
 */
static Vcore_BpPoolObj VcoreBpPoolObjs;


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vcoreBpPrivInit
 *  \brief Initializes Bypass core objects, allocates memory etc.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vcoreBpPrivInit(UInt32 numInst, const Vcore_BpInitParams *initPrms)
{
    Int32               retVal = VPS_SOK;
    UInt32              instCnt, hCnt, chCnt;
    Vcore_BpInstObj    *instObj;
    Semaphore_Params    semParams;

    /* Check for errors */
    GT_assert(BpCoreTrace, (numInst <= VCORE_BP_INST_MAX));
    GT_assert(BpCoreTrace, (NULL != initPrms));

    /* Init pool objects */
    VpsUtils_initPool(
        &VcoreBpPoolObjs.chPoolPrm,
        (Void *) VcoreBpPoolObjs.chMemPool,
        VCORE_BP_MAX_CHANNEL_OBJECTS,
        sizeof (Vcore_BpChObj),
        VcoreBpPoolObjs.chMemFlag,
        BpCoreTrace);
    VpsUtils_initPool(
        &VcoreBpPoolObjs.mInfoPoolPrm,
        (Void *) VcoreBpPoolObjs.mInfoMemPool,
        VCORE_BP_MAX_MOSAIC_MEM_POOL,
        sizeof (Vcore_BpMosaicInfo),
        VcoreBpPoolObjs.mInfoMemFlag,
        BpCoreTrace);

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    VcoreBpPoolObjs.lockSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == VcoreBpPoolObjs.lockSem)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        for (instCnt = 0u; instCnt < numInst; instCnt++)
        {
            /* Initialize instance object members */
            instObj = &VcoreBpInstObjs[instCnt];
            instObj->instId = initPrms[instCnt].instId;
            instObj->numHandle = initPrms[instCnt].maxHandle;
            instObj->vpdmaPath = initPrms[instCnt].vpdmaPath;
            /* Get the VPDMA channels */
            for (chCnt = 0u; chCnt < VCORE_BP_MAX_VPDMA_CH; chCnt++)
            {
                instObj->vpdmaCh[chCnt] = initPrms[instCnt].vpdmaCh[chCnt];
            }
            instObj->openCnt = 0u;
            instObj->curMode = VCORE_OPMODE_INVALID;
            instObj->numVpdmaCh = VCORE_BP_MAX_VPDMA_CH;
            instObj->numFreeCh = 0u;
            VpsUtils_memset(
                instObj->freeChList,
                0u,
                sizeof(instObj->freeChList));

            /* Mark all handles as free */
            for (hCnt = 0u; hCnt < VCORE_BP_MAX_HANDLES; hCnt++)
            {
                instObj->hObjs[hCnt].isUsed = FALSE;
            }
        }
    }

    return (retVal);
}



/**
 *  vcoreBpPrivDeInit
 *  \brief Deallocates memory allocated by init function.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vcoreBpPrivDeInit(void)
{
    UInt32              instCnt;
    Vcore_BpInstObj    *instObj;

    for (instCnt = 0u; instCnt < VCORE_BP_INST_MAX; instCnt++)
    {
        instObj = &VcoreBpInstObjs[instCnt];
        instObj->instId = VCORE_BP_INST_0;
        instObj->curMode = VCORE_OPMODE_INVALID;
        instObj->numHandle = 0u;
    }

    /* Delete the Semaphore created for pool objects */
    if (NULL != VcoreBpPoolObjs.lockSem)
    {
        Semaphore_delete(&VcoreBpPoolObjs.lockSem);
        VcoreBpPoolObjs.lockSem = NULL;
    }

    return (VPS_SOK);
}



/**
 *  vcoreBpCreateLayout
 *  \brief Creates a layout according to the multi-window parameter and assigns
 *  a valid layout ID to layoutId parameter on success.
 */
Int32 vcoreBpCreateLayout(Vcore_BpInstObj *instObj,
                          Vcore_BpChObj *chObj,
                          const Vps_MultiWinParams *multiWinPrms,
                          UInt32 *layoutId)
{
    Int32                       retVal = VPS_SOK;
    UInt32                      numFreeCh, vpdmaChCnt;
    UInt32                      winCnt, bufOffset;
    Vcore_BpMosaicInfo         *mInfo = NULL;
    const Vps_WinFormat        *winFmt;
    VpsHal_VpdmaMosaicWinFmt   *mulWinFmt;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
    UInt32                      numAbortDesc;
    Bool                        isStartOfRow;
#endif

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != chObj));
    GT_assert(BpCoreTrace, (NULL != multiWinPrms));
    GT_assert(BpCoreTrace, (NULL != layoutId));

    /* Allocate a mosaic object */
    mInfo = vcoreBpAllocMosaicObj(chObj, layoutId);
    if (NULL == mInfo)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Mosaic object memory alloc failed!!\n");
        retVal = VPS_EALLOC;
    }

    if (VPS_SOK == retVal)
    {
        /* Copy the application configuration to core object */
        mInfo->appMulWinPrms.channelNum = multiWinPrms->channelNum;
        mInfo->appMulWinPrms.numWindows = multiWinPrms->numWindows;
        mInfo->appMulWinPrms.winFmt = &mInfo->appMulWinFmt[0u];
        GT_assert(BpCoreTrace,
            (multiWinPrms->numWindows <= VCORE_BP_MAX_MOSAIC_WINDOW));
        VpsUtils_memcpy(
            &mInfo->appMulWinFmt[0u],
            multiWinPrms->winFmt,
            (sizeof(Vps_WinFormat) * multiWinPrms->numWindows));

        /* Initialize VPDMA mosaic params */
        mInfo->mulWinPrms.frameWidth = chObj->coreFmt.frameWidth;
        mInfo->mulWinPrms.frameHeight = chObj->coreFmt.frameHeight;
        mInfo->mulWinPrms.splitWindows = &mInfo->mulWinFmt[0u];
        mInfo->mulWinPrms.maxNumSplitWindows = VCORE_BP_MAX_MOSAIC_WINDOW;

        /* Setup mosaic configuration using VPDMA HAL */
        mInfo->mulWinPrms.getNoWinsPerRow = FALSE;
        mInfo->mulWinPrms.numWindowsPerRow = NULL;
        retVal = VpsHal_vpdmaSetupMosaicCfg(
                     &mInfo->appMulWinPrms,
                     &mInfo->mulWinPrms);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
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
            GT_assert(BpCoreTrace, (NULL != winFmt));

            /* Assuming 422 interleaved data - 2 bytes per pixel */
            bufOffset = (mulWinFmt->bufferOffsetX * 2u);
            if ((bufOffset & (VPSHAL_VPDMA_BUF_ADDR_ALIGN - 1u)) != 0u)
            {
                GT_3trace(BpCoreTrace, GT_ERR,
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
        numFreeCh =
            mInfo->mulWinPrms.numFreeChans * VCORE_BP_NUM_DESC_PER_WINDOW;
        if (numFreeCh > VCORE_BP_MAX_VPDMA_FREE_CH)
        {
            GT_2trace(BpCoreTrace, GT_ERR,
                "Number of VPDMA free channels %d is more than supported %d\n",
                numFreeCh,
                VCORE_BP_MAX_VPDMA_FREE_CH);
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Check if number of rows is more than what is supported */
        if (mInfo->mulWinPrms.numRows > VCORE_BP_MAX_MOSAIC_ROW)
        {
            GT_2trace(BpCoreTrace, GT_ERR,
                "Number of rows %d is more than supported %d\n",
                mInfo->mulWinPrms.numRows,
                VCORE_BP_MAX_MOSAIC_ROW);
            retVal = VPS_EOUT_OF_RANGE;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Allocate free channels */
        retVal = vcoreBpAllocFreeCh(instObj, numFreeCh);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR, "Alloc free channels failed!!\n");
        }
        else
        {
            mInfo->numFreeCh = numFreeCh;
        }
    }

    if (VPS_SOK == retVal)
    {
        /* Dummy descriptor is not needed if frame size and last window
         * bottom-right coordinates match */
        mInfo->numExtraDesc = 0u;
        if (TRUE == mInfo->mulWinPrms.isDummyNeeded)
        {
            mInfo->numExtraDesc += VCORE_BP_NUM_DESC_PER_WINDOW;
        }
        /* Add data descriptor for SOCH and abort of actual channel
         * in display mode */
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            mInfo->numExtraDesc += VCORE_BP_NUM_DESC_PER_WINDOW;
        }

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        /* Add the abort descriptors required for each row's last window
         * free channel */
        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            /* Since data descriptor size is twice that of control descriptor,
             * divide by 2 (Add 1 for odd values cases). */
            mInfo->numExtraDesc += ((numAbortDesc + 1u) / 2u);

            GT_2trace(BpCoreTrace, GT_DEBUG,
                "Number of free channels abort desc: %d (Extra Desc: %d)\n",
                numAbortDesc,
                (numAbortDesc + 1u) / 2u);
        }
#endif

        /* Set the number of data descriptors needed for mosaic mode */
        mInfo->descInfo.numInDataDesc =
            mInfo->mulWinPrms.numWindowsFirstRow * VCORE_BP_NUM_DESC_PER_WINDOW;

        GT_assert(BpCoreTrace, (mInfo->mulWinPrms.numWindowsFirstRow
            <= mInfo->mulWinPrms.numSplitWindows));
        mInfo->descInfo.numMultiWinDataDesc =
            mInfo->numExtraDesc + ((mInfo->mulWinPrms.numSplitWindows -
                                        mInfo->mulWinPrms.numWindowsFirstRow) *
                                    VCORE_BP_NUM_DESC_PER_WINDOW);

        /* Copy the SOCH desc info from main channel structure */
        mInfo->descInfo.numChannels = chObj->descInfo.numChannels;
        for(vpdmaChCnt = 0; vpdmaChCnt < chObj->descInfo.numChannels;
            vpdmaChCnt++)
        {
            mInfo->descInfo.socChNum[vpdmaChCnt] =
                chObj->descInfo.socChNum[vpdmaChCnt];
        }
    }

    /* If error, free-up the already allocated objects */
    if ((VPS_SOK != retVal) && (NULL != mInfo))
    {
        vcoreBpFreeMosaicObj(instObj, chObj, *layoutId);
        *layoutId = VCORE_INVALID_LAYOUT_ID;
    }

    return (retVal);
}



/**
 *  vcoreBpDeleteLayout
 *  \brief Free-up the mosaic layout object corresponding to the layout ID.
 *  Also frees-up any alocated VPDMA free channels.
 */
Int32 vcoreBpDeleteLayout(Vcore_BpInstObj *instObj,
                          Vcore_BpChObj *chObj,
                          UInt32 layoutId)
{
    Int32           retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != chObj));

    /* Free-up the allocated mosaic object */
    retVal = vcoreBpFreeMosaicObj(instObj, chObj, layoutId);

    return (retVal);
}



/**
 *  vcoreBpProgramDesc
 *  \brief Programs the data descriptor depending on the parameters set.
 *  This is used in case of non-mosaic mode of operation.
 */
UInt32 vcoreBpProgramDesc(const Vcore_BpInstObj *instObj,
                          const Vcore_BpChObj *chObj,
                          const Vcore_DescMem *descMem)
{
    Int32                       retVal = VPS_SOK;
    Void                       *descPtr;
    UInt32                      cnt;
    VpsHal_VpdmaInDescParams    inDescPrm;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != chObj));
    GT_assert(BpCoreTrace, (NULL != descMem));

    /* Set the VPDMA parameters */
    inDescPrm.channel = instObj->vpdmaCh[VCORE_BP_422P_IDX];
    inDescPrm.nextChannel = instObj->vpdmaCh[VCORE_BP_422P_IDX];
    inDescPrm.transWidth = chObj->coreFmt.fmt.width;
    inDescPrm.frameWidth = chObj->coreFmt.frameWidth;
    inDescPrm.startX = chObj->coreFmt.startX;
    inDescPrm.notify = FALSE;
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        inDescPrm.priority = VPS_CFG_DISP_VPDMA_PRIORITY;
    }
    else
    {
        inDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
    }
    inDescPrm.memType = (VpsHal_VpdmaMemoryType) chObj->coreFmt.memType;
    inDescPrm.is1DMem = FALSE;
    if (FVID2_DF_YUV422I_YUYV == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_YC422;
    }
    else if (FVID2_DF_YUV422I_YVYU == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_YCb422;
    }
    else if (FVID2_DF_YUV422I_UYVY == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_CY422;
    }
    else if (FVID2_DF_YUV422I_VYUY == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_CbY422;
    }
    else
    {
        GT_assert(BpCoreTrace, (0));
    }
    inDescPrm.lineStride = chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX];

    /* Depending on scan format configure height parameters */
    if (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat)
    {
        inDescPrm.transHeight = chObj->coreFmt.fmt.height >> 1u;
        inDescPrm.frameHeight = chObj->coreFmt.frameHeight >> 1u;
        inDescPrm.startY = chObj->coreFmt.startY >> 1u;
        if (TRUE == chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX])
        {
            /* Fields are merged - read alternate lines */
            inDescPrm.lineSkip = VPSHAL_VPDMA_LS_2;
        }
        else
        {
            /* Fields are separated - read next line */
            inDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
        }
    }
    else
    {
        inDescPrm.transHeight = chObj->coreFmt.fmt.height;
        inDescPrm.frameHeight = chObj->coreFmt.frameHeight;
        inDescPrm.startY = chObj->coreFmt.startY;
        inDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
    }

    /* Program the descriptors */
    descPtr = descMem->inDataDesc[0u];
    GT_assert(BpCoreTrace, (NULL != descPtr));
    retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr, &inDescPrm);
    if (VPS_SOK != retVal)
    {
        GT_0trace(BpCoreTrace, GT_ERR,
            "VPDMA Descriptor Creation Failed!!\n");
    }
    else
    {
        /* Print the created descriptor memory */
        GT_0trace(BpCoreTrace, GT_DEBUG, "Data Descrirptor:\n");
        GT_0trace(BpCoreTrace, GT_DEBUG, "-----------------\n");
        VpsHal_vpdmaPrintDesc(descPtr, BpCoreTrace);
    }

    if (VPS_SOK == retVal)
    {
        /* Make a dummy data descriptor at the last with zero transfer size
         * if required */
        if (TRUE == chObj->isDummyNeeded)
        {
            inDescPrm.transWidth = 0u;
            inDescPrm.transHeight = 0u;
            inDescPrm.startX = inDescPrm.frameWidth;
            inDescPrm.startY = inDescPrm.frameHeight;
            inDescPrm.channel = instObj->vpdmaCh[VCORE_BP_422P_IDX];
            inDescPrm.nextChannel = instObj->vpdmaCh[VCORE_BP_422P_IDX];

            /* Dummy data descriptor is only descriptor in multiWinDataDesc */
            descPtr = descMem->multiWinDataDesc[0u];
            GT_assert(BpCoreTrace, (NULL != descPtr));

            /* Form the descriptor */
            retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr, &inDescPrm);
            if (VPS_SOK != retVal)
            {
                GT_0trace(BpCoreTrace, GT_ERR,
                    "VPDMA Descriptor Creation Failed!!\n");
            }
            /* Set some address to make valid FID field */
            VpsHal_vpdmaSetAddress(descPtr, 0u, descPtr);

            /* Set mosaic mode bit for dummy descriptor */
#ifdef VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE
            VpsHal_vpdmaSetMosaicMode(descPtr, TRUE);
#else
            VpsHal_vpdmaSetMosaicMode(descPtr, FALSE);
#endif

            /* Print the created descriptor memory */
            GT_0trace(BpCoreTrace, GT_DEBUG, "Data Descrirptor:\n");
            GT_0trace(BpCoreTrace, GT_DEBUG, "-----------------\n");
            VpsHal_vpdmaPrintDesc(descPtr, BpCoreTrace);
        }
        else if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            /* Use dummy descriptor instead of dummy data descriptor */
            descPtr = descMem->multiWinDataDesc[0u];
            GT_assert(BpCoreTrace, (NULL != descPtr));
            for (cnt = 0u; cnt < (VCORE_BP_NUM_DESC_PER_WINDOW * 2u); cnt++)
            {
                VpsHal_vpdmaCreateDummyDesc(descPtr);
                descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
        }

        if (VCORE_OPMODE_DISPLAY == instObj->curMode)
        {
            /* Use SOCH and Abort on actual channel */
            descPtr = descMem->multiWinDataDesc[1u];
            GT_assert(BpCoreTrace, (NULL != descPtr));
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOC
            VpsHal_vpdmaCreateSOCCtrlDesc(
                descPtr,
                instObj->vpdmaCh[VCORE_BP_422P_IDX],
                VPSHAL_VPDMA_SOC_EOF,
                0u,
                0u);
#else
#ifdef VPS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS
            VpsHal_vpdmaCreateSOCHCtrlDesc(
                descPtr,
                instObj->vpdmaCh[VCORE_BP_422P_IDX]);
#else
            VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif
#endif

            descPtr = (UInt8 *) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
            VpsHal_vpdmaCreateAbortCtrlDesc(
                descPtr,
                instObj->vpdmaCh[VCORE_BP_422P_IDX]);
#else
            VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif

        }
    }

    return (retVal);
}



/**
 *  vcoreBpProgramMosaicDesc
 *  \brief Configures the data descriptor memory depending on the format and
 *  mosaic configuration. This is used in case of mosaic mode of operation.
 */
UInt32 vcoreBpProgramMosaicDesc(const Vcore_BpInstObj *instObj,
                                Vcore_BpChObj *chObj,
                                UInt32 layoutId,
                                const Vcore_DescMem *descMem)
{
    Int32                           retVal = VPS_SOK;
    UInt32                          winCnt;
    UInt32                          descOffset;
    Void                           *descPtr;
    Vcore_BpMosaicInfo             *mInfo;
    const Vps_WinFormat            *winFmt;
    VpsHal_VpdmaMosaicWinFmt       *mulWinFmt;
    VpsHal_VpdmaInDescParams        inDescPrm;
    VpsHal_VpdmaChannel             nextCh = VPSHAL_VPDMA_CHANNEL_INVALID;
    VpsHal_VpdmaChannel             sochVpdmaChan;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
    UInt32                          cnt, numAbortDesc;
    VpsHal_VpdmaChannel             abortFreeChan[VCORE_BP_MAX_MOSAIC_ROW];
#endif

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != chObj));
    GT_assert(BpCoreTrace, (NULL != descMem));

    /* Set the VPDMA parameters */
    sochVpdmaChan = instObj->vpdmaCh[VCORE_BP_422P_IDX];
    if (FVID2_DF_YUV422I_YUYV == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_YC422;
    }
    else if (FVID2_DF_YUV422I_YVYU == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_YCb422;
    }
    else if (FVID2_DF_YUV422I_UYVY == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_CY422;
    }
    else if (FVID2_DF_YUV422I_VYUY == chObj->coreFmt.fmt.dataFormat)
    {
        inDescPrm.dataType = VPSHAL_VPDMA_CHANDT_CbY422;
    }
    else
    {
        GT_assert(BpCoreTrace, (0));
    }
    inDescPrm.frameWidth = chObj->coreFmt.frameWidth;
    inDescPrm.notify = FALSE;
    if (VCORE_OPMODE_DISPLAY == instObj->curMode)
    {
        inDescPrm.priority = VPS_CFG_DISP_VPDMA_PRIORITY;
    }
    else
    {
        inDescPrm.priority = VPS_CFG_M2M_VPDMA_PRIORITY_RD;
    }
    inDescPrm.memType = (VpsHal_VpdmaMemoryType) chObj->coreFmt.memType;
    inDescPrm.is1DMem = FALSE;
    if (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat)
    {
        inDescPrm.frameHeight = chObj->coreFmt.frameHeight >> 1u;
        if (TRUE == chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX])
        {
            /* Fields are merged - read alternate lines */
            inDescPrm.lineSkip = VPSHAL_VPDMA_LS_2;
        }
        else
        {
            /* Fields are separated - read next line */
            inDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
        }
    }
    else
    {
        inDescPrm.frameHeight = chObj->coreFmt.frameHeight;
        inDescPrm.lineSkip = VPSHAL_VPDMA_LS_1;
    }

    /* Get the layout object for the requested layout */
    mInfo = vcoreBpGetLayoutObj(chObj, layoutId);
    if (NULL == mInfo)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid layout ID!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }

    if (VPS_SOK == retVal)
    {
        descOffset = 0u;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        numAbortDesc = 0u;
#endif

        /* Configure each window/descriptor */
        for (winCnt = 0u; winCnt < mInfo->mulWinPrms.numSplitWindows; winCnt++)
        {
            if (winCnt < mInfo->mulWinPrms.numWindowsFirstRow)
            {
                /* Get descriptor pointer for the first row */
                descPtr = descMem->inDataDesc[descOffset];
            }
            else
            {
                if (winCnt == mInfo->mulWinPrms.numWindowsFirstRow)
                {
                    /* Reset the count when moving to 2nd row */
                    descOffset = 0u;
                }
                /* Get descriptor pointer for the non-first row */
                descPtr = descMem->multiWinDataDesc[descOffset];
            }
            GT_assert(BpCoreTrace, (NULL != descPtr));

            /* Configure the parameters of each mosaic window */
            mulWinFmt = &mInfo->mulWinFmt[winCnt];
            winFmt = mulWinFmt->ipWindowFmt;
            GT_assert(BpCoreTrace, (NULL != winFmt));

            /* Note the free channel of the last window */
            if ((TRUE == mulWinFmt->isLastWin) &&
                (VCORE_OPMODE_DISPLAY == instObj->curMode) &&
                (FALSE == mInfo->mulWinPrms.isDummyNeeded) &&
                (FALSE == mulWinFmt->isActualChanReq))
            {
                GT_assert(BpCoreTrace,
                    (mulWinFmt->freeChanIndex < instObj->numFreeCh));
                sochVpdmaChan = (VpsHal_VpdmaChannel)
                    instObj->freeChList[mulWinFmt->freeChanIndex];
            }

            inDescPrm.transWidth = mulWinFmt->width;
            inDescPrm.startX = mulWinFmt->startX;
            /* Note: Pitch of split window is same as original buffer pitch!! */
            inDescPrm.lineStride = winFmt->pitch[FVID2_YUV_INT_ADDR_IDX];
            if (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat)
            {
                inDescPrm.transHeight = mulWinFmt->height >> 1u;
                inDescPrm.startY = mulWinFmt->startY >> 1u;
            }
            else
            {
                inDescPrm.transHeight = mulWinFmt->height;
                inDescPrm.startY = mulWinFmt->startY;
            }

            /* Determine next channel.
             * Caution: This should be done before assigning channel done in the
             * the next step */
            if (VPSHAL_VPDMA_CHANNEL_INVALID != nextCh)
            {
                /* Use the previously assigned next channel */
                inDescPrm.nextChannel = nextCh;
            }
            else
            {
                inDescPrm.nextChannel = instObj->vpdmaCh[VCORE_BP_422P_IDX];

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
                /* Get the free channel number for the abort descriptors */
                if (FALSE == mulWinFmt->isActualChanReq)
                {
                    GT_assert(BpCoreTrace,
                        (numAbortDesc < VCORE_BP_MAX_MOSAIC_ROW));
                    GT_assert(BpCoreTrace,
                        (mulWinFmt->freeChanIndex < instObj->numFreeCh));

                    abortFreeChan[numAbortDesc] = (VpsHal_VpdmaChannel)
                        instObj->freeChList[mulWinFmt->freeChanIndex];
                    numAbortDesc++;
                }
#endif
            }

            /* Determine channel depending on flag */
            if (TRUE == mulWinFmt->isActualChanReq)
            {
                /* Use actual channel */
                inDescPrm.channel = instObj->vpdmaCh[VCORE_BP_422P_IDX];

                /* Row is ending - reset next channel */
                nextCh = VPSHAL_VPDMA_CHANNEL_INVALID;
            }
            else
            {
                /* Use free channel */
                GT_assert(BpCoreTrace,
                    (mulWinFmt->freeChanIndex < instObj->numFreeCh));
                inDescPrm.channel = (VpsHal_VpdmaChannel)
                    instObj->freeChList[mulWinFmt->freeChanIndex];

                /* Next channel of next window is this free channel.
                 * This assumes right to left sorting of split windows */
                nextCh = (VpsHal_VpdmaChannel)
                    instObj->freeChList[mulWinFmt->freeChanIndex];
            }

            /* Configure data descriptor */
            retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr, &inDescPrm);
            if (VPS_SOK != retVal)
            {
                GT_0trace(BpCoreTrace, GT_ERR,
                    "VPDMA Descriptor Creation Failed!!\n");
                break;
            }

            /* Set mosaic mode bit for the non-first row base channels */
            VpsHal_vpdmaSetMosaicMode(descPtr, FALSE);
#ifdef VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE
            if ((winCnt >= mInfo->mulWinPrms.numWindowsFirstRow) &&
                (TRUE == mulWinFmt->isActualChanReq))
            {
                VpsHal_vpdmaSetMosaicMode(descPtr, TRUE);
            }
#endif

            /* Print the created descriptor memory */
            GT_1trace(BpCoreTrace, GT_DEBUG,
                "Data Descrirptor %d:\n", winCnt);
            GT_0trace(BpCoreTrace, GT_DEBUG,
                "---------------------\n");
            VpsHal_vpdmaPrintDesc(descPtr, BpCoreTrace);

            descOffset++;
        }
    }

    /* Make a dummy descriptor at the last with zero transfer size
     * if required */
    if ((VPS_SOK == retVal) && (TRUE == mInfo->mulWinPrms.isDummyNeeded))
    {
        /* When only one row is present, descOffset should be reset as it
         * wouldn't have been reset in above for loop */
        if (mInfo->mulWinPrms.numSplitWindows ==
            mInfo->mulWinPrms.numWindowsFirstRow)
        {
            descOffset = 0u;
        }

        inDescPrm.transWidth = 0u;
        inDescPrm.transHeight = 0u;
        inDescPrm.startX = inDescPrm.frameWidth;
        inDescPrm.startY = inDescPrm.frameHeight;
        inDescPrm.channel = instObj->vpdmaCh[VCORE_BP_422P_IDX];
        inDescPrm.nextChannel = instObj->vpdmaCh[VCORE_BP_422P_IDX];

        descPtr = descMem->multiWinDataDesc[descOffset];
        GT_assert(BpCoreTrace, (NULL != descPtr));

        /* Form the descriptor */
        retVal = VpsHal_vpdmaCreateInBoundDataDesc(descPtr, &inDescPrm);
        if (VPS_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
                "VPDMA Descriptor Creation Failed!!\n");
        }
        /* Set some address to make valid FID field */
        VpsHal_vpdmaSetAddress(descPtr, 0u, descPtr);

        /* Set mosaic mode bit for dummy descriptor */
#ifdef VPS_CFG_VPDMA_MOSIAC_SET_MOSAIC_MODE
        VpsHal_vpdmaSetMosaicMode(descPtr, TRUE);
#else
        VpsHal_vpdmaSetMosaicMode(descPtr, FALSE);
#endif

        /* Print the created descriptor memory */
        GT_1trace(BpCoreTrace, GT_DEBUG, "Data Descrirptor %d:\n", winCnt);
        GT_0trace(BpCoreTrace, GT_DEBUG, "--------------------\n");
        VpsHal_vpdmaPrintDesc(descPtr, BpCoreTrace);

        winCnt++;
        descOffset++;
    }

    /* Put SOCH on the last window channel - This could be actual channel if
     * the last channel is actual channel or dummy data desc is needed.
     * Otherwise this will be the last window free channel. */
    if ((VPS_SOK == retVal) && (VCORE_OPMODE_DISPLAY == instObj->curMode))
    {
        /* When only one row is present, descOffset should be reset as it
         * wouldn't have been reset in above for loop */
        if ((FALSE == mInfo->mulWinPrms.isDummyNeeded) &&
            (mInfo->mulWinPrms.numSplitWindows ==
             mInfo->mulWinPrms.numWindowsFirstRow))
        {
            descOffset = 0u;
        }

        descPtr = descMem->multiWinDataDesc[descOffset];
        GT_assert(BpCoreTrace, (NULL != descPtr));

        GT_2trace(BpCoreTrace, GT_DEBUG,
            "mInfo: 0x%p, Free Channel Number: %d\n", mInfo, sochVpdmaChan);

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOC
        VpsHal_vpdmaCreateSOCCtrlDesc(
            descPtr,
            instObj->vpdmaCh[VCORE_BP_422P_IDX],
            VPSHAL_VPDMA_SOC_EOF,
            0u,
            0u);
#else
#ifdef VPS_CFG_VPDMA_MOSAIC_ENABLE_WORKAROUNDS
        /* Form SOCH for the last channel */
        VpsHal_vpdmaCreateSOCHCtrlDesc(descPtr, sochVpdmaChan);
#else
        VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif
#endif

        /* Since data descriptor size is twice that of control descriptor,
         * use the 2nd part of descriptor to put abort on actual channel. */
        descPtr = (Void *) ((UInt32) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE);
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        VpsHal_vpdmaCreateAbortCtrlDesc(
            descPtr,
            instObj->vpdmaCh[VCORE_BP_422P_IDX]);
#else
        VpsHal_vpdmaCreateDummyDesc(descPtr);
#endif

        descOffset++;

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_ABORT
        /* Put abort descriptor on each row's last window free channel */
        for (cnt = 0u; cnt < numAbortDesc; cnt++)
        {
            if ((cnt & 1u) == 0u)
            {
                descPtr = descMem->multiWinDataDesc[descOffset];
                GT_assert(BpCoreTrace, (NULL != descPtr));
            }
            else
            {
                GT_assert(BpCoreTrace, (NULL != descPtr));
                descPtr =
                    (Void *) ((UInt32) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE);
                descOffset++;
            }

            GT_1trace(BpCoreTrace, GT_DEBUG,
                "Abort Free Channel: %d\n", abortFreeChan[cnt]);

            VpsHal_vpdmaCreateAbortCtrlDesc(descPtr, abortFreeChan[cnt]);
        }
        if (numAbortDesc & 1u)
        {
            GT_assert(BpCoreTrace, (NULL != descPtr));
            descPtr =
                (Void *) ((UInt32) descPtr + VPSHAL_VPDMA_CTRL_DESC_SIZE);
            /* Use dummy descriptor for odd number of abort descriptors */
            VpsHal_vpdmaCreateDummyDesc(descPtr);

            descOffset++;
        }
#endif
    }

    return (retVal);
}



/**
 *  vcoreBpCheckFmt
 *  \brief Checks whether a given parameters is valid or not.
 *  Returns VPS_SOK if everything is fine else returns error value.
 */
Int32 vcoreBpCheckFmt(const Vcore_BpInstObj *instObj,
                      const Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != coreFmt));

    if (Vps_platformGetCpuRev() <= VPS_PLATFORM_CPU_REV_1_1)
    {
        /* Check whether data format is supported or not */
#ifdef _IPNC_HW_PLATFORM_
        if ((FVID2_DF_YUV422I_YUYV != coreFmt->fmt.dataFormat) &&
		    (FVID2_DF_YUV422I_UYVY != coreFmt->fmt.dataFormat))
#else
        if (FVID2_DF_YUV422I_YUYV != coreFmt->fmt.dataFormat)
#endif
        {
            GT_1trace(BpCoreTrace, GT_ERR,
                "Data format (%d) not supported!!\n", coreFmt->fmt.dataFormat);
            retVal = VPS_EINVALID_PARAMS;
        }
    }
    else
    {
        /* Additional data formats are supported on >= 2.0 only */
        if (!((FVID2_DF_YUV422I_YUYV == coreFmt->fmt.dataFormat) ||
              (FVID2_DF_YUV422I_YVYU == coreFmt->fmt.dataFormat) ||
              (FVID2_DF_YUV422I_UYVY == coreFmt->fmt.dataFormat) ||
              (FVID2_DF_YUV422I_VYUY == coreFmt->fmt.dataFormat)))
        {
            GT_1trace(BpCoreTrace, GT_ERR,
                "Data format (%d) not supported!!\n", coreFmt->fmt.dataFormat);
            retVal = VPS_EINVALID_PARAMS;
        }
    }

    /* Check whether the pitch is valid */
    if (coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX] < (coreFmt->fmt.width * 2u))
    {
        GT_2trace(BpCoreTrace, GT_ERR,
            "Pitch (%d) less than Width (%d) in bytes!!\n",
            coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX],
            (coreFmt->fmt.width * 2u));
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check whether window width/startX is even */
    if ((coreFmt->fmt.width & 0x01u) || (coreFmt->startX & 0x01u))
    {
        GT_2trace(BpCoreTrace, GT_ERR,
            "Width(%d)/StartX(%d) can't be odd!!\n",
            coreFmt->fmt.width,
            coreFmt->startX);
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check whether the active window is within frame limit */
    if (coreFmt->frameWidth < (coreFmt->startX + coreFmt->fmt.width))
    {
        GT_2trace(BpCoreTrace, GT_ERR,
            "Frame Width (%d) less than Width + startX (%d)!!\n",
            coreFmt->frameWidth,
            (coreFmt->startX + coreFmt->fmt.width));
        retVal = VPS_EINVALID_PARAMS;
    }
    if (coreFmt->frameHeight < (coreFmt->startY + coreFmt->fmt.height))
    {
        GT_2trace(BpCoreTrace, GT_ERR,
            "Frame Height (%d) less than Height + startY (%d)!!\n",
            coreFmt->frameHeight,
            (coreFmt->startY + coreFmt->fmt.height));
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check if the scan format are matching */
    if ((FVID2_SF_INTERLACED == coreFmt->secScanFmt) &&
        (FVID2_SF_INTERLACED != coreFmt->fmt.scanFormat))
    {
        GT_0trace(BpCoreTrace, GT_ERR,
            "Progressive to Interlaced conversion not supported!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }
    if ((FVID2_SF_PROGRESSIVE == coreFmt->secScanFmt) &&
        (FVID2_SF_PROGRESSIVE != coreFmt->fmt.scanFormat))
    {
        GT_0trace(BpCoreTrace, GT_ERR,
            "Interlaced to Progressive conversion not supported!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        retVal = VpsHal_vpdmaIsValidSize(
                    instObj->vpdmaPath,
                    coreFmt->fmt.dataFormat,
                    (VpsHal_VpdmaMemoryType) coreFmt->memType,
                    coreFmt->fmt.width,
                    coreFmt->fmt.height);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(BpCoreTrace, GT_ERR,
                "Wrong Input Size!!\n");
        }
    }

    return (retVal);
}



/**
 *  vcoreBpCheckMosaicCfg
 *  \brief Checks whether a given mosaic config is valid or not.
 *  Returns VPS_SOK if everything is fine else returns error value.
 */
Int32 vcoreBpCheckMosaicCfg(const Vps_MultiWinParams *multiWinPrms)
{
    Int32       retVal = VPS_SOK;

    /* Check for valid number of windows */
    if (multiWinPrms->numWindows > VCORE_BP_MAX_MOSAIC_WINDOW)
    {
        GT_1trace(BpCoreTrace, GT_ERR,
            "Number of windows more than what could be supported (%d)\n",
            VCORE_BP_MAX_MOSAIC_WINDOW);
        retVal = VPS_EOUT_OF_RANGE;
    }

    return (retVal);
}



/**
 *  vcoreBpGetLayoutObj
 *  \brief Returns the layout object pointer.
 */
Vcore_BpMosaicInfo *vcoreBpGetLayoutObj(const Vcore_BpChObj *chObj,
                                        UInt32 layoutId)
{
    UInt32              layoutIdx;
    Vcore_BpMosaicInfo *mInfo = NULL;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != chObj));

    /* Check for valid layout ID */
    if (vcoreBpIsValidLayoutId(layoutId))
    {
        /* Get the mosaic object index */
        GT_assert(BpCoreTrace, (layoutId >= VCORE_START_VALID_LAYOUT_ID));
        layoutIdx = layoutId - VCORE_START_VALID_LAYOUT_ID;
        mInfo = chObj->mInfo[layoutIdx];
    }

    return (mInfo);
}



/**
 *  vcoreBpAllocHandleObj
 *  \brief Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
Vcore_BpHandleObj *vcoreBpAllocHandleObj(UInt32 instId, Vcore_OpMode mode)
{
    UInt32              instCnt, hCnt;
    Vcore_BpInstObj    *instObj = NULL;
    Vcore_BpHandleObj  *hObj = NULL;

    /* Find out the instance to which this handle belongs to */
    for (instCnt = 0u; instCnt < VCORE_BP_INST_MAX; instCnt++)
    {
        if (VcoreBpInstObjs[instCnt].instId == instId)
        {
            instObj = &VcoreBpInstObjs[instCnt];
            break;
        }
    }

    Semaphore_pend(VcoreBpPoolObjs.lockSem, VCORE_BP_SEM_TIMEOUT);

    if (NULL != instObj)
    {
        /* Check if the required mode is proper */
        if (VCORE_OPMODE_INVALID > instObj->curMode)
        {
            /* Core already opend for some mode. For display mode, only one
             * handle should be allowed. It doesn't make sense isn't it? 
             * 
             * Making instObj to NULL and removed assert.
             *
             */
            instObj = NULL;
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
        /* Update the state and return the handle */
        hObj->isUsed = TRUE;
        hObj->instObj = instObj;
        instObj->openCnt++;
        instObj->curMode = mode;
    }

    Semaphore_post(VcoreBpPoolObjs.lockSem);

    return (hObj);
}



/**
 *  vcoreBpFreeHandleObj
 *  \brief Frees-up the handle object and resets the variables.
 */
Int32 vcoreBpFreeHandleObj(Vcore_BpHandleObj *hObj)
{
    Int32               retVal = VPS_EFAIL;
    UInt32              hCnt;
    Vcore_BpInstObj    *instObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(BpCoreTrace, (NULL != instObj));

    Semaphore_pend(VcoreBpPoolObjs.lockSem, VCORE_BP_SEM_TIMEOUT);

    /* Free the handle object */
    for (hCnt = 0u; hCnt < instObj->numHandle; hCnt++)
    {
        if (hObj == &instObj->hObjs[hCnt])
        {
            /* Check if the handle is already allocated */
            GT_assert(BpCoreTrace, (TRUE == instObj->hObjs[hCnt].isUsed));

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

    Semaphore_post(VcoreBpPoolObjs.lockSem);

    return (retVal);
}



/**
 *  vcoreBpAllocChObjs
 *  \brief Allocates channel objects.
 *  Returns error if allocation failed.
 */
Int32 vcoreBpAllocChObjs(Vcore_BpHandleObj *hObj, UInt32 numCh)
{
    Int32       retVal = VPS_SOK;
    UInt32      chCnt;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != hObj));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        /* Allocate channel objects one at a time */
        hObj->chObjs[chCnt] = (Vcore_BpChObj *)
            VpsUtils_alloc(
                &VcoreBpPoolObjs.chPoolPrm,
                sizeof (Vcore_BpChObj),
                BpCoreTrace);
        if (NULL == hObj->chObjs[chCnt])
        {
            GT_1trace(BpCoreTrace, GT_ERR,
                "Channel Object memory alloc failed for channel %d!!\n",
                chCnt);
            retVal = VPS_EALLOC;
            break;
        }

        /* Set default values for the channel object */
        vcoreBpSetDefChInfo(hObj->chObjs[chCnt], chCnt);
    }

    /* If error, free-up the already allocated objects */
    if (VPS_SOK != retVal)
    {
        vcoreBpFreeChObjs(hObj, chCnt);
    }

    return (retVal);
}



/**
 *  vcoreBpFreeChObjs
 *  \brief Frees-up the channel objects.
 */
Int32 vcoreBpFreeChObjs(Vcore_BpHandleObj *hObj, UInt32 numCh)
{
    Int32               retVal = VPS_SOK;
    UInt32              chCnt, mCnt;
    Vcore_BpChObj      *chObj;
    Vcore_BpMosaicInfo *mInfo;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != hObj));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        GT_assert(BpCoreTrace, (NULL != chObj));

        /* Free all the allocated mosaic objects */
        for (mCnt = 0u; mCnt < VCORE_BP_MAX_LAYOUT_PER_CH; mCnt++)
        {
            mInfo = chObj->mInfo[mCnt];
            if (NULL != mInfo)
            {
                /* Free the VPDMA free channels */
                if (mInfo->numFreeCh > 0u)
                {
                    retVal = vcoreBpFreeFreeCh(hObj->instObj, mInfo);
                    mInfo->numFreeCh = 0u;
                }

                /* Free the mosaic object */
                retVal |= VpsUtils_free(
                              &VcoreBpPoolObjs.mInfoPoolPrm,
                              mInfo,
                              BpCoreTrace);
                chObj->mInfo[mCnt] = NULL;
            }
        }

        /* Memset the channel object before freeing */
        VpsUtils_memset(chObj, 0u, sizeof(Vcore_BpChObj));
        retVal |= VpsUtils_free(
                      &VcoreBpPoolObjs.chPoolPrm,
                      chObj,
                      BpCoreTrace);
        hObj->chObjs[chCnt] = NULL;
    }

    return (retVal);
}



/**
 *  vcoreBpIsValidLayoutId
 *  \brief Checks whether a layout ID is valid or not.
 */
static UInt32 vcoreBpIsValidLayoutId(UInt32 layoutId)
{
    UInt32      isValid = FALSE;

    if ((layoutId >= VCORE_START_VALID_LAYOUT_ID) &&
        (layoutId < (VCORE_BP_MAX_LAYOUT_PER_CH + VCORE_START_VALID_LAYOUT_ID)))
    {
        isValid = TRUE;
    }

    return (isValid);
}



/**
 *  vcoreBpAllocMosaicObj
 *  \brief Allocates a mosaic object from the memory pool and assigns it to a
 *  free mosaic info pointer in channel object.
 */
static Vcore_BpMosaicInfo *vcoreBpAllocMosaicObj(Vcore_BpChObj *chObj,
                                                 UInt32 *layoutId)
{
    Int32               retVal = VPS_SOK;
    UInt32              mCnt;
    Vcore_BpMosaicInfo *mInfo;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != chObj));
    GT_assert(BpCoreTrace, (NULL != layoutId));

    /* Allocate a free mosaic object */
    mInfo =(Vcore_BpMosaicInfo *)
        VpsUtils_alloc(
            &VcoreBpPoolObjs.mInfoPoolPrm,
            sizeof (Vcore_BpMosaicInfo),
            BpCoreTrace);
    if (NULL == mInfo)
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Mosaic object memory alloc failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        /* Set default mosaic info */
        vcoreBpSetDefMosaicInfo(mInfo);

        /* Find a free space in the channel mosaic list and store the
         * allocated object */
        retVal = VPS_EALLOC;
        for (mCnt = 0u; mCnt < VCORE_BP_MAX_LAYOUT_PER_CH; mCnt++)
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
        VpsUtils_free(&VcoreBpPoolObjs.mInfoPoolPrm, mInfo, BpCoreTrace);
        *layoutId = VCORE_INVALID_LAYOUT_ID;
    }

    return (mInfo);
}



/**
 *  vcoreBpFreeMosaicObj
 *  \brief Frees the mosaic object and frees any allocated VPDMA free channels
 *  if already allocated.
 */
static Int32 vcoreBpFreeMosaicObj(Vcore_BpInstObj *instObj,
                                  Vcore_BpChObj *chObj,
                                  UInt32 layoutId)
{
    Int32               retVal = VPS_SOK;
    UInt32              layoutIdx;
    Vcore_BpMosaicInfo *mInfo;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != chObj));
    GT_assert(BpCoreTrace, (NULL != layoutId));

    /* Check for valid layout ID */
    if (!vcoreBpIsValidLayoutId(layoutId))
    {
        GT_0trace(BpCoreTrace, GT_ERR, "Invalid layout ID!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }

    if (VPS_SOK == retVal)
    {
        /* Get the mosaic object index */
        GT_assert(BpCoreTrace, (layoutId >= VCORE_START_VALID_LAYOUT_ID));
        layoutIdx = layoutId - VCORE_START_VALID_LAYOUT_ID;

        /* Free the mosaic object */
        mInfo = chObj->mInfo[layoutIdx];
        if (NULL != mInfo)
        {
            /* Free the VPDMA free channels */
            if (mInfo->numFreeCh > 0u)
            {
                retVal = vcoreBpFreeFreeCh(instObj, mInfo);
                mInfo->numFreeCh = 0u;
            }

            /* Free the mosaic object */
            retVal |= VpsUtils_free(
                          &VcoreBpPoolObjs.mInfoPoolPrm,
                          mInfo,
                          BpCoreTrace);
            chObj->mInfo[layoutIdx] = NULL;
        }
    }

    return (retVal);
}



/**
 *  vcoreBpAllocFreeCh
 *  \brief Allocates VPDMA free channels if the new layout number of
 *  free channels is more than the already allocated free channels for an
 *  instance.
 */
static Int32 vcoreBpAllocFreeCh(Vcore_BpInstObj *instObj, UInt32 numFreeCh)
{
    Int32       retVal = VPS_SOK;
    UInt32      freeChToAllocate;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));

    Semaphore_pend(VcoreBpPoolObjs.lockSem, VCORE_BP_SEM_TIMEOUT);

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
            GT_0trace(BpCoreTrace, GT_ERR,
                "VPDMA free channel allocation failed!!\n");
        }
        else
        {
            GT_3trace(BpCoreTrace, GT_DEBUG,
                "FreeCh Alloc: %d, Prev Num FreeCh: %d, Cur Num FreeCh: %d\n",
                freeChToAllocate, instObj->numFreeCh, numFreeCh);

            instObj->numFreeCh = numFreeCh;
        }
    }

    Semaphore_post(VcoreBpPoolObjs.lockSem);

    return (retVal);
}



/**
 *  vcoreBpFreeFreeCh
 *  \brief Frees the VPDMA free channels if not used by other layouts in an
 *  instance.
 */
static Int32 vcoreBpFreeFreeCh(Vcore_BpInstObj *instObj,
                               Vcore_BpMosaicInfo *mInfo)
{
    Int32               retVal = VPS_SOK;
    UInt32              hCnt, chCnt, mCnt;
    UInt32              freeChToFree, maxNumFreeCh = 0u;
    Vcore_BpChObj      *chObj;
    Vcore_BpHandleObj  *hObj;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != instObj));
    GT_assert(BpCoreTrace, (NULL != mInfo));

    Semaphore_pend(VcoreBpPoolObjs.lockSem, VCORE_BP_SEM_TIMEOUT);

    /* Find the max free channel in an instance object excluding this layout */
    for (hCnt = 0u; hCnt < VCORE_BP_MAX_HANDLES; hCnt++)
    {
        hObj = &instObj->hObjs[hCnt];
        if (TRUE == hObj->isUsed)
        {
            for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
            {
                chObj = hObj->chObjs[chCnt];
                /* NULL pointer check */
                GT_assert(BpCoreTrace, (NULL != chObj));

                for (mCnt = 0u; mCnt < VCORE_BP_MAX_LAYOUT_PER_CH; mCnt++)
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

        GT_3trace(BpCoreTrace, GT_DEBUG,
            "FreeCh Freed: %d, Prev Num FreeCh: %d, Cur Num FreeCh: %d\n",
            freeChToFree, instObj->numFreeCh, maxNumFreeCh);

        instObj->numFreeCh = maxNumFreeCh;
    }

    Semaphore_post(VcoreBpPoolObjs.lockSem);

    return (retVal);
}



/**
 *  vcoreBpSetDefChInfo
 *  \brief Resets the channel information with default values.
 */
static void vcoreBpSetDefChInfo(Vcore_BpChObj *chObj, UInt32 chNum)
{
    UInt32      mCnt;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != chObj));

    /* Set proper states */
    chObj->state.isFormatSet = FALSE;
    chObj->state.isParamsSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    /* Initialize core parameters - Assumes YUV422 720x480 progressive */
    chObj->coreFmt.frameWidth = VCORE_BP_DEFAULT_WIDTH;
    chObj->coreFmt.frameHeight = VCORE_BP_DEFAULT_HEIGHT;
    chObj->coreFmt.startX = 0u;
    chObj->coreFmt.startY = 0u;
    chObj->coreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.memType = (UInt32) VPSHAL_VPDMA_MT_NONTILEDMEM;

    chObj->coreFmt.fmt.channelNum = chNum;
    chObj->coreFmt.fmt.width = VCORE_BP_DEFAULT_WIDTH;
    chObj->coreFmt.fmt.height = VCORE_BP_DEFAULT_HEIGHT;
    chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
        VCORE_BP_DEFAULT_WIDTH * 2u;
    chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chObj->coreFmt.fmt.dataFormat = FVID2_DF_YUV422I_YUYV;
    chObj->coreFmt.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.fmt.bpp = FVID2_BPP_BITS16;
    chObj->coreFmt.fmt.reserved = NULL;

    /* Initialize channel descriptor info structure */
    VpsUtils_memset(&chObj->descInfo, 0u, sizeof(Vcore_DescInfo));
    chObj->numExtraDesc = 0u;
    chObj->isDummyNeeded = FALSE;

    /* Initialize mosaic info */
    for (mCnt = 0u; mCnt < VCORE_BP_MAX_LAYOUT_PER_CH; mCnt++)
    {
        chObj->mInfo[mCnt] = NULL;
    }

    return;
}



/**
 *  vcoreBpSetDefMosaicInfo
 *  \brief Resets the mosaic information with default values.
 */
static void vcoreBpSetDefMosaicInfo(Vcore_BpMosaicInfo *mInfo)
{
    UInt32          cnt, pitchCnt;

    /* NULL pointer check */
    GT_assert(BpCoreTrace, (NULL != mInfo));

    /* Initialize mosaic info structure */
    mInfo->numExtraDesc = 0u;
    VpsUtils_memset(&mInfo->descInfo, 0u, sizeof(Vcore_DescInfo));
    mInfo->numFreeCh = 0u;

    /* Initialize application params */
    mInfo->appMulWinPrms.channelNum = 0u;
    mInfo->appMulWinPrms.numWindows = 0u;
    mInfo->appMulWinPrms.winFmt = NULL;

    /* Initialize window format structure */
    for (cnt = 0u; cnt < VCORE_BP_MAX_MOSAIC_WINDOW; cnt++)
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
    mInfo->mulWinPrms.frameWidth = VCORE_BP_DEFAULT_WIDTH;
    mInfo->mulWinPrms.frameHeight = VCORE_BP_DEFAULT_HEIGHT;

    return;
}
