/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vps_dctrl.c
 *
 * \brief Display Controller source file for driver interfaces
 *  This file implements APIs, which will be used by the display driver
 *  to get/set the information.
 *  Display controller supports two kind of interfaces, one for the
 *  application and other for the driver. Driver interface is mainly
 *  used to get the mode information from the venc to which a path is
 *  connected and also to set the runtime parameters in the CIG, VCOMP
 *  and Blender.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vps.h>

#include <ti/psp/vps/hal/vpshal_cig.h>
#include <ti/psp/vps/hal/vpshal_vcomp.h>

#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/drivers/display/vps_dlm.h>
#include <ti/psp/vps/drivers/display/vps_dctrlInt.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlPriv.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlTraversePriv.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlHalPriv.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/platforms/vps_platform.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                     LOCAL FUNCTION DECLARATION                             */
/* ========================================================================== */

static Int32 dcEnableInputPaths(
                Dc_InputNodeInfo            *inputNode,
                UInt32                      *endNodes,
                UInt32                      *numEndNodes,
                UInt32                       isEnable);
static Int32 dcCheckForTiedVencs(
                Dc_DisplayCtrlInfo          *dcCtrlInfo,
                UInt32                      *endNodes,
                UInt32                       numEndNodes);
static Int32 dcGetResources(
                UInt8                       *listNum,
                UInt32                      *lmFidNum,
                VpsHal_VpdmaChannel         *freeClientNum,
                UInt32                      *isListAllocated,
                UInt32                      *endNodes,
                UInt32                       numEndNodes);
static Int32 dcGetFreeClient(VpsHal_VpdmaChannel *freeClientNum);
static Int32 dcReleaseFreeClient(VpsHal_VpdmaChannel freeClientNum);
static Int32 dcCreateConfigOverlay(Dc_DisplayCtrlInfo *dcCtrlInfo);
static Int32 dcCreateRtConfigOverlay(Dc_DisplayCtrlInfo *dctrlInfo);
static Int32 dcDlmClientCallBack(UInt32 curSet, UInt32 timeStamp, Ptr arg);


/* ========================================================================== */
/*                        Global Variables                                    */
/* ========================================================================== */

/** \brief Overlay memory for the shadow configuration */
static UInt8 *gDcShOvlyMem;

/** \brief Overlay memory for the non-shadow configuration */
static UInt8 *gDcNonShOvlyMem;

/* Descriptor memory used for configuring shadow/non shadow
   registers through display controller. This is only for the
   display controller client. */
static UInt8 *gDcDescMem;

/* Memory pool used for runtime configuration. This is used for VCOMP main
 * and Aux path and CIG pip window */
static UInt8 *gDcRtCfgOvlyMem;

/* Descriptor memory used for configuring runtime
   configuration for VCOMP/CIG. */
static UInt8 *gDcRtCfgDescMem;

VpsHal_VpdmaChannel DcFreeClientNum[] = DC_FREE_CLIENT_NUM_DEFAULTS;
UInt32 DcFreeClientFlag[DC_MAX_OUTPUT] = {FALSE};

/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */


Int32 dcIntInit(Dc_DisplayCtrlInfo *dcCtrlInfo)
{
    Int32 retVal = FVID2_SOK;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));
    gDcShOvlyMem = NULL;
    gDcNonShOvlyMem = NULL;
    gDcDescMem = NULL;
    gDcRtCfgOvlyMem = NULL;
    gDcRtCfgDescMem = NULL;

    gDcShOvlyMem = VpsUtils_allocDescMem(
                       DC_SHADOW_REG_OVLY_SIZE,
                       VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gDcNonShOvlyMem = VpsUtils_allocDescMem(
                          DC_NONSHADOW_REG_OVLY_SIZE,
                          VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gDcDescMem = VpsUtils_allocDescMem(
                     DC_DESC_MEM_SIZE,
                     VPSHAL_VPDMA_DESC_BYTE_ALIGN);
    gDcRtCfgOvlyMem = VpsUtils_allocDescMem(
                          DC_RUNTIME_CONFIG_OVLY_SIZE,
                          VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
    gDcRtCfgDescMem = VpsUtils_allocDescMem(
                          DC_RT_DESC_MEM_SIZE,
                          VPSHAL_VPDMA_DESC_BYTE_ALIGN);
    if ((NULL == gDcShOvlyMem) ||
        (NULL == gDcNonShOvlyMem) ||
        (NULL == gDcDescMem) ||
        (NULL == gDcRtCfgOvlyMem)||
        (NULL == gDcRtCfgDescMem))
    {
        GT_0trace(DcTrace, GT_ERR, "Could not allocate descrioptor!\n");
        retVal = FVID2_EALLOC;
    }

    if (FVID2_SOK == retVal)
    {
        /* Initializing Overlay Memories to zero */
        VpsUtils_memset(gDcShOvlyMem, 0x0, DC_SHADOW_REG_OVLY_SIZE);
        VpsUtils_memset(gDcNonShOvlyMem, 0x0, DC_NONSHADOW_REG_OVLY_SIZE);
        VpsUtils_memset(gDcRtCfgOvlyMem, 0x0, DC_RUNTIME_CONFIG_OVLY_SIZE);

        /* Initialize Runtime configuration overlay */
        retVal = dcCreateConfigOverlay(dcCtrlInfo);
        retVal |= dcCreateRtConfigOverlay(dcCtrlInfo);
    }

    if (FVID2_SOK != retVal)
    {
        /* Free the descriptor memories in case of error */
        if (NULL != gDcShOvlyMem)
        {
            VpsUtils_freeDescMem(gDcShOvlyMem, DC_SHADOW_REG_OVLY_SIZE);
            gDcShOvlyMem = NULL;
        }
        if (NULL != gDcNonShOvlyMem)
        {
            VpsUtils_freeDescMem(gDcNonShOvlyMem, DC_NONSHADOW_REG_OVLY_SIZE);
            gDcNonShOvlyMem = NULL;
        }
        if (NULL != gDcDescMem)
        {
            VpsUtils_freeDescMem(gDcDescMem, DC_DESC_MEM_SIZE);
            gDcDescMem = NULL;
        }
        if (NULL != gDcRtCfgOvlyMem)
        {
            VpsUtils_freeDescMem(gDcRtCfgOvlyMem, DC_RUNTIME_CONFIG_OVLY_SIZE);
            gDcRtCfgOvlyMem = NULL;
        }
        if (NULL != gDcRtCfgDescMem)
        {
            VpsUtils_freeDescMem(gDcRtCfgDescMem, DC_RT_DESC_MEM_SIZE);
            gDcRtCfgDescMem = NULL;
        }
    }

    return (retVal);
}



Int32 dcIntDeInit(Dc_DisplayCtrlInfo *dcCtrlInfo)
{
    UInt32 cnt;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));
    for (cnt = 0u; cnt < DC_RT_CONFIG_NUM_IDX; cnt ++)
    {
        if (NULL != dcCtrlInfo->rtDlmHandle[cnt])
        {
            /* Remove Descriptors from Dlm */
            VpsDlm_clrDescMem(dcCtrlInfo->rtDlmHandle[cnt]);
            /* UnRegister DLM Runtime clients */
            VpsDlm_unRegister(dcCtrlInfo->rtDlmHandle[cnt]);
        }
    }

    if (NULL != dcCtrlInfo->dlmClientCtrlHandle)
    {
        /* Unregister from DLM */
        VpsDlm_unRegister(dcCtrlInfo->dlmClientCtrlHandle);
    }

    /* Free the descriptor memories */
    if (NULL != gDcShOvlyMem)
    {
        VpsUtils_freeDescMem(gDcShOvlyMem, DC_SHADOW_REG_OVLY_SIZE);
        gDcShOvlyMem = NULL;
    }
    if (NULL != gDcNonShOvlyMem)
    {
        VpsUtils_freeDescMem(gDcNonShOvlyMem, DC_NONSHADOW_REG_OVLY_SIZE);
        gDcNonShOvlyMem = NULL;
    }
    if (NULL != gDcDescMem)
    {
        VpsUtils_freeDescMem(gDcDescMem, DC_DESC_MEM_SIZE);
        gDcDescMem = NULL;
    }
    if (NULL != gDcRtCfgOvlyMem)
    {
        VpsUtils_freeDescMem(gDcRtCfgOvlyMem, DC_RUNTIME_CONFIG_OVLY_SIZE);
        gDcRtCfgOvlyMem = NULL;
    }
    if (NULL != gDcRtCfgDescMem)
    {
        VpsUtils_freeDescMem(gDcRtCfgDescMem, DC_RT_DESC_MEM_SIZE);
        gDcRtCfgDescMem = NULL;
    }

    return (FVID2_SOK);
}



/** \brief Function to register display driver to the display controller.
 *  Display driver registers to the display controller, which in turn
 *  registers display driver to the DLM. All display driver must register
 *  to the display controller on the specific input node, which identifies
 *  the input path.
 */
DcClientHandle Dc_registerClient(Dc_NodeNum         nodeNum,
                                 Dc_ClientInfo     *clientInfo,
                                 Ptr                arg)
{
    UInt32                   cnt;
    Dc_NodeInfo             *node = NULL;
    Dc_InputNodeInfo        *inputNode = NULL;
    VpsDlm_Handle            dlmHandle = NULL;
    DcClientHandle           clientHandle = NULL;
    Dc_DisplayCtrlInfo      *dcCtrlInfo = NULL;

    /** Steps
     * 1, Get the global object of display controller.
     * 2, get the global samaphore i.e. No other API can be called when
     *    this function executes.
     * 3, Error checking.
     *    3.1, Check the validity of the input node number
     *    3.2, Check to see if anyone registered to this node
     *    3.3, clientInfo pointer should not be null.
     * 4, Register input display path to the DLM
     * 5, Release global semaphore
     */
    dcCtrlInfo = dcGetDctrlObj();
    GT_assert(DcTrace, (NULL != dcCtrlInfo));

    Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);

    /* Find a input node with the given node number */
    for (cnt = 0u; cnt < DC_MAX_INPUT; cnt ++)
    {
        /* Get the node info structure from the input node */
        inputNode = dcGetInputNode(cnt);

        GT_assert(DcTrace, (NULL != inputNode));

        node = inputNode->node;

        GT_assert(DcTrace, (NULL != node));

        if (node->nodeNum == nodeNum)
        {
            /* Use this input node */
            break;
        }
    }

    /* If the input node is found and client is unregistered, register
     * the client with DLM and store its information in the input node */
    if ((cnt < DC_MAX_INPUT) && (FALSE == inputNode->isRegistered) &&
        (NULL != clientInfo))
    {
        dlmHandle = VpsDlm_register(
                        inputNode->dlmClientType,
                        clientInfo->isFbMode,
                        (VpsDlm_ClientCbFxn)clientInfo->cbFxn,
                        clientInfo->arg);
        if (NULL != dlmHandle)
        {
            /* Store client info pointer in the input node info structure */
            inputNode->clientInfo = clientInfo;

            /* Store the DLM Handle in the inputnode */
            inputNode->dlmHandle = dlmHandle;
            inputNode->isRegistered = TRUE;
            clientHandle = (DcClientHandle)inputNode;
        }
        else
        {
            GT_1trace(DcTrace,
                      GT_CRIT,
                      "%s : Dlm Registration Failed",
                      __FUNCTION__);
        }
    }
    Semaphore_post(dcCtrlInfo->sem);

    return (clientHandle);
}


Int32 Dc_unRegisterClient(DcClientHandle handle)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InputNodeInfo   *inputNode = (Dc_InputNodeInfo *)handle;

    /** Steps
     * 1, Check for errors
     *    1.1 input Node should not be null
     *    1.2 input node should not be started
     *    1.3 input node should be registered
     *    1.4 its dcCtrlInfo should not be null
     * 2, get the global samaphore i.e. No other API can be called when
     *    this function executes.
     * 3, Un-Register input display path to the DLM
     * 4, Cleanup the data structure
     * 5, Release global semaphore
     */
    if ((NULL != inputNode) &&
        (FALSE == inputNode->isStarted) &&
        (FALSE == inputNode->isSwitched) &&
        (TRUE == inputNode->isRegistered) &&
        (NULL != inputNode->dcCtrlInfo))
    {
        Semaphore_pend(inputNode->dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        /* Client is not registered and it is not running or switched */

        /* Unregister client from DLM */
        retVal = VpsDlm_unRegister(inputNode->dlmHandle);

        /* UnRegister client from Media Controller */
        if (FVID2_SOK == retVal)
        {
            /* Reset the client information structure */
            inputNode->isRegistered = FALSE;
            inputNode->clientInfo = NULL;
            inputNode->dlmHandle = NULL;
        }
        else
        {
            GT_1trace(DcTrace,
                      GT_CRIT,
                      "%s : Dlm Un-Registration Failed",
                      __FUNCTION__);
        }
        Semaphore_post(inputNode->dcCtrlInfo->sem);
    }

    return (retVal);
}

/* This function is just wrapper function over Dlm's API. */
Int32 Dc_setDescMem(DcClientHandle handle,
                    const Dc_DescInfo *descInfo)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InputNodeInfo   *inputNode = (Dc_InputNodeInfo *)handle;

    if ((NULL != inputNode) && (NULL != inputNode->dcCtrlInfo))
    {
        Semaphore_pend(inputNode->dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        retVal = VpsDlm_setDescMem(
                    inputNode->dlmHandle,
                    (VpsDlm_DescInfo *) descInfo);

        Semaphore_post(inputNode->dcCtrlInfo->sem);
    }

    return (retVal);
}

/* This function is just wrapper function over Dlm's API. */
Int32 Dc_clrDescMem(DcClientHandle handle)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InputNodeInfo   *inputNode = (Dc_InputNodeInfo *)handle;

    if ((NULL != inputNode) && (NULL != inputNode->dcCtrlInfo))
    {
        Semaphore_pend(inputNode->dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        retVal = VpsDlm_clrDescMem(inputNode->dlmHandle);

        Semaphore_post(inputNode->dcCtrlInfo->sem);
    }

    return (retVal);
}

Int32 Dc_setDispMode(DcClientHandle handle, UInt32 isFbMode)
{
    Int32               retVal = FVID2_SOK;
    Dc_InputNodeInfo   *inputNode = (Dc_InputNodeInfo *)handle;

    if ((NULL != inputNode) &&
        (TRUE == inputNode->isRegistered) &&
        (NULL != inputNode->dcCtrlInfo) &&
        (FALSE == inputNode->isStarted))
    {
        Semaphore_pend(inputNode->dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        VpsDlm_setDispMode(inputNode->dlmHandle, isFbMode);

        Semaphore_post(inputNode->dcCtrlInfo->sem);
    }

    return (retVal);
}

/* This function is just wrapper function over Dlm's API. */
Int32 Dc_updateDescMem(DcClientHandle handle,
                       UInt32 descSet,
                       const Dc_UpdateDescInfo *descInfo)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InputNodeInfo   *inputNode = (Dc_InputNodeInfo *)handle;

    if ((NULL != inputNode) && (NULL != inputNode->dcCtrlInfo))
    {
        retVal = VpsDlm_updateDescMem(
                     inputNode->dlmHandle,
                     descSet,
                     (VpsDlm_UpdateDescInfo *) descInfo);
    }

    return (retVal);
}

/* Function to get the information about the mode running on the VENC
 * where the given path is connected */
Int32 Dc_getPathInfo(DcClientHandle handle, Dc_PathInfo *pathInfo)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InputNodeInfo   *inputNode = (Dc_InputNodeInfo *)handle;
    UInt32              numNodes = 0u, outNodeCnt, inNodeCnt, endNode;
    UInt32              outputNodes[DC_MAX_OUTPUT];
    Dc_OutputNodeInfo  *outputNode = NULL;
    Dc_NodeInfo        *currNode = NULL, *prevNode = NULL, *parentNode = NULL;
    UInt32              isIntEnabled = FALSE;

    if ((NULL != inputNode) && (NULL != pathInfo) &&
        (NULL != inputNode->node) && (NULL != inputNode->dcCtrlInfo))
    {
        Semaphore_pend(inputNode->dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        VpsUtils_memset(pathInfo, 0u, sizeof(Dc_PathInfo));

        /* Traverse from input node to the VENC to get the Mode information */
        currNode = dcGetNodeInfo(inputNode->node->nodeNum);
        GT_assert(DcTrace, (NULL != currNode));

        /* Initialize Traverser with the current Node */
        dcInitTraverser(currNode);

        /* dcGetNextTraversedNode */
        currNode = dcGetNextTraversedNode(TRUE);

        while (NULL != currNode)
        {
			if (DC_NODE_VCOMP == currNode->nodeNum)
            {
                /* Since the lasttraversed node always returns the last, which
                 * does not have any child, the only way to know the parent of
                 * the current node is by peek of the stack. For VCOMP, peek of
                 * the stack will be either primary input path of vcomp pip mux.
                 * Depending on peek node, vcomp input path can be selected. */
                parentNode = dcStackPeakParent();
                GT_assert(DcTrace, (NULL != parentNode));

                if (DC_NODE_PRI_MUX == parentNode->nodeNum)
                {
                    pathInfo->isMainVcompAvail = TRUE;
                }
                else
                {
                    pathInfo->isAuxVcompAvail = TRUE;
                }
            }
            else if (DC_NODE_CIG_1 == currNode->nodeNum)
            {
                pathInfo->isPipCigAvail = TRUE;
            }

            /* If it is LastNode, store it into endNodes array */
            if (TRUE == dcIsLastNode(currNode, TRUE))
            {
                outputNodes[numNodes] = currNode->nodeNum;
                numNodes ++;
            }

            /* dcGetNextTraversedNode */
            currNode = dcGetNextTraversedNode(TRUE);
        }

        /* If the current input is not connected to any of the VENCs/output
         * node or enableInputPaths returns error, return error. */
        if (0u == numNodes)
        {
            GT_2trace(DcTrace,
                      GT_ERR,
                      "%s: No End Nodes %d",
                      __FUNCTION__,
                      numNodes);
            retVal = FVID2_EFAIL;
        }
        else
        {
            /* Get the mode information from the venc at the 0th index.
             * traverse function provides all tied vencs where this input
             * node is conneceted. Since all output vencs are tied and are
             * running the same mode, venc at the 0th index is used to get
             * mode information */
            endNode = outputNodes[0u];

            currNode = dcGetNodeInfo(endNode);
            GT_assert(DcTrace, (NULL != currNode));

            outputNode = (Dc_OutputNodeInfo *)(currNode->parent);

            /* Get the mode information from the venc */
            if (NULL != outputNode->vencHandle)
            {
                retVal = outputNode->getMode(
                            outputNode->vencHandle,
                            &(pathInfo->modeInfo));

            }
            else
            {
                GT_1trace(DcTrace,
                          GT_ERR,
                          "%s: Venc Handle is null",
                          __FUNCTION__);
                retVal = FVID2_EFAIL;
            }

            /* If interlacing is enabled in the CIG for any of the output node,
             * input frame format should be progressive. */
            /* For each of the output nodes, check if it progressive and its
             * input from CIG is in interlacing enabled*/
            for (outNodeCnt = 0u; (outNodeCnt < numNodes) &&
                                  (FVID2_SOK == retVal); outNodeCnt ++)
            {
                endNode = outputNodes[outNodeCnt];

                currNode = dcGetNodeInfo(endNode);
                GT_assert(DcTrace, (NULL != currNode));

                outputNode = (Dc_OutputNodeInfo *)(currNode->parent);

                retVal = outputNode->getMode(
                            outputNode->vencHandle,
                            &(pathInfo->modeInfo));

                /* There is some error returned from teh getmodeinfo API */
                if (FVID2_SOK != retVal)
                {
                    GT_1trace(DcTrace,
                              GT_ERR,
                              "%s: Error from getMode Venc",
                              __FUNCTION__);
                    break;
                }

                /* Get the node to the Blender. Since output nodes have
                 * only single input nodes, taking index zero to get the
                 * blender node */
                currNode = currNode->input.node[0u];
                endNode = currNode->input.numNodes;
                for (inNodeCnt = 0u; inNodeCnt < endNode; inNodeCnt ++)
                {
                    prevNode = currNode->input.node[inNodeCnt];
                    if ((TRUE == currNode->input.isEnabled[inNodeCnt]) &&
                        (NULL != prevNode->priv) &&
                        (DC_INTERLACING_ENABLED == (UInt32)prevNode->priv))
                    {
                        GT_2trace(DcTrace,
                                  GT_INFO,
                                  "%s: Interlacing is enabled on node %d",
                                  __FUNCTION__,
                                  prevNode->nodeNum);

                        isIntEnabled = TRUE;
                        break;
                    }
                }

                if (TRUE == isIntEnabled)
                {
                    if (FVID2_SF_PROGRESSIVE ==
                            pathInfo->modeInfo.mInfo.scanFormat)
                    {
                        GT_1trace(DcTrace,
                                  GT_INFO,
                                  "%s: Venc Format is progressive and \
                                  interlacing is enabled",
                                  __FUNCTION__);
                        /* Interlacing is enabled and output mode is in
                         * progressive format */
                        retVal = FVID2_EFAIL;
                    }
                    else
                    {
                        /* Interlacing is enabled and output is also interlaced,
                         * so input format should be progressive. */
                        pathInfo->modeInfo.mInfo.scanFormat =
                                FVID2_SF_PROGRESSIVE;
                        retVal = FVID2_SOK;
                        break;
                    }
                }
            }
        }
        Semaphore_post(inputNode->dcCtrlInfo->sem);
    }

    return (retVal);
}



/* \brief This function is used to set the frame size of the input path.
 * VCOMP takes two input paths main and aux. The register to enable these
 * two paths also has fields to configure the input video size, so overlay
 * used for enabling/disabling these paths should also configure appropriate
 * video size. This function configures input video size in the overlay if
 * it is required on the given input path.
 */
Int32 Dc_setInputFrameSize(DcClientHandle handle, Dc_RtConfig *rtConfig)
{
    Int32                       retVal = FVID2_EINVALID_PARAMS;
    UInt32                      cnt, descSet;
    Dc_InputNodeInfo           *inputNode = (Dc_InputNodeInfo *)handle;
    Dc_NodeInfo                *currNode = NULL, *parentNode = NULL;
    Dc_DisplayCtrlInfo         *dcCtrlInfo = NULL;
    UInt32                      isRtConfig[DC_RTCONFIGMODULE_MAX] = {FALSE,
                                                                     FALSE};

    if ((NULL != inputNode) && (NULL != rtConfig))
    {
        /* Mark return value as success as pointers are not null */
        retVal = FVID2_SOK;

        dcCtrlInfo = inputNode->dcCtrlInfo;
        GT_assert(DcTrace, (NULL != dcCtrlInfo));

        Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        /* Get the pointer node associated with this input node */
        currNode = inputNode->node;

        /* Initialize Traverser with the current Node */
        dcInitTraverser(currNode);

        /* dcGetNextTraversedNode */
        currNode = dcGetNextTraversedNode(TRUE);

        while (NULL != currNode)
        {
            if (DC_NODE_VCOMP == currNode->nodeNum)
            {
                GT_1trace(DcTrace, GT_INFO, "%s: VCOMP", __FUNCTION__);
                /* Since the lasttraversed node always returns the last, which
                 * does not have any child, the only way to know the parent of
                 * the current node is by peek of the stack. For VCOMP, peek of
                 * the stack will be either primary input path of vcomp pip mux.
                 * Depending on peek node, vcomp input path can be selected. */
                parentNode = dcStackPeakParent();
                GT_assert(DcTrace, (NULL != parentNode));
                if (DC_NODE_PRI_MUX == parentNode->nodeNum)
                {
                    GT_1trace(DcTrace,
                              GT_INFO,
                              "%s: VCOMP: PRIMARY",
                              __FUNCTION__);
                    /* Copy Dlm handle, overlay pointer and overlay
                     * size in the input node information structure */
                    inputNode->rtDlmHandle[DC_RTCONFIGMODULE_VCOMP] =
                            dcCtrlInfo->rtDlmHandle[DC_RT_CONFIG_VCOMP_MAIN_IDX];
                    for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
                    {
                        inputNode->rtConfigOvlyPtr[DC_RTCONFIGMODULE_VCOMP]
                            [descSet] = dcCtrlInfo->ovlyPtr
                            [DC_RT_CONFIG_VCOMP_MAIN_IDX][descSet];
                    }
                    inputNode->rtConfigOvlySize[DC_RTCONFIGMODULE_VCOMP] =
                            dcCtrlInfo->ovlySize[DC_RT_CONFIG_VCOMP_MAIN_IDX];
                }
                else
                {
                    GT_1trace(DcTrace,
                              GT_INFO,
                              "%s: VCOMP: Auxialary",
                              __FUNCTION__);
                    /* Copy Dlm handle, overlay pointer and overlay
                     * size in the input node information structure */
                    inputNode->rtDlmHandle[DC_RTCONFIGMODULE_VCOMP] =
                            dcCtrlInfo->rtDlmHandle[DC_RT_CONFIG_VCOMP_AUX_IDX];
                    for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
                    {
                        inputNode->rtConfigOvlyPtr[DC_RTCONFIGMODULE_VCOMP]
                            [descSet] = dcCtrlInfo->ovlyPtr
                            [DC_RT_CONFIG_VCOMP_AUX_IDX][descSet];
                    }
                    inputNode->rtConfigOvlySize[DC_RTCONFIGMODULE_VCOMP] =
                            dcCtrlInfo->ovlySize[DC_RT_CONFIG_VCOMP_AUX_IDX];
                }

                /* Configure Runtime configuration in the appropriate module */
                for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
                {
                    GT_2trace(DcTrace,
                              GT_INFO,
                              "%s: setRtConfig: Vcomp: descSet %d",
                              __FUNCTION__,
                              descSet);
                    retVal = Dc_setRtConfig(handle,
                                            descSet,
                                            DC_RTCONFIGMODULE_VCOMP,
                                            rtConfig);
                }
                isRtConfig[DC_RTCONFIGMODULE_VCOMP] = TRUE;
            }
            if (DC_NODE_CIG_1 == currNode->nodeNum)
            {
                GT_1trace(DcTrace,
                          GT_INFO,
                          "%s: CIG ",
                          __FUNCTION__);
                /* Assign overlay pointers and DLM handles in the input node
                 * for CIG rt configuration */
                inputNode->rtDlmHandle[DC_RTCONFIGMODULE_CIG] =
                            dcCtrlInfo->rtDlmHandle[DC_RT_CONFIG_CIG_PIP_IDX];
                for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
                {
                    inputNode->rtConfigOvlyPtr[DC_RTCONFIGMODULE_CIG][descSet] =
                         dcCtrlInfo->ovlyPtr[DC_RT_CONFIG_CIG_PIP_IDX][descSet];
                }
                inputNode->rtConfigOvlySize[DC_RTCONFIGMODULE_CIG] =
                            dcCtrlInfo->ovlySize[DC_RT_CONFIG_CIG_PIP_IDX];

                /* Configure Runtime configuration in the appropriate module */
                for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
                {
                    GT_2trace(DcTrace,
                              GT_INFO,
                              "%s: setRtConfig: Set: descSet %d",
                              __FUNCTION__,
                              descSet);
                    retVal = Dc_setRtConfig(handle,
                                            descSet,
                                            DC_RTCONFIGMODULE_CIG,
                                            rtConfig);
                }
                isRtConfig[DC_RTCONFIGMODULE_CIG] = TRUE;
            }

            /* dcGetNextTraversedNode */
            currNode = dcGetNextTraversedNode(TRUE);
        }

        /* Initialize rt config members to null */
        for (cnt = 0u; cnt < DC_RTCONFIGMODULE_MAX; cnt ++)
        {
            if (FALSE == isRtConfig[cnt])
            {
                GT_2trace(DcTrace,
                          GT_INFO,
                          "%s: Setting handle to NULL %d",
                          __FUNCTION__,
                          cnt);

                inputNode->rtDlmHandle[cnt] = NULL;
                for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
                {
                    inputNode->rtConfigOvlyPtr[cnt][descSet] = NULL;
                }
                inputNode->rtConfigOvlySize[cnt] = 0u;
            }
        }
        Semaphore_post(inputNode->dcCtrlInfo->sem);
    }
    return (retVal);
}



Int32 Dc_setRtConfig(DcClientHandle     handle,
                     UInt32             descSet,
                     Dc_RtConfigModule  rtConfigModule,
                     Dc_RtConfig       *rtConfig)
{
    Int32                       retVal = FVID2_EINVALID_PARAMS;
    Dc_InputNodeInfo           *inputNode = (Dc_InputNodeInfo *)handle;
    Dc_InitParams              *initParams = NULL;
    VpsHal_VcompInputVideo      inputVcompPath;
    VpsHal_VcompInputPathConfig vcompInputConfig;
    VpsHal_CigRtConfig          cigPipConfig;
    Dc_DisplayCtrlInfo         *dcCtrlInfo = NULL;
    VpsHal_Handle               halHandle;
    UInt32                      shift;

    if ((NULL != inputNode) && (descSet < DC_NUM_DESC_SET) &&
        (NULL != rtConfig))
    {
        dcCtrlInfo = inputNode->dcCtrlInfo;
        GT_assert(DcTrace, (NULL != dcCtrlInfo));

        initParams = &(dcCtrlInfo->initParams);

        if (DC_RTCONFIGMODULE_VCOMP == rtConfigModule)
        {
            halHandle = initParams->halHandle[VDC_VCOMP_IDX];
        }
        else if (DC_RTCONFIGMODULE_CIG == rtConfigModule)
        {
            halHandle = initParams->halHandle[VDC_CIG_IDX];
        }
        else
        {
            halHandle = NULL;
        }

        if (FVID2_SF_PROGRESSIVE == rtConfig->scanFormat)
        {
            /* No need to divide vertical configurations */
            shift = 0u;
        }
        else
        {
            /* divide vertical configurations by 2 */
            shift = 1u;
        }

        /* If RT config is for VCOMP and Dlm handle is not null,
         * configuration in rtconfig structure can be configured in
         * vcomp overlay */
        if ((DC_RTCONFIGMODULE_VCOMP == rtConfigModule) &&
            (NULL != inputNode->rtDlmHandle[DC_RTCONFIGMODULE_VCOMP]) &&
            (NULL != halHandle))
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: VCOMP",
                      __FUNCTION__);

            /* For configuring input video size in VCOMP, we should know
             * which is this input in the VCOMP. The input node of the
             * VCOMP can be either HQ input path, vcomp_mux. Depending on
             * whether input node is hq input path node, input path for VCOMP
             * is selected here. */
            inputVcompPath = (VpsHal_VcompInputVideo)inputNode->node->priv;
            if (VPSHAL_VCOMP_SOURCE_MAIN != inputVcompPath)
            {
                inputVcompPath = VPSHAL_VCOMP_SOURCE_AUX;
            }

            /* Configure other cropping parameters */
            retVal = VpsHal_vcompGetInputVideoConfig(
                        halHandle,
                        inputVcompPath,
                        &vcompInputConfig);

            /* Copy the parameters from the RTConfig structure to the VCOMP
             * specific structure */
            /* TODO: How to avoid this copy operation */
            vcompInputConfig.enabled = TRUE;
            vcompInputConfig.enableFixedData = FALSE;
            vcompInputConfig.numLines = rtConfig->inputHeight >> shift;
            vcompInputConfig.numPix = rtConfig->inputWidth;
            vcompInputConfig.clipWindow.skipNumPix = rtConfig->cropStartX;
            vcompInputConfig.clipWindow.useNumPix = rtConfig->cropWidth;
            vcompInputConfig.clipWindow.skipNumLines =
                rtConfig->cropStartY >> shift;
            vcompInputConfig.clipWindow.useNumLines =
                rtConfig->cropHeight >> shift;
            vcompInputConfig.dsplyPos.xOrigin = rtConfig->outCropStartX;
            vcompInputConfig.dsplyPos.yOrigin =
                rtConfig->outCropStartY >> shift;

            /* Assumption here is that output is always on so for start up
             * handling, input and output both are on */
            vcompInputConfig.startUpHandling =
                                    VPSHAL_VCOMP_START_UP_VIDEO_IN_OUT;

            /* Set the configuration in the overlay memory */
            retVal = VpsHal_vcompSetInputVideoConfig(
                        halHandle,
                        inputVcompPath,
                        &vcompInputConfig,
                inputNode->rtConfigOvlyPtr[DC_RTCONFIGMODULE_VCOMP][descSet]);

        }
        else if ((DC_RTCONFIGMODULE_CIG == rtConfigModule) &&
                 (NULL != inputNode->rtDlmHandle[DC_RTCONFIGMODULE_CIG]) &&
                 (NULL != halHandle))
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: CIG",
                      __FUNCTION__);

            /* TODO: How to avoid this copy operation */
            cigPipConfig.xPos = rtConfig->outCropStartX;
            cigPipConfig.yPos = rtConfig->outCropStartY;
            cigPipConfig.pipWidth = rtConfig->inputWidth;
            cigPipConfig.pipHeight = rtConfig->inputHeight >> shift;
            /* Get the default configuration from CIG */
            retVal = VpsHal_cigSetRtConfig(
                   halHandle,
                   &cigPipConfig,
                   inputNode->rtConfigOvlyPtr[DC_RTCONFIGMODULE_CIG][descSet]);
        }
        else /* Either handle is null or incorrect value of rtConfigModule */
        {
            retVal = FVID2_EINVALID_PARAMS;
        }
    }
    return (retVal);
}



Int32 Dc_startClient(DcClientHandle handle)
{
    Int32              retVal = FVID2_EINVALID_PARAMS;
    UInt32             endNodes[DC_MAX_OUTPUT], numEndNodes;
    UInt32             cnt, lmFidNum = VPSHAL_VPDMA_MAX_LM_FID;
    VpsHal_VpdmaChannel freeClientNum = VPSHAL_VPDMA_CHANNEL_NUM_CHANNELS;
    Dc_NodeInfo       *node = NULL;
    Dc_InputNodeInfo  *inputNode = (Dc_InputNodeInfo *)handle;
    UInt8              listNum = VPSHAL_VPDMA_MAX_LIST;
    UInt32             dlmNumStartClients = 0u;
    UInt32             isListAllocated = FALSE;
    Dc_DisplayCtrlInfo *dcCtrlInfo = NULL;
    VpsDlm_Request     dlmReq;
    Vps_PlatformId     platform;

    /* Node is not null, input is already registered and is not
     * already started */
    if ((NULL != inputNode) && (TRUE == inputNode->isRegistered) &&
        (TRUE != inputNode->isStarted) && (NULL != inputNode->node))
    {
        dcCtrlInfo = inputNode->dcCtrlInfo;
        GT_assert(DcTrace, (NULL != dcCtrlInfo));

        /* Get the semaphore */
        Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        inputNode->numDlmStartClient = 0;

        /* Enable inputs in Display Controller's overlay and also get
         * leaf nodes */
        retVal = dcEnableInputPaths(
                    inputNode,
                    endNodes,
                    &numEndNodes,
                    TRUE);

        /* If the current input is not connected to any of the VENCs/output
         * node or enableInputPaths returns error, return error. */
        if ((FVID2_SOK != retVal) || (0u == numEndNodes))
        {
            GT_3trace(DcTrace,
                      GT_ERR,
                      "%s: Error from getLeafNodes NumEnd Nodes %d retVal %d",
                      __FUNCTION__,
                      numEndNodes,
                      retVal);
            retVal = -1;
        }
        else
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: dcCheckForTiedVencs",
                      __FUNCTION__);

            /* If input path is connected to two vencs, they must be tied
             * together. If they are not tied, return error. */
            retVal = dcCheckForTiedVencs(dcCtrlInfo, endNodes, numEndNodes);
            if(FVID2_SOK == retVal)
            {
                GT_1trace(DcTrace,
                          GT_INFO,
                          "%s: dcGetResources",
                          __FUNCTION__);

                retVal = dcGetResources(
                            &listNum,
                            &lmFidNum,
                            &freeClientNum,
                            &isListAllocated,
                            endNodes,
                            numEndNodes);
            }
        }

        if (FVID2_SOK == retVal)
        {
            /* Call the starting callback function of the client passing
             * them exact framestart event so clients can set the frame
             * start event for all the VPDMA clients that this client is
             * handling */
            if ((NULL != inputNode->clientInfo) &&
                (NULL != inputNode->clientInfo->starting))
            {
                GT_1trace(DcTrace,
                          GT_INFO,
                          "%s: calling starting callback",
                          __FUNCTION__);

                retVal = inputNode->clientInfo->starting(
                                inputNode->clientInfo->arg,
                                (VpsHal_VpdmaFSEvent)(lmFidNum +
                                VPSHAL_VPDMA_FSEVENT_LM_FID0));
            }
        }

        if (FVID2_SOK == retVal)
        {
            platform = Vps_platformGetId();

            if ((platform <= VPS_PLATFORM_ID_UNKNOWN) ||
                (platform >= VPS_PLATFORM_ID_MAX))
            {
                GT_assert( GT_DEFAULT_MASK, FALSE);
            }

            if (VPS_PLATFORM_ID_EVM_TI8107 != platform)
            {
                node = dcGetNodeInfo(endNodes[0u]);
                GT_assert(DcTrace, (NULL != node));
            }
            else
            {
                node = dcGetNodeInfo(endNodes[0u]);
                GT_assert(DcTrace, (NULL != node));

                if (DC_NODE_HDCOMP_VENC == node->nodeNum)
                {
                    GT_assert(DcTrace, (2u == numEndNodes));

                    node = dcGetNodeInfo(endNodes[1u]);
                    GT_assert(DcTrace, (NULL != node));
                }
            }

            /* Set the frame start event as the venc fid change in the
             * write client */
            VpsHal_vpdmaSetFrameStartEvent(
                               freeClientNum,
                               (VpsHal_VpdmaFSEvent)(node->priv),
                               VPSHAL_VPDMA_LM_0,
                               0,
                               NULL,
                               0u);

            /* Display Controller's Overlay to configure paths needs to be added
             * in the DLM */
            dlmReq.startClients[dlmNumStartClients] = dcCtrlInfo->dlmClientCtrlHandle;
            dlmNumStartClients ++;
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: Display Controller DLM Handle",
                      __FUNCTION__);

            /* Client, which is actually starting to display, needs to be added
             * in the DLM */
            dlmReq.startClients[dlmNumStartClients] = inputNode->dlmHandle;
            dlmNumStartClients ++;
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: Client's DLM handle",
                      __FUNCTION__);

            /* If there is any module, which supports runtime configuration,
             * in the path, these runtime configuration clients should also be
             * added to the DLM */
            for (cnt = 0u; cnt < DC_RTCONFIGMODULE_MAX; cnt ++)
            {
                if (NULL != inputNode->rtDlmHandle[cnt])
                {
                    GT_1trace(DcTrace,
                              GT_INFO,
                              "%s: RT DLM Client",
                              __FUNCTION__);
                    dlmReq.startClients[dlmNumStartClients] =
                                        inputNode->rtDlmHandle[cnt];
                    dlmNumStartClients ++;
                    inputNode->numDlmStartClient ++;
                }
            }
            inputNode->numDlmStartClient ++;

            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: StartStopMultiple",
                      __FUNCTION__);

            /* Call DLM to start multiple clients at the same time on the given
             * list */
            dlmReq.numStart = dlmNumStartClients;
            dlmReq.numStop = 0u;
            dlmReq.listNum = listNum;
            dlmReq.lmfid = lmFidNum;
            dlmReq.socChNum = freeClientNum;
            retVal = VpsDlm_startStopClients(&dlmReq);
        }

        if (FVID2_SOK != retVal)
        {
            GT_1trace(DcTrace,
                      GT_ERR,
                      "%s: Disable all the inputs paths",
                      __FUNCTION__);

            VpsDlm_clrDescMem(dcCtrlInfo->dlmClientCtrlHandle);

            /* If list was allocated by calling resource manager, free that
             * list */
            if (TRUE == isListAllocated)
            {
                Vrm_releaseList(listNum);
                Vrm_releaseLmFid(lmFidNum);
                dcReleaseFreeClient(freeClientNum);
            }
        }
        else
        {
            /* Wait for the DLM Callback */
            Semaphore_pend(dcCtrlInfo->isrWaitSem, DC_SEM_TIMEOUT);

            VpsDlm_clrDescMem(dcCtrlInfo->dlmClientCtrlHandle);

            inputNode->isStarted = TRUE;

            /* Clear the VENC Overflow bit. If the list is allocated,
               it is the first client. */
            if (TRUE == isListAllocated)
            {
                /* Get the output node */
                node = dcGetNodeInfo(endNodes[0u]);
                GT_assert(DcTrace, (NULL != node));

                VpsHal_vpsClkcModuleClearUnderFlow(
                ((Dc_OutputNodeInfo *)(node->parent))->nodeId);
            }
        }
        /* Release the semaphore */
        Semaphore_post(dcCtrlInfo->sem);
    }

    return (retVal);
}



Int32 Dc_stopClient(DcClientHandle handle)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    UInt32              endNodes[DC_MAX_OUTPUT], numEndNodes = 0;
    Dc_InputNodeInfo   *inputNode = (Dc_InputNodeInfo *)handle;
    Dc_OutputNodeInfo  *outputNode = NULL;
    Dc_NodeInfo        *node = NULL;
    UInt8               listNum;
    UInt32              cnt, endNodeIdx, lmFidNum;
    VpsHal_VpdmaChannel freeClientNum;
    Dc_DisplayCtrlInfo *dcCtrlInfo = NULL;
    UInt32              dlmNumStopClients = 0, dlmNumStartClients = 0;
    VpsDlm_Request      dlmReq;

    /* Node is not null, input is already registered and is not
     * already started */
    if ((NULL != inputNode) && (TRUE == inputNode->isRegistered) &&
        (TRUE == inputNode->isStarted) && (NULL != inputNode->node))
    {
        dcCtrlInfo = inputNode->dcCtrlInfo;
        GT_assert(DcTrace, (NULL != dcCtrlInfo));

        /* Get the semaphore */
        Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);

        /* Disable inputs in Display Controller's overlay and also get
         * leaf nodes */
        retVal = dcEnableInputPaths(
                    inputNode,
                    endNodes,
                    &numEndNodes,
                    FALSE);
        /* If the current input is not connected to any of the VENCs/output
         * node or enableInputPaths returns error, return error. */
        if ((FVID2_SOK != retVal) || (0u == numEndNodes))
        {
            GT_3trace(DcTrace,
                      GT_ERR,
                      "%s: Error from getLeafNodes NumEnd Nodes %d retVal %d",
                      __FUNCTION__,
                      numEndNodes,
                      retVal);
            retVal = FVID2_EFAIL;
        }
        else
        {
            /* Atleast, node is connected to one Venc, so index 0 is used
             * to get the end node number. From the end node number, get the
             * parent node, which is outputnode info structure and from this
             * structure, get the list number */
            endNodeIdx = endNodes[0u];
            node = dcGetNodeInfo(endNodeIdx);
            GT_assert(DcTrace, (NULL != node));
            outputNode = (Dc_OutputNodeInfo *) node->parent;
            GT_assert(DcTrace, (NULL != outputNode));
            listNum = outputNode->listNum;
            lmFidNum = outputNode->lmFidNum;
            freeClientNum = outputNode->freeClientNum;

            /* Display Controller's client needs to be added in the DLM to disable
             * paths in VCOMP, CIG, BLEND and Mux modules*/
            dlmReq.startClients[dlmNumStartClients] =
                                dcCtrlInfo->dlmClientCtrlHandle;
            dlmNumStartClients ++;
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: DisplyController DLM hamdle",
                      __FUNCTION__);

            /* Given clients needs to be stopped to get displayed */
            dlmReq.stopClients[dlmNumStopClients] = inputNode->dlmHandle;
            dlmNumStopClients ++;
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: Client's DLM handle",
                      __FUNCTION__);

            /* If there is any module, which supports runtime configuration,
             * in the path, these runtime configuration clients should also be
             * removed to the DLM */
            for (cnt = 0u; cnt < DC_RTCONFIGMODULE_MAX; cnt ++)
            {
                if (NULL != inputNode->rtDlmHandle[cnt])
                {
                    GT_2trace(DcTrace,
                              GT_INFO,
                              "%s: RT DLM handle %d",
                              __FUNCTION__,
                              cnt);
                    /* Adding Real Time handles */
                    dlmReq.stopClients[dlmNumStopClients] = inputNode->rtDlmHandle[cnt];
                    dlmNumStopClients ++;
                }
            }

            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: StartStopMultiple",
                      __FUNCTION__);

            /* Start and stop multiple DLM clients at the same time */
            dlmReq.numStart = dlmNumStartClients;
            dlmReq.numStop = dlmNumStopClients;
            dlmReq.listNum = listNum;
            dlmReq.lmfid = lmFidNum;
            dlmReq.socChNum = freeClientNum;
            retVal = VpsDlm_startStopClients(&dlmReq);
        }

        if (FVID2_SOK == retVal)
        {
            /* Wait for the DLM Callback */
            Semaphore_pend(dcCtrlInfo->isrWaitSem, DC_SEM_TIMEOUT);

            if (0u == VpsDlm_getNumActiveClients(listNum))
            {
                /* Wait for the ListComplete Callback */
                Semaphore_pend(dcCtrlInfo->isrWaitSem, DC_SEM_TIMEOUT);

                /* Since there are no client active, free up the list and
                 * other resources */
                for (cnt = 0u; cnt < numEndNodes; cnt ++)
                {
                    node = dcGetNodeInfo(endNodes[cnt]);
                    GT_assert(DcTrace, (NULL != node));
                    outputNode = (Dc_OutputNodeInfo *) node->parent;
                    GT_assert(DcTrace, (NULL != outputNode));
                    outputNode->isListAllocated = FALSE;
                    outputNode->listNum = 0;
                }

                GT_1trace(DcTrace,
                          GT_INFO,
                          "%s: Free Resources",
                          __FUNCTION__);

                retVal = Vrm_releaseList(listNum);
                retVal |= Vrm_releaseLmFid(lmFidNum);
                retVal |= dcReleaseFreeClient(freeClientNum);
                retVal |= VpsDlm_freeListResource(listNum);
                if (FVID2_SOK != retVal)
                {
                    GT_2trace(DcTrace,
                              GT_ERR,
                              "%s: Return from VpsDlm_freeListResource %d",
                              __FUNCTION__,
                              retVal);
                }
            }

            VpsDlm_clrDescMem(dcCtrlInfo->dlmClientCtrlHandle);
            inputNode->isStarted = FALSE;
        }
        Semaphore_post(dcCtrlInfo->sem);
    }

    return (retVal);
}



static Int32 dcEnableInputPaths(
                Dc_InputNodeInfo            *inputNode,
                UInt32                      *endNodes,
                UInt32                      *numEndNodes,
                UInt32                       isEnable)
{
    Int32                retVal = FVID2_SOK;
    UInt32               endNode, cnt, descSet, numActClients;
    Dc_NodeInfo         *currNode = NULL, *prevNode = NULL;
    UInt32               traversePaths = TRUE;
    Vps_DcModeInfo       modeInfo;
    Dc_OutputNodeInfo   *outputNode = NULL;
    Vps_DcNodeInput      nodeInfo;
    Dc_MemInfo           memInfo;
    Dc_DescInfo          descInfo;
    UInt8               *tempPtr;

    GT_assert(DcTrace, (NULL != inputNode));
    GT_assert(DcTrace, (NULL != endNodes));
    GT_assert(DcTrace, (NULL != numEndNodes));

    /* Initialize the structures */
    nodeInfo.isEnable = isEnable;

    VpsUtils_memset(gDcShOvlyMem, 0x0, DC_SHADOW_REG_OVLY_SIZE);
    VpsUtils_memset(gDcNonShOvlyMem, 0x0, DC_NONSHADOW_REG_OVLY_SIZE);
    VpsUtils_memset(gDcDescMem, 0x0, DC_DESC_MEM_SIZE);

    memInfo.shadowRegOvlyMem = gDcShOvlyMem;
    memInfo.nonShadowRegOvlyMem = gDcNonShOvlyMem;
    memInfo.shadowConfigOvlySize = 0u;
    memInfo.nonShadowConfigOvlySize = 0u;

    *numEndNodes = 0u;
    /* Get the pointer to the current Node */
    currNode = inputNode->node;

    /* Initialize the traverser with the current node */
    dcInitTraverser(currNode);

    /* Get the next node to be traversed */
    currNode = dcGetNextTraversedNode(TRUE);
    while (NULL != currNode)
    {
        /* If it is the last node, store it into the array of end nodes
         * and increment and end node counter */
        if(TRUE == dcIsLastNode(currNode, TRUE))
        {
            endNodes[(*numEndNodes)] = currNode->nodeNum;
            (*numEndNodes) ++;
            traversePaths = TRUE;
        }

        /* Get the stack top node */
        prevNode = dcStackGetParentNode();

        /* Enable next node as output in the current node and
         * current node as input to next Node */
        if ((NULL != currNode) && (NULL != prevNode))
        {
            if ((NULL != prevNode->setOutput) && (TRUE == traversePaths))
            {
                nodeInfo.nodeId = prevNode->nodeNum;
                nodeInfo.inputId = currNode->nodeNum;
                /* Enabled Output in the current Node */
                retVal = prevNode->setOutput(&nodeInfo, &memInfo);
            }
            if ((NULL != currNode->setInput) && (TRUE == traversePaths))
            {
                nodeInfo.nodeId = currNode->nodeNum;
                nodeInfo.inputId = prevNode->nodeNum;
                /* Enabled Output in the current Node */
                retVal = currNode->setInput(&nodeInfo, &memInfo);
            }
            /* Do not need to disable paths if there is comp node in the
             * path and comp has multiple inputs enabled */
            if (VPS_DC_NODETYPE_COMP == currNode->nodeType)
            {
                /* If node is to be disabled, decrement number of enabled
                 * nodes for the comp node since it is already disabled in the
                 * overlay and then check for the number of enabled path in
                 * the comp. */
                if (FALSE == isEnable)
                {
                    currNode->numEnabledInputs --;
                }
                if (currNode->numEnabledInputs > 0u)
                {
                    traversePaths = FALSE;
                }
                if (TRUE == isEnable)
                {
                    currNode->numEnabledInputs ++;
                }
            }
        }

        /* If the current node is vcomp, its input needs to be enabled in
           the RT config also */
        if (DC_NODE_VCOMP == currNode->nodeNum)
        {
            for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
            {
                if (TRUE == isEnable)
                {
                    retVal |= dcVcompEnableInput(
                            &nodeInfo,
                            inputNode->rtConfigOvlyPtr[DC_RTCONFIGMODULE_VCOMP]
                            [descSet]);
                }
            }
        }

        if(FVID2_SOK != retVal)
        {
            /* No need to continue if error is returned when enabling/disabling
             * input or output. */
            break;
        }

        /* dcGetNextTraversedNode */
        currNode = dcGetNextTraversedNode(TRUE);
    }

    /* Check to see if interlacing is enabled in the CIG and output venc set
     * is in progressive, return error */
    if ((FVID2_SOK == retVal) && (TRUE == isEnable))
    {
        /* For each of the output nodes, check if it progressive and its
         * input from CIG is in interlacing enabled*/
        for (cnt = 0u; (cnt < (*numEndNodes)) && (FVID2_SOK == retVal); cnt ++)
        {
            endNode = endNodes[cnt];
            currNode = dcGetNodeInfo(endNode);
            GT_assert(DcTrace, (NULL != currNode));

            outputNode = (Dc_OutputNodeInfo *)(currNode->parent);

            retVal = outputNode->getMode(outputNode->vencHandle, &modeInfo);

            /* There is some error returned from teh getmodeinfo API */
            if (FVID2_SOK != retVal)
            {
                break;
            }

            /* Set Mode Information in all intermediate modules
               coming in this path */
            /* Assuming here that writing register with the same
               value here does not affect path which is alread streaming on */
            retVal = dcSetModeInfo(endNode, &modeInfo, FALSE);
        }
    }

    if (FALSE == isEnable)
    {
        currNode = dcGetNodeInfo(endNodes[0u]);
        GT_assert(DcTrace, (NULL != currNode));
        numActClients = VpsDlm_getNumActiveClients(
                            ((Dc_OutputNodeInfo *)(currNode->parent))->listNum);
    }

    /* Create the configuration descriptor and set it in the DLM */
    if (FVID2_SOK == retVal)
    {
        VpsUtils_memset(&descInfo, 0u, sizeof(descInfo));

        tempPtr = gDcDescMem;
        for (cnt = 0u; cnt < DC_NUM_DESC_SET; cnt ++)
        {
            descInfo.descAddr[cnt][DC_SHADOW_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            descInfo.descSize[cnt][DC_SHADOW_IDX] =
                VPSHAL_VPDMA_CTRL_DESC_SIZE + VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            descInfo.rldDescAddr[cnt][DC_SHADOW_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;

            descInfo.descAddr[cnt][DC_NSHADOW_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            descInfo.descSize[cnt][DC_NSHADOW_IDX] =
                VPSHAL_VPDMA_CTRL_DESC_SIZE + VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            descInfo.rldDescAddr[cnt][DC_NSHADOW_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;

            VpsHal_vpdmaCreateDummyDesc(
                descInfo.descAddr[cnt][DC_SHADOW_IDX]);
            VpsHal_vpdmaCreateDummyDesc(
                descInfo.descAddr[cnt][DC_NSHADOW_IDX]);
            if (memInfo.shadowConfigOvlySize)
            {
                /* Create Config Descriptor for shadow/nonshadow configurations */
                VpsHal_vpdmaCreateConfigDesc(
                    descInfo.descAddr[cnt][DC_SHADOW_IDX],
                    VPSHAL_VPDMA_CONFIG_DEST_MMR,
                    VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    memInfo.shadowConfigOvlySize,
                    memInfo.shadowRegOvlyMem,
                    0u,
                    0u);
            }
            if (memInfo.nonShadowConfigOvlySize)
            {
                VpsHal_vpdmaCreateConfigDesc(
                    descInfo.descAddr[cnt][DC_NSHADOW_IDX],
                    VPSHAL_VPDMA_CONFIG_DEST_MMR,
                    VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                    VPSHAL_VPDMA_CCT_INDIRECT,
                    memInfo.nonShadowConfigOvlySize,
                    memInfo.nonShadowRegOvlyMem,
                    0u,
                    0u);
            }
            if ((FALSE == isEnable) && (0 == cnt) &&
                (inputNode->numDlmStartClient == numActClients) /*&&
                (DLM_CT_GRPX0 != inputNode->dlmClientType) &&
                (DLM_CT_GRPX1 != inputNode->dlmClientType) &&
                (DLM_CT_GRPX2 != inputNode->dlmClientType)*/)
            {
                VpsHal_vpdmaCreateDummyDesc(
                    descInfo.descAddr[cnt][DC_SHADOW_IDX]);
                VpsHal_vpdmaCreateDummyDesc(
                    descInfo.descAddr[cnt][DC_NSHADOW_IDX]);
            }
        }

        retVal = VpsDlm_setDescMem(
                    dcGetDctrlObj()->dlmClientCtrlHandle,
                    (VpsDlm_DescInfo *) &descInfo);
    }

    return (retVal);
}



static Int32 dcCheckForTiedVencs(
                Dc_DisplayCtrlInfo          *dcCtrlInfo,
                UInt32                      *endNodes,
                UInt32                       numEndNodes)
{
    Int32 retVal = FVID2_SOK;
    UInt32 cnt;
    UInt32 vencs = 0u;
    Dc_OutputNodeInfo *outputNode = NULL;
    Dc_NodeInfo       *node = NULL;

    /* If number of end nodes are more than one, they should be tied
     * together. If they are not tied, return error. */
    if (1u < numEndNodes)
    {
        /* Get the bitmask of the all the output VENCs */
        for (cnt = 0u; cnt < numEndNodes; cnt ++)
        {
            node = dcGetNodeInfo(endNodes[cnt]);
            GT_assert(DcTrace, (NULL != node));
            outputNode = (Dc_OutputNodeInfo *) node->parent;
            GT_assert(DcTrace, (NULL != outputNode));
            vencs |= outputNode->nodeId;
        }

        if ((dcCtrlInfo->tiedVencs & vencs) != vencs)
        {
            retVal = FVID2_EFAIL;
        }
    }
    return (retVal);
}



static Int32 dcGetResources(
                UInt8                       *listNum,
                UInt32                      *lmFidNum,
                VpsHal_VpdmaChannel         *freeClientNum,
                UInt32                      *isListAllocated,
                UInt32                      *endNodes,
                UInt32                       numEndNodes)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt, cnt2;
    Dc_OutputNodeInfo  *outputNode = NULL;
    UInt32              listAllocated = FALSE;
    UInt32              lmFidAllocated = FALSE;
    UInt32              freeClientAllocated = FALSE;
    Dc_NodeInfo        *node = NULL;

    GT_assert(DcTrace, (NULL != listNum));
    GT_assert(DcTrace, (NULL != lmFidNum));
    GT_assert(DcTrace, (NULL != freeClientNum));
    GT_assert(DcTrace, (NULL != isListAllocated));
    GT_assert(DcTrace, (NULL != endNodes));

    *isListAllocated = FALSE;

    /* If the list is already allocated to one of the tied nodes,
     * list must be allocated to other nodes in the tied set and
     * this list should be same */
    for (cnt = 0u; cnt < numEndNodes; cnt ++)
    {
        node = dcGetNodeInfo(endNodes[cnt]);
        GT_assert(DcTrace, (NULL != node));
        outputNode = (Dc_OutputNodeInfo *) node->parent;
        GT_assert(DcTrace, (NULL != outputNode));

        if(TRUE == outputNode->isListAllocated)
        {
            listAllocated = TRUE;

            /* Copy List Num, LM Fid number and free client number from the
             * output node*/
            *listNum = outputNode->listNum;
            *lmFidNum = outputNode->lmFidNum;
            *freeClientNum = outputNode->freeClientNum;

            /* All the tied output nodes should have same list number
             * allocated them */
            for (cnt2 = 0u; cnt2 < numEndNodes; cnt2 ++)
            {
                node = dcGetNodeInfo(endNodes[cnt2]);
                GT_assert(DcTrace, (NULL != node));
                outputNode = (Dc_OutputNodeInfo *) node->parent;
                GT_assert(DcTrace, (NULL != outputNode));
                if((FALSE == outputNode->isListAllocated) ||
                   ((*listNum) != outputNode->listNum))
                {
                    retVal = FVID2_EFAIL;
                    break;
                }
            }
            break;
        }
    }

    /* If the list is not allocated to the output node, get a free
     * list from the VRM and assign it to all output nodes */
    if ((FVID2_SOK == retVal) && (FALSE == listAllocated))
    {
        /* Get a free list from the Resource manager */
        retVal = Vrm_allocList(listNum, VRM_LIST_TYPE_DISPLAY);
        if(FVID2_SOK == retVal)
        {
            listAllocated = TRUE;

            /* Get a free LM FID from the Resource Manager */
            retVal = Vrm_allocLmFid(lmFidNum);
        }

        if (FVID2_SOK == retVal)
        {
            lmFidAllocated = TRUE;

            /* Get the Free Write client */
            retVal = dcGetFreeClient(freeClientNum);
        }

        if (FVID2_SOK == retVal)
        {
            freeClientAllocated = TRUE;
            /* Assign List to all the outputNodes */
            for (cnt = 0u; cnt < numEndNodes; cnt++)
            {
                node = dcGetNodeInfo(endNodes[cnt]);
                GT_assert(DcTrace, (NULL != node));
                outputNode = (Dc_OutputNodeInfo *) node->parent;
                outputNode->isListAllocated = TRUE;
                outputNode->listNum = *listNum;
                outputNode->lmFidNum = *lmFidNum;
                outputNode->freeClientNum = *freeClientNum;
            }
        }
    }

    if (FVID2_SOK != retVal)
    {
        /* There is some error so free list if it is allocated */
        if (TRUE == listAllocated)
        {
            Vrm_releaseList(*listNum);
            listAllocated = FALSE;
        }

        /* release lm fid if it is allocated */
        if (TRUE == lmFidAllocated)
        {
            Vrm_releaseLmFid(*lmFidNum);
        }

        /* Free write client if it is allocated */
        if (TRUE == freeClientAllocated)
        {
            dcReleaseFreeClient(*freeClientNum);
        }
    }

    *isListAllocated = listAllocated;

    return (retVal);
}



static Int32 dcGetFreeClient(VpsHal_VpdmaChannel *freeClientNum)
{
    Int32 retVal = FVID2_EFAIL;
    UInt32 cnt;

    GT_assert(DcTrace, (NULL != freeClientNum));

    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt ++)
    {
        if (FALSE == DcFreeClientFlag[cnt])
        {
            break;
        }
    }

    if (cnt < DC_MAX_OUTPUT)
    {
        *freeClientNum = DcFreeClientNum[cnt];
        DcFreeClientFlag[cnt] = TRUE;
        retVal = FVID2_SOK;
    }
    return (retVal);
}



static Int32 dcReleaseFreeClient(VpsHal_VpdmaChannel freeClientNum)
{
    Int32 retVal = FVID2_EFAIL;
    UInt32 cnt;

    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt ++)
    {
        if ((freeClientNum == DcFreeClientNum[cnt]) &&
            (TRUE == DcFreeClientFlag[cnt]))
        {
            DcFreeClientFlag[cnt] = FALSE;
            retVal = FVID2_SOK;
            break;
        }
    }
    return (retVal);
}



/* Creates the configuration overlays used by display controller to
 * enable/display specific path in VCOMP or in BLEND. Also it assigns
 * appropriate overlay pointer to nodeinfo structure */
static Int32 dcCreateConfigOverlay(Dc_DisplayCtrlInfo *dcCtrlInfo)
{
    Int32                   retVal = FVID2_SOK;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));
    /* Register DLM client for adding and removing clients with the
       display controller client type and nonFbmode */
    dcCtrlInfo->dlmClientCtrlHandle = VpsDlm_register(
                                        DLM_CT_DCTRL_MUX,
                                        FALSE,
                                        (VpsDlm_ClientCbFxn)dcDlmClientCallBack,
                                        dcCtrlInfo);

    if (NULL == dcCtrlInfo->dlmClientCtrlHandle)
    {
        GT_1trace(DcTrace, GT_ERR, "%s: DLM Registration Failed", __FUNCTION__);
        retVal = FVID2_EFAIL;
    }

    return (retVal);
}



/** \brief VCOMP, CIG and COMP supports runtime configuration. For
 *  this runtime configuration, overlay needs to be created and each
 *  of these modules must be registered to the DLM. This function performs
 *  the same.
 */
static Int32 dcCreateRtConfigOverlay(Dc_DisplayCtrlInfo *dcCtrlInfo)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cnt, ovlySize;
    VpsHal_VcompInputVideo  vcompInputVideo;
    Dc_InitParams          *initPrms;
    UInt32                  descSet;
    VpsHal_Handle           halHandle;
    Dc_DescInfo             descInfo;
    UInt8                  *tempPtr;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));

    initPrms = &dcCtrlInfo->initParams;

    tempPtr = gDcRtCfgOvlyMem;
    for (cnt = 0u; cnt < DC_RT_CONFIG_NUM_IDX; cnt ++)
    {
        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
        {
            dcCtrlInfo->ovlyPtr[cnt][descSet] = tempPtr;
            tempPtr += (DC_RUNTIME_CONFIG_OVLY_SIZE /
                        (DC_NUM_DESC_SET * DC_RT_CONFIG_NUM_IDX));
        }
        dcCtrlInfo->ovlySize[cnt] = 0u;
    }

    halHandle = initPrms->halHandle[VDC_VCOMP_IDX];
    if (NULL != halHandle)
    {
        /* Get config overlay size for the main path in VCOMP */
        vcompInputVideo = VPSHAL_VCOMP_SOURCE_MAIN;
        ovlySize = VpsHal_vcompGetConfigOvlySize(halHandle, vcompInputVideo);

        /* Store config overlay size in control info structure */
        dcCtrlInfo->ovlySize[DC_RT_CONFIG_VCOMP_MAIN_IDX] = ovlySize;

        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
        {
            /* Create Overlay in the overlay memory */
            retVal = VpsHal_vcompCreateConfigOvly(
                    halHandle,
                    vcompInputVideo,
                    dcCtrlInfo->ovlyPtr[DC_RT_CONFIG_VCOMP_MAIN_IDX][descSet]);
        }

        /* Get config overlay size for the Aux path in VCOMP */
        vcompInputVideo = VPSHAL_VCOMP_SOURCE_AUX;
        ovlySize = VpsHal_vcompGetConfigOvlySize(halHandle, vcompInputVideo);

        /* Store config overlay size in control info structure */
        dcCtrlInfo->ovlySize[DC_RT_CONFIG_VCOMP_AUX_IDX] = ovlySize;

        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
        {
            /* Create Overlay in the overlay memory */
            retVal = VpsHal_vcompCreateConfigOvly(
                    halHandle,
                    vcompInputVideo,
                    dcCtrlInfo->ovlyPtr[DC_RT_CONFIG_VCOMP_AUX_IDX][descSet]);
        }
    }

    halHandle = initPrms->halHandle[VDC_CIG_IDX];
    if (NULL != halHandle)
    {
        /* Get config overlay size for the PIP Path in CIG */
        ovlySize = VpsHal_cigGetConfigOvlySize(halHandle, VPSHAL_COT_RT_CONFIG);

        /* Store config overlay size in control info structure */
        dcCtrlInfo->ovlySize[DC_RT_CONFIG_CIG_PIP_IDX] = ovlySize;

        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
        {
            /* Create Overlay in the overlay memory */
            retVal = VpsHal_cigCreateConfigOvly(
                        halHandle,
                        VPSHAL_COT_RT_CONFIG,
                        dcCtrlInfo->ovlyPtr[DC_RT_CONFIG_CIG_PIP_IDX][descSet]);
        }
    }

    for (cnt = 0u; cnt < DC_RT_CONFIG_NUM_IDX; cnt ++)
    {
        /* Assign Overlay Pointer to the memory */
        if (0u == dcCtrlInfo->ovlySize[cnt])
        {
            for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
            {
                dcCtrlInfo->ovlyPtr[cnt][descSet] = NULL;
            }
        }
    }

    tempPtr = gDcRtCfgDescMem;
    for (cnt = 0u; (cnt < DC_RT_CONFIG_NUM_IDX) && (FVID2_SOK == retVal); cnt ++)
    {
        /* Register to Dlm client for the run time configuration. Since
         * this overlay memory will be updated in the client call back
         * function, there is no need to register call back function in
         * the DLM */
        /* Register with the display controller client type and nonFbMode. */
        dcCtrlInfo->rtDlmHandle[cnt] = VpsDlm_register(
                                          DLM_CT_DCTRL_RT,
                                          FALSE,
                                          NULL,
                                          NULL);

        if (NULL == dcCtrlInfo->rtDlmHandle[cnt])
        {
            retVal = FVID2_EFAIL;
            break;
        }

        VpsUtils_memset(&descInfo, 0u, sizeof(descInfo));

        for (descSet = 0u; descSet < DC_NUM_DESC_SET; descSet ++)
        {
            descInfo.descAddr[descSet][DC_SHADOW_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            descInfo.rldDescAddr[descSet][DC_SHADOW_IDX] = tempPtr;
            tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
            descInfo.descSize[descSet][DC_SHADOW_IDX] =
                VPSHAL_VPDMA_CTRL_DESC_SIZE + VPSHAL_VPDMA_CONFIG_DESC_SIZE;

            /* Create Config Descriptor for shadow/nonshadow configurations */
            VpsHal_vpdmaCreateConfigDesc(
                descInfo.descAddr[descSet][DC_SHADOW_IDX],
                VPSHAL_VPDMA_CONFIG_DEST_MMR,
                VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                dcCtrlInfo->ovlySize[cnt],
                dcCtrlInfo->ovlyPtr[cnt][descSet],
                0u,
                0u);
        }

        retVal = VpsDlm_setDescMem(
                    dcCtrlInfo->rtDlmHandle[cnt],
                    (VpsDlm_DescInfo *) &descInfo);
    }

    return (retVal);
}



/**
 * Dlm call back function. This function just post the semaphore on
 * which startClient/StopClient function waiting on. */
static Int32 dcDlmClientCallBack(UInt32 curSet, UInt32 timeStamp, Ptr arg)
{
    Int32 retVal;
    Dc_DisplayCtrlInfo *dcCtrlInfo = (Dc_DisplayCtrlInfo *)arg;

    if (1u == curSet)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Remove DC client from DLM",
                  __FUNCTION__);

        retVal = VpsDlm_unLinkDctrlClient(dcCtrlInfo->dlmClientCtrlHandle);
        if (FVID2_SOK != retVal)
        {
            GT_2trace(DcTrace,
                      GT_ERR,
                      "%s: Return from VpsDlm_unLinkDctrlClient %d",
                      __FUNCTION__,
                      retVal);
        }

        Semaphore_post(dcCtrlInfo->isrWaitSem);
    }

    return (FVID2_SOK);
}



/**
 *  Dc_dlmListCompleteCb
 *  \brief Callback function called by DLM to intimate the DCTRL that a
 *  particular list is complete.
 */
Int32 Dc_dlmListCompleteCb(UInt32 listNum)
{
    GT_2trace(DcTrace, GT_INFO, "%s: List %d completed", __FUNCTION__, listNum);
    Semaphore_post(dcGetDctrlObj()->isrWaitSem);

    return (FVID2_SOK);
}

