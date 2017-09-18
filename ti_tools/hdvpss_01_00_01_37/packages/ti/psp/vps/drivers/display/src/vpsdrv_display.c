/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_display.c
 *
 *  \brief VPS display driver for the two bypass paths and the SD display path.
 *  This file implements the VPS display driver.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/vps_display.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
#include <ti/psp/vps/core/vpscore_secPath.h>
#include <ti/psp/vps/core/vpscore_deiPath.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/drivers/display/src/vpsdrv_displayCfg.h>
#include <ti/psp/vps/drivers/display/vpsdrv_display.h>
#include <ti/psp/vps/drivers/display/src/vpsdrv_displayPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/** \brief Invalid descriptor set index. */
#define VPSDDRV_INVALID_DESC_SET        (VPSDDRV_NUM_DESC_SET + 1u)

/** \brief Time out to be used in sem pend. */
#define VPSDDRV_SEM_TIMEOUT             (BIOS_WAIT_FOREVER)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Fdrv_Handle VpsDdrv_create(UInt32 drvId,
                                  UInt32 instId,
                                  Ptr createArgs,
                                  Ptr createStatusArgs,
                                  const FVID2_DrvCbParams *fdmCbPrms);
static Int32 VpsDdrv_delete(Fdrv_Handle handle, Ptr deleteArgs);
static Int32 VpsDdrv_control(Fdrv_Handle handle,
                             UInt32 cmd,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs);
static Int32 VpsDdrv_queue(Fdrv_Handle handle,
                           FVID2_FrameList *frameList,
                           UInt32 streamId);
static Int32 VpsDdrv_dequeue(Fdrv_Handle handle,
                             FVID2_FrameList *frameList,
                             UInt32 streamId,
                             UInt32 timeout);

static Int32 vpsDdrvUpdateBufIsr(UInt32 curSet, UInt32 timeStamp, Ptr arg);
static Int32 vpsDdrvStartCb(Ptr arg);
static Int32 vpsDdrvStopCb(Ptr arg);
static Int32 vpsDdrvStartingCb(Ptr arg, VpsHal_VpdmaFSEvent event);

static Int32 vpsDdrvStartIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvStopIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs);

static Int32 vpsDdrvSetFmtIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvGetFmtIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs);

static Int32 vpsDdrvSetDeiDispParamIoctl(VpsDdrv_InstObj *instObj,
                                         Ptr cmdArgs);
static Int32 vpsDdrvSetScCoeff(VpsDdrv_InstObj *instObj,
                               Ptr cmdArgs);

static Int32 vpsDdrvCreateLayoutIoctl(VpsDdrv_InstObj *instObj,
                                      Ptr cmdArgs,
                                      Ptr cmdStatusArgs);
static Int32 vpsDdrvDeleteLayoutIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvSelectLayoutIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvDeleteAllLayoutIoctl(VpsDdrv_InstObj *instObj);
static Int32 vpsDdrvGetStatusIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvSetDispMode(VpsDdrv_InstObj *instObj, Ptr cmdArgs);
static UInt32 vpsDrvCanCurBufBeDequeued(const VpsDdrv_BufManObj *bmObj,
                                        UInt32 curSet);
static VpsDdrv_QueObj *vpsDrvGetNextBufState(VpsDdrv_InstObj *instObj,
                                             UInt32 curSet,
                                             UInt32 nextSet,
                                             UInt32 curFid);
static VpsDdrv_QueObj *vpsDdrvRemoveBufFromState(VpsDdrv_BufManObj *bmObj,
                                                 UInt32 reverseOrder);
static Void vpsDdrvRetErrFrameList(VpsDdrv_InstObj *instObj,
                                   VpsDdrv_QueObj *qObj);

static Int32 vpsDdrvUpdateRtParams(VpsDdrv_InstObj *instObj,
                                   const Vps_DispRtParams *rtPrms,
                                   UInt32 descSet);
static Int32 vpsDeiDdrvUpdateRtParams(VpsDdrv_InstObj *instObj,
                                      const Vps_DeiDispRtParams *rtPrms,
                                      UInt32 descSet);
static Int32 vpsDdrvUpdateCoreRtParams(VpsDdrv_InstObj *instObj,
                                       const Vps_DispRtParams *rtPrms,
                                       UInt32 descSet);
static Int32 vpsDeiDdrvUpdateCoreRtParams(VpsDdrv_InstObj *instObj,
                                          const Vps_DeiDispRtParams *rtPrms,
                                          UInt32 descSet);
static Int32 vpsDdrvCheckRtParams(VpsDdrv_InstObj *instObj,
                                  const Vps_DispRtParams *rtPrms);
static Int32 vpsDeiDdrvCheckRtParams(VpsDdrv_InstObj *instObj,
                                     const Vps_DeiDispRtParams *rtPrms);
static Int32 VpsDdrvFbUpdateDesc(VpsDdrv_InstObj *instObj,
                                 FVID2_FrameList *frmList);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** Display driver function pointer. */
static const FVID2_DrvOps VpsDdrvOps =
{
    FVID2_VPS_DISP_DRV,         /* Driver ID */
    VpsDdrv_create,             /* Create */
    VpsDdrv_delete,             /* Delete */
    VpsDdrv_control,            /* Control */
    VpsDdrv_queue,              /* Queue */
    VpsDdrv_dequeue,            /* Dequeue */
    NULL,                       /* ProcessFrames */
    NULL                        /* GetProcessedFrames */
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsDdrv_init
 *  \brief Display driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of display driver API.
 *  This should be called only once for all the instance together.
 *  This function could be called from task context only.
 *
 *  \param numInst          [IN] Number of instance objects to be initialized.
 *  \param initPrms         [IN] Pointer to the instance parameter containing
 *                          instance specific information. If the number of
 *                          instance is greater than 1, then this pointer
 *                          should point to an array of init parameter
 *                          structure of size numInst.
 *  \param arg              [IN] Not used currently. Meant for future purpose.
 *                          Set this to NULL.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
Int32 VpsDdrv_init(UInt32 numInst,
                   const VpsDdrv_InitPrms *initPrms,
                   Ptr arg)
{
    Int32       retVal = FVID2_SOK;

    /* Check for errors */
    GT_assert(VpsDdrvDispTrace, (NULL != initPrms));

    /* Initialize the internal objects */
    retVal = vpsDdrvPrivInit(numInst, initPrms);
    if (FVID2_SOK == retVal)
    {
        /* Register the driver to the driver manager */
        retVal = FVID2_registerDriver(&VpsDdrvOps);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Registering to FVID2 driver manager failed\n");
            /* Uninitialize the internal objects if error occurs */
            vpsDdrvPrivDeInit();
        }
    }

    return (retVal);
}



/**
 *  VpsDdrv_deInit
 *  \brief Display driver exit function.
 *  Deallocates memory allocated by init function. This should be called only
 *  once for all the instance together. This function could be called from
 *  task context only.
 *
 *  \param arg              [IN] Not used currently. Meant for future purpose.
 *                          Set this to NULL.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
Int32 VpsDdrv_deInit(Ptr arg)
{
    Int32       retVal = FVID2_SOK;

    /* Unregister from driver manager */
    retVal = FVID2_unRegisterDriver(&VpsDdrvOps);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "Unregistering from FVID2 driver manager failed\n");
    }

    /* Uninitialize the internal objects */
    retVal |= vpsDdrvPrivDeInit();

    return (retVal);
}



/**
 *  VpsDdrv_create
 *  \brief Display driver create function. Returns the handle to the created
 *  driver instance. This function could be called from task context only.
 *  And is reentrant - could be called from multiple tasks at the same time for
 *  creating different instances.
 *
 *  \param drvId            [IN] Driver ID - Should be FVID2_VPS_DISP_DRV.
 *  \param instId           [IN] Instance to open - VPS_DISP_INST_BP0,
 *                          VPS_DISP_INST_BP1 or VPS_DISP_INST_SEC1.
 *  \param createArgs       [IN] Display driver create parameter - pointer
 *                          to Vps_DispCreateParams. This parameter should be
 *                          non-NULL.
 *  \param createStatusArgs [OUT] Display driver return status parameter -
 *                          pointer to Vps_DispCreateStatus. This parameter
 *                          could be NULL and the driver fills the status
 *                          information only if this is not NULL.
 *  \param fdmCbParams      [IN] Call back function parameters. This parameter
 *                          should be non-NULL. But the callback function
 *                          pointer inside this structure could be NULL if the
 *                          application decides not to get display completion
 *                          callback. In this case application has to poll
 *                          the driver for knowing the status about display
 *                          completion.
 *
 *  \return                 Returns instance handle for the respective instance
 *                          ID else returns NULL on error. Appropriate error
 *                          code is returned via status argument.
 */
static Fdrv_Handle VpsDdrv_create(UInt32 drvId,
                                  UInt32 instId,
                                  Ptr createArgs,
                                  Ptr createStatusArgs,
                                  const FVID2_DrvCbParams *fdmCbPrms)
{
    Int32                   retVal = FVID2_EALLOC, tempRetVal;
    UInt32                  descSet, memIdx;
    UInt32                  resrcAllocFlag = FALSE;
    Fdrv_Handle             handle = NULL;
    VpsDdrv_InstObj        *instObj = NULL;
    VpsDdrv_BufManObj      *bmObj;
    Vps_DispCreateParams   *createPrms;
    Vps_DispCreateStatus   *createStatus;

    /* Check for NULL pointers */
    if ((NULL == createArgs) || (NULL == fdmCbPrms))
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Get the instance object for this instance */
        instObj = vpsDdrvGetInstObj(instId);
        if (NULL == instObj)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Invalid instance ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
        else
        {
            bmObj = &instObj->bmObj;
            retVal = FVID2_SOK;
        }
    }

    if (NULL != instObj)
    {
        /* Take the instance semaphore */
        Semaphore_pend(instObj->instSem, VPSDDRV_SEM_TIMEOUT);

        /* Check if the instance is already opened */
        if (TRUE == instObj->state.isOpened)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "DEVICE_INUSE: Device already opened\n");
            /* Release semaphore and don't modify instance object */
            Semaphore_post(instObj->instSem);
            instObj = NULL;
            retVal = FVID2_EDEVICE_INUSE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        GT_assert(VpsDdrvDispTrace, (NULL != instObj));
        GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->open));

        /* Init the handles to NULL and other objects first */
        instObj->coreHandle = NULL;
        instObj->dcHandle = NULL;
        bmObj->reqQ = NULL;
        bmObj->doneQ = NULL;
        bmObj->fbQObj = NULL;
        instObj->curMemIdx = VPSDDRV_NON_MOSAIC_MEM_IDX;
        for (memIdx = 0u; memIdx < VPSDDRV_MAX_MEMORY_OBJECTS; memIdx++)
        {
            instObj->memObj[memIdx].layoutId = VPSDDRV_INVALID_LAYOUT_ID;
            instObj->memObj[memIdx].coreLayoutId = VCORE_DEFAULT_LAYOUT_ID;
            instObj->memObj[memIdx].descMem = NULL;
            instObj->memObj[memIdx].descMemSize = 0u;
        }

        /* Init DCTRL client info before registering */
        instObj->dcClientInfo.cbFxn = vpsDdrvUpdateBufIsr;
        instObj->dcClientInfo.arg = (Ptr) instObj;
        instObj->dcClientInfo.started = vpsDdrvStartCb;
        instObj->dcClientInfo.stopped = vpsDdrvStopCb;
        instObj->dcClientInfo.starting = vpsDdrvStartingCb;
        /* Reset to Non FB Mode at crete time */
        instObj->dcClientInfo.isFbMode = FALSE;
        instObj->state.isFbMode = FALSE;
        instObj->isFrmUpdated = FALSE;

        /* Open all the handles and create queues */
        retVal = Vrm_allocResource(instObj->resrcId);
        if (FVID2_SOK == retVal)
        {
            resrcAllocFlag = TRUE;
        }
        instObj->coreHandle = instObj->coreOps->open(
                                  instObj->coreInstId,
                                  VCORE_OPMODE_DISPLAY,
                                  1u,   /* Display is always single channel */
                                  FALSE);
        instObj->dcHandle = Dc_registerClient(
                                instObj->dcNodeNum,
                                &instObj->dcClientInfo,
                                NULL);
        bmObj->reqQ = VpsUtils_createQ();
        bmObj->doneQ = VpsUtils_createQ();
        if ((FVID2_SOK != retVal) ||
            (NULL == instObj->coreHandle) ||
            (NULL == instObj->dcHandle) ||
            (NULL == bmObj->reqQ) ||
            (NULL == bmObj->doneQ))
        {
            if (FALSE == resrcAllocFlag)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "ALLOC: Resource already used by some other driver\n");
            }
            else
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "ALLOC: Memory allocation failed\n");
            }
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Copy the create parameters */
        createPrms = (Vps_DispCreateParams *) createArgs;
        instObj->memType = createPrms->memType;
        instObj->periodicCallbackEnable = createPrms->periodicCallbackEnable;

        /* Get the VENC information from display controller */
        retVal = Dc_getPathInfo(instObj->dcHandle, &instObj->dcPathInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "DCtrl get mode info failed\n");
        }
        else
        {
            /* Initialize VCOMP runtime configuration */
            instObj->vcompRtCfg.scanFormat =
                instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
            instObj->vcompRtCfg.inputWidth =
                instObj->dcPathInfo.modeInfo.mInfo.width;
            instObj->vcompRtCfg.inputHeight =
                instObj->dcPathInfo.modeInfo.mInfo.height;
            instObj->vcompRtCfg.cropStartX = 0u;
            instObj->vcompRtCfg.cropStartY = 0u;
            instObj->vcompRtCfg.cropWidth =
                instObj->dcPathInfo.modeInfo.mInfo.width;
            instObj->vcompRtCfg.cropHeight =
                instObj->dcPathInfo.modeInfo.mInfo.height;
            instObj->vcompRtCfg.outCropStartX = 0u;
            instObj->vcompRtCfg.outCropStartY = 0u;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Configure the core according to DCtrl settings */
        retVal = vpsDdrvConfigCore(instObj, NULL);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Config core failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Initialize instance variables */
        instObj->state.isOpened = TRUE;
        instObj->state.isStarted = FALSE;
        instObj->state.isMultiWinMode = FALSE;
        instObj->queueCount = 0u;
        instObj->dequeueCount = 0u;
        instObj->displayedFrameCount = 0u;
        instObj->repeatFrameCount = 0u;
        bmObj->expectedSet = 0u;
        bmObj->fid = 0u;
        bmObj->semiPrgmedBuf = NULL;
        for (descSet = 0u; descSet < VPSDDRV_NUM_DESC_SET; descSet++)
        {
            bmObj->prgmedBuf[descSet] = NULL;
        }

        instObj->fdmCbPrms.fdmCbFxn = fdmCbPrms->fdmCbFxn;
        instObj->fdmCbPrms.fdmErrCbFxn = fdmCbPrms->fdmErrCbFxn;
        instObj->fdmCbPrms.errList = fdmCbPrms->errList;
        instObj->fdmCbPrms.fdmData = fdmCbPrms->fdmData;

        handle = (Fdrv_Handle) instObj;
    }
    else
    {
        /* Close all the handles if already opened */
        if (NULL != instObj)
        {
            /* Free resource if already allocated */
            if (TRUE == resrcAllocFlag)
            {
                tempRetVal = Vrm_releaseResource(instObj->resrcId);
                GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
                resrcAllocFlag = FALSE;
            }
            if (NULL != instObj->coreHandle)
            {
                GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->close));
                tempRetVal = instObj->coreOps->close(instObj->coreHandle);
                GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
                instObj->coreHandle = NULL;
            }
            if (NULL != instObj->dcHandle)
            {
                tempRetVal = Dc_unRegisterClient(instObj->dcHandle);
                GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
                instObj->dcHandle = NULL;
            }
            if (NULL != bmObj->reqQ)
            {
                tempRetVal = VpsUtils_deleteQ(bmObj->reqQ);
                GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
                bmObj->reqQ = NULL;
            }
            if (NULL != bmObj->doneQ)
            {
                tempRetVal = VpsUtils_deleteQ(bmObj->doneQ);
                GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
                bmObj->doneQ = NULL;
            }
        }
    }

    if (NULL != instObj)
    {
        /* Post the instance semaphore */
        Semaphore_post(instObj->instSem);
    }

    /* Return the status if possible */
    if (NULL != createStatusArgs)
    {
        createStatus = (Vps_DispCreateStatus *) createStatusArgs;
        createStatus->retVal = retVal;
        if (FVID2_SOK == retVal)
        {
            vpsDdrvFillCreateStatus(instObj, createStatus);
        }
    }

    return (handle);
}



/**
 *  VpsDdrv_delete
 *  \brief Display driver delete function. Relinquish the resources allocated
 *  by create function. This function could be called from task context only.
 *  And is reentrant - could be called from multiple tasks at the same time for
 *  deleting different instances.
 *
 *  \param handle           [IN] Display driver instance handle returned by
 *                          create function.
 *  \param deleteArgs       [IN] Not used currently. Meant for future purpose.
 *                          Set this to NULL.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
static Int32 VpsDdrv_delete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Int32               retVal = FVID2_SOK, tempRetVal;
    UInt32              descSet;
    VpsDdrv_QueObj     *qObj;
    VpsDdrv_InstObj    *instObj = NULL;
    VpsDdrv_BufManObj  *bmObj;

    /* Check for NULL pointers */
    if (NULL == handle)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        instObj = handle;
        bmObj = &instObj->bmObj;

        /* Take the instance semaphore */
        Semaphore_pend(instObj->instSem, VPSDDRV_SEM_TIMEOUT);

        /* Check if already opened. */
        if (TRUE != instObj->state.isOpened)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "ERROR: Driver not opened\n");
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* If already started, stop it. */
        if (TRUE == instObj->state.isStarted)
        {
            retVal = vpsDdrvStopIoctl(instObj, NULL);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR, "Stop display failed\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Free up the queue objects if application did not call dequeue after
         * stopping the display */
        /* Flush the done queue */
        while (NULL != (qObj = VpsUtils_dequeue(bmObj->doneQ)))
        {
            /* Give back the queue object back to the free pool */
            VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
        }
        /* Flush buffers in current state */
        while (NULL != (qObj = vpsDdrvRemoveBufFromState(bmObj, FALSE)))
        {
            /* Give back the queue object back to the free pool */
            VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
        }
        /* Flush the request queue */
        while (NULL != (qObj = VpsUtils_dequeue(bmObj->reqQ)))
        {
            /* Give back the queue object back to the free pool */
            VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
        }

        if (TRUE == instObj->state.isFbMode)
        {
            bmObj->fbQObj = NULL;
        }

        /* Delete all the layouts */
        tempRetVal = vpsDdrvDeleteAllLayout(instObj);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));

        /*
         * Close all the handles
         */
        tempRetVal = Vrm_releaseResource(instObj->resrcId);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));

        GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->close));
        tempRetVal = instObj->coreOps->close(instObj->coreHandle);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
        instObj->coreHandle = NULL;

        tempRetVal = Dc_unRegisterClient(instObj->dcHandle);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
        instObj->dcHandle = NULL;

        tempRetVal = VpsUtils_deleteQ(bmObj->reqQ);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
        bmObj->reqQ = NULL;

        tempRetVal = VpsUtils_deleteQ(bmObj->doneQ);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
        bmObj->doneQ = NULL;

        /* Reset other variables */
        instObj->state.isOpened = FALSE;
        instObj->state.isStarted = FALSE;
        instObj->state.isMultiWinMode = FALSE;
        instObj->state.isFbMode = FALSE;
        instObj->queueCount = 0u;
        instObj->dequeueCount = 0u;
        instObj->displayedFrameCount = 0u;
        instObj->repeatFrameCount = 0u;
        bmObj->expectedSet = 0u;
        bmObj->fid = 0u;
        bmObj->isProgressive = FALSE;

        /* Assert if buffers are not NULL i.e. if not freed */
        GT_assert(VpsDdrvDispTrace, (NULL == bmObj->semiPrgmedBuf));
        bmObj->semiPrgmedBuf = NULL;
        for (descSet = 0u; descSet < VPSDDRV_NUM_DESC_SET; descSet++)
        {
            GT_assert(VpsDdrvDispTrace, (NULL == bmObj->prgmedBuf[descSet]));
            bmObj->prgmedBuf[descSet] = NULL;
        }
        instObj->fdmCbPrms.fdmCbFxn = NULL;
        instObj->fdmCbPrms.fdmErrCbFxn = NULL;
        instObj->fdmCbPrms.errList = NULL;
        instObj->fdmCbPrms.fdmData = NULL;
    }

    if (NULL != instObj)
    {
        /* Post the instance semaphore */
        Semaphore_post(instObj->instSem);
    }

    return (retVal);
}



/**
 *  VpsDdrv_control
 *  \brief Display driver control function. This function implements various
 *  IOCTLS.
 *  All IOCTLS are blocking and could not be called from ISR context unless
 *  otherwise specified for a particular IOCTL.
 *
 *  cmd             : FVID2_START
 *  cmdArgs         : NULL
 *  cmdStatusArgs   : NULL
 *  Description     : Starts the display. At least minimum number of buffers
 *                    should be queued before calling start IOCTL.
 *
 *  cmd             : FVID2_STOP
 *  cmdArgs         : NULL
 *  cmdStatusArgs   : NULL
 *  Description     : Stops the display

 *  cmd             : IOCTL_VPS_DEI_DISP_SET_PARAMS
 *  cmdArgs         : const Vps_DeiDispParams *
 *  cmdStatusArgs   : NULL
 *  Description     : Sets all the parametes related to Dei display driver.
 *
 *  cmd             : FVID2_SET_FORMAT
 *  cmdArgs         : const FVID2_Format *
 *  cmdStatusArgs   : NULL
 *  Description     : Sets the buffer format to be displayed.
 *
 *  cmd             : FVID2_GET_FORMAT
 *  cmdArgs         : FVID2_Format *
 *  cmdStatusArgs   : NULL
 *  Description     : Gets the default buffer format or already set format.
 *
 *  cmd             : IOCTL_VPS_CREATE_LAYOUT
 *  cmdArgs         : const Vps_MultiWinParams *
 *  cmdStatusArgs   : Vps_LayoutId *
 *  Description     : Creates the mosaic layout, allocates the required
 *                    descriptor memories, program it and return the layout ID.
 *
 *  cmd             : IOCTL_VPS_DELETE_LAYOUT
 *  cmdArgs         : const Vps_LayoutId *
 *  cmdStatusArgs   : NULL
 *  Description     : Deletes the mosaic layout, frees the allocated descriptor
 *                    memories based on the passed layout ID.
 *
 *  cmd             : IOCTL_VPS_SELECT_LAYOUT
 *  cmdArgs         : const Vps_LayoutId *
 *  cmdStatusArgs   : NULL
 *  Description     : Selects the already created layout for display operation.
 *
 *  cmd             : IOCTL_VPS_DELETE_ALL_LAYOUT
 *  cmdArgs         : NULL
 *  cmdStatusArgs   : NULL
 *  Description     : Deletes all the created the mosaic layouts, frees the
 *                    allocated descriptor memories.
 *
 *  cmd             : IOCTL_VPS_DISP_GET_STATUS
 *  cmdArgs         : Vps_DispStatus *
 *  cmdStatusArgs   : NULL
 *  Description     : Gets the current display status information.
 *
 *  \param handle           [IN] Display driver instance handle returned by
 *                          create function.
 *  \param cmd              [IN] Supports the above mentioned commands.
 *  \param cmdArgs          [IN] Depending on the command this will vary.
 *  \param cmdStatusArgs    [OUT] Depending on the command this will vary.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
static Int32 VpsDdrv_control(Fdrv_Handle handle,
                             UInt32 cmd,
                             Ptr cmdArgs,
                             Ptr cmdStatusArgs)
{
    Int32               retVal = FVID2_SOK;
    VpsDdrv_InstObj    *instObj;

    /* Check for NULL pointers */
    if (NULL == handle)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        instObj = handle;
        switch (cmd)
        {
            case FVID2_START:
                retVal = vpsDdrvStartIoctl(instObj, cmdArgs);
                break;

            case FVID2_STOP:
                retVal = vpsDdrvStopIoctl(instObj, cmdArgs);
                break;

            case FVID2_SET_FORMAT:
                retVal = vpsDdrvSetFmtIoctl(instObj, cmdArgs);
                break;

            case FVID2_GET_FORMAT:
                retVal = vpsDdrvGetFmtIoctl(instObj, cmdArgs);
                break;

            case IOCTL_VPS_CREATE_LAYOUT:
                retVal = vpsDdrvCreateLayoutIoctl(
                             instObj,
                             cmdArgs,
                             cmdStatusArgs);
                break;

            case IOCTL_VPS_DELETE_LAYOUT:
                retVal = vpsDdrvDeleteLayoutIoctl(instObj, cmdArgs);
                break;

            case IOCTL_VPS_DELETE_ALL_LAYOUT:
                retVal = vpsDdrvDeleteAllLayoutIoctl(instObj);
                break;

            case IOCTL_VPS_SELECT_LAYOUT:
                retVal = vpsDdrvSelectLayoutIoctl(instObj, cmdArgs);
                break;

            case IOCTL_VPS_DISP_GET_STATUS:
                retVal = vpsDdrvGetStatusIoctl(instObj, cmdArgs);
                break;

            case IOCTL_VPS_DEI_DISP_SET_PARAMS:
                retVal = vpsDdrvSetDeiDispParamIoctl(instObj, cmdArgs);
                break;

            case IOCTL_VPS_SET_COEFFS:
                retVal = vpsDdrvSetScCoeff(instObj, cmdArgs);
                break;

            case IOCTL_VPS_DISP_SET_DISP_MODE:
                retVal = vpsDdrvSetDispMode(instObj, cmdArgs);
                break;

            case IOCTL_VPS_DISP_GET_DISP_MODE:
            {
                Vps_DispMode *dispMode = (Vps_DispMode *)cmdArgs;

                if (NULL != dispMode)
                {
                    dispMode->isFbMode = instObj->state.isFbMode;
                }
                else
                {
                    retVal = FVID2_EBADARGS;
                }
                break;
            }

            default:
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "UNSUPPORTED_CMD: IOCTL not supported\n");
                retVal = FVID2_EUNSUPPORTED_CMD;
                break;
        }
    }

    return (retVal);
}



/**
 *  VpsDdrv_queue
 *  \brief Display driver queue buffer API. This submits the buffers
 *  to the driver for display. When mosaic mode is set, then the frame list
 *  should contain frames for all the mosaic windows. The display driver
 *  does not maintain individual queue for each mosaic window.
 *  This function could be called from ISR or task context.
 *  And is reentrant - could be called from multiple tasks/ISR at the same time
 *  to queue buffers for the same or different instances.
 *
 *  \param handle           [IN] Display driver instance handle returned by
 *                          create function.
 *  \param frameList        [IN] Pointer to frame list containing valid
 *                          frame pointers.
 *  \param streamId         [IN] Not used currently. Meant for future purpose.
 *                          Set this to 0.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
static Int32 VpsDdrv_queue(Fdrv_Handle handle,
                           FVID2_FrameList *frameList,
                           UInt32 streamId)
{
    Int32                        retVal = FVID2_SOK;
    UInt32                       cookie;
    VpsDdrv_QueObj              *qObj = NULL;
    VpsDdrv_InstObj             *instObj;
    VpsDdrv_BufManObj           *bmObj;
    const Vps_DispRtParams      *rtPrms;
    const Vps_DeiDispRtParams   *deiRtPrms;

    /* Check for NULL pointers */
    if ((NULL == handle) || (NULL == frameList))
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        instObj = handle;
        bmObj = &instObj->bmObj;

        /* Check framelist for error and NULL pointer check */
        retVal = FVID2_checkFrameList(frameList, VPSDDRV_MAX_FRAMES);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Check frame list error\n");
        }
    }

    if ((FVID2_SOK == retVal) && (FALSE == instObj->state.isFbMode))
    {
        /* Check for valid RT params */
        if (NULL != frameList->perListCfg)
        {
            if ((VPS_DISP_INST_BP0 == instObj->drvInstId) ||
                (VPS_DISP_INST_BP1 == instObj->drvInstId) ||
                (VPS_DISP_INST_SEC1 == instObj->drvInstId))
            {
                rtPrms = (Vps_DispRtParams *) frameList->perListCfg;
                retVal = vpsDdrvCheckRtParams(instObj, rtPrms);
            }
            else /* DEI Display Driver */
            {
                deiRtPrms = (Vps_DeiDispRtParams *) frameList->perListCfg;
                retVal = vpsDeiDdrvCheckRtParams(instObj, deiRtPrms);
            }

            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR, "Invalid runtime params\n");
            }
        }
    }

    if (FALSE == instObj->state.isFbMode)
    {
        if (FVID2_SOK == retVal)
        {
            cookie = Hwi_disable();             /* Disable global interrupts */
            /* Allocate a free queue object from the pool */
            qObj = VpsUtils_dequeue(instObj->freeQ);
            Hwi_restore(cookie);                /* Restore global interrupts */
            if (NULL == qObj)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "ALLOC: Q object allocation failed\n");
                retVal = FVID2_EALLOC;
            }
        }

        if (FVID2_SOK == retVal)
        {
            /* Copy the frame list to the driver's queue object */
            FVID2_copyFrameList(&qObj->frameList, frameList);

            cookie = Hwi_disable();             /* Disable global interrupts */

            /* Add the queue object in driver's request queue */
            VpsUtils_queue(bmObj->reqQ, &qObj->qElem, qObj);
            instObj->queueCount++;

            Hwi_restore(cookie);                /* Restore global interrupts */
        }
    }
    else
    {
        if ((FVID2_SOK == retVal) && (NULL == bmObj->fbQObj))
        {
            cookie = Hwi_disable();             /* Disable global interrupts */
            /* Allocate a free queue object from the pool */
            bmObj->fbQObj = VpsUtils_dequeue(instObj->freeQ);
            Hwi_restore(cookie);                /* Restore global interrupts */
            if (NULL == bmObj->fbQObj)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "ALLOC: Q object allocation failed\n");
                retVal = FVID2_EALLOC;
            }
        }

        if (FVID2_SOK == retVal)
        {
            FVID2_copyFrameList(&bmObj->fbQObj->frameList,
                                frameList);

            if (TRUE == instObj->state.isStarted)
            {
                retVal = VpsDdrvFbUpdateDesc(
                            instObj,
                            &bmObj->fbQObj->frameList);

                if (FVID2_SOK == retVal)
                {
                    GT_0trace(VpsDdrvDispTrace, GT_DEBUG,
                                "Update Desc Failed\n");
                }

                instObj->isFrmUpdated = TRUE;
            }
        }
    }

    return (retVal);
}



/**
 *  VpsDdrv_dequeue
 *  \brief Display driver dequeue buffer API. This returns the displayed buffers
 *  from the driver. When mosaic mode is set, then the frame list
 *  should contain space for storing that many number of frames pointers for
 *  each of the mosaic windows.
 *  The behaviour of this function depends on whether the display is in
 *  progress or not. When display is in progress, this will return the
 *  the displayed buffers from the driver's done queue. If done queue is
 *  empty, then it returns FVID2_EAGAIN, indicating the application to try
 *  again after sometime (used in polling mode).
 *  When display is stopped, this returns the displayed buffers, display in
 *  progress buffers, buffers in input request queue in that order. This
 *  is used by the application to flush out any driver owned buffers before
 *  deleting the display driver. When no more buffers are with the driver,
 *  then it returns FVID2_ENO_MORE_BUFFERS.
 *
 *  This function could be called from ISR or task context.
 *  And is reentrant - could be called from multiple tasks/ISR at the same time
 *  to queue buffers for the same or different instances.
 *
 *  \param handle           [IN] Display driver instance handle returned by
 *                          create function.
 *  \param frameList        [IN] Pointer to frame list containing valid
 *                          frame pointers.
 *  \param streamId         [IN] Not used currently. Meant for future purpose.
 *                          Set this to 0.
 *  \param timeout          [IN] Not used currently. Meant for future purpose.
 *                          Set this to FVID2_TIMEOUT_NONE.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
static Int32 VpsDdrv_dequeue(Fdrv_Handle handle,
                             FVID2_FrameList *frameList,
                             UInt32 streamId,
                             UInt32 timeout)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cookie;
    VpsDdrv_QueObj     *qObj = NULL;
    VpsDdrv_InstObj    *instObj;
    VpsDdrv_BufManObj  *bmObj;

    /* Check for NULL pointers */
    if ((NULL == handle) || (NULL == frameList))
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        instObj = handle;
        if (TRUE == instObj->state.isFbMode)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: Not Supported for FbMode\n");
            retVal = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        instObj = handle;
        bmObj = &instObj->bmObj;

        /* Check framelist for error and NULL pointer check */
        retVal = FVID2_checkDqFrameList(frameList, VPSDDRV_MAX_FRAMES);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Check frame list error\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        cookie = Hwi_disable();             /* Disable global interrupts */

        /* Get a completed queue object from driver's done queue */
        qObj = VpsUtils_dequeue(bmObj->doneQ);
        if (NULL == qObj)
        {
            /* When display is stopped, give back all the driver owned buffer
             * including the current programmed and the buffers in request
             * queue */
            if (TRUE != instObj->state.isStarted)
            {
                /* Give the buffers in current state */
                qObj = vpsDdrvRemoveBufFromState(bmObj, FALSE);
                if (NULL == qObj)
                {
                    /* At last give back the buffers in request queue as well */
                    qObj = VpsUtils_dequeue(bmObj->reqQ);
                }

                /* Check if all these operation has failed */
                if (NULL == qObj)
                {
                    GT_0trace(VpsDdrvDispTrace, GT_DEBUG,
                        "NO_MORE_BUFFERS: No more buffers with driver\n");
                    retVal = FVID2_ENO_MORE_BUFFERS;
                }
            }
            else
            {
                GT_0trace(VpsDdrvDispTrace, GT_DEBUG,
                    "AGAIN: Out queue Empty. Try again\n");
                retVal = FVID2_EAGAIN;
            }
        }

        Hwi_restore(cookie);                /* Restore global interrupts */

        /* Copy the driver's frame list to application's frame list */
        if (NULL != qObj)
        {
            FVID2_copyFrameList(frameList, &qObj->frameList);

            cookie = Hwi_disable();         /* Disable global interrupts */

            /* Give back the queue object back to the free pool */
            VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
            instObj->dequeueCount++;

            Hwi_restore(cookie);            /* Restore global interrupts */
        }
        else
        {
            /* No frames are dequeued, set num frames dequeued as zero */
            frameList->numFrames = 0u;
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvUpdateBufIsr
 *  DLM/DCTRL callback ISR which programs the descriptor set depending on the
 *  buffer availability.
 */
static Int32 vpsDdrvUpdateBufIsr(UInt32 curSet, UInt32 timeStamp, Ptr arg)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  curFid, nextFid;
    UInt32                  frmCnt, chCnt;
    UInt32                  nextSet, prevSet, dqSet = VPSDDRV_INVALID_DESC_SET;
    UInt32                  frmConsumed;
    UInt8                  *tempPtr;
    VpsDdrv_QueObj         *qObj = NULL;
    VpsDdrv_InstObj        *instObj;
    VpsDdrv_BufManObj      *bmObj;
    VpsDdrv_MemObj         *memObj;
    FVID2_FrameList        *frameList;
    const Vps_DispRtParams *rtPrms;
    const Vps_DeiDispRtParams *deiRtPrms;

    /* Check for NULL pointers and other errors */
    GT_assert(VpsDdrvDispTrace, (NULL != arg));
    GT_assert(VpsDdrvDispTrace, (curSet < VPSDDRV_NUM_DESC_SET));

    instObj = (VpsDdrv_InstObj *) arg;
    bmObj = &instObj->bmObj;

    GT_1trace(VpsDdrvDispTrace, GT_DEBUG,
        "Getting update buffer ISR. Set: %d\n", curSet);

    /* Figure out the next set */
    if (curSet >= (VPSDDRV_NUM_DESC_SET - 1u))
    {
        /* Wrap around!! */
        nextSet = 0u;
    }
    else
    {
        nextSet = curSet + 1u;
    }

    /* Figure out the previous set */
    if (0u == curSet)
    {
        prevSet = (VPSDDRV_NUM_DESC_SET - 1u);
    }
    else
    {
        prevSet = curSet - 1u;
    }

    /* Figure out the current FID */
    if (TRUE == bmObj->isProgressive)
    {
        curFid = 0u;
        nextFid = 0u;
    }
    else
    {
        curFid = (curSet & 1u);
        nextFid = !curFid;
    }

    /* Check if we are getting field ID's at proper sequence */
    if (bmObj->fid != curFid)
    {
        GT_2trace(VpsDdrvDispTrace, GT_ERR,
            "Expected field %d but getting field %d\n",
            bmObj->fid,
            curFid);
    }

    /* Check if we are getting interrupts in proper sequence */
    if (bmObj->expectedSet != curSet)
    {
        GT_2trace(VpsDdrvDispTrace, GT_ERR,
            "Expected set %d but getting set %d\n",
            bmObj->expectedSet,
            curSet);
    }

    /* Reprogram the abort descriptor as dummy descriptor once the channels
     * are aborted after the first SI */
    if (0u == instObj->displayedFrameCount)
    {
        memObj = &instObj->memObj[instObj->curMemIdx];
        tempPtr = memObj->abortDesc[0u];
        for (chCnt = 0u; chCnt < memObj->numAbortCh; chCnt++)
        {
            VpsHal_vpdmaCreateDummyDesc(tempPtr);
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
    }

    if (1u == instObj->displayedFrameCount)
    {
        memObj = &instObj->memObj[instObj->curMemIdx];
        if (memObj->dcDescInfo.descAddr[0u][DC_NSHADOW_IDX])
        {
            tempPtr = memObj->dcDescInfo.descAddr[0u][DC_NSHADOW_IDX];
            VpsHal_vpdmaCreateDummyDesc(tempPtr);
            tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            VpsHal_vpdmaCreateDummyDesc(tempPtr);
            tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            VpsHal_vpdmaCreateDummyDesc(tempPtr);
        }
     }

    /* Check if we can dequeue a buffer */
    if (TRUE == bmObj->isProgressive)
    {
        dqSet = prevSet;
        instObj->displayedFrameCount++;
    }
    else
    {
        /* For interlaced mode, buffers are dequeued at even fields */
        if (0u == curFid)
        {
            /* For interlaced we virtually deal with half the set. Since
             * two sets are required to program a buffer, two DC set acts
             * like a buffer management set!! */
            dqSet = prevSet / FVID2_MAX_FIELDS;
            instObj->displayedFrameCount++;
        }
    }

    if (VPSDDRV_INVALID_DESC_SET != dqSet)
    {
        if ((TRUE == instObj->state.isFbMode) &&
            (instObj->displayedFrameCount > 2u))
        {
            /* Intimate the application that a buffer is ready to be dequeued */
            if (NULL != instObj->fdmCbPrms.fdmCbFxn)
            {
                instObj->fdmCbPrms.fdmCbFxn(instObj->fdmCbPrms.fdmData, NULL);
            }
        }
        /* Check if we can dequeue the previous set buffer */
        else if (TRUE == vpsDrvCanCurBufBeDequeued(bmObj, dqSet))
        {
            GT_assert(VpsDdrvDispTrace, (NULL != bmObj->prgmedBuf[dqSet]));

            /* Set the timestamp */
            frameList = &bmObj->prgmedBuf[dqSet]->frameList;
            for (frmCnt = 0u; frmCnt < frameList->numFrames; frmCnt++)
            {
                frameList->frames[frmCnt]->timeStamp = timeStamp;
            }

            /* Add the completed frames in driver's done queue */
            VpsUtils_queue(bmObj->doneQ,
                &bmObj->prgmedBuf[dqSet]->qElem,
                bmObj->prgmedBuf[dqSet]);
            bmObj->prgmedBuf[dqSet] = NULL;

            /* Intimate the application that a buffer is ready to be dequeued */
            if (NULL != instObj->fdmCbPrms.fdmCbFxn)
            {
                instObj->fdmCbPrms.fdmCbFxn(instObj->fdmCbPrms.fdmData, NULL);
            }
        }
        /* Check if periodic callback option is enabled */
        else if (TRUE == instObj->periodicCallbackEnable)
        {
            /* Call application callback even if no buffers are to be
             * dequeued */
            if (NULL != instObj->fdmCbPrms.fdmCbFxn)
            {
                instObj->fdmCbPrms.fdmCbFxn(instObj->fdmCbPrms.fdmData, NULL);
            }
        }
    }

    /* For Frame Buffer mode, descriptors is updated in the
       Task Context, i.e. when FVID2_queue is called. */
    if (FALSE == instObj->state.isFbMode)
    {
        /* Figure out what to program next */
        qObj = vpsDrvGetNextBufState(instObj, curSet, nextSet, curFid);
    }
    /* FVID2_start returns only after 2 interrupt,
       so repeat cound should be updated onlyif
       display Frame cound is greater than 2 */
    else if (instObj->displayedFrameCount > 2u)
    {
        if (FALSE == instObj->isFrmUpdated)
        {
            instObj->repeatFrameCount++;
        }

        instObj->isFrmUpdated = FALSE;
    }

    /* For FrameBuffer Mode, qObj is NULL, so no RT and descriptor update */
    if (NULL != qObj)
    {
        /* Update runtime parameters */
        if ((VPS_DISP_INST_BP0 == instObj->drvInstId) ||
            (VPS_DISP_INST_BP1 == instObj->drvInstId) ||
            (VPS_DISP_INST_SEC1 == instObj->drvInstId))
        {
            rtPrms = (const Vps_DispRtParams *) qObj->frameList.perListCfg;
            retVal = vpsDdrvUpdateRtParams(instObj, rtPrms, nextSet);
        }
        else
        {
            deiRtPrms = (const Vps_DeiDispRtParams *) qObj->frameList.perListCfg;
            retVal = vpsDeiDdrvUpdateRtParams(instObj, deiRtPrms, nextSet);
        }

        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Update RT params failed\n");
        }
        else
        {
            /* We have to program something. Call core function to update
             * buffers depending on whether in mosaic mode or not */
            memObj = &instObj->memObj[instObj->curMemIdx];
            if (TRUE == instObj->state.isMultiWinMode)
            {
                GT_assert(VpsDdrvDispTrace,
                    (NULL != instObj->coreOps->updateMultiDesc));
                retVal = instObj->coreOps->updateMultiDesc(
                             instObj->coreHandle,
                             memObj->coreLayoutId,
                             &memObj->coreDescMem[nextSet],
                             &qObj->frameList,
                             nextFid,
                             0u,
                             &frmConsumed);
            }
            else
            {
                GT_assert(VpsDdrvDispTrace,
                    (NULL != instObj->coreOps->updateDesc));
                retVal = instObj->coreOps->updateDesc(
                             instObj->coreHandle,
                             &memObj->coreDescMem[nextSet],
                             qObj->frameList.frames[0u],
                             nextFid);
            }
        }
    }

    if (FVID2_SOK != retVal)
    {
        if (NULL != qObj)
        {
            /* Give back the framelist to application through error callback */
            /* TODO: We have to remove this qObj from driver state!! */
            vpsDdrvRetErrFrameList(instObj, qObj);
            qObj = NULL;
        }
    }

    /* Figure out the next set */
    if (curSet >= (VPSDDRV_NUM_DESC_SET - 1u))
    {
        /* Wrap around!! */
        bmObj->expectedSet = 0u;
    }
    else
    {
        bmObj->expectedSet = curSet + 1u;
    }

    /* Figure out the next FID only in interlaced mode */
    if (TRUE != bmObj->isProgressive)
    {
        bmObj->fid ^= 1u;
    }

    return (retVal);
}



/**
 *  vpsDdrvStartCb
 *  DCTRL callback function to indicate that the display operation has
 *  started. This is not used currently.
 */
static Int32 vpsDdrvStartCb(Ptr arg)
{
    /* Todo: This should be implemented for dynamic switching of mux */
    GT_0trace(VpsDdrvDispTrace, GT_DEBUG, "vpsDdrvStartCb\n");
    return (FVID2_SOK);
}



/**
 *  vpsDdrvStopCb
 *  DCTRL callback function to indicate that the display operation has
 *  stopped. This is not used currently.
 */
static Int32 vpsDdrvStopCb(Ptr arg)
{
    /* Todo: This should be implemented for dynamic switching of mux */
    GT_0trace(VpsDdrvDispTrace, GT_DEBUG, "vpsDdrvStopCb\n");
    return (FVID2_SOK);
}



/**
 *  vpsDdrvStartingCb
 *  DCTRL callback function to indicate that the display operation is
 *  going to start. Through this callback the DCTRL will inform the driver
 *  to which list the driver is added in DLM and accordingly provide the frame
 *  start event to which the VPDMA clients should be programmed with.
 *  The display driver in turn uses the core layer to program the frame start
 *  event.
 */
static Int32 vpsDdrvStartingCb(Ptr arg, VpsHal_VpdmaFSEvent fsEvent)
{
    Int32               retVal = FVID2_SOK;
    VpsDdrv_InstObj    *instObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != arg));
    instObj = (VpsDdrv_InstObj *) arg;
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->setFsEvent));

    /* Set the frame start events for core clients */
    retVal = instObj->coreOps->setFsEvent(
                 instObj->coreHandle,
                 0u,
                 fsEvent,
                 NULL);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Set frame start event failed\n");
    }

    return (retVal);
}



/**
 *  vpsDdrvStartIoctl
 *  \brief Implements the start display IOCTL.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvStartIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32                   retVal = FVID2_SOK, tempRetVal;
    UInt8                  *tempPtr;
    UInt32                  descSet, chCnt;
    UInt32                  numElemInReqQ;
    UInt32                  frmConsumed;
    UInt32                  dcConfigFlag = FALSE;
    VpsDdrv_QueObj         *qObj = NULL;
    VpsDdrv_MemObj         *memObj;
    VpsDdrv_BufManObj      *bmObj;
    const Vps_DispRtParams *rtPrms;
    const Vps_DeiDispRtParams *deiRtPrms;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    bmObj = &instObj->bmObj;

    /* Initialize instance variables that needs to be reset in stop/start
     * sequence */
    instObj->displayedFrameCount = 0u;
    instObj->repeatFrameCount = 0u;
    bmObj->expectedSet = 0u;
    bmObj->fid = 0u;
    /* Mark the dirty flags as clean */
    for (descSet = 0u; descSet < VPSDDRV_NUM_DESC_SET; descSet++)
    {
        instObj->isDescDirty[descSet] = FALSE;
    }

    if (FALSE == instObj->state.isFbMode)
    {
        /* Check if the number of elements in the queue is sufficient to
         * program the initial set of descriptors */
        numElemInReqQ = VpsUtils_getNumQElem(bmObj->reqQ);
        /* Check if the primed buffers are sufficient */
        if (numElemInReqQ < VPSDDRV_MIN_PRIME_BUFFERS)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "ERROR: Insufficient buffers queued\n");
            retVal = FVID2_EFAIL;
        }
    }
    else
    {
        if (NULL == bmObj->fbQObj)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "ERROR: Insufficient buffers queued\n");
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Update runtime parameters present in the initial frames */
        descSet = 0u;

        if (FALSE == instObj->state.isFbMode)
        {
            qObj = VpsUtils_dequeue(bmObj->reqQ);
        }
        else
        {
            qObj = bmObj->fbQObj;
        }

        GT_assert(VpsDdrvDispTrace, (NULL != qObj));
        if (FVID2_SF_PROGRESSIVE == instObj->dcPathInfo.modeInfo.mInfo.scanFormat)
        {
            bmObj->prgmedBuf[0u] = qObj;
        }
        else
        {
            /* For interlaced display, both the fields are queued to the
             * driver. Program the top field to first set. Since the next
             * field is programmed in ISR, put it in semi programmed state. */
            bmObj->semiPrgmedBuf = qObj;
        }

        /* Update runtime parameters */
        if (FALSE == instObj->state.isFbMode)
        {
            if ((VPS_DISP_INST_BP0 == instObj->drvInstId) ||
                (VPS_DISP_INST_BP1 == instObj->drvInstId) ||
                (VPS_DISP_INST_SEC1 == instObj->drvInstId))
            {
                rtPrms = (const Vps_DispRtParams *) qObj->frameList.perListCfg;
                retVal = vpsDdrvUpdateRtParams(instObj, rtPrms, descSet);
            }
            else
            {
                deiRtPrms = (const Vps_DeiDispRtParams *) qObj->frameList.perListCfg;
                retVal = vpsDeiDdrvUpdateRtParams(instObj, deiRtPrms, descSet);
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Configure display controller */
        retVal = vpsDdrvConfigDctrl(instObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Config core memory failed\n");
        }
        else
        {
            dcConfigFlag = TRUE;
            if (FALSE == instObj->state.isFbMode)
            {
                /* Update runtime parameters that needs to be done after slicing
                 * descriptors */
                if ((VPS_DISP_INST_BP0 == instObj->drvInstId) ||
                    (VPS_DISP_INST_BP1 == instObj->drvInstId) ||
                    (VPS_DISP_INST_SEC1 == instObj->drvInstId))
                {
                    retVal = vpsDdrvUpdateCoreRtParams(instObj, rtPrms, descSet);
                    if (FVID2_SOK != retVal)
                    {
                        GT_0trace(VpsDdrvDispTrace, GT_ERR,
                            "Update Core RT params failed\n");
                    }
                }
                else
                {
                    retVal = vpsDeiDdrvUpdateCoreRtParams(instObj, deiRtPrms, descSet);
                    if (FVID2_SOK != retVal)
                    {
                        GT_0trace(VpsDdrvDispTrace, GT_ERR,
                            "Update Core RT params failed\n");
                    }
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        memObj = &instObj->memObj[instObj->curMemIdx];

        /* Program the abort descriptors for the first set. Second set
         * should not be done so let it be dummy descriptor as programmed
         * earlier */
        tempPtr = memObj->abortDesc[0u];
        for (chCnt = 0u; chCnt < memObj->numAbortCh; chCnt++)
        {
            VpsHal_vpdmaCreateAbortCtrlDesc(tempPtr, memObj->abortCh[chCnt]);
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }

        if (FALSE == instObj->state.isFbMode)
        {
            /* Update the descriptor with buffer address */
            if (TRUE == instObj->state.isMultiWinMode)
            {
                GT_assert(VpsDdrvDispTrace,
                    (NULL != instObj->coreOps->updateMultiDesc));
                retVal = instObj->coreOps->updateMultiDesc(
                             instObj->coreHandle,
                             memObj->coreLayoutId,
                             &memObj->coreDescMem[descSet],
                             &qObj->frameList,
                             (descSet & 1u),    /* FID */
                             0u,
                             &frmConsumed);
            }
            else
            {
                GT_assert(VpsDdrvDispTrace,
                    (NULL != instObj->coreOps->updateDesc));
                retVal = instObj->coreOps->updateDesc(
                             instObj->coreHandle,
                             &memObj->coreDescMem[descSet],
                             qObj->frameList.frames[0u],
                             (descSet & 1u));   /* FID */
            }
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR, "Update buffer failed\n");
            }
        }
        else
        {
            retVal = VpsDdrvFbUpdateDesc(instObj, &qObj->frameList);
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Start display */
        instObj->state.isStarted = TRUE;
        retVal = Dc_startClient(instObj->dcHandle);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Start client failed\n");
        }
    }

    if (FVID2_SOK != retVal)
    {
        if (NULL != qObj)
        {
            /* Give back the framelist to application through error callback */
            vpsDdrvRetErrFrameList(instObj, qObj);
            qObj = NULL;
        }

        instObj->state.isStarted = FALSE;
        if (TRUE == dcConfigFlag)
        {
            /* Clear Dctrl memory if error occurs */
            tempRetVal = vpsDdrvDeConfigDctrl(instObj);
            GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
            dcConfigFlag = FALSE;
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvStopIoctl
 *  \brief Implements the stop display IOCTL. When the display is stopped, this
 *  function queues back the on going frame back to the input request queue
 *  so that when the display starts again, it starts displaying from the current
 *  un-displayed frame.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvStopIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32               retVal = FVID2_SOK;
    VpsDdrv_QueObj     *qObj;
    VpsDdrv_BufManObj  *bmObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    bmObj = &instObj->bmObj;

    /* Stop display */
    retVal = Dc_stopClient(instObj->dcHandle);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Stop client failed\n");
    }
    else
    {
        instObj->state.isStarted = FALSE;
    }

    if (FVID2_SOK == retVal)
    {
        /* Clear Dctrl memory */
        retVal = vpsDdrvDeConfigDctrl(instObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Deconfig core memory failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Take the buffers in current state and push them in to the request
         * queue so that the next start will use those buffers */
        while (NULL != (qObj = vpsDdrvRemoveBufFromState(bmObj, TRUE)))
        {
            /* Push it to the request queue */
            VpsUtils_queueBack(bmObj->reqQ, &qObj->qElem, qObj);
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvSetFmtIoctl
 *  \brief Sets the FVID2 format by calling the core function.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvSetFmtIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32               retVal = FVID2_SOK;
    const FVID2_Format *fmt;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        if (instObj->state.isStarted == TRUE)
        {

            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "IOCTL cannot be called when display is running \n");
            retVal = FVID2_EFAIL;

        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the VENC information from display controller */
        retVal = Dc_getPathInfo(instObj->dcHandle, &instObj->dcPathInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "DCtrl get mode info failed\n");
        }
        else
        {
            /* Initialize VCOMP runtime configuration */
            instObj->vcompRtCfg.scanFormat =
                instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
            instObj->vcompRtCfg.inputWidth =
                instObj->dcPathInfo.modeInfo.mInfo.width;
            instObj->vcompRtCfg.inputHeight =
                instObj->dcPathInfo.modeInfo.mInfo.height;
            instObj->vcompRtCfg.cropStartX = 0u;
            instObj->vcompRtCfg.cropStartY = 0u;
            instObj->vcompRtCfg.cropWidth =
                instObj->dcPathInfo.modeInfo.mInfo.width;
            instObj->vcompRtCfg.cropHeight =
                instObj->dcPathInfo.modeInfo.mInfo.height;
            instObj->vcompRtCfg.outCropStartX = 0u;
            instObj->vcompRtCfg.outCropStartY = 0u;
        }
    }

    if (FVID2_SOK == retVal)
    {
        fmt = (FVID2_Format *) cmdArgs;

        /* Do a basic error check */
        retVal = vpsDdrvCheckFormat(instObj, fmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Check format failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Configure the core according to user passed settings */
        retVal = vpsDdrvConfigCore(instObj, fmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Config core failed\n");
        }
        else
        {
            /* Reset the mosaic mode */
            instObj->curMemIdx = VPSDDRV_NON_MOSAIC_MEM_IDX;
            instObj->state.isMultiWinMode = FALSE;
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvGetFmtIoctl
 *  \brief Gets the FVID2 format by calling the core function.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvGetFmtIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32               retVal = FVID2_SOK;
    FVID2_Format       *fmt;
    Vcore_Format        coreFmt;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getFormat));

    if (FVID2_SOK == retVal)
    {
        fmt = (FVID2_Format *) cmdArgs;

        /* Get the format from core */
        retVal = instObj->coreOps->getFormat(instObj->coreHandle, 0u, &coreFmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Core get format failed\n");
        }
        else
        {
            /* Copy the format to application memory */
            VpsUtils_memcpy(fmt, &coreFmt.fmt, sizeof(FVID2_Format));
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvCreateLayoutIoctl
 *  \brief Creates the mosaic layout, allocates the required descriptor
 *  memories, program the and return the layout ID.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvCreateLayoutIoctl(VpsDdrv_InstObj *instObj,
                                      Ptr cmdArgs,
                                      Ptr cmdStatusArgs)
{
    Int32                       retVal = FVID2_SOK, tempRetVal;
    const Vps_MultiWinParams   *multiWinPrms;
    Vps_LayoutId               *layoutId;
    VpsDdrv_MemObj             *memObj = NULL;
    UInt32                      coreLayoutCreated = FALSE;
    UInt32                      descAllocated = FALSE;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if ((NULL == cmdArgs) || (NULL == cmdStatusArgs))
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        if (NULL == instObj->coreOps->createLayout)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Create layout not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_OPS;
        }
    }

    /* Take the instance semaphore */
    Semaphore_pend(instObj->instSem, VPSDDRV_SEM_TIMEOUT);

    if (FVID2_SOK == retVal)
    {
        multiWinPrms = (Vps_MultiWinParams *) cmdArgs;
        layoutId = (Vps_LayoutId *) cmdStatusArgs;

        /* Allocate a free memory object */
        memObj = vpdDdrvAllocMemObj(instObj);
        if (NULL == memObj)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "ALLOC: Allocation of memory object failed\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Create the layout by calling core function */
        retVal = instObj->coreOps->createLayout(
                     instObj->coreHandle,
                     0u,
                     multiWinPrms,
                     &memObj->coreLayoutId);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Core create layout failed\n");
        }
        else
        {
            coreLayoutCreated = TRUE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate, slice and program descriptor memory */
        retVal = vpsDrvAllocSliceCoreMem(instObj, memObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Alloc and slice core memory failed\n");
        }
        else
        {
            descAllocated = TRUE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Assign an unique layout ID */
        retVal = vpsDdrvAssignLayoutId(instObj, memObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Assigne layout ID failed\n");
        }
        else
        {
            layoutId->layoutId = memObj->layoutId;
        }
    }

    /* Undo if error occurs */
    if (FVID2_SOK != retVal)
    {
        if (TRUE == descAllocated)
        {
            /* Free descriptor memory */
            tempRetVal = vpsDrvFreeCoreMem(instObj, memObj);
            GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
            descAllocated = FALSE;
        }

        if (TRUE == coreLayoutCreated)
        {
            /* Delete core layout */
            GT_assert(VpsDdrvDispTrace,
                (NULL != instObj->coreOps->deleteLayout));
            tempRetVal = instObj->coreOps->deleteLayout(
                             instObj->coreHandle,
                             0u,
                             memObj->coreLayoutId);
            GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
            coreLayoutCreated = FALSE;
        }

        if (NULL != memObj)
        {
            /* Free memory object */
            tempRetVal = vpdDdrvFreeMemObj(instObj, memObj);
            GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
            memObj = NULL;
        }
    }

    /* Post the instance semaphore */
    Semaphore_post(instObj->instSem);

    return (retVal);
}



/**
 *  vpsDdrvDeleteLayoutIoctl
 *  \brief Deletes the mosaic layout, frees the allocated descriptor
 *  memories based on the passed layout ID. If the layout ID is used currently
 *  when the display is in progress then this function returns error.
 *  This IOCTL also deletes all the non active layouts if delete all falg is
 *  set.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvDeleteLayoutIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32               retVal = FVID2_SOK;
    const Vps_LayoutId *layoutId;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        if (NULL == instObj->coreOps->deleteLayout)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Delete layout not supported for this instance\n");
            retVal = FVID2_EUNSUPPORTED_OPS;
        }
    }

    /* Take the instance semaphore */
    Semaphore_pend(instObj->instSem, VPSDDRV_SEM_TIMEOUT);

    if (FVID2_SOK == retVal)
    {
        layoutId = (Vps_LayoutId *) cmdArgs;
        /* Delete the specific layout */
        retVal = vpsDdrvDeleteLayout(instObj, layoutId->layoutId);
    }

    /* Post the instance semaphore */
    Semaphore_post(instObj->instSem);

    return (retVal);
}



/**
 *  vpsDdrvDeleteAllLayoutIoctl
 *  \brief Deletes all the mosaic layouts, frees the allocated descriptor
 *  memories. If any one of the layout is used by the display then this
 *  function returns error.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvDeleteAllLayoutIoctl(VpsDdrv_InstObj *instObj)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    if (NULL == instObj->coreOps->deleteLayout)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "Delete layout not supported for this instance\n");
        retVal = FVID2_EUNSUPPORTED_OPS;
    }

    /* Take the instance semaphore */
    Semaphore_pend(instObj->instSem, VPSDDRV_SEM_TIMEOUT);

    if (FVID2_SOK == retVal)
    {
        /* Delete all the layouts */
        retVal = vpsDdrvDeleteAllLayout(instObj);
    }

    /* Post the instance semaphore */
    Semaphore_post(instObj->instSem);

    return (retVal);
}



/**
 *  vpsDdrvSelectLayoutIoctl
 *  \brief Selects the already created layout for display operation.
 *  Since creating a layout does not select the layout for display operation,
 *  application has to explicitly select a layout using this IOCTL before
 *  starting display.
 *  The application could select a layout using this IOCTL only when display
 *  has not started. Once the display starts, the application could
 *  change or select the layout through runtime parameter while queuing a
 *  request.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvSelectLayoutIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32               retVal = FVID2_SOK;
    VpsDdrv_MemObj     *memObj = NULL;
    const Vps_LayoutId *layoutId;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    /* Take the instance semaphore */
    Semaphore_pend(instObj->instSem, VPSDDRV_SEM_TIMEOUT);

    if (FVID2_SOK == retVal)
    {
        if (TRUE == instObj->state.isStarted)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Can't select a layout using IOCTL when display in progress\n");
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Select the layout using runtime parameter\n");
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        layoutId = (Vps_LayoutId *) cmdArgs;

        /* Get the memory object for the layout ID */
        memObj = vpsDdrvGetMemObjForLayoutId(instObj, layoutId->layoutId);
        if (NULL == memObj)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Invalid layout ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Set the current memory object index and enable mosaic mode */
        instObj->curMemIdx = vpsDdrvGetIdxForMemObj(instObj, memObj);
        instObj->state.isMultiWinMode = TRUE;
    }

    /* Post the instance semaphore */
    Semaphore_post(instObj->instSem);

    return (retVal);
}



/**
 *  vpsDdrvGetStatusIoctl
 *  \brief Gets the current display status information. This could be called
 *  from ISR context.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvGetStatusIoctl(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32           retVal = FVID2_SOK;
    Vps_DispStatus *dispStatus;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Copy the status */
        dispStatus = (Vps_DispStatus *) cmdArgs;
        dispStatus->queueCount = instObj->queueCount;
        dispStatus->dequeueCount = instObj->dequeueCount;
        dispStatus->displayedFrameCount = instObj->displayedFrameCount;
        dispStatus->repeatFrameCount = instObj->repeatFrameCount;
    }

    return (retVal);
}



/**
 *  vpsDdrvSetDeiDispParamIoctl
 *  \brief Sets the DEI display driver related parameters.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvSetDeiDispParamIoctl(VpsDdrv_InstObj *instObj,
                                         Ptr cmdArgs)
{

    Int32                    retVal = FVID2_SOK;
    const Vps_DeiDispParams *deiDispPrms;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {

#ifdef TI_816X_BUILD
        if ((instObj->drvInstId != VPS_DISP_INST_MAIN_DEIH_SC1) &&
            (instObj->drvInstId != VPS_DISP_INST_AUX_DEI_SC2))
        {

            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "IOCTL Not supported for this Instance of driver \n");
            retVal = FVID2_EFAIL;

        }
#endif  /* #ifdef TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_813X_BUILD) || defined(TI_811X_BUILD) || defined(TI_8149_BUILD)
            if ((instObj->drvInstId != VPS_DISP_INST_MAIN_DEI_SC1) &&
                (instObj->drvInstId != VPS_DISP_INST_AUX_SC2))
            {

                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "IOCTL Not supported for this Instance of driver \n");
                retVal = FVID2_EFAIL;

            }
#endif  /* #ifdef TI_814X_BUILD || TI_813X_BUILD || TI_8149_BUILD */

    }

    if (FVID2_SOK == retVal)
    {
        if (instObj->state.isStarted == TRUE)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "IOCTL cannot be called when display is running \n");
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the VENC information from display controller */
        retVal = Dc_getPathInfo(instObj->dcHandle, &instObj->dcPathInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "DCtrl get mode info failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        deiDispPrms = (Vps_DeiDispParams *) cmdArgs;

        /* Do a basic error check */
        retVal = vpsDdrvCheckDeiDispParams(instObj, deiDispPrms);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Check DEI Disp Param failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Copy the paramaters into instObj */
        VpsUtils_memcpy(
            &instObj->deiDispParams,
            deiDispPrms,
            sizeof(Vps_DeiDispParams));

    }

    if (FVID2_SOK == retVal)
    {
        /* Initialize VCOMP runtime configuration */
        instObj->vcompRtCfg.scanFormat =
            instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
        instObj->vcompRtCfg.inputWidth =
            instObj->deiDispParams.scTarWidth;
        instObj->vcompRtCfg.inputHeight =
            instObj->deiDispParams.scTarHeight;
        instObj->vcompRtCfg.cropStartX = 0u;
        instObj->vcompRtCfg.cropStartY = 0u;
        instObj->vcompRtCfg.cropWidth =
            instObj->deiDispParams.scTarWidth;
        instObj->vcompRtCfg.cropHeight =
            instObj->deiDispParams.scTarHeight;

        if (NULL != instObj->deiDispParams.posCfg)
        {
            instObj->vcompRtCfg.outCropStartX = instObj->deiDispParams.posCfg->startX;
            instObj->vcompRtCfg.outCropStartY = instObj->deiDispParams.posCfg->startY;
        }
        else
        {
            instObj->vcompRtCfg.outCropStartX = 0u;
            instObj->vcompRtCfg.outCropStartY = 0u;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Configure the core according to user passed settings */
        retVal = vpsDdrvConfigCore(instObj, &instObj->deiDispParams.fmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Config core failed\n");
        }
        else
        {
            /* Reset the mosaic mode */
            instObj->curMemIdx = VPSDDRV_NON_MOSAIC_MEM_IDX;
            instObj->state.isMultiWinMode = FALSE;
        }
    }

    return (retVal);
}



static Int32 vpsDdrvSetScCoeff(VpsDdrv_InstObj *instObj,
                               Ptr cmdArgs)
{
    Int32                    retVal = FVID2_SOK;
    const Vps_ScCoeffParams *coeffPrms;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {

#ifdef TI_816X_BUILD
        if ((instObj->drvInstId != VPS_DISP_INST_MAIN_DEIH_SC1) &&
            (instObj->drvInstId != VPS_DISP_INST_AUX_DEI_SC2))
        {

            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "IOCTL Not supported for this Instance of driver \n");
            retVal = FVID2_EFAIL;

        }
#endif  /* #ifdef TI_816X_BUILD */

#if defined(TI_814X_BUILD) || defined(TI_813X_BUILD) || defined(TI_811X_BUILD) || defined(TI_8149_BUILD)
            if ((instObj->drvInstId != VPS_DISP_INST_MAIN_DEI_SC1) &&
                (instObj->drvInstId != VPS_DISP_INST_AUX_SC2))
            {

                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "IOCTL Not supported for this Instance of driver \n");
                retVal = FVID2_EFAIL;

            }
#endif  /* #ifdef TI_814X_BUILD || TI_813X_BUILD || TI_8149_BUILD */
    }

    if (FVID2_SOK == retVal)
    {
        if (instObj->state.isStarted == TRUE)
        {

            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "IOCTL cannot be called when display is running \n");
            retVal = FVID2_EFAIL;

        }
    }
    if (FVID2_SOK == retVal)
    {
        coeffPrms = (Vps_ScCoeffParams *) cmdArgs;

        instObj->coeffPrms.hScalingSet = coeffPrms->hScalingSet;
        instObj->coeffPrms.vScalingSet = coeffPrms->vScalingSet;
        instObj->coeffPrms.horzCoeffMemPtr = NULL; /* Returned by core */
        instObj->coeffPrms.vertCoeffMemPtr = NULL; /* Returned by core */
        instObj->coeffPrms.vertBilinearCoeffMemPtr = NULL; /*Returned by core*/
        instObj->coeffPrms.userCoeffPtr = coeffPrms->coeffPtr;
    }

    return (retVal);
}



/**
 *  vpsDrvCanCurBufBeDequeued
 *  Returns TRUE if the current set frame could be put in the done queue
 *  by checking if the frame is present in any of the buffer management states
 *  like other sets, semi programmed buffer etc... Else this returns FALSE.
 */
static UInt32 vpsDrvCanCurBufBeDequeued(const VpsDdrv_BufManObj *bmObj,
                                        UInt32 curSet)
{
    UInt32          descSet;
    UInt32          safeFlag = TRUE;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != bmObj));
    GT_assert(VpsDdrvDispTrace, (curSet < VPSDDRV_NUM_DESC_SET));

    /* If the current buffer is NULL, we can't dequeue, isn't it!! */
    if (NULL == bmObj->prgmedBuf[curSet])
    {
        safeFlag = FALSE;
    }

    /* Check whether the current buffer is programmed some where else in the
     * programmed buffer set */
    for (descSet = 0u; descSet < bmObj->numPrgmedFrm; descSet++)
    {
        /* Leave out the current set. It is going to be TRUE always!! */
        if (curSet != descSet)
        {
            if (bmObj->prgmedBuf[curSet] == bmObj->prgmedBuf[descSet])
            {
                /* Current set buffer is used in other sets!!
                 * So can't dequeue current buffer */
                safeFlag = FALSE;
                break;
            }
        }
    }

    /* Check with other state variables */
    if (bmObj->prgmedBuf[curSet] == bmObj->semiPrgmedBuf)
    {
        safeFlag = FALSE;
    }

    return (safeFlag);
}



/**
 *  vpsDrvGetNextBufState
 *  Depending on the availability of buffer in the input request queue and
 *  depending on progressive or interlaced mode of operation, this function
 *  determines what buffer should be programmed to the next descriptor set.
 *  If no reprogramming is required, then this returns NULL and the caller
 *  should not update the descriptor again.
 */
static VpsDdrv_QueObj *vpsDrvGetNextBufState(VpsDdrv_InstObj *instObj,
                                             UInt32 curSet,
                                             UInt32 nextSet,
                                             UInt32 curFid)
{
    VpsDdrv_QueObj     *nextQObj = NULL, *newQObj;
    VpsDdrv_BufManObj  *bmObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (curSet < VPSDDRV_NUM_DESC_SET));
    bmObj = &instObj->bmObj;

    /* For interlaced we virtually deal with half the set. Since
     * two sets are required to program a buffer, two DC set acts
     * like a buffer management set!! */
    if (TRUE != bmObj->isProgressive)
    {
        curSet = (curSet / FVID2_MAX_FIELDS);
        nextSet = (nextSet / FVID2_MAX_FIELDS);
    }

    if (TRUE == bmObj->isProgressive)
    {
        /* Get new buffer from the request queue */
        newQObj = VpsUtils_dequeue(bmObj->reqQ);
        if (NULL != newQObj)
        {
            /* Program the new buffer */
            bmObj->prgmedBuf[nextSet] = newQObj;
            nextQObj = newQObj;
        }
        else
        {
            /* No new buffer available - program the current buffer again
             * to the next set if needed */
            GT_assert(VpsDdrvDispTrace, (NULL != bmObj->prgmedBuf[curSet]));
            if (bmObj->prgmedBuf[nextSet] != bmObj->prgmedBuf[curSet])
            {
                nextQObj = bmObj->prgmedBuf[curSet];
            }
            bmObj->prgmedBuf[nextSet] = bmObj->prgmedBuf[curSet];
            instObj->repeatFrameCount++;
        }
    }
    else
    {
        if (0u == curFid)
        {
            /* Make previously programmed field buffer as current buffer */
            GT_assert(VpsDdrvDispTrace, (NULL != bmObj->semiPrgmedBuf));
            nextQObj = bmObj->semiPrgmedBuf;
            bmObj->prgmedBuf[curSet] = bmObj->semiPrgmedBuf;
            bmObj->semiPrgmedBuf = NULL;
        }
        else
        {
            /* Get new buffer from the request queue */
            newQObj = VpsUtils_dequeue(bmObj->reqQ);
            if (NULL != newQObj)
            {
                /* Program the new buffer */
                bmObj->semiPrgmedBuf = newQObj;
                nextQObj = newQObj;
            }
            else
            {
                /* No new buffer available - program the current buffer again
                 * to the next set if needed */
                GT_assert(VpsDdrvDispTrace, (NULL != bmObj->prgmedBuf[curSet]));
                if (bmObj->prgmedBuf[nextSet] != bmObj->prgmedBuf[curSet])
                {
                    nextQObj = bmObj->prgmedBuf[curSet];
                }
                bmObj->semiPrgmedBuf = bmObj->prgmedBuf[curSet];
                instObj->repeatFrameCount++;
            }
        }
    }

    return (nextQObj);
}



/**
 *  vpsDdrvRemoveBufFromState
 *  This removes frames from the buffer management state and accordingly clear
 *  the states of its occurrence. reverseOrder will determine in which
 *  order the buffers should be removed from the state. When stopping
 *  display, frames should be removed in reverse order so that the next start
 *  IOCTL will start displaying frames in the same sequence as the user
 *  has queued it initially. For all other operation (Dequeue) the order
 *  is normal.
 */
static VpsDdrv_QueObj *vpsDdrvRemoveBufFromState(VpsDdrv_BufManObj *bmObj,
                                                 UInt32 reverseOrder)
{
    UInt32              descSet, setIndex;
    VpsDdrv_QueObj     *qObj = NULL;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != bmObj));

    /* When reverse order is TRUE, then give back the semi programmed buffer
     * before programmed buffers. */
    if (TRUE == reverseOrder)
    {
        /* Give back the semi programmed buffer if any */
        if (NULL == qObj)
        {
            if (NULL != bmObj->semiPrgmedBuf)
            {
                qObj = bmObj->semiPrgmedBuf;
                bmObj->semiPrgmedBuf = NULL;
            }
        }
    }

    /* Then give back the programmed buffer */
    if (NULL == qObj)
    {
        /* Always look from the current buffer so that buffers will not
         * be returned in out of sequence. */
        if (TRUE == bmObj->isProgressive)
        {
            setIndex = bmObj->expectedSet;
        }
        else
        {
            /* For interlaced we virtually deal with half the set. Since
             * two sets are required to program a buffer, two DC set acts
             * like a buffer management set!! */
            setIndex = (bmObj->expectedSet / FVID2_MAX_FIELDS);
        }

        if (TRUE == reverseOrder)
        {
            for (descSet = 0u; descSet < bmObj->numPrgmedFrm; descSet++)
            {
                /* Wrap around */
                if (0u == setIndex)
                {
                    setIndex = (bmObj->numPrgmedFrm - 1u);
                }
                else
                {
                    setIndex--;
                }
                if (NULL != bmObj->prgmedBuf[setIndex])
                {
                    qObj = bmObj->prgmedBuf[setIndex];
                    bmObj->prgmedBuf[setIndex] = NULL;
                    break;
                }
            }
        }
        else
        {
            for (descSet = 0u; descSet < bmObj->numPrgmedFrm; descSet++)
            {
                /* Wrap around */
                if (setIndex >= bmObj->numPrgmedFrm)
                {
                    setIndex = 0u;
                }
                if (NULL != bmObj->prgmedBuf[setIndex])
                {
                    qObj = bmObj->prgmedBuf[setIndex];
                    bmObj->prgmedBuf[setIndex] = NULL;
                    break;
                }
                setIndex++;
            }
        }
    }

    /* When reverse order is FALSE, then give back the semi programmed buffer
     * after the programmed buffers. */
    if (TRUE != reverseOrder)
    {
        /* Give back the semi programmed buffer if any */
        if (NULL == qObj)
        {
            if (NULL != bmObj->semiPrgmedBuf)
            {
                qObj = bmObj->semiPrgmedBuf;
                bmObj->semiPrgmedBuf = NULL;
            }
        }
    }

    /* Reset all the similar buffers in the set. Otherwise we might return
     * the same buffer again. */
    if (NULL != qObj)
    {
        for (descSet = 0u; descSet < bmObj->numPrgmedFrm; descSet++)
        {
            if (qObj == bmObj->prgmedBuf[descSet])
            {
                bmObj->prgmedBuf[descSet] = NULL;
            }
        }

        if (qObj == bmObj->semiPrgmedBuf)
        {
            bmObj->semiPrgmedBuf = NULL;
        }
    }

    return (qObj);
}



/**
 *  vpsDdrvRetErrFrameList
 *  Returns the frame list back to the application if error callback is
 *  registered. It also puts the queue object back to the free queue.
 */
static Void vpsDdrvRetErrFrameList(VpsDdrv_InstObj *instObj,
                                   VpsDdrv_QueObj *qObj)
{
    UInt32      cookie;

    /* NULL pointer check */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != qObj));

    if ((NULL != instObj->fdmCbPrms.fdmErrCbFxn) &&
        (NULL != instObj->fdmCbPrms.errList))
    {
        /* Copy the driver's frame list to application's error frame list */
        FVID2_copyFrameList(instObj->fdmCbPrms.errList, &qObj->frameList);

        /* Call the application's error call back function */
        instObj->fdmCbPrms.fdmErrCbFxn(
            instObj->fdmCbPrms.fdmData,
            instObj->fdmCbPrms.errList,
            NULL);
    }

    cookie = Hwi_disable();         /* Disable global interrupts */

    /* Give back the queue object back to the free pool */
    VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
    instObj->dequeueCount++;

    Hwi_restore(cookie);            /* Restore global interrupts */

    return;
}



/**
 *  vpsDdrvUpdateRtParams
 *  Updates the runtime parameters. If rtPrms is NULL and the current set is
 *  dirty, it will update the descriptor with the old configuration.
 */
static Int32 vpsDdrvUpdateRtParams(VpsDdrv_InstObj *instObj,
                                   const Vps_DispRtParams *rtPrms,
                                   UInt32 descSet)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    Dc_RtConfig        *vcompRtCfg = NULL;
    VpsDdrv_MemObj     *memObj = NULL;
    Dc_UpdateDescInfo   dcDescInfo;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (descSet < VPSDDRV_NUM_DESC_SET));

    if (NULL != rtPrms)
    {
        /* Select the valid layout ID, ignore if set to invalid */
        if (NULL != rtPrms->layoutId)
        {
            /* Get the memory object for the layout ID */
            memObj = vpsDdrvGetMemObjForLayoutId(
                         instObj,
                         rtPrms->layoutId->layoutId);
            if (NULL == memObj)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR, "Invalid layout ID\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
            else
            {
                /* Set the current memory object index and enable mosaic mode */
                instObj->curMemIdx = vpsDdrvGetIdxForMemObj(instObj, memObj);
                instObj->state.isMultiWinMode = TRUE;
            }
        }

        /* Check for VCOMP crop params */
        if (NULL != rtPrms->vcompCropCfg)
        {
            vcompRtCfg = &instObj->vcompRtCfg;

            /* Update the driver object */
            vcompRtCfg->cropStartX = rtPrms->vcompCropCfg->cropStartX;
            vcompRtCfg->cropStartY = rtPrms->vcompCropCfg->cropStartY;
            vcompRtCfg->cropWidth = rtPrms->vcompCropCfg->cropWidth;
            vcompRtCfg->cropHeight = rtPrms->vcompCropCfg->cropHeight;
        }

        /* Check for VCOMP position params */
        if (NULL != rtPrms->vcompPosCfg)
        {
            vcompRtCfg = &instObj->vcompRtCfg;

            /* Update the driver object */
            vcompRtCfg->outCropStartX = rtPrms->vcompPosCfg->startX;
            vcompRtCfg->outCropStartY = rtPrms->vcompPosCfg->startY;
        }
    }
    else if (TRUE == instObj->isDescDirty[descSet])
    {
        /* Update with the old configuration */
        memObj = &instObj->memObj[instObj->curMemIdx];

        /* Update VCOMP RT only if it is available in the path */
        if ((TRUE == instObj->dcPathInfo.isMainVcompAvail) ||
            (TRUE == instObj->dcPathInfo.isAuxVcompAvail))
        {
            vcompRtCfg = &instObj->vcompRtCfg;
        }
    }

    /* Update descriptors in DLM if layout changes and only if display has
     * already started */
    if ((FVID2_SOK == retVal) &&
        (NULL != memObj) &&
        (TRUE == instObj->state.isStarted))
    {
        /* Copy the descriptor info for the current set */
        VpsUtils_memcpy(
            &dcDescInfo.descAddr[0u],
            &memObj->dcDescInfo.descAddr[descSet][0u],
            (sizeof(Ptr) * DC_DESC_PER_CLIENT_PER_SET));
        VpsUtils_memcpy(
            &dcDescInfo.descSize[0u],
            &memObj->dcDescInfo.descSize[descSet][0u],
            (sizeof(UInt32) * DC_DESC_PER_CLIENT_PER_SET));
        VpsUtils_memcpy(
            &dcDescInfo.rldDescAddr[0u],
            &memObj->dcDescInfo.rldDescAddr[descSet][0u],
            (sizeof(Ptr) * DC_DESC_PER_CLIENT_PER_SET));

        /* Update the descriptors */
        retVal = Dc_updateDescMem(instObj->dcHandle, descSet, &dcDescInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Update DCTRL descriptor memory failed\n");
        }
    }

    if ((FVID2_SOK == retVal) &&
        (NULL != vcompRtCfg) &&
        (TRUE == instObj->state.isStarted))
    {
        /* Update VCOMP RT only if it is available in the path */
        if ((TRUE == instObj->dcPathInfo.isMainVcompAvail) ||
            (TRUE == instObj->dcPathInfo.isAuxVcompAvail))
        {
            retVal = Dc_setRtConfig(
                         instObj->dcHandle,
                         descSet,
                         DC_RTCONFIGMODULE_VCOMP,
                         vcompRtCfg);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "Update DCTRL VCOMP runtime parameter failed\n");
            }
        }
        else
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "VCOMP not available in this path. Ignoring runtime params\n");
        }
    }

    /* Update core RT params only if display has already started */
    if ((FVID2_SOK == retVal) && (TRUE == instObj->state.isStarted))
    {
        retVal = vpsDdrvUpdateCoreRtParams(instObj, rtPrms, descSet);
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the dirty flags */
        if (NULL != rtPrms)
        {
            /* Mark all other sets as dirty except the updated set */
            for (cnt = 0u; cnt < VPSDDRV_NUM_DESC_SET; cnt++)
            {
                instObj->isDescDirty[cnt] = TRUE;
            }
        }

        /* Mark the current set as clean */
        instObj->isDescDirty[descSet] = FALSE;
    }

    return (retVal);
}



/**
 *  vpsDdrvUpdateCoreRtParams
 *  Updates the runtime core parameters. If rtPrms is NULL and the current set
 *  is dirty, it will update the descriptor with the old configuration.
 *  Note: This should be called only after slicing the core descriptor.
 */
static Int32 vpsDdrvUpdateCoreRtParams(VpsDdrv_InstObj *instObj,
                                       const Vps_DispRtParams *rtPrms,
                                       UInt32 descSet)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Property      property;
    Vcore_BpRtParams    bpRtPrms;
    Vcore_SecRtParams   secRtPrms;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (descSet < VPSDDRV_NUM_DESC_SET));

    /* Update width/height and position RT params only for non-mosaic mode */
    if (FALSE == instObj->state.isMultiWinMode)
    {
        GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getProperty));
        GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->updateRtMem));

        /* Get core property */
        retVal = instObj->coreOps->getProperty(instObj->coreHandle, &property);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == retVal));

        if (VCORE_TYPE_BP == property.name)
        {
            /* Update runtime parameters for BP core */
            if (NULL != rtPrms)
            {
                bpRtPrms.inFrmPrms = rtPrms->inFrmPrms;
                bpRtPrms.posCfg = rtPrms->vpdmaPosCfg;

                /* Update the runtime configuration in core descriptors */
                retVal = instObj->coreOps->updateRtMem(
                             instObj->coreHandle,
                             &instObj->memObj[instObj->curMemIdx].
                                                        coreDescMem[descSet],
                             0u,
                             &bpRtPrms,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsDdrvDispTrace, GT_ERR,
                        "Could not update runtime desc for BP core!\n");
                }
            }
            else if (TRUE == instObj->isDescDirty[descSet])
            {
                /* Descriptor is dirty, use the core to just update the
                 * descriptors without providing any parameters */
                retVal = instObj->coreOps->updateRtMem(
                             instObj->coreHandle,
                             &instObj->memObj[instObj->curMemIdx].
                                                        coreDescMem[descSet],
                             0u,
                             NULL,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsDdrvDispTrace, GT_ERR,
                        "Could not update runtime desc for BP core!\n");
                }
            }
        }
        else
        {
            /* Update runtime parameters for SEC core */
            if (NULL != rtPrms)
            {
                secRtPrms.inFrmPrms = rtPrms->inFrmPrms;
                secRtPrms.posCfg = rtPrms->vpdmaPosCfg;

                /* Update the runtime configuration in core descriptors */
                retVal = instObj->coreOps->updateRtMem(
                             instObj->coreHandle,
                             &instObj->memObj[instObj->curMemIdx].
                                                        coreDescMem[descSet],
                             0u,
                             &secRtPrms,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsDdrvDispTrace, GT_ERR,
                        "Could not update runtime desc for SEC core!\n");
                }
            }
            else if (TRUE == instObj->isDescDirty[descSet])
            {
                /* Descriptor is dirty, use the core to just update the
                 * descriptors without providing any parameters */
                retVal = instObj->coreOps->updateRtMem(
                             instObj->coreHandle,
                             &instObj->memObj[instObj->curMemIdx].
                                                        coreDescMem[descSet],
                             0u,
                             NULL,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsDdrvDispTrace, GT_ERR,
                        "Could not update runtime desc for SEC core!\n");
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvCheckRtParams
 *  Checks whether the runtime parameters passed by the user is valid or not
 *  and returns appropriate error codes.
 */
static Int32 vpsDdrvCheckRtParams(VpsDdrv_InstObj *instObj,
                                  const Vps_DispRtParams *rtPrms)
{
    Int32           retVal = FVID2_SOK;
    VpsDdrv_MemObj *memObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != rtPrms));

    /* Check for valid layout ID, ignore if set to invalid */
    if (NULL != rtPrms->layoutId)
    {
        memObj = vpsDdrvGetMemObjForLayoutId(
                     instObj,
                     rtPrms->layoutId->layoutId);
        if (NULL == memObj)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Invalid layout ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    return (retVal);
}



/**
 *  vpsDeiDdrvCheckRtParams
 *  Checks whether the runtime parameters passed by the user is valid or not
 *  and returns appropriate error codes.
 */
static Int32 vpsDeiDdrvCheckRtParams(VpsDdrv_InstObj *instObj,
                                     const Vps_DeiDispRtParams *rtPrms)
{
    Int32           retVal = FVID2_SOK;
    UInt32          isVcompAvail = FALSE;
    UInt32          inWidth, inHeight, outWidth, outHeight;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != rtPrms));

    if (NULL != rtPrms->layoutId)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Mosaic on DEI Path is not supported\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if ((TRUE == instObj->dcPathInfo.isMainVcompAvail) ||
        (TRUE == instObj->dcPathInfo.isAuxVcompAvail))
    {
        isVcompAvail = TRUE;
    }

    if (NULL != rtPrms->inFrmPrms)
    {
        inWidth = rtPrms->inFrmPrms->width;
        inHeight = rtPrms->inFrmPrms->height;
    }
    else
    {
        inWidth = instObj->deiDispParams.fmt.width;
        inHeight = instObj->deiDispParams.fmt.height;
    }

    if (NULL != rtPrms->outFrmPrms)
    {
        outWidth = rtPrms->outFrmPrms->width;
        outHeight = rtPrms->outFrmPrms->height;
    }
    else
    {
        outWidth = instObj->deiDispParams.scTarWidth;
        outHeight = instObj->deiDispParams.scTarHeight;
    }

    if ((inWidth > instObj->dcPathInfo.modeInfo.mInfo.width) ||
        (inHeight > instObj->dcPathInfo.modeInfo.mInfo.height))
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "INVALID_PARAMS: input frame size greater than VENC frame size\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (NULL != rtPrms->inFrmPrms)
    {
        if (rtPrms->inFrmPrms->dataFormat != instObj->deiDispParams.fmt.dataFormat)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Input DataType is not matching\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
        if (rtPrms->inFrmPrms->memType != instObj->memType)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Memory Type is not matching\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (TRUE == isVcompAvail)
    {
        if ((outWidth > instObj->dcPathInfo.modeInfo.mInfo.width) ||
            (outHeight > instObj->dcPathInfo.modeInfo.mInfo.height))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Sc Target Size is more than VENC Frame Size\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
    else
    {
        /* If there is no VCOMP, target size should be same as VENC size */
        if ((outWidth != instObj->dcPathInfo.modeInfo.mInfo.width) ||
            (outHeight != instObj->dcPathInfo.modeInfo.mInfo.height))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Sc Target Size is not equal to VENC Frame Size\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if ((TRUE == instObj->deiDispParams.scCfg.bypass) &&
        (NULL != rtPrms->deiScCropCfg))
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "INVALID_PARAMS: Scalar cropping cannot be used when \
                scalar is bypassed\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if ((FALSE == instObj->deiDispParams.scCfg.bypass) && (NULL != rtPrms->deiScCropCfg))
    {
        if (((rtPrms->deiScCropCfg->cropStartX +
              rtPrms->deiScCropCfg->cropWidth) >
                inWidth) ||
            ((rtPrms->deiScCropCfg->cropStartY +
              rtPrms->deiScCropCfg->cropHeight) >
              inHeight))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Crop Size is Invalid\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (TRUE == instObj->deiDispParams.scCfg.bypass)
    {
        /* Input width cannot be more than output width
           when scalar is bypassed.
           This is possible only in case VCOMP cropping is enabled,
           which is not supported. */
        if ((inWidth > outWidth) || (inHeight > outHeight))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: output frame size is less than \
                    input frame size\n");
            retVal = FVID2_EINVALID_PARAMS;
        }

        if (FALSE == isVcompAvail)
        {
            if ((instObj->dcPathInfo.modeInfo.mInfo.width !=
                        outWidth) ||
                (instObj->dcPathInfo.modeInfo.mInfo.height !=
                        outHeight))
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "INVALID_PARAMS: Target frame size must be same as VENC \
                        frame size\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    if (NULL != rtPrms->posCfg)
    {
        if (TRUE == isVcompAvail)
        {
            if (((rtPrms->posCfg->startX + outWidth) >
                    instObj->dcPathInfo.modeInfo.mInfo.width) ||
                ((rtPrms->posCfg->startY + outHeight) >
                    instObj->dcPathInfo.modeInfo.mInfo.height))
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "INVALID_PARAMS: Wrong Position for the VCOMP input\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
        else
        {
            if (((rtPrms->posCfg->startX + inWidth) >
                    instObj->dcPathInfo.modeInfo.mInfo.width) ||
                ((rtPrms->posCfg->startY + inHeight) >
                    instObj->dcPathInfo.modeInfo.mInfo.height))
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "INVALID_PARAMS: Wrong Position for the VCOMP input\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    return (retVal);
}



/**
 *  vpsDeiDdrvUpdateRtParams
 *  Updates the runtime parameters. If rtPrms is NULL and the current set is
 *  dirty, it will update the descriptor with the old configuration.
 */
static Int32 vpsDeiDdrvUpdateRtParams(VpsDdrv_InstObj *instObj,
                                      const Vps_DeiDispRtParams *rtPrms,
                                      UInt32 descSet)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt, isVcompAvail = FALSE;
    Dc_RtConfig        *vcompRtCfg = NULL;
    VpsDdrv_MemObj     *memObj = NULL;
    Dc_UpdateDescInfo   dcDescInfo;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (descSet < VPSDDRV_NUM_DESC_SET));

    if ((TRUE == instObj->dcPathInfo.isMainVcompAvail) ||
        (TRUE == instObj->dcPathInfo.isAuxVcompAvail))
    {
        isVcompAvail = TRUE;
    }

    if (NULL != rtPrms)
    {
        /* Select the valid layout ID, ignore if set to invalid */
        if (NULL != rtPrms->layoutId)
        {
            /* Get the memory object for the layout ID */
            memObj = vpsDdrvGetMemObjForLayoutId(
                         instObj,
                         rtPrms->layoutId->layoutId);
            if (NULL == memObj)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR, "Invalid layout ID\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
            else
            {
                /* Set the current memory object index and enable mosaic mode */
                instObj->curMemIdx = vpsDdrvGetIdxForMemObj(instObj, memObj);
                instObj->state.isMultiWinMode = TRUE;
            }
        }

        if (TRUE == isVcompAvail)
        {
            if (NULL != rtPrms->outFrmPrms)
            {
                vcompRtCfg = &instObj->vcompRtCfg;

                vcompRtCfg->inputWidth = rtPrms->outFrmPrms->width;
                vcompRtCfg->inputHeight = rtPrms->outFrmPrms->height;

                /* Change VCOMP Croppping as well since input size for
                   VCOMP is changed */
                vcompRtCfg->cropStartX = 0;
                vcompRtCfg->cropStartY = 0;
                vcompRtCfg->cropWidth = vcompRtCfg->inputWidth;
                vcompRtCfg->cropHeight = vcompRtCfg->inputHeight;
            }

            /* Check for VCOMP crop params */
            if (NULL != rtPrms->vcompCropCfg)
            {
                vcompRtCfg = &instObj->vcompRtCfg;

                /* Update the driver object */
                vcompRtCfg->cropStartX = rtPrms->vcompCropCfg->cropStartX;
                vcompRtCfg->cropStartY = rtPrms->vcompCropCfg->cropStartY;
                vcompRtCfg->cropWidth = rtPrms->vcompCropCfg->cropWidth;
                vcompRtCfg->cropHeight = rtPrms->vcompCropCfg->cropHeight;
            }

            if (NULL != rtPrms->posCfg)
            {
                vcompRtCfg = &instObj->vcompRtCfg;

                /* Update the driver object */
                vcompRtCfg->outCropStartX = rtPrms->posCfg->startX;
                vcompRtCfg->outCropStartY = rtPrms->posCfg->startY;
            }
        }
    }
    else if (TRUE == instObj->isDescDirty[descSet])
    {
        /* Update with the old configuration */
        memObj = &instObj->memObj[instObj->curMemIdx];

        /* Update VCOMP RT only if it is available in the path */
        if (TRUE == isVcompAvail)
        {
            vcompRtCfg = &instObj->vcompRtCfg;
        }
    }

    /* Update descriptors in DLM if layout changes and only if display has
     * already started */
    if ((FVID2_SOK == retVal) &&
        (NULL != memObj) &&
        (TRUE == instObj->state.isStarted))
    {
        /* Copy the descriptor info for the current set */
        VpsUtils_memcpy(
            &dcDescInfo.descAddr[0u],
            &memObj->dcDescInfo.descAddr[descSet][0u],
            (sizeof(Ptr) * DC_DESC_PER_CLIENT_PER_SET));
        VpsUtils_memcpy(
            &dcDescInfo.descSize[0u],
            &memObj->dcDescInfo.descSize[descSet][0u],
            (sizeof(UInt32) * DC_DESC_PER_CLIENT_PER_SET));
        VpsUtils_memcpy(
            &dcDescInfo.rldDescAddr[0u],
            &memObj->dcDescInfo.rldDescAddr[descSet][0u],
            (sizeof(Ptr) * DC_DESC_PER_CLIENT_PER_SET));

        /* Update the descriptors */
        retVal = Dc_updateDescMem(instObj->dcHandle, descSet, &dcDescInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Update DCTRL descriptor memory failed\n");
        }
    }

    if ((FVID2_SOK == retVal) &&
        (NULL != vcompRtCfg) &&
        (TRUE == instObj->state.isStarted))
    {
        /* Update VCOMP RT only if it is available in the path */
        if (TRUE == isVcompAvail)
        {
            retVal = Dc_setRtConfig(
                         instObj->dcHandle,
                         descSet,
                         DC_RTCONFIGMODULE_VCOMP,
                         vcompRtCfg);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "Update DCTRL VCOMP runtime parameter failed\n");
            }
        }
        else
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "VCOMP not available in this path. Ignoring runtime params\n");
        }
    }

    /* Update core RT params only if display has already started */
    if ((FVID2_SOK == retVal) && (TRUE == instObj->state.isStarted))
    {
        retVal = vpsDeiDdrvUpdateCoreRtParams(instObj, rtPrms, descSet);
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the dirty flags */
        if (NULL != rtPrms)
        {
            /* Mark all other sets as dirty except the updated set */
            for (cnt = 0u; cnt < VPSDDRV_NUM_DESC_SET; cnt++)
            {
                instObj->isDescDirty[cnt] = TRUE;
            }
        }

        /* Mark the current set as clean */
        instObj->isDescDirty[descSet] = FALSE;
    }

    return (retVal);
}

/**
 *  vpsDeiDdrvUpdateCoreRtParams
 *  Updates the runtime core parameters. If rtPrms is NULL and the current set
 *  is dirty, it will update the descriptor with the old configuration.
 *  Note: This should be called only after slicing the core descriptor.
 */
static Int32 vpsDeiDdrvUpdateCoreRtParams(VpsDdrv_InstObj *instObj,
                                          const Vps_DeiDispRtParams *rtPrms,
                                          UInt32 descSet)
{
    Int32               retVal = FVID2_SOK;
    Vcore_DeiRtParams   deiRtPrms;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (descSet < VPSDDRV_NUM_DESC_SET));

    /* Update width/height and position RT params only for non-mosaic mode */
    if (FALSE == instObj->state.isMultiWinMode)
    {
        GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->updateRtMem));

        /* Update runtime parameters for DEI core */
        if (NULL != rtPrms)
        {
            deiRtPrms.inFrmPrms = rtPrms->inFrmPrms;
            if ((TRUE == instObj->dcPathInfo.isMainVcompAvail) ||
                (TRUE == instObj->dcPathInfo.isAuxVcompAvail) ||
                (FALSE == instObj->deiDispParams.scCfg.bypass))
            {
                deiRtPrms.posCfg = NULL;
                deiRtPrms.isMosaicMode = 0u;
            }
            else
            {
                /* */
                deiRtPrms.posCfg = rtPrms->posCfg;
                deiRtPrms.isMosaicMode = 1u;
            }

            deiRtPrms.outFrmPrms = rtPrms->outFrmPrms;
            deiRtPrms.scCropCfg = rtPrms->deiScCropCfg;
            deiRtPrms.scRtCfg = NULL;
            deiRtPrms.deiRtCfg = NULL;

            /* Update the runtime configuration in core descriptors */
            retVal = instObj->coreOps->updateRtMem(
                         instObj->coreHandle,
                         &instObj->memObj[instObj->curMemIdx].
                                                    coreDescMem[descSet],
                         0u,
                         &deiRtPrms,
                         0u);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "Could not update runtime desc for DEI core!\n");
            }
        }
        else if (TRUE == instObj->isDescDirty[descSet])
        {
            /* Descriptor is dirty, use the core to just update the
             * descriptors without providing any parameters */
            retVal = instObj->coreOps->updateRtMem(
                         instObj->coreHandle,
                         &instObj->memObj[instObj->curMemIdx].
                                                    coreDescMem[descSet],
                         0u,
                         NULL,
                         0u);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "Could not update runtime desc for DEI core!\n");
            }
        }
    }

    return (retVal);
}


/* Function to update the descriptor memoy with the buffer address
 * in frame buffer mode of the display driver.
 *
 * This function updates the buffer address in all the descriptor set.
 */
static Int32 VpsDdrvFbUpdateDesc(VpsDdrv_InstObj *instObj,
                                 FVID2_FrameList *frmList)
{
    Int32                   retVal = FVID2_SOK;
    VpsDdrv_MemObj         *memObj;
    UInt32                  descSet, frmConsumed;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != frmList));

    for (descSet = 0u; descSet < VPSDDRV_NUM_DESC_SET; descSet ++)
    {
        /* TODO: Update the RT Params */

        /* Caution: It is assumed here that the input/output are in
           progressive format and FID is always 0. */

        memObj = &instObj->memObj[instObj->curMemIdx];
        if (TRUE == instObj->state.isMultiWinMode)
        {
            GT_assert(VpsDdrvDispTrace,
                (NULL != instObj->coreOps->updateMultiDesc));
            retVal = instObj->coreOps->updateMultiDesc(
                         instObj->coreHandle,
                         memObj->coreLayoutId,
                         &memObj->coreDescMem[descSet],
                         frmList,
                         0,
                         0u,
                         &frmConsumed);
        }
        else
        {
            GT_assert(VpsDdrvDispTrace,
                (NULL != instObj->coreOps->updateDesc));
            retVal = instObj->coreOps->updateDesc(
                         instObj->coreHandle,
                         &memObj->coreDescMem[descSet],
                         frmList->frames[0u],
                         0);
        }
   }

    return (retVal);
}

static Int32 vpsDdrvSetDispMode(VpsDdrv_InstObj *instObj, Ptr cmdArgs)
{
    Int32            retVal = FVID2_SOK;
    Vps_DispMode    *dispMode = (Vps_DispMode *)cmdArgs;

    /* Check for NULL Pointer */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Cannot change to FB mode once display is started */
        if (TRUE == instObj->state.isStarted)
        {
            GT_0trace(
                VpsDdrvDispTrace,
                GT_ERR,
                "BADARGS: Display Already Started\n");
            retVal = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if ((TRUE != instObj->bmObj.isProgressive) &&
            (TRUE == dispMode->isFbMode))
        {
            GT_0trace(
                VpsDdrvDispTrace,
                GT_ERR,
                "BADARGS: Frame Buffer mode is supported \
                    only for Progressive display\n");
            retVal = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Mosaic display requires specific layout which cannot be supported
           with FB mode. */
        if ((TRUE == instObj->state.isMultiWinMode) &&
            (TRUE == dispMode->isFbMode))
        {
            GT_0trace(
                VpsDdrvDispTrace,
                GT_ERR,
                "BADARGS: Frame Buffer mode is supported \
                    only for Single Window Display\n");
            retVal = FVID2_EBADARGS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (dispMode->isFbMode)
        {
            instObj->state.isFbMode = TRUE;
        }
        else
        {
            instObj->state.isFbMode = FALSE;
        }

        retVal = Dc_setDispMode(instObj->dcHandle, instObj->state.isFbMode);
    }

    return (retVal);
}
