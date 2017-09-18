/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

/**
 *  \file vpscore_scWrbkPath.c
 *
 *  \brief VPS Scalar5 writeback2 path core file.
 *  This file implements the core layer for writeback scalar.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/psp/vps/common/vps_config.h>
#include <ti/psp/vps/common/trace.h>
#include <ti/psp/vps/common/vps_types.h>
#include <ti/psp/vps/vps.h>
#include <ti/psp/vps/hal/vpshal_vpdma.h>
#include <ti/psp/vps/hal/vpshal_sc.h>
#include <ti/psp/vps/fvid2.h>
#include <ti/psp/vps/vps_m2m.h>
#include <ti/psp/vps/common/vps_utils.h>
#include <ti/psp/vps/core/vpscore.h>
#include <ti/psp/vps/core/vpscore_scWrbkPath.h>

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/**
 *  \brief Maximum number of handles supported per scalar writeback instance.
 */
#define VCORE_SWP_MAX_HANDLES           (4u)

/**
 *  \brief Maximum number of channel objects to be statically allocated for all
 *  the instances. Each of the channels in an instance is assumed to be
 *  shared across all handles of that instance.
 *  All the handles will allocate channel objects from this pool only.
 *  Instead of allocating VCORE_SWP_MAX_HANDLES *
 *  VCORE_SWP_MAX_CHANNELS_PER_INST objects per instance,
 *  only the below number of objects are allocated
 *  to save memory.
 */
#define VCORE_SWP_MAX_CHANNEL_OBJECTS    (VCORE_SWP_MAX_CHANNELS_PER_INST * \
                                          VCORE_SWP_NUM_INST)

/**
 *  \brief Time out to be used in sem pend
 */
#define VCORE_SWP_SEM_TIMEOUT            (BIOS_WAIT_FOREVER)

/**
 *  \brief Default width used for initializing format structure.
 */
#define VCORE_SWP_DEFAULT_WIDTH          (720u)

/**
 *  \brief Default height used for initializing format structure.
 */
#define VCORE_SWP_DEFAULT_HEIGHT         (480u)

/**
 * \brief Maximum number of outbound Data descriptor required by this core
 *  under worst case.
 */
#define VCORE_SWP_MAX_OUT_DATA_DESC      (1u)

/**
 * \brief Maximum number of inbound Data descriptor required by this core
 *  under worst case.
 */
#define VCORE_SWP_MAX_IN_DATA_DESC      (0u)

/** \brief Maximum VPDMA channels to be programmed  */
#define VCORE_SWP_MAX_VPDMA_CHANNELS (1u)


/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**
 *  \brief Typedef for scalar writeback path instance object.
 */
typedef struct Vcore_SwpInstObj_t Vcore_SwpInstObj;

/**
 *  struct Vcore_SwpChObj
 *  \brief Structure containing per channel information.
 */
typedef struct
{
    Vcore_States                state;
    /**< Current state */
    Vcore_SwpParams             params;
    /**< Scalar params */
    Vcore_Format                coreFmt;
    /**< Format structure */
    Vcore_DescInfo              descInfo;
    /**< Config descriptor information to be used by driver to allocate
         config descriptor memories required by this core. */
    VpsHal_VpdmaOutDescParams   dataDescParam;
    /**< Data descriptor parameters to be passed to VPDMA HAL to configure
         the data descriptor.  This param structure will be initialized once
         and then after with each queue if there will any run time parameter
         change then only that part will be re-written. For Display driver
         it will be initialized once per handle.  For Mem-mem driver it
         will be initlaized for each channel if Mem-mem driver supports
         configuration per channel.  Else like display driver it will
         be intialized once only.*/
    VpsHal_ScConfig             scHalCfg;
    /**< Scalar configuration.  This is to pass to the scalar
         HAL.  It will be configured once during handle open.
         After that only relevant info will be configure for each queue. */
    UInt32                      isAllocated;
    /**< Whether the channel object is allocated or not */
} Vcore_SwpChObj;

/**
 * struct Vcore_SwpHandleObj
 *  \brief Structure containing per handle information.
 */
typedef struct
{
    UInt32                          used;
    /**< Flag to indicate whether the handle object is already used. */
    Vcore_SwpInstObj                *parent;
    /**< Pointer to the instance object. */
    UInt32                          perChCfg;
    /**< Flag indicating whether format is common for all the channel or
         it is different for each channel. */
    Vcore_SwpChObj                  handleContext;
    /**< Used when the handle is opened with perChCfg as false.
         Even in this mode, the data descriptor information is not updated
         in this variable - still the channel objects is used for this
         purpose. */
    UInt32                          numCh;
    /**< Number of channel for this handle. This determines the number of
         valid pointers in above channel array. */
    Vcore_SwpChObj                  *chObjs[VCORE_SWP_MAX_CHANNELS_PER_INST];
    /**< Pointer to the channel information. Array of pointers is used so that
         memory for each channel could be allocated from the pool */
} Vcore_SwpHandleObj;

/**
 *  \brief Structure containing per instance information.
 */
struct Vcore_SwpInstObj_t
{
    UInt32                          instId;
    /**< Instance number/ID. */
    UInt32                          openCnt;
    /**< Counter to keep track of number of open calls for an instance. */
    VpsHal_VpdmaChannel             scCh;
    /**< Scalar writeback VPDM channel. */
    Vcore_OpMode                    curMode;
    /**< Current operational mode of this core. Used to differentiate between
         display/capture or mem-mem operation of the core. */
    Vcore_SwpHandleObj              *handleObjs;
    /**< Pointer to the handle information. This is a contiguous memory of
         numHandle Vcore_SwpHandleObj structure */
    UInt32                          numHandle;
    /**< Number of handle for this instance. This determines the size of
         handle array. */
    VpsHal_Handle                   scHandle;
    /**<Handle of the writeback scalar HAL.  This will be used to do
        create overlay mem for writeback scalar. */
    UInt32                      VpsHal_numVpdmaCh;
    /**< Number of vpdma channels supported by this path */
    UInt32                      numVpdmaCh;
    /**< Number of VPDMA channels*/
};

/* Forward declarations of function table functions */
static Vcore_Handle Vcore_swpOpen(UInt32 instId,
                                  Vcore_OpMode mode,
                                  UInt32 numCh,
                                  UInt32 perChCfg);
static Int32 Vcore_swpClose(Vcore_Handle handle);
static Int32 Vcore_swpSetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                const Vcore_Format *coreFmt);
static Int32 Vcore_swpSetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                const Void *arg);
static Int32 Vcore_swpGetFormat(Vcore_Handle handle,
                                UInt32 chNum,
                                Vcore_Format *coreFmt);
static Int32 Vcore_swpSetFSEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_swpProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem);

static Int32 Vcore_swpProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem);
static Int32 Vcore_swpUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid);
static Int32 Vcore_swpUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtParams,
                                 UInt32 frmIdx);
static Int32 Vcore_swpGetParams(Vcore_Handle handle,
                                UInt32 chNum,
                                const Ptr arg);
static Int32 Vcore_swpGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo);
static Int32 Vcore_swpControl(Vcore_Handle handle, UInt32 cmd, Ptr args);
static Int32 Vcore_swpProgramScCoeff(Vcore_Handle handle,
                                     UInt32 chNum,
                                     Vcore_ScCoeffParams *coeffPrms);
static Int32 Vcore_swpGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property);
static Int32 Vcore_swpGetScFactorConfig(Vcore_Handle handle,
                                        UInt32 chNum,
                                        VpsHal_ScFactorConfig *scFactorConfig,
                                        const Vcore_DescMem *descMem);

/* Forward declarations of internal functions */
static Int32 swpCheckFormat(const Vcore_Format *coreFmt);
static Vcore_SwpHandleObj *swpAllocHandleObj(UInt32 instId,
                                             Vcore_OpMode mode);
static Int32 swpFreeHandleObj(Vcore_SwpHandleObj *hObj);
static Int32 swpAllocChannelObj(Vcore_SwpChObj **chObjs,
                                UInt32 numCh);
static Int32 swpFreeChannelObj(Vcore_SwpChObj **chObjs,
                               UInt32 numCh);
static void swpSetDefaultChInfo(Vcore_SwpChObj *chObj,
                                 UInt32 channelCnt);

static Vcore_SwpHandleObj *swpAllocHandleMem(UInt32 numHandle);

static Int32 swpFreeHandleMem(const Vcore_SwpHandleObj *handleObjs);

static Vcore_SwpChObj *swpAllocChannelMem(void);

static Int32 swpFreeChannelMem(const Vcore_SwpChObj *chObj);

static UInt32 swpProgramDataDesc(const Vcore_SwpHandleObj *hObj,
                                 Vcore_SwpChObj *chObj,
                                 const Vcore_DescMem *descMem);
static UInt32 swpProgramReg(Vcore_SwpHandleObj *hObj,
                               Vcore_SwpChObj *chObj,
                              const Vcore_DescMem *descMem);
static Void swpUpdateRtParams(Vcore_SwpChObj *chObj,
                             const Vcore_SwpRtParams *rtParams);
static Int32 swpUpdateRtMem(Vcore_SwpHandleObj *hObj,
                          Vcore_SwpChObj *chObj,
                          const Vcore_DescMem *descMem);
static Vcore_SwpChObj *swpGetChObj(Vcore_SwpHandleObj *hObj, UInt32 chNum);


/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */
/** \brief Core function pointer. */
static const Vcore_Ops SwpCoreOps =
{
    Vcore_swpGetProperty,                               /* Get property */
    Vcore_swpOpen,                      /* Open function */
    Vcore_swpClose,                     /* Close function */
    Vcore_swpSetFormat,                 /* Set format */
    Vcore_swpGetFormat,                 /* Get format */
    NULL,                               /* Set mosaic */
    NULL,                               /* Create layout */
    NULL,                               /* Delete layout */
    Vcore_swpGetDescInfo,               /* Get descriptor */
    Vcore_swpSetFSEvent,                /* Frame start event for handle */
    Vcore_swpSetParams,                 /* Set the core params */
    Vcore_swpGetParams,                 /* Get the core params */
    Vcore_swpControl,                   /* Control ioctls */
    Vcore_swpProgramDesc,               /* Program Descriptors */
    Vcore_swpProgramReg,                /* Program registors/overlay memory */
    Vcore_swpUpdateDesc,                /* Update the descriptors */
    NULL,                               /* Update Multidesc */
    NULL,                               /* Update context */
    NULL,                               /* Get DEI ctx info funct pointer. */
    Vcore_swpUpdateRtMem,               /* Update run time configuration */
    Vcore_swpProgramScCoeff,            /* Program scalar coefficient */
    NULL,                               /* Update DEI mode */
    Vcore_swpGetScFactorConfig          /* Get scaling factor config */
};

/** \brief scalar writeback path objects */
static Vcore_SwpInstObj SwpInstObjects[VCORE_SWP_NUM_INST];

/** \brief Properties of this core */
static Vcore_Property SwpProperty;

/**
 *  \brief Memory pool for the handle objects per instance.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_SwpHandleObj SwpHandleMemPool
                            [VCORE_SWP_NUM_INST][VCORE_SWP_MAX_HANDLES];

/**
 *  \brief The flag variable represents whether a given handle object is
 *  allocated or not
 */
static UInt32 SwpHandleMemFlag[VCORE_SWP_NUM_INST];

/**
 *  \brief Memory pool for the sub-channel objects.
 *  This memory is allocated statically and is fixed for each of the
 *  instance objects.
 */
static Vcore_SwpChObj SwpChannelMemPool[VCORE_SWP_MAX_CHANNEL_OBJECTS];

/** \brief Semaphore for protecting allocation and freeing of
    memory pool objects */
static Semaphore_Handle SwpPoolSem = NULL;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/**
 *  Vcore_swpInit
 *  \brief Scalar writeback path core init function.
 *  Initializes scalar writeback path core objects, allocates memory etc.
 *  This function should be called before calling any of SWP core API's.
 *
 *  \param numInstance  Number of instance objects to be initialized
 *  \param initParams   Pointer to the instance parameter containing
 *                      instance specific information. If the number of
 *                      instance is greater than 1, then this pointer
 *                      should point to an array of init parameter
 *                      structure of size numInstances.
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_swpInit(UInt32 numInstance,
                   const Vcore_SwpInitParams *initParams,
                   Ptr arg)
{
    UInt32              instCnt, handleCnt;
    Vcore_SwpInstObj    *instObj;
    Int32               retVal = VPS_SOK;
    Semaphore_Params    semParams;

    /* Check for errors */
    GT_assert(SwpCoreTrace, (numInstance <= VCORE_SWP_NUM_INST));
    GT_assert(SwpCoreTrace, (NULL != initParams));

    /* Initialize swp Objects to zero */
    VpsUtils_memset(SwpInstObjects, 0, sizeof(SwpInstObjects));
    VpsUtils_memset(&SwpProperty, 0, sizeof(SwpProperty));
    VpsUtils_memset(&SwpHandleMemPool, 0, sizeof(SwpHandleMemPool));
    VpsUtils_memset(&SwpChannelMemPool, 0, sizeof(SwpChannelMemPool));

    /* Mark pool flags as free */
    for (instCnt = 0u; instCnt < VCORE_SWP_NUM_INST; instCnt++)
    {
        SwpHandleMemFlag[instCnt] = FALSE;
        for (handleCnt = 0u; handleCnt < VCORE_SWP_MAX_HANDLES; handleCnt++)
        {
            SwpHandleMemPool[instCnt][handleCnt].used = FALSE;
        }
    }

    /* Create Pool Semaphore */
    Semaphore_Params_init(&semParams);
    SwpPoolSem = Semaphore_create(1u, &semParams, NULL);
    if (NULL == SwpPoolSem)
    {
        GT_0trace(SwpCoreTrace, GT_ERR, "Sem create failed!!\n");
        retVal = VPS_EALLOC;
    }
    else
    {
        /* Initialize scalar writeback path core properties */
        SwpProperty.numInstance = numInstance;
        SwpProperty.name = VCORE_TYPE_SC_WB2;
        SwpProperty.type = VCORE_TYPE_OUTPUT;
        SwpProperty.internalContext = FALSE;
        for (instCnt = 0u; instCnt < numInstance; instCnt++)
        {

            GT_assert(SwpCoreTrace,
                (initParams[instCnt].instId < VCORE_SWP_NUM_INST));

            /* Initialize instance object members */
            instObj = &SwpInstObjects[instCnt];
            instObj->instId = initParams[instCnt].instId;
            instObj->scCh = initParams[instCnt].scCh;
            instObj->scHandle = initParams[instCnt].scHandle;
            instObj->curMode = VCORE_OPMODE_INVALID;
            instObj->numVpdmaCh = VCORE_SWP_MAX_VPDMA_CHANNELS;

            /* Allocate handle object memory */
            instObj->handleObjs = swpAllocHandleMem(
                                  initParams[instCnt].maxHandle);
            if (NULL == instObj->handleObjs)
            {
                GT_1trace(SwpCoreTrace, GT_ERR,
                    "Handle allocation failed for %d!!\n", instCnt);
                retVal = VPS_EALLOC;
                break;
            }
            instObj->numHandle = initParams[instCnt].maxHandle;
        }
    }

    /* Cleanup if error occurs */
    if (VPS_SOK != retVal)
    {
        Vcore_swpDeInit(NULL);
    }

    return (retVal);
}

/**
 *  Vcore_swpDeInit
 *  \brief Scalar writeback path exit function.
 *  Deallocates memory allocated by init function.
 *
 *  \param arg          Not used currently. Meant for future purpose.
 *
 *  \return             Returns 0 on success else returns error value.
 */
Int32 Vcore_swpDeInit(Ptr arg)
{
    UInt32              instCnt;
    Vcore_SwpInstObj    *instObj;

    /* Free-up handle objects for each instance */
    for (instCnt = 0u; instCnt < SwpProperty.numInstance; instCnt++)
    {
        instObj = &SwpInstObjects[instCnt];
        instObj->instId = VCORE_SWP_INST_WB2;
        instObj->scCh = (VpsHal_VpdmaChannel) 0;
        instObj->curMode = VCORE_OPMODE_INVALID;
        if (instObj->handleObjs)
        {
            swpFreeHandleMem(instObj->handleObjs);
            instObj->handleObjs = NULL;
            instObj->numHandle = 0u;
        }
    }
    SwpProperty.numInstance = 0u;

    /* Delete the Semaphore created for Pool objects */
    if (SwpPoolSem)
    {
        Semaphore_delete(&SwpPoolSem);
        SwpPoolSem = NULL;
    }

    return (VPS_SOK);

}

/**
 *  Vcore_swpGetCoreOps
 *  \brief Returns the pointer to core function pointer table.
 *
 *  \return             Returns the pointer to core function pointer table.
 */
const Vcore_Ops *Vcore_swpGetCoreOps(void)
{
    return &SwpCoreOps;

}

/**
 *  Vcore_swpOpen
 *  \brief Opens a particular instance of scalar writeback in either
 *  display or mem-mem mode and returns handle to the handle
 *  When opened in display mode, only one handle could be opened. Also
 *  only one channel is supported.
 *  When opened in mem-mem mode, the same instance could be opened more than
 *  once. The number of handles supported per instance is determined by
 *  VCORE_SWP_MAX_HANDLES macro. Also each of the handle could have multiple
 *  channel to support in a single handle.  The maximum number of channels for
 *  all the handles is determined by
 *
 *  \param instId       Instance to open - Secondary path 0
 *                      or Scalar writeback path.
 *  \param mode         Mode to open the instance for - display or mem-mem.
 *  \param numCh        Number of channel to be associated with this open.
 *                      For display operation only one channel is allowed.
 *                      For mem-mem operation, more than one channel could
 *                      be associated with a handle handle.
 *                      The maximum supported channel per handle depends on
 *                      VCORE_SWP_NUM_CHANNEL_OBJECTS macro.
 *  \param perChCfg     Flag indicating whether the config descriptors
 *                      should be separate for each and every channel or
 *                      not. This is valid only for mem-mem drivers. For
 *                      display drivers this should be set to FALSE.
 *                      TRUE - Separate config descriptors for each channel.
 *                      FALSE - One config descriptors used per handle
 *                      handle.
 *
 *  \return             If success returns the handle handle else returns NULL.
 */
static Vcore_Handle Vcore_swpOpen(UInt32 instId,
                          Vcore_OpMode mode,
                          UInt32 numCh,
                          UInt32 perChCfg)
{
    Vcore_SwpHandleObj      *hObj = NULL;
    Int32               retVal = VPS_SOK;

    /* Check for valid instance ID */
    GT_assert(SwpCoreTrace, (instId < VCORE_SWP_NUM_INST));

    GT_assert(SwpCoreTrace,
        (VCORE_OPMODE_MEMORY == mode || VCORE_OPMODE_DISPLAY == mode));

    /* Check for maximum channel supported per handle */
    GT_assert(SwpCoreTrace, (numCh <= VCORE_SWP_MAX_CHANNELS_PER_INST));
    /* There should be atleast one channel */
    GT_assert(SwpCoreTrace, (0u != numCh));

    /* For display operation, per channel format should be FALSE. */
    if ((VCORE_OPMODE_DISPLAY == mode) && (FALSE != perChCfg))
    {
        GT_0trace(SwpCoreTrace, GT_ERR,
            "Per channel format not supported for display!!\n");
        retVal = VPS_EINVALID_PARAMS;
    }

    if (VPS_SOK == retVal)
    {
        /* Allocate a handle object */
        hObj = swpAllocHandleObj(instId, mode);
        if (NULL != hObj)
        {
            /* Allocate channel objects */
            retVal = swpAllocChannelObj(hObj->chObjs, numCh);
            if (VPS_SOK != retVal)
            {
                GT_0trace(SwpCoreTrace, GT_ERR,
                    "Channel allocation failed!!\n");
                /* Free-up handle object if channel allocation failed */
                swpFreeHandleObj(hObj);
                hObj = NULL;
            }
            else
            {
                /* Initialize variables */
                hObj->perChCfg = perChCfg;
                hObj->numCh = numCh;
                swpSetDefaultChInfo(&hObj->handleContext, 0u);
            }
        }
        else
        {
            GT_0trace(SwpCoreTrace, GT_ERR, "Handle allocation failed!!\n");
        }
    }
    return ((Vcore_Handle) hObj);
}
static Int32 Vcore_swpControl(Vcore_Handle handle, UInt32 cmd, Ptr args)
{
    Int32 retVal = VPS_SOK;
    Vcore_SwpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != args));
    hObj = (Vcore_SwpHandleObj *) handle;
    switch (cmd)
    {
        case VCORE_IOCTL_GET_SC_CFG:
        {
            Vcore_ScCfgParams *cfgPrms = (Vcore_ScCfgParams *)args;
            Vps_ScAdvConfig  *scAdvCfg;
            VpsHal_ScConfig     scCfg;
            Vps_ScPeakingConfig peakingCfg;
            Vps_ScEdgeDetectConfig edgeDetectCfg;

            scCfg.peakingCfg = &peakingCfg;
            scCfg.edgeDetectCfg = &edgeDetectCfg;
            GT_assert(SwpCoreTrace,(NULL != cfgPrms));
            GT_assert(SwpCoreTrace,(NULL != cfgPrms->scAdvCfg));
            scAdvCfg = cfgPrms->scAdvCfg;
            retVal = VpsHal_scGetConfig(hObj->parent->scHandle,
                        &scCfg,
                        cfgPrms->ovlyPtr);

            scAdvCfg->tarWidth = scCfg.tarWidth;
            scAdvCfg->tarHeight = scCfg.tarHeight;
            scAdvCfg->srcWidth = scCfg.srcWidth;
            scAdvCfg->srcHeight = scCfg.srcHeight;
            scAdvCfg->cropStartX = scCfg.cropStartX;
            scAdvCfg->cropStartY = scCfg.cropStartY;
            scAdvCfg->cropWidth = scCfg.cropWidth;
            scAdvCfg->cropHeight = scCfg.cropHeight;
            scAdvCfg->inFrameMode = scCfg.inFrameMode;
            scAdvCfg->outFrameMode = scCfg.outFrameMode;
            scAdvCfg->hsType = scCfg.hsType;
            scAdvCfg->nonLinear = scCfg.nonLinear;
            scAdvCfg->stripSize = scCfg.stripSize;
            scAdvCfg->vsType = scCfg.vsType;
            scAdvCfg->fidPol = scCfg.fidPol;
            scAdvCfg->selfGenFid = scCfg.selfGenFid;
            scAdvCfg->defConfFactor = scCfg.defConfFactor;
            scAdvCfg->biLinIntpType = scCfg.biLinIntpType;
            scAdvCfg->enableEdgeDetect = scCfg.enableEdgeDetect;
            scAdvCfg->hPolyBypass = scCfg.hPolyBypass;
            scAdvCfg->enablePeaking = scCfg.enablePeaking;
            scAdvCfg->bypass = scCfg.bypass;
            scAdvCfg->rowAccInc = scCfg.rowAccInc;
            scAdvCfg->rowAccOffset = scCfg.rowAccOffset;
            scAdvCfg->rowAccOffsetB = scCfg.rowAccOffsetB;
            scAdvCfg->ravScFactor = scCfg.ravScFactor;
            scAdvCfg->ravRowAccInit = scCfg.ravRowAccInit;
            scAdvCfg->ravRowAccInitB = scCfg.ravRowAccInitB;

            if (NULL != scAdvCfg->peakingCfg)
            {
                VpsUtils_memcpy(scAdvCfg->peakingCfg,
                                scCfg.peakingCfg,
                                sizeof(Vps_ScPeakingConfig));

            }
            if (NULL != scAdvCfg->edgeDetectCfg)
            {
                VpsUtils_memcpy(scAdvCfg->edgeDetectCfg,
                                scCfg.edgeDetectCfg,
                                sizeof(Vps_ScEdgeDetectConfig));

            }
            break;
        }
        case VCORE_IOCTL_SET_SC_CFG:
        {
            Vcore_ScCfgParams *cfgPrms = (Vcore_ScCfgParams *)args;
            Vps_ScAdvConfig  *scAdvCfg;
            VpsHal_ScConfig     scCfg;

            GT_assert(SwpCoreTrace,(NULL != cfgPrms));
            GT_assert(SwpCoreTrace,(NULL != cfgPrms->scAdvCfg));
            scAdvCfg = cfgPrms->scAdvCfg;

            scCfg.tarWidth = scAdvCfg->tarWidth;
            scCfg.tarHeight = scAdvCfg->tarHeight;
            scCfg.srcWidth = scAdvCfg->srcWidth;
            scCfg.srcHeight = scAdvCfg->srcHeight;
            scCfg.cropStartX = scAdvCfg->cropStartX;
            scCfg.cropStartY = scAdvCfg->cropStartY;
            scCfg.cropWidth = scAdvCfg->cropWidth;
            scCfg.cropHeight = scAdvCfg->cropHeight;
            scCfg.inFrameMode = (Vps_ScanFormat)scAdvCfg->inFrameMode;
            scCfg.outFrameMode = (Vps_ScanFormat)scAdvCfg->outFrameMode;
            scCfg.nonLinear = scAdvCfg->nonLinear;
            scCfg.stripSize = scAdvCfg->stripSize;
            scCfg.fidPol = (Vps_FidPol)scAdvCfg->fidPol;
            scCfg.selfGenFid = (Vps_ScSelfGenFid)scAdvCfg->selfGenFid;
            scCfg.bypass = scAdvCfg->bypass;

            scCfg.hsType = scAdvCfg->hsType;
            scCfg.vsType = scAdvCfg->vsType;
            scCfg.defConfFactor = scAdvCfg->defConfFactor;
            scCfg.biLinIntpType = scAdvCfg->biLinIntpType;
            scCfg.enableEdgeDetect = scAdvCfg->enableEdgeDetect;
            scCfg.hPolyBypass = scAdvCfg->hPolyBypass;
            scCfg.enablePeaking = scAdvCfg->enablePeaking;
            scCfg.rowAccInc = scAdvCfg->rowAccInc;
            scCfg.rowAccOffset = scAdvCfg->rowAccOffset;
            scCfg.rowAccOffsetB = scAdvCfg->rowAccOffsetB;
            scCfg.ravScFactor = scAdvCfg->ravScFactor;
            scCfg.ravRowAccInit = scAdvCfg->ravRowAccInit;
            scCfg.ravRowAccInitB = scAdvCfg->ravRowAccInitB;
            scCfg.phInfoMode = VPS_SC_SET_PHASE_INFO_FROM_APP;
            scCfg.peakingCfg = scAdvCfg->peakingCfg;
            scCfg.edgeDetectCfg = scAdvCfg->edgeDetectCfg;

            retVal = VpsHal_scSetAdvConfig(hObj->parent->scHandle,
                        &scCfg,
                        cfgPrms->ovlyPtr);
            break;
        }
        default:
        {
            retVal = VPS_EINVALID_PARAMS;
        }
    }
    return retVal;
}
/**
 *  Vcore_swpClose
 *  \brief Closes the already opened handle.
 *
 *  \param handle       scalar writeback Path Core handle.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpClose(Vcore_Handle handle)
{
    Int32               retVal = VPS_EFAIL;
    Vcore_SwpHandleObj      *hObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));

    hObj = (Vcore_SwpHandleObj *) handle;

    /* Free channel objects */
    retVal = swpFreeChannelObj(hObj->chObjs, hObj->numCh);
    /* Free handle objects */
    retVal += swpFreeHandleObj(hObj);
    /* Null the handle for sake of completeness */
    hObj = NULL;

    return (retVal);
}

/**
 *  Vcore_swpSetParams.
 *  \brief Set the scalar parameters like type of scalar, etc.
 *
 *  \param handle       scalar writeback Path Core handle.
 *  \param chNum        Channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param arg          Pointer to the parameters information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpSetParams(Vcore_Handle handle,
                        UInt32 chNum,
                        const Void *arg)
{
    Int32               retVal = VPS_SOK;
    Vcore_SwpHandleObj      *hObj;
    Vcore_SwpChObj *chObj;
    Vcore_SwpParams     *params;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != arg));

    hObj = (Vcore_SwpHandleObj *) handle;
    params = (Vcore_SwpParams *)arg;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    /* Check if the channel is within the range allocated during open */
    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        /* Copy the format to the local structure */
        VpsUtils_memcpy(&chObj->params, params, sizeof(Vcore_SwpParams));
    }
    return (retVal);
}

/**
 *  Vcore_swpSetFormat.
 *  \brief Configures the swp core. Initializes how many data and config
 *  descriptors will be required depending upon the format passed.
 *
 *  \param handle       scalar writeback Path Core handle.
 *  \param chNum      Channel to which the format should be applied.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param coreFmt      Pointer to the core parameters information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpSetFormat(Vcore_Handle handle,
                        UInt32 chNum,
                        const Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;
    Vcore_SwpHandleObj      *hObj;
    Vcore_SwpChObj *chObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_SwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    /* Check if the channel is within the range allocated during open */
    if (VPS_SOK == retVal)
    {
        /* Check whether the requested format is valid */
        retVal = swpCheckFormat(coreFmt);
        if (VPS_SOK != retVal)
        {
            GT_0trace(SwpCoreTrace, GT_ERR, "Invalid Format!!\n");
        }
    }
    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);

        /* One data descriptors will be required for scalar in any case */
        chObj->descInfo.numOutDataDesc = VCORE_SWP_MAX_OUT_DATA_DESC;
        chObj->descInfo.numInDataDesc = VCORE_SWP_MAX_IN_DATA_DESC;
        chObj->descInfo.numChannels = VCORE_SWP_MAX_VPDMA_CHANNELS;
        chObj->descInfo.socChNum[0] = hObj->parent->scCh;

        /* Get the overlay memory size of the scalar */
        chObj->descInfo.shadowOvlySize =
            VpsHal_scGetConfigOvlySize(hObj->parent->scHandle);

        /* Get the horizontal, vertical and bilinear (if applicable) coeff
         * overlay sizes.
         */
        VpsHal_scGetCoeffOvlySize(hObj->parent->scHandle,
                                  &(chObj->descInfo.horzCoeffOvlySize),
                                  &(chObj->descInfo.vertCoeffOvlySize),
                                  &(chObj->descInfo.vertBilinearCoeffOvlySize));

        chObj->descInfo.coeffConfigDest = VPSHAL_VPDMA_CONFIG_DEST_SC5;

        /* Copy the format to the local structure */
        VpsUtils_memcpy(&chObj->coreFmt, coreFmt, sizeof(Vcore_Format));

        /* Set proper states - since format is set, desc info should be set
         * again */
        chObj->state.isFormatSet = TRUE;
    }

    return (retVal);
}

/**
 *  Vcore_swpGetParams
 *  \brief Gets the params for a given channel.
 *
 *  \param handle       scalar writeback Path Core handle.
 *  \param chNum      Channel from which the format should be read.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param arg          Pointer to the structure where the params will be
 *                      copied.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpGetParams(Vcore_Handle handle,
                        UInt32 chNum,
                        const Ptr arg)
{
    Int32               retVal = VPS_SOK;
    Vcore_SwpHandleObj      *hObj;
    Vcore_SwpChObj *chObj;
    Vcore_SwpParams     *params;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != arg));

    hObj = (Vcore_SwpHandleObj *) handle;
    params = (Vcore_SwpParams *)arg;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    /* Check if the channel is within the range allocated during open */
    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        /* Copy the format */
        VpsUtils_memcpy(params, &chObj->params, sizeof(Vcore_SwpParams));
    }

    return (retVal);
}

/**
 *  Vcore_swpGetFormat
 *  \brief Gets the Format for a given channel.
 *
 *  \param handle       scalar writeback Path Core handle.
 *  \param chNum      Channel from which the format should be read.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param coreFmt       Pointer to the copied format information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpGetFormat(Vcore_Handle handle,
                        UInt32 chNum,
                        Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;
    Vcore_SwpHandleObj      *hObj;
    Vcore_SwpChObj *chObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != coreFmt));

    hObj = (Vcore_SwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        /* Copy the format */
        VpsUtils_memcpy(coreFmt, &chObj->coreFmt, sizeof(Vcore_Format));
    }
    return (retVal);
}

/**
 *  Vcore_swpGetDescInfo
 *  \brief Depending on the format set, returns the number of data
 *  descriptors needed for the actual driver to allocate memory.
 *  Format should have be set for each of the channel before calling this
 *  function.
 *
 *  \param handle       scalar writeback Path Core handle.
 *  \param chNum      Channel to which the info is required.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. But if the perChCfg flag
 *                      is FALSE at the time of open, then this should also be
 *                      0 for mem-mem operation.
 *  \param layoutId     [IN] Layout ID for which to return the information.
 *  \param descInfo Pointer to the data descriptor information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpGetDescInfo(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  Vcore_DescInfo *descInfo)
{
    Int32               retVal = VPS_SOK        ;
    Vcore_SwpHandleObj      *hObj;
    Vcore_SwpChObj *chObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != descInfo));

    hObj = (Vcore_SwpHandleObj *) handle;

    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        if (TRUE == chObj->state.isFormatSet)
        {

            retVal = VPS_SOK;
            /* Copy the data descriptor information */
            VpsUtils_memcpy(descInfo, &chObj->descInfo, sizeof(Vcore_DescInfo));
        }
    }

    return (retVal);
}

/**
 *  Vcore_swpSetFSEvent
 *  \brief Program the client's frame start.
 *
 *  \param handle       scalar writeback Path Core handle.
 *  \param fsEvent      Frame start event.
 *  \param chNum        ChannelNum for which the framestart event needs to
 *                      be set.
 *  \param descMem      Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actualregisters. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpSetFSEvent(Vcore_Handle handle,
                                 UInt32 chNum,
                                 VpsHal_VpdmaFSEvent fsEvent,
                                 const Vcore_DescMem *descMem)
{
    /* Important: It is not required to set the frame start
                  event for writeback client. This event is just used for
                  client event generation. It is not used for outputting
                  data. VPDMA will output data from this client
                  whenever there is input data.
                  Also these clients might be used in the display as free
                  write back client */
    return (VPS_SOK);
}

/**
 *  Vcore_swpProgramDesc
 *  \brief Programs the data descriptor for a given channel. This will not
 *  program the internal context descriptors.
 *  Only after programming the data descriptor for each of the channel,
 *  the actual driver is supposed to call the update buffer function.
 *
 *  \param handle       Scalar Writeback Path Core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param layoutId     [IN] Layout ID used to program the descriptor.
 *  \param descMem      Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpProgramDesc(Vcore_Handle handle,
                                  UInt32 chNum,
                                  UInt32 layoutId,
                                  const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SwpHandleObj     *hObj;
    Vcore_SwpChObj         *chObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != descMem));

    hObj = (Vcore_SwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SwpCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }
    if (VPS_SOK == retVal)
    {
        /* Configure data descriptors */
        retVal = swpProgramDataDesc(hObj, chObj, descMem);
    }
    return (retVal);
}
/**
 *  Vcore_swpProgramReg
 *  \brief Programs the registers for HALs through direct
 *  register programming if descMem is NULL else creates and programs the
 *  overlay memory. The upper driver has to submit the programmed overlay
 *  to the VPDMA to do the actual register programming.
 *
 *  \param handle       Scalar Writeback Path Core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param descMem      Pointer to the overlay memory information.
 *                      If this parameter is NULL, then the configuration
 *                      is written to the actualregisters. Otherwise the
 *                      configuration is updated in the overlay memory.
 *                      This parameter can be NULL depending on the
 *                      intended usage.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpProgramReg(Vcore_Handle handle,
                                 UInt32 chNum,
                                 const Vcore_DescMem *descMem)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SwpHandleObj     *hObj;
    Vcore_SwpChObj    *chObj;
    Vcore_SwpInstObj       *instObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    hObj = (Vcore_SwpHandleObj *) handle;
    instObj = hObj->parent;
    GT_assert(SwpCoreTrace, (NULL != instObj));

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
       chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SwpCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }

    if ((VPS_SOK == retVal) && (NULL != descMem))
    {
        /* Check if overlay pointers are non-NULL for the required overlay */
        GT_assert(SwpCoreTrace,
            !((0u != chObj->descInfo.shadowOvlySize)
            && (NULL == descMem->shadowOvlyMem)));
        swpProgramReg(hObj, chObj, descMem);
    }
    return (retVal);
}
/**
 *  Vcore_swpUpdateDesc
 *  \brief Updates the buffer addresses provided in the frame structure
 *  to the descriptor memories.
 *
 *  \param handle       scalar Writeback Path Core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param descMem      Pointer to the data descriptor memory information.
 *                      This parameter should be non-NULL.
 *  \param frame        Pointer to the FVID frame containing the buffer address.
 *                      This parameter should be non-NULL.
 *  \param fid          FID to be programmed in the descriptor.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpUpdateDesc(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 const FVID2_Frame *frame,
                                 UInt32 fid)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SwpHandleObj      *hObj;
    Vcore_SwpChObj     *chObj;
    Void                    *descPtr;
    UInt32                  chNum;
    UInt32                  numDataDesc;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != descMem));
    GT_assert(SwpCoreTrace, (NULL != frame));

    hObj = (Vcore_SwpHandleObj *) handle;
    chNum = frame->channelNum;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SwpCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }

    if (VPS_SOK == retVal)
    {
        for (numDataDesc = 0; numDataDesc < VCORE_SWP_MAX_OUT_DATA_DESC;
                    numDataDesc++)
        {
            descPtr = descMem->outDataDesc[numDataDesc];
            GT_assert(SwpCoreTrace, (NULL != descPtr));
            VpsHal_vpdmaSetAddress(
                descPtr,
                0u,             /* FID is reserved for outbound descriptors */
                frame->addr[fid][FVID2_YUV_INT_ADDR_IDX]);
        }
    }
    return (retVal);
}

static Int32 Vcore_swpUpdateRtMem(Vcore_Handle handle,
                                 const Vcore_DescMem *descMem,
                                 UInt32 chNum,
                                 const Void *rtParams,
                                 UInt32 frmIdx)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SwpHandleObj      *hObj;
    Vcore_SwpChObj     *chObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != descMem));

    hObj = (Vcore_SwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));
        if (TRUE != chObj->state.isFormatSet)
        {
            GT_0trace(SwpCoreTrace, GT_ERR,
                "Set the format before setting the descriptors!!\n");
            retVal = VPS_EBADARGS;
        }
    }

    if (VPS_SOK == retVal)
    {
        if (NULL != rtParams)
        {
            swpUpdateRtParams(chObj, rtParams);
            retVal = swpUpdateRtMem(hObj, chObj, descMem);
        }
        else
        {
            retVal = swpUpdateRtMem(hObj, chObj, descMem);
        }
    }
    return (retVal);
}

/**
 *  Vcore_swpGetProperty
 *  \brief Gets the core properties of the SWP core.
 *
 *  \param handle       SWP core handle.
 *  \param property     Pointer to which the SWP core properties to
 *                      be copied.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpGetProperty(Vcore_Handle handle,
                                  Vcore_Property *property)
{
    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != property));

    property->numInstance =1;
    property->name = VCORE_TYPE_SC_WB2;
    property->type = VCORE_TYPE_OUTPUT;
    property->internalContext= 0;

    return (VPS_SOK);
}

/**
 *  Vcore_swpProgramScCoeff
 *  \brief Programs the coefficient overlay memory with the scalar coefficients
 *  according to the parameter passed. The upper driver has to submit the
 *  programmed overlay to the VPDMA to do the actual coefficient programming.
 *
 *  \param handle       SWP core handle.
 *  \param chNum        Channel to which the memory should be used for.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param coeffPrms    Pointer to the coeff information.
 *                      This parameter should be non-NULL. The pointers to the
 *                      coeff overlays are returned in this structure.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpProgramScCoeff(Vcore_Handle handle,
                                     UInt32 chNum,
                                     Vcore_ScCoeffParams *coeffPrms)
{
    Int32                   retVal = VPS_SOK;
    Vcore_SwpHandleObj     *hObj;
    VpsHal_ScCoeffConfig    coeffCfg;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != coeffPrms));
    hObj = (Vcore_SwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }

    if (VPS_SOK == retVal)
    {
        if (NULL != coeffPrms->userCoeffPtr)
        {
            retVal = VpsHal_scCreateUserCoeffOvly(handle,
                         coeffPrms->userCoeffPtr,
                         &(coeffPrms->horzCoeffMemPtr),
                         &(coeffPrms->vertCoeffMemPtr),
                         &(coeffPrms->vertBilinearCoeffMemPtr));
        }
        else
        {
            coeffCfg.hScalingSet = coeffPrms->hScalingSet;
            coeffCfg.vScalingSet = coeffPrms->vScalingSet;
            retVal = VpsHal_scCreateCoeffOvly(
                         hObj->parent->scHandle,
                         &coeffCfg,
                         &(coeffPrms->horzCoeffMemPtr),
                         &(coeffPrms->vertCoeffMemPtr),
                         &(coeffPrms->vertBilinearCoeffMemPtr));
        }
    }

    return (retVal);
}

/**
 *  Vcore_swpGetScFactorConfig
 *  \brief Returns information about the scaling factor configuration for a
 *  given channel.
 *
 *  \param handle       Scalar Writeback Path Core handle.
 *  \param chNum        Channel for which the information is to be returned.
 *                      When opened in display mode, this should be always set
 *                      to 0. When opened in mem-mem mode, this represents the
 *                      channel to which this has to be applied. This
 *                      varies from 0 to the (number of channel - 1) provided
 *                      at the time of open. Even if the perChCfg flag
 *                      is FALSE at the time of open, data descriptor should be
 *                      programmed for all the channel for mem-mem operation.
 *  \param scFactorConfig Pointer to the scaling factor configuration structure
 *                      in which the information is to be returned.
 *                      This parameter should be non-NULL.
 *  \param descMem      Pointer to the overlay memory information.
 *
 *  \return             Returns 0 on success else returns error value.
 */
static Int32 Vcore_swpGetScFactorConfig(Vcore_Handle handle,
                                        UInt32 chNum,
                                        VpsHal_ScFactorConfig *scFactorConfig,
                                        const Vcore_DescMem *descMem)
{
    Int32 retVal = VPS_SOK;
    Vcore_SwpHandleObj *hObj;
    Vcore_SwpChObj *chObj;
    VpsHal_ScConfig *scHalCfg;

    /* Check for errors */
    GT_assert(SwpCoreTrace, (NULL != handle));
    GT_assert(SwpCoreTrace, (NULL != scFactorConfig));
    GT_assert(SwpCoreTrace, (NULL != descMem));
    GT_assert(SwpCoreTrace, (NULL != descMem->shadowOvlyMem));

    hObj = (Vcore_SwpHandleObj *) handle;

    /* Check if the channel is within the range allocated during open */
    if (chNum >= hObj->numCh)
    {
        retVal = VPS_EOUT_OF_RANGE;
        GT_0trace(SwpCoreTrace, GT_ERR, "Invalid channel number!!\n");
    }
    else
    {
        chObj = swpGetChObj(hObj, chNum);
        /* NULL pointer check */
        GT_assert(SwpCoreTrace, (NULL != chObj));

        /* Call the HAL function to get the scaling factor config. */
        scHalCfg = &chObj->scHalCfg;
        retVal = VpsHal_scGetScFactorConfig(hObj->parent->scHandle,
                    scHalCfg,
                    scFactorConfig,
                    descMem->shadowOvlyMem);
    }

    return (retVal);
}

/*------------------------------------------------------------------------------
 * Internal functions
 *------------------------------------------------------------------------------
 */
static UInt32 swpProgramDataDesc(const Vcore_SwpHandleObj *hObj,
                                 Vcore_SwpChObj *chObj,
                                 const Vcore_DescMem *descMem)
{
    Int32               retVal = VPS_EFAIL;
    UInt32              descCount = 0;
    VpsHal_VpdmaOutDescParams   *dataDescParam;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != hObj));
    GT_assert(SwpCoreTrace, (NULL != chObj));

    dataDescParam = &chObj->dataDescParam;

    dataDescParam->channel = hObj->parent->scCh;
    dataDescParam->nextChannel = hObj->parent->scCh;
    dataDescParam->dataType = VPSHAL_VPDMA_CHANDT_YC422 ;
    dataDescParam->lineStride =
        chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX];
    if (FVID2_SF_INTERLACED == chObj->coreFmt.fmt.scanFormat)
    {
        if (TRUE == chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX])
        {
            dataDescParam->lineSkip = VPSHAL_VPDMA_LS_2;
        }
    }
    else
    {
        dataDescParam->lineSkip = VPSHAL_VPDMA_LS_1;
    }
    dataDescParam->notify = FALSE;
    if (VCORE_OPMODE_CAPTURE == hObj->parent->curMode)
    {
        dataDescParam->priority = VPS_CFG_CAPT_WB_VPDMA_PRIORITY;
    }
    else
    {
        dataDescParam->priority = VPS_CFG_M2M_VPDMA_PRIORITY_WR;
    }
    dataDescParam->memType = (VpsHal_VpdmaMemoryType)
        chObj->coreFmt.memType;

    retVal = VpsHal_vpdmaCreateOutBoundDataDesc(
        descMem->outDataDesc[descCount],
            &dataDescParam[descCount]);
    if (VPS_SOK != retVal)
    {
        GT_0trace(SwpCoreTrace, GT_ERR,
            "VPDMA Descriptor Creation Failed!!\n");
    }
    return (retVal);
}

static UInt32 swpProgramReg(Vcore_SwpHandleObj *hObj,
                               Vcore_SwpChObj *chObj,
                              const Vcore_DescMem *descMem)
{
    Int32               retVal = VPS_EFAIL;
    VpsHal_ScConfig     *scHalCfg;

    scHalCfg = &chObj->scHalCfg;


    /* Configure the salar config structure */
    scHalCfg->srcWidth = chObj->params.srcWidth;
    scHalCfg->srcHeight = chObj->params.srcHeight;
    scHalCfg->cropStartX = chObj->params.srcCropCfg.cropStartX;
    scHalCfg->cropStartY = chObj->params.srcCropCfg.cropStartY;
    scHalCfg->cropWidth = chObj->params.srcCropCfg.cropWidth;
    scHalCfg->cropHeight = chObj->params.srcCropCfg.cropHeight;
    scHalCfg->outFrameMode = (Vps_ScanFormat)
        chObj->coreFmt.fmt.scanFormat;
    //scHalCfg->hsType = VPS_SC_HST_AUTO;
    scHalCfg->hsType = VPS_SC_HST_POLYPHASE;
    scHalCfg->hsType = chObj->params.scCfg.hsType;
    scHalCfg->nonLinear = chObj->params.scCfg.nonLinear;
    if (FALSE == scHalCfg->nonLinear)
    {
        scHalCfg->stripSize = 0;
    }
    else
    {
        scHalCfg->stripSize = chObj->params.scCfg.stripSize;
    }
    scHalCfg->vsType = chObj->params.scCfg.vsType;
    scHalCfg->defConfFactor = 0;
    scHalCfg->biLinIntpType = VPS_SC_BINTP_MODIFIED;
    scHalCfg->enableEdgeDetect =  chObj->params.scCfg.enableEdgeDetect;
    if (chObj->params.scCfg.hsType == VPS_SC_HST_AUTO)
        scHalCfg->hPolyBypass = TRUE;
    else
        scHalCfg->hPolyBypass = FALSE;

    scHalCfg->bypass = chObj->params.scCfg.bypass;

    /* Below three fields are not user programmable */
    scHalCfg->selfGenFid = VPS_SC_SELF_GEN_FID_DISABLE;
    scHalCfg->fidPol = VPS_FIDPOL_NORMAL;
    scHalCfg->arg = NULL;
    scHalCfg->phInfoMode    = VPS_SC_SET_PHASE_INFO_DEFAULT;

    /* Below fields are programmed according to the Input frame format */
    scHalCfg->inFrameMode = (Vps_ScanFormat)
        chObj->coreFmt.fmt.scanFormat;
    scHalCfg->tarWidth = chObj->coreFmt.fmt.width;
    scHalCfg->tarHeight = chObj->coreFmt.fmt.height;
    scHalCfg->enablePeaking = chObj->params.scCfg.enablePeaking;

    retVal = VpsHal_scCreateConfigOvly(hObj->parent->scHandle,
             descMem->shadowOvlyMem);
    if (VPS_SOK == retVal)
    {
        retVal = VpsHal_scSetConfig(hObj->parent->scHandle,
            scHalCfg,
            descMem->shadowOvlyMem);
    }

    return retVal;
}
static Void swpUpdateRtParams(Vcore_SwpChObj *chObj,
                             const Vcore_SwpRtParams *rtParams)
{
    Vcore_SwpParams     *swpParams;
    Vcore_Format        *coreFmt;
    VpsHal_ScConfig     *scHalCfg;
    UInt32 pitchCnt;
    Vps_FrameParams *sFrmPrms, *tFrmPrms;
    Vps_CropConfig *cropCfg;
    Vps_ScRtConfig     * scRtCfg;

    swpParams = &chObj->params;
    coreFmt =  &chObj->coreFmt;
    scHalCfg = &chObj->scHalCfg;

    sFrmPrms = rtParams->inFrmPrms;
    tFrmPrms = rtParams->outFrmPrms;
    cropCfg = rtParams->srcCropCfg;
    scRtCfg = rtParams->scRtCfg;
    if (NULL != tFrmPrms)
    {
        coreFmt->fmt.width = tFrmPrms->width;
        coreFmt->fmt.height = tFrmPrms->height;
        for (pitchCnt = 0; pitchCnt < FVID2_MAX_PLANES; pitchCnt++)
        {
            coreFmt->fmt.pitch[pitchCnt] = tFrmPrms->pitch[pitchCnt];
        }
    }
    if (NULL != sFrmPrms)
    {
        swpParams->srcWidth             = sFrmPrms->width;
        swpParams->srcHeight            = sFrmPrms->height;
    }
    if (NULL != cropCfg)
    {
        swpParams->srcCropCfg.cropWidth = cropCfg->cropWidth;
        swpParams->srcCropCfg.cropHeight = cropCfg->cropHeight;
        swpParams->srcCropCfg.cropStartX = cropCfg->cropStartX;
        swpParams->srcCropCfg.cropStartY = cropCfg->cropStartY;
    }
    else if (NULL != sFrmPrms)
    {
        swpParams->srcCropCfg.cropWidth = sFrmPrms->width;;
        swpParams->srcCropCfg.cropHeight = sFrmPrms->height;
        swpParams->srcCropCfg.cropStartX = 0;
        swpParams->srcCropCfg.cropStartY = 0;
    }
    if (NULL != scRtCfg)
    {
        swpParams->scCfg.bypass = scRtCfg->scBypass;
        /* update scalar phase info from application provided data*/
        if (TRUE == scRtCfg->scSetPhInfo)
        {
            scHalCfg->phInfoMode     =  VPS_SC_SET_PHASE_INFO_FROM_APP;
            scHalCfg->rowAccInc      =  scRtCfg->rowAccInc;
            scHalCfg->rowAccOffset   =  scRtCfg->rowAccOffset;
            scHalCfg->rowAccOffsetB  =  scRtCfg->rowAccOffsetB;
            scHalCfg->ravScFactor    =  scRtCfg->ravScFactor;
            scHalCfg->ravRowAccInit  =  scRtCfg->ravRowAccInit;
            scHalCfg->ravRowAccInitB =  scRtCfg->ravRowAccInitB;

            scHalCfg->linAccIncr     =  scRtCfg->linAccIncr;
            scHalCfg->colAccOffset   =  scRtCfg->colAccOffset;
        }
    }
    scHalCfg->tarWidth      =  chObj->coreFmt.fmt.width;
    scHalCfg->tarHeight     =  chObj->coreFmt.fmt.height;
    scHalCfg->cropWidth     =  chObj->params.srcCropCfg.cropWidth;
    scHalCfg->cropHeight    =  chObj->params.srcCropCfg.cropHeight;
    scHalCfg->cropStartX    =  chObj->params.srcCropCfg.cropStartX;
    scHalCfg->cropStartY    = chObj->params.srcCropCfg.cropStartY;
    scHalCfg->srcWidth      = chObj->params.srcWidth;
    scHalCfg->srcHeight     = chObj->params.srcHeight;
    scHalCfg->bypass        = chObj->params.scCfg.bypass;
    chObj->dataDescParam.lineStride =
            coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX];
}
static Int32 swpUpdateRtMem(Vcore_SwpHandleObj *hObj,
                                  Vcore_SwpChObj *chObj,
                                  const Vcore_DescMem *descMem)
{
    Int32 retVal = FVID2_SOK;

    retVal = VpsHal_scSetConfig(hObj->parent->scHandle,
                &chObj->scHalCfg,
                descMem->shadowOvlyMem);
    retVal += VpsHal_vpdmaCreateOutBoundDataDesc(
        descMem->outDataDesc[0],
            &chObj->dataDescParam);

    return retVal;

}

/**
 *  swpCheckParams
 *  Checks whether a given format is valid or not.
 *  Returns 0 if everything is fine else returns error value.
 */
static Int32 swpCheckFormat(const Vcore_Format *coreFmt)
{
    Int32               retVal = VPS_SOK;
   if (coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX] < (coreFmt->fmt.width * 2u))
    {
        GT_2trace(SwpCoreTrace, GT_ERR,
            "Pitch (%d) less than Width (%d) in bytes!!\n",
            coreFmt->fmt.pitch[FVID2_YUV_INT_ADDR_IDX],
            (coreFmt->fmt.width * 2u));
        retVal = VPS_EINVALID_PARAMS;
    }

    if (FVID2_DF_YUV422I_YUYV != coreFmt->fmt.dataFormat)
    {
        GT_1trace(SwpCoreTrace, GT_ERR,
            "Buffer format (%d) not supported!!\n",
            coreFmt->fmt.dataFormat);
        retVal = VPS_EINVALID_PARAMS;
    }

    /* Check whether window width is even */
    if (coreFmt->fmt.width & 0x01u)
    {
        GT_1trace(SwpCoreTrace, GT_ERR,
            "Width(%d) can't be odd!!\n", coreFmt->fmt.width);
        retVal = VPS_EINVALID_PARAMS;
    }

    return (retVal);
}

/**
 *  swpFreeHandleObj
 *  Frees-up the handle object and resets the variables.
 */
static Int32 swpFreeHandleObj(Vcore_SwpHandleObj *hObj)
{
    UInt32              handleCnt;
    UInt32              cookie;
    Int32               retVal = VPS_EFAIL;
    Vcore_SwpInstObj    *instObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != hObj));
    instObj = hObj->parent;
    GT_assert(SwpCoreTrace, (NULL != instObj));

    /* Disable global interrupts */
    cookie = Hwi_disable();

    /* Free the handle object */
    for (handleCnt = 0u; handleCnt < instObj->numHandle; handleCnt++)
    {
        if (hObj == &instObj->handleObjs[handleCnt])
        {
            /* Check if the handle is already allocated */
            GT_assert(SwpCoreTrace,
                (TRUE == instObj->handleObjs[handleCnt].used));

            /* Reset the variables */
            hObj->used = FALSE;
            hObj->parent = NULL;
            hObj->perChCfg = FALSE;
            hObj->numCh = 0u;
            swpSetDefaultChInfo(&hObj->handleContext, 0u);

            /* If last handle of the instance, then set mode to invalid */
            instObj->openCnt--;
            if (0 == instObj->openCnt)
            {
                instObj->curMode = VCORE_OPMODE_INVALID;
            }
            retVal = VPS_SOK;
            break;
        }
    }
    /* Restore global interrupts */
    Hwi_restore(cookie);
    return (retVal);
}

/**
 *  swpFreeChannelMem
 *  Frees-up the channel object memory.
 */
static Int32 swpFreeChannelMem(const Vcore_SwpChObj *chObj)
{
    UInt32              channelCnt;
    Int32               retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != chObj));

    Semaphore_pend(SwpPoolSem, VCORE_SWP_SEM_TIMEOUT);

    for (channelCnt = 0; channelCnt < VCORE_SWP_MAX_CHANNEL_OBJECTS;
         channelCnt++)
    {
        if (&SwpChannelMemPool[channelCnt] == chObj)
        {
            /* Check if the memory is already allocated */
            GT_assert(SwpCoreTrace, (TRUE ==
                        SwpChannelMemPool[channelCnt].isAllocated));
            SwpChannelMemPool[channelCnt].isAllocated = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }

    Semaphore_post(SwpPoolSem);

    return (retVal);
}

/**
 *  swpFreeChannelObj
 *  Frees-up the channel objects.
 */
static Int32 swpFreeChannelObj(Vcore_SwpChObj **chObjs,
                              UInt32 numCh)
{
    UInt32              channelCnt;
    Int32               retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != chObjs));

    for (channelCnt = 0; channelCnt < numCh; channelCnt++)
    {
        /* Memset the channel object before freeing */
        retVal += swpFreeChannelMem(chObjs[channelCnt]);
        if (VPS_SOK == retVal)
        {
            VpsUtils_memset(chObjs[channelCnt],
            0,
            sizeof(Vcore_SwpChObj));
        }
        chObjs[channelCnt] = NULL;
    }

    return (retVal);
}

/**
 *  swpAllocChannelObj
 *  Allocates channel objects.
 *  Returns error if allocation failed.
 */
static Int32 swpAllocChannelObj(Vcore_SwpChObj **chObjs,
                               UInt32 numCh)
{
    UInt32      channelCnt, errCnt;
    Int32       retVal = VPS_SOK;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != chObjs));

    for (channelCnt = 0u; channelCnt < numCh; channelCnt++)
    {
        /* Allocate channel objects one at a time */
        chObjs[channelCnt] = swpAllocChannelMem();
        if (NULL == chObjs[channelCnt])
        {
            GT_1trace(SwpCoreTrace, GT_ERR,
                "Channel Object memory alloc failed for channel %d!!\n",
                channelCnt);
            retVal = VPS_EALLOC;
            break;
        }

        /* Set default values for the channel object */
        swpSetDefaultChInfo(chObjs[channelCnt], channelCnt);
    }

    /* If error, free-up the already allocated objects */
    if (0 != retVal)
    {
        for (errCnt = 0u; errCnt < channelCnt; errCnt++)
        {
            swpFreeChannelMem(chObjs[errCnt]);
            chObjs[errCnt] = NULL;
        }
    }

    return (retVal);
}

/**
 *  swpSetDefaultChInfo
 *  Resets the channel information with default values.
 */
static void swpSetDefaultChInfo(Vcore_SwpChObj *chObj,
                                UInt32 channelCnt)
{
    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != chObj));

    /* Set proper states */
    chObj->state.isFormatSet = FALSE;
    chObj->state.isMultiWinMode = FALSE;

    /* Initialize format structure - Assumes YUV422 720x480 progressive */
    chObj->coreFmt.memType = VPS_VPDMA_MT_NONTILEDMEM;

    chObj->coreFmt.fmt.channelNum = channelCnt;
    chObj->coreFmt.fmt.width = VCORE_SWP_DEFAULT_WIDTH;
    chObj->coreFmt.fmt.height = VCORE_SWP_DEFAULT_HEIGHT;
    chObj->coreFmt.fmt.pitch[FVID2_YUV_INT_ADDR_IDX] =
        VCORE_SWP_DEFAULT_WIDTH * 2u;
    chObj->coreFmt.fmt.fieldMerged[FVID2_YUV_INT_ADDR_IDX] = FALSE;
    chObj->coreFmt.fmt.dataFormat = FVID2_DF_YUV422I_YUYV;
    chObj->coreFmt.fmt.scanFormat = FVID2_SF_PROGRESSIVE;
    chObj->coreFmt.fmt.bpp = FVID2_BPP_BITS16;
    chObj->coreFmt.fmt.reserved = NULL;

    /* Initialize channel info structure */
    chObj->descInfo.numInDataDesc = 0;
    chObj->descInfo.numOutDataDesc = 0;
    chObj->descInfo.shadowOvlySize = 0;
    chObj->descInfo.nonShadowOvlySize = 0;
    chObj->descInfo.horzCoeffOvlySize = 0;
    chObj->descInfo.vertCoeffOvlySize = 0;
    chObj->descInfo.vertBilinearCoeffOvlySize = 0;

    return;
}

/**
 *  swpAllocChannelMem
 *  Allocate memory for channel object from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_SwpChObj *swpAllocChannelMem(void)
{
    UInt32              channelCnt;
    Vcore_SwpChObj *chObj = NULL;

    Semaphore_pend(SwpPoolSem, VCORE_SWP_SEM_TIMEOUT);

    for (channelCnt = 0u; channelCnt < VCORE_SWP_MAX_CHANNEL_OBJECTS;
         channelCnt++)
    {
        if (FALSE == SwpChannelMemPool[channelCnt].isAllocated)
        {
            chObj = &SwpChannelMemPool[channelCnt];
            SwpChannelMemPool[channelCnt].isAllocated = TRUE;
            break;
        }
    }

    if (NULL == chObj)
    {
        GT_0trace(SwpCoreTrace, GT_ERR, "Channel Object memory alloc failed!!\n");
    }

    Semaphore_post(SwpPoolSem);

    return (chObj);
}

/**
 *  swpAllocHandleObj
 *  Allocates handle object from the available pool of handle objects.
 *  Returns error if allocation failed or if invalid mode is requested.
 */
static Vcore_SwpHandleObj *swpAllocHandleObj(UInt32 instId,
                                        Vcore_OpMode mode)
{
    UInt32              instCnt, handleCnt;
    UInt32              cookie;
    Vcore_SwpInstObj    *instObj = NULL;
    Vcore_SwpHandleObj      *hObj = NULL;

    /* Find out the instance to which this handle belongs to. */
    for (instCnt = 0u; instCnt < SwpProperty.numInstance; instCnt++)
    {
        if (SwpInstObjects[instCnt].instId == instId)
        {
            instObj = &SwpInstObjects[instCnt];
            break;
        }
    }

    /* Disable global interrupts */
    cookie = Hwi_disable();

    if (NULL != instObj)
    {
        /* Check if the required mode is proper */
        if (VCORE_OPMODE_INVALID > instObj->curMode)
        {
            /* Core already opend for some mode. For display mode, only one
               handle should be allowed. It doesn't make sense isn't it? */
            if ((VCORE_OPMODE_DISPLAY == instObj->curMode)
                || ((VCORE_OPMODE_MEMORY == instObj->curMode)
                    && (VCORE_OPMODE_MEMORY != mode)))
            {
                /* Already opened mode and requested mode are not matching */
                GT_0trace(SwpCoreTrace, GT_ERR,
                    "Already opened mode and requested mode not matching!!\n");
                instObj = NULL;
            }
        }
    }

    if (NULL != instObj)
    {
        /* Use a free handle object */
        for (handleCnt = 0u; handleCnt < instObj->numHandle; handleCnt++)
        {
            if (FALSE == instObj->handleObjs[handleCnt].used)
            {
                hObj = &instObj->handleObjs[handleCnt];
                break;
            }
        }
    }

    if (NULL != hObj)
    {
        /* Update the state and return the handle handle */
        hObj->used = TRUE;
        hObj->parent = instObj;
        instObj->curMode = mode;
        instObj->openCnt++;
    }

    /* Restore global interrupts */
    Hwi_restore(cookie);

    return (hObj);
}

/**
 *  swpFreeHandleMem
 *  Frees-up the handle objects memory.
 */
static Int32 swpFreeHandleMem(const Vcore_SwpHandleObj *handleObjs)
{
    UInt32              instCnt;
    Int32               retVal = VPS_EFAIL;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != handleObjs));

    Semaphore_pend(SwpPoolSem, VCORE_SWP_SEM_TIMEOUT);

    for (instCnt = 0; instCnt < VCORE_SWP_NUM_INST; instCnt++)
    {
        if (&SwpHandleMemPool[instCnt][0] == handleObjs)
        {
            /* Check if the memory is already allocated */
            GT_assert(SwpCoreTrace, (TRUE == SwpHandleMemFlag[instCnt]));
            SwpHandleMemFlag[instCnt] = FALSE;
            retVal = VPS_SOK;
            break;
        }
    }
    Semaphore_post(SwpPoolSem);
    return (retVal);
}

/**
 *  swpAllocHandleMem
 *  Allocate memory for handle objects from static memory pool.
 *  Returns NULL if memory pool is full.
 */
static Vcore_SwpHandleObj *swpAllocHandleMem(UInt32 numHandle)
{
    UInt32              instCnt;
    Vcore_SwpHandleObj      *handleObjs = NULL;

    /* Check if the requested number of handles is within range */
    GT_assert(SwpCoreTrace, (numHandle <= VCORE_SWP_MAX_HANDLES));

    Semaphore_pend(SwpPoolSem, VCORE_SWP_SEM_TIMEOUT);

    for (instCnt = 0u; instCnt < VCORE_SWP_NUM_INST; instCnt++)
    {
        if (FALSE == SwpHandleMemFlag[instCnt])
        {
            handleObjs = &SwpHandleMemPool[instCnt][0];
            SwpHandleMemFlag[instCnt] = TRUE;
            break;
        }
    }

    if (NULL == handleObjs)
    {
        GT_0trace(SwpCoreTrace, GT_ERR,
            "Handle Object memory alloc failed!!\n");
    }

    Semaphore_post(SwpPoolSem);

    return (handleObjs);
}

/**
 *  swpGetChObj
 *  Returns the channel object pointer depending on per channel config is
 *  enabled or not.
 */
static Vcore_SwpChObj *swpGetChObj(Vcore_SwpHandleObj *hObj, UInt32 chNum)
{
    Vcore_SwpChObj  *chObj;

    /* NULL pointer check */
    GT_assert(SwpCoreTrace, (NULL != hObj));

    /* Check whether the formatting is common for all channel or
     * different for each of the channel for a given handle */
    if (TRUE == hObj->perChCfg)
    {
        chObj = hObj->chObjs[chNum];
    }
    else
    {
        /* Same format - so use handle context. Use each channel
         * objects to just store the data descriptor information */
        chObj = &hObj->handleContext;
    }

    return (chObj);
}
