/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *
 *  \brief VPS M2M driver for COMP (video + graphics blending, scaling and writeback).
 *  This file implements the VPS COMP M2M driver FVID2 level functions and sub-functions.
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
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/hal/vpshal_grpx.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
//#include <ti/psp/vps/core/vpscore_deiPath.h>
#include <ti/psp/vps/core/vpscore_graphics.h>
#include <ti/psp/vps/core/vpscore_scWrbkPath.h>
#include <ti/psp/vps/core/vpscore_vipRes.h>
#include <ti/psp/vps/core/vpscore_vip.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/drivers/m2m/vps_mlm.h>
#include <ti/psp/vps/drivers/m2m/vps_m2mInt.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSlice.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mComp.h>
#include <ti/psp/vps/vps_m2mComp.h>
#include <ti/psp/vps/drivers/m2m/src/vpsdrv_m2mCompPriv.h>


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
static Fdrv_Handle VpsMdrv_compCreate(UInt32 drvId,
                                     UInt32 instId,
                                     Ptr createArgs,
                                     Ptr createStatusArgs,
                                     const FVID2_DrvCbParams *fdmCbPrms);
static Int32 VpsMdrv_compDelete(Fdrv_Handle handle, Ptr deleteArgs);
static Int32 VpsMdrv_compMlmCbFxn(Mlm_SubmitReqInfo *reqInfo);
static Int32 vpsMdrvCompSetScCoeffIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs);

static Int32 VpsMdrv_compControl(Fdrv_Handle handle,
                                UInt32 cmd,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs);
static Int32 VpsMdrv_compProcessFrms(Fdrv_Handle handle,
                                    FVID2_ProcessList *procList);
static Int32 VpsMdrv_compGetProcessedFrms(Fdrv_Handle handle,
                                         FVID2_ProcessList *procList,
                                         UInt32 timeout);

static Int32 vpsMdrvCompProcessReq(VpsMdrv_CompHandleObj *hObj,
                                  VpsMdrv_CompQueueObj *qObj);
static Int32 vpsMdrvCompUpdateChDesc(VpsMdrv_CompHandleObj *hObj,
                                    VpsMdrv_CompChObj *chObj,
                                    FVID2_ProcessList *procList,
                                    UInt32 frmCnt,
                                    UInt32 descSet);
static Int32 vpsMdrvCompUpdateRtParams(VpsMdrv_CompHandleObj *hObj,
                                      VpsMdrv_CompChObj *chObj,
                                      const Vps_M2mCompRtParams *rtPrms,
                                      UInt32 descSet);

static Void vpsMdrvCompRetErrProcList(VpsMdrv_CompHandleObj *hObj,
                                     VpsMdrv_CompQueueObj *qObj);
static Int32 vpsMdrvCompCheckProcList(VpsMdrv_CompHandleObj *hObj,
                                     FVID2_ProcessList *procList);

static UInt32 vpsMdrvCompAllocDescSet(VpsMdrv_CompHandleObj *hObj);
static Int32 vpsMdrvCompFreeDescSet(VpsMdrv_CompHandleObj *hObj, UInt32 descSet);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** COMP M2M driver function pointer. */
static const FVID2_DrvOps VpsMdrvCompOps =
{
    FVID2_VPS_M2M_COMP_DRV,          /* Driver ID */
    VpsMdrv_compCreate,              /* Create */
    VpsMdrv_compDelete,              /* Delete */
#if 1
    VpsMdrv_compControl,             /* Control */
    NULL,                           /* Queue */
    NULL,                           /* Dequeue */
    VpsMdrv_compProcessFrms,         /* ProcessFrames */
    VpsMdrv_compGetProcessedFrms     /* GetProcessedFrames */
#else
    NULL,                           /* Queue */
    NULL,                           /* Queue */
    NULL,                           /* Queue */
    NULL,                           /* Queue */
    NULL,                           /* Queue */
#endif
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsMdrv_compInit
 *  \brief COMP M2M driver init function.
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
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_compInit(UInt32 numInst,
                      const VpsMdrv_CompInitParams *initPrms,
                      Ptr arg)
{
    Int32       retVal = FVID2_SOK;

    /* Check for errors */
    GT_assert(VpsMdrvCompTrace, (NULL != initPrms));

    /* Initialize the internal objects */
    retVal = vpsMdrvCompInit(numInst, initPrms);
    if (FVID2_SOK == retVal)
    {
        /* Register the driver to the driver manager */
        retVal = FVID2_registerDriver(&VpsMdrvCompOps);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR,
                "Registering to FVID2 driver manager failed\n");
            /* Uninitialize the internal objects if error occurs */
            vpsMdrvCompDeInit();
        }
    }

    return (retVal);
}



/**
 *  VpsMdrv_compDeInit
 *  \brief COMP M2M driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_compDeInit(Ptr arg)
{
    Int32       retVal = FVID2_SOK;

    /* Unregister from driver manager */
    retVal = FVID2_unRegisterDriver(&VpsMdrvCompOps);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Unregistering from FVID2 driver manager failed\n");
    }

    /* Uninitialize the internal objects */
    retVal |= vpsMdrvCompDeInit();

    return (retVal);
}


/**
 *  VpsMdrv_compCreate
 */
static Fdrv_Handle VpsMdrv_compCreate(UInt32 drvId,
                                     UInt32 instId,
                                     Ptr createArgs,
                                     Ptr createStatusArgs,
                                     const FVID2_DrvCbParams *fdmCbPrms)
{
    Int32                   retVal, tempRetVal;
    UInt32                  coreOpenFlag = FALSE, coreAllocFlag = FALSE;
    UInt32                  resrcAllocFlag = FALSE;
    VpsMdrv_CompInstObj     *instObj = NULL;
    Vps_M2mCompCreateParams *createPrms;
    VpsMdrv_CompHandleObj   *hObj = NULL;
    Vps_M2mCompCreateStatus *createStatus;

    /* Check for NULL pointers */
    if ((NULL == createArgs) ||
        (NULL == createStatusArgs) ||
        (NULL == fdmCbPrms))
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        /* Get the instance object for this instance */
        instObj = vpsMdrvCompGetInstObj(instId);
        if (NULL == instObj)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Invalid instance ID\n");
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
        Semaphore_pend(instObj->instSem, VPSMDRV_COMP_SEM_TIMEOUT);
    }

    if (FVID2_SOK == retVal)
    {
        /* Check if a instance is opened more than the supported handles */
        if (instObj->openCnt >= VPS_M2M_COMP_MAX_HANDLE_PER_INST)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Only %d handles supported per instance!\n",
                VPS_M2M_COMP_MAX_HANDLE_PER_INST);
            retVal = FVID2_EALLOC;
        }

        /* Check for valid create parameters */
        createPrms = (Vps_M2mCompCreateParams *) createArgs;
        if ((createPrms->numCh > VPS_M2M_COMP_MAX_CH_PER_INST) ||
            (createPrms->numCh <= 0u))
        {
            GT_2trace(VpsMdrvCompTrace, GT_ERR,
                "Invalid number of channels(%d) - Supported max channels %d\n",
                createPrms->numCh,
                VPS_M2M_COMP_MAX_CH_PER_INST);
            retVal = FVID2_EINVALID_PARAMS;
        }

        if (NULL == createPrms->chParams)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer: Ch Params\n");
            retVal = FVID2_EBADARGS;
        }
        /* graphics params can be NULL, as same driver can be used without graphics */
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate and initialize handle object memory including some of the members */
        hObj = vpsMdrvCompAllocInitHandleObj(
                   createPrms,
                   instObj,
                   fdmCbPrms,
                   &retVal);
        if ((NULL == hObj) || (FVID2_SOK != retVal))
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Handle allocation failed\n");
        }
        if ((NULL == hObj) && (FVID2_SOK != retVal))
        {
            retVal = FVID2_EALLOC;
        }
    }
/* TODO 17/03/2011; some of this is done in vpsMdrvCompAllocResrc()
    Settign Muxes in the path, HDMI_MUX above VCOMP
    VCOMP setting
    CIG, Blender setting: VENC clocks etc.,:: seperate core for this?
    Wrback CSC and MUX after that setting :: seperate core for this?
    SC5_MUX setting
    */
    if (FVID2_SOK == retVal)
    {
        /* Open the cores */
        retVal = vpsMdrvCompOpenCores(hObj);
        if (FVID2_SOK == retVal)
        {
            coreOpenFlag = TRUE;
        }
        else
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Opening of cores failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate core memories */
        retVal = vpsMdrvCompAllocCoreMem(hObj);
        if (FVID2_SOK == retVal)
        {
            coreAllocFlag = TRUE;
            /* Program the descriptor memories of cores */
            retVal = vpsMdrvCompProgramDesc(hObj);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR,
                    "Programming of cores failed\n");
            }
        }
        else
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Core memory alloc failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (0u == instObj->openCnt)
        {
            /* Allocate the resources if this is the first time */
            retVal = vpsMdrvCompAllocResrc(instObj);
            if (FVID2_SOK == retVal)
            {
                resrcAllocFlag = TRUE;

                /* Register with MLM if first handle is opened.
                   channels are non-static, there is no need to
                   provide channel numbers here. */
                instObj->mlmInfo.reqCompleteCb = VpsMdrv_compMlmCbFxn;
                instObj->mlmInfo.isChannelStatic = FALSE;
                instObj->mlmHandle = Mlm_register(&instObj->mlmInfo, NULL);
                if (NULL == instObj->mlmHandle)
                {
                    GT_0trace(VpsMdrvCompTrace, GT_ERR,
                        "Registration with MLM failed!\n");
                    retVal = FVID2_EALLOC;
                }
            }
            else
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR, "Resource alloc failed\n");
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
                tempRetVal = vpsMdrvCompFreeResrc(instObj);
                GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
            }

            if (TRUE == coreAllocFlag)
            {
                /* Free the memories allocated for cores */
                tempRetVal = vpsMdrvCompFreeCoreMem(hObj);
                GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
            }

            if (TRUE == coreOpenFlag)
            {
                /* Close the opened cores */
                tempRetVal = vpsMdrvCompCloseCores(hObj);
                GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
            }

            /* Free allocated handle object */
            tempRetVal = vpsMdrvCompFreeHandleObj(hObj);
            GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
            hObj = NULL;
        }
    }

    /* Return the status if possible */
    if (NULL != createStatusArgs)
    {
        createStatus = (Vps_M2mCompCreateStatus *) createStatusArgs;
        createStatus->retVal = retVal;
        if (FVID2_SOK == retVal)
        {
            createStatus->maxHandles = VPS_M2M_COMP_MAX_HANDLE_PER_INST;
            createStatus->maxChPerHandle = VPS_M2M_COMP_MAX_CH_PER_INST;
            createStatus->maxSameChPerRequest = 1u;
            createStatus->maxReqInQueue = VPSMDRV_COMP_MAX_QOBJS_PER_HANDLE;
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
 *  VpsMdrv_compDelete
 */
static Int32 VpsMdrv_compDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Int32                   retVal = FVID2_SOK, tempRetVal;
    VpsMdrv_CompInstObj     *instObj;
    VpsMdrv_CompHandleObj   *hObj;

    /* Check for NULL pointers */
    if (NULL == handle)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        hObj = handle;
        instObj = hObj->parent;
        /* NULL pointer check */
        GT_assert(VpsMdrvCompTrace, (NULL != instObj));

        /* Check if still some request are pending with driver */
        if (0u != hObj->numPendReq)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Still %d request pending. Dequeue all before closing!!\n",
                hObj->numPendReq);
            retVal = FVID2_EAGAIN;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Take the instance semaphore */
        Semaphore_pend(instObj->instSem, VPSMDRV_COMP_SEM_TIMEOUT);

        /* Decrement number of handle open count */
        GT_assert(VpsMdrvCompTrace, (0u != instObj->openCnt));
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
            tempRetVal = vpsMdrvCompFreeResrc(instObj);
            GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
        }

        /* Free the memories allocated for cores */
        tempRetVal = vpsMdrvCompFreeCoreMem(hObj);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));

        /* Close the opened cores */
        tempRetVal = vpsMdrvCompCloseCores(hObj);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));

        /* Free allocated handle object */
        tempRetVal = vpsMdrvCompFreeHandleObj(hObj);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
        hObj = NULL;

        /* Post the instance semaphore */
        Semaphore_post(instObj->instSem);
    }

    return (retVal);
}

#if 1
/**
 *  VpsMdrv_compControl
 */
static Int32 VpsMdrv_compControl(Fdrv_Handle handle,
                                UInt32 cmd,
                                Ptr cmdArgs,
                                Ptr cmdStatusArgs)
{
    Int32                   retVal = FVID2_SOK;
    VpsMdrv_CompHandleObj   *hObj;

    /* Check for NULL pointers */
    if (NULL == handle)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    if (FVID2_SOK == retVal)
    {
        hObj = handle;
        switch (cmd)
        {
            case IOCTL_VPS_SET_COEFFS:
                /* Set scalar coefficient IOCTL */
                retVal = vpsMdrvCompSetScCoeffIoctl(hObj, cmdArgs);
                break;
#if 0
            case IOCTL_VPS_READ_ADV_SC_CFG:
                /* Read Scalar advance configuration IOCTL */
                retVal = vpsMdrvCompScRdAdvCfgIoctl(hObj, cmdArgs);
                break;

            case IOCTL_VPS_WRITE_ADV_SC_CFG:
                /* Write Scalar advance configuration IOCTL */
                retVal = vpsMdrvCompScWrAdvCfgIoctl(hObj, cmdArgs);
                break;
#endif
            default:
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "UNSUPPORTED_CMD: IOCTL (%d) not supported\n", cmd);
                retVal = FVID2_EUNSUPPORTED_CMD;
                break;
        }
    }

    return (retVal);
}



/**
 *  VpsMdrv_compProcessFrms
 */
static Int32 VpsMdrv_compProcessFrms(Fdrv_Handle handle,
                                    FVID2_ProcessList *procList)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    VpsMdrv_CompQueueObj    *qObj = NULL;
    VpsMdrv_CompHandleObj   *hObj = NULL;

    /* Check for NULL pointers */
    if ((NULL == handle) || (NULL == procList))
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (VpsMdrv_CompHandleObj *) handle;

        /* Check for error in parameters */
        retVal = vpsMdrvCompCheckProcList(hObj, procList);
    }

    /* Disable global interrupts */
    cookie = Hwi_disable();

    if (FVID2_SOK == retVal)
    {
        /* Get a queue object from the free queue */
        qObj = VpsUtils_dequeue(hObj->freeQ);
        if (NULL == qObj)
        {
            GT_0trace(VpsMdrvCompTrace, GT_DEBUG,
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
        qObj->descSetInUse = vpsMdrvCompAllocDescSet(hObj);
        if (VPSMDRV_COMP_INVALID_DESC_SET == qObj->descSetInUse)
        {
            /* We did not get the free desc set so queue the request
             * until the call back is called. */
            VpsUtils_queue(hObj->reqQ, &qObj->qElem, qObj);
        }
        else
        {
            /* We have a free descriptor set, update the descriptors, link the
             * channels and queue it to MLM for processing */
            retVal = vpsMdrvCompProcessReq(hObj, qObj);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_DEBUG,
                    "Process request failed\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Increment the pending request count */
        hObj->numPendReq++;
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        if (NULL != qObj)
        {
            /* Free the descriptor set if already allocated */
            if (VPSMDRV_COMP_INVALID_DESC_SET != qObj->descSetInUse)
            {
                vpsMdrvCompFreeDescSet(hObj, qObj->descSetInUse);
                qObj->descSetInUse = VPSMDRV_COMP_INVALID_DESC_SET;
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
 *  VpsMdrv_compGetProcessedFrms
 */
static Int32 VpsMdrv_compGetProcessedFrms(Fdrv_Handle handle,
                                         FVID2_ProcessList *procList,
                                         UInt32 timeout)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cookie;
    VpsMdrv_CompQueueObj    *qObj = NULL;
    VpsMdrv_CompHandleObj   *hObj = NULL;

    /* Check for NULL pointers */
    if ((NULL == handle) || (NULL == procList))
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }
    else
    {
        hObj = (VpsMdrv_CompHandleObj *) handle;

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
                GT_0trace(VpsMdrvCompTrace, GT_DEBUG,
                    "Nothing to dequeue. No request pending with driver!!\n");
                retVal = FVID2_ENO_MORE_BUFFERS;
            }
            else
            {
                /* If no request have completed, return try again */
                GT_0trace(VpsMdrvCompTrace, GT_DEBUG,
                    "Nothing to dequeue. Try again!!\n");
                retVal = FVID2_EAGAIN;
            }
        }
        else
        {
            /* Copy the driver's process list to application's process list */
            FVID2_copyProcessList(procList, &qObj->procList);

            /* Return back the queue object to the free queue */
            qObj->descSetInUse = VPSMDRV_COMP_INVALID_DESC_SET;
            VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);

            /* Decrement the pending request count */
            GT_assert(VpsMdrvCompTrace, (0u != hObj->numPendReq));
            hObj->numPendReq--;
        }

        /* Restore global interrupts */
        Hwi_restore(cookie);
    }

    return (retVal);
}
#endif

/**
 *  VpsMdrv_compMlmCbFxn
 */
static Int32 VpsMdrv_compMlmCbFxn(Mlm_SubmitReqInfo *reqInfo)
{
#if 1
    UInt32                  retVal = FVID2_SOK;
    VpsMdrv_CompQueueObj    *qObj;
    VpsMdrv_CompHandleObj   *hObj;
    VpsMdrv_CompInstObj     *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != reqInfo));
    qObj = reqInfo->cbArg;
    GT_assert(VpsMdrvCompTrace, (NULL != qObj));
    hObj = qObj->hObj;
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    /* Check whether the request type is frame or coefficient */
    if (VPSMDRV_QOBJ_TYPE_FRAMES == qObj->qObjType)
    {

//?TODO
        /* Free the descriptor set */
        vpsMdrvCompFreeDescSet(hObj, qObj->descSetInUse);

        /*  Queue the completed request to the Done Queue */
        VpsUtils_queue(hObj->doneQ, &qObj->qElem, qObj);
        qObj = NULL;

        /* If callback is registered, intimate the application that a
         * request is completed and is ready to be dequeued */
        if (NULL != hObj->fdmCbPrms.fdmCbFxn)
        {
            hObj->fdmCbPrms.fdmCbFxn(hObj->fdmCbPrms.fdmData, NULL);
        }

        /* Check if any request is pending */
        qObj = VpsUtils_dequeue(hObj->reqQ);
        if (NULL != qObj)
        {
            /* Get a free descriptor to work with */
            qObj->descSetInUse = vpsMdrvCompAllocDescSet(hObj);
            GT_assert(VpsMdrvCompTrace,
                (VPSMDRV_COMP_INVALID_DESC_SET != qObj->descSetInUse));

            /* We have a free descriptor set, update the descriptors, link the
             * channels and queue it to MLM for processing */
            retVal = vpsMdrvCompProcessReq(hObj, qObj);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_DEBUG,
                    "Process request failed\n");
            }
        }

        /* If error occurs free objects */
        if (FVID2_SOK != retVal)
        {
            if (NULL != qObj)
            {
                /* Return process list to application using error callback */
                vpsMdrvCompRetErrProcList(hObj, qObj);

                /* Return back the queue object to the free queue */
                VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);
                qObj = NULL;
            }
        }
    }
    else if (VPSMDRV_QOBJ_TYPE_COEFF == qObj->qObjType)
    {
        /* Callback got for programming of coefficients. */
        /* Signal the semaphore */
        Semaphore_post(hObj->ctrlSem);
    }
#endif
    return (0u);
}
#if 1
/**
 *  vpsMdrvCompProcessReq
 */
static Int32 vpsMdrvCompProcessReq(VpsMdrv_CompHandleObj *hObj,
                                  VpsMdrv_CompQueueObj *qObj)
{
    Int32                       retVal = FVID2_SOK;
    UInt32                      descSet, chNum;
    UInt32                      frmCnt, numFrames;
    UInt32                      rldSize;
    UInt32                      chCnt;
    Void                       *descPtr = NULL;
    VpsMdrv_CompChObj           *chObj;
    VpsMdrv_CompInstObj         *instObj;
    FVID2_ProcessList          *procList;
    VpsHal_VpdmaReloadDesc     *rldDesc = NULL;
    Vps_M2mCompRtParams         *rtPrms = NULL;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != qObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    /* Get the numFrames from in frame list */
    procList = &qObj->procList;
    GT_assert(VpsMdrvCompTrace, (NULL != procList->inFrameList[0u]));
    numFrames = procList->inFrameList[0u]->numFrames;
    descSet = qObj->descSetInUse;

    /* Initialize the reload pointer to handle descriptors so that the first
     * channel will be linked to it */
    rldDesc = hObj->rldDesc[descSet];
    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != rldDesc));

    /* Update the descriptors of the requested channels and link them */
    for (frmCnt = 0u; frmCnt < numFrames; frmCnt++)
    {
        if (NULL == procList->inFrameList[0u]->frames[frmCnt])
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer\n");
            retVal = FVID2_EBADARGS;
            break;
        }

        /* Get the channel number from the in frame list's frame pointer */
        chNum = procList->inFrameList[0u]->frames[frmCnt]->channelNum;

        /* Check if the channel number is within the allocated one */
        if (chNum >= hObj->numCh)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Invalid channel number\n");
            retVal = FVID2_EOUT_OF_RANGE;
            break;
        }

        /* Get the channel object */
        chObj = hObj->chObjs[chNum];
        GT_assert(VpsMdrvCompTrace, (NULL != chObj));

        /* Update runtime params */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            /* Assign the runtime params if present in any of the core frame */
            rtPrms = (Vps_M2mCompRtParams *)
                procList->inFrameList[0u]->frames[frmCnt]->perFrameCfg;
        }
        else
        {
            rtPrms = (Vps_M2mCompRtParams *)
                procList->inFrameList[0u]->perListCfg;
        }

        retVal = vpsMdrvCompUpdateRtParams(hObj, chObj, rtPrms, descSet);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Could not update runtime params for channel %d!\n", chNum);
            break;
        }

        /* Update channel descriptors */
        retVal = vpsMdrvCompUpdateChDesc(hObj, chObj, procList, frmCnt, descSet);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Update of channel descriptor failed for channel %d\n", chNum);
            break;
        }

        /* Get the descriptor address and size to which the previous reload
         * descriptor should be linked */
        descPtr = chObj->firstDesc[descSet];
        rldSize = chObj->totalDescMem / VPSMDRV_COMP_MAX_DESC_SET;

        /* Link the reload descriptor of previous channel to the descriptor
         * of this channel */
        VpsHal_vpdmaCreateRLCtrlDesc(rldDesc, descPtr, rldSize);

        /* Store the reload descriptor of this channel, so that next channel
         * will link to it */
        rldDesc = chObj->rldDesc[descSet];

    }

    if (FVID2_SOK == retVal)
    {
        /*
         * Update the MLM parameters and submit the request to MLM
         */
        GT_assert(VpsMdrvCompTrace,
            (hObj->numVpdmaChannels <= MLM_MAX_CHANNELS));
        /* Copy Channel Number */
        for (chCnt = 0u; chCnt < hObj->numVpdmaChannels; chCnt++)
        {
            qObj->mlmReqInfo.channelNum[chCnt] =
                                hObj->socChNum[chCnt];
        }
        qObj->mlmReqInfo.numChannels = hObj->numVpdmaChannels;
        /* Always handle descriptor is the start */
        qObj->mlmReqInfo.reqStartAddr = hObj->firstDesc[descSet];
        qObj->mlmReqInfo.firstChannelSize =
            hObj->totalDescMem / VPSMDRV_COMP_MAX_DESC_SET;
        /* Last channel reload descriptor */
        qObj->mlmReqInfo.lastRlDescAddr = rldDesc;
        if (VPSMDRV_SYNC_MODE_SOCH == hObj->syncMode)
        {
            /* We have already put SOCH for each channel in the driver,
             * so tell MLM not to put again */
            qObj->mlmReqInfo.submitReqType = MLM_SRT_NO_SOC;
        }
        else
        {
            /* Tell MLM to put SOCH at the end of for each request */
            qObj->mlmReqInfo.submitReqType = MLM_SRT_USE_SOC;
        }
        qObj->mlmReqInfo.cbArg = qObj;
        retVal = Mlm_submit(instObj->mlmHandle, &qObj->mlmReqInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Submission to MLM failed\n");
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompUpdateChDesc
 */
static Int32 vpsMdrvCompUpdateChDesc(VpsMdrv_CompHandleObj *hObj,
                                    VpsMdrv_CompChObj *chObj,
                                    FVID2_ProcessList *procList,
                                    UInt32 frmCnt,
                                    UInt32 descSet)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  coreCnt;
    UInt32                  frmListIdx;
    UInt32                  fid;
    UInt32                  firstOut = TRUE;
    FVID2_Frame            *frame;
    FVID2_FrameList        *frmList;
    VpsMdrv_CompInstObj     *instObj;
    const Vcore_Ops        *coreOps;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    GT_assert(VpsMdrvCompTrace, (NULL != procList));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    frmListIdx = 0u;
    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Program cores only if needed for that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->updateDesc));

            /* Get the frame list pointer and the frame pointer */
            if (coreCnt < VPSMDRV_COMP_MAX_INPUTS)
            {
                /* Get the frame list from in frame list */
                frmList = procList->inFrameList[frmListIdx];
                frmListIdx++;
            }
            else
            {
                /* Reset index for the first out frame list */
                if (TRUE == firstOut)
                {
                    frmListIdx = 0u;
                    firstOut = FALSE;
                }
                /* Get the frame list from out frame list */
                frmList = procList->outFrameList[frmListIdx];
                frmListIdx++;
            }
            GT_assert(VpsMdrvCompTrace, (NULL != frmList));
            frame = frmList->frames[frmCnt];
            GT_assert(VpsMdrvCompTrace, (NULL != frame));

            fid = 0u;
            if ((FVID2_FID_BOTTOM == frame->fid) &&
                (coreCnt < VPSMDRV_COMP_MAX_INPUTS))
            {
                /* Use FID only for input frame. Output frame FID should always
                 * be 0. */
                fid = 1u;
            }

            /* Update the core descriptors for the FID and buffer address */
            retVal = coreOps->updateDesc(
                         hObj->coreHandle[coreCnt],
                         &chObj->coreDescMem[descSet][coreCnt],
                         frame,
                         fid);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not update desc for core %d!\n", coreCnt);
                break;
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompUpdateRtParams
 */
static Int32 vpsMdrvCompUpdateRtParams(VpsMdrv_CompHandleObj *hObj,
                                      VpsMdrv_CompChObj *chObj,
                                      const Vps_M2mCompRtParams *rtPrms,
                                      UInt32 descSet)
{
    Int32                   retVal = FVID2_SOK;
#if 0 //?TODO
    UInt32                  cnt;
    VpsMdrv_CompInstObj     *instObj;
    const Vcore_Ops        *coreOps;
    Vcore_SwpRtParams       compRtPrms;
    Vcore_DescMem          *coreDescMem;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    /* Update the COMP runtime parameters if COMP is present */
    if (TRUE == instObj->isCoreReq[VPSMDRV_COMP_COMP_CORE_IDX])
    {
        coreOps = instObj->coreOps[VPSMDRV_COMP_COMP_CORE_IDX];
        coreDescMem =
            &chObj->coreDescMem[descSet][VPSMDRV_COMP_COMP_CORE_IDX];
        /* NULL pointer check */
        GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
        GT_assert(VpsMdrvCompTrace, (NULL != coreOps->updateRtMem));

        if (NULL != rtPrms)
        {
            /* Copy the COMP runtime parameters */
            compRtPrms.outFrmPrms = rtPrms->deiOutFrmPrms;
            compRtPrms.scCropCfg  = rtPrms->deiScCropCfg;
            compRtPrms.scRtCfg    = rtPrms->deiScRtCfg;
            compRtPrms.deiRtCfg   = rtPrms->deiRtCfg;
            compRtPrms.inFrmPrms  = rtPrms->deiInFrmPrms;


            /* Update the runtime configuration in core descriptors */
            retVal = coreOps->updateRtMem(
                         hObj->coreHandle[VPSMDRV_COMP_COMP_CORE_IDX],
                         coreDescMem,
                         chObj->chNum,
                         &compRtPrms,
                         0u);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not update runtime desc for COMP core!\n");
            }

            /* Check COMP runtime configiration */
            if ((NULL != rtPrms->deiRtCfg) &&
                (TRUE == rtPrms->deiRtCfg->resetComp))
            {
                /* If the reset is required, initialize the prevFldState to zero
                   again so that COMP will start working in Line average
                   mode for first few fields and builds up the context again */
                chObj->prevFldState = 0u;
            }
        }
        else if (TRUE == chObj->isDescDirty[descSet])
        {
            /* Descriptor is dirty, use the core to just update the
             * descriptors without providing any parameters */
            retVal = coreOps->updateRtMem(
                         hObj->coreHandle[VPSMDRV_COMP_COMP_CORE_IDX],
                         coreDescMem,
                         chObj->chNum,
                         NULL,
                         0);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not update runtime desc for COMP core!\n");
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the DWP runtime parameters if DWP path is present */
        if (TRUE == instObj->isCoreReq[VPSMDRV_COMP_DWP_CORE_IDX])
        {
            coreOps = instObj->coreOps[VPSMDRV_COMP_DWP_CORE_IDX];
            coreDescMem =
                &chObj->coreDescMem[descSet][VPSMDRV_COMP_DWP_CORE_IDX];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->updateRtMem));

            if (NULL != rtPrms)
            {
                /* Copy the DWP runtime parameters */
                dwpRtPrms.outFrmPrms = rtPrms->deiOutFrmPrms;

                /* Update the runtime configuration in core descriptors */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_COMP_DWP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             &dwpRtPrms,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvCompTrace, GT_ERR,
                        "Could not update runtime desc for DWP core!\n");
                }
            }
            else if (TRUE == chObj->isDescDirty[descSet])
            {
                /* Descriptor is dirty, use the core to just update the
                 * descriptors without providing any parameters */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_COMP_DWP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             NULL,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvCompTrace, GT_ERR,
                        "Could not update runtime desc for DWP core!\n");
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the VIP runtime parameters if VIP is present */
        if (TRUE == instObj->isCoreReq[VPSMDRV_COMP_VIP_CORE_IDX])
        {
            coreOps = instObj->coreOps[VPSMDRV_COMP_VIP_CORE_IDX];
            coreDescMem =
                &chObj->coreDescMem[descSet][VPSMDRV_COMP_VIP_CORE_IDX];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->updateRtMem));

            if (NULL != rtPrms)
            {
                /* Copy the VIP runtime parameters */
                vipRtPrms.outFrmPrms = rtPrms->vipOutFrmPrms;
                vipRtPrms.scCropCfg = rtPrms->vipScCropCfg;
                vipRtPrms.scRtCfg = rtPrms->vipScRtCfg;

                /* Update the runtime configuration in core descriptors */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_COMP_VIP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             &vipRtPrms,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvCompTrace, GT_ERR,
                        "Could not update runtime desc for VIP core!\n");
                }
            }
            else if (TRUE == chObj->isDescDirty[descSet])
            {
                /* Descriptor is dirty, use the core to just update the
                 * descriptors without providing any parameters */
                retVal = coreOps->updateRtMem(
                             hObj->coreHandle[VPSMDRV_COMP_VIP_CORE_IDX],
                             coreDescMem,
                             chObj->chNum,
                             NULL,
                             0u);
                if (FVID2_SOK != retVal)
                {
                    GT_0trace(VpsMdrvCompTrace, GT_ERR,
                        "Could not update runtime desc for VIP core!\n");
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Update the dirty flags */
        if (NULL != rtPrms)
        {
            /* Mark all other sets as dirty except the current descriptor set */
            for (cnt = 0u; cnt < VPSMDRV_COMP_MAX_DESC_SET; cnt++)
            {
                chObj->isDescDirty[cnt] = TRUE;
            }
        }

        /* Mark the current set as clean */
        chObj->isDescDirty[descSet] = FALSE;
    }
#endif
    return (retVal);
}



/**
 *  vpsMdrvCompRetErrProcList
 */
static Void vpsMdrvCompRetErrProcList(VpsMdrv_CompHandleObj *hObj,
                                     VpsMdrv_CompQueueObj *qObj)
{
    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != qObj));

    if ((NULL != hObj->fdmCbPrms.fdmErrCbFxn) &&
        (NULL != hObj->fdmCbPrms.errList))
    {
        /* Copy the driver's process list to application's error process list */
        FVID2_copyProcessList(hObj->fdmCbPrms.errList, &qObj->procList);

        /* Call the application's error call back function */
        hObj->fdmCbPrms.fdmErrCbFxn(
            hObj->fdmCbPrms.fdmData,
            hObj->fdmCbPrms.errList,
            NULL);
    }

    /* Decrement the pending request count */
    GT_assert(VpsMdrvCompTrace, (0u != hObj->numPendReq));
    hObj->numPendReq--;

    return;
}



/**
 *  vpsMdrvCompCheckProcList
 */
static Int32 vpsMdrvCompCheckProcList(VpsMdrv_CompHandleObj *hObj,
                                     FVID2_ProcessList *procList)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  numInListReq;
    VpsMdrv_CompInstObj     *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != procList));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    /* For mode 1 HQ driver, in frame list is one more to take N-1
     * field buffer */
    numInListReq = instObj->numInListReq;

    /* Check for error in process list */
    retVal = FVID2_checkProcessList(
                 procList,
                 numInListReq,
                 instObj->numOutListReq,
                 hObj->numCh,
                 TRUE);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR, "Error in process list\n");
    }

    return (retVal);
}



/**
 *  vpsMdrvCompAllocDescSet
 */
static UInt32 vpsMdrvCompAllocDescSet(VpsMdrv_CompHandleObj *hObj)
{
    UInt32      allocedDescSet = VPSMDRV_COMP_INVALID_DESC_SET;
    UInt32      descSet;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));

    for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
    {
        if (TRUE == hObj->isDescSetFree[descSet])
        {
            /* Set flag as alloced */
            hObj->isDescSetFree[descSet] = FALSE;
            allocedDescSet = descSet;
            break;
        }
    }

    return (allocedDescSet);
}



/**
 *  vpsMdrvCompFreeDescSet
 */
static Int32 vpsMdrvCompFreeDescSet(VpsMdrv_CompHandleObj *hObj, UInt32 descSet)
{
    Int32       retVal = FVID2_SOK;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    /* Assert if index is more than the index */
    GT_assert(VpsMdrvCompTrace, (descSet < VPSMDRV_COMP_MAX_DESC_SET));

    /* Set flag as free */
    hObj->isDescSetFree[descSet] = TRUE;

    return (retVal);
}
#endif

static Int32 vpsMdrvCompSetScCoeffIoctl(VpsMdrv_CompHandleObj *hObj, Ptr cmdArgs)
{
    Int32                       retVal = FVID2_SOK, tempRetVal;
    UInt32                      cookie;
    UInt32                      tempCnt, coreCnt, matchedCoreCnt = 0u, chCnt;
    Ptr                         coeffDesc = NULL, tmpDescPtr = NULL;
    const Vcore_Ops            *coreOps = NULL;
    VpsMdrv_CompQueueObj        *qObj = NULL;
    VpsMdrv_CompChObj           *chObj;
    VpsMdrv_CompInstObj         *instObj;
    const Vps_ScCoeffParams    *scCoeffPrms;
    Vcore_ScCoeffParams         coreCoeffPrms;

    /* Check for NULL pointers */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    if (NULL == cmdArgs)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR, "NULL pointer\n");
        retVal = FVID2_EBADARGS;
    }

    /* Disable global interrupts */
    cookie = Hwi_disable();

    if (FVID2_SOK == retVal)
    {
        scCoeffPrms = (Vps_ScCoeffParams *) cmdArgs;
        instObj = hObj->parent;
        GT_assert(VpsMdrvCompTrace, (NULL != instObj));

        /* TODO: Assuming first channel information */
        chObj = hObj->chObjs[0u];
        GT_assert(VpsMdrvCompTrace, (NULL != chObj));

        /* Figure out the core to which the scalar ID matches */
        tempCnt = 0u;
        for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
        {
            if (TRUE == instObj->isCoreReq[coreCnt])
            {
                if (TRUE == chObj->scPresent[coreCnt])
                {
                    if (tempCnt == scCoeffPrms->scalarId)
                    {
                        coreOps = instObj->coreOps[coreCnt];
                        matchedCoreCnt = coreCnt;
                        /* NULL pointer check */
                        GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
                        break;
                    }
                    tempCnt++;
                }
            }
        }

        /* Check if the scalar ID is valid */
        if (NULL == coreOps)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Invalid Scalar ID\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate coeff descriptors */
        coeffDesc = vpsMdrvCompAllocCoeffDesc();
        /* Get a queue object from the free queue */
        qObj = VpsUtils_dequeue(hObj->freeQ);
        if ((NULL == coeffDesc) || (NULL == qObj))
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR,
                "Could not allocate coeff descriptor/queue object\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* NULL pointer check */
        GT_assert(VpsMdrvCompTrace, (NULL != coreOps->programScCoeff));

        coreCoeffPrms.hScalingSet = scCoeffPrms->hScalingSet;
        coreCoeffPrms.vScalingSet = scCoeffPrms->vScalingSet;
        coreCoeffPrms.horzCoeffMemPtr = NULL; /* Returned by core */
        coreCoeffPrms.vertCoeffMemPtr = NULL; /* Returned by core */
        coreCoeffPrms.vertBilinearCoeffMemPtr = NULL; /*Returned by core*/
        coreCoeffPrms.userCoeffPtr = scCoeffPrms->coeffPtr;

        /* Program the coefficient memory */
        retVal = coreOps->programScCoeff(
                     hObj->coreHandle[coreCnt],
                     0u,
                     &coreCoeffPrms);
    }

    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Could not program coeff memory\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* First create the horizontal coeff descriptor. */
        tmpDescPtr = coeffDesc;
        GT_assert(VpsMdrvCompTrace, (NULL != coreCoeffPrms.horzCoeffMemPtr));
        VpsHal_vpdmaCreateConfigDesc(
            tmpDescPtr,
            chObj->coeffConfigDest[matchedCoreCnt],
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            chObj->horzCoeffOvlySize[matchedCoreCnt],
            coreCoeffPrms.horzCoeffMemPtr,
            0u,
            chObj->horzCoeffOvlySize[matchedCoreCnt]);

        /* Now create the vertical coeff descriptor. */
        tmpDescPtr = (Ptr)
            (((UInt32) tmpDescPtr) + VPSHAL_VPDMA_CONFIG_DESC_SIZE);
        GT_assert(VpsMdrvCompTrace, (NULL != coreCoeffPrms.vertCoeffMemPtr));
        VpsHal_vpdmaCreateConfigDesc(
            tmpDescPtr,
            chObj->coeffConfigDest[matchedCoreCnt],
            VPSHAL_VPDMA_CPT_BLOCK_SET,
            VPSHAL_VPDMA_CCT_INDIRECT,
            chObj->vertCoeffOvlySize[matchedCoreCnt],
            coreCoeffPrms.vertCoeffMemPtr,
            (Ptr) (chObj->horzCoeffOvlySize[matchedCoreCnt]),
            chObj->vertCoeffOvlySize[matchedCoreCnt]);

        /* Create the vertical bilinear coeff descriptor only if HQ scalar,
         * otherwise create dummy descriptor.
         */
        tmpDescPtr = (Ptr)
            (((UInt32) tmpDescPtr) + VPSHAL_VPDMA_CONFIG_DESC_SIZE);
        if (NULL != coreCoeffPrms.vertBilinearCoeffMemPtr)
        {
            VpsHal_vpdmaCreateConfigDesc(
                tmpDescPtr,
                chObj->coeffConfigDest[matchedCoreCnt],
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
            VpsHal_vpdmaCreateDummyDesc(tmpDescPtr);
        }

        /*
         * Update the MLM parameters and submit the request to MLM
         */
        qObj->qObjType = VPSMDRV_QOBJ_TYPE_COEFF;
        qObj->mlmReqInfo.reqStartAddr = coeffDesc;
        qObj->mlmReqInfo.firstChannelSize =
            ((VPSMDRV_COMP_MAX_COEFF_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE) +
             VPSHAL_VPDMA_CTRL_DESC_SIZE);
        qObj->mlmReqInfo.lastRlDescAddr = ((UInt8 *)coeffDesc +
            (VPSMDRV_COMP_MAX_COEFF_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE));
        qObj->mlmReqInfo.submitReqType = MLM_SRT_NO_SOC;
        qObj->mlmReqInfo.cbArg = qObj;
        GT_assert(VpsMdrvCompTrace,
            (hObj->numVpdmaChannels <= MLM_MAX_CHANNELS));
        /* Copy Channel Number */
        for (chCnt = 0u; chCnt < hObj->numVpdmaChannels; chCnt++)
        {
            qObj->mlmReqInfo.channelNum[chCnt] = hObj->socChNum[chCnt];
        }
        qObj->mlmReqInfo.numChannels = hObj->numVpdmaChannels;
        retVal = Mlm_submit(instObj->mlmHandle, &qObj->mlmReqInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Submission to MLM failed\n");
        }
        else
        {
            /* Increment the pending request count */
            hObj->numPendReq++;
            hObj->numUnProcessedReq++;

            /* Restore global interrupts */
            Hwi_restore(cookie);

            /* Wait for completion of coefficient programming */
            Semaphore_pend(hObj->ctrlSem, VPSMDRV_COMP_SEM_TIMEOUT);

            /* Disable global interrupts */
            cookie = Hwi_disable();

            /* Decrement the pending request count */
            GT_assert(VpsMdrvCompTrace, (0u != hObj->numPendReq));
            GT_assert(VpsMdrvCompTrace, (0u != hObj->numUnProcessedReq));
            hObj->numUnProcessedReq--;
            hObj->numPendReq--;
        }
    }

    /* Free the memories if already allocated */
    if (NULL != coeffDesc)
    {
        tempRetVal = vpsMdrvCompFreeCoeffDesc(coeffDesc);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
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



