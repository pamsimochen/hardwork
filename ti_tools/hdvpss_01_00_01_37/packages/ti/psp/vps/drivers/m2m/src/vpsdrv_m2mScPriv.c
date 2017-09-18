/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


 /**
 *  \file vpsdrv_m2mScPriv.c
 *
 *  \brief Memory to Memory driver.
 *  This file implements the memory to memory driver for the various SC paths.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/std.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>
#include <string.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSc.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/core/vpscore_secPath.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
#include <ti/psp/vps/core/vpscore_scWrbkPath.h>
#include <ti/psp/vps/drivers/m2m/vps_m2mInt.h>
#include <ti/psp/vps/drivers/m2m/vps_mlm.h>
#include <ti/psp/vps/drivers/m2m/vpsdrv_m2mSlice.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_m2mSc.h>
#include <ti/psp/vps/drivers/m2m/src/vpsdrv_m2mScPriv.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */
/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static VpsMdrv_ScChObj *vpsMdrvScAllocChObj(VpsMdrv_ScChObj *chObjs);
static Void vpsMdrvScFreeChObj(VpsMdrv_ScChObj *chObj);
static Int32 vpsMdrvScAllocChDescs(VpsMdrv_ScHandleObj *hObj);
static Void vpsMdrvScFreeChDescs(VpsMdrv_ScChObj **chObjs, UInt32 numCh);
static Int32 vpsMdrvScAllocChOvlyMems(VpsMdrv_ScHandleObj *hObj);
static Void vpsMdrvScFreeChOvlyMems(VpsMdrv_ScChObj **chObjs, UInt32 numCh);
static VpsMdrv_ScChObj *vpsMdrvScAllocSPadChObj(VpsMdrv_ScChObj *sPadChObjs);
static Void vpsMdrvScFreeSPadChObj(VpsMdrv_ScChObj *sPadChObj);
static Vps_M2mScChParams *vpsMdrvScAllocChParam(Void);
static Void vpsMdrvScFreeChParam(Vps_M2mScChParams *chPrms);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief Memory pool for the channel objects.  Handle objects
 * will allocate channel objects from this pool. This contains
 * normal channel objects and scratchpad channel objects */
static VpsMdrv_ScChObj VpsMdrvScChannelObjs[VPSMDRV_SC_MAX_CH_MEM_POOL * 2u];

/** \brief Memory pool for the scratch pad channel objects.  Handle objects
 * will allocate channel objects from this pool */
static VpsMdrv_ScChObj VpsMdrvScScPadChObjs[VPSMDRV_SC_MAX_CH_MEM_POOL];

/** \brief Memory pool for storing the params associated with each channel */
static VpsMdrv_ScChParams VpsMdrvScChParams[VPSMDRV_SC_MAX_CH_MEM_POOL];

/** \brief Flags for channel params allocation/de-allocation */
static UInt32 VpsMdrvScChParamsIsAllocated[VPSMDRV_SC_MAX_CH_MEM_POOL];

/** \brief Instance objects for each instance of the driver */
static VpsMdrv_ScInstObj VpsMdrvScInstObjects[VPS_M2M_SC_INST_MAX];

/** \brief Handle objects for each handle of the driver.  Handle objects
 * are shared between all the instance of the drivers */
static VpsMdrv_ScHandleObj VpsMdrvScHandleObjects[VPSMDRV_SC_MAX_EFF_INST *
                                                  VPS_M2M_SC_MAX_HANDLE];

/** \brief Queue objects pool for queueing the incoming requests
 * One container pool for each instance.  Requests from each of the
 * handles of same instance will be served from this pool */
static VpsMdrv_ScQueueObj VpsMdrvScQueueObjPool[VPSMDRV_SC_MAX_EFF_INST]
                                               [VPSMDRV_SC_MAX_Q_OBJS];

/** \brief Flag to indicate whether Q objects are allocataed.  One flag for
 *  all queue objects since only once instance can be opened and whatever
 * instance is opened first, queue objects are allocated to that.
 */
static UInt32 VpsMdrvScQbjsAllocated[VPSMDRV_SC_MAX_EFF_INST];

/** Memory to Memory driver function pointer */
static const FVID2_DrvOps VpsMdrvScOps =
{
    FVID2_VPS_M2M_SC_DRV,           /* Driver ID */
    VpsMdrv_scCreate,               /* Create */
    VpsMdrv_scDelete,               /* Delete */
    VpsMdrv_scControl,              /* Control */
    NULL,                           /* Queue */
    NULL,                           /* Dequeue */
    VpsMdrv_scProcessFrms,          /* ProcessFrames */
    VpsMdrv_scGetProcessedFrms,     /* GetProcessedFrames */
};

/**
 *  \brief Default scalar configuration. Used when user gives scalar
 *  configuration as NULL in channel parameter.
 */
static const Vps_ScConfig VpsMdrvScDefScCfg =
{
    FALSE,                      /* bypass */
    FALSE,                      /* nonLinear */
    0u,                         /* stripSize */
    VPS_SC_HST_AUTO,            /* hsType */
    VPS_SC_VST_POLYPHASE,       /* vsType */
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  VpsMdrv_scInit
 *  \brief Scalar M2M driver init function.
 *  Initializes driver objects, allocates memory etc.
 *  This function should be called before calling any of FVID2 APIs for this
 *  driver.
 *
 *  \param numInst      Number of instance objects to be initialized.
 *  \param initPrms     Pointer to the init parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInst.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_scInit(UInt32 numInst,
                     const VpsMdrv_ScInitParams *initPrms,
                     Ptr arg)
{
    Int32               retVal = FVID2_SOK;
    UInt32              instCnt, poolCnt, resCnt;
    Semaphore_Params    semParams;
    VpsMdrv_ScInstObj  *instObj;

    GT_assert(VpsMdrvScTrace, (NULL != initPrms));

    /* Memory reset all the global variables to be used */
    VpsUtils_memset(VpsMdrvScInstObjects, 0, sizeof(VpsMdrvScInstObjects));
    VpsUtils_memset(VpsMdrvScHandleObjects,
                    0,
                    sizeof(VpsMdrvScHandleObjects));
    VpsUtils_memset(VpsMdrvScScPadChObjs,0,sizeof(VpsMdrvScScPadChObjs));
    VpsUtils_memset(VpsMdrvScChannelObjs,0,sizeof(VpsMdrvScChannelObjs));
    VpsUtils_memset(VpsMdrvScQueueObjPool,
                    0,
                    sizeof(VpsMdrvScQueueObjPool));

    for (poolCnt = 0; poolCnt < VPSMDRV_SC_MAX_CH_MEM_POOL; poolCnt++)
    {
        /* Mark memory pools are unallocated */
        VpsMdrvScChParamsIsAllocated[poolCnt] = FALSE;
    }
    for (instCnt = 0; instCnt < VPSMDRV_SC_MAX_EFF_INST; instCnt++)
    {
        VpsMdrvScQbjsAllocated[instCnt] = FALSE;
    }

    retVal = FVID2_registerDriver(&VpsMdrvScOps);
    if (FVID2_SOK == retVal)
    {
        /* Initiallize the instance objects variables */
        for (instCnt = 0; instCnt < numInst; instCnt++)
        {
            instObj = &VpsMdrvScInstObjects[instCnt];
            instObj->drvInstId      = initPrms[instCnt].drvInstId;
            instObj->inCoreInstId   = initPrms[instCnt].inCoreInstId;
            instObj->outCoreInstId  = initPrms[instCnt].outCoreInstId;
            for (resCnt = 0u; resCnt < VPSMDRV_SC_MAX_RESR; resCnt ++)
            {
                instObj->resrcId[resCnt] = initPrms[instCnt].resrcId[resCnt];
            }
            instObj->inCoreOps      = initPrms[instCnt].inCoreOps;
            instObj->outCoreOps     = initPrms[instCnt].outCoreOps;
            instObj->openCnt = 0;
            instObj->enableLazyLoading = FALSE;
            instObj->enableFilterSelect = FALSE;

            /* Set the current scaling factor configuration values to invalid so
             * that the first call to process the frames will result in setting
             * coefficients to the scalar configuration of that frame (channel).
             */
            instObj->curScFactor.hsType = VPS_SC_HST_MAX;
            instObj->curScFactor.hScalingSet = VPS_SC_SET_MAX;
            instObj->curScFactor.vsType = VPS_SC_VST_MAX;
            instObj->curScFactor.vScalingSet = VPS_SC_VST_MAX;

            instObj->initDone = TRUE;
            Semaphore_Params_init(&semParams);
            instObj->instSem = Semaphore_create(1u, &semParams, NULL);
            /* Initialize the VPDMA channel number and mux configuration for
             * each instances of the driver */
            if (VPS_M2M_INST_SEC0_SC5_WB2 == instCnt)
            {
                instObj->wb2MuxSrc = VPSHAL_VPS_WB2_MUX_SRC_SEC1;
            }
            if (VPS_M2M_INST_BP0_SC5_WB2 == instCnt)
            {
                instObj->wb2MuxSrc = VPSHAL_VPS_WB2_MUX_SRC_BP0;
            }
            if (VPS_M2M_INST_BP1_SC5_WB2 == instCnt)
            {
                instObj->wb2MuxSrc = VPSHAL_VPS_WB2_MUX_SRC_BP1;
            }
            if ((VPS_M2M_INST_SEC0_SC3_VIP0 == instCnt) ||
                (VPS_M2M_INST_SEC1_SC4_VIP1 == instCnt))
            {

                 instObj->secMuxSrc = VPSHAL_VPS_SEC_MUX_SRC_SEC;
            }
        }
    }
    else
    {
        GT_0trace(VpsMdrvScTrace, GT_ERR, "FAIL: Driver Register Failed\n");
    }

    return (retVal);
}

/**
 *  VpsMdrv_scDeInit
 *  \brief Scalar M2M driver exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns VPS_SOK on success else returns error value.
 */
Int32 VpsMdrv_scDeInit(Ptr arg)
{
    Int32               retVal = FVID2_SOK;
    UInt32              instCnt;
    VpsMdrv_ScInstObj  *instObj;

    for (instCnt = 0; instCnt < VPS_M2M_SC_INST_MAX; instCnt++)
    {
        instObj = &VpsMdrvScInstObjects[instCnt];
        /* Return if the driver is not initialized */
        if (FALSE == instObj->initDone)
        {
            retVal = FVID2_EFAIL;
            break;
        }
        /* Return if all the handles of the driver are not closed */
        if (instObj->openCnt != 0)
        {
            retVal = FVID2_EFAIL;
            break;
        }
    }

    if (FVID2_SOK == retVal)
    {
        for (instCnt = 0; instCnt < VPS_M2M_SC_INST_MAX; instCnt++)
        {
            instObj = &VpsMdrvScInstObjects[instCnt];
            instObj->qObjPool = NULL;
            instObj->drvInstId = VPS_M2M_SC_INST_MAX;
            Semaphore_delete(&instObj->instSem);
        }
        /* Unregister the driver */
        FVID2_unRegisterDriver(&VpsMdrvScOps);
        instObj->initDone = FALSE;
    }

    return (retVal);
}

/**
 *  vpsMdrvScAllocResrc
 *  \brief Allocates the resources requried for the driver from VRM
 */
Int32 vpsMdrvScAllocResrc(VpsMdrv_ScInstObj *instObj)
{
    Int32           retVal = FVID2_SOK, errRetVal;
    UInt32          resCnt, errCnt;

    GT_assert(VpsMdrvScTrace, (NULL != instObj));

    for (resCnt = 0u; resCnt < VPSMDRV_SC_MAX_RESR; resCnt++)
    {
        if (VRM_RESOURCE_INVALID == instObj->resrcId[resCnt])
        {
            /* Invalid resource id should be allocated */
            continue;
        }

        retVal = Vrm_allocResource(instObj->resrcId[resCnt]);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvScTrace, GT_ERR,
                "Could not allocate resource %d!\n", instObj->resrcId[resCnt]);
            break;
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
            GT_assert(VpsMdrvScTrace, (FVID2_SOK == errRetVal));
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScFreeResrc
 *  \brief Frees the resources to the VRM
 */
Void vpsMdrvScFreeResrc(VpsMdrv_ScInstObj *instObj)
{
    Int32           retVal = FVID2_SOK;
    UInt32          resCnt;

   /* Unreserve the resources */
    GT_assert(VpsMdrvScTrace, (NULL != instObj));

    for (resCnt = 0u; resCnt < VPSMDRV_SC_MAX_RESR; resCnt++)
    {
        if (VRM_RESOURCE_INVALID == instObj->resrcId[resCnt])
        {
            continue;
        }

        retVal = Vrm_releaseResource(instObj->resrcId[resCnt]);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvScTrace, GT_ERR,
                "Could not free resource %d!\n", instObj->resrcId[resCnt]);
            break;
        }
    }

    return;
}

/**
 *  vpsMdrvScAllocChDescs
 *  \brief Allocates the descriptors for each of the channels in
 *  handle.  Each channel will get four sets of descriptors.
 *  Two for the normal descriptors and two for the scratch pad descriptors
 */
static Int32 vpsMdrvScAllocChDescs(VpsMdrv_ScHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, descSetSize;
    UInt32              descMemPerCh = 0;
    VpsMdrv_ScChObj    *chObj;
    VpsMdrv_ScChObj    *sPadChObj;

    /* Initialize all the descriptor pointer to null  */
    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        hObj->chObjs[chCnt]->descMem = NULL;
    }

    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        descMemPerCh = 0;
        chObj = hObj->chObjs[chCnt];
        sPadChObj = hObj->sPadChObjs[chCnt];

        /* Add the total number of descriptors required */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            descMemPerCh += (VPSMDRV_SC_MAX_RT_COEFF_CFG_DESC *
                             VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            descMemPerCh +=
                (VPSMDRV_SC_MAX_NSHW_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            descMemPerCh +=
                (VPSMDRV_SC_MAX_SHW_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
        }
        descMemPerCh += chObj->numOutDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE;
        descMemPerCh += chObj->numInDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE;
        descMemPerCh +=
            (chObj->numSochDesc * VPSHAL_VPDMA_CTRL_DESC_SIZE);
        descMemPerCh +=
            (VPSMDRV_SC_MAX_RLD_DESC * VPSHAL_VPDMA_CTRL_DESC_SIZE);
        /* Multiplied by 2 is done because of the scratchpad descs */
        descMemPerCh = (descMemPerCh * VPSMDRV_SC_MAX_DESC_SET * 2u);

        /* Assert if the required number of descriptors are not allocated */
        GT_assert(VpsMdrvScTrace,
            (descMemPerCh <= VPSMDRV_SC_MAX_DESC_MEM_PER_CH));

        /* Store the desciptor set pointers in the channel object */
        GT_assert(VpsMdrvScTrace,
            (descMemPerCh <= VPSMDRV_SC_MAX_DESC_MEM_PER_CH));
        chObj->descMem = VpsUtils_allocDescMem(
                             VPSMDRV_SC_MAX_DESC_MEM_PER_CH,
                             VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        /* This size is for total 4 sets of desc Memories.  So give half
         * that is two sets to chObj and other half that is two set to
         * scratch pad objects */
        descSetSize = (VPSMDRV_SC_MAX_DESC_MEM_PER_CH >> 1);
        sPadChObj->descMem = ((UInt8 *) chObj->descMem + descSetSize);
        if (NULL == chObj->descMem)
        {
            retVal = FVID2_EALLOC;
            break;
        }
    }

    if (FVID2_SOK != retVal)
    {
        /* If allocation of the descriptors failed free the already
         * allocated descriptors.*/
        vpsMdrvScFreeChDescs(hObj->chObjs, hObj->numCh);
    }

    return (retVal);
}

/**
 *  vpsMdrvScAllocChOvlyMems
 *  \brief Allocates the overlay memory for each of the channels in
 *  handle.
 *  Shadow descriptors will also point to normal overlay memory instead
 *  of shadow overlay memory. So application submit multiple frames of the
 *  same channel but configuration will remain same for each of them.
 */
static Int32 vpsMdrvScAllocChOvlyMems(VpsMdrv_ScHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, coreCnt;
    UInt32              ovlyMemPerCh = 0;
    VpsMdrv_ScChObj    *chObj;

    /* Initialize the shadow memory pointers to NULL */
    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        chObj->nshwOvlyMem = NULL;
        chObj->shwOvlyMem = NULL;
    }

    if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
    {
        for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
        {
            chObj = hObj->chObjs[chCnt];

            /* Allocate non-shadow overlay memory */
            ovlyMemPerCh = 0;
            for (coreCnt = 0; coreCnt < VPSMDRV_SC_MAX_CORE; coreCnt++)
            {
                ovlyMemPerCh += chObj->totalNshwOvlyMem[coreCnt];
            }
            ovlyMemPerCh *= VPSMDRV_SC_MAX_DESC_SET;

            /* Assert if the overlay memory required is more than allocated */
            GT_assert(VpsMdrvScTrace,
                (ovlyMemPerCh <= VPSMDRV_SC_MAX_NSHW_OVLY_MEM));
            chObj->nshwOvlyMem = VpsUtils_allocDescMem(
                                     VPSMDRV_SC_MAX_NSHW_OVLY_MEM,
                                     VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
            if (NULL == chObj->nshwOvlyMem)
            {
                retVal = FVID2_EALLOC;
                break;
            }

            /* Allocate shadow overlay memory */
            ovlyMemPerCh = 0;
            for (coreCnt = 0; coreCnt < VPSMDRV_SC_MAX_CORE; coreCnt++)
            {
                ovlyMemPerCh += chObj->totalShwOvlyMem[coreCnt];
            }
            ovlyMemPerCh *= VPSMDRV_SC_MAX_DESC_SET;

            /* Assert if the overlay memory required is more than allocated */
            GT_assert(VpsMdrvScTrace,
                (ovlyMemPerCh <= VPSMDRV_SC_MAX_SHW_OVLY_MEM));
            chObj->shwOvlyMem = VpsUtils_allocDescMem(
                                    VPSMDRV_SC_MAX_SHW_OVLY_MEM,
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
            if (NULL == chObj->shwOvlyMem)
            {
                retVal = FVID2_EALLOC;
                break;
            }
        }
    }

    if (FVID2_SOK != retVal)
    {
        /* If allocation fails free the already allocated memories */
        vpsMdrvScFreeChOvlyMems(hObj->chObjs, hObj->numCh);
    }

    return (retVal);
}

/**
 *  vpsMdrvScAllocChObjs
 *  \brief Allocates the channelobjs for each channel of the handle
 */
Int32 vpsMdrvScAllocChObjs(VpsMdrv_ScHandleObj *hObj, UInt32 numCh)
{
    Int32       retVal = 0;
    UInt32      chCnt, sPadChCnt, freeChCnt;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    /* Initialize all the channel objects and scratch pad channel object
     * pointers to  NULL */
    for (chCnt = 0; chCnt < VPS_M2M_SC_MAX_CH; chCnt++)
    {
        hObj->chObjs[chCnt] = NULL;
        hObj->sPadChObjs[chCnt] = NULL;
    }

    /* Allocate channel objects for each  channel */
    for (chCnt = 0; chCnt < numCh; chCnt++)
    {
        hObj->chObjs[chCnt] = vpsMdrvScAllocChObj(VpsMdrvScChannelObjs);
        if (NULL == hObj->chObjs[chCnt])
        {
            retVal = FVID2_EALLOC;
            break;
        }
    }

    /* Allocate scratch pad channel objects for each channel */
    for (sPadChCnt = 0; sPadChCnt < numCh; sPadChCnt++)
    {
        hObj->sPadChObjs[sPadChCnt] =
            vpsMdrvScAllocSPadChObj(VpsMdrvScScPadChObjs);
        if (NULL == hObj->sPadChObjs[sPadChCnt])
        {
            retVal = FVID2_EALLOC;
            break;
        }
    }

    /* Free the allocated channel objects in case of error */
    if (FVID2_SOK != retVal)
    {
        /* If allocation fails free up the already allocated channel objs */
        for (freeChCnt = 0; freeChCnt < chCnt; freeChCnt++)
        {
            vpsMdrvScFreeChObj(hObj->chObjs[freeChCnt]);
        }
        for (freeChCnt = 0; freeChCnt < sPadChCnt; freeChCnt++)
        {
            vpsMdrvScFreeSPadChObj(hObj->sPadChObjs[freeChCnt]);
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScUpdatePrms
 *  \brief Allocates the params structure for each channel in the handle and
 *  copies the application params to driver params
 */
Int32 vpsMdrvScUpdatePrms(VpsMdrv_ScHandleObj *hObj,
                          Vps_M2mScCreateParams *params)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, channelFreeCnt;
    Vps_M2mScChParams  *inChPrms;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    GT_assert(VpsMdrvScTrace, (NULL != params));

    hObj->mode = params->mode;
    hObj->numCh = params->numChannels;

    /* Allocate the configuration parameters for each channel and
     * copy the user provided configuration parameters to the allocated
     * configuration paramters */
    inChPrms = params->chParams;
    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        hObj->chParams[chCnt] = vpsMdrvScAllocChParam();

        if (NULL == inChPrms)
        {
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EBADARGS: NULL Pointer in Chparams struct\n");
            retVal = FVID2_EBADARGS;
        }
        else if (NULL == hObj->chParams[chCnt])
        {
            retVal = FVID2_EALLOC;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Allocation of Channel params failed\n");
        }
        else
        {
            retVal = vpsMdrvScUpdateChPrms(
                        hObj,
                        hObj->chObjs[chCnt],
                        hObj->chParams[chCnt],
                        inChPrms);

            if (FVID2_SOK != retVal)
                break;
        }

        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            inChPrms++;
        }
    }

    /* Free the channel params in case of error */
    if (FVID2_SOK != retVal)
    {
        for (channelFreeCnt = 0; channelFreeCnt < chCnt; channelFreeCnt++)
        {
            vpsMdrvScFreeChParam(hObj->chParams[channelFreeCnt]);
        }
    }

    return (retVal);
}

/**
 *  vpsMdrvScUpdateChPrms
 *  \brief Update Channel Params from the given input params.
 */
Int32 vpsMdrvScUpdateChPrms(VpsMdrv_ScHandleObj *hObj,
                            VpsMdrv_ScChObj *chObj,
                            Vps_M2mScChParams *chPrms,
                            const Vps_M2mScChParams *inChPrms)
{
    Int32               retVal = FVID2_SOK;
    Vps_CropConfig     *cropCfg;
    Vps_SubFrameParams *locSlcPrms;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));
    GT_assert(VpsMdrvScTrace, (NULL != chObj));
    GT_assert(VpsMdrvScTrace, (NULL != chPrms));
    GT_assert(VpsMdrvScTrace, (NULL != inChPrms));

    VpsUtils_memcpy(
        &chPrms->inFmt,
        &inChPrms->inFmt,
        sizeof(FVID2_Format));
    VpsUtils_memcpy(
        &chPrms->outFmt,
        &inChPrms->outFmt,
        sizeof(FVID2_Format));
    if (NULL != inChPrms->scCfg)
    {
        VpsUtils_memcpy(
            chPrms->scCfg,
            inChPrms->scCfg,
            sizeof(Vps_ScConfig));
    }
    else
    {
        VpsUtils_memcpy(
            chPrms->scCfg,
            &VpsMdrvScDefScCfg,
            sizeof(Vps_ScConfig));
    }

    /* Crop configuration is optional from app.  If its not provided
     * cropping is set to input width and input height. */
    if (NULL != inChPrms->srcCropCfg)
    {
        VpsUtils_memcpy(
            chPrms->srcCropCfg,
            inChPrms->srcCropCfg,
            sizeof(Vps_CropConfig));
    }
    else
    {
        cropCfg = chPrms->srcCropCfg;
        cropCfg->cropStartX = 0;
        cropCfg->cropStartY = 0;
        cropCfg->cropWidth = chPrms->inFmt.width;
        cropCfg->cropHeight = chPrms->inFmt.height;
    }

    /* SubFrame configuration is optional from app.  If its not provided
     * Slicing is disabled as default.
     * if SubFrame mode is enabled, then sliceInst structure in chObj is
     * update based on configuration passed */
    if (NULL != inChPrms->subFrameParams)
    {
        VpsUtils_memcpy(
            chPrms->subFrameParams,
            inChPrms->subFrameParams,
            sizeof(Vps_SubFrameParams));

        /* Update sub-frame info in chObj required for scPh and offset
         * calculation*/
        chObj->slcChObj.subFrameMdEnable =
            chPrms->subFrameParams->subFrameModeEnable;
        chObj->slcChObj.slcSz =
            chPrms->subFrameParams->numLinesPerSubFrame;
        chObj->slcChObj.srcFrmH = chPrms->inFmt.height;
        chObj->slcChObj.dataFmt = chPrms->inFmt.dataFormat;
        chObj->slcChObj.tarFrmH = chPrms->outFmt.height;
        chObj->slcChObj.srcFrmW = chPrms->inFmt.width;
        chObj->slcChObj.tarFrmW = chPrms->outFmt.width;
        chObj->slcChObj.filtType = chPrms->scCfg->vsType;
        chObj->slcChObj.hSubFrmInfo.slcSz = chPrms->subFrameParams->numPixelsPerLine;
        chObj->slcChObj.isEnable = TRUE;
        chObj->slcChObj.hSubFrmInfo.isEnable = TRUE;

        if (0u == chObj->slcChObj.slcSz)
        {
            chObj->slcChObj.slcSz = chPrms->inFmt.height;
        }
        if (0u == chObj->slcChObj.hSubFrmInfo.slcSz)
        {
            chObj->slcChObj.hSubFrmInfo.slcSz = chPrms->inFmt.width;
        }

        if (chObj->slcChObj.slcSz == chPrms->inFmt.height)
        {
            chObj->slcChObj.isEnable = FALSE;
        }

        if (chObj->slcChObj.hSubFrmInfo.slcSz == chPrms->inFmt.width)
        {
            chObj->slcChObj.hSubFrmInfo.isEnable = FALSE;
        }

        /* check if sliceSize divides frame into Integer number of
         * subframes */
        if (0 == (chObj->slcChObj.srcFrmH % chObj->slcChObj.slcSz))
        {
            chObj->slcChObj.noOfSlcs =
                (chObj->slcChObj.srcFrmH / chObj->slcChObj.slcSz);
        }
        else
        {
            chObj->slcChObj.noOfSlcs =
                (chObj->slcChObj.srcFrmH / chObj->slcChObj.slcSz) + 1;
        }

        /* check if sliceSize divides frame into Integer number of
         * subframes */
        if (0 == (chObj->slcChObj.tarFrmW % chObj->slcChObj.hSubFrmInfo.slcSz))
        {
            chObj->slcChObj.hSubFrmInfo.noOfSlcs =
                (chObj->slcChObj.tarFrmW / chObj->slcChObj.hSubFrmInfo.slcSz);
        }
        else
        {
            chObj->slcChObj.hSubFrmInfo.noOfSlcs =
                (chObj->slcChObj.tarFrmW / chObj->slcChObj.hSubFrmInfo.slcSz) + 1;
        }
        if (TRUE == chObj->slcChObj.isEnable)
        {
            if (chPrms->inFmt.dataFormat == FVID2_DF_YUV420SP_UV)
            {
                if ((chObj->slcChObj.slcSz + VPSMDRV_VERT_SUBFRM_OFFSET) >
                      VPSMDRV_SUBFRM_MAX_HEIGHT)
                {
                    retVal = FVID2_EINVALID_PARAMS;
                }
            }
            else
            {
                if (chObj->slcChObj.slcSz > VPSMDRV_SUBFRM_MAX_HEIGHT)
                {
                    retVal = FVID2_EINVALID_PARAMS;
                }
            }
        }

    }
    else
    {
        locSlcPrms                      = chPrms->subFrameParams;
        locSlcPrms->subFrameModeEnable  = FALSE;
        locSlcPrms->numLinesPerSubFrame = chPrms->inFmt.height;

        /* update subFrameInfo in chObj required for scPh and offset calculation*/
        chObj->slcChObj.subFrameMdEnable = FALSE;
        chObj->slcChObj.slcSz       = chPrms->inFmt.height;
        chObj->slcChObj.srcFrmH     = chPrms->inFmt.height;
        chObj->slcChObj.dataFmt     = chPrms->inFmt.dataFormat;
        chObj->slcChObj.tarFrmH     = chPrms->outFmt.height;
        chObj->slcChObj.filtType    = chPrms->scCfg->vsType;
        chObj->slcChObj.srcFrmW     = chPrms->inFmt.width;
        chObj->slcChObj.tarFrmW     = chPrms->outFmt.width;
        chObj->slcChObj.noOfSlcs    = (chObj->slcChObj.srcFrmH/
                                        chObj->slcChObj.slcSz);
        chObj->slcChObj.hSubFrmInfo.isEnable = FALSE;
    }

    if (TRUE == chObj->slcChObj.hSubFrmInfo.isEnable)
    {
        /* Calculate the horizontal subframe information, subframe information is
           precalculated for horizontal side and this information is provided back
           to the application for allocating output buffer */
        retVal = vpsMdrvScSubFrameCalcHorzInfo(&chObj->slcChObj);
    }

    chPrms->inMemType = inChPrms->inMemType;
    chPrms->outMemType = inChPrms->outMemType;

    return (retVal);
}

/**
 *  vpsMdrvScFreeChObjs
 *  \brief Releases the channel objects for all the channels of handle.
 */
Void vpsMdrvScFreeChObjs(VpsMdrv_ScHandleObj *hObj)
{
    UInt32 chCnt;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        /* Free channel as well as scratch pad channel objects */
        vpsMdrvScFreeChObj(hObj->chObjs[chCnt]);
        vpsMdrvScFreeSPadChObj(hObj->sPadChObjs[chCnt]);
        hObj->chObjs[chCnt] = NULL;
        hObj->sPadChObjs[chCnt] = NULL;
    }

    return;
}

/**
 *  vpsMdrvScFreeChPrms
 *  \brief Releases the channel config params for all the channels of handle
 */
Void vpsMdrvScFreeChPrms(VpsMdrv_ScHandleObj *hObj)
{
    UInt32 chCnt;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    for (chCnt = 0; chCnt < hObj->numCh; chCnt++)
    {
        vpsMdrvScFreeChParam(hObj->chParams[chCnt]);
        hObj->chParams[chCnt] = NULL;
    }

    return;
}

/**
 *  vpsMdrvScFreeChOvlyMems
 *  \brief Releases ovleray memories for all the channels of the handle
 */
Void vpsMdrvScFreeChOvlyMems(VpsMdrv_ScChObj **chObjs, UInt32 numCh)
{
    UInt32 chCnt;

    for (chCnt = 0; chCnt < numCh; chCnt++)
    {
        if (NULL != chObjs[chCnt]->nshwOvlyMem)
        {
            VpsUtils_freeDescMem(
                chObjs[chCnt]->nshwOvlyMem,
                VPSMDRV_SC_MAX_NSHW_OVLY_MEM);
            chObjs[chCnt]->nshwOvlyMem = NULL;
        }

        if (NULL != chObjs[chCnt]->shwOvlyMem)
        {
            VpsUtils_freeDescMem(
                chObjs[chCnt]->shwOvlyMem,
                VPSMDRV_SC_MAX_SHW_OVLY_MEM);
            chObjs[chCnt]->shwOvlyMem = NULL;
        }
    }

    return;
}

/**
 *  vpsMdrvScFreeChDescs
 *  \brief Releases the descriptors for all the channels of handle
 */
static Void vpsMdrvScFreeChDescs(VpsMdrv_ScChObj **chObjs, UInt32 numCh)
{
    UInt32 chCnt;

    for (chCnt = 0; chCnt < numCh; chCnt++)
    {
        if (NULL != chObjs[chCnt]->descMem)
        {
            VpsUtils_freeDescMem(
                chObjs[chCnt]->descMem,
                VPSMDRV_SC_MAX_DESC_MEM_PER_CH);
            chObjs[chCnt]->descMem = NULL;
        }
    }

    return;
}

/**
 *  vpsMdrvScAllocHandleObj
 *  \brief Allocates handle objects from the pool.
 */
VpsMdrv_ScHandleObj *vpsMdrvScAllocHandleObj(void)
{
    UInt32                  hCnt;
    UInt32                  flag = FALSE;
    VpsMdrv_ScHandleObj    *hObj = NULL;

    for (hCnt = 0;
         hCnt < (VPS_M2M_SC_MAX_HANDLE * VPSMDRV_SC_MAX_EFF_INST);
         hCnt++)
    {
        hObj = &VpsMdrvScHandleObjects[hCnt];
        if (FALSE == hObj->isAllocated)
        {
            hObj->isAllocated = TRUE;
            flag = TRUE;
            break;
        }
    }

    if (TRUE == flag)
    {
        return (hObj);
    }
    else
    {
        return (NULL);
    }
}

/**
 *  vpsMdrvScAllocChObj
 *  \brief Allocates the channelobject from the pool
 */
static VpsMdrv_ScChObj *vpsMdrvScAllocChObj(VpsMdrv_ScChObj *chObjs)
{
    UInt32              chCnt;
    UInt32              flag = FALSE;
    VpsMdrv_ScChObj    *chObj = NULL;

    for (chCnt = 0; chCnt < VPSMDRV_SC_MAX_CH_MEM_POOL; chCnt++)
    {
        chObj = &chObjs[chCnt];
        if (FALSE == chObj->isAllocated)
        {
            chObj->isAllocated = TRUE;
            flag = TRUE;
            break;
        }
    }

    if (TRUE == flag)
    {
        return (chObj);
    }
    else
    {
        return (NULL);
    }
}

/**
 *  vpsMdrvScAllocSPadChObj
 *  \brief Allocates the scratch pad channel objects from the pool
 */
static VpsMdrv_ScChObj *vpsMdrvScAllocSPadChObj(VpsMdrv_ScChObj *sPadChObjs)
{
    UInt32              chCnt;
    UInt32              flag = FALSE;
    VpsMdrv_ScChObj    *sPadChObj = NULL;

    for (chCnt = 0; chCnt < VPSMDRV_SC_MAX_CH_MEM_POOL; chCnt++)
    {
        sPadChObj = &sPadChObjs[chCnt];
        if (FALSE == sPadChObj->isAllocated)
        {
            sPadChObj->isAllocated = TRUE;
            flag = TRUE;
            break;
        }
    }
    if (TRUE == flag)
    {
        return (sPadChObj);
    }
    else
    {
        return (NULL);
    }
}

/**
 *  vpsMdrvScAllocChParam
 *  \brief Allocates the configuration parameter from the pool
 */
static Vps_M2mScChParams *vpsMdrvScAllocChParam(Void)
{
    UInt32              prmCnt;
    VpsMdrv_ScChParams *chPrms = NULL;

    for (prmCnt = 0; prmCnt < VPSMDRV_SC_MAX_CH_MEM_POOL; prmCnt++)
    {
        if( FALSE == VpsMdrvScChParamsIsAllocated[prmCnt])
        {
            chPrms = &VpsMdrvScChParams[prmCnt];
            chPrms->chParams.scCfg = &chPrms->scCfg;
            chPrms->chParams.srcCropCfg = &chPrms->srcCropCfg;
            chPrms->chParams.subFrameParams= &chPrms->subFrameParams;
            VpsMdrvScChParamsIsAllocated[prmCnt] = TRUE;
            break;
        }
    }

    return (&chPrms->chParams);
}

/**
 *  vpsMdrvScFreeChObj
 *  \brief Returns the channel object to the pool
 */
static Void vpsMdrvScFreeChObj(VpsMdrv_ScChObj *chObj)
{
    GT_assert(VpsMdrvScTrace, (TRUE == chObj->isAllocated));
    chObj->isAllocated = FALSE;

    return;
}

/**
 *  vpsMdrvScFreeSPadChObj
 *  \brief Returns scratch pad channel object to the pool
 */
static Void vpsMdrvScFreeSPadChObj(VpsMdrv_ScChObj *sPadChObj)
{
    GT_assert(VpsMdrvScTrace, (TRUE == sPadChObj->isAllocated));
    sPadChObj->isAllocated = FALSE;

    return;
}

/**
 *  vpsMdrvScFreeHandleObj
 *  \brief Frees the handle object and returns it to the global pool
 */
Void vpsMdrvScFreeHandleObj(VpsMdrv_ScHandleObj *hObj)
{
    GT_assert(VpsMdrvScTrace, (TRUE == hObj->isAllocated));
    hObj->isAllocated = FALSE;

    return;
}

/**
 *  vpsMdrvScFreeChParam
 *  \brief Frees the configuration parameter and returns it to the pool
 */
static Void vpsMdrvScFreeChParam(Vps_M2mScChParams *chPrms)
{
    UInt32 prmCnt;

    for (prmCnt = 0; prmCnt < VPSMDRV_SC_MAX_CH_MEM_POOL; prmCnt++)
    {
        if (chPrms == &VpsMdrvScChParams[prmCnt].chParams)
        {
            GT_assert(VpsMdrvScTrace,
                (TRUE == VpsMdrvScChParamsIsAllocated[prmCnt]));
            VpsMdrvScChParamsIsAllocated[prmCnt] = FALSE;
            break;
        }
    }

    return;
}

/**
 *  vpsMdrvScAllocDescMem
 *  \brief Allocate the handle Object and initialize its required members
 */
Int32 vpsMdrvScAllocDescMem(VpsMdrv_ScHandleObj *hObj)
{
    Int32   retVal = FVID2_SOK;
    UInt32  descSet, i;
    UInt8  *descPtr;
    Int32   retStage = 0;

    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    hObj->descMem = NULL;
    hObj->nshwOvlyMem = NULL;
    hObj->shwOvlyMem = NULL;

    /* Allocate the set of non shadow config descriptor per handle */
    while (1)
    {
        if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
        {
            /* Allocate non-shadow descriptors */
            descPtr = VpsUtils_allocDescMem(
                          VPSMDRV_SC_NSHW_DESC_MEM,
                          VPSHAL_VPDMA_DESC_BYTE_ALIGN);
            if (NULL == descPtr)
            {
                retVal = FVID2_EALLOC;
                retStage = 1;
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                      "EALLOC: Non shadow config desc alloc failed\n");
                break;
            }

            for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
            {
                /* If the configuration is per handle then setup the
                 * coeff descriptor and shadow descriptor also. The reload
                 * descriptor is after the shadow config descriptor. */
                hObj->firstDesc[descSet] = descPtr;
                for (i = 0; i < VPSMDRV_SC_MAX_RT_COEFF_CFG_DESC; i++)
                {
                    hObj->coeffDesc[descSet][i] =
                        (VpsHal_VpdmaConfigDesc *) descPtr;
                    descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

                    VpsHal_vpdmaCreateDummyDesc(
                        hObj->coeffDesc[descSet][i]);
                }

                hObj->nshwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

                hObj->shwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

                hObj->rldDesc[descSet] = (VpsHal_VpdmaReloadDesc *) descPtr;
                descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }

            hObj->firstDescListSize =
                (VPSMDRV_SC_NSHW_DESC_MEM / VPSMDRV_SC_MAX_DESC_SET);

            /* Allocate the overlay memory for the non-shadow descriptor. */
            hObj->nshwOvlyMem = VpsUtils_allocDescMem(
                                    VPSMDRV_SC_MAX_NSHW_OVLY_MEM,
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
            if (NULL == hObj->nshwOvlyMem)
            {
                retVal = FVID2_EALLOC;
                retStage = 2;
                GT_0trace(VpsMdrvScTrace, GT_ERR,
                      "EALLOC: Non shadow ovly mem alloc failed\n");
                break;
            }
        }

        /* Allocate the descriptor sets for each channel */
        retVal = vpsMdrvScAllocChDescs(hObj);
        if (FVID2_SOK != retVal)
        {
            retVal = FVID2_EALLOC;
            retStage = 3;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Creation of Semaphore failed\n");
            break;
        }

        /* Allocate the overlay memories for each channel */
        retVal = vpsMdrvScAllocChOvlyMems(hObj);
        if (FVID2_SOK != retVal)
        {
            retVal = FVID2_EALLOC;
            retStage = 4;
            GT_0trace(VpsMdrvScTrace, GT_ERR,
                "EALLOC: Creation of Semaphore failed\n");
            break;
        }
        retStage = 5;
        break;
    }

    switch (retStage)
    {
        case 4:
            vpsMdrvScFreeChDescs(hObj->chObjs, hObj->numCh);
        case 3:
            if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
            {
                VpsUtils_freeDescMem(
                    hObj->nshwOvlyMem,
                    VPSMDRV_SC_MAX_NSHW_OVLY_MEM);
            }
        case 2:
            if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
            {
                VpsUtils_freeDescMem(
                    hObj->descMem,
                    VPSMDRV_SC_NSHW_DESC_MEM);
            }
        case 1:
            break;
        case 5:
            for (descSet = 0; descSet < VPSMDRV_SC_MAX_DESC_SET; descSet++)
            {
                /* Mark both descriptor set is free for use */
                hObj->isDescSetFree[descSet] = TRUE;
            }
            break;
    }

    return (retVal);
}

/**
 *  vpsMdrvScFreeDescMem
 *  \brief Frees the handle object and de-initializes its requried elements
 */
Int32 vpsMdrvScFreeDescMem(VpsMdrv_ScHandleObj *hObj)
{
    GT_assert(VpsMdrvScTrace, (NULL != hObj));

    vpsMdrvScFreeChDescs(hObj->chObjs, hObj->numCh);
    vpsMdrvScFreeChOvlyMems(hObj->chObjs, hObj->numCh);
    if (NULL != hObj->nshwOvlyMem)
    {
        VpsUtils_freeDescMem(hObj->nshwOvlyMem, VPSMDRV_SC_MAX_NSHW_OVLY_MEM);
        hObj->nshwOvlyMem = NULL;
    }
    if (NULL != hObj->descMem)
    {
        VpsUtils_freeDescMem(hObj->descMem, VPSMDRV_SC_NSHW_DESC_MEM);
        hObj->descMem = NULL;
    }

    return (FVID2_SOK);
}

/**
 *  vpsMdrvScGetInstObj
 *  \brief Get the instance object pointer for the instance id.
 */
VpsMdrv_ScInstObj* vpsMdrvScGetInstObj(UInt32 instId)
{
    GT_assert(VpsMdrvScTrace, (instId <= VPS_M2M_SC_INST_MAX));
    return (&VpsMdrvScInstObjects[instId]);
}

/**
 *  vpsMdrvScAllocQueObjs
 *  \brief Allocate the QObjs
 */
VpsMdrv_ScQueueObj *vpsMdrvScAllocQueObjs(void)
{
    UInt32              instCnt;
    VpsMdrv_ScQueueObj *qObj;

    for (instCnt = 0; instCnt < VPSMDRV_SC_MAX_EFF_INST; instCnt++)
    {
        if (TRUE == VpsMdrvScQbjsAllocated[instCnt])
        {
            qObj = NULL;
        }
        else
        {
            VpsMdrvScQbjsAllocated[instCnt] = TRUE;
            qObj = &VpsMdrvScQueueObjPool[instCnt][0];
            break;
        }
    }

    return (qObj);
}

/**
 *  vpsMdrvScFreeQueObjs
 *  \brief Allocate the QObjs
 */
Void vpsMdrvScFreeQueObjs(VpsMdrv_ScQueueObj *qObj)
{
    UInt32 instCnt;

    for (instCnt = 0; instCnt < VPSMDRV_SC_MAX_EFF_INST; instCnt++)
    {
        if (qObj == &VpsMdrvScQueueObjPool[instCnt][0])
        {
            VpsMdrvScQbjsAllocated[instCnt] = FALSE;
            break;
        }
    }
}

/**
 *  \brief Returns proper channel object based on index and descriptor set
 *  type to be used.
 */
VpsMdrv_ScChObj *vpsMdrvScGetChObj(VpsMdrv_ScHandleObj *hObj,
                                   UInt32 descSetType,
                                   UInt32 index)
{
    VpsMdrv_ScChObj *chObj;

    GT_assert(VpsMdrvScTrace, !(index > hObj->numCh));

    if (VPSMDRV_SC_NORMAL_DESC_SET == descSetType)
    {
        chObj = hObj->chObjs[index];
    }
    else
    {
        chObj = hObj->sPadChObjs[index];
    }

    return (chObj);
}

/**
 *  \brief Returns the Runtime parameters based on driver is opened per handle
 *  or per channel
 */
Vps_M2mScRtParams *vpsMdrvScGetRtPrms(VpsMdrv_ScHandleObj *hObj,
                                      FVID2_ProcessList *procList,
                                      UInt32 frmCnt)

{
    Vps_M2mScRtParams *rtParams;

    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
    {

        rtParams = procList->inFrameList[0]->perListCfg;
    }
    else
    {
        rtParams = procList->inFrameList[0]->frames[frmCnt]->perFrameCfg;
    }

    return (rtParams);
}
