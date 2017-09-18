/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
/**
 *  \file vpsdrv_m2mSc.c
 *
 *  \brief Memory to Memory driver.
 *  This file implements the memory to memory driver for the
 *  two bypass paths BP0 and BP1 and secondary path SEC0 to the scalar5
 *  writeback path 2.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <string.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSc.h>
#include <ti/psp/vps/core/vpscore_secPath.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
#include <ti/psp/vps/core/vpscore_scWrbkPath.h>
#include <ti/psp/vps/drivers/m2m/vps_m2mInt.h>
#include <ti/psp/vps/drivers/m2m/vps_mlm.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSlice.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mSc.h>
#include <ti/psp/vps/drivers/m2m/src/vpsdrv_m2mScPriv.h>
#include <ti/psp/vps/core/vpscore_vipRes.h>
#include <ti/psp/vps/core/vpscore_vip.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */


/* Functions for checking and updating run time parameters                    */
static Int32 vpsMdrvScCheckParams(VpsMdrv_ScHandleObj *hObj,
                                  FVID2_ProcessList *procList);
static Int32 vpsMdrvScCheckPerListParams(VpsMdrv_ScHandleObj *hObj,
                                         FVID2_ProcessList *procList);
static Int32 vpsMdrvScCheckPerChParams(VpsMdrv_ScHandleObj *hObj,
                                       FVID2_ProcessList *procList);

/* Functions dealing with queueing and servicing of coeff requests            */
static Int32 vpsMdrvScSetCoeffQObj(VpsMdrv_ScHandleObj *hObj,
                                   Ptr descPtr,
                                   Ptr horzCoeffMemPtr,
                                   Ptr vertCoeffMemPtr,
                                   Ptr vertBilinearCoeffMemPtr);
static Int32 vpsMdrvScProgCoeff(VpsMdrv_ScHandleObj *hObj,
                                Vps_ScCoeffParams *params);
static Void vpsMdrvScProcCoeffReq(VpsMdrv_ScQueueObj *qObj,
                                  VpsMdrv_ScHandleObj *hObj,
                                  VpsMdrv_ScInstObj *instObj);
static Int32 vpsMdrvScModifyLazyLoadingCfg(VpsMdrv_ScHandleObj *hObj,
                                           UInt32 scalarId,
                                           UInt32 enableLazyLoadingModified,
                                           UInt32 enableFilterSelectModified);

/* Functions dealing with queueing and servicing of frame requests            */
static Int32 vpsMdrvScGetFreeDescSet(VpsMdrv_ScHandleObj *hObj,
                                     VpsMdrv_ScQueueObj *qObj);
static Void VpsMDrvScProcDoneAndQdFrameReq(VpsMdrv_ScQueueObj *qObj,
                                           VpsMdrv_ScHandleObj *hObj,
                                           VpsMdrv_ScInstObj *instObj);
static Void vpsMdrvScCreateCfgDescs(VpsMdrv_ScHandleObj *hObj);
static Int32 vpsMdrvScUpdateDescList(VpsMdrv_ScHandleObj *hObj,
                                     VpsMdrv_ScQueueObj *qObj);
static inline Void vpsMdrvScGetChObjIndex(VpsMdrv_ScHandleObj *hObj,
                                          FVID2_Frame *frame,
                                          UInt32 descSetInUse,
                                          UInt32 *index,
                                          UInt32 *descSetType);
static inline Int32 vpsMdrvScUpdateDescs(VpsMdrv_ScHandleObj *hObj,
                                         FVID2_Frame *inFrm,
                                         FVID2_Frame *outFrm,
                                         VpsMdrv_ScChObj *chOj,
                                         UInt32 descSetInUse,
                                         UInt32 descType,
                                         Vps_M2mScRtParams *rtParams);
static Int32 vpsMdrvScSetChPrms(VpsMdrv_ScHandleObj *hObj,
                                UInt32 chNum);

/* Misc functions                                                             */
static Void vpsMdrvScSplitDescMem(VpsMdrv_ScHandleObj *hObj);
static Void vpsMdrvScSplitSPadDescMem(VpsMdrv_ScHandleObj *hObj);
static Void vpsMdrvScSplitOvlyMem(VpsMdrv_ScHandleObj *hObj);

static Int32 VpsMdrv_scMlmCbFxn(Mlm_SubmitReqInfo *mlmReqInfo);
static Void vpsMdrvScReturnErrorProclist(VpsMdrv_ScHandleObj *hObj,
                                         VpsMdrv_ScQueueObj *qObj);


/*     SubFrame level procesing related functions                    */
void vpsMdrvScSubFrmUpdate(VpsMdrv_SubFrameChInst *slcData,
                           FVID2_Frame *inFrm,
                           FVID2_Frame *outFrm,
                           Vps_M2mScRtParams *rtParams,
                           Vps_M2mScChParams *chParam,
                           UInt32 hSlcNum);
static Int32 vpsMdrvScCopyHSubFrmInfo(VpsMdrv_ScHandleObj *hObj,
                                      Vps_SubFrameHorzSubFrmInfo *hSlcInfo);

/* ========================================================================== */
/*       Functions talking with input and output cores                        */
/* ========================================================================== */
static Int32 vpsMdrvScOpenCores(VpsMdrv_ScHandleObj *hObj);
static Int32 vpsMdrvScGetDescInfo(VpsMdrv_ScHandleObj *hObj);
static Int32 vpsMdrvScCloseCores(VpsMdrv_ScHandleObj *hObj);
static Int32 vpsMdrvScGetScFactorConfig(VpsMdrv_ScHandleObj *hObj,
                                        UInt32 chNum,
                                        UInt32 descSet);
static Int32 vpsMdrvScInitDescAndRegs(VpsMdrv_ScHandleObj *hObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/* None */

/**
 *  VpsMdrv_scCreate
 *  \brief Memory driver open function.
 *
 *  \param drvId            ID of driver to open
 *  \param instId       Instance of the driver to open
 *  \param createArgs       Driver specific arguments to be passed by
 *                          application.
 *  \param createStatusArgs Returns the status of the open call.
 *  \param fdmCbPrms        FVID2 call back function params.
 *  \return                 Return driver handle if open successful. Else
 *                          returns NULL pointer.
 */
Fdrv_Handle VpsMdrv_scCreate(UInt32 drvId,
                             UInt32 instId,
                             Ptr createArgs,
                             Ptr createStatusArgs,
                             const FVID2_DrvCbParams *fdmCbPrms)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  qCnt;
    UInt32                  retStage = 99;
    VpsMdrv_ScHandleObj    *hObj = NULL;
    VpsMdrv_ScInstObj      *instObj = NULL;
    Semaphore_Params        semParams;
    VpsMdrv_ScQueueObj     *tmpQObj;
    Vps_ScCoeffParams       coeffParams;
    Vps_M2mScCreateParams  *params = (Vps_M2mScCreateParams *) createArgs;
    Vps_M2mScCreateStatus  *status = (Vps_M2mScCreateStatus *) createStatusArgs;

    /* Check for the valid params */
    if ((NULL == fdmCbPrms) ||
        (NULL == createArgs) ||
        (NULL == fdmCbPrms->fdmErrCbFxn) ||
        (NULL == fdmCbPrms->errList))
    {
        retVal = FVID2_EBADARGS;
        GT_0trace(VpsMdrvScTrace, GT_ERR, "BADARGS: NULL pointer\n");
    }

    /* Check for the valid instance number */
    if (FVID2_SOK == retVal)
    {
        if ((drvId != FVID2_VPS_M2M_SC_DRV) || (instId >= VPS_M2M_SC_INST_MAX))
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EINVALID_PARAMS: Invalid Driver Name\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    /* Check for the valid channel numbers */
    if ((FVID2_SOK == retVal) &&
        ((params->numChannels > VPS_M2M_SC_MAX_CH)  ||
         (params->numChannels <= 0)))
    {
        retVal = FVID2_EINVALID_PARAMS;
        GT_1trace(VpsMdrvScTrace, GT_ERR,
            "EINVALID_PARAMS: More Channels than max supported "
            "channels %d\n", VPS_M2M_SC_MAX_CH);
    }

    if ((FVID2_SOK == retVal) &&
        (VPS_M2M_CONFIG_PER_CHANNEL != params->mode))
    {
        GT_0trace(VpsMdrvScTrace, GT_ERR,
            "Per handle mode of operation is not supported. "
            "Try with per channel mode!!\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    /* Get the instance pointer based on instance Id */
    if (FVID2_SOK == retVal)
    {
        instObj = vpsMdrvScGetInstObj(instId);
        if (NULL == instObj)
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EINVALID_PARAMS: Invalid Instance ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    /* Check for the maximum handles supported */
    if ((FVID2_SOK == retVal) &&
        (VPS_M2M_SC_MAX_HANDLE == instObj->openCnt))
    {
        retVal = FVID2_EALLOC;
        GT_1trace(VpsMdrvScTrace, GT_ERR,
            "EINVALID_PARAMS: More handles than max supported handles %d\n",
            VPS_M2M_SC_MAX_HANDLE);
    }

    if (NULL != instObj)
    {
        /* Protect the multiple open calls while driver is opening */
        Semaphore_pend(instObj->instSem, BIOS_WAIT_FOREVER);
    }

    while (FVID2_SOK == retVal)
    {
        /* Allocate handle object */
        hObj = vpsMdrvScAllocHandleObj();
        if (NULL == hObj)
        {
            retVal = FVID2_EALLOC;
            retStage = 1;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Handle allocation failed\n");
            break;
        }

        /* Initialized the fields of the handle object */
        hObj->instObj = vpsMdrvScGetInstObj(instId);
        hObj->fdmCbPrms.fdmCbFxn = fdmCbPrms->fdmCbFxn;
        hObj->fdmCbPrms.fdmData = fdmCbPrms->fdmData;
        hObj->fdmCbPrms.fdmErrCbFxn = fdmCbPrms->fdmErrCbFxn;
        hObj->fdmCbPrms.errList = fdmCbPrms->errList;

        /* Get the channe objects for each channel and assign it to the
         * handle object */
        retVal = vpsMdrvScAllocChObjs(hObj, params->numChannels);
        if (FVID2_SOK != retVal)
        {
            retVal = FVID2_EALLOC;
            retStage = 2;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Allocation of channel object failed\n");
            break;
        }

        /* Allocate configuration parameters and copy the application
         * configuration to the driver allocated configuration structure */
        retVal = vpsMdrvScUpdatePrms(hObj, params);
        if (FVID2_SOK != retVal)
        {
            retStage = 3;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Param structure allocation failed\n");
            break;
        }

        /* open the core handles and set the required params in cores */
        retVal = vpsMdrvScOpenCores(hObj);
        if  (FVID2_SOK != retVal)
        {
            retStage = 4;
            GT_0trace(VpsMdrvScTrace, GT_ERR, "EFAIL: Core Opening failed\n");
            break;
        }

        /* Get the information about the number of data descs,
         * config descriptors, overlay memory and coeff memory sizes */
        retVal = vpsMdrvScGetDescInfo(hObj);
        if (FVID2_SOK != retVal)
        {
            retStage = 5;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EFAIL: Getting the Descriptor information Failed\n");
            break;

        }

        /* Allocate the descriptors and memories depending upon the core
         * requirement */
        retVal = vpsMdrvScAllocDescMem(hObj);
        if (FVID2_SOK != retVal)
        {
            retStage = 6;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Descriptor or Memory allocation failed\n");
        }

        /* split the pointers to the data, config, reload descriptors for each
         * channel and each set of the channel */
        vpsMdrvScSplitDescMem(hObj);

        /* Ideally the slicing of the scratch pad descriptors should be done
         * runtime. Since the scatchpad descriptors are not tied to any
         * channel each channel can have differnt descriptor number requirement
         * depending upon the channel configuration.  But for this driver
         * decriptor number requirement is same for any configuration so
         * slicing the descriptor right away to save the runtime CPU
         * cycles. */
        vpsMdrvScSplitSPadDescMem(hObj);

        /* update the pointers to the overlay memories for each set of the
         * channel to the core. */
        vpsMdrvScSplitOvlyMem(hObj);

        /* Initialize the descriptors and memories according to the
         * configuration selected. In Queue only the address and runtime
         * paramters will be modified in descriptors and overlay memories */
        retVal = vpsMdrvScInitDescAndRegs(hObj);
        if (FVID2_SOK != retVal)
        {
            retVal = FVID2_EFAIL;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EFAIL: Initialization of the Descriptor failed\n");
            retStage = 7;
            break;
        }

        /* Create the configuration descriptors for shadow overlay memory */
        vpsMdrvScCreateCfgDescs(hObj);
        if (0 == instObj->openCnt)
        {
            /* Allocate the resources from the resource manager if the first
             * handle is opened */
            retVal = vpsMdrvScAllocResrc(instObj);
            if (FVID2_SOK != retVal)
            {
                retStage = 8;
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "EALLOC: Reservation of resources failed\n");
                break;
            }

            /* Enabled the proper Muxes */
            if ((VPS_M2M_INST_SEC0_SC5_WB2 == instObj->drvInstId) ||
                (VPS_M2M_INST_BP0_SC5_WB2 == instObj->drvInstId) ||
                (VPS_M2M_INST_BP1_SC5_WB2 == instObj->drvInstId) )
            {
                /* Enable the mux if the first handle is opened */
                VpsHal_vpsWb2MuxSrcSelect(instObj->wb2MuxSrc);
            }
            else if (VPS_M2M_INST_SEC0_SC3_VIP0 == instObj->drvInstId)
            {
                VpsHal_vpsSecMuxSelect(
                    VPSHAL_VPS_SEC_INST_0,
                    instObj->secMuxSrc);
            }
            else
            {
                VpsHal_vpsSecMuxSelect(
                    VPSHAL_VPS_SEC_INST_1,
                    instObj->secMuxSrc);
            }

            /* Register with the MLM if the first handle is opened */
            instObj->mlmInfo.reqCompleteCb = VpsMdrv_scMlmCbFxn;
            instObj->mlmHandle = Mlm_register(&instObj->mlmInfo, NULL);
            if (NULL == instObj->mlmHandle)
            {
                retVal = FVID2_EALLOC;
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "EALLOC: MLM Register failed\n");
                retStage = 9;
                break;
            }

            /* Allocate the Q objects and create the Q of the
             * free Q objects.  Incoming requests will use these
             * Q objects */
            if ((FVID2_SOK == retVal) &&
                (NULL != (tmpQObj = vpsMdrvScAllocQueObjs())))
            {
                instObj->qObjPool = tmpQObj;
                instObj->freeQ = VpsUtils_createQ();
                if (NULL != instObj->freeQ)
                {
                    for (qCnt = 0; qCnt < VPSMDRV_SC_MAX_Q_OBJS; qCnt++)
                    {
                        VpsUtils_queue(instObj->freeQ,
                        &instObj->qObjPool[qCnt].qElem,
                        &instObj->qObjPool[qCnt]);
                    }
                }
                else
                {
                    retVal = FVID2_EALLOC;
                    GT_0trace(VpsMdrvScTrace, GT_ERR,
                        "EALLOC: Queue creation Failed\n");
                    retStage = 10;
                    break;
                }
            }
            else
            {
                retVal = FVID2_EALLOC;
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "EALLOC: Free Queue Objects alloc failed\n");
                break;
            }
        }

        /* Create the request Q. */
        hObj->reqQ = VpsUtils_createQ();
        if (NULL == hObj->reqQ)
        {
            retVal = FVID2_EALLOC;
            retStage = 11;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Creation of Request Q failed\n");
            break;
        }

        /* Create DoneQ */
        hObj->doneQ = VpsUtils_createQ();
        if (NULL == hObj->doneQ)
        {
            retVal = FVID2_EALLOC;
            retStage = 12;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Creation of Semaphore failed\n");
            break;
        }

        /* Semaphore is to wait on the control commands till they get applied.
         * example coeff programming control command */
        Semaphore_Params_init(&semParams);
        hObj->ctrlSem = Semaphore_create(0u, &semParams, NULL);
        if (NULL == hObj->ctrlSem)
        {
            retVal = FVID2_EALLOC;
            retStage = 13;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Creation of Semaphore failed\n");
            break;
        }

        /* Program the coefficient descriptor and queue it to MLM
         * This is loaded first time when driver is created to
         * avoid hang condition when app doesnt load coff */
        if (0 == instObj->openCnt)
        {
            /* Update the coeff params */
            coeffParams.coeffPtr      = NULL;
            coeffParams.hScalingSet   = VPS_SC_US_SET;
            coeffParams.vScalingSet   = VPS_SC_US_SET;
            coeffParams.scalarId      = instId;

            retVal = vpsMdrvScProgCoeff(hObj, &coeffParams);
            if (FVID2_SOK == retVal)
            {
                /* Wait till coefficients are updated */
                Semaphore_pend(hObj->ctrlSem, BIOS_WAIT_FOREVER);
                VpsUtils_freeDescMem(
                    hObj->ioctlCoeffDesc,
                    VPSMDRV_SC_MAX_COEFF_DESC_MEM);
            }
        }

        /* Check Lazy Loading configuration in instance object and setup
         * required fields if it is enabled. */
        if (TRUE == hObj->instObj->enableLazyLoading)
        {
            retVal = vpsMdrvScModifyLazyLoadingCfg(
                         hObj,
                         VPS_M2M_SC_SCALAR_ID_DEFAULT, /* Default scalar ID */
                         TRUE,
                         TRUE);
            if (FVID2_SOK != retVal)
            {
                retStage = 14;
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "Failed to modify lazy loading config!\n");
                break;
            }
        }

        /* Everything passed so Ready to go */
        retStage = 0;
        break;
    }

    /* Clear everthing if failed else return with success */
    switch (retStage)
    {
        case 14:
        case 13:
            VpsUtils_deleteQ(hObj->doneQ);
        case 12:
            VpsUtils_deleteQ(hObj->reqQ);
        case 11:
            do
            {
                tmpQObj = VpsUtils_dequeue(instObj->freeQ);
            }while (tmpQObj != NULL);
            vpsMdrvScFreeQueObjs(instObj->qObjPool);
            VpsUtils_deleteQ(instObj->freeQ);
        case 10:
            Mlm_unRegister(instObj->mlmHandle);
        case 9:
            vpsMdrvScFreeResrc(instObj);
        case 8:
        case 7:
            vpsMdrvScFreeDescMem(hObj);
        case 6:
        case 5:
            vpsMdrvScCloseCores(hObj);
        case 4:
            vpsMdrvScFreeChPrms(hObj);
        case 3:
            vpsMdrvScFreeChObjs(hObj);
        case 2:
            vpsMdrvScFreeHandleObj(hObj);
            hObj = NULL;
        case 1:
            break;
        case 0:
            instObj->openCnt++;
            break;
        default:
            GT_0trace(VpsMdrvScTrace, GT_ERR, "ERROR\n");
    }

    if (NULL != instObj)
    {
        Semaphore_post(instObj->instSem);
    }

    /* Set the status information */
    status->retVal = retVal;
    status->maxHandles = VPS_M2M_SC_MAX_HANDLE;
    status->maxChannelsPerHandle = VPS_M2M_SC_MAX_CH;
    status->maxSameChPerRequest = VPS_M2M_SC_MAX_CH;
    status->maxReqInQueue = VPSMDRV_SC_MAX_Q_OBJS;

    return ((FVID2_Handle) hObj);
}

/**
 *  VpsMdrv_scDelete
 *  \brief Close function for the writeback Scalar mem to mem driver
 *
 * \param handle        Driver Handle
 *  \param deleteArgs   Driver specific arguments.  Currently not used
 *
 *  \return             Returns 0 if close successful. Else returns appropriate
 *                      error code.
 */
Int32 VpsMdrv_scDelete(FVID2_Handle handle, Ptr deleteArgs)
{
    Int32                   retVal = FVID2_SOK;
    VpsMdrv_ScHandleObj    *hObj = (VpsMdrv_ScHandleObj *)handle;
    VpsMdrv_ScInstObj      *instObj = hObj->instObj;
    VpsMdrv_ScQueueObj     *tmpQObj;

    if (NULL == handle)
    {
        retVal = FVID2_EBADARGS;
    }

    /* Semaphore to protect concurrency */
    Semaphore_pend(instObj->instSem, BIOS_WAIT_FOREVER);

    if (FVID2_SOK == retVal)
    {
        if (0 != hObj->numPendReq)
        {
            retVal = FVID2_EAGAIN;
            GT_0trace(VpsMdrvScTrace, GT_INFO,
                "AGAIN: Request Pending, Driver could not be closed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        Semaphore_delete(&hObj->ctrlSem);
        VpsUtils_deleteQ(hObj->doneQ);
        VpsUtils_deleteQ(hObj->reqQ);

        instObj->openCnt--;
        if (0 == instObj->openCnt)
        {
            /* Release Resources and disable Muxes */
            Mlm_unRegister(instObj->mlmHandle);
            vpsMdrvScFreeResrc(instObj);
            VpsHal_vpsWb2MuxSrcSelect(VPSHAL_VPS_WB2_MUX_SRC_DISABLE);
            do
            {
                tmpQObj = VpsUtils_dequeue(instObj->freeQ);
            } while (tmpQObj != NULL);
            vpsMdrvScFreeQueObjs(instObj->qObjPool);
            VpsUtils_deleteQ(instObj->freeQ);
        }
        vpsMdrvScFreeDescMem(hObj);
        vpsMdrvScCloseCores(hObj);
        vpsMdrvScFreeChPrms(hObj);
        vpsMdrvScFreeChObjs(hObj);
        hObj->instObj = NULL;
        hObj->fdmCbPrms.fdmCbFxn = NULL;
        hObj->fdmCbPrms.fdmData = NULL;
        hObj->fdmCbPrms.fdmErrCbFxn = NULL;
        hObj->fdmCbPrms.errList = NULL;
        vpsMdrvScFreeHandleObj(hObj);
    }

    Semaphore_post(instObj->instSem);

    return (retVal);
}

/**
 *  VpsMdrv_scProcessFrms
 *  \brief Process frames function for writeback scalar mem to mem driver.
 *
 *  \param handle       Handle returned to application while opening of the
 *                      driver.
 *  \param procList     Processlist containing frames to be processed
 *
 *  \return             Returns 0 on success else returns appropriate error
 *                      code.
 */
Int32 VpsMdrv_scProcessFrms(FVID2_Handle handle,
                            FVID2_ProcessList *procList)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    VpsMdrv_ScInstObj      *instObj;
    VpsMdrv_ScHandleObj    *hObj;
    VpsMdrv_ScQueueObj     *qObj;

    /* NULL element check */
    if (NULL == handle || NULL == procList)
    {
        retVal = FVID2_EBADARGS;
        GT_0trace(VpsMdrvScTrace, GT_ERR, "BADARGS: NULL pointer\n");
    }

    if (FVID2_SOK == retVal)
    {
        hObj = (VpsMdrv_ScHandleObj *)handle;
        /* Check the process list params for the run time configuration */
        retVal = vpsMdrvScCheckParams(hObj, procList);
    }

    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvScTrace, GT_ERR,
            "INVALID_PARAMS: Run time params not proper\n");
    }

    cookie  = Hwi_disable();
    if (FVID2_SOK == retVal)
    {
        instObj = hObj->instObj;

        /* Get the free queue object from the queue */
        qObj = VpsUtils_dequeue(instObj->freeQ);
        if (NULL == qObj)
        {
            retVal = FVID2_EAGAIN;
            GT_0trace(VpsMdrvScTrace, GT_INFO, "AGAIN: Input queueu full\n");
        }

        if (FVID2_SOK == retVal)
        {
            /* Copy the process list to the driver process list */
            FVID2_copyProcessList(&qObj->procList, procList);
            /* referencing the handle object in Q object to indentify the
             * request when it get de-queued from mlm */
            qObj->hObj = hObj;
            /* Mark queue object as Frames, so after dequeing from MLM it
             * needs to be returned to application */
            qObj->qObjType = VPSMDRV_QOBJ_TYPE_FRAMES;
            /* Get the free descriptor set for programming new buffer address
             * in process list  Descriptor set is updated in the queue object
               itself.*/
            retVal = vpsMdrvScGetFreeDescSet(hObj, qObj);
        }

        /* If the descriptor set is free for use update the descriptor set and
         * create a list of requests and submit it to MLM */
        if (FVID2_SOK == retVal)
        {
            /* Update the buffer address and runtime params in descriptors
            *  and overlay memories */
            retVal = vpsMdrvScUpdateDescList(hObj, qObj);
            if (FVID2_SOK != retVal)
            {
                /* If error updating the buffer address free the descriptor set
                 * in use, put the qobject back in the free queue
                 * and return with error code */
                hObj->isDescSetFree[qObj->descSetInUse] = FALSE;
                VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
            }

            if (FVID2_SOK == retVal)
            {
                /* Submit the request to MLM  */
                retVal = Mlm_submit(instObj->mlmHandle, &qObj->mlmReqInfo);
            }

            if (FVID2_SOK != retVal)
            {
                /* If error updating the buffer address free the descriptor set
                 * in use, put the qobject back in the free queue
                 * and return with error code */
                hObj->isDescSetFree[qObj->descSetInUse] = FALSE;
                VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
            }
        }
        /* If the descriptor set is not free.  Queue the requests
         * for till the descriptor sets get free. Again check
         * for the free descriptor set in the call back function. */
        else
        {
            VpsUtils_queue(hObj->reqQ, &qObj->qElem, qObj);
            retVal = FVID2_SOK;
        }
    }

    /* Increase the request count if request is queued */
    if (FVID2_SOK == retVal)
    {
        hObj->numPendReq++;
    }

    Hwi_restore(cookie);

    return (retVal);
}

/**
 *  VpsMdrv_scGetProcessedFrms
 *  \brief              Get processed frames function for writeback scalar
 *                      memory to memory memory driver
 *
 *  \param handle       Handle returned to the application while opening of the
 *                      driver.\
 *  \param procList  Process list in which the driver will copy the processed
 *                      frames.
 *  \param timeout      Not used.
 *
 *  \return             Returns 0 on success and appropriate error code
 *                      on failure.
 */
Int32 VpsMdrv_scGetProcessedFrms(FVID2_Handle handle,
                                     FVID2_ProcessList *procList,
                                     UInt32 timeout)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    VpsMdrv_ScHandleObj    *hObj;
    VpsMdrv_ScQueueObj     *qObj;
    VpsMdrv_ScInstObj      *instObj;

    /* NULL pointer check */
    if (NULL == handle || NULL == procList)
    {
        retVal = FVID2_EBADARGS;
        GT_0trace(VpsMdrvScTrace, GT_ERR, "BADARGS: NULL pointer\n");
    }

    cookie = Hwi_disable();

    if (FVID2_SOK == retVal)
    {
        hObj = (VpsMdrv_ScHandleObj *)handle;
        instObj = hObj->instObj;

        /* Dequeue the queue object from done queue done queue is per handle */
        qObj = VpsUtils_dequeue(hObj->doneQ);
        /* If no buffer to be dequeued return with Error. */
        if (NULL == qObj )
        {
            /* if buffers are given for processing but not yet processed
             * return with error codes as EGAIN */
            if (0 != hObj->numPendReq)
            {
                retVal =  (FVID2_EAGAIN);
                GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                    "AGAIN: Request under process\n");
            }
            /* If not buffer queueud return with FVID2_ENO_MORE_BUFFERS
             * error */
            else
            {
                retVal = (FVID2_ENO_MORE_BUFFERS);
                GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                    "AGAIN: No More buffers to process\n");
            }

        }
        else if (FVID2_SOK == retVal)
        {
            /* Copy the procList to the application given process List */
            FVID2_copyProcessList(procList, &qObj->procList);
            /* Return back the queue object to the free queue */
            VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
            /* decrement the count by 1 */
            hObj->numPendReq--;
        }
    }

    Hwi_restore(cookie);

    return (retVal);
}

/**
 *  VpsMdrv_scControl
 *  \brief          Control function for the writeback scalar memory
 *                  to memory driver
 *  \param handle   Handle returned to the application while opening of the
 *                  driver.
 *  \param cmd      Control command for the driver
 *
 *  \param cmdArgs  Arguments to be passed specific to the control cmd.
 *
 *  \param cmdStatusArgs   Status returned by driver. Currently not used.
 *
 *  \return         Returns 0 on succcess and appropriate error code on
 *                  failure.
 */
Int32 VpsMdrv_scControl(FVID2_Handle handle,
                        UInt32 cmd,
                        Ptr cmdArgs,
                        Ptr cmdStatusArgs)
{
    Int32                   retVal = FVID2_SOK;
    VpsMdrv_ScHandleObj    *hObj;

    /* NULL pointer check */
    if (NULL == handle)
    {
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (VpsMdrv_ScHandleObj *) handle;
        switch (cmd)
        {
            case IOCTL_VPS_SET_COEFFS:
            {
                if (FVID2_SOK == retVal && NULL != cmdArgs)
                {
                    Vps_ScCoeffParams *params = (Vps_ScCoeffParams *) cmdArgs;
                    /* Program the coefficient descriptor and queue it to MLM */
                    retVal = vpsMdrvScProgCoeff(hObj, params);
                    if (FVID2_SOK == retVal)
                    {
                        /* Wait till coefficients are updated */
                        Semaphore_pend(hObj->ctrlSem, BIOS_WAIT_FOREVER);
                        VpsUtils_freeDescMem(
                            hObj->ioctlCoeffDesc,
                            VPSMDRV_SC_MAX_COEFF_DESC_MEM);
                    }
                }
                else
                {
                    GT_0trace(VpsMdrvScTrace, GT_ERR,
                        "BADARGS: NULL pointer\n");
                }
                break;
            }

            case IOCTL_VPS_SC_SET_LAZY_LOADING:
            {
                if ((FVID2_SOK == retVal) && (NULL != cmdArgs))
                {
                    Vps_ScLazyLoadingParams *params =
                        (Vps_ScLazyLoadingParams *)cmdArgs;
                    UInt32 enableLazyLoadingModified = FALSE;
                    UInt32 enableFilterSelectModified = FALSE;

                    /* Check if Lazy Loading configuration is changed from
                     * current. */
                    if (hObj->instObj->enableLazyLoading !=
                        params->enableLazyLoading)
                    {
                        enableLazyLoadingModified = TRUE;
                        hObj->instObj->enableLazyLoading =
                            params->enableLazyLoading;
                    }

                    if (hObj->instObj->enableFilterSelect !=
                        params->enableFilterSelect)
                    {
                        enableFilterSelectModified = TRUE;
                        hObj->instObj->enableFilterSelect =
                            params->enableFilterSelect;
                    }

                    /* scalerId not needed for SC5, SC3, SC4, since only a
                     * single scalar is present in each path.
                     */
                    /* enableFilterSelect currently needs to be TRUE if
                     * enableLazyLoading is TRUE.
                     */
                    if ((TRUE  == params->enableLazyLoading) &&
                        (FALSE == params->enableFilterSelect))
                    {
                        GT_0trace(VpsMdrvScTrace, GT_ERR,
                            "enableFilterSelect must be set to TRUE if "
                            "enableLazyLoading is TRUE for the scalar!\n");
                        retVal = FVID2_EBADARGS;
                    }
                    GT_assert(VpsMdrvScTrace,
                            (   (TRUE == params->enableLazyLoading)
                             && (TRUE == params->enableFilterSelect))
                        ||  (FALSE == params->enableLazyLoading));

                    /* If there are any changes in lazy loading configuration,
                     * make the required changes.
                     */
                    if ((FVID2_SOK == retVal) &&
                        ((TRUE == enableLazyLoadingModified) ||
                         (TRUE == enableFilterSelectModified)))
                    {
                        retVal = vpsMdrvScModifyLazyLoadingCfg(
                                     hObj,
                                     params->scalarId,
                                     enableLazyLoadingModified,
                                     enableFilterSelectModified);
                        if (FVID2_SOK != retVal)
                        {
                            GT_0trace(VpsMdrvScTrace, GT_ERR,
                                "Failed to modify lazy loading config!\n");
                        }
                    }
                }
                else
                {
                    GT_0trace(VpsMdrvScTrace, GT_ERR,
                        "BADARGS: NULL pointer\n");
                }
                break;
            }

            case IOCTL_VPS_READ_ADV_SC_CFG:
            {
                UInt32              descSet;
                Vps_ScRdWrAdvCfg   *readCfg = (Vps_ScRdWrAdvCfg *) cmdArgs;
                Vcore_ScCfgParams   swpCfgPrms;
                VpsMdrv_ScChObj    *chObj;

                if (NULL == cmdArgs)
                {
                    GT_0trace(VpsMdrvScTrace, GT_ERR,
                        "BADARGS: NULL pointer\n");
                    retVal = FVID2_EBADARGS;
                }

                if (FVID2_SOK == retVal)
                {
                    if (readCfg->chNum > hObj->numCh)
                    {
                        retVal = FVID2_EINVALID_PARAMS;
                    }
                }

                if (FVID2_SOK == retVal)
                {
                    chObj = hObj->chObjs[readCfg->chNum];
                    for (descSet = 0;
                         descSet < VPSMDRV_SC_MAX_DESC_SET;
                         descSet++)
                    {
                        swpCfgPrms.scAdvCfg = &readCfg->scAdvCfg;
                        swpCfgPrms.ovlyPtr =
                            chObj->coreDescMem[descSet]
                                        [VPSMDRV_SC_OUT_CORE_IDX].shadowOvlyMem;
                        retVal +=  hObj->instObj->outCoreOps->control(
                                       hObj->outCoreHandle,
                                       VCORE_IOCTL_GET_SC_CFG,
                                       &swpCfgPrms);
                    }
                }
                break;
            }

            case IOCTL_VPS_WRITE_ADV_SC_CFG:
            {
                UInt32              descSet;
                Vps_ScRdWrAdvCfg   *writeCfg = (Vps_ScRdWrAdvCfg *) cmdArgs;
                Vcore_ScCfgParams   swpCfgPrms;
                VpsMdrv_ScChObj    *chObj;

                if (NULL == cmdArgs)
                {
                    GT_0trace(VpsMdrvScTrace, GT_ERR,
                        "BADARGS: NULL pointer\n");
                    retVal = FVID2_EBADARGS;
                }
                if (FVID2_SOK == retVal)
                {
                    if (writeCfg->chNum > hObj->numCh)
                    {
                        retVal = FVID2_EINVALID_PARAMS;
                    }
                }
                if (FVID2_SOK == retVal)
                {

                    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
                    {
                        chObj = hObj->chObjs[0];
                    }
                    else
                    {
                        chObj = hObj->chObjs[writeCfg->chNum];
                    }
                    for (descSet = 0;
                         descSet < VPSMDRV_SC_MAX_DESC_SET;
                         descSet++)
                    {
                        swpCfgPrms.scAdvCfg = &writeCfg->scAdvCfg;
                        swpCfgPrms.ovlyPtr =
                            chObj->coreDescMem[descSet]
                                [VPSMDRV_SC_OUT_CORE_IDX].shadowOvlyMem;
                        retVal +=  hObj->instObj->outCoreOps->control(
                                       hObj->outCoreHandle,
                                       VCORE_IOCTL_SET_SC_CFG,
                                       &swpCfgPrms);
                    }
                }
                break;
            }

            case IOCTL_VPS_GET_SC_HORZ_SUB_FRAME_INFO:
            {
                retVal = vpsMdrvScCopyHSubFrmInfo(
                            hObj,
                            (Vps_SubFrameHorzSubFrmInfo *)cmdArgs);
                break;
            }

            case IOCTL_VPS_SET_SC_CHANNEL_INFO:
            {
                UInt32             chNum;
                Vps_M2mScChParams *inChPrm;

                if (NULL == cmdArgs)
                {
                    GT_0trace(VpsMdrvScTrace, GT_ERR,
                        "BADARGS: NULL pointer\n");
                    retVal = FVID2_EBADARGS;
                }

                if (FVID2_SOK == retVal)
                {
                    inChPrm = (Vps_M2mScChParams *)cmdArgs;
                    chNum = inChPrm->inFmt.channelNum;

                    /* Update the Channel Params in the local data structures,
                       which in turn will calculate the subframe
                       information also */
                    retVal = vpsMdrvScUpdateChPrms(
                                hObj,
                                hObj->chObjs[chNum],
                                hObj->chParams[chNum],
                                inChPrm);

                    if (FVID2_SOK == retVal)
                    {
                        /* Update the new channel information in the cores */
                        retVal = vpsMdrvScSetChPrms(
                                    hObj,
                                    chNum);
                    }
                }
                break;
            }

            default:
            {
                retVal = FVID2_EINVALID_PARAMS;
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "INVAtLID_PARAMS: Not a supported IOCTL\n");
            }
        }
    }

    return (retVal);
}

/**
 *  VpsMdrv_scMlmCbFxn
 *  \brief call back function for the scalar write back memory to memory
 *  driver.  MLM calls this function for each submitted request when the
 *  request gets completed.
 */
Int32 VpsMdrv_scMlmCbFxn(Mlm_SubmitReqInfo *mlmReqInfo)
{
    VpsMdrv_ScQueueObj  *qObj = mlmReqInfo->cbArg;
    VpsMdrv_ScHandleObj *hObj = qObj->hObj;
    VpsMdrv_ScInstObj   *instObj = hObj->instObj;

    /* Check the request type was FRAMES or coefficient */
    if (VPSMDRV_QOBJ_TYPE_FRAMES == qObj->qObjType)
    {
        /* Put the completed Request to Done queue and Submit the already queued
         * request to MLM after updating descriptors */
        VpsMDrvScProcDoneAndQdFrameReq(qObj, hObj, instObj);
    }
    else
    {
        /* Process the requests for the coefficients */
        vpsMdrvScProcCoeffReq(qObj, hObj, instObj);
        Semaphore_post(hObj->ctrlSem);
    }

    return (FVID2_SOK);
}

/**
 * vpsMdrvScCheckParams
 * \brief Check the run time parameters for the request
 */
static Int32 vpsMdrvScCheckParams(VpsMdrv_ScHandleObj *hObj,
                                  FVID2_ProcessList *procList)
{
    Int32 retVal = FVID2_SOK;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    retVal = FVID2_checkProcessList(procList,
                 VPSMDRV_SC_MAX_INPUTS,
                 VPSMDRV_SC_MAX_OUTPUTS,
                 hObj->numCh,
                 FALSE);
    if (FVID2_SOK == retVal)
    {
        /* Check per List parameters if driver is opened per handle */
        if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
        {
            retVal = vpsMdrvScCheckPerListParams(hObj, procList);
        }
        /* Check perFrame parameters if the driver is opened per channel */
        else if ((FVID2_SOK == retVal) &&
                 (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode))
        {
            retVal = vpsMdrvScCheckPerChParams(hObj, procList);
        }
        else
        {
            retVal = FVID2_EFAIL;
            GT_0trace(VpsMdrvScTrace, GT_ERR, "FAIL:Mode not proper\n");
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScCheckPerListParams
 *  \brief check run time params for the per list configuration
 */
static Int32 vpsMdrvScCheckPerListParams(VpsMdrv_ScHandleObj *hObj,
                                         FVID2_ProcessList *procList)
{
    Int32               retVal = FVID2_SOK;
    UInt32              pMul;
    Vps_M2mScRtParams  *perListCfg;
    Vps_FrameParams    *sFrmPrms = NULL;
    Vps_FrameParams    *tFrmPrms = NULL;
    Vps_CropConfig     *cropCfg = NULL;

    perListCfg = (Vps_M2mScRtParams *) procList->inFrameList[0]->perListCfg;
    if (NULL != perListCfg)
    {
        sFrmPrms = perListCfg->inFrmPrms;
        tFrmPrms = perListCfg->outFrmPrms;
        cropCfg = perListCfg->srcCropCfg;
    }
    if (NULL != sFrmPrms)
    {
        /* If data format is YUV422I then pitch will be multiplied by 2 as
         * YUV422I is 2 bytes per pixel. While for YUV420 pitch is multiplied
         * by 1 as for each Y and UV plane there is 1 byte per pixel
         */
        if (FVID2_DF_YUV422I_YUYV == sFrmPrms->dataFormat)
        {
            pMul = 2u;
        }
        else
        {
            pMul = 1u;
        }
        /* Check for pitch is not less than width for both planes */
        if ((sFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] * pMul) < sFrmPrms->width)
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "Source pitch is less than width\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
        if (1u == pMul)
        {
            if ((sFrmPrms->pitch[FVID2_YUV_PL_CB_ADDR_IDX] * pMul) <
                 sFrmPrms->width)
            {
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "Source pitch is less than width\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    if (NULL != cropCfg && NULL != sFrmPrms)
    {
        /* Check CropWidth + cropStartx is not less than total width of
         * picture */
        if (sFrmPrms->width < (cropCfg->cropWidth + cropCfg->cropStartX))
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "Target width is less than crop width + horizontal offset\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
        if (sFrmPrms->height < (cropCfg->cropHeight + cropCfg->cropStartY))
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "Target height is less than crop height + vertical Offset\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (NULL != tFrmPrms)
    {
        /* Check for pitch is not less than width */
        if (tFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] < (tFrmPrms->width * 2u))
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "Target Pitch less than Target Width\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScCheckPerChParams
 *  \brief check params for the per channel configuration
 */
Int32 vpsMdrvScCheckPerChParams(VpsMdrv_ScHandleObj *hObj,
                                FVID2_ProcessList *procList)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt;
    UInt32              pMul;
    Vps_M2mScRtParams  *perChCfg;
    Vps_FrameParams    *sFrmPrms = NULL;
    Vps_FrameParams    *tFrmPrms = NULL;
    Vps_CropConfig     *cropCfg = NULL;

    for (chCnt = 0; chCnt < procList->inFrameList[0]->numFrames; chCnt++)
    {
        perChCfg = procList->inFrameList[0]->frames[chCnt]->perFrameCfg;
        if (NULL != perChCfg)
        {
            sFrmPrms = perChCfg->inFrmPrms;
            tFrmPrms = perChCfg->outFrmPrms;
            cropCfg = perChCfg->srcCropCfg;
        }

        /* If data format is YUV422I then pitch will be multiplied by 2 as
         * YUV422I is 2 bytes per pixel. While for YUV420 pitch is multiplied
         * by 1 as for each Y and UV plane there is 1 byte per pixel
         */
        if (NULL != sFrmPrms)
        {
            if (FVID2_DF_YUV422I_YUYV == sFrmPrms->dataFormat)
            {
                pMul = 2u;
            }
            else
            {
                pMul = 1u;
            }

            /* Check for pitch is not less than width for both planes */
            if ((sFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] * pMul) <
                 sFrmPrms->width)
            {
                GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                    "Source pitch is less than width\n");
                retVal = FVID2_EINVALID_PARAMS;
                break;
            }

            if (1u == pMul)
            {
                if ((sFrmPrms->pitch[FVID2_YUV_PL_CB_ADDR_IDX] * pMul) <
                     sFrmPrms->width)
                {
                    GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                        "Source pitch is less than width\n");
                    retVal = FVID2_EINVALID_PARAMS;
                    break;
                }
            }
        }

        if (NULL != cropCfg && NULL != sFrmPrms)
        {
            /* Check CropWidth + cropStartx is not less than total width of
             * picture. Same for height */
            if (sFrmPrms->width < (cropCfg->cropWidth + cropCfg->cropStartX))
            {
                GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                    "Source width is less than crop width + horizontal "
                    "Offset\n");
                retVal = FVID2_EINVALID_PARAMS;
                break;
            }
            if (sFrmPrms->height < (cropCfg->cropHeight + cropCfg->cropStartY))
            {
                 GT_1trace(VpsMdrvScTrace, GT_DEBUG,
                    "Source height is less than crop height + vertical ",
                    "Offset\n");
                retVal = FVID2_EINVALID_PARAMS;
                break;
            }
        }
        if (NULL != tFrmPrms)
        {
            if (FVID2_DF_YUV422I_YUYV ==
                hObj->chParams[chCnt]->outFmt.dataFormat)
            {
                pMul = 2u;
            }
            else
            {
                pMul = 1u;
            }
            /* Check for pitch is not less than width */
            if ((tFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] * pMul) <
                 tFrmPrms->width)
            {
                GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                    "Target Pitch less than Target Width\n");
                retVal = FVID2_EINVALID_PARAMS;
                break;
            }
            if (1u == pMul)
            {
                if ((tFrmPrms->pitch[FVID2_YUV_PL_CB_ADDR_IDX] * pMul) <
                     tFrmPrms->width)
                {
                    GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                        "Target pitch is less than width\n");
                    retVal = FVID2_EINVALID_PARAMS;
                    break;
                }
            }
        }
    }

    return retVal;
}

/**
 *  vpsMdrvScSetCoeffQObj
 *  \brief Sets the queue object for the coefficient programming.
 *  Creates the configuration descriptor.  Initializes the MLM object
 *  and queue object.
 */
static Int32 vpsMdrvScSetCoeffQObj(VpsMdrv_ScHandleObj *hObj,
                                   Ptr descPtr,
                                   Ptr horzCoeffMemPtr,
                                   Ptr vertCoeffMemPtr,
                                   Ptr vertBilinearCoeffMemPtr)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cookie;
    Ptr                 tmpDescPtr;
    VpsMdrv_ScQueueObj *qObj;
    VpsMdrv_ScInstObj  *instObj;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    GT_assert(VpsMdrvScTrace, (NULL != descPtr));
    GT_assert(VpsMdrvScTrace, (NULL != horzCoeffMemPtr));
    GT_assert(VpsMdrvScTrace, (NULL != vertCoeffMemPtr));

    instObj = hObj->instObj;
    cookie = Hwi_disable();
    qObj = VpsUtils_dequeue(instObj->freeQ);
    if (NULL == qObj)
    {
        retVal = FVID2_EALLOC;
        VpsUtils_freeDescMem(descPtr, VPSMDRV_SC_MAX_COEFF_DESC_MEM);
    }

    if (FVID2_SOK == retVal)
    {
        qObj->hObj = hObj;
        qObj->mlmReqInfo.reqStartAddr = descPtr;
        qObj->mlmReqInfo.lastRlDescAddr = ((UInt8 *)descPtr +
            (VPSMDRV_SC_MAX_COEFF_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE));
        qObj->mlmReqInfo.submitReqType = MLM_SRT_NO_SOC;
        qObj->mlmReqInfo.firstChannelSize =
            ((VPSMDRV_SC_MAX_COEFF_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE) +
             VPSHAL_VPDMA_CTRL_DESC_SIZE);
        qObj->mlmReqInfo.cbArg = qObj;
        qObj->qObjType = VPSMDRV_QOBJ_TYPE_COEFF;

        /* First create the horizontal coeff descriptor. */
        tmpDescPtr = descPtr;
        VpsHal_vpdmaCreateConfigDesc(
            tmpDescPtr,
            hObj->chObjs[0]->coeffConfigDest,
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            hObj->chObjs[0]->horzCoeffOvlySize,
            horzCoeffMemPtr,
            0u,
            hObj->chObjs[0]->horzCoeffOvlySize);

        /* Now create the vertical coeff descriptor. */
        tmpDescPtr = (Ptr)
            (((UInt32) tmpDescPtr) + VPSHAL_VPDMA_CONFIG_DESC_SIZE);
        VpsHal_vpdmaCreateConfigDesc(
            tmpDescPtr,
            hObj->chObjs[0]->coeffConfigDest,
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            hObj->chObjs[0]->vertCoeffOvlySize,
            vertCoeffMemPtr,
            (Ptr) (hObj->chObjs[0]->horzCoeffOvlySize),
            hObj->chObjs[0]->vertCoeffOvlySize);

        /* Create the vertical bilinear coeff descriptor only if HQ scalar,
         * otherwise create dummy descriptor.
         */
        tmpDescPtr = (Ptr)
            (((UInt32) tmpDescPtr) + VPSHAL_VPDMA_CONFIG_DESC_SIZE);
        if (NULL != vertBilinearCoeffMemPtr)
        {
            VpsHal_vpdmaCreateConfigDesc(
                tmpDescPtr,
                hObj->chObjs[0]->coeffConfigDest,
                VPSHAL_VPDMA_CPT_BLOCK_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                hObj->chObjs[0]->vertBilinearCoeffOvlySize,
                vertBilinearCoeffMemPtr,
                (Ptr) (hObj->chObjs[0]->horzCoeffOvlySize +
                       hObj->chObjs[0]->vertCoeffOvlySize),
                hObj->chObjs[0]->vertBilinearCoeffOvlySize);
        }
        else
        {
            VpsHal_vpdmaCreateDummyDesc(tmpDescPtr);
        }

        retVal = Mlm_submit(hObj->instObj->mlmHandle, &qObj->mlmReqInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvScTrace, GT_DEBUG, "MLM Queue Full");
            /* If MLM submit fails return with error */
            VpsUtils_freeDescMem(descPtr, VPSMDRV_SC_MAX_COEFF_DESC_MEM);
            VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
            retVal = FVID2_EALLOC;
        }
        else
        {
            /* Save the desc and overlay reference so that it can be freed
               when call back arrives */
            hObj->ioctlCoeffDesc = (VpsHal_VpdmaConfigDesc *) descPtr;
        }
    }

    if (FVID2_SOK == retVal)
    {
        hObj->numPendReq++;
    }

    Hwi_restore(cookie);

    return (retVal);
}

/**
 *  vpsMdrvScProgCoeff
 *  \brief Call the core function for programming the coefficient overlay
 *  memory
 */
static Int32 vpsMdrvScProgCoeff(VpsMdrv_ScHandleObj *hObj,
                                Vps_ScCoeffParams *params)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    Ptr                     descPtr;
    Vcore_ScCoeffParams     coreCoeffParams;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    cookie = Hwi_disable();
    /* Still processlist requests are pending to be addressed so try after
     * some time after dequeing the pending requests */
    if (0 != hObj->numPendReq)
    {
        retVal = FVID2_EAGAIN;
        GT_0trace(VpsMdrvScTrace, GT_INFO,
            "AGAIN: Requests Pending, Coeff could not be programmed\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate a set of descriptors for the coefficients + one control
         * descriptor for reload. */
        descPtr = VpsUtils_allocDescMem(
                      VPSMDRV_SC_MAX_COEFF_DESC_MEM,
                      VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        if (NULL == descPtr)
        {
            GT_0trace(VpsMdrvScTrace, GT_DEBUG,
              "Coeff Descriptor Allocation failed\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        coreCoeffParams.hScalingSet = params->hScalingSet;
        coreCoeffParams.vScalingSet = params->vScalingSet;
        coreCoeffParams.horzCoeffMemPtr = NULL; /* Returned by core */
        coreCoeffParams.vertCoeffMemPtr = NULL; /* Returned by core */
        coreCoeffParams.vertBilinearCoeffMemPtr = NULL; /*Returned by core*/
        coreCoeffParams.userCoeffPtr = params->coeffPtr;

        retVal = hObj->instObj->outCoreOps->programScCoeff(
                     hObj->outCoreHandle,
                     0u,
                     &coreCoeffParams);
        if (FVID2_SOK != retVal)
        {
            VpsUtils_freeDescMem(descPtr, VPSMDRV_SC_MAX_COEFF_DESC_MEM);
            retVal = FVID2_EINVALID_PARAMS;
            GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                "Core IOCTL for the coeff programming failed\n");
        }
        else
        {
            /* Here the descriptor memory pointer is freed
             * by the below function if it returns the error so no need to
             * free it here */
            retVal = vpsMdrvScSetCoeffQObj(
                         hObj,
                         descPtr,
                         coreCoeffParams.horzCoeffMemPtr,
                         coreCoeffParams.vertCoeffMemPtr,
                         coreCoeffParams.vertBilinearCoeffMemPtr);
        }
    }

    Hwi_restore(cookie);

    return retVal;
}

/**
 *  vpsMdrvScProcCoeffReq
 *  \brief Returns back the queue used for the coefficient back to the
 *  free queue and mark the  queue object as Free.
 */
static Void vpsMdrvScProcCoeffReq(VpsMdrv_ScQueueObj *qObj,
                                  VpsMdrv_ScHandleObj *hObj,
                                  VpsMdrv_ScInstObj *instObj)
{
    /* Put the qObj back to the free queue */
    VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
    hObj->numPendReq--;

    return;
}

/**
 *  vpsMdrvScModifyLazyLoadingCfg
 *  \brief Make required changes for modification of Lazy Loading configuration
 *  if there are any changes in the lazy loading configuration.
 */
static Int32 vpsMdrvScModifyLazyLoadingCfg(VpsMdrv_ScHandleObj *hObj,
                                           UInt32 scalarId,
                                           UInt32 enableLazyLoadingModified,
                                           UInt32 enableFilterSelectModified)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cookie;
    UInt32              chCnt, descSet;
    VpsMdrv_ScChObj    *chObj, *sPadChObj;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    cookie = Hwi_disable();

    /* Currently assume that nothing is to be done for
     * enableFilterSelectModified */
    if (TRUE == enableLazyLoadingModified)
    {
        /* If Lazy Loading is enabled, calculate and set the scaling factor
         * config for each channel. */
        if (TRUE == hObj->instObj->enableLazyLoading)
        {
            /* Go down into the core to get the scaling factor config
             * for each channel.
             * Also do this for all descriptor sets, hence provide it as -1.
             */
            retVal = vpsMdrvScGetScFactorConfig(hObj, (UInt32) -1, (UInt32) -1);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "EFAIL: Getting the Scaling Factor Config Failed\n");
            }
        }

        for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            sPadChObj = hObj->sPadChObjs[chCnt];
            for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
            {
                /* If the configuration is per channel first descriptor in the
                 * layout will be set to config descriptor to avoid impact when
                 * lazy loading is disabled. When lazy loading is enabled, it is
                 * set to the coeff descriptor. */
                if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
                {
                    /* If Lazy Loading is enabled, and if the scaling factors
                     * are valid, indicating that the scalar is not in bypass,
                     * set the first descriptor pointer to the coeff descriptor,
                     * and adjust the list size. */
                    if ((TRUE == hObj->instObj->enableLazyLoading) &&
                        (VPS_SC_HST_MAX != chObj->scFactor.hsType) &&
                        (VPS_SC_VST_MAX != chObj->scFactor.vsType))
                    {
                        /* Set the filtType in sub-frame params so that it is
                         * correctly taken into account for sub-frame as well.
                         */
                        chObj->slcChObj.filtType = chObj->scFactor.vsType;
                        sPadChObj->slcChObj.filtType = chObj->scFactor.vsType;

                        chObj->firstDesc[descSet] =
                            chObj->coeffDesc[descSet][0];
                        sPadChObj->firstDesc[descSet] =
                            sPadChObj->coeffDesc[descSet][0];
                        if (descSet == 0)
                        {
                            /* Update the size of the list to be posted to VPDMA
                             * for the first descriptor set. The second set has
                             * the same size. */
                            chObj->descListSize += chObj->totalCoeffDescMem;
                            sPadChObj->descListSize +=
                                sPadChObj->totalCoeffDescMem;
                        }
                    }
                    else
                    {
                        chObj->firstDesc[descSet] =
                            chObj->nshwCfgDesc[descSet];
                        sPadChObj->firstDesc[descSet] =
                            sPadChObj->nshwCfgDesc[descSet];
                        if (descSet == 0)
                        {
                            /* Update the size of the list to be posted to VPDMA
                             * for the first descriptor set. The second set has
                             * the same size. */
                            chObj->descListSize -= chObj->totalCoeffDescMem;
                            sPadChObj->descListSize -=
                                sPadChObj->totalCoeffDescMem;
                        }
                    }
                }
            }
        }
    }

    Hwi_restore(cookie);

    return (retVal);
}

/**
 *  vpsMdrvScGetFreeDescSet
 *  \brief Get the free descriptor set for programming the buffers
 */
Int32 vpsMdrvScGetFreeDescSet(VpsMdrv_ScHandleObj *hObj,
                              VpsMdrv_ScQueueObj *qObj)
{
    Int32           retVal = FVID2_EFAIL;
    UInt32          cookie;
    UInt32          descSet;

    cookie = Hwi_disable();
    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
    {
        if (TRUE == hObj->isDescSetFree[descSet])
        {
            hObj->isDescSetFree[descSet] = FALSE;
            Hwi_restore(cookie);
            retVal = FVID2_SOK;
            qObj->descSetInUse = descSet;
            break;
        }
    }

    if (FVID2_SOK != retVal)
    {
        Hwi_restore(cookie);
        qObj->descSetInUse = -1;
        GT_0trace(VpsMdrvScTrace, GT_DEBUG, "Free Desc Set not available\n");
    }

    return (retVal);
}

/**
 *  VpsMDrvScProcDoneAndQdFrameReq
 *  \brief Process the completed and queued frames.
 *  Put the frames in the done queue of the appropriate handle. Get the queued
 *  frames from the handle and update the descriptor address with new buffers
 *  and submit it to the MLM
 */
static Void VpsMDrvScProcDoneAndQdFrameReq(VpsMdrv_ScQueueObj *qObj,
                                           VpsMdrv_ScHandleObj *hObj,
                                           VpsMdrv_ScInstObj *instObj)
{
    Int32       result;
    UInt32      descSetInUse;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    /*  Queue the frame to the Done Queue */
    VpsUtils_queue(hObj->doneQ, &qObj->qElem, qObj);
    /* Get the descriptor already in  use and mark it as free */
    descSetInUse = qObj->descSetInUse;
    hObj->isDescSetFree[descSetInUse] = TRUE;

    /* Callback the application for the done frame */
    if (hObj->fdmCbPrms.fdmCbFxn)
    {
        hObj->fdmCbPrms.fdmCbFxn(hObj->fdmCbPrms.fdmData, NULL);
    }
    /* Now process the queued frames */
    qObj = VpsUtils_dequeue(hObj->reqQ);
    if (NULL != qObj)
    {
        /* Set the descset in use same as the one which became free in the call
         * back as the descset in use */
        qObj->descSetInUse = descSetInUse;
        /* Mark the descriptor set as used */
        hObj->isDescSetFree[qObj->descSetInUse] = FALSE;
        /* Update buffer Address */
        result = vpsMdrvScUpdateDescList(hObj, qObj);
        if (FVID2_SOK != result)
        {
            /* Copy the error process list to the err process
             * list container give byy app */
            vpsMdrvScReturnErrorProclist(hObj, qObj);
            GT_0trace(VpsMdrvScTrace, GT_DEBUG, "Buffer updation failed\n");
        }
        else
        {
            /* Once the descriptor set is created submit the request to the
             * MLM  */
            result = Mlm_submit(hObj->instObj->mlmHandle, &qObj->mlmReqInfo);
        }

        /* Return the error processlist if MLM is not able to queue the
         * request */
        if (FVID2_SOK != result)
        {
            /* Copy the error process list to the err process
             * list container give byy app */
            vpsMdrvScReturnErrorProclist(hObj, qObj);
            GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                "MLM Submission failed. MLM Queue full\n");
        }
    }

    return;
}

/**
 *  vpsMdrvScUpdateDescList
 *  \brief Update the buffers for all the frames and create the list
 *  of request
 */
static Int32 vpsMdrvScUpdateDescList(VpsMdrv_ScHandleObj *hObj,
                                     VpsMdrv_ScQueueObj *qObj)
{
    Int32                   retVal = FVID2_SOK;
    Bool                    done = FALSE;
    UInt32                  frmCnt;
    FVID2_ProcessList      *procList;
    FVID2_Frame            *inFrm, *outFrm;
    FVID2_FrameList        *inFrmLst, *outFrmLst;
    Void                   *descPtr, *rldDesc, *listPostDesc;
    UInt32                  listPostSize, hSlcNum = 0u;
    VpsMdrv_ScChObj        *chObj;
    VpsMdrv_ScInstObj      *instObj;
    UInt32                  descSetInUse;
    UInt32                  descSetType;
    UInt32                  index, chNum, chCnt;
    Vps_M2mScRtParams      *rtParams;
    Ptr                     inAddrY, inAddrC, outAddr;
    Vps_M2mScRtParams       locRtPrms;
    Vps_FrameParams         inFrmPrms, outFrmPrms;
    Vps_ScRtConfig          scRtCfg;
    Vps_CropConfig          cropRtCfg;
    Vps_M2mScChParams      *chParam;
    VpsHal_ScFactorConfig  *curScFactor;

    procList = &qObj->procList;
    inFrmLst = procList->inFrameList[0];
    outFrmLst = procList->outFrameList[0];
    descSetInUse = qObj->descSetInUse;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    instObj = hObj->instObj;
    GT_assert(VpsMdrvScTrace, (NULL != instObj));

    /* Reset the channel count for this request and mark all the scratch pad
     * descs as un-allocated
     */
    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        hObj->chCnt[descSetInUse][chCnt] = 0;
        hObj->sPadChIsAllocated[descSetInUse][chCnt] = FALSE;
    }
    curScFactor = &(instObj->curScFactor);

    if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
    {
        rldDesc = NULL;
        listPostDesc = NULL;
        listPostSize = 0u;
    }
    else
    {
        /* Initialize the reload pointer to handle descriptors so that the first
         * channel will be linked to it */
        listPostDesc = hObj->firstDesc[descSetInUse];
        listPostSize = hObj->firstDescListSize;
        rldDesc = hObj->rldDesc[descSetInUse];
        /* NULL pointer check */
        GT_assert(VpsMdrvScTrace, (NULL != rldDesc));
    }

    /* This loop updates the data descriptors for all the channels.
     * Further it links the reload descriptor for the previous channels
     * with the current channel on which the loop is running.  For the
     * first loop count it links the current channel with the non shadow
     * config descriptor pointer */
    for (frmCnt = 0; frmCnt < inFrmLst->numFrames; frmCnt++)
    {
        inFrm = inFrmLst->frames[frmCnt];
        outFrm = outFrmLst->frames[frmCnt];
        chNum = outFrm->channelNum;

        /* Get the channel object for the frame */
        vpsMdrvScGetChObjIndex(hObj, inFrm, descSetInUse, &index, &descSetType);

        /* Get the proper channel object based on the channel number and
         * descriptor type set to be used */
        /* Descriptors are seperate memory for the scratch pad channels but the
         * overlay memories will always point to the same overlay memories used
         * for the same channel objects */
        chObj = vpsMdrvScGetChObj(hObj, descSetType, index);
        rtParams = vpsMdrvScGetRtPrms(hObj, procList, frmCnt);

        chParam = hObj->chParams[chNum];
        /* Do SubFrame processing if configured for this channel/frame:
           1. Initialise subframe instance memory of channel at first subframe
           and keep calling
           SubFrame pre and post calc functions for subsequent subFrame;
           2. Update RtParams to configure subframe stuff to Descriptors by
           Core fcns.
           3.  make a copy and update inFrm and outFrm addresses here
           considering offset values required for subframe Line memory and
           pass to vpsMdrvScUpdateDescs() to be programmed in Desc.
           These values are restored after vpsMdrvScUpdateDescs() function
           returns. this is needed as APP expects ProcList with No changes.
           NOTE: No support for runtime subframe enable/disable support,
           can be added later if needed
        */
        if (chObj->slcChObj.subFrameMdEnable == TRUE)
        {
            /* Only lower 16 bits are used for vertical slice number */
            chObj->slcChObj.subFrameNum =
                inFrm->subFrameInfo->subFrameNum & 0xFFFFu;
            chObj->slcChObj.slcLnsAvailable = inFrm->subFrameInfo->numInLines;

            if (chObj->slcChObj.subFrameNum == 0)
            {
                vpsMdrvScSubFrameInitInfo(&chObj->slcChObj);
                outFrm->subFrameInfo->numOutLines = 0;

                if (FALSE== chObj->slcChObj.hSubFrmInfo.isEnable)
                {
                    vpsMdrvScSubFrameHorzInitInfo(&chObj->slcChObj);
                }
            }

            if (TRUE == chObj->slcChObj.hSubFrmInfo.isEnable)
            {
                hSlcNum = (inFrm->subFrameInfo->subFrameNum >> 16u) & 0xFFFFu;
            }
            else
            {
                hSlcNum = 0u;
            }

            vpsMdrvScSubFrameCalcInfo(&chObj->slcChObj);
            if (rtParams == NULL)
            {
                /* use locally allocated rtparams memory
                 * TODO: To avoid run-time cycles penalty in allocating and
                 * assigning the values/ check for NULL in all RtPrms sub
                 * structures, Allocate this as global static variable and
                 * use across all channels as this is just needed locally
                 * and all values are updated for every call
                 */
                locRtPrms.inFrmPrms = &inFrmPrms;
                locRtPrms.outFrmPrms = &outFrmPrms;
                locRtPrms.srcCropCfg = &cropRtCfg;
                locRtPrms.scCfg = &scRtCfg;
                rtParams = &locRtPrms;
            }
            else
            {
                /* Only pitch can be changed at runtime
                   when subframe is enabled */
            }

            /* make local copy of inFrm and outFrm address */
            inAddrY = inFrm->addr[FVID2_FRAME_ADDR_IDX]
                                 [FVID2_YUV_SP_Y_ADDR_IDX];
            inAddrC = inFrm->addr[FVID2_FRAME_ADDR_IDX]
                                 [FVID2_YUV_SP_CBCR_ADDR_IDX];
            outAddr = outFrm->addr[FVID2_FRAME_ADDR_IDX]
                                  [FVID2_YUV_INT_ADDR_IDX];

            vpsMdrvScSubFrmUpdate(
                &chObj->slcChObj,
                inFrm,
                outFrm,
                rtParams,
                chParam,
                hSlcNum);
        }

        /* If the scratchpad memory is used then program the whole descriptor
         * along with the configuration descriptor */
        retVal = vpsMdrvScUpdateDescs(
                     hObj,
                     inFrm,
                     outFrm,
                     chObj,
                     descSetInUse,
                     descSetType,
                     rtParams);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvScTrace, GT_DEBUG,
              "Updation of the descriptor from core failed\n");
            break;
        }

        /* If Lazy Loading is enabled and the scalar is not in bypass, convert
         * the dummy descriptors to coeff descriptors if required. */
        if ((TRUE == instObj->enableLazyLoading) &&
            (TRUE != done) &&
            (VPS_SC_HST_MAX != chObj->scFactor.hsType) &&
            (VPS_SC_VST_MAX != chObj->scFactor.vsType))
        {
            /* Check if horizontal scaling coeff desc has to be created. */
            if ((curScFactor->hsType != chObj->scFactor.hsType) ||
                (curScFactor->hScalingSet != chObj->scFactor.hScalingSet))
            {
                /* The first coeff descriptor is for horizontal scaling. */
                VpsHal_vpdmaCreateConfigDesc(
                    chObj->coeffDesc[descSetInUse][0u],
                    chObj->coeffConfigDest,
                    VPSHAL_VPDMA_CPT_BLOCK_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    chObj->horzCoeffOvlySize,
                    chObj->horzCoeffMemPtr,
                    0u,
                    chObj->horzCoeffOvlySize);

                /* Now set the current hsType and hScalingSet to that of
                 * this channel. */
                instObj->curScFactor.hsType = chObj->scFactor.hsType;
                instObj->curScFactor.hScalingSet =
                    chObj->scFactor.hScalingSet;
            }
            else /* Create dummy descriptor */
            {
                VpsHal_vpdmaCreateDummyDesc(
                    chObj->coeffDesc[descSetInUse][0u]);
            }

            /* Check if vertical scaling coeff desc has to be created. */
            if ((curScFactor->vsType != chObj->scFactor.vsType) ||
                (curScFactor->vScalingSet != chObj->scFactor.vScalingSet))
            {
                /* The second coeff descriptor is for vertical scaling. */
                VpsHal_vpdmaCreateConfigDesc(
                    chObj->coeffDesc[descSetInUse][1u],
                    chObj->coeffConfigDest,
                    VPSHAL_VPDMA_CPT_BLOCK_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    chObj->vertCoeffOvlySize,
                    chObj->vertCoeffMemPtr,
                    (Ptr) (chObj->horzCoeffOvlySize),
                    chObj->vertCoeffOvlySize);

                /* Now set the current vsType and vScalingSet to that of
                 * this channel. */
                instObj->curScFactor.vsType = chObj->scFactor.vsType;
                instObj->curScFactor.vScalingSet =
                    chObj->scFactor.vScalingSet;
            }
            else /* Create dummy descriptor */
            {
                VpsHal_vpdmaCreateDummyDesc(
                    chObj->coeffDesc[descSetInUse][1u]);
            }

            /* For per-handle mode, the coeff descriptors need to be programmed
             * only once. */
            if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
            {
                done = TRUE;
            }
        }

        /* Do subframe specific post processing */
        if (chObj->slcChObj.subFrameMdEnable == TRUE)
        {
            outFrm->subFrameInfo->subFrameNum = chObj->slcChObj.subFrameNum;
            outFrm->subFrameInfo->numOutLines += chObj->slcChObj.tarH;

            vpsMdrvScSubFrameCalcPostProcInfo(&chObj->slcChObj);
            /* Restore inFrm and outFrm addresses to pass processlist
               intact to application */
            inFrm->addr[FVID2_FRAME_ADDR_IDX]
                       [FVID2_YUV_SP_Y_ADDR_IDX] = inAddrY;
            inFrm->addr[FVID2_FRAME_ADDR_IDX]
                       [FVID2_YUV_SP_CBCR_ADDR_IDX]= inAddrC;
            outFrm->addr[FVID2_FRAME_ADDR_IDX]
                        [FVID2_YUV_INT_ADDR_IDX] = outAddr;
        }
        /* Get the first descriptor pointer for the channel */
        descPtr = (Ptr) chObj->firstDesc[descSetInUse];

        if (rldDesc != NULL)
        {
            /* Link the reload descriptor of previous channel to the descriptor
             * of this channel */
            VpsHal_vpdmaCreateRLCtrlDesc(rldDesc, descPtr, chObj->descListSize);
        }
        else
        {
            /* Per handle configuration, store the first channel's start addr */
            listPostDesc = descPtr;
            listPostSize = chObj->descListSize;
        }

        if (FVID2_SOK != retVal)
        {
            break;
        }
        /* Update the reload desc pointer to this channel for linking with
         * next channel
         */
        rldDesc = chObj->rldDesc[descSetInUse];
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the MLM params with the size of the first list and address of
         * first descriptor of the list */
        GT_assert(VpsMdrvScTrace, (NULL != listPostDesc));
        GT_assert(VpsMdrvScTrace, (NULL != rldDesc));
        qObj->mlmReqInfo.firstChannelSize = listPostSize;
        qObj->mlmReqInfo.reqStartAddr = listPostDesc;
        qObj->mlmReqInfo.lastRlDescAddr = rldDesc;
        /* We have already put SOCH for each channel in the driver,
         * so tell MLM not to put again */
        qObj->mlmReqInfo.numChannels = 0u;
        qObj->mlmReqInfo.submitReqType = MLM_SRT_NO_SOC;
        qObj->mlmReqInfo.cbArg = (Ptr) qObj;
    }
    else
    {
         GT_0trace(VpsMdrvScTrace, GT_DEBUG,
              "Updation of the descriptor from core failed\n");
    }

    return (retVal);
}

/**
 *  vpsMdrvScGetChObjIndex
 *  \brief Get the index of the channel object.  Also the flag to use the
 *  channel objects and scratchpad objects
 */
inline Void vpsMdrvScGetChObjIndex(VpsMdrv_ScHandleObj *hObj,
                                   FVID2_Frame *frame,
                                   UInt32 descSetInUse,
                                   UInt32 *index,
                                   UInt32 *descSetType)
{
    UInt32      chNum, chCnt;

    chNum = frame->channelNum;
    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    if (0 != hObj->chCnt[descSetInUse][chNum])
    {
        for (chCnt = 0; chCnt < VPS_M2M_SC_MAX_CH; chCnt++)
        {
            if (FALSE == hObj->sPadChIsAllocated[descSetInUse][chCnt])
            {
                *index = chCnt;
                *descSetType = VPSMDRV_SC_SCRATCH_PAD_DESC_SET;
                hObj->sPadChIsAllocated[descSetInUse][chCnt] = TRUE;
                break;
            }
        }
    }
    else
    {
        *index = chNum;
        *descSetType = VPSMDRV_SC_NORMAL_DESC_SET;
        hObj->chCnt[descSetInUse][chNum]++;
    }

    return;
}

/**
 *  vpsMdrvScUpdateDescs
 *  \brief Update the descriptors buffer addresses and configurtion descriptors
 *  for the scratchpad channels
 */
static inline Int32 vpsMdrvScUpdateDescs(VpsMdrv_ScHandleObj *hObj,
                                         FVID2_Frame *inFrm,
                                         FVID2_Frame *outFrm,
                                         VpsMdrv_ScChObj *chObj,
                                         UInt32 descSetInUse,
                                         UInt32 descType,
                                         Vps_M2mScRtParams *rtParams)
{
    UInt32              chNum;
    Int32               retVal = FVID2_SOK;
    UInt32              descSet;
    Vcore_SecRtParams   secRtParams, *secPtr;
    Vcore_Property      property;
    Vcore_SwpRtParams   swpRtParams, *swpRtPtr = NULL;
    Vcore_VipRtParams   vipRtParams, *vipRtPtr = NULL;


    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    GT_assert(VpsMdrvScTrace, (NULL != inFrm));
    GT_assert(VpsMdrvScTrace, (NULL != outFrm));
    GT_assert(VpsMdrvScTrace, (NULL != chObj));

    GT_assert(VpsMdrvScTrace, (NULL != hObj->instObj->outCoreOps->getProperty));
    retVal = hObj->instObj->outCoreOps->getProperty(
                 hObj->outCoreHandle,
                 &property);

    if (VPSMDRV_SC_SCRATCH_PAD_DESC_SET == descType)
    {
        chNum = inFrm->channelNum;
        GT_assert(VpsMdrvScTrace,
            (NULL != hObj->instObj->outCoreOps->programDesc));
        retVal = hObj->instObj->outCoreOps->programDesc(
                     hObj->outCoreHandle,
                     chNum,
                     0u,
                     &chObj->coreDescMem[descSetInUse]
                                        [VPSMDRV_SC_OUT_CORE_IDX]);
        GT_assert(VpsMdrvScTrace, (NULL != hObj->instObj->inCoreOps->programDesc));
        retVal += hObj->instObj->inCoreOps->programDesc(
                      hObj->inCoreHandle,
                      chNum,
                      0u,
                      &chObj->coreDescMem[descSetInUse]
                                         [VPSMDRV_SC_IN_CORE_IDX]);
        /* Configuration descriptor also needs to be created for the scratchpad
         * memory. For the scratchpad descriptor set there is no overlay memory
         * so overlay memory will still be pointing to the overlay memory
         * used for the proper descriptors */
        VpsHal_vpdmaCreateConfigDesc(
            chObj->nshwCfgDesc[descSetInUse],
            VPSHAL_VPDMA_CONFIG_DEST_MMR,
            VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            (chObj->totalNshwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] +
                chObj->totalNshwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX]),
            chObj->coreDescMem[descSetInUse]
                              [VPSMDRV_SC_IN_CORE_IDX].nonShadowOvlyMem,
            NULL,
            0u);
        VpsHal_vpdmaCreateConfigDesc(
            chObj->shwCfgDesc[descSetInUse],
            VPSHAL_VPDMA_CONFIG_DEST_MMR,
            VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            (chObj->totalShwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] +
                chObj->totalShwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX]),
            chObj->coreDescMem[descSetInUse]
                              [VPSMDRV_SC_IN_CORE_IDX].shadowOvlyMem,
            NULL,
            0u);
    }

    if (NULL != rtParams && NULL != rtParams->inFrmPrms)
    {
        secRtParams.inFrmPrms = rtParams->inFrmPrms;
        secRtParams.posCfg = NULL;
        secPtr = &secRtParams;
    }
    else
    {
        secPtr = NULL;
    }

    /* Update the runtime params */
    if (FVID2_SOK == retVal && (NULL != rtParams ||
        TRUE == chObj->isDescDirty[descSetInUse]))
    {
        GT_assert(VpsMdrvScTrace,
            (NULL != hObj->instObj->inCoreOps->updateRtMem));
        retVal += hObj->instObj->inCoreOps->updateRtMem(
                      hObj->inCoreHandle,
                      &chObj->coreDescMem[descSetInUse][VPSMDRV_SC_IN_CORE_IDX],
                      inFrm->channelNum,
                      secPtr,
                      0u);

        if (VCORE_TYPE_VIP == property.name)
        {
            if (NULL != rtParams)
            {
                vipRtPtr = &vipRtParams;

                vipRtPtr->inFrmPrms   = rtParams->inFrmPrms;
                vipRtPtr->outFrmPrms = rtParams->outFrmPrms;
                vipRtPtr->scCropCfg   = rtParams->srcCropCfg;
                vipRtPtr->scRtCfg       = rtParams->scCfg;
                vipRtPtr->isDeinterlacing = FALSE;

            }

            GT_assert(VpsMdrvScTrace,
                (NULL != hObj->instObj->outCoreOps->updateRtMem));
            retVal += hObj->instObj->outCoreOps->updateRtMem(
                          hObj->outCoreHandle,
                          &chObj->coreDescMem[descSetInUse]
                                             [VPSMDRV_SC_OUT_CORE_IDX],
                          outFrm->channelNum,
                          (Vcore_VipRtParams*)vipRtPtr,
                          0u);
        }
        else if (VCORE_TYPE_SC_WB2 == property.name)
        {
            if (NULL != rtParams )
            {
                swpRtPtr = &swpRtParams;

                swpRtPtr->inFrmPrms = rtParams->inFrmPrms;
                swpRtPtr->outFrmPrms = rtParams->outFrmPrms;
                swpRtPtr->srcCropCfg = rtParams->srcCropCfg;
                swpRtPtr->scRtCfg = rtParams->scCfg;
            }

            GT_assert(VpsMdrvScTrace,
                (NULL != hObj->instObj->outCoreOps->updateRtMem));
            retVal += hObj->instObj->outCoreOps->updateRtMem(
                          hObj->outCoreHandle,
                          &chObj->coreDescMem[descSetInUse]
                                             [VPSMDRV_SC_OUT_CORE_IDX],
                          outFrm->channelNum,
                          (Vcore_SwpRtParams *) swpRtPtr,
                          0u);
        }

        /* If Lazy Loading is enabled, recalculate and set the scaling
         * factor config for the channel for the current descriptor set in use.
         * The other descriptor set may already be under processing, so that
         * must not be touched now. It must just be set to 'dirty' so that the
         * next time when it becomes free, it can be updated.
         */
        if (TRUE == hObj->instObj->enableLazyLoading)
        {
            /* To take care of the updated configuration due to RT params,
             * go down into the core to get the scaling factor config for
             * the channel.
             */
            retVal = vpsMdrvScGetScFactorConfig(
                         hObj,
                         outFrm->channelNum,
                         descSetInUse);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                    "EFAIL: Getting the Scaling Factor Config Failed\n");
            }
            /* The scaling ratio does not change for sub-frame scaling from
             * what was set originally when the scaling factor config was
             * calculated. The scaling ratio for each slice will be the same
             * as that for the whole frame.
             */
        }

        if (NULL != rtParams)
        {
            for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
            {
                chObj->isDescDirty[descSet] = TRUE;
            }
        }
        chObj->isDescDirty[descSetInUse] = FALSE;
    }

    if (FVID2_SOK == retVal)
    {
        GT_assert(VpsMdrvScTrace,
            (NULL != hObj->instObj->inCoreOps->updateDesc));
        retVal = hObj->instObj->inCoreOps->updateDesc(
                     hObj->inCoreHandle,
                     &chObj->coreDescMem[descSetInUse][VPSMDRV_SC_IN_CORE_IDX],
                     inFrm,
                     0);
        GT_assert(VpsMdrvScTrace,
                (NULL != hObj->instObj->outCoreOps->updateDesc));
        retVal += hObj->instObj->outCoreOps->updateDesc(
                      hObj->outCoreHandle,
                      &chObj->coreDescMem[descSetInUse]
                                         [VPSMDRV_SC_OUT_CORE_IDX],
                      outFrm,
                      0);
    }

    return (retVal);
}


/**
 *  vpsMdrvScGetDescInfo
 *  \brief Update the number of data and config descriptors
 *  and overlay and coefficient memory needed
 *  for each channel of handle
 */
static Int32 vpsMdrvScGetDescInfo(VpsMdrv_ScHandleObj *hObj)
{
    Int32               retVal = FVID2_EFAIL;
    UInt32              chCnt, sochCntIn, sochCntOut;
    VpsMdrv_ScChObj     *chObj;
    Vcore_DescInfo      inDescInfo, outDescInfo;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        GT_assert(VpsMdrvScTrace,
            (NULL != hObj->instObj->inCoreOps->getDescInfo));
        retVal = hObj->instObj->inCoreOps->getDescInfo(
                     hObj->inCoreHandle,
                     chCnt,
                     0u,
                     &inDescInfo);
        if (FVID2_SOK != retVal)
        {
             GT_0trace(VpsMdrvScTrace, GT_DEBUG,
              "Getting the descriptor information from Core failed\n");
            break;
        }

        GT_assert(VpsMdrvScTrace,
            (NULL != hObj->instObj->outCoreOps->getDescInfo));
        retVal = hObj->instObj->outCoreOps->getDescInfo(
                     hObj->outCoreHandle,
                     chCnt,
                     0u,
                     &outDescInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvScTrace, GT_DEBUG,
              "Getting the descriptor information from Core failed\n");
           break;
        }

        chObj->numInDataDesc =
            (inDescInfo.numInDataDesc + outDescInfo.numInDataDesc);
        chObj->numOutDataDesc =
            (inDescInfo.numOutDataDesc + outDescInfo.numOutDataDesc);
        chObj->numCoeffDesc = VPSMDRV_SC_MAX_RT_COEFF_CFG_DESC;

        /*chObj->numSochDesc = VPSMDRV_SC_MAX_SOCH_DESC;*/
        chObj->numSochDesc = (inDescInfo.numChannels + outDescInfo.numChannels);

        /* First take the syncChannel descriptors for outbound channel */
        for(sochCntOut = 0; sochCntOut < outDescInfo.numChannels; sochCntOut++)
        {
            chObj->sochChNum[sochCntOut] = outDescInfo.socChNum[sochCntOut];
        }
        for (sochCntIn = 0; sochCntIn < inDescInfo.numChannels; sochCntIn++)
        {
            chObj->sochChNum[sochCntOut + sochCntIn] =
                inDescInfo.socChNum[sochCntIn];
        }
        chObj->totalNshwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] =
            inDescInfo.nonShadowOvlySize;
        chObj->totalNshwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX] =
            outDescInfo.nonShadowOvlySize;
        chObj->totalShwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] =
            inDescInfo.shadowOvlySize;
        chObj->totalShwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX] =
            outDescInfo.shadowOvlySize;

        /* Only the outCore has scalar. So the coeff info is needed only for
         * outCore. */
        chObj->horzCoeffOvlySize = outDescInfo.horzCoeffOvlySize;
        chObj->vertCoeffOvlySize = outDescInfo.vertCoeffOvlySize;
        chObj->vertBilinearCoeffOvlySize =
            outDescInfo.vertBilinearCoeffOvlySize;
        chObj->horzCoeffMemPtr = NULL;
        chObj->vertCoeffMemPtr = NULL;
        chObj->coeffConfigDest = outDescInfo.coeffConfigDest;
    }

    return (retVal);
}

/**
 *  vpsMdrvScOpenCores
 *  \brief Opens the handles of respective cores and sets the parameters
 */
static Int32 vpsMdrvScOpenCores(VpsMdrv_ScHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              perChCfg;
    UInt32              chCnt;
    Vcore_Property      property;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    /* if the configuration is required per handle set the core in
     * appropriate mode. Update the configuration parameters only once
     * if the configuration is per handle */
    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
    {
        perChCfg = FALSE;
    }
    else
    {
        perChCfg = TRUE;
    }

    GT_assert(VpsMdrvScTrace, (NULL != hObj->instObj->inCoreOps->open));
    /* Open the input core */
    hObj->inCoreHandle = hObj->instObj->inCoreOps->open(
                             hObj->instObj->inCoreInstId,
                             VCORE_OPMODE_MEMORY,
                             hObj->numCh,
                             perChCfg);
    if (NULL == (Void *) hObj->inCoreHandle)
    {
        GT_0trace(VpsMdrvScTrace, GT_DEBUG, "Opening of the core failed\n");
        retVal = FVID2_EALLOC;
    }

    /* Open the output core */
    if (FVID2_SOK == retVal)
    {
        GT_assert(VpsMdrvScTrace,
            (NULL != hObj->instObj->outCoreOps->open));
        hObj->outCoreHandle = hObj->instObj->outCoreOps->open(
                                  hObj->instObj->outCoreInstId,
                                  VCORE_OPMODE_MEMORY,
                                  hObj->numCh,
                                  perChCfg);
        if (NULL == (Void *) hObj->outCoreHandle)
        {
            GT_assert(VpsMdrvScTrace,
                (NULL != hObj->instObj->inCoreOps->close));
            hObj->instObj->inCoreOps->close(hObj->inCoreHandle);
            GT_0trace(VpsMdrvScTrace, GT_DEBUG, "Opening of the core failed\n");
            retVal = FVID2_EALLOC;
        }

        if (FVID2_SOK == retVal)
        {
            GT_assert(VpsMdrvScTrace,
                  (NULL != hObj->instObj->outCoreOps->getProperty));
            retVal = hObj->instObj->outCoreOps->getProperty(
                         hObj->outCoreHandle,
                         &property);
        }
    }

    if (FVID2_SOK == retVal)
    {
        for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
        {
            retVal = vpsMdrvScSetChPrms(
                        hObj,
                        chCnt);

            if (FVID2_SOK != retVal)
            {
                /* if setting of params failed close both the core handles */
                vpsMdrvScCloseCores(hObj);
                break;
            }
        }

        if (VCORE_TYPE_VIP == property.name)
        {
            if (FVID2_SOK == retVal)
            {
                /* Allocate the path in VIP based on the parameters set
                 * using SetParams */
                GT_assert(VpsMdrvScTrace,
                        (NULL != hObj->instObj->outCoreOps->control));
                retVal = hObj->instObj->outCoreOps->control(
                             hObj->outCoreHandle,
                             VCORE_IOCTL_VIP_ALLOC_PATH,
                             NULL);
            }

            if (FVID2_SOK == retVal)
            {
                for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
                {
                    GT_assert(VpsMdrvScTrace,
                              (NULL != hObj->instObj->outCoreOps->control));

                    retVal = hObj->instObj->outCoreOps->control(
                                 hObj->outCoreHandle,
                                 VCORE_IOCTL_VIP_SET_CONFIG,
                                 &chCnt);
                    if (FVID2_SOK != retVal)
                    {
                        GT_1trace(VpsMdrvScTrace, GT_ERR,
                            "Set Config for %d channel in VIP core failed!\n",
                            chCnt);
                        break;
                    }
                }
            }
        }

        if (FVID2_SOK != retVal)
        {
            /* if setting of params failed close both the core handles */
            vpsMdrvScCloseCores(hObj);
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScCloseCores
 *  \brief Closes the core handles
 */
static Int32 vpsMdrvScCloseCores(VpsMdrv_ScHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Property      property;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    /* Even if core returns error while closing we are continuing
     * closing the driver */
    GT_assert(VpsMdrvScTrace, (NULL != hObj->instObj->inCoreOps->close));
    GT_assert(VpsMdrvScTrace, (NULL != hObj->instObj->outCoreOps->close));
    hObj->instObj->inCoreOps->close(hObj->inCoreHandle);

    GT_assert(VpsMdrvScTrace, (NULL != hObj->instObj->outCoreOps->getProperty));

    retVal = hObj->instObj->outCoreOps->getProperty(
                 hObj->outCoreHandle,
                 &property);

    GT_assert(VpsMdrvScTrace, (NULL != hObj->instObj->outCoreOps->control));

    if (VCORE_TYPE_VIP == property.name)
    {
        hObj->instObj->outCoreOps->control(
            hObj->outCoreHandle,
            VCORE_IOCTL_VIP_FREE_PATH,
            NULL);
    }

    hObj->instObj->outCoreOps->close(hObj->outCoreHandle);

    return (retVal);
}

/**
 *  vpsMdrvScGetScFactorConfig
 *  \brief Gets the Scaling Factor Configuration for all channels.
 */
static Int32 vpsMdrvScGetScFactorConfig(VpsMdrv_ScHandleObj *hObj,
                                        UInt32 chNum,
                                        UInt32 descSet)
{
    Int32               retVal = FVID2_SOK;
    UInt32              numCh, chCnt, descSetCnt;
    VpsMdrv_ScChObj    *chObj;
    Vcore_ScCoeffParams coeffPrms;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    GT_assert(VpsMdrvScTrace,
        (NULL != hObj->instObj->outCoreOps->getScFactorConfig));

    /* For the m2mSc driver, inCore does not have a scalar. Only the outCore has
     * scalar SC5 or VIP scalars SC3/SC4 in the path. Hence, call the core
     * function to get the scalar config only for the outCore.
     * Do this for all channels.
     */
    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
    {
        /* If the configuration is applied per handle, only the config in
         * channel 0 is valid.
         */
        numCh = 1;
    }
    else
    {
        /* Get the scalar config for all the channels if the configuration is
         * different for all the channels of the handle
         */
        numCh = hObj->numCh;
    }

    /* A value of -1 for descSet means all descriptor sets must be processed.*/
    if ((UInt32) -1 == descSet)
    {
        descSetCnt = 0;
    }
    else
    {
        descSetCnt = descSet;
    }

    for (; descSetCnt < VPSMDRV_SC_MAX_DESC_SET; descSetCnt++)
    {
        /* A value of -1 for chNum means all channels should be processed. */
        if ((UInt32) -1 == chNum)
        {
            chCnt = 0;
        }
        else
        {
            chCnt = chNum;
        }

        for (; chCnt < numCh; chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            GT_assert(VpsMdrvScTrace, (NULL != chObj));
            /* For outCore, coreCnt is 0. */
            retVal = hObj->instObj->outCoreOps->getScFactorConfig(
                        hObj->outCoreHandle,
                        chCnt,
                        &(chObj->scFactor),
                        &(chObj->coreDescMem[descSetCnt]
                                            [VPSMDRV_SC_OUT_CORE_IDX]));
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvScTrace, GT_ERR,
                    "EFAIL: Getting the Scaling Factor Config Failed:"
                    " ChNum %d\n",
                    chCnt);
                break;
            }


            /* Break the loop if only the specified channel number is to be
             * processed.
             */
            if ((UInt32) -1 != chNum)
            {
                break;
            }
        }

        if (FVID2_SOK != retVal)
        {
            break;
        }

        /* Break the loop if only the specified descriptor set is to be
         * processed.
         */
        if ((UInt32) -1 != descSet)
        {
            break;
        }
    }

    /* Now call the function to program SC coeff in order to get the
     * pointers to the corresponding coeff memories based on the scaling
     * factors just received.
     * Even if one descriptor set is already under progress, we can still do
     * this to get the new horizontal & vertical scaling coefficient pointers,
     * since we are not changing any actual coefficient memory, but just setting
     * the pointers in the channel object. The existing descriptor set would
     * already be using the correct coefficients as set previously, and that
     * would not be disturbed. Only the next list submission will use the new
     * scalar coefficient set pointers.
     */
    if (FVID2_SOK == retVal)
    {
        /* A value of -1 for chNum means all channels should be processed. */
        if ((UInt32) -1 == chNum)
        {
            chCnt = 0;
        }
        else
        {
            chCnt = chNum;
        }

        for (; chCnt < numCh; chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            GT_assert(VpsMdrvScTrace, (NULL != chObj));

            /* Call this only if the scaling factors are valid, indicating that
             * the scalar is not in bypass.
             */
            if ((VPS_SC_HST_MAX != chObj->scFactor.hsType) &&
                (VPS_SC_VST_MAX != chObj->scFactor.vsType))
            {
                /* NULL pointer check */
                GT_assert(VpsMdrvScTrace,
                    (NULL != hObj->instObj->outCoreOps->programScCoeff));

                coeffPrms.hScalingSet = chObj->scFactor.hScalingSet;
                coeffPrms.vScalingSet = chObj->scFactor.vScalingSet;
                coeffPrms.horzCoeffMemPtr = NULL; /* Returned by core */
                coeffPrms.vertCoeffMemPtr = NULL; /* Returned by core */
                coeffPrms.vertBilinearCoeffMemPtr = NULL; /* Returned by core */
                coeffPrms.userCoeffPtr = NULL;

                /* Get and store the pointers to coefficient memory for
                 * this channel.
                 */
                retVal = hObj->instObj->outCoreOps->programScCoeff(
                             hObj->outCoreHandle,
                             0u,
                             &coeffPrms);
                if (FVID2_SOK == retVal)
                {
                    GT_assert(VpsMdrvScTrace,
                        (NULL != coeffPrms.horzCoeffMemPtr));
                    /* coreCnt is 0u for output core. */
                    chObj->horzCoeffMemPtr = coeffPrms.horzCoeffMemPtr;
                    GT_assert(VpsMdrvScTrace,
                        (NULL != coeffPrms.vertCoeffMemPtr));
                    chObj->vertCoeffMemPtr = coeffPrms.vertCoeffMemPtr;
                }
                else
                {
                    GT_0trace(VpsMdrvScTrace, GT_ERR,
                        "Could not get coeff memory pointers\n");
                }
            }

            if ((UInt32) -1 != chNum)
            {
                break;
            }
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScInitDescAndRegs
 *  \brief Intiliaze all fields of data descriptors except the buffer
 *  address.  Each queue and dequeue will only update the buffer addresses
 *  and run time params if any. Also initialize the overlay memories according
 *  to the configuration of each driver
 */
static Int32 vpsMdrvScInitDescAndRegs(VpsMdrv_ScHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, descSet;
    VpsMdrv_ScChObj     *chObj;
    const Vcore_Ops     *inCoreOps, *outCoreOps;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    inCoreOps = hObj->instObj->inCoreOps;
    outCoreOps = hObj->instObj->outCoreOps;
    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
        {
            GT_assert(VpsMdrvScTrace, (NULL != inCoreOps->programDesc));
            GT_assert(VpsMdrvScTrace, (NULL != outCoreOps->programDesc));
            retVal  = inCoreOps->programDesc(
                          hObj->inCoreHandle,
                          chCnt,
                          0u,
                          &chObj->coreDescMem[descSet]
                                             [VPSMDRV_SC_IN_CORE_IDX]);
            retVal += outCoreOps->programDesc(
                          hObj->outCoreHandle,
                          chCnt,
                          0u,
                          &chObj->coreDescMem[descSet]
                                             [VPSMDRV_SC_OUT_CORE_IDX]);

            GT_assert(VpsMdrvScTrace, (NULL != inCoreOps->programReg));
            GT_assert(VpsMdrvScTrace, (NULL != outCoreOps->programReg));
            retVal += inCoreOps->programReg(
                          hObj->inCoreHandle,
                          chCnt,
                          &chObj->coreDescMem[descSet][VPSMDRV_SC_IN_CORE_IDX]);

            retVal += outCoreOps->programReg(
                          hObj->outCoreHandle,
                          chCnt,
                          &chObj->coreDescMem[descSet]
                                             [VPSMDRV_SC_OUT_CORE_IDX]);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvScTrace, GT_DEBUG,
                    "Program of the Desc/Overlay Memory failed\n");
                break;
            }

        }

        if (FVID2_SOK != retVal)
        {
            break;
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScCreateCfgDescs
 *  \brief Create the configuration descriptors for shadow overlays memory
 */
Void vpsMdrvScCreateCfgDescs(VpsMdrv_ScHandleObj *hObj)
{
    UInt32              chCnt, descSet, i;
    UInt32              sochCnt;
    UInt8              *descPtr;
    VpsMdrv_ScChObj    *chObj;
    VpsMdrv_ScChObj    *sPadChObj;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    /* If configuration is per channel update coeff and config descriptors for
     * all channels. Coeff descriptors are initially set as dummy descriptors
     * only. */
    if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
    {
        for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
        {
            chObj = hObj->chObjs[chCnt];
            for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
            {
                for (i = 0; i < chObj->numCoeffDesc; i++)
                {
                    VpsHal_vpdmaCreateDummyDesc(chObj->coeffDesc[descSet][i]);
                }
                VpsHal_vpdmaCreateConfigDesc(
                    chObj->nshwCfgDesc[descSet],
                    VPSHAL_VPDMA_CONFIG_DEST_MMR,
                    VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    (chObj->totalNshwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] +
                        chObj->totalNshwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX]),
                    chObj->coreDescMem[descSet]
                        [VPSMDRV_SC_IN_CORE_IDX].nonShadowOvlyMem,
                    NULL,
                    0u);
                VpsHal_vpdmaCreateConfigDesc(
                    chObj->shwCfgDesc[descSet],
                    VPSHAL_VPDMA_CONFIG_DEST_MMR,
                    VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    (chObj->totalShwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] +
                        chObj->totalShwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX]),
                    chObj->coreDescMem[descSet]
                        [VPSMDRV_SC_IN_CORE_IDX].shadowOvlyMem,
                    NULL,
                    0u);

                /* Set the non shadow overlay memeory for the proper frame start
                 * event and the  proper line mode. This is required only
                 * for the read clients */
                GT_assert(VpsMdrvScTrace,
                    (NULL != hObj->instObj->inCoreOps->setFsEvent));
                hObj->instObj->inCoreOps->setFsEvent(
                    hObj->inCoreHandle,
                    chCnt,
                    VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE,
                    &chObj->coreDescMem[descSet][VPSMDRV_SC_IN_CORE_IDX]);
            }
        }
    }
    /* Else if configuration is per handle update only the config descriptor
     * for handle.  Configuration like the size of the overlay memory etc
     * will be hold by the first channel object */
    else
    {
        chObj = hObj->chObjs[0];
        for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
        {
            for (i = 0; i < chObj->numCoeffDesc; i++)
            {
                VpsHal_vpdmaCreateDummyDesc(hObj->coeffDesc[descSet][i]);
            }
            VpsHal_vpdmaCreateConfigDesc(
                hObj->nshwCfgDesc[descSet],
                VPSHAL_VPDMA_CONFIG_DEST_MMR,
                VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                (chObj->totalNshwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] +
                    chObj->totalNshwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX]),
                chObj->coreDescMem[descSet]
                    [VPSMDRV_SC_IN_CORE_IDX].nonShadowOvlyMem,
                NULL,
                0u);
            VpsHal_vpdmaCreateConfigDesc(
                hObj->shwCfgDesc[descSet],
                VPSHAL_VPDMA_CONFIG_DEST_MMR,
                VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                (chObj->totalShwOvlyMem[VPSMDRV_SC_IN_CORE_IDX] +
                    chObj->totalShwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX]),
                chObj->coreDescMem[descSet]
                    [VPSMDRV_SC_IN_CORE_IDX].shadowOvlyMem,
                NULL,
                0u);

            /* Set the non shadow overlay memeory for the proper frame start
             * event and the  proper line mode. This is required only
             * for the read clients */
            GT_assert(VpsMdrvScTrace,
                (NULL != hObj->instObj->inCoreOps->setFsEvent));
            hObj->instObj->inCoreOps->setFsEvent(
                hObj->inCoreHandle,
                0,
                VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE,
                &chObj->coreDescMem[descSet][VPSMDRV_SC_IN_CORE_IDX]);
        }
    }

    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        sPadChObj = hObj->sPadChObjs[chCnt];
        for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
        {
            descPtr = (UInt8 *) chObj->sochDesc[descSet];
            for (sochCnt = 0; sochCnt < chObj->numSochDesc; sochCnt++)
            {
                VpsHal_vpdmaCreateSOCHCtrlDesc(
                    descPtr,
                   (VpsHal_VpdmaChannel) chObj->sochChNum[sochCnt]);
                descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            descPtr = (UInt8 *) sPadChObj->sochDesc[descSet];
            for (sochCnt = 0; sochCnt < chObj->numSochDesc; sochCnt++)
            {
                VpsHal_vpdmaCreateSOCHCtrlDesc(
                    descPtr,
                   (VpsHal_VpdmaChannel) chObj->sochChNum[sochCnt]);
                descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
           }
        }
    }

    return;
}

/**
 *  vpsMdrvScReturnErrorProclist
 *  \brief Returns the error callback to application if the processing of the
 *  frame fails after queueing it.
 */
Void vpsMdrvScReturnErrorProclist(VpsMdrv_ScHandleObj *hObj,
                                  VpsMdrv_ScQueueObj *qObj)
{
    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    /* Copy the error process list to the err process
     * list container give byy app */
    /* Call the error call back, if not NULL */
    if (hObj->fdmCbPrms.fdmErrCbFxn)
    {
        FVID2_copyProcessList(hObj->fdmCbPrms.errList, &qObj->procList);

        hObj->fdmCbPrms.fdmErrCbFxn(
            hObj->fdmCbPrms.fdmData,
            hObj->fdmCbPrms.errList,
            NULL);
    }

    /* Decrement the request count by 1 */
    hObj->numPendReq--;
    /* Return the queue object back to the free queue
     * handle
     */
    VpsUtils_queue(hObj->instObj->freeQ, &qObj->qElem, qObj);

    return;
}

/**
 *  vpsMdrvScSplitOvlyMem
 *  \brief Update the pointers to the overlay  memories
 *  for each channel and each set of the channel
 */
Void vpsMdrvScSplitOvlyMem(VpsMdrv_ScHandleObj *hObj)
{
    UInt32              chCnt, descSet;
    UInt8              *shadowMemPtr, *nshwMemPtr;
    VpsMdrv_ScChObj    *chObj;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        /* If the driver is opened in per handle configuration all the
         * channels will point to the same overlay memory.  If the
         * driver is opened in the per channel configuration all the
         * channels will point to its own overlay memory */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            shadowMemPtr = chObj->shwOvlyMem;
            nshwMemPtr = chObj->nshwOvlyMem;
        }
        else
        {
            shadowMemPtr = hObj->shwOvlyMem;
            nshwMemPtr = hObj->nshwOvlyMem;
        }
        for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
        {
            chObj->coreDescMem[descSet]
                [VPSMDRV_SC_IN_CORE_IDX].shadowOvlyMem = shadowMemPtr;
            shadowMemPtr += chObj->totalShwOvlyMem[VPSMDRV_SC_IN_CORE_IDX];
            chObj->coreDescMem[descSet]
                [VPSMDRV_SC_OUT_CORE_IDX].shadowOvlyMem = shadowMemPtr;
            shadowMemPtr += chObj->totalShwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX];

            /* Calaculation for non-shadow */
            chObj->coreDescMem[descSet]
                [VPSMDRV_SC_IN_CORE_IDX].nonShadowOvlyMem = nshwMemPtr;
            nshwMemPtr += chObj->totalNshwOvlyMem[VPSMDRV_SC_IN_CORE_IDX];
            chObj->coreDescMem[descSet]
                [VPSMDRV_SC_OUT_CORE_IDX].nonShadowOvlyMem = nshwMemPtr;
            nshwMemPtr += chObj->totalNshwOvlyMem[VPSMDRV_SC_OUT_CORE_IDX];
        }
    }

    return;
}

/**
 *  vpsMdrvScSplitDescMem
 *  \brief Maximum number of descriptors are allocated for each channel
 *  now each channel needs to have reference to its data descriptor, config
 *  descriptor, reload descriptor and control descriptors. So this function
 *  updates all the references for particular channel from the chunk of
 *  descriptor allocated
 */
Void vpsMdrvScSplitDescMem(VpsMdrv_ScHandleObj *hObj)
{
    UInt32              chCnt, descSet, i;
    UInt32              cnt;
    UInt8              *descPtr;
    VpsMdrv_ScChObj    *chObj;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        descPtr = chObj->descMem;
        for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
        {
            /* No coeff or config descriptors if the configuration is per
             * handle */
            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                /* First are the coeff descriptor pointers. */
                for (i = 0; i < chObj->numCoeffDesc; i++)
                {
                    chObj->coeffDesc[descSet][i] =
                        (VpsHal_VpdmaConfigDesc *) descPtr;
                    descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
                }
                chObj->totalCoeffDescMem =
                    (chObj->numCoeffDesc * VPSHAL_VPDMA_CONFIG_DESC_SIZE);

                /* Now setup the config descriptor pointer. */
                chObj->nshwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += (VPSMDRV_SC_MAX_NSHW_CFG_DESC *
                            VPSHAL_VPDMA_CONFIG_DESC_SIZE);

                chObj->shwCfgDesc[descSet] = (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += (VPSMDRV_SC_MAX_SHW_CFG_DESC *
                            VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            }

            for (cnt = 0; cnt < chObj->numOutDataDesc; cnt++)
            {
                chObj->coreDescMem[descSet]
                    [VPSMDRV_SC_OUT_CORE_IDX].outDataDesc[cnt] = descPtr;
                descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }

            /* At create time, lazy loading is disabled.
             * If the configuration is per channel first descriptor in the
             * layout will be set to config descriptor to avoid impact when
             * lazy loading is disabled. If lazy loading is enabled later, it
             * will be set to the coeff descriptor. */
            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                chObj->firstDesc[descSet] = chObj->nshwCfgDesc[descSet];
            }
            /* else if the configuration is per handle first descriptor in the
             * layout will be first outbound data descriptor */
            else
            {
                chObj->firstDesc[descSet] = chObj->coreDescMem[descSet]
                    [VPSMDRV_SC_OUT_CORE_IDX].outDataDesc[0];
            }

            for (cnt = 0; cnt < chObj->numInDataDesc; cnt++)
            {
                chObj->coreDescMem[descSet]
                    [VPSMDRV_SC_IN_CORE_IDX].inDataDesc[cnt] = descPtr;
                descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }

            chObj->sochDesc[descSet] =
                (VpsHal_VpdmaSyncOnChannelDesc *) descPtr;
            descPtr += chObj->numSochDesc * VPSHAL_VPDMA_CTRL_DESC_SIZE;

            chObj->rldDesc[descSet] = (VpsHal_VpdmaReloadDesc *) descPtr;
            descPtr += VPSMDRV_SC_MAX_RLD_DESC * VPSHAL_VPDMA_CTRL_DESC_SIZE;

            /* Get the size of the list to be posted to VPDMA */
            chObj->descListSize =
                ((UInt32) descPtr) - ((UInt32) chObj->firstDesc[descSet]);
        }
    }

    return;
}

/**
 *  vpsMdrvScSplitSPadDescMem
 *  \brief update the pointers to the data, config, reload
 *  descriptors for each channel and each set of the channel
 *  No of data descriptors and config descriptors required for each channel
 *  is always same in any configuration for this M2M driver.  For the other
 *  drivers where the number of descriptors are not fixed slicing needs to be
 *  done at runt time since the scratch pad channels objects are not tied to
 *  any particular channel. Here taking the first channel object
 *  configuration for slicing.
 */
Void vpsMdrvScSplitSPadDescMem(VpsMdrv_ScHandleObj *hObj)
{
    UInt32              chCnt, descSet, i;
    UInt32              cnt;
    UInt8              *descPtr;
    VpsMdrv_ScChObj    *sPadChObj;
    VpsMdrv_ScChObj    *chObj;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    chObj = hObj->chObjs[0];
    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        sPadChObj = hObj->sPadChObjs[chCnt];
        descPtr = sPadChObj->descMem;
        for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
        {
            /* No coeff or config descriptors if the configuration is per
             * handle */
            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                /* First are the coeff descriptor pointers. */
                for (i = 0; i < chObj->numCoeffDesc; i++)
                {
                    sPadChObj->coeffDesc[descSet][i] =
                        (VpsHal_VpdmaConfigDesc *) descPtr;
                    descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
                }
                sPadChObj->totalCoeffDescMem =
                    (chObj->numCoeffDesc * VPSHAL_VPDMA_CONFIG_DESC_SIZE);

                /* Now setup the config descriptor pointer. */
                sPadChObj->nshwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += (VPSMDRV_SC_MAX_NSHW_CFG_DESC *
                            VPSHAL_VPDMA_CONFIG_DESC_SIZE);

                sPadChObj->shwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += (VPSMDRV_SC_MAX_SHW_CFG_DESC *
                            VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            }

            for (cnt = 0; cnt < chObj->numOutDataDesc; cnt++)
            {
                sPadChObj->coreDescMem[descSet]
                    [VPSMDRV_SC_OUT_CORE_IDX].outDataDesc[cnt] = descPtr;
                descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }

            /* At create time, lazy loading is disabled.
             * If the configuration is per channel first descriptor in the
             * layout will be set to config descriptor to avoid impact when
             * lazy loading is disabled. If lazy loading is enabled later, it
             * will be set to the coeff descriptor. */
            if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
            {
                sPadChObj->firstDesc[descSet] = sPadChObj->nshwCfgDesc[descSet];
            }
            /* else if the configuration is per handle first descriptor in the
             * layout will be first outbound data descriptor */
            else
            {
                sPadChObj->firstDesc[descSet] = sPadChObj->coreDescMem[descSet]
                    [VPSMDRV_SC_OUT_CORE_IDX].outDataDesc[0];
            }

            for (cnt = 0; cnt < chObj->numInDataDesc; cnt++)
            {
                sPadChObj->coreDescMem[descSet]
                    [VPSMDRV_SC_IN_CORE_IDX].inDataDesc[cnt] = descPtr;
                descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }

            sPadChObj->sochDesc[descSet] =
                (VpsHal_VpdmaSyncOnChannelDesc *) descPtr;
            descPtr += chObj->numSochDesc * VPSHAL_VPDMA_CTRL_DESC_SIZE;

            sPadChObj->rldDesc[descSet] = (VpsHal_VpdmaReloadDesc *) descPtr;
            descPtr += VPSMDRV_SC_MAX_RLD_DESC * VPSHAL_VPDMA_CTRL_DESC_SIZE;

            /* Get the size of the list to be posted to VPDMA */
            sPadChObj->descListSize =
                ((UInt32) descPtr) - ((UInt32) sPadChObj->firstDesc[descSet]);
        }
    }

    return;
}

/*
 * vpsMdrvScSubFrmUpdate
 * \brief Update procList Rtparams with subframe info calculated
 */
void vpsMdrvScSubFrmUpdate(VpsMdrv_SubFrameChInst *slcData,
                           FVID2_Frame *inFrm,
                           FVID2_Frame *outFrm,
                           Vps_M2mScRtParams *rtParams,
                           Vps_M2mScChParams *chParam,
                           UInt32 hSlcNum)
{
    UInt32             yPitch, cPitch, outputPitch;
    Vps_FrameParams   *inFrmPrms   = rtParams->inFrmPrms;
    Vps_FrameParams   *outFrmPrms  = rtParams->outFrmPrms;
    Vps_ScRtConfig    *scRtparams  = rtParams->scCfg;
    Vps_CropConfig    *srcCropCfg  = rtParams->srcCropCfg;

    outputPitch = chParam->outFmt.pitch[FVID2_YUV_INT_ADDR_IDX];

    /* update outFrm start address with required offsets for current subframe */
    outFrm->addr[FVID2_FRAME_ADDR_IDX]
                [FVID2_YUV_INT_ADDR_IDX] =
                (char *) outFrm->addr[FVID2_FRAME_ADDR_IDX]
                                     [FVID2_YUV_INT_ADDR_IDX]
                + (outputPitch * slcData->tarBufOffset) +
                slcData->hSubFrmInfo.slcCfg[hSlcNum].tarBufOffset * 2;

    if (VPS_VPDMA_MT_TILEDMEM == chParam->inMemType)
    {
        yPitch = VPSUTILS_TILER_CNT_8BIT_PITCH;
        cPitch = VPSUTILS_TILER_CNT_16BIT_PITCH;
    }
    else
    {
        yPitch = chParam->inFmt.pitch[0u];
        cPitch = chParam->inFmt.pitch[1u];
    }

    /* update inFrm start addresses with required offsets for current subframe */
    if (slcData->dataFmt == FVID2_DF_YUV420SP_UV)
    {
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_SP_Y_ADDR_IDX] =
            (char *)inFrm->addr[FVID2_FRAME_ADDR_IDX]
                               [FVID2_YUV_SP_Y_ADDR_IDX]
                   + (yPitch * slcData->srcBufOffset)
                   + slcData->hSubFrmInfo.slcCfg[hSlcNum].srcBufOffset;
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_SP_CBCR_ADDR_IDX] =
            (char *) inFrm->addr[FVID2_FRAME_ADDR_IDX]
                                [FVID2_YUV_SP_CBCR_ADDR_IDX]
                   + (cPitch * ((slcData->srcBufOffset) >> 1))
                   + slcData->hSubFrmInfo.slcCfg[hSlcNum].srcBufOffset;
        inFrmPrms->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = yPitch;
        inFrmPrms->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = cPitch;
    }
    else if (slcData->dataFmt == FVID2_DF_YUV422SP_UV)
    {
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_SP_Y_ADDR_IDX] =
            (char *)inFrm->addr[FVID2_FRAME_ADDR_IDX]
                               [FVID2_YUV_SP_Y_ADDR_IDX]
                   + (yPitch * slcData->srcBufOffset)
                   + slcData->hSubFrmInfo.slcCfg[hSlcNum].srcBufOffset;
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_SP_CBCR_ADDR_IDX] =
            (char *) inFrm->addr[FVID2_FRAME_ADDR_IDX]
                                [FVID2_YUV_SP_CBCR_ADDR_IDX]
                   + (cPitch * slcData->srcBufOffset)
                   + slcData->hSubFrmInfo.slcCfg[hSlcNum].srcBufOffset;
        inFrmPrms->pitch[FVID2_YUV_SP_Y_ADDR_IDX] = yPitch;
        inFrmPrms->pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] = cPitch;
    }
    /* 422 input case*/
    else
    {
        inFrm->addr[FVID2_FRAME_ADDR_IDX]
                   [FVID2_YUV_INT_ADDR_IDX] =
             (char *) inFrm->addr[FVID2_FRAME_ADDR_IDX]
                                 [FVID2_YUV_INT_ADDR_IDX]
                    + (yPitch * slcData->srcBufOffset)
                    + (slcData->hSubFrmInfo.slcCfg[hSlcNum].srcBufOffset * 2u);
        inFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] = yPitch;
    }

    /* update rtPrms with calculated values for current subframe */
    inFrmPrms->width =
        slcData->hSubFrmInfo.slcCfg[hSlcNum].srcW +
            (slcData->hSubFrmInfo.slcCfg[hSlcNum].chrUsPixMemSlcLft +
             slcData->hSubFrmInfo.slcCfg[hSlcNum].chrUsPixMemSlcRgt) +
             (slcData->hSubFrmInfo.slcCfg[hSlcNum].scCropPixLft +
              slcData->hSubFrmInfo.slcCfg[hSlcNum].scCropPixRgt);

    //?? 3 for RAV, how it handles for chroma
    inFrmPrms->height =
        slcData->srcH + (slcData->chrUsLnMemSlcTop + slcData->chrUsLnMemSlcBtm);
    inFrmPrms->dataFormat = chParam->inFmt.dataFormat;

    outFrmPrms->width = slcData->hSubFrmInfo.slcCfg[hSlcNum].tarW;
    outFrmPrms->height = slcData->tarH;
    outFrmPrms->pitch[FVID2_YUV_INT_ADDR_IDX] = outputPitch;

    scRtparams->scBypass = FALSE;
    scRtparams->scSetPhInfo = TRUE;

    if (slcData->filtType == VPS_SC_VST_POLYPHASE)
    {
        scRtparams->rowAccInc = slcData->rowAccIncr;
        scRtparams->rowAccOffset = slcData->rowAccOffset;
        scRtparams->rowAccOffsetB = slcData->rowAccOffsetB;
    }
    else
    {
        scRtparams->ravScFactor = slcData->ravScFactor;
        scRtparams->ravRowAccInit = slcData->ravRowAccInit;
        scRtparams->ravRowAccInitB = slcData->ravRowAccInitB;
    }

    scRtparams->linAccIncr = slcData->hSubFrmInfo.linAccIncr;
    scRtparams->colAccOffset = slcData->hSubFrmInfo.slcCfg[hSlcNum].colAccOffset;

    srcCropCfg->cropHeight = slcData->srcH;
    srcCropCfg->cropWidth = slcData->hSubFrmInfo.slcCfg[hSlcNum].srcW;
    srcCropCfg->cropStartX = slcData->hSubFrmInfo.slcCfg[hSlcNum].scCropPixLft +
                                slcData->hSubFrmInfo.slcCfg[hSlcNum].chrUsPixMemSlcLft;
    srcCropCfg->cropStartY = slcData->scCropLnsTop;

    inFrmPrms->memType = chParam->inMemType;
    outFrmPrms->memType = chParam->outMemType;

    return;
}

/**
 *  vpsMdrvScCopyHSubFrmInfo
 *  \brief
 */
static Int32 vpsMdrvScCopyHSubFrmInfo(VpsMdrv_ScHandleObj *hObj,
                                      Vps_SubFrameHorzSubFrmInfo *hSlcInfo)
{
    Int32               retVal = FVID2_SOK;
    UInt32              slcCnt, tarSrcW = 0;
    VpsMdrv_ScChObj    *chObj;

    if (NULL == hSlcInfo)
    {
        GT_0trace(VpsMdrvScTrace, GT_ERR,
            "BADARGS: NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        if (hSlcInfo->chNum > hObj->numCh)
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "BADARGS: Wrong Channel Number\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
    if (FVID2_SOK == retVal)
    {
        if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
        {
            chObj = hObj->chObjs[0];
        }
        else
        {
            chObj = hObj->chObjs[hSlcInfo->chNum];
        }

        if (FALSE == chObj->slcChObj.subFrameMdEnable)
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "Wrong Ioctl: Invalid IOCTL for this mode\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        hSlcInfo->numSubFrm = chObj->slcChObj.hSubFrmInfo.noOfSlcs;
        hSlcInfo->subFrmSize = chObj->slcChObj.hSubFrmInfo.slcSz;
        for (slcCnt = 0;
             slcCnt < hSlcInfo->numSubFrm;
             slcCnt++)
        {
            hSlcInfo->subFrmCfg[slcCnt].srcW =
                chObj->slcChObj.hSubFrmInfo.slcCfg[slcCnt].fstSrcW;

            /* Mask the last pixel if it is odd target width. This make last
               pixel masked out from the output image */
            if ((slcCnt == (hSlcInfo->numSubFrm - 1)) &&
                ((tarSrcW + chObj->slcChObj.hSubFrmInfo.
                                slcCfg[slcCnt].fstTarW) >
                                    chObj->slcChObj.tarFrmW))
            {
                hSlcInfo->subFrmCfg[slcCnt].tarW =
                    chObj->slcChObj.tarFrmW - tarSrcW;
            }
            else
            {
                hSlcInfo->subFrmCfg[slcCnt].tarW =
                    chObj->slcChObj.hSubFrmInfo.slcCfg[slcCnt].fstTarW & (~(0x1));
            }

            tarSrcW += hSlcInfo->subFrmCfg[slcCnt].tarW;
        }
    }

    return (retVal);
}


static Int32 vpsMdrvScSetChPrms(VpsMdrv_ScHandleObj *hObj,
                                UInt32 chNum)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Format        inVcoreFmt, outVcoreFmt;
    Vcore_SwpParams     swpParams;
    Vcore_VipParams     vipPrms;
    Vps_M2mScChParams  *chPrm;
    VpsMdrv_ScChObj    *chObj;
    Vcore_Property      property;

    chObj = hObj->chObjs[chNum];
    chPrm = hObj->chParams[chNum];
    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    GT_assert(VpsMdrvScTrace, (NULL != chObj));
    GT_assert(VpsMdrvScTrace, (NULL != chPrm));

    GT_assert(VpsMdrvScTrace,
          (NULL != hObj->instObj->outCoreOps->getProperty));
    retVal = hObj->instObj->outCoreOps->getProperty(
                 hObj->outCoreHandle,
                 &property);

    if (FVID2_SOK == retVal)
    {
        inVcoreFmt.memType = chPrm->inMemType;
        inVcoreFmt.frameWidth = chPrm->inFmt.width;
        inVcoreFmt.frameHeight = chPrm->inFmt.height;
        inVcoreFmt.startX = 0;
        inVcoreFmt.startY = 0;
        inVcoreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;

        VpsUtils_memcpy(
            &inVcoreFmt.fmt,
            &chPrm->inFmt,
            sizeof(FVID2_Format));
        /* Copy the slice sizes as the frame size in the core format */
        if (chObj->slcChObj.subFrameMdEnable == TRUE)
        {
            inVcoreFmt.frameWidth = inVcoreFmt.fmt.width = chObj->slcChObj.hSubFrmInfo.slcSz;
            inVcoreFmt.frameHeight = inVcoreFmt.fmt.height = chObj->slcChObj.slcSz;
        }

        GT_assert(VpsMdrvScTrace,
            (NULL != hObj->instObj->inCoreOps->setFormat));
        /* Set the format and other parameters for the cores */
        retVal = hObj->instObj->inCoreOps->setFormat(
                     hObj->inCoreHandle,
                     chNum,
                     &inVcoreFmt);
    }

    if (FVID2_SOK == retVal)
    {
        if (VCORE_TYPE_VIP != property.name)
        {
            outVcoreFmt.frameWidth = chPrm->outFmt.width;
            outVcoreFmt.frameHeight = chPrm->outFmt.height;
            outVcoreFmt.startX = 0;
            outVcoreFmt.startY = 0;
            outVcoreFmt.secScanFmt = chPrm->outFmt.scanFormat;
            outVcoreFmt.memType = chPrm->outMemType;
            swpParams.srcWidth = chPrm->inFmt.width;
            swpParams.srcHeight = chPrm->inFmt.height;
            VpsUtils_memcpy(
                &outVcoreFmt.fmt,
                &chPrm->outFmt,
                sizeof(FVID2_Format));
            VpsUtils_memcpy(
                &swpParams.scCfg,
                chPrm->scCfg,
                sizeof(Vps_ScConfig));
            VpsUtils_memcpy(
                &swpParams.srcCropCfg,
                chPrm->srcCropCfg,
                sizeof(Vps_CropConfig));

            /* Cropping cannot be used when slice based scaling is enabled,
               so copying slice size into crop sizes */
            if (chObj->slcChObj.subFrameMdEnable == TRUE)
            {
                swpParams.srcCropCfg.cropWidth = swpParams.srcWidth =
                    chObj->slcChObj.hSubFrmInfo.slcSz;
                swpParams.srcCropCfg.cropHeight = swpParams.srcHeight =
                    chObj->slcChObj.slcSz;
            }

            GT_assert(VpsMdrvScTrace,
                (NULL != hObj->instObj->outCoreOps->setParams));
            retVal = hObj->instObj->outCoreOps->setParams(
                         hObj->outCoreHandle,
                         chNum,
                         &swpParams);
            GT_assert(VpsMdrvScTrace,
                (NULL != hObj->instObj->outCoreOps->setFormat));
            retVal += hObj->instObj->outCoreOps->setFormat(
                          hObj->outCoreHandle,
                          chNum,
                          &outVcoreFmt);
        }
        else
        {
            vipPrms.srcWidth = chPrm->inFmt.width;
            vipPrms.memType = chPrm->outMemType;
            vipPrms.srcHeight = chPrm->inFmt.height;
            vipPrms.isScReq = TRUE;
            VpsUtils_memcpy(
                &vipPrms.scCfg,
                chPrm->scCfg,
                sizeof(Vps_ScConfig));
            VpsUtils_memcpy(
                &vipPrms.cropCfg,
                chPrm->srcCropCfg,
                sizeof(Vps_CropConfig));
            VpsUtils_memcpy(
                &vipPrms.fmt,
                &chPrm->outFmt,
                sizeof(FVID2_Format));

            /* Cropping cannot be used when slice based scaling is enabled,
               so copying slice size into crop sizes */
            if (chObj->slcChObj.subFrameMdEnable == TRUE)
            {
                vipPrms.cropCfg.cropWidth = vipPrms.srcWidth =
                    chObj->slcChObj.hSubFrmInfo.slcSz;
                vipPrms.cropCfg.cropHeight = vipPrms.srcHeight =
                    chObj->slcChObj.slcSz;
            }

            GT_assert(VpsMdrvScTrace,
              (NULL != hObj->instObj->outCoreOps->setParams));
            retVal = hObj->instObj->outCoreOps->setParams(
                         hObj->outCoreHandle,
                         chNum,
                         &vipPrms);
        }
    }

    return (retVal);
}

