/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpsdrv_dispGrpxPriv.c
 *
 *  \brief VPS GRPX Display driver internal file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/sysbios/knl/Semaphore.h>

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


/**
 *  \brief scaling ratio configure parameter size(bytes)
 */
#define VPSDDRV_GRPX_SC_CONFIG_SIZE       (16u)

/**
 *  \brief horizontal scalar parameter size in memory (bytes)
 *     since the scaling coefficient is stored in the 16bits, so x2
 *     to make it in 8bits(byte)
 *     (scaling coefficent size + horizontal ratio size)
 */
#define VPSDDRV_GRPX_SC_HOR_SIZE          (2 * VPS_GRPX_HOR_COEFF_SIZE + \
                                              VPSDDRV_GRPX_SC_CONFIG_SIZE)
/**
 *  \brief vertical scalar parameter size in memory (bytes)
 *     since the scaling coefficient is stored in the 16bits, so x2
 *     to make it in 8bits(byte)
 *   (scaling coefficient size + vertical ratio)
 */
#define VPSDDRV_GRPX_SC_VER_SIZE          (2 * VPS_GRPX_VER_COEFF_SIZE + \
                                             VPSDDRV_GRPX_SC_CONFIG_SIZE)
/**
 *  \brief one complete scalar parameter set size in memory(bytes)
 *   this include: horizontal scaling coefficeints + horizontal scaling ratio
 *   + vertical scaling coefficeitns + vertical scaling ratio
 */
#define VPSDDRV_GRPX_ONE_SC_MEM_SIZE      (VPSDDRV_GRPX_SC_HOR_SIZE + \
                                             VPSDDRV_GRPX_SC_VER_SIZE)
/**
 *  \brief GRPX frame attribute size in memory
 */
#define VPSDDRV_GRPX_FRAME_ATTR_SIZE      (16u)

/**
 *  \brief one set config memory size(bytes)
 *   it contains frame attribute + one set of saling
 *   parameters since now all regions sharing the same
 *   coefficients and ratio. If in the future, each region can
 *   has its own coefficients and ratio, then one set of
 *   scaling paramters is need removed from this MACRO

 */
#define VPSDDRV_GRPX_ONE_CONFIG_MEM_SIZE  (VPSDDRV_GRPX_ONE_SC_MEM_SIZE + \
                                               VPSDDRV_GRPX_FRAME_ATTR_SIZE)

/**
 *  \brief total config memory size(bytes) required by the GRPX
 *  display driver, since for dispaly we need two set of non shadow
 *  memory to store ping and pong set
 */

#define VPSDDRV_GRPX_CONFIG_MEM_SIZE      (VPSDDRV_GRPX_ONE_CONFIG_MEM_SIZE * \
                                               DC_NUM_DESC_SET)

/**
 *  \brief total scaling parameters memory size(bytes) required by the GRPX
 *  display driver, since for dispaly we need two set of memory to store
 *  scaling paramter for the ping and pong
 */
#define VPSDDRV_GRPX_SC_MEM_SIZE          (VPSDDRV_GRPX_ONE_SC_MEM_SIZE * \
                                               DC_NUM_DESC_SET)

/**
 *  \brief Number of the inbound data descriptor per region
 *  each region need two data descriptors: one for stenciling data,
 *  one for normal grpx data.
 */
#define VPSDDRV_GRPX_NUM_DATA_DESCS_REGION      (2u)

/**
 *  \brief Number of the inbound data descriptor per set
 *    one more additional data descriptor is required for the CLUT data.
 *    this is for the first region only, the rest region is in the MOSAIC
 *    desc.
 */
#define VPSDDRV_GRPX_MAX_IN_DESC (VPSDDRV_GRPX_NUM_DATA_DESCS_REGION * \
                                         1u + 1u)

/**
 *  \brief Maximum number of mosaic data descriptor to allocate per set
 *  for mosaic display to program from 2nd row descriptors.
 */
#define VPSDDRV_GRPX_MAX_MOSAIC_DESC   ((VPSDDRV_GRPX_MAX_REGIONS - 1)* \
                                          VPSDDRV_GRPX_NUM_DATA_DESCS_REGION)

/**
 *  \brief Maximum number of abort control descriptor to allocate per set
 *  used to abort all the clients before starting the display.
 *  Max 3 - CLUT, Stenciling and GRPX Data.
 */
#define VPSDDRV_GRPX_MAX_ABORT_DESC          (3u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/**  \brief GRPX Display driver instance objects. */
static VpsDdrv_GrpxInstObj  VpsDdrvGrpxInstObjects[VPS_DISP_GRPX_MAX_INST];

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */


/**
 *  vpsDdrvGrpxPrivInit
 *  Initializes driver objects, allocates memory etc.
 *  Returns FVID2_SOK on success else returns appropriate error code.
*/
Int32 vpsDdrvGrpxPrivInit(UInt32 numInst,
                        const VpsDdrv_GrpxInitParams *initParams)
{

    Int32                    retVal = FVID2_SOK;
    UInt32                   instCnt;
    UInt32                   qObjCnt;
    UInt32                   descSet;
    UInt32                   scNum;
    UInt32                   dataSet;
    VpsDdrv_GrpxInstObj      *instObj;
    VpsDdrv_GrpxQueueObj     *qObj;
    Semaphore_Params         semParams;

    /* Check for errors */
    GT_assert(VpsDdrvGrpxTrace, (numInst <= VPS_DISP_GRPX_MAX_INST));
    GT_assert(VpsDdrvGrpxTrace, (NULL != initParams));

    /* Init variable to zero state */
    for (instCnt = 0u; instCnt < VPS_DISP_GRPX_MAX_INST; instCnt++)
    {
        instObj = &VpsDdrvGrpxInstObjects[instCnt];

        instObj->freeGrpxQ = NULL;
        instObj->state.isInit = FALSE;
        instObj->instSem = NULL;
        instObj->memType = VPSHAL_VPDMA_MT_NONTILEDMEM;
    }

    /* Initialize instance object members */
    for (instCnt = 0; instCnt < numInst; instCnt++)
    {
        GT_assert(VpsDdrvGrpxTrace, (NULL != initParams[instCnt].coreOps));
        instObj = &VpsDdrvGrpxInstObjects[instCnt];
        /*create grpx free queue to queuing the Q*/
        instObj->freeGrpxQ = VpsUtils_createQ();
        if (NULL == instObj->freeGrpxQ)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "GRPX Queue creation failed.\n");
            retVal = FVID2_EALLOC;
            break;
        }
        for (qObjCnt = 0u; qObjCnt < VPSDDRV_GRPX_MAX_QOBJS_PER_INST;
                                                            qObjCnt++)
        {
            qObj = &instObj->qObjPool[qObjCnt];

            /*Put qObj into the Queue*/
            VpsUtils_queue(instObj->freeGrpxQ, &qObj->qElem, qObj);
        }

        /* Allocate instance semaphore */
        Semaphore_Params_init(&semParams);
        instObj->instSem = Semaphore_create(1u, &semParams, NULL);
        if (NULL == instObj->instSem)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "Instance semaphore create failed!!\n");
            retVal = FVID2_EALLOC;
            break;
        }

        /* store the information */
        instObj->drvInstId = initParams[instCnt].drvInstId;
        instObj->resourceId = initParams[instCnt].resourceId;
        instObj->dcNodeNum = initParams[instCnt].dcNodeNum;
        instObj->coreInstId = initParams[instCnt].coreInstId;
        instObj->coreOps = initParams[instCnt].coreOps;

        /* Initialize other variables */
        instObj->state.isFbMode = FALSE;
        instObj->state.isOpened = FALSE;
        instObj->state.isStarted = FALSE;
        instObj->state.isMultiRegion = FALSE;
        instObj->state.isInit = TRUE;
        instObj->numRegions = 0u;
        /* Assuming non Frame Buffer mode for the display driver. This
           should be changed depending on whether graphics is opened
           from the FBDEV driver on Linux or not */
        instObj->dcClientInfo.isFbMode = FALSE;
        /* Initialize the descriptor and virtual register params*/
        instObj->descParams.configMem = NULL;
        instObj->descParams.numInDataDesc = 0u;
        instObj->descParams.numOutDataDesc = 0u;
        instObj->descParams.numMultiDataDesc = 0u;
        instObj->descParams.configMemSize = 0u;
        instObj->descParams.confgiMemOffset = 0u;
        instObj->descParams.configDest = VPSHAL_VPDMA_CONFIG_DEST_SC_GRPX0;
        /*init sc memory*/
        for (scNum = 0; scNum < VPSDDRV_GRPX_MAX_REGIONS; scNum++)
        {
            instObj->descParams.scMem[scNum] = NULL;
        }

        for (descSet = 0; descSet < DC_NUM_DESC_SET; descSet++)
        {
            instObj->descParams.configDesc[descSet] = NULL;
            instObj->configSet[descSet] = FALSE;
            instObj->clutSet[descSet] = FALSE;

            instObj->descParams.coreDescMem[descSet].shadowOvlyMem = NULL;
            instObj->descParams.coreDescMem[descSet].nonShadowOvlyMem = NULL;
            instObj->descParams.coreDescMem[descSet].coeffOvlyMem = NULL;

            for (dataSet = 0u; dataSet < VCORE_MAX_IN_DESC; dataSet++)
            {
                instObj->descParams.coreDescMem[descSet].
                                       inDataDesc[dataSet] = NULL;
            }
            for (dataSet = 0u; dataSet < VCORE_MAX_OUT_DESC; dataSet++)
            {
                instObj->descParams.coreDescMem[descSet].
                                      outDataDesc[dataSet] =NULL;
            }

            for (dataSet = 0u; dataSet < VCORE_MAX_MULTI_WIN_DESC; dataSet++)
            {
                instObj->descParams.coreDescMem[descSet].
                                      multiWinDataDesc[dataSet] =NULL;
            }

        }

    }
    if (FVID2_SOK != retVal)
    {
        vpsDdrvGrpxPrivDeInit();
    }
    return retVal;
}
/**
 *  vpsDdrvGrpxPrivDeInit
 *  Deallocates memory allocated by init function.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */

Int32 vpsDdrvGrpxPrivDeInit(void)
{
    UInt32                   instCnt;
    VpsDdrv_GrpxInstObj      *instObj;

    for (instCnt = 0; instCnt < VPS_DISP_GRPX_MAX_INST; instCnt++)
    {
        instObj = &VpsDdrvGrpxInstObjects[instCnt];
        if (TRUE == instObj->state.isInit)
        {
            instObj->drvInstId = VPS_DISP_GRPX_MAX_INST;
            if (NULL != instObj->instSem)
            {
                Semaphore_delete(&instObj->instSem);
                instObj->instSem = NULL;
            }

            if (NULL != instObj->freeGrpxQ)
            {
                /* Free-up all the queued free queue objects */
                while(VpsUtils_dequeue(instObj->freeGrpxQ) != NULL);
                /* Delete the free Q */
                VpsUtils_deleteQ(instObj->freeGrpxQ);
                instObj->freeGrpxQ = NULL;
            }

            instObj->state.isInit = FALSE;
        }

    }
    return (FVID2_SOK);
}

/**
 *  vpsDdrvGrpxGetInstObj
 *  Returns the instance object pointer for the instance id.
 */

VpsDdrv_GrpxInstObj *vpsDdrvGrpxGetInstObj(UInt32 instId)
{
    UInt32 instCnt;
    VpsDdrv_GrpxInstObj *instObj = NULL;

    for (instCnt = 0; instCnt< VPS_DISP_GRPX_MAX_INST; instCnt++)
    {
        if ((instId == VpsDdrvGrpxInstObjects[instCnt].drvInstId) &&
            (TRUE == VpsDdrvGrpxInstObjects[instCnt].state.isInit))
            {
                instObj = &VpsDdrvGrpxInstObjects[instCnt];
                break;
            }

    }
    return (instObj);
}

/**
 * vpsDdrvGrpxSetCore
 * Configure the core based on both VENC settings and app input. This
 * functin will recapture the current VENC mode information
 */
UInt32 vpsDdrvGrpxSetCore(VpsDdrv_GrpxInstObj *instObj,
                           const FVID2_Format        *fmt)
{
    Int32               retVal = FVID2_SOK;
    UInt32              bpp;
    UInt32              cnt;
    Vcore_Format        cFmt;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->getFormat));
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->setFormat));


    /* Get the default format from core first */
    retVal = instObj->coreOps->getFormat(
                 instObj->coreHandle,
                 0u,
                 &cFmt);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR, "GRPX Core get format failed\n");
    }

    if (FVID2_SOK == retVal)
    {
        /* Set core format depending upon the display control mode */
        cFmt.frameWidth = instObj->dcPathInfo.modeInfo.mInfo.width;
        cFmt.frameHeight = instObj->dcPathInfo.modeInfo.mInfo.height;
        cFmt.secScanFmt = instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
        cFmt.memType = instObj->memType;

         /* Initialize runtime config, which are used by the DC now */
        instObj->dcRtConfig.scanFormat =
                        instObj->dcPathInfo.modeInfo.mInfo.scanFormat;
        instObj->dcRtConfig.inputWidth =
                    (UInt16)instObj->dcPathInfo.modeInfo.mInfo.width;
        instObj->dcRtConfig.inputHeight =
                    (UInt16)instObj->dcPathInfo.modeInfo.mInfo.height;
        instObj->dcRtConfig.cropStartX = 0u;
        instObj->dcRtConfig.cropStartY = 0u;
        instObj->dcRtConfig.cropWidth =
                    (UInt16)instObj->dcPathInfo.modeInfo.mInfo.width;
        instObj->dcRtConfig.cropHeight =
                    (UInt16)instObj->dcPathInfo.modeInfo.mInfo.height;
        instObj->dcRtConfig.outCropStartX = 0u;
        instObj->dcRtConfig.outCropStartY = 0u;
        cFmt.startX = 0u;
        cFmt.startY = 0u;

        if (NULL == fmt)
        {
            /* Create time - Set the core format depending on DCTRL setting */
            cFmt.fmt.height = instObj->dcPathInfo.modeInfo.mInfo.height;
            cFmt.fmt.width = instObj->dcPathInfo.modeInfo.mInfo.width;

            /* By default make the mode the same as VENC format
               It will be overridden when called set format IOCTL */
            if(FVID2_SF_PROGRESSIVE == instObj->dcPathInfo.modeInfo.mInfo.scanFormat)
            {
                cFmt.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
                for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                {
                    cFmt.fmt.fieldMerged[cnt] = FALSE;
                }
            }
            else
            {
                cFmt.fmt.scanFormat = FVID2_SF_INTERLACED;
                for (cnt = 0u; cnt < FVID2_MAX_PLANES; cnt++)
                {
                    cFmt.fmt.fieldMerged[cnt] = TRUE;
                }
            }

            /*get the bits per pixel from the data format and calculate the pitch*/
            retVal = vpsDdrvGrpxGetBppFromFmt(cFmt.fmt.dataFormat, &bpp);
            if (FVID2_SOK == retVal)
            {
                cFmt.fmt.pitch[FVID2_RGB_ADDR_IDX] =
                                (cFmt.fmt.width * bpp) >> 3;
                if (0u != (cFmt.fmt.pitch[FVID2_RGB_ADDR_IDX] & 0xF))
                    cFmt.fmt.pitch[FVID2_RGB_ADDR_IDX] += 16 -
                         (cFmt.fmt.pitch[FVID2_RGB_ADDR_IDX] & 0xF);
            }
            else
            {
                GT_1trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Unsupported data format %d.\n", cFmt.fmt.dataFormat);
            }

        }
        else
        {
            VpsUtils_memcpy(&cFmt.fmt, fmt, sizeof(FVID2_Format));
        }

        retVal = instObj->coreOps->setFormat(
                instObj->coreHandle,
                0u,
                &cFmt);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "GRPX Set Core Format Failed.\n");

        }

    }
    /*update the buf*/
    if (FVID2_SOK == retVal)
    {
        if(FVID2_SF_PROGRESSIVE == cFmt.fmt.scanFormat)
        {
            instObj->bmObj.numPrgmedReg = DC_NUM_DESC_SET;
            instObj->bmObj.isProgressive = TRUE;
        }
        else
        {
            instObj->bmObj.numPrgmedReg = (DC_NUM_DESC_SET / FVID2_MAX_FIELDS);
            instObj->bmObj.isProgressive = FALSE;
        }
    }

    return (retVal);

}
/**
 *  vpsDdrvGrpxStop
 *  Stop the GRPX streaming
 */
UInt32 vpsDdrvGrpxStop(VpsDdrv_GrpxInstObj *instObj)
{
    UInt32 retVal = FVID2_SOK;
    VpsDdrv_GrpxQueueObj     *qObj;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));

    /* Stop graphics */
    retVal = Dc_stopClient(instObj->dcHandle);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "ERROR: Stop client failed\n");
    }
    else
    {
        instObj->state.isStarted = FALSE;
    }


    if (FVID2_SOK == retVal)
    {
        /* Deallocate descriptor memory */
        retVal = vpsDdrvGrpxDeConfigDctrl(instObj);
        if (FVID2_SOK !=retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "ERROR: Core Deconfig memory failed\n");
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Take the buffers in current state and push them in to the InQ
         * so that the next start will use those buffers */
        if (FALSE == instObj->state.isFbMode)
        {
            while (NULL !=
                    (qObj = vpsDdrvGrpxRemoveBufFromState(
                                        &instObj->bmObj, TRUE)))
            {
                /* Push it to the InQ */
                VpsUtils_queueBack(instObj->bmObj.inQHandle,
                                   &qObj->qElem,
                                   qObj);
            }
        }
        else
        {
            /*push the buffer back*/
            VpsUtils_queueBack(instObj->bmObj.inQHandle,
                               &instObj->bmObj.fbQObj->qElem,
                               instObj->bmObj.fbQObj);
        }
    }

    return retVal;
}
/**
 *  vpsDrvCanCurBufBeDequeued
 *  Returns TRUE if the current set frame could be put in the done queue
 *  by checking if the frame is present in any of the buffer management states
 *  like other sets, semi programmed buffer etc... Else this returns FALSE.
 */
UInt32 vpsDdrvGrpxCanCurBufBeDequeued(const VpsDdrv_GrpxBufManObj *bmObj,
                                           UInt32 curSet)
{
    UInt32                   descSet;
    UInt32                   safeFlag = TRUE;


    /*sanity check*/
    GT_assert(VpsDdrvGrpxTrace, (NULL != bmObj));
    GT_assert(VpsDdrvGrpxTrace, (curSet < DC_NUM_DESC_SET));

    /* If the current buffer is NULL, we can't dequeue*/
    if ( NULL == bmObj->prgmedBuf[curSet])
    {
        safeFlag = FALSE;
    }

    /* Check whether the current buffer is programmed some where else in the
     * programmed buffer set */
    for (descSet = 0u; descSet < bmObj->numPrgmedReg; descSet++)
    {

        /* Leave out the current set. It is going to be TRUE always!! */
        if (curSet != descSet)
        {
            if (bmObj->prgmedBuf[curSet] ==
                         bmObj->prgmedBuf[descSet])
            {
                /* Current set buffer is used in other sets!!
                 * So can't dequeue current buffer */
                safeFlag = FALSE;
                break;
            }
        }
    }

    /* Check with other state variables */
    if (bmObj->prgmedBuf[curSet] == bmObj->sPrgmedBuf)
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
VpsDdrv_GrpxQueueObj *vpsDdrvGrpxGetNextBufState(
                                                VpsDdrv_GrpxBufManObj *bmObj,
                                                UInt32 curSet,
                                                UInt32 nextSet,
                                                UInt32 curFid)
{
    VpsDdrv_GrpxQueueObj     *nextQObj = NULL, *newQObj;

    /*sanity check*/
    GT_assert(VpsDdrvGrpxTrace, (NULL != bmObj));
    GT_assert(VpsDdrvGrpxTrace, (curSet < DC_NUM_DESC_SET));


    /* For interlaced we virtually deal with half the set. Since
     * two sets are required to program a buffer, two DC set acts
     * like a buffer management set!! */
    if (TRUE != bmObj->isProgressive)
    {
        curSet /= FVID2_MAX_FIELDS;
        nextSet /= FVID2_MAX_FIELDS;
    }

    /* Check if we can dequeue the current buffer */
    if(TRUE == bmObj->isProgressive)
    {
        /* Get new buffer from the InQ */
        newQObj = VpsUtils_dequeue(bmObj->inQHandle);
        if (NULL != newQObj)
        {
            /* Program the new buffer */
            bmObj->prgmedBuf[nextSet] = newQObj;
            nextQObj = newQObj;
        }
        else
        {
            /* No new buffer available - program the previous buffer if they
             * are not the same */
             GT_assert(VpsDdrvGrpxTrace, (NULL != bmObj->prgmedBuf[curSet]));
            if (bmObj->prgmedBuf[nextSet] !=
                                   bmObj->prgmedBuf[curSet])
            {
                bmObj->prgmedBuf[nextSet] =
                                       bmObj->prgmedBuf[curSet];
                /* we need send this duplicate Q out since we need have both
                 * set to be set to the same information*/
                nextQObj = bmObj->prgmedBuf[curSet];
            }

        }
    }
    else
    {
        if(0u == curFid)
        {
            /* Make previously programmed field buffer as current buffer */
            GT_assert(VpsDdrvGrpxTrace, (NULL != bmObj->sPrgmedBuf));
            nextQObj = bmObj->sPrgmedBuf;
            bmObj->prgmedBuf[curSet] = bmObj->sPrgmedBuf;
            bmObj->sPrgmedBuf = NULL;

        }
        else
        {

            /* Get new buffer from the InQ */
            newQObj = VpsUtils_dequeue(bmObj->inQHandle);
            if (NULL != newQObj)
            {
                /* Program the new buffer */
                bmObj->sPrgmedBuf = newQObj;
                nextQObj = newQObj;
            }
            else
            {
                GT_assert(VpsDdrvGrpxTrace,
                            (NULL != bmObj->prgmedBuf[curSet]));
                /* No new buffer available - program the current buffer again
                 * to the next set if needed */
                if (bmObj->prgmedBuf[nextSet] != bmObj->prgmedBuf[curSet])
                {
                    nextQObj = bmObj->prgmedBuf[curSet];
                }
                bmObj->sPrgmedBuf = bmObj->prgmedBuf[curSet];
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
VpsDdrv_GrpxQueueObj *vpsDdrvGrpxRemoveBufFromState(
                                               VpsDdrv_GrpxBufManObj *bmObj,
                                               UInt32 reverseOrder)
{
    UInt32                   descSet, setIndex;
    VpsDdrv_GrpxQueueObj     *qObj = NULL;

    /*sanity check*/
    GT_assert(VpsDdrvGrpxTrace, (NULL != bmObj));

    /* When reverse order is TRUE, then give back the semi programmed buffer
     * before programmed buffers. */
    if (TRUE == reverseOrder)
    {
        /* Give back the semi programmed buffer if any */
        if (NULL != bmObj->sPrgmedBuf)
        {
            qObj = bmObj->sPrgmedBuf;
            bmObj->sPrgmedBuf = NULL;

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

            setIndex = bmObj->expectedSet / FVID2_MAX_FIELDS;
        }

        if (TRUE == reverseOrder)
        {
            for (descSet = 0u; descSet < bmObj->numPrgmedReg; descSet++)
            {
                /* Wrap around */
                if (0u == setIndex)
                {
                    setIndex = (bmObj->numPrgmedReg - 1u);
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
            for (descSet = 0u; descSet < bmObj->numPrgmedReg; descSet++)
            {
                /* Wrap around */
                if (setIndex >= bmObj->numPrgmedReg)
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
            if (NULL != bmObj->sPrgmedBuf)
            {
                qObj = bmObj->sPrgmedBuf;
                bmObj->sPrgmedBuf = NULL;
            }
        }
    }

    /* Reset all the similar buffers in the set. Otherwise we might return
     * the same buffer again. */
    if (NULL != qObj)
    {
        for (descSet = 0u; descSet < bmObj->numPrgmedReg; descSet++)
        {
            if (qObj == bmObj->prgmedBuf[descSet])
            {
                bmObj->prgmedBuf[descSet] = NULL;
            }
        }
        if(qObj == bmObj->sPrgmedBuf)
        {
            bmObj->sPrgmedBuf = NULL;
        }
    }


    return (qObj);
}

/**
 *  vpsDdrvGrpxAllocDescMem
 *  This function allocates the descriptor memory based on the core's
 *  requirement and assign core descriptor pointers and finally program
 *  the descriptor by calling core function.
 */

Int vpsDdrvGrpxAllocDescMem(VpsDdrv_GrpxInstObj *instObj,
                            VpsDdrv_GrpxDescParams   *descParams)
{

    Int32                       retVal = FVID2_SOK;
    Int32                       tempRetVal, abCnt;
    UInt32                      descSet, descCnt;
    UInt8                       *tempPtr, *tempCfgPtr;
    UInt32                      totalDescMem;
    Vcore_DescInfo              descInfo;
    Dc_DescInfo                 *dcDescInfo;
    UInt32                      regCnt;


    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    GT_assert(VpsDdrvGrpxTrace, (NULL != descParams));
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->getDescInfo));
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->programDesc));

    retVal = instObj->coreOps->getDescInfo(
                 instObj->coreHandle,
                 0u,
                 0u,
                 &descInfo);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                   "ERROR: Core get desc info failed\n");
    }

    if (FVID2_SOK == retVal)
    {

        GT_assert(VpsDdrvGrpxTrace,
            ((descInfo.numInDataDesc <= VPSDDRV_GRPX_MAX_IN_DESC) &&
            (descInfo.numInDataDesc > 0u)));
        GT_assert(VpsDdrvGrpxTrace,
            (descInfo.numMultiWinDataDesc <= VPSDDRV_GRPX_MAX_MOSAIC_DESC));
        /* coeff is required by grpx core*/
        GT_assert(VpsDdrvGrpxTrace,
            ((  (descInfo.horzCoeffOvlySize + descInfo.vertCoeffOvlySize)
             <= VPSDDRV_GRPX_ONE_CONFIG_MEM_SIZE) &&
            ((descInfo.horzCoeffOvlySize + descInfo.vertCoeffOvlySize) > 16u)));

        GT_assert(VpsDdrvGrpxTrace,
            (descInfo.numChannels <= VPSDDRV_GRPX_MAX_ABORT_DESC));
        GT_assert(VpsDdrvGrpxTrace,
            (descInfo.numChannels < VCORE_MAX_VPDMA_CH));

        /* no shadow/nonshadow overlay memory required by grpx core */
        GT_assert(VpsDdrvGrpxTrace, (0u == descInfo.shadowOvlySize));
        GT_assert(VpsDdrvGrpxTrace, (0u == descInfo.nonShadowOvlySize));
        /* no outbound descriptor required by grpx core*/
        GT_assert(VpsDdrvGrpxTrace, (0u == descInfo.numOutDataDesc));

        /*store abort channel info*/
        descParams->numAbortCh = descInfo.numChannels;
        for (abCnt = 0; abCnt < descParams->numAbortCh; abCnt++)
        {
            descParams->abortCh[abCnt] = descInfo.socChNum[abCnt];
        }
        /* Calculate the total descriptor memory required */
        totalDescMem = 0u;
        /*abort descriptor */
        if (descParams->numAbortCh > 0u)
        {
            totalDescMem += (descParams->numAbortCh *
                        VPSHAL_VPDMA_CTRL_DESC_SIZE);
            /*reload descriptor*/
            totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }

        /*inbound descriptor is mandatory*/
        totalDescMem += descInfo.numInDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE;
        /*this is for reload descriptor*/
        totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;

        if (descInfo.numMultiWinDataDesc > 0u)
        {
            totalDescMem +=
                descInfo.numMultiWinDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE;
            /*add one for relaod descriptor*/
            totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
        /*config descriptor is mandatory.*/
        totalDescMem += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
        totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;

        totalDescMem *= DC_NUM_DESC_SET;

        descParams->descMem = VpsUtils_allocDescMem(
                                  totalDescMem,
                                  VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        if (NULL == descParams->descMem)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "Failed to allocate descrioptor!\n");
            retVal = FVID2_EALLOC;

        }
        else
        {
            descParams->descMemSize = totalDescMem;
        }
    }

    /* allocate the frame configuraiton memory including the coeff
     * Plan to let have this as config memory for the first region only
     * and not sharing the coeff with other regions*/

    if (FVID2_SOK == retVal)
    {
        descParams->configMem = VpsUtils_allocDescMem(
                                    VPSDDRV_GRPX_CONFIG_MEM_SIZE,
                                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
        if (NULL == descParams->configMem)
        {
            GT_0trace(VpsDdrvGrpxTrace,GT_ERR,
                " failed to allocate GRPX config memory\n");
            retVal = FVID2_EALLOC;
        }
    }

    /*allocate the coeff config memory for rest of the regions
      this is not implemented, but plan to let region has its
      own coeff instead sharing*/
    if (FVID2_SOK == retVal)
    {
        for (regCnt = 0; regCnt < (descInfo.numMultiWinDataDesc /
                VPSDDRV_GRPX_NUM_DATA_DESCS_REGION); regCnt++)
        {
            descParams->scMem[regCnt] =
                VpsUtils_allocDescMem(
                    VPSDDRV_GRPX_SC_MEM_SIZE,
                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
            if (NULL == descParams->scMem[regCnt])
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "failed to allocate GRPX scalar coeff memory.\n");
                retVal = FVID2_SOK;
            }
        }
    }


    if (FVID2_SOK == retVal)
    {
        tempPtr = descParams->descMem;
        tempCfgPtr = descParams->configMem;

        dcDescInfo = & descParams->dcDescInfo;
        /*copy to local*/
        descParams->configMemSize =
            descInfo.horzCoeffOvlySize + descInfo.vertCoeffOvlySize;
        descParams->configDest = descInfo.coeffConfigDest;
        descParams->numInDataDesc= descInfo.numInDataDesc;
        descParams->numOutDataDesc= descInfo.numOutDataDesc;
        descParams->numMultiDataDesc =
                          descInfo.numMultiWinDataDesc;
        /* For frame it is 1, for coefficient it is 2*/
        descParams->confgiMemOffset = 1u;

        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet++)
        {

            /* No shadow overlay memory required for GRPX*/
            descParams->coreDescMem[descSet].shadowOvlyMem = NULL;
            dcDescInfo->descAddr[descSet][DC_SHADOW_IDX] = NULL;
            dcDescInfo->descSize[descSet][DC_SHADOW_IDX] = 0u;
            dcDescInfo->rldDescAddr[descSet][DC_SHADOW_IDX] = NULL;

            /*abort descriptor for GRPX*/
            descParams->abortDesc[descSet] = NULL;
            if (descParams->numAbortCh > 0u)
            {
                /*set the abort descriptor address and size
                 (abort descriptro + reload descriptor)*/
                dcDescInfo->descAddr[descSet][DC_OUT_DATA_IDX] = tempPtr;
                dcDescInfo->descSize[descSet][DC_OUT_DATA_IDX] =
                    (descParams->numAbortCh * VPSHAL_VPDMA_CTRL_DESC_SIZE) +
                    VPSHAL_VPDMA_CTRL_DESC_SIZE;
                /*create abort descriptor in ping set
                 dummy descriptor on pong set*/
                descParams->abortDesc[descSet] = tempPtr;
                for (abCnt = 0; abCnt < descParams->numAbortCh; abCnt++)
                {
                    if (descSet == 0)
                    {
                        VpsHal_vpdmaCreateAbortCtrlDesc(tempPtr,
                                                descParams->abortCh[abCnt]);
                    }
                    else
                    {
                        VpsHal_vpdmaCreateDummyDesc(tempPtr);
                    }
                    tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                }
                /*set the reload address*/
                dcDescInfo->rldDescAddr[descSet][DC_OUT_DATA_IDX] = tempPtr;
                tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            else
            {
                dcDescInfo->descAddr[descSet][DC_OUT_DATA_IDX] = NULL;
                dcDescInfo->descSize[descSet][DC_OUT_DATA_IDX] = 0u;
                dcDescInfo->rldDescAddr[descSet][DC_OUT_DATA_IDX] = NULL;
            }
            /* Set the descriptor start address and set descriptor size
             * for the first region inbound descriptor
             * (one extra for reload descriptor at the last */

            dcDescInfo->descAddr[descSet][DC_IN_DATA_IDX] = tempPtr;
            dcDescInfo->descSize[descSet][DC_IN_DATA_IDX] =
                (descInfo.numInDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE) +
                VPSHAL_VPDMA_CTRL_DESC_SIZE;
            for(descCnt = 0u; descCnt < descInfo.numInDataDesc; descCnt++)
            {
                descParams->coreDescMem[descSet].inDataDesc[descCnt] =
                    tempPtr;
                tempPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
            }
            dcDescInfo->rldDescAddr[descSet][DC_IN_DATA_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;


            /* set the descriptor address and size for the coeff
             * extra for the reload descriptor
             */

            dcDescInfo->descAddr[descSet][DC_NSHADOW_IDX] = tempPtr;
            dcDescInfo->descSize[descSet][DC_NSHADOW_IDX] =
                        1u * VPSHAL_VPDMA_CONFIG_DESC_SIZE +
                        1u * VPSHAL_VPDMA_CTRL_DESC_SIZE;


            /*assign the overlay memory to core*/
            descParams->coreDescMem[descSet].coeffOvlyMem =
                  tempCfgPtr + descSet * VPSDDRV_GRPX_ONE_CONFIG_MEM_SIZE;

            /*create the config descriptor*/
            /* Function to create configuration descriptor in the memory */
            VpsHal_vpdmaCreateConfigDesc(
                tempPtr,
                descParams->configDest,
                VPSHAL_VPDMA_CPT_BLOCK_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                descParams->configMemSize,
                descParams->coreDescMem[descSet].coeffOvlyMem,
                (Ptr)descParams->confgiMemOffset,
                descParams->configMemSize);

            descParams->configDesc[descSet] = tempPtr;

            tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            dcDescInfo->rldDescAddr[descSet][DC_NSHADOW_IDX] = tempPtr;

            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;


            /*set the descriptor address and size for the multiple region case
             * extra for the reload descriptor
             */
            if (descInfo.numMultiWinDataDesc > 0u)
            {
                dcDescInfo->descAddr[descSet][DC_MOSAIC_DATA_IDX] = tempPtr;
                dcDescInfo->descSize[descSet][DC_MOSAIC_DATA_IDX] =
                    descInfo.numMultiWinDataDesc * VPSHAL_VPDMA_DATA_DESC_SIZE
                    + VPSHAL_VPDMA_CTRL_DESC_SIZE;

                for (descCnt = 0u;
                     descCnt < descInfo.numMultiWinDataDesc;
                     descCnt++)
                {
                    descParams->coreDescMem[descSet].multiWinDataDesc[descCnt] =
                                    tempPtr;
                    tempPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
                }
                dcDescInfo->rldDescAddr[descSet][DC_MOSAIC_DATA_IDX] = tempPtr;
                tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }
            else
            {
                dcDescInfo->descAddr[descSet][DC_MOSAIC_DATA_IDX] = NULL;
                dcDescInfo->descSize[descSet][DC_MOSAIC_DATA_IDX] = 0u;
                dcDescInfo->rldDescAddr[descSet][DC_MOSAIC_DATA_IDX] = NULL;
            }

            /* Program the descriptor memories using core */
            retVal = instObj->coreOps->programDesc(
                         instObj->coreHandle,
                         0u,
                         0u,
                         &descParams->coreDescMem[descSet]);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Core program descriptor memory failed\n");
                instObj->clutSet[descSet] = FALSE;
                break;
            }


        }

    }

    if (FVID2_SOK != retVal)
    {
        tempRetVal = vpsDdrvGrpxFreeDescMem(instObj, descParams);
        GT_assert(VpsDdrvGrpxTrace, (FVID2_SOK == tempRetVal));
    }
    return (retVal);
}

/**
 *  vpsDdrvGrpxFreeDescMem
 *  Frees the allocated core memory and resets the core descriptor pointers.
 */

Int32 vpsDdrvGrpxFreeDescMem(VpsDdrv_GrpxInstObj *instObj,
                             VpsDdrv_GrpxDescParams  *descParams)
{
    Int32              regCnt;

     /* Check for NULL pointers */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    GT_assert(VpsDdrvGrpxTrace, (NULL != descParams));

    VpsUtils_memset(
        descParams->coreDescMem,
        0u,
        (sizeof(Vcore_DescMem) * DC_NUM_DESC_SET));
    VpsUtils_memset(&descParams->dcDescInfo, 0u, sizeof(Dc_DescInfo));
    VpsUtils_memset(&descParams->abortDesc, 0u, sizeof(descParams->abortDesc));
    if (NULL != descParams->descMem)
    {
        VpsUtils_freeDescMem(descParams->descMem, descParams->descMemSize);
        descParams->descMem = NULL;
        descParams->descMemSize = 0u;
    }
    /*remove frame configuration memory*/
    if (NULL != descParams->configMem)
    {
        VpsUtils_freeDescMem(
            descParams->configMem,
            VPSDDRV_GRPX_CONFIG_MEM_SIZE);
        descParams->configMem = NULL;
    }
    /*remove coeff config memory of rest regions*/
    for (regCnt = 0u; regCnt < VPSDDRV_GRPX_MAX_REGIONS; regCnt++)
    {
        if (NULL != descParams->scMem[regCnt])
        {
            VpsUtils_freeDescMem(
                descParams->scMem[regCnt],
                VPSDDRV_GRPX_SC_MEM_SIZE);
            descParams->scMem[regCnt] = NULL;
        }
    }

    return (FVID2_SOK);

}

/**
 *  vpsDdrvGrpxConfigDctrl
 *  \brief Configures display controller and programs the register overlay.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */

Int32 vpsDdrvGrpxConfigDctrl(VpsDdrv_GrpxInstObj *instObj)
{
    Int32                    retVal = FVID2_SOK;
    Int32                    tempRetVal;
    UInt32                   descSet;
    UInt32                   descAllocFlag = FALSE;
    VpsDdrv_GrpxDescParams   *descParams = NULL;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj->coreOps->programReg));

    /* Set the input frame size to the display controller , this is just a
     * dummy call to make consistant with the rest display driver, no
     * real actions are taked by the  display controller
     */
    retVal = Dc_setInputFrameSize(instObj->dcHandle, &instObj->dcRtConfig);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "ERROR: DC set frame size failed.\n");
    }

    if (FVID2_SOK == retVal)
    {
        descParams = &instObj->descParams;
        retVal = vpsDdrvGrpxAllocDescMem(instObj, descParams);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "Alloc core memory failed\n");
        }
    }
    /*set the descriptor memory to display controller*/
    if (FVID2_SOK == retVal)
    {
        retVal = Dc_setDescMem(instObj->dcHandle, &descParams->dcDescInfo);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "Set DCTRL descriptor memory failed\n");
        }
        else
        {
            descAllocFlag = TRUE;
        }
    }


    if (FVID2_SOK == retVal)
    {
        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet++)
        {
            /* Program the descriptor memories and program the registers
             * directly in register using core */
            retVal = instObj->coreOps->programReg(
                         instObj->coreHandle,
                         0u,
                         &descParams->coreDescMem[descSet]);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                    "Core program register failed\n");
                break;
            }
            instObj->configSet[descSet] = TRUE;
        }
    }

    /* Undo if error occurs */
    if (FVID2_SOK != retVal)
    {
        if (NULL != descParams)
        {
            /* Free the descriptor memory */
            tempRetVal = vpsDdrvGrpxFreeDescMem(instObj, descParams);
            GT_assert(VpsDdrvGrpxTrace, (FVID2_SOK == tempRetVal));
        }

        if (TRUE == descAllocFlag)
        {
            /* Clear descriptor memory from DCTRL */
            tempRetVal = Dc_clrDescMem(instObj->dcHandle);
            GT_assert(VpsDdrvGrpxTrace, (FVID2_SOK == tempRetVal));
        }
    }

    return (retVal);
}

/**
 *  vpsDdrvGrpxDeConfigDctrl
 *  \brief Clear the descriptor memory from display controller.
 *  Returns FVID2_SOK on success else returns appropriate error code.
 */

Int32 vpsDdrvGrpxDeConfigDctrl(VpsDdrv_GrpxInstObj *instObj)
{

    Int32                      retVal = FVID2_SOK;
    VpsDdrv_GrpxDescParams     *descParams;

    /* Check for NULL pointers */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));

    /* Clear descriptor memory from DCTRL */
    retVal = Dc_clrDescMem(instObj->dcHandle);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "Clear DCTRL descriptor memory failed\n");
    }
    descParams = &instObj->descParams;
    retVal = vpsDdrvGrpxFreeDescMem(instObj, descParams);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
            "Free descriptor memory failed\n");
    }

    return (retVal);

}

/**
 *  vpsDdrvGrpxCanReCreateConfigDesc
 *  \brief whether it is able to update the coefficient and create the
 *  configuration descriptor
 */
UInt32 vpsDdrvGrpxCanReCreateConfigDesc(
                                    const VpsDdrv_GrpxInstObj *instObj,
                                    FVID2_FrameList *frameList)
{
    UInt32                   flag = FALSE;
    Vps_GrpxRtList           *rtList;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));

    /* only create if it is not duplicate buffer */
    if (FALSE == vpsDdrvGrpxIsDuplicateQ(instObj))
    {
        if (NULL != frameList->perListCfg)
        {
             rtList = (Vps_GrpxRtList *)frameList->perListCfg;
             if (NULL != rtList->scParams)
             {
                flag = TRUE;
             }
        }
    }
    /* */
    return (flag);
}

/**
 *  vpsDdrvGrpxCheckFormat
 *  \brief Checks the format provided by the application.
 *  Returns FVID2_SOK on success else returns error value.
 */
Int32 vpsDdrvGrpxCheckFormat(const VpsDdrv_GrpxInstObj *instObj,
                                    const FVID2_Format *fmt)
{
    Int32                    retVal = FVID2_SOK;

    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != instObj));
    GT_assert(VpsDdrvGrpxTrace, (NULL != fmt));

    /* Check if the dimensions are matching with that of the VENC */
    if (fmt->width > instObj->dcPathInfo.modeInfo.mInfo.width)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "INVALID_PARAMS: Width greater than destination width\n");
        retVal = FVID2_EINVALID_PARAMS;
    }
    if (fmt->height > instObj->dcPathInfo.modeInfo.mInfo.height)
    {
        GT_0trace(VpsDdrvGrpxTrace, GT_ERR,
                "INVALID_PARAMS: Height greater than destination height\n");
        retVal = FVID2_EINVALID_PARAMS;
    }

    return (retVal);
}

/**
 *  vpsDdrvGrpxIsDuplicateQ
 *  \brief check whether the Q is duplicate or not
 *   return TRUE if it is duplicate Q
 */
UInt32 vpsDdrvGrpxIsDuplicateQ(const VpsDdrv_GrpxInstObj *instObj)
{
    const VpsDdrv_GrpxBufManObj *bmObj = &instObj->bmObj;
    /* pointer sanity check */
    GT_assert(VpsDdrvGrpxTrace, (NULL != bmObj));

    /*frame buffer mode not duplicate Q*/
    if (TRUE == instObj->state.isFbMode)
    {
        return FALSE;
    }

    if (TRUE ==bmObj->isProgressive)
    {
        if (bmObj->prgmedBuf[0] == bmObj->prgmedBuf[1])
        {
            return TRUE;
        }
    }
    else
    {
        if(bmObj->prgmedBuf[0] == bmObj->sPrgmedBuf)
        {
            return TRUE;
        }
    }

    return FALSE;
}

/**
 *  vpsDdrvGrpxGetBppFromFmt
 *  This function is to get the real bits per pixel from the format
 */
Int32  vpsDdrvGrpxGetBppFromFmt(UInt32 dataFormat, UInt32 *bpp)
{

    int retVal = FVID2_SOK;
    GT_assert(VpsDdrvGrpxTrace, (NULL != bpp));
    switch (dataFormat)
    {
        case FVID2_DF_RGB16_565:
        case FVID2_DF_ARGB16_1555:
        case FVID2_DF_RGBA16_5551:
        case FVID2_DF_ARGB16_4444:
        case FVID2_DF_RGBA16_4444:
            *bpp = 16;
            break;
        case FVID2_DF_ARGB24_6666:
        case FVID2_DF_RGBA24_6666:
        case FVID2_DF_RGB24_888:
            *bpp = 24;
            break;
        case FVID2_DF_ARGB32_8888:
        case FVID2_DF_RGBA32_8888:
            *bpp = 32;
            break;
        case FVID2_DF_BITMAP8:
            *bpp = 8;
            break;
        case FVID2_DF_BITMAP4_LOWER:
        case FVID2_DF_BITMAP4_UPPER:
            *bpp = 4;
            break;
        case FVID2_DF_BITMAP2_OFFSET0:
        case FVID2_DF_BITMAP2_OFFSET1:
        case FVID2_DF_BITMAP2_OFFSET2:
        case FVID2_DF_BITMAP2_OFFSET3:
            *bpp = 2;
            break;
        case FVID2_DF_BITMAP1_OFFSET0:
        case FVID2_DF_BITMAP1_OFFSET1:
        case FVID2_DF_BITMAP1_OFFSET2:
        case FVID2_DF_BITMAP1_OFFSET3:
        case FVID2_DF_BITMAP1_OFFSET4:
        case FVID2_DF_BITMAP1_OFFSET5:
        case FVID2_DF_BITMAP1_OFFSET6:
        case FVID2_DF_BITMAP1_OFFSET7:
            *bpp = 1;
            break;
        default:
            retVal = FVID2_EINVALID_PARAMS;
            break;

    }

    return retVal;
}

