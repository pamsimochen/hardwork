/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDeiPriv.c
 *
 *  \brief VPS DEI M2M driver internal file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/psp/vps/drivers/m2m/src/vpsdrv_m2mDeiPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 vpsMdrvDeiAllocChObjs(VpsMdrv_DeiHandleObj *hObj, UInt32 numCh);
static Int32 vpsMdrvDeiFreeChObjs(VpsMdrv_DeiHandleObj *hObj, UInt32 numCh);

static Int32 vpsMdrvDeiCopyChPrms(VpsMdrv_DeiHandleObj *hObj,
                                  const Vps_M2mDeiChParams *inChPrms);

static Int32 vpsMdrvDeiAllocSplitHandleDescMem(VpsMdrv_DeiHandleObj *hObj);
static Int32 vpsMdrvDeiAllocChDescMem(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj);
static Int32 vpsMdrvDeiFreeChDescMem(VpsMdrv_DeiChObj *chObj);
static Int32 vpsMdrvDeiCalcChCtxMem(VpsMdrv_DeiHandleObj *hObj,
                                    VpsMdrv_DeiChObj *chObj);
static Int32 vpsMdrvDeiFreeChCtxMem(VpsMdrv_DeiChObj *chObj);

static Int32 vpsMdrvDeiSplitChDescMem(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj);
static Int32 vpsMdrvDeiProgramChDesc(VpsMdrv_DeiHandleObj *hObj,
                                     VpsMdrv_DeiChObj *chObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief DEI M2M driver instance objects. */
static VpsMdrv_DeiInstObj VpsMdrvDeiInstObjects[VPS_M2M_DEI_INST_MAX];

/**
 *  \brief DEI M2M driver pool objects used for storing pool memories, pool
 *  flags and pool handles.
 */
VpsMdrv_DeiPoolObj gVpsMdrvDeiPoolObj;

/**
 *  \brief Default scalar configuration. Used when user gives DEI/VIP scalar
 *  configuration as NULL in channel parameter.
 */
static const Vps_ScConfig VpsMdrvDeiDefScCfg =
{
    TRUE,                   /* bypass */
    FALSE,                  /* nonLinear */
    0u,                     /* stripSize */
    VPS_SC_HST_AUTO,        /* hsType */
    VPS_SC_VST_POLYPHASE,   /* vsType */
};

static const Vps_CropConfig VpsMdrvDeiDefCropCfg =
{
    0u,                     /* cropStartY */
    0u,                     /* cropStartX */
    720u,                   /* cropWidth */
    480u,                   /* cropHeight */
};

static const Vps_SubFrameParams VpsMdrvDeiDefSubFramePrms =
{
    FALSE,                  /* subFrameModeEnable */
    480u,                   /* numLinesPerSubFrame */
};

/**
 *  \brief Default FVID2 format configuration. Used when user gives in/out
 *  FVID2 format as NULL in channel parameter.
 */
static const FVID2_Format VpsMdrvDeiDefFmt =
{
    0u,                     /* channelNum */
    720u,                   /* width */
    480u,                   /* height */
    {720u * 2u, 0u, 0u},    /* pitch */
    {FALSE, FALSE, FALSE},  /* fieldMerged */
    FVID2_DF_YUV422I_YUYV,  /* dataFormat */
    FVID2_SF_PROGRESSIVE,   /* scanFormat */
    FVID2_BPP_BITS16,       /* bpp */
    NULL                    /* reserved */
};

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
/**
 *  \brief Default Dei Cfg configuration. Used when TI814x
 *  operates in AUX path where DEI is not present.
 */
static const Vps_DeiConfig VpsMdrvDeiDefDeiCfg =
{
    TRUE,                       /* bypass */
    VPS_DEI_EDIMODE_LUMA_CHROMA,/* inpMode */
    FALSE,                      /* tempInpEnable */
    FALSE,                      /* tempInpChromaEnable */
    FALSE,                      /* spatMaxBypass */
    FALSE                       /* tempMaxBypass */
};
#endif


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vpsMdrvDeiInit
 *  Initializes DEI M2M driver objects, allocates memory etc.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vpsMdrvDeiInit(UInt32 numInst,
                     const VpsMdrv_DeiInitParams *initPrms)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  instCnt, coreCnt, scalarId;
    Semaphore_Params        semPrms;
    VpsMdrv_DeiInstObj     *instObj;

    /* Check for errors */
    GT_assert(VpsMdrvDeiTrace, (numInst <= VPS_M2M_DEI_INST_MAX));
    GT_assert(VpsMdrvDeiTrace, (NULL != initPrms));

    /* Initialize driver objects to zero */
    VpsUtils_memset(VpsMdrvDeiInstObjects, 0u, sizeof(VpsMdrvDeiInstObjects));
    VpsUtils_memset(&gVpsMdrvDeiPoolObj, 0u, sizeof(gVpsMdrvDeiPoolObj));

    /* Init pool objects */
    VpsUtils_initPool(
        &gVpsMdrvDeiPoolObj.handlePoolPrm,
        (Void *) gVpsMdrvDeiPoolObj.handleMemPool,
        VPSMDRV_DEI_MAX_HANDLE_MEM_POOL,
        sizeof(VpsMdrv_DeiHandleObj),
        gVpsMdrvDeiPoolObj.handleMemFlag,
        VpsMdrvDeiTrace);
    VpsUtils_initPool(
        &gVpsMdrvDeiPoolObj.chPoolPrm,
        (Void *) gVpsMdrvDeiPoolObj.chMemPool,
        VPSMDRV_DEI_MAX_CH_MEM_POOL,
        sizeof(VpsMdrv_DeiChObj),
        gVpsMdrvDeiPoolObj.chMemFlag,
        VpsMdrvDeiTrace);
    VpsUtils_initPool(
        &gVpsMdrvDeiPoolObj.qObjPoolPrm,
        (Void *) gVpsMdrvDeiPoolObj.qObjMemPool,
        VPSMDRV_DEI_MAX_QOBJS_POOL,
        sizeof(VpsMdrv_DeiQueueObj),
        gVpsMdrvDeiPoolObj.qObjMemFlag,
        VpsMdrvDeiTrace);

    /* Initialize instance object members */
    for (instCnt = 0u; instCnt < numInst; instCnt++)
    {
        /* DEI path core should be present */
        GT_assert(VpsMdrvDeiTrace,
            (NULL != initPrms->coreOps[VPSMDRV_DEI_DEI_CORE_IDX]));

        instObj = &VpsMdrvDeiInstObjects[instCnt];

        /* Allocate instance semaphore */
        Semaphore_Params_init(&semPrms);
        instObj->instSem = Semaphore_create(1u, &semPrms, NULL);
        if (NULL == instObj->instSem)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Instance semaphore create failed!!\n");
            retVal = FVID2_EALLOC;
            break;
        }

        /* Copy the information */
        instObj->drvInstId = initPrms->drvInstId;
        instObj->secMuxInstId = initPrms->secMuxInstId;

        for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_RESR; coreCnt++)
        {
            instObj->resrcId[coreCnt] = initPrms->resrcId[coreCnt];
        }
        for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
        {
            instObj->coreInstId[coreCnt] = initPrms->coreInstId[coreCnt];
            instObj->coreOps[coreCnt] = initPrms->coreOps[coreCnt];
            /* If core ops is NULL then that particular core is not used */
            if (NULL == instObj->coreOps[coreCnt])
            {
                instObj->isCoreReq[coreCnt] = FALSE;
            }
            else
            {
                instObj->isCoreReq[coreCnt] = TRUE;
            }
        }

        /* Figure out how many input framelists are required for an instance */
        instObj->numInListReq = 0u;
        if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DEI_CORE_IDX])
        {
            instObj->numInListReq++;
        }

        /* Figure out how many output framelists are required for an instance */
        instObj->numOutListReq = 0u;
        if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DWP_CORE_IDX])
        {
            instObj->numOutListReq++;
        }
        if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_VIP_CORE_IDX])
        {
            instObj->numOutListReq++;
        }

        /* Initialize other variables */
        instObj->openCnt = 0u;
        for (scalarId = 0u; scalarId < VPS_M2M_DEI_SCALAR_ID_MAX; scalarId++)
        {
            instObj->enableLazyLoading[scalarId] = FALSE;
            instObj->enableFilterSelect[scalarId] = FALSE;

            /* DEI core is always present in all driver paths. Hence set it
             * here. */
            if (VPS_M2M_DEI_SCALAR_ID_DEI_SC == scalarId)
            {
                instObj->scalarCoreId[scalarId] = VPSMDRV_DEI_DEI_CORE_IDX;
            }
            else
            {
                instObj->scalarCoreId[scalarId] = VPSMDRV_DEI_MAX_CORE;
            }

            /* Set the current scaling factor configuration values to invalid so
             * that the first call to process the frames will result in setting
             * coefficients to the scalar configuration of that frame (channel).
             */
            instObj->curScFactor[scalarId].hsType = VPS_SC_HST_MAX;
            instObj->curScFactor[scalarId].hScalingSet = VPS_SC_SET_MAX;
            instObj->curScFactor[scalarId].vsType = VPS_SC_VST_MAX;
            instObj->curScFactor[scalarId].vScalingSet = VPS_SC_VST_MAX;
        }
        instObj->initDone = TRUE;
        instObj->numChAllocated = 0u;

        /* Move to next parameter */
        initPrms++;
    }

    /* Deinit if error occurs */
    if (FVID2_SOK != retVal)
    {
        vpsMdrvDeiDeInit();
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiDeInit
 *  Deallocates memory allocated by init function.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vpsMdrvDeiDeInit(void)
{
    Int32               retVal = FVID2_SOK;
    UInt32              instCnt, coreCnt;
    VpsMdrv_DeiInstObj *instObj;

    for (instCnt = 0u; instCnt < VPS_M2M_DEI_INST_MAX; instCnt++)
    {
        instObj = &VpsMdrvDeiInstObjects[instCnt];
        if (TRUE == instObj->initDone)
        {
            /* Check if all the handles of the driver are closed */
            if (0u != instObj->openCnt)
            {
                GT_2trace(VpsMdrvDeiTrace, GT_ERR,
                    "%d handles of driver inst %d not closed!\n",
                    instObj->openCnt,
                    instCnt);
                retVal = FVID2_EDRIVER_INUSE;
                break;
            }

            /* Init variables to zero */
            instObj->drvInstId = 0u;
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                instObj->coreInstId[coreCnt] = 0u;
                instObj->coreOps[coreCnt] = NULL;
                instObj->isCoreReq[coreCnt] = FALSE;
            }

            /* Delete the instance semaphore */
            if (NULL != instObj->instSem)
            {
                Semaphore_delete(&instObj->instSem);
                instObj->instSem = NULL;
            }

            /* Reset the init flag */
            instObj->initDone = FALSE;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiAllocResrc
 *  Allocates the resources required for the driver from resource manager.
 *  This function will also set the secondary path mux if
 *  VIP secondary path is required.
 */
Int32 vpsMdrvDeiAllocResrc(VpsMdrv_DeiInstObj *instObj)
{
    Int32       retVal = FVID2_SOK, errRetVal;
    UInt32      resCnt, errCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    for (resCnt = 0u; resCnt < VPSMDRV_DEI_MAX_RESR; resCnt++)
    {
        if (VRM_RESOURCE_INVALID == instObj->resrcId[resCnt])
        {
            /* Invalid resource id should be allocated */
            continue;
        }
        retVal = Vrm_allocResource(instObj->resrcId[resCnt]);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not allocate resource %d!\n",
                instObj->resrcId[resCnt]);
            break;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (TRUE == instObj->isCoreReq[VPSMDRV_DEI_VIP_CORE_IDX])
        {
            /* Set ITP mux register to VIP path if needed */
            retVal = VpsHal_vpsSecMuxSelect(
                         instObj->secMuxInstId,
                         VPSHAL_VPS_SEC_MUX_SRC_PRI_AUX);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Could not set ITP mux!\n");
            }
        }
    }

    /* If error free the already allocated resource */
    if (FVID2_SOK != retVal)
    {
        for (errCnt = 0u; errCnt < resCnt; errCnt++)
        {
            if (VRM_RESOURCE_INVALID == instObj->resrcId[errCnt])
            {
                continue;
            }
            errRetVal = Vrm_releaseResource(instObj->resrcId[errCnt]);
            GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == errRetVal));
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiFreeResrc
 *  Frees the resources allocated by the driver from resource manager.
 */
Int32 vpsMdrvDeiFreeResrc(VpsMdrv_DeiInstObj *instObj)
{
    Int32       retVal = FVID2_SOK;
    UInt32      resCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    for (resCnt = 0u; resCnt < VPSMDRV_DEI_MAX_RESR; resCnt++)
    {
        if (VRM_RESOURCE_INVALID == instObj->resrcId[resCnt])
        {
            continue;
        }
        retVal = Vrm_releaseResource(instObj->resrcId[resCnt]);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not free resource %d!\n",
                instObj->resrcId[resCnt]);
            break;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiGetInstObj
 *  Returns the instance object pointer for the instance id.
 */
VpsMdrv_DeiInstObj *vpsMdrvDeiGetInstObj(UInt32 instId)
{
    UInt32              instCnt;
    VpsMdrv_DeiInstObj *instObj = NULL;

    /* Find out the instance to which this channel belongs to. */
    for (instCnt = 0u; instCnt < VPS_M2M_DEI_INST_MAX; instCnt++)
    {
        if (VpsMdrvDeiInstObjects[instCnt].drvInstId == instId)
        {
            instObj = &VpsMdrvDeiInstObjects[instCnt];
            break;
        }
    }

    return (instObj);
}



/**
 *  vpsMdrvDeiAllocHandleObj
 *  Allocates handle object and all other memories of the handle like handle
 *  queues, descriptor/shadow overlay memories, channel objects etc.
 *  Returns the handle object pointer on success else returns NULL.
 */
VpsMdrv_DeiHandleObj *vpsMdrvDeiAllocHandleObj(
                          Vps_M2mDeiCreateParams *createPrms,
                          VpsMdrv_DeiInstObj *instObj,
                          const FVID2_DrvCbParams *fdmCbPrms,
                          Int32 *retValPtr)
{
    Int32                       retVal = FVID2_SOK;
    UInt32                      qCnt, chCnt, descSet, i;
    Semaphore_Params            semPrms;
    VpsMdrv_DeiQueueObj        *qObj;
    VpsMdrv_DeiHandleObj       *hObj = NULL;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != createPrms));
    GT_assert(VpsMdrvDeiTrace, (NULL != createPrms->chParams));
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != fdmCbPrms));
    GT_assert(VpsMdrvDeiTrace, (NULL != retValPtr));

    /* Allocate handle memory */
    hObj = (VpsMdrv_DeiHandleObj *) VpsUtils_alloc(
                                        &gVpsMdrvDeiPoolObj.handlePoolPrm,
                                        sizeof (VpsMdrv_DeiHandleObj),
                                        VpsMdrvDeiTrace);
    if (NULL == hObj)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Handle object memory alloc failed!!\n");
        retVal = FVID2_EALLOC;
    }
    else
    {
        hObj->instObj = instObj;
        /* Reset some of the important fields used by free function so that
         * when error occurs, we won't free un-allocated pointers!! */
        hObj->numCh = 0u;
        hObj->reqQ = NULL;
        hObj->doneQ = NULL;
        hObj->freeQ = NULL;
        hObj->ctrlSem = NULL;
        hObj->descMem = NULL;
        hObj->nshwOvlyMem = NULL;
        hObj->shwOvlyMem = NULL;
        hObj->numCoeffDesc = 0u;
        hObj->isStopped = FALSE;
        hObj->numPendReq = 0u;
        hObj->numUnProcessedReq = 0u;
        hObj->numVpdmaChannels = 0u;
        for (chCnt = 0u; chCnt < VPS_M2M_DEI_MAX_CH_PER_HANDLE; chCnt++)
        {
            hObj->chObjs[chCnt] = NULL;
        }
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            hObj->firstDesc[descSet] = NULL;
            hObj->nshwCfgDesc[descSet] = NULL;
            hObj->shwCfgDesc[descSet] = NULL;
            for (i = 0u; i < VPSMDRV_DEI_MAX_RT_COEFF_CFG_DESC; i++)
            {
                hObj->coeffDesc[descSet][i] = NULL;
            }
            hObj->rldDesc[descSet] = NULL;

            /* Mark both descriptor set is free for use */
            hObj->isDescSetFree[descSet] = TRUE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* If per handle config flag is set, then the handle will have config
         * descriptor. */
        if (VPS_M2M_CONFIG_PER_HANDLE == createPrms->mode)
        {
            retVal = vpsMdrvDeiAllocSplitHandleDescMem(hObj);
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Create the different queues - request, done and free queues */
        hObj->reqQ = VpsUtils_createQ();
        hObj->doneQ = VpsUtils_createQ();
        hObj->freeQ = VpsUtils_createQ();
        if ((NULL == hObj->reqQ) ||
            (NULL == hObj->doneQ) ||
            (NULL == hObj->freeQ))
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Queue creation failed!!\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Create semaphore */
        Semaphore_Params_init(&semPrms);
        hObj->ctrlSem = Semaphore_create(0u, &semPrms, NULL);
        if (NULL == hObj->ctrlSem)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Control semaphore create failed!!\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate queue object memory and queue them to free queue */
        for (qCnt = 0u; qCnt < VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE; qCnt++)
        {
            /* Allocate queue object memory */
            qObj = (VpsMdrv_DeiQueueObj *) VpsUtils_alloc(
                                               &gVpsMdrvDeiPoolObj.qObjPoolPrm,
                                               sizeof(VpsMdrv_DeiQueueObj),
                                               VpsMdrvDeiTrace);

    		/* KC: keep track of Que obj's so that they can be freeed later */
	   	    hObj->qObj[qCnt]	= qObj;
		
            if (NULL == qObj)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Queue object memory alloc failed!!\n");
                retVal = FVID2_EALLOC;
                break;
            }

            /* Initialize and queue the allocate queue object to free Q */
            qObj->hObj = hObj;
            qObj->descSetInUse = VPSMDRV_DEI_INVALID_DESC_SET;
            VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate channel object */
        retVal = vpsMdrvDeiAllocChObjs(hObj, createPrms->numCh);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Channel object memory alloc failed!!\n");
        }
        else
        {
            /* Copy the handle parameters */
            hObj->mode = createPrms->mode;
            hObj->numCh = createPrms->numCh;
            hObj->isVipScReq = createPrms->isVipScReq;
#ifdef VPS_HAL_INCLUDE_DEIH
            hObj->deiHqCtxMode = createPrms->deiHqCtxMode;
#else
            /* Override user settings if DEIHQ is not present */
            hObj->deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;
#endif  /* #ifdef VPS_HAL_INCLUDE_DEIH*/

            /* Copy the callback params */
            hObj->fdmCbPrms.fdmCbFxn = fdmCbPrms->fdmCbFxn;
            hObj->fdmCbPrms.fdmData = fdmCbPrms->fdmData;
            hObj->fdmCbPrms.fdmErrCbFxn = fdmCbPrms->fdmErrCbFxn;
            hObj->fdmCbPrms.errList = fdmCbPrms->errList;

            /* Copy the channel parameters */
            retVal = vpsMdrvDeiCopyChPrms(hObj, createPrms->chParams);
        }
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        if (NULL != hObj)
        {
            vpsMdrvDeiFreeHandleObj(hObj);
            hObj = NULL;
        }
    }

    *retValPtr = retVal;
    return (hObj);
}



/**
 *  vpsMdrvDeiFreeHandleObj
 *  Frees the handle object and all other memories of the handle like handle
 *  queues, descriptor/shadow overlay memories, channel objects etc.
 */
Int32 vpsMdrvDeiFreeHandleObj(VpsMdrv_DeiHandleObj *hObj)
{
    Int32                   retVal = FVID2_SOK, tempRetVal;
    UInt32                  chCnt, descSet, i;
    Vps_M2mDeiChParams     *chPrms;
    VpsMdrv_DeiQueueObj    *qObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));

    /* Reset the channel parameters. Esp set the pointers to NULL */
    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != hObj->chObjs[chCnt]));

        chPrms = &hObj->chObjs[chCnt]->chPrms;
        chPrms->outFmtDei = NULL;
        chPrms->outFmtVip = NULL;
        chPrms->inFmtFldN_1 = NULL;
        chPrms->deiHqCfg = NULL;
        chPrms->deiCfg = NULL;
        chPrms->scCfg = NULL;
        chPrms->vipScCfg = NULL;
        chPrms->subFrameParams = NULL;
    }

    /* Free channel objects */
    retVal = vpsMdrvDeiFreeChObjs(hObj, hObj->numCh);
    hObj->numCh = 0u;

    if (NULL != hObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        #if 0 /* orignal code */		
        while (1u)
        {
            qObj = VpsUtils_dequeue(hObj->freeQ);
            if (NULL == qObj)
            {
                /* No more in queue */
                break;
            }

            /* Free queue object memory */
            qObj->hObj = NULL;
            tempRetVal = VpsUtils_free(
                             &gVpsMdrvDeiPoolObj.qObjPoolPrm,
                             qObj,
                             VpsMdrvDeiTrace);
            GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
        }
        #else 
        { 
            /* KC: free que's objects based on object pointer collected during create 
                instead of relying on freeQ

                This is to fix a bug found in DM810x non-real time to real time
                use-case switch where qObj's were getting lost
                resulting in a memory leak.
                
                This seems to occur when previous field override flag is set
            */
            int qCnt;

            for (qCnt = 0u; qCnt < VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE; qCnt++)
            {
                qObj = hObj->qObj[qCnt];
                if (NULL == qObj)
                {
                    continue;
                }

                 /* Free queue object memory */
                 tempRetVal = VpsUtils_free(
                             &gVpsMdrvDeiPoolObj.qObjPoolPrm,
                             qObj,
                             VpsMdrvDeiTrace);
                GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
            }
            /* Ensure there are no elements in the freeQ, we have released 
                memory for all the elements but the queue does not yet know, it.
                Pop all the elements and discard it */
            while (1u)
            {
                qObj = VpsUtils_dequeue(hObj->freeQ);
                if (NULL == qObj)
                {
                    /* No more in queue */
                    break;
                }
                qObj->hObj = NULL;
            }
        }
        #endif
    }

    if (NULL != hObj->reqQ)
    {
        /* Delete the request Q */
        VpsUtils_deleteQ(hObj->reqQ);
        hObj->reqQ = NULL;
    }

    if (NULL != hObj->doneQ)
    {
        /* Delete the done Q */
        VpsUtils_deleteQ(hObj->doneQ);
        hObj->doneQ = NULL;
    }

    if (NULL != hObj->freeQ)
    {
        /* Delete the free Q */
        VpsUtils_deleteQ(hObj->freeQ);
        hObj->freeQ = NULL;
    }

    if (NULL != hObj->ctrlSem)
    {
        /* Delete the control semaphore */
        Semaphore_delete(&hObj->ctrlSem);
        hObj->ctrlSem = NULL;
    }

    /* Free the handle descriptor memories */
    if (NULL != hObj->descMem)
    {
        tempRetVal = VpsUtils_freeDescMem(hObj->descMem, hObj->totalDescMem);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
        hObj->totalDescMem = 0u;
        hObj->descMem = NULL;
    }
    if (NULL != hObj->nshwOvlyMem)
    {
        tempRetVal = VpsUtils_freeDescMem(
                         hObj->nshwOvlyMem,
                         VPSMDRV_DEI_MAX_NSHW_OVLY_MEM);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
        hObj->nshwOvlyMem = NULL;
        hObj->totalNshwOvlyMem = 0u;
    }
    if (NULL != hObj->shwOvlyMem)
    {
        tempRetVal = VpsUtils_freeDescMem(
                         hObj->shwOvlyMem,
                         VPSMDRV_DEI_MAX_SHW_OVLY_MEM);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
        hObj->shwOvlyMem = NULL;
        hObj->totalShwOvlyMem = 0u;
    }
    hObj->numCoeffDesc = 0u;
    for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
    {
        hObj->firstDesc[descSet] = NULL;
        for (i = 0u; i < VPSMDRV_DEI_MAX_RT_COEFF_CFG_DESC; i++)
        {
            hObj->coeffDesc[descSet][i] = NULL;
        }
        hObj->nshwCfgDesc[descSet] = NULL;
        hObj->shwCfgDesc[descSet] = NULL;
        hObj->rldDesc[descSet] = NULL;
    }

    /* Reset the variables */
    hObj->instObj = NULL;
    hObj->deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;
    hObj->numVpdmaChannels = 0u;
    hObj->fdmCbPrms.fdmCbFxn = NULL;
    hObj->fdmCbPrms.fdmData = NULL;
    hObj->fdmCbPrms.fdmErrCbFxn = NULL;
    hObj->fdmCbPrms.errList = NULL;

    /* Free handle object memory */
    tempRetVal = VpsUtils_free(
                     &gVpsMdrvDeiPoolObj.handlePoolPrm,
                     hObj,
                     VpsMdrvDeiTrace);
    GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));

    return (retVal);
}



/**
 *  vpsMdrvDeiAllocCoreMem
 *  Allocates the required descriptor and overlay memories for all the channels
 *  and for all the cores for this handle. It also slices the descriptors and
 *  overlay memory and init the core desc memory structure.
 *  It also allocates the context buffer required by DEI core.
 */
Int32 vpsMdrvDeiAllocCoreMem(VpsMdrv_DeiHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt;
    VpsMdrv_DeiChObj   *chObj;
    VpsMdrv_DeiInstObj *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        /* Allocate desc memory for each channel */
        retVal = vpsMdrvDeiAllocChDescMem(hObj, chObj);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not allocate descrioptor for channel: %d!\n",
                chObj->chNum);
            break;
        }

        /* Split the allocated descriptors across cores */
        retVal = vpsMdrvDeiSplitChDescMem(hObj, chObj);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Error slicing descrioptor for channel: %d!\n", chObj->chNum);
            break;
        }

        /* Figure out context memory for each channel */
        retVal = vpsMdrvDeiCalcChCtxMem(hObj, chObj);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not allocate context buffers for channel: %d!\n",
                chObj->chNum);
            break;
        }
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        vpsMdrvDeiFreeCoreMem(hObj);
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiFreeCoreMem
 *  Frees the allocated descriptors and overlay memories for all the channels
 *  and for all the cores for this handle.
 *  It also frees the context buffer allocated for DEI core.
 */
Int32 vpsMdrvDeiFreeCoreMem(VpsMdrv_DeiHandleObj *hObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  chCnt, descSet, coreCnt, cnt, fldCnt, ptrCnt, i;
    VpsMdrv_DeiChObj       *chObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));

    /* Free desc memory for each channel */
    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        /* Free channel descriptor memory */
        retVal = vpsMdrvDeiFreeChDescMem(chObj);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == retVal));

        /* Free channel context buffer memory */
        retVal = vpsMdrvDeiFreeChCtxMem(chObj);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == retVal));

        /* Reset the dependent pointers */
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            chObj->firstDesc[descSet] = NULL;
            for (i = 0u; i < VPSMDRV_DEI_MAX_RT_COEFF_CFG_DESC; i++)
            {
                chObj->coeffDesc[descSet][i] = NULL;
            }
            chObj->nshwCfgDesc[descSet] = NULL;
            chObj->shwCfgDesc[descSet] = NULL;
            chObj->sochDesc[descSet] = NULL;
            chObj->rldDesc[descSet] = NULL;
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                for (cnt = 0u; cnt < VCORE_MAX_IN_DESC; cnt++)
                {
                    chObj->coreDescMem[descSet][coreCnt].inDataDesc[cnt]
                        = NULL;
                }
                for (cnt = 0u; cnt < VCORE_MAX_OUT_DESC; cnt++)
                {
                    chObj->coreDescMem[descSet][coreCnt].outDataDesc[cnt]
                        = NULL;
                }
                for (cnt = 0u; cnt < VCORE_MAX_MULTI_WIN_DESC; cnt++)
                {
                    chObj->coreDescMem[descSet][coreCnt].multiWinDataDesc[cnt]
                        = NULL;
                }
                chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem = NULL;
            }
        }

        for (fldCnt = 0u; fldCnt < VCORE_DEI_MAX_OUT_FLD; fldCnt++)
        {
            for (ptrCnt = 0u; ptrCnt < VCORE_DEI_NUM_DESC_PER_FIELD; ptrCnt++)
            {
                chObj->curOutWriteDesc[fldCnt][ptrCnt] = NULL;
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiProgramDesc
 *  Program the core descriptors and overlay memories for all the channels
 *  of the handle. This also programs the handle descriptor and overlay
 *  memory if per handle config is true.
 */
Int32 vpsMdrvDeiProgramDesc(VpsMdrv_DeiHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, coreCnt;
    VpsMdrv_DeiInstObj *instObj;
    const Vcore_Ops    *coreOps;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        /* Program descriptor and overlay memory for each channel */
        retVal = vpsMdrvDeiProgramChDesc(hObj, hObj->chObjs[chCnt]);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not program descriptors for channel %d!\n",
                hObj->chObjs[chCnt]->chNum);
            break;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Program the frame start event for the cores - only one time
         * is sufficient */
        /* TODO: FIXME - Changing the frame start event before getting the
         * resource. Bug: When DEI display is going, opening of this driver will
         * overwrite the frame start from LM FID change to channel active!! */
        if (0u == instObj->openCnt)
        {
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                /* Program cores only if needed for that instance */
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    coreOps = instObj->coreOps[coreCnt];
                    /* NULL pointer check */
                    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
                    GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->setFsEvent));
                    retVal = coreOps->setFsEvent(
                                 hObj->coreHandle[coreCnt],
                                 0u,
                                 VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE,
                                 NULL);
                    if (FVID2_SOK != retVal)
                    {
                        GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                            "Could not set frame start for core %d!\n",
                            coreCnt);
                        break;
                    }
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiAllocCoeffDesc
 */
Void *vpsMdrvDeiAllocCoeffDesc(void)
{
    Void       *coeffDesc;

    /* Allocate coeff descriptor memory */
    coeffDesc = VpsUtils_allocDescMem(
                    VPSMDRV_DEI_MAX_COEFF_DESC_MEM,
                    VPSHAL_VPDMA_DESC_BYTE_ALIGN);

    return (coeffDesc);
}



/**
 *  vpsMdrvDeiFreeCoeffDesc
 */
Int32 vpsMdrvDeiFreeCoeffDesc(Void *coeffDesc)
{
    Int32       retVal;

    /* Free coeff descriptor memory */
    retVal = VpsUtils_freeDescMem(
                 coeffDesc,
                 VPSMDRV_DEI_MAX_COEFF_DESC_MEM);

    return (retVal);
}


/**
 *  vpsMdrvDeiAllocSplitHandleDescMem
 *  Allocate the required descriptors and overlay memories for the handle
 *  and split the memories as well.
 */
static Int32 vpsMdrvDeiAllocSplitHandleDescMem(VpsMdrv_DeiHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              descSet;
    UInt32              i, scalarId;
    UInt8              *descPtr;
    UInt32              totalDescMem = 0u, totalCoeffDescMem = 0u;
    UInt32              totalNshwOvlyMem = 0u, totalShwOvlyMem = 0u;
    VpsMdrv_DeiInstObj *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* There are VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC coeff
     * descriptors needed for each core in the path that has a scalar. */
    hObj->numCoeffDesc = 0u;
    for (scalarId = 0u; scalarId < VPS_M2M_DEI_SCALAR_ID_MAX; scalarId++)
    {
        if (VPSMDRV_DEI_MAX_CORE != instObj->scalarCoreId[scalarId])
        {
            hObj->numCoeffDesc += VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC;
        }
    }

    /* Calculate the total descriptor memory and overlay memory
     * required per handle */
    totalCoeffDescMem = (hObj->numCoeffDesc * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
    totalDescMem +=
        (VPSMDRV_DEI_MAX_SHW_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
    totalDescMem +=
        (VPSMDRV_DEI_MAX_NSHW_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
    totalDescMem += totalCoeffDescMem;
    totalDescMem += (VPSMDRV_DEI_MAX_RLD_DESC * VPSHAL_VPDMA_CTRL_DESC_SIZE);

    /* Allocate for all the sets */
    totalDescMem *= VPSMDRV_DEI_MAX_DESC_SET;
    /* For overlay, allocate full mem as still cores are not initialized */
    totalNshwOvlyMem = VPSMDRV_DEI_MAX_NSHW_OVLY_MEM;
    totalShwOvlyMem = VPSMDRV_DEI_MAX_SHW_OVLY_MEM;

    /* Allocate descriptor and overlay memories used per handle */
    hObj->descMem = VpsUtils_allocDescMem(
                        totalDescMem,
                        VPSHAL_VPDMA_DESC_BYTE_ALIGN);
    hObj->nshwOvlyMem = VpsUtils_allocDescMem(
                            totalNshwOvlyMem,
                            VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    hObj->shwOvlyMem = VpsUtils_allocDescMem(
                           totalShwOvlyMem,
                           VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    if ((NULL == hObj->descMem) ||
        (NULL == hObj->nshwOvlyMem) ||
        (NULL == hObj->shwOvlyMem))
    {
        if (NULL != hObj->descMem)
        {
            hObj->totalDescMem = totalDescMem;
        }
        if (NULL != hObj->nshwOvlyMem)
        {
            hObj->totalNshwOvlyMem = totalNshwOvlyMem;
        }
        if (NULL != hObj->shwOvlyMem)
        {
            hObj->totalShwOvlyMem = totalShwOvlyMem;
        }
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Could not allocate handle descriptor/overlay memories\n");
        retVal = FVID2_EALLOC;
    }
    else
    {
        hObj->totalDescMem = totalDescMem;
        hObj->totalNshwOvlyMem = totalNshwOvlyMem;
        hObj->totalShwOvlyMem = totalShwOvlyMem;
        hObj->totalCoeffDescMem = totalCoeffDescMem;
    }

    if (FVID2_SOK == retVal)
    {
        /* Split the descriptor memory for handle */
        descPtr = hObj->descMem;
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            hObj->firstDesc[descSet] = descPtr;

            for (i = 0u; i < hObj->numCoeffDesc; i++)
            {
                hObj->coeffDesc[descSet][i] = (VpsHal_VpdmaConfigDesc *)
                        (  ((UInt32) descPtr)
                         + (i * VPSHAL_VPDMA_CONFIG_DESC_SIZE));
                /* Create a dummy descriptor to initialize. */
                VpsHal_vpdmaCreateDummyDesc(hObj->coeffDesc[descSet][i]);
            }
            descPtr += (hObj->numCoeffDesc * VPSHAL_VPDMA_CONFIG_DESC_SIZE);

            hObj->nshwCfgDesc[descSet] = (VpsHal_VpdmaConfigDesc *) descPtr;
            descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

            hObj->shwCfgDesc[descSet] = (VpsHal_VpdmaConfigDesc *) descPtr;
            descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

            /* Assign reload descriptor */
            hObj->rldDesc[descSet] = (VpsHal_VpdmaReloadDesc *) descPtr;
            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiAllocChDescMem
 *  Allocate the required descriptors and overlay memories for the channel
 *  according to the requirement of all the cores.
 */
static Int32 vpsMdrvDeiAllocChDescMem(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj)
{
    Int32               retVal = FVID2_SOK, tempRetVal;
    UInt32              coreCnt;
    UInt32              scalarId;
    Vcore_DescInfo      descInfo;
    const Vcore_Ops    *coreOps;
    VpsMdrv_DeiInstObj *instObj;
    UInt32              totalDescMem, totalCoeffDescMem, totalWrDescMem;
    UInt32              totalNshwOvlyMem, totalShwOvlyMem;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
    UInt32              totalDescCnt;
#endif

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* Reset variables */
    totalDescMem = 0u;
    totalCoeffDescMem = 0u;
    totalWrDescMem = 0u;
    totalNshwOvlyMem = 0u;
    totalShwOvlyMem = 0u;
    GT_assert(VpsMdrvDeiTrace, (NULL == chObj->descMem));
    GT_assert(VpsMdrvDeiTrace, (NULL == chObj->wrDescMem));
    GT_assert(VpsMdrvDeiTrace, (NULL == chObj->nshwOvlyMem));
    GT_assert(VpsMdrvDeiTrace, (NULL == chObj->shwOvlyMem));
    chObj->descMem = NULL;
    chObj->wrDescMem = NULL;
    chObj->nshwOvlyMem = NULL;
    chObj->shwOvlyMem = NULL;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
    totalDescCnt = 0u;
#endif

    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getDescInfo));

            /* Get the descriptor info for each core */
            retVal = coreOps->getDescInfo(
                         hObj->coreHandle[coreCnt],
                         chObj->chNum,
                         0u,
                         &descInfo);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not get core info for core %d!\n", coreCnt);
                break;
            }

            /* Accumulate the info in total info variable */
            totalDescMem +=
                (descInfo.numInDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE);
            totalDescMem +=
                (descInfo.numOutDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE);
            totalDescMem +=
                (descInfo.numMultiWinDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE);
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
            totalDescCnt +=
                (descInfo.numInDataDesc +
                 descInfo.numOutDataDesc +
                 descInfo.numMultiWinDataDesc);
#endif
            totalNshwOvlyMem += descInfo.nonShadowOvlySize;
            totalShwOvlyMem += descInfo.shadowOvlySize;
            /* Get the coefficient information */
            chObj->coeffConfigDest[coreCnt] = descInfo.coeffConfigDest;
            chObj->horzCoeffOvlySize[coreCnt] = descInfo.horzCoeffOvlySize;
            chObj->vertCoeffOvlySize[coreCnt] = descInfo.vertCoeffOvlySize;
            chObj->horzCoeffMemPtr[coreCnt] = NULL;
            chObj->vertCoeffMemPtr[coreCnt] = NULL;
            chObj->vertBilinearCoeffOvlySize[coreCnt] =
                descInfo.vertBilinearCoeffOvlySize;
        }
    }

    if (FVID2_SOK == retVal)
    {
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
        /* Add the number of SOT descriptors needed */
        totalDescMem +=
            ((totalDescCnt / VPS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT) *
                VPSHAL_VPDMA_CTRL_DESC_SIZE);
#endif

        /*
         * If per channel config is TRUE, allocate mem for shadow/non-shadow
         * config descriptor in main descriptor memory of the channel.
         */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            totalDescMem +=
                (VPSMDRV_DEI_MAX_SHW_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            totalDescMem +=
                (VPSMDRV_DEI_MAX_NSHW_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE);

            /* There are VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC coeff
             * descriptors needed for each core in the path that has a scalar.
             */
            chObj->numCoeffDesc = 0u;
            for (scalarId = 0u; scalarId < VPS_M2M_DEI_SCALAR_ID_MAX; scalarId++)
            {
                if (VPSMDRV_DEI_MAX_CORE != instObj->scalarCoreId[scalarId])
                {
                    chObj->numCoeffDesc += VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC;
                    totalCoeffDescMem +=
                        (  VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC
                         * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
                }
            }
            totalDescMem += totalCoeffDescMem;
        }
        else
        {
            chObj->numCoeffDesc = 0u;
        }

        /* Allocate for SOCH control descriptors.
         * Use all channels returned by the cores as SOCH */
        totalDescMem += (hObj->numVpdmaChannels * VPSHAL_VPDMA_CTRL_DESC_SIZE);

        /* Allocate for reload descriptor in main descriptor memory */
        totalDescMem +=
            (VPSMDRV_DEI_MAX_RLD_DESC * VPSHAL_VPDMA_CTRL_DESC_SIZE);

        /* Allocate descriptor and overlay memory per channel for all
         * the sets. */
        totalDescMem *= VPSMDRV_DEI_MAX_DESC_SET;
        totalCoeffDescMem *= VPSMDRV_DEI_MAX_DESC_SET;
        totalNshwOvlyMem *= VPSMDRV_DEI_MAX_DESC_SET;
        totalShwOvlyMem *= VPSMDRV_DEI_MAX_DESC_SET;

        /* Allocate memory */
        chObj->descMem = VpsUtils_allocDescMem(
                             totalDescMem,
                             VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        if (NULL == chObj->descMem)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not allocate channel descrioptor!\n");
            retVal = FVID2_EALLOC;
        }
        else
        {
            chObj->totalDescMem = totalDescMem;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Assign non-shadow config overlay memory if needed */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            chObj->nshwOvlyMem = VpsUtils_allocDescMem(
                                     totalNshwOvlyMem,
                                     VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
            if (NULL == chObj->nshwOvlyMem)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not allocate channel non-shadow overlay memory!\n");
                retVal = FVID2_EALLOC;
            }
            else
            {
                chObj->totalNshwOvlyMem = totalNshwOvlyMem;
            }
        }
        else
        {
            /* In per handle configuration we use the handle overlay memory
             * and no memory is allocated per channel. But still the
             * size is used from channel object. */
            chObj->totalNshwOvlyMem = totalNshwOvlyMem;
            chObj->nshwOvlyMem = NULL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Assign shadow config overlay memory if needed */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            chObj->shwOvlyMem = VpsUtils_allocDescMem(
                                    totalShwOvlyMem,
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
            if (NULL == chObj->shwOvlyMem)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not allocate channel shadow overlay memory!\n");
                retVal = FVID2_EALLOC;
            }
            else
            {
                chObj->totalShwOvlyMem = totalShwOvlyMem;
            }
        }
        else
        {
            /* In per handle configuration we use the handle overlay memory
             * and no memory is allocated per channel. But still the
             * size is used from channel object. */
            chObj->totalShwOvlyMem = totalShwOvlyMem;
            chObj->shwOvlyMem = NULL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate for compressor write descriptor if needed */
        if (((TRUE == chObj->deiCoreCtxInfo.isDeinterlacing) ||
                (TRUE == chObj->deiCoreCtxInfo.isTnrMode))
            && (TRUE == chObj->chPrms.comprEnable))
        {
            totalWrDescMem = ((chObj->deiCoreCtxInfo.numCurFldOut
                * VCORE_DEI_NUM_DESC_PER_FIELD)
                * VPSHAL_VPDMA_DATA_DESC_SIZE);
            chObj->wrDescMem = VpsUtils_allocDescMem(
                                   totalWrDescMem,
                                   VPSHAL_VPDMA_WR_DESC_BUF_ADDR_ALIGN);
            if (NULL == chObj->wrDescMem)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Could not allocate channel write descrioptor!\n");
                retVal = FVID2_EALLOC;
            }
            else
            {
                chObj->totalWrDescMem = totalWrDescMem;
            }
        }
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        tempRetVal = vpsMdrvDeiFreeChDescMem(chObj);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
    }
    else
    {
        chObj->totalCoeffDescMem = totalCoeffDescMem;

        /* Check if already allocated overlay memory is sufficient */
        if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
        {
            GT_assert(VpsMdrvDeiTrace,
                (hObj->totalNshwOvlyMem >= totalNshwOvlyMem));
            hObj->totalNshwOvlyMem = totalNshwOvlyMem;
            GT_assert(VpsMdrvDeiTrace,
                (hObj->totalShwOvlyMem >= totalShwOvlyMem));
            hObj->totalShwOvlyMem = totalShwOvlyMem;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiFreeChDescMem
 *  Frees the allocated descriptors and overlay memories for the channel.
 */
static Int32 vpsMdrvDeiFreeChDescMem(VpsMdrv_DeiChObj *chObj)
{
    Int32       retVal = FVID2_SOK;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

    if (NULL != chObj->descMem)
    {
        retVal = VpsUtils_freeDescMem(chObj->descMem, chObj->totalDescMem);
        chObj->descMem = NULL;
        chObj->totalDescMem = 0u;
    }

    if (NULL != chObj->wrDescMem)
    {
        retVal |= VpsUtils_freeDescMem(chObj->wrDescMem, chObj->totalWrDescMem);
        chObj->wrDescMem = NULL;
        chObj->totalWrDescMem = 0u;
    }

    if (NULL != chObj->nshwOvlyMem)
    {
        retVal |= VpsUtils_freeDescMem(
                      chObj->nshwOvlyMem,
                      chObj->totalNshwOvlyMem);
        chObj->nshwOvlyMem = NULL;
        chObj->totalNshwOvlyMem = 0u;
    }

    if (NULL != chObj->shwOvlyMem)
    {
        retVal |= VpsUtils_freeDescMem(
                      chObj->shwOvlyMem,
                      chObj->totalShwOvlyMem);
        chObj->shwOvlyMem = NULL;
        chObj->totalShwOvlyMem = 0u;
    }

    chObj->totalCoeffDescMem = 0u;

    return (retVal);
}



/**
 *  vpsMdrvDeiCalcChCtxMem
 *  Calculate the required DEI context buffers for the channel according to the
 *  requirement of DEI core.
 */
static Int32 vpsMdrvDeiCalcChCtxMem(VpsMdrv_DeiHandleObj *hObj,
                                    VpsMdrv_DeiChObj *chObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  fldCnt, ptrCnt, mvCnt;
    VpsMdrv_DeiInstObj     *instObj;
    UInt32                  width, height, lineStride;
    Vcore_DeiCtxInfo       *deiCoreCtxInfo;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    deiCoreCtxInfo = &chObj->deiCoreCtxInfo;
    deiCoreCtxBuf = &chObj->deiCoreCtxBuf;

    width = chObj->chPrms.inFmt.width;
    height = chObj->chPrms.inFmt.height;

    if (FVID2_SF_PROGRESSIVE == chObj->chPrms.inFmt.scanFormat)
    {
        /* For the progressive mode, height from the format is
           frame height, so for the previous fields, it should
           be field size. */
        height = height / 2u;
    }

    /* Reset variables */
    for (fldCnt = 0u; fldCnt < VCORE_DEI_MAX_PREV_FLD; fldCnt++)
    {
        for (ptrCnt = 0u; ptrCnt < VCORE_DEI_NUM_DESC_PER_FIELD; ptrCnt++)
        {
            GT_assert(VpsMdrvDeiTrace,
                (NULL == deiCoreCtxBuf->prevInBuf[fldCnt][ptrCnt]));
            deiCoreCtxBuf->prevInBuf[fldCnt][ptrCnt] = NULL;
        }
    }
    for (fldCnt = 0u; fldCnt < VCORE_DEI_MAX_OUT_FLD; fldCnt++)
    {
        for (ptrCnt = 0u; ptrCnt < VCORE_DEI_NUM_DESC_PER_FIELD; ptrCnt++)
        {
            GT_assert(VpsMdrvDeiTrace,
                     (NULL == deiCoreCtxBuf->curOutBuf[fldCnt][ptrCnt]));
            deiCoreCtxBuf->curOutBuf[fldCnt][ptrCnt] = NULL;
        }
    }
    for (mvCnt = 0u; mvCnt < VCORE_DEI_MAX_MV_IN; mvCnt++)
    {
        GT_assert(VpsMdrvDeiTrace, (NULL == deiCoreCtxBuf->mvInBuf[mvCnt]));
        deiCoreCtxBuf->mvInBuf[mvCnt] = NULL;
    }
    GT_assert(VpsMdrvDeiTrace, (NULL == deiCoreCtxBuf->mvOutBuf));
    deiCoreCtxBuf->mvOutBuf = NULL;

    for (mvCnt = 0u; mvCnt < VCORE_DEI_MAX_MVSTM_IN + 1u; mvCnt++)
    {
        GT_assert(VpsMdrvDeiTrace, (NULL == deiCoreCtxBuf->mvstmInBuf[mvCnt]));
        deiCoreCtxBuf->mvstmInBuf[mvCnt] = NULL;
    }
    GT_assert(VpsMdrvDeiTrace, (NULL == deiCoreCtxBuf->mvstmOutBuf));
    deiCoreCtxBuf->mvstmOutBuf = NULL;

    chObj->isDeiCtxBufSet = FALSE;
    if (FVID2_SF_PROGRESSIVE == chObj->chPrms.inFmt.scanFormat)
    {
        /* For the progressive mode, fld index should start from 0 */
        chObj->fldIdx = 0u;
    }
    else
    {
        chObj->fldIdx = 1u;
    }
    chObj->mvIdx = 1u;
    chObj->mvstmIdx = 1u;
    chObj->prevFldState = 0u;
    chObj->dontRotateCtxBuf = FALSE;

    /* Fill the number of context buffer information */
    chObj->deiCtxInfo.channelNum = chObj->chNum;
    chObj->deiCtxInfo.numFld =
        deiCoreCtxInfo->numPrevFld + deiCoreCtxInfo->numCurFldOut;
    chObj->deiCtxInfo.numMv =
        deiCoreCtxInfo->numMvIn + deiCoreCtxInfo->numMvOut;
    chObj->deiCtxInfo.numMvstm =
        deiCoreCtxInfo->numMvstmIn + deiCoreCtxInfo->numMvstmOut;
    /* For mode 1, N-1 buffer will be provided through framelist */
    if ((VPS_DEIHQ_CTXMODE_APP_N_1 == hObj->deiHqCtxMode)
        && (chObj->deiCtxInfo.numFld > 0u))
    {
        chObj->deiCtxInfo.numFld--;
    }

    /*
     * Fill the context buffer size. Align to VPDMA line stride
     * requirement.
     */
    /* Field buffers are Y and CbCr separate (NV16 format).
     * Hence line stride equal to width. */
    lineStride = width;
    lineStride = VpsUtils_align(lineStride, VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
    /* Allocate for both Y and C - multiply by 2. */
    chObj->deiCtxInfo.fldBufSize = (lineStride * height * 2u);
    if (TRUE == chObj->chPrms.comprEnable)
    {
        /* When compressor is enabled, half the buffer size is sufficient. */
        chObj->deiCtxInfo.fldBufSize /= 2u;
    }

    /* MV and MVSTM are 4-bits per pixel. Hence line stride equal to
     * width by 2 */
    lineStride = width / 2u;
    lineStride = VpsUtils_align(lineStride, VPSHAL_VPDMA_LINE_STRIDE_ALIGN);
    chObj->deiCtxInfo.mvBufSize = (lineStride * height);
    chObj->deiCtxInfo.mvstmBufSize = (lineStride * height);

    return (retVal);
}



/**
 *  vpsMdrvDeiFreeChCtxMem
 *  Frees the allocated DEI context buffers of the channel.
 */
static Int32 vpsMdrvDeiFreeChCtxMem(VpsMdrv_DeiChObj *chObj)
{
    Int32           retVal = FVID2_SOK;

    /* TODO: In case if we decide to allocate context buffer in driver,
     * free them here */

    return (retVal);
}



/**
 *  vpsMdrvDeiSplitChDescMem
 *  Splits the allocated descriptor and shadow overlay memories of a channel
 *  according to the requirement of each core.
 */
static Int32 vpsMdrvDeiSplitChDescMem(VpsMdrv_DeiHandleObj *hObj,
                                      VpsMdrv_DeiChObj *chObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  descSet, coreCnt, cnt, ptrCnt, i;
    Vcore_DescInfo          descInfo[VPSMDRV_DEI_MAX_CORE];
    const Vcore_Ops        *coreOps;
    VpsMdrv_DeiInstObj     *instObj;
    UInt8                  *descPtr;
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
    UInt32                  totalDescCnt;
#endif

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj->descMem));

    /* Get the descriptor info of all cores only once */
    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->getDescInfo));

            /* Get the descriptor info of each core */
            retVal = coreOps->getDescInfo(
                         hObj->coreHandle[coreCnt],
                         chObj->chNum,
                         0u,
                         &descInfo[coreCnt]);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not get core info for core %d!\n", coreCnt);
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Assign main data descriptor memory pointers */
        descPtr = chObj->descMem;
        GT_assert(VpsMdrvDeiTrace, (NULL != descPtr));
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
            /* Reset the counter for every set */
            totalDescCnt = 0u;
#endif

            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                for (i = 0u; i < chObj->numCoeffDesc; i++)
                {
                    chObj->coeffDesc[descSet][i] = (VpsHal_VpdmaConfigDesc *)
                            (  ((UInt32) descPtr)
                             + (i * VPSHAL_VPDMA_CONFIG_DESC_SIZE));
                    /* Create a dummy descriptor to initialize. */
                    VpsHal_vpdmaCreateDummyDesc(
                        chObj->coeffDesc[descSet][i]);
                }
                descPtr +=
                    (chObj->numCoeffDesc * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            }
            else
            {
                /* For per-handle config, point the channel descriptors to the
                 * handle coeff descriptors. */
                for (i = 0u; i < hObj->numCoeffDesc; i++)
                {
                    chObj->coeffDesc[descSet][i] =
                        hObj->coeffDesc[descSet][i];
                    /* Handle coeff descriptors are already initialized to dummy
                     * descriptors. */
                }
            }

            /* Assign shadow and non-shadow config descriptors if needed */
            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                chObj->nshwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += (VPSMDRV_DEI_MAX_NSHW_CFG_DESC *
                            VPSHAL_VPDMA_CONFIG_DESC_SIZE);

                chObj->shwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += (VPSMDRV_DEI_MAX_SHW_CFG_DESC *
                            VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            }
            else
            {
                /* Point it to handle config descriptor */
                chObj->nshwCfgDesc[descSet] = hObj->nshwCfgDesc[descSet];
                chObj->shwCfgDesc[descSet] = hObj->shwCfgDesc[descSet];
            }

            /* Set the first descriptor in the layout to the appropriate
             * descriptor depending on the configuration. This value is
             * used for configuring the reload descriptor at runtime.
             * Assume that Lazy Loading is disabled at create time, and set the
             * firstDesc to the non-shadow config descriptor.
             */
            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                chObj->firstDesc[descSet] = chObj->nshwCfgDesc[descSet];
            }
            else
            {
                /* Else if the configuration is per handle first descriptor in
                 * the layout will be first outbound data descriptor */
                chObj->firstDesc[descSet] = descPtr;
            }

            /*
             * Split the main descriptor and overlay memory for each core.
             */
            /* First assign out data descriptor for all cores */
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    for (cnt = 0u;
                         cnt < descInfo[coreCnt].numOutDataDesc;
                         cnt++)
                    {
                        chObj->coreDescMem[descSet][coreCnt].outDataDesc[cnt]
                            = descPtr;
                        descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
                        /* Program a SOT descriptor every
                         * VPS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT data
                         * descriptor.
                         */
                        totalDescCnt++;
                        if ((totalDescCnt %
                                VPS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT) == 0u)
                        {
                            /* Program SOT descriptor */
                            VpsHal_vpdmaCreateSOTCtrlDesc(
                                descPtr,
                                VPS_CFG_VPDMA_MOSIAC_SOT_DESC_WAIT_CYCLES);
                            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                        }
#endif
                    }
                }
            }

            /* Then assign in data descriptor for all cores */
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    for (cnt = 0u;
                         cnt < descInfo[coreCnt].numInDataDesc;
                         cnt++)
                    {
                        chObj->coreDescMem[descSet][coreCnt].inDataDesc[cnt]
                            = descPtr;
                        descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
                        /* Program a SOT descriptor every
                         * VPS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT data
                         * descriptor.
                         */
                        totalDescCnt++;
                        if ((totalDescCnt %
                                VPS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT) == 0u)
                        {
                            /* Program SOT descriptor */
                            VpsHal_vpdmaCreateSOTCtrlDesc(
                                descPtr,
                                VPS_CFG_VPDMA_MOSIAC_SOT_DESC_WAIT_CYCLES);
                            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                        }
#endif
                    }
                }
            }

            /* Then assign multi window data descriptor for all cores */
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    for (cnt = 0u;
                         cnt < descInfo[coreCnt].numMultiWinDataDesc;
                         cnt++)
                    {
                        chObj->coreDescMem[descSet][coreCnt]
                            .multiWinDataDesc[cnt] = descPtr;
                        descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;

#ifdef VPS_CFG_VPDMA_MOSIAC_USE_SOT
                        /* Program a SOT descriptor every
                         * VPS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT data
                         * descriptor.
                         */
                        totalDescCnt++;
                        if ((totalDescCnt %
                                VPS_CFG_VPDMA_MOSIAC_NUM_DESC_PER_SOT) == 0u)
                        {
                            /* Program SOT descriptor */
                            VpsHal_vpdmaCreateSOTCtrlDesc(
                                descPtr,
                                VPS_CFG_VPDMA_MOSIAC_SOT_DESC_WAIT_CYCLES);
                            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                        }
#endif
                    }
                }
            }

            /* Assign SOCH descriptor */
            chObj->sochDesc[descSet] =
                (VpsHal_VpdmaSyncOnChannelDesc *) descPtr;
            /* Use all channels returned by the cores as SOCH */
            descPtr += (hObj->numVpdmaChannels * VPSHAL_VPDMA_CTRL_DESC_SIZE);

            /* Assign reload descriptor */
            chObj->rldDesc[descSet] = (VpsHal_VpdmaReloadDesc *) descPtr;
            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }

        /* Assign write descriptors if compressor is needed */
        if (((TRUE == chObj->deiCoreCtxInfo.isDeinterlacing) ||
                (TRUE == chObj->deiCoreCtxInfo.isTnrMode))
            && (TRUE == chObj->chPrms.comprEnable))
        {
            GT_assert(VpsMdrvDeiTrace, (NULL != chObj->wrDescMem));
            descPtr = chObj->wrDescMem;
            for (cnt = 0u; cnt < chObj->deiCoreCtxInfo.numCurFldOut; cnt++)
            {
                for (ptrCnt = 0u;
                     ptrCnt < VCORE_DEI_NUM_DESC_PER_FIELD;
                     ptrCnt++)
                {
                    chObj->curOutWriteDesc[cnt][ptrCnt] = descPtr;
                    descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
                }
            }
        }

        /* Assign non-shadow overlay memory pointers */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            descPtr = chObj->nshwOvlyMem;
        }
        else
        {
            /* If per config channel is FALSE, then use handle overlay memory
             * pointer */
            descPtr = hObj->nshwOvlyMem;
        }
        GT_assert(VpsMdrvDeiTrace, (NULL != descPtr));
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    if (descInfo[coreCnt].nonShadowOvlySize > 0u)
                    {
                        chObj->coreDescMem[descSet][coreCnt].nonShadowOvlyMem
                            = descPtr;
                        descPtr += descInfo[coreCnt].nonShadowOvlySize;
                    }
                    else
                    {
                        chObj->coreDescMem[descSet][coreCnt].nonShadowOvlyMem
                            = NULL;
                    }
                }
            }
        }

        /* Assign shadow overlay memory pointers */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            descPtr = chObj->shwOvlyMem;
        }
        else
        {
            /* If per config channel is FALSE, then use handle overlay memory
             * pointer */
            descPtr = hObj->shwOvlyMem;
        }
        GT_assert(VpsMdrvDeiTrace, (NULL != descPtr));
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    if (descInfo[coreCnt].shadowOvlySize > 0u)
                    {
                        chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem
                            = descPtr;
                        descPtr += descInfo[coreCnt].shadowOvlySize;
                    }
                    else
                    {
                        chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem
                            = NULL;
                    }
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiProgramChDesc
 *  Program the core descriptors and overlay memories for a channel.
 */
static Int32 vpsMdrvDeiProgramChDesc(VpsMdrv_DeiHandleObj *hObj,
                                     VpsMdrv_DeiChObj *chObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, descSet, coreCnt;
    UInt8              *tempPtr;
    VpsMdrv_DeiInstObj *instObj;
    const Vcore_Ops    *coreOps;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    for (coreCnt = 0u; coreCnt < VPSMDRV_DEI_MAX_CORE; coreCnt++)
    {
        /* Program cores only if needed for that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->programDesc));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->programReg));

            for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
            {
                /* Program the core descriptors */
                retVal = coreOps->programDesc(
                             hObj->coreHandle[coreCnt],
                             chObj->chNum,
                             0u,
                             &chObj->coreDescMem[descSet][coreCnt]);
                if (FVID2_SOK != retVal)
                {
                    GT_3trace(VpsMdrvDeiTrace, GT_ERR,
                        "Could not program desc set %d of channel "
                        "%d for core %d!\n", descSet, chObj->chNum, coreCnt);
                    break;
                }

                /* Program the overlay memory */
                retVal = coreOps->programReg(
                             hObj->coreHandle[coreCnt],
                             chObj->chNum,
                             &chObj->coreDescMem[descSet][coreCnt]);
                if (FVID2_SOK != retVal)
                {
                    GT_3trace(VpsMdrvDeiTrace, GT_ERR,
                        "Could not program overlay set %d of channel "
                        "%d for core %d!\n",
                        descSet, chObj->chNum, coreCnt);
                    break;
                }
            }

            /* Break if error occurs */
            if (FVID2_SOK != retVal)
            {
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            /* Program frame start event of DEI core in overlay memory */
            coreOps = instObj->coreOps[VPSMDRV_DEI_DEI_CORE_IDX];
            GT_assert(VpsMdrvDeiTrace,
                (TRUE == instObj->isCoreReq[VPSMDRV_DEI_DEI_CORE_IDX]));
            GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->setFsEvent));

            retVal =
                coreOps->setFsEvent(
                    hObj->coreHandle[VPSMDRV_DEI_DEI_CORE_IDX],
                    chObj->chNum,
                    VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE,
                    &chObj->coreDescMem[descSet][VPSMDRV_DEI_DEI_CORE_IDX]);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Could not set frame start for DEI Core!\n");
                break;
            }

            /* Program non-shadow config descriptor. Point to the first
             * cores overlay memory */
            GT_assert(VpsMdrvDeiTrace, (NULL != chObj->nshwCfgDesc[descSet]));
            GT_assert(VpsMdrvDeiTrace, (NULL !=
                chObj->coreDescMem[descSet][0u].nonShadowOvlyMem));
            GT_assert(VpsMdrvDeiTrace, (0u != chObj->totalNshwOvlyMem));
            VpsHal_vpdmaCreateConfigDesc(
                chObj->nshwCfgDesc[descSet],
                VPSHAL_VPDMA_CONFIG_DEST_MMR,
                VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                (chObj->totalNshwOvlyMem / VPSMDRV_DEI_MAX_DESC_SET),
                chObj->coreDescMem[descSet][0u].nonShadowOvlyMem,
                NULL,
                0u);

            /* Program non-shadow config descriptor. Point to the first
             * cores overlay memory */
            GT_assert(VpsMdrvDeiTrace, (NULL != chObj->shwCfgDesc[descSet]));
            GT_assert(VpsMdrvDeiTrace, (NULL !=
                chObj->coreDescMem[descSet][0u].shadowOvlyMem));
            GT_assert(VpsMdrvDeiTrace, (0u != chObj->totalShwOvlyMem));
            VpsHal_vpdmaCreateConfigDesc(
                chObj->shwCfgDesc[descSet],
                VPSHAL_VPDMA_CONFIG_DEST_MMR,
                VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                (chObj->totalShwOvlyMem / VPSMDRV_DEI_MAX_DESC_SET),
                chObj->coreDescMem[descSet][0u].shadowOvlyMem,
                NULL,
                0u);

            /* Program the SOCH descriptors */
            tempPtr = (UInt8 *) chObj->sochDesc[descSet];
            for (chCnt = 0u; chCnt < hObj->numVpdmaChannels; chCnt++)
            {
                VpsHal_vpdmaCreateSOCHCtrlDesc(
                    tempPtr,
                    hObj->socChNum[chCnt]);
                tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Mark the dirty flags as clean */
        for (descSet = 0u; descSet < VPSMDRV_DEI_MAX_DESC_SET; descSet++)
        {
            chObj->isDescDirty[descSet] = FALSE;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiAllocChObjs
 *  Allocate channel objects for the handle.
 */
static Int32 vpsMdrvDeiAllocChObjs(VpsMdrv_DeiHandleObj *hObj, UInt32 numCh)
{
    Int32               retVal = FVID2_SOK, tempRetVal;
    UInt32              chCnt, errCnt, ctxIndex;
    VpsMdrv_DeiChObj   *chObj;
    VpsMdrv_DeiInstObj *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    /* Check for out of bound */
    GT_assert(VpsMdrvDeiTrace, (numCh <= VPS_M2M_DEI_MAX_CH_PER_HANDLE));

    /* Allocate channel objects */
    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        chObj = (VpsMdrv_DeiChObj *) VpsUtils_alloc(
                                         &gVpsMdrvDeiPoolObj.chPoolPrm,
                                         sizeof(VpsMdrv_DeiChObj),
                                         VpsMdrvDeiTrace);
        if (NULL == chObj)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Channel memory alloc failed: %d!!\n", chCnt);
            retVal = FVID2_EALLOC;
            break;
        }
        hObj->chObjs[chCnt] = chObj;
        chObj->chNum = chCnt;

        /* Initialize field context buffer, Will be updated
            1. Set to the first field - for the very first field
            2. When user gives new field for de-interlacing - current field
               moved to fieldCtxBufs[0] and contents of fieldCtxBufs[0] to
               fieldCtxBufs[1] and contenct of fieldCtxBufs[1] to
               fieldCtxBufs[2] and until VPSMDRV_DEI_MAX_FIELD_CONTEXT - 1 */
        for (ctxIndex = 0u;
             ctxIndex <= VPSMDRV_DEI_MAX_FIELD_CONTEXT;
             ctxIndex++)
        {
            chObj->ctxFieldFifo[ctxIndex].fieldCtxBuf = NULL;
            chObj->ctxFieldFifo[ctxIndex].tth = 0u;
        }
        chObj->isCtxFieldInit = FALSE;

        retVal = VpsUtils_queCreate(
                     &(chObj->agedCtxFieldQ),
                     VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE + 1u,
                     chObj->sapce4AgedCtxFieldQ,
                     VPSUTILS_QUE_FLAG_NO_BLOCK_QUE);
        if (retVal != FVID2_SOK)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Channel memory alloc - alloc of agedQ fails %d!!\n", chCnt);
            retVal = FVID2_EALLOC;
            break;
        }
    }

    if (FVID2_SOK != retVal)
    {
        /* Free-up the already allocated channel memories */
        for (errCnt = 0u; errCnt < chCnt; errCnt++)
        {
            hObj->chObjs[errCnt]->chNum = 0u;
            tempRetVal = VpsUtils_free(
                             &gVpsMdrvDeiPoolObj.chPoolPrm,
                             hObj->chObjs[errCnt],
                             VpsMdrvDeiTrace);
            GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
            hObj->chObjs[errCnt] = NULL;
        }
    }
    else
    {
        instObj->numChAllocated += numCh;
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiFreeChObjs
 *  Frees the channel and scratch pad objects of the handle.
 */
static Int32 vpsMdrvDeiFreeChObjs(VpsMdrv_DeiHandleObj *hObj, UInt32 numCh)
{
    Int32               retVal = FVID2_SOK, tempRetVal;
    UInt32              chCnt;
    VpsMdrv_DeiChObj   *chObj;
    VpsMdrv_DeiInstObj *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));
    /* Check for out of bound */
    GT_assert(VpsMdrvDeiTrace, (numCh <= VPS_M2M_DEI_MAX_CH_PER_HANDLE));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        if (NULL != chObj)
        {
            chObj->chNum = 0u;
            tempRetVal = VpsUtils_free(
                             &gVpsMdrvDeiPoolObj.chPoolPrm,
                             chObj,
                             VpsMdrvDeiTrace);
            GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
            hObj->chObjs[chCnt] = NULL;
        }
    }

    GT_assert(VpsMdrvDeiTrace, (instObj->numChAllocated >= numCh));
    instObj->numChAllocated -= numCh;

    return (retVal);
}



/**
 *  vpsMdrvDeiCopyChPrms
 *  Copies the channel parameters from create params to channel objects.
 */
static Int32 vpsMdrvDeiCopyChPrms(VpsMdrv_DeiHandleObj *hObj,
                                  const Vps_M2mDeiChParams *inChPrms)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  chCnt;
    Vps_M2mDeiChParams     *chPrms;
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    UInt32                  drvInstId;
#endif

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != inChPrms));

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    drvInstId = hObj->instObj->drvInstId;
#endif

    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != hObj->chObjs[chCnt]));

        chPrms = &hObj->chObjs[chCnt]->chPrms;

        chPrms->inMemType = inChPrms->inMemType;
        chPrms->outMemTypeDei = inChPrms->outMemTypeDei;
        chPrms->outMemTypeVip = inChPrms->outMemTypeVip;
#ifdef VPS_HAL_INCLUDE_DRN
        chPrms->drnEnable = inChPrms->drnEnable;
#else
        /* Override user settings if DRN is not present */
        chPrms->drnEnable = FALSE;
#endif  /* #ifdef VPS_HAL_INCLUDE_DRN */

#ifdef VPS_HAL_INCLUDE_COMPR
        chPrms->comprEnable = inChPrms->comprEnable;
#else
        /* Override user settings if compressor is not present */
        chPrms->comprEnable = FALSE;
#endif  /* #ifdef VPS_HAL_INCLUDE_COMPR */

        /* Copy the DEI buffer format */
        VpsUtils_memcpy(&chPrms->inFmt, &inChPrms->inFmt, sizeof(FVID2_Format));

        /* Copy the DEI out buffer format if present */
        if (NULL != inChPrms->outFmtDei)
        {
            chPrms->outFmtDei = &hObj->chObjs[chCnt]->outFmtDei;
            VpsUtils_memcpy(
                chPrms->outFmtDei,
                inChPrms->outFmtDei,
                sizeof(FVID2_Format));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->outFmtDei = &hObj->chObjs[chCnt]->outFmtDei;
            VpsUtils_memcpy(
                chPrms->outFmtDei,
                &VpsMdrvDeiDefFmt,
                sizeof(FVID2_Format));
        }

        /* Copy the VIP out buffer format if present */
        if (NULL != inChPrms->outFmtVip)
        {
            chPrms->outFmtVip = &hObj->chObjs[chCnt]->outFmtVip;
            VpsUtils_memcpy(
                chPrms->outFmtVip,
                inChPrms->outFmtVip,
                sizeof(FVID2_Format));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->outFmtVip = &hObj->chObjs[chCnt]->outFmtVip;
            VpsUtils_memcpy(
                chPrms->outFmtVip,
                &VpsMdrvDeiDefFmt,
                sizeof(FVID2_Format));
        }

        /* Copy the N-1 field buffer format if present */
        if ((VPS_DEIHQ_CTXMODE_APP_N_1 == hObj->deiHqCtxMode) &&
            (NULL != inChPrms->inFmtFldN_1))
        {
            chPrms->inFmtFldN_1 = &hObj->chObjs[chCnt]->inFmtFldN_1;
            VpsUtils_memcpy(
                chPrms->inFmtFldN_1,
                inChPrms->inFmtFldN_1,
                sizeof(FVID2_Format));
        }
        else
        {
            chPrms->inFmtFldN_1 = NULL;
        }

        /* Copy the DEI HQ parameters */
        if (NULL != inChPrms->deiHqCfg)
        {
            chPrms->deiHqCfg = &hObj->chObjs[chCnt]->deiHqCfg;
            VpsUtils_memcpy(
                chPrms->deiHqCfg,
                inChPrms->deiHqCfg,
                sizeof(Vps_DeiHqConfig));
        }
        else
        {
            chPrms->deiHqCfg = NULL;
        }

        /* Copy the DEI parameters */
        if (NULL != inChPrms->deiCfg)
        {
            chPrms->deiCfg = &hObj->chObjs[chCnt]->deiCfg;
            VpsUtils_memcpy(
                chPrms->deiCfg,
                inChPrms->deiCfg,
                sizeof(Vps_DeiConfig));
        }
        else
        {
            chPrms->deiCfg = NULL;

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
            if ((drvInstId == VPS_M2M_INST_AUX_SC2_WB1) ||
                (drvInstId == VPS_M2M_INST_AUX_SC4_VIP1) ||
                (drvInstId == VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1))
            {
                chPrms->deiCfg = &hObj->chObjs[chCnt]->deiCfg;
                VpsUtils_memcpy(
                    chPrms->deiCfg,
                    &VpsMdrvDeiDefDeiCfg,
                    sizeof(Vps_DeiConfig));
            }
#endif   /* #ifdef TI_814X_BUILD || TI_8107_BUILD */
        }

        /* Copy the scalar parameters */
        if (NULL != inChPrms->scCfg)
        {
            chPrms->scCfg = &hObj->chObjs[chCnt]->scCfg;
            VpsUtils_memcpy(
                chPrms->scCfg,
                inChPrms->scCfg,
                sizeof(Vps_ScConfig));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->scCfg = &hObj->chObjs[chCnt]->scCfg;
            VpsUtils_memcpy(
                chPrms->scCfg,
                &VpsMdrvDeiDefScCfg,
                sizeof(Vps_ScConfig));
        }

        /* Copy the crop parameters */
        if (NULL != inChPrms->deiCropCfg)
        {
            chPrms->deiCropCfg = &hObj->chObjs[chCnt]->deiCropCfg;
            VpsUtils_memcpy(
                chPrms->deiCropCfg,
                inChPrms->deiCropCfg,
                sizeof(Vps_CropConfig));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->deiCropCfg = &hObj->chObjs[chCnt]->deiCropCfg;
            VpsUtils_memcpy(
                chPrms->deiCropCfg,
                &VpsMdrvDeiDefCropCfg,
                sizeof(Vps_CropConfig));
        }

        /* Copy the VIP scalar parameters */
        if (NULL != inChPrms->vipScCfg)
        {
            chPrms->vipScCfg = &hObj->chObjs[chCnt]->vipScCfg;
            VpsUtils_memcpy(
                chPrms->vipScCfg,
                inChPrms->vipScCfg,
                sizeof(Vps_ScConfig));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->vipScCfg = &hObj->chObjs[chCnt]->vipScCfg;
            VpsUtils_memcpy(
                chPrms->vipScCfg,
                &VpsMdrvDeiDefScCfg,
                sizeof(Vps_ScConfig));
        }

        /* Copy the VIP scalar crop parameters */
        if (NULL != inChPrms->vipScCfg)
        {
            chPrms->vipCropCfg = &hObj->chObjs[chCnt]->vipCropCfg;
            VpsUtils_memcpy(
                chPrms->vipCropCfg,
                inChPrms->vipCropCfg,
                sizeof(Vps_CropConfig));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->vipCropCfg = &hObj->chObjs[chCnt]->vipCropCfg;
            VpsUtils_memcpy(
                chPrms->vipCropCfg,
                &VpsMdrvDeiDefCropCfg,
                sizeof(Vps_CropConfig));
        }

        /* Copy  SubFrame procesing config parameters and initialize subframe
         * Instance of the Channel*/
        if (NULL != inChPrms->subFrameParams)
        {
            VpsMdrv_SubFrameChInst *slcObjPtr;

            slcObjPtr = &hObj->chObjs[chCnt]->slcChObj;
            chPrms->subFrameParams= &hObj->chObjs[chCnt]->subFrameInitPrms;
            VpsUtils_memcpy(
                chPrms->subFrameParams,
                inChPrms->subFrameParams,
                sizeof(Vps_SubFrameParams));

            /* Update sliceInstance in chObj required for scPh and offset
             * calculation*/
            slcObjPtr->subFrameMdEnable =
                chPrms->subFrameParams->subFrameModeEnable;
            slcObjPtr->slcSz = chPrms->subFrameParams->numLinesPerSubFrame;
            slcObjPtr->srcFrmH = chPrms->inFmt.height;
            slcObjPtr->dataFmt = chPrms->inFmt.dataFormat;
            slcObjPtr->tarFrmH = chPrms->outFmtDei->height;
            slcObjPtr->srcFrmW = chPrms->inFmt.width;
            slcObjPtr->tarFrmW = chPrms->outFmtDei->width;
            slcObjPtr->filtType = chPrms->scCfg->vsType;
            /* Check if sliceSize divides frame into Integer number of
             * subframes */
            if (0u == (slcObjPtr->srcFrmH%slcObjPtr->slcSz))
            {
                slcObjPtr->noOfSlcs = (slcObjPtr->srcFrmH / slcObjPtr->slcSz);
            }
            else
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "EBADARGS: NoOfSubFrames not integer, adjust sliceSize \n");
                retVal = FVID2_EBADARGS;
            }
        }
        else
        {
            VpsMdrv_SubFrameChInst *slcObjPtr;

            slcObjPtr = &hObj->chObjs[chCnt]->slcChObj;
            /* Use the default parameters if user fails to give it */
            chPrms->subFrameParams = &hObj->chObjs[chCnt]->subFrameInitPrms;
            VpsUtils_memcpy(
                chPrms->subFrameParams,
                &VpsMdrvDeiDefSubFramePrms,
                sizeof(Vps_SubFrameParams));

            /* Update sliceInstance in chObj required for scPh and offset
             * calculation*/
            slcObjPtr->subFrameMdEnable = FALSE;
            slcObjPtr->slcSz = chPrms->inFmt.height;
            slcObjPtr->srcFrmH = chPrms->inFmt.height;
            slcObjPtr->dataFmt = chPrms->inFmt.dataFormat;
            slcObjPtr->tarFrmH = chPrms->outFmtDei->height;
            slcObjPtr->filtType = chPrms->scCfg->vsType;
            slcObjPtr->srcFrmW = chPrms->inFmt.width;
            slcObjPtr->tarFrmW = chPrms->outFmtDei->width;
            slcObjPtr->noOfSlcs = (slcObjPtr->srcFrmH / slcObjPtr->slcSz);
        }

        /* If the configuration is per handle user will give only one channel
         * channel parameter. Hence for per handle don't increment user
         * channel parameter. Increment only for per channel configuration */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            inChPrms++;
        }
    }

    return (retVal);
}
