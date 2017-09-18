/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 * \file vps_dctrl.c
 *
 * \brief Display Controller
 * This file implements Display controller
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/BIOS.h>
#include <ti/psp/vps/common/trace.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/drivers/display/vps_dlm.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/drivers/fvid2_drvMgr.h>

#include <ti/psp/vps/vps_displayCtrl.h>
#include <ti/psp/vps/drivers/display/vps_dctrlInt.h>
#include <ti/psp/vps/hal/vpshal_sdvenc.h>
#include <ti/psp/vps/hal/vpshal_hdvenc.h>
#include <ti/psp/vps/hal/vpshal_cig.h>
#include <ti/psp/vps/hal/vpshal_comp.h>
#include <ti/psp/vps/hal/vpshal_csc.h>
#include <ti/psp/vps/hal/vpshal_ede.h>
#include <ti/psp/vps/hal/vpshal_vcomp.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlPriv.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlTraversePriv.h>
#include <ti/psp/vps/drivers/display/src/vps_dctrlHalPriv.h>
#include <ti/psp/platforms/vps_platform.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define DC_ALL_VENCS    (VPS_DC_VENC_HDMI | VPS_DC_VENC_HDCOMP |               \
                         VPS_DC_VENC_DVO2 | VPS_DC_VENC_SD)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */


/* ========================================================================== */
/*                     LOCAL FUNCTION DECLARATION                             */
/* ========================================================================== */

static Fdrv_Handle Vps_dcCreate(UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs,
                                const FVID2_DrvCbParams *fdmCbParams);
static Int32 Vps_dcDelete(Fdrv_Handle handle, Ptr deleteArgs);
static Int32 Vps_dcControl(Fdrv_Handle handle,
                           UInt32 cmd,
                           Ptr cmdArgs,
                           Ptr cmdStatusArgs);
static Int32 dcSetConfig(Dc_DisplayCtrlInfo *dCtrlInfo,
                         Vps_DcConfig *dcConfig);
static Int32 dcClearConfig(Dc_DisplayCtrlInfo *dCtrlInfo,
                           Vps_DcConfig *dcConfig);
static Int32 dcEnumerateNodes(Dc_DisplayCtrlInfo *dCtrlInfo,
                              Vps_DcEnumNode *nodeInfo);
static Int32 dcEnumerateNodeInput(Dc_DisplayCtrlInfo *dCtrlInfo,
                                  Vps_DcEnumNodeInput *nodeInput);
static Int32 dcCtrlNodeInput(Dc_DisplayCtrlInfo *dCtrlInfo,
                             Vps_DcNodeInput    *nodeInput);
static Int32 dcNodeInputGetStatus(Dc_DisplayCtrlInfo *dCtrlInfo,
                                  Vps_DcNodeInput    *nodeInput);
static Int32 dcSetVencMode(Dc_DisplayCtrlInfo *dCtrlInfo,
                           Vps_DcVencInfo *vencInfo);
static Int32 dcGetVencMode(Dc_DisplayCtrlInfo *dCtrlInfo,
                           Vps_DcVencInfo *vencInfo);
static Int32 dcSetVencOutput(Dc_DisplayCtrlInfo *dCtrlInfo,
                             Vps_DcOutputInfo *outputInfo);
static Int32 dcGetVencOutput(Dc_DisplayCtrlInfo *dCtrlInfo,
                             Vps_DcOutputInfo *outputInfo);
static Int32 dcDisableVenc(Dc_DisplayCtrlInfo *dCtrlInfo, UInt32 vencs);
static Int32 dcSetVencControl(Dc_DisplayCtrlInfo *dCtrlInfo,
                              Vps_DcVencControl *bCtrl);
static Int32 dcGetVencControl(Dc_DisplayCtrlInfo *dCtrlInfo,
                              Vps_DcVencControl *bCtrl);

static Void  dcCompErrCallback(const UInt32 *event,
                               UInt32 numEvents,
                               Ptr arg);
static Int32 dcFormMesh(Dc_DisplayCtrlInfo *dCtrlInfo,
                        const Vps_DcEdgeInfo *edgeInfo,
                        UInt32 numEdges,
                        UInt32 isEnabled);
static Int32 dcSetVencClkSrc(Dc_DisplayCtrlInfo *dcCtrlInfo,
                             Vps_DcVencClkSrc *clkSrc);
static Int32 dcGetVencClkSrc(Dc_DisplayCtrlInfo *dcCtrlInfo,
                             Vps_DcVencClkSrc *clkSrc);

/* Creates static table. Called at the init time only */
static Void dcCreateNodeTable();
static Dc_NodeInfo *dcGetEnumNode(UInt32 nodeIdx);

/* Function to configure venc and start tied venc at the same time. It also
 * connect Blender to the venc and starts blender. It also sets the mode
 * information all other modules. */
static Int32 dcConfigVencMode(Vps_DcVencInfo *vencInfo);
/* This function checks to see if any of the input node of the given node
 * is started. If it is started, it returns true otherwise false. This logic
 * is need for a given node at multiple places so creating a separate
 * function */
static UInt32 dcIsInputNodeStarted(UInt32 nodeNum);

/* Function used to control venc and/or its paired on-chip encoders.
 * For now, noe venc specific commands. Only paired on-chip ecoder commands are
 * supported.
 */
static Int32 dcVencCmd(UInt32 cmd, Vps_DcOnchipEncoderCmd *encCmd);

Void PrintNodeTable();

/* ========================================================================== */
/*                        Global Variables                                    */
/* ========================================================================== */

/** Display driver function pointer. */
static const FVID2_DrvOps DCtrlOps =
{
    FVID2_VPS_DCTRL_DRV,        /* Driver ID */
    Vps_dcCreate,               /* Create */
    Vps_dcDelete,               /* Delete */
    Vps_dcControl,              /* Control */
    NULL,                       /* Queue */
    NULL,                       /* Dequeue */
    NULL,                       /* ProcessFrames */
    NULL                        /* GetProcessedFrames */
} ;

/* Predefined cnfiguration */
static Vps_DcConfig DcPreDefConfig[VPS_DC_NUM_USECASE - 1u] = DC_PREDEF_CONFIGS;

static Dc_NodeInfo DcNodes[DC_NUM_NODES] = DC_NODEINFO_DEFAULTS;

static Dc_InputNodeInfo DcInputNode[DC_MAX_INPUT] = DC_INPUTNODE_DEFAULTS;

static Dc_OutputNodeInfo DcOutputNode[DC_MAX_OUTPUT];

static Dc_DisplayCtrlInfo DcCtrlInfo;

/* ========================================================================== */
/*                        FUNCTION DEFINITIONS                                */
/* ========================================================================== */

Int32 Dc_init(Dc_InitParams *initParams, Ptr arg)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cnt;
    Semaphore_Params        semParams1, semParams2;
    UInt32                  errEvent;

    GT_assert(DcTrace, (NULL != initParams));

    VpsUtils_memset(&DcCtrlInfo, 0u, sizeof(DcCtrlInfo));

    VpsUtils_memcpy(&(DcCtrlInfo.initParams),
                    initParams,
                    sizeof(Dc_InitParams));

    /* Assign parent pointers in input and output Nodes */
    for (cnt = 0u; cnt < DC_MAX_INPUT; cnt ++)
    {
        DcInputNode[cnt].node->parent = (Ptr)(&DcInputNode[cnt]);
        DcInputNode[cnt].dcCtrlInfo = &(DcCtrlInfo);
    }
    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt ++)
    {
        DcOutputNode[cnt].node = &DcNodes[initParams->outNode[cnt].nodeNum];

        DcOutputNode[cnt].node->parent = (Ptr)(&DcOutputNode[cnt]);
        DcOutputNode[cnt].nodeId = initParams->outNode[cnt].vencId;
        DcOutputNode[cnt].vencHandle = initParams->outNode[cnt].vencHalHandle;
        DcOutputNode[cnt].listNum = 0;
        DcOutputNode[cnt].isListAllocated = FALSE;
        DcOutputNode[cnt].clkcModule = initParams->outNode[cnt].clkcModule;
        DcOutputNode[cnt].lmFidNum = 0;
        DcOutputNode[cnt].freeClientNum = VPSHAL_VPDMA_CHANNEL_INVALID;
        DcOutputNode[cnt].isVencStarted = FALSE;
        DcOutputNode[cnt].vencOutPixClk = initParams->outNode[cnt].vencOutPixClk;
        DcOutputNode[cnt].vencClkSrc = VPS_DC_CLKSRC_MAX;

        if (VPS_DC_VENC_SD == DcOutputNode[cnt].nodeId)
        {
            DcOutputNode[cnt].setMode = VpsHal_sdVencSetMode;
            DcOutputNode[cnt].getMode = VpsHal_sdVencGetMode;
            DcOutputNode[cnt].startVenc = VpsHal_sdvencStartVenc;
            DcOutputNode[cnt].stopVenc = VpsHal_sdvencStopVenc;
            DcOutputNode[cnt].controlVenc = VpsHal_sdvencIoctl;
            DcOutputNode[cnt].setOutput = VpsHal_sdvencSetOutput;
            DcOutputNode[cnt].getOutput = VpsHal_sdvencGetOutput;
            DcOutputNode[cnt].resetVenc = VpsHal_sdvencResetVenc;
        }
        else
        {
            DcOutputNode[cnt].setMode = VpsHal_hdVencSetMode;
            DcOutputNode[cnt].getMode = VpsHal_hdVencGetMode;
            DcOutputNode[cnt].startVenc = VpsHal_hdvencStartVenc;
            DcOutputNode[cnt].stopVenc = VpsHal_hdvencStopVenc;
            DcOutputNode[cnt].controlVenc = VpsHal_hdvencIoctl;
            DcOutputNode[cnt].setOutput = VpsHal_hdVencSetOutput;
            DcOutputNode[cnt].getOutput = VpsHal_hdVencGetOutput;
            DcOutputNode[cnt].resetVenc = VpsHal_hdvencResetVenc;
        }
    }

    /* Initialize Semaphore */
    Semaphore_Params_init(&semParams1);
    /* This has to be binary semaphore */
    semParams1.mode = Semaphore_Mode_BINARY;
    DcCtrlInfo.isrWaitSem = Semaphore_create(1u, &semParams1, NULL);
    if (NULL == DcCtrlInfo.isrWaitSem)
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: semCreate failed for isrsemwait",
                  __FUNCTION__);
        retVal = FVID2_EALLOC;
    }
    Semaphore_pend(DcCtrlInfo.isrWaitSem, DC_SEM_TIMEOUT);

    Semaphore_Params_init(&semParams2);
    DcCtrlInfo.sem = Semaphore_create(1u, &semParams2, NULL);
    if (NULL == DcCtrlInfo.sem)
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: semCreate failed for sem",
                  __FUNCTION__);
        retVal = FVID2_EALLOC;
    }

    for (cnt = 0u; cnt < VPS_DC_CSC_MAX; cnt ++)
    {
        DcCtrlInfo.cscCfg[cnt].bypass = FALSE;
        DcCtrlInfo.cscCfg[cnt].coeff  = NULL;
        DcCtrlInfo.cscCfg[cnt].mode = VPS_CSC_MODE_HDTV_GRAPHICS_Y2R; //VPS_CSC_MODE_SDTV_GRAPHICS_Y2R;
    }

    GT_assert(DcTrace, (initParams->numClkSrc <= DC_MAX_OUTPUT));
    for (cnt = 0u; cnt < initParams->numClkSrc; cnt ++)
    {
        retVal |= dcSetVencClkSrc(&DcCtrlInfo, &initParams->clkSrc[cnt]);
    }

    /* Creates the configuration overlays used by display controller to
     * enable/display specific path in VCOMP or in BLEND */
    if (FVID2_SOK == retVal)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Creating Configuration Overlay",
                  __FUNCTION__);
        retVal = dcIntInit(&DcCtrlInfo);
    }

    if (FVID2_SOK == retVal)
    {
        errEvent = VEM_EE_COMP;
        /* Register Error Callback for the Comp Errors */
        DcCtrlInfo.vemHandle = Vem_register(VEM_EG_ERROR,
                                            &errEvent,
                                            1u,
                                            VEM_PRIORITY0,
                                            dcCompErrCallback,
                                            NULL);
        if (NULL == DcCtrlInfo.vemHandle)
        {
            GT_1trace(DcTrace,
                      GT_ERR,
                      "%s: Cannot register error callback for COMP",
                      __FUNCTION__);
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Create and Initialize VPS Node Table",
                  __FUNCTION__);

        /* Create and Initialize VPS Node Table */
        dcCreateNodeTable();
    }
    else
    {
        /* There is some error, so remove descriptors from DLM
         * and unregister it from DLM */
        if (NULL != DcCtrlInfo.dlmClientCtrlHandle)
        {
            /* Remove Descriptors from Dlm */
            VpsDlm_clrDescMem(DcCtrlInfo.dlmClientCtrlHandle);
            /* Unregister from DLM */
            VpsDlm_unRegister(DcCtrlInfo.dlmClientCtrlHandle);
        }
        if(NULL != DcCtrlInfo.isrWaitSem)
        {
            /* Delete Semaphore */
            Semaphore_delete(&(DcCtrlInfo.isrWaitSem));
        }
        if(NULL != DcCtrlInfo.sem)
        {
            /* Delete Semaphore */
            Semaphore_delete(&(DcCtrlInfo.sem));
        }

        for (cnt = 0u; cnt < DC_RT_CONFIG_NUM_IDX; cnt ++)
        {
            if (NULL != DcCtrlInfo.rtDlmHandle[cnt])
            {
                /* UnRegister DLM Runtime clients */
                VpsDlm_unRegister(DcCtrlInfo.rtDlmHandle[cnt]);
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Enabling clock for COMP",
                  __FUNCTION__);

        VpsHal_vpsClkcModuleEnable(VPSHAL_VPS_CLKC_COMP, TRUE);
    }

    if (FVID2_SOK == retVal)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Registering to FVID2",
                  __FUNCTION__);

        /* Store pre-defined configuration */
        DcCtrlInfo.predefConfig = DcPreDefConfig;

        /* Register the driver to the driver manager */
        retVal = FVID2_registerDriver(&DCtrlOps);
    }

    return (retVal);
}



Int32 Dc_deInit(Ptr arg)
{
    Int32 retVal;

    if(NULL != DcCtrlInfo.isrWaitSem)
    {
        /* Delete Semaphore */
        Semaphore_delete(&(DcCtrlInfo.isrWaitSem));
    }
    if(NULL != DcCtrlInfo.sem)
    {
        /* Delete Semaphore */
        Semaphore_delete(&(DcCtrlInfo.sem));
    }

    dcIntDeInit(&DcCtrlInfo);

    VpsHal_vpsClkcModuleEnable(VPSHAL_VPS_CLKC_COMP, FALSE);

    if (NULL != DcCtrlInfo.vemHandle)
    {
        Vem_unRegister(DcCtrlInfo.vemHandle);
    }

    VpsUtils_memset(&(DcCtrlInfo.initParams),
                    0,
                    sizeof(Dc_InitParams));

    DcCtrlInfo.predefConfig = NULL;

    retVal = FVID2_unRegisterDriver(&DCtrlOps);

    return (retVal);
}



static Fdrv_Handle Vps_dcCreate(UInt32                  drvId,
                                UInt32                  instanceId,
                                Ptr                     createArgs,
                                Ptr                     createStatusArgs,
                                const FVID2_DrvCbParams *fdmCbParams)
{
    Int32               retVal = FVID2_SOK;
    Fdrv_Handle         handle = NULL;
    Dc_InitParams      *initParams = &(DcCtrlInfo.initParams);
    Vps_DcCreateConfig *openConfig = NULL;

    Semaphore_pend(DcCtrlInfo.sem, DC_SEM_TIMEOUT);

    if (NULL == createStatusArgs)
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: BADARGS: NULL pointer",
                  __FUNCTION__);

        retVal = FVID2_EBADARGS;
    }

    openConfig = (Vps_DcCreateConfig *)createArgs;
    if (NULL != openConfig)
    {
        /* Resource is allocated so configure HALs */
        retVal = dcConfigHal(openConfig, initParams);

        /* Copy CSC configs if provided */
        if (FVID2_SOK == retVal)
        {
            if (NULL != openConfig->sdCscConfig)
            {
                VpsUtils_memcpy(
                    &DcCtrlInfo.cscCfg[VPS_DC_SD_CSC],
                    openConfig->sdCscConfig,
                    sizeof(Vps_CscConfig));
            }

            if (NULL != openConfig->hdcompCscConfig)
            {
                VpsUtils_memcpy(
                    &DcCtrlInfo.cscCfg[VPS_DC_HDCOMP_CSC],
                    openConfig->hdcompCscConfig,
                    sizeof(Vps_CscConfig));
            }

            if (NULL != openConfig->vcompCscConfig)
            {
#if defined(TI_814X_BUILD) || defined(TI_8107_BUILD)
                VpsUtils_memcpy(
                    &DcCtrlInfo.cscCfg[VPS_DC_VCOMP_CSC],
                    openConfig->vcompCscConfig,
                    sizeof(Vps_CscConfig));
#endif
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        DcCtrlInfo.openCnt ++;
        handle = (Fdrv_Handle)(&DcCtrlInfo);
    }

    /* Return the error if possible */
    if (createStatusArgs)
    {
        *((Int32 *) createStatusArgs) = retVal;
    }

    Semaphore_post(DcCtrlInfo.sem);
    return (handle);
}


static Int32 Vps_dcDelete(Fdrv_Handle handle, Ptr deleteArgs)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_DisplayCtrlInfo *dcCtrlInfo = (Dc_DisplayCtrlInfo *)handle;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));

    Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);

    if (dcCtrlInfo->openCnt > 0u)
    {
        dcCtrlInfo->openCnt --;

        if (0 == dcCtrlInfo->openCnt)
        {
            GT_1trace(DcTrace,
                      GT_INFO,
                      "%s: FREE Resources",
                      __FUNCTION__);

            /* Release the Resources */
            dcDeAllocResource(DC_NODE_VCOMP);
            dcDeAllocResource(DC_NODE_CIG_1);
            dcDeAllocResource(DC_NODE_HDMI_BLEND);
#if defined(TI_816X_BUILD) || defined(TI_8107_BUILD)
            dcDeAllocResource(DC_NODE_HDCOMP_BLEND);       //TBD
#endif
            dcDeAllocResource(DC_NODE_DVO2_BLEND);
            dcDeAllocResource(DC_NODE_SD_BLEND);
        }

        retVal = FVID2_SOK;
    }

    Semaphore_post(dcCtrlInfo->sem);
    return (retVal);
}



static Int32 Vps_dcControl(Fdrv_Handle  handle,
                           UInt32       cmd,
                           Ptr          cmdArgs,
                           Ptr          cmdStatusArgs)
{
    Int32               retVal = FVID2_EINVALID_PARAMS;
    Dc_DisplayCtrlInfo *dcCtrlInfo = (Dc_DisplayCtrlInfo *)handle;
    Dc_InitParams      *initParams = NULL;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));

    if (NULL != cmdArgs)
    {
        initParams = &(dcCtrlInfo->initParams);

        switch (cmd)
        {
            /* Case is to set DC Config */
            case IOCTL_VPS_DCTRL_SET_CONFIG:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcSetConfig(dcCtrlInfo, (Vps_DcConfig *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to clear DC Config */
            case IOCTL_VPS_DCTRL_CLEAR_CONFIG:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcClearConfig(dcCtrlInfo, (Vps_DcConfig *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to Enumerate Nodes */
            case IOCTL_VPS_DCTRL_ENUM_NODES:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcEnumerateNodes(
                            dcCtrlInfo,
                            (Vps_DcEnumNode *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to Enumerate Node's inputs */
            case IOCTL_VPS_DCTRL_ENUM_NODE_INPUTS:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcEnumerateNodeInput(
                            dcCtrlInfo,
                            (Vps_DcEnumNodeInput *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to Enable/Disable node's input */
            case IOCTL_VPS_DCTRL_NODE_INPUT:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcCtrlNodeInput(
                            dcCtrlInfo,
                            (Vps_DcNodeInput *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to get the status Node's input */
            case IOCTL_VPS_DCTRL_GET_NODE_INPUT_STATUS:
                retVal = dcNodeInputGetStatus(
                            dcCtrlInfo,
                            (Vps_DcNodeInput *)cmdArgs);
                break;

            /* Case is to set Venc Mode */
            case IOCTL_VPS_DCTRL_SET_VENC_MODE:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcSetVencMode(
                            dcCtrlInfo,
                            (Vps_DcVencInfo *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to Get Venc Mode */
            case IOCTL_VPS_DCTRL_GET_VENC_MODE:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcGetVencMode(dcCtrlInfo, (Vps_DcVencInfo *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to set VENC Output */
            case IOCTL_VPS_DCTRL_SET_VENC_OUTPUT:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcSetVencOutput(
                            dcCtrlInfo,
                            (Vps_DcOutputInfo *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to get Venc Output */
            case IOCTL_VPS_DCTRL_GET_VENC_OUTPUT:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcGetVencOutput(
                            dcCtrlInfo,
                            (Vps_DcOutputInfo *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to disable Venc */
            case IOCTL_VPS_DCTRL_DISABLE_VENC:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                if (0u == *(UInt32 *)cmdArgs)
                {
                    retVal = FVID2_EINVALID_PARAMS;
                }
                else
                {
                    retVal = dcDisableVenc(dcCtrlInfo, *(UInt32 *)cmdArgs);
                }
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to set VENC brightness */
            case IOCTL_VPS_DCTRL_SET_VENC_CONTROL:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcSetVencControl(
                            dcCtrlInfo,
                            (Vps_DcVencControl *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to get VENC brightness */
            case IOCTL_VPS_DCTRL_GET_VENC_CONTROL:
                /* Get semaphore */
                Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                retVal = dcGetVencControl(
                            dcCtrlInfo,
                            (Vps_DcVencControl *)cmdArgs);
                /* Release semaphore */
                Semaphore_post(dcCtrlInfo->sem);
                break;

            /* Case is to set VCOMP RT config */
            case IOCTL_VPS_DCTRL_SET_VCOMP_RTCONFIG:
                retVal = dcVcompSetRtConfig(
                            initParams,
                            (Vps_DcVcompRtConfig *)cmdArgs);
                break;

            /* Case is to get VCOMP RT config */
            case IOCTL_VPS_DCTRL_GET_VCOMP_RTCONFIG:
                retVal = dcVcompGetRtConfig(
                            initParams,
                            (Vps_DcVcompRtConfig *)cmdArgs);
                break;

            /* Case is to set COMP RT config */
            case IOCTL_VPS_DCTRL_SET_COMP_RTCONFIG:
                retVal = dcCompSetRtConfig(
                            initParams,
                            (Vps_DcCompRtConfig *)cmdArgs);
                break;

            /* Case is to get COMP RT config */
            case IOCTL_VPS_DCTRL_GET_COMP_RTCONFIG:
                retVal = dcCompGetRtConfig(
                            initParams,
                            (Vps_DcCompRtConfig *)cmdArgs);
                break;

            /* Case is to set CIG RT config */
            case IOCTL_VPS_DCTRL_SET_CIG_RTCONFIG:
                retVal = dcCigSetRtConfig(
                            initParams,
                            (Vps_DcCigRtConfig *)cmdArgs);
                break;

            /* Case is to get CIG RT config */
            case IOCTL_VPS_DCTRL_GET_CIG_RTCONFIG:
                retVal = dcCigGetRtConfig(
                            initParams,
                            (Vps_DcCigRtConfig *)cmdArgs);
                break;

            /* Case is to set clock source for clk1x input */
            case IOCTL_VPS_DCTRL_SET_VENC_CLK_SRC:
                retVal = dcSetVencClkSrc(
                            dcCtrlInfo,
                            (Vps_DcVencClkSrc *)cmdArgs);
                break;

            /* Case is to set clock source for clk1x input */
            case IOCTL_VPS_DCTRL_GET_VENC_CLK_SRC:
                retVal = dcGetVencClkSrc(
                            dcCtrlInfo,
                            (Vps_DcVencClkSrc *)cmdArgs);
                break;

            /*
             * Check if the IOCTL is intended for an on-chip encoder,
             * If so, let the paired venc handle it.
             * Else let the apps know that this is an un-supported command
             */
            default:
                retVal = FVID2_EUNSUPPORTED_CMD;
                if (cmd > VPS_DCTRL_IOCTL_ADV_MAX)
                {
                    retVal = FVID2_EBADARGS;
                    if (cmdArgs != NULL)
                    {
                        /*
                         * Ensure exclusive access, we do not want mutiple threads
                         * trying to configure/use on-chip encoders at the same
                         * time
                         */
                        Semaphore_pend(dcCtrlInfo->sem, DC_SEM_TIMEOUT);
                        retVal = dcVencCmd(cmd,
                                           (Vps_DcOnchipEncoderCmd *)cmdArgs);
                        Semaphore_post(dcCtrlInfo->sem);
                    }
                }
                break;
        }
    }

    return (retVal);
}



/**
 *  dcSetConfig
 *  \brief Function to set the entire VPS display path configuration in
 *  one shot. This function takes either name of the use case and
 *  configures entire display
 *  path or takes list of edges connecting nodes and configures display paths.
 *  It first validates these paths and then configures VPS for the display
 *  paths. It configures all the center modules, except blender. Blender will
 *  only be enabled when any one of its input is enabled.
 *
 *  \param dcCtrlInfo   Pointer to display controller object
 *  \param dcConfig     Pointer to config containing configuration
 *                      It also contains use case is to be configured for.
 *                      Display Controller provides set of standard
 *                      configuration
 *                      for some standard use cases. Application can
 *                      directly If it is standard use case, there is no need
 *                      to specify other params. Application can also specify
 *                      user defined path configuration by specifying
 *                      VPS_DC_USERSETTINGS in this argument and providing list of
 *                      edges
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcSetConfig(Dc_DisplayCtrlInfo     *dcCtrlInfo,
                         Vps_DcConfig           *dcConfig)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    UInt32              vencs;
    UInt32              isMeshCreated = 0;
    Vps_DcVencInfo     *vencInfo;
    Vps_PlatformId     platform;

    if (VPS_DC_USERSETTINGS != dcConfig->useCase)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Using PreDefined UseCase",
                  __FUNCTION__);

        /* If predefined config is used, get the pointer to it */
        dcConfig = &(dcCtrlInfo->predefConfig[dcConfig->useCase]);
    }

    /* Enable appropriate input/output paths in the node as per the provided
     * list of edges. This function will just enables flags available in the
     * list of input and output of a node. It does not check for the error
     * like multiplexer cannot have multiple inputs. It is assumed here that
     * such errors are not available in the list of edges and list of edges
     * are correct in all respect. */
    retVal = dcFormMesh(
                dcCtrlInfo,
                dcConfig->edgeInfo,
                dcConfig->numEdges,
                TRUE);
    if (FVID2_SOK == retVal)
    {
        isMeshCreated = 1u;
    }

    vencInfo = &(dcConfig->vencInfo);
    if (VPS_DC_MAX_VENC < vencInfo->numVencs)
    {
        GT_2trace(DcTrace,
                  GT_ERR,
                  "%s: Wrong number of vencs",
                  __FUNCTION__,
                  vencInfo->numVencs);
        /* Number of vencs are more than supported */
        retVal = FVID2_EINVALID_PARAMS;
    }

    platform = Vps_platformGetId();
    if ((platform <= VPS_PLATFORM_ID_UNKNOWN) ||
        (platform >= VPS_PLATFORM_ID_MAX))
    {
        GT_assert (GT_DEFAULT_MASK, FALSE);
    }

    vencs = 0u;
    for (cnt = 0u; (cnt < vencInfo->numVencs) && (FVID2_SOK == retVal); cnt++)
    {
        if (0u == vencInfo->modeInfo[cnt].vencId)
        {
            /* Venc ID provided is zero */
            retVal = FVID2_EINVALID_PARAMS;
            break;
        }

        /* Check to see if same venc id is used in multiple
           modeInfo instances */
        if (vencs & vencInfo->modeInfo[cnt].vencId)
        {
            retVal = FVID2_EINVALID_PARAMS;
            break;
        }

        if (vencInfo->tiedVencs == vencInfo->modeInfo[cnt].vencId)
        {
            /* Single VENC cannot be tied with anyone */
            retVal = FVID2_EINVALID_PARAMS;
            break;
        }

        if (VPS_PLATFORM_ID_EVM_TI8107 == platform)
        {
            if ((VPS_DC_VENC_HDCOMP == vencInfo->modeInfo[cnt].vencId) &&
                ((0u == vencInfo->tiedVencs) || (vencInfo->numVencs < 2u)))
            {
                retVal = FVID2_EINVALID_PARAMS;
                break;
            }
        }

        vencs |= vencInfo->modeInfo[cnt].vencId;
    }

    if (FVID2_SOK == retVal)
    {
        /* If two vencs are to be tied, they should be available in the
           modeinfo list */
        if ((0 != vencInfo->tiedVencs) &&
            (vencInfo->tiedVencs != (vencInfo->tiedVencs & vencs)))
        {
            retVal = FVID2_EINVALID_PARAMS;
        }

        /* Configure the Venc and start it */
        if(FVID2_SOK == retVal)
        {
            retVal = dcConfigVencMode(&(dcConfig->vencInfo));
        }
    }

    if (FVID2_SOK != retVal)
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: Error Occured so Clearing out configured paths",
                  __FUNCTION__);
        if (isMeshCreated)
        {
            /* ignoring return value as retVal should be returned */
            dcFormMesh(
                dcCtrlInfo,
                dcConfig->edgeInfo,
                dcConfig->numEdges,
                FALSE);
        }
    }

    return (retVal);
}



/**
 *  dcClearConfig
 *  \brief Function to clear the VPS display path configuration in
 *  one shot. This function takes either name of the use case and
 *  disables the path connecting vps modules for this use case
 *  or takes list of edges connecting nodes and disables the path
 *  between these nodes.
 *  It does not validates the edge list. It simply disables the edge
 *  connecting nodes. For the vencs, it checks for the validity and then
 *  disables the venc if there are not errors.
 *
 *  \param dcCtrlInfo   Pointer to display controller object
 *  \param dcConfig     Pointer to config containing configuration
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcClearConfig(Dc_DisplayCtrlInfo   *dcCtrlInfo,
                           Vps_DcConfig         *dcConfig)
{
    Int32               retVal =FVID2_SOK;
    UInt32              cnt, vencs;
    Vps_DcVencInfo     *vencInfo = NULL;
    Dc_OutputNodeInfo  *outputNode = NULL;

    if (VPS_DC_USERSETTINGS != dcConfig->useCase)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Using PreDefined UseCase",
                  __FUNCTION__);

        /* If predefined config is used, get the pointer to it */
        dcConfig = &(dcCtrlInfo->predefConfig[dcConfig->useCase]);
    }

     vencInfo = &dcConfig->vencInfo;
     if (VPS_DC_MAX_VENC < vencInfo->numVencs)
     {
         GT_2trace(DcTrace,
                   GT_ERR,
                   "%s: Wrong number of vencs",
                   __FUNCTION__,
                   vencInfo->numVencs);
         /* Number of vencs are more than supported */
         retVal = FVID2_EINVALID_PARAMS;
     }

     vencs = 0u;
     for (cnt = 0u; (cnt < vencInfo->numVencs) && (FVID2_SOK == retVal); cnt++)
     {
         if (0u == vencInfo->modeInfo[cnt].vencId)
         {
             Vps_printf("VencId zero\n");
             /* Venc ID provided is zero */
             retVal = FVID2_EINVALID_PARAMS;
             break;
         }

         /* Check to see if same venc id is used in multiple
            modeInfo instances */
         if (vencs & vencInfo->modeInfo[cnt].vencId)
         {
             Vps_printf("multiple times venc ID\n");
             /* VencID is getting repeated */
             retVal = FVID2_EINVALID_PARAMS;
             break;
         }

         vencs |= vencInfo->modeInfo[cnt].vencId;
    }

    for (cnt = 0u; (cnt < DC_MAX_OUTPUT) && (FVID2_SOK == retVal); cnt++)
    {
        outputNode = &(DcOutputNode[cnt]);

        if (outputNode->nodeId & vencs)
        {
            if (TRUE == outputNode->isVencStarted)
            {
                if (0u != VpsDlm_getNumActiveClients(outputNode->listNum))
                {
                    /* There is no need to disable this VENC as some client
                       is already running */
                    vencs &= ~(outputNode->nodeId);
                }
            }
            else
            {
                 Vps_printf("VENC is not started\n");
                 /* VencID is getting repeated */
                 retVal = FVID2_EINVALID_PARAMS;
                 break;
            }
        }
    }

    if ((FVID2_SOK == retVal) && (0u != vencs))
    {
        /* Disable the Venc. It is sure here that none of the input
           path are running on these vencs */
        retVal = dcDisableVenc(dcCtrlInfo, vencs);
    }

    if (FVID2_SOK == retVal)
    {
        GT_1trace(DcTrace,
                  GT_INFO,
                  "%s: Clearing out the paths",
                  __FUNCTION__);

        dcFormMesh(dcCtrlInfo, dcConfig->edgeInfo, dcConfig->numEdges, FALSE);
    }

    return (retVal);
}



/**
 *  dcEnumerateNodes
 *  \brief Function to enumerate VPS nodes and its information for the
 *  node available at the given index. This function is used for dynamically
 *  creating VPS mesh. Enumeration starts from the node number 0 and
 *  continues untill function returns -1. On each index, this function
 *  returns node and its information like type of node, name, number of
 *  inputs and number output available at given index.
 *
 *  \param handle       Display Controller Handle
 *  \param nodeInfo     Pointer to structure in which node information will be
 *                      stored
 *  \return             0 on success, -1 on error
 */
static Int32 dcEnumerateNodes(Dc_DisplayCtrlInfo    *dcCtrlInfo,
                              Vps_DcEnumNode        *nodeInfo)
{
    Int32            retVal = FVID2_EINVALID_PARAMS;
    Dc_NodeInfo     *node = NULL;

    /* Get the enum node for the given index */
    node = dcGetEnumNode(nodeInfo->nodeIdx);

    if (NULL != node)
    {
        /* Copy information about the node in nodeInfo pointer */
        nodeInfo->nodeType = node->nodeType;
        nodeInfo->nodeId = node->nodeNum;
        VpsUtils_memcpy(nodeInfo->nodeName,
                        node->nodeName,
                        sizeof(node->nodeName));
        nodeInfo->numInputs = node->input.numNodes;
        nodeInfo->numOutputs = node->output.numNodes;
        retVal = FVID2_SOK;
    }

    return (retVal);
}



/**
 *  dcEnumerateNodeInput
 *  \brief Function to enumerate nodes connected as a inputs to this node.
 *  It enumerates inputs nodes of this node. To enumerate all inputs,
 *  applications shall begin with input index zero, get the information,
 *  increment by one until the driver returns -1.
 *
 *  \param handle       Display Controller Handle
 *  \param nodeIdx      Node Index for which input is to be enumerated.
 *  \param inputIdx     Input Index starting from 0 to maximum number of inputs.
 *                      After max number of inputs, this function returns zero.
 *  \param inputName    Name of the input
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcEnumerateNodeInput(Dc_DisplayCtrlInfo    *dcCtrlInfo,
                                  Vps_DcEnumNodeInput   *nodeInput)
{
    Int32            retVal = FVID2_EINVALID_PARAMS;
    Dc_NodeInfo     *node = NULL;

    node = dcGetNodeInfo(nodeInput->nodeId);

    if ((NULL != node) && (nodeInput->inputIdx < node->input.numNodes))
    {
        /* Input is found, so copy name of the input path from the
         * input node */
        node = node->input.node[nodeInput->inputIdx];
        VpsUtils_memcpy(nodeInput->inputName,
                        node->nodeName,
                        sizeof(node->nodeName));
        nodeInput->inputId = node->nodeNum;
        retVal = FVID2_SOK;
    }

    return (retVal);
}



/**
 *  dcCtrlNodeInput
 *  \brief Function to enable/disable the given input on the given node
 *
 *  This function enables/disable input at the given
 *  index on the given node. It enables/disables given input
 *  as inputs node to this node and enables/disables given node
 *  as output node to the parent node i.e. it enables/disables edge connecting
 *  given node and input node.
 *
 *  \param handle       Display Controller Handle
 *  \param nodeInput    structure containing nodeId and input id
 *                      and flag indicating whether to enable or
 *                      disable node input
 *  \return             0 on success, -1 on error
 */
static Int32 dcCtrlNodeInput(Dc_DisplayCtrlInfo       *dcCtrlInfo,
                             Vps_DcNodeInput          *nodeInput)
{
    Int32                retVal = FVID2_SOK;
    UInt32               cnt, inputIdx;
    Dc_NodeInfo         *node = NULL, *inNode = NULL/*, *tempNode = NULL*/;
    UInt32               found = FALSE;
    Vps_DcEdgeInfo       edgeInfo;

    node = dcGetNodeInfo(nodeInput->nodeId);
    GT_assert(DcTrace, (NULL != node));

    GT_3trace(DcTrace,
              GT_INFO,
              "%s: Enabling input %d on Node %d",
              __FUNCTION__,
              nodeInput->inputId,
              nodeInput->nodeId);

    /* Only non-dummy nodes have features of enabling inputs. Other
       nodes have inputs, which are always enabled */
    if (TRUE == node->isDummy)
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: Input cannot be enabled on Dummy node",
                  __FUNCTION__);
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the index of the input ID for given node */
        for (cnt = 0u; cnt < node->input.numNodes; cnt ++)
        {
            inNode = node->input.node[cnt];
            if (inNode->nodeNum == nodeInput->inputId)
            {
                inputIdx = cnt;
                found = TRUE;
                break;
            }
        }

        /* Incorrect Input ID */
        if (FALSE == found)
        {
            GT_1trace(DcTrace,
                      GT_ERR,
                      "%s: Wrong Input ID for this node",
                      __FUNCTION__);
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Input path should not be on on the non-comp node */
        if (VPS_DC_NODETYPE_COMP != node->nodeType)
        {
            if (TRUE == dcIsInputNodeStarted(node->nodeNum))
            {
                GT_1trace(DcTrace,
                          GT_ERR,
                          "%s: Streaming is on on the start node",
                          __FUNCTION__);
                retVal = FVID2_EDEVICE_INUSE;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Input path should not be on on the non-comp node */
        if (VPS_DC_NODETYPE_COMP != inNode->nodeType)
        {
            if (TRUE == dcIsInputNodeStarted(inNode->nodeNum))
            {
                GT_1trace(DcTrace,
                          GT_ERR,
                          "%s: Streaming is on on the end node",
                          __FUNCTION__);
                retVal = FVID2_EDEVICE_INUSE;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        edgeInfo.startNode = inNode->nodeNum;
        edgeInfo.endNode = node->nodeNum;

        if (TRUE == nodeInput->isEnable)
        {
            retVal = dcAllocEdgeResource(&edgeInfo);
        }
        else
        {
            retVal = dcDeAllocEdgeResource(&edgeInfo);
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* If this is multiplxer, only one input should be enabled */
        if (VPS_DC_NODETYPE_MUX == node->nodeType)
        {
#if 0
            found = FALSE;
            for (cnt = 0u; cnt < node->input.numNodes; cnt ++)
            {
                /* Disable all inputs */
                if (TRUE == node->input.isEnabled[cnt])
                {
                    tempNode = node->input.node[cnt];
                    found = TRUE;
                    break;
                }
            }
            if (TRUE == found)
            {
                /* Disconnect input node's output to this node */
                for (cnt = 0u; cnt < tempNode->output.numNodes; cnt ++)
                {
                    if (tempNode->output.node[cnt] == node)
                    {
                        tempNode->output.isEnabled[cnt] = FALSE;
                        break;
                    }
                }
                node->input.isEnabled[cnt] = FALSE;
            }
#endif

            /* Enable input at the index inputIdx */
            node->input.isEnabled[inputIdx] = nodeInput->isEnable;
        }
        else /* Not multiplexer, so can enable multiple input */
        {
             /* Enable input at the index inputIdx */
             node->input.isEnabled[inputIdx] = nodeInput->isEnable;
        }
        /* Enable output in the parent Node */
        for (cnt = 0u; cnt < inNode->output.numNodes; cnt ++)
        {
            /* enumerated node is same as the output node, enable
             * that output */
            if (node == inNode->output.node[cnt])
            {
                inNode->output.isEnabled[cnt] = nodeInput->isEnable;

                break;
            }
        }
    }
    return (retVal);
}



/**
 *  dcNodeInputGetStatus
 *  \brief Function to get the status of the nodeinput
 *
 *  \param handle       Display Controller Handle
 *  \param nodeIdx      Node Index
 *  \param inputIdx     Input Index
 *  \return             0 on success, -1 on error
 */
static Int32 dcNodeInputGetStatus(Dc_DisplayCtrlInfo      *dcCtrlInfo,
                                  Vps_DcNodeInput         *nodeInput)
{
    Int32                retVal = FVID2_SOK;
    UInt32               cnt, inputIdx;
    Dc_NodeInfo         *node = NULL;
    UInt32               found = FALSE;

    node = dcGetNodeInfo(nodeInput->nodeId);
    GT_assert(DcTrace, (NULL != node));

    GT_3trace(DcTrace,
              GT_INFO,
              "%s: GetStatus input %d on Node %d",
              __FUNCTION__,
              nodeInput->inputId,
              nodeInput->nodeId);

    /* Only non-dummy nodes have features of enabling inputs. Other
       nodes have inputs, which are always enabled */
    if (TRUE == node->isDummy)
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: Input cannot be disabled on Dummy node",
                  __FUNCTION__);
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the index of the input ID for given node */
        for (cnt = 0u; cnt < node->input.numNodes; cnt ++)
        {
            if (node->input.node[cnt]->nodeNum == nodeInput->inputId)
            {
                inputIdx = cnt;
                found = TRUE;
                break;
            }
        }

        /* Incorrect Input ID */
        if (FALSE == found)
        {
            GT_1trace(DcTrace,
                      GT_ERR,
                      "%s: Wrong Input ID for this node",
                      __FUNCTION__);
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Assuming if input is enabled output is also enabled */
        nodeInput->isEnable = node->input.isEnabled[inputIdx];
    }

    return (retVal);
}



/**
 *  dcSetVencMode
 *  \brief Function to set mode/standard in the given Venc if its inputs
 *  are not running. If the multiple venc is tied to this venc, then
 *  it sets mode in all the vencs if their inputs are not running.
 *  Otherwise it returns error.
 *
 *  \param handle       Display Controller Handle
 *  \param vencNodeNum  VENC Node Number. How to get Venc Node number.
 *  \param modeName     Name of the mode to be set
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcSetVencMode(Dc_DisplayCtrlInfo *dcCtrlInfo,
                           Vps_DcVencInfo *vencInfo)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  cnt, vencs;

     if (VPS_DC_MAX_VENC < vencInfo->numVencs)
     {
         GT_2trace(DcTrace,
                   GT_ERR,
                   "%s: Wrong number of vencs",
                   __FUNCTION__,
                   vencInfo->numVencs);
         /* Number of vencs are more than supported */
         retVal = FVID2_EINVALID_PARAMS;
     }

     vencs = 0u;
     for (cnt = 0u; (cnt < vencInfo->numVencs) && (FVID2_SOK == retVal); cnt++)
     {
         if (0u == vencInfo->modeInfo[cnt].vencId)
         {
             /* Venc ID provided is zero */
             retVal = FVID2_EINVALID_PARAMS;
             break;
         }

         /* Check to see if same venc id is used in multiple
            modeInfo instances */
         if (vencs & vencInfo->modeInfo[cnt].vencId)
         {
             retVal = FVID2_EINVALID_PARAMS;
             break;
         }

         if (vencInfo->tiedVencs == vencInfo->modeInfo[cnt].vencId)
         {
             /* Single VENC cannot be tied with anyone */
             retVal = FVID2_EINVALID_PARAMS;
             break;
         }

         vencs |= vencInfo->modeInfo[cnt].vencId;
     }

     /* If two vencs are to be tied, they should be available in the
        modeinfo list */
     if ((0 != vencInfo->tiedVencs) &&
         (vencInfo->tiedVencs != (vencInfo->tiedVencs & vencs)))
     {
         retVal = FVID2_EINVALID_PARAMS;
     }

     if (FVID2_SOK == retVal)
     {
         /* Configure the Venc and start it */
         retVal = dcConfigVencMode(vencInfo);
     }

    return (retVal);
}



/**
 *  dcGetVencMode
 *  \brief Function to get current mode/standard set in the given Venc.
 *
 *  \param handle       Display Controller Handle
 *  \param vencNodeNum  VENC Node Number. How to get Venc Node number.
 *  \param modeName     Name of the mode currently set in the Venc
 *  \
 *  \return             0 on success, -1 on error
 */
static Int32 dcGetVencMode(Dc_DisplayCtrlInfo   *dcCtrlInfo,
                           Vps_DcVencInfo       *vencInfo)
{
    Int32               retVal = FVID2_SOK;
    UInt32              isVencFound = 0u;
    UInt32              cnt, cnt2, numStartNodes;
    UInt32              startNodes[DC_MAX_INPUT];
    Dc_OutputNodeInfo  *outputNode = NULL;

    /* Get the output info structure instance for this Venc */
    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt ++)
    {
        outputNode = &(DcOutputNode[cnt]);

        for (cnt2 = 0u; cnt2 < vencInfo->numVencs; cnt2++)
        {
            if (outputNode->nodeId == vencInfo->modeInfo[cnt2].vencId)
            {
                VpsUtils_memcpy(&(vencInfo->modeInfo[cnt2]),
                                &(outputNode->modeInfo),
                                sizeof(Vps_DcModeInfo));

                retVal |= dcGetLeafNodes(
                            outputNode->node,
                            startNodes,
                            &numStartNodes,
                            FALSE);

                if (FVID2_SOK == retVal)
                {
                    vencInfo->modeInfo[cnt2].isVencRunning =
                        outputNode->isVencStarted;
                    vencInfo->modeInfo[cnt2].numInPath = numStartNodes;
                }
                isVencFound = 1u;
            }
        }
    }

    if (!isVencFound)
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    return (retVal);
}



/**
 *  dcSetVencOutput
 *  \brief Function to set output in the given Venc if its inputs are not
 *  running. For the tied vencs, venc has to be stopped first, then output can
 *  can be changed.
 *
 *  \param handle       Display Controller Handle
 *  \param vencNodeNum  VENC Node Number. How to get Venc Node number?
 *  \param outputName   Name of output to be set
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcSetVencOutput(Dc_DisplayCtrlInfo     *dcCtrlInfo,
                             Vps_DcOutputInfo       *outputInfo)
{
    Int32                retVal = FVID2_SOK;
    UInt32               cnt;
    Dc_OutputNodeInfo   *outputNode = NULL;

    /* Get the output info structure instance for this Venc */
    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt ++)
    {
        outputNode = &(DcOutputNode[cnt]);
        if (outputInfo->vencNodeNum & outputNode->nodeId)
        {
            break;
        }
    }
    if (NULL != outputNode->setOutput)
    {
        retVal = outputNode->setOutput(
                    outputNode->vencHandle,
                    outputInfo);
    }

    return (retVal);
}



/**
 *  dcGetVencOutput
 *  \brief Function to get output in the given Venc.
 *
 *  \param handle       Display Controller Handle
 *  \param vencNodeNum  VENC Node Number. How to get Venc Node number?
 *  \param outputName   Name of output to be set
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcGetVencOutput(Dc_DisplayCtrlInfo     *dcCtrlInfo,
                             Vps_DcOutputInfo       *outputInfo)
{
    Int32               retVal = FVID2_SOK;
    UInt32              cnt;
    Dc_OutputNodeInfo  *outputNode = NULL;

    /* Get the output info structure instance for this Venc */
    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt ++)
    {
        outputNode = &(DcOutputNode[cnt]);
        if (outputInfo->vencNodeNum == outputNode->nodeId)
        {
            break;
        }
    }
    if (NULL != outputNode->getOutput)
    {
        outputNode->getOutput(
                        outputNode->vencHandle,
                        outputInfo);

        /* Restore the VENC ID as it could be overwritten by VENC HAL */
        outputInfo->vencNodeNum = outputNode->nodeId;
    }

    return (retVal);
}


/**
 *  dcSetVencControl
 *  \brief Function to set VENC control.
 *
 *  \param handle       Display Controller Handle
 *  \param bCtrl        Brightness control params
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcSetVencControl(Dc_DisplayCtrlInfo *dCtrlInfo,
                                 Vps_DcVencControl *bCtrl)
{
    Int32                retVal = FVID2_SOK;
    UInt32               cnt;
    Dc_OutputNodeInfo   *outputNode = NULL;

    /* Get the output info structure instance for this Venc */
    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt++)
    {
        outputNode = &(DcOutputNode[cnt]);
        if (bCtrl->vencNodeNum & outputNode->nodeId)
        {
            break;
        }
    }

    if (outputNode->controlVenc != NULL)
    {
        retVal = outputNode->controlVenc(
                     outputNode->vencHandle,
                     IOCTL_VPS_DCTRL_SET_VENC_CONTROL,
                     bCtrl,
                     NULL);
    }

    return (retVal);
}

/**
 *  dcGetVencControl
 *  \brief Function to get VENC control.
 *
 *  \param handle       Display Controller Handle
 *  \param bCtrl        Brightness control params
 *
 *  \return             0 on success, -1 on error
 */
static Int32 dcGetVencControl(Dc_DisplayCtrlInfo *dCtrlInfo,
                              Vps_DcVencControl *bCtrl)
{
    Int32                retVal = FVID2_SOK;
    UInt32               cnt;
    Dc_OutputNodeInfo   *outputNode = NULL;

    /* Get the output info structure instance for this Venc */
    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt++)
    {
        outputNode = &(DcOutputNode[cnt]);
        if (bCtrl->vencNodeNum & outputNode->nodeId)
        {
            break;
        }
    }

    if (outputNode->controlVenc != NULL)
    {
        retVal = outputNode->controlVenc(
                     outputNode->vencHandle,
                     IOCTL_VPS_DCTRL_GET_VENC_CONTROL,
                     bCtrl,
                     NULL);
    }

    return (retVal);
}

/**
 * \brief Function to stop the venc. It stops all the vencs, whose bitmask
 *  is passed as the argument and which are already running. If only few
 *  vencs out of passed vencs are running, it will stop running vencs.
 *
 *  If any error comes while stopping any venc, it does not restart
 *  already stopped vencs.
 */
static Int32 dcDisableVenc(Dc_DisplayCtrlInfo *dcCtrlInfo, UInt32 vencs)
{
    Int32                retVal = FVID2_SOK;
    UInt32               outNodeCnt;
    Dc_OutputNodeInfo   *outputNode = NULL;
    VpsHal_CompId        compId;
    Dc_InitParams       *initParams = NULL;
    Dc_NodeInfo         *vencNode = NULL, *blendNode = NULL;
    Vps_DcEdgeInfo       edgeInfo;

    /** Follow below steps for each venc in given bitmask
     *  1, If input connected to this venc is on, return error
     *  2, Disable Comp Output
     *  3, Stop Venc
     *  4, Stop the clk for the Venc in CLKC module
     *  5, Deallocate venc and comp resources
     *  6, Disable venc clk from the common clkc register
     *  7, If this venc is tied with other vencs, remove it from
     *     diplay controller object
     *  8, If there is only tied venc in display controller object, set
     *     it to zero
     */
    initParams = &(dcCtrlInfo->initParams);

    if (0u != (~(DC_ALL_VENCS) & vencs))
    {
        /* Venc ID should be one of the supported one */
        retVal = FVID2_EINVALID_PARAMS;
    }

    /* Check to see if input node is still pumping in the data to
       any of these vencs, return error if it is */
    for (outNodeCnt = 0u; (outNodeCnt < DC_MAX_OUTPUT) &&
                          (FVID2_SOK == retVal); outNodeCnt++)
    {
        outputNode = &(DcOutputNode[outNodeCnt]);

        if ((outputNode->nodeId & vencs) &&
            (TRUE == outputNode->isVencStarted))
        {
            /* Output node found for this venc and Venc is still running */

            /* Check to see if the input is already running on the
               input node for this VENC */
            GT_assert(DcTrace, (NULL != outputNode->node));
            if (TRUE == dcIsInputNodeStarted(outputNode->node->nodeNum))
            {
                GT_1trace(DcTrace,
                          GT_ERR,
                          "%s: Streaming is on on the node",
                          __FUNCTION__);
                retVal = FVID2_EDEVICE_INUSE;
                break;
            }
        }
    }

    for (outNodeCnt = 0u; (outNodeCnt < DC_MAX_OUTPUT) && (FVID2_SOK == retVal);
                outNodeCnt++)
    {
        outputNode = &(DcOutputNode[outNodeCnt]);

        if ((outputNode->nodeId & vencs) &&
            (TRUE == outputNode->isVencStarted))
        {
            /* Get the nodes for the blender and VENC. Assuming here
               that venc has only Blender as the input node */
            vencNode = outputNode->node;
            blendNode = vencNode->input.node[0];

            /* Stop the Blender first otherwise after stopping VENC,
               it might give overflow error */
            compId = (VpsHal_CompId)blendNode->priv;
            retVal = VpsHal_compDisableOutput(
                        initParams->halHandle[VDC_COMP_IDX],
                        compId);

            /* First stop the venc as it will stop pumping out the
               data as well as stop requesting data to the upper
               modules */
            /* This will always succeed as it is already started,
               COMP and clock can be disabled */
            /* This step is as such not required as disabling
               venc at CLKC module will disable timing generator */
            retVal |= outputNode->stopVenc(outputNode->vencHandle);

            /* Stop output pixel clock. Not for the SDVENC */
            if (VPS_DC_VENC_SD != outputNode->nodeId)
            {
                retVal |= VpsHal_vpsVencOutputPixClkEnable(
                            outputNode->vencOutPixClk,
                            FALSE);
            }

            /* Disable Blender output and Venc nodes input */
            vencNode->input.isEnabled[0u] = FALSE;
            blendNode->output.isEnabled[0u] = FALSE;

            /* Disable Clock for this VENC in the CLKC module */
            retVal |= VpsHal_vpsClkcModuleEnable(
                        outputNode->clkcModule,
                        FALSE);

            /* De-Allocate the resources */
            edgeInfo.startNode = blendNode->nodeNum;
            edgeInfo.endNode = vencNode->nodeNum;
            retVal |= dcDeAllocEdgeResource(&edgeInfo);

            /* Disable Main Clock Source. This step will only fail if
               there is wrong vencId */
            retVal |= VpsHal_vpsClkcVencEnable(outputNode->nodeId, FALSE);

            if (outputNode->nodeId & dcCtrlInfo->tiedVencs)
            {
                /* This venc is tied with the other venc, so remove
                   this tying as it is disabled */
                dcCtrlInfo->tiedVencs &= ~(outputNode->nodeId);
            }

            if (FVID2_SOK == retVal)
            {
                outputNode->isVencStarted = FALSE;
            }
        }

        /* If there is only venc set in the display controller, set
           it to zero */
        if ((dcCtrlInfo->tiedVencs & ~(outputNode->nodeId)) ==
                outputNode->nodeId)
        {
            dcCtrlInfo->tiedVencs = 0u;
        }
    }

    return (retVal);
}



/** \brief Function to return enumerated node given index. There are
 *  some dummy nodes in the nodeinfo structure. These dummy nodes need
 *  not be enumerated. This function skips dummy nodes and returns next
 *  non-dummy node for the given index.
 */
static Dc_NodeInfo *dcGetEnumNode(UInt32 nodeIdx)
{
    Dc_NodeInfo *node = NULL;
    UInt32 enumCnt, cnt;

    enumCnt = 0;
    for (cnt = 0; cnt < DC_NUM_NODES; cnt ++)
    {
        node = dcGetNodeInfo(cnt);
        GT_assert(DcTrace, (NULL != node));

        /* Skip the dummy nodes and allow to enumerate actual nodes */
        if (FALSE == node->isDummy)
        {
            if (enumCnt == nodeIdx)
            {
                /* node to be enumerate is same as this one so break the loop
                 * and return this node. */
                break;
            }
            else /* move to next node for enumeration */
            {
                enumCnt ++;
            }
        }
    }
    if (cnt == DC_NUM_NODES)
    {
        node = NULL;
    }

    return (node);
}



/** \brief This function creates complete topology of DSS by
 *  enabling/disabling edges. To enable/disable an edge, it enables/disables
 *  output of source node and enables/disables input of the target node.
 */
static Int32 dcFormMesh(Dc_DisplayCtrlInfo     *dCtrlInfo,
                        const Vps_DcEdgeInfo   *edgeInfo,
                        UInt32                  numEdges,
                        UInt32                  isEnabled)
{
    Int32 ret = 0;
    UInt32 cnt, lastEdge = 0u;
    Vps_DcNodeInput nodeInput;

    GT_assert(DcTrace, (NULL != edgeInfo));

    for (cnt = 0u; cnt < numEdges; cnt ++)
    {
        nodeInput.inputId = edgeInfo[cnt].startNode;
        nodeInput.nodeId = edgeInfo[cnt].endNode;
        nodeInput.isEnable = isEnabled;
        ret = dcCtrlNodeInput(dCtrlInfo, &nodeInput);

        if (0 != ret)
        {
            lastEdge = cnt;
            break;
        }
    }

    if (FVID2_SOK != ret)
    {
        /* Deallocate the resources and clean out mesh */
        if (0u != isEnabled)
        {
            isEnabled = 0u;
        }
        else
        {
            isEnabled = 1u;
        }

        for (cnt = 0u; cnt < lastEdge; cnt ++)
        {
            nodeInput.inputId = edgeInfo[cnt].startNode;
            nodeInput.nodeId = edgeInfo[cnt].endNode;
            nodeInput.isEnable = isEnabled;
            ret = dcCtrlNodeInput(dCtrlInfo, &nodeInput);

            if (0 != ret)
            {
                break;
            }
        }
    }

    return (ret);
}


/** \brief Creates static DSS topology for the fixed edges/nodes. There
 *  are some dummy nodes in the DSS topology. Input and output of these
 *  nodes are always enabled and cannot be changed. This function creates
 *  this static table.
 *  Called at the init time only
 */
Void dcCreateNodeTable()
{
    Vps_DcEdgeInfo edgeInfo[VPS_DC_MAX_EDGES] = DC_EDGEINFO_DEFAULTS;
    UInt32 cnt, startNode, endNode, index1, index2;

    for (cnt = 0; cnt < DC_NUM_NODES; cnt ++)
    {
        VpsUtils_memset(&DcNodes[cnt].input, 0, sizeof(Dc_NodeSet));
        VpsUtils_memset(&DcNodes[cnt].output, 0, sizeof(Dc_NodeSet));
    }

    for (cnt = 0u; cnt < VPS_DC_MAX_EDGES; cnt ++)
    {
        startNode = edgeInfo[cnt].startNode;
        endNode = edgeInfo[cnt].endNode;
        /* End Node is output node for the start Node so update
         * information in start node*/
        index1 = DcNodes[startNode].output.numNodes;
        DcNodes[startNode].output.node[index1] = &DcNodes[endNode];
        DcNodes[startNode].output.numNodes ++;

        /* Start Node is input node for the end Node so update
         * information in end node*/
        index2 = DcNodes[endNode].input.numNodes;
        DcNodes[endNode].input.node[index2] = &DcNodes[startNode];
        DcNodes[endNode].input.numNodes ++;

        /* Dummy node's input is always enabled */
        if ((TRUE == DcNodes[endNode].isDummy) &&
            (TRUE == DcNodes[startNode].isDummy))
        {
            DcNodes[startNode].output.isEnabled[index1] = TRUE;
            DcNodes[endNode].input.isEnabled[index2] = TRUE;
        }
    }

    /* VCOMP to CIG path is always enabled */
    startNode = DC_NODE_VCOMP;
    endNode = DC_NODE_CIG_0;
    /* Since VCOMP has single output and CIG_1 has single input so using
     * index 0 for both of them */
    DcNodes[startNode].output.isEnabled[0u] = TRUE;
    DcNodes[endNode].input.isEnabled[0u] = TRUE;

    /* CIG to CIG_SPLITTER is always enabled */
    startNode = DC_NODE_CIG_1;
    endNode = 18u;
    /* Since CIG_2 has single output and CIG_SPLITTTER has single input
     * so using index 0 for both of them */
    DcNodes[startNode].output.isEnabled[0u] = TRUE;
    DcNodes[endNode].input.isEnabled[0u] = TRUE;

    /* Graphics to Graphics Splitter is always enabled */
    startNode = DC_NODE_G0;
    endNode = VPS_DC_GRPX0_INPUT_PATH;
    DcNodes[startNode].output.isEnabled[0u] = TRUE;
    DcNodes[endNode].input.isEnabled[0u] = TRUE;

    startNode = DC_NODE_G1;
    endNode = VPS_DC_GRPX1_INPUT_PATH;
    DcNodes[startNode].output.isEnabled[0u] = TRUE;
    DcNodes[endNode].input.isEnabled[0u] = TRUE;

    startNode = DC_NODE_G2;
    endNode = VPS_DC_GRPX2_INPUT_PATH;
    DcNodes[startNode].output.isEnabled[0u] = TRUE;
    DcNodes[endNode].input.isEnabled[0u] = TRUE;
}



Void PrintNodeTable()
{
    UInt32 cnt, cnt2;
    for (cnt = 0u; cnt < DC_NUM_NODES; cnt ++)
    {
        Vps_printf("Node Number = %d\nInput Nodes = ", DcNodes[cnt].nodeNum);
        for (cnt2 = 0; cnt2 < DcNodes[cnt].input.numNodes; cnt2 ++)
        {
            Vps_printf("%d ", DcNodes[cnt].input.node[cnt2]->nodeNum);
        }
        Vps_printf("\nEnabled Inputs =");
        for (cnt2 = 0; cnt2 < DcNodes[cnt].input.numNodes; cnt2 ++)
        {
            if (TRUE == DcNodes[cnt].input.isEnabled[cnt2])
            {
                Vps_printf("%d ", DcNodes[cnt].input.node[cnt2]->nodeNum);
            }
        }

        Vps_printf("\nOutput Nodes = ");
        for (cnt2 = 0; cnt2 < DcNodes[cnt].output.numNodes; cnt2 ++)
        {
            Vps_printf("%d ", DcNodes[cnt].output.node[cnt2]->nodeNum);
        }
        Vps_printf("\nEnabled Output =");
        for (cnt2 = 0; cnt2 < DcNodes[cnt].output.numNodes; cnt2 ++)
        {
            if (TRUE == DcNodes[cnt].output.isEnabled[cnt2])
            {
                Vps_printf("%d ", DcNodes[cnt].output.node[cnt2]->nodeNum);
            }
        }
        Vps_printf("\n\n");
    }
}



/** \brief Function to set the modeinformation in all modules coming
 *  on the path. Once Venc is configured for a mode, frame size needs to be
 *  set in the individual modules also, which are coming on the path to venc.
 *  This function traverses back from the venc node and sets the sizes in
 *  the modules by calling their function.
 */
Int32 dcSetModeInfo(UInt32           startNode,
                    Vps_DcModeInfo  *modeInfo,
                    UInt32           isForward)
{
    Int32               retVal = FVID2_EFAIL;
    Dc_NodeInfo        *currNode = NULL;
    UInt32              originalSf, isIntEnabled = 0;

    /* Get the pointer to the current node */
    currNode = dcGetNodeInfo(startNode);
    GT_assert(DcTrace, (NULL != currNode));
    GT_assert(DcTrace, (NULL != modeInfo));

    if (NULL != modeInfo)
    {
        /* Store the original scanFormat */
        originalSf = modeInfo->mInfo.scanFormat;

        /* Initialize Traverser with the current node */
        dcInitTraverser(currNode);

        /* Get the next node to apply mode information into it */
        currNode = dcGetNextTraversedNode(isForward);
        while (NULL != currNode)
        {
            /* If the interlacing is enabled in any of the CIG
               nodes, Progressive scanformat needs to be set in
               upper modules connected to that CIG path */
            /* Since CIG needs to be configured in frame
               size if interlacing is enabled, Passing progressive
               scanformat to the CIG node as well */
            if (VPS_DC_CIG_NON_CONSTRAINED_OUTPUT == currNode->nodeNum)
            {
                if ((NULL != currNode->priv) &&
                    (DC_INTERLACING_ENABLED == (UInt32)currNode->priv))
                {
                    if (FVID2_SF_PROGRESSIVE == modeInfo->mInfo.scanFormat)
                    {
                        /* Interlacing is enabled and venc is set
                           in progressive mode*/
                        retVal = FVID2_EALLOC;
                    }
                    else
                    {
                        modeInfo->mInfo.scanFormat = FVID2_SF_PROGRESSIVE;
                        isIntEnabled = 1;
                    }
                }
                else
                {
                    modeInfo->mInfo.scanFormat = originalSf;
                }
            }
            else if (VPS_DC_CIG_CONSTRAINED_OUTPUT == currNode->nodeNum)
            {
                if ((NULL != currNode->priv) &&
                    (DC_INTERLACING_ENABLED == (UInt32)currNode->priv))
                {
                    if (FVID2_SF_PROGRESSIVE == modeInfo->mInfo.scanFormat)
                    {
                        /* Interlacing is enabled and venc is set
                           in progressive mode*/
                        retVal = FVID2_EALLOC;
                    }
                    else
                    {
                        modeInfo->mInfo.scanFormat = FVID2_SF_PROGRESSIVE;
                    }
                }
                else
                {
                    if (0 == isIntEnabled)
                        modeInfo->mInfo.scanFormat = originalSf;
                }
            }
            else if (VPS_DC_CIG_PIP_OUTPUT == currNode->nodeNum)
            {
                if ((NULL != currNode->priv) &&
                    (DC_INTERLACING_ENABLED == (UInt32)currNode->priv))
                {
                    if (FVID2_SF_PROGRESSIVE == modeInfo->mInfo.scanFormat)
                    {
                        /* Interlacing is enabled and venc is set
                           in progressive mode*/
                        retVal = FVID2_EALLOC;
                    }
                    else
                    {
                        modeInfo->mInfo.scanFormat = FVID2_SF_PROGRESSIVE;
                    }
                }
                else
                {
                    modeInfo->mInfo.scanFormat = originalSf;
                }
            }

            if (NULL != currNode->setModeInfo)
            {
                currNode->setModeInfo(currNode->nodeNum, modeInfo);
            }

            /* Get the next node to apply mode information into it */
            currNode = dcGetNextTraversedNode(isForward);
        }

        /* Restore original scanformat */
        modeInfo->mInfo.scanFormat = originalSf;

        retVal = FVID2_SOK;
    }
    return (retVal);
}



/**
 * Compositor fires interrupt when one of the following condition occurs.
 * 1, Not all layers have data available when VENC request data.
 * 2, EOL signal for each layer does not happen at same time.
 * 3, EOF signal for each layer does not happen at same time.
 * This callback handler handles the Comp error callback. Currently it just
 * informs application that error has occured.
 * TODO: What action is to be taken when comp gives error.
 */
static Void dcCompErrCallback(const UInt32 *event,
                              UInt32 numEvents,
                              Ptr arg)
{
    UInt32  errStatus;
    VpsHal_Handle compHandle = DcCtrlInfo.initParams.halHandle[VDC_COMP_IDX];

    if (NULL != compHandle)
    {
        VpsHal_CompGetErrorSatus(compHandle, &errStatus);
        /* TODO: What to do with this error status */
    }
    /* Read from the COMP HAL about which error has occured and which blender
     * is in the error condition */
}



/** \brief Function to set the mode in the vencs. It enables COMP
 *  output to venc, sets given mode in the venc and starts the vencs.
 *
 *  All the vencs passed to this function are assumed to be disabled.
 */
static Int32 dcConfigVencMode(Vps_DcVencInfo *vencInfo)
{
    Int32               retVal = FVID2_SOK;
    UInt32              outNodeCnt, vencCnt;
    Dc_OutputNodeInfo  *outputNode = NULL;
    Dc_NodeInfo        *vencNode = NULL, *blendNode = NULL;
    VpsHal_CompMode     compMode;
    VpsHal_CompId       compId;
    VpsHal_Handle       compHandle;
    Vps_DcEdgeInfo      edgeInfo;
    UInt32              vencs;
    Vps_DcModeInfo     *modeInfo;
    UInt32              tiedVenc;
    UInt32              isResAllocated = FALSE;

    GT_assert(DcTrace, (NULL != vencInfo));

    /**
     * Steps
     * 1, If venc is alread running, return error
     * 2, Copy ModeInfo in the outputinfo data structure
     * 3, Allocate Blender and Venc resource
     * 4, Enable clock for the venc in CLKC module
     * 5, Set the mode in the Venc
     * 6, Get the mode from the venc and set it on all the intermediate release
     * 7, Enable the Comp Output for the Venc and enable output pixel clock
     * 8, Start the venc
     * 9, Start all the tied venc at the same time
     * 10, If there are multiple venc store them in the internal data structure
     */
    /* Get the HAL Handles */
    compHandle = DcCtrlInfo.initParams.halHandle[VDC_COMP_IDX];

    /* Create a bitmask of all the tied vencs */
    vencs = 0u;
    for (vencCnt = 0u; vencCnt < vencInfo->numVencs; vencCnt++)
    {
        vencs |= vencInfo->modeInfo[vencCnt].vencId;
    }

    if (0u != (~(DC_ALL_VENCS) & vencs))
    {
        /* Venc ID should be one of the supported one */
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        tiedVenc = dcGetDctrlObj()->tiedVencs;
        if (tiedVenc && (vencs & tiedVenc))
        {
            if (((tiedVenc & vencs) | (tiedVenc ^ vencs)) != tiedVenc)
            {
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    /* Find matching outputnode for each venc */
    for (outNodeCnt = 0u; (outNodeCnt < DC_MAX_OUTPUT) && (FVID2_SOK == retVal);
                outNodeCnt++)
    {
        /* Get the output Node */
        outputNode = &(DcOutputNode[outNodeCnt]);
        GT_assert(DcTrace, (NULL != outputNode));

        for (vencCnt = 0u; vencCnt < vencInfo->numVencs; vencCnt++)
        {
            modeInfo = &(vencInfo->modeInfo[vencCnt]);

            if (outputNode->nodeId == modeInfo->vencId)
            {
                isResAllocated = FALSE;

                if (TRUE == outputNode->isVencStarted)
                {
                    if ((FVID2_STD_CUSTOM != modeInfo->mInfo.standard) &&
                        (outputNode->modeInfo.mInfo.standard ==
                                modeInfo->mInfo.standard))
                    {
                        /* Since mode is same, there is no need to set
                           it again */
                        continue;
                    }
                    else
                    {
                        /* Mode is changed and VENC is running, first
                           disable VENC*/
                        retVal = FVID2_EDEVICE_INUSE;
                    }
                }

                /* Copy Mode Information to the outputnode */
                VpsUtils_memcpy(
                    &outputNode->modeInfo,
                    modeInfo,
                    sizeof(Vps_DcModeInfo));

                vencNode = outputNode->node;
                blendNode = vencNode->input.node[0];

                /* Get the resources for the Blender and Venc Nodes */
                edgeInfo.startNode = blendNode->nodeNum;
                edgeInfo.endNode = vencNode->nodeNum;
                retVal = dcAllocEdgeResource(&edgeInfo);

                if (FVID2_SOK == retVal)
                {
                    isResAllocated = TRUE;

                    /* Enable Clock for this VENC */
                    retVal = VpsHal_vpsClkcModuleEnable(
                                outputNode->clkcModule,
                                TRUE);

                }

                if (FVID2_SOK == retVal)
                {
                    /* Call the setMode of the Venc in order to make sure
                       that mode is correct */
                    retVal = outputNode->setMode(
                                            outputNode->vencHandle,
                                            modeInfo);

                    /* Mode is set in the VENC, so get the mode information */
                    if (FVID2_SOK == retVal)
                    {
                        retVal = outputNode->getMode(
                                            outputNode->vencHandle,
                                            &outputNode->modeInfo);
                    }
                }
                /* Setting modeinfo in the intermediate nodes
                   is not needed here as when first path start streaming,
                   it will be set */
                /* If the mode is set correctly in the Venc as well as in
                   all other intermediate modules, enable Comp output and clock
                   and start the VENC */
                if (FVID2_SOK == retVal)
                {
                    /* Enable Blender output and Venc nodes input */
                    vencNode->input.isEnabled[0u] = TRUE;
                    blendNode->output.isEnabled[0u] = TRUE;

                    /* Enable Blender associated with this VENC.
                     * Blender is enabled along with the VENC so that
                     * if nothing is connected to the blender, at least
                     * background color is available to display */
                    if (FVID2_SF_INTERLACED ==
                            outputNode->modeInfo.mInfo.scanFormat)
                    {
                        compMode = VPSHAL_COMP_MODE_INTERLACED;
                    }
                    else
                    {
                        compMode = VPSHAL_COMP_MODE_PROGRESSIVE;
                    }

                    /* As the output venc node has just one input node, which is
                     * blender node. So get the blender node and enable output in
                     * blender. */
                    compId = (VpsHal_CompId)blendNode->priv;
                    retVal = VpsHal_compEnableOutput(
                                compHandle,
                                compId,
                                compMode);

                    /* Enable the Output Clock */
                    if (VPS_DC_VENC_SD != outputNode->nodeId)
                    {
                        retVal |= VpsHal_vpsVencOutputPixClkEnable(
                                    outputNode->vencOutPixClk,
                                    TRUE);
                    }

                    /* everything is setup now, so venc can be started */
                    retVal |= outputNode->startVenc(outputNode->vencHandle);

                    /* Enable VENC Timing Generator for non-tied vencs */
                    if (0u == (modeInfo->vencId & vencInfo->tiedVencs))
                    {
                        VpsHal_vpsClkcVencEnable(modeInfo->vencId, TRUE);
                    }

                    if (FVID2_SOK != retVal)
                    {
                        /* Disable Everything */
                        VpsHal_compDisableOutput(
                            compHandle,
                            compId);

                        VpsHal_vpsClkcModuleEnable(
                            outputNode->clkcModule,
                            FALSE);

                        outputNode->stopVenc(outputNode->vencHandle);

                        /* Disable Blender output and Venc nodes input */
                        vencNode->input.isEnabled[0u] = FALSE;
                        blendNode->output.isEnabled[0u] = FALSE;

                        if (isResAllocated)
                        {
                            dcDeAllocEdgeResource(&edgeInfo);
                        }
                    }
                    else
                    {
                        outputNode->isVencStarted = TRUE;
                    }
                }

                /* Found the Venc, so breaking loop for the venc */
                break;
            }
        }
    }


    if (FVID2_SOK == retVal)
    {
        if (0u != vencInfo->tiedVencs)
        {
            /* Reset the VENC so that they are phase aligned */
            for (outNodeCnt = 0u; (outNodeCnt < DC_MAX_OUTPUT) && (FVID2_SOK == retVal);
                        outNodeCnt++)
            {
                /* Get the output Node */
                outputNode = &(DcOutputNode[outNodeCnt]);
                GT_assert(DcTrace, (NULL != outputNode));

                for (vencCnt = 0u; vencCnt < vencInfo->numVencs; vencCnt++)
                {
                    modeInfo = &(vencInfo->modeInfo[vencCnt]);

                    if (outputNode->nodeId & vencInfo->tiedVencs)
                    {
                        /* Ideally VENC should be reset using CLKC module,
                           since reset in CLKC module is not working,
                           resetting it by setting counters to zero */

                        /* Enable VENC Clock */
                        VpsHal_vpsClkcVencEnable(outputNode->nodeId, TRUE);

                        /* Reset VENC Counters */
                        if (NULL != outputNode->resetVenc)
                        {
                            outputNode->resetVenc(outputNode->vencHandle);
                        }

                        /* Disable VENC Clock */
                        VpsHal_vpsClkcVencEnable(outputNode->nodeId, FALSE);

                        retVal = outputNode->setMode(
                                            outputNode->vencHandle,
                                            &outputNode->modeInfo);
                        retVal |= outputNode->startVenc(outputNode->vencHandle);

                        break;
                    }
                }
            }

            /* Enable All these tied/non-tied VENCs at the same time */
            /* Ignoring return value as it always returns success */
            VpsHal_vpsClkcVencEnable(vencInfo->tiedVencs, TRUE);

            /* Store TiedVencInformation in Display Controller Object */
            dcGetDctrlObj()->tiedVencs = vencInfo->tiedVencs;
        }
    }

    return (retVal);
}


/** \brief Function to check whether streaming is already on on any of
 *  the input nodes connected to given node number.
 */
static UInt32 dcIsInputNodeStarted(UInt32 nodeNum)
{
    Int32               retVal = 0;
    UInt32              isStarted = FALSE;
    UInt32              cnt;
    UInt32              numStartNodes, nodeId;
    UInt32              startNodes[DC_MAX_INPUT];
    Dc_InputNodeInfo   *inputNode = NULL;
    Dc_NodeInfo        *node = NULL;

    /* Get the inputs nodes from this node to see if streaming is on
     * in these nodes */
    retVal = dcGetLeafNodes(
                dcGetNodeInfo(nodeNum),
                startNodes,
                &numStartNodes,
                FALSE);

    for (cnt = 0u; (cnt < numStartNodes) && (FVID2_SOK == retVal); cnt ++)
    {
        nodeId = startNodes[cnt];

        node = dcGetNodeInfo(nodeId);
        GT_assert(DcTrace, (NULL != node));
        inputNode = (Dc_InputNodeInfo *) node->parent;
        GT_assert(DcTrace, (NULL != inputNode));

        /* Check to see if the input is started or not */
        if (TRUE == inputNode->isStarted)
        {
            GT_2trace(DcTrace,
                      GT_ERR,
                      "%s: Streaming is on on the node %s",
                      __FUNCTION__,
                      inputNode->node->nodeName);
            isStarted = TRUE;
            break;
        }
    }

    if (FVID2_SOK != retVal)
    {
        GT_3trace(DcTrace,
                  GT_ERR,
                  "%s: numStartNodes %d, retVal %d",
                  __FUNCTION__,
                  numStartNodes,
                  retVal);
        /* Consider some error as input node is started. */
        isStarted = TRUE;
    }

    return (isStarted);
}



/** \brief Display Controller is divided into many files, which uses
 *  global display controller object. This function returns pointer to
 *  this global object.
 */
Dc_DisplayCtrlInfo *dcGetDctrlObj()
{
    return (&DcCtrlInfo);
}



/** \brief Function to get the pointer to input node for the given index
 */
Dc_InputNodeInfo *dcGetInputNode(UInt32 cnt)
{
    Dc_InputNodeInfo *inputNode = NULL;
    if (cnt < DC_MAX_INPUT)
    {
        inputNode = &DcInputNode[cnt];
    }
    return (inputNode);
}



/** \brief Function to get the pointer to output node for the given index
 */
Dc_OutputNodeInfo *dcGetOutputNode(UInt32 cnt)
{
    Dc_OutputNodeInfo *outputNode = NULL;
    if (cnt < DC_MAX_OUTPUT)
    {
        outputNode = &DcOutputNode[cnt];
    }
    return (outputNode);
}



/** \brief Function to get the pointer to node for the given index
 */
Dc_NodeInfo *dcGetNodeInfo(UInt32 cnt)
{
    Dc_NodeInfo *node = NULL;
    if (cnt < DC_NUM_NODES)
    {
        node = &DcNodes[cnt];
    }
    return (node);
}

/**
 * \brief This function controls vencs and / or its paired on-chip encoders.
 * \par CAUTION This function DOES NOT validate the function arguments. Its
 *      expected that caller would have validated the arguments.
 */
static Int32 dcVencCmd(UInt32 cmd, Vps_DcOnchipEncoderCmd *encCmd)
{
    Int32               rtnValue = FVID2_EFAIL;
    UInt32              cnt;
    Dc_OutputNodeInfo   *outputNode = NULL;

    for (cnt = 0u; cnt < DC_MAX_OUTPUT; cnt ++)
    {
        if (encCmd->vencId == DcOutputNode[cnt].nodeId)
        {
            outputNode = &(DcOutputNode[cnt]);

            /* Check if the control function of the venc is specified */
            if (outputNode->controlVenc != NULL)
            {
                rtnValue = outputNode->controlVenc(
                                        outputNode->vencHandle,
                                        cmd,
                                        encCmd,
                                        NULL);
            }
            /* Supporting control command a single venc at a time. */
            break;
        }
    }
    return (rtnValue);
}



/* \brief Function to set the clock source and muxes in the dss */
static Int32 dcSetVencClkSrc(Dc_DisplayCtrlInfo *dcCtrlInfo,
                             Vps_DcVencClkSrc *clkSrc)
{
    Int32                retVal = FVID2_SOK;
    UInt32               vencCnt;
    Dc_OutputNodeInfo   *outputNode = NULL;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));

    if ((VPS_DC_VENC_HDMI == clkSrc->venc) &&
        ((VPS_DC_CLKSRC_VENCD != clkSrc->clkSrc) &&
         (VPS_DC_CLKSRC_VENCD_DIV2 != clkSrc->clkSrc) &&
         (VPS_DC_CLKSRC_VENCD_DIV2_DIFF != clkSrc->clkSrc)))
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: Wrong Source Clock fro HDMI Venc",
                  __FUNCTION__);
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {

        for (vencCnt = 0u; vencCnt < DC_MAX_OUTPUT; vencCnt ++)
        {
            outputNode = &(DcOutputNode[vencCnt]);
            if (outputNode->nodeId == clkSrc->venc)
            {
                break;
            }
        }

        if (DC_MAX_OUTPUT == vencCnt)
        {
            GT_1trace(DcTrace,
                      GT_ERR,
                      "%s: Wrong Venc",
                      __FUNCTION__);
            retVal = FVID2_EINVALID_PARAMS;
        }
    }

    if (FVID2_SOK == retVal)
    {
        if (outputNode->vencClkSrc != clkSrc->clkSrc)
        {
            outputNode->vencClkSrc = (Vps_DcVencClkSrcSel)clkSrc->clkSrc;

            /* Clk source is not same */
            if (VPS_DC_VENC_HDMI == clkSrc->venc)
            {
                switch (clkSrc->clkSrc)
                {
                    case VPS_DC_CLKSRC_VENCD:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_DVO1,
                            FALSE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_D_CLK1X,
                            FALSE);
                        break;

                    case VPS_DC_CLKSRC_VENCD_DIV2:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_DVO1,
                            TRUE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_D_CLK1X,
                            TRUE);
                        break;

                    case VPS_DC_CLKSRC_VENCD_DIV2_DIFF:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_DVO1,
                            FALSE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_D_CLK1X,
                            TRUE);
                        break;

                    default:
                        retVal = FVID2_EINVALID_PARAMS;
                        break;
                }
            }
            else if (VPS_DC_VENC_DVO2 == clkSrc->venc)
            {
                /* Select the clock source for the DVO2 */
                if ((VPS_DC_CLKSRC_VENCD == clkSrc->clkSrc) ||
                    (VPS_DC_CLKSRC_VENCD_DIV2 == clkSrc->clkSrc) ||
                    (VPS_DC_CLKSRC_VENCD_DIV2_DIFF == clkSrc->clkSrc))
                {
                    /* Select VENCD as clock source for DVO2 */
                    VpsHal_vpsVencGClkSrcSelect(
                        VPSHAL_VPS_VENC_G_CLK_SRC_VENC_D_CLK);
                }
                else
                {
                    /* Select VENCA as clock source for DVO2 */
                    VpsHal_vpsVencGClkSrcSelect(
                        VPSHAL_VPS_VENC_G_CLK_SRC_VENC_A_CLK);
                }

                switch (clkSrc->clkSrc)
                {
                    case VPS_DC_CLKSRC_VENCD:
                    case VPS_DC_CLKSRC_VENCA:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_DVO2,
                            FALSE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_G_CLK1X,
                            FALSE);
                        break;

                    case VPS_DC_CLKSRC_VENCD_DIV2:
                    case VPS_DC_CLKSRC_VENCA_DIV2:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_DVO2,
                            TRUE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_G_CLK1X,
                            TRUE);
                        break;

                    case VPS_DC_CLKSRC_VENCD_DIV2_DIFF:
                    case VPS_DC_CLKSRC_VENCA_DIV2_DIFF:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_DVO2,
                            FALSE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_G_CLK1X,
                            TRUE);
                        break;

                    default:
                        retVal = FVID2_EINVALID_PARAMS;
                        break;
                }
            }
            else if (VPS_DC_VENC_HDCOMP == clkSrc->venc)
            {
                /* Select the clock source for the VENCA */
                if ((VPS_DC_CLKSRC_VENCD == clkSrc->clkSrc) ||
                    (VPS_DC_CLKSRC_VENCD_DIV2 == clkSrc->clkSrc) ||
                    (VPS_DC_CLKSRC_VENCD_DIV2_DIFF == clkSrc->clkSrc))
                {
                    /* Select VENCD as clock source for Venc A */
                    VpsHal_vpsVencAClkSrcSelect(
                        VPSHAL_VPS_VENC_A_CLK_SRC_VENC_D_CLK);
                }
                else
                {
                    /* Select VENCA as clock source for DVO2 */
                    VpsHal_vpsVencAClkSrcSelect(
                        VPSHAL_VPS_VENC_A_CLK_SRC_VENC_A_CLK);
                }

                switch (clkSrc->clkSrc)
                {
                    case VPS_DC_CLKSRC_VENCD:
                    case VPS_DC_CLKSRC_VENCA:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_A_VBI,
                            FALSE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_A_CLK1X,
                            FALSE);
                        break;

                    case VPS_DC_CLKSRC_VENCD_DIV2:
                    case VPS_DC_CLKSRC_VENCA_DIV2:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_A_VBI,
                            TRUE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_A_CLK1X,
                            TRUE);
                        break;

                    case VPS_DC_CLKSRC_VENCD_DIV2_DIFF:
                    case VPS_DC_CLKSRC_VENCA_DIV2_DIFF:
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_A_VBI,
                            FALSE);
                        VpsHal_vpsVencClkDivide(
                            VPSHAL_VPS_VENC_CLK_DIV_VENC_A_CLK1X,
                            TRUE);
                        break;

                    default:
                        retVal = FVID2_EINVALID_PARAMS;
                        break;
                }
            }
            else
            {
                retVal = FVID2_EINVALID_PARAMS;
            }
        }
    }

    return (retVal);
}



/* \brief Function to set the clock source and muxes in the dss */
static Int32 dcGetVencClkSrc(Dc_DisplayCtrlInfo *dcCtrlInfo,
                             Vps_DcVencClkSrc *clkSrc)
{
    Int32                retVal = FVID2_SOK;
    UInt32               vencCnt;
    Dc_OutputNodeInfo   *outputNode = NULL;

    GT_assert(DcTrace, (NULL != dcCtrlInfo));

    if (0u == clkSrc->venc)
    {
        retVal = FVID2_EINVALID_PARAMS;
    }

    for (vencCnt = 0u; vencCnt < DC_MAX_OUTPUT; vencCnt ++)
    {
        outputNode = &(DcOutputNode[vencCnt]);
        if (outputNode->nodeId == clkSrc->venc)
        {
            break;
        }
    }

    if (DC_MAX_OUTPUT == vencCnt)
    {
        GT_1trace(DcTrace,
                  GT_ERR,
                  "%s: Wrong Venc",
                  __FUNCTION__);
        retVal = FVID2_EINVALID_PARAMS;
    }

    if (FVID2_SOK == retVal)
    {
        clkSrc->clkSrc = (UInt32) outputNode->vencClkSrc;
    }

    return (retVal);
}

