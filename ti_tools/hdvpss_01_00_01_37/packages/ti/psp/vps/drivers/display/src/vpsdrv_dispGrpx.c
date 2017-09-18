/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_m2mDei.c
 *
 *  \brief VPS DEI M2M driver for the DEI HQ and DEI paths.
 *  This file implements the VPS DEI M2M driver.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <string.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/hal/vpshal_grpx.h>
#include <ti/psp/vps/core/vpscore_graphics.h>
#include <ti/psp/vps/drivers/display/vpsdrv_graphics.h>
#include <ti/psp/vps/drivers/display/src/vpsdrv_dispGrpxPriv.h>

/* =========================================================================== */
/*                           Macros & Typedefs                                 */
/* =========================================================================== */

/** \brief Time out to be used in sem pend. */
#define VPSDDRV_GRPX_SEM_TIMEOUT             (BIOS_WAIT_FOREVER)

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static Fdrv_Handle VpsDdrv_grpxCreate(UInt32 drvId,
                                      UInt32 instId,
                                      Ptr createArgs,
                                      Ptr createStatusArgs,
                                      const FVID2_DrvCbParams *fdmCbParams);
static Int32 VpsDdrv_grpxDelete(Fdrv_Handle handle, Ptr deleteArgs);
static Int32 VpsDdrv_grpxControl(Fdrv_Handle handle,
                                 UInt32 cmd,
                                 Ptr cmdArgs,
                                 Ptr cmdStatusArgs);
static Int32 VpsDdrv_grpxQueue(Fdrv_Handle handle,
                               FVID2_FrameList *frameList,
                               UInt32 streamId);
static Int32 VpsDdrv_grpxDequeue(Fdrv_Handle handle,
                                 FVID2_FrameList *frameList,
                                 UInt32 streamId,
                                 UInt32 timeOut);

static Int32 VpsDdrvGrpxUpdateDesc(VpsDdrv_GrpxInstObj *instObj,
                                   FVID2_FrameList *frameList);

static Int32 vpsDdrvGrpxFbUpdateBufIsr(UInt32 curSet,
                                       UInt32 timeStamp,
                                       Ptr arg);

static Int32 vpsDdrvGrpxUpdateBufIsr(UInt32 curSet,
                                     UInt32 timeStamp,
                                     Ptr arg);
static Int32 vpsDdrvGrpxStartCb(Ptr arg);
static Int32 vpsDdrvGrpxStopCb(Ptr arg);
static Int32 vpsDdrvGrpxStartingCb(Ptr arg, VpsHal_VpdmaFSEvent event);

static Int32 vpsDdrvGrpxStartIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvGrpxStopIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvGrpxSetFmtIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvGrpxGetFmtIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvGrpxCreateLayoutIoctl(VpsDdrv_GrpxInstObj *instObj,
                                          Ptr cmdArgs,
                                          Ptr cmdStatusArgs);
static Int32 vpsDdrvGrpxSetParams(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs);
static Int32 vpsDdrvGrpxGetParams(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs);
static Int32  vpsDdrvGrpxUpdateRtParams(VpsDdrv_GrpxInstObj *instObj,
                                        FVID2_FrameList     *frmList,
                                        UInt32  descSet);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**
 *  \brief Display Graphics Driver function pointer
 */

/** graphics driver function pointer. */
static const FVID2_DrvOps VpsDdrvGrpxOps =
{
    FVID2_VPS_DISP_GRPX_DRV,        /* Driver ID */
    VpsDdrv_grpxCreate,             /* Create */
    VpsDdrv_grpxDelete,             /* Delete */
    VpsDdrv_grpxControl,            /* Control */
    VpsDdrv_grpxQueue,              /* Queue */
    VpsDdrv_grpxDequeue,            /* Dequeue */
    NULL,                           /* ProcessFrames */
    NULL                            /* GetProcessedFrames */
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/**
 *  VpsDdrv_grpxInit
 *  \brief Graphics driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of graphics driver API.
 *
 *  \param numInst      Number of instance objects to be initialized.
 *  \param initParams   [IN] Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances.
 *  \param arg          [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 VpsDdrv_grpxInit(UInt32 numInst,
                       const VpsDdrv_GrpxInitParams *initParams,
                       Ptr arg)
{
    Int32                    retVal = FVID2_SOK;

    /* Check for errors */
    GT_assert(VpsDdrvGrpxTrace, (numInst <= VPS_DISP_GRPX_MAX_INST));
    GT_assert(VpsDdrvGrpxTrace, (NULL != initParams));

    retVal = vpsDdrvGrpxPrivInit(numInst, initParams);
    if (FVID2_SOK == retVal)
    {
        /* Register the driver to the driver manager */
        retVal = FVID2_registerDriver(&VpsDdrvGrpxOps);
        if (FVID2_SOK != retVal)
        {
            vpsDdrvGrpxPrivDeInit();
        }
    }

    return (retVal);
}

/**
 *  VpsDdrv_grpxDeInit
 *  \brief Graphis driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg         [IN] Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */

Int32 VpsDdrv_grpxDeInit(Ptr arg)
{
    Int32                    retVal;

    retVal = FVID2_unRegisterDriver(&VpsDdrvGrpxOps);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "Unregistering GRPX From FVID2 driver system failed\n");
    }

    retVal |= vpsDdrvGrpxPrivDeInit();
    return (retVal);
}

/**
 *  VpsDdrv_grpxCreate
 *  \brief GRPX Display driver create function. Returns the handle to the
 *  created driver instance. This function could be called from task
 *  context only.
 *  And is reentrant - could be called from multiple tasks at the same time for
 *  creating different instances.
 *
 *  \param drvId           [IN] Driver ID - Should be FVID2_VPS_DISP_GRPX_DRV.
 *  \param instId          [IN] Instance to open - VPS_DISP_INST_GRPX0,
 *                          VPS_DISP_INST_GRPX1 or VPS_DISP_INST_GRPX2.
 *  \param createArgs      [IN] GRPX Display driver create parameter - pointer
 *                          to Vps_GrpxCreateParams. This parameter should be
 *                          non-NULL.
 *  \param createStatusArgs [OUT] GRPX Display driver return status parameter -
 *                          pointer to Vps_GrpxCreateStatus. This parameter
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
static Fdrv_Handle VpsDdrv_grpxCreate(UInt32 drvId,
                                      UInt32 instId,
                                      Ptr createArgs,
                                      Ptr createStatusArgs,
                                      const FVID2_DrvCbParams *fdmCbParams)
{
    Int32                    retVal = FVID2_SOK;
    UInt32                   descSet;
    UInt32                   rscAllocFlag = FALSE;
    Fdrv_Handle              handle = NULL;
    VpsDdrv_GrpxInstObj      *instObj = NULL;
    Vps_GrpxCreateParams     *createParams;
    Vps_GrpxCreateStatus     *createStatus;

    /* pointer sanity check*/
    if ((NULL == createArgs) ||
            (NULL == fdmCbParams))
    {
        retVal = FVID2_EBADARGS;
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "BADARGS: NULL pointer.\n");
    }

    /* ID checking*/
    if (FVID2_SOK == retVal)
    {
        if (drvId != FVID2_VPS_DISP_GRPX_DRV)
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    /* Get the instance object from the pool*/
    if(FVID2_SOK == retVal)
    {
        createParams = (Vps_GrpxCreateParams *)createArgs;
        instObj = vpsDdrvGrpxGetInstObj(instId);
        if (instObj == NULL)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "Invalid instance ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
    if (FVID2_SOK == retVal)
    {
        /*acquire semaphore*/
        Semaphore_pend(instObj->instSem, VPSDDRV_GRPX_SEM_TIMEOUT);
        if (TRUE == instObj->state.isOpened)
        {
            GT_1trace(VpsDdrvGrpxTrace, GT_ERR,
                      "GRPX%d Device is alreay in used\n",instId);
            /*release semaphore*/
            Semaphore_post(instObj->instSem);
            instObj = NULL;
            retVal = FVID2_EDEVICE_INUSE;
        }
    }
    /* get the graphics driver object handle*/
    if (FVID2_SOK == retVal)
    {
        GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps));
        GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->open));
        retVal = Vrm_allocResource(instObj->resourceId);
        if (retVal != FVID2_SOK)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "DEVICE_INUSE: Resource is already allocated.\n");
            /*release semaphore*/
            Semaphore_post(instObj->instSem);
            instObj = NULL;
            retVal = FVID2_EALLOC;
        }
    }

    /* open core handle,register dc handle and create in/out Q handle*/
    if (FVID2_SOK == retVal)
    {
        instObj->dcClientInfo.arg = (Ptr) instObj;
        instObj->dcClientInfo.started = vpsDdrvGrpxStartCb;
        instObj->dcClientInfo.stopped = vpsDdrvGrpxStopCb;
        instObj->dcClientInfo.starting = vpsDdrvGrpxStartingCb;

        rscAllocFlag = TRUE;
        /*store the create parameter*/
        instObj->periodicCallbackEnable = createParams->periodicCallbackEnable;
        if(VPS_GRPX_STREAMING_MODE == createParams->drvMode)
        {
            instObj->dcClientInfo.isFbMode = FALSE;
            instObj->state.isFbMode = FALSE;
            instObj->dcClientInfo.cbFxn = vpsDdrvGrpxUpdateBufIsr;
        }
        else
        {
            instObj->dcClientInfo.isFbMode = TRUE;
            instObj->state.isFbMode = TRUE;
            instObj->dcClientInfo.cbFxn = vpsDdrvGrpxFbUpdateBufIsr;
        }


        instObj->coreHandle = instObj->coreOps->open(
                instObj->coreInstId,
                VCORE_OPMODE_DISPLAY,
                1u,
                FALSE);
        instObj->dcHandle = Dc_registerClient(
                instObj->dcNodeNum,
                &instObj->dcClientInfo,
                NULL);
        instObj->bmObj.inQHandle = VpsUtils_createQ();
        instObj->bmObj.outQHandle = VpsUtils_createQ();
        instObj->bmObj.fbQObj = NULL;

        if ((NULL == instObj->coreHandle) ||
                (NULL == instObj->dcHandle) ||
                (NULL == instObj->bmObj.inQHandle) ||
                (NULL == instObj->bmObj.outQHandle))
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                     "GRPX: Alloc memory allocation failed.\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the VENC information from display controller */
        retVal = Dc_getPathInfo(instObj->dcHandle, &instObj->dcPathInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "DCtrl get mode info failed\n");
        }
    }

    /*set the Core */
    if (FVID2_SOK == retVal)
    {
        retVal = vpsDdrvGrpxSetCore(instObj, NULL);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "GRPX Set Core Failed\n");
        }
    }
    if (FVID2_SOK == retVal)
    {
        /* initialize state */
        instObj->state.isOpened = TRUE;
        instObj->state.isStarted = FALSE;
        instObj->state.isMultiRegion = FALSE;
        instObj->displayCount = 0u;
        instObj->bmObj.sPrgmedBuf = NULL;
        for (descSet = 0; descSet < DC_NUM_DESC_SET; descSet++)
        {
          instObj->bmObj.prgmedBuf[descSet] = NULL;
        }
        /*store the create parameters*/
        instObj->fdmCbParams.fdmCbFxn = fdmCbParams->fdmCbFxn;
        instObj->fdmCbParams.fdmErrCbFxn = fdmCbParams->fdmErrCbFxn;
        instObj->fdmCbParams.errList = fdmCbParams->errList;
        instObj->fdmCbParams.fdmData = fdmCbParams->fdmData;

        handle = (Fdrv_Handle)instObj;
    }
    else
    {
        /* close all allocated handle and resources*/
        if (NULL != instObj)
        {
            if (TRUE == rscAllocFlag)
            {
                Vrm_releaseResource(instObj->resourceId);
                rscAllocFlag = FALSE;
            }
            if (NULL != instObj->coreHandle)
            {
                GT_assert(VpsDdrvGrpxTrace,
                             (NULL != instObj->coreOps->close));
                instObj->coreOps->close(instObj->coreHandle);
                instObj->coreHandle = NULL;
            }
            if (NULL != instObj->dcHandle)
            {
                Dc_unRegisterClient(instObj->dcHandle);
                instObj->dcHandle = NULL;
            }
            if (NULL != instObj->bmObj.inQHandle)
            {
                VpsUtils_deleteQ(instObj->bmObj.inQHandle);
                instObj->bmObj.inQHandle = NULL;
            }
            if (NULL != instObj->bmObj.outQHandle)
            {
                VpsUtils_deleteQ(instObj->bmObj.outQHandle);
                instObj->bmObj.outQHandle = NULL;
            }

            instObj->state.isFbMode = FALSE;
        }
    }

    if (NULL != instObj)
    {
        /*release semaphore*/
        Semaphore_post(instObj->instSem);
    }

    if (NULL != createStatusArgs)
    {
        createStatus = (Vps_GrpxCreateStatus *)createStatusArgs;
        createStatus->retVal = retVal;
        if(FVID2_SOK == retVal)
        {
            createStatus->minBufNum = VPSDDRV_GRPX_MIN_BUFFERS;
            createStatus->dispWidth = instObj->dcPathInfo.modeInfo.mInfo.width;
            createStatus->dispHeight = instObj->dcPathInfo.modeInfo.mInfo.height;
            /*only one region is support for the framebuffer mode*/
            if (VPS_GRPX_STREAMING_MODE  == instObj->state.isFbMode)
            {
                createStatus->maxRegions = VPSDDRV_GRPX_MAX_REGIONS;
                createStatus->maxReq = VPSDDRV_GRPX_MAX_QOBJS_PER_INST;
            }
            else
            {
                createStatus->maxReq = 1u;
                createStatus->maxRegions = 1u;
            }
        }
    }
    return (handle);
}

/**
 *  VpsDdrv_grpxDelete
 *  \brief Grpx driver delete function. Relinquish the resources allocated
 *  by create function. This function could be called from task context only.
 *  And is reentrant - could be called from multiple tasks at the same time for
 *  deleting different instances.
 *
 *  \param handle           [IN] GRPX driver instance handle returned by
 *                          create function.
 *  \param deleteArgs       [IN] Not used currently. Meant for future purpose.
 *                          Set this to NULL.
 *
 *  \return                 Returns FVID2_SOK on success else returns
 *                          appropriate error code.
 */
static Int32 VpsDdrv_grpxDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Int32                    retVal = FVID2_SOK;
    UInt32                   descSet;
    VpsDdrv_GrpxQueueObj     *qObj = NULL;
    VpsDdrv_GrpxInstObj      *instObj = NULL;

    /* pointer sanity check */
    if (NULL == handle)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        instObj = (VpsDdrv_GrpxInstObj *)handle;
        /*acquire semaphore*/
        Semaphore_pend(instObj->instSem, VPSDDRV_GRPX_SEM_TIMEOUT);
        /* Check if already opened. */
        if (TRUE != instObj->state.isOpened)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "ERROR: Driver not opened\n");
            retVal = FVID2_EFAIL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* If already started, stop it. */
        if (TRUE == instObj->state.isStarted)
        {
            retVal = vpsDdrvGrpxStop(instObj);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                        "ERROR: Stop Display Failed\n");
            }

        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Free up the queue objects if application did not call dequeue after
         * stopping the display. Flush the OutQ */
        while (NULL != (qObj = VpsUtils_dequeue(instObj->bmObj.outQHandle)))
        {
            /* Give back the queue object back to the free pool */
            VpsUtils_queue(instObj->freeGrpxQ, &qObj->qElem, qObj);

        }

        /* Flush buffers in current state */
        while (NULL !=
              (qObj = vpsDdrvGrpxRemoveBufFromState(&instObj->bmObj, FALSE)))
        {
            /* Give back the queue object back to the free pool */
            VpsUtils_queue(instObj->freeGrpxQ, &qObj->qElem, qObj);

        }

        /* Flush the InQ */
        while (NULL != (qObj = VpsUtils_dequeue(instObj->bmObj.inQHandle)))
        {
            /* Give back the queue object back to the free pool */
            VpsUtils_queue(instObj->freeGrpxQ, &qObj->qElem, qObj);

        }
        /* Close handles */
        Vrm_releaseResource(instObj->resourceId);
        GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->close));
        instObj->coreOps->close(instObj->coreHandle);
        Dc_unRegisterClient(instObj->dcHandle);
        VpsUtils_deleteQ(instObj->bmObj.inQHandle);
        VpsUtils_deleteQ(instObj->bmObj.outQHandle);
        instObj->dcHandle = NULL;
        instObj->coreHandle = NULL;
        instObj->bmObj.inQHandle = NULL;
        instObj->bmObj.outQHandle = NULL;
        instObj->bmObj.fbQObj = NULL;
        /* Reset other variables */
        instObj->state.isStarted = FALSE;
        instObj->state.isOpened = FALSE;
        instObj->state.isFbMode = FALSE;
        instObj->state.isMultiRegion = FALSE;
        instObj->bmObj.expectedSet = 0u;
        instObj->bmObj.fid = 0u;
        instObj->bmObj.isProgressive = FALSE;
        instObj->displayCount = 0u;
        /* Assert of the buffers are not NULL - if not freed */
        GT_assert(VpsDdrvGrpxTrace, (NULL == instObj->bmObj.sPrgmedBuf));
        instObj->bmObj.sPrgmedBuf = NULL;
        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet++)
        {
            GT_assert(VpsDdrvGrpxTrace,
                   (NULL == instObj->bmObj.prgmedBuf[descSet]));
            instObj->bmObj.prgmedBuf[descSet] = NULL;
            instObj->configSet[descSet] = FALSE;
            instObj->clutSet[descSet] = FALSE;

        }
        instObj->fdmCbParams.fdmCbFxn = NULL;
        instObj->fdmCbParams.fdmErrCbFxn = NULL;
        instObj->fdmCbParams.errList = NULL;
        instObj->fdmCbParams.fdmData = NULL;

    }

    if (NULL != instObj)
    {
        Semaphore_post(instObj->instSem);
    }
    return (retVal);
}

/**
 *  VpsDdrv_grpxControl
 *  \brief GRPX driver control function. This function implements various
 *  IOCTLS. This function could be called from task context only.
 *  All IOCTLS are blocking and could not be called from ISR context unless
 *  otherwise specified for a particular IOCTL.
 *
 *  cmd                 Description                 cmdArgs        cmdStatusArgs
 *  ----------------------------------------------------------------------------
 *  FVID2_START         Starts the display. At      NULL                NULL
 *                      least minimum number of
 *                      buffers should be queued
 *                      before calling start IOCTL
 *
 *  FVID2_STOP          Stops the display           NULL                NULL
 *
 *  FVID2_SET_FORMAT    Sets the buffer format      const FVID2_Format* NULL
 *                      to be displayed
 *
 *  FVID2_GET_FORMAT    Gets the default buffer     FVID2_Format*       NULL
 *                      format or already set
 *                      format
 *
 *  IOCTL_VPS_CREATE_LAYOUT Creates and sets the    const               NULL
 *                      multi window format to      Vps_MultiWinParams*
 *                      start mosaic display
 *
 *  IOCTL_VPS_SET_GRPX_PARAMS   Set the GRPX params Vps_GrpxParamsList* NULL
 *
 *  IOCTL_VPS_GET_GRPX_PARAMS   Get the GRPX params Vps_GrpxParamsLit * NULL
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
static Int32 VpsDdrv_grpxControl(Fdrv_Handle handle,
                                 UInt32 cmd,
                                 Ptr cmdArgs,
                                 Ptr cmdStatusArgs)
{
    Int32                    retVal = FVID2_SOK;
    VpsDdrv_GrpxInstObj      *instObj;
    /*Sanity Check*/

    if (NULL == handle)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "BADARGS: NULL pinter.\n");
        retVal = FVID2_EBADARGS;
    }
    if (FVID2_SOK == retVal)
    {
        instObj = (VpsDdrv_GrpxInstObj *)handle;
        switch (cmd)
        {
            case FVID2_START:
                retVal = vpsDdrvGrpxStartIoctl(instObj, cmdArgs);
                break;

            case FVID2_STOP:
                retVal = vpsDdrvGrpxStopIoctl(instObj, cmdArgs);
                break;

            case FVID2_SET_FORMAT:
                retVal = vpsDdrvGrpxSetFmtIoctl(instObj, cmdArgs);
                break;

            case FVID2_GET_FORMAT:
                retVal = vpsDdrvGrpxGetFmtIoctl(instObj, cmdArgs);
                break;

            case IOCTL_VPS_CREATE_LAYOUT:
                retVal = vpsDdrvGrpxCreateLayoutIoctl(instObj,
                                                      cmdArgs,
                                                      cmdStatusArgs);
                break;

            case IOCTL_VPS_SET_GRPX_PARAMS:
                retVal = vpsDdrvGrpxSetParams(instObj, cmdArgs);
                break;

            case IOCTL_VPS_GET_GRPX_PARAMS:
                retVal = vpsDdrvGrpxGetParams(instObj, cmdArgs);
                break;
            default:
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                       "UNSUPPORTED_CMD: IOCTL not supported\n");
                retVal = FVID2_EUNSUPPORTED_CMD;
                break;
        }
    }
    return (retVal);
}



/**
 *  VpsDdrv_grpxQueue
 *  \brief GRPX driver queue buffer API. This submits the buffers
 *  to the driver for grpx display. When mosaic mode is set, then the frame list
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
static Int32 VpsDdrv_grpxQueue(Fdrv_Handle handle,
                               FVID2_FrameList *frameList,
                               UInt32 streamId)
{
    Int32                    retVal = FVID2_SOK;
    UInt32                   cookie;
    UInt32                   regCnt;
    VpsDdrv_GrpxQueueObj     *qObj = NULL;
    VpsDdrv_GrpxInstObj      *instObj;

    /* pointer sanity check */
    if ((NULL == handle) ||
            (NULL == frameList))
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        instObj = handle;
        /* Check framelist for error and NULL pointer check */
        retVal = FVID2_checkFrameList(frameList, VPSDDRV_GRPX_MAX_REGIONS);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                      "ERROR: Check frame list error\n");
        }
    }

    /* driver handle the Queue comand based on the operation mode
     * for NFB: Q the framelist into the Queue
     * for FB:  if Not start, put into the local variable frame list,
     *          otherwise call core driver
     * to update the descriptor and overlay memory immideiately*/
    if (FVID2_SOK == retVal)
    {
        if (FALSE == instObj->state.isFbMode)
        {
           /* Allocate a free queue object from the pool */
            cookie = Hwi_disable();
            qObj = VpsUtils_dequeue(instObj->freeGrpxQ);
            Hwi_restore(cookie);
            if (NULL == qObj)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                       "ALLOC: Q object allocation failed\n");
                retVal = FVID2_EALLOC;
            }

            if (FVID2_SOK == retVal)
            {
                /* Copy the frame list to the driver's copy */
                FVID2_copyFrameList(&qObj->frameList, frameList);
                for (regCnt = 0; regCnt < instObj->numRegions; regCnt++)
                {
                    GT_1trace(VpsDdrvGrpxTrace, GT_DEBUG,
                              "Q 0x%0.8x in inputQ\n",
                              qObj->frameList.frames[regCnt]->addr[0][0]);
                }
                /* Disable global interrupts */
                cookie = Hwi_disable();

                /* Add the queue object in driver's input Q */
                VpsUtils_queue(instObj->bmObj.inQHandle, &qObj->qElem, qObj);
                /* Restore global interrupts */
                Hwi_restore(cookie);
            }

        }
        else
        {
            if (NULL == instObj->bmObj.fbQObj)
            {
                cookie = Hwi_disable();
                instObj->bmObj.fbQObj = VpsUtils_dequeue(instObj->freeGrpxQ);
                Hwi_restore(cookie);
                if (NULL == instObj->bmObj.fbQObj)
                {
                    GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                           "ALLOC: Q object allocation failed\n");
                    retVal = FVID2_EALLOC;
                }
                else
                {
                    /* Add the queue object in driver's input Q */
                    /* Disable global interrupts */
                    cookie = Hwi_disable();

                    VpsUtils_queue(instObj->bmObj.inQHandle,
                                   &instObj->bmObj.fbQObj->qElem,
                                   instObj->bmObj.fbQObj);

                    /* Restore global interrupts */
                    Hwi_restore(cookie);


                }
            }

            if (FVID2_SOK == retVal)
            {
                FVID2_duplicateFrameList(&instObj->bmObj.fbQObj->frameList,
                                    frameList);
                if (TRUE == instObj->state.isStarted)
                {
                    retVal = VpsDdrvGrpxUpdateDesc(instObj,
                                           &instObj->bmObj.fbQObj->frameList);

                    if (FVID2_SOK == retVal)
                    {
                        GT_1trace(VpsDdrvGrpxTrace, GT_DEBUG,
                                    "Q 0x%0.8x in inputQ\n",
                                     instObj->bmObj.fbQObj->frameList.
                                            frames[0]->addr[0][0]);
                    }

                }
            }
        }
    }
    return (retVal);
}


/**
 *  VpsDdrv_grpxDequeue
 *  \brief Grpx driver dequeue buffer API. This returns the displayed buffers
 *  from the driver. When mosaic mode is set, then the frame list
 *  should contain space for storing that many number of frames pointers for each
 *  of the mosaic windows.
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
 *  This function could be called from ISR or task context and only valid if Driver
 *  is opened as non frame buffer mode.
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
static Int32 VpsDdrv_grpxDequeue(Fdrv_Handle handle,
                                 FVID2_FrameList *frameList,
                                 UInt32 streamId,
                                 UInt32 timeOut)
{
    Int32                    retVal = FVID2_SOK;
    UInt32                   cookie;
    UInt32                   regCnt;
    VpsDdrv_GrpxQueueObj     *qObj = NULL;
    VpsDdrv_GrpxInstObj      *instObj;

    /* pointer sanity check */
    if ((NULL == handle) ||
        (NULL == frameList))
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }


    if (FVID2_SOK == retVal)
    {
        instObj = (VpsDdrv_GrpxInstObj *)handle;

        if (VPS_GRPX_FRAME_BUFFER_MODE  == instObj->state.isFbMode)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "deQueue command is not valid when \
                    operating in Frame Buffer Mode\n");
            return FVID2_EUNSUPPORTED_CMD;
        }
        else
        {
            /* Check framelist for error and NULL pointer check */
            retVal = FVID2_checkDqFrameList(frameList,
                        VPSDDRV_GRPX_MAX_REGIONS);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace,GT_ERR,
                          "ERROR: Check frame list error\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {

        /* Disable global interrupts */
        cookie = Hwi_disable();

        /* Get a completed queue object from driver's output Q */
        qObj = VpsUtils_dequeue(instObj->bmObj.outQHandle);
        if (NULL == qObj)
        {
            /* When display is stopped, give back all the driver owned buffer
             * including the current programmed and the buffers in InQ */
            if (TRUE != instObj->state.isStarted)
            {
                /* Give the buffers in current state */
                qObj = vpsDdrvGrpxRemoveBufFromState(&instObj->bmObj, FALSE);
                if (NULL == qObj)
                {
                    /* At last give back the buffers in InQ as well */
                    qObj = VpsUtils_dequeue(instObj->bmObj.inQHandle);
                }

                /* Check if all these operation has failed */
                if (NULL == qObj)
                {
                    GT_0trace(VpsDdrvGrpxTrace, GT_DEBUG,
                            "NO_MORE_BUFFERS: No more buffers with driver\n");
                    retVal = FVID2_ENO_MORE_BUFFERS;
                }
            }
            else
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_DEBUG,
                                  "AGAIN: Out Q Empty. Try again\n");
                retVal = FVID2_EAGAIN;
            }
        }

        Hwi_restore(cookie);                /* Restore global interrupts */

        /* Copy the driver's frame list to application's frame list*/
        if (NULL != qObj)
        {
            FVID2_copyFrameList(frameList, &qObj->frameList);
            for (regCnt = 0; regCnt < frameList->numFrames; regCnt++)
                GT_1trace(VpsDdrvGrpxTrace, GT_DEBUG, "DQ 0x%0.8x in outputQ\n",
                               frameList->frames[regCnt]->addr[0][0]);
            /* Give back the queue object back to the free pool */
            cookie = Hwi_disable();
            VpsUtils_queue(instObj->freeGrpxQ, &qObj->qElem, qObj);
            Hwi_restore(cookie);
        }
    }

    return (retVal);
}

/**
 *  VpsDdrvGrpxUpdateDesc
 *  Update the buffer and runtime parameter for frame buffer mode only
 *  this function is call by the Queue command
 */

static Int32 VpsDdrvGrpxUpdateDesc(VpsDdrv_GrpxInstObj * instObj,
                                   FVID2_FrameList *frameList)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  set;
    UInt32                  frmConsumed = 0;
    UInt32                  setToDummy = 0;
    UInt32                  recreateConfig;
    Vps_GrpxRtList          *rtList;
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    GT_assert(VpsDdrvGrpxTrace, (NULL != frameList));
    GT_assert(VpsDdrvGrpxTrace, (TRUE == instObj->state.isFbMode));

    setToDummy = instObj->bmObj.expectedSet == 1 ? 0: 1u;

    for (set = 0; set < DC_NUM_DESC_SET; set++)
    {

        /*update the RT parameters to core*/
        retVal = vpsDdrvGrpxUpdateRtParams(instObj,
                                           frameList,
                                           set);

        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Update RT parameters failed.\n");
            return retVal;
        }

        /* Program the next buffer if required */
        if (TRUE == instObj->state.isMultiRegion)
        {
            GT_assert(VpsDdrvGrpxTrace,
                         (NULL != instObj->coreOps->updateMultiDesc));
            retVal = instObj->coreOps->updateMultiDesc(
                                instObj->coreHandle,
                                0u,
                                &instObj->descParams.coreDescMem[set],
                                frameList,
                                0,
                                0,
                                &frmConsumed);
        }
        else
        {
            GT_assert(VpsDdrvGrpxTrace,
                      (NULL != instObj->coreOps->updateDesc));
            retVal = instObj->coreOps->updateDesc(
                                    instObj->coreHandle,
                                    &instObj->descParams.coreDescMem[set],
                                    frameList->frames[0],
                                    0);
        }
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Update Desc failed.\n");
            return retVal;
        }
        /*we have CLUT descriptor, set the flag*/
        rtList = (Vps_GrpxRtList *)frameList->perListCfg;
        if ((NULL != rtList) && (NULL != rtList->clutPtr))
        {
            instObj->clutSet[set] = TRUE;
        }

        if (setToDummy != set)
        {
            if (TRUE == instObj->state.isStarted)
            {
                recreateConfig =
                      vpsDdrvGrpxCanReCreateConfigDesc(instObj, frameList);
                if (TRUE == recreateConfig)
                {
                    if (FALSE == instObj->configSet[set])
                    {
                        VpsHal_vpdmaCreateConfigDesc(
                            instObj->descParams.configDesc[set],
                            instObj->descParams.configDest,
                            VPSHAL_VPDMA_CPT_BLOCK_SET,
                            VPSHAL_VPDMA_CCT_INDIRECT,
                            instObj->descParams.configMemSize,
                            instObj->descParams.coreDescMem[set].coeffOvlyMem,
                            (Ptr)instObj->descParams.confgiMemOffset,
                            instObj->descParams.configMemSize);

                        instObj->configSet[set] = TRUE;
                    }
                }

            }
        }
    }

    return retVal;
}
static Int32 vpsDdrvGrpxFbUpdateBufIsr(UInt32 curSet,
                                       UInt32 timeStamp,
                                       Ptr arg)
{

    Int32                    retVal = FVID2_SOK;
    UInt32                   curFid, abCnt;
    UInt32                   nextSet;
    UInt8                    *tempPtr;
    VpsDdrv_GrpxInstObj      *instObj;

    GT_1trace(VpsDdrvGrpxTrace, GT_DEBUG,
                     "Getting update buffer ISR. Set: %d\n", curSet);

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != arg));
    GT_assert(VpsDdrvGrpxTrace, (curSet < DC_NUM_DESC_SET));


    instObj = (VpsDdrv_GrpxInstObj *) arg;

    if (FALSE == instObj->state.isFbMode)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                  "update FB buffer in the streaming mode\n");
        return (FVID2_EFAIL);
    }

    /*Figure out the next set*/
    nextSet = ( 0 == curSet) ? 1u: 0u;

    /* Figure out the current FID */
    curFid = (TRUE == instObj->bmObj.isProgressive) ? 0u: (curSet & 1u);


    /* Check if we are getting field ID's at proper sequence */
    if (instObj->bmObj.fid != curFid)
    {
        GT_2trace(VpsDdrvGrpxTrace, GT_ERR,
            "Expected %d field but getting %d field\n",
            instObj->bmObj.fid,
            curFid);
    }

    /* Check if we are getting interrupts in proper sequence */
    if (instObj->bmObj.expectedSet != curSet)
    {
        GT_2trace(VpsDdrvGrpxTrace, GT_ERR,
            "Graphics:Expected %d set but getting %d set\n",
            instObj->bmObj.expectedSet,
            curSet);
    }

    /* replace abort descriptor with dummy descriptor when receiving the first SI*/
    if (0u == instObj->displayCount)
    {
        tempPtr = instObj->descParams.abortDesc[0];
        for (abCnt = 0u; abCnt < instObj->descParams.numAbortCh; abCnt++)
        {
            VpsHal_vpdmaCreateDummyDesc(tempPtr);
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
    }


    /*first is to replace the clut data descriptor or configuration
        descriptor to dummy if they are not dummy*/
    if (TRUE == instObj->configSet[nextSet])
    {
        VpsHal_vpdmaCreateDummyDesc(
                instObj->descParams.configDesc[nextSet]);
        instObj->configSet[nextSet] = FALSE;
    }

    if (TRUE == instObj->clutSet[nextSet])
    {
        VpsHal_vpdmaCreateDummyDesc(
                    instObj->descParams.coreDescMem[nextSet].inDataDesc[0]);
        VpsHal_vpdmaCreateDummyDesc((Ptr)
           (((UInt32)instObj->descParams.coreDescMem[nextSet].inDataDesc[0])
                + VPSHAL_VPDMA_CONFIG_DESC_SIZE));
        instObj->clutSet[nextSet] = FALSE;

    }
    /* Intimate the application that a buffer is ready to be dequeued */
    if (instObj->bmObj.expectedSet >= (DC_NUM_DESC_SET - 1u))
    {
        instObj->bmObj.expectedSet = 0u;
    }
    else
    {
        instObj->bmObj.expectedSet = 1u;
    }

    if (NULL != instObj->fdmCbParams.fdmCbFxn)
    {
        instObj->fdmCbParams.fdmCbFxn(instObj->fdmCbParams.fdmData, NULL);
    }
    instObj->displayCount++;
    return retVal;

}

/**
 *  vpsDdrvGrpxUpdateBufIsr
 *  DLM/DCTRL callback ISR which programs the descriptor set depending on the
 *  buffer availability.
 */
static Int32 vpsDdrvGrpxUpdateBufIsr(UInt32 curSet, UInt32 timeStamp, Ptr arg)
{
    Int32                    retVal = FVID2_SOK;
    UInt32                   frmCnt, abCnt;
    UInt32                   curFid, nextFid;
    UInt32                   nextSet, prevSet, dqSet = DC_NUM_DESC_SET;
    UInt32                   frmConsumed =0u;
    UInt32                   recreateConfig;
    UInt32                   clutToDummy = FALSE;
    UInt32                   configToDummy = FALSE;
    UInt8                    *tempPtr;
    VpsDdrv_GrpxQueueObj     *qObj;
    VpsDdrv_GrpxInstObj      *instObj;
    FVID2_FrameList          *frameList;
    Vps_GrpxRtList           *rtList;

    GT_1trace(VpsDdrvGrpxTrace, GT_DEBUG,
                     "Getting update buffer ISR. Set: %d\n", curSet);

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != arg));
    GT_assert(VpsDdrvGrpxTrace, (curSet < DC_NUM_DESC_SET));



    instObj = (VpsDdrv_GrpxInstObj *) arg;

    if (TRUE == instObj->state.isFbMode)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "update buffer ISR is framebuffer mode\n");
        return FVID2_EFAIL;
    }
    /*Figure out the next set*/
    nextSet = ( 0 == curSet) ? 1u: 0u;
    /* Figure out the previous set */
    prevSet = (0 == curSet) ? 1u : 0u;

    /* Figure out the current FID */
    curFid = (TRUE == instObj->bmObj.isProgressive) ? 0u: (curSet & 1u);
    nextFid = (TRUE == instObj->bmObj.isProgressive) ? 0u: !curFid;

    /* Check if we are getting field ID's at proper sequence */
    if (instObj->bmObj.fid != curFid)
    {
        GT_2trace(VpsDdrvGrpxTrace, GT_ERR,
            "Expected %d field but getting %d field\n",
            instObj->bmObj.fid,
            curFid);
    }

    /* Check if we are getting interrupts in proper sequence */
    if (instObj->bmObj.expectedSet != curSet)
    {
        GT_2trace(VpsDdrvGrpxTrace, GT_ERR,
            "Graphics:Expected %d set but getting %d set\n",
            instObj->bmObj.expectedSet,
            curSet);
    }


    /* replace abort descriptor with dummy descriptor when receiving the first SI*/
    if (0u == instObj->displayCount)
    {
        tempPtr = instObj->descParams.abortDesc[0];
        for (abCnt = 0u; abCnt < instObj->descParams.numAbortCh; abCnt++)
        {
            VpsHal_vpdmaCreateDummyDesc(tempPtr);
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
    }

    if (TRUE == instObj->bmObj.isProgressive)
    {
        dqSet = prevSet;
        instObj->displayCount++;
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
            instObj->displayCount++;
        }
    }

    /* Check if we can dequeue a buffer */
    if (DC_NUM_DESC_SET != dqSet)
    {
        /* Check if we can dequeue the previous set buffer */
        if (TRUE == vpsDdrvGrpxCanCurBufBeDequeued(&instObj->bmObj, dqSet))
        {
            GT_assert(VpsDdrvGrpxTrace,
                (NULL != instObj->bmObj.prgmedBuf[dqSet]));

            /* Set the timestamp */
            frameList = &instObj->bmObj.prgmedBuf[dqSet]->frameList;
            for (frmCnt = 0u; frmCnt < frameList->numFrames; frmCnt++)
            {
                frameList->frames[frmCnt]->timeStamp = timeStamp;
            }

            /* Add the completed frames in driver's done queue */
            VpsUtils_queue(
                instObj->bmObj.outQHandle,
                &instObj->bmObj.prgmedBuf[dqSet]->qElem,
                instObj->bmObj.prgmedBuf[dqSet]);
                instObj->bmObj.prgmedBuf[dqSet] = NULL;

            /* Intimate the application that a buffer is ready to be dequeued */
            if (NULL != instObj->fdmCbParams.fdmCbFxn)
            {
                instObj->fdmCbParams.fdmCbFxn(instObj->fdmCbParams.fdmData,
                                              NULL);
            }
        }
        else if (TRUE == instObj->periodicCallbackEnable)
        {
            /* notify application callback */
            if (NULL != instObj->fdmCbParams.fdmCbFxn)
            {
                instObj->fdmCbParams.fdmCbFxn(instObj->fdmCbParams.fdmData,
                                              NULL);
            }

        }

    }

    configToDummy = instObj->configSet[nextSet];
    clutToDummy = instObj->clutSet[nextSet];

    /* Figure out what to program next */
    qObj = vpsDdrvGrpxGetNextBufState(&instObj->bmObj,
                                      curSet,
                                      nextSet,
                                      curFid);

    if (NULL != qObj)
    {
        /*check to make sure whether driver need to create config descriptor*/
        recreateConfig =
              vpsDdrvGrpxCanReCreateConfigDesc(instObj, &qObj->frameList);

        if (TRUE == recreateConfig)

        {
            configToDummy = FALSE;
            if (FALSE == instObj->configSet[nextSet])
            {
                VpsHal_vpdmaCreateConfigDesc(
                    instObj->descParams.configDesc[nextSet],
                    instObj->descParams.configDest,
                    VPSHAL_VPDMA_CPT_BLOCK_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    instObj->descParams.configMemSize,
                    instObj->descParams.coreDescMem[nextSet].coeffOvlyMem,
                    (Ptr)instObj->descParams.confgiMemOffset,
                    instObj->descParams.configMemSize);

                instObj->configSet[nextSet] = TRUE;
            }
        }
        /*CLUT ptr is avaible, so need to update(this is done in the core
         * driver) but somehow the duplicate Q will be update in the core as
         * well, driver need replace it with dummy
         */
        rtList = (Vps_GrpxRtList *)qObj->frameList.perListCfg;
        if ((NULL != rtList) && (NULL != rtList->clutPtr))
        {
            instObj->clutSet[nextSet] = TRUE;
            clutToDummy = FALSE;
            if (TRUE == vpsDdrvGrpxIsDuplicateQ(instObj))
            {
                clutToDummy = TRUE;
            }
        }
    }

    if (NULL != qObj)
    {
        /*update the RT parameters to core*/
        retVal = vpsDdrvGrpxUpdateRtParams(instObj,
                                           &qObj->frameList,
                                           nextSet);

        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Update RT parameters failed.\n");
        }

        /* Program the next buffer if required */
        if (TRUE == instObj->state.isMultiRegion)
        {

            GT_assert(VpsDdrvGrpxTrace,
                         (NULL != instObj->coreOps->updateMultiDesc));
            retVal = instObj->coreOps->updateMultiDesc(
                                instObj->coreHandle,
                                0u,
                                &instObj->descParams.coreDescMem[nextSet],
                                &qObj->frameList,
                                nextFid,
                                0,
                                &frmConsumed);
        }
        else
        {

            GT_assert(VpsDdrvGrpxTrace,
                        (NULL != instObj->coreOps->updateDesc));
            retVal = instObj->coreOps->updateDesc(
                                    instObj->coreHandle,
                                    &instObj->descParams.coreDescMem[nextSet],
                                    qObj->frameList.frames[0],
                                    nextFid);
        }
    }

    /* no update, use the previous buffer, so we need to replace the normal
     * configuration with dummy*/
    if (TRUE == configToDummy)
    {
        VpsHal_vpdmaCreateDummyDesc(instObj->descParams.configDesc[nextSet]);
        instObj->configSet[nextSet] = FALSE;
    }
    /*no update, remove the CLUT inbound descriptor if it is available*/
    if (TRUE == clutToDummy)
    {
        VpsHal_vpdmaCreateDummyDesc(
                 instObj->descParams.coreDescMem[nextSet].inDataDesc[0]);
        VpsHal_vpdmaCreateDummyDesc((Ptr)
              (((UInt32)instObj->descParams.coreDescMem[nextSet].inDataDesc[0])
                +  VPSHAL_VPDMA_CONFIG_DESC_SIZE));
        instObj->clutSet[nextSet] = FALSE;
    }

    /* Figure out the next set */
    if (curSet >= (DC_NUM_DESC_SET - 1u))
    {
        instObj->bmObj.expectedSet = 0u;
    }
    else
    {
        instObj->bmObj.expectedSet = curSet + 1u;
    }

    /* Figure out the next FID */
    if (TRUE != instObj->bmObj.isProgressive)
    {
        instObj->bmObj.fid ^= 1u;
    }

    return (retVal);
}



/**
 *  vpsDdrvGrpxStartCb
 *  DCTRL callback function to indicate that the display operation has
 *  started. This is not used currently.
 */
static Int32 vpsDdrvGrpxStartCb(Ptr arg)
{
    //Todo: This should be implemented for dynamic switching of mux
    GT_0trace(VpsDdrvGrpxTrace, GT_DEBUG, "vpsDdrvGrpxStartCb\n");
    return (FVID2_SOK);
}


/**
 *  vpsDdrvGrpxStopCb
 *  DCTRL callback function to indicate that the display operation has
 *  stopped. This is not used currently.
 */
static Int32 vpsDdrvGrpxStopCb(Ptr arg)
{
    //Todo: This should be implemented for dynamic switching of mux
    GT_0trace(VpsDdrvGrpxTrace, GT_DEBUG, "vpsDdrvGrpxStopCb\n");
    return (FVID2_SOK);
}



/**
 *  vpsDdrvGrpxStartingCb
 *  DCTRL callback function to indicate that the display operation is
 *  going to start. Through this callback the DCTRL will inform the driver
 *  to which list the driver is added in DLM and accordingly provide the frame
 *  start event to which the VPDMA clients should be programmed with.
 *  The display driver in turn uses the core layer to program the frame start
 *  event.

 */
static Int32 vpsDdrvGrpxStartingCb(Ptr arg, VpsHal_VpdmaFSEvent fsEvent)
{
    Int32                    retVal = FVID2_SOK;
    VpsDdrv_GrpxInstObj      *instObj;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != arg));

    instObj = (VpsDdrv_GrpxInstObj *) arg;
    if (NULL != instObj->coreOps->setFsEvent)
    {
        /* Set the frame start events for core clients */
        retVal = instObj->coreOps->setFsEvent(
                            instObj->coreHandle, 0u,fsEvent,NULL);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                      "ERROR: Set frame start event failed\n");
        }
    }

    return (retVal);
}


/**
 *  vpsDdrvGrpxStartIoctl
 *  Implements the start display IOCTL.
 *  Return FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvGrpxStartIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs)
{
    Int32                    retVal = FVID2_SOK;
    UInt32                   descSet;
    Uint32                   regCnt;
    UInt32                   numElemInQ;
    UInt32                   fid;
    UInt32                   frmConsumed = 0u;
    UInt32                   coreConfigFlag = FALSE;
    VpsDdrv_GrpxQueueObj     *qObj = NULL;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    /*acquire the semaphore*/
    Semaphore_pend(instObj->instSem, VPSDDRV_GRPX_SEM_TIMEOUT);

    instObj->displayCount = 0u;
    if (FVID2_SOK == retVal)
    {
        if (TRUE == instObj->state.isStarted)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                          "ERROR: handle already started.\n");
            retVal = FVID2_EFAIL;
        }
        else
        {
            /*configure descriptor memory */
            retVal = vpsDdrvGrpxConfigDctrl(instObj);
        }
    }

    if (FVID2_SOK == retVal)
    {
        coreConfigFlag = TRUE;

        /* Check if the number of elements in the queue is sufficient to
         * program the initial set of descriptors */
        numElemInQ = VpsUtils_getNumQElem(instObj->bmObj.inQHandle);
        /* Each set requires a buffer */
        if (numElemInQ < VPSDDRV_GRPX_MIN_BUFFERS)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                       "ERROR: Insufficient buffers queued\n");
            retVal = FVID2_EFAIL;
        }
    }
    else
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                      "ERROR: Core Config memory failed.\n");
    }

    if (FVID2_SOK == retVal)
    {
        qObj = VpsUtils_dequeue(instObj->bmObj.inQHandle);
        if (TRUE == instObj->state.isFbMode)
        {
            retVal = VpsDdrvGrpxUpdateDesc(instObj,
                                &instObj->bmObj.fbQObj->frameList);

            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                                "ERROR: Update Desc failed\n");

            }
        }
        else
        {
            descSet = 0;
            GT_assert(VpsDdrvGrpxTrace, (NULL != qObj));

            /* processed different for progressive and interlaced*/
            if(TRUE == instObj->bmObj.isProgressive)
            {
                instObj->bmObj.prgmedBuf[0u] = qObj;
            }
            else
            {
                instObj->bmObj.sPrgmedBuf = qObj;
            }
            /* Program the descriptor */
            if (NULL !=qObj)
            {
                fid = instObj->bmObj.isProgressive?0:(descSet &1u);
                if (TRUE == instObj->state.isMultiRegion)
                {
                    GT_assert(VpsDdrvGrpxTrace,
                                 (NULL != instObj->coreOps->updateMultiDesc));
                    retVal = instObj->coreOps->updateMultiDesc(
                                    instObj->coreHandle,
                                    0u,
                                    &instObj->descParams.coreDescMem[descSet],
                                    &qObj->frameList,
                                    fid,
                                    0,
                                    &frmConsumed);
                }
                else
                {
                    GT_assert(VpsDdrvGrpxTrace,
                                  (NULL != instObj->coreOps->updateDesc));
                    retVal = instObj->coreOps->updateDesc(
                             instObj->coreHandle,
                             &instObj->descParams.coreDescMem[descSet],
                             qObj->frameList.frames[0],
                             fid);
                }
            }
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                                "ERROR: Update buffer failed\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        instObj->bmObj.expectedSet = 0u;
        instObj->bmObj.fid = 0u;
        instObj->state.isStarted = TRUE;
        /* Start display */

        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet++)
        {
            for (regCnt = 0u; regCnt < VPSDDRV_GRPX_MAX_REGIONS; regCnt++)
            {
                instObj->descDirty[descSet].configDescSet[regCnt] = FALSE;
                instObj->descDirty[descSet].dataDescDirty[regCnt] = FALSE;
            }

        }
        retVal = Dc_startClient(instObj->dcHandle);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace,GT_ERR,
                "ERROR: Start client failed\n");
        }
    }

    if (FVID2_SOK != retVal)
    {
        instObj->state.isStarted = FALSE;
        if (TRUE == coreConfigFlag)
        {
            /* Deallocate memory if error occurs */
            vpsDdrvGrpxDeConfigDctrl(instObj);
            coreConfigFlag = FALSE;
        }
    }
    /*release semaphore*/
    Semaphore_post(instObj->instSem);
    return (retVal);
}



/**
 *  vpsDdrvGrpxStopIoctl
 *  Implements the stop display IOCTL. When the display is stopped, this
 *  function queues back the on going frame back to the input request queue
 *  so that when the display starts again, it starts displaying from the current
 *  un-displayed frame.
 *  Return FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvGrpxStopIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs)
{
    Int32                    retVal = FVID2_SOK;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));

    if (FALSE == instObj->state.isStarted)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "ERROR: Handled already stopped\n");
        retVal = FVID2_EFAIL;

    }
    /*acquire semaphore*/
    Semaphore_pend(instObj->instSem, VPSDDRV_GRPX_SEM_TIMEOUT);
    if (FVID2_SOK == retVal)
    {
        retVal = vpsDdrvGrpxStop(instObj);
    }
    /*release semaphore*/
    Semaphore_post(instObj->instSem);

    return (retVal);
}


/**
 *  vpsDdrvGrpxSetFmtIoctl
 *  Sets the FVID2 format by calling the core function.
 *  Return FVID2_SOK on success else returns error value.
 */
static Int32 vpsDdrvGrpxSetFmtIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs)
{
    Int32                    retVal = FVID2_SOK;
    FVID2_Format             *fmt;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));

    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the VENC information from display controller */
        retVal = Dc_getPathInfo(instObj->dcHandle, &instObj->dcPathInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "DCtrl get mode info failed\n");
        }

    }
    if (FVID2_SOK == retVal)
    {
        fmt = (FVID2_Format *) cmdArgs;
        retVal = vpsDdrvGrpxSetCore(instObj, fmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "GRPX core Set failed\n");

        }
        else
        {
            /*reset the status, really need this????*/
            instObj->state.isMultiRegion = FALSE;
        }
    }

    return (retVal);
}


/**
 *  vpsDdrvGrpxGetFmtIoctl
 *  Gets the FVID2 format by calling the core function.
 *  Return FVID2_SOK on success else returns error value.
 */
static Int32 vpsDdrvGrpxGetFmtIoctl(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs)
{
    Int32                    retVal = FVID2_SOK;
    FVID2_Format             *fmt;
    Vcore_Format             coreFmt;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    if (FVID2_SOK == retVal)
    {
        fmt = (FVID2_Format *) cmdArgs;

        /* Get format */
        GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->getFormat));
        retVal = instObj->coreOps->getFormat(instObj->coreHandle,
                                             0u,
                                             &coreFmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "ERROR: Get format failed\n");
        }
        else
        {
            VpsUtils_memcpy(fmt, &coreFmt.fmt, sizeof(FVID2_Format));
        }
    }

    return (retVal);
}

/**
 *  vpsDdrvGrpxCreateLayoutIoctl
 *  \brief Creates the mosaic layout, allocates the required descriptor
 *  memories, program the and return the layout ID.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
static Int32 vpsDdrvGrpxCreateLayoutIoctl(VpsDdrv_GrpxInstObj *instObj,
                                          Ptr cmdArgs,
                                          Ptr cmdStatusArgs)
{
    Int32               retVal = FVID2_SOK;
    Vps_MultiWinParams *multiWinPrms;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        multiWinPrms = (Vps_MultiWinParams *) cmdArgs;
        if (NULL == instObj->coreOps->createLayout)
        {
            retVal = FVID2_EUNSUPPORTED_OPS;
        }

        if ((VPSDDRV_GRPX_MAX_REGIONS < multiWinPrms->numWindows) ||
            ( 2 > multiWinPrms->numWindows))
        {
            retVal = FVID2_EOUT_OF_RANGE;
        }
    }

    if (FVID2_SOK == retVal)
    {

        /* Set mosaic format */
        retVal = instObj->coreOps->createLayout(
                instObj->coreHandle,
                0u,
                multiWinPrms,
                NULL);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                   "ERROR: Set mosaic format failed\n");
        }
        else
        {
            instObj->state.isMultiRegion = TRUE;
            instObj->numRegions = multiWinPrms->numWindows;
        }
    }

    return (retVal);
}

/**
 *  vpsDdrvGrpxSetParams
 *  Set the params for the graphics
 *  Return FVID2_SOK on success else return error value
 */

static Int32 vpsDdrvGrpxSetParams(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs)
{
    Int32                    retVal = FVID2_SOK;
    Int32                    descSet;
    Vps_GrpxParamsList       *params;

    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));

    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "NULL pointer\n");
        return (FVID2_EBADARGS);
    }


    params = (Vps_GrpxParamsList *)cmdArgs;
    /*parameters checking*/
    if (VPSDDRV_GRPX_MAX_REGIONS < params->numRegions)
    {
        GT_2trace(VpsDdrvGrpxTrace, GT_ERR,
                "Num Region %d is bigger than supported(%d)\n",
                params->numRegions, VPSDDRV_GRPX_MAX_REGIONS);
        return (FVID2_EOUT_OF_RANGE);
    }

    if (0u == params->numRegions)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "Num Region can not be zero\n");
        return (FVID2_EINVALID_PARAMS);
    }
    else if (1u != params->numRegions)
    {

        if (params->numRegions != instObj->numRegions)
        {
            GT_2trace(VpsDdrvGrpxTrace, GT_ERR,
                "number Regions %d not match %d regions in Mult info\n",
                params->numRegions, instObj->numRegions);

            return (FVID2_EINVALID_PARAMS);
        }
    }
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->setParams));
    retVal = instObj->coreOps->setParams(instObj->coreHandle,0u,cmdArgs);
    if ((FVID2_SOK == retVal) && (NULL != params->clutPtr))
    {
        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet++)
            instObj->clutSet[descSet] = TRUE;
    }

    if(FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "Core Set parameters failed.\n");
    }
    else
    {
        /*This is in the case, application restart
            after multiple region case*/
        if (params->numRegions > 1)
            instObj->state.isMultiRegion = TRUE;
        else
            instObj->state.isMultiRegion = FALSE;

        instObj->numRegions = params->numRegions;
    }
    return retVal;
}

/**
 *  vpsDdrvGrpxGetParams
 *  Get the params for the graphics
 *  Return FVID2_SOK on success else return error value
 */

static Int32 vpsDdrvGrpxGetParams(VpsDdrv_GrpxInstObj *instObj, Ptr cmdArgs)
{
    Int32                    retVal = FVID2_SOK;

    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));

    if (NULL == cmdArgs)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "NULL pointer\n");
        return (FVID2_EBADARGS);
    }


    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->getParams));

    retVal = instObj->coreOps->getParams(instObj->coreHandle,0u,cmdArgs);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "Core get parameters failed\n");
    }
    return retVal;

}

/**
 *  vpsDdrvGrpxUpdateRtParams
 *  Update the runtime params for the graphics
 *  Return FVID2_SOK on success else return error value
 */

static Int32  vpsDdrvGrpxUpdateRtParams(VpsDdrv_GrpxInstObj *instObj,
                                        FVID2_FrameList   *frmList,
                                        UInt32  descSet)
{

    Int32               retVal = FVID2_SOK;
    UInt32              descCnt;
    UInt32              regCnt;
    Vcore_GrpxRtParams   coreRtParams;
    Vcore_DescMem       *descMem;
    Vps_GrpxRtList      *rtList;
    Vps_GrpxRtParams    *rtParams;
    /* Check for NULL pointers */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    GT_assert(VpsDdrvGrpxTrace, (descSet < DC_NUM_DESC_SET));
    GT_assert(VpsDdrvGrpxTrace, (NULL != frmList));
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps));
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->updateRtMem));

    /*assemble the structure for the core*/

    for (regCnt = 0u; regCnt < frmList->numFrames; regCnt++)
    {
        rtParams = (Vps_GrpxRtParams *)frmList->frames[regCnt]->perFrameCfg;
        /* perlist only need send out with the first region,
           the rest will be set to NULL*/
        if (regCnt == 0u)
        {
            rtList = frmList->perListCfg;

        }
        else
        {
            rtList = NULL;
        }

        coreRtParams.rtList = rtList;
        coreRtParams.rtParams = rtParams;;

        descMem = &instObj->descParams.coreDescMem[descSet];
        /*rtParams or rtList is not NULL*/
        if ((NULL != rtParams) || (NULL != rtList))
        {

            retVal = instObj->coreOps->updateRtMem(
                            instObj->coreHandle,
                            descMem,
                            0u,
                            &coreRtParams,
                            regCnt);

            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Update GRPX runtime parameter failed\n");
            }

        }
        else if (instObj->descDirty[descSet].dataDescDirty[regCnt] == TRUE)
        {
            /*need regId of the coreRtParams to tell core which
             region to update*/
            coreRtParams.rtParams = NULL;
            coreRtParams.rtList = NULL;
            retVal = instObj->coreOps->updateRtMem(
                            instObj->coreHandle,
                            descMem,
                            0u,
                            NULL,
                            regCnt);

            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Update GRPX runtime parameter failed\n");
            }
        }

        if (FVID2_SOK == retVal)
        {
            if (NULL != rtParams)
            {
                for (descCnt = 0u; descCnt < DC_NUM_DESC_SET; descCnt++)
                {
                    instObj->descDirty[descCnt].dataDescDirty[regCnt] = TRUE;
                }
            }
            /*clear the flag for current set*/
            instObj->descDirty[descSet].dataDescDirty[regCnt] = FALSE;
        }
    }
    return (retVal);
}
