/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vps_dctrl.c
 *
 * \brief Display Controller HAL Source File
 * This file implements HAL APIs
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/trace.h>

#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_vps.h>

#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/drivers/display/vps_dlm.h>

#include <ti/psp/vps/drivers/display/vps_dctrlInt.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlPriv.h>

#include <ti/psp/vps/drivers/display/src/vps_dctrlHalPriv.h>

#include <ti/psp/vps/hal/vpshal_comp.h>
#include <ti/psp/vps/hal/vpshal_cig.h>
#include <ti/psp/vps/hal/vpshal_vcomp.h>
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/vps/hal/vpshal_ede.h>
#include <ti/psp/vps/hal/vpshal_cproc.h>

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

/* Function to configure VCOMP Open Config */
static Int32 dcConfigVcomp(Vps_DcVcompConfig *vcompConfig,
                           Dc_InitParams *initParams);

/* Function to configure EDE Open Config */
static Int32 dcConfigEde(Vps_DcEdeConfig *edeConfig,
                         Dc_InitParams *initParams);

/* Function to configure CIG Main Open Config */
static Int32 dcConfigCigMain(Vps_DcCigMainConfig *cigConfig,
                             Dc_InitParams *initParams);

/* Function to configure CIG Main Open Config */
static Int32 dcConfigCproc(Vps_CprocConfig *cproc,
                           Dc_InitParams *initParams);

/* Function to configure CIG Aux Open Config */
static Int32 dcConfigCigAux(Vps_DcCigPipConfig *cigConfig,
                            Dc_InitParams *initParams);

/* Function to configure CIG Aux Open Config */
static Int32 dcConfigComp(Vps_DcCompConfig *compConfig,
                          Dc_InitParams *initParams);


/* ========================================================================== */
/*                        Global Variables                                    */
/* ========================================================================== */

/* Resource Manager */
Dc_ResMngr DcResMngr[DC_MAX_RESOURCES] = DC_RES_MGR_DEFAUTS;

/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */

Int32 dcCompSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InitParams      *initParams;
    VpsHal_CompId       compId;
    VpsHal_CompMode     compMode;
    VpsHal_Handle       compHandle;
    Dc_NodeInfo        *node = NULL;

    initParams = &(dcGetDctrlObj()->initParams);
    GT_assert(DcTrace, (NULL != initParams));

    node = dcGetNodeInfo(nodeNum);
    GT_assert(DcTrace, (NULL != node));
    compId = (VpsHal_CompId) node->priv;

    if (FVID2_SF_INTERLACED == modeInfo->mInfo.scanFormat)
    {
        compMode = VPSHAL_COMP_MODE_INTERLACED;
    }
    else
    {
        compMode = VPSHAL_COMP_MODE_PROGRESSIVE;
    }

    /* NOTE: All SetModeInfo API will be called as part of
       setting mode in the VENC, so if the handle is not available,
       function will return success without configuring module. */
    compHandle = initParams->halHandle[VDC_COMP_IDX];
    if (NULL != compHandle)
    {
        GT_3trace(DcTrace,
                  GT_INFO,
                  "%s : Comp Id %d CompMode %d",
                  __FUNCTION__,
                  compId,
                  compMode);

        retVal = VpsHal_compEnableOutput(compHandle, compId, compMode);
    }
    return (retVal);
}



Int32 dcVcompSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo)
{
    Int32                           retVal = FVID2_EINVALID_PARAMS;
    Dc_InitParams                  *initParams;
    VpsHal_VcompOutputDsplyConfig   vcompOutputConfig;
    VpsHal_Handle                   halHandle;
#ifdef TI_816X_BUILD
    VpsHal_EdeConfig                edeConfig;
#else
    Vps_CscConfig                   *cscConfig;
#endif
    UInt32                          shift;

    if (FVID2_SF_INTERLACED == modeInfo->mInfo.scanFormat)
    {
        /* Configure field size */
        shift = 1u;
    }
    else
    {
        shift = 0u;
    }

    /* Get the pointer to initParams */
    initParams = &(dcGetDctrlObj()->initParams);
    GT_assert(DcTrace, (NULL != initParams));

    halHandle = initParams->halHandle[VDC_VCOMP_IDX];
    if (NULL != halHandle)
    {
        /* Get the Default output Configuration */
        retVal = VpsHal_vcompGetOutputDisplayConfig(
                    halHandle,
                    &vcompOutputConfig);
        if (0 == retVal)
        {
            GT_3trace(DcTrace,
                      GT_INFO,
                      "%s : VComp: %dx%d",
                      __FUNCTION__,
                      modeInfo->mInfo.width,
                      modeInfo->mInfo.height);

            /* Set the VCOMP output display parameters */
            vcompOutputConfig.dsplyNumPix = modeInfo->mInfo.width;
            vcompOutputConfig.dsplyNumlines = modeInfo->mInfo.height >> shift;

            /* Set the VCOMP Output parameters using Hal layer */
            retVal = VpsHal_vcompSetOutputDisplayConfig(
                        halHandle,
                        &vcompOutputConfig,
                        NULL);
        }
    }

#ifdef TI_816X_BUILD
    halHandle = initParams->halHandle[VDC_EDE_IDX];
    if ((NULL != halHandle) && (FVID2_SOK == retVal))
    {
        /* Get the Default output Configuration */
        retVal = VpsHal_edeGetConfig(halHandle, &edeConfig);
        if (0 == retVal)
        {
            GT_3trace(DcTrace,
                      GT_INFO,
                      "%s : EDE: %dx%d",
                      __FUNCTION__,
                      modeInfo->mInfo.width,
                      modeInfo->mInfo.height);

            /* Set the EDE parameters */
            edeConfig.width = modeInfo->mInfo.width;
            edeConfig.height = modeInfo->mInfo.height >> shift;

            /* Set the EDE parameters using Hal layer */
            retVal = VpsHal_edeSetConfig(halHandle, &edeConfig, NULL);
        }
    }

    halHandle = initParams->halHandle[VDC_CPROC_IDX];
    if ((NULL != halHandle) && (FVID2_SOK == retVal))
    {
        retVal = VpsHal_cprocSetFrameSize(
                    halHandle,
                    modeInfo->mInfo.width,
                    modeInfo->mInfo.height >> shift,
                    NULL);
    }
#else
    halHandle = initParams->halHandle[VDC_VCOMP_CSC_IDX];
    if ((NULL != halHandle) && (FVID2_SOK == retVal))
    {
        cscConfig = &(dcGetDctrlObj()->cscCfg[VPS_DC_VCOMP_CSC]);
        retVal = VpsHal_cscSetConfig(halHandle, cscConfig, NULL);
    }
#endif

    return (retVal);
}



Int32 dcCigSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InitParams      *initParams = &(dcGetDctrlObj()->initParams);
    VpsHal_CigConfig    cigConfig;
    VpsHal_Handle       cigHandle;
    UInt32              shift, cookie;

    if (FVID2_SF_INTERLACED == modeInfo->mInfo.scanFormat)
    {
        /* Configure field size */
        shift = 1u;
    }
    else
    {
        shift = 0u;
    }

    cigHandle = initParams->halHandle[VDC_CIG_IDX];
    if (NULL != cigHandle)
    {
        /* Since configuration for Main path and PIP path is in a
           single register and main path and PIP could be used in display as
           well as mem2mem driver, Read and write to CIG register
           should be atomic. */
        cookie = Hwi_disable();

        /* Get the Default PIP Configuration */
        retVal = VpsHal_cigGetConfig(cigHandle, &cigConfig);
        if (0 == retVal)
        {
            /* Set the CIG parameters */
            if (DC_NODE_CIG_0 == nodeNum)
            {
                GT_3trace(DcTrace,
                          GT_INFO,
                          "%s : CIG_1: %dx%d",
                          __FUNCTION__,
                          modeInfo->mInfo.width,
                          modeInfo->mInfo.height);

                /* Configure nonPIP path parameters */
                cigConfig.width = modeInfo->mInfo.width;
                cigConfig.height = modeInfo->mInfo.height >> shift;
            }
            else
            {
                GT_3trace(DcTrace,
                          GT_INFO,
                          "%s : CIG_2: %dx%d",
                          __FUNCTION__,
                          modeInfo->mInfo.width,
                          modeInfo->mInfo.height);

                /* Configure PIP Parameters */
                cigConfig.pipConfig.xPos = cigConfig.pipConfig.yPos = 0u;
                cigConfig.pipConfig.dispWidth = modeInfo->mInfo.width;
                cigConfig.pipConfig.dispHeight = modeInfo->mInfo.height >> shift;
                cigConfig.pipConfig.pipWidth = modeInfo->mInfo.width;
                cigConfig.pipConfig.pipHeight = modeInfo->mInfo.height >> shift;

            }

            /* Set the CIG PIP parameters using Hal layer */
            retVal = VpsHal_cigSetConfig(cigHandle, &cigConfig, NULL);
        }
        Hwi_restore(cookie);
    }

    return (retVal);
}



/**
 * This function sets parameters of CSC available after HDCOMP and SDVENC
 * multiplexers.
 */
Int32 dcMuxSetModeInfo(UInt32 nodeNum, Vps_DcModeInfo *modeInfo)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_InitParams      *initParams = &(dcGetDctrlObj()->initParams);
    VpsHal_Handle       cscHandle;
    Vps_CscConfig      *cscConfig;
    Dc_DisplayCtrlInfo *dcCtrlInfo;

    dcCtrlInfo = dcGetDctrlObj();

    if (DC_NODE_SD_MUX == nodeNum)
    {
        cscConfig = &dcCtrlInfo->cscCfg[VPS_DC_SD_CSC];
        cscHandle = initParams->halHandle[VDC_SD_CSC_IDX];
    }
    else if (DC_NODE_HDCOMP_MUX == nodeNum)
    {
        /* Assumption: HD VENC supports only HD Modes */
        cscConfig = &dcCtrlInfo->cscCfg[VPS_DC_HDCOMP_CSC];
        cscHandle = initParams->halHandle[VDC_AUX_CSC_IDX];
    }
    else
    {
        cscHandle = NULL;
    }

    /* If Mode is SD, set SDTV Y2R mode in CSC */
    if (NULL != cscHandle)
    {
        GT_2trace(DcTrace,
                  GT_INFO,
                  "%s : CSC : %d",
                  __FUNCTION__,
                  cscConfig->mode);

        /* Set the CSC parameters using Hal layer */
        retVal = VpsHal_cscSetConfig(cscHandle, cscConfig, NULL);
    }

    return (retVal);
}



Int32 dcMuxSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  numInputs, cnt;
    Dc_NodeInfo            *node = NULL;
    UInt32                  mux;
    Ptr                     ovlyPtr;

    GT_assert(DcTrace, (NULL != nodeInfo));
    GT_assert(DcTrace, (NULL != memInfo));

    node = dcGetNodeInfo(nodeInfo->nodeId);
    GT_assert(DcTrace, (NULL != node));

    /* Select index of the input path for the given nodes. Since all the
     * inputs paths in ascending order in the input structure of the node,
     * index of the input path in the node structure can be directly used
     * to select input path the VPS Hal */
    numInputs = node->input.numNodes;

    for (cnt = 0; cnt < numInputs; cnt ++)
    {
        if(nodeInfo->inputId == node->input.node[cnt]->nodeNum)
        {
            break;
        }
    }

    /* Select the mux number from the private data of the node */
    mux = (UInt32)(node->priv);

    if (TRUE == nodeInfo->isEnable)
    {
        cnt += 1u; /* Since input path number starts from 1, input
                         * num is incremented by 1 */
    }
    else
    {
        cnt = VPSHAL_VPS_VENC_MUX_SRC_DISABLED;  /* disable multiplexer */
    }

    /* Calculate the Overlay Pointer */
    ovlyPtr = (Ptr) (((UInt32)memInfo->nonShadowRegOvlyMem) +
                        memInfo->nonShadowConfigOvlySize);
    memInfo->nonShadowConfigOvlySize +=
        VpsHal_vpsGetOvlySize(VPSHAL_OVLY_TYPE_MUX);
    /* Create the configuration Overlay at this location */
    retVal = VpsHal_vpsCreateConfigOvly(VPSHAL_OVLY_TYPE_MUX, ovlyPtr);

    GT_4trace(DcTrace,
              GT_INFO,
              "%s : CSC : Mux %d Source %d OverlayPtr %x",
              __FUNCTION__,
              mux,
              cnt,
              ovlyPtr);

    /* Select the mux source */
    retVal |= VpsHal_vpsVencMuxSrcSelect(
                (VpsHal_VpsVencMux)mux,
                (VpsHal_VpsVencMuxSrc)cnt,
                ovlyPtr);

    return (retVal);
}



Int32 dcMuxSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    /* There is nothing to be done for enabling output on MUX */
    return (0);
}



Int32 dcVcompSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    Int32                   retVal = FVID2_EINVALID_PARAMS;
    Dc_InitParams          *initParams = &(dcGetDctrlObj()->initParams);
    Dc_NodeInfo            *node = NULL, *parentNode;
    VpsHal_VcompInputVideo  inputVideo;
    Ptr                     ovlyPtr = NULL;
    VpsHal_Handle           vcompHandle;
    UInt32                  width, height;

    GT_assert(DcTrace, (NULL != nodeInfo));
    GT_assert(DcTrace, (NULL != memInfo));

    /* Note: If the input is not enabled, data will not flow, so
       if the handle is not available here, return error */
    vcompHandle = initParams->halHandle[VDC_VCOMP_IDX];
    if (NULL != vcompHandle)
    {
        node = dcGetNodeInfo(nodeInfo->nodeId);
        GT_assert(DcTrace, (NULL != node));
        parentNode = dcGetNodeInfo(nodeInfo->inputId);
        GT_assert(DcTrace, (NULL != parentNode));

        if (DC_NODE_PRI_MUX == parentNode->nodeNum)
        {
            inputVideo = VPSHAL_VCOMP_SOURCE_MAIN;
        }
        else
        {
            inputVideo = VPSHAL_VCOMP_SOURCE_AUX;
        }

        /* Calculate the Overlay Pointer */
        if (TRUE == nodeInfo->isEnable)
        {
            ovlyPtr = (Ptr) (((UInt32)memInfo->shadowRegOvlyMem) +
                                        memInfo->shadowConfigOvlySize);
            memInfo->shadowConfigOvlySize +=
                VpsHal_vcompGetEnableConfigOvlySize(
                    vcompHandle,
                    inputVideo);
        }
        else
        {
            ovlyPtr = (Ptr) (((UInt32)memInfo->nonShadowRegOvlyMem) +
                                        memInfo->nonShadowConfigOvlySize);
            memInfo->nonShadowConfigOvlySize +=
                VpsHal_vcompGetEnableConfigOvlySize(
                    vcompHandle,
                    inputVideo);
        }
        /* Create the configuration Overlay at this location */
        retVal = VpsHal_vcompCreateEnableConfigOvly(
                    vcompHandle,
                    inputVideo,
                    ovlyPtr);

        /* Get the input size and set it in the overlay */
        VpsHal_vcompGetInputSize(
                    vcompHandle,
                    inputVideo,
                    &width,
                    &height);

        retVal |= VpsHal_vcompSetInputSize(
                    vcompHandle,
                    inputVideo,
                    width,
                    height,
                    ovlyPtr);

        GT_4trace(DcTrace,
                  GT_INFO,
                  "%s : InputVideo %d Enable %d OverlayPtr %x",
                  __FUNCTION__,
                  inputVideo,
                  nodeInfo->isEnable,
                  ovlyPtr);

        /* Enable/Disable appropriate input path in the VCOMP overlay */
        retVal |= VpsHal_vcompEnableVideoInputPath(
                    vcompHandle,
                    inputVideo,
                    nodeInfo->isEnable,
                    ovlyPtr);
    }
    return (retVal);
}



Int32 dcVcompSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    /* There is nothing to be done for enabling output on VCOMP */
    return (0);
}



Int32 dcCompSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    Int32                   retVal = FVID2_EINVALID_PARAMS;
    Dc_InitParams          *initParams = &(dcGetDctrlObj()->initParams);
    VpsHal_CompId           compId;
    Dc_NodeInfo            *node = NULL;
    UInt32                  cnt, numInputs;
    VpsHal_Handle           compHandle;
    Ptr                     ovlyPtr;

    GT_assert(DcTrace, (NULL != nodeInfo));
    GT_assert(DcTrace, (NULL != memInfo));

    compHandle = initParams->halHandle[VDC_COMP_IDX];
    if (NULL != compHandle)
    {
        node = dcGetNodeInfo(nodeInfo->nodeId);
        GT_assert(DcTrace, (NULL != node));

        /* Get the number of the blender from the private data of the node */
        compId = (VpsHal_CompId)(node->priv);

        /* Select index of the input path for the given nodes. Since all the
         * inputs paths in ascending order in the input structure of the node,
         * index of the input path in the node structure can be directly used
         * to select input path the VPS Hal */
        numInputs = node->input.numNodes;
        for (cnt = 0; cnt < numInputs; cnt ++)
        {
            if(nodeInfo->inputId == node->input.node[cnt]->nodeNum)
            {
                /* Input node is found, use index of this input node to
                 * enable it in the comp node */
                break;
            }
        }
        if (cnt < numInputs)
        {
            /* Calculate the Overlay Pointer */
            ovlyPtr = (Ptr) (((UInt32)memInfo->shadowRegOvlyMem) +
                                    memInfo->shadowConfigOvlySize);
            memInfo->shadowConfigOvlySize +=
                VpsHal_compGetConfigOvlySize(
                    compHandle,
                    compId);

            GT_5trace(DcTrace,
                      GT_INFO,
                      "%s : ID %d input %d enable %d OverlayPtr %x",
                      __FUNCTION__,
                      compId,
                      cnt,
                      nodeInfo->isEnable,
                      ovlyPtr);

            /* Crate Config Overlay for Blender in the given memory */
            retVal = VpsHal_compCreateConfigOvly(compHandle, compId, ovlyPtr);

            /* Enable input path */
            retVal |= VpsHal_compEnableInput(
                        compHandle,
                        compId,
                        cnt,
                        ovlyPtr,
                        nodeInfo->isEnable);
        }
    }
    return (retVal);
}



Int32 dcCompSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    /* There is nothing to be done for enabling output on Blender output
     * since blender output should be enabled along with the VENC enable */
    return (0);
}



Int32 dcCigSetInput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    Int32                   retVal = FVID2_EINVALID_PARAMS;
    UInt32                  cookie;
    Dc_InitParams          *initParams = &(dcGetDctrlObj()->initParams);
    Dc_NodeInfo            *node = NULL;
    VpsHal_Handle           cigHandle;
    Ptr                     ovlyPtr;

    GT_assert(DcTrace, (NULL != nodeInfo));
    GT_assert(DcTrace, (NULL != memInfo));

    /* Only PIP part of the CIG requires enabling. Main CIG doesnot require
     * to be enabled as it can either work in contrained enable mode or bypass
     * mode. Constrained enable is separate control */
    cigHandle = initParams->halHandle[VDC_CIG_IDX];
    if (NULL != cigHandle)
    {
        retVal = 0;

        node = dcGetNodeInfo(nodeInfo->nodeId);
        GT_assert(DcTrace, (NULL != node));

        /* Calculate the Overlay Pointer */
        if (TRUE == nodeInfo->isEnable)
        {
            ovlyPtr = (Ptr) (((UInt32)memInfo->shadowRegOvlyMem) +
                                        memInfo->shadowConfigOvlySize);
            memInfo->shadowConfigOvlySize +=
                VpsHal_cigGetConfigOvlySize(
                    cigHandle,
                    VPSHAL_COT_PIP_ENABLE_CONFIG);
        }
        else
        {
            ovlyPtr = (Ptr) (((UInt32)memInfo->nonShadowRegOvlyMem) +
                                        memInfo->nonShadowConfigOvlySize);
            memInfo->nonShadowConfigOvlySize +=
                VpsHal_cigGetConfigOvlySize(
                    cigHandle,
                    VPSHAL_COT_PIP_ENABLE_CONFIG);
        }

        GT_3trace(DcTrace,
                  GT_INFO,
                  "%s : enable %d OverlayPtr %x",
                  __FUNCTION__,
                  nodeInfo->isEnable,
                  ovlyPtr);

        /* Crate Config Overlay for Blender in the given memory */
        retVal = VpsHal_cigCreateConfigOvly(
                    cigHandle,
                    VPSHAL_COT_PIP_ENABLE_CONFIG,
                    ovlyPtr);

        /* Since configuration for Main path and PIP path is in a
           single register and main path and PIP could be used in display as
           well as mem2mem driver, Read and write to CIG register
           should be atomic. */
        cookie = Hwi_disable();

        if (DC_NODE_CIG_1 == nodeInfo->nodeId)
        {
            retVal |= VpsHal_cigEnablePip(cigHandle, nodeInfo->isEnable, ovlyPtr);
        }
        else
        {
            retVal |= VpsHal_cigEnableMain(cigHandle, nodeInfo->isEnable, ovlyPtr);
        }
        Hwi_restore(cookie);
    }
    return (retVal);
}



Int32 dcCigSetOutput(Vps_DcNodeInput *nodeInfo, Dc_MemInfo *memInfo)
{
    /* There is nothing to be done for enabling output on MUX */
    return (0);
}



Int32 dcConfigHal(Vps_DcCreateConfig *createConfig, Dc_InitParams *initParams)
{
    Int32               retVal = FVID2_SOK;
    UInt32              isVcompAllocated = FALSE;
    UInt32              isPipCigAllocated = FALSE;

    GT_assert(DcTrace, (NULL != createConfig));
    GT_assert(DcTrace, (NULL != initParams));

    /* If createConfig pointer is not null, configure VCOMP, EDE
       and CIG. To configure these modules, get the HQ resource from
       the resource manager */
    if((NULL != createConfig->vcompConfig) ||
       (NULL != createConfig->edeConfig) ||
       (NULL != createConfig->cigMainConfig) ||
       (NULL != createConfig->cprocConfig))
    {
        retVal = dcAllocResource(DC_NODE_VCOMP);

        if (FVID2_SOK == retVal)
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s : HQ Resource is allocated",
                      __FUNCTION__);
            isVcompAllocated = TRUE;
        }
    }

    if (TRUE == isVcompAllocated)
    {
        if (NULL != createConfig->vcompConfig)
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s : Configuring VCOMP",
                      __FUNCTION__);

            retVal = dcConfigVcomp(createConfig->vcompConfig, initParams);
        }
        if ((FVID2_SOK == retVal) && (NULL != createConfig->edeConfig))
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s : Configuring EDE",
                      __FUNCTION__);
            retVal = dcConfigEde(createConfig->edeConfig, initParams);
        }
        if ((FVID2_SOK == retVal) && (NULL != createConfig->cigMainConfig))
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s : Configuring MainCIG",
                      __FUNCTION__);
            retVal = dcConfigCigMain(createConfig->cigMainConfig, initParams);
        }
        if ((FVID2_SOK == retVal) && (NULL != createConfig->cprocConfig))
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s : Configuring MainCIG",
                      __FUNCTION__);
            retVal = dcConfigCproc(createConfig->cprocConfig, initParams);
        }
    }

    if ((FVID2_SOK == retVal) && (NULL != createConfig->cigPipConfig))
    {
        retVal = dcAllocResource(DC_NODE_CIG_1);

        if (FVID2_SOK == retVal)
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s : AUX Resource is allocated Configuring CIG AUX",
                      __FUNCTION__);

            isPipCigAllocated = TRUE;
            retVal = dcConfigCigAux(createConfig->cigPipConfig, initParams);
        }
    }

    if ((FVID2_SOK == retVal) && (NULL != createConfig->compConfig))
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s : Configuring COMP",
                  __FUNCTION__);

        /* Since only background color is modified here, we are
           not taking blender resource here */
        retVal = dcConfigComp(createConfig->compConfig, initParams);
    }

    if (FVID2_SOK != retVal)
    {
        if (TRUE == isVcompAllocated)
        {
            dcDeAllocResource(DC_NODE_VCOMP);
        }
        if (TRUE == isPipCigAllocated)
        {
            dcDeAllocResource(DC_NODE_CIG_1);
        }
    }

    return (retVal);
}



/**
 * \brief Function to get runtime configuration in the VCOMP module
 */
Int32 dcVcompGetRtConfig(Dc_InitParams *initParams,
                         Vps_DcVcompRtConfig *vcompRtConfig)
{
    Int32                          retVal = FVID2_EINVALID_PARAMS;
    VpsHal_VcompOutputDsplyConfig  displayConfig;
    VpsHal_Handle                  vcompHandle;

    GT_assert(DcTrace, (NULL != initParams));
    GT_assert(DcTrace, (NULL != vcompRtConfig));

    vcompHandle = initParams->halHandle[VDC_VCOMP_IDX];
    if ((NULL != vcompHandle) && (0 == dcAllocResource(DC_NODE_VCOMP)))
    {
        /* Get the outputDisplay Config from the VCOMP */
        retVal = VpsHal_vcompGetOutputDisplayConfig(
                    vcompHandle,
                    &displayConfig);

        if (0 == retVal)
        {
            if (VPSHAL_VCOMP_SOURCE_MAIN == displayConfig.selectVideoOntop)
            {
                vcompRtConfig->isPrimaryVideoOnTop = TRUE;
            }
            else
            {
                vcompRtConfig->isPrimaryVideoOnTop = FALSE;
            }
        }
    }
    return (retVal);
}



/**
 * \brief Function to set runtime configuration in the VCOMP module
 */
Int32 dcVcompSetRtConfig(Dc_InitParams *initParams,
                         Vps_DcVcompRtConfig *vcompRtConfig)
{
    Int32                          retVal = FVID2_EINVALID_PARAMS;
    VpsHal_VcompOutputDsplyConfig  displayConfig;
    VpsHal_Handle                  vcompHandle;

    GT_assert(DcTrace, (NULL != initParams));
    GT_assert(DcTrace, (NULL != vcompRtConfig));

    vcompHandle = initParams->halHandle[VDC_VCOMP_IDX];
    if ((NULL != vcompHandle) && (0 == dcAllocResource(DC_NODE_VCOMP)))
    {
        /* Get the outputDisplay Config from the VCOMP */
        retVal = VpsHal_vcompGetOutputDisplayConfig(
                    vcompHandle,
                    &displayConfig);

        if (FVID2_SOK == retVal)
        {
            if (TRUE == vcompRtConfig->isPrimaryVideoOnTop)
            {
                displayConfig.selectVideoOntop = VPSHAL_VCOMP_SOURCE_MAIN;
            }
            else
            {
                displayConfig.selectVideoOntop = VPSHAL_VCOMP_SOURCE_AUX;
            }
            /* Set the outputDisplay Config from the VCOMP */
            retVal = VpsHal_vcompSetOutputDisplayConfig(
                        vcompHandle,
                        &displayConfig,
                        NULL);
        }
    }
    return (retVal);
}



/**
 * \brief Function to set runtime configuration in the CIG module
 */
Int32 dcCigSetRtConfig(Dc_InitParams *initParams,
                       Vps_DcCigRtConfig *cigRtConfig)
{
    Int32                         retVal = FVID2_EINVALID_PARAMS;
    VpsHal_CigTransConfig         cigTransConfig;
    VpsHal_Handle                 cigHandle;

    GT_assert(DcTrace, (NULL != initParams));
    GT_assert(DcTrace, (NULL != cigRtConfig));

    cigHandle = initParams->halHandle[VDC_CIG_IDX];
    if (NULL != cigHandle)
    {
        /* TODO: Check to see if the node is alread allocated or not */
        switch (cigRtConfig->nodeId)
        {
            case VPS_DC_CIG_NON_CONSTRAINED_OUTPUT:
                /* Get the HDMI Transparency Config */
                retVal = VpsHal_cigGetHdmiTransConfig(
                            cigHandle,
                            &cigTransConfig);

                if (FVID2_SOK == retVal)
                {
                    /* Copy Config in the HAL Structure */
                    cigTransConfig.transparency = cigRtConfig->transparency;
                    cigTransConfig.mask =
                            (VpsHal_CigTransMask)cigRtConfig->mask;
                    cigTransConfig.alphaBlending = cigRtConfig->alphaBlending;
                    cigTransConfig.alphaValue = cigRtConfig->alphaValue;
                    cigTransConfig.rTransColor = cigRtConfig->transColor.r;
                    cigTransConfig.gTransColor = cigRtConfig->transColor.g;
                    cigTransConfig.bTransColor = cigRtConfig->transColor.b;
                    /* Get the HDMI Transparency Config */
                    retVal = VpsHal_cigSetHdmiTransConfig(
                                cigHandle,
                                &cigTransConfig,
                                NULL);
                }
                break;
            case VPS_DC_CIG_CONSTRAINED_OUTPUT:
                /* Get the HDMI Transparency Config */
                retVal = VpsHal_cigGetHdcompTransConfig(
                            cigHandle,
                            &cigTransConfig);

                if (FVID2_SOK == retVal)
                {
                    /* Copy Config in the HAL Structure */
                    cigTransConfig.transparency = cigRtConfig->transparency;
                    cigTransConfig.mask =
                            (VpsHal_CigTransMask)cigRtConfig->mask;
                    cigTransConfig.alphaBlending = cigRtConfig->alphaBlending;
                    cigTransConfig.alphaValue = cigRtConfig->alphaValue;
                    cigTransConfig.rTransColor = cigRtConfig->transColor.r;
                    cigTransConfig.gTransColor = cigRtConfig->transColor.g;
                    cigTransConfig.bTransColor = cigRtConfig->transColor.b;
                    /* Get the HDMI Transparency Config */
                    retVal = VpsHal_cigSetHdcompTransConfig(
                                cigHandle,
                                &cigTransConfig,
                                NULL);
                }
                break;
            case VPS_DC_CIG_PIP_OUTPUT:
                /* Get the HDMI Transparency Config */
                retVal = VpsHal_cigGetPipTransConfig(
                            cigHandle,
                            &cigTransConfig);

                if (FVID2_SOK == retVal)
                {
                    /* Copy Config in the HAL Structure */
                    cigTransConfig.transparency = cigRtConfig->transparency;
                    cigTransConfig.mask =
                            (VpsHal_CigTransMask)cigRtConfig->mask;
                    cigTransConfig.alphaBlending = cigRtConfig->alphaBlending;
                    cigTransConfig.alphaValue = cigRtConfig->alphaValue;
                    cigTransConfig.rTransColor = cigRtConfig->transColor.r;
                    cigTransConfig.gTransColor = cigRtConfig->transColor.g;
                    cigTransConfig.bTransColor = cigRtConfig->transColor.b;
                    /* Get the HDMI Transparency Config */
                    retVal = VpsHal_cigSetPipTransConfig(
                                cigHandle,
                                &cigTransConfig,
                                NULL);
                }
                break;
            default:
                retVal = FVID2_EINVALID_PARAMS;
                break;
        }
    }
    return (retVal);
}



/**
 * \brief Function to set runtime configuration in the CIG module
 */
Int32 dcCigGetRtConfig(Dc_InitParams *initParams,
                       Vps_DcCigRtConfig *cigRtConfig)
{
    Int32                         retVal = FVID2_EINVALID_PARAMS;
    VpsHal_CigTransConfig         cigTransConfig;
    VpsHal_Handle                 cigHandle;

    GT_assert(DcTrace, (NULL != initParams));
    GT_assert(DcTrace, (NULL != cigRtConfig));

    cigHandle = initParams->halHandle[VDC_CIG_IDX];
    if (NULL != cigHandle)
    {
        /* TODO: Check to see if the node is already allocated or not */
        switch (cigRtConfig->nodeId)
        {
            case VPS_DC_CIG_NON_CONSTRAINED_OUTPUT:
                /* Get the HDMI Transparency Config */
                retVal = VpsHal_cigGetHdmiTransConfig(
                            cigHandle,
                            &cigTransConfig);

                break;
            case VPS_DC_CIG_CONSTRAINED_OUTPUT:
                /* Get the HDMI Transparency Config */
                retVal = VpsHal_cigGetHdcompTransConfig(
                            cigHandle,
                            &cigTransConfig);
                break;
            case VPS_DC_CIG_PIP_OUTPUT:
                /* Get the HDMI Transparency Config */
                retVal = VpsHal_cigGetPipTransConfig(
                            cigHandle,
                            &cigTransConfig);
                break;
            default:
                retVal = -1;
                break;
        }
        if (FVID2_SOK == retVal)
        {
            /* Copy Config from the HAL Structure */
            cigRtConfig->transparency = cigTransConfig.transparency;
            cigRtConfig->mask = cigTransConfig.mask;
            cigRtConfig->alphaBlending = cigTransConfig.alphaBlending;
            cigRtConfig->alphaValue = cigTransConfig.alphaValue;
            cigRtConfig->transColor.r = cigTransConfig.rTransColor;
            cigRtConfig->transColor.g = cigTransConfig.gTransColor;
            cigRtConfig->transColor.b = cigTransConfig.bTransColor;
        }
    }
    return (retVal);
}



/**
 * \brief Function to set runtime configuration in the COMP module
 */
Int32 dcCompSetRtConfig(Dc_InitParams *initParams,
                        Vps_DcCompRtConfig *compRtConfig)
{
    Int32             retVal = FVID2_EINVALID_PARAMS;
    UInt32            cnt;
    VpsHal_CompConfig compConfig;
    VpsHal_Handle     compHandle;
    Dc_NodeInfo      *nodeInfo;
    VpsHal_CompId     compId;

    GT_assert(DcTrace, (NULL != initParams));
    GT_assert(DcTrace, (NULL != compRtConfig));

    compHandle = initParams->halHandle[VDC_COMP_IDX];
    if (NULL != compHandle)
    {
        /* Check to see if the node number is correct or not */
        if ((compRtConfig->nodeId < DC_NODE_HDMI_BLEND) ||
            (compRtConfig->nodeId > DC_NODE_SD_BLEND))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }

        nodeInfo = dcGetNodeInfo(compRtConfig->nodeId);
        GT_assert(DcTrace, (nodeInfo != NULL));

        /* Get the ID of hte Comp */
        compId = (VpsHal_CompId)(nodeInfo->priv);

        /* Get the Comp Config from the COMP */
        /* There is no problem in getting configuration even if
           resource is not available */
        retVal = VpsHal_compGetConfig(
                    compHandle,
                    &compConfig,
                    compId);

        if (FVID2_SOK == retVal)
        {
            /* Copy Parameters from Display Controller's structure to
             * comp structure */
            compConfig.fbPath =
                (VpsHal_CompFeedbkPathSelect)compRtConfig->fbPath;

            switch (compId)
            {
                /* Case for HDMI Comp */
                case VPSHAL_COMP_SELECT_HDMI :
                    if (TRUE == compRtConfig->isPipBaseVideo)
                    {
                        compConfig.videoInConfig.hdmiConfig.hdmiBase =
                                VPSHAL_COMP_HDMI_BLEND_BASE_HD_PIP;
                    }
                    else
                    {
                        compConfig.videoInConfig.hdmiConfig.hdmiBase =
                                VPSHAL_COMP_HDMI_BLEND_BASE_HD_VID;
                    }
                    break;

                /* Case for HDCOMP Comp */
                case VPSHAL_COMP_SELECT_HDCOMP :
                    if (TRUE == compRtConfig->isPipBaseVideo)
                    {
                        compConfig.videoInConfig.hdCompConfig.hdCompBase =
                                VPSHAL_COMP_HDCOMP_BLEND_BASE_HD_PIP;
                    }
                    else
                    {
                        compConfig.videoInConfig.hdCompConfig.hdCompBase =
                                VPSHAL_COMP_HDCOMP_BLEND_BASE_HD_CIT;
                    }
                    break;

                /* Case for DVO2 Comp*/
                case VPSHAL_COMP_SELECT_DVO2 :
                    if (TRUE == compRtConfig->isPipBaseVideo)
                    {
                        compConfig.videoInConfig.dvo2Config.dvo2Base =
                                VPSHAL_COMP_DVO2_BLEND_BASE_HD_PIP;
                    }
                    else
                    {
                        compConfig.videoInConfig.dvo2Config.dvo2Base =
                                VPSHAL_COMP_DVO2_BLEND_BASE_HD_VID;
                    }
                    break;

                /* case for SD COMP */
                case VPSHAL_COMP_SELECT_SD :
                    break;
            }

            if (TRUE == compRtConfig->isGlobalReorderEnable)
            {
                /* Enable Global Reorder */
                compConfig.gReorderMode = VPSHAL_COMP_GLOBAL_REORDER_ON;
            }
            else
            {
                /* Use Pixel Based Reordering */
                compConfig.gReorderMode = VPSHAL_COMP_GLOBAL_REORDER_OFF;
            }

            /* Copy the priority of all the input windows */
            for (cnt = 0u; cnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; cnt ++)
            {
                compConfig.displayOrder[cnt] =
                    (VpsHal_CompDisplayOrder)compRtConfig->displayOrder[cnt];
            }

            /* TODO: Check to see if resource is allocated or not */

            if (FVID2_SOK == retVal)
            {
                /* Set the configuration in the Comp */
                retVal = VpsHal_compSetConfig(
                            compHandle,
                            &compConfig,
                            compId,
                            NULL);
            }
        }
    }
    return (retVal);
}



/**
 * \brief Function to set runtime configuration in the COMP module
 */
Int32 dcCompGetRtConfig(Dc_InitParams *initParams,
                        Vps_DcCompRtConfig *compRtConfig)
{
    Int32             retVal = FVID2_EINVALID_PARAMS;
    UInt32            cnt;
    VpsHal_CompConfig compConfig;
    VpsHal_Handle     compHandle;
    Dc_NodeInfo      *nodeInfo;
    VpsHal_CompId     compId;

    GT_assert(DcTrace, (NULL != initParams));
    GT_assert(DcTrace, (NULL != compRtConfig));

    compHandle = initParams->halHandle[VDC_COMP_IDX];
    if (NULL != compHandle)
    {
        /* Check to see if the node number is correct or not */
        if ((compRtConfig->nodeId < DC_NODE_HDMI_BLEND) ||
            (compRtConfig->nodeId > DC_NODE_SD_BLEND))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }

        nodeInfo = dcGetNodeInfo(compRtConfig->nodeId);
        GT_assert(DcTrace, (nodeInfo != NULL));

        /* Get the ID of hte Comp */
        compId = (VpsHal_CompId)(nodeInfo->priv);

        /* Get the Comp Config from the COMP */
        /* There is no problem in getting configuration even if
           resource is not available */
        retVal = VpsHal_compGetConfig(
                    compHandle,
                    &compConfig,
                    compId);

        if (FVID2_SOK == retVal)
        {
            /* Copy Parameters from COMP HAL */
            compRtConfig->fbPath = compConfig.fbPath;

            switch (compId)
            {
                /* Case for HDMI Comp */
                case VPSHAL_COMP_SELECT_HDMI :
                    if (VPSHAL_COMP_HDMI_BLEND_BASE_HD_PIP ==
                            compConfig.videoInConfig.hdmiConfig.hdmiBase)
                    {
                        compRtConfig->isPipBaseVideo = TRUE;
                    }
                    else
                    {
                        compRtConfig->isPipBaseVideo = FALSE;
                    }
                    break;

                /* Case for HDCOMP Comp */
                case VPSHAL_COMP_SELECT_HDCOMP :
                    if (VPSHAL_COMP_HDCOMP_BLEND_BASE_HD_PIP ==
                            compConfig.videoInConfig.hdCompConfig.hdCompBase)
                    {
                        compRtConfig->isPipBaseVideo = TRUE;
                    }
                    else
                    {
                        compRtConfig->isPipBaseVideo = FALSE;
                    }
                    break;

                /* Case for DVO2 Comp*/
                case VPSHAL_COMP_SELECT_DVO2 :
                    if (VPSHAL_COMP_DVO2_BLEND_BASE_HD_PIP ==
                            compConfig.videoInConfig.dvo2Config.dvo2Base)
                    {
                        compRtConfig->isPipBaseVideo = TRUE;
                    }
                    else
                    {
                        compRtConfig->isPipBaseVideo = FALSE;
                    }
                    break;

                /* case for SD COMP */
                case VPSHAL_COMP_SELECT_SD :
                    break;
            }

            if (VPSHAL_COMP_GLOBAL_REORDER_ON == compConfig.gReorderMode)
            {
                compRtConfig->isGlobalReorderEnable = TRUE;
            }
            else
            {
                compRtConfig->isGlobalReorderEnable = FALSE;
            }

            /* Copy the priority of all the input windows */
            for (cnt = 0u; cnt < VPSHAL_COMP_DISPLAY_ORDER_MAX; cnt ++)
            {
                compRtConfig->displayOrder[cnt] = compConfig.displayOrder[cnt];
            }
        }
    }
    return (retVal);
}



/* Function to allocate resource for the given node number */
Int32 dcAllocResource(UInt32 nodeNum)
{
    Int32   retVal = FVID2_SOK;
    UInt32  resCnt, nodeCnt, numNodes;
    UInt32  found = FALSE;

    /* Traverse through all the resources used in the display controller */
    for (resCnt = 0u; resCnt < DC_MAX_RESOURCES; resCnt ++)
    {
        numNodes = DcResMngr[resCnt].numNodes;
        for (nodeCnt = 0u; nodeCnt < numNodes; nodeCnt ++)
        {
            if (nodeNum == DcResMngr[resCnt].nodes[nodeCnt])
            {
                /* Node is part of this resource */
                found = TRUE;
                break;
            }
        }
        if (TRUE == found)
        {
            break;
        }
    }

    /* If Node number is found and resource is not allocated,
       allocate the resource and set the flag */
    if (TRUE == found)
    {
        if (FALSE == DcResMngr[resCnt].isAllocated)
        {
            retVal = Vrm_allocResource(DcResMngr[resCnt].resource);
        }
        if (FVID2_SOK == retVal)
        {
            DcResMngr[resCnt].isAllocated = TRUE;
            DcResMngr[resCnt].useCnt ++;
        }
    }

    return (retVal);
}



/* Allocate the resources for multiple nodes */
Int32 dcAllocEdgeResource(const Vps_DcEdgeInfo *edgeInfo)
{
    Int32       retVal = FVID2_SOK;

    GT_assert(DcTrace, (NULL != edgeInfo));

    /* Check to see if resource is available for both the nodes */
    if (FVID2_SOK == dcAllocResource(edgeInfo->startNode))
    {
        if (FVID2_SOK != dcAllocResource(edgeInfo->endNode))
        {
            GT_2trace(DcTrace,
                      GT_ERR,
                      "%s: Resource allocate failed for %d node",
                      __FUNCTION__,
                      edgeInfo->endNode);
            /* Ignoring return valus as it will always succeed */
            dcDeAllocResource(edgeInfo->startNode);
            retVal = FVID2_EALLOC;
        }
    }
    else
    {
        GT_2trace(DcTrace,
                  GT_ERR,
                  "%s: Resource allocate failed for %d node",
                  __FUNCTION__,
                  edgeInfo->startNode);
        retVal = FVID2_EALLOC;
    }
    return (retVal);
}


/* Function to allocate resource for the given node number */
Int32 dcDeAllocResource(UInt32 nodeNum)
{
    Int32   retVal = FVID2_SOK;
    UInt32  cnt, nodeCnt, numNodes;
    UInt32  found = FALSE;

    /* Traverse through all the resources used in the display controller */
    for (cnt = 0u; cnt < DC_MAX_RESOURCES; cnt ++)
    {
        numNodes = DcResMngr[cnt].numNodes;
        for (nodeCnt = 0u; nodeCnt < numNodes; nodeCnt ++)
        {
            if (nodeNum == DcResMngr[cnt].nodes[nodeCnt])
            {
                /* Node is part of this resource */
                found = TRUE;
                break;
            }
        }
        if (TRUE == found)
        {
            break;
        }
    }

    /* If Node number is found and resource is allocated,
       free the resource and reset the flag */
    if (TRUE == found)
    {
        if ((TRUE == DcResMngr[cnt].isAllocated) &&
            (DcResMngr[cnt].useCnt > 0u))
        {
            DcResMngr[cnt].useCnt --;

            if (0u == DcResMngr[cnt].useCnt)
            {
                retVal = Vrm_releaseResource(DcResMngr[cnt].resource);

                if (FVID2_SOK == retVal)
                {
                    DcResMngr[cnt].isAllocated = FALSE;
                }
            }
        }
        else
        {
            /* Trying to free resource which is not allocated */
            retVal = FVID2_EALLOC;
        }
    }

    return (retVal);
}



/* Allocate the resources for multiple nodes */
Int32 dcDeAllocEdgeResource(const Vps_DcEdgeInfo *edgeInfo)
{
    Int32       retVal = FVID2_SOK;

    GT_assert(DcTrace, (NULL != edgeInfo));

    /* Check to see if resource is available for both the nodes */
    if (FVID2_SOK == dcDeAllocResource(edgeInfo->startNode))
    {
        if (FVID2_SOK != dcDeAllocResource(edgeInfo->endNode))
        {
            GT_2trace(DcTrace,
                      GT_ERR,
                      "%s: Resource de-allocate failed for %d node",
                      __FUNCTION__,
                      edgeInfo->endNode);
            /* Ignoring return valus as it will always succeed */
            dcAllocResource(edgeInfo->startNode);
            retVal = FVID2_EALLOC;
        }
    }
    else
    {
        GT_2trace(DcTrace,
                  GT_ERR,
                  "%s: Resource de-allocate failed for %d node",
                  __FUNCTION__,
                  edgeInfo->startNode);
        retVal = FVID2_EALLOC;
    }
    return (retVal);
}



Int32 dcVcompEnableInput(Vps_DcNodeInput *nodeInfo, Ptr ovlyPtr)
{
    Int32                   retVal = FVID2_EINVALID_PARAMS;
    Dc_InitParams          *initParams = &(dcGetDctrlObj()->initParams);
    Dc_NodeInfo            *node = NULL, *parentNode;
    VpsHal_VcompInputVideo  inputVideo;
    VpsHal_Handle           vcompHandle;

    GT_assert(DcTrace, (NULL != nodeInfo));

    /* Note: If the input is not enabled, data will not flow, so
       if the handle is not available here, return error */
    vcompHandle = initParams->halHandle[VDC_VCOMP_IDX];
    if (NULL != vcompHandle)
    {
        node = dcGetNodeInfo(nodeInfo->nodeId);
        GT_assert(DcTrace, (NULL != node));
        parentNode = dcGetNodeInfo(nodeInfo->inputId);
        GT_assert(DcTrace, (NULL != parentNode));

        if (DC_NODE_PRI_MUX == parentNode->nodeNum)
        {
            inputVideo = VPSHAL_VCOMP_SOURCE_MAIN;
        }
        else
        {
            inputVideo = VPSHAL_VCOMP_SOURCE_AUX;
        }

        /* Enable/Disable appropriate input path in the VCOMP overlay */
        retVal = VpsHal_vcompEnableVideoInputPath(
                    vcompHandle,
                    inputVideo,
                    nodeInfo->isEnable,
                    ovlyPtr);
    }

    return (retVal);
}



static Int32 dcConfigVcomp(Vps_DcVcompConfig *vConfig,
                           Dc_InitParams *initParams)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    VpsHal_VcompConfig  vcompConfig;
    VpsHal_Handle       vcompHandle;

    GT_assert(DcTrace, (NULL != vConfig));
    GT_assert(DcTrace, (NULL != initParams));

    vcompHandle = initParams->halHandle[VDC_VCOMP_IDX];
    /* Note: Since This configuration is from application, if the handle
       is not available, return error */
    /* Set the Vcomp Static Configuration */
    if (NULL != vcompHandle)
    {
        retVal = VpsHal_vcompGetConfig(vcompHandle, &vcompConfig);
        if (FVID2_SOK == retVal)
        {
            vcompConfig.dsplyConfig.bckGrndColor.yLuma =
                            vConfig->bckGrndColor.yLuma;
            vcompConfig.dsplyConfig.bckGrndColor.cbChroma =
                            vConfig->bckGrndColor.cbChroma;
            vcompConfig.dsplyConfig.bckGrndColor.crChroma =
                            vConfig->bckGrndColor.crChroma;

            vcompConfig.mainVideo.altColor.yLuma =
                            vConfig->mainAltColor.yLuma;
            vcompConfig.mainVideo.altColor.cbChroma =
                            vConfig->mainAltColor.cbChroma;
            vcompConfig.mainVideo.altColor.crChroma =
                            vConfig->mainAltColor.crChroma;

            vcompConfig.auxVideo.altColor.yLuma =
                            vConfig->auxAltColor.yLuma;
            vcompConfig.auxVideo.altColor.cbChroma =
                            vConfig->auxAltColor.cbChroma;
            vcompConfig.auxVideo.altColor.crChroma =
                            vConfig->auxAltColor.crChroma;

            /* Set the configuration in the VCOMP */
            retVal = VpsHal_vcompSetConfig(vcompHandle, &vcompConfig, NULL);
        }
    }

    return (retVal);
}



/* Function to configure EDE Open Config */
static Int32 dcConfigEde(Vps_DcEdeConfig *eConfig,
                         Dc_InitParams *initParams)
{
#ifdef TI_816X_BUILD
    Int32               retVal = FVID2_EINVALID_PARAMS;
    VpsHal_EdeConfig    edeConfig;
    VpsHal_Handle       edeHandle;

    GT_assert(DcTrace, (NULL != eConfig));
    GT_assert(DcTrace, (NULL != initParams));

    edeHandle = initParams->halHandle[VDC_EDE_IDX];
    /* Set the Ede Static Configuration */
    if (NULL != edeHandle)
    {
        /* Get the default Configuration */
        retVal = VpsHal_edeGetConfig(edeHandle, &edeConfig);
        if (FVID2_SOK == retVal)
        {
            edeConfig.ltiEnable = eConfig->ltiEnable;
            edeConfig.horzPeaking = eConfig->horzPeaking;
            edeConfig.ctiEnable = eConfig->ctiEnable;
            edeConfig.transAdjustEnable = eConfig->transAdjustEnable;
            edeConfig.lumaPeaking = eConfig->lumaPeaking;
            edeConfig.chromaPeaking = eConfig->chromaPeaking;
            edeConfig.minClipLuma = eConfig->minClipLuma;
            edeConfig.maxClipLuma = eConfig->maxClipLuma;
            edeConfig.minClipChroma = eConfig->minClipChroma;
            edeConfig.maxClipChroma = eConfig->maxClipChroma;
            edeConfig.bypass = eConfig->bypass;
        }

        /* Set the configuration in the EDE */
        retVal = VpsHal_edeSetConfig(edeHandle, &edeConfig, NULL);
    }

    return (retVal);
#else
    return (FVID2_SOK);
#endif
}



/* Function to configure CIG Main Open Config */
static Int32 dcConfigCigMain(Vps_DcCigMainConfig *cigMainConfig,
                             Dc_InitParams *initParams)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    UInt32              cookie;
    VpsHal_CigConfig    cigConfig;
    Dc_NodeInfo        *node = NULL;
    VpsHal_Handle       cigHandle;

    GT_assert(DcTrace, (NULL != cigMainConfig));
    GT_assert(DcTrace, (NULL != initParams));

    cigHandle = initParams->halHandle[VDC_CIG_IDX];
    /* Set the CIG Static Configuration */
    if (NULL != cigHandle)
    {
        /* Since configuration for Main path and PIP path is in a
           single register and main path and PIP could be used in display as
           well as mem2mem driver, Read and write to CIG register
           should be atomic. */
        cookie = Hwi_disable();

        /* Get the default Configuration */
        retVal = VpsHal_cigGetConfig(cigHandle, &cigConfig);
        if (FVID2_SOK == retVal)
        {
            cigConfig.enableCig = cigMainConfig->enableContraining;
            cigConfig.nonCtrInterlace = cigMainConfig->nonCtrInterlace;
            cigConfig.ctrInterlace = cigMainConfig->ctrInterlace;
            cigConfig.vertDecimation = cigMainConfig->enableContraining;

            /* Set the configuration in the CIG */
            retVal = VpsHal_cigSetConfig(cigHandle, &cigConfig, NULL);
            if (FVID2_SOK == retVal)
            {
                node = dcGetNodeInfo(VPS_DC_CIG_NON_CONSTRAINED_OUTPUT);
                GT_assert(DcTrace, (NULL != node));

                /* If the constraining is enabled, set the flag in the
                 * CIG nodes as the private data */
                if (TRUE == cigMainConfig->nonCtrInterlace)
                {
                    /* For the nonconstrained output, enable it in the
                     * node number 17 */
                    node->priv = DC_INTERLACING_ENABLED;
                }
                else
                {
                    node->priv = 0;
                }

                node = dcGetNodeInfo(VPS_DC_CIG_CONSTRAINED_OUTPUT);
                GT_assert(DcTrace, (NULL != node));

                if (TRUE == cigMainConfig->ctrInterlace)
                {
                    /* For the constrained output, enable it in the
                     * node number 15 */
                    node->priv = DC_INTERLACING_ENABLED;
                }
                else
                {
                    node->priv = 0;
                }
            }
        }
        Hwi_restore(cookie);
    }

    return (retVal);
}



/* Function to configure CIG Aux Open Config */
static Int32 dcConfigCigAux(Vps_DcCigPipConfig *cigPipConfig,
                            Dc_InitParams *initParams)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    UInt32              cookie;
    VpsHal_CigConfig    cigConfig;
    Dc_NodeInfo        *node = NULL;
    VpsHal_Handle       cigHandle;

    GT_assert(DcTrace, (NULL != cigPipConfig));
    GT_assert(DcTrace, (NULL != initParams));

    cigHandle = initParams->halHandle[VDC_CIG_IDX];
    /* Set the CIG Static Configuration */
    if (NULL != cigHandle)
    {
        /* Since configuration for Main path and PIP path is in a
           single register and main path and PIP could be used in display as
           well as mem2mem driver, Read and write to CIG register
           should be atomic. */
        cookie = Hwi_disable();

        /* Get the default Configuration */
        retVal = VpsHal_cigGetConfig(cigHandle, &cigConfig);
        if (FVID2_SOK == retVal)
        {
            cigConfig.pipInterlace = cigPipConfig->pipInterlace;
            /* Set the configuration in the CIG */
            retVal = VpsHal_cigSetConfig(
                        cigHandle,
                        &cigConfig,
                        NULL);
            if (FVID2_SOK == retVal)
            {
                node = dcGetNodeInfo(VPS_DC_CIG_PIP_OUTPUT);
                GT_assert(DcTrace, (NULL != node));

                if (TRUE == cigPipConfig->pipInterlace)
                {
                    /* For the pip output, enable it in the
                     * node number 18 */
                    node->priv = DC_INTERLACING_ENABLED;
                }
                else
                {
                    node->priv = 0;
                }
            }
        }
        Hwi_restore(cookie);
    }

    return (retVal);
}



static Int32 dcConfigComp(Vps_DcCompConfig *compConfig,
                          Dc_InitParams *initParams)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    UInt32              bkColor;
    VpsHal_Handle       compHandle;

    GT_assert(DcTrace, (NULL != compConfig));
    GT_assert(DcTrace, (NULL != initParams));

    compHandle = initParams->halHandle[VDC_COMP_IDX];
    /* Set the COMP Static Configuration */
    if (NULL != compHandle)
    {
        bkColor = (compConfig->bckGrndColor.r) |
                  (compConfig->bckGrndColor.g << 10u) |
                  (compConfig->bckGrndColor.b << 20u);
        /* Set the background Color in Comp */
        retVal = VpsHal_compSetBackGrColor(compHandle, bkColor);
    }

    return (retVal);
}



static Int32 dcConfigCproc(Vps_CprocConfig *cproc,
                           Dc_InitParams *initParams)
{
#ifdef TI_816X_BUILD
    Int32               retVal = FVID2_EINVALID_PARAMS;
    VpsHal_Handle       cprocHandle;

    GT_assert(DcTrace, (NULL != cproc));
    GT_assert(DcTrace, (NULL != initParams));

    cprocHandle = initParams->halHandle[VDC_CPROC_IDX];
    /* Set the COMP Static Configuration */
    if (NULL != cprocHandle)
    {
        retVal = VpsHal_cprocSetConfig(
                    cprocHandle,
                    cproc,
                    NULL);
    }

    return (retVal);
#else
    return (FVID2_SOK);
#endif
}
