/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDeiApi.c
 *
 *  \brief VPS DEI M2M driver for the DEI HQ and DEI paths.
 *  This file implements the VPS DEI M2M driver.
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

static Fdrv_Handle VpsMdrv_deiCreate(UInt32 drvId,
                                     UInt32 instId,
                                     Ptr createArgs,
                                     Ptr createStatusArgs,
                                     const FVID2_DrvCbParams *fdmCbPrms);
static Int32 VpsMdrv_deiDelete(Fdrv_Handle handle, Ptr deleteArgs);
static Int32 VpsMdrv_deiControl(Fdrv_Handle handle,
                                UInt32 cmd,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs);
static Int32 VpsMdrv_deiProcessFrms(Fdrv_Handle handle,
                                    FVID2_ProcessList *procList);
static Int32 VpsMdrv_deiGetProcessedFrms(Fdrv_Handle handle,
                                         FVID2_ProcessList *procList,
                                         UInt32 timeout);

static Int32 vpsMdrvDeiGetCtxInfoIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
static Int32 vpsMdrvDeiSetCtxBufIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
static Int32 vpsMdrvDeiGetCtxBufIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);

static Int32 vpsMdrvDeiSetScCoeffIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs);
static Int32 vpsMdrvDeiSetScLazyLoadingIoctl(VpsMdrv_DeiHandleObj *hObj,
                                             Ptr cmdArgs);

static Int32 vpsMdrvDeiCheckProcList(VpsMdrv_DeiHandleObj *hObj,
                                     FVID2_ProcessList *procList);
static UInt32 vpsMdrvDeiCheckParams(const Vps_M2mDeiCreateParams *createPrms,
                                    VpsMdrv_DeiInstObj *instObj,
                                    UInt32 instId);

static Int32 vpsMdrvDeiModifyLazyLoadingCfg(VpsMdrv_DeiHandleObj *hObj,
                                            UInt32 scalarId,
                                            UInt32 enableLazyLoadingModified,
                                            UInt32 enableFilterSelectModified);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** DEI M2M driver function pointer. */
static const FVID2_DrvOps VpsMdrvDeiOps =
{
    FVID2_VPS_M2M_DEI_DRV,          /* Driver ID */
    VpsMdrv_deiCreate,              /* Create */
    VpsMdrv_deiDelete,              /* Delete */
    VpsMdrv_deiControl,             /* Control */
    NULL,                           /* Queue */
    NULL,                           /* Dequeue */
    VpsMdrv_deiProcessFrms,         /* ProcessFrames */
    VpsMdrv_deiGetProcessedFrms     /* GetProcessedFrames */
};


/* ========================================================================== */
/*                          FVID2 API Functions                               */
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
 *                      structure of size numInst.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_deiInit(UInt32 numInst,
                      const VpsMdrv_DeiInitParams *initPrms,
                      Ptr arg)
{
    Int32       retVal = FVID2_SOK;

    /* Check for errors */
    GT_assert(VpsMdrvDeiTrace, (NULL != initPrms));

    /* Initialize the internal objects */
    retVal = vpsMdrvDeiInit(numInst, initPrms);
    if (FVID2_SOK == retVal)
    {
        /* Register the driver to the driver manager */
        retVal = FVID2_registerDriver(&VpsMdrvDeiOps);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Registering to FVID2 driver manager failed\n");
            /* Uninitialize the internal objects if error occurs */
            vpsMdrvDeiDeInit();
        }
    }

    return (retVal);
}



/**
 *  VpsMdrv_deiDeInit
 *  \brief DEI M2M driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_deiDeInit(Ptr arg)
{
    Int32       retVal = FVID2_SOK;

    /* Unregister from driver manager */
    retVal = FVID2_unRegisterDriver(&VpsMdrvDeiOps);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Unregistering from FVID2 driver manager failed\n");
    }

    /* Uninitialize the internal objects */
    retVal |= vpsMdrvDeiDeInit();

    return (retVal);
}



/**
 *  VpsMdrv_deiCreate
 */
static Fdrv_Handle VpsMdrv_deiCreate(UInt32 drvId,
                                     UInt32 instId,
                                     Ptr createArgs,
                                     Ptr createStatusArgs,
                                     const FVID2_DrvCbParams *fdmCbPrms)
{
    Int32                   retVal, tempRetVal;
    UInt32                  coreOpenFlag = FALSE, coreAllocFlag = FALSE;
    UInt32                  resrcAllocFlag = FALSE;
    VpsMdrv_DeiInstObj     *instObj = NULL;
    Vps_M2mDeiCreateParams *createPrms;
    VpsMdrv_DeiHandleObj   *hObj = NULL;
    Vps_M2mDeiCreateStatus *createStatus;
    UInt32                  scalarId;
    Vps_ScCoeffParams       coeffParams;

    /* Check for NULL pointers */
    if ((NULL == createArgs) ||
        (NULL == createStatusArgs) ||
        (NULL == fdmCbPrms))
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Get the instance object for this instance */
        instObj = vpsMdrvDeiGetInstObj(instId);
        if (NULL == instObj)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid instance ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
        else
        {
            retVal = FVID2_SOK;
        }
    }

    if (NULL != instObj)
    {
        /* Take the instance semaphore */
        Semaphore_pend(instObj->instSem, VPSMDRV_DEI_SEM_TIMEOUT);
    }

    /* Update the coeff params to default values */
    coeffParams.coeffPtr = NULL;
    coeffParams.hScalingSet = VPS_SC_US_SET;
    coeffParams.vScalingSet = VPS_SC_US_SET;

    if (FVID2_SOK == retVal)
    {
        /* Check if a instance is opened more than the supported handles */
        if (instObj->openCnt >= VPS_M2M_DEI_MAX_HANDLE_PER_INST)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Only %d handles supported per instance!\n",
                VPS_M2M_DEI_MAX_HANDLE_PER_INST);
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check for valid create parameters */
        createPrms = (Vps_M2mDeiCreateParams *) createArgs;
        retVal = vpsMdrvDeiCheckParams(createPrms, instObj, instId);
    }

    if (FVID2_SOK == retVal)
    {
        /* Based on createPrms, set scalarCoreId for VIP scalar. */
        if (TRUE == createPrms->isVipScReq)
        {
            instObj->scalarCoreId[VPS_M2M_DEI_SCALAR_ID_VIP_SC] =
                VPSMDRV_DEI_VIP_CORE_IDX;
        }

        /* Allocate handle object memory including some of the members */
        hObj = vpsMdrvDeiAllocHandleObj(
                   createPrms,
                   instObj,
                   fdmCbPrms,
                   &retVal);
        if ((NULL == hObj) || (FVID2_SOK != retVal))
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Handle allocation failed\n");
        }
        if ((NULL == hObj) && (FVID2_SOK != retVal))
        {
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Open the cores */
        retVal = vpsMdrvDeiOpenCores(hObj);
        if (FVID2_SOK == retVal)
        {
            coreOpenFlag = TRUE;
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Opening of cores failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate core memories */
        retVal = vpsMdrvDeiAllocCoreMem(hObj);
        if (FVID2_SOK == retVal)
        {
            coreAllocFlag = TRUE;
            /* Program the descriptor memories of cores */
            retVal = vpsMdrvDeiProgramDesc(hObj);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Programming of cores failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Core memory alloc failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (0u == instObj->openCnt)
        {
            /* Allocate the resources if this is the first time */
            retVal = vpsMdrvDeiAllocResrc(instObj);
            if (FVID2_SOK == retVal)
            {
                resrcAllocFlag = TRUE;

                /* Register with MLM if first handle is opened.
                   Channels are non-static, there is no need to
                   provide channel numbers here. */
                instObj->mlmInfo.reqCompleteCb = VpsMdrv_deiMlmCbFxn;
                instObj->mlmInfo.isChannelStatic = FALSE;
                instObj->mlmHandle = Mlm_register(&instObj->mlmInfo, NULL);
                if (NULL == instObj->mlmHandle)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Registration with MLM failed!\n");
                    retVal = FVID2_EALLOC;
                }
            }
            else
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Resource alloc failed\n");
            }

            if (FVID2_SOK == retVal)
            {
                /* Load default coefficient for the first time create of the
                driver , this is to avoid scalar hang. Also load vip scalar coff
                only if its used by this instance. coeff for dei sc is always
                loaded */
                for (scalarId = 0u; scalarId < VPS_M2M_DEI_SCALAR_ID_MAX; scalarId++)
                {
                    coeffParams.scalarId = scalarId;
                    if ((0u == scalarId) || (createPrms->isVipScReq))
                    {
                        retVal = vpsMdrvDeiSetScCoeffIoctl(hObj , &coeffParams);
                        if (FVID2_SOK != retVal)
                        {
                            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                                "Failed to load default scalar co-efficient!\n");
                            break;
                        }
                    }
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Check Lazy Loading configuration in instance object and setup
         * required fields if it is enabled. */
        for (scalarId = 0u; scalarId < VPS_M2M_DEI_SCALAR_ID_MAX; scalarId++)
        {
            if (TRUE == instObj->enableLazyLoading[scalarId])
            {
                retVal = vpsMdrvDeiModifyLazyLoadingCfg(
                             hObj,
                             scalarId,
                             TRUE,
                             TRUE);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Failed to modify lazy loading config!\n");
                    break;
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Increment the count to keep track of number of handles opened */
        instObj->openCnt++;
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        if (NULL != hObj)
        {
            if (NULL != instObj->mlmHandle)
            {
                /* Unregister from MLM */
                Mlm_unRegister(instObj->mlmHandle);
                instObj->mlmHandle = NULL;
            }

            if (TRUE == resrcAllocFlag)
            {
                /* Free allocated resources */
                tempRetVal = vpsMdrvDeiFreeResrc(instObj);
                GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
            }

            if (TRUE == coreAllocFlag)
            {
                /* Free the memories allocated for cores */
                tempRetVal = vpsMdrvDeiFreeCoreMem(hObj);
                GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
            }

            if (TRUE == coreOpenFlag)
            {
                /* Close the opened cores */
                tempRetVal = vpsMdrvDeiCloseCores(hObj);
                GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
            }

            /* Free allocated handle object */
            tempRetVal = vpsMdrvDeiFreeHandleObj(hObj);
            GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
            hObj = NULL;
        }
    }

    /* Return the status if possible */
    if (NULL != createStatusArgs)
    {
        createStatus = (Vps_M2mDeiCreateStatus *) createStatusArgs;
        createStatus->retVal = retVal;
        if (FVID2_SOK == retVal)
        {
            createStatus->maxHandles = VPS_M2M_DEI_MAX_HANDLE_PER_INST;
            createStatus->maxChPerHandle = VPS_M2M_DEI_MAX_CH_PER_HANDLE;
            createStatus->maxSameChPerRequest = 1u;
            createStatus->maxReqInQueue = VPSMDRV_DEI_MAX_QOBJS_PER_HANDLE;
        }
    }

    if (NULL != instObj)
    {
        /* Post the instance semaphore */
        Semaphore_post(instObj->instSem);
    }

    return ((Fdrv_Handle) hObj);
}



/**
 *  VpsMdrv_deiDelete
 */
static Int32 VpsMdrv_deiDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Int32                   retVal = FVID2_SOK, tempRetVal;
    VpsMdrv_DeiInstObj     *instObj;
    VpsMdrv_DeiHandleObj   *hObj;

    /* Check for NULL pointers */
    if (NULL == handle)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        hObj = handle;
        instObj = hObj->instObj;
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

        /* Check if still some request are pending with driver */
        if (0u != hObj->numPendReq)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Still %d request pending. Dequeue all before closing!!\n",
                hObj->numPendReq);
            retVal = FVID2_EAGAIN;
        }
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
        if (vpsMdrvDeiIsCtxFieldsSet(hObj) == TRUE)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Context fields are not retrived back. Try FVID2_stop () \
                 before closing!!\n");
            retVal = FVID2_EAGAIN;
        }
#endif /* #ifdef TI_814X_BUILD */
#ifdef TI_8107_BUILD
        /*TODO: Requires further debug*/       
	    /*Modified by DVR RDK team. The above checks are not required as 
		  all buffers are owned by the App*/
		retVal = FVID2_SOK;
#endif		

    }

    if (FVID2_SOK == retVal)
    {
        /* Take the instance semaphore */
        Semaphore_pend(instObj->instSem, VPSMDRV_DEI_SEM_TIMEOUT);

        /* Decrement number of handle open count */
        GT_assert(VpsMdrvDeiTrace, (0u != instObj->openCnt));
        instObj->openCnt--;

        /* For the last instance, unregister from MLM and deallocate resource */
        if (0u == instObj->openCnt)
        {
            if (NULL != instObj->mlmHandle)
            {
                /* Unregister from MLM */
                Mlm_unRegister(instObj->mlmHandle);
                instObj->mlmHandle = NULL;
            }

            /* Free allocated resources */
            tempRetVal = vpsMdrvDeiFreeResrc(instObj);
            GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
        }

        /* Free the memories allocated for cores */
        tempRetVal = vpsMdrvDeiFreeCoreMem(hObj);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));

        /* Close the opened cores */
        tempRetVal = vpsMdrvDeiCloseCores(hObj);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));

        /* Free allocated handle object */
        tempRetVal = vpsMdrvDeiFreeHandleObj(hObj);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
        hObj = NULL;

        /* Post the instance semaphore */
        Semaphore_post(instObj->instSem);
    }

    return (retVal);
}



/**
 *  VpsMdrv_deiControl
 */
static Int32 VpsMdrv_deiControl(Fdrv_Handle handle,
                                UInt32 cmd,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs)
{
    Int32                   retVal = FVID2_SOK;
    VpsMdrv_DeiHandleObj   *hObj;

    /* Check for NULL pointers */
    if (NULL == handle)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        hObj = (VpsMdrv_DeiHandleObj *) handle;
        switch (cmd)
        {
            case IOCTL_VPS_GET_DEI_CTX_INFO:
                /* Get DEI context information IOCTL */
                retVal = vpsMdrvDeiGetCtxInfoIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_SET_DEI_CTX_BUF:
                /* Set DEI context buffer IOCTL */
                retVal = vpsMdrvDeiSetCtxBufIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_GET_DEI_CTX_BUF:
                /* Get DEI context buffer IOCTL */
                retVal = vpsMdrvDeiGetCtxBufIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_DEI_OVERRIDE_PREV_FLD_BUF:
                /* Get DEI context buffer IOCTL */
                retVal = vpsMdrvDeiOverridePrevFldBufIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_SET_COEFFS:
                /* Set scalar coefficient IOCTL */
                retVal = vpsMdrvDeiSetScCoeffIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_SC_SET_LAZY_LOADING:
                /* Set scalar lazy loading IOCTL */
                retVal = vpsMdrvDeiSetScLazyLoadingIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_READ_ADV_DEIHQ_CFG:
                /* Read DEI HQ advance configuration IOCTL */
                retVal = vpsMdrvDeiHqRdAdvCfgIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_WRITE_ADV_DEIHQ_CFG:
                /* Write DEI HQ advance configuration IOCTL */
                retVal = vpsMdrvDeiHqWrAdvCfgIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_READ_ADV_DEI_CFG:
                /* Read DEI advance configuration IOCTL */
                retVal = vpsMdrvDeiRdAdvCfgIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_WRITE_ADV_DEI_CFG:
                /* Write DEI advance configuration IOCTL */
                retVal = vpsMdrvDeiWrAdvCfgIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_READ_ADV_SC_CFG:
                /* Read Scalar advance configuration IOCTL */
                retVal = vpsMdrvDeiScRdAdvCfgIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_WRITE_ADV_SC_CFG:
                /* Write Scalar advance configuration IOCTL */
                retVal = vpsMdrvDeiScWrAdvCfgIoctl(hObj, cmdArgs);
                break;

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
            case FVID2_STOP:
                /* Release all the fields queued up and held context fields */
                retVal = vpsMdrvDeiStop(hObj, cmdArgs);
                break;
#endif  /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

            default:
                GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                    "UNSUPPORTED_CMD: IOCTL (%d) not supported\n", cmd);
                retVal = FVID2_EUNSUPPORTED_CMD;
                break;
        }
    }

    return (retVal);
}



/**
 *  VpsMdrv_deiProcessFrms
 */
static Int32 VpsMdrv_deiProcessFrms(Fdrv_Handle handle,
                                    FVID2_ProcessList *procList)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    VpsMdrv_DeiQueueObj    *qObj = NULL;
    VpsMdrv_DeiHandleObj   *hObj = NULL;

    /* Check for NULL pointers */
    if ((NULL == handle) || (NULL == procList))
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (VpsMdrv_DeiHandleObj *) handle;

        /* Check for error in parameters */
        retVal = vpsMdrvDeiCheckProcList(hObj, procList);
    }

    /* Disable global interrupts */
    cookie = Hwi_disable();

    if (FVID2_SOK == retVal)
    {
        /* Check if the driver was stopped, being re-started (with new fields)
         * ensure to clear up the flag */
        if (hObj->isStopped == TRUE)
        {
            hObj->isStopped = FALSE;
        }

        /* Get a queue object from the free queue */
        qObj = VpsUtils_dequeue(hObj->freeQ);
        if (NULL == qObj)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_DEBUG,
                "Failed to allocate queue object. Try again!!\n");
            retVal = FVID2_EAGAIN;
        }
        else
        {
            /* Copy the application's process list to driver's process list */
            FVID2_copyProcessList(&qObj->procList, procList);
            qObj->qObjType = VPSMDRV_QOBJ_TYPE_FRAMES;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get a free descriptor to work with */
        qObj->descSetInUse = vpsMdrvDeiAllocDescSet(hObj);
        if (VPSMDRV_DEI_INVALID_DESC_SET == qObj->descSetInUse)
        {
            /* We did not get the free desc set so queue the request
             * until the call back is called. */
            VpsUtils_queue(hObj->reqQ, &qObj->qElem, qObj);
        }
        else
        {
            /* We have a free descriptor set, update the descriptors, link the
             * channels and queue it to MLM for processing */
            retVal = vpsMdrvDeiProcessReq(hObj, qObj);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_DEBUG,
                    "Process request failed\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Increment the pending request count */
        hObj->numPendReq++;
        hObj->numUnProcessedReq++;
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        if (NULL != qObj)
        {
            /* Free the descriptor set if already allocated */
            if (VPSMDRV_DEI_INVALID_DESC_SET != qObj->descSetInUse)
            {
                vpsMdrvDeiFreeDescSet(hObj, qObj->descSetInUse);
                qObj->descSetInUse = VPSMDRV_DEI_INVALID_DESC_SET;
            }

            /* Return back the queue object to the free queue */
            VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);
            qObj = NULL;
        }
    }

    /* Restore global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



/**
 *  VpsMdrv_deiGetProcessedFrms
 */
static Int32 VpsMdrv_deiGetProcessedFrms(Fdrv_Handle handle,
                                         FVID2_ProcessList *procList,
                                         UInt32 timeout)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    VpsMdrv_DeiQueueObj    *qObj = NULL;
    VpsMdrv_DeiHandleObj   *hObj = NULL;

    /* Check for NULL pointers */
    if ((NULL == handle) || (NULL == procList))
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (VpsMdrv_DeiHandleObj *) handle;

        /* Disable global interrupts */
        cookie = Hwi_disable();

        /* Dequeue the completed request from done queue */
        qObj = VpsUtils_dequeue(hObj->doneQ);
        if (NULL == qObj)
        {
            /* Check if requests are pending with driver */
            if (0u == hObj->numPendReq)
            {
                /* Nothing is queued */
                GT_0trace(VpsMdrvDeiTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\n");
                retVal = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VpsMdrvDeiTrace, GT_DEBUG,
                    "Nothing to dequeue. Try again!!\n");
                retVal = FVID2_EAGAIN;
            }
        }
        else
        {
            /* Copy the driver's process list to application's process list */
            FVID2_copyProcessList(procList, &qObj->procList);

            /* Return back the queue object to the free queue */
            qObj->descSetInUse = VPSMDRV_DEI_INVALID_DESC_SET;
            VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);

            /* Decrement the pending request count. When stopped, there could
               instances when we have no pending requests but application
               might have stopped the driver (instructing to release held
               context fileds.*/
            if (0u != hObj->numPendReq)
            {
                hObj->numPendReq--;
            }
        }

        /* Restore global interrupts */
        Hwi_restore(cookie);
    }

    return (retVal);
}


/* ========================================================================== */
/*                             Local Functions                                */
/* ========================================================================== */

/**
 *  vpsMdrvDeiGetCtxInfoIoctl
 */
static Int32 vpsMdrvDeiGetCtxInfoIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32               retVal = FVID2_SOK;
    Vps_DeiCtxInfo     *deiCtxInfo;
    VpsMdrv_DeiChObj   *chObj;
    Bool                isDeiPresent;

    /* Check for NULL pointers */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    if (FVID2_SOK == retVal)
    {
        deiCtxInfo = (Vps_DeiCtxInfo *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (deiCtxInfo->channelNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the channel object */
        chObj = hObj->chObjs[deiCtxInfo->channelNum];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
        GT_assert(VpsMdrvDeiTrace, (chObj->chNum == deiCtxInfo->channelNum));

        /* Copy the context information from channel object */
        VpsUtils_memcpy(deiCtxInfo, &chObj->deiCtxInfo, sizeof(Vps_DeiCtxInfo));

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
        /* For platform TI814X, apps needs not allocate context buffers for
         * fields Ensure to clear them. */
        deiCtxInfo->numFld      = 0x0u;
        deiCtxInfo->fldBufSize  = 0x0u;
#endif  /* #ifdef TI_814X_BUILD || TI_8107_BUILD */
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiSetCtxBufIoctl
 */
static Int32 vpsMdrvDeiSetCtxBufIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cookie;
    UInt32              bufCnt;
    Vps_DeiCtxBuf      *deiCtxBuf;
    VpsMdrv_DeiChObj   *chObj;
    Bool                isDeiPresent;

    /* Check for NULL pointers */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    /* Disable global interrupts */
    cookie = Hwi_disable();

    if (FVID2_SOK == retVal)
    {
        deiCtxBuf = (Vps_DeiCtxBuf *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (deiCtxBuf->channelNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
        /* Don't allow to set the buffer address when requests are pending
         * with driver */
        if (0u != hObj->numUnProcessedReq)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Can't set context buffers when %d request are pending!!\n",
                hObj->numUnProcessedReq);
            retVal = FVID2_EAGAIN;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the channel object */
        chObj = hObj->chObjs[deiCtxBuf->channelNum];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
        GT_assert(VpsMdrvDeiTrace, (chObj->chNum == deiCtxBuf->channelNum));
        GT_assert(VpsMdrvDeiTrace,
            (chObj->deiCtxInfo.numFld <= VPS_DEI_MAX_CTX_FLD_BUF));
        GT_assert(VpsMdrvDeiTrace,
            (chObj->deiCtxInfo.numMv <= VPS_DEI_MAX_CTX_MV_BUF));
        GT_assert(VpsMdrvDeiTrace,
            (chObj->deiCtxInfo.numMvstm <= VPS_DEI_MAX_CTX_MVSTM_BUF));

#ifdef TI_816X_BUILD
        /* Check whether user has given the right number of context buffers */
        for (bufCnt = 0u; bufCnt < chObj->deiCtxInfo.numFld; bufCnt++)
        {
            if (NULL == deiCtxBuf->fldBuf[bufCnt])
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Field buffer NULL pointer\n");
                retVal = FVID2_EBADARGS;
                break;
            }
        }
#endif  /* #ifdef TI_816X_BUILD */

        for (bufCnt = 0u; bufCnt < chObj->deiCtxInfo.numMv; bufCnt++)
        {
            if (NULL == deiCtxBuf->mvBuf[bufCnt])
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "MV buffer NULL pointer\n");
                retVal = FVID2_EBADARGS;
                break;
            }
        }
        for (bufCnt = 0u; bufCnt < chObj->deiCtxInfo.numMvstm; bufCnt++)
        {
            if (NULL == deiCtxBuf->mvstmBuf[bufCnt])
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "MV buffer NULL pointer\n");
                retVal = FVID2_EBADARGS;
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Copy the context buffer to the channel object */
        VpsUtils_memcpy(&chObj->deiCtxBuf, deiCtxBuf, sizeof(Vps_DeiCtxBuf));
        chObj->isDeiCtxBufSet = TRUE;
    }

    /* Restore global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



/**
 *  vpsMdrvDeiGetCtxBufIoctl
 */
static Int32 vpsMdrvDeiGetCtxBufIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    UInt32                  bufCnt, ptrCnt;
    Vps_DeiCtxBuf          *deiCtxBuf;
    VpsMdrv_DeiChObj       *chObj;
    Vcore_DeiUpdateContext *deiCoreCtxBuf;
    Bool                    isDeiPresent;

    /* Check for NULL pointers */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    isDeiPresent = vpsMdrvDeiIsDeiPresent(hObj);
    if (FALSE == isDeiPresent)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "IOCTL not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_CMD;
    }

    /* Disable global interrupts */
    cookie = Hwi_disable();

    if (FVID2_SOK == retVal)
    {
        deiCtxBuf = (Vps_DeiCtxBuf *) cmdArgs;
        /* Check if the channel number is within the allocated one */
        if (deiCtxBuf->channelNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
        }
        /* Don't allow to get the buffer address when requests are pending
         * with driver */
        if (0u != hObj->numUnProcessedReq)
        {
            GT_1trace(VpsMdrvDeiTrace, GT_ERR,
                "Can't get context buffers when %d request are pending!!\n",
                hObj->numUnProcessedReq);
            retVal = FVID2_EAGAIN;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the channel object */
        chObj = hObj->chObjs[deiCtxBuf->channelNum];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));
        GT_assert(VpsMdrvDeiTrace, (chObj->chNum == deiCtxBuf->channelNum));
        GT_assert(VpsMdrvDeiTrace,
            (chObj->deiCtxInfo.numFld <= VPS_DEI_MAX_CTX_FLD_BUF));
        GT_assert(VpsMdrvDeiTrace,
            (chObj->deiCtxInfo.numMv <= VPS_DEI_MAX_CTX_MV_BUF));
        GT_assert(VpsMdrvDeiTrace,
            (chObj->deiCtxInfo.numMvstm <= VPS_DEI_MAX_CTX_MVSTM_BUF));

        /* Copy the context buffer from the channel object */
        VpsUtils_memcpy(deiCtxBuf, &chObj->deiCtxBuf, sizeof(Vps_DeiCtxBuf));

        /* Clear the context buffers */
        chObj->isDeiCtxBufSet = FALSE;
        for (bufCnt = 0u; bufCnt < chObj->deiCtxInfo.numFld; bufCnt++)
        {
            chObj->deiCtxBuf.fldBuf[bufCnt] = NULL;
        }
        for (bufCnt = 0u; bufCnt < chObj->deiCtxInfo.numMv; bufCnt++)
        {
            chObj->deiCtxBuf.mvBuf[bufCnt] = NULL;
        }
        for (bufCnt = 0u; bufCnt < chObj->deiCtxInfo.numMvstm; bufCnt++)
        {
            chObj->deiCtxBuf.mvstmBuf[bufCnt] = NULL;
        }

        /* Clear the core context buffers */
        deiCoreCtxBuf = &chObj->deiCoreCtxBuf;
        for (bufCnt = 0u; bufCnt < VCORE_DEI_MAX_PREV_FLD; bufCnt++)
        {
            for (ptrCnt = 0u; ptrCnt < VCORE_DEI_NUM_DESC_PER_FIELD; ptrCnt++)
            {
                deiCoreCtxBuf->prevInBuf[bufCnt][ptrCnt] = NULL;
            }
        }
        for (bufCnt = 0u; bufCnt < VCORE_DEI_MAX_OUT_FLD; bufCnt++)
        {
            for (ptrCnt = 0u; ptrCnt < VCORE_DEI_NUM_DESC_PER_FIELD; ptrCnt++)
            {
                deiCoreCtxBuf->curOutBuf[bufCnt][ptrCnt] = NULL;
            }
        }
        for (bufCnt = 0u; bufCnt < VCORE_DEI_MAX_MV_IN; bufCnt++)
        {
            deiCoreCtxBuf->mvInBuf[bufCnt] = NULL;
        }
        deiCoreCtxBuf->mvOutBuf = NULL;
        for (bufCnt = 0u; bufCnt < VCORE_DEI_MAX_MVSTM_IN + 1u; bufCnt++)
        {
            deiCoreCtxBuf->mvstmInBuf[bufCnt] = NULL;
        }
        deiCoreCtxBuf->mvstmOutBuf = NULL;
    }

    /* Restore global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



/**
 *  vpsMdrvDeiSetScCoeffIoctl
 */
static Int32 vpsMdrvDeiSetScCoeffIoctl(VpsMdrv_DeiHandleObj *hObj, Ptr cmdArgs)
{
    Int32                       retVal = FVID2_SOK, tempRetVal;
    UInt32                      cookie;
    UInt32                      coreCnt;
    UInt8                      *coeffDesc = NULL, *descPtr = NULL;
    const Vcore_Ops            *coreOps = NULL;
    VpsMdrv_DeiQueueObj        *qObj = NULL;
    VpsMdrv_DeiChObj           *chObj;
    VpsMdrv_DeiInstObj         *instObj;
    const Vps_ScCoeffParams    *scCoeffPrms;
    Vcore_ScCoeffParams         coreCoeffPrms;

    /* Check for NULL pointers */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    /* Disable global interrupts */
    cookie = Hwi_disable();

    if (FVID2_SOK == retVal)
    {
        scCoeffPrms = (Vps_ScCoeffParams *) cmdArgs;
        instObj = hObj->instObj;
        GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

        /* TODO: Assuming first channel information */
        chObj = hObj->chObjs[0u];
        GT_assert(VpsMdrvDeiTrace, (NULL != chObj));

        /* Get the core to which the scalar ID matches */
        coreCnt = instObj->scalarCoreId[scCoeffPrms->scalarId];
        GT_assert(VpsMdrvDeiTrace, (VPSMDRV_DEI_MAX_CORE != coreCnt));
        coreOps = instObj->coreOps[coreCnt];
        /* Check if the scalar ID is valid */
        if (NULL == coreOps)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Invalid Scalar ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate coeff descriptor and memory */
        coeffDesc = (UInt8 *) vpsMdrvDeiAllocCoeffDesc();
        /* Get a queue object from the free queue */
        qObj = VpsUtils_dequeue(hObj->freeQ);
        if ((NULL == coeffDesc) || (NULL == qObj))
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not allocate coeff memory/descriptor/queue object\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* NULL pointer check */
        GT_assert(VpsMdrvDeiTrace, (NULL != coreOps->programScCoeff));

        coreCoeffPrms.hScalingSet = scCoeffPrms->hScalingSet;
        coreCoeffPrms.vScalingSet = scCoeffPrms->vScalingSet;
        coreCoeffPrms.horzCoeffMemPtr = NULL; /* Returned by core */
        coreCoeffPrms.vertCoeffMemPtr = NULL; /* Returned by core */
        coreCoeffPrms.vertBilinearCoeffMemPtr = NULL; /*Returned by core*/
        coreCoeffPrms.userCoeffPtr = scCoeffPrms->coeffPtr;

        /* Get the pointers to coefficient memory */
        retVal = coreOps->programScCoeff(
                     hObj->coreHandle[coreCnt],
                     0u,
                     &coreCoeffPrms);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Could not program coeff memory\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* First create the horizontal coeff descriptor. */
        descPtr = coeffDesc;
        GT_assert(VpsMdrvDeiTrace, (NULL != coreCoeffPrms.horzCoeffMemPtr));
        VpsHal_vpdmaCreateConfigDesc(
            descPtr,
            chObj->coeffConfigDest[coreCnt],
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            chObj->horzCoeffOvlySize[coreCnt],
            coreCoeffPrms.horzCoeffMemPtr,
            0u,
            chObj->horzCoeffOvlySize[coreCnt]);

        /* Now create the vertical coeff descriptor. */
        descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
        GT_assert(VpsMdrvDeiTrace, (NULL != coreCoeffPrms.vertCoeffMemPtr));
        VpsHal_vpdmaCreateConfigDesc(
            descPtr,
            chObj->coeffConfigDest[coreCnt],
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            chObj->vertCoeffOvlySize[coreCnt],
            coreCoeffPrms.vertCoeffMemPtr,
            (Ptr) (chObj->horzCoeffOvlySize[coreCnt]),
            chObj->vertCoeffOvlySize[coreCnt]);

        /* Create the vertical bilinear coeff descriptor only if HQ scalar,
         * otherwise create dummy descriptor. */
        descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
        if (NULL != coreCoeffPrms.vertBilinearCoeffMemPtr)
        {
            VpsHal_vpdmaCreateConfigDesc(
                descPtr,
                chObj->coeffConfigDest[coreCnt],
                VPSHAL_VPDMA_CPT_BLOCK_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                chObj->vertBilinearCoeffOvlySize[0],
                coreCoeffPrms.vertBilinearCoeffMemPtr,
                (Ptr) (chObj->horzCoeffOvlySize[0] +
                       chObj->vertCoeffOvlySize[0]),
                chObj->vertBilinearCoeffOvlySize[0]);
        }
        else
        {
            VpsHal_vpdmaCreateDummyDesc(descPtr);
        }

        /*
         * Update the MLM parameters and submit the request to MLM
         */
        qObj->qObjType = VPSMDRV_QOBJ_TYPE_COEFF;
        qObj->mlmReqInfo.reqStartAddr = coeffDesc;
        qObj->mlmReqInfo.firstChannelSize =
            ((VPSMDRV_DEI_MAX_COEFF_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE) +
             VPSHAL_VPDMA_CTRL_DESC_SIZE);
        qObj->mlmReqInfo.lastRlDescAddr = (coeffDesc +
            (VPSMDRV_DEI_MAX_COEFF_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE));
        qObj->mlmReqInfo.submitReqType = MLM_SRT_NO_SOC;
        qObj->mlmReqInfo.cbArg = qObj;
        qObj->mlmReqInfo.numChannels = 0u;
        retVal = Mlm_submit(instObj->mlmHandle, &qObj->mlmReqInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Submission to MLM failed\n");
        }
        else
        {
            /* Increment the pending request count */
            hObj->numPendReq++;
            hObj->numUnProcessedReq++;

            /* Restore global interrupts */
            Hwi_restore(cookie);

            /* Wait for completion of coefficient programming */
            Semaphore_pend(hObj->ctrlSem, VPSMDRV_DEI_SEM_TIMEOUT);

            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Decrement the pending request count */
            GT_assert(VpsMdrvDeiTrace, (0u != hObj->numPendReq));
            GT_assert(VpsMdrvDeiTrace, (0u != hObj->numUnProcessedReq));
            hObj->numUnProcessedReq--;
            hObj->numPendReq--;
        }
    }

    /* Free the memories if already allocated */
    if (NULL != coeffDesc)
    {
        tempRetVal = vpsMdrvDeiFreeCoeffDesc(coeffDesc);
        GT_assert(VpsMdrvDeiTrace, (FVID2_SOK == tempRetVal));
    }
    if (NULL != qObj)
    {
        /* Return back the queue object to the free queue */
        VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);
    }

    /* Restore global interrupts */
    Hwi_restore(cookie);

    return (retVal);
}



/**
 *  vpsMdrvDeiCheckProcList
 */
static Int32 vpsMdrvDeiCheckProcList(VpsMdrv_DeiHandleObj *hObj,
                                     FVID2_ProcessList *procList)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  numInListReq;
    VpsMdrv_DeiInstObj     *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (NULL != procList));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    /* For mode 1 HQ driver, in frame list is one more to take N-1
     * field buffer */
    numInListReq = instObj->numInListReq;
    if (VPS_DEIHQ_CTXMODE_APP_N_1 == hObj->deiHqCtxMode)
    {
        numInListReq++;
    }

    /* Check for error in process list */
    retVal = FVID2_checkProcessList(
                 procList,
                 numInListReq,
                 instObj->numOutListReq,
                 hObj->numCh,
                 FALSE);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "Error in process list\n");
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiCheckParams
 *  Checks for valid create parameters.
 */
static UInt32 vpsMdrvDeiCheckParams(const Vps_M2mDeiCreateParams *createPrms,
                                    VpsMdrv_DeiInstObj *instObj,
                                    UInt32 instId)
{
    Int32                       retVal = FVID2_SOK;
    UInt32                      chCnt;
    const Vps_M2mDeiChParams   *inChPrms;

    /* NULL pointer check */
    GT_assert(VpsMdrvDeiTrace, (NULL != createPrms));
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    if (VPS_M2M_CONFIG_PER_CHANNEL != createPrms->mode)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Per handle mode of operation is not supported. "
            "Try with per channel mode!!\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if ((createPrms->numCh > VPS_M2M_DEI_MAX_CH_PER_HANDLE) ||
        (createPrms->numCh <= 0u))
    {
        GT_2trace(VpsMdrvDeiTrace, GT_ERR,
            "Invalid number of channels(%d) - Supported max channels %d\n",
            createPrms->numCh,
            VPS_M2M_DEI_MAX_CH_PER_HANDLE);
        retVal = FVID2_EINVALID_PARAMS;
    }

    if ((instObj->numChAllocated + createPrms->numCh) >
            VPS_M2M_DEI_MAX_CH_PER_INST)
    {
        GT_1trace(VpsMdrvDeiTrace, GT_ERR,
            "Exceeds number of channels per instance (Max %d per instance)\n",
            VPS_M2M_DEI_MAX_CH_PER_INST);
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (NULL == createPrms->chParams)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer: Ch Params\n");
        retVal = FVID2_EBADARGS;
    }

#ifdef VPS_HAL_INCLUDE_DEIH
    /* Mode 1 is supported only in DEI HQ drivers */
    if ((createPrms->deiHqCtxMode == VPS_DEIHQ_CTXMODE_APP_N_1)
        && !((VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == instId)
            || (VPS_M2M_INST_MAIN_DEIH_SC3_VIP0 == instId)
            || (VPS_M2M_INST_MAIN_DEIH_SC1_SC3_WB0_VIP0 == instId)))
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "Mode 1 supported only in HQ DEI drivers\n");
        retVal = FVID2_EINVALID_PARAMS;
    }
#endif  /* #ifdef VPS_HAL_INCLUDE_DEIH */

#ifdef TI_816X_BUILD
    if (((VPS_M2M_INST_MAIN_DEIH_SC1_WB0 == instId) ||
        (VPS_M2M_INST_AUX_DEI_SC2_WB1 == instId)) &&
        (TRUE == createPrms->isVipScReq))
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "VIP scalar cannot be enabled for this instances\n");
        retVal = FVID2_EINVALID_PARAMS;
    }
#endif  /* #ifdef TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
    if (((VPS_M2M_INST_MAIN_DEI_SC1_WB0 == instId) ||
        (VPS_M2M_INST_AUX_SC2_WB1 == instId)) &&
        (TRUE == createPrms->isVipScReq))
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR,
            "VIP scalar cannot be enabled for this instances\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (((VPS_M2M_INST_AUX_SC2_WB1 == instId) ||
         (VPS_M2M_INST_AUX_SC4_VIP1 == instId) ||
         (VPS_M2M_INST_AUX_SC2_SC4_WB1_VIP1 == instId)) &&
         (FVID2_SOK == retVal))
    {
        if (VPS_M2M_CONFIG_PER_CHANNEL == createPrms->mode)
        {
            inChPrms = createPrms->chParams;
            for (chCnt = 0u; chCnt < createPrms->numCh; chCnt++)
            {
                if ((inChPrms->deiCfg != NULL) ||
                    (inChPrms->deiHqCfg != NULL))
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "DEI config for this instances should be NULL\n");
                    retVal = FVID2_EINVALID_PARAMS;
                    break;
                }
                inChPrms++;
            }
        }
        else if (VPS_M2M_CONFIG_PER_HANDLE == createPrms->mode)
        {
            inChPrms = createPrms->chParams;
            if ((inChPrms->deiCfg != NULL) ||
                (inChPrms->deiHqCfg != NULL))
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "DEI config for this instances should be NULL\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }
#endif  /* #ifdef TI_814X_BUILD || TI_8107_BUILD */

    if (FVID2_SOK == retVal)
    {
        /* Compression should be disabled as this is not supported anymore!! */
        if (VPS_M2M_CONFIG_PER_CHANNEL == createPrms->mode)
        {
            inChPrms = createPrms->chParams;
            for (chCnt = 0u; chCnt < createPrms->numCh; chCnt++)
            {
                if (TRUE == inChPrms->comprEnable)
                {
                    GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                        "Compression enable is not supported!!\n");
                    retVal = FVID2_EINVALID_PARAMS;
                    break;
                }
                inChPrms++;
            }
        }
        else if (VPS_M2M_CONFIG_PER_HANDLE == createPrms->mode)
        {
            inChPrms = createPrms->chParams;
            if (TRUE == inChPrms->comprEnable)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                    "Compression enable is not supported!!\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    return (retVal);
}



/**
 * vpsMdrvDeiSetScLazyLoadingIoctl
 */
static Int32 vpsMdrvDeiSetScLazyLoadingIoctl(VpsMdrv_DeiHandleObj *hObj,
                                             Ptr cmdArgs)
{
    Int32                       retVal = FVID2_SOK;
    UInt32                      enableLazyLoadingModified = FALSE;
    UInt32                      enableFilterSelectModified = FALSE;
    Vps_ScLazyLoadingParams    *params;

    /* Check for NULL pointers */
    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvDeiTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        params = (Vps_ScLazyLoadingParams *) cmdArgs;

        /* Check if Lazy Loading configuration is changed from
         * current. */
        if (hObj->instObj->enableLazyLoading[params->scalarId] !=
            params->enableLazyLoading)
        {
            enableLazyLoadingModified = TRUE;
            hObj->instObj->enableLazyLoading[params->scalarId] =
                params->enableLazyLoading;
        }

        if (hObj->instObj->enableFilterSelect[params->scalarId] !=
            params->enableFilterSelect)
        {
            enableFilterSelectModified = TRUE;
            hObj->instObj->enableFilterSelect[params->scalarId] =
                params->enableFilterSelect;
        }

        /* enableFilterSelect currently needs to be TRUE if
         * enableLazyLoading is TRUE for each scalar. */
        if ((TRUE == params->enableLazyLoading) &&
            (FALSE == params->enableFilterSelect))
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "enableFilterSelect must be set to TRUE if "
                "enableLazyLoading is TRUE for the scalar!\n");
            retVal = FVID2_EBADARGS;
        }

        GT_assert(VpsMdrvDeiTrace,
                (((TRUE == params->enableLazyLoading) &&
                    (TRUE == params->enableFilterSelect)) ||
                (FALSE == params->enableLazyLoading)));
    }

    /* If there are any changes in lazy loading configuration,
     * make the required changes. */
    if ((FVID2_SOK == retVal) &&
        ((TRUE == enableLazyLoadingModified) ||
            (TRUE == enableFilterSelectModified)))
    {
        retVal = vpsMdrvDeiModifyLazyLoadingCfg(
                     hObj,
                     params->scalarId,
                     enableLazyLoadingModified,
                     enableFilterSelectModified);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                "Failed to modify lazy loading config!\n");
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvDeiModifyLazyLoadingCfg
 *  \brief Make required changes for modification of Lazy Loading configuration
 *         if there are any changes in the lazy loading configuration.
 */
static Int32 vpsMdrvDeiModifyLazyLoadingCfg(VpsMdrv_DeiHandleObj *hObj,
                                            UInt32 scalarId,
                                            UInt32 enableLazyLoadingModified,
                                            UInt32 enableFilterSelectModified)
{
    Int32 retVal = FVID2_SOK;
    UInt32 cookie;
    UInt32 chCnt, descSetCnt, i;
    UInt32 validScalarCoeffNum;
    VpsMdrv_DeiChObj *chObj;
    VpsMdrv_DeiInstObj *instObj;

    GT_assert(VpsMdrvDeiTrace, (NULL != hObj));
    GT_assert(VpsMdrvDeiTrace, (VPS_M2M_DEI_SCALAR_ID_MAX > scalarId));
    instObj = hObj->instObj;
    GT_assert(VpsMdrvDeiTrace, (NULL != instObj));

    cookie = Hwi_disable();

    /* Currently assume that nothing is to be done for
     * enableFilterSelectModified
     */
    if (TRUE == enableLazyLoadingModified)
    {
        /* If Lazy Loading is enabled, calculate and set the scaling factor
         * config for each channel.
         */
        if (TRUE == hObj->instObj->enableLazyLoading[scalarId])
        {
            /* Go down into the core to get the scaling factor config
             * for each channel.
             * Also do this for all descriptor sets, hence provide it as -1.
             */
            retVal = VpsMdrvDeiGetScFactorConfig(hObj,
                        scalarId,
                        (UInt32) -1,
                        (UInt32) -1);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvDeiTrace, GT_ERR,
                          "EFAIL: Getting the Scaling Factor Config Failed\n");
            }
        }

        for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
        {
            chObj = hObj->chObjs[chCnt];

            /* Recalculate the number of valid scalar coefficient descriptors
             * for each channel based on the new configuration.
             */
            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                validScalarCoeffNum = 0u;
                for (i = 0u; i < VPS_M2M_DEI_SCALAR_ID_MAX; i++)
                {
                    if (    (TRUE == instObj->enableLazyLoading[i])
                        &&  (VPS_SC_HST_MAX != chObj->scFactor[i].hsType)
                        &&  (VPS_SC_VST_MAX != chObj->scFactor[i].vsType))
                    {
                        if (VPS_M2M_DEI_SCALAR_ID_DEI_SC == i)
                        {
                            /* TBD: slcChObj should be one for each scalar Id.
                             * Currently assuming DEI only.
                             * Sub-frame scaling is not supported for dual-
                             * output for VIP scalar.
                             */
                            chObj->slcChObj.filtType =
                                chObj->scFactor[VPS_M2M_DEI_SCALAR_ID_DEI_SC].
                                    vsType;
                        }
                        validScalarCoeffNum +=
                            VPSMDRV_DEI_RT_COEFF_CFG_DESC_PER_SC;
                    }
                }
            }

            for (descSetCnt = 0u;
                 descSetCnt < VPSMDRV_DEI_MAX_DESC_SET;
                 descSetCnt++)
            {
                /* If the configuration is per channel first descriptor in the
                 * layout will be set to config descriptor to avoid impact when
                 * lazy loading is disabled. When lazy loading is enabled, it is
                 * set to the appropriate coeff descriptor. This value is used
                 * for configuring the reload descriptor at runtime.
                 */
                if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
                {
                    /* If there are valid coeff descriptors because lazy loading
                     * is enabled for at least one scalar in the path, then set
                     * the first descriptor in the layout to the first valid
                     * scalar coeff descriptor.
                     * If lazy loading is not enabled for any scalars in the
                     * path, then set the first descriptor to the non-shadow
                     * config descriptor.
                     */
                    if (0u != validScalarCoeffNum)
                    {
                        chObj->firstDesc[descSetCnt] =
                            chObj->coeffDesc[descSetCnt]
                                [chObj->numCoeffDesc - validScalarCoeffNum];
                    }
                    else
                    {
                        chObj->firstDesc[descSetCnt] =
                            chObj->nshwCfgDesc[descSetCnt];
                    }
                }
            }
        }
    }

    Hwi_restore(cookie);

    return retVal;
}
