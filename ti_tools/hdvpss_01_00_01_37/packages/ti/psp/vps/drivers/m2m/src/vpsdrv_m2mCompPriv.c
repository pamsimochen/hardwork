/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *
 *  \brief VPS COMP M2M driver internal file.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/common/vps_resrcMgr.h>
#include <ti/psp/vps/common/vps_evtMgr.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/vps_cfgSc.h>
#include <ti/psp/vps/vps_graphics.h>
#include <ti/psp/vps/hal/vpshal_vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_grpx.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_bypass.h>
#include <ti/psp/vps/core/vpscore_deiPath.h>
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

/**
 *  \brief Maximum number of effective driver instance - one HDMI_BLEND and
 *  one DVO2_BLEND paths. All driver instance can't be opened at a time.
 *  Hence for memory allocation like descriptors, channels etc this macro
 *  is used instead of VPS_M2M_COMP_INST_MAX which could be as many as different
 *  possible drivers for the same paths.
 */
#define VPSMDRV_COMP_MAX_EFF_INST        (2u)

/**
 *  \brief Maximum number of inbound data descriptor required by driver
 *  per channel.
 *  Considering (Mosaic Video input + multi region graphics) for max supported videos/regions
 */
#define VPSMDRV_COMP_MAX_IN_DESC         (VPS_CFG_MAX_IN_DESC+VPS_GRPX_MAX_REGIONS_PER_FRAME)

/**
 *  \brief Maximum number of outbound data descriptor required by driver
 *  per channel.
 *  Considering VIP0/1 Core with 3 outputs - 2*3 (2 for 420 YUV OUT with Y and C separate)+
 *  DEI HQ Core     - 6 (2 x 2 curr fld out, 1 MV OUT, 1 MVSTM OUT)
 */
#define VPSMDRV_COMP_MAX_OUT_DESC        (12u)


/**
 *  \brief Maximum number of config descriptor required by driver to configure
 *  shadow memory per channel or per handle.
 */
#define VPSMDRV_COMP_MAX_SHW_CFG_DESC    (1u)

/**
 *  \brief Maximum number of config descriptor required by driver
 *  to configure the non shadow memory per channel or per handle.
 */
#define VPSMDRV_COMP_MAX_NSHW_CFG_DESC   (1u)

/**
 *  \brief Maximum number of SOC control descriptors required by driver
 *  to wait on client to start the transfer so that the next shadow register
 *  configuration don't overwrite the current register configuration.
 *  Used 5 to support 2 DEI inputs, 1 DWP output and max 2 VIP outputs.
 */
#define VPSMDRV_COMP_MAX_SOC_DESC        (VPSMDRV_COMP_MAX_IN_DESC +             \
                                         VPSMDRV_COMP_MAX_OUT_DESC)

/**
 *  \brief Maximum number of Reload descriptors required by driver
 *  to link multiple channels for each request.
 */
#define VPSMDRV_COMP_MAX_RLD_DESC        (1u)

/**
 *  \brief Shadow config overlay memory (in bytes) needed by driver for
 *  programming shadow registers for each channel * number of sets
 *  per channel (2 for ping/pong, no scratch memory as same overlay memory
 *  is used for both channel and scratch pad channel).
 *  Caution: Make this a multiple of VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN so
 *  that each memory pool element will also be aligned to this alignment
 *  when defining the array for this pool.
 */
#define VPSMDRV_COMP_MAX_SHW_OVLY_MEM    (864u * VPSMDRV_COMP_MAX_DESC_SET)

/**
 *  \brief Non shadow config overlay memory (in bytes) needed by driver for
 *  programming non shadow registers for each handle * number of sets
 *  per channel (2 for ping/pong, no scratch memory as same overlay memory
 *  is used for both channel and scratch pad channel).
 *  Caution: Make this a multiple of VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN so
 *  that each memory pool element will also be aligned to this alignment
 *  when defining the array for this pool.
 */
#define VPSMDRV_COMP_MAX_NSHW_OVLY_MEM   ((96u + 320u) * VPSMDRV_COMP_MAX_DESC_SET)

/** \brief Maximum number of handle to allocate. */
#define VPSMDRV_COMP_MAX_HANDLE_MEM_POOL (VPS_M2M_COMP_MAX_HANDLE_PER_INST       \
                                       * VPSMDRV_COMP_MAX_EFF_INST)

/**
 *  \brief Maximum number of queue objects required to store the incoming
 *  requests per instance. After the queue objects are full, driver will
 *  not queue further requests and will return error to application.
 *  Note: Queue objects pool is shared between all the handles of instance.
 */
#define VPSMDRV_COMP_MAX_QOBJS_POOL      (VPSMDRV_COMP_MAX_EFF_INST              \
                                       * VPS_M2M_COMP_MAX_HANDLE_PER_INST       \
                                       * VPSMDRV_COMP_MAX_QOBJS_PER_HANDLE)

/**
 *  \brief Maximum number of channel memory pool to allocate.
 *  Maximum number of instance * maximum number of channels per instance.
 */
#define VPSMDRV_COMP_MAX_CH_MEM_POOL    (VPSMDRV_COMP_MAX_EFF_INST               \
                                      * VPS_M2M_COMP_MAX_CH_PER_INST)

/**
 *  \brief Total coefficient descriptor memory required for each instance
 *  to configure the coefficient memory each time ioctl is fired to update
 *  the coefficients.
 */
#define VPSMDRV_COMP_MAX_COEFF_DESC_MEM  ((VPSMDRV_COMP_MAX_COEFF_CFG_DESC * \
                                          VPSHAL_VPDMA_CONFIG_DESC_SIZE)      \
                                       + VPSHAL_VPDMA_CTRL_DESC_SIZE)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  struct VpsMdrv_CompPoolObj
 *  COMP M2M driver memory pool object containing all the statically allocated
 *  objects - used structure to avoid multiple global variables.
 */
typedef struct
{
    VpsMdrv_CompHandleObj    handleMemPool[VPSMDRV_COMP_MAX_HANDLE_MEM_POOL];
    /**< Memory pool for the handle objects. This memory is allocated
         statically and is common for all of the instance objects.
         Any instance can take any handle object. */
    UInt32                  handleMemFlag[VPSMDRV_COMP_MAX_HANDLE_MEM_POOL];
    /**< The flag variable represents whether a handle object is allocated
         or not. */
    VpsUtils_PoolParams     handlePoolPrm;
    /**< Pool params for handle memory. */

    VpsMdrv_CompChObj        chMemPool[VPSMDRV_COMP_MAX_CH_MEM_POOL];
    /**< Memory pool for the channel objects. This memory is allocated
         statically and is common for all of the instance objects.
         Any instance can take any channel object. */
    UInt32                  chMemFlag[VPSMDRV_COMP_MAX_CH_MEM_POOL];
    /**< The flag variable represents whether a channel object is allocated
         or not. */
    VpsUtils_PoolParams     chPoolPrm;
    /**< Pool params for channel memory. */

    VpsMdrv_CompQueueObj     qObjMemPool[VPSMDRV_COMP_MAX_QOBJS_POOL];
    /**< Memory pool for the queue objects. This memory is allocated
         statically and is common for all of the instance objects.
         Any instance can take any queue object. */
    UInt32                  qObjMemFlag[VPSMDRV_COMP_MAX_QOBJS_POOL];
    /**< The flag variable represents whether a queue object is allocated
         or not. */
    VpsUtils_PoolParams     qObjPoolPrm;
    /**< Pool params for queue object memory. */
} VpsMdrv_CompPoolObj;


/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */
static Int32 vpsMdrvCompAllocChObjs(VpsMdrv_CompHandleObj *hObj, UInt32 numCh);
static Int32 vpsMdrvCompFreeChObjs(VpsMdrv_CompHandleObj *hObj, UInt32 numCh);
static Int32 vpsMdrvCompCopyChPrms(VpsMdrv_CompHandleObj *hObj,
                                  const Vps_M2mCompChParams *inChPrms);
static Int32 vpsMdrvCompAllocChDescMem(VpsMdrv_CompHandleObj *hObj,
                                      VpsMdrv_CompChObj *chObj,
                                      UInt32 perChCfg);
static Int32 vpsMdrvCompFreeChDescMem(VpsMdrv_CompChObj *chObj);

static Int32 vpsMdrvCompSplitChDescMem(VpsMdrv_CompHandleObj *hObj,
                                      VpsMdrv_CompChObj *chObj);
static Int32 vpsMdrvCompSplitNonShoadowMem(VpsMdrv_CompHandleObj *hObj);
static Int32 vpsMdrvCompProgramChDesc(VpsMdrv_CompHandleObj *hObj,
                                    VpsMdrv_CompChObj *chObj);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

/** \brief COMP M2M driver instance objects. */
static VpsMdrv_CompInstObj VpsMdrvCompInstObjects[VPS_M2M_COMP_INST_MAX];

/**
 *  \brief COMP M2M driver pool objects used for storing pool memories, pool
 *  flags and pool handles.
 */
VpsMdrv_CompPoolObj gVpsMdrvCompPoolObj;


/**
 *  \brief Default scalar configuration. Used when user gives DEI/VIP scalar
 *  configuration as NULL in channel parameter.
 */
static const Vps_ScConfig VpsMdrvCompDefScCfg =
{
    TRUE,                   /* bypass */
    FALSE,                  /* nonLinear */
    0u,                     /* stripSize */
    VPS_SC_HST_AUTO,        /* hsType */
    VPS_SC_VST_POLYPHASE,   /* vsType */
};

static const Vps_CropConfig VpsMdrvCompDefCropCfg =
{
        0u,                 /* cropStartY */
        0u,                 /* cropStartX */
        720u,               /* cropWidth */
        480u,               /* cropHeight */
};

/**
 *  \brief Default FVID2 format configuration. Used when user gives in/out
 *  FVID2 format as NULL in channel parameter.
 */
static const FVID2_Format VpsMdrvCompDefFmt =
{
    0u,                     /* channelNum */
    720u,                   /* width */
    480u,                   /* height */
    {720u * 2u, 0u, 0u},    /* pitch */
    {FALSE, FALSE, FALSE},  /* fieldMerged */
    FVID2_DF_YUV422I_YUYV,  /* dataFormat */
    FVID2_SF_PROGRESSIVE,   /* scanFormat */
    FVID2_BPP_BITS16,       /* bpp */
    NULL                    /* reserved */
};

/**
 *  \brief Default Graphics configuration. Used when user gives
 *  Vps_GrpxCreateParams as NULL in channel parameter.
 */
static const Vps_GrpxCreateParams VpsMdrvCompGrpxPrms =
{
    VPS_VPDMA_MT_NONTILEDMEM,   /* memory type */
    VPS_GRPX_FRAME_BUFFER_MODE,  /* drvMode */
    FALSE,
};


/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  vpsMdrvCompInit
 *  Initializes COMP M2M driver objects, allocates memory etc.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vpsMdrvCompInit(UInt32 numInst,
                     const VpsMdrv_CompInitParams *initPrms)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  instCnt, coreCnt;
    Semaphore_Params        semPrms;
    VpsMdrv_CompInstObj     *instObj;

    /* Check for errors */
    GT_assert(VpsMdrvCompTrace, (numInst <= VPS_M2M_COMP_INST_MAX));
    GT_assert(VpsMdrvCompTrace, (NULL != initPrms));

    /* Initialize driver objects to zero */
    VpsUtils_memset(VpsMdrvCompInstObjects, 0u, sizeof(VpsMdrvCompInstObjects));
    VpsUtils_memset(&gVpsMdrvCompPoolObj, 0u, sizeof(gVpsMdrvCompPoolObj));

    /* Init pool objects */
    VpsUtils_initPool(
        &gVpsMdrvCompPoolObj.handlePoolPrm,
        (Void *) gVpsMdrvCompPoolObj.handleMemPool,
        VPSMDRV_COMP_MAX_HANDLE_MEM_POOL,
        sizeof(VpsMdrv_CompHandleObj),
        gVpsMdrvCompPoolObj.handleMemFlag,
        VpsMdrvCompTrace);
    VpsUtils_initPool(
        &gVpsMdrvCompPoolObj.chPoolPrm,
        (Void *) gVpsMdrvCompPoolObj.chMemPool,
        VPSMDRV_COMP_MAX_CH_MEM_POOL,
        sizeof(VpsMdrv_CompChObj),
        gVpsMdrvCompPoolObj.chMemFlag,
        VpsMdrvCompTrace);
    VpsUtils_initPool(
        &gVpsMdrvCompPoolObj.qObjPoolPrm,
        (Void *) gVpsMdrvCompPoolObj.qObjMemPool,
        VPSMDRV_COMP_MAX_QOBJS_POOL,
        sizeof(VpsMdrv_CompQueueObj),
        gVpsMdrvCompPoolObj.qObjMemFlag,
        VpsMdrvCompTrace);

    /* Initialize instance object members */
    for (instCnt = 0u; instCnt < numInst; instCnt++)
    {
        /* Atleast one input and one output cores should be present */
        GT_assert(VpsMdrvCompTrace,
            (NULL != initPrms->coreOps[VPSMDRV_COMP_CORE_IN_VIDEO_IDX]));
        GT_assert(VpsMdrvCompTrace,
            (NULL != initPrms->coreOps[VPSMDRV_COMP_CORE_OUT_WRBK0_IDX]));

        instObj = &VpsMdrvCompInstObjects[instCnt];

        /* Allocate instance semaphore */
        Semaphore_Params_init(&semPrms);
        instObj->instSem = Semaphore_create(1u, &semPrms, NULL);
        if (NULL == instObj->instSem)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR,
                "Instance semaphore create failed!!\n");
            retVal = FVID2_EALLOC;
            break;
        }

        /* Copy the information */
        instObj->drvInstId = initPrms->drvInstId;
        for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
        {
            instObj->resrcId[coreCnt] = initPrms->resrcId[coreCnt];
            instObj->coreInstId[coreCnt] = initPrms->coreInstId[coreCnt];
            instObj->coreOps[coreCnt] = initPrms->coreOps[coreCnt];
            /* If core ops is NULL then that particular core is not used */
            if (NULL == instObj->coreOps[coreCnt])
            {
                instObj->isCoreReq[coreCnt] = FALSE;
            }
            else
            {
                instObj->isCoreReq[coreCnt] = TRUE;
            }
        }

        /* Figure out how many input framelists are required for an instance */
        instObj->numInListReq = 0u;
        if (TRUE == instObj->isCoreReq[VPSMDRV_COMP_CORE_IN_VIDEO_IDX])
        {
            instObj->numInListReq++;
        }
        if (TRUE == instObj->isCoreReq[VPSMDRV_COMP_CORE_IN_GRPX_IDX])
        {
            instObj->numInListReq++;
        }

        /* Figure out how many output framelists are required for an instance */
        instObj->numOutListReq = 0u;
        if (TRUE == instObj->isCoreReq[VPSMDRV_COMP_CORE_OUT_WRBK0_IDX])
        {
            instObj->numOutListReq++;
        }
        if (TRUE == instObj->isCoreReq[VPSMDRV_COMP_CORE_OUT_WRBK1_IDX])
        {
            instObj->numOutListReq++;
        }

        /* Initialize other variables */
        instObj->openCnt = 0u;
        instObj->initDone = TRUE;

        /* Move to next parameter */
        initPrms++;
    }

    /* Deinit if error occurs */
    if (FVID2_SOK != retVal)
    {
        vpsMdrvCompDeInit();
    }

    return (retVal);
}



/**
 *  vpsMdrvCompDeInit
 *  Deallocates memory allocated by init function.
 *  Returns VPS_SOK on success else returns error value.
 */
Int32 vpsMdrvCompDeInit(void)
{
    Int32               retVal = FVID2_SOK;
    UInt32              instCnt, coreCnt;
    VpsMdrv_CompInstObj *instObj;

    for (instCnt = 0u; instCnt < VPS_M2M_COMP_INST_MAX; instCnt++)
    {
        instObj = &VpsMdrvCompInstObjects[instCnt];
        if (TRUE == instObj->initDone)
        {
            /* Check if all the handles of the driver are closed */
            if (0u != instObj->openCnt)
            {
                GT_2trace(VpsMdrvCompTrace, GT_ERR,
                    "%d handles of driver inst %d not closed!\n",
                    instObj->openCnt,
                    instCnt);
                retVal = FVID2_EDRIVER_INUSE;
                break;
            }

            /* Init variables to zero */
            instObj->drvInstId = 0u;
            for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
            {
                instObj->coreInstId[coreCnt] = 0u;
                instObj->coreOps[coreCnt] = NULL;
                instObj->isCoreReq[coreCnt] = FALSE;
            }

            /* Delete the instance semaphore */
            if (NULL != instObj->instSem)
            {
                Semaphore_delete(&instObj->instSem);
                instObj->instSem = NULL;
            }

            /* Reset the init flag */
            instObj->initDone = FALSE;
        }
    }

    return (retVal);
}

#if 0
/**
 *  VpsMdrvCompInitDctrlPath
 *  Allocates the resources required and initializes the Dctrl path hardware blocks.
 */
Int32 VpsMdrvCompInitDctrlPath(UInt32 drvInstId)
{
    Int32       retVal = FVID2_SOK, errRetVal;
    if (VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB == drvInstId)
    {
       /* Allocate resources, Enable and configure CSC, CIG and HDMI Blender, write back CSC;
          Disable HDVENC_D/HDMI VENC clock, otherwise this m2m driver will not work
          */
       /* Allocate resource needed by this instance :
          VRM_RESOURCE_HD1_PATH, VRM_RESOURCE_HDMI_BLEND*/
        retVal = Vrm_allocResource(VRM_RESOURCE_HD1_PATH);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Could not allocate resource %d!\n",
                VRM_RESOURCE_HD1_PATH);
            break;
        }
        retVal = Vrm_allocResource(VRM_RESOURCE_HDMI_BLEND);
        if (FVID2_SOK != retVal)
        {
            /* If error free the already allocated resource */
            errRetVal = Vrm_releaseResource(VRM_RESOURCE_HD1_PATH);
            GT_assert(VpsMdrvCompTrace, (FVID2_SOK == errRetVal));

            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Could not allocate resource %d!\n",
                VRM_RESOURCE_HDMI_BLEND);
            break;
        }

        /* Enable and configure CSC, CIG and HDMI Blender, write back CSC */
        if (FVID2_SOK == retVal)
        {
            /* configure CSC, CIG */
            retVal |= VpsHal_cigEnablePip(cigHandle, nodeInfo->isEnable, ovlyPtr);
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

            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR, "Could not set SC5 input mux!\n");
            }
        }
    }

    return (retVal);
}
#endif
/**
 *  vpsMdrvCompAllocResrc
 *  Allocates the resources required for the driver from resource manager.
 *  This function will also set the Required Muxes.
 */
Int32 vpsMdrvCompAllocResrc(VpsMdrv_CompInstObj *instObj)
{
    Int32       retVal = FVID2_SOK, errRetVal;
    UInt32      coreCnt, errCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Allocate resource only if needed by that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            if (VRM_RESOURCE_INVALID == instObj->resrcId[coreCnt])
            {
                /* Invalid resource id should be allocated */
                continue;
            }
            retVal = Vrm_allocResource(instObj->resrcId[coreCnt]);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not allocate resource %d!\n",
                    instObj->resrcId[coreCnt]);
                break;
            }
        }
    }

    //Set the Muxes Required based on Driver Path/instance ID : TODO ?? update
    if (FVID2_SOK == retVal)
    {
        /* Enabled the proper Muxes */
        if (VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB == instObj->drvInstId)
        {
            /* input to Blender is via VCOMP Mux */
            retVal |= VpsHal_vpsVencMuxSrcSelect(
                            VPSHAL_VPS_VENC_MUX_HDCOMP,
                            VPSHAL_VPS_VENC_MUX_SRC_BP0,
                            NULL);
            /* input to SC5 is via HDMI */
            retVal |= VpsHal_vpsWb2MuxSrcSelect(VPSHAL_VPS_WB2_MUX_SRC_HDMI);

            /* Allocate resources, Enable and configure CSC, CIG and HDMI Blender, write back CSC;
               Disable HDVENC_D/HDMI VENC clock, otherwise this m2m driver will not work
               VRM_RESOURCE_HD1_PATH, VRM_RESOURCE_HDMI_BLEND*/
//              retVal |= VpsMdrvCompInitDctrlPath(VPS_M2M_INST_COMP_BP0_GRPX_CSC_BLND1_SC5_WB);


            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR, "Could not set SC5 input mux!\n");
            }
        }

        /* TODO:Add for other flavours of blender drivers when supported */
    }

    /* If error free the already allocated resource */
    if (FVID2_SOK != retVal)
    {
        for (errCnt = 0u; errCnt < coreCnt; errCnt++)
        {
            /* Free resource only if already allocated */
            if (TRUE == instObj->isCoreReq[errCnt])
            {
                errRetVal = Vrm_releaseResource(instObj->resrcId[errCnt]);
                GT_assert(VpsMdrvCompTrace, (FVID2_SOK == errRetVal));
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompFreeResrc
 *  Frees the resources allocated by the driver from resource manager.
 */
Int32 vpsMdrvCompFreeResrc(VpsMdrv_CompInstObj *instObj)
{
    Int32       retVal = FVID2_SOK;
    UInt32      coreCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Free resource only if previously allocated by driver */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            if (VRM_RESOURCE_INVALID == instObj->resrcId[coreCnt])
            {
                continue;
            }
            retVal = Vrm_releaseResource(instObj->resrcId[coreCnt]);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not free resource %d!\n",
                    instObj->resrcId[coreCnt]);
                break;
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompGetInstObj
 *  Returns the instance object pointer for the instance id.
 */
VpsMdrv_CompInstObj *vpsMdrvCompGetInstObj(UInt32 instId)
{
    UInt32              instCnt;
    VpsMdrv_CompInstObj *instObj = NULL;

    /* Find out the instance to which this channel belongs to. */
    for (instCnt = 0u; instCnt < VPS_M2M_COMP_INST_MAX; instCnt++)
    {
        if (VpsMdrvCompInstObjects[instCnt].drvInstId == instId)
        {
            instObj = &VpsMdrvCompInstObjects[instCnt];
            break;
        }
    }

    return (instObj);
}



/**
 *  vpsMdrvCompAllocHandleObj
 *  Allocates handle object and all other memories of the handle like handle
 *  queues, descriptor/shadow overlay memories, channel objects etc.
 *  Returns the handle object pointer on success else returns NULL.
 */
VpsMdrv_CompHandleObj *vpsMdrvCompAllocInitHandleObj(
                          Vps_M2mCompCreateParams *createPrms,
                          VpsMdrv_CompInstObj *instObj,
                          const FVID2_DrvCbParams *fdmCbPrms,
                          Int32 *retValPtr)
{
    Int32                       retVal = FVID2_SOK;
    UInt32                      qCnt;
    UInt32                      chCnt;
    UInt32                      descSet;
    UInt8                      *descPtr;
    Semaphore_Params            semPrms;
    VpsMdrv_CompQueueObj        *qObj;
    VpsMdrv_CompHandleObj       *hObj = NULL;
    UInt32                      totalDescMem = 0u;
    UInt32                      totalShwOvlyMem = 0u, totalNshwOvlyMem = 0u;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != createPrms));
    GT_assert(VpsMdrvCompTrace, (NULL != createPrms->chParams));
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));
    GT_assert(VpsMdrvCompTrace, (NULL != fdmCbPrms));
    GT_assert(VpsMdrvCompTrace, (NULL != retValPtr));

    /* Allocate handle memory */
    hObj = (VpsMdrv_CompHandleObj *) VpsUtils_alloc(
                                        &gVpsMdrvCompPoolObj.handlePoolPrm,
                                        sizeof (VpsMdrv_CompHandleObj),
                                        VpsMdrvCompTrace);
    if (NULL == hObj)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Handle object memory alloc failed!!\n");
        retVal = FVID2_EALLOC;
    }
    else
    {
        /* Reset some of the important fields used by free function so that
         * when error occurs, we won't free un-allocated pointers!! */
        hObj->numCh = 0u;
        hObj->reqQ = NULL;
        hObj->doneQ = NULL;
        hObj->freeQ = NULL;
        hObj->ctrlSem = NULL;
        hObj->descMem = NULL;
        hObj->shwOvlyMem = NULL;
        hObj->nshwOvlyMem = NULL;
        hObj->numCfgDesc = 0u;
        for (chCnt = 0u; chCnt < VPS_M2M_COMP_MAX_CH_PER_INST; chCnt++)
        {
            hObj->chObjs[chCnt] = NULL;
        }
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            hObj->firstDesc[descSet] = NULL;
            hObj->shwCfgDesc[descSet] = NULL;
            hObj->nshwCfgDesc[descSet] = NULL;
            hObj->rldDesc[descSet] = NULL;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* If per handle config flag is set, then the handle will have config
         * descriptor. */
        if (VPS_M2M_CONFIG_PER_HANDLE == createPrms->mode)
        {
            hObj->numCfgDesc = VPSMDRV_COMP_MAX_SHW_CFG_DESC;
        }
        else
        {
            hObj->numCfgDesc = 0u;
        }

        /* Calculate the total descriptor memory and overlay memory
         * required per handle */
        totalDescMem = (hObj->numCfgDesc * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
        totalDescMem += VPSHAL_VPDMA_CONFIG_DESC_SIZE;  /* For non shadow */
        totalDescMem +=
            (VPSMDRV_COMP_MAX_RLD_DESC * VPSHAL_VPDMA_CTRL_DESC_SIZE);

        /* Allocate for all the sets */
        totalDescMem *= VPSMDRV_COMP_MAX_DESC_SET;
        /* For overlay, allocate full mem as still cores are not initialized */
        totalShwOvlyMem = VPSMDRV_COMP_MAX_SHW_OVLY_MEM;
        totalNshwOvlyMem = VPSMDRV_COMP_MAX_NSHW_OVLY_MEM;

        /* Allocate descriptor and non shadow ovly memories used per handle */
        hObj->descMem = VpsUtils_allocDescMem(
                            totalDescMem,
                            VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        hObj->nshwOvlyMem = VpsUtils_allocDescMem(
                                totalNshwOvlyMem,
                                VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
        if ((NULL == hObj->descMem) || (NULL == hObj->nshwOvlyMem))
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR,
                "Could not allocate handle descriptor/non-shadow overlay!\n");
            retVal = FVID2_EALLOC;
        }
        else
        {
            hObj->totalDescMem = totalDescMem;
            hObj->totalNshwOvlyMem = totalNshwOvlyMem;
            /* Allocate the overlay memories used per handle if needed */
            if (hObj->numCfgDesc > 0u)
            {
                hObj->shwOvlyMem = VpsUtils_allocDescMem(
                                        totalShwOvlyMem,
                                        VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
                if (NULL == hObj->shwOvlyMem)
                {
                    GT_0trace(VpsMdrvCompTrace, GT_ERR,
                        "Could not allocate handle shadow overlay memory!\n");
                    retVal = FVID2_EALLOC;
                }
                else
                {
                    hObj->totalShwOvlyMem = totalShwOvlyMem;
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Split the descriptor memory for handle */
        descPtr = hObj->descMem;
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            hObj->firstDesc[descSet] = descPtr;
            hObj->nshwCfgDesc[descSet] = (VpsHal_VpdmaConfigDesc *) descPtr;
            descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            if (hObj->numCfgDesc > 0u)
            {
                hObj->shwCfgDesc[descSet] = (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += VPSHAL_VPDMA_CONFIG_DESC_SIZE;
            }
            else
            {
                hObj->shwCfgDesc[descSet] = NULL;
            }

            /* Assign reload descriptor */
            hObj->rldDesc[descSet] = (VpsHal_VpdmaReloadDesc *) descPtr;
            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Create the different queues - request, done and free queues */
        hObj->reqQ = VpsUtils_createQ();
        hObj->doneQ = VpsUtils_createQ();
        hObj->freeQ = VpsUtils_createQ();
        if ((NULL == hObj->reqQ) ||
            (NULL == hObj->doneQ) ||
            (NULL == hObj->freeQ))
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR, "Queue creation failed!!\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Create semaphore */
        Semaphore_Params_init(&semPrms);
        hObj->ctrlSem = Semaphore_create(0u, &semPrms, NULL);
        if (NULL == hObj->ctrlSem)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR,
                "Control semaphore create failed!!\n");
            retVal = FVID2_EALLOC;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate queue object memory and queue them to free queue */
        for (qCnt = 0u; qCnt < VPSMDRV_COMP_MAX_QOBJS_PER_HANDLE; qCnt++)
        {
            /* Allocate queue object memory */
            qObj = (VpsMdrv_CompQueueObj *) VpsUtils_alloc(
                                               &gVpsMdrvCompPoolObj.qObjPoolPrm,
                                               sizeof(VpsMdrv_CompQueueObj),
                                               VpsMdrvCompTrace);
            if (NULL == qObj)
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR,
                    "Queue object memory alloc failed!!\n");
                retVal = FVID2_EALLOC;
                break;
            }

            /* Initialize and queue the allocate queue object to free Q */
            qObj->hObj = hObj;
            qObj->descSetInUse = VPSMDRV_COMP_INVALID_DESC_SET;
            VpsUtils_queue(hObj->freeQ, &qObj->qElem, qObj);
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Allocate channel object */
        retVal = vpsMdrvCompAllocChObjs(hObj, createPrms->numCh);
        if (FVID2_SOK != retVal)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR,
                "Channel object memory alloc failed!!\n");
        }
        else
        {
            /* Copy the channel parameters */
            hObj->mode = createPrms->mode;
            hObj->numCh = createPrms->numCh;
            retVal = vpsMdrvCompCopyChPrms(hObj, createPrms->chParams);
        }
    }

    if (FVID2_SOK == retVal)
    {
        hObj->parent = instObj;
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            /* Mark both descriptor set is free for use */
            hObj->isDescSetFree[descSet] = TRUE;
        }

        /* Copy the create parameters */
        hObj->numPendReq = 0u;
        hObj->numVpdmaChannels = 0u;
        hObj->syncMode = VPSMDRV_COMP_DEFAULT_SYNC_MODE;

        /* Copy the callback params */
        hObj->fdmCbPrms.fdmCbFxn = fdmCbPrms->fdmCbFxn;
        hObj->fdmCbPrms.fdmData = fdmCbPrms->fdmData;
        hObj->fdmCbPrms.fdmErrCbFxn = fdmCbPrms->fdmErrCbFxn;
        hObj->fdmCbPrms.errList = fdmCbPrms->errList;
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        if (NULL != hObj)
        {
            vpsMdrvCompFreeHandleObj(hObj);
            hObj = NULL;
        }
    }

    *retValPtr = retVal;
    return (hObj);
}



/**
 *  vpsMdrvCompFreeHandleObj
 *  Frees the handle object and all other memories of the handle like handle
 *  queues, descriptor/shadow overlay memories, channel objects etc.
 */
Int32 vpsMdrvCompFreeHandleObj(VpsMdrv_CompHandleObj *hObj)
{
    Int32                   retVal = FVID2_SOK, tempRetVal;
    UInt32                  chCnt, descSet;
    Vps_M2mCompChParams     *chPrms;
    VpsMdrv_CompQueueObj    *qObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));

    /* Reset the channel parameters. Esp set the pointers to NULL */
    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        /* NULL pointer check */
        GT_assert(VpsMdrvCompTrace, (NULL != hObj->chObjs[chCnt]));
        chPrms = &hObj->chObjs[chCnt]->chPrms;
        chPrms->sc5Cfg = NULL;
        chPrms->grpxPrms = NULL;
        chPrms->sc5CropCfg = NULL;
    }

    /* Free channel objects */
    retVal = vpsMdrvCompFreeChObjs(hObj, hObj->numCh);
    hObj->numCh = 0u;

    if (NULL != hObj->freeQ)
    {
        /* Free-up all the queued free queue objects */
        while (1u)
        {
            qObj = VpsUtils_dequeue(hObj->freeQ);
            if (NULL == qObj)
            {
                /* No more in queue */
                break;
            }

            /* Free queue object memory */
            qObj->hObj = NULL;
            tempRetVal = VpsUtils_free(
                             &gVpsMdrvCompPoolObj.qObjPoolPrm,
                             qObj,
                             VpsMdrvCompTrace);
            GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
        }
    }

    if (NULL != hObj->reqQ)
    {
        /* Delete the request Q */
        VpsUtils_deleteQ(hObj->reqQ);
        hObj->reqQ = NULL;
    }

    if (NULL != hObj->doneQ)
    {
        /* Delete the done Q */
        VpsUtils_deleteQ(hObj->doneQ);
        hObj->doneQ = NULL;
    }

    if (NULL != hObj->freeQ)
    {
        /* Delete the free Q */
        VpsUtils_deleteQ(hObj->freeQ);
        hObj->freeQ = NULL;
    }

    if (NULL != hObj->ctrlSem)
    {
        /* Delete the control semaphore */
        Semaphore_delete(&hObj->ctrlSem);
        hObj->ctrlSem = NULL;
    }

    /* Free the handle descriptor memories */
    if (NULL != hObj->descMem)
    {
        tempRetVal = VpsUtils_freeDescMem(
                         hObj->descMem,
                         hObj->totalDescMem);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
        hObj->totalDescMem = 0u;
        hObj->descMem = NULL;
    }
    if (NULL != hObj->shwOvlyMem)
    {
        tempRetVal = VpsUtils_freeDescMem(
                         hObj->shwOvlyMem,
                         hObj->totalShwOvlyMem);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
        hObj->shwOvlyMem = NULL;
        hObj->totalShwOvlyMem = 0u;
    }
    if (NULL != hObj->nshwOvlyMem)
    {
        tempRetVal = VpsUtils_freeDescMem(
                         hObj->nshwOvlyMem,
                         hObj->totalNshwOvlyMem);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
        hObj->nshwOvlyMem = NULL;
        hObj->totalNshwOvlyMem = 0u;
    }
    hObj->numCfgDesc = 0u;
    for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
    {
        hObj->firstDesc[descSet] = NULL;
        hObj->shwCfgDesc[descSet] = NULL;
        hObj->nshwCfgDesc[descSet] = NULL;
        hObj->rldDesc[descSet] = NULL;
    }

    /* Reset the variables */
    hObj->parent = NULL;
    hObj->numVpdmaChannels = 0u;
    hObj->syncMode = VPSMDRV_COMP_DEFAULT_SYNC_MODE;
    hObj->fdmCbPrms.fdmCbFxn = NULL;
    hObj->fdmCbPrms.fdmData = NULL;
    hObj->fdmCbPrms.fdmErrCbFxn = NULL;
    hObj->fdmCbPrms.errList = NULL;

    /* Free handle object memory */
    tempRetVal = VpsUtils_free(
                     &gVpsMdrvCompPoolObj.handlePoolPrm,
                     hObj,
                     VpsMdrvCompTrace);
    GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));

    return (retVal);
}

/**
 *  vpsMdrvCompCfgBpCore
 *  Configures the BP core according to the channel parameters.
 */
static Int32 vpsMdrvCompCfgBpCore(VpsMdrv_CompHandleObj *hObj,
                                  VpsMdrv_CompChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Format        inVcoreFmt;
    Vps_M2mCompChParams *chParams;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    GT_assert(VpsMdrvCompTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps));

    /* copy ch params to coreCfg structure */
    chParams = &(chObj->chPrms);
    inVcoreFmt.memType = chParams->inMemType;
    inVcoreFmt.frameWidth = chParams->inFmt.width;
    inVcoreFmt.frameHeight = chParams->inFmt.height;
    inVcoreFmt.startX = 0;
    inVcoreFmt.startY = 0;
    inVcoreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;
    VpsUtils_memcpy(&inVcoreFmt.fmt, &chParams->inFmt,
        sizeof(FVID2_Format));
    GT_assert(VpsMdrvCompTrace,
        (NULL != coreOps->setFormat));

    /* Set the format and other parameters for the cores */
    retVal = coreOps->setFormat(
                 coreHandle,
                 chObj->chNum,
                 &inVcoreFmt);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Set params for BP core failed!\n");
    }

    return (retVal);
}

/**
 *  vpsMdrvCompCfgScwrbkCore
 *  Configures the SCWB core according to the channel parameters.
 */
static Int32 vpsMdrvCompCfgScwrbkCore(VpsMdrv_CompHandleObj *hObj,
                                  VpsMdrv_CompChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Format        outVcoreFmt;
    Vcore_SwpParams     swpParams;
    Vps_M2mCompChParams *chParams;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    GT_assert(VpsMdrvCompTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj->chPrms.outFmtSc5));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj->chPrms.sc5Cfg));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj->chPrms.sc5CropCfg));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps->setParams));

    /* copy ch params to coreCfg structures */
    chParams = &(chObj->chPrms);
    outVcoreFmt.frameWidth = chParams->outFmtSc5->width;
    outVcoreFmt.frameHeight = chParams->outFmtSc5->height;
    outVcoreFmt.startX = 0;
    outVcoreFmt.startY = 0;
    outVcoreFmt.secScanFmt = chParams->outFmtSc5->scanFormat;
    outVcoreFmt.memType = chParams->outMemTypeSc5;
    swpParams.srcWidth =  chParams->inFmt.width;
    swpParams.srcHeight = chParams->inFmt.height;
    VpsUtils_memcpy(&outVcoreFmt.fmt, chParams->outFmtSc5,
        sizeof(FVID2_Format));
    VpsUtils_memcpy(&swpParams.scCfg,
        chParams->sc5Cfg,
        sizeof(Vps_ScConfig));
    VpsUtils_memcpy(&swpParams.srcCropCfg,
        chParams->sc5CropCfg,
        sizeof(Vps_CropConfig));

    /* Set the format and other parameters for the cores */
    GT_assert(VpsMdrvCompTrace,
        (NULL != coreOps->setParams));
    retVal = coreOps->setParams(
                            coreHandle,
                            chObj->chNum,
                            &swpParams);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Set params for SCWRBK core failed!\n");
    }
    GT_assert(VpsMdrvCompTrace,
        (NULL != coreOps->setFormat));
    retVal += coreOps->setFormat(
                            coreHandle,
                            chObj->chNum,
                            &outVcoreFmt);

    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Set format for SCWRBK core failed!\n");
    }

    return (retVal);

}

/**
 *  vpsMdrvCompCfgDctrlCore
 *  Configures the CSC+CIG core according to the channel parameters.
 */
static Int32 vpsMdrvCompCfgDctrlCore(VpsMdrv_CompHandleObj *hObj,
                                  VpsMdrv_CompChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    Vcore_Format        inVcoreFmt;
    Vps_M2mCompChParams *chParams;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    GT_assert(VpsMdrvCompTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps));

    /* copy ch params to coreCfg structure: input and CSC+CIG core
    formats are same */
    chParams = &(chObj->chPrms);
    inVcoreFmt.memType = chParams->inMemType;
    inVcoreFmt.frameWidth = chParams->inFmt.width;
    inVcoreFmt.frameHeight = chParams->inFmt.height;
    inVcoreFmt.startX = 0;
    inVcoreFmt.startY = 0;
    inVcoreFmt.secScanFmt = FVID2_SF_PROGRESSIVE;
    VpsUtils_memcpy(&inVcoreFmt.fmt, &chParams->inFmt,
        sizeof(FVID2_Format));
    GT_assert(VpsMdrvCompTrace,
        (NULL != coreOps->setFormat));

    /* Set the format and other parameters for the cores */
    retVal = coreOps->setFormat(
                 coreHandle,
                 chObj->chNum,
                 &inVcoreFmt);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Set params for BP core failed!\n");
    }

    return (retVal);
}

/**
 *  vpsMdrvCompOpenCores
 *  Opens and configures all the cores required for this handle.
 */
Int32 vpsMdrvCompOpenCores(VpsMdrv_CompHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              perChCfg;
    UInt32              chCnt, coreCnt;
    VpsMdrv_CompInstObj *instObj;
    const Vcore_Ops    *coreOps;
    Vcore_Property      property;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
    {
        perChCfg = FALSE;
    }
    else
    {
        perChCfg = TRUE;
    }

    /* Reset the handles first */
    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        hObj->coreHandle[coreCnt] = NULL;
    }

    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Open and configure core only if needed by that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->open));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->getProperty));

            /* Open core */
            hObj->coreHandle[coreCnt] = coreOps->open(
                                            instObj->coreInstId[coreCnt],
                                            VCORE_OPMODE_MEMORY,
                                            hObj->numCh,
                                            perChCfg);
            if (NULL == hObj->coreHandle[coreCnt])
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not open core %d!\n", coreCnt);
                retVal = FVID2_EALLOC;
                break;
            }

            /* Get core property */
            retVal = coreOps->getProperty(hObj->coreHandle[coreCnt], &property);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not get property of core %d!\n", coreCnt);
                break;
            }

            /* Configure all channels of the core */
            for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
            {
                if (VCORE_TYPE_BP== property.name)
                {
                    /* Configure BP core */
                    retVal = vpsMdrvCompCfgBpCore(
                                 hObj,
                                 hObj->chObjs[chCnt],
                                 hObj->coreHandle[coreCnt],
                                 coreOps);
                }
                else if (VCORE_TYPE_SC_WB2== property.name)
                {
                    /* Configure SCWRBK core */
                    retVal = vpsMdrvCompCfgScwrbkCore(
                                 hObj,
                                 hObj->chObjs[chCnt],
                                 hObj->coreHandle[coreCnt],
                                 coreOps);
                }
            /* displayCtrl Core init*/
                else if (VCORE_TYPE_CSC_CIG == property.name)
                {
                    /* Configure DCTRL core */
                    retVal = vpsMdrvCompCfgDctrlCore(
                                 hObj,
                                 hObj->chObjs[chCnt],
                                 hObj->coreHandle[coreCnt],
                                 coreOps);
                }
                else
                {
                    GT_1trace(VpsMdrvCompTrace, GT_ERR,
                        "Core type %d not supported\n", property.type);
                    retVal = FVID2_EFAIL;
                }

                /* Break if error occurs */
                if (FVID2_SOK != retVal)
                {
                    GT_2trace(VpsMdrvCompTrace, GT_ERR,
                        "Config of core %d for channel %d failed.\n",
                        coreCnt, hObj->chObjs[chCnt]->chNum);
                    break;
                }
            }

            /* If error occurs break from outer loop as well */
            if (FVID2_SOK != retVal)
            {
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Get the VPDMA channels for which to program the sync on client */
        retVal = vpsMdrvCompGetSocCh(hObj);
    }

    /* Close handles if error occurs */
    if (FVID2_SOK != retVal)
    {
        vpsMdrvCompCloseCores(hObj);
    }

    return (retVal);
}



/**
 *  vpsMdrvCompCloseCores
 *  Closes all the cores opened for this handle.
 */
Int32 vpsMdrvCompCloseCores(VpsMdrv_CompHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              coreCnt;
    VpsMdrv_CompInstObj *instObj;
    const Vcore_Ops    *coreOps;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Close cores only if already opened for that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->close));

            if (NULL != hObj->coreHandle[coreCnt])
            {
                retVal = coreOps->close(hObj->coreHandle[coreCnt]);
                GT_assert(VpsMdrvCompTrace, (FVID2_SOK == retVal));
                hObj->coreHandle[coreCnt] = NULL;
            }
        }
    }

    return (retVal);
}




/**
 *  vpsMdrvCompAllocCoreMem
 *  Allocates the required descriptor and overlay memories for all the channels
 *  and for all the cores for this handle. It also slices the descriptors and
 *  overlay memory and init the core desc memory structure.
 *  It also allocates the context buffer required by DEI core.
 */
Int32 vpsMdrvCompAllocCoreMem(VpsMdrv_CompHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt;
    UInt32              perChCfg;
    VpsMdrv_CompChObj   *chObj;
    VpsMdrv_CompInstObj *instObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    if (VPS_M2M_CONFIG_PER_HANDLE == hObj->mode)
    {
        perChCfg = FALSE;
    }
    else
    {
        perChCfg = TRUE;
    }

    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        GT_assert(VpsMdrvCompTrace, (NULL != chObj));

        /* Allocate desc memory for each channel */
        retVal = vpsMdrvCompAllocChDescMem(hObj, chObj, perChCfg);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Could not allocate descrioptor for channel: %d!\n",
                chObj->chNum);
            break;
        }

        /* Split the allocated descriptors across cores */
        retVal = vpsMdrvCompSplitChDescMem(hObj, chObj);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Error slicing descrioptor for channel: %d!\n", chObj->chNum);
            break;
        }

        /* Figure out context memory for each channel, if needed */
    }

    if (FVID2_SOK == retVal)
    {
        /* Split NonShadow Overlay Memory */
        retVal = vpsMdrvCompSplitNonShoadowMem(hObj);
    }

    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        vpsMdrvCompFreeCoreMem(hObj);
    }

    return (retVal);
}


/**
 *  vpsMdrvCompFreeCoreMem
 *  Frees the allocated descriptors and overlay memories for all the channels
 *  and for all the cores for this handle.
 *  It also frees the context buffer allocated for DEI core.
 */
Int32 vpsMdrvCompFreeCoreMem(VpsMdrv_CompHandleObj *hObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  chCnt, descSet, coreCnt, cnt;
    VpsMdrv_CompChObj       *chObj;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));

    /* Free desc memory for each channel */
    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        chObj = hObj->chObjs[chCnt];
        GT_assert(VpsMdrvCompTrace, (NULL != chObj));

        /* Free channel descriptor memory */
        retVal = vpsMdrvCompFreeChDescMem(chObj);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == retVal));

        /* Free channel context buffer memory, if allocated */

        /* Reset the dependent pointers */
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            chObj->firstDesc[descSet] = NULL;
            chObj->shwCfgDesc[descSet] = NULL;
            chObj->socDesc[descSet] = NULL;
            chObj->rldDesc[descSet] = NULL;
            for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
            {
                for (cnt = 0u; cnt < VCORE_MAX_IN_DESC; cnt++)
                {
                    chObj->coreDescMem[descSet][coreCnt].inDataDesc[cnt]
                        = NULL;
                }
                for (cnt = 0u; cnt < VCORE_MAX_OUT_DESC; cnt++)
                {
                    chObj->coreDescMem[descSet][coreCnt].outDataDesc[cnt]
                        = NULL;
                }
                for (cnt = 0u; cnt < VCORE_MAX_MULTI_WIN_DESC; cnt++)
                {
                    chObj->coreDescMem[descSet][coreCnt].multiWinDataDesc[cnt]
                        = NULL;
                }
                chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem = NULL;
            }
        }

    }

    return (retVal);
}

/**
 *  vpsMdrvCompProgramDesc
 *  Program the core descriptors and overlay memories for all the channels
 *  of the handle. This also programs the handle descriptor and overlay
 *  memory if per handle config is true.
 */
Int32 vpsMdrvCompProgramDesc(VpsMdrv_CompHandleObj *hObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, coreCnt /*,descSet*/;
    VpsMdrv_CompInstObj *instObj;
    const Vcore_Ops    *coreOps;
/*    Vcore_DescMem       descMem;
    UInt8              *descPtr;
*/
    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        /* Program descriptor and overlay memory for each channel */
        retVal = vpsMdrvCompProgramChDesc(hObj, hObj->chObjs[chCnt]);
        if (FVID2_SOK != retVal)
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Could not program descriptors for channel %d!\n",
                hObj->chObjs[chCnt]->chNum);
            break;
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Program the frame start event for the cores - only one time
         * is sufficient */
        /* TODO: FIXME - Changing the frame start event before getting the
         * resource. Bug: When DEI display is going, opening of this driver will
         * overwrite the frame start from LM FID change to channel active!! */
        if (0u == instObj->openCnt)
        {
            for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
            {
                /* Program cores only if needed for that instance */
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    coreOps = instObj->coreOps[coreCnt];
                    /* NULL pointer check */
                    GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
                    GT_assert(VpsMdrvCompTrace, (NULL != coreOps->setFsEvent));
                    retVal = coreOps->setFsEvent(
                                 hObj->coreHandle[coreCnt],
                                 0u,
                                 VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE,
                                 NULL);
                    if (FVID2_SOK != retVal)
                    {
                        GT_1trace(VpsMdrvCompTrace, GT_ERR,
                            "Could not set frame start for core %d!\n",
                            coreCnt);
                        break;
                    }
                }
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
#if 0 // ?TODO: no nshdw mem for 1st driver */
        /* Update the non-shadow descriptor of handle */
        GT_assert(VpsMdrvCompTrace, (NULL != hObj->nshwOvlyMem));
        GT_assert(VpsMdrvCompTrace, (0u != hObj->totalNshwOvlyMem));

        descPtr = hObj->nshwOvlyMem;
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            VpsHal_vpdmaCreateConfigDesc(
                hObj->nshwCfgDesc[descSet],
                VPSHAL_VPDMA_CONFIG_DEST_MMR,
                VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                VPSHAL_VPDMA_CCT_INDIRECT,
                (hObj->totalNshwOvlyMem / VPSMDRV_COMP_MAX_DESC_SET),
                descPtr,
                NULL,
                0u);

            /* Program frame start event of  core in overlay memory */
            coreOps = instObj->coreOps[VPSMDRV_COMP_CORE_IN_VIDEO_IDX];
            GT_assert(VpsMdrvCompTrace,
                (TRUE == instObj->isCoreReq[VPSMDRV_COMP_CORE_IN_VIDEO_IDX]));
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->setFsEvent));
            GT_assert(VpsMdrvCompTrace, (NULL != hObj->chObjs[0u]));

            /* Since the non-shadow overlay memory is same for all the
               channels, its pointer is taken from channel-0, It is not
               taken from handle object becuase complete non-shadow memory
               is sliced as per the other cores requirement. */
            descMem.nonShadowOvlyMem =
                hObj->chObjs[0u]->coreDescMem[descSet]
                    [VPSMDRV_COMP_CORE_IN_VIDEO_IDX].nonShadowOvlyMem;
            retVal = coreOps->setFsEvent(
                         hObj->coreHandle[VPSMDRV_COMP_CORE_IN_VIDEO_IDX],
                         0u,
                         VPSHAL_VPDMA_FSEVENT_CHANNEL_ACTIVE,
                         &descMem);
            if (FVID2_SOK != retVal)
            {
                GT_0trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not set frame start for DEI Core!\n");
                break;
            }

            descPtr += (hObj->totalNshwOvlyMem / VPSMDRV_COMP_MAX_DESC_SET);
        }
#endif
    }
    return (retVal);
}



/**
 *  vpsMdrvCompAllocCoeffDesc
 */
Void *vpsMdrvCompAllocCoeffDesc(void)
{
    Void       *coeffDesc;

    /* Allocate coeff descriptor memory */
    coeffDesc = VpsUtils_allocDescMem(
                    VPSMDRV_COMP_MAX_COEFF_DESC_MEM,
                    VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);

    return (coeffDesc);
}



/**
 *  vpsMdrvCompFreeCoeffDesc
 */
Int32 vpsMdrvCompFreeCoeffDesc(Void *coeffDesc)
{
    Int32       retVal;

    /* Free coeff descriptor memory */
    retVal = VpsUtils_freeDescMem(
                 coeffDesc,
                 VPSMDRV_COMP_MAX_COEFF_DESC_MEM);

    return (retVal);
}



/**
 *  vpsMdrvCompAllocChDescMem
 *  Allocate the required descriptors and overlay memories for the channel
 *  according to the requirement of all the cores.
 */
static Int32 vpsMdrvCompAllocChDescMem(VpsMdrv_CompHandleObj *hObj,
                                      VpsMdrv_CompChObj *chObj,
                                      UInt32 perChCfg)
{
    Int32               retVal = FVID2_SOK, tempRetVal;
    UInt32              coreCnt;
    Vcore_DescInfo      descInfo;
    const Vcore_Ops    *coreOps;
    VpsMdrv_CompInstObj *instObj;
    UInt32              totalDescMem;
    UInt32              totalShwOvlyMem, totalNshwOvlyMem;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    /* Reset variables */
    totalDescMem = 0u;
    totalShwOvlyMem = 0u;
    totalNshwOvlyMem = 0u;
    GT_assert(VpsMdrvCompTrace, (NULL == chObj->descMem));
    GT_assert(VpsMdrvCompTrace, (NULL == chObj->shwOvlyMem));
    chObj->descMem = NULL;
    chObj->shwOvlyMem = NULL;

    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->getDescInfo));

            /* Get the descriptor info for each core */
            retVal = coreOps->getDescInfo(
                         hObj->coreHandle[coreCnt],
                         chObj->chNum,
                         0u,
                         &descInfo);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not get core info for core %d!\n", coreCnt);
                break;
            }

            /* Accumulate the info in total info variable */
            totalDescMem += (descInfo.numInDataDesc
                * VPSHAL_VPDMA_DATA_DESC_SIZE);
            totalDescMem += (descInfo.numOutDataDesc
                * VPSHAL_VPDMA_DATA_DESC_SIZE);
            totalDescMem += (descInfo.numMultiWinDataDesc
                * VPSHAL_VPDMA_DATA_DESC_SIZE);
            totalShwOvlyMem += descInfo.shadowOvlySize;
            totalNshwOvlyMem += descInfo.nonShadowOvlySize;
            /* Get the coefficient information */
            chObj->horzCoeffOvlySize[coreCnt] = descInfo.horzCoeffOvlySize;
            chObj->vertCoeffOvlySize[coreCnt] = descInfo.vertCoeffOvlySize;
            chObj->vertBilinearCoeffOvlySize[coreCnt] =
                                            descInfo.vertBilinearCoeffOvlySize;
            chObj->coeffConfigDest[coreCnt] = descInfo.coeffConfigDest;
            if (   (0u != chObj->horzCoeffOvlySize[coreCnt])
                && (0u != chObj->vertCoeffOvlySize[coreCnt]))
            {
                chObj->scPresent[coreCnt] = TRUE;
            }
            else
            {
                chObj->scPresent[coreCnt] = FALSE;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /*
         * If per channel config is TRUE, allocate mem for shadow config
         * descriptor in main descriptor memory of the channel.
         */
        if (TRUE == perChCfg)
        {
            totalDescMem +=
                (VPSMDRV_COMP_MAX_SHW_CFG_DESC * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            chObj->numCfgDesc = VPSMDRV_COMP_MAX_SHW_CFG_DESC;
        }
        else
        {
            chObj->numCfgDesc = 0u;
        }

        /* Allocate for SOC/SOCH control descriptors */
        if (VPSMDRV_SYNC_MODE_SOCH == hObj->syncMode)
        {
            /* Use all channels returned by the cores as SOCH */
            GT_assert(VpsMdrvCompTrace,
                (hObj->numVpdmaChannels <= VPSMDRV_COMP_MAX_SOC_DESC));
            totalDescMem +=
                (hObj->numVpdmaChannels * VPSHAL_VPDMA_CTRL_DESC_SIZE);
        }
        else
        {
            /* Only one SOC is used */
            totalDescMem += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }

        /* Allocate for reload descriptor in main descriptor memory */
        totalDescMem +=
            (VPSMDRV_COMP_MAX_RLD_DESC * VPSHAL_VPDMA_CTRL_DESC_SIZE);

        /* Allocate descriptor and overlay memory per channel for all
         * the sets. */
        totalDescMem *= VPSMDRV_COMP_MAX_DESC_SET;
        totalShwOvlyMem *= VPSMDRV_COMP_MAX_DESC_SET;
        totalNshwOvlyMem *= VPSMDRV_COMP_MAX_DESC_SET;

        /* Allocate memory */
        chObj->descMem = VpsUtils_allocDescMem(
                             totalDescMem,
                             VPSHAL_VPDMA_DESC_BYTE_ALIGN);
        if (NULL == chObj->descMem)
        {
            GT_0trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not allocate channel descrioptor!\n");
            retVal = FVID2_EALLOC;
        }
        else
        {
            /* Set the total count */
            chObj->totalDescMem = totalDescMem;

            /* Assign config overlay memory if needed */
            if (chObj->numCfgDesc > 0u)
            {
                chObj->shwOvlyMem = VpsUtils_allocDescMem(
                                        totalShwOvlyMem,
                                        VPSHAL_VPDMA_CONFIG_PAYLOAD_BYTE_ALIGN);
                if (NULL == chObj->shwOvlyMem)
                {
                    GT_0trace(VpsMdrvCompTrace, GT_ERR,
                        "Could not allocate channel shadow overlay memory!\n");
                    retVal = FVID2_EALLOC;
                }
                else
                {
                    /* Set the total count */
                    chObj->totalShwOvlyMem = totalShwOvlyMem;
                }
            }
            else
            {
                chObj->shwOvlyMem = NULL;
            }
        }
    }


    /* Deallocate if error occurs */
    if (FVID2_SOK != retVal)
    {
        tempRetVal = vpsMdrvCompFreeChDescMem(chObj);
        GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
    }
    else
    {
        /* Check if already allocated overlay memory is sufficient */
        GT_assert(VpsMdrvCompTrace,
            (hObj->totalNshwOvlyMem >= totalNshwOvlyMem));
        hObj->totalNshwOvlyMem = totalNshwOvlyMem;
        if (hObj->numCfgDesc > 0u)
        {
            GT_assert(VpsMdrvCompTrace,
                (hObj->totalShwOvlyMem >= totalShwOvlyMem));
            hObj->totalShwOvlyMem = totalShwOvlyMem;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompFreeChDescMem
 *  Frees the allocated descriptors and overlay memories for the channel.
 */
static Int32 vpsMdrvCompFreeChDescMem(VpsMdrv_CompChObj *chObj)
{
    Int32       retVal = FVID2_SOK;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));

    if (NULL != chObj->descMem)
    {
        retVal = VpsUtils_freeDescMem(chObj->descMem, chObj->totalDescMem);
        chObj->descMem = NULL;
    }

    if (NULL != chObj->shwOvlyMem)
    {
        retVal |= VpsUtils_freeDescMem(
                      chObj->shwOvlyMem,
                      chObj->totalShwOvlyMem);
        chObj->shwOvlyMem = NULL;
    }

    /* Reset the total count */
    chObj->totalDescMem = 0u;
    chObj->totalShwOvlyMem = 0u;

    return (retVal);
}




/**
 *  vpsMdrvCompSplitChDescMem
 *  Splits the allocated descriptor and shadow overlay memories of a channel
 *  according to the requirement of each core.
 */
static Int32 vpsMdrvCompSplitChDescMem(VpsMdrv_CompHandleObj *hObj,
                                      VpsMdrv_CompChObj *chObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  descSet, coreCnt, cnt;
    Vcore_DescInfo          descInfo[VPSMDRV_COMP_MAX_CORE];
    const Vcore_Ops        *coreOps;
    VpsMdrv_CompInstObj     *instObj;
    UInt8                  *descPtr;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj->descMem));

    /* Get the descriptor info of all cores only once */
    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->getDescInfo));

            /* Get the descriptor info of each core */
            retVal = coreOps->getDescInfo(
                         hObj->coreHandle[coreCnt],
                         chObj->chNum,
                         0u,
                         &descInfo[coreCnt]);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not get core info for core %d!\n", coreCnt);
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Assign main data descriptor memory pointers */
        descPtr = chObj->descMem;
        GT_assert(VpsMdrvCompTrace, (NULL != descPtr));
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            /* Get a reference to the first descriptor pointer of the set */
            chObj->firstDesc[descSet] = descPtr;

            /* Assign config descriptor if needed */
            if (chObj->numCfgDesc > 0u)
            {
                chObj->shwCfgDesc[descSet] =
                    (VpsHal_VpdmaConfigDesc *) descPtr;
                descPtr += (chObj->numCfgDesc * VPSHAL_VPDMA_CONFIG_DESC_SIZE);
            }
            else
            {
                /* Point it to handle config descriptor */
                chObj->shwCfgDesc[descSet] = hObj->shwCfgDesc[descSet];
            }

            /*
             * Split the main descriptor and shadow overlay memory for
             * each core.
             */
            /* First assign out data descriptor for all cores */
            for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    for (cnt = 0u;
                         cnt < descInfo[coreCnt].numOutDataDesc;
                         cnt++)
                    {
                        chObj->coreDescMem[descSet][coreCnt].outDataDesc[cnt]
                            = descPtr;
                        descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
                    }
                }
            }

            /* Then assign in data descriptor for all cores */
            for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    for (cnt = 0u;
                         cnt < descInfo[coreCnt].numInDataDesc;
                         cnt++)
                    {
                        chObj->coreDescMem[descSet][coreCnt].inDataDesc[cnt]
                            = descPtr;
                        descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
                    }
                }
            }

            /* Then assign multi window data descriptor for all cores */
            for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    for (cnt = 0u;
                         cnt < descInfo[coreCnt].numMultiWinDataDesc;
                         cnt++)
                    {
                        chObj->coreDescMem[descSet][coreCnt]
                            .multiWinDataDesc[cnt] = descPtr;
                        descPtr += VPSHAL_VPDMA_DATA_DESC_SIZE;
                    }
                }
            }

            /* Assign SOC descriptor */
            chObj->socDesc[descSet] = (VpsHal_VpdmaSyncOnClientDesc *) descPtr;
            if (VPSMDRV_SYNC_MODE_SOCH == hObj->syncMode)
            {
                /* Use all channels returned by the cores as SOCH */
                GT_assert(VpsMdrvCompTrace,
                    (hObj->numVpdmaChannels <= VPSMDRV_COMP_MAX_SOC_DESC));
                descPtr +=
                    (hObj->numVpdmaChannels * VPSHAL_VPDMA_CTRL_DESC_SIZE);
            }
            else
            {
                /* Only one SOC is used */
                descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
            }

            /* Assign reload descriptor */
            chObj->rldDesc[descSet] = (VpsHal_VpdmaReloadDesc *) descPtr;
            descPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
        }


        /* Assign shadow overlay memory pointers */
        if (NULL != chObj->shwOvlyMem)
        {
            descPtr = chObj->shwOvlyMem;
        }
        else
        {
            /* If per config channel is FALSE, then use handle overlay memory
             * pointer */
            descPtr = hObj->shwOvlyMem;
        }
        GT_assert(VpsMdrvCompTrace, (NULL != descPtr));
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
            {
                if (TRUE == instObj->isCoreReq[coreCnt])
                {
                    if (descInfo[coreCnt].shadowOvlySize > 0u)
                    {
                        chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem
                            = descPtr;
                        descPtr += descInfo[coreCnt].shadowOvlySize;
                    }
                    else
                    {
                        chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem
                            = NULL;
                    }
                }
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompSplitNonShoadowMem
 *  Splits the non shadow overlay memories of the handle
 *  according to the requirement of each core.
 */
static Int32 vpsMdrvCompSplitNonShoadowMem(VpsMdrv_CompHandleObj *hObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  descSet, coreCnt, chCnt;
    const Vcore_Ops        *coreOps;
    VpsMdrv_CompInstObj     *instObj;
    VpsMdrv_CompChObj       *chObj;
    UInt8                  *descPtr;
    Vcore_DescInfo          descInfo;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    /* Sliced nonShadow Overlay Memory */
    descPtr = hObj->nshwOvlyMem;
    GT_assert(VpsMdrvCompTrace, (NULL != descPtr));
    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Get the Descriptor Information for this core */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->getDescInfo));

            /* Caution: Get the DescInfo for the channel, which is at the 0th
               index since it is only used here for getting nonshadow
               overlay size and nonshadow configuration is per handle.  */
            /* Get the descriptor info of each core */
            retVal = coreOps->getDescInfo(
                         hObj->coreHandle[coreCnt],
                         hObj->chObjs[0u]->chNum,
                         0u,
                         &descInfo);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not get core info for core %d!\n", coreCnt);
                break;
            }
            for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
            {
                chObj = hObj->chObjs[chCnt];
                GT_assert(VpsMdrvCompTrace, (NULL != chObj));

                for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
                {
                    /* Assign DescPtr to nonShadow pointer to descInfo
                       of all channels */
                    if (descInfo.nonShadowOvlySize > 0u)
                    {
                        chObj->coreDescMem[descSet][coreCnt].nonShadowOvlyMem
                                = descPtr;
                    }
                    else
                    {
                        chObj->coreDescMem[descSet][coreCnt].nonShadowOvlyMem
                                = NULL;
                    }
                }
            }

            if (descInfo.nonShadowOvlySize > 0u)
            {
                /* Increment descPtr */
                descPtr += descInfo.nonShadowOvlySize;
            }
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompProgramChDesc
 *  Program the core descriptors and overlay memories for a channel.
 */
static Int32 vpsMdrvCompProgramChDesc(VpsMdrv_CompHandleObj *hObj,
                                     VpsMdrv_CompChObj *chObj)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt, descSet, coreCnt;
    UInt8              *tempPtr;
    VpsMdrv_CompInstObj *instObj;
    const Vcore_Ops    *coreOps;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Program cores only if needed for that instance */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->programDesc));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->programReg));

            for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
            {
                /* Program the core descriptors */
                retVal = coreOps->programDesc(
                             hObj->coreHandle[coreCnt],
                             chObj->chNum,
                             0u,
                             &chObj->coreDescMem[descSet][coreCnt]);
                if (FVID2_SOK != retVal)
                {
                    GT_3trace(VpsMdrvCompTrace, GT_ERR,
                        "Could not program desc set %d of channel "
                        "%d for core %d!\n", descSet, chObj->chNum, coreCnt);
                    break;
                }

                if (NULL != chObj->shwCfgDesc[descSet])
                {
                    /* Program the overlay memory */
                    retVal = coreOps->programReg(
                                 hObj->coreHandle[coreCnt],
                                 chObj->chNum,
                                 &chObj->coreDescMem[descSet][coreCnt]);
                    if (FVID2_SOK != retVal)
                    {
                        GT_3trace(VpsMdrvCompTrace, GT_ERR,
                            "Could not program overlay set %d of channel "
                            "%d for core %d!\n",
                            descSet, chObj->chNum, coreCnt);
                        break;
                    }

                    /* Also update the config descriptor. Update for the
                     * first core is sufficient. Also point to the first
                     * cores overlay memory */
                    if (VPSMDRV_COMP_CORE_OUT_WRBK0_IDX== coreCnt) //?? needs change as BP core may not have shadow ovly mem TODO
                    {
                        #if 1
                        /* Assuming first core have shadow overlay for sure */
                        GT_assert(VpsMdrvCompTrace, (NULL !=
                            chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem));
                        GT_assert(VpsMdrvCompTrace,
                            (0u != chObj->totalShwOvlyMem));

                        VpsHal_vpdmaCreateConfigDesc(
                            chObj->shwCfgDesc[descSet],
                            VPSHAL_VPDMA_CONFIG_DEST_MMR,
                            VPSHAL_VPDMA_CPT_ADDR_DATA_SET,
                            VPSHAL_VPDMA_CCT_INDIRECT,
                            (chObj->totalShwOvlyMem / VPSMDRV_COMP_MAX_DESC_SET),
                            chObj->coreDescMem[descSet][coreCnt].shadowOvlyMem,
                            NULL,
                            0u);

                        /* Program the SOCH/SOC descriptors */
                        if (VPSMDRV_SYNC_MODE_SOCH == hObj->syncMode)
                        {
                            tempPtr = (UInt8 *) chObj->socDesc[descSet];
                            for (chCnt = 0u;
                                 chCnt < hObj->numVpdmaChannels;
                                 chCnt++)
                            {
                                VpsHal_vpdmaCreateSOCHCtrlDesc(
                                    tempPtr,
                                    hObj->socChNum[chCnt]);
                                tempPtr += VPSHAL_VPDMA_CTRL_DESC_SIZE;
                            }
                        }
                        else
                        {
                            /*
                             * TODO: Channel could change depending on the
                             * configuration like height, format etc...
                             * Currently assuming it will wait till 50 lines
                             * are transferred.
                             */
                            VpsHal_vpdmaCreateSOCCtrlDesc(
                                chObj->socDesc[descSet],
                                hObj->socChNum[hObj->socIdx],
                                VPSHAL_VPDMA_SOC_SOL,
                                50u,
                                0);
                        }
                        #endif
                    }
                }
            }

            /* Break if error occurs */
            if (FVID2_SOK != retVal)
            {
                break;
            }
        }
    }

    if (FVID2_SOK == retVal)
    {
        /* Mark the dirty flags as clean */
        for (descSet = 0u; descSet < VPSMDRV_COMP_MAX_DESC_SET; descSet++)
        {
            chObj->isDescDirty[descSet] = FALSE;
        }
    }

    return (retVal);
}


/**
 *  vpsMdrvCompAllocChObjs
 *  Allocate channel objects for the handle.
 */
static Int32 vpsMdrvCompAllocChObjs(VpsMdrv_CompHandleObj *hObj, UInt32 numCh)
{
    Int32       retVal = FVID2_SOK, tempRetVal;
    UInt32      chCnt, errCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    /* Check for out of bound */
    GT_assert(VpsMdrvCompTrace, (numCh <= VPS_M2M_COMP_MAX_CH_PER_INST));

    /* Allocate channel objects */
    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        hObj->chObjs[chCnt] =
            (VpsMdrv_CompChObj *) VpsUtils_alloc(
                                     &gVpsMdrvCompPoolObj.chPoolPrm,
                                     sizeof(VpsMdrv_CompChObj),
                                     VpsMdrvCompTrace);
        if (NULL == hObj->chObjs[chCnt])
        {
            GT_1trace(VpsMdrvCompTrace, GT_ERR,
                "Channel memory alloc failed: %d!!\n", chCnt);
            retVal = FVID2_EALLOC;
            break;
        }
        hObj->chObjs[chCnt]->chNum = chCnt;
    }

    if (FVID2_SOK != retVal)
    {
        /* Free-up the already allocated channel memories */
        for (errCnt = 0u; errCnt < chCnt; errCnt++)
        {
            hObj->chObjs[errCnt]->chNum = 0u;
            tempRetVal = VpsUtils_free(
                             &gVpsMdrvCompPoolObj.chPoolPrm,
                             hObj->chObjs[errCnt],
                             VpsMdrvCompTrace);
            GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
            hObj->chObjs[errCnt] = NULL;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompFreeChObjs
 *  Frees the channel and scratch pad objects of the handle.
 */
static Int32 vpsMdrvCompFreeChObjs(VpsMdrv_CompHandleObj *hObj, UInt32 numCh)
{
    Int32       retVal = FVID2_SOK, tempRetVal;
    UInt32      chCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    /* Check for out of bound */
    GT_assert(VpsMdrvCompTrace, (numCh <= VPS_M2M_COMP_MAX_CH_PER_INST));

    for (chCnt = 0u; chCnt < numCh; chCnt++)
    {
        if (NULL != hObj->chObjs[chCnt])
        {
            hObj->chObjs[chCnt]->chNum = 0u;
            tempRetVal = VpsUtils_free(
                             &gVpsMdrvCompPoolObj.chPoolPrm,
                             hObj->chObjs[chCnt],
                             VpsMdrvCompTrace);
            GT_assert(VpsMdrvCompTrace, (FVID2_SOK == tempRetVal));
            hObj->chObjs[chCnt] = NULL;
        }
    }

    return (retVal);
}



/**
 *  vpsMdrvCompCopyChPrms
 *  Copies the channel parameters from create params to channel objects.
 */
static Int32 vpsMdrvCompCopyChPrms(VpsMdrv_CompHandleObj *hObj,
                                  const Vps_M2mCompChParams *inChPrms)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  chCnt;
    Vps_M2mCompChParams     *chPrms;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != inChPrms));

    for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
    {
        /* NULL pointer check */
        GT_assert(VpsMdrvCompTrace, (NULL != hObj->chObjs[chCnt]));

        chPrms = &hObj->chObjs[chCnt]->chPrms;

        /* Copy/update input Video path related parmeters:  BP0/BP1/DEI_H/DEI */
        chPrms->inMemType = inChPrms->inMemType;
        chPrms->vcompEnable = inChPrms->vcompEnable;
        chPrms->edeEnable = inChPrms->edeEnable;

        chPrms->outMemTypeSc5= inChPrms->outMemTypeSc5;
        chPrms->outMemTypeVip0= inChPrms->outMemTypeVip0;
        chPrms->outMemTypeVip0Sc= inChPrms->outMemTypeVip0Sc;
        chPrms->outMemTypeVip1Sc= inChPrms->outMemTypeVip1Sc;

        /* Copy the Input buffer format */
        if (NULL != &inChPrms->inFmt)
        {
            VpsUtils_memcpy(
                &chPrms->inFmt,
                &inChPrms->inFmt,
                sizeof(FVID2_Format));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            VpsUtils_memcpy(
                &chPrms->inFmt,
                &VpsMdrvCompDefFmt,
                sizeof(FVID2_Format));
        }

        /* Copy the Graphics parameters if present */
        if (NULL != inChPrms->grpxPrms)
        {
            chPrms->grpxPrms = &hObj->chObjs[chCnt]->grpxPrms;
            VpsUtils_memcpy(
                chPrms->grpxPrms,
                inChPrms->grpxPrms,
                sizeof(Vps_GrpxCreateParams));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->grpxPrms = &hObj->chObjs[chCnt]->grpxPrms;
            VpsUtils_memcpy(
                chPrms->grpxPrms,
                &VpsMdrvCompGrpxPrms,
                sizeof(Vps_GrpxCreateParams));
        }

        /* Copy the Sc5 out buffer format if present */
        if (NULL != inChPrms->outFmtSc5)
        {
            chPrms->outFmtSc5 = &hObj->chObjs[chCnt]->outFmtSc5;
            VpsUtils_memcpy(
                chPrms->outFmtSc5,
                inChPrms->outFmtSc5,
                sizeof(FVID2_Format));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->outFmtSc5 = &hObj->chObjs[chCnt]->outFmtSc5;
            VpsUtils_memcpy(
                chPrms->outFmtSc5,
                &VpsMdrvCompDefFmt,
                sizeof(FVID2_Format));
        }


        /* Copy SC5 scalar parameters */
        if (NULL != inChPrms->sc5Cfg)
        {
            chPrms->sc5Cfg = &hObj->chObjs[chCnt]->sc5Cfg;
            VpsUtils_memcpy(
                chPrms->sc5Cfg,
                inChPrms->sc5Cfg,
                sizeof(Vps_ScConfig));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->sc5Cfg = &hObj->chObjs[chCnt]->sc5Cfg;
            VpsUtils_memcpy(
                chPrms->sc5Cfg,
                &VpsMdrvCompDefScCfg,
                sizeof(Vps_ScConfig));
        }

        /* Copy SC5 crop parameters */
        if (NULL != inChPrms->sc5CropCfg)
        {
            chPrms->sc5CropCfg = &hObj->chObjs[chCnt]->sc5CropCfg;
            VpsUtils_memcpy(
                chPrms->sc5CropCfg,
                inChPrms->sc5CropCfg,
                sizeof(Vps_CropConfig));
        }
        else
        {
            /* Use the default parameters if user fails to give it */
            chPrms->sc5CropCfg = &hObj->chObjs[chCnt]->sc5CropCfg;
            VpsUtils_memcpy(
                chPrms->sc5CropCfg,
                &VpsMdrvCompDefCropCfg,
                sizeof(Vps_CropConfig));
        }

        /* If the configuration is per handle user will give only one channel
         * channel parameter. Hence for per handle don't increment user
         * channel parameter. Increment only for per channel configuration */
        if (VPS_M2M_CONFIG_PER_CHANNEL == hObj->mode)
        {
            inChPrms++;
        }
    }

    return (retVal);
}

/**
 */
Int32 vpsMdrvCompGetSocCh(VpsMdrv_CompHandleObj *hObj)
{
    Int32                   retVal = FVID2_SOK;
    UInt32                  coreCnt;
    UInt32                  numSocCh;
    UInt32                  chCnt;
    Vcore_DescInfo          descInfo;
    Vcore_Property          property;
    const Vcore_Ops        *coreOps;
    VpsMdrv_CompInstObj     *instObj = NULL;

    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(VpsMdrvCompTrace, (NULL != instObj));

    numSocCh = 0u;
    for (coreCnt = 0u; coreCnt < VPSMDRV_COMP_MAX_CORE; coreCnt++)
    {
        /* Get the Descriptor Information for this core */
        if (TRUE == instObj->isCoreReq[coreCnt])
        {
            coreOps = instObj->coreOps[coreCnt];
            /* NULL pointer check */
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
            GT_assert(VpsMdrvCompTrace, (NULL != coreOps->getDescInfo));

            /* Get the descriptor info of each core */
            retVal = coreOps->getDescInfo(
                         hObj->coreHandle[coreCnt],
                         hObj->chObjs[0u]->chNum,
                         0u,
                         &descInfo);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not get core info for core %d!\n", coreCnt);
                break;
            }

            /* Get the Property info to see if it is input ot output core */
            retVal = coreOps->getProperty(
                         hObj->coreHandle[coreCnt],
                         &property);
            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Could not get property for core %d!\n", coreCnt);
                break;
            }

            /* Get the index of the VPDMA channel, which will be used for
             * programming sync on client control
             * descriptor at the end of each channel's descriptors */
            if (VCORE_TYPE_INPUT == property.type)
            {
                hObj->socIdx = numSocCh;
            }

            GT_assert(VpsMdrvCompTrace,
                (descInfo.numChannels < VCORE_MAX_VPDMA_CH));
            for (chCnt = 0u; chCnt < descInfo.numChannels; chCnt++)
            {
                GT_assert(VpsMdrvCompTrace,
                    (numSocCh < VCORE_MAX_VPDMA_CH));
                hObj->socChNum[numSocCh] = descInfo.socChNum[chCnt];
                numSocCh++;
            }
        }
        if (FVID2_SOK != retVal)
        {
            break;
        }
    }

    if (FVID2_SOK == retVal)
    {
        hObj->numVpdmaChannels = numSocCh;
    }

    return (retVal);
}

#if 0

/**
 *  vpsMdrvCompCfgVipCore
 *  Configures the VIP core according to the channel parameters.
 */
static Int32 vpsMdrvCompCfgVipCore(VpsMdrv_CompHandleObj *hObj,
                                  VpsMdrv_CompChObj *chObj,
                                  Vcore_Handle coreHandle,
                                  const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    Vcore_VipParams     vipPrms;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj));
    GT_assert(VpsMdrvCompTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj->chPrms.outFmtVip));
    GT_assert(VpsMdrvCompTrace, (NULL != chObj->chPrms.vipScCfg));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps->setParams));

    /* Copy the VIP buffer format */
    VpsUtils_memcpy(
        &vipPrms.fmt,
        chObj->chPrms.outFmtVip,
        sizeof(FVID2_Format));

    /* Fill other VIP core parameters */
    vipPrms.isScReq = hObj->isVipScReq;
    vipPrms.memType = chObj->chPrms.outMemTypeVip;
    vipPrms.srcWidth = chObj->chPrms.inFmt.width;
    vipPrms.srcHeight = chObj->chPrms.inFmt.height;

    /* Copy the VIP scalar parameters */
    VpsUtils_memcpy(
        &vipPrms.scCfg,
        chObj->chPrms.vipScCfg,
        sizeof(Vps_ScConfig));

    /* Copy the COMP crop parameters */
    VpsUtils_memcpy(&vipPrms.cropCfg,
                    chObj->chPrms.vipCropCfg,
                    sizeof(Vps_CropConfig));

    /* Set the VIP core parameter */
    retVal = coreOps->setParams(coreHandle, chObj->chNum, &vipPrms);
    if (FVID2_SOK != retVal)
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Set params for VIP core failed!\n");
    }

    return (retVal);
}



/**
 *  vpsMdrvCompAlcCfgVipCore
 *  Allocate and configure VIP core
 */
static Int32 vpsMdrvCompAlcCfgVipCore(VpsMdrv_CompHandleObj *hObj,
                                     Vcore_Handle coreHandle,
                                     const Vcore_Ops *coreOps)
{
    Int32               retVal = FVID2_SOK;
    UInt32              chCnt;

    /* NULL pointer check */
    GT_assert(VpsMdrvCompTrace, (NULL != hObj));
    GT_assert(VpsMdrvCompTrace, (NULL != coreHandle));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps));
    GT_assert(VpsMdrvCompTrace, (NULL != coreOps->control));

    /* Allocate the path in VIP based on the parameters set using SetParams */
    retVal = coreOps->control(
                coreHandle,
                VCORE_IOCTL_VIP_ALLOC_PATH,
                NULL);

    if (FVID2_SOK == retVal)
    {
        /* Configure all channels of the core */
        for (chCnt = 0u; chCnt < hObj->numCh; chCnt++)
        {
            GT_assert(VpsMdrvCompTrace, (NULL != hObj->chObjs[chCnt]));

            retVal = coreOps->control(
                        coreHandle,
                        VCORE_IOCTL_VIP_SET_CONFIG,
                        &(hObj->chObjs[chCnt]->chNum));

            if (FVID2_SOK != retVal)
            {
                GT_1trace(VpsMdrvCompTrace, GT_ERR,
                    "Set Config for %d channel in VIP core failed!\n",
                    hObj->chObjs[chCnt]->chNum);
            }
        }
    }
    else
    {
        GT_0trace(VpsMdrvCompTrace, GT_ERR,
            "Resource Allocation in VIP core failed!\n");
    }

    return (retVal);
}
#endif


