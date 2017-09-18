/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_displayPriv.c
 *
 *  \brief VPS display driver internal file containing helper functions and
 *  allocation objects and functions.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

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
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_deiPath.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>
#include <ti/psp/vps/drivers/display/src/vpsdrv_displayCfg.h>
#include <ti/psp/vps/drivers/display/vpsdrv_display.h>
#include <ti/psp/vps/drivers/display/src/vpsdrv_displayPriv.h>


/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of inbound data descriptor to allocate per set.
 *  First row multi window descriptor * 2u for Y/C separate clients.
 *  DEI HQ Core     - 9 (3x2 previous fields, 2 MV in, 1 MVSTM in)
 */
#define VPSDDRV_MAX_IN_DESC             (VPS_CFG_MAX_MULTI_WIN_COL * 2u + 9u)

/**
 *  \brief Maximum number of outbound data descriptor to allocate per set.
 *  DEI HQ Core     - 6 (2 x 2 curr fld out, 1 MV OUT, 1 MVSTM OUT)
 */
#define VPSDDRV_MAX_OUT_DESC            (6u)

/**
 *  \brief Maximum number of mosaic data descriptor to allocate per set
 *  for mosaic display to program from 2nd row descriptors.
 */
#define VPSDDRV_MAX_MOSAIC_DESC         (VPS_CFG_MAX_MULTI_WIN_DESC)

/**
 *  \brief Maximum number of abort control descriptor to allocate per set
 *  used to abort all the clients before starting the display.
 *  DEI HQ Core IN  - 11 (2 x 3 prev flds, 2 x 1 curr fld, 2 MV IN, 1 MVSTM OUT)
 *  DEI HQ Core OUT - 6 (2 x 2 curr fld out, 1 MV OUT, 1 MVSTM OUT)
 */
#define VPSDDRV_MAX_ABORT_DESC          (17u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static Int32 vpsDdrvConfigBpSecCore(VpsDdrv_InstObj *instObj,
                                    const FVID2_Format *fmt);
static Int32 vpsDdrvConfigDeiCore(VpsDdrv_InstObj *instObj,
                                  const FVID2_Format *fmt);
static UInt32 vpsDdrvCalcMemReq(const VpsDdrv_MemObj *memObj,
                                const Vcore_DescInfo *descInfo,
                                const Vcore_Property *property);
static Void vpsDdrvSliceCoreMem(VpsDdrv_MemObj *memObj,
                                const Vcore_DescInfo *descInfo,
                                const Vcore_Property *property);
static Int32 vpsDdrvProgramDesc(VpsDdrv_InstObj *instObj,
                                VpsDdrv_MemObj *memObj);
static Int32 vpsDdrvProgramReg(VpsDdrv_InstObj *instObj,
                                VpsDdrv_MemObj *memObj);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** Display driver instance objects. */
static VpsDdrv_InstObj VpsDdrvInstObjects[VPS_DISPLAY_INST_MAX];


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vpsDdrvPrivInit
 *  Initializes driver objects, allocates memory etc.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDdrvPrivInit(UInt32 numInst, const VpsDdrv_InitPrms *initPrms)
{
    Int32               retVal = FVID2_SOK;
    UInt32              instCnt;
    UInt32              qObjCnt;
    VpsDdrv_InstObj    *instObj;
    Semaphore_Params    semPrms;
    VpsDdrv_QueObj     *qObj;

    /* Check for errors */
    GT_assert(VpsDdrvDispTrace, (numInst <= VPS_DISPLAY_INST_MAX));
    GT_assert(VpsDdrvDispTrace, (NULL != initPrms));

    /* Initialize required driver objects to sane state */
    for (instCnt = 0u; instCnt < VPS_DISPLAY_INST_MAX; instCnt++)
    {
        instObj = &VpsDdrvInstObjects[instCnt];

        instObj->freeQ = NULL;
        instObj->state.isInit = FALSE;
        instObj->instSem = NULL;
    }

    /* Initialize instance object members */
    for (instCnt = 0u; instCnt < numInst; instCnt++)
    {
        /* Core should be present */
        GT_assert(VpsDdrvDispTrace, (NULL != initPrms->coreOps));

        instObj = &VpsDdrvInstObjects[instCnt];

        /* Create free queue */
        instObj->freeQ = VpsUtils_createQ();
        if (NULL == instObj->freeQ)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "Queue creation failed!!\n");
            retVal = FVID2_EALLOC;
            break;
        }
        for (qObjCnt = 0u; qObjCnt < VPSDDRV_MAX_QOBJS_PER_INST; qObjCnt++)
        {
            qObj = &instObj->qObjPool[qObjCnt];

            /* Queue the queue object to free Q */
            VpsUtils_queue(instObj->freeQ, &qObj->qElem, qObj);
        }

        /* Allocate instance semaphore */
        Semaphore_Params_init(&semPrms);
        instObj->instSem = Semaphore_create(1u, &semPrms, NULL);
        if (NULL == instObj->instSem)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Instance semaphore create failed!!\n");
            retVal = FVID2_EALLOC;
            break;
        }

        /* Copy the information */
        instObj->drvInstId = initPrms->drvInstId;
        instObj->resrcId = initPrms->resrcId;
        instObj->dcNodeNum = initPrms->dcNodeNum;
        instObj->coreInstId = initPrms->coreInstId;
        instObj->coreOps = initPrms->coreOps;

        /* Initialize other variables */
        instObj->state.isOpened = FALSE;
        instObj->state.isStarted = FALSE;
        instObj->state.isMultiWinMode = FALSE;
        instObj->state.isInit = TRUE;

        /* Initialize Scalar Coefficients */
        instObj->coeffPrms.hScalingSet  = VPS_SC_US_SET;
        instObj->coeffPrms.vScalingSet = VPS_SC_US_SET;
        instObj->coeffPrms.horzCoeffMemPtr = NULL;
        instObj->coeffPrms.vertCoeffMemPtr = NULL;
        instObj->coeffPrms.vertBilinearCoeffMemPtr = NULL;
        instObj->coeffPrms.userCoeffPtr = NULL;

        /* Move to next parameter */
        initPrms++;
    }

    /* Deinit if error occurs */
    if (FVID2_SOK != retVal)
    {
        vpsDdrvPrivDeInit();
    }

    return (retVal);
}



/**
 *  vpsDdrvPrivDeInit
 *  Deallocates memory allocated by init function.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDdrvPrivDeInit(void)
{
    UInt32              instCnt;
    VpsDdrv_InstObj    *instObj;
    VpsDdrv_QueObj     *qObj;

    /* Deinitialize instance object members */
    for (instCnt = 0u; instCnt < VPS_DISPLAY_INST_MAX; instCnt++)
    {
        instObj = &VpsDdrvInstObjects[instCnt];

        instObj->drvInstId = 0u;
        instObj->state.isInit = FALSE;

        /* Delete the instance semaphore */
        if (NULL != instObj->instSem)
        {
            Semaphore_delete(&instObj->instSem);
            instObj->instSem = NULL;
        }

        if (NULL != instObj->freeQ)
        {
            /* Free-up all the queued free queue objects */
            while (1u)
            {
                qObj = VpsUtils_dequeue(instObj->freeQ);
                if (NULL == qObj)
                {
                    /* No more in queue */
                    break;
                }
            }

            /* Delete the free Q */
            VpsUtils_deleteQ(instObj->freeQ);
            instObj->freeQ = NULL;
        }
    }

    return (FVID2_SOK);
}



/**
 *  vpsDdrvGetInstObj
 *  Returns the instance object pointer for the instance id.
 */
VpsDdrv_InstObj *vpsDdrvGetInstObj(UInt32 instId)
{
    UInt32              instCnt;
    VpsDdrv_InstObj    *instObj = NULL;

    /* Find out the instance object to which this instance belongs to. */
    for (instCnt = 0u; instCnt < VPS_DISPLAY_INST_MAX; instCnt++)
    {
        if ((VpsDdrvInstObjects[instCnt].drvInstId == instId) &&
            (TRUE == VpsDdrvInstObjects[instCnt].state.isInit))
        {
            instObj = &VpsDdrvInstObjects[instCnt];
            break;
        }
    }

    return (instObj);
}



/**
 *  vpsDdrvFillCreateStatus
 *  Fills the driver create status information.
 */
void vpsDdrvFillCreateStatus(const VpsDdrv_InstObj *instObj,
                             Vps_DispCreateStatus *createStatus)
{
    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != createStatus));

    createStatus->standard = instObj->dcPathInfo.modeInfo.mInfo.standard;
    createStatus->dispWidth = instObj->dcPathInfo.modeInfo.mInfo.width;
    createStatus->dispHeight = instObj->dcPathInfo.modeInfo.mInfo.height;
    createStatus->minNumPrimeBuf = VPSDDRV_MIN_PRIME_BUFFERS;
    createStatus->maxReqInQueue = VPSDDRV_MAX_QOBJS_PER_INST;

    /* Check if the core supports multi window layouts. If not set the
     * values to one window (minimum needed!!). */
    if (NULL != instObj->coreOps->createLayout)
    {
        createStatus->maxMultiWin = VPS_CFG_MAX_MULTI_WIN;
        createStatus->maxMultiWinCol = VPS_CFG_MAX_MULTI_WIN_COL;
        createStatus->maxMultiWinRow = VPS_CFG_MAX_MULTI_WIN_ROW;
        createStatus->maxMultiWinLayout = VPSDDRV_MAX_MULTIWIN_SETTINGS;
    }
    else
    {
        createStatus->maxMultiWin = 1u;
        createStatus->maxMultiWinCol = 1u;
        createStatus->maxMultiWinRow = 1u;
        createStatus->maxMultiWinLayout = 0u;
    }

    return;
}



/**
 *  vpsDdrvConfigCore
 *  Configures the core according to the DCTRL/VENC settings or depending
 *  on the format passed by user. If fmt is NULL, then this is called from
 *  create time and hence sets the DCTRL settings to core.
 */
Int32 vpsDdrvConfigCore(VpsDdrv_InstObj *instObj, const FVID2_Format *fmt)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Property      property;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getProperty));

    /* Get core property */
    retVal = instObj->coreOps->getProperty(instObj->coreHandle, &property);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Could not get core property\n");
    }

    if (FVID2_SOK == retVal)
    {
        if ((VCORE_TYPE_DEI == property.name) ||
            (VCORE_TYPE_DEI_HQ == property.name))
        {
            /* Configure DEI core */
            retVal = vpsDdrvConfigDeiCore(instObj, fmt);
        }
        else
        {
            /* Configure BP or SEC core */
            retVal = vpsDdrvConfigBpSecCore(instObj, fmt);
        }
    }

    return (retVal);
}



/**
 *  vpsDrvAllocSliceCoreMem
 *  This function allocates the descriptor memory based on the core's
 *  requirement for a particular layout, slices the descriptor memory and
 *  assign core descriptor pointers and finally program the descriptor by
 *  calling core function.
 */
Int32 vpsDrvAllocSliceCoreMem(VpsDdrv_InstObj *instObj,
                              VpsDdrv_MemObj *memObj)
{
    Int32                   retVal = FVID2_SOK, tempRetVal;
    UInt32                  chCnt;
    UInt32                  totalDescMem;
    Vcore_DescInfo          descInfo;
    Vcore_Property          property;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != memObj));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getDescInfo));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->programDesc));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getProperty));

    GT_assert(VpsDdrvDispTrace, (NULL == memObj->descMem));
    memObj->descMem = NULL;
    memObj->descMemSize = 0u;

    /* Get core property */
    retVal = instObj->coreOps->getProperty(instObj->coreHandle, &property);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Could not get core property\n");
    }

    /* Get the required descriptor info from core */
    retVal = instObj->coreOps->getDescInfo(
                 instObj->coreHandle,
                 0u,
                 memObj->coreLayoutId,
                 &descInfo);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Core get desc info failed\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Assert if core's requirement is more than what we have */
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numInDataDesc <= VPSDDRV_MAX_IN_DESC));
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numOutDataDesc <= VPSDDRV_MAX_OUT_DESC));
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numMultiWinDataDesc <= VPSDDRV_MAX_MOSAIC_DESC));
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numChannels <= VPSDDRV_MAX_ABORT_DESC));
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numChannels < VCORE_MAX_VPDMA_CH));

        /* Caution: Assuming no overlay memory required by core */
        //TODO
        //GT_assert(VpsDdrvDispTrace, (0u == descInfo.shadowOvlySize));
        GT_assert(VpsDdrvDispTrace, (0u == descInfo.nonShadowOvlySize));
        //GT_assert(VpsDdrvDispTrace, (0u == descInfo.coeffOvlySize));

        /* Copy the abort channel information */
        memObj->numAbortCh = descInfo.numChannels;
        for (chCnt = 0u; chCnt < memObj->numAbortCh; chCnt++)
        {
            memObj->abortCh[chCnt] = descInfo.socChNum[chCnt];
        }

        /* Calculate the total descriptor memory required */
        totalDescMem = vpsDdrvCalcMemReq(memObj, &descInfo, &property);

        memObj->descMem = VpsUtils_allocDescMem(
                              totalDescMem,
                              VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        if (NULL == memObj->descMem)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Could not allocate descrioptor!\n");
            retVal = FVID2_EALLOC;
        }
        else
        {
            VpsUtils_memset(memObj->descMem, 0, totalDescMem);
            memObj->descMemSize = totalDescMem;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Assert if the core descriptor memory structure is insufficient to
         * hold all the descriptor memory pointers */
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numInDataDesc < VCORE_MAX_IN_DESC));
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numOutDataDesc < VCORE_MAX_OUT_DESC));
        GT_assert(VpsDdrvDispTrace,
            (descInfo.numMultiWinDataDesc < VCORE_MAX_MULTI_WIN_DESC));

        /* Slice the descriptor memory to give it to core */
        vpsDdrvSliceCoreMem(memObj, &descInfo, &property);

        /* Program Descriptor and Register using core APIs */
        retVal = vpsDdrvProgramDesc(instObj, memObj);
    }

    /* Undo if error occurs */
    if (FVID2_SOK != retVal)
    {
        tempRetVal = vpsDrvFreeCoreMem(instObj, memObj);
        GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
    }

    return (retVal);
}



/**
 *  vpsDrvFreeCoreMem
 *  Frees the allocated core memory and resets the core descriptor pointers.
 */
Int32 vpsDrvFreeCoreMem(VpsDdrv_InstObj *instObj, VpsDdrv_MemObj *memObj)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != memObj));

    VpsUtils_memset(
        memObj->coreDescMem,
        0u,
        (sizeof(Vcore_DescMem) * VPSDDRV_NUM_DESC_SET));
    VpsUtils_memset(&memObj->dcDescInfo, 0u, sizeof(Dc_DescInfo));
    VpsUtils_memset(&memObj->abortDesc, 0u, sizeof(memObj->abortDesc));
    if (NULL != memObj->descMem)
    {
        VpsUtils_freeDescMem(memObj->descMem, memObj->descMemSize);
        memObj->descMem = NULL;
        memObj->descMemSize = 0u;
    }

    return (retVal);
}



/**
 *  vpsDdrvConfigDctrl
 *  \brief Configures display controller and programs the register overlay.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDdrvConfigDctrl(VpsDdrv_InstObj *instObj)
{
    Int32                   retVal = FVID2_SOK, tempRetVal;
    UInt32                  dcMemSetFlag = FALSE;
    Dc_RtConfig             dcRtCfg;
    VpsDdrv_MemObj         *memObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->programReg));

    memObj = &instObj->memObj[instObj->curMemIdx];

    /* Set the input frame size to the display controller */
    dcRtCfg.scanFormat = instObj->vcompRtCfg.scanFormat;
    dcRtCfg.inputWidth = instObj->vcompRtCfg.inputWidth;
    dcRtCfg.inputHeight = instObj->vcompRtCfg.inputHeight;
    dcRtCfg.cropStartX = instObj->vcompRtCfg.cropStartX;
    dcRtCfg.cropStartY = instObj->vcompRtCfg.cropStartY;
    dcRtCfg.cropWidth = instObj->vcompRtCfg.cropWidth;
    dcRtCfg.cropHeight = instObj->vcompRtCfg.cropHeight;
    dcRtCfg.outCropStartX = instObj->vcompRtCfg.outCropStartX;
    dcRtCfg.outCropStartY = instObj->vcompRtCfg.outCropStartY;
    retVal = Dc_setInputFrameSize(instObj->dcHandle, &dcRtCfg);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Set input frame size failed\n");
    }

    if (TRUE != instObj->state.isMultiWinMode)
    {
        GT_assert(VpsDdrvDispTrace,
            (VPSDDRV_NON_MOSAIC_MEM_IDX == instObj->curMemIdx));

        /* Allocate, slice and program descriptor memory for non-mosaic
         * configuration */
        retVal = vpsDrvAllocSliceCoreMem(instObj, memObj);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Alloc and slice core memory failed\n");
        }
    }
    else
    {
        GT_assert(VpsDdrvDispTrace,
            (VPSDDRV_NON_MOSAIC_MEM_IDX != instObj->curMemIdx));
    }

    if (FVID2_SOK == retVal)
    {
        /* Set descriptor memory to display controller */
        retVal = Dc_setDescMem(instObj->dcHandle, &memObj->dcDescInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Set DCTRL descriptor memory failed\n");
        }
        else
        {
            dcMemSetFlag = TRUE;
        }
    }

    if (FVID2_SOK == retVal)
    {
       retVal = vpsDdrvProgramReg(instObj, memObj);
    }

    /* Undo if error occurs */
    if (FVID2_SOK != retVal)
    {
        /* Free the descriptor memory only in non-mosaic mode - we don't want
         * to implicitly delete an already created mosaic layout!! */
        if (TRUE != instObj->state.isMultiWinMode)
        {
            tempRetVal = vpsDrvFreeCoreMem(instObj, memObj);
            GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
        }

        if (TRUE == dcMemSetFlag)
        {
            /* Clear descriptor memory from DCTRL */
            tempRetVal = Dc_clrDescMem(instObj->dcHandle);
            GT_assert(VpsDdrvDispTrace, (FVID2_SOK == tempRetVal));
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvDeConfigDctrl
 *  \brief Clear the descriptor memory from display controller.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDdrvDeConfigDctrl(VpsDdrv_InstObj *instObj)
{
    Int32               retVal = FVID2_SOK;
    VpsDdrv_MemObj     *memObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    /* Clear descriptor memory from DCTRL */
    retVal = Dc_clrDescMem(instObj->dcHandle);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "Clear DCTRL descriptor memory failed\n");
    }

    /*
     * Note: Check for NOT mosaic flag is not done as user would have selected
     * mosaic layout at runtime while starting the display in non-mosaic mode.
     * This would have set the mosaic flag to TRUE but still we have allocated
     * memory for non-mosaic descriptor at start IOCTL.
     */
    /* Free-up allocated descriptor memory for non-mosaic configuration */
    memObj = &instObj->memObj[VPSDDRV_NON_MOSAIC_MEM_IDX];
    retVal = vpsDrvFreeCoreMem(instObj, memObj);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "Free descriptor memory failed\n");
    }

    return (retVal);
}



/**
 *  vpsDdrvDeleteLayout
 *  Deletes the mosaic layout, frees the allocated descriptor
 *  memories based on the passed layout ID.
 */
Int32 vpsDdrvDeleteLayout(VpsDdrv_InstObj *instObj, UInt32 layoutId)
{
    Int32               retVal = FVID2_SOK;
    VpsDdrv_MemObj     *memObj = NULL;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    /* Get the memory object for the layout ID */
    memObj = vpsDdrvGetMemObjForLayoutId(instObj, layoutId);
    if (NULL == memObj)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Invalid layout ID\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Check if this is the current selected layout */
        if (memObj == &instObj->memObj[instObj->curMemIdx])
        {
            /* Return error if display is in progress */
            if (TRUE == instObj->state.isStarted)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "Can't delete the current layout used for display\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
            else
            {
                /* Deleting a currently selected layout when display is NOT
                 * in progress - Deselect it and move to non-mosaic
                 * configuration */
                instObj->curMemIdx = VPSDDRV_NON_MOSAIC_MEM_IDX;
                instObj->state.isMultiWinMode = FALSE;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Free descriptor memory */
        retVal = vpsDrvFreeCoreMem(instObj, memObj);

        /* Delete core layout */
        GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->deleteLayout));
        retVal |= instObj->coreOps->deleteLayout(
                      instObj->coreHandle,
                      0u,
                      memObj->coreLayoutId);

        /* Free memory object */
        retVal |= vpdDdrvFreeMemObj(instObj, memObj);
    }

    return (retVal);
}



/**
 *  vpsDdrvDeleteAllLayout
 *  Deletes all the created layouts.
 */
Int32 vpsDdrvDeleteAllLayout(VpsDdrv_InstObj *instObj)
{
    Int32       retVal = FVID2_SOK;
    UInt32      cnt;
    UInt32      layoutId;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    /* Delete all valid layouts one by one */
    for (cnt = 0u; cnt < VPSDDRV_MAX_MEMORY_OBJECTS; cnt++)
    {
        /* Ignore the non-mosaic memory object */
        if (VPSDDRV_NON_MOSAIC_MEM_IDX == cnt)
        {
            continue;
        }

        layoutId = instObj->memObj[cnt].layoutId;
        if (VPSDDRV_INVALID_LAYOUT_ID != layoutId)
        {
            retVal = vpsDdrvDeleteLayout(instObj, layoutId);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsDdrvDispTrace, GT_ERR,
                    "Error deleting layout ID: %d\n", layoutId);
                break;
            }
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvAssignLayoutId
 *  Assigns an unique layout ID.
 */
Int32 vpsDdrvAssignLayoutId(VpsDdrv_InstObj *instObj, VpsDdrv_MemObj *memObj)
{
    Int32       retVal = FVID2_SOK;
    UInt32      cnt;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != memObj));

    for (cnt = 0u; cnt < VPSDDRV_MAX_MEMORY_OBJECTS; cnt++)
    {
        /* Ignore the non-mosaic memory object */
        if (VPSDDRV_NON_MOSAIC_MEM_IDX == cnt)
        {
            continue;
        }

        if (memObj == &instObj->memObj[cnt])
        {
            memObj->layoutId = cnt - 1u;
            break;
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvGetMemObjForLayoutId
 *  \brief Returns the matching memory object for the corresponding layout ID.
 */
VpsDdrv_MemObj *vpsDdrvGetMemObjForLayoutId(VpsDdrv_InstObj *instObj,
                                            UInt32 layoutId)
{
    UInt32              cnt;
    VpsDdrv_MemObj     *memObj = NULL;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    if (layoutId != VPSDDRV_INVALID_LAYOUT_ID)
    {
        for (cnt = 0u; cnt < VPSDDRV_MAX_MEMORY_OBJECTS; cnt++)
        {
            /* Ignore the non-mosaic memory object */
            if (VPSDDRV_NON_MOSAIC_MEM_IDX == cnt)
            {
                continue;
            }

            if (layoutId == instObj->memObj[cnt].layoutId)
            {
                memObj = &instObj->memObj[cnt];
                break;
            }
        }
    }

    return (memObj);
}



/**
 *  vpsDdrvGetIdxForMemObj
 *  \brief Returns the array index to the matching memory object.
 */
UInt32 vpsDdrvGetIdxForMemObj(const VpsDdrv_InstObj *instObj,
                              const VpsDdrv_MemObj *memObj)
{
    UInt32      cnt, curMemIdx = VPSDDRV_NON_MOSAIC_MEM_IDX;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != memObj));

    for (cnt = 0u; cnt < VPSDDRV_MAX_MEMORY_OBJECTS; cnt++)
    {
        if (memObj == &instObj->memObj[cnt])
        {
            curMemIdx = cnt;
            break;
        }
    }

    return (curMemIdx);
}



/**
 *  vpdDdrvAllocMemObj
 *  Allocates a free memory object.
 *  This function assumes the caller protects this call using a semaphore.
 */
VpsDdrv_MemObj *vpdDdrvAllocMemObj(VpsDdrv_InstObj *instObj)
{
    UInt32          cnt;
    VpsDdrv_MemObj *memObj = NULL;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));

    for (cnt = 0u; cnt < VPSDDRV_MAX_MEMORY_OBJECTS; cnt++)
    {
        /* Ignore the non-mosaic memory object */
        if (VPSDDRV_NON_MOSAIC_MEM_IDX == cnt)
        {
            continue;
        }

        if (VPSDDRV_INVALID_LAYOUT_ID == instObj->memObj[cnt].layoutId)
        {
            memObj = &instObj->memObj[cnt];
            memObj->layoutId = 0u;
            memObj->coreLayoutId = VCORE_DEFAULT_LAYOUT_ID;
            break;
        }
    }

    return (memObj);
}



/**
 *  vpdDdrvFreeMemObj
 *  Frees a memory object.
 *  This function assumes the caller protects this call using a semaphore.
 */
Int32 vpdDdrvFreeMemObj(VpsDdrv_InstObj *instObj, VpsDdrv_MemObj *memObj)
{
    Int32       retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != memObj));

    memObj->layoutId = VPSDDRV_INVALID_LAYOUT_ID;
    memObj->coreLayoutId = VCORE_DEFAULT_LAYOUT_ID;

    return (retVal);
}



/**
 *  vpsDdrvCheckFormat
 *  \brief Checks the format provided by the application.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDdrvCheckFormat(const VpsDdrv_InstObj *instObj,
                         const FVID2_Format *fmt)
{
    Int32   retVal = FVID2_SOK;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != fmt));

    /* Check if the dimensions are matching with that of the VENC */
    if (fmt->width > instObj->dcPathInfo.modeInfo.mInfo.width)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "INVALID_PARAMS: Width greater than destination width\n");
        retVal = FVID2_EINVALID_PARAMS;
    }
    if (fmt->height > instObj->dcPathInfo.modeInfo.mInfo.height)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "INVALID_PARAMS: Height greater than destination height\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    return (retVal);
}



/**
 *  vpsDdrvCheckDeiDispParams
 *  \brief Checks the Parameters provided by the application.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */
Int32 vpsDdrvCheckDeiDispParams(const VpsDdrv_InstObj *instObj,
                                const Vps_DeiDispParams *deiDispPrms)
{
    Int32   retVal = FVID2_SOK;
    UInt32  isVcompAvail = FALSE;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != deiDispPrms));

    if ((TRUE == instObj->dcPathInfo.isMainVcompAvail) ||
        (TRUE == instObj->dcPathInfo.isAuxVcompAvail))
    {
        isVcompAvail = TRUE;
    }

    /* Check if the dimensions are matching with that of the VENC */
    if (TRUE == deiDispPrms->scCfg.bypass)
    {
        if ((deiDispPrms->fmt.width > instObj->dcPathInfo.modeInfo.mInfo.width) ||
            (deiDispPrms->fmt.height > instObj->dcPathInfo.modeInfo.mInfo.height))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: input frame size greater than VENC frame size\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (TRUE == isVcompAvail)
    {
        if ((deiDispPrms->scTarWidth > instObj->dcPathInfo.modeInfo.mInfo.width) ||
            (deiDispPrms->scTarHeight > instObj->dcPathInfo.modeInfo.mInfo.height))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Sc Target Size is more than VENC Frame Size\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
    else
    {
        /* If there is no VCOMP, target size should be same as VENC size */
        if ((deiDispPrms->scTarWidth != instObj->dcPathInfo.modeInfo.mInfo.width) ||
            (deiDispPrms->scTarHeight != instObj->dcPathInfo.modeInfo.mInfo.height))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Sc Target Size is not equal to VENC Frame Size\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if ((TRUE == deiDispPrms->scCfg.bypass) &&
        (NULL != deiDispPrms->deiScCropCfg))
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "INVALID_PARAMS: Scalar cropping cannot be used when \
                scalar is bypassed\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    if ((FALSE == deiDispPrms->scCfg.bypass) && (NULL != deiDispPrms->deiScCropCfg))
    {
        if (((deiDispPrms->deiScCropCfg->cropStartX +
             deiDispPrms->deiScCropCfg->cropWidth) >
                deiDispPrms->fmt.width) ||
            ((deiDispPrms->deiScCropCfg->cropStartY +
             deiDispPrms->deiScCropCfg->cropHeight) >
             deiDispPrms->fmt.height))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: Crop Size is Invalid\n");
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (TRUE == deiDispPrms->scCfg.bypass)
    {
        /* Input width cannot be more than output width
           when scalar is bypassed.
           This is possible only in case VCOMP cropping is enabled,
           which is not supported. */
        if ((deiDispPrms->fmt.width > deiDispPrms->scTarWidth) ||
            (deiDispPrms->fmt.height > deiDispPrms->scTarHeight))
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "INVALID_PARAMS: output frame size is less than \
                    input frame size\n");
            retVal = FVID2_EINVALID_PARAMS;
        }

        if (FALSE == isVcompAvail)
        {
            if ((instObj->dcPathInfo.modeInfo.mInfo.width !=
                        deiDispPrms->scTarWidth) ||
                (instObj->dcPathInfo.modeInfo.mInfo.height !=
                        deiDispPrms->scTarHeight))
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "INVALID_PARAMS: Target frame size must be same as VENC \
                        frame size\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    if (NULL != deiDispPrms->posCfg)
    {
        if (TRUE == isVcompAvail)
        {
            if (((deiDispPrms->posCfg->startX +
                  deiDispPrms->scTarWidth) >
                    instObj->dcPathInfo.modeInfo.mInfo.width) ||
                ((deiDispPrms->posCfg->startY +
                  deiDispPrms->scTarHeight) >
                    instObj->dcPathInfo.modeInfo.mInfo.height))
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "INVALID_PARAMS: Wrong Position for the VCOMP input\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
        else
        {
            if (((deiDispPrms->posCfg->startX +
                  deiDispPrms->fmt.width) >
                    instObj->dcPathInfo.modeInfo.mInfo.width) ||
                ((deiDispPrms->posCfg->startY +
                  deiDispPrms->fmt.height) >
                    instObj->dcPathInfo.modeInfo.mInfo.height))
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "INVALID_PARAMS: Wrong Position for the VCOMP input\n");
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    if (TRUE == deiDispPrms->comprEnable)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR,
            "INVALID_PARAMS: Compression enable is not supported!!\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    return (retVal);
}



/**
 *  vpsDdrvConfigBpSecCore
 *  \brief Configures the BP/SEC core according to the format set.
 */
static Int32 vpsDdrvConfigBpSecCore(VpsDdrv_InstObj *instObj,
                                    const FVID2_Format *fmt)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    Vcore_Format        coreFmt;
    VpsDdrv_BufManObj  *bmObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getFormat));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->setFormat));
    bmObj = &instObj->bmObj;

    /* Get the default format from core first */
    retVal = instObj->coreOps->getFormat(
                 instObj->coreHandle,
                 0u,
                 &coreFmt);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "BP/SEC core get format failed\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Set core format depending upon the display control mode */
        coreFmt.frameWidth = instObj->dcPathInfo.modeInfo.mInfo.width;
        coreFmt.frameHeight = instObj->dcPathInfo.modeInfo.mInfo.height;
        coreFmt.secScanFmt = instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
        coreFmt.memType = instObj->memType;

        if (NULL == fmt)
        {
            /* Create time - Set the core format depending on DCTRL setting */
            coreFmt.startX = 0u;
            coreFmt.startY = 0u;
            coreFmt.fmt.width = instObj->dcPathInfo.modeInfo.mInfo.width;
            coreFmt.fmt.height = instObj->dcPathInfo.modeInfo.mInfo.height;
            coreFmt.fmt.scanFormat = instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
            if (FVID2_SF_PROGRESSIVE == instObj->dcPathInfo.modeInfo.mInfo.scanFormat)
            {
                for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                {
                    coreFmt.fmt.fieldMerged[cnt] = FALSE;
                }
            }
            else
            {
                for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                {
                    coreFmt.fmt.fieldMerged[cnt] = TRUE;
                }
            }
            /* Set the pitch depending on format and width */
            if ((FVID2_DF_YUV422I_UYVY == coreFmt.fmt.dataFormat) ||
                (FVID2_DF_YUV422I_YUYV == coreFmt.fmt.dataFormat) ||
                (FVID2_DF_YUV422I_YVYU == coreFmt.fmt.dataFormat) ||
                (FVID2_DF_YUV422I_VYUY == coreFmt.fmt.dataFormat))
            {
                /* YUV 422 interleaved - pitch is twice that of width */
                coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
                    coreFmt.fmt.width * 2u;
            }
            else
            {
                /* YUV 422 or 420 semiplanar - Y/CbCr pitch is same as width */
                coreFmt.fmt.pitch[FVID2_YUV_PL_Y_ADDR_IDX] =
                    coreFmt.fmt.width;
                coreFmt.fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] =
                    coreFmt.fmt.width;
            }
        }
        else
        {
            /* Set format IOCTL - Set the core format depending on user passed
             * format */
            coreFmt.startX =
                (instObj->dcPathInfo.modeInfo.mInfo.width - fmt->width) / 2u;
            coreFmt.startY =
                (instObj->dcPathInfo.modeInfo.mInfo.height - fmt->height) / 2u;

            /* Make X offset to even pixel alignment */
            coreFmt.startX &= ~0x01u;
            if ((FVID2_SF_INTERLACED == coreFmt.fmt.scanFormat) &&
                (FVID2_DF_YUV420SP_UV == coreFmt.fmt.dataFormat))
            {
                /* Make Y offset to multiple of 4 lines alignment for
                 * interlaced mode for YUV420 format */
                coreFmt.startY &= ~0x03u;
            }
            else
            {
                /* Make Y offset to even line alignment for progressive mode
                 * or for other formats */
                coreFmt.startY &= ~0x01u;
            }

            VpsUtils_memcpy(&coreFmt.fmt, fmt, sizeof(FVID2_Format));
        }

        /* Set the format to core */
        retVal = instObj->coreOps->setFormat(
                     instObj->coreHandle,
                     0u,
                     &coreFmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "BP/SEC core set format failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Set the mode depending on VENC format or user passed scan format */
        if (FVID2_SF_PROGRESSIVE == coreFmt.fmt.scanFormat)
        {
            bmObj->isProgressive = TRUE;
            bmObj->numPrgmedFrm = VPSDDRV_NUM_DESC_SET;
        }
        else
        {
            bmObj->isProgressive = FALSE;
            /* One frame takes two sets as frame (both fields) is queued to
             * driver at a time */
            bmObj->numPrgmedFrm = (VPSDDRV_NUM_DESC_SET / FVID2_MAX_FIELDS);
        }
    }

    return (retVal);
}



/**
 *  vpsDdrvConfigDeiCore
 *  \brief Configures the DEI core according to the format set.
 */
static Int32 vpsDdrvConfigDeiCore(VpsDdrv_InstObj *instObj,
                                  const FVID2_Format *fmt)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    Vcore_DeiParams     deiPrms;
    VpsDdrv_BufManObj  *bmObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getParams));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->setParams));
    bmObj = &instObj->bmObj;

    /* Get the default params from core */
    retVal = instObj->coreOps->getParams(
                 instObj->coreHandle,
                 0u,
                 &deiPrms);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "DEI core get params failed\n");
    }

    if (FVID2_SOK == retVal)
    {
        if (NULL == fmt)
        {
            /* Set core format depending upon the display control mode */
            deiPrms.frameWidth = instObj->dcPathInfo.modeInfo.mInfo.width;
            deiPrms.frameHeight = instObj->dcPathInfo.modeInfo.mInfo.height;
            deiPrms.secScanFmt = instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
            deiPrms.memType = instObj->memType;
            deiPrms.tarWidth = instObj->dcPathInfo.modeInfo.mInfo.width;
            deiPrms.tarHeight = instObj->dcPathInfo.modeInfo.mInfo.height;

            /* Initialize DEI Display Params with default values */
            instObj->deiDispParams.fmt.width = deiPrms.frameWidth;
            instObj->deiDispParams.fmt.height = deiPrms.frameHeight;
            instObj->deiDispParams.scTarWidth = deiPrms.tarWidth;
            instObj->deiDispParams.scTarHeight = deiPrms.tarHeight;
            instObj->deiDispParams.drnEnable = deiPrms.drnEnable = FALSE;
            instObj->deiDispParams.scCfg.bypass = TRUE;
            instObj->deiDispParams.scCfg.nonLinear = FALSE;
            instObj->deiDispParams.scCfg.stripSize = 0u;
            instObj->deiDispParams.scCfg.vsType = VPS_SC_VST_POLYPHASE;
            instObj->deiDispParams.scCfg.hsType = VPS_SC_HST_AUTO;
            instObj->deiDispParams.scCfg.enablePeaking= FALSE;
            instObj->deiDispParams.scCfg.enableEdgeDetect = FALSE;
            instObj->deiDispParams.deiHqCfg = NULL;
            instObj->deiDispParams.deiCfg = NULL;
            instObj->deiDispParams.deiScCropCfg = NULL;
            instObj->deiDispParams.comprEnable = FALSE;

            /* Create time - Set the core format depending on DCTRL setting */
            deiPrms.startX = 0u;
            deiPrms.startY = 0u;
            deiPrms.fmt.width = instObj->dcPathInfo.modeInfo.mInfo.width;
            deiPrms.fmt.height = instObj->dcPathInfo.modeInfo.mInfo.height;
            deiPrms.fmt.scanFormat =
                instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
            if (FVID2_SF_PROGRESSIVE ==
                instObj->dcPathInfo.modeInfo.mInfo.scanFormat)
            {
                for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                {
                    deiPrms.fmt.fieldMerged[cnt] = FALSE;
                }
            }
            else
            {
                for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                {
                    deiPrms.fmt.fieldMerged[cnt] = TRUE;
                }
            }
            deiPrms.fmt.dataFormat = FVID2_DF_YUV420SP_UV;
            /* Set the pitch depending on format and width */
            if ((FVID2_DF_YUV422I_UYVY == deiPrms.fmt.dataFormat) ||
                (FVID2_DF_YUV422I_YUYV == deiPrms.fmt.dataFormat) ||
                (FVID2_DF_YUV422I_YVYU == deiPrms.fmt.dataFormat) ||
                (FVID2_DF_YUV422I_VYUY == deiPrms.fmt.dataFormat))
            {
                /* YUV 422 interleaved - pitch is twice that of width */
                deiPrms.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
                    deiPrms.fmt.width * 2u;
            }
            else
            {
                /* YUV 422 or 420 semiplanar - Y/CbCr pitch is same as width */
                deiPrms.fmt.pitch[FVID2_YUV_PL_Y_ADDR_IDX] =
                    deiPrms.fmt.width;
                deiPrms.fmt.pitch[FVID2_YUV_SP_CBCR_ADDR_IDX] =
                    deiPrms.fmt.width;
            }

            VpsUtils_memcpy(
                &deiPrms.scCfg,
                &instObj->deiDispParams.scCfg,
                sizeof(Vps_ScConfig));
        }
        else
        {
            /* Set core format depending upon the user inputs,
               VPDMA positioning is used only if there is no VCOMP
               in the path and scalar is bypassed. If scalar is used
               with VPDMA mosaic, image portion with the background color
               will also get scaled. */
            if ((TRUE == instObj->dcPathInfo.isMainVcompAvail) ||
                (TRUE == instObj->dcPathInfo.isAuxVcompAvail) ||
                (FALSE == instObj->deiDispParams.scCfg.bypass))
            {
                deiPrms.frameWidth  = instObj->deiDispParams.fmt.width;
                deiPrms.frameHeight = instObj->deiDispParams.fmt.height;
            }
            else
            {
                /* Frame size is specified in scTar* variables in case vcomp
                   or scalar is not used */
                deiPrms.frameWidth  = instObj->deiDispParams.scTarWidth;
                deiPrms.frameHeight = instObj->deiDispParams.scTarHeight;
            }

            deiPrms.secScanFmt = instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
            deiPrms.memType    = instObj->memType;
            deiPrms.tarWidth   = instObj->deiDispParams.scTarWidth;
            deiPrms.tarHeight  = instObj->deiDispParams.scTarHeight;

            deiPrms.drnEnable  = instObj->deiDispParams.drnEnable;
            deiPrms.deiHqCtxMode = VPS_DEIHQ_CTXMODE_DRIVER_ALL;

            VpsUtils_memcpy(
                &deiPrms.scCfg,
                &instObj->deiDispParams.scCfg,
                sizeof(Vps_ScConfig));
            VpsUtils_memcpy(
                &deiPrms.fmt,
                fmt,
                sizeof(FVID2_Format));

            if (NULL != instObj->deiDispParams.deiScCropCfg)
            {
                VpsUtils_memcpy(
                    &deiPrms.cropCfg,
                    instObj->deiDispParams.deiScCropCfg,
                    sizeof(Vps_CropConfig));
            }
            else
            {
                deiPrms.cropCfg.cropStartX = 0u;
                deiPrms.cropCfg.cropStartY = 0u;
                deiPrms.cropCfg.cropWidth = fmt->width;
                deiPrms.cropCfg.cropHeight = fmt->height;
            }

            if (NULL != instObj->deiDispParams.deiCfg)
            {
                VpsUtils_memcpy(
                    &deiPrms.deiCfg,
                    instObj->deiDispParams.deiCfg,
                    sizeof(Vps_DeiConfig));
            }

            if (NULL != instObj->deiDispParams.deiHqCfg)
            {
                VpsUtils_memcpy(
                    &deiPrms.deiHqCfg,
                    instObj->deiDispParams.deiHqCfg,
                    sizeof(Vps_DeiHqConfig));
            }

            /* Set format IOCTL - Set the core format depending on user passed
             * format */
            deiPrms.startX = (deiPrms.frameWidth - fmt->width) / 2u;
            deiPrms.startY = (deiPrms.frameHeight - fmt->height) / 2u;

            /* Make X offset to even pixel alignment */
            deiPrms.startX &= ~0x01u;
            if ((FVID2_SF_INTERLACED == deiPrms.fmt.scanFormat) &&
                (FVID2_DF_YUV420SP_UV == deiPrms.fmt.dataFormat))
            {
                /* Make Y offset to multiple of 4 lines alignment for
                 * interlaced mode for YUV420 format */
                deiPrms.startY &= ~0x03u;
            }
            else
            {
                /* Make Y offset to even line alignment for progressive mode
                 * or for other formats */
                deiPrms.startY &= ~0x01u;
            }

            for (cnt = 0u; cnt < VCORE_DEI_MAX_COMPR; cnt++)
            {
                deiPrms.comprEnable[cnt] = instObj->deiDispParams.comprEnable;
            }
            for (cnt = 0u; cnt < VCORE_DEI_MAX_DCOMPR; cnt++)
            {
                deiPrms.dcomprEnable[cnt] = instObj->deiDispParams.comprEnable;
            }
        }

        /* Set the params to core */
        retVal = instObj->coreOps->setParams(
                     instObj->coreHandle,
                     0u,
                     &deiPrms);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR, "DEI Core set params failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Set the mode depending on VENC format or user passed scan format */
        if (FVID2_SF_PROGRESSIVE == deiPrms.fmt.scanFormat)
        {
            bmObj->isProgressive = TRUE;
            bmObj->numPrgmedFrm = VPSDDRV_NUM_DESC_SET;
        }
        else
        {
            bmObj->isProgressive = FALSE;
            /* One frame takes two sets as frame (both fields) is queued to
             * driver at a time */
            bmObj->numPrgmedFrm = (VPSDDRV_NUM_DESC_SET / FVID2_MAX_FIELDS);
        }
    }

    return (retVal);
}



/*
 */
static UInt32 vpsDdrvCalcMemReq(const VpsDdrv_MemObj *memObj,
                                const Vcore_DescInfo *descInfo,
                                const Vcore_Property *property)
{
    UInt32 totalDescMem;

    GT_assert(VpsDdrvDispTrace, (NULL != memObj));
    GT_assert(VpsDdrvDispTrace, (NULL != descInfo));
    GT_assert(VpsDdrvDispTrace, (NULL != property));

    /* Calculate the total descriptor memory required */
    totalDescMem = 0u;
    if ((descInfo->numOutDataDesc > 0u) || (memObj->numAbortCh > 0u))
    {
        totalDescMem +=
            (memObj->numAbortCh * VPSHAL_VPDMA_CTRL_DESC_SIZE);
        totalDescMem +=
            (descInfo->numOutDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE);
        /* One for reload descriptor at the end */
        totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;
    }
    if (descInfo->numInDataDesc > 0u)
    {
        totalDescMem +=
            (descInfo->numInDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE);
        /* One for reload descriptor at the end */
        totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;
    }
    if (descInfo->numMultiWinDataDesc > 0u)
    {
        totalDescMem +=
            (descInfo->numMultiWinDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE);
        /* One for reload descriptor at the end */
        totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;
    }

    if ((VCORE_TYPE_DEI == property->name) ||
        (VCORE_TYPE_DEI_HQ == property->name))
    {
        /* Allocate the config descriptor for nonshadow and coefficient
           config descriptor */
        totalDescMem +=
            (4 * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
        /* One for reload descriptor at the end */
        totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;
    }

    /* Allocate descriptor memory for all the sets */
    totalDescMem *= VPSDDRV_NUM_DESC_SET;

    return (totalDescMem);
}



static Void vpsDdrvSliceCoreMem(VpsDdrv_MemObj *memObj,
                                const Vcore_DescInfo *descInfo,
                                const Vcore_Property *property)
{
    UInt32 descSet, chCnt, descCnt;
    UInt8 *tempPtr, *tempPtr1;
    Dc_DescInfo *dcDescInfo;

    GT_assert(VpsDdrvDispTrace, (NULL != memObj));
    GT_assert(VpsDdrvDispTrace, (NULL != descInfo));
    GT_assert(VpsDdrvDispTrace, (NULL != property));

    /* Slice the descriptor memory to give it to core */
    tempPtr = memObj->descMem;

    GT_assert(VpsDdrvDispTrace, (NULL != tempPtr));

    dcDescInfo = &memObj->dcDescInfo;
    for (descSet = 0u; descSet < VPSDDRV_NUM_DESC_SET; descSet++)
    {
        memObj->abortDesc[descSet] = NULL;
        if ((descInfo->numOutDataDesc > 0u) || (memObj->numAbortCh > 0u))
        {
            /* Set the descriptor start address and set descriptor size
             * (abort desc + one extra for reload descriptor at the last) */
            dcDescInfo->descAddr[descSet][DC_OUT_DATA_IDX] = tempPtr;
            dcDescInfo->descSize[descSet][DC_OUT_DATA_IDX] =
                (memObj->numAbortCh * VPSHAL_VPDMA_CTRL_DESC_SIZE)
                + (descInfo->numOutDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE)
                + VPSHAL_VPDMA_CTRL_DESC_SIZE;

            /* Place the abort descrioptors above outbound descriptors */
            if (memObj->numAbortCh > 0u)
            {
                memObj->abortDesc[descSet] = tempPtr;
                for (chCnt = 0u; chCnt < memObj->numAbortCh; chCnt++)
                {
                    /* Initially program dummy descriptor instead of
                     * abort descriptor */
                    VpsHal_vpdmaCreateDummyDesc(tempPtr);
                    tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                }
            }

            for (descCnt = 0u; descCnt < descInfo->numOutDataDesc; descCnt++)
            {
                memObj->coreDescMem[descSet].outDataDesc[descCnt] = tempPtr;
                tempPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }

            /* Set the reload address */
            dcDescInfo->rldDescAddr[descSet][DC_OUT_DATA_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
        else
        {
            dcDescInfo->descAddr[descSet][DC_OUT_DATA_IDX] = NULL;
            dcDescInfo->descSize[descSet][DC_OUT_DATA_IDX] = 0u;
            dcDescInfo->rldDescAddr[descSet][DC_OUT_DATA_IDX] = NULL;
        }

        if (descInfo->numInDataDesc > 0u)
        {
            /* Set the descriptor start address and set descriptor size
             * (one extra for reload descriptor at the last) */
            dcDescInfo->descAddr[descSet][DC_IN_DATA_IDX] = tempPtr;
            dcDescInfo->descSize[descSet][DC_IN_DATA_IDX] =
                (descInfo->numInDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE)
                + VPSHAL_VPDMA_CTRL_DESC_SIZE;

            for (descCnt = 0u; descCnt < descInfo->numInDataDesc; descCnt++)
            {
                memObj->coreDescMem[descSet].inDataDesc[descCnt] = tempPtr;
                tempPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }

            /* Set the reload address */
            dcDescInfo->rldDescAddr[descSet][DC_IN_DATA_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
        else
        {
            dcDescInfo->descAddr[descSet][DC_IN_DATA_IDX] = NULL;
            dcDescInfo->descSize[descSet][DC_IN_DATA_IDX] = 0u;
            dcDescInfo->rldDescAddr[descSet][DC_IN_DATA_IDX] = NULL;
        }

        if (descInfo->numMultiWinDataDesc > 0u)
        {
            /* Set the descriptor start address and set descriptor size
             * (one extra for reload descriptor at the last) */
            dcDescInfo->descAddr[descSet][DC_MOSAIC_DATA_IDX] = tempPtr;
            dcDescInfo->descSize[descSet][DC_MOSAIC_DATA_IDX] =
                (descInfo->numMultiWinDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE)
                + VPSHAL_VPDMA_CTRL_DESC_SIZE;

            for (descCnt = 0u;
                 descCnt < descInfo->numMultiWinDataDesc;
                 descCnt++)
            {
                memObj->coreDescMem[descSet].multiWinDataDesc[descCnt] =
                    tempPtr;
                tempPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }

            /* Set the reload address */
            dcDescInfo->rldDescAddr[descSet][DC_MOSAIC_DATA_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
        else
        {
            dcDescInfo->descAddr[descSet][DC_MOSAIC_DATA_IDX] = NULL;
            dcDescInfo->descSize[descSet][DC_MOSAIC_DATA_IDX] = 0u;
            dcDescInfo->rldDescAddr[descSet][DC_MOSAIC_DATA_IDX] = NULL;
        }

        /* Caution: Assuming no shadow overlay memory required */
        memObj->coreDescMem[descSet].shadowOvlyMem = NULL;
        dcDescInfo->descAddr[descSet][DC_SHADOW_IDX] = NULL;
        dcDescInfo->descSize[descSet][DC_SHADOW_IDX] = 0u;
        dcDescInfo->rldDescAddr[descSet][DC_SHADOW_IDX] = NULL;

        /* Caution: Assuming no non shadow/coeff overlay memory required */
        memObj->coreDescMem[descSet].nonShadowOvlyMem = NULL;
        memObj->coreDescMem[descSet].coeffOvlyMem = NULL;
        dcDescInfo->descAddr[descSet][DC_NSHADOW_IDX] = NULL;
        dcDescInfo->descSize[descSet][DC_NSHADOW_IDX] = 0u;
        dcDescInfo->rldDescAddr[descSet][DC_NSHADOW_IDX] = NULL;
        if ((VCORE_TYPE_DEI == property->name) ||
            (VCORE_TYPE_DEI_HQ == property->name))
        {
            /* Assign pointer to nonshadow and coeff descriptors */
            dcDescInfo->descAddr[descSet][DC_NSHADOW_IDX] = tempPtr;
            tempPtr += (4 * VPSHAL_VPDMA_CONFIG_DESC_SIZE);

            dcDescInfo->descSize[descSet][DC_NSHADOW_IDX] =
                4 * VPSHAL_VPDMA_CONFIG_DESC_SIZE +
                    VPSHAL_VPDMA_CTRL_DESC_SIZE;

            dcDescInfo->rldDescAddr[descSet][DC_NSHADOW_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;

            /* Create dummy descriptor for shadow/nonshadow/coeff
               config descriptor */
            tempPtr1 = dcDescInfo->descAddr[descSet][DC_NSHADOW_IDX];
            VpsHal_vpdmaCreateDummyDesc(tempPtr1);
            tempPtr1 += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            VpsHal_vpdmaCreateDummyDesc(tempPtr1);
            tempPtr1 += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            VpsHal_vpdmaCreateDummyDesc(tempPtr1);
            tempPtr1 += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            VpsHal_vpdmaCreateDummyDesc(tempPtr1);
        }

        /* Print the mosaic descriptors in debug mode */
        if (((GT_DEBUG | GT_TraceState_Enable) == VpsDdrvDispTrace) &&
            (0u == descSet))
        {
            GT_0trace((GT_INFO | GT_TraceState_Enable), GT_INFO,
                "\n/****************************************************/\n"
                "Start of mosaic layout print....\n");
            GT_0trace((GT_INFO | GT_TraceState_Enable), GT_INFO,
                "[First Row Descriptors]\n");
            VpsHal_vpdmaPrintList(
                (UInt32) dcDescInfo->descAddr[descSet][DC_IN_DATA_IDX],
                (descInfo->numInDataDesc * 2u),
                NULL,
                NULL,
                1);
            GT_0trace((GT_INFO | GT_TraceState_Enable), GT_INFO,
                "\n[Non-First Row Descriptors]\n");
            VpsHal_vpdmaPrintList(
                (UInt32) dcDescInfo->descAddr[descSet][DC_MOSAIC_DATA_IDX],
                (descInfo->numMultiWinDataDesc * 2u),
                NULL,
                NULL,
                1);
            GT_0trace((GT_INFO | GT_TraceState_Enable), GT_INFO,
                "End of mosaic layout print!!\n"
                "/****************************************************/\n");
        }
    }
}

static Int32 vpsDdrvProgramDesc(VpsDdrv_InstObj *instObj,
                                VpsDdrv_MemObj *memObj)
{
    Int32                   retVal;
    UInt32                  descSet;

    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != memObj));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->programDesc));

    for (descSet = 0u; descSet < VPSDDRV_NUM_DESC_SET; descSet++)
    {
        /* Program the descriptor memories using core */
        retVal = instObj->coreOps->programDesc(
                     instObj->coreHandle,
                     0u,
                     memObj->coreLayoutId,
                     &memObj->coreDescMem[descSet]);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Core program descriptor memory failed\n");
            break;
        }
    }

    return (retVal);
}



static Int32 vpsDdrvProgramReg(VpsDdrv_InstObj *instObj,
                                VpsDdrv_MemObj *memObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt8                  *tempPtr;
    UInt32                  descSet;
    Dc_DescInfo            *dcDescInfo;
    Vcore_ScCoeffParams    *coeffPrms;
    Vcore_Property          property;
    Vcore_DescInfo          descInfo;

    GT_assert(VpsDdrvDispTrace, (NULL != instObj));
    GT_assert(VpsDdrvDispTrace, (NULL != memObj));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getDescInfo));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->getProperty));
    GT_assert(VpsDdrvDispTrace, (NULL != instObj->coreOps->programDesc));

    /* Get core property */
    retVal = instObj->coreOps->getProperty(instObj->coreHandle, &property);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Could not get core property\n");
    }

    /* Get the required descriptor info from core */
    retVal = instObj->coreOps->getDescInfo(
                 instObj->coreHandle,
                 0u,
                 memObj->coreLayoutId,
                 &descInfo);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvDispTrace, GT_ERR, "Core get desc info failed\n");
    }

    dcDescInfo = &memObj->dcDescInfo;
    for (descSet = 0u; descSet < VPSDDRV_NUM_DESC_SET; descSet++)
    {
        /* Program the descriptor memories and program the registers
         * directly in register using core */
        retVal = instObj->coreOps->programReg(
                     instObj->coreHandle,
                     0u,
                     NULL);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvDispTrace, GT_ERR,
                "Core program register failed\n");
            break;
        }

        if ((VCORE_TYPE_DEI == property.name) ||
            (VCORE_TYPE_DEI_HQ == property.name))
        {
            coeffPrms = &instObj->coeffPrms;
            /* Program scalar coefficients and get the pointer to the
               coefficients memory */
            retVal = instObj->coreOps->programScCoeff(
                         instObj->coreHandle,
                         0u,
                         coeffPrms);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvDispTrace, GT_ERR,
                    "Core program Sc Coeff failed\n");
            }

            if (0 == descSet)
            {
                tempPtr = dcDescInfo->descAddr[descSet][DC_NSHADOW_IDX];

                /* Create the configuration descriptor for
                   Coefficients Configuration */
                VpsHal_vpdmaCreateConfigDesc(
                    tempPtr,
                    descInfo.coeffConfigDest,
                    VPSHAL_VPDMA_CPT_BLOCK_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    descInfo.horzCoeffOvlySize,
                    coeffPrms->horzCoeffMemPtr,
                    NULL,
                    0u);

                tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

                VpsHal_vpdmaCreateConfigDesc(
                    tempPtr,
                    descInfo.coeffConfigDest,
                    VPSHAL_VPDMA_CPT_BLOCK_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    descInfo.vertCoeffOvlySize,
                    coeffPrms->vertCoeffMemPtr,
                    (Ptr) (descInfo.horzCoeffOvlySize),
                    descInfo.horzCoeffOvlySize);

                if (0 < descInfo.vertBilinearCoeffOvlySize)
                {
                    tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;

                    VpsHal_vpdmaCreateConfigDesc(
                        tempPtr,
                        descInfo.coeffConfigDest,
                        VPSHAL_VPDMA_CPT_BLOCK_SET,
                        VPSHAL_VPDMA_CCT_INDIRECT,
                        descInfo.vertBilinearCoeffOvlySize,
                        coeffPrms->vertBilinearCoeffMemPtr,
                        (Ptr) (descInfo.horzCoeffOvlySize +
                               descInfo.vertCoeffOvlySize),
                        descInfo.vertBilinearCoeffOvlySize);
                }
            }
        }
    }

    return (retVal);
}
